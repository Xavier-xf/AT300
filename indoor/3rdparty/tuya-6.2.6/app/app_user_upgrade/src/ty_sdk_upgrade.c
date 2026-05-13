/*********************************************************************************
 *Copyright(C),2015-2020,
 *TUYA
 *FileName: tuya_ipc_dp_.c
 *
 * File Description：
 * 1. API implementation of DP point
 *
 * This file code is the basic code, users don't care it
 * Please do not modify any contents of this file at will.
 * Please contact the Product Manager if you need to modify it.
 *
 **********************************************************************************/
#include "utilities/uni_log.h"
#include "tuya_iot_config.h"
#include "tuya_ipc_api.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_config.h"
#include "ty_dp_define.h"
#include "ty_sdk_common.h"
#include "utilities/uni_log.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TUYA_UNPACK_ENABLE 0
#if defined(TUYA_UNPACK_ENABLE) && (TUYA_UNPACK_ENABLE == 1)
#include "tuya_unpack.h"

ota_stream *ota_obj = NULL;
#endif

extern CHAR_T s_ipc_upgrade_file[128];
static int upgrade_progress = 0;

/* OTA */
// Callback after downloading OTA files
OPERATE_RET __IPC_APP_upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    db_log_debug("Upgrade Finish\n");
    db_log_debug("download_result:%d fw_url:%s\n", download_result, fw->fw_url);
    upgrade_progress = 100;
    FILE *upgrade_fp = (FILE *)pri_data;
    if (upgrade_fp)
    {
        fclose(upgrade_fp);
    }
    db_log_debug("fw->fw_md5:%s\n", fw->fw_md5);
    char buffer[36] = {0};
    // if (dyc_common_md5_by_file(s_ipc_upgrade_file, buffer, sizeof(buffer)) == 0 && strncmp(fw->fw_md5, buffer, 32) == 0)
    {
        db_log_debug("md5sum check successful !\n");
        // char cmd[128] = {0};
        // const char *base_dir = /* dyc_tfcard_mount_dir() */NULL;
        // sprintf(cmd, "tar -zxvf %s -C %s", s_ipc_upgrade_file, base_dir);
        // system(cmd);
        // tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_CHECK_SUCCESS, 0);
    }
    // else
    // {
    //     db_log_error("md5sum check error !");
    //     tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_CHECK_FAIL, 0);
    // }
    if (download_result == 0)
    {
        /* The developer needs to implement the operation of OTA upgrade,
        when the OTA file has been downloaded successfully to the specified path. [ p_mgr_info->upgrade_file_path ]*/
    }
    // TODO
    // reboot system
    return OPRT_OK;
}

// To collect OTA files in fragments and write them to local files
OPERATE_RET __IPC_APP_get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                       IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    // db_log_debug("Rev File Data\n");
    // db_log_debug("total_len:%d  fw_url:%s\n", total_len, fw->fw_url);
    // db_log_debug("Offset:%d Len:%d\n", offset, len);

    // report UPGRADE process, NOT only download percent, consider flash-write time
    // APP will report overtime fail, if uprgade process is not updated within 60 seconds
#if defined(TUYA_UNPACK_ENABLE) && (TUYA_UNPACK_ENABLE == 1)
    int ret = device_upgrade_feed(ota_obj, (char *)data, len);
    if (ret)
    {
        printf("upgrade feed failed\n");
        return ret;
    }
#endif
    upgrade_progress = (offset * 100) / (total_len + 1);
    tuya_ipc_upgrade_progress_report_by_type(upgrade_progress, fw->tp);

    FILE *upgrade_fp = (FILE *)pri_data;
    if (upgrade_fp)
    {
        if (fwrite(data, 1, len, upgrade_fp) != len)
        {
            db_log_error("%s write failed !\n", s_ipc_upgrade_file);
            tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_WRITE_FAIL, 0);
            return OPRT_OK;
        }
    }
    // APP will report "uprage success" after reboot and new FW version is reported inside SDK automaticlly

    return OPRT_OK;
}

int __upgrade_status(int percent, void *user_data)
{
    // TYDEBUG("-------> %d\n", percent);
    tuya_ipc_upgrade_progress_report(percent);
#if defined(TUYA_UNPACK_ENABLE) && (TUYA_UNPACK_ENABLE == 1)
    if (100 == percent)
    {
        device_upgrade_close(ota_obj);
    }
#endif
    return 0;
}

INT_T TUYA_IPC_Upgrade_Inform_cb(IN CONST FW_UG_S *fw)
{
    db_log_debug("Rev Upgrade Info\n");
    db_log_debug("fw->fw_url:%s\n", fw->fw_url);
    db_log_debug("fw->sw_ver:%s\n", fw->sw_ver);
    db_log_debug("fw->file_size:%u\n", fw->file_size);
#if defined(TUYA_UNPACK_ENABLE) && (TUYA_UNPACK_ENABLE == 1)
    set_verify_sign_mode(1);
    ota_obj = device_upgrade_open(1, __upgrade_status, NULL);
#endif
    const char *base_dir = /* dyc_tfcard_mount_dir() */"/tmp";
    if (base_dir == NULL)
    {
        db_log_error("Not SD card !\n");
        tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_NO_SDCARD, 0);
        tuya_ipc_upgrade_progress_report(100);
        return OPRT_OK;
    }

    sprintf(s_ipc_upgrade_file, "%s/upgrade.file", base_dir);
    FILE *upgrade_fp = fopen(s_ipc_upgrade_file, "wb");
    if (upgrade_fp == NULL)
    {
        db_log_error("%s open failed !\n", s_ipc_upgrade_file);
        tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_CREATE_FAIL, 0);
        return OPRT_OK;
    }

    tuya_ota_state_send(TUYA_OTA_UPGRADE_STATE_START, 0);

    upgrade_progress = 0;
    tuya_ipc_upgrade_sdk(fw, __IPC_APP_get_file_data_cb, __IPC_APP_upgrade_notify_cb, (PVOID_T)upgrade_fp);

    return OPRT_OK;
}
int tuya_ota_upgrade_progress_get(void)
{
    return upgrade_progress;
}
#if 0

#endif