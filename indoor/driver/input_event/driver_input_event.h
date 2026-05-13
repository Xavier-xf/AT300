#ifndef _DIRVER_INPUT_EVENT_H_
#define _DIRVER_INPUT_EVENT_H_

#include "../db_hal_driver.h"

typedef struct
{
    int x;
    int y;
    int key;
    int state; // 0:release,1:press.
} db_input_event_t;

#endif // _DIRVER_INPUT_EVENT_H_
