/*
 *  Driver for Goodix Touchscreens
 *
 *  Copyright (c) 2014 Red Hat Inc.
 *
 *  This code is based on gt9xx.c authored by andrew@goodix.com:
 *
 *  2010 - 2012 Goodix Technology.
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; version 2 of the License.
 */

#include <linux/kernel.h>
#include <linux/dmi.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/acpi.h>
#include <linux/of.h>
#include <asm/unaligned.h>

#include <linux/firmware.h>
#include <linux/gpio/consumer.h>

struct goodix_ts_data
{
	struct i2c_client *client;
	struct input_dev *input_dev;
	int abs_x_max;
	int abs_y_max;
	unsigned int max_touch_num;
	unsigned int int_trigger_type;
	bool rotated_screen;

	struct gpio_desc *gpiod_int;
	struct gpio_desc *gpiod_rst;
};

#define X_NOM 8
#define X_DEN 10

#define Y_NOM 3
#define Y_DEN 4

#define MAP_X_COORDINATE(x) ((int)((x) * X_NOM / X_DEN))
#define MAP_Y_COORDINATE(y) ((int)((y) * Y_NOM / Y_DEN))


#define GOODIX_MAX_HEIGHT 4096
#define GOODIX_MAX_WIDTH 4096
#define GOODIX_INT_TRIGGER 1
#define GOODIX_CONTACT_SIZE 5
#define GOODIX_MAX_CONTACTS 10

#define GOODIX_CONFIG_MAX_LENGTH 240

/* Register defines */
#define GOODIX_READ_COOR_ADDR 0x00 // 0x814E
#define GOODIX_REG_CONFIG_DATA 0x00
#define GOODIX_REG_ID 0x00

#define RESOLUTION_LOC 1
#define MAX_CONTACTS_LOC 5
#define TRIGGER_LOC 6



static const unsigned long goodix_irq_flags[] = {
    IRQ_TYPE_EDGE_RISING,
    IRQ_TYPE_EDGE_FALLING,
    IRQ_TYPE_LEVEL_LOW,
    IRQ_TYPE_LEVEL_HIGH,
};

/*
 * Those tablets have their coordinates origin at the bottom right
 * of the tablet, as if rotated 180 degrees
 */
static const struct dmi_system_id rotated_screen[] = {
#if defined(CONFIG_DMI) && defined(CONFIG_X86)
    {.ident = "WinBook TW100",
     .matches = {
	 DMI_MATCH(DMI_SYS_VENDOR, "WinBook"),
	 DMI_MATCH(DMI_PRODUCT_NAME, "TW100")}},
    {
	.ident = "WinBook TW700",
	.matches = {DMI_MATCH(DMI_SYS_VENDOR, "WinBook"), DMI_MATCH(DMI_PRODUCT_NAME, "TW700")},
    },
#endif
    {}};

/**
 * goodix_i2c_read - read data from a register of the i2c slave device.
 *
 * @client: i2c device.
 * @reg: the register to read from.
 * @buf: raw write data buffer.
 * @len: length of the buffer to write
 */
static int goodix_i2c_read(struct i2c_client *client,
						   u16 reg, u8 *buf, int len)
{
	struct i2c_msg msgs[2];
	u16 wbuf = cpu_to_be16(reg);
	int ret;

	msgs[0].flags = 0;
	msgs[0].addr = client->addr;
	msgs[0].len = 2;
	msgs[0].buf = (u8 *)&wbuf;

	msgs[1].flags = I2C_M_RD;
	msgs[1].addr = client->addr;
	msgs[1].len = len;
	msgs[1].buf = buf;

	ret = i2c_transfer(client->adapter, msgs, 2);
	return ret < 0 ? ret : (ret != ARRAY_SIZE(msgs) ? -EIO : 0);
}

static int goodix_ts_read_input_report(struct goodix_ts_data *ts, u8 *data)
{
	int touch_num;
	int touch_status;
	int error;
	// int x;
	// int y;

	error = goodix_i2c_read(ts->client, 0, data,
				GOODIX_CONTACT_SIZE + 1);
	if (error)
	{
		dev_err(&ts->client->dev, "I2C transfer error: %d\n", error);
		return error;
	}
	// dev_err(&ts->client->dev, "touch number:%d", data[0]);
	touch_status = data[1] >> 6;
	// dev_err(&ts->client->dev, "touch status:%s", touch_status == 0 ? "Down" : touch_status == 1 ? "Up"
	// 											    : touch_status == 2 ? "CONTACT" : "NO_EVENT");
	// x = ((data[1] & 0x0F) << 8) | data[2];
	// y = ((data[3] & 0x0F) << 8) | data[4];
	// x = MAP_X_COORDINATE(x);
	// y = MAP_Y_COORDINATE(y);

	
	dev_err(&ts->client->dev, "touch point :%d,%d,%d,%d,%d,%d\n", data[0], data[1] ,data[2],data[3],data[4], data[5]);
	return touch_status;

	if (!(data[0] & 0x80))
		return -EAGAIN;

	touch_num = data[0] & 0x0f;
	if (touch_num > ts->max_touch_num)
		return -EPROTO;

	if (touch_num > 1)
	{
		data += 1 + GOODIX_CONTACT_SIZE;
		error = goodix_i2c_read(ts->client,
					GOODIX_READ_COOR_ADDR +
					    1 + GOODIX_CONTACT_SIZE,
					data,
					GOODIX_CONTACT_SIZE * (touch_num - 1));
		if (error)
			return error;

		dev_err(&ts->client->dev, "%d %d %d %d %d", data[0], data[1], data[2], data[3], data[4]);
	}

	return touch_status;
}

static void goodix_ts_report_touch(struct goodix_ts_data *ts, u8 *coor_data)
{
	static int prev_x,prev_y;
	int id = coor_data[0] & 0x0F;
	// int input_x = get_unaligned_le16(&coor_data[1]);
	// int input_y = get_unaligned_le16(&coor_data[3]);
	// int input_w = get_unaligned_le16(&coor_data[4]);
	int input_x = ((coor_data[1] & 0x0F) << 8) | coor_data[2];
	int input_y = ((coor_data[3] & 0x0F) << 8) | coor_data[4];
	int input_w = (coor_data[1] >> 6) == 1 ? 0 : 10;
	input_x = MAP_X_COORDINATE(input_x);
	input_y = MAP_Y_COORDINATE(input_y);
	
	if((coor_data[1] >> 6) == 1)
	{
		prev_x = prev_y = -1;
	}
	else
	{
		if(prev_x != input_x || input_y != input_y)
		{
			prev_x = input_x;
			prev_y = input_y;
		}
		else
		{
			return;
		}
	}
	dev_err(&ts->client->dev, "input_x: %d input_y: %d input_w: %d\n", input_x,input_y, (coor_data[1] >> 6));

	// dev_err(&ts->client->dev, "goodix_ts_report_touch :%d,%d,%d,%d", coor_data[1] ,coor_data[2],coor_data[3],coor_data[4]);
	if (ts->rotated_screen)
	{
		input_x = ts->abs_x_max - input_x;
		input_y = ts->abs_y_max - input_y;
	}

	input_mt_slot(ts->input_dev, id);
	input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, true);
	input_report_abs(ts->input_dev, ABS_MT_POSITION_X, input_x);
	input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, input_y);
	input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, input_w);
	input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, input_w);
}

/**
 * goodix_process_events - Process incoming events
 *
 * @ts: our goodix_ts_data pointer
 *
 * Called when the IRQ is triggered. Read the current device state, and push
 * the input events to the user space.
 */
static void goodix_process_events(struct goodix_ts_data *ts)
{
	u8 point_data[1 + GOODIX_CONTACT_SIZE * GOODIX_MAX_CONTACTS];
	int touch_status;
	int i;

	touch_status = goodix_ts_read_input_report(ts, point_data);
	
		// dev_err(&ts->client->dev, "touch_status: %d\n", touch_status);
	if (touch_status < 0)
		return;

	// for (i = 0; i < touch_num; i++)
	// if(touch_status == 0 || touch_status == 1)
		goodix_ts_report_touch(ts,
				       &point_data[GOODIX_CONTACT_SIZE * i]);

	input_mt_sync_frame(ts->input_dev);
	input_sync(ts->input_dev);
}

/**
 * goodix_ts_irq_handler - The IRQ handler
 *
 * @irq: interrupt number.
 * @dev_id: private data pointer.
 */
static irqreturn_t goodix_ts_irq_handler(int irq, void *dev_id)
{
	static const u8 end_cmd[] = {
	    GOODIX_READ_COOR_ADDR >> 8,
	    GOODIX_READ_COOR_ADDR & 0xff,
	    0};
	struct goodix_ts_data *ts = dev_id;

	goodix_process_events(ts);

	if (i2c_master_send(ts->client, end_cmd, sizeof(end_cmd)) < 0)
		dev_err(&ts->client->dev, "I2C write end_cmd error\n");

	return IRQ_HANDLED;
}

/**
 * goodix_read_config - Read the embedded configuration of the panel
 *
 * @ts: our goodix_ts_data pointer
 *
 * Must be called during probe
 */
static void goodix_read_config(struct goodix_ts_data *ts)
{
	return;

	u8 config[GOODIX_CONFIG_MAX_LENGTH];
	int error;

	error = goodix_i2c_read(ts->client, GOODIX_REG_CONFIG_DATA,
				config,
				GOODIX_CONFIG_MAX_LENGTH);
	if (error)
	{
		dev_warn(&ts->client->dev,
			 "Error reading config (%d), using defaults\n",
			 error);
		ts->abs_x_max = GOODIX_MAX_WIDTH;
		ts->abs_y_max = GOODIX_MAX_HEIGHT;
		ts->int_trigger_type = GOODIX_INT_TRIGGER;
		ts->max_touch_num = GOODIX_MAX_CONTACTS;
		return;
	}

	ts->abs_x_max = get_unaligned_le16(&config[RESOLUTION_LOC]);
	ts->abs_y_max = get_unaligned_le16(&config[RESOLUTION_LOC + 2]);
	ts->int_trigger_type = config[TRIGGER_LOC] & 0x03;
	ts->max_touch_num = config[MAX_CONTACTS_LOC] & 0x0f;
	if (!ts->abs_x_max || !ts->abs_y_max || !ts->max_touch_num)
	{
		dev_err(&ts->client->dev,
			"Invalid config, using defaults\n");
		ts->abs_x_max = GOODIX_MAX_WIDTH;
		ts->abs_y_max = GOODIX_MAX_HEIGHT;
		ts->max_touch_num = GOODIX_MAX_CONTACTS;
	}

	ts->rotated_screen = dmi_check_system(rotated_screen);
	if (ts->rotated_screen)
		dev_dbg(&ts->client->dev,
			"Applying '180 degrees rotated screen' quirk\n");
}

/**
 * goodix_read_version - Read goodix touchscreen version
 *
 * @client: the i2c client
 * @version: output buffer containing the version on success
 * @id: output buffer containing the id on success
 */
static int goodix_read_version(struct i2c_client *client, u16 *version, u16 *id)
{
	return 0;
	int error;
	u8 buf[4];

	error = goodix_i2c_read(client, GOODIX_REG_ID, buf, sizeof(buf));
	if (error)
	{
		dev_err(&client->dev, "read version failed: %d\n", error);
		return error;
	}

	*version = buf[3];
	*id = buf[0] << 8 | buf[1];
	dev_info(&client->dev, "ID %x, version: %x\n", buf[0] << 8 | buf[1], buf[3]);

	return 0;
}

/**
 * goodix_i2c_test - I2C test function to check if the device answers.
 *
 * @client: the i2c client
 */
static int goodix_i2c_test(struct i2c_client *client)
{
	int retry = 0;
	int error;
	u8 test[12] = {0};

	while (retry++ < 2)
	{
		error = goodix_i2c_read(client, 0x02,
					test, 12);
		dev_err(&client->dev, "read:[%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]\n", 
			test[0], test[1], test[2], test[3], 
			test[4], test[5], test[6], test[7], 
			test[8], test[9], test[10], test[11]);
		if (!error)
		{
			dev_err(&client->dev, "ic:%x \n", test);
			return 0;
		}

		dev_err(&client->dev, "i2c test failed attempt %d: %d\n",
			retry, error);
		msleep(20);
	}

	return error;
}

/**
 * goodix_request_input_dev - Allocate, populate and register the input device
 *
 * @ts: our goodix_ts_data pointer
 * @version: device firmware version
 * @id: device ID
 *
 * Must be called during probe
 */
static int goodix_request_input_dev(struct goodix_ts_data *ts, u16 version,
				    u16 id)
{
	int error;

	ts->input_dev = devm_input_allocate_device(&ts->client->dev);
	if (!ts->input_dev)
	{
		dev_err(&ts->client->dev, "Failed to allocate input device.");
		return -ENOMEM;
	}

	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X,
			     0, ts->abs_x_max, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y,
			     0, ts->abs_y_max, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);

	input_mt_init_slots(ts->input_dev, ts->max_touch_num,
			    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);

	ts->input_dev->name = "Goodix Capacitive TouchScreen";
	ts->input_dev->phys = "input/ts";
	ts->input_dev->id.bustype = BUS_I2C;
	ts->input_dev->id.vendor = 0x0416;
	ts->input_dev->id.product = id;
	ts->input_dev->id.version = version;

	error = input_register_device(ts->input_dev);
	if (error)
	{
		dev_err(&ts->client->dev,
			"Failed to register input device: %d", error);
		return error;
	}

	return 0;
}

/**
 * goodix_get_gpio_config - Get GPIO config from ACPI/DT
 *
 * @ts: goodix_ts_data pointer
 */

#define GOODIX_GPIO_INT_NAME "irq"
#define GOODIX_GPIO_RST_NAME "reset"
static int goodix_get_gpio_config(struct goodix_ts_data *ts)
{
	int error;
	struct device *dev;
	struct gpio_desc *gpiod;

	if (!ts->client)
		return -EINVAL;
	dev = &ts->client->dev;

	/* Get the interrupt GPIO pin number */
	gpiod = devm_gpiod_get_optional(dev, GOODIX_GPIO_INT_NAME, GPIOD_IN);
	// printk("%s GOODIX_GPIO_INT_NAME======>>%d\n",__func__,gpiod);
	if (IS_ERR(gpiod))
	{
		error = PTR_ERR(gpiod);
		if (error != -EPROBE_DEFER)
			dev_dbg(dev, "Failed to get %s GPIO: %d\n",
				GOODIX_GPIO_INT_NAME, error);
		return error;
	}

	ts->gpiod_int = gpiod;

	/* Get the reset line GPIO pin number */
	gpiod = devm_gpiod_get_optional(dev, GOODIX_GPIO_RST_NAME, GPIOD_IN);
	// printk("%s GOODIX_GPIO_RST_NAME======>>%d\n",__func__,gpiod);
	if (IS_ERR(gpiod))
	{
		error = PTR_ERR(gpiod);
		if (error != -EPROBE_DEFER)
			dev_dbg(dev, "Failed to get %s GPIO: %d\n",
				GOODIX_GPIO_RST_NAME, error);
		return error;
	}

	ts->gpiod_rst = gpiod;

	return 0;
}
static int goodix_int_sync(struct goodix_ts_data *ts)
{
	int error;

	error = gpiod_direction_output(ts->gpiod_int, 0);
	if (error)
		return error;

	msleep(50); /* T5: 50ms */

	error = gpiod_direction_input(ts->gpiod_int);
	if (error)
		return error;

	return 0;
}

/**
 * goodix_reset - Reset device during power on
 *
 * @ts: goodix_ts_data pointer
 */
static int goodix_reset(struct goodix_ts_data *ts)
{
	int error;

	/* begin select I2C slave addr */
	error = gpiod_direction_output(ts->gpiod_rst, 0);
	if (error)
		return error;

	msleep(20); /* T2: > 10ms */

	/* HIGH: 0x28/0x29, LOW: 0xBA/0xBB */
	error = gpiod_direction_output(ts->gpiod_int, 1);
	if (error)
		return error;

	usleep_range(100, 2000); /* T3: > 100us */

	error = gpiod_direction_output(ts->gpiod_rst, 1);
	if (error)
		return error;

	usleep_range(6000, 10000); /* T4: > 5ms */

	/* end select I2C slave addr */
	error = gpiod_direction_input(ts->gpiod_rst);
	if (error)
		return error;

	error = goodix_int_sync(ts);
	if (error)
		return error;

	return 0;
}

static int goodix_ts_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	printk("%s ================================================>>%d\n",__func__,__LINE__);
	struct goodix_ts_data *ts;
	unsigned long irq_flags;
	int error;
	u16 version_info, id_info;
	client->addr = 0x2e;
	dev_dbg(&client->dev, "I2C Address: 0x%02x\n", client->addr);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		dev_err(&client->dev, "I2C check functionality failed.\n");
		return -ENXIO;
	}

	ts = devm_kzalloc(&client->dev, sizeof(*ts), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	ts->client = client;
	i2c_set_clientdata(client, ts);
	error = goodix_get_gpio_config(ts);
	if (error)
		return error;

	if (ts->gpiod_int && ts->gpiod_rst)
	{
		/* reset the controller */
		error = goodix_reset(ts);
		if (error)
		{
			dev_err(&client->dev, "Controller reset failed.\n");
			return error;
		}
	}

	error = goodix_i2c_test(client);
	if (error)
	{
		dev_err(&client->dev, "I2C communication failure: %d\n", error);
		return error;
	}

	error = goodix_read_version(client, &version_info, &id_info);
	if (error)
	{
		dev_err(&client->dev, "Read version failed.\n");
		return error;
	}

	goodix_read_config(ts);

	error = goodix_request_input_dev(ts, version_info, id_info);
	if (error)
		return error;

	irq_flags = goodix_irq_flags[ts->int_trigger_type] | IRQF_ONESHOT;
	error = devm_request_threaded_irq(&ts->client->dev, client->irq,
					  NULL, goodix_ts_irq_handler,
					  irq_flags, client->name, ts);
	if (error)
	{
		dev_err(&client->dev, "request IRQ pin:%d failed: %d\n", client->irq,error);
		return error;
	}

	return 0;
}

static const struct i2c_device_id goodix_ts_id[] = {
    {"GDIX1001:00", 0},
    {}};
MODULE_DEVICE_TABLE(i2c, goodix_ts_id);

#ifdef CONFIG_ACPI
static const struct acpi_device_id goodix_acpi_match[] = {
    {"GDIX1001", 0},
    {"GDIX1002", 0},
    {}};
MODULE_DEVICE_TABLE(acpi, goodix_acpi_match);
#endif

#ifdef CONFIG_OF
static const struct of_device_id goodix_of_match[] = {
    {.compatible = "leo,chsc5448"},
    {}};
MODULE_DEVICE_TABLE(of, goodix_of_match);
#endif

static struct i2c_driver goodix_ts_driver = {
    .probe = goodix_ts_probe,
    .id_table = goodix_ts_id,
    .driver = {
	.name = "LEO-TS",
	.acpi_match_table = ACPI_PTR(goodix_acpi_match),
	.of_match_table = of_match_ptr(goodix_of_match),
    },
};
module_i2c_driver(goodix_ts_driver);

MODULE_AUTHOR("Benjamin Tissoires <benjamin.tissoires@gmail.com>");
MODULE_AUTHOR("Bastien Nocera <hadess@hadess.net>");
MODULE_DESCRIPTION("Goodix touchscreen driver");
MODULE_LICENSE("GPL v2");
