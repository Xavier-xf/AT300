#ifndef __TP2850_H__
#define __TP2850_H__

//#include <linux/ioctl.h>

#define TP2850_VERSION_CODE KERNEL_VERSION(0, 0, 3)

enum{
	TP2850 =0x2850,
};

enum{
	TP2802_1080P25 =	    0x03,
	TP2802_1080P30 =	    0x02,
	TP2802_720P25  =	    0x05,
	TP2802_720P30  =    	0x04,
	TP2802_720P50  =	    0x01,
	TP2802_720P60  =    	0x00,
	TP2802_SD      =        0x06,
	INVALID_FORMAT =		0x07,
	TP2802_720P25V2=	    0x0D,
	TP2802_720P30V2=		0x0C,
	TP2802_PAL	   =        0x08,
	TP2802_NTSC	   =    	0x09
};
enum{
    VIDEO_UNPLUG,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};
enum{
    MIPI_2LANE,
    MUX656_8BIT,   //reserved
    SEP656_8BIT    //reserved
};
enum{
    VIDEO_PAGE=0,   //
    MIPI_PAGE=8
};
enum{
    SCAN_DISABLE=0,
    SCAN_AUTO,
    SCAN_TVI,
    SCAN_HDA,
    SCAN_HDC,
    SCAN_MANUAL,
    SCAN_TEST
};
enum{
    STD_TVI,
    STD_HDA,
    STD_HDC,
    STD_HDA_DEFAULT,
    STD_HDC_DEFAULT
};
enum{
    PTZ_TVI=0,
    PTZ_HDA=1,
    PTZ_CVBS=3,
    PTZ_HDC=4
};
enum{
    PTZ_RX_TVI_CMD,
    PTZ_RX_TVI_BURST
};
enum{
    PTZ_PIN_PTZ1=1,
    PTZ_PIN_PTZ2=2
};
enum{
    SINGLE_VIN1=0,
    SINGLE_VIN2=1,
    SINGLE_VIN3=2,
    SINGLE_VIN4=3,
    DIFF_VIN12 =6,
    DIFF_VIN34 =7
};
	
#define FLAG_LOSS         0x80
#define FLAG_H_LOCKED     0x20
#define FLAG_HV_LOCKED    0x60

#define FLAG_HDC_MODE    0x80
#define FLAG_HALF_MODE    0x40
#define FLAG_MEGA_MODE    0x20
#define FLAG_HDA_MODE    0x10

#define CHANNELS_PER_CHIP 	1
#define MAX_CHIPS 			2
#define SUCCESS				0
#define FAILURE				-1

#define BRIGHTNESS			0x10
#define CONTRAST			0x11
#define SATURATION			0x12
#define HUE					0X13
#define SHARPNESS			0X14


#define     MAX_COUNT       0x20

typedef struct _tp2802_register
{
    unsigned char chip;
    unsigned char ch;
    unsigned int reg_addr;
    unsigned int value;
} tp2802_register;

typedef struct _tp2802_work_mode
{
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
} tp2802_work_mode;

typedef struct _tp2802_video_mode
{
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char std;
} tp2802_video_mode;

typedef struct _tp2802_video_loss
{
    unsigned char chip;
    unsigned char ch;
    unsigned char is_lost;
} tp2802_video_loss;

typedef struct _tp2802_image_adjust
{
    unsigned char chip;
    unsigned char ch;
	unsigned int hue;
	unsigned int contrast;
	unsigned int brightness;
	unsigned int saturation;
	unsigned int sharpness;
} tp2802_image_adjust;

typedef struct _tp2802_PTZ_data
{
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char data[16];
} tp2802_PTZ_data;


// IOCTL Definitions
#define TP2802_IOC_MAGIC            'v'

#define TP2802_READ_REG				_IOWR(TP2802_IOC_MAGIC, 1, tp2802_register)
#define TP2802_WRITE_REG			_IOW(TP2802_IOC_MAGIC, 2, tp2802_register)
#define TP2802_SET_VIDEO_MODE		_IOW(TP2802_IOC_MAGIC, 3, tp2802_video_mode)
#define TP2802_GET_VIDEO_MODE	    _IOWR(TP2802_IOC_MAGIC, 4, tp2802_video_mode)
#define TP2802_GET_VIDEO_LOSS	    _IOWR(TP2802_IOC_MAGIC, 5, tp2802_video_loss)
#define TP2802_SET_IMAGE_ADJUST	    _IOW(TP2802_IOC_MAGIC, 6, tp2802_image_adjust)
#define TP2802_GET_IMAGE_ADJUST	    _IOWR(TP2802_IOC_MAGIC, 7, tp2802_image_adjust)
#define TP2802_SET_PTZ_DATA 	    _IOW(TP2802_IOC_MAGIC, 8, tp2802_PTZ_data)
#define TP2802_GET_PTZ_DATA 	    _IOWR(TP2802_IOC_MAGIC, 9, tp2802_PTZ_data)
#define TP2802_SET_SCAN_MODE 	    _IOW(TP2802_IOC_MAGIC, 10, tp2802_work_mode)
#define TP2802_DUMP_REG     	    _IOW(TP2802_IOC_MAGIC, 11, tp2802_register)
#define TP2802_FORCE_DETECT    	    _IOW(TP2802_IOC_MAGIC, 12, tp2802_work_mode)
#define TP2802_SET_SAMPLE_RATE      _IOW(TP2802_IOC_MAGIC, 13, tp2802_audio_samplerate)
#define TP2802_SET_AUDIO_PLAYBACK   _IOW(TP2802_IOC_MAGIC, 14, tp2802_audio_playback)
#define TP2802_SET_AUDIO_DA_VOLUME  _IOW(TP2802_IOC_MAGIC, 15, tp2802_audio_da_volume)
#define TP2802_SET_AUDIO_DA_MUTE    _IOW(TP2802_IOC_MAGIC, 16, tp2802_audio_da_mute)
#define TP2802_SET_BURST_DATA 	    _IOW(TP2802_IOC_MAGIC, 17, tp2802_PTZ_data)
#define TP2802_SET_VIDEO_INPUT 	    _IOW(TP2802_IOC_MAGIC, 18, tp2802_register)
#define TP2802_SET_PTZ_MODE 	    _IOW(TP2802_IOC_MAGIC, 19, tp2802_PTZ_data)

// Function prototypes

//static int tp2802_set_video_mode(unsigned char addr, unsigned char mode, 
//			unsigned char ch, unsigned char std);

//static void tp2802_set_reg_page(unsigned char addr, unsigned char ch);

#endif


