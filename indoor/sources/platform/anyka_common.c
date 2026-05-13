#ifdef PLATFORM_TYPE_ANYKA
#include "anyka/ak_common_graphics.h"
#include "anyka/ak_common.h"
#include "anyka/ak_tde.h"

void anyka_sdk_init(void)
{
	sdk_run_config config;

	memset(&config, 0, sizeof(sdk_run_config));
	//	config.isp_tool_server_flag = 1;
	ak_sdk_init(&config);
	/***** 打开tde 模块 ******/
	ak_tde_open();

	//	ak_its_start(1234);
}
#endif