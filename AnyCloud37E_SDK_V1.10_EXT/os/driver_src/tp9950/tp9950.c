/*
 * TP2850.c    tp9950 Camera Driver
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
#include <linux/of_gpio.h>
#include <linux/v4l2-mediabus.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-image-sizes.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>

#include "../include/ak_video_priv_cmd.h"
#include "../include_internal/ak_video_priv_cmd_internal.h"
#include "../include/ak_isp_drv.h"
#include "../include/ak_sensor.h"
#include "../include/sensor_cmd.h"
#include "../common/sensor_sys.h"
#include "../common/sensor_i2c.h"

#include "tp2850.h"
#include "tp9950_720p_25fps.h"
#include "tp9950_720p_30fps.h"
#include "tp9950_1080p_25fps.h"
#include "tp9950_1080p_30fps.h"
#include "tp9950_ntsc_60fps.h"
#include "tp9950_pal_50fps.h"
#include "tp9950_pal_nosig.h"

#define DEBUG            1  //printk debug information on/off

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
EXP_EFFECT_FRAMES
DELAY_FLAG
 *
 * */
#define SENSOR_PWDN_LEVEL		0 
#define SENSOR_RESET_LEVEL		0
#define SENSOR_I2C_ADDR			0x45
#define SENSOR_ID				0x2850
#define SENSOR_MCLK				27
#define SENSOR_REGADDR_BYTE		1
#define SENSOR_DATA_BYTE		1

#if 0
#define SENSOR_OUTPUT_WIDTH		1280
#define SENSOR_OUTPUT_HEIGHT	720
#else
static int SENSOR_OUTPUT_WIDTH = 960;//960
static int SENSOR_OUTPUT_HEIGHT = 1080; //256
#endif

static char* video_in_state[] = {"unplug", "in", "locked", "unlock"};

static int SENSOR_TYPE = 0; 
//#define SENSOR_VALID_OFFSET_X	1
static int SENSOR_VALID_OFFSET_X = 0;
#define SENSOR_VALID_OFFSET_Y	0//0
#define SENSOR_BUS_TYPE			BUS_TYPE_YUV
#if defined(CONFIG_MACH_AK37E)
#define SENSOR_IO_INTERFACE		DVP_INTERFACE_BT656
#else
#define SENSOR_IO_INTERFACE		DVP_INTERFACE
#endif
#define SENSOR_IO_LEVEL			IO_LEVEL_3V3
#define MAX_FPS					25

#define EXP_EFFECT_FRAMES		1
#define DELAY_FLAG		(0xffff)

/*
 *  * check_id - check hardware sensor ID whether it meets this driver
 *  * 0-no check, force to meet
 *  * others-check, if not meet return fail
 *  */
static int check_id = 0;
module_param(check_id, int, 0644);

static int force_output = 0;
module_param(force_output, int, 0644);

static int force_check = 0;
module_param(force_check, int, 0644);

enum input_res {
	INPUT_SD480I = 0,
	INPUT_SD576I,
	INPUT_HD720P,
	INPUT_HD1080P,
	INPUT_HD960P_HD800P,
};

struct res_to_timing_info {
	int fps;
	int res;
	struct isp_timing_info isp_timing;
};

/*
 * Struct
 */
struct regval_list {
	u8 reg_num;
	u8 value;
};

struct  tp9950_win_size {
	char				*name;
	u32				width;
	u32				height;
	const struct regval_list	*regs;
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

/*
 * the host callbacks for sensor
 * @isp_timing_cb_info: isp timing callback.
 * 	a few sensor need the callback when fps had changed
 */
struct host_callbacks {
	struct isp_timing_cb_info isp_timing_cb_info;
};

/*
 * aec paramters. some sensors need record paramters.
 */
struct tp9950_aec_parms {

};

struct tp9950_priv {
	struct i2c_client *client;
	struct v4l2_subdev		subdev;
	struct v4l2_ctrl_handler	hdl;
	u32	cfmt_code;
	const struct tp9950_win_size	*win;

	int gpio_reset;
	int gpio_pwdn;
	struct sensor_cb_info cb_info;
	struct sensor_fps_info fps_info;

	struct host_callbacks hcb;
	struct tp9950_aec_parms aec_parms;

	enum scan_method method;

	AK_ISP_SENSOR_REG_INFO *preg_info;
	int num;
	int fps_res;

	struct v4l2_ctrl *ctrl_sensor_get_id;

};

struct tp9950_priv *p_gpriv = NULL;

spinlock_t readid_lock;
spinlock_t reg_lock;

static AK_ISP_SENSOR_CB tp9950_cb;

static struct res_to_timing_info res_to_timing[] = {
	{60, INPUT_SD480I, {9000,	1,	1600,	18}},	/*INPUT_SD480I NTSC */ 
	{50, INPUT_SD576I, {7680,	1,	1500,	24}},	/*INPUT_SD576I PAL */
	{25, INPUT_HD720P, {9599,	1,	3373,	18}},	/*INPUT_HD720P@25fps*/
	{30, INPUT_HD720P, {8000,	1,	1774,	17}},	/*15 INPUT_HD720P@30fps*/
	{25, INPUT_HD1080P, {6480,	1,	1728,	28}},	/*INPUT_HD1080P@25*/
	{30, INPUT_HD1080P, {6480,	1,	1728,	23}},	/*INPUT_HD1080P@30*/
	{50, INPUT_HD960P_HD800P, {6480,	1,	1728,	3}},	/*INPUT_HD1080P@30*/
};

static const struct v4l2_ctrl_ops tp9950_ctrl_ops;
static const struct v4l2_ctrl_config config_sensor_get_id = {
	.ops	= &tp9950_ctrl_ops,
	.id		= SENSOR_GET_ID,
	.name	= "get sensor id",
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.min	= 0,
	.max	= 0xfffffff,
	.step	= 1,
	.def	= 0,
	.flags	= V4L2_CTRL_FLAG_VOLATILE,/*set V4L2_CTRL_FLAG_VOLATILE avoid get cached*/
};

static u32 tp9950_codes[] = {
	MEDIA_BUS_FMT_YUYV8_2X8,
	MEDIA_BUS_FMT_UYVY8_2X8,
	MEDIA_BUS_FMT_RGB565_2X8_BE,
	MEDIA_BUS_FMT_RGB565_2X8_LE,
};

typedef struct
{
	unsigned int	count;
	unsigned int	mode;
	unsigned int	scan;
	unsigned int	gain[4];
	unsigned int 	std;
	unsigned int	state;
	unsigned int	force;
	unsigned char	addr;
	unsigned int	fps;
} tp9950au_info;


static void tp9950_set_fps_async(struct tp9950_priv *priv);
static int __tp9950_sensor_probe_id_func(struct i2c_client *client);  //use IIC bus
static int tp9950_sensor_read_id_func(void *arg);	//no use IIC bus
static int tp9950_sensor_get_resolution_func(void *arg, int *width, int *height);
static int tp9950_sensor_probe_id_func(void *arg);
static int call_sensor_sys_init(void);
static int call_sensor_sys_deinit(void);

static AK_ISP_SENSOR_REG_INFO *tp9950_get_init_sequence(
		struct i2c_client *client, struct tp9950_priv *priv, int *fps, int *num);

static int tp9950_auto(const struct i2c_client *client);
static int tp9950_auto_detect(void *arg);
static void tp9950_set_reg_page(const struct i2c_client *client, unsigned char ch);

/*
 * General functions
 */
static struct tp9950_priv *to_tp9950(const struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct tp9950_priv,
			subdev);
}

static struct v4l2_subdev *ctrl_to_sd(struct v4l2_ctrl *ctrl)
{
	return &container_of(ctrl->handler, struct tp9950_priv, hdl)->subdev;
}

#if 0
/*
 * General functions
 */
unsigned int ConvertACPV1Data(unsigned char dat)
{
	unsigned int i, tmp=0;

	for(i = 0; i < 8; i++)
	{
		tmp <<= 3;

		if(0x01 & dat) tmp |= 0x06;
		else tmp |= 0x04;

		dat >>= 1;
	}

	return tmp;
}
#endif

/*sensor i2c write*/
static int tp9950_write(const struct i2c_client *client, int reg, int value)
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
static int tp9950_read(const struct i2c_client *client, int reg)
{
	struct i2c_transfer_struct trans = {
		.client = client,
		.reg_bytes = SENSOR_REGADDR_BYTE,
		.value_bytes = SENSOR_DATA_BYTE,
		.reg = reg,
		.value = 0,
	};

	//printk(KERN_ERR "%s client:%p, addr:%x\n", __func__, client, client->addr);
	return i2c_read(&trans);
}

/*
 * XXXX_s_stream -
 * set steaming enable/disable
 * soc_camera_ops functions
 *
 * @sd:				subdev
 * @enable:			enable flags
 */
static int tp9950_s_stream(struct v4l2_subdev *sd, int enable)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	
	return 0;
}

static int tp9950_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = 0;
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	switch (ctrl->id) {
		case SENSOR_GET_ID:
			ctrl->val = ((SENSOR_ID << 8) | SENSOR_TYPE);
			break;

		default:
			ret = -EINVAL;
			pr_err("%s id:%d no support\n", __func__, ctrl->id);
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
static int tp9950_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = 0;
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);

#if (DEBUG)
	printk(KERN_ERR "%s set 0x%x\n", __func__, ctrl->id);
#endif

	switch (ctrl->id) {
		case SENSOR_GET_ID:  //unuse here, xiaozijie
			break;
		case V4L2_CID_HFLIP:
			break;
		case V4L2_CID_VFLIP:
			break;
		case V4L2_CID_BRIGHTNESS:
			break;
		case V4L2_CID_CONTRAST:
			break;
		default:
			pr_err("%s cmd:%x not support\n", __func__, ctrl->id);
			//return -EINVAL;
			break;
	}

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
static int tp9950_core_s_power(struct v4l2_subdev *sd, int on)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	
	return 0;
}

/*
 * XXXX_get_sensor_id -
 * get sensor ID
 * private callback functions
 *
 * @ctrl:			pointer to ctrl
 */
static int tp9950_get_sensor_id(struct v4l2_control *ctrl)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	
	ctrl->value = tp9950_sensor_read_id_func(NULL);   //no use IIC bus
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
static int tp9950_get_sensor_cb(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);

#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

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
static int tp9950_core_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	int ret;

#if (DEBUG)
	printk(KERN_ERR "%s %x\n", __func__, ctrl->id);
#endif	

	switch (ctrl->id) {
		case GET_SENSOR_ID:
			ret = tp9950_get_sensor_id(ctrl);
			break;

		case GET_SENSOR_CB:
			ret = tp9950_get_sensor_cb(sd, ctrl);
			break;

		default:
			pr_err("%s cmd:%d not support\n", __func__, ctrl->id);
			ret = -1;
			break;
	}

	return ret;
}

/*
 * XXXX_set_isp_timing_cb -
 * set isp timing callback function
 * private callback functions
 *
 * @sd:				subdev
 * @ctrl:			pointer to ctrl
 */
static int tp9950_set_isp_timing_cb(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);
	struct isp_timing_cb_info *isp_timing_cb_info = (void *)ctrl->value;

	pr_info("%s\n", __func__);
	//pr_info("%s isp_timing_cb_info = %p\n", __func__, isp_timing_cb_info);;

	memcpy(&priv->hcb.isp_timing_cb_info,
			isp_timing_cb_info,
			sizeof(struct isp_timing_cb_info));

	return 0;
}

/*
 * XXXX_core_s_ctrl -
 * set core s_ctrl
 * core functions
 *
 * @sd:				subdev
 * @ctrl:			pointer to ctrl
 */
static int tp9950_core_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	int ret;

#if (DEBUG)	
	pr_info("%s ctrl->id = 0x%x\n", __func__,ctrl->id);
#endif

	switch (ctrl->id) {
		case SET_ISP_MISC_CALLBACK:
			ret = tp9950_set_isp_timing_cb(sd, ctrl);
			break;

		default:
			pr_err("%s cmd:%d not support\n", __func__, ctrl->id);
			ret = -1;
			break;
	}

	return ret;
}

static long tp9950_core_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);
	int ret = 0;

#if (DEBUG)
	pr_info("%s cmd= 0x%x\n", __func__, cmd);
#endif

	switch (cmd) {
		case AK_SENSOR_SET_INIT:
			/*pr2000 regs in driver, so returen success*/
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
static int tp9950_get_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client  *client = v4l2_get_subdevdata(sd);
	struct tp9950_priv *priv = to_tp9950(client);
	static struct tp9950_win_size win = {
		.name = "default",
		.width = 0,//SENSOR_OUTPUT_WIDTH,
		.height = 0,//SENSOR_OUTPUT_HEIGHT,
		.regs = NULL,
	};

	printk(KERN_ERR "%s %d\n", __func__, __LINE__);

	win.width = SENSOR_OUTPUT_WIDTH;
	win.height = SENSOR_OUTPUT_HEIGHT;

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
static int tp9950_set_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	//struct i2c_client *client = v4l2_get_subdevdata(sd);

	printk(KERN_ERR "%s %d\n", __func__, __LINE__);

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
static int tp9950_get_selection(struct v4l2_subdev *sd,
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
static int tp9950_enum_mbus_code(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_mbus_code_enum *code)
{
	if (code->pad || code->index >= ARRAY_SIZE(tp9950_codes))
		return -EINVAL;

	//code->code = MEDIA_BUS_FMT_YUYV8_1_5X8;//pr2000_codes[code->index];
	code->code = MEDIA_BUS_FMT_YUYV8_2X8;//pr2000_codes[code->index];
	//code->code = MEDIA_BUS_FMT_SBGGR8_1X8;
	//code->code = V4L2_PIX_FMT_YUV420;

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
static int tp9950_g_crop(struct v4l2_subdev *sd, struct v4l2_crop *a)
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
static int tp9950_cropcap(struct v4l2_subdev *sd, struct v4l2_cropcap *a)
{
	tp9950_sensor_get_resolution_func(NULL, &a->bounds.width, &a->bounds.height);
	a->bounds.left			= SENSOR_VALID_OFFSET_X;
	a->bounds.top			= SENSOR_VALID_OFFSET_Y;
	//a->bounds.width			= UXGA_WIDTH;
	//a->bounds.height		= UXGA_HEIGHT;
	a->defrect			= a->bounds;
	a->type				= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	a->pixelaspect.numerator	= 1;
	a->pixelaspect.denominator	= 1;

	pr_info("%s bounds.width:%d, bounds.height:%d\n",
			__func__, a->bounds.width, a->bounds.height);

	pr_info("%s defrect [%d %d %d %d]\n",
			__func__, a->defrect.left, a->defrect.top,
			a->defrect.width, a->defrect.height);

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
static int tp9950_video_probe(struct i2c_client *client)
{
	int ret;
	struct tp9950_priv *priv = to_tp9950(client);

#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	ret = v4l2_ctrl_handler_setup(&priv->hdl);
	return ret;
}

/*ctrl ops*/
static const struct v4l2_ctrl_ops tp9950_ctrl_ops = {
	.s_ctrl 			= tp9950_s_ctrl,
	.g_volatile_ctrl	= tp9950_g_volatile_ctrl,
};

/*core ops*/
static struct v4l2_subdev_core_ops tp9950_subdev_core_ops = {
	.s_power	= tp9950_core_s_power,
	.g_ctrl		= tp9950_core_g_ctrl,
	.s_ctrl		= tp9950_core_s_ctrl,
	.ioctl		= tp9950_core_ioctl,
};

/*
 * XXXX_g_mbus_config -
 * get buf config
 * video functions
 *
 * @sd:				subdev
 * @cfg:			return config
 */
static int tp9950_g_mbus_config(struct v4l2_subdev *sd,
		struct v4l2_mbus_config *cfg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	printk(KERN_ERR "%s %d\n",__func__, __LINE__);
	cfg->flags = V4L2_MBUS_PCLK_SAMPLE_FALLING | V4L2_MBUS_MASTER |
		V4L2_MBUS_VSYNC_ACTIVE_HIGH | V4L2_MBUS_HSYNC_ACTIVE_HIGH |
		V4L2_MBUS_DATA_ACTIVE_HIGH;//V4L2_MBUS_PCLK_SAMPLE_RISING
	cfg->type = V4L2_MBUS_PARALLEL;
	cfg->flags = 0;

	return 0;
}

/*set crop*/
static int tp9950_s_crop(struct v4l2_subdev *sd, const struct v4l2_crop *crop)
{
	printk(KERN_ERR "%s %d, left:%d, top:%d, width:%d, height:%d\n",
			__func__, __LINE__, crop->c.left, crop->c.top, crop->c.width, crop->c.height);
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
static struct v4l2_subdev_video_ops tp9950_subdev_video_ops = {
	.s_stream			= tp9950_s_stream,
	.cropcap			= tp9950_cropcap,
	.g_crop				= tp9950_g_crop,
	.g_mbus_config		= tp9950_g_mbus_config,
	.s_crop				= tp9950_s_crop,
};

/*pad ops*/
static const struct v4l2_subdev_pad_ops tp9950_subdev_pad_ops = {
	.enum_mbus_code 	= tp9950_enum_mbus_code,
	.get_fmt			= tp9950_get_fmt,
	.set_fmt			= tp9950_set_fmt,
	.get_selection		= tp9950_get_selection,
};

/*sensor driver subdev ops*/
static struct v4l2_subdev_ops tp9950_subdev_ops = {
	.core	= &tp9950_subdev_core_ops,
	.video	= &tp9950_subdev_video_ops,
	.pad	= &tp9950_subdev_pad_ops,
};

/*
 * sensor_of_parse -
 * parse node of device
 *
 * @client:			pointor to i2c client
 * @priv:			sensor struct
 */
static int sensor_of_parse(struct i2c_client *client, struct tp9950_priv *priv)
{
	struct device_node *np = client->dev.of_node;
	int ret = -1;

	//printk(KERN_ERR"%s start\n", __func__);

	/*it is exist reset but lack of pwdn gpio in most case*/
	priv->gpio_reset = of_get_named_gpio(np, "reset-gpio", 0);
	priv->gpio_pwdn = of_get_named_gpio(np, "pwdn-gpio", 0);

	dev_info(&client->dev, "gpio_reset:%d, gpio_pwdn:%d\n", priv->gpio_reset, priv->gpio_pwdn);

	if (priv->gpio_reset >= 0) {
		ret = devm_gpio_request(&client->dev, priv->gpio_reset, "sensor-reset");
		if(ret < 0)
		{
			dev_err(&client->dev,"Request gpio reset fail\n");
		}
		else
			dev_info(&client->dev,"Request gpio reset OK\n");
	}

	if (priv->gpio_pwdn >= 0) {
		ret = devm_gpio_request(&client->dev, priv->gpio_pwdn, "sensor-pwdn");
		if(ret < 0)
		{
			dev_err(&client->dev,"Request gpio pwdn fail\n");
		}
		else
			dev_info(&client->dev,"Request gpio pwdn OK\n");
	}

	return 0;
}

/*
 * XXXX_detect- detect mode
 * @priv:			pointer to pirvate structure
 *
 * @RETURN:	none
 */
static void tp9950_detect(struct tp9950_priv *priv)
{
	struct i2c_client *client = priv->client;
	int fps = 0;
	int num;
	int val;

	printk(KERN_ERR"%s start\n", __func__);

	priv->cb_info.cb->sensor_set_power_on_func(priv);

	tp9950_set_reg_page(client, VIDEO_PAGE);

	/*auto detect video mode input*/
	priv->preg_info = tp9950_get_init_sequence(client, priv, &fps, &num);
	if(priv->preg_info == NULL)
	{
		printk(KERN_ERR"%s unknow preg info\n", __func__);
		return;
	}

	priv->num = num;
	priv->fps_info.current_fps = fps;
}


/*
 *  * XXXX_match - check sensor id for match driver
 *  * @priv:matchpointer to pirvate structure
 *  *
 *  * @RETURN:RETURN0-match fail; 1-match success
 *  */
static int tp9950_match(struct tp9950_priv *priv)
{
	int pid = 0;

	if (!check_id)
		return 1;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	if(priv->cb_info.cb->sensor_set_power_on_func)
		priv->cb_info.cb->sensor_set_power_on_func(priv);

	if(priv->cb_info.cb->sensor_probe_id_func)
	{
		pid = priv->cb_info.cb->sensor_probe_id_func(priv);
		if (pid <= 0) {
			pr_err("%s fail\n", __func__);
			return 0;
		}
	}

	return 1;
}

/*
 * i2c_driver functions
 */
static int tp9950_probe(struct i2c_client *client,
		const struct i2c_device_id *did)
{
	int ret = 0, i = 0, val = 0;
	struct tp9950_priv	*priv;
	struct i2c_adapter	*adapter = to_i2c_adapter(client->dev.parent);

	printk("TP9950 driver version %d.%d.%d loaded\n",
			(TP2850_VERSION_CODE >> 16) & 0xff,
			(TP2850_VERSION_CODE >>  8) & 0xff,
			TP2850_VERSION_CODE & 0xff);

	printk("Compiled %s.%s.\n", __DATE__, __TIME__);

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&adapter->dev,
				"TP9950: I2C-Adapter doesn't support SMBUS\n");
		return -EIO;
	}

	priv = devm_kzalloc(&client->dev, sizeof(struct tp9950_priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&client->dev,
				"Failed to allocate memory for private data!\n");
		return -ENOMEM;
	}

	/*parse of node*/
	sensor_of_parse(client, priv);

	/*
	 * the current sensor slave address to client.
	 * the address from dts may incorrect
	 * */
	client->addr 		= SENSOR_I2C_ADDR;
	priv->cb_info.cb 	= &tp9950_cb;
	priv->cb_info.arg 	= priv;
	priv->client 		= client;

	if (!client->dev.of_node) {
		dev_err(&client->dev, "Missing platform_data for driver\n");
		ret = -EINVAL;
		goto err_clk;
	}

	tp9950_detect(priv);

	if (!tp9950_match(priv)) {
		ret = -ENODEV;
		goto err_clk;
	}

	/*subdev init*/
	v4l2_i2c_subdev_init(&priv->subdev, client, &tp9950_subdev_ops);
	priv->subdev.flags |= /*V4L2_SUBDEV_FL_HAS_EVENTS | */V4L2_SUBDEV_FL_HAS_DEVNODE;
	v4l2_ctrl_handler_init(&priv->hdl, 5);
	v4l2_ctrl_new_std(&priv->hdl, &tp9950_ctrl_ops,
			V4L2_CID_VFLIP, 0, 1, 1, 0);
	v4l2_ctrl_new_std(&priv->hdl, &tp9950_ctrl_ops,
			V4L2_CID_HFLIP, 0, 1, 1, 0);
	v4l2_ctrl_new_std(&priv->hdl, &tp9950_ctrl_ops,
			V4L2_CID_BRIGHTNESS, 0, 1, 1, 0);
	v4l2_ctrl_new_std(&priv->hdl, &tp9950_ctrl_ops,
			V4L2_CID_CONTRAST, 0, 1, 1, 0);

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

	ret = tp9950_video_probe(client);
	if (ret < 0) {
		dev_err(&client->dev, "TP9950 video probe fail\n");
		goto err_videoprobe;
	}

	/*register async subdev*/
	ret = v4l2_async_register_subdev(&priv->subdev);
	if (ret < 0) {
		dev_err(&client->dev, "v4l2 async register subdev fail, ret:%d\n", ret);
		goto err_videoprobe;
	}

	dev_info(&adapter->dev, "TP9950 Probed success, subdev:%p\n", &priv->subdev);

	spin_lock_init(&readid_lock);
	spin_lock_init(&reg_lock);	

	/*ak platform need export some informations from sensor*/
	call_sensor_sys_init();

	p_gpriv = priv;
	printk("TP9950 Driver Init Successfully@0x%x!\n", client->addr);

	return 0;

err_videoprobe:
	v4l2_ctrl_handler_free(&priv->hdl);
err_clk:
	return ret;
}

static int tp9950_remove(struct i2c_client *client)
{
	struct tp9950_priv *priv = to_tp9950(client);

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

	priv = NULL;
	p_gpriv = NULL;
	return 0;
}

static const struct i2c_device_id tp9950_id[] = {
	{ "tp9950", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, tp9950_id);

static const struct of_device_id tp9950_of_match[] = {
	/*donot changed compatible, must the same as dts*/
	{.compatible = "anyka,sensor0", },
	{},
};
MODULE_DEVICE_TABLE(of, tp9950_of_match);

static struct i2c_driver tp9950_i2c_driver = {
	.driver = {
		.name = "anyka-sensor",
		.of_match_table = of_match_ptr(tp9950_of_match),
	},
	.probe    = tp9950_probe,
	.remove   = tp9950_remove,
	.id_table = tp9950_id,
};

module_i2c_driver(tp9950_i2c_driver);

MODULE_DESCRIPTION("SoC Camera driver for TechPoint TP2850(tp9950) sensor");
MODULE_AUTHOR("Anyka Microelectronic");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0.04");

///////////////////////////////////////////////////////////////

static void tp9950_set_reg_page(const struct i2c_client *client, unsigned char ch)
{
	switch(ch)
	{
		case MIPI_PAGE:
			tp9950_write(client, 0x40, 0x08);
			break;
		default:
			tp9950_write(client, 0x40, 0x00);
			break;
	}
}

static void tp9950_reset_default(const struct i2c_client *client, unsigned char ch)
{
	unsigned int tmp;

	printk(KERN_ERR "%s %d\n", __func__, __LINE__);

	tp9950_write(client, 0x40, 0x08);
	tp9950_write(client, 0x23, 0x02);

	tp9950_write(client, 0x40, 0x00);
	tp9950_write(client, 0x07, 0xC0);
	tp9950_write(client, 0x0B, 0xC0);

	tmp = tp9950_read(client, 0x26);
	tmp &= 0xfe;
	tp9950_write(client, 0x26, tmp);

	tmp = tp9950_read(client, 0xa7);
	tmp &= 0xfe;
	tp9950_write(client, 0xa7, tmp);

	tmp = tp9950_read(client, 0x06);
	tmp &= 0xfb;
	tp9950_write(client, 0x06, tmp);
}

//
//有video信号时，使用的reset*/
//
static void tp9950_soft_reset(struct i2c_client *client)
{
	int val = 0;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	

	//
	//soft reset
	//
	tp9950_set_reg_page(client, VIDEO_PAGE);

	val = tp9950_read(client, 0x06);
	tp9950_write(client, 0x06, 0x80 | val);

	msleep(20);
}

static void tp9950_comm_init(const struct i2c_client *client)
{
	tp9950_write(client, 0x40, 0x00); //关闭mipi寄存访问
	tp9950_write(client, 0x41, 0x00); // default Vin1,
	tp9950_write(client, 0x4c, 0x40); //VSPOL NOT inversed && HSPOL NOT inversed
	tp9950_write(client, 0x4e, 0x00);
	tp9950_write(client, 0x35, 0x25); //MISC参数，25：FSL选择72.45MHz  05：FSL选择148.5MHz
	tp9950_write(client, 0xf5, 0x10); // Vin ADC 时钟极性反向 VIN HDTV Decoder system clock is 148.5MHz
	tp9950_write(client, 0xfd, 0x80); // test SCL/SDA bus mode
	tp9950_write(client, 0x38, 0x40); // AFE 电流50uA
	tp9950_write(client, 0x3d, 0x60);
}

static int tp9950_get_intput_sensor(int mode,enum input_res *res,int *fps)
{
	int video_det = 0;

#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	switch(mode)
	{
		case TP2802_1080P25:
			*fps 	= 25;
			*res 	= INPUT_HD1080P;
			video_det = 1;
			pr_info("set INPUT_HD1080P\n");
			break;

		case TP2802_1080P30:
			*fps = 30;
			*res = INPUT_HD1080P;
			video_det = 1;
			break;

		case TP2802_720P25:
			*fps = 25;
			*res = INPUT_HD720P;
			break;

		case TP2802_720P30:
			*fps = 30;
			*res = INPUT_HD720P;
			break;

		case TP2802_720P50:
			*fps = 50;
			*res = INPUT_HD720P;
			break;

		case TP2802_720P60:
			*fps = 60;
			*res = INPUT_HD720P;
			break;

		case TP2802_720P30V2:
			*fps = 30;
			*res=INPUT_HD720P;
			video_det = 1;
			pr_info("set INPUT_HD720P\n");
			break;

		case TP2802_720P25V2:
			*fps = 25;
			*res = INPUT_HD720P;
			video_det = 1;
			pr_info("set INPUT_HD720P\n");
			break;

		case TP2802_PAL:
			*fps = 50;
			*res = INPUT_SD576I;
			video_det = 1;
			pr_info("set INPUT_SD576I\n");
			break;

		case TP2802_NTSC:
			*fps = 60;//50
			*res = INPUT_SD480I;
			video_det = 1;
			break;
		case INVALID_FORMAT:
			*fps = 50;
			*res = INPUT_SD576I;
			pr_err("ERROR:INVALID_FORMAT\n");
		default:
			video_det = 0;
			break;
	}

#if (DEBUG)		
	pr_info("%s mode:%d\n", __func__, mode);
#endif	

	return video_det;
}

static int  tp9950_get_resolution(int mode)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	switch(mode)
	{
		case TP2802_1080P25:
		#ifdef SENSOR_1080P_25FPS_FULL_OUTPUT
			SENSOR_OUTPUT_WIDTH = 1920;
		#else
			SENSOR_OUTPUT_WIDTH = 960;
		#endif
			SENSOR_OUTPUT_HEIGHT = 1080;
			break;
		case TP2802_1080P30:
		#ifdef SENSOR_1080P_30FPS_FULL_OUTPUT
			SENSOR_OUTPUT_WIDTH = 1920;
		#else
			SENSOR_OUTPUT_WIDTH = 960;
		#endif
			SENSOR_OUTPUT_HEIGHT = 1080; 
			break;
		case TP2802_720P25:
		case TP2802_720P30:
		case TP2802_720P50:
		case TP2802_720P60:
		case TP2802_720P30V2:
			SENSOR_OUTPUT_WIDTH = 1280;
			SENSOR_OUTPUT_HEIGHT = 720; 
			break;

		case TP2802_720P25V2:
			SENSOR_OUTPUT_WIDTH = 1280;
			SENSOR_OUTPUT_HEIGHT = 720; 
			break;

		case TP2802_PAL:
			SENSOR_OUTPUT_WIDTH = 960;//960
			SENSOR_OUTPUT_HEIGHT = 288;// 288
			break;

		case TP2802_NTSC:
			SENSOR_OUTPUT_WIDTH = 960;
			SENSOR_OUTPUT_HEIGHT = 240;//240
			break;

		case INVALID_FORMAT:
			/*force setting 960*288, only for one input source */
			SENSOR_OUTPUT_WIDTH = 960;
			SENSOR_OUTPUT_HEIGHT = 288; 
			pr_err("ERR:INVALID_FORMAT\n");
		default:
			return -1;
			break;
	}

	return 0;
}

static void set_timing(struct tp9950_priv *priv, int fps_res)
{
	int i;

	for (i = 0; i < sizeof(res_to_timing) / sizeof(res_to_timing[0]); i++)
	{
		if (((res_to_timing[i].fps << 8) | res_to_timing[i].res) == fps_res) 
		{
			void *arg = priv->hcb.isp_timing_cb_info.isp_timing_arg;
			SET_ISP_MISC set_isp_timing = priv->hcb.isp_timing_cb_info.set_isp_timing;

			//pr_err("set_isp_timing:%p, arg:%p, i:%d\n", set_isp_timing, arg, i);

			if (set_isp_timing)
				set_isp_timing(arg, &res_to_timing[i].isp_timing);

			break;
		}
	}

	return;
}

/*get init sequence suite for current analog input signal*/
static AK_ISP_SENSOR_REG_INFO *tp9950_get_init_sequence(
		struct i2c_client *client, struct tp9950_priv *priv, int *fps, int *num)
{
	int ret = 0;
	int fps_res;
	enum input_res res;
	AK_ISP_SENSOR_REG_INFO *preg_info = NULL;

#if (DEBUG)
	pr_err("%s %d force = %d\n", __func__, __LINE__, force_output);
#endif

	//
	//detect analog signal input
	//
	SENSOR_TYPE = tp9950_auto(client);
	if(SENSOR_TYPE == INVALID_FORMAT)
	{
		pr_err("%s force output %d\n", __func__,__LINE__);
		if(force_output == 0)
		{
			goto end;
		}


	}
	else
	{
		ret = tp9950_get_resolution(SENSOR_TYPE);
		if(ret == 0)
		{
			pr_err("%s SENSOR_TYPE:0x%x ,OUTPUT_WIDTH = %d,OUTPUT_HEIGHT=%d\n", 
					__func__,
					SENSOR_TYPE,
					SENSOR_OUTPUT_WIDTH,
					SENSOR_OUTPUT_HEIGHT);
		}
	}

	if(!tp9950_get_intput_sensor(SENSOR_TYPE, &res, fps))
	{
		pr_err("%s err %d\n", __func__,__LINE__);
		if(force_output == 0)
		{
			goto end;
		}


	}

	fps_res = (*fps << 8) | res;
	switch (fps_res)
	{

		case ((60 << 8) | INPUT_SD480I):
			preg_info = TP9950_NTSC_60FPS_INIT;
			*num = sizeof(TP9950_NTSC_60FPS_INIT) / sizeof(TP9950_NTSC_60FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 0;
			//
			//丢帧设置*/
			//
			priv->method = SCAN_METHOD_INTERLACED_EVEN_FIRST;
			//priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("TP9950_NTSC_60FPS_INIT\n");
			break;
		case ((50 << 8) | INPUT_SD576I):
			preg_info = TP9950_PAL_50FPS_INIT;
			*num = sizeof(TP9950_PAL_50FPS_INIT) / sizeof(TP9950_PAL_50FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 0;
			//
			//丢帧设置*/
			//
			priv->method = SCAN_METHOD_INTERLACED_ODD_FIRST;
			//priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("TP9950_PAL_50FPS_INIT\n");
			break;

		case ((25 << 8) | INPUT_HD1080P):
			preg_info = TP9950_HDA_1080P_25FPS_INIT;
			*num = sizeof(TP9950_HDA_1080P_25FPS_INIT) / sizeof(TP9950_HDA_1080P_25FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 0;
			//不丢帧设*/
			priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("TP9950_HDA_1080P_25FPS_INIT\n");
			break;
		case ((30 << 8) | INPUT_HD1080P):
			preg_info = TP9950_HDA_1080P_30FPS_INIT;
			*num = sizeof(TP9950_HDA_1080P_30FPS_INIT) / sizeof(TP9950_HDA_1080P_30FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 0;
			//不丢帧设*/
			priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("TP9950_HDA_1080P_30FPS_INIT\n");
			break;
		case ((25 << 8) | INPUT_HD720P):
			preg_info = TP9950_HDA_720P_25FPS_INIT;
			*num = sizeof(TP9950_HDA_720P_25FPS_INIT) / sizeof(TP9950_HDA_720P_25FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 0;
			//
			priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("TP9950_HDA_720P_25FPS_INIT\n");
			break;	
		case ((30 << 8) | INPUT_HD720P):
		#if 1
			preg_info = TP9950_HDA_720P_30FPS_INIT;
			*num = sizeof(TP9950_HDA_720P_30FPS_INIT) / sizeof(TP9950_HDA_720P_30FPS_INIT[0]);
			SENSOR_VALID_OFFSET_X = 1;//1
		#else
			preg_info = TP9950_TVI_720P_30FPS_INIT;
			*num = sizeof(TP9950_TVI_720P_30FPS_INIT) / sizeof(TP9950_TVI_720P_30FPS_INIT[0]);
		#endif
			priv->method = SCAN_METHOD_PROGRESSIVE;
			pr_info("INPUT_HD720P 30fps x=%d y=%d\n",SENSOR_VALID_OFFSET_X,SENSOR_VALID_OFFSET_Y);
			break;
		default:
			pr_err("%s cannot support res:%d, fps:%d\n", __func__, res, *fps);
			break;
	}

	if(force_output == 1 && SENSOR_TYPE == INVALID_FORMAT)
	{
		pr_info("%s force output %d %d\n", __func__, res, *fps);

		preg_info = TP9950_PAL_50FPS_NOSIG_INIT;
		*num = sizeof(TP9950_PAL_50FPS_NOSIG_INIT) / sizeof(TP9950_PAL_50FPS_NOSIG_INIT[0]);
		SENSOR_VALID_OFFSET_X = 0;
		priv->method = SCAN_METHOD_INTERLACED_ODD_FIRST;
	}

	set_timing(priv, fps_res);

end:
	return preg_info;
}

static tp9950au_info au_info = {
	.count 	= 0,
	.force 	= 0,
	.mode 	= INVALID_FORMAT,
	.scan 	= SCAN_AUTO,
	.state 	= VIDEO_UNPLUG,
	.std 	= STD_TVI,
};

static int tp9950_auto(const struct i2c_client *client)
{
	unsigned char status, cvstd, tmp,flag_locked = 0;
	unsigned int val;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	tp9950_comm_init(client);

	while (au_info.count < MAX_COUNT)
	{
		if(SCAN_DISABLE == au_info.scan)
		{
			continue;
		}
		tp9950_set_reg_page(client, VIDEO_PAGE);
		status = tp9950_read(client, 0x01);
		pr_info("@%d: status 0x%02x\n",au_info.count, status);
		//state machine for video checking
		if(status & FLAG_LOSS) //no video
		{
#if (DEBUG)
			printk(KERN_ERR "video loss status 0x%x\n", status);
#endif
			//au_info.state = VIDEO_UNPLUG;
			//au_info.mode  = INVALID_FORMAT;

			if(0 == au_info.count) //first time into no video
			{
				tp9950_reset_default(client, VIDEO_PAGE);
				au_info.count++;
			}
			else
			{
				if(au_info.count < MAX_COUNT)
				{
					au_info.count++;
				}
				continue;
			}
		}
		else //there is video
		{
#if (DEBUG)
			printk(KERN_ERR "there is video ,status 0x%x\n", status);
#endif
			flag_locked = FLAG_HV_LOCKED;
			if( flag_locked == (status & flag_locked) ) //video locked
			{
				if(VIDEO_LOCKED == au_info.state)
				{
#if (DEBUG)
					printk(KERN_ERR "Video Locked, status 0x%x\n", status);
#endif
					if(au_info.count < MAX_COUNT) 
					{
						au_info.count++;
					}
				}
				else if(VIDEO_UNPLUG == au_info.state)
				{
					au_info.state = VIDEO_IN;
					au_info.count = 0;
#if (DEBUG)
					printk(KERN_ERR "1video in,status 0x%x\n", status);
#endif
				}
				else if(au_info.mode != INVALID_FORMAT)
				{
					au_info.state = VIDEO_LOCKED;
					au_info.count = 0;
#if (DEBUG)
					printk(KERN_ERR "video locked, status 0x%x\n", status);
#endif
				}
				else
				{
					/* Xiaozijie, 异常强制退出 */
#if (DEBUG)
					printk(KERN_ERR "Unknow video status 0x%x\n", status);
#endif                	
					if(au_info.count < MAX_COUNT) 
					{
						au_info.count++;
					}
				}
			}
			else  //video in but unlocked
			{
#if (DEBUG)
				printk(KERN_ERR "video not locked ,status 0x%x\n", status);
#endif
				if(VIDEO_UNPLUG == au_info.state)
				{
					au_info.state = VIDEO_IN;
					au_info.count = 0;
#if (DEBUG)
					printk(KERN_ERR "2video in, status 0x%x\n", status);
#endif
				}
				else if(VIDEO_LOCKED == au_info.state)
				{
					au_info.state = VIDEO_UNLOCK;
					au_info.count = 0;
#if (DEBUG)
					printk(KERN_ERR "video unstable, status 0x%x\n", status);
#endif
				}
				else
				{
					if(au_info.count < MAX_COUNT)
					{
						au_info.count++;
					}

					if(VIDEO_UNLOCK == au_info.state && au_info.count > 2)
					{
						au_info.state = VIDEO_IN;
						au_info.count = 0;

						if(SCAN_MANUAL != au_info.scan)
						{
							tp9950_reset_default(client, VIDEO_PAGE);
						}
#if (DEBUG)
						printk(KERN_ERR "video unlocked, status 0x%x\n", status);
#endif
					}
				}
			}
		}

		printk(KERN_INFO "%s au_info state video %s\n", __func__,
			(au_info.state < VIDEO_UNLOCK) ? video_in_state[au_info.state]: "Unknow");

		if( VIDEO_IN == au_info.state )
		{
			if(SCAN_MANUAL != au_info.scan)
			{
				cvstd = tp9950_read(client, 0x03);
				cvstd &= 0x07;
				au_info.std= STD_TVI;
				pr_info("%s: CVSTD 0x%02x\n", __func__, cvstd);
				if(cvstd<0x06){
					switch (cvstd & 0x07)
					{
						case TP2802_SD:
							if(status & 0x09)
							{
								au_info.mode = TP2802_PAL;
								au_info.fps = 50;

								pr_err("TP2802_PAL \r\n");//pr_info
							}
							else
							{
								au_info.mode = TP2802_NTSC;

								pr_err("TP2802_NTSC \r\n");
							}
							goto RETURN_MODE;
							break;
						case TP2802_720P25:
							au_info.mode = TP2802_720P25V2;
							au_info.fps = 25;

							pr_err("TP2802_720P25V2 \r\n");
							goto RETURN_MODE;
							break;
						case TP2802_720P30:
							au_info.mode = TP2802_720P30V2;
							au_info.fps = 30;

							pr_err("TP2802_720P30V2 \r\n");
							goto RETURN_MODE;
							break;
						case TP2802_1080P25:
							au_info.mode = TP2802_1080P25;
							au_info.fps = 25;

							pr_err("TP2802_1080P25 \r\n");
							goto RETURN_MODE;
							break;
						case TP2802_1080P30:
							au_info.mode = TP2802_1080P30;
							au_info.fps = 30;

							pr_err("TP2802_1080P30 \r\n");
							goto RETURN_MODE;
							break;
						default:
							pr_err("not support:%d \r\n",cvstd & 0x07);
							break;
					}
				}else{
					unsigned char tmp = 0;
					tp9950_write(client,0x2f,0x09);
					tmp = tp9950_read(client, 0x04);
					/*
					* DONOT know why here 0x2F to 0x09 and read 0x04(Internal Status Register)
					* As datasheet say it is for internal use.
					*/
					pr_info("Reg(0x04): %02x",tmp);
					if (tmp == 0x94){
						au_info.mode = TP2802_PAL;
						au_info.fps = 50;
						pr_info("TP2802_PAL\n");
						goto RETURN_MODE;
					} else if(tmp == 0x93)
					{
						au_info.mode = TP2802_NTSC;
						au_info.fps = 60;
						pr_info("TP2802_NTSC\n");
						goto RETURN_MODE;

					}
				}
			}
		}

#if 0
#define EQ_COUNT 10
		if( VIDEO_LOCKED == au_info.state) //check signal lock
		{
			if(0 == au_info.count)
			{
				tp9950_write(client, 0x40, 0x08);
				tp9950_write(client,0x23, 0x00);

				tp9950_write(client, 0x40, 0x00);
				tmp =  tp9950_read(client, 0x26);
				tmp |= 0x01;
				tp9950_write(client, 0x26, tmp);

				if( (SCAN_AUTO == au_info.scan || SCAN_TVI == au_info.std ))
				{
					if( (TP2802_720P30V2 == au_info.mode) || (TP2802_720P25V2 == au_info.mode) )
					{
						tmp =  tp9950_read(client,  0x03);
#if (DEBUG)
						printk("CVSTD%02x\n", tmp);
#endif
						if( ! (0x08 & tmp) )
						{
#if (DEBUG)
							printk("720P V1 Detected\n");
#endif
							au_info.mode &= 0xf7;

							//TODO
							//tp2802_set_video_mode(iChip, wdi-> mode[i], i, STD_TVI); //to speed the switching
						}
					}

					//these code need to keep bottom
					{

					}
				}
			}
			else if(1 == au_info.count)
			{

				tmp =  tp9950_read(client,  0xa7);
				tmp |= 0x01;
				tp9950_write(client, 0xa7, tmp);
#if (DEBUG)
				tmp =  tp9950_read(client,  0x01);
				printk("status %02x\r\n", tmp);
				tmp =  tp9950_read(client,  0x03);
				printk("CVSTD %02x\r\n", tmp);
#endif
				if(STD_TVI ==  au_info.std  && SCAN_AUTO == au_info.scan )
				{
					tmp = tp9950_read(client, 0x01);

					if(TP2802_PAL == au_info.mode || TP2802_NTSC == au_info.mode)
					{
						//do nothing
					}
					else if(0x60 == (tmp & 0x64) )
					{

	//						UTC_data = tp28xx_byte_read(0x94);
	//						Printf("UTC RX 0x%02x \r\n", (WORD)UTC_data);
	//                             	if     (0xff == UTC_data)                {std[i] = STD_HDC;}
	//                            	 	else if(0x00 == UTC_data)                std[i] = STD_HDC_DEFAULT;
	//                            	 	else                                	 {std[i] = STD_HDA;}
						 au_info.std = STD_HDA;

						//TODO
						//tp2802_set_video_mode(iChip, wdi-> mode[i], i, STD_HDA);
					}
				}
			}
			else if( au_info.count < EQ_COUNT)
			{

			}
			else if( au_info.count == EQ_COUNT)
			{

			}
			else
			{
				if( SCAN_AUTO == au_info.scan)
				{
					tmp = tp9950_read(client, 0x03); //
					tmp &= 0x07;

					if(tmp != (au_info.mode & 0x07) && tmp < TP2802_SD)
					{
 #if (DEBUG)
						printk("correct %02x from %02x\r\n", tmp, au_info.mode);
						//wdi->force[i] = 1;
#endif
					}
				}
			}
#endif
	}

RETURN_MODE:
	return au_info.mode;
}

/*
 * The sensor must set callbacks
 *
 * */
static int tp9950_sensor_init_func(void *arg, const AK_ISP_SENSOR_INIT_PARA *para)
{
	int i;
	int fps=0;
	int num;
	int val;
	int flag = 0;
	AK_ISP_SENSOR_REG_INFO *preg_info;
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;

#if DEBUG	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	tp9950_set_reg_page(client, VIDEO_PAGE);

	//
	//默认设置AHD信号的参数，后期再兼容TVI信号*/
	//
	//
	//get data of ahd
	//
	preg_info = tp9950_get_init_sequence(client, priv, &fps, &num);
	if(!preg_info)
	{
		pr_err("%s:no preg\n",__func__);
		val = tp9950_read(client, 0x01);

		//
		//there is video signal
		//
		if(!(val & FLAG_LOSS)){ 
			//
			//ERROR,so soft reset
			//
			tp9950_soft_reset(client);
		}

		return -1;
	}

	//
	//soft reset
	//
	//tp9950_soft_reset(client);
	tp9950_set_reg_page(client, VIDEO_PAGE);
	for (i = 0; i < num; i++)
	{
		tp9950_write(client, preg_info->reg_addr, preg_info->value);
		preg_info++;
	}

	priv->fps_info.current_fps = fps;
	priv->fps_info.to_fps = priv->fps_info.current_fps;
	priv->fps_info.to_fps_value = 0;

	//
	//delay
	//
	msleep(200);

	tp9950_set_reg_page(client, VIDEO_PAGE);
	i = 0;
	while(1)
	{
		//
		//read reg 0x01 for lock or unlock
		//
		val = tp9950_read(client, 0x01);
		//
		//is signal locked
		//
		if((FLAG_HV_LOCKED & val) == FLAG_HV_LOCKED)
		{
			if((SENSOR_OUTPUT_WIDTH >= 960) || (SENSOR_OUTPUT_HEIGHT >= 720))
			{
				//
				//getting signal type:TVI or AHD 
				//
				val = tp9950_read(client, 0x01);
				if(val & (0x1 << 2))
				{
					pr_info("%s:the signal is TVI\n",__func__);
					pr_err("%s:the signal is TVI\n",__func__);
				}
				else
				{
					pr_info("%s:**the signal is AHD\n",__func__);
					pr_err("%s:**the signal is AHD\n",__func__);
				}
			}

			break;
		}

		//
		//1.5s timeout
		//
		if(i > 15){
			flag = 1;
			break;
		}

		msleep(100);
		i++;
	}


	if(1 == flag){
		pr_err("%s:the video signal is unlocked\n",__func__);
	}else{
		pr_err("%s:the video signal is locked\n",__func__);
		// unsigned short j1,value;
		// printk(KERN_ERR "-----tp9950 reg read----------\n");
		// for(j1 = 0; j1 <= 0xFF; j1++)
		// {
		// 	value = tp9950_read(client,j1);
		// 	printk(KERN_ERR "reg:%02x = %02x\n",j1,value);
		// }
	}

	pr_info("%s:sensor init is ok\n", __func__);
	pr_err("%s:**sensor init is ok\n", __func__);

	return 0;
}

/*read sensor register*/
static int tp9950_sensor_read_reg_func(void *arg, const int reg_addr)
{
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	tp9950_set_reg_page(client, VIDEO_PAGE);
	return tp9950_read(client, reg_addr);
}

/*write sensor register*/
static int tp9950_sensor_write_reg_func(void *arg, const int reg_addr, int value)
{
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	tp9950_set_reg_page(client, VIDEO_PAGE);
	return tp9950_write(client, reg_addr, value);
}

/*read sensor register, NO i2c ops*/
static int tp9950_sensor_read_id_func(void *arg)   //no use IIC bus
{
	struct tp9950_priv *priv = arg;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	return ((SENSOR_ID<<8) | SENSOR_TYPE);
}

/*set sensor again*/
static int tp9950_sensor_update_a_gain_func(void *arg, const unsigned int a_gain)
{
	return 0;
}

/*set sensor dgain*/
static int tp9950_sensor_update_d_gain_func(void *arg, const unsigned int d_gain)
{
	return 0;
}

/*set sensor exp time*/
static int tp9950_sensor_updata_exp_time_func(void *arg, unsigned int exp_time)
{
	return 0;
}

/*sensor timer*/
static int tp9950_sensor_timer_func(void *arg)
{
	return 0;
}

/*standby in*/
static int tp9950_sensor_set_standby_in_func(void *arg)
{
	return 0;
}

/*standby out*/
static int tp9950_sensor_set_standby_out_func(void *arg)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	
	return 0;
}

/*low level read sensor ID, user i2c ops*/
static int __tp9950_sensor_probe_id_func(struct i2c_client *client)  //use IIC bus
{
	int id = 0;
	int value = 0;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	tp9950_set_reg_page(client, VIDEO_PAGE);

	value = tp9950_read(client, 0xfe);
	id = value << 8;
	value = tp9950_read(client, 0xff);
	id |= value;

	if(SENSOR_ID != id) {
		printk(KERN_ERR"Invalid chip 0x%2x\n", id);
		return 0;
	}

	printk(KERN_INFO "Detected TP2850 success\n");
	return ((SENSOR_ID << 8) | SENSOR_TYPE);
}

/*read sensor ID, user i2c ops*/
static int tp9950_sensor_probe_id_func(void *arg)  //use IIC bus
{
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	return __tp9950_sensor_probe_id_func(client);
}

/*get resolution*/
static int tp9950_sensor_get_resolution_func(void *arg, int *width, int *height)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif	

	*width = SENSOR_OUTPUT_WIDTH;
	*height = SENSOR_OUTPUT_HEIGHT;
	return 0;
}

/*get mclk*/
static int tp9950_sensor_get_mclk_func(void *arg)
{
#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	return SENSOR_MCLK;
}

/*get current fps*/
static int tp9950_sensor_get_fps_func(void *arg)
{
	struct tp9950_priv *priv = arg;

#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif
	return priv->fps_info.current_fps;
}

/*get valid coordinate*/
static int tp9950_sensor_get_valid_coordinate_func(void *arg, int *x, int *y)
{
	*x = SENSOR_VALID_OFFSET_X;
	*y = SENSOR_VALID_OFFSET_Y;
	return 0;
}

/*get bus type*/
static enum sensor_bus_type tp9950_sensor_get_bus_type_func(void *arg)
{
	return SENSOR_BUS_TYPE;
}

/*get self definded params*/
static int tp9950_sensor_get_parameter_func(void *arg, int param, void *value)
{
	int ret = 0;
	struct tp9950_priv *priv = arg;

#if (DEBUG)	
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

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
			*((int *)value) = priv->method;
			//		*((int *)value) =  SCAN_METHOD_PROGRESSIVE;
			break;

		default:
			pr_err("%s param:0x%x not support\n", __func__, param);
			ret = -1;
			break;
	}

	return ret;
}

static int tp9950_auto_detect(void *arg)
{
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;
	unsigned int i;
	int ret = -1;

	pr_err("%s %d force = %d\n", __func__, __LINE__, force_output);

	//for(i = 0; i < 8; i++)
	{
		SENSOR_TYPE = tp9950_auto(client);
#if (DEBUG)	   
		pr_info("%s TP9950_SENSOR_TYPE:0x%x\n", __func__,SENSOR_TYPE);
#endif		
		//if(SENSOR_TYPE != INVALID_FORMAT )
		//break;
	}

	ret = tp9950_get_resolution(SENSOR_TYPE);
	if(ret == 0)
	{
		pr_info("%s SENSOR_TYPE:0x%x ,OUTPUT_WIDTH = %d,OUTPUT_HEIGHT=%d\n", 
				__func__,
				SENSOR_TYPE,
				SENSOR_OUTPUT_WIDTH,
				SENSOR_OUTPUT_HEIGHT);
	}

	return 0;
}

static int power_flag = 0;

/*sensor power on*/
static int tp9950_sensor_set_power_on_func(void *arg)
{
	struct tp9950_priv *priv = arg;
	struct i2c_client *client = priv->client;
	unsigned int i;


#if (DEBUG)
	printk(KERN_ERR"%s start\n", __func__);
#endif

	if(power_flag == 0)
	{
		if (priv->gpio_pwdn >= 0) {
			gpio_direction_output(priv->gpio_pwdn, !SENSOR_PWDN_LEVEL);
		}

		//
		//waiting for power is ok
		//
		msleep(100);
		if (priv->gpio_reset >= 0) {
			pr_info("%s Setting reset\n", __func__);

			gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
			msleep(50);
			gpio_direction_output(priv->gpio_reset, SENSOR_RESET_LEVEL);
			msleep(50);
			gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
			msleep(100);

			pr_info("%s gpio_reset %d\n", __func__,priv->gpio_reset);
		}

		power_flag = 1;
	}
	else
	{
		pr_info("%s Power had been on\n", __func__);
	}

	return 0;
}

/*sensor power off*/
static int tp9950_sensor_set_power_off_func(void *arg)
{
	struct tp9950_priv *priv = arg;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

	if (priv->gpio_pwdn >= 0) {
		gpio_direction_output(priv->gpio_pwdn, SENSOR_PWDN_LEVEL);

		gpio_direction_input(priv->gpio_pwdn);
	}

	if (priv->gpio_reset >= 0) {
		gpio_direction_output(priv->gpio_reset, SENSOR_RESET_LEVEL);

		gpio_direction_input(priv->gpio_reset);
	}

	power_flag = 0;

	return 0;
}

/*set sensor fps*/
static int tp9950_sensor_set_fps_func(void *arg, const int fps)
{
#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif
	return 0;
}

static AK_ISP_SENSOR_CB tp9950_cb = {
	.sensor_init_func 					= tp9950_sensor_init_func,
	.sensor_read_reg_func 				= tp9950_sensor_read_reg_func,
	.sensor_write_reg_func 				= tp9950_sensor_write_reg_func,
	.sensor_read_id_func 				= tp9950_sensor_read_id_func,
	.sensor_update_a_gain_func 			= tp9950_sensor_update_a_gain_func,
	.sensor_update_d_gain_func 			= tp9950_sensor_update_d_gain_func,
	.sensor_updata_exp_time_func 		= tp9950_sensor_updata_exp_time_func,
	.sensor_timer_func 					= tp9950_sensor_timer_func,
	.sensor_set_standby_in_func			= tp9950_sensor_set_standby_in_func,
	.sensor_set_standby_out_func 		= tp9950_sensor_set_standby_out_func,
	.sensor_probe_id_func				= tp9950_sensor_probe_id_func,
	.sensor_get_resolution_func 		= tp9950_sensor_get_resolution_func,
	.sensor_get_mclk_func 				= tp9950_sensor_get_mclk_func,
	.sensor_get_fps_func 				= tp9950_sensor_get_fps_func,
	.sensor_get_valid_coordinate_func 	= tp9950_sensor_get_valid_coordinate_func,
	.sensor_get_bus_type_func 			= tp9950_sensor_get_bus_type_func,
	.sensor_get_parameter_func 			= tp9950_sensor_get_parameter_func,
	.sensor_set_power_on_func 			= tp9950_sensor_set_power_on_func,
	.sensor_set_power_off_func 			= tp9950_sensor_set_power_off_func,
	.sensor_set_fps_func 				= tp9950_sensor_set_fps_func,
};

static int sensor_id_func(void)
{
	struct tp9950_priv *priv = p_gpriv;

#if (DEBUG)
	printk(KERN_ERR "%s %d\n", __func__, __LINE__);
#endif

#if 0
	return tp9950_sensor_read_id_func();
#else
	/*
	 * 主要解决前端输入在启动时上电输出信号给AD
	 */
	if(1 == force_check)
	{
		spin_lock(&readid_lock);

#if 1
		if (priv->gpio_reset >= 0) {
#if (DEBUG)			
			pr_info("%s Setting reset\n", __func__);
#endif		
			gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
			msleep(50);
			gpio_direction_output(priv->gpio_reset, SENSOR_RESET_LEVEL);
			msleep(50);
			gpio_direction_output(priv->gpio_reset, !SENSOR_RESET_LEVEL);
			msleep(50);
		}
#endif

		au_info.count 	= 0,
			au_info.force 	= 0,
			au_info.mode 	= INVALID_FORMAT,
			au_info.scan 	= SCAN_AUTO,
			au_info.state 	= VIDEO_UNPLUG,
			au_info.std 	= STD_TVI,

			tp9950_auto_detect(priv);

		spin_unlock(&readid_lock);
	}

	return ((SENSOR_ID<<8) | SENSOR_TYPE);	
#endif
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

