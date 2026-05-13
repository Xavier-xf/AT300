#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <signal.h>
#include <sys/prctl.h>

#include "utilities/uni_log.h"
#include "tuya_iot_config.h"
#include "tuya_ipc_api.h"
#include "tuya_ipc_media.h"
#include "tuya_ring_buffer.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_media_stream_event.h"
#include "ty_sdk_common.h"
#include "tal_video_enc.h"
#include "tuya_ipc_album.h"
#include "codec/tuya_g711_utils.h"

#define MAX_SNAPSHOT_BUFFER_SIZE_KB (100) // in KB

#define AUDIO_FRAME_SIZE 640
#define AUDIO_FPS 25
#define VIDEO_BUF_SIZE (1024 * 400) // Maximum frame
#if defined(IPC_CHANNEL_NUM)
#define MAX_IPC_CHANNEL_NUM IPC_CHANNEL_NUM
#else
#define MAX_IPC_CHANNEL_NUM 1
#endif

typedef struct
{
    BOOL_T enabled;
    TRANSFER_VIDEO_CLARITY_TYPE_E live_clarity;
    UINT_T max_users;
    TUYA_CODEC_ID_E p2p_audio_codec;
} TUYA_APP_P2P_MGR;

// typedef struct playback_context
// {
//     UINT_T ch;
//     UINT_T id;
//     UINT64_T start_time;
//     UINT64_T play_time;
//     UINT64_T duration;
//     UINT64_T late_time;
//     BOOL_T finish;
//     player_context_t play_ctx;
// };

extern CHAR_T s_ipc_sd_path[128];
extern VOID *v_rev_audio_ringbuffer;

// static struct playback_context tuya_playback_ctx[STREAM_CLIENT_MAX] =
//     {
//         {.play_ctx = NULL},
//         {.play_ctx = NULL},
//         {.play_ctx = NULL},
//         {.play_ctx = NULL},
// };

static int client_online_num = 0;

STATIC TUYA_APP_P2P_MGR s_p2p_mgr = {0};

// static int tuya_playback_start(UINT_T ch, UINT_T id, UINT64_T start_time, UINT64_T play_time, UINT64_T duration);
// static int tuya_playback_pause(UINT_T ch);
// static int tuya_playback_resume(UINT_T ch);
// static int tuya_playback_stop(UINT_T ch);
// static int tuya_playback_late_timestamp_set(UINT_T ch, UINT64_T timestamp);

STATIC VOID __TUYA_APP_media_frame_TO_trans_video(IN CONST MEDIA_FRAME_T *p_in, INOUT MEDIA_VIDEO_FRAME_T *p_out)
{
    UINT_T codec_type = 0;
    codec_type = (p_in->type & 0xff00) >> 8;
    p_out->video_codec = (codec_type == 0 ? TUYA_CODEC_VIDEO_H264 : TUYA_CODEC_VIDEO_H265);
    p_out->video_frame_type = (p_in->type && 0xff) == E_VIDEO_PB_FRAME ? TUYA_VIDEO_FRAME_PBFRAME : TUYA_VIDEO_FRAME_IFRAME;
    p_out->p_video_buf = p_in->p_buf;
    p_out->buf_len = p_in->size;
    p_out->pts = p_in->pts;
    p_out->timestamp = p_in->timestamp;

    return;
}

STATIC VOID __TUYA_APP_media_frame_TO_trans_audio(IN CONST MEDIA_FRAME_T *p_in, INOUT MEDIA_AUDIO_FRAME_T *p_out)
{
    DEVICE_MEDIA_INFO_T media_info = {0};
    tuya_ipc_media_adapter_get_media_info(0, 0, &media_info);
    p_out->audio_codec = media_info.av_encode_info.audio_codec[E_IPC_STREAM_AUDIO_MAIN];
    p_out->audio_sample = media_info.av_encode_info.audio_sample[E_IPC_STREAM_AUDIO_MAIN];
    p_out->audio_databits = media_info.av_encode_info.audio_databits[E_IPC_STREAM_AUDIO_MAIN];
    p_out->audio_channel = media_info.av_encode_info.audio_channel[E_IPC_STREAM_AUDIO_MAIN];
    p_out->p_audio_buf = p_in->p_buf;
    p_out->buf_len = p_in->size;
    p_out->pts = p_in->pts;
    p_out->timestamp = p_in->timestamp;

    return;
}
STATIC INT_T TUYA_APP_Enable_Speaker_CB(IN BOOL_T enable)
{
    return 0;
}

STATIC VOID __TUYA_APP_ss_pb_event_cb(IN UINT_T pb_idx, IN SS_PB_EVENT_E pb_event, IN PVOID_T args)
{
    db_log_debug("ss pb rev event: %u %d", pb_idx, pb_event);
    if (pb_event == SS_PB_FINISH)
    {
        tuya_ipc_media_playback_send_finish(pb_idx);
    }

    return;
}

STATIC VOID __TUYA_APP_ss_pb_get_video_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_T *p_frame)
{
    MEDIA_VIDEO_FRAME_T video_frame = {0};
    __TUYA_APP_media_frame_TO_trans_video(p_frame, &video_frame);
    tuya_ipc_media_playback_send_video_frame(pb_idx, &video_frame);

    return;
}

STATIC VOID __TUYA_APP_ss_pb_get_audio_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_T *p_frame)
{
    MEDIA_AUDIO_FRAME_T audio_frame = {0};
    __TUYA_APP_media_frame_TO_trans_audio(p_frame, &audio_frame);
    tuya_ipc_media_playback_send_audio_frame(pb_idx, &audio_frame);

    return;
}

STATIC VOID __TUYA_APP_ss_pb_get_video_encrypt_cb(IN UINT_T pb_idx, UINT_T reqId, IN CONST SS_MEDIA_FRAME_WITH_ENCRYPT_T *p_frame)
{
    TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T trans_frame;

    memset(&trans_frame, 0, sizeof(TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T));
    trans_frame.frame_type = p_frame->frame_type;
    trans_frame.p_buf = p_frame->p_buf;
    trans_frame.size = p_frame->size;
    trans_frame.pts = p_frame->pts;
    trans_frame.timestamp = p_frame->timestamp;

    trans_frame.media.video.video_codec = p_frame->media.video.video_codec;
    trans_frame.media.video.frame_rate = p_frame->media.video.frame_rate;
    trans_frame.media.video.video_width = p_frame->media.video.video_width;
    trans_frame.media.video.video_height = p_frame->media.video.video_height;

    trans_frame.encrypt_info.encrypt = p_frame->encrypt_info.encrypt;
    trans_frame.encrypt_info.security_level = p_frame->encrypt_info.security_level;
    memcpy(&trans_frame.encrypt_info.uuid, &p_frame->encrypt_info.uuid, sizeof(trans_frame.encrypt_info.uuid));
    memcpy(&trans_frame.encrypt_info.iv, &p_frame->encrypt_info.iv, sizeof(trans_frame.encrypt_info.iv));
    tuya_ipc_media_playback_send_video_frame_with_encrypt(pb_idx, reqId, (TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T *)&trans_frame);
    return;
}

static void tuya_playback_video_put_frame(const UINT_T ch, UINT_T id, unsigned char *data, int size, unsigned long long pts)
{
#if 0 // 加密
    TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T trans_frame;
    memset(&trans_frame, 0, sizeof(TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T));
    trans_frame.frame_type = data[4] & 0x1f == 0x01 ? E_VIDEO_PB_FRAME : E_VIDEO_I_FRAME;
    trans_frame.p_buf = data;
    trans_frame.size = size;
    trans_frame.pts = pts;
    trans_frame.timestamp = pts;

    trans_frame.media.video.video_codec = TUYA_CODEC_VIDEO_H264;
    trans_frame.media.video.frame_rate = 30;
    trans_frame.media.video.video_width = 640;
    trans_frame.media.video.video_height = 360;

    trans_frame.encrypt_info.encrypt = 0;
    tuya_ipc_media_playback_send_video_frame_with_encrypt(ch,id, (TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T *)&trans_frame);
#else
    MEDIA_VIDEO_FRAME_T video_frame = {0};
    video_frame.video_codec = TUYA_CODEC_VIDEO_H264;
    video_frame.video_frame_type = data[4] & 0x1f == 0x01 ? E_VIDEO_PB_FRAME : E_VIDEO_I_FRAME;
    video_frame.width = 1920;
    video_frame.height = 1080;
    video_frame.fps = 30;
    video_frame.p_video_buf = data;
    video_frame.buf_len = size;
    video_frame.pts = pts;
    video_frame.timestamp = pts;
    tuya_ipc_media_playback_send_video_frame(ch, &video_frame);
#endif
}

static void tuya_playback_audio_put_frame(const UINT_T ch, UINT_T id, unsigned char *data, int size, unsigned long long pts)
{
#if 0 // 加密
    TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T trans_frame;
    memset(&trans_frame, 0, sizeof(TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T));
    trans_frame.frame_type = type;
    trans_frame.p_buf = data;
    trans_frame.size = size;
    trans_frame.pts = pts;
    trans_frame.timestamp = pts;

    trans_frame.media.audio.audio_codec = TUYA_CODEC_AUDIO_PCM;
    trans_frame.media.audio.audio_sample = TUYA_AUDIO_SAMPLE_8K;
    trans_frame.media.audio.audio_databits = TUYA_AUDIO_DATABITS_16;
    trans_frame.media.audio.audio_channel = TUYA_AUDIO_CHANNEL_MONO;

    trans_frame.encrypt_info.encrypt = 0;
    tuya_ipc_media_playback_send_video_frame_with_encrypt(ch,id, (TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T *)&trans_frame);
#else
    MEDIA_AUDIO_FRAME_T audio_frame = {0};
    audio_frame.audio_codec = TUYA_CODEC_AUDIO_PCM;
    audio_frame.audio_sample = TUYA_AUDIO_SAMPLE_8K;
    audio_frame.audio_databits = TUYA_AUDIO_DATABITS_16;
    audio_frame.audio_channel = TUYA_AUDIO_CHANNEL_MONO;

    while (size >= 640)
    {
        audio_frame.p_audio_buf = data;
        audio_frame.buf_len = 640;
        audio_frame.pts = pts;
        audio_frame.timestamp = pts;
        tuya_ipc_media_playback_send_audio_frame(ch, &audio_frame);
        data += 640;
        size -= 640;
        pts += 20;
    }
    if (size > 0)
    {
        audio_frame.p_audio_buf = data;
        audio_frame.buf_len = size;
        audio_frame.pts = pts;
        audio_frame.timestamp = pts;
        tuya_ipc_media_playback_send_audio_frame(ch, &audio_frame);
    }
#endif
}

STATIC VOID __TUYA_APP_ss_pb_get_audio_encrypt_cb(IN UINT_T pb_idx, int reqId, IN CONST SS_MEDIA_FRAME_WITH_ENCRYPT_T *p_frame)
{
    tuya_ipc_media_playback_send_audio_frame_with_encrypt(pb_idx, reqId, (TRANSFER_MEDIA_FRAME_WIHT_ENCRYPT_T *)p_frame);
    return;
}
/* Callback functions for transporting events */
INT_T TUYA_IPC_p2p_event_cb(IN CONST INT_T device, IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args)
{
    int ret = 0;
    db_log_debug("p2p rev event cb=[%d] ", event);
    char buf[16] = {0};
    switch (event)
    {
    case MEDIA_STREAM_LIVE_VIDEO_START:
    {
        if (client_online_num > 1)
        {
            client_online_num++;
            break;
        }
        client_online_num++;
        C2C_TRANS_CTRL_VIDEO_START *parm = (C2C_TRANS_CTRL_VIDEO_START *)args;
        PR_DEBUG("chn[%u] video start", parm->channel);
        tuya_ring_buffer_clear();
        tuya_event_cmd_send(TUYA_EVENT_CMD_VIDEO_START, 0);
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_STOP:
    {
        if (client_online_num > 1)
        {
            client_online_num--;
            break;
        }
        client_online_num--;
        C2C_TRANS_CTRL_VIDEO_STOP *parm = (C2C_TRANS_CTRL_VIDEO_STOP *)args;
        PR_DEBUG("chn[%u] video stop", parm->channel);
        tuya_event_cmd_send(TUYA_EVENT_CMD_VIDEO_STOP, 0);
        break;
    }
    case MEDIA_STREAM_LIVE_AUDIO_START:
    {
        C2C_TRANS_CTRL_AUDIO_START *parm = (C2C_TRANS_CTRL_AUDIO_START *)args;
        PR_DEBUG("chn[%u] audio start", parm->channel);
        break;
    }
    case MEDIA_STREAM_LIVE_AUDIO_STOP:
    {
        C2C_TRANS_CTRL_AUDIO_STOP *parm = (C2C_TRANS_CTRL_AUDIO_STOP *)args;
        PR_DEBUG("chn[%u] audio stop", parm->channel);
        break;
    }
    case MEDIA_STREAM_SPEAKER_START:
    {
        if (client_online_num > 1)
        {
            break;
        }
        PR_DEBUG("enbale audio speaker");
        // TUYA_APP_Enable_Speaker_CB(TRUE);
        // dyc_ringbuffer_ioctl(v_rev_audio_ringbuffer, DYC_RINGBUFFER_FLUSH_CMD, NULL, NULL);
        tuya_event_cmd_send(TUYA_EVENT_CMD_AUDIO_START, 0);
        break;
    }
    case MEDIA_STREAM_SPEAKER_STOP:
    {
        if (client_online_num > 1)
        {
            break;
        }
        PR_DEBUG("disable audio speaker");
        // TUYA_APP_Enable_Speaker_CB(FALSE);
        // dyc_ringbuffer_ioctl(v_rev_audio_ringbuffer, DYC_RINGBUFFER_FLUSH_CMD, NULL, NULL);
        tuya_event_cmd_send(TUYA_EVENT_CMD_AUDIO_STOP, 0);
        break;
    }
    case MEDIA_STREAM_DISPLAY_START:
    {
        PR_DEBUG("enable video display");
        break;
    }
    case MEDIA_STREAM_DISPLAY_STOP:
    {
        PR_DEBUG("disable video display");
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_SEND_PAUSE:
    {
        PR_DEBUG("app pause video");
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_SEND_RESUME:
    {
        PR_DEBUG("app resume video");
        break;
    }
    case MEDIA_STREAM_LIVE_LOAD_ADJUST:
    {
        C2C_TRANS_LIVE_LOAD_PARAM_S *quality = (C2C_TRANS_LIVE_LOAD_PARAM_S *)args;
        PR_DEBUG("live quality %d -> %d", quality->curr_load_level, quality->new_load_level);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_LOAD_ADJUST:
    {
        C2C_TRANS_PB_LOAD_PARAM_S *quality = (C2C_TRANS_PB_LOAD_PARAM_S *)args;
        PR_DEBUG("pb idx:%d quality %d -> %d", quality->client_index, quality->curr_load_level, quality->new_load_level);
        break;
    }
    case MEDIA_STREAM_ABILITY_QUERY:
    {
        C2C_TRANS_QUERY_FIXED_ABI_REQ *pAbiReq;
        pAbiReq = (C2C_TRANS_QUERY_FIXED_ABI_REQ *)args;
        pAbiReq->ability_mask = TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_VIDEO |
                                TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_SPEAKER |
                                TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_MIC;
        break;
    }
    case MEDIA_STREAM_PLAYBACK_QUERY_MONTH_SIMPLIFY:
    {
        C2C_TRANS_QUERY_PB_MONTH_REQ *p = (C2C_TRANS_QUERY_PB_MONTH_REQ *)args;
        db_log_debug("pb query by month: %d-%d\n", p->year, p->month);
#if 1
        OPERATE_RET ret = tuya_ipc_pb_query_by_month(p->ipcChan, p->year, p->month, &(p->day));
        if (OPRT_OK != ret)
        {
            db_log_error("pb query by month: %d-%d ret:%d", p->year, p->month, ret);
        }
#else
        dyc_media_context media = dyc_media_open(s_ipc_sd_path, MEDIA_TYPE_VIDEO, 0);
        struct tm tm = {
            .tm_year = p->year,
            .tm_mon = p->month,
        };
        dyc_media_query_everyday_by_month(media, &tm, 0, &(p->day));
        dyc_media_close(media);
        db_log_debug("pb query get day: %u\n", p->day);
#endif
        break;
    }
    case MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS:
    {
        C2C_TRANS_QUERY_PB_DAY_RESP *pquery = (C2C_TRANS_QUERY_PB_DAY_RESP *)args;
        db_log_debug("pb_ts query by day: idx[%d]%d-%d-%d\n", pquery->channel, pquery->year, pquery->month, pquery->day);
        SS_QUERY_DAY_TS_ARR_T *p_day_ts = NULL;
#if 1
        OPERATE_RET ret = tuya_ipc_pb_query_by_day(pquery->channel, pquery->ipcChan, pquery->year, pquery->month, pquery->day, &p_day_ts);
        if (OPRT_OK != ret)
        {
            db_log_error("pb_ts query by day: %d-%d-%d Fail", pquery->channel, pquery->year, pquery->month, pquery->day);
            break;
        }
#else
        struct tm tm = {
            .tm_year = pquery->year,
            .tm_mon = pquery->month,
            .tm_mday = pquery->day,
        };
        char path[128] = {0};
        int total = 0;
        dyc_media_context media = dyc_media_open(s_ipc_sd_path, MEDIA_TYPE_VIDEO, 1000);
        media_file_info_t *pinfo = dyc_media_query_by_day(media, &tm, 0, &total);
        UINT_T late_timestamp = 0;
        if (total > 0)
        {
            printf("count = %d\n", total);
            p_day_ts = (SS_QUERY_DAY_TS_ARR_T *)malloc(sizeof(SS_QUERY_DAY_TS_ARR_T) + sizeof(SS_FILE_TIME_TS_T) * total);
            if (p_day_ts)
            {
                p_day_ts->file_count = total;
                for (int i = 0; i < total; i++)
                {
                    p_day_ts->file_arr[i].type = 0;
#ifdef PLATFORM_TYPE_X86_64
                    p_day_ts->file_arr[i].start_timestamp = (unsigned int)strtoul(pinfo[i].timestamp, NULL, 10) - 24 * 3600;
#elif PLATFORM_TYPE_SSD20X
                    p_day_ts->file_arr[i].start_timestamp = (unsigned int)strtoul(pinfo[i].timestamp, NULL, 10) - 8 * 3600;
#endif
                    p_day_ts->file_arr[i].end_timestamp = (unsigned int)(p_day_ts->file_arr[i].start_timestamp + strtoul(pinfo[i].duration, NULL, 10));
                    if (late_timestamp < p_day_ts->file_arr[i].end_timestamp)
                        late_timestamp = p_day_ts->file_arr[i].end_timestamp;
                    printf("type:%d start:%d end:%d\n", p_day_ts->file_arr[i].type, p_day_ts->file_arr[i].start_timestamp, p_day_ts->file_arr[i].end_timestamp);
                }
                tuya_playback_late_timestamp_set(pquery->channel, late_timestamp);
            }
        }
        dyc_media_close(media);
#endif
        if (p_day_ts)
        {
            PLAY_BACK_ALARM_INFO_ARR *pResult = (PLAY_BACK_ALARM_INFO_ARR *)malloc(sizeof(PLAY_BACK_ALARM_INFO_ARR) + p_day_ts->file_count * sizeof(PLAY_BACK_ALARM_FRAGMENT));
            if (NULL == pResult)
            {
                db_log_error("%s %d malloc failed \n", __FUNCTION__, __LINE__);
                tuya_ipc_pb_query_free_ts_arr(p_day_ts);
                pquery->alarm_arr = NULL;
                break;
            }
            INT_T i;
            pResult->file_count = p_day_ts->file_count;
            for (i = 0; i < p_day_ts->file_count; i++)
            {
                pResult->file_arr[i].type = p_day_ts->file_arr[i].type;
                pResult->file_arr[i].time_sect.start_timestamp = p_day_ts->file_arr[i].start_timestamp;
                pResult->file_arr[i].time_sect.end_timestamp = p_day_ts->file_arr[i].end_timestamp;
            }
            pquery->alarm_arr = pResult;
            free(p_day_ts);
        }
        else
        {
            pquery->alarm_arr = NULL;
        }
        break;
    }
    case MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS_WITH_ENCRYPT:
    {
        C2C_TRANS_QUERY_PB_DAY_WITH_ENCRYPT_RESP *pquery = (C2C_TRANS_QUERY_PB_DAY_WITH_ENCRYPT_RESP *)args;
        PR_DEBUG("pb_ts query by day: idx[%d]%d-%d-%d", pquery->channel, pquery->year, pquery->month, pquery->day);
        SS_DAY_TS_RESULT_T *p_day_ts = NULL;
        OPERATE_RET ret = tuya_ipc_pb_query_by_day_with_encrypt_info(pquery->channel, pquery->ipcChan, pquery->allow_encrypt, pquery->year, pquery->month, pquery->day, &p_day_ts);
        if (OPRT_OK != ret)
        {
            PR_ERR("pb_ts query by day: %d-%d-%d Fail", pquery->channel, pquery->year, pquery->month, pquery->day);
            break;
        }
        if (p_day_ts)
        {
            printf("%s %d count = %d\n", __FUNCTION__, __LINE__, p_day_ts->file_count);
            int len = sizeof(PLAY_BACK_ALARM_INFO_WITH_ENCRYPT_ARR) + p_day_ts->file_count * sizeof(PLAY_BACK_FILE_INFOS_WITH_ENCRYPT);
            PLAY_BACK_ALARM_INFO_WITH_ENCRYPT_ARR *pResult = (PLAY_BACK_ALARM_INFO_WITH_ENCRYPT_ARR *)malloc(len);
            if (NULL == pResult)
            {
                printf("%s %d malloc failed \n", __FUNCTION__, __LINE__);
                tuya_ipc_pb_query_free_ts_arr_with_encrypt(p_day_ts);
                pquery->alarm_arr = NULL;
                ret = 0;
                return 0;
            }
            memset(pResult, 0, len);

            INT_T i;
            pResult->file_count = p_day_ts->file_count;
            for (i = 0; i < p_day_ts->file_count; i++)
            {
                pResult->file_arr[i].type = p_day_ts->file_arr[i].type;
                pResult->file_arr[i].time_sect.start_timestamp = p_day_ts->file_arr[i].start_timestamp;
                pResult->file_arr[i].time_sect.end_timestamp = p_day_ts->file_arr[i].end_timestamp;
                memcpy(pResult->file_arr[i].uuid, p_day_ts->file_arr[i].enrypt_info.uuid, 32);
                pResult->file_arr[i].encrypt = p_day_ts->file_arr[i].enrypt_info.encrypt;
                memcpy(pResult->file_arr[i].key_hash, p_day_ts->file_arr[i].enrypt_info.key_hash, 16);
            }
            pquery->alarm_arr = pResult;
            tuya_ipc_pb_query_free_ts_arr_with_encrypt(p_day_ts);
        }
        else
        {
            pquery->alarm_arr = NULL;
        }
        break;
    }
    case MEDIA_STREAM_PLAYBACK_START_TS:
    {
        /* Client will bring the start time when playback.
        For the sake of simplicity, only log printing is done. */
        C2C_TRANS_CTRL_PB_START *pParam = (C2C_TRANS_CTRL_PB_START *)args;
        db_log_debug("PB StartTS ch:%d id:%d encrypt:%d playTime:%u [%u %u]\n", pParam->channel, pParam->reqId, pParam->allow_encrypt, pParam->playTime,
                      pParam->time_sect.start_timestamp, pParam->time_sect.end_timestamp);
#if 1
        SS_FILE_TIME_TS_T pb_file_info;
        memset(&pb_file_info, 0x00, sizeof(SS_FILE_TIME_TS_T));
        // memcpy(&pb_file_info, &pParam->time_sect, sizeof(SS_FILE_TIME_TS_T));
        pb_file_info.start_timestamp = pParam->time_sect.start_timestamp;
        pb_file_info.end_timestamp = pParam->time_sect.end_timestamp;
        int ret = tuya_ipc_ss_pb_start_with_encrypt(pParam->channel, pParam->reqId, pParam->allow_encrypt, __TUYA_APP_ss_pb_event_cb,
                                                    __TUYA_APP_ss_pb_get_video_encrypt_cb,
                                                    __TUYA_APP_ss_pb_get_audio_encrypt_cb);

        if (0 != ret)
        {
            printf("%s %d pb_start failed\n", __FUNCTION__, __LINE__);
            tuya_ipc_media_playback_send_finish(pParam->channel);
        }
        else
        {
            if (0 != tuya_ipc_ss_pb_seek_with_reqId(pParam->channel, pParam->reqId, &pb_file_info, pParam->playTime))
            {
                printf("%s %d pb_seek failed\n", __FUNCTION__, __LINE__);
                tuya_ipc_media_playback_send_finish(pParam->channel);
            }
        }
#else
        tuya_playback_start(pParam->channel, pParam->reqId, pParam->time_sect.start_timestamp, pParam->playTime, pParam->time_sect.end_timestamp - pParam->time_sect.start_timestamp);
#endif
        break;
    }
    case MEDIA_STREAM_PLAYBACK_PAUSE:
    {
        C2C_TRANS_CTRL_PB_PAUSE *pParam = (C2C_TRANS_CTRL_PB_PAUSE *)args;
        PR_DEBUG("PB Pause idx:%d", pParam->channel);
#if 1
        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_PAUSE);
#else
        tuya_playback_pause(pParam->channel);
#endif
        break;
    }
    case MEDIA_STREAM_PLAYBACK_RESUME:
    {
        C2C_TRANS_CTRL_PB_RESUME *pParam = (C2C_TRANS_CTRL_PB_RESUME *)args;
        PR_DEBUG("PB Resume idx:%d", pParam->channel);
#if 1
        tuya_ipc_ss_pb_set_status_with_reqId(pParam->channel, SS_PB_RESUME, pParam->reqId);
#else
        tuya_playback_resume(pParam->channel);
#endif
        break;
    }
    case MEDIA_STREAM_PLAYBACK_MUTE:
    {
        C2C_TRANS_CTRL_PB_MUTE *pParam = (C2C_TRANS_CTRL_PB_MUTE *)args;
        PR_DEBUG("PB idx:%d mute", pParam->channel);
#if 1
        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_MUTE);
#else
        tuya_playback_stop(pParam->channel);
#endif
        break;
    }
    case MEDIA_STREAM_PLAYBACK_UNMUTE:
    {
        C2C_TRANS_CTRL_PB_UNMUTE *pParam = (C2C_TRANS_CTRL_PB_UNMUTE *)args;
        db_log_debug("PB idx:%d unmute", pParam->channel);

        tuya_ipc_ss_pb_set_status_with_reqId(pParam->channel, SS_PB_UN_MUTE, pParam->reqId);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_SET_SPEED:
    {
        C2C_TRANS_CTRL_PB_SET_SPEED *pParam = (C2C_TRANS_CTRL_PB_SET_SPEED *)args;
        PR_DEBUG("chn[%u] video set speed[%u]\n", pParam->channel, pParam->speed);
        int ret = tuya_ipc_ss_pb_set_speed_with_reqId(pParam->channel, pParam->reqId, pParam->speed);

        if (0 != ret)
        {
            PR_ERR("%s %d pb set speed failed\n", __FUNCTION__, __LINE__);
            tuya_ipc_p2p_playback_send_finish(pParam->channel);
        }
        break;
    }
    case MEDIA_STREAM_PLAYBACK_STOP:
    {
        C2C_TRANS_CTRL_PB_STOP *pParam = (C2C_TRANS_CTRL_PB_STOP *)args;
        db_log_debug("PB Stop idx:%d", pParam->channel);
        tuya_ipc_ss_pb_stop(pParam->channel);
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_CLARITY_SET:
    {
        C2C_TRANS_LIVE_CLARITY_PARAM_S *pParam = (C2C_TRANS_LIVE_CLARITY_PARAM_S *)args;
        PR_DEBUG("set clarity:%d", pParam->clarity);
        if ((pParam->clarity == TY_VIDEO_CLARITY_STANDARD) || (pParam->clarity == TY_VIDEO_CLARITY_HIGH))
        {
            PR_DEBUG("set clarity:%d OK", pParam->clarity);
            s_p2p_mgr.live_clarity = pParam->clarity;
        }
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_CLARITY_QUERY:
    {
        C2C_TRANS_LIVE_CLARITY_PARAM_S *pParam = (C2C_TRANS_LIVE_CLARITY_PARAM_S *)args;
        pParam->clarity = s_p2p_mgr.live_clarity;
        PR_DEBUG("query larity:%d", pParam->clarity);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_START:
    {
        C2C_TRANS_CTRL_DL_START *pParam = (C2C_TRANS_CTRL_DL_START *)args;
        SS_DOWNLOAD_FILES_TS_T strParm = {0};
        strParm.file_count = pParam->fileNum;
        strParm.dl_start_time = pParam->downloadStartTime;
        strParm.dl_end_time = pParam->downloadEndTime;
        strParm.file_count = pParam->fileNum;
        int len = pParam->fileNum * sizeof(SS_FILE_INFO_T);
        strParm.p_file_info_arr = (SS_FILE_INFO_T *)malloc(len);
        if (strParm.p_file_info_arr == NULL)
        {
            PR_DEBUG("mallocTRANS_DOWNLOAD_START failed");
            break;
        }
        if (pParam->pFileInfo)
        {
            memcpy(strParm.p_file_info_arr, pParam->pFileInfo, len);
        }
        else
        {
            free(strParm.p_file_info_arr);
            PR_DEBUG("TRANS_DOWNLOAD_START p_file_info_arr NULL ");
            break;
        }
        if (OPRT_OK == tuya_ipc_ss_donwload_pre(pParam->channel, &strParm))
        {
            tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_START);
        }
        free(strParm.p_file_info_arr);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_START_WITH_ENCRYPT:
    {
        C2C_TRANS_CTRL_DL_ENCRYPT_START *pParam = (C2C_TRANS_CTRL_DL_ENCRYPT_START *)args;
        SS_DOWNLOAD_FILES_TS_T strParm = {0};
        strParm.dl_start_time = pParam->downloadStartTime;
        strParm.dl_end_time = pParam->downloadEndTime;
        strParm.file_count = pParam->fileNum;
        int arr_size = pParam->fileNum * sizeof(SS_FILE_INFO_T);
        strParm.p_file_info_arr = (SS_FILE_INFO_T *)malloc(arr_size);
        if (strParm.p_file_info_arr == NULL)
        {
            PR_DEBUG("mallocTRANS_DOWNLOAD_START failed");
            break;
        }
        if (pParam->pFileInfo)
        {
            memcpy(strParm.p_file_info_arr, pParam->pFileInfo, arr_size);
        }
        else
        {
            free(strParm.p_file_info_arr);
            PR_DEBUG("TRANS_DOWNLOAD_START p_file_info_arr NULL ");
            break;
        }
        if (OPRT_OK == tuya_ipc_ss_donwload_pre_support_encrypt(pParam->channel, pParam->allow_encrypt, &strParm))
        {
            tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_START);
        }
        free(strParm.p_file_info_arr);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_STOP:
    {
        C2C_TRANS_CTRL_DL_STOP *pParam = (C2C_TRANS_CTRL_DL_STOP *)args;
        tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_STOP);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_PAUSE:
    {
        C2C_TRANS_CTRL_DL_PAUSE *pParam = (C2C_TRANS_CTRL_DL_PAUSE *)args;
        tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_PAUSE);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_RESUME:
    {
        C2C_TRANS_CTRL_DL_RESUME *pParam = (C2C_TRANS_CTRL_DL_RESUME *)args;
        tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_RESUME);
        break;
    }
    case MEDIA_STREAM_DOWNLOAD_CANCLE:
    {
        C2C_TRANS_CTRL_DL_CANCLE *pParam = (C2C_TRANS_CTRL_DL_CANCLE *)args;
        tuya_ipc_ss_download_set_status(pParam->channel, SS_DL_CANCLE);
        break;
    }
    case MEDIA_STREAM_ALBUM_QUERY:
    {
        C2C_QUERY_ALBUM_REQ *pSrcType = (C2C_QUERY_ALBUM_REQ *)args;
        if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName))
        {
        }
        else
        {
            ret = tuya_ipc_stor_album_cb(event, args);
        }
        break;
    }
    case MEDIA_STREAM_ALBUM_DOWNLOAD_START:
    {
        C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START *pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START *)args;
        if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName))
        {
        }
        else
        {
            ret = tuya_ipc_stor_album_cb(event, args);
        }

        break;
    }
    case MEDIA_STREAM_ALBUM_DOWNLOAD_CANCEL:
    {
        C2C_ALBUM_DOWNLOAD_CANCEL *pSrcType = (C2C_ALBUM_DOWNLOAD_CANCEL *)args;
        PR_DEBUG("%s downlaod cancle\n", pSrcType->albumName);
        if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName))
        {
        }
        else
        {
            ret = tuya_ipc_stor_album_cb(event, args);
        }
        break;
    }
    case MEDIA_STREAM_ALBUM_DELETE:
    {
        C2C_CMD_IO_CTRL_ALBUM_DELETE *pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DELETE *)args;
        if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName))
        {
        }
        else
        {
            ret = tuya_ipc_stor_album_cb(event, args);
        }
        break;
    }
    default:
        break;
    }
    return ret;
}

VOID TUYA_IPC_APP_rev_audio_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_AUDIO_FRAME_T *p_audio_frame)
{
    MEDIA_FRAME_T audio_frame = {0};
    audio_frame.p_buf = p_audio_frame->p_audio_buf;
    audio_frame.size = p_audio_frame->buf_len;

    printf("Rev Audio. size:[%u] audio_codec:[%d] audio_sample:[%d] audio_databits:[%d] audio_channel:[%d]\n", p_audio_frame->buf_len,
           p_audio_frame->audio_codec, p_audio_frame->audio_sample, p_audio_frame->audio_databits, p_audio_frame->audio_channel);

    // unsigned int pcm_8k_size = 0;
    // static unsigned char *pcm_8k_buffer = NULL;
    // if (pcm_8k_buffer == NULL)
    // {
    //     pcm_8k_buffer = (unsigned char *)malloc(p_audio_frame->buf_len * 2);
    // }
    // tuya_g711_decode(TUYA_G711_MU_LAW, (short unsigned int *)p_audio_frame->p_audio_buf, p_audio_frame->buf_len, pcm_8k_buffer, &pcm_8k_size);
    // unsigned int free_length = 0;
    // dyc_ringbuffer_ioctl(v_rev_audio_ringbuffer, DYC_RINGBUFFER_FREE_GET_CMD, NULL, &free_length);
    // if (free_length < pcm_8k_size)
    // {
    //     dyc_ringbuffer_ioctl(v_rev_audio_ringbuffer, DYC_RINGBUFFER_FLUSH_CMD, NULL, NULL);
    // }
    // dyc_ringbuffer_write(v_rev_audio_ringbuffer, pcm_8k_buffer, pcm_8k_size);

    // static FILE *fp = NULL;
    // if(fp == NULL)
    // {
    //     fp = fopen("/tmp/audio.pcm", "wb");
    // }
    // fwrite(pcm_8k_buffer, 1, pcm_8k_size, fp);
    return;
}

VOID TUYA_IPC_APP_rev_video_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_VIDEO_FRAME_T *p_video_frame)
{
    PR_INFO("Rev video. size:[%u] video_codec:[%d] video_frame_type:[%d]\n", p_video_frame->buf_len, p_video_frame->video_codec, p_video_frame->video_frame_type);
    return;
}

VOID TUYA_IPC_APP_rev_file_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_FILE_DATA_T *p_file_data)
{
    return;
}

VOID TUYA_APP_get_snapshot_cb(IN INT_T device, IN INT_T channel, OUT CHAR_T *snap_addr, OUT INT_T *snap_size)
{
    // in this demo ignore device and channel parameter
    IPC_APP_get_snapshot(snap_addr, snap_size);
}

VOID TUYA_IPC_Media_Adapter_Init(TUYA_IPC_SDK_MEDIA_ADAPTER_S *p_media_adatper_info, TUYA_IPC_SDK_MEDIA_STREAM_S *p_media_infos)
{
    TUYA_IPC_MEDIA_ADAPTER_VAR_T media_var;
    media_var.get_snapshot_cb = p_media_adatper_info->get_snapshot_cb;
    media_var.on_recv_audio_cb = p_media_adatper_info->rev_audio_cb;
    media_var.on_recv_video_cb = p_media_adatper_info->rev_video_cb;
    media_var.on_recv_file_cb = p_media_adatper_info->rev_file_cb;
    media_var.available_media_memory = 0;

    tuya_ipc_media_adapter_init(&media_var);

    DEVICE_MEDIA_INFO_T device_media_info = {0};
    memcpy(&device_media_info.av_encode_info, &p_media_infos->media_info, sizeof(IPC_MEDIA_INFO_T));

    device_media_info.audio_decode_info.enable = 1;
    device_media_info.audio_decode_info.audio_codec = TUYA_CODEC_AUDIO_G711U;
    device_media_info.audio_decode_info.audio_sample = TUYA_AUDIO_SAMPLE_8K;
    device_media_info.audio_decode_info.audio_databits = TUYA_AUDIO_DATABITS_16;
    device_media_info.audio_decode_info.audio_channel = TUYA_AUDIO_CHANNEL_MONO;

    device_media_info.max_pic_len = MAX_SNAPSHOT_BUFFER_SIZE_KB;

#ifdef VIDEO_DECODE // 上报视频解码能力，用于双向视频功能
    TUYA_DECODER_T video_decoder = {0};
    video_decoder.codec_id = TUYA_CODEC_VIDEO_H264;
    video_decoder.decoder_desc.v_decoder.height = 320;
    video_decoder.decoder_desc.v_decoder.width = 240;
    video_decoder.decoder_desc.v_decoder.profile = VIDEO_AVC_PROFILE_BASE_LINE;
    device_media_info.decoder_cnt = 1;
    device_media_info.decoders = &video_decoder;
#endif
    tuya_ipc_media_adapter_set_media_info(0, 0, device_media_info);

    return;
}

VOID TUYA_IPC_Media_Stream_Init(TUYA_IPC_SDK_MEDIA_ADAPTER_S *p_media_adatper_info)
{
    MEDIA_STREAM_VAR_T media_stream_var = {0};
    media_stream_var.on_event_cb = p_media_adatper_info->media_event_cb;
    media_stream_var.max_client_num = p_media_adatper_info->max_stream_client;
    media_stream_var.def_live_mode = p_media_adatper_info->live_mode;
#ifdef VIDEO_DECODE // 增加接收缓存区大小，用于双向视频功能
    media_stream_var.recv_buffer_size = 300 * 1024;
#else
    media_stream_var.recv_buffer_size = 16 * 1024;
#endif
    int ret = tuya_ipc_media_stream_init(&media_stream_var);
    PR_DEBUG("media stream init result is %d\n", ret);

    return;
}

/*
---------------------------------------------------------------------------------
code related RingBuffer
---------------------------------------------------------------------------------
*/
IPC_MEDIA_INFO_T old_mediaInfo = {0};
RING_BUFFER_USER_HANDLE_T g_a_handle[MAX_IPC_CHANNEL_NUM] = {NULL};
RING_BUFFER_USER_HANDLE_T g_v_handle[MAX_IPC_CHANNEL_NUM] = {NULL};
RING_BUFFER_USER_HANDLE_T g_v_handle_sub[MAX_IPC_CHANNEL_NUM] = {NULL};
STATIC BOOL_T s_ring_buffer_inited[MAX_IPC_CHANNEL_NUM] = {FALSE};

OPERATE_RET TUYA_APP_Init_Ring_Buffer(CONST IPC_MEDIA_INFO_T *pMediaInfo, INT_T channel)
{
    if (NULL == pMediaInfo)
    {
        PR_ERR("create ring buffer para is NULL\n");
        return -1;
    }
    OPERATE_RET ret = OPRT_OK;

    if (s_ring_buffer_inited[channel] == TRUE)
    {
        PR_DEBUG("The Ring Buffer Is Already Inited");
        return OPRT_OK;
    }

    IPC_STREAM_E ringbuffer_stream_type;
    // CHANNEL_E channel;
    RING_BUFFER_INIT_PARAM_T param = {0};
    for (ringbuffer_stream_type = E_IPC_STREAM_VIDEO_MAIN; ringbuffer_stream_type < E_IPC_STREAM_MAX; ringbuffer_stream_type++)
    {
        PR_DEBUG("init ring buffer Channel:%d Stream:%d Enable:%d", channel, ringbuffer_stream_type, pMediaInfo->stream_enable[ringbuffer_stream_type]);
        if (pMediaInfo->stream_enable[ringbuffer_stream_type] == TRUE)
        {
            if (ringbuffer_stream_type == E_IPC_STREAM_AUDIO_MAIN)
            {
                param.bitrate = pMediaInfo->audio_sample[E_IPC_STREAM_AUDIO_MAIN] * pMediaInfo->audio_databits[E_IPC_STREAM_AUDIO_MAIN] / 1024;
                param.fps = pMediaInfo->audio_fps[E_IPC_STREAM_AUDIO_MAIN];
                param.max_buffer_seconds = 0;
                param.request_key_frame_cb = NULL;
                PR_DEBUG("audio_sample %d, audio_databits %d, audio_fps %d", pMediaInfo->audio_sample[E_IPC_STREAM_AUDIO_MAIN], pMediaInfo->audio_databits[E_IPC_STREAM_AUDIO_MAIN], pMediaInfo->audio_fps[E_IPC_STREAM_AUDIO_MAIN]);
                ret = tuya_ipc_ring_buffer_init(0, channel, ringbuffer_stream_type, &param);
            }
            else
            {
                param.bitrate = pMediaInfo->video_bitrate[ringbuffer_stream_type];
                param.fps = pMediaInfo->video_fps[ringbuffer_stream_type];
                param.max_buffer_seconds = 0;
                param.request_key_frame_cb = NULL;
                PR_DEBUG("video_bitrate %d, video_fps %d", pMediaInfo->video_bitrate[ringbuffer_stream_type], pMediaInfo->video_fps[ringbuffer_stream_type]);
                ret = tuya_ipc_ring_buffer_init(0, channel, ringbuffer_stream_type, &param);
            }
            if (ret != 0)
            {
                PR_ERR("init ring buffer fails. %d %d", ringbuffer_stream_type, ret);
                return OPRT_MALLOC_FAILED;
            }
            PR_DEBUG("init ring buffer success. channel:%d", ringbuffer_stream_type);
        }
    }

    memcpy(&old_mediaInfo, pMediaInfo, sizeof(IPC_MEDIA_INFO_T));
    s_ring_buffer_inited[channel] = TRUE;

    return OPRT_OK;
}

OPERATE_RET TUYA_APP_Put_Frame(RING_BUFFER_USER_HANDLE_T handle, IN CONST MEDIA_FRAME_T *p_frame)
{
    PR_TRACE("Put Frame. type:%d size:%u pts:%llu ts:%llu",
             p_frame->type, p_frame->size, p_frame->pts, p_frame->timestamp);

    OPERATE_RET ret = tuya_ipc_ring_buffer_append_data(handle, p_frame->p_buf, p_frame->size, p_frame->type, p_frame->pts);

    if (ret != OPRT_OK)
    {
        PR_ERR("Put Frame Fail.%d  type:%d size:%u pts:%llu ts:%llu", ret,
               p_frame->type, p_frame->size, p_frame->pts, p_frame->timestamp);
    }
    return ret;
}

int IPC_APP_get_snapshot(char *snap_addr, int *snap_size)
{
    TAL_VENC_FRAME_T frame = {0};
    if (snap_addr == NULL || snap_size == NULL || *snap_size <= 0)
    {
        PR_ERR("parm is wrong\n");
        return OPRT_COM_ERROR;
    }

    frame.pbuf = snap_addr;
    frame.buf_size = *snap_size;
    OPERATE_RET ret = tal_venc_get_frame(0, 4, &frame);
    if (ret != OPRT_OK)
    {
        PR_ERR("get pic failed\n", ret);
        return OPRT_COM_ERROR;
    }
    *snap_size = frame.used_size;
    PR_INFO("get pic suc, size:%d", frame.used_size);

    return 0;
}
#if 0
static void tuya_playback_video_packet_cb(uint8_t *data, uint32_t size, uint64_t pts, void *user_data)
{
    struct playback_context *ctx = (struct playback_context *)user_data;
    tuya_playback_video_put_frame(ctx->ch, ctx->id, data, size, ctx->start_time * 1000 + pts / 1000);
}
static void tuya_playback_audio_packet_cb(uint8_t *data, uint32_t size, uint64_t pts, void *user_data)
{
    struct playback_context *ctx = (struct playback_context *)user_data;
    tuya_playback_audio_put_frame(ctx->ch, ctx->id, data, size, ctx->start_time * 1000 + pts / 1000);
}

static void tuya_playback_finish_cb(uint8_t *data, uint32_t size, uint64_t pts, void *user_data)
{
    struct playback_context *ctx = (struct playback_context *)user_data;
    PLAYBACK_TIME_S fgmt = {
        .start_timestamp = ctx->start_time,
        .end_timestamp = ctx->start_time + ctx->duration,
    };
    ctx->finish = TRUE;
    if (size)
    {
        if (ctx->late_time == fgmt.end_timestamp)
        {
            tuya_ipc_media_playback_send_finish(ctx->ch);
        }
        else
        {
            tuya_ipc_media_playback_send_fragment_end(ctx->ch, &fgmt);
        }
    }
}

static int tuya_playback_start(UINT_T ch, UINT_T id, UINT64_T start_time, UINT64_T play_time, UINT64_T duration)
{
    struct tm tm;
    char path[128] = {0};

    if (ch >= STREAM_CLIENT_MAX)
    {
        return -1;
    }

    if (tuya_playback_ctx[ch].play_ctx && (tuya_playback_ctx[ch].start_time != start_time || tuya_playback_ctx[ch].finish == TRUE))
    {
        dyc_video_player_stop(tuya_playback_ctx[ch].play_ctx);
        tuya_playback_ctx[ch].play_ctx = NULL;
    }

    tuya_playback_ctx[ch].ch = ch;
    tuya_playback_ctx[ch].id = id;
    tuya_playback_ctx[ch].start_time = start_time;
    tuya_playback_ctx[ch].play_time = play_time;
    tuya_playback_ctx[ch].duration = duration;
    tuya_playback_ctx[ch].finish = FALSE;

    if (tuya_playback_ctx[ch].play_ctx == NULL)
    {
#ifdef PLATFORM_TYPE_X86_64
        start_time += 24 * 3600;
#elif PLATFORM_TYPE_SSD20X
        start_time += 8 * 3600;
#endif
        dyc_timestamp_to_time(&start_time, &tm);

        sprintf(path, "%s/media/%d/%d/%d/%llu_%llu.mp4", s_ipc_sd_path, tm.tm_year, tm.tm_mon, tm.tm_mday, start_time, duration);

        player_config cfg;
        cfg.file_name = path;
        cfg.v_pkt_cb = tuya_playback_video_packet_cb;
        cfg.a_pkt_cb = NULL;
        cfg.v_frame_cb = NULL;
        cfg.a_frame_cb = tuya_playback_audio_packet_cb;
        cfg.finish_cb = tuya_playback_finish_cb;
        cfg.channel = 1;
        cfg.rate = 8000;
        cfg.s_fmt = AV_SAMPLE_FMT_S16;
        cfg.mode = /* PLAYER_MODE_VDEC | */ PLAYER_MODE_ADEC | PLAYER_MODE_RESAMPLE;
        cfg.user_data = &tuya_playback_ctx[ch];
        tuya_playback_ctx[ch].play_ctx = dyc_video_player_start(&cfg);
    }

    dyc_video_player_time_seek(tuya_playback_ctx[ch].play_ctx, (tuya_playback_ctx[ch].play_time - tuya_playback_ctx[ch].start_time) * 1000000);
    return 0;
}

static int tuya_playback_pause(UINT_T ch)
{
    if (ch >= STREAM_CLIENT_MAX)
    {
        return -1;
    }
    if (tuya_playback_ctx[ch].play_ctx)
    {
        int status = dyc_video_player_status_get(tuya_playback_ctx[ch].play_ctx);
        if (status == 1)
        {
            dyc_video_player_toggle_pause(tuya_playback_ctx[ch].play_ctx);
        }
    }
    return 0;
}

static int tuya_playback_resume(UINT_T ch)
{
    if (ch >= STREAM_CLIENT_MAX)
    {
        return -1;
    }
    if (tuya_playback_ctx[ch].play_ctx)
    {
        int status = dyc_video_player_status_get(tuya_playback_ctx[ch].play_ctx);
        if (status == 2)
        {
            dyc_video_player_toggle_pause(tuya_playback_ctx[ch].play_ctx);
        }
    }
    return 0;
}

static int tuya_playback_stop(UINT_T ch)
{
    if (ch >= STREAM_CLIENT_MAX)
    {
        return -1;
    }
    if (tuya_playback_ctx[ch].play_ctx)
    {
        dyc_video_player_stop(tuya_playback_ctx[ch].play_ctx);
        tuya_playback_ctx[ch].play_ctx = NULL;
    }
    return 0;
}

static int tuya_playback_late_timestamp_set(UINT_T ch, UINT64_T timestamp)
{
    if (ch >= STREAM_CLIENT_MAX)
    {
        return -1;
    }
    tuya_playback_ctx[ch].late_time = timestamp;
    return 0;
}
#endif
int tuya_client_num_get(void)
{
    if (tuya_online_status_get() == false)
    {
        return 0;
    }
    return client_online_num;
}

// int tuya_realtime_audio_get_frame(unsigned char *data, int size)
// {
//     return dyc_ringbuffer_read(v_rev_audio_ringbuffer, data, size);
// }
