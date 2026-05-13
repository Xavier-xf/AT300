#ifndef _DB_GPIO_CTRL_H_
#define _DB_GPIO_CTRL_H_

typedef enum
{
    GPIO_DIR_IN = 0,
    GPIO_DIR_OUT = 1
} gpio_dir_t;

typedef enum
{
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

int db_gpio_open(const int pin, gpio_dir_t dir, bool pull_enable);

int db_gpio_close(const int pin);

int db_gpio_level_set(const int pin, gpio_level_t level);

int db_gpio_level_get(const int pin, gpio_level_t *level);

#endif // _DB_GPIO_CTRL_H_