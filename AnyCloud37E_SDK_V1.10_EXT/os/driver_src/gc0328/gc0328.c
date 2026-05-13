/*
 * gc0328 Camera Driver
 *
 * Copyright anyka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of_gpio.h>
#include <linux/v4l2-mediabus.h>
#include <linux/videodev2.h>

#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-image-sizes.h>
#include "../include/ak_video_priv_cmd.h"
#include "../include_internal/ak_video_priv_cmd_internal.h"
#include "../include/ak_isp_drv.h"

#include "../include/ak_sensor.h"
#include "../include/sensor_cmd.h"
#include "../common/sensor_sys.h"
#include "../common/sensor_i2c.h"

#include "gc0328.h"

/*
 *
SENSOR_PWDN_LEVEL:	the io level when sensor power down
SENSOR_RESET_LEVEL:	the io level when sensor reset
SENSOR_I2C_ADDR:	the sensor slave address, 7 bits width.
					the i2c bus left shift 1bits.
SENSOR_ID:	the sensor ID.
SENSOR_MCLK:	mclk, unit：MHz。24MHz or 27MHz mclk for most sensors.
				NOTE: 27MHz mclk is inaccurate.
SENSOR_REGADDR_BYTE:	sensor register address bytes width, 1 byte or 2 bytes.
SENSOR_DATA_BYTE:		register's value bytes width, 1 byte or 2bytes.
SENSOR_OUTPUT_WIDTH:	use for isp cut window MAX width.
SENSOR_OUTPUT_HEIGHT:	use for isp cut window MAX height.
SENSOR_VALID_OFFSET_X:	use for isp cut window left offset.
SENSOR_VALID_OFFSET_Y:	use for isp cut window top offset.
SENSOR_BUS_TYPE:		sensor output data type.
SENSOR_IO_INTERFACE:	sensor output data bus type.
SENSOR_IO_LEVEL:		sensor output data io level.
MAX_FPS:				current sensor configurtion support max fps.
EXP_EFFECT_FRAMES:		!!IMPORT!! frequency for update exp_time
		0 - adjust exp_time for every frames
		1 - adjust exp_time for 1/2 frames
		2 - adjust exp_time for 1/3 frames
		and so on.
A_GAIN_EFFECT_FRAMES:	!!IMPORT!! frequency for update a_gain
		0 - adjust a_gain for every frames
		1 - adjust a_gain for 1/2 frames
		2 - adjust a_gain for 1/3 frames
		and so on.
D_GAIN_EFFECT_FRAMES:	!!IMPORT!! frequency for update d_gain
		0 - adjust d_gain for every frames
		1 - adjust d_gain for 1/2 frames
		2 - adjust d_gain for 1/3 frames
		and so on.
DELAY_FLAG
 *
 * */
#define SENSOR_PWDN_LEVEL		1
#define SENSOR_RESET_LEVEL		0
#define SENSOR_I2C_ADDR			0x21 
#define SENSOR_ID				0x9d
#define SENSOR_MCLK				24
#define SENSOR_REGADDR_BYTE		1
#define SENSOR_DATA_BYTE		1
#define SENSOR_OUTPUT_WIDTH		640
#define SENSOR_OUTPUT_HEIGHT	480
#define SENSOR_VALID_OFFSET_X	0
#define SENSOR_VALID_OFFSET_Y	0
#define SENSOR_BUS_TYPE			BUS_TYPE_YUV
#define SENSOR_IO_INTERFACE		DVP_INTERFACE
#define SENSOR_IO_LEVEL			IO_LEVEL_3V3
#define MAX_FPS					25

#define EXP_EFFECT_FRAMES		1	/*adjust exp_time for 1/2 frames*/
#define A_GAIN_EFFECT_FRAMES	0	/*adjust a_gain for every frames*/
#define D_GAIN_EFFECT_FRAMES	1	/*adjust d_gain for 1/2 frames*/

struct gc0328_win_size {
	char				*name;
	u32				width;
	u32				height;
};

/*
 * store sensor fps informations
 * @current_fps: current fps
 * @to_fps: should changed to the fps
 * @to_fps_value: the fps paramter
 */
struct sensor_fps_info {
	int current_fps;
	int to_fps;
	int to_fps_value;
};

static const struct v4l2_ctrl_ops gc0328_ctrl_ops;
static const struct v4l2_ctrl_config config_sensor_get_id = {
	.ops	= &gc0328_ctrl_ops,
	.id		= SENSOR_GET_ID,
	.name	= "get sensor id",
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.min	= 0,
	.max	= 0xfffffff,
	.step	= 1,
	.def	= 0,
	.flags	= V4L2_CTRL_FLAG_VOLATILE,/*set V4L2_CTRL_FLAG_VOLATILE avoid get cached*/
};

struct gc0328_aec_parms {
};

/*
 * the sensor struct
 * @client: point to i2c client
 * @subdev: v4l2 subdev struct
 * @hdl: 	v4l2 control handler
 * @cfmt_code:
 * @gpio_reset:	gpio number for reset pin
 * @gpio_pwdn:	gpio number for power down pin
 * @cb_info:	sensor callbacks
 * @fps_info:	fps informations
 * @hcb:		host callbacks
 * @aec_parms:	aec paramters
 * @method:		scanning method
 * @preg_info:	sensor registers
 * @num:		number of sensor registers
 * @fps_num:	fps & resolution
 */
struct gc0328_priv {
	struct i2c_client *client;
	struct v4l2_subdev		subdev;
	struct v4l2_ctrl_handler	hdl;
	u32	cfmt_code;
	const struct gc0328_win_size *win;

	int gpio_reset;
	int gpio_pwdn;
	struct sensor_cb_info cb_info;
	struct sensor_fps_info fps_info;

	struct gc0328_aec_parms aec_parms;

	enum scan_method method;

	AK_ISP_SENSOR_REG_INFO *preg_info;
	int num;
	int fps_res;

	struct v4l2_ctrl *ctrl_sensor_get_id;
};

static AK_ISP_SENSOR_CB gc0328_cb;

/*
 * check_id - check hardware sensor ID whether it meets this driver
 * 0-no check, force to meet
 * others-check, if not meet return fail
 */
static int check_id = 0;
module_param(check_id, int, 0644);

static void gc0328_set_fps_async(struct gc0328_priv *priv);
static int gc0328_sensor_probe_id_func(void *arg);
static int __gc0328_sensor_probe_id_func(struct i2c_client *client);  //use IIC bus
static int gc0328_sensor_read_id_func(void *arg); //no use IIC bus
static int gc0328_sensor_get_resolution_func(void *arg, int *width, int *height);
static int call_sensor_sys_init(void);
static int call_sensor_sys_deinit(void);

static u32 gc0328_codes[] = {
	MEDIA_BUS_FMT_YUYV8_2X8,
	MEDIA_BUS_FMT_UYVY8_2X8,
	MEDIA_BUS_FMT_RGB565_2X8_BE,
	MEDIA_BUS_FMT_RGB565_2X8_LE,
};

/*
 * General functions
 */
static struct gc0328_priv *to_gc0328(const struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct gc0328_priv,
			    subdev);
}

static struct v4l2_subdev *ctrl_to_sd(struct v4l2_ctrl *ctrl)
{
	return &container_of(ctrl->handler, struct gc0328_priv, hdl)->subdev;
}

/*
 * XXXX_s_stream -
 * set steaming enable/disable
 * soc_camera_ops functions
 *
 * @sd:				subdev
 * @enable:			enable flags
 */
static int gc0328_s_stream(struct v4l2_subdev *sd, int enable)
{
	return 0;
}

static int gc0328_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	int ret = 0;

	switch (ctrl->id) {
		case SENSOR_GET_ID:
			ctrl->val = SENSOR_ID;
			break;

		default:
			ret = -EINVAL;
			pr_err("%s id:0x%x no support\n", __func__, ctrl->id);
			break;
	}

	return ret;
}

/*
 * XXXX_s_stream -
 * set ctrls
 * soc_camera_ops functions
 *
 * @ctrl:			pointer to ctrl
 */
static int gc0328_s_ctrl(struct v4l2_ctrl *ctrl)
{
	return 0;
}

/*
 * XXXX_s_power -
 * set power operation
 * soc_camera_ops functions
 *
 * @sd:			subdev
 * @on:			power flags
 */
static int gc0328_core_s_power(struct v4l2_subdev *sd, int on)
{
	return 0;
}

/*
 * XXXX_get_sensor_id -
 * get sensor ID
 * private callback functions
 *
 * @ctrl:			pointer to ctrl
 */
static int gc0328_get_sensor_id(struct v4l2_control *ctrl)
{
	ctrl->value = gc0328_sensor_read_id_func(NULL);   //no use IIC bus
	return 0;
}

/*
 * XXXX_get_sensor_id -
 * get sensor callback
 * private callback functions
 *
 * @sd:				subdev
 * @ctrl:			pointer to ctrl
 */
static int gc0328_get_sensor_cb(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct gc0328_priv *priv = to_gc0328(client);

	ctrl->value = (int)&priv->cb_info;
	return 0;
}

/*
 * XXXX_core_g_ctrl -
 * get control
 * core functions
 *
 * @sd:				subdev
 * @ctrl:			pointer to ctrl
 */
static int gc0328_core_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	int ret;

	switch (ctrl->id) {
		case GET_SENSOR_ID:
			ret = gc0328_get_sensor_id(ctrl);
			break;

		case GET_SENSOR_CB:
			ret = gc0328_get_sensor_cb(sd, ctrl);
			break;

		default:
			pr_err("%s cmd:%d not support\n", __func__, ctrl->id);
			ret = -1;
			break;
	}

	return ret;
}

/*
 * XXXX_core_s_ctrl -
 * set core s_ctrl
 * core functions
 *
 * @sd:				subdev
 * @ctrl:			pointer to ctrl
 */
static int gc0328_core_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	int ret;

	switch (ctrl->id) {
		case SET_FPS_DIRECT:
			//ret = gc0328_set_fps_direct(sd, ctrl);
			break;

		default:
			pr_err("%s cmd:0x%x not support\n", __func__, ctrl->id);
			ret = -1;
			break;
	}

	return ret;
}

static long gc0328_core_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct gc0328_priv *priv = to_gc0328(client);
	int ret = 0;

	pr_debug("%s\n", __func__);

	switch (cmd) {
		case AK_SENSOR_SET_INIT:
			/*gc0328 regs in driver, so returen success*/
			break;

		default:
			ret = -EINVAL;
			break;
	}

	return ret;
}

/*
 * XXXX_get_fmt -
 * get format
 * pad functions
 *
 * @sd:				subdev
 * @cfg:			pointer to pad config
 * @format:			return format
 */
static int gc0328_get_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client  *client = v4l2_get_subdevdata(sd);
	struct gc0328_priv *priv = to_gc0328(client);

	static struct gc0328_win_size win = {
		.name = "default",
		.width = SENSOR_OUTPUT_WIDTH,
		.height = SENSOR_OUTPUT_HEIGHT,
	};

	if (format->pad)
		return -EINVAL;

	if (!priv->win) {
		priv->win = &win;
		priv->cfmt_code = MEDIA_BUS_FMT_UYVY8_2X8;
	}

	mf->width	= priv->win->width;
	mf->height	= priv->win->height;
	mf->code	= priv->cfmt_code;

	switch (mf->code) {
	case MEDIA_BUS_FMT_RGB565_2X8_BE:
	case MEDIA_BUS_FMT_RGB565_2X8_LE:
		mf->colorspace = V4L2_COLORSPACE_SRGB;
		break;
	default:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_UYVY8_2X8:
		mf->colorspace = V4L2_COLORSPACE_JPEG;
	}
	mf->field	= V4L2_FIELD_NONE;

	return 0;
}

/*
 * XXXX_set_fmt -
 * set format
 * pad functions
 *
 * @sd:				subdev
 * @cfg:			pointer to pad config
 * @format:			format to set
 */
static int gc0328_set_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;

	if (format->pad)
		return -EINVAL;

	mf->field	= V4L2_FIELD_NONE;

	switch (mf->code) {
	case MEDIA_BUS_FMT_RGB565_2X8_BE:
	case MEDIA_BUS_FMT_RGB565_2X8_LE:
		mf->colorspace = V4L2_COLORSPACE_SRGB;
		break;
	default:
		mf->code = MEDIA_BUS_FMT_UYVY8_2X8;
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_UYVY8_2X8:
		mf->colorspace = V4L2_COLORSPACE_JPEG;
	}

	if (cfg)
		cfg->try_fmt = *mf;
	return 0;
}

/*
 * XXXX_set_fmt -
 * set format
 * pad functions
 *
 * @sd:				subdev
 * @cfg:			pointer to pad config
 * @sel:			return selection
 */
static int gc0328_get_selection(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_selection *sel)
{
	sel->r.left = SENSOR_VALID_OFFSET_X;
	sel->r.top = SENSOR_VALID_OFFSET_Y;
	sel->r.width = SENSOR_OUTPUT_WIDTH;
	sel->r.height = SENSOR_OUTPUT_HEIGHT;

	return 0;
}

/*
 * XXXX_enum_bus -
 * enum bus
 * pad functions
 *
 * @sd:				subdev
 * @cfg:			pointer to pad config
 * @code:			return code of bus type
 */
static int gc0328_enum_mbus_code(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_mbus_code_enum *code)
{
	if (code->pad || code->index >= ARRAY_SIZE(gc0328_codes))
		return -EINVAL;

	code->code = MEDIA_BUS_FMT_YUYV8_2X8;

	return 0;
}

/*
 * XXXX_g_crop -
 * get crop
 * video functions
 *
 * @sd:				subdev
 * @a:				return crop
 */
static int gc0328_g_crop(struct v4l2_subdev *sd, struct v4l2_crop *a)
{
	/*return sensor cut window size&offset*/
	a->c.left	= SENSOR_VALID_OFFSET_X;
	a->c.top	= SENSOR_VALID_OFFSET_Y;
	a->c.width	= SENSOR_OUTPUT_WIDTH;
	a->c.height	= SENSOR_OUTPUT_HEIGHT;
	a->type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;

	return 0;
}

/*
 * XXXX_cropcap -
 * get crop capbilities
 * video functions
 *
 * @sd:				subdev
 * @a:				return crop capbilities
 */
static int gc0328_cropcap(struct v4l2_subdev *sd, struct v4l2_cropcap *a)
{
	gc0328_sensor_get_resolution_func(NULL, &a->bounds.width, &a->bounds.height);
	a->bounds.left			= SENSOR_VALID_OFFSET_X;
	a->bounds.top			= SENSOR_VALID_OFFSET_Y;
	a->defrect				= a->bounds;
	a->type					= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	a->pixelaspect.numerator	= 1;
	a->pixelaspect.denominator	= 1;

	pr_err("%s bounds.width:%d, bounds.height:%d\n", __func__, a->bounds.width, a->bounds.height);

	return 0;
}

/*
 * XXXX_video_probe -
 * do some process in subdev probe
 * locale functions
 *
 * @sd:				subdev
 * @a:				return crop capbilities
 */
static int gc0328_video_probe(struct i2c_client *client)
{
	struct gc0328_priv *priv = to_gc0328(client);
	int ret;

	ret = v4l2_ctrl_handler_setup(&priv->hdl);

	return ret;
}

/*ctrl ops*/
static const struct v4l2_ctrl_ops gc0328_ctrl_ops = {
	.g_volatile_ctrl	= gc0328_g_volatile_ctrl,
	.s_ctrl				= gc0328_s_ctrl,
};

/*core ops*/
static struct v4l2_subdev_core_ops gc0328_subdev_core_ops = {
	.s_power	= gc0328_core_s_power,
	.g_ctrl		= gc0328_core_g_ctrl,
	.s_ctrl		= gc0328_core_s_ctrl,
	.ioctl		= gc0328_core_ioctl,
};

/*
 * XXXX_g_mbus_config -
 * get buf config
 * video functions
 *
 * @sd:				subdev
 * @cfg:			return config
 */
static int gc0328_g_mbus_config(struct v4l2_subdev *sd,
				struct v4l2_mbus_config *cfg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	cfg->flags = V4L2_MBUS_PCLK_SAMPLE_RISING | V4L2_MBUS_MASTER |
		V4L2_MBUS_VSYNC_ACTIVE_HIGH | V4L2_MBUS_HSYNC_ACTIVE_HIGH |
		V4L2_MBUS_DATA_ACTIVE_HIGH;
	cfg->type = V4L2_MBUS_PARALLEL;
	cfg->flags = 0;

	return 0;
}

/*
 * XXXX_s_crop -
 * set crop
 * video functions
 *
 * @sd:				subdev
 * @crop:			crop to set
 */
static int gc0328_s_crop(struct v4l2_subdev *sd, const struct v4l2_crop *crop)
{
	printk(KERN_INFO "%s %d, left:%d, top:%d, width:%d, height:%d\n",
			__func__, __LINE__, crop->c.left, crop->c.top, crop->c.width, crop->c.height);
	return 0;
}

/*video ops*/
static struct v4l2_subdev_video_ops gc0328_subdev_video_ops = {
	.s_stream	= gc0328_s_stream,
	.cropcap	= gc0328_cropcap,
	.g_crop		= gc0328_g_crop,
	.g_mbus_config	= gc0328_g_mbus_config,
	.s_crop		= gc0328_s_crop,
};

/*pad ops*/
static const struct v4l2_subdev_pad_ops gc0328_subdev_pad_ops = {
	.enum_mbus_code = gc0328_enum_mbus_code,
	.get_fmt	= gc0328_get_fmt,
	.set_fmt	= gc0328_set_fmt,
	.get_selection	= gc0328_get_selection,
};

/*sensor driver subdev ops*/
static struct v4l2_subdev_ops gc0328_subdev_ops = {
	.core	= &gc0328_subdev_core_ops,
	.video	= &gc0328_subdev_video_ops,
	.pad	= &gc0328_subdev_pad_ops,
};

/*
 * sensor_of_parse -
 * parse node of device
 *
 * @client:			pointor to i2c client
 * @priv:			sensor struct
 */
static int sensor_of_parse(struct i2c_client *client, struct gc0328_priv *priv)
{
	struct device_node *np = client->dev.of_node;

	/*it is exist reset but lack of pwdn gpio in most case*/
	priv->gpio_pwdn = of_get_named_gpio(np, "pwdn-gpio", 0);
	if (priv->gpio_pwdn >= 0) {
		devm_gpio_request(&client->dev, priv->gpio_pwdn, "sensor-pwdn");
	}
	
	/* gc0328 do not have reset pin */
	priv->gpio_reset = of_get_named_gpio(np, "reset-gpio", 0);
	if (priv->gpio_reset >= 0) {
		devm_gpio_request(&client->dev, priv->gpio_reset, "sensor-reset");
	}
	return 0;
}

/*
 * XXXX_match - check sensor id for match driver
 * @priv:			pointer to pirvate structure
 *
 * @RETURN:	0-match fail; 1-match success
 */
static int gc0328_match(struct gc0328_priv *priv)
{
	int pid;

	if (!check_id)
		return 1;

	priv->cb_info.cb->sensor_set_power_on_func(priv);
	pid = priv->cb_info.cb->sensor_probe_id_func(priv);
	if (pid <= 0) {
		pr_err("%s fail\n", __func__);
		return 0;
	}

	return 1;
}

/*
 * XXXX_probe -
 * driver probe after platform probe ok
 * i2c_driver functions
 *
 * @client:			pointor to i2c client
 * @did:			driver ids
 */
static int gc0328_probe(struct i2c_client *client,
			const struct i2c_device_id *did)
{
	struct gc0328_priv	*priv;
	struct i2c_adapter	*adapter = to_i2c_adapter(client->dev.parent);
	int			ret;

	pr_info("%s %s %s\n", __func__, __DATE__, __TIME__);

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&adapter->dev,
			"gc0328: I2C-Adapter doesn't support SMBUS\n");
		return -EIO;
	}

	priv = devm_kzalloc(&client->dev, sizeof(struct gc0328_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&adapter->dev,
			"Failed to allocate memory for private data!\n");
		return -ENOMEM;
	}

	/*parse of node*/
	sensor_of_parse(client, priv);

	/*
	 * the current sensor slave address to client.
	 * the address from dts may incorrect
	 * */
	client->addr = SENSOR_I2C_ADDR;
	priv->cb_info.cb = &gc0328_cb;
	priv->cb_info.arg = priv;
	priv->client = client;

	if (!client->dev.of_node) {
		dev_err(&client->dev, "Missing platform_data for driver\n");
		ret = -EINVAL;
		goto err_clk;
	}

	if (!gc0328_match(priv)) {
		ret = -ENODEV;
		goto err_clk;
	}

	/*subdev init*/
	v4l2_i2c_subdev_init(&priv->subdev, client, &gc0328_subdev_ops);
	priv->subdev.flags |= /*V4L2_SUBDEV_FL_HAS_EVENTS | */V4L2_SUBDEV_FL_HAS_DEVNODE;
	v4l2_ctrl_handler_init(&priv->hdl, 2);
	v4l2_ctrl_new_std(&priv->hdl, &gc0328_ctrl_ops,
			V4L2_CID_VFLIP, 0, 1, 1, 0);
	v4l2_ctrl_new_std(&priv->hdl, &gc0328_ctrl_ops,
			V4L2_CID_HFLIP, 0, 1, 1, 0);

	/*private ctrl*/
	priv->ctrl_sensor_get_id = v4l2_ctrl_new_custom(&priv->hdl, &config_sensor_get_id, NULL);
	if (!priv->ctrl_sensor_get_id)
		pr_err("creat ctrl_sensor_get_id fail\n");

	priv->subdev.ctrl_handler = &priv->hdl;
	if (priv->hdl.error) {
		dev_err(&client->dev, "Hdl error\n");
		ret = priv->hdl.error;
		goto err_clk;
	}

	ret = gc0328_video_probe(client);
	if (ret < 0) {
		dev_err(&client->dev, "gc0328 probe fail\n");
		goto err_videoprobe;
	}

	/*register async subdev*/
	ret = v4l2_async_register_subdev(&priv->subdev);
	if (ret < 0) {
		dev_err(&client->dev, "v4l2 async register subdev fail, ret:%d\n", ret);
		goto err_videoprobe;
	}

	dev_notice(&adapter->dev, "gc0328 Probed success, subdev:%p\n", &priv->subdev);

	/*ak platform need export some informations from sensor*/
	call_sensor_sys_init();

	return 0;

err_videoprobe:
	v4l2_ctrl_handler_free(&priv->hdl);
err_clk:
	return ret;
}

/*
 * XXXX_remove -
 * driver probe after platform remove ok
 * i2c_driver functions
 *
 * @client:			pointor to i2c client
 */
static int gc0328_remove(struct i2c_client *client)
{
	struct gc0328_priv       *priv = to_gc0328(client);

	pr_err("%s %d\n", __func__, __LINE__);

	if (!priv) {
		pr_err("%s had remove\n", __func__);
		return 0;
	}

	/*unregister async subdev*/
	v4l2_async_unregister_subdev(&priv->subdev);
	/*unregister subdev*/
	v4l2_device_unregister_subdev(&priv->subdev);
	/*unregister handler*/
	v4l2_ctrl_handler_free(&priv->hdl);
	/*unregister sys node*/
	call_sensor_sys_deinit();
	return 0;
}

static const struct i2c_device_id gc0328_id[] = {
	{ "gc0328", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, gc0328_id);

static const struct of_device_id gc0328_of_match[] = {
	/*donot changed compatible, must the same as dts*/
	{.compatible = "anyka,sensor0", },
	{},
};
MODULE_DEVICE_TABLE(of, gc0328_of_match);

static struct i2c_driver gc0328_i2c_driver = {
	.driver = {
		.name = "gc0328",
		.of_match_table = of_match_ptr(gc0328_of_match),
	},
	.probe    = gc0328_probe,
	.remove   = gc0328_remove,
	.id_table = gc0328_id,
};

module_i2c_driver(gc0328_i2c_driver);

MODULE_DESCRIPTION("SoC Camera driver for gc0328 sensor");
MODULE_AUTHOR("Anyka Microelectronic");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0.00");

/*sensor i2c write*/
static int gc0328_write(const struct i2c_client *client, int reg, int value)
{
	struct i2c_transfer_struct trans = {
		.client = client,
		.reg_bytes = SENSOR_REGADDR_BYTE,
		.value_bytes = SENSOR_DATA_BYTE,
		.reg = reg,
		.value = value,
	};

	return i2c_write(&trans);
}

/*sensor i2c read*/
static int gc0328_read(const struct i2c_client *client, int reg)
{
	struct i2c_transfer_struct trans = {
		.client = client,
		.reg_bytes = SENSOR_REGADDR_BYTE,
		.value_bytes = SENSOR_DATA_BYTE,
		.reg = reg,
		.value = 0,
	};

	return i2c_read(&trans);
}

static void gc0328_setup(struct i2c_client *client, const unsigned char tab_parameter[])
{
	int i = 0;

	while (1) {
		if ((END_FLAG == tab_parameter[i]) && (END_FLAG == tab_parameter[i + 1])) {
			break;
		} else if (DELAY_FLAG == tab_parameter[i]) {
			msleep(tab_parameter[i + 1]);
		} else {
			gc0328_write(client, tab_parameter[i], tab_parameter[i + 1]);
		}

		i += 2;
	}
}

/*
 * The sensor must set callbacks
 *
 * */
static int gc0328_sensor_init_func(void *arg, const AK_ISP_SENSOR_INIT_PARA *para)
{
	struct gc0328_priv *priv = arg;
	struct i2c_client *client = priv->client;

	gc0328_sensor_probe_id_func(arg);

	gc0328_setup(client, INIT_TAB);

	return 0;
}

/*read sensor register*/
static int gc0328_sensor_read_reg_func(void *arg, const int reg_addr)
{
	struct gc0328_priv *priv = arg;
	struct i2c_client *client = priv->client;

	return gc0328_read(client, reg_addr);
}
/*write sensor register*/
static int gc0328_sensor_write_reg_func(void *arg, const int reg_addr, int value)
{
	struct gc0328_priv *priv = arg;
	struct i2c_client *client = priv->client;

	return gc0328_write(client, reg_addr, value);
}

/*read sensor register, NO i2c ops*/
static int gc0328_sensor_read_id_func(void *arg) //no use IIC bus
{
	return SENSOR_ID;
}

/*set sensor again*/
static int gc0328_sensor_update_a_gain_func(void *arg, const unsigned int a_gain)
{
	return 0;
}
/*set sensor dgain*/
static int gc0328_sensor_update_d_gain_func(void *arg, const unsigned int d_gain)
{
	return 0;
}
/*set sensor exp time*/
static int gc0328_sensor_updata_exp_time_func(void *arg, unsigned int exp_time)
{
	return 0;
}
/*sensor timer*/
static int gc0328_sensor_timer_func(void *arg)
{
	return 0;
}
/*standby in*/
static int gc0328_sensor_set_standby_in_func(void *arg)
{
	return 0;
}
/*standby out*/
static int gc0328_sensor_set_standby_out_func(void *arg)
{
	return 0;
}
/*low level read sensor ID, user i2c ops*/
static int __gc0328_sensor_probe_id_func(struct i2c_client *client)  //use IIC bus
{
	int value;

	value = gc0328_read(client, 0xf0);
	if(SENSOR_ID != value) {
		printk(KERN_ERR"Invalid chip 0x%2x\n", value);
		return 0;
	}
	pr_info("%s id:0x%x\n", __func__, value);

	return SENSOR_ID;
}
/*read sensor ID, user i2c ops*/
static int gc0328_sensor_probe_id_func(void *arg)  //use IIC bus
{
	struct gc0328_priv *priv = arg;
	struct i2c_client *client = priv->client;

	return __gc0328_sensor_probe_id_func(client);
}
/*get resolution*/
static int gc0328_sensor_get_resolution_func(void *arg, int *width, int *height)
{
	*width = SENSOR_OUTPUT_WIDTH;
	*height = SENSOR_OUTPUT_HEIGHT;
	return 0;
}
/*get mclk*/
static int gc0328_sensor_get_mclk_func(void *arg)
{
	return SENSOR_MCLK;
}
/*get current fps*/
static int gc0328_sensor_get_fps_func(void *arg)
{
	struct gc0328_priv *priv = arg;

	return priv->fps_info.current_fps;
}
/*get valid coordinate*/
static int gc0328_sensor_get_valid_coordinate_func(void *arg, int *x, int *y)
{
	*x = SENSOR_VALID_OFFSET_X;
	*y = SENSOR_VALID_OFFSET_Y;
	return 0;
}
/*get bus type*/
static enum sensor_bus_type gc0328_sensor_get_bus_type_func(void *arg)
{
	return SENSOR_BUS_TYPE;
}
/*get self definded params*/
static int gc0328_sensor_get_parameter_func(void *arg, int param, void *value)
{
	int ret = 0;
	struct gc0328_priv *priv = arg;

	switch (param) {
		case GET_INTERFACE:
			*((int *)value) = SENSOR_IO_INTERFACE;
			break;

		case GET_IO_LEVEL:
			*((int *)value) = SENSOR_IO_LEVEL;
			break;

		case GET_RESET_GPIO:
			*((int *)value) = priv->gpio_reset;
			break;

		case GET_PWDN_GPIO:
			*((int *)value) = priv->gpio_pwdn;
			break;

		case GET_SCAN_METHOD:
			*((int *)value) = SCAN_METHOD_PROGRESSIVE;//priv->method;
			break;

		default:
			pr_err("%s param:%d not support\n", __func__, param);
			ret = -1;
			break;
	}

	return ret;
}
/*sensor power on*/
static int gc0328_sensor_set_power_on_func(void *arg)
{
	struct gc0328_priv *priv = arg;
	struct i2c_client *client = priv->client;

	pr_info("%s: pwdn %d reset %d\n", __func__, priv->gpio_pwdn, priv->gpio_reset);

	if (priv->gpio_pwdn >= 0) {
		gpio_direction_output(priv->gpio_pwdn, !SENSOR_PWDN_LEVEL);
	}

	/* gc0328 do not have reset pin */
	#if 0
	if (priv->gpio_reset >= 0) {
		gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
		msleep(10);
		gpio_direction_output(priv->gpio_reset, SENSOR_RESET_LEVEL);
		msleep(10);
		gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
		msleep(20);
	}
	#endif
    
	priv->fps_info.to_fps = priv->fps_info.current_fps;
	priv->fps_info.to_fps_value = 0;

	return 0;
}
/*sensor power off*/
static int gc0328_sensor_set_power_off_func(void *arg)
{
	struct gc0328_priv *priv = arg;

	pr_info("%s: pwdn %d reset %d\n", __func__, priv->gpio_pwdn, priv->gpio_reset);

	if (priv->gpio_pwdn >= 0) {
		gpio_direction_output(priv->gpio_pwdn, SENSOR_PWDN_LEVEL);
	}

	/* gc0328 do not have reset pin */
	#if 0
	if (priv->gpio_reset >= 0) {
		gpio_direction_output(priv->gpio_pwdn, SENSOR_PWDN_LEVEL);
	}
	#endif
	
	return 0;
}
/*set sensor fps*/
static int gc0328_sensor_set_fps_func(void *arg, const int fps)
{
	return 0;
}

static AK_ISP_SENSOR_CB gc0328_cb = {
	.sensor_init_func = gc0328_sensor_init_func,
	.sensor_read_reg_func = gc0328_sensor_read_reg_func,
	.sensor_write_reg_func = gc0328_sensor_write_reg_func,
	.sensor_read_id_func = gc0328_sensor_read_id_func,
	.sensor_update_a_gain_func = gc0328_sensor_update_a_gain_func,
	.sensor_update_d_gain_func = gc0328_sensor_update_d_gain_func,
	.sensor_updata_exp_time_func = gc0328_sensor_updata_exp_time_func,
	.sensor_timer_func = gc0328_sensor_timer_func,
	.sensor_set_standby_in_func = gc0328_sensor_set_standby_in_func,
	.sensor_set_standby_out_func = gc0328_sensor_set_standby_out_func,
	.sensor_probe_id_func = gc0328_sensor_probe_id_func,
	.sensor_get_resolution_func = gc0328_sensor_get_resolution_func,
	.sensor_get_mclk_func = gc0328_sensor_get_mclk_func,
	.sensor_get_fps_func = gc0328_sensor_get_fps_func,
	.sensor_get_valid_coordinate_func = gc0328_sensor_get_valid_coordinate_func,
	.sensor_get_bus_type_func = gc0328_sensor_get_bus_type_func,
	.sensor_get_parameter_func = gc0328_sensor_get_parameter_func,
	.sensor_set_power_on_func = gc0328_sensor_set_power_on_func,
	.sensor_set_power_off_func = gc0328_sensor_set_power_off_func,
	.sensor_set_fps_func = gc0328_sensor_set_fps_func,
};

static int sensor_id_func(void)
{
	return gc0328_sensor_read_id_func(NULL);
}

static char *sensor_if_func(void)
{
	static char ifstr[16] = "dvp"; 

	return ifstr;
}

static int call_sensor_sys_init(void)
{
	return sensor_sys_init(sensor_id_func, sensor_if_func);
}

static int call_sensor_sys_deinit(void)
{
	sensor_sys_exit();
	return 0;
}
