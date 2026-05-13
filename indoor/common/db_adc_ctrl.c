#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include "db_adc_ctrl.h"

#define FILE_PATH_MAX 128

#define READ_BUFFER_LEN 32

#define ADC_READ_DEFAULT_CH 0

#define AK_SAR_ADC_PATH "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"

static int adc_dev_fd = -1;

/*******************************************************************
 * @brief  : 打开adc设备
 * @return  {*}
 * @param {int} channel：通道
 *******************************************************************/
static bool db_adc_device_open(int channel)
{
    if (adc_dev_fd >= 0)
    {
        return false;
    }
    char path[FILE_PATH_MAX] = {0};
    sprintf(path, AK_SAR_ADC_PATH, channel);

    if (!access(path, F_OK))
    {
        adc_dev_fd = open(path, O_RDONLY);
        if (adc_dev_fd < 0)
        {
            printf("\n\n\n\n\n\n  adc device open error![%s]  \n\n\n\n\n\n", path);
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

/*******************************************************************
 * @brief  : 关闭adc设备
 * @return  {*}
 *******************************************************************/
static bool db_adc_device_close(void)
{
    if (adc_dev_fd < 0)
    {
        return false;
    }
    close(adc_dev_fd);
    adc_dev_fd = -1;
    return true;
}

/*******************************************************************
 * @brief  : 读取adc引脚的值，默认是通道0
 * @return  {*}
 *******************************************************************/
int db_adc_value_read(void)
{
    int value = 0;
    char buffer[READ_BUFFER_LEN] = {0};

    if (adc_dev_fd < 0)
    {
        db_adc_device_open(ADC_READ_DEFAULT_CH);
    }
    lseek(adc_dev_fd, 0, SEEK_SET);
    if (read(adc_dev_fd, buffer, READ_BUFFER_LEN) < 0)
    {
        db_adc_device_close();
        printf("\n\n\n\n\n\n  adc value read error! restarting  \n\n\n\n\n\n");
        return -1;
    }
    value = atoi(buffer);

    return value;
}
