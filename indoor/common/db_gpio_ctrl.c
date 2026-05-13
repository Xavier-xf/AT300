#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include "db_gpio_ctrl.h"
#include "db_common.h"
#define FILE_PATH_MAX 64

/*******************************************************************
 * @brief  : 打开gpio初始化
 * @return  {int} 0表示成功，-1表示失败
 * @param {int} pin：引脚号（必须为非负整数）
 * @param {gpio_dir_t} dir：GPIO方向
 * @param {bool} pull_enable：使能上下拉
 *******************************************************************/
int db_gpio_open(const int pin, gpio_dir_t dir, bool pull_enable)
{
    int fd = -1;
    char value[FILE_PATH_MAX] = {0};
    char path[FILE_PATH_MAX] = {0};

    // 参数验证
    if (pin < 0 || (dir != GPIO_DIR_IN && dir != GPIO_DIR_OUT))
    {
        db_log_error("gpio%d: Invalid parameters\n", pin);
        return -1;
    }

    // 检查GPIO是否已导出
    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d", pin);
    if (access(path, F_OK) != 0)
    {
        // 导出GPIO
        snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/export");
        fd = open(path, O_WRONLY);
        if (fd < 0)
        {
            db_log_error("gpio%d: Failed to open export file: %s\n", pin, strerror(errno));
            return -1;
        }

        snprintf(value, FILE_PATH_MAX, "%d\n", pin);
        if (write(fd, value, strlen(value)) < 0)
        {
            db_log_error("gpio%d: Failed to export: %s\n", pin, strerror(errno));
            close(fd);
            return -1;
        }
        close(fd);
        fd = -1;

        // 等待文件系统更新
        usleep(10000); // 10ms
    }

    // 设置GPIO方向
    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        db_log_error("gpio%d: Failed to open direction file: %s\n", pin, strerror(errno));
        return -1;
    }

    const char* dir_str = (dir == GPIO_DIR_IN) ? "in" : "out";
    if (write(fd, dir_str, strlen(dir_str)) < 0)
    {
        db_log_error("gpio%d: Failed to set direction to %s: %s\n", pin, dir_str, strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    fd = -1;

    // 设置上下拉
    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d/pull_enable", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        db_log_warn("gpio%d: Failed to open pull_enable file (this may be normal on some platforms): %s\n", pin, strerror(errno));
        return 0; // 上下拉设置失败不一定是致命错误
    }

    const char* pull_str = pull_enable ? "1" : "0";
    if (write(fd, pull_str, 1) < 0)
    {
        db_log_error("gpio%d: Failed to set pull_enable to %s: %s\n", pin, pull_str, strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    db_log_info("gpio%d: Successfully opened, direction: %s, pull_enable: %s\n", pin, dir_str, pull_str);
    return 0;
}

/*******************************************************************
 * @brief  : 关闭gpio
 * @return  {int} 0表示成功，-1表示失败
 * @param {int} pin：引脚号（必须为非负整数）
 *******************************************************************/
int db_gpio_close(const int pin)
{
    int fd = -1;
    char value[FILE_PATH_MAX] = {0};
    char path[FILE_PATH_MAX] = {0};

    // 参数验证
    if (pin < 0)
    {
        db_log_error("gpio: Invalid pin number\n");
        return -1;
    }

    // 检查GPIO是否已导出
    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d", pin);
    if (access(path, F_OK) == 0)
    {
        // 取消导出GPIO
        snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/unexport");
        fd = open(path, O_WRONLY);
        if (fd < 0)
        {
            db_log_error("gpio%d: Failed to open unexport file: %s\n", pin, strerror(errno));
            return -1;
        }

        snprintf(value, FILE_PATH_MAX, "%d\n", pin);
        if (write(fd, value, strlen(value)) < 0)
        {
            db_log_error("gpio%d: Failed to unexport: %s\n", pin, strerror(errno));
            close(fd);
            return -1;
        }
        close(fd);
        db_log_info("gpio%d: Successfully closed\n", pin);
    }

    return 0;
}

/*******************************************************************
 * @brief  : 设置gpio引脚电平，需要先调用gpio_open打开初始化gpio
 * @return  {int} 0表示成功，-1表示失败
 * @param {int} pin：引脚号（必须为非负整数）
 * @param {gpio_level_t} level：GPIO电平值
 *******************************************************************/
int db_gpio_level_set(const int pin, gpio_level_t level)
{
    char path[FILE_PATH_MAX] = {0};
    int gpio_fd = -1;

    // 参数验证
    if (pin < 0 || (level != GPIO_LEVEL_LOW && level != GPIO_LEVEL_HIGH))
    {
        db_log_error("gpio%d: Invalid parameters\n", pin);
        return -1;
    }

    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d/value", pin);
    gpio_fd = open(path, O_WRONLY);
    if (gpio_fd < 0)
    {
        db_log_error("gpio%d: Failed to open value file for writing: %s\n", pin, strerror(errno));
        return -1;
    }

    const char* level_str = (level == GPIO_LEVEL_LOW) ? "0" : "1";
    if (write(gpio_fd, level_str, 1) < 0)
    {
        db_log_error("gpio%d: Failed to set value to %s: %s\n", pin, level_str, strerror(errno));
        close(gpio_fd);
        return -1;
    }

    close(gpio_fd);
    return 0;
}

/*******************************************************************
 * @brief  : 读取gpio引脚电平，需要先调用gpio_open打开初始化gpio
 * @return  {int} 0表示成功，-1表示失败
 * @param {int} pin：引脚号（必须为非负整数）
 * @param {gpio_level_t} *level：保存读取到电平的指针（必须非空）
 *******************************************************************/
int db_gpio_level_get(const int pin, gpio_level_t *level)
{
    char value[FILE_PATH_MAX] = {0};
    char path[FILE_PATH_MAX] = {0};
    int gpio_fd = -1;

    // 参数验证
    if (pin < 0 || level == NULL)
    {
        db_log_error("gpio%d: Invalid parameters\n", pin);
        return -1;
    }

    snprintf(path, FILE_PATH_MAX, "/sys/class/gpio/gpio%d/value", pin);
    gpio_fd = open(path, O_RDONLY);
    if (gpio_fd < 0)
    {
        db_log_error("gpio%d: Failed to open value file for reading: %s\n", pin, strerror(errno));
        return -1;
    }

    // 确保读取到最新值
    lseek(gpio_fd, 0, SEEK_SET);
    if (read(gpio_fd, value, 1) < 0)
    {
        db_log_error("gpio%d: Failed to read value: %s\n", pin, strerror(errno));
        close(gpio_fd);
        return -1;
    }

    close(gpio_fd);

    // 解析电平值
    if (value[0] == '0')
    {
        *level = GPIO_LEVEL_LOW;
    }
    else if (value[0] == '1')
    {
        *level = GPIO_LEVEL_HIGH;
    }
    else
    {
        db_log_error("gpio%d: Invalid value read: %c\n", pin, value[0]);
        return -1;
    }

    return 0;
}
