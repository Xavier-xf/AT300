// #include <pthread.h>
// #include <stdbool.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>
// #include <sys/stat.h>
// #include "db_queue.h"
// #include "db_video_player.h"
// #include "db_hal_driver.h"
// #include "db_common.h"
// #include "db_time.h"
// #include "video_display/driver_video_display.h"
// #include "video_decoder/driver_video_decoder.h"
// #include "audio_output/driver_audio_output.h"
// #include "audio_decoder/driver_audio_decoder.h"
// #include "minimp4.h"

// #define VIDEO_INPUT_BUFFER_SIZE (1024 * 1024)

// typedef struct
// {
// 	FILE *fp;
// 	uint8_t *buffer;
// 	ssize_t size;
// } INPUT_BUFFER;

// typedef struct
// {
// 	char path[128];

// 	MP4D_demux_t mp4_ctx;
// 	void *vdec_ctx;

// 	void *disp_ctx;
// 	int disp_x;
// 	int disp_y;
// 	int disp_w;
// 	int disp_h;

// 	void *adec_ctx;
// 	void *sound_ctx;
// 	int channel;
// 	int rate;

// 	// void *video_queue;
// 	// void *audio_queue;

// 	pthread_t thread_id;
// 	bool is_running;
// 	bool first_frame;

// 	db_hal_frame_cb video_pkt_cb;
// 	db_hal_frame_cb audio_pkt_cb;
// 	db_hal_frame_cb video_frame_cb;
// 	db_hal_frame_cb audio_frame_cb;
// 	db_hal_frame_cb play_finish_cb;

// 	void *user_data;
// } video_player_context;

// // static void video_player_video_recv_cb(unsigned char *data, unsigned int size, unsigned long long pts, void *user_data)
// // {
// // 	video_player_context *vp_ctx = (video_player_context *)user_data;
// // 	if (vp_ctx && data && size)
// // 	{
// // 		db_hal_video_packet_t pkt = {
// // 			.data = (unsigned char *)malloc(size),
// // 			.size = size,
// // 			.pts = pts,
// // 		};
// // 		memcpy(pkt.data, data, size);
// // 		db_queue_write(vp_ctx->video_queue, &pkt);
// // 	}
// // }

// static FILE *preload_file(const char *path, int *data_size)
// {
// 	if (!path || !data_size)
// 	{
// 		db_log_error("Invalid parameters for preload_file");
// 		return NULL;
// 	}

// 	FILE *file = fopen(path, "rb");
// 	*data_size = 0;
// 	if (!file)
// 	{
// 		db_log_error("Failed to open file %s: %s", path, strerror(errno));
// 		return NULL;
// 	}

// 	if (fseek(file, 0, SEEK_END))
// 	{
// 		db_log_error("Failed to seek end of file %s: %s", path, strerror(errno));
// 		fclose(file);
// 		return NULL;
// 	}

// 	long file_size = ftell(file);
// 	if (file_size < 0)
// 	{
// 		db_log_error("Failed to get file size of %s: %s", path, strerror(errno));
// 		fclose(file);
// 		return NULL;
// 	}

// 	if (fseek(file, 0, SEEK_SET))
// 	{
// 		db_log_error("Failed to seek start of file %s: %s", path, strerror(errno));
// 		fclose(file);
// 		return NULL;
// 	}

// 	*data_size = (int)file_size;
// 	return file;
// }

// static void float_to_s16(const float *src, int16_t *dst, int num_samples)
// {
// 	if (!src || !dst)
// 	{
// 		db_log_error("Invalid parameters for float_to_s16");
// 		return;
// 	}

// 	for (int i = 0; i < num_samples; i++)
// 	{
// 		dst[i] = amplitude_limit(src[i] * 32768);
// 	}
// }

// static int video_player_read_callback(int64_t offset, void *buffer, size_t size, void *token)
// {
// 	FILE *fp = (FILE *)token;
// 	if (!fp || !buffer || size == 0)
// 	{
// 		db_log_error("Invalid parameters for video_player_read_callback");
// 		return -1;
// 	}

// 	if (fseek(fp, offset, SEEK_SET))
// 	{
// 		db_log_error("Failed to seek file: %s", strerror(errno));
// 		return -1;
// 	}

// 	size_t read_bytes = fread(buffer, 1, size, fp);
// 	if (read_bytes != size)
// 	{
// 		db_log_error("Failed to read requested bytes: expected %zu, got %zu", size, read_bytes);
// 		return -1;
// 	}

// 	return 0;
// }

// static void *video_player_thread(void *arg)
// {
// 	video_player_context *vp_ctx = (video_player_context *)arg;
// 	db_hal_video_frame_t v_frame = {0};
// 	db_hal_audio_frame_t a_frame = {0};
// 	int input_size = 0;
// 	FILE *fp = NULL;
// 	int video_frame_index = 0;
// 	int audio_frame_index = 0;
// 	int spspps_bytes = 0;
// 	unsigned long long timestamp = 0;
	
// 	if (!vp_ctx)
// 	{
// 		db_log_error("Invalid video player context in thread");
// 		return NULL;
// 	}

// 	// Allocate input buffer
// 	unsigned char *buffer = (unsigned char *)malloc(1024 * 1024);
// 	if (!buffer)
// 	{
// 		db_log_error("Failed to allocate input buffer");
// 		return NULL;
// 	}

// 	// Preload video file
// 	fp = preload_file(vp_ctx->path, &input_size);
// 	if (!fp)
// 	{
// 		db_log_error("Failed to preload video file %s", vp_ctx->path);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Initialize display context
// 	db_video_display_config disp_cfg = {
// 		.pos.x = vp_ctx->disp_x,
// 		.pos.y = vp_ctx->disp_y,
// 		.pos.w = vp_ctx->disp_w,
// 		.pos.h = vp_ctx->disp_h,
// 	};

// 	vp_ctx->disp_ctx = db_hal_video_display_open(&disp_cfg);
// 	if (!vp_ctx->disp_ctx)
// 	{
// 		db_log_error("Failed to open video display");
// 		fclose(fp);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Initialize video decoder
// 	vp_ctx->vdec_ctx = db_hal_video_decoder_open(NULL);
// 	if (!vp_ctx->vdec_ctx)
// 	{
// 		db_log_error("Failed to open video decoder");
// 		db_hal_video_display_close(vp_ctx->disp_ctx);
// 		fclose(fp);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Initialize audio decoder
// 	db_audio_decoder_config adec_cfg = {
// 		.channel = vp_ctx->channel > 0 ? vp_ctx->channel : 2,
// 		.rate = vp_ctx->rate > 0 ? vp_ctx->rate : 16000,
// 	};
// 	vp_ctx->adec_ctx = db_hal_audio_decoder_open(&adec_cfg);
// 	if (!vp_ctx->adec_ctx)
// 	{
// 		db_log_error("Failed to open audio decoder");
// 		db_hal_video_decoder_close(vp_ctx->vdec_ctx);
// 		db_hal_video_display_close(vp_ctx->disp_ctx);
// 		fclose(fp);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Initialize audio output
// 	db_audio_output_config sound_cfg = {
// 		.channel = vp_ctx->channel > 0 ? vp_ctx->channel : 1,
// 		.rate = vp_ctx->rate > 0 ? vp_ctx->rate : 16000,
// 	};
// 	vp_ctx->sound_ctx = db_hal_audio_output_open(&sound_cfg);
// 	if (!vp_ctx->sound_ctx)
// 	{
// 		db_log_error("Failed to open audio output");
// 		db_hal_audio_decoder_close(vp_ctx->adec_ctx);
// 		db_hal_video_decoder_close(vp_ctx->vdec_ctx);
// 		db_hal_video_display_close(vp_ctx->disp_ctx);
// 		fclose(fp);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Enable audio output
// 	bool enable = true;
// 	if (db_hal_audio_output_ioctl(vp_ctx->sound_ctx, AUDIO_OUTPUT_USER_ENABLE_CMD, &enable, NULL) != 0)
// 	{
// 		db_log_warn("Failed to enable audio output");
// 	}

// 	// Initialize MP4 demuxer
// 	if (MP4D_open(&vp_ctx->mp4_ctx, video_player_read_callback, fp, input_size) != 0)
// 	{
// 		db_log_error("Failed to open MP4 file");
// 		db_hal_audio_output_close(vp_ctx->sound_ctx);
// 		db_hal_audio_decoder_close(vp_ctx->adec_ctx);
// 		db_hal_video_decoder_close(vp_ctx->vdec_ctx);
// 		db_hal_video_display_close(vp_ctx->disp_ctx);
// 		fclose(fp);
// 		free(buffer);
// 		return NULL;
// 	}

// 	// Main playback loop
// 	while (vp_ctx->is_running)
// 	{
// 		bool has_more_frames = false;
		
// 		for (int ntrack = 0; ntrack < vp_ctx->mp4_ctx.track_count; ntrack++)
// 		{
// 			MP4D_track_t *tr = vp_ctx->mp4_ctx.track + ntrack;
			
// 			if (tr->handler_type == MP4D_HANDLER_TYPE_VIDE) 
// 			{ // Video track (assuming h264)
// 				// Read SPS/PPS first if not done yet
// 				if (!spspps_bytes)
// 				{
// 					const void *spspps = NULL;
// 					char sync[4] = {0, 0, 0, 1};
					
// 					// Read SPS
// 					while ((spspps = MP4D_read_sps(&vp_ctx->mp4_ctx, ntrack, video_frame_index, &spspps_bytes)) != NULL)
// 					{
// 						memcpy(buffer, sync, sizeof(sync));
// 						memcpy(buffer + sizeof(sync), spspps, spspps_bytes);
						
// 						if (db_hal_video_decoder_write(vp_ctx->vdec_ctx, buffer, spspps_bytes + 4) != 0)
// 						{
// 							db_log_warn("Failed to write SPS to decoder");
// 						}
						
// 						// Flush decoder
// 						if (db_hal_video_decoder_read(vp_ctx->vdec_ctx, &v_frame, sizeof(v_frame)) == 0)
// 						{
// 							db_hal_video_decoder_ioctl(vp_ctx->vdec_ctx, VIDEO_DECODER_FRAME_RELEASE_CMD, &v_frame, NULL);
// 						}
// 						video_frame_index++;
// 					}
					
// 					video_frame_index = 0;
// 					spspps_bytes = 0;
					
// 					// Read PPS
// 					while ((spspps = MP4D_read_pps(&vp_ctx->mp4_ctx, ntrack, video_frame_index, &spspps_bytes)) != NULL)
// 					{
// 						memcpy(buffer, sync, sizeof(sync));
// 						memcpy(buffer + sizeof(sync), spspps, spspps_bytes);
						
// 						if (db_hal_video_decoder_write(vp_ctx->vdec_ctx, buffer, spspps_bytes + 4) != 0)
// 						{
// 							db_log_warn("Failed to write PPS to decoder");
// 						}
						
// 						// Flush decoder
// 						if (db_hal_video_decoder_read(vp_ctx->vdec_ctx, &v_frame, sizeof(v_frame)) == 0)
// 						{
// 							db_hal_video_decoder_ioctl(vp_ctx->vdec_ctx, VIDEO_DECODER_FRAME_RELEASE_CMD, &v_frame, NULL);
// 						}
// 						video_frame_index++;
// 					}
					
// 					video_frame_index = 0;
// 					timestamp = 0;
// 				}
				
// 				// Process video frames
// 				if (video_frame_index < vp_ctx->mp4_ctx.track[ntrack].sample_count)
// 				{
// 					has_more_frames = true;
					
// 					unsigned frame_bytes, frame_times, duration;
// 					MP4D_file_offset_t ofs = MP4D_frame_offset(&vp_ctx->mp4_ctx, ntrack, video_frame_index, &frame_bytes, &frame_times, &duration);
					
// 					// Check if it's time to display this frame
// 					if (timestamp < frame_times * 1000 / tr->timescale)
// 					{
// 						continue;
// 					}
					
// 					// Read frame data
// 					if (fseek(fp, ofs, SEEK_SET) != 0)
// 					{
// 						db_log_error("Failed to seek video frame: %s", strerror(errno));
// 						break;
// 					}
					
// 					if (fread(buffer, 1, frame_bytes, fp) != frame_bytes)
// 					{
// 						db_log_error("Failed to read video frame data");
// 						break;
// 					}
					
// 					uint8_t *mem = buffer;
					
// 					// Process NAL units
// 					while (frame_bytes > 0)
// 					{
// 						if (frame_bytes < 4)
// 						{
// 							db_log_warn("Invalid frame bytes, remaining: %u", frame_bytes);
// 							break;
// 						}
						
// 						uint32_t size = ((uint32_t)mem[0] << 24) | ((uint32_t)mem[1] << 16) | 
// 										 ((uint32_t)mem[2] << 8) | mem[3];
						
// 						if (size + 4 > frame_bytes)
// 						{
// 							db_log_error("Invalid NAL unit size: %u, frame_bytes: %u", size, frame_bytes);
// 							break;
// 						}
						
// 						// Convert to Annex B format (add start code)
// 						mem[0] = 0, mem[1] = 0, mem[2] = 0, mem[3] = 1;
						
// 						// Write to decoder
// 						if (db_hal_video_decoder_write(vp_ctx->vdec_ctx, mem, size + 4) != 0)
// 						{
// 							db_log_warn("Failed to write video NAL to decoder");
// 							break;
// 						}
						
// 						// Read decoded frame and display it
// 						if (db_hal_video_decoder_read(vp_ctx->vdec_ctx, &v_frame, sizeof(v_frame)) == 0)
// 						{
// 							db_hal_video_display_write(vp_ctx->disp_ctx, &v_frame, sizeof(v_frame));
// 							db_hal_video_decoder_ioctl(vp_ctx->vdec_ctx, VIDEO_DECODER_FRAME_RELEASE_CMD, &v_frame, NULL);
// 						}
						
// 						frame_bytes -= (size + 4);
// 						mem += (size + 4);
// 					}
					
// 					video_frame_index++;
// 				}
// 			}
// 			else if (tr->handler_type == MP4D_HANDLER_TYPE_SOUN) 
// 			{ // Audio track (assuming aac)
// 				if (audio_frame_index < vp_ctx->mp4_ctx.track[ntrack].sample_count)
// 				{
// 					has_more_frames = true;
					
// 					// Check audio output buffer space
// 					int free_length = 0;
// 					if (db_hal_audio_output_ioctl(vp_ctx->sound_ctx, AUDIO_OUTPUT_USER_FREE_CMD, NULL, &free_length) != 0)
// 					{
// 						db_log_warn("Failed to get audio output free space");
// 						continue;
// 					}
					
// 					if (free_length < 2048)
// 					{
// 						continue; // Wait for more free space
// 					}
					
// 					// Get frame information
// 					unsigned frame_bytes, frame_times, duration;
// 					MP4D_file_offset_t ofs = MP4D_frame_offset(&vp_ctx->mp4_ctx, ntrack, audio_frame_index, &frame_bytes, &frame_times, &duration);
					
// 					// Read audio frame data
// 					if (fseek(fp, ofs, SEEK_SET) != 0)
// 					{
// 						db_log_error("Failed to seek audio frame: %s", strerror(errno));
// 						break;
// 					}
					
// 					if (fread(buffer, 1, frame_bytes, fp) != frame_bytes)
// 					{
// 						db_log_error("Failed to read audio frame data");
// 						break;
// 					}
					
// 					// Decode audio
// 					if (db_hal_audio_decoder_write(vp_ctx->adec_ctx, buffer, frame_bytes) != 0)
// 					{
// 						db_log_warn("Failed to write audio data to decoder");
// 						break;
// 					}
					
// 					// Read decoded audio
// 					if (db_hal_audio_decoder_read(vp_ctx->adec_ctx, &a_frame, sizeof(a_frame)) != 0)
// 					{
// 						db_log_warn("Failed to read decoded audio frame");
// 						break;
// 					}
					
// 					// Convert float to s16 and play
// 					float_to_s16((float *)a_frame.data[0], (int16_t *)buffer, a_frame.size / 4);
					
// 					if (db_hal_audio_output_write(vp_ctx->sound_ctx, buffer, a_frame.size / 2) != 0)
// 					{
// 						db_log_warn("Failed to write audio to output");
// 					}
					
// 					audio_frame_index++;
// 				}
// 			}
// 		}
		
// 		// If no more frames, stop playback
// 		if (!has_more_frames)
// 		{
// 			db_log_info("No more frames to play, stopping");
// 			vp_ctx->is_running = false;
			
// 			// Notify playback finished if callback is set
// 			if (vp_ctx->play_finish_cb)
// 			{
// 				vp_ctx->play_finish_cb(NULL, 0, 0, vp_ctx->user_data);
// 			}
// 			break;
// 		}
		
// 		// Small delay to reduce CPU usage
// 		usleep(1000);
// 	}

// 	// Cleanup resources in reverse order of initialization
// 	MP4D_close(&vp_ctx->mp4_ctx);
	
// 	if (fp)
// 	{
// 		fclose(fp);
// 	}
	
// 	if (vp_ctx->sound_ctx)
// 	{
// 		db_hal_audio_output_close(vp_ctx->sound_ctx);
// 	}
	
// 	if (vp_ctx->adec_ctx)
// 	{
// 		db_hal_audio_decoder_close(vp_ctx->adec_ctx);
// 	}
	
// 	if (vp_ctx->vdec_ctx)
// 	{
// 		db_hal_video_decoder_close(vp_ctx->vdec_ctx);
// 	}
	
// 	if (vp_ctx->disp_ctx)
// 	{
// 		db_hal_video_display_close(vp_ctx->disp_ctx);
// 	}
	
// 	if (buffer)
// 	{
// 		free(buffer);
// 	}
	
// 	pthread_exit(NULL);
// 	return NULL;
// }

// void *db_video_player_start(db_video_player_config *cfg)
// {
// 	if (!cfg || !cfg->path)
// 	{
// 		db_log_error("Invalid parameters for db_video_player_start");
// 		return NULL;
// 	}

// 	// Check if file exists
// 	struct stat file_stat;
// 	if (stat(cfg->path, &file_stat) != 0)
// 	{
// 		db_log_error("Video file does not exist: %s", cfg->path);
// 		return NULL;
// 	}

// 	// Allocate player context
// 	video_player_context *vp_ctx = (video_player_context *)malloc(sizeof(video_player_context));
// 	if (!vp_ctx)
// 	{
// 		db_log_error("Failed to allocate video player context");
// 		return NULL;
// 	}

// 	// Initialize context
// 	memset(vp_ctx, 0, sizeof(video_player_context));
	
// 	// Copy path (ensure no buffer overflow)
// 	snprintf(vp_ctx->path, sizeof(vp_ctx->path), "%s", cfg->path);

// 	// Set callbacks
// 	vp_ctx->video_pkt_cb = cfg->v_pkt_cb;
// 	vp_ctx->audio_pkt_cb = cfg->a_pkt_cb;
// 	vp_ctx->video_frame_cb = cfg->v_frame_cb;
// 	vp_ctx->audio_frame_cb = cfg->a_frame_cb;
// 	vp_ctx->play_finish_cb = cfg->finish_cb;

// 	// Set display parameters
// 	vp_ctx->disp_x = cfg->disp_x;
// 	vp_ctx->disp_y = cfg->disp_y;
// 	vp_ctx->disp_w = cfg->disp_w;
// 	vp_ctx->disp_h = cfg->disp_h;

// 	// Set audio parameters
// 	vp_ctx->rate = cfg->rate;
// 	vp_ctx->channel = cfg->channel;

// 	vp_ctx->user_data = cfg->user_data;
// 	vp_ctx->is_running = true;
// 	vp_ctx->first_frame = true;

// 	// Create playback thread
// 	if (pthread_create(&(vp_ctx->thread_id), NULL, video_player_thread, vp_ctx) != 0)
// 	{
// 		db_log_error("Failed to create video player thread: %s", strerror(errno));
// 		free(vp_ctx);
// 		return NULL;
// 	}

// 	db_log_info("Video player started for file: %s", cfg->path);
// 	return vp_ctx;
// }

// int db_video_player_stop(void *context)
// {
// 	video_player_context *vp_ctx = (video_player_context *)context;

// 	if (!vp_ctx)
// 	{
// 		db_log_error("Invalid context for db_video_player_stop");
// 		return -1;
// 	}

// 	// Signal thread to stop
// 	vp_ctx->is_running = false;
	
// 	// Wait for thread to finish
// 	if (pthread_join(vp_ctx->thread_id, NULL) != 0)
// 	{
// 		db_log_error("Failed to join video player thread: %s", strerror(errno));
// 		return -1;
// 	}

// 	// Free context
// 	free(vp_ctx);
	
// 	db_log_info("Video player stopped");
// 	return 0;
// }
