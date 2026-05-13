#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/stat.h>

#include "utilities/uni_log.h"
#include "tuya_iot_config.h"
#include "tuya_ipc_sdk_init.h"
#include "tuya_svc_netmgr_linkage.h"
#include "ty_sdk_common.h"
#include "ty_sdk_call.h"
#include "tal_system.h"
#include "tuya_sdk.h"
#include "tuya_iot_base_api.h"
#include "tuya_devos_utils.h"

// STATIC INT_T s_mode = -1;
// STATIC CHAR_T s_token[30] = {0};

CHAR_T s_raw_path[128] = "/tmp/";

CHAR_T s_ipc_pid[64] = {0};     // Product ID of TUYA device, this is for demo only.
CHAR_T s_ipc_uuid[64] = {0};    // Unique identification of each device//Contact tuya PM/BD for developing devices or BUY more
CHAR_T s_ipc_authkey[64] = {0}; // Authentication codes corresponding to UUID, one machine one code, paired with UUID.
CHAR_T s_ipc_storage[64] = {0}; // Path to save tuya sdk DB files, should be readable, writeable and storable
CHAR_T s_ipc_env[64] = "";
CHAR_T s_ipc_sd_path[128] = "/tmp/tf/";                  // SD card mount directory
CHAR_T s_ipc_upgrade_file[128] = "/tmp/tf/upgrade.file"; // File with path to download file during OTA
CHAR_T s_app_version[64] = "1.2.3";                      // Firemware version displayed on TUYA APP
CHAR_T s_net_device[32] = "eth0";                        // Network device interface name
CHAR_T s_sub_version[64] = "0.0.3";                      // Firemware version displayed on TUYA APP
VOID *v_rev_audio_ringbuffer = NULL;

STATIC TUYA_IPC_SDK_RUN_VAR_S g_sdk_run_info = {0};
extern RING_BUFFER_USER_HANDLE_T s_ring_buffer_handles[E_IPC_STREAM_MAX];
extern VOID TUYA_IPC_Status_Changed_cb(IN TUYA_IPC_STATUS_GROUP_E changed_group, IN CONST TUYA_IPC_STATUS_E status[TUYA_IPC_STATUS_GROUP_MAX]);
extern INT_T TUYA_IPC_sd_status_upload(INT_T status);
extern VOID TUYA_IPC_qrcode_shorturl_cb(CHAR_T *shorturl);
extern INT_T TUYA_IPC_Upgrade_Inform_cb(IN CONST FW_UG_S *fw);
extern VOID TUYA_IPC_Reset_System_CB(GW_RESET_TYPE_E type);
extern VOID TUYA_IPC_Restart_Process_CB(VOID);
extern INT_T TUYA_IPC_Get_MqttStatus();

extern INT_T TUYA_IPC_p2p_event_cb(IN CONST INT_T device, IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args);
extern VOID TUYA_IPC_APP_rev_audio_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_AUDIO_FRAME_T *p_audio_frame);
extern VOID TUYA_IPC_APP_rev_video_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_VIDEO_FRAME_T *p_video_frame);
extern VOID TUYA_IPC_APP_rev_file_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_FILE_DATA_T *p_file_data);
extern VOID TUYA_APP_get_snapshot_cb(IN INT_T device, IN INT_T channel, OUT CHAR_T *snap_addr, OUT INT_T *snap_size);
extern VOID TUYA_APP_ai_result_cb(OUT CHAR_T *result_list, OUT CHAR_T *result_msg, OUT UINT_T msg_time, OUT UINT_T sn, OUT RULE_RESULT_STATE_E state);
extern VOID TUYA_APP_video_msg_cb(OUT CHAR_T *video_msg, OUT UINT_T msg_time, OUT UINT_T sn, OUT RULE_RESULT_STATE_E state);
extern VOID TUYA_IPC_Media_Adapter_Init(TUYA_IPC_SDK_MEDIA_ADAPTER_S *p_media_adatper_info, TUYA_IPC_SDK_MEDIA_STREAM_S *p_media_infos);
extern VOID TUYA_IPC_Media_Stream_Init(TUYA_IPC_SDK_MEDIA_ADAPTER_S *p_media_adatper_info);
extern OPERATE_RET TUYA_APP_Init_Ring_Buffer(CONST IPC_MEDIA_INFO_T *pMediaInfo, INT_T channel);
extern OPERATE_RET TUYA_APP_Put_Frame(RING_BUFFER_USER_HANDLE_T handle, IN CONST MEDIA_FRAME_T *p_frame);

extern VOID TUYA_IPC_upload_all_status(VOID);
extern VOID TUYA_IPC_handle_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S *dp_rev);
extern VOID TUYA_IPC_handle_raw_dp_cmd_objs(IN CONST TY_RECV_RAW_DP_S *dp_rev);
extern VOID TUYA_IPC_handle_dp_query_objs(IN CONST TY_DP_QUERY_S *dp_query);
extern VOID TUYA_IPC_INIT_DP_CONFIG();

static OPERATE_RET tuya_ipc_app_start(IN TUYA_IPC_SDK_RUN_VAR_S *pRunInfo)
{
    if (NULL == pRunInfo)
    {
        PR_ERR("start sdk para is NULL\n");
        return OPRT_INVALID_PARM;
    }

    OPERATE_RET ret = 0;
    STATIC BOOL_T s_ipc_sdk_started = FALSE;
    if (TRUE == s_ipc_sdk_started)
    {
        db_log_info("IPC SDK has started\n");
        return ret;
    }

    memcpy(&g_sdk_run_info, pRunInfo, SIZEOF(TUYA_IPC_SDK_RUN_VAR_S));

    // setup1:init sdk
    TUYA_IPC_ENV_VAR_T env;
    memset(&env, 0, sizeof(TUYA_IPC_ENV_VAR_T));
    strcpy(env.storage_path, pRunInfo->iot_info.cfg_storage_path);
    strcpy(env.product_key, pRunInfo->iot_info.product_key);
    strcpy(env.uuid, pRunInfo->iot_info.uuid);
    strcpy(env.auth_key, pRunInfo->iot_info.auth_key);
    strcpy(env.dev_sw_version, pRunInfo->iot_info.dev_sw_version);
    strcpy(env.dev_serial_num, "tuya_ipc");
    // TODO:raw
    env.dev_raw_dp_cb = pRunInfo->dp_info.raw_dp_cmd_proc;
    env.dev_obj_dp_cb = pRunInfo->dp_info.common_dp_cmd_proc;
    env.dev_dp_query_cb = pRunInfo->dp_info.dp_query;
    env.status_changed_cb = pRunInfo->net_info.ipc_status_change_cb;
    env.upgrade_cb_info.upgrade_cb = pRunInfo->upgrade_info.upgrade_cb;
    env.gw_rst_cb = pRunInfo->iot_info.gw_reset_cb;
    env.gw_restart_cb = pRunInfo->iot_info.gw_restart_cb;
    env.qrcode_active_cb = pRunInfo->qrcode_active_cb;
    env.dev_type = pRunInfo->iot_info.dev_type;
    env.link_type = pRunInfo->net_info.link_type;
    env.ip_mode_type = pRunInfo->net_info.ip_mode_type;
    ret = tuya_ipc_init_sdk(&env);
    if (OPRT_OK != ret)
    {
        PR_ERR("init sdk is error\n");
        return ret;
    }

    // 设置日志等级
    tuya_ipc_set_log_attr(pRunInfo->debug_info.log_level, NULL);

    // setup2: set media adapter
    TUYA_IPC_Media_Adapter_Init(&pRunInfo->media_adatper_info, &pRunInfo->media_info);

    // setup 3: ring buffer 创建。
    ret = TUYA_APP_Init_Ring_Buffer(&pRunInfo->media_info.media_info, 0);
    if (OPRT_OK != ret)
    {
        PR_ERR("create ring buffer is error\n");
        return ret;
    }
    tuya_ipc_ring_buffer_adapter_register_media_source();
#if 0
	//低功耗 优先开启P2P
    if(g_sdk_run_info.quick_start_info.enable)
    {
        pthread_t low_power_p2p_thread_handler;
        int op_ret = pthread_create(&low_power_p2p_thread_handler,NULL, tuya_ipc_sdk_low_power_p2p_init_proc, NULL);
        if(op_ret < 0)
        {
            PR_ERR("create p2p start thread is error\n");
            return -1;
        }
    }
#endif
    ret = tuya_ipc_start_sdk(pRunInfo->net_info.connect_mode, pRunInfo->debug_info.qrcode_token);
    if (OPRT_OK != ret)
    {
        PR_ERR("start sdk is error\n");
        return ret;
    }

    s_ipc_sdk_started = true;
    db_log_info("tuya ipc sdk start is complete\n");
    return ret;
}

static OPERATE_RET __TUYA_IPC_SDK_START(TUYA_IPC_PARING_MODE_E connect_mode, CHAR_T *p_token)
{
    printf("SDK Version:%s\r\n", tuya_ipc_get_sdk_info());
    TUYA_IPC_LINK_TYPE_E link_type = TUYA_IPC_LINK_WIRE;
    TUYA_IPC_SDK_RUN_VAR_S ipc_sdk_run_var = {0};
    memset(&ipc_sdk_run_var, 0, sizeof(ipc_sdk_run_var));

    /*certification information(essential)*/
    strcpy(ipc_sdk_run_var.iot_info.product_key, s_ipc_pid);
    strcpy(ipc_sdk_run_var.iot_info.uuid, s_ipc_uuid);
    strcpy(ipc_sdk_run_var.iot_info.auth_key, s_ipc_authkey);
    strcpy(ipc_sdk_run_var.iot_info.dev_sw_version, s_app_version);
    strcpy(ipc_sdk_run_var.iot_info.cfg_storage_path, s_ipc_storage);
    // normal device
    ipc_sdk_run_var.iot_info.dev_type = NORMAL_POWER_DEV;
    // if needed, change to low power device
    // ipc_sdk_run_var.iot_info.dev_type= LOW_POWER_DEV;

    /*connect mode (essential)*/
    ipc_sdk_run_var.net_info.connect_mode = connect_mode;
    ipc_sdk_run_var.net_info.ipc_status_change_cb = TUYA_IPC_Status_Changed_cb;
    ipc_sdk_run_var.net_info.link_type = link_type;
    ipc_sdk_run_var.net_info.ip_mode_type = TUYA_IPC_IPV4_ONLY;
    printf("MODE:%d  LINK_TYPE:%d IP_MODE_TYPE:%d\r\n", connect_mode, ipc_sdk_run_var.net_info.link_type, ipc_sdk_run_var.net_info.ip_mode_type);
    if (p_token)
    {
        strcpy(ipc_sdk_run_var.debug_info.qrcode_token, p_token);
    }
    /* 0-5, the bigger, the more log */
    ipc_sdk_run_var.debug_info.log_level = 0;
    /*media info (essential)*/
    /* main stream(HD), video configuration*/
    /* NOTE
    FIRST:If the main stream supports multiple video stream configurations, set each item to the upper limit of the allowed configuration.
    SECOND:E_IPC_STREAM_VIDEO_MAIN must exist.It is the data source of SDK.
    please close the E_IPC_STREAM_VIDEO_SUB for only one stream*/
    ipc_sdk_run_var.media_info.media_info.stream_enable[E_IPC_STREAM_VIDEO_MAIN] = TRUE;                  /* Whether to enable local HD video streaming */
    ipc_sdk_run_var.media_info.media_info.video_fps[E_IPC_STREAM_VIDEO_MAIN] = 30;                        /* FPS */
    ipc_sdk_run_var.media_info.media_info.video_gop[E_IPC_STREAM_VIDEO_MAIN] = 30;                        /* GOP */
    ipc_sdk_run_var.media_info.media_info.video_bitrate[E_IPC_STREAM_VIDEO_MAIN] = TUYA_VIDEO_BITRATE_2M; /* Rate limit */
    ipc_sdk_run_var.media_info.media_info.video_width[E_IPC_STREAM_VIDEO_MAIN] = 1920;                    /* Single frame resolution of width*/
    ipc_sdk_run_var.media_info.media_info.video_height[E_IPC_STREAM_VIDEO_MAIN] = 1080;                   /* Single frame resolution of height */
    ipc_sdk_run_var.media_info.media_info.video_freq[E_IPC_STREAM_VIDEO_MAIN] = 90000;                    /* Clock frequency */
    ipc_sdk_run_var.media_info.media_info.video_codec[E_IPC_STREAM_VIDEO_MAIN] = TUYA_CODEC_VIDEO_H264;   /* Encoding format */

    /* substream(HD), video configuration */
    /* Please note that if the substream supports multiple video stream configurations, please set each item to the upper limit of the allowed configuration. */
    ipc_sdk_run_var.media_info.media_info.stream_enable[E_IPC_STREAM_VIDEO_SUB] = TRUE;                    /* Whether to enable local SD video stream */
    ipc_sdk_run_var.media_info.media_info.video_fps[E_IPC_STREAM_VIDEO_SUB] = 30;                          /* FPS */
    ipc_sdk_run_var.media_info.media_info.video_gop[E_IPC_STREAM_VIDEO_SUB] = 30;                          /* GOP */
    ipc_sdk_run_var.media_info.media_info.video_bitrate[E_IPC_STREAM_VIDEO_SUB] = TUYA_VIDEO_BITRATE_512K; /* Rate limit */
    ipc_sdk_run_var.media_info.media_info.video_width[E_IPC_STREAM_VIDEO_SUB] = 640;                       /* Single frame resolution of width */
    ipc_sdk_run_var.media_info.media_info.video_height[E_IPC_STREAM_VIDEO_SUB] = 360;                      /* Single frame resolution of height */
    ipc_sdk_run_var.media_info.media_info.video_freq[E_IPC_STREAM_VIDEO_SUB] = 90000;                      /* Clock frequency */
    ipc_sdk_run_var.media_info.media_info.video_codec[E_IPC_STREAM_VIDEO_SUB] = TUYA_CODEC_VIDEO_H264;     /* Encoding format */

    /* Audio stream configuration.
    Note: The internal P2P preview, cloud storage, and local storage of the SDK are all use E_IPC_STREAM_AUDIO_MAIN data. */
    ipc_sdk_run_var.media_info.media_info.stream_enable[E_IPC_STREAM_AUDIO_MAIN] = TRUE;                    /* Whether to enable local sound collection */
    ipc_sdk_run_var.media_info.media_info.audio_codec[E_IPC_STREAM_AUDIO_MAIN] = TUYA_CODEC_AUDIO_PCM;      /* Encoding format */
    ipc_sdk_run_var.media_info.media_info.audio_sample[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_SAMPLE_8K;     /* Sampling Rate */
    ipc_sdk_run_var.media_info.media_info.audio_databits[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_DATABITS_16; /* Bit width */
    ipc_sdk_run_var.media_info.media_info.audio_channel[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_CHANNEL_MONO; /* channel */
    ipc_sdk_run_var.media_info.media_info.audio_fps[E_IPC_STREAM_AUDIO_MAIN] = 25;                          /* Fragments per second */

    /*local storage (custome whether enable or not)*/
    ipc_sdk_run_var.local_storage_info.enable = 0;
    ipc_sdk_run_var.local_storage_info.max_event_num_per_day = 500;
    ipc_sdk_run_var.local_storage_info.skills = 0; // 0 means default skills   (TUYA_IPC_SKILL_BASIC | TUYA_IPC_SKILL_DELETE_BY_DAY | TUYA_IPC_SKILL_SPEED_PLAY_0Point5 | TUYA_IPC_SKILL_SPEED_PLAY_2 | TUYA_IPC_SKILL_SPEED_PLAY_4 | TUYA_IPC_SKILL_SPEED_PLAY_8)
    // eg support local download
    // ipc_sdk_run_var.local_storage_info.skills = TUYA_IPC_SKILL_DOWNLOAD | TUYA_IPC_SKILL_BASIC | TUYA_IPC_SKILL_DELETE_BY_DAY | TUYA_IPC_SKILL_SPEED_PLAY_0Point5 | TUYA_IPC_SKILL_SPEED_PLAY_2 | TUYA_IPC_SKILL_SPEED_PLAY_4 | TUYA_IPC_SKILL_SPEED_PLAY_8 ;
    ipc_sdk_run_var.local_storage_info.sd_status_cb = TUYA_IPC_sd_status_upload;
    strcpy(ipc_sdk_run_var.local_storage_info.storage_path, s_ipc_sd_path);

    /*cloud_storage_info*/
    ipc_sdk_run_var.cloud_storage_info.enable = 0;
    ipc_sdk_run_var.cloud_storage_info.en_audio_record = 1;
    ipc_sdk_run_var.cloud_storage_info.pre_record_time = 2;

    /*media adapter function (essential)*/
    ipc_sdk_run_var.media_adatper_info.max_stream_client = STREAM_CLIENT_MAX;
    ipc_sdk_run_var.media_adatper_info.live_mode = TRANS_DEFAULT_STANDARD;
    ipc_sdk_run_var.media_adatper_info.media_event_cb = TUYA_IPC_p2p_event_cb;
    ipc_sdk_run_var.media_adatper_info.rev_audio_cb = TUYA_IPC_APP_rev_audio_cb;
    ipc_sdk_run_var.media_adatper_info.rev_video_cb = TUYA_IPC_APP_rev_video_cb;
    ipc_sdk_run_var.media_adatper_info.rev_file_cb = TUYA_IPC_APP_rev_file_cb;
    ipc_sdk_run_var.media_adatper_info.get_snapshot_cb = TUYA_APP_get_snapshot_cb;

    /*event function (essential)*/
    TUYA_ALARM_BITMAP_T alarm_of_events[1] = {0};
    tuya_ipc_event_add_alarm_types(&alarm_of_events[0], E_ALARM_MOTION);
    ipc_sdk_run_var.event_info.enable = 1;
    ipc_sdk_run_var.event_info.max_event = sizeof(alarm_of_events) / sizeof(TUYA_ALARM_BITMAP_T);
    ipc_sdk_run_var.event_info.alarms_of_events = alarm_of_events;
    ipc_sdk_run_var.event_info.alarms_cnt_per_event = 1;

    /*AI detect (custome whether enable or not)*/
    ipc_sdk_run_var.cloud_rule_info.enable = 1;
    ipc_sdk_run_var.cloud_rule_info.on_ai_result_cb = TUYA_APP_ai_result_cb;
    ipc_sdk_run_var.cloud_rule_info.on_video_msg_cb = TUYA_APP_video_msg_cb;

    /*door bell (custome whether enable or not)*/
    ipc_sdk_run_var.video_msg_info.enable = 1;
    ipc_sdk_run_var.video_msg_info.type = MSG_BOTH;
    ipc_sdk_run_var.video_msg_info.msg_duration = 10;

    /*dp function(essential)*/
    ipc_sdk_run_var.dp_info.dp_query = TUYA_IPC_handle_dp_query_objs;
    ipc_sdk_run_var.dp_info.raw_dp_cmd_proc = TUYA_IPC_handle_raw_dp_cmd_objs;
    ipc_sdk_run_var.dp_info.common_dp_cmd_proc = TUYA_IPC_handle_dp_cmd_objs;

    /*upgrade function(essential)*/
    ipc_sdk_run_var.upgrade_info.enable = true;
    ipc_sdk_run_var.upgrade_info.upgrade_cb = TUYA_IPC_Upgrade_Inform_cb;
    strcpy(ipc_sdk_run_var.upgrade_info.upgrade_file, s_ipc_upgrade_file);

    ipc_sdk_run_var.iot_info.gw_reset_cb = TUYA_IPC_Reset_System_CB;
    ipc_sdk_run_var.iot_info.gw_restart_cb = TUYA_IPC_Restart_Process_CB;

    /*QR-active function(essential)*/
    ipc_sdk_run_var.qrcode_active_cb = TUYA_IPC_qrcode_shorturl_cb;

    /* rtc call function(essential), only use in two-way video talk */
    ipc_sdk_run_var.call_info.enable = FALSE;

    OPERATE_RET ret;
    ret = tuya_ipc_app_start(&ipc_sdk_run_var);
    if (ret != 0)
    {
        db_log_info("ipc sdk start fail,please check run parameter，ret=%d\n", ret);
    }
    if (TUYA_IPC_LINK_WIRE == link_type)
    {
        // if only use wire for paring
        tuya_svc_netmgr_linkage_set_default(LINKAGE_TYPE_WIRED);
    }
    return ret;
}

static OPERATE_RET TUYA_APP_Init_Stream_Storage(TUYA_IPC_SDK_LOCAL_STORAGE_S *p_local_storage_info)
{
    STATIC BOOL_T s_stream_storage_inited = FALSE;
    char *mount_path = tuya_ipc_get_sd_mount_path();

    if (s_stream_storage_inited == TRUE)
    {
        db_log_info("The Stream Storage Is Already Inited");
        return OPRT_OK;
    }

    if (p_local_storage_info == NULL)
    {
        db_log_info("Init Stream Storage fail. Param is null");
        return OPRT_INVALID_PARM;
    }
    TUYA_IPC_STORAGE_VAR_T stg_var;
    memset(&stg_var, 0, SIZEOF(TUYA_IPC_STORAGE_VAR_T));
    memcpy(stg_var.base_path, p_local_storage_info->storage_path, SS_BASE_PATH_LEN);
    strncpy(mount_path, p_local_storage_info->storage_path, sizeof(p_local_storage_info->storage_path) - 1);
    stg_var.max_event_per_day = p_local_storage_info->max_event_num_per_day;
    stg_var.sd_status_changed_cb = p_local_storage_info->sd_status_cb;
    stg_var.skills = p_local_storage_info->skills;

    stg_var.album_info.cnt = 0; // Default off
    memcpy(&stg_var.album_info.album_name[0], TUYA_IPC_ALBUM_EMERAGE_FILE, strlen(TUYA_IPC_ALBUM_EMERAGE_FILE));

    db_log_info("Init Stream_Storage SD:%s", p_local_storage_info->storage_path);
    OPERATE_RET ret = tuya_ipc_ss_init(&stg_var);
    if (ret != OPRT_OK)
    {
        PR_ERR("Init Main Video Stream_Storage Fail. %d", ret);
        return OPRT_COM_ERROR;
    }
    // tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_NONE);
    return OPRT_OK;
}

#if defined(ENABLE_CLOUD_RULE) && (ENABLE_CLOUD_RULE == 1)
static OPERATE_RET TUYA_APP_Init_Cloud_Rule(TUYA_IPC_SDK_CLOUD_RULE_S *p_cloud_rule)
{
    tuya_ipc_rule_init((RULE_AI_RESULT_CB)p_cloud_rule->on_ai_result_cb, (RULE_VIDEO_MSG_CB)p_cloud_rule->on_video_msg_cb);
}
#endif

static OPERATE_RET TUYA_APP_Enable_CloudStorage(TUYA_IPC_SDK_CLOUD_STORAGE_S *p_cloud_storage_info)
{
    OPERATE_RET ret;
    ret = tuya_ipc_cloud_storage_init();
    if (ret != OPRT_OK)
    {
        db_log_info("Cloud Storage Init Err! ret :%d", ret);
        return ret;
    }

    if (p_cloud_storage_info->en_audio_record == FALSE)
    {
        tuya_ipc_cloud_storage_set_audio_stat(p_cloud_storage_info->en_audio_record);
        db_log_info("Disable audio record");
    }

    // Set pre-record time ,if needed. default pre-record time:2 seconds
    if (p_cloud_storage_info->pre_record_time >= 0)
    {
        ret = tuya_ipc_cloud_storage_set_pre_record_time(p_cloud_storage_info->pre_record_time);
        db_log_info("Set pre-record time to [%d], [%s]", p_cloud_storage_info->pre_record_time, ret == OPRT_OK ? "success" : "failure");
    }
    return OPRT_OK;
}

GW_UG_INFORM_CB tuya_gw_ug_cb(const FW_UG_S *fw)
{
    db_log_debug("====>>\n");
    return 0;
}

GW_UG_INFORM_CB tuya_pre_gw_ug_cb(const FW_UG_S *fw)
{
    db_log_debug("====>>\n");
    return 0;
}

TY_IOT_CBS_S iot_cbs;

int ty_sdk_main(const tuya_init_config_t *cfg)
{
    INT_T res = -1;

    memset(s_ipc_pid, 0, sizeof(s_ipc_pid));
    memset(s_ipc_uuid, 0, sizeof(s_ipc_uuid));
    memset(s_ipc_authkey, 0, sizeof(s_ipc_authkey));
    memset(s_ipc_storage, 0, sizeof(s_ipc_storage));
    memset(s_ipc_sd_path, 0, sizeof(s_ipc_sd_path));
    memset(s_app_version, 0, sizeof(s_app_version));
    memset(s_net_device, 0, sizeof(s_net_device));

    strcpy(s_ipc_pid, cfg->pid);
    strcpy(s_ipc_uuid, cfg->uuid);
    strcpy(s_ipc_authkey, cfg->key);
    strcpy(s_ipc_storage, cfg->cache_dir);
    strcpy(s_ipc_sd_path, cfg->sd_dir);
    strcpy(s_app_version, cfg->ver);
    strcpy(s_net_device, cfg->net_dev);

    if (access(s_ipc_storage, F_OK) != 0 && mkdir(s_ipc_storage, 0777) < 0)
    {
        db_log_error("tuya cache dir:[%s] create failed!\n", s_ipc_storage);
        return -1;
    }

    printf("\n*******************************************\n"
           "pid:%s\n"
           "uuid:%s\n"
           "key:%s\n"
           "cache_dir:%s\n"
           "sd_dir:%s\n"
           "ver:%s\n"
           "net_dev:%s\n"
           "*******************************************\n",
           s_ipc_pid, s_ipc_uuid, s_ipc_authkey, s_ipc_storage, s_ipc_sd_path, s_app_version, s_net_device);

    TUYA_IPC_INIT_DP_CONFIG();

    res = __TUYA_IPC_SDK_START(PARING_MODE_WIRED, NULL);
    if (res != OPRT_OK)
    {
        return res;
    }

    // TUYA_APP_Enable_Motion_Detect();

#ifdef ENABLE_SHADOW_DEVICE
    rpc_set_buffer_size(65535);
    rpc_server_init(tuya_ipc_rpc_server_func_call_cb);
    tuya_ipc_rpc_server_declare_all();
#endif

    /* whether SDK is connected to MQTT */
    while (TUYA_IPC_Get_MqttStatus() == FALSE)
    {
        tal_system_sleep(1000);
    }
    db_log_debug("tuya_ipc_sdk_mqtt_online_proc is start run\n");
    // 同步服务器时间
    TIME_T time_utc;
    INT_T time_zone;
    do
    {
        // 需要SDK同步到时间后才能开启下面的业务
        res = tuya_ipc_get_service_time_force(&time_utc, &time_zone);

    } while (res != OPRT_OK);

    tuya_event_cmd_send(TUYA_EVENT_CMD_ONLINE_STATUS, 1); // 此时才能获取网络时间
    // TUYA_IPC_av_start();

    if (FALSE == g_sdk_run_info.quick_start_info.enable)
    {
        TUYA_IPC_Media_Stream_Init(&(g_sdk_run_info.media_adatper_info));
    }

    if (g_sdk_run_info.event_info.enable)
    {
        res = tuya_ipc_event_module_init(g_sdk_run_info.event_info.max_event,
                                         g_sdk_run_info.event_info.alarms_of_events, g_sdk_run_info.event_info.alarms_cnt_per_event);
        db_log_info("event module init result is %d\n", res);
    }

    if (g_sdk_run_info.local_storage_info.enable)
    {
        res = TUYA_APP_Init_Stream_Storage(&(g_sdk_run_info.local_storage_info));
        db_log_info("local storage init result is %d\n", res);
    }

    if (g_sdk_run_info.video_msg_info.enable)
    {
        res = TUYA_APP_Enable_Video_Msg(&(g_sdk_run_info.video_msg_info));
        db_log_info("door bell init result is %d\n", res);
    }
#if defined(ENABLE_CLOUD_RULE) && (ENABLE_CLOUD_RULE == 1)
    if (g_sdk_run_info.cloud_rule_info.enable)
    {
        res = TUYA_APP_Init_Cloud_Rule(&(g_sdk_run_info.cloud_rule_info));
        db_log_info("cloud rule init result is %d\n", res);
    }
#endif
    if (g_sdk_run_info.cloud_storage_info.enable)
    {
        res = TUYA_APP_Enable_CloudStorage(&(g_sdk_run_info.cloud_storage_info));
        db_log_info("cloud storage init result is %d\n", res);
    }
#if defined(ENABLE_TUYA_CALL) && (ENABLE_TUYA_CALL == 1)
    if (g_sdk_run_info.call_info.enable)
    {
        TUYA_IPC_call_init();
    }
#endif

    TUYA_IPC_upload_all_status();

    tuya_ipc_upload_skills();
    db_log_debug("tuya_ipc_sdk_mqtt_online_proc is end run\n");

    s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN] = tuya_ipc_ring_buffer_open(0, 0, E_IPC_STREAM_VIDEO_MAIN, E_RBUF_WRITE);
    s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN] = tuya_ipc_ring_buffer_open(0, 0, E_IPC_STREAM_AUDIO_MAIN, E_RBUF_WRITE);

    int size = 320 * 6;
    // v_rev_audio_ringbuffer = dyc_ringbuffer_open(&size);
    // printf("[%s, %d]\n", __FUNCTION__, __LINE__);
    // tuya_version_push("0.11.28");
    // GW_ATTACH_ATTR_T attach_arr[] = {
    //     {
    //         .tp = DEV_NM_ATH_SNGL,
    //         .ver = "0.0.1",
    //     },
    // };
    // strcpy(attach_arr->ver, s_app_version);
    // db_log_debug("tuya_iot_dev_init:%d\n", tuya_iot_dev_update_attachs(CNTSOF(attach_arr), attach_arr));

    return 0;
}

int tuya_realtime_video_put_frame(unsigned char *data, int size, unsigned long long pts)
{
    if (tuya_client_num_get() > 0 && s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN])
    {
        tuya_ipc_ring_buffer_append_data(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN], data, size, data[4] & 0x1f == 0x01 ? E_VIDEO_PB_FRAME : E_VIDEO_I_FRAME, pts);
        return 0;
    }
    return -1;
}

int tuya_realtime_audio_put_frame(unsigned char *data, int size, unsigned long long pts)
{
    if (tuya_client_num_get() > 0 && s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN])
    {
        tuya_ipc_ring_buffer_append_data(s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN], data, size, E_AUDIO_FRAME, pts);
        return 0;
    }
    return -1;
}

void tuya_ring_buffer_clear(void)
{
    tuya_ipc_ring_buffer_clean_user_state_and_buffer(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN]);
    tuya_ipc_ring_buffer_clean_user_state_and_buffer(s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN]);
    tuya_ipc_ring_buffer_get_frame(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN], true);
    tuya_ipc_ring_buffer_get_frame(s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN], true);
}

int tuya_event_cmd_send(int cmd, int arg)
{
    // char data[64] = {0};
    // sprintf(data, "cmd %d arg %d", cmd, arg);
    // return dyc_ui_event_write(0, UI_EVENT_TUYA_CMD, data, strlen(data));
    return 0;
}

int tuya_ota_state_send(int state, int arg)
{
    // char data[64] = {0};
    // sprintf(data, "state %d arg %d", state, arg);
    // return dyc_ui_event_write(0, UI_EVENT_OTA_UPGRADE_STATE, data, strlen(data));
    return 0;
}

int tuya_notify_call_event(int ch, const uint8_t *jpeg_buf, int size)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    tuya_ipc_notify_door_bell_press(jpeg_buf, size, NOTIFICATION_CONTENT_JPEG);
    // tuya_ipc_door_bell_press(DOORBELL_NORMAL, jpeg_buf, size, NOTIFICATION_CONTENT_JPEG);
    // tuya_ipc_dp_report(NULL, TUYA_DP_DOOR_BELL,PROP_STR,"666",1);
    tuya_ipc_notify_alarm(jpeg_buf, size, ch == 0 ? NOTIFICATION_NAME_PASSBY : NOTIFICATION_NAME_CAR, TRUE, NULL);
    return 0;
}

int tuya_notify_alarm_event(int ch, const uint8_t *jpeg_buf, int size)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    tuya_ipc_notify_alarm(jpeg_buf, size, ch == 0 ? NOTIFICATION_NAME_IO_ALARM : NOTIFICATION_NAME_USER_IO, TRUE, NULL);
    return 0;
}

int tuya_notify_motion_event(int ch, const uint8_t *jpeg_buf, int size)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    tuya_ipc_notify_alarm(jpeg_buf, size, NOTIFICATION_NAME_MOTION, TRUE, NULL);
    return 0;
}

int tuya_network_time_sync(struct tm *new_tm)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    unsigned int time_t;
    int time_tone;
    if (tuya_ipc_get_service_time(&time_t, &time_tone) == 0)
    {
        struct tm old_tm;
        tuya_ipc_get_local_time(time_t, new_tm);
        db_time_get(&old_tm);
        if ((new_tm->tm_year == old_tm.tm_year) && (new_tm->tm_mon == old_tm.tm_mon) && (new_tm->tm_mday == old_tm.tm_mday) && (new_tm->tm_hour == old_tm.tm_hour) && (new_tm->tm_min == old_tm.tm_min))
        {
            return -1;
        }
        return 0;
    }
    return -1;
}

int tuya_media_stream_stop(void)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    return tuya_ipc_media_service_pause();
}

/**
 * @brief 递归打印JSON对象，保持原结构
 * @param item: cJSON对象
 * @param depth: 当前深度（用于缩进）
 */
void print_json_structure(const cJSON *item, int depth)
{
    if (item == NULL)
    {
        return;
    }

    // 缩进
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    switch (item->type)
    {
    case cJSON_NULL:
        printf("null");
        break;

    case cJSON_False:
        printf("false");
        break;

    case cJSON_True:
        printf("true");
        break;

    case cJSON_Number:
        if (item->valuedouble == (double)item->valueint)
        {
            printf("%d", item->valueint);
        }
        else
        {
            printf("%f", item->valuedouble);
        }
        break;

    case cJSON_String:
        printf("\"%s\"", item->valuestring);
        break;

    case cJSON_Array:
        printf("[\n");
        {
            cJSON *child = item->child;
            int first = 1;
            while (child)
            {
                if (!first)
                {
                    printf(",\n");
                }
                print_json_structure(child, depth + 1);
                child = child->next;
                first = 0;
            }
            printf("\n");
            for (int i = 0; i < depth; i++)
            {
                printf("  ");
            }
            printf("]");
        }
        break;

    case cJSON_Object:
        printf("{\n");
        {
            cJSON *child = item->child;
            int first = 1;
            while (child)
            {
                if (!first)
                {
                    printf(",\n");
                }
                // 打印键
                for (int i = 0; i < depth + 1; i++)
                {
                    printf("  ");
                }
                printf("\"%s\": ", child->string);
                // 打印值
                print_json_structure(child, depth + 1);
                child = child->next;
                first = 0;
            }
            printf("\n");
            for (int i = 0; i < depth; i++)
            {
                printf("  ");
            }
            printf("}");
        }
        break;

    default:
        printf("unknown type");
        break;
    }
}

/**
 * @brief 使用cJSON内置的打印函数（格式化输出）
 * @param json_str: JSON字符串
 */
void print_with_cjson_formatted(const char *json_str)
{
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }

    char *formatted = cJSON_Print(json);
    if (formatted != NULL)
    {
        printf("Formatted JSON:\n%s\n", formatted);
        free(formatted);
    }

    cJSON_Delete(json);
}

/**
 * @brief 解析并验证JSON字符串
 * @param json_str: 要解析的JSON字符串
 * @return 解析成功的cJSON对象，失败返回NULL
 */
cJSON *parse_and_validate_json(const char *json_str)
{
    if (json_str == NULL || strlen(json_str) == 0)
    {
        printf("Error: Empty JSON string\n");
        return NULL;
    }

    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "JSON Parse Error before: %s\n", error_ptr);
        }
        return NULL;
    }

    return json;
}

/**
 * @brief 获取JSON值的类型名称
 * @param item: cJSON对象
 * @return 类型名称字符串
 */
const char *get_json_type_name(const cJSON *item)
{
    switch (item->type)
    {
    case cJSON_NULL:
        return "NULL";
    case cJSON_False:
        return "False";
    case cJSON_True:
        return "True";
    case cJSON_Number:
        return "Number";
    case cJSON_String:
        return "String";
    case cJSON_Array:
        return "Array";
    case cJSON_Object:
        return "Object";
    case cJSON_Raw:
        return "Raw";
    default:
        return "Unknown";
    }
}

/**
 * @brief 打印JSON结构信息（调试用）
 * @param json: cJSON对象
 */
void print_json_info(const cJSON *json)
{
    printf("=== JSON Structure Info ===\n");
    printf("Root type: %s\n", get_json_type_name(json));

    if (json->type == cJSON_Object || json->type == cJSON_Array)
    {
        int count = 0;
        const cJSON *child = json->child;
        while (child)
        {
            count++;
            child = child->next;
        }
        printf("Child count: %d\n", count);
    }
    printf("===========================\n");
}

int tuya_api_weather_get(void)
{
    if (tuya_online_status_get() == false)
    {
        return -1;
    }
    const char *weather_choose[] = {
        "{                              \
            \"codes\":[                 \
                \"w.currdate\",         \
                \"w.temp\",             \
                \"w.humidity\",         \
                \"w.conditionNum\",     \
                \"w.pressure\",         \
                \"w.realFeel\",         \
                \"w.uvi\",              \
                \"w.sunRise\",          \
                \"w.sunSet\",           \
                \"w.unix\",             \
                \"w.local\",            \
                \"w.windSpeed\",        \
                \"w.windDir\",          \
                \"w.windLevel\",        \
                \"w.aqi\",              \
                \"w.rank/w.quality\",   \
                \"w.pm10\",             \
                \"w.pm25\",             \
                \"w.o3\",               \
                \"w.co\",               \
                \"w.so2\",              \
                \"w.thigh\",            \
                \"w.tlow\",             \
                \"c.area\",             \
                \"c.city\",             \
                \"c.province\",         \
                ]                       \
        }"

    };

    cJSON *forecast = NULL;
    OPERATE_RET ret = tuya_http_gw_ipc_custom_msg("tuya.device.public.data.get", "1.0", *weather_choose, &forecast);

    if (forecast == NULL)
    {
        db_log_error("tuya weather get failed!\n");
        return -1;
    }
    // if (ret)
    // {
    //     printf("get weather fail\n");
    //     return false;
    // }

    // cJSON *data_json = cJSON_GetArrayItem(forecast, 0);
    // if (data_json == NULL)
    // {
    //     printf("%s,%d data_json is NULL\n", __func__, __LINE__);
    //     return false;
    // }
    // // printf("%s  \n", cJSON_Print(cJSON_Parse(cJSON_Print(data_json))));

    // /* 用字符串来解析json， 防止返回的数据发生变化时， 解析错误 */
    // cJSON *wHumidity = cJSON_GetObjectItem(data_json, "w.humidity");
    // cJSON *wPressure = cJSON_GetObjectItem(data_json, "w.pressure");
    // cJSON *wPm10 = cJSON_GetObjectItem(data_json, "w.pm10");
    // cJSON *wPm25 = cJSON_GetObjectItem(data_json, "w.pm25");
    // cJSON *wTemp = cJSON_GetObjectItem(data_json, "w.temp");
    // cJSON *wCond = cJSON_GetObjectItem(data_json, "w.conditionNum");
    // cJSON *wThigh = cJSON_GetObjectItem(data_json, "w.thigh.0");
    // cJSON *wTlow = cJSON_GetObjectItem(data_json, "w.tlow.0");

    // /* 数据不为空时， 才能使用 */
    // if (wHumidity != NULL)
    //     sscanf(cJSON_Print(wHumidity), "%d", &(nm->humidity));

    // if (wPressure != NULL)
    //     sscanf(cJSON_Print(wPressure), "%d", &(nm->pressure));

    // if (wPm10 != NULL)
    //     sscanf(cJSON_Print(wPm10), "%d", &(nm->pm10));

    // if (wPm25 != NULL)
    //     sscanf(cJSON_Print(wPm25), "%d", &(nm->pm25));

    // if (wTemp != NULL)
    //     sscanf(cJSON_Print(wTemp), "%d", &(nm->temp));

    // if (wCond != NULL)
    //     sscanf(cJSON_Print(wCond), "\"%d\"", &(nm->condition));

    // if (wThigh != NULL)
    //     sscanf(cJSON_Print(wThigh), "%d", &(nm->thigh));

    // if (wTlow != NULL)
    //     sscanf(cJSON_Print(wTlow), "%d", &(nm->tlow));
    print_json_info(forecast);
    print_json_structure(forecast, 0);
    printf("\n\n");
    cJSON_Delete(forecast);
    return 0;
}

void tuya_sub_version_report(const char *version)
{
    if (version)
    {
        GW_ATTACH_ATTR_T attach_arr[] = {
            {
                .tp = DEV_ATTACH_MOD_1,
                .ver = "0.0.1",
            },
        };
        strcpy(attach_arr->ver, version);
        db_log_debug("sub version report:%d\n", tuya_iot_dev_update_attachs(CNTSOF(attach_arr), attach_arr));
    }
    else
    {
        GW_ATTACH_ATTR_T attach_arr[] = {
            {
                .tp = DEV_NM_ATH_SNGL,
                .ver = "0.0.1",
            },
        };
        strcpy(attach_arr->ver, s_app_version);
        db_log_debug("main version report:%d\n", tuya_iot_dev_update_attachs(CNTSOF(attach_arr), attach_arr));
    }
}