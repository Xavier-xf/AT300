#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "user/user_app.h"
#ifdef PLATFORM_TYPE_ANYKA
#include "platform/anyka_common.h"
#endif

int main(int argc, char *argv[])
{
#ifdef PLATFORM_TYPE_ANYKA
    anyka_sdk_init();
#endif
    return user_app_main(argc, argv);
}