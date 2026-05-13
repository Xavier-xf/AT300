#ifndef __AK_SENSOR_H
#define __AK_SENSOR_H

#define AKSENSOR_MAGIC 'S'

/*set cmd*/
#define AK_SENSOR_SET_INIT	_IOW(AKSENSOR_MAGIC,  1, AK_ISP_SENSOR_INIT_PARA)

/*get cmd*/
#define AK_SENSOR_GET_MAX_EXP_FOR_FPS	_IOWR(AKSENSOR_MAGIC,  0x100, struct sensor_max_exp_for_fps)

enum sensor_master_or_slave_mode {
	SINGLE_MODE = 0,
	MASTER_MODE,
	SLAVER_MODE,
};

struct sensor_max_exp_for_fps {
	int fps;
	int max_exp;
};

#endif
