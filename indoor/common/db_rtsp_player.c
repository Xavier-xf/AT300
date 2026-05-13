#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "db_queue.h"
#include "db_rtsp_player.h"
#include "db_hal_driver.h"
#include "video_display/driver_video_display.h"
#include "audio_output/driver_audio_output.h"
#include "video_decoder/driver_video_decoder.h"
#include "audio_decoder/driver_audio_decoder.h"
#include "live555_rtsp.h"
#include "codec/tuya_g711_utils.h"
#include "db_motion_detect.h"

typedef struct
{
	char url[128];

	void *rtsp_ctx;

	void *vdec_ctx;
	void *disp_ctx;
	int x;
	int y;
	int width;
	int height;

	void *adec_ctx;
	void *sound_ctx;
	int channel;
	int rate;

	int mode;

	pthread_t thread_id;
	bool is_running;
	bool first_frame;

	db_hal_frame_cb video_pkt_cb;
	db_hal_frame_cb audio_pkt_cb;
	db_hal_frame_cb video_frame_cb;
	db_hal_frame_cb audio_frame_cb;
	db_hal_frame_cb play_finish_cb;

	void *user_data;

} rtsp_player_context;

static void rtsp_player_video_recv_cb(unsigned char *data, unsigned int size, unsigned long long pts, void *user_data)
{
	rtsp_player_context *rp_ctx = (rtsp_player_context *)user_data;
	if (rp_ctx && data && size)
	{
		if (rp_ctx->video_pkt_cb)
		{
			rp_ctx->video_pkt_cb(data, size, pts, rp_ctx->user_data);
		}
		if (rp_ctx->vdec_ctx)
		{
			if (db_hal_video_decoder_write(rp_ctx->vdec_ctx, data, size) != 0)
			{
				// db_log_error("db_hal_video_decoder_write failed");
			}
		}
	}
	else
	{
		db_log_warn("rtsp_player_video_recv_cb: Invalid parameters");
	}
}

static void rtsp_player_audio_recv_cb(unsigned char *data, unsigned int size, unsigned long long pts, void *user_data)
{
	rtsp_player_context *rp_ctx = (rtsp_player_context *)user_data;
	if (rp_ctx && data && size)
	{
		if (rp_ctx->audio_pkt_cb)
		{
			rp_ctx->audio_pkt_cb(data, size, pts, rp_ctx->user_data);
		}
		if (rp_ctx->adec_ctx)
		{
			if (db_hal_audio_decoder_write(rp_ctx->adec_ctx, data, size) != 0)
			{
				db_log_error("db_hal_audio_decoder_write failed");
			}
		}
	}
	else
	{
		db_log_warn("rtsp_player_audio_recv_cb: Invalid parameters");
	}
}

static void *rtsp_player_thread(void *arg)
{
	if (!arg)
	{
		db_log_error("rtsp_player_thread: Invalid argument");
		pthread_exit(NULL);
		return NULL;
	}

	rtsp_player_context *rp_ctx = (rtsp_player_context *)arg;
	db_hal_video_frame_t v_frame = {0};
	db_hal_audio_frame_t a_frame = {0};
	int count = 0;
	db_motion_detect_context_t *md_ctx = NULL;

	if (rp_ctx->mode & VIDEO_PLAYER_MODE_DISP)
	{
		db_video_display_config disp_cfg = {
			.pos.x = rp_ctx->x,
			.pos.y = rp_ctx->y,
			.pos.w = rp_ctx->width,
			.pos.h = rp_ctx->height,
		};
		rp_ctx->disp_ctx = db_hal_video_display_open(&disp_cfg);
		if (rp_ctx->disp_ctx == NULL)
		{
			db_log_error("db_hal_video_display_open failed");
			goto end;
		}
	}

	if (rp_ctx->mode & VIDEO_PLAYER_MODE_SOUND)
	{
		db_audio_output_config sound_cfg = {
			.channel = rp_ctx->channel,
			.rate = rp_ctx->rate,
		};
		rp_ctx->sound_ctx = db_hal_audio_output_open(&sound_cfg);
		if (rp_ctx->sound_ctx == NULL)
		{
			db_log_error("db_hal_audio_output_open failed");
			goto end;
		}
		bool enable = true;
		if (db_hal_audio_output_ioctl(rp_ctx->sound_ctx, AUDIO_OUTPUT_USER_ENABLE_CMD, &enable, NULL) != 0)
		{
			db_log_error("Failed to enable audio output");
			// Continue even if enabling fails
		}
	}

	if (rp_ctx->mode & VIDEO_PLAYER_MODE_VDEC)
	{
		db_video_decoder_config vdec_cfg = {
			.type = VIDEO_DECODER_TYPE_H264,
		};
		rp_ctx->vdec_ctx = db_hal_video_decoder_open(&vdec_cfg);
		if (rp_ctx->vdec_ctx == NULL)
		{
			db_log_error("db_hal_video_decoder_open failed");
			goto end;
		}
	}

	if (rp_ctx->mode & VIDEO_PLAYER_MODE_ADEC)
	{
		db_audio_decoder_config adec_cfg = {
			.channel = rp_ctx->channel,
			.rate = rp_ctx->rate,
			.type = AUDIO_DECODER_TYPE_PCMU,
		};
		rp_ctx->adec_ctx = db_hal_audio_decoder_open(&adec_cfg);
		if (rp_ctx->adec_ctx == NULL)
		{
			db_log_error("db_hal_audio_decoder_open failed");
			goto end;
		}
	}

	rtsp_media_client_config rtsp_cfg = {
		.url = rp_ctx->url,
		.v_recv_cb = rtsp_player_video_recv_cb,
		.a_recv_cb = rtsp_player_audio_recv_cb,
		.user_data = rp_ctx,
	};
	rp_ctx->rtsp_ctx = rtsp_media_client_open(&rtsp_cfg);
	if (rp_ctx->rtsp_ctx == NULL)
	{
		db_log_error("rtsp_media_client_open failed");
		goto end;
	}

	db_log_info("rtsp player open succeeded");

	while (rp_ctx->is_running)
	{
		if (rp_ctx->mode & VIDEO_PLAYER_MODE_VDEC && rp_ctx->vdec_ctx)
		{
			while (db_hal_video_decoder_read(rp_ctx->vdec_ctx, &v_frame, sizeof(v_frame)) == 0)
			{
				if (v_frame.data == NULL)
				{
					db_log_error("Invalid video frame data");
					break;
				}
				if (md_ctx == NULL)
					md_ctx = db_motion_detect_open(v_frame.width, v_frame.hight, 30, 5, NULL);
				if (count++ % 10 == 0)
					db_motion_detect_write(md_ctx, v_frame.data, v_frame.width * v_frame.hight * 3 / 2);
				if (rp_ctx->video_frame_cb)
				{
					rp_ctx->video_frame_cb(v_frame.data, v_frame.width * v_frame.hight * 3 / 2, 0, rp_ctx->user_data);
				}
				if (rp_ctx->mode & VIDEO_PLAYER_MODE_DISP && rp_ctx->disp_ctx)
				{
					if (db_hal_video_display_write(rp_ctx->disp_ctx, &v_frame, sizeof(v_frame)) != 0)
					{
						db_log_error("db_hal_video_display_write failed");
					}
				}
				if (db_hal_video_decoder_ioctl(rp_ctx->vdec_ctx, VIDEO_DECODER_FRAME_RELEASE_CMD, &v_frame, NULL) != 0)
				{
					db_log_error("Failed to release video frame");
				}
			}
		}
		if (rp_ctx->mode & VIDEO_PLAYER_MODE_ADEC && rp_ctx->adec_ctx)
		{
			while (db_hal_audio_decoder_read(rp_ctx->adec_ctx, &a_frame, sizeof(a_frame)) == 0)
			{
				if (a_frame.data[0] == NULL)
				{
					db_log_error("Invalid audio frame data");
					break;
				}

				if (rp_ctx->audio_frame_cb)
				{
					rp_ctx->audio_frame_cb(a_frame.data[0], a_frame.size, 0, rp_ctx->user_data);
				}
				if (rp_ctx->mode & VIDEO_PLAYER_MODE_SOUND && rp_ctx->sound_ctx)
				{
					if (db_hal_audio_output_write(rp_ctx->sound_ctx, a_frame.data[0], a_frame.size) != 0)
					{
						db_log_error("db_hal_audio_output_write failed");
					}
				}
			}
		}
		// Small delay to reduce CPU usage
		usleep(1 * 1000);
	}

	db_log_info("rtsp player close succeeded");

	if (rp_ctx->play_finish_cb)
	{
		rp_ctx->play_finish_cb(NULL, 0, 0, rp_ctx->user_data);
	}

end:
	// Clean up resources in reverse order of initialization
	db_motion_detect_close(md_ctx);
	rtsp_media_client_close(rp_ctx->rtsp_ctx);
	db_hal_audio_decoder_close(rp_ctx->adec_ctx);
	db_hal_video_decoder_close(rp_ctx->vdec_ctx);
	db_hal_audio_output_close(rp_ctx->sound_ctx);
	db_hal_video_display_close(rp_ctx->disp_ctx);

	db_log_info("rtsp player thread exited");
	pthread_exit(NULL);
	return NULL;
}

void *db_rtsp_player_start(db_rtsp_player_config *cfg)
{
	if (cfg == NULL)
	{
		db_log_error("db_rtsp_player_start: Invalid configuration");
		return NULL;
	}

	if (cfg->rtsp_url == NULL)
	{
		db_log_error("db_rtsp_player_start: RTSP URL is NULL");
		return NULL;
	}

	rtsp_player_context *rp_ctx = malloc(sizeof(rtsp_player_context));
	if (rp_ctx == NULL)
	{
		db_log_error("db_rtsp_player_start: Failed to allocate memory: %s", strerror(errno));
		return NULL;
	}

	memset(rp_ctx, 0, sizeof(rtsp_player_context));

	// Use snprintf to prevent buffer overflow
	snprintf(rp_ctx->url, sizeof(rp_ctx->url), "%s", cfg->rtsp_url);

	rp_ctx->video_pkt_cb = cfg->v_pkt_cb;
	rp_ctx->audio_pkt_cb = cfg->a_pkt_cb;
	rp_ctx->video_frame_cb = cfg->v_frame_cb;
	rp_ctx->audio_frame_cb = cfg->a_frame_cb;
	rp_ctx->play_finish_cb = cfg->finish_cb;

	rp_ctx->x = cfg->x;
	rp_ctx->y = cfg->y;
	rp_ctx->width = cfg->width;
	rp_ctx->height = cfg->height;

	rp_ctx->rate = cfg->rate;
	rp_ctx->channel = cfg->channel;

	rp_ctx->mode = cfg->mode;

	rp_ctx->user_data = cfg->user_data;
	rp_ctx->is_running = true;

	// Create thread
	if (pthread_create(&(rp_ctx->thread_id), NULL, rtsp_player_thread, rp_ctx) != 0)
	{
		db_log_error("db_rtsp_player_start: Failed to create thread: %s", strerror(errno));
		free(rp_ctx);
		return NULL;
	}

	db_log_info("rtsp player started with URL: %s", rp_ctx->url);
	return rp_ctx;
}

int db_rtsp_player_stop(void *context)
{
	if (context == NULL)
	{
		db_log_error("db_rtsp_player_stop: Invalid context");
		return -1;
	}

	rtsp_player_context *rp_ctx = (rtsp_player_context *)context;

	rp_ctx->is_running = false;

	// Join thread
	if (pthread_join(rp_ctx->thread_id, NULL) != 0)
	{
		db_log_error("db_rtsp_player_stop: Failed to join thread: %s", strerror(errno));
		return -1;
	}

	// Free context
	free(rp_ctx);

	db_log_info("rtsp player stopped");
	return 0;
}
