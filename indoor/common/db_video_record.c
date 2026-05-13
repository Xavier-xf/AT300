// #include <pthread.h>
// #include <stdbool.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>
// #include "db_queue.h"
// #include "db_ringbuffer.h"
// #include "db_video_record.h"
// #include "db_hal_driver.h"
// #include "db_common.h"
// #include "db_time.h"
// #include "audio_encoder/driver_audio_encoder.h"
// #include "audio_decoder/driver_audio_decoder.h"
// #include "minimp4.h"
// #include "live555_rtsp.h"
// #include "codec/tuya_g711_utils.h"

// #define VIDEO_INPUT_BUFFER_SIZE (1024 * 1024)
// #define VIDEO_FPS 60
// #define ENABLE_AUDIO 1
// #define AUDIO_RATE 16000
// #define DEFAULT_VIDEO_WIDTH 640
// #define DEFAULT_VIDEO_HEIGHT 360
// #define DEFAULT_OUTPUT_FILENAME "output.mp4"
// #define DEFAULT_AUDIO_FILE "daoxiang_16k_flt_1ck.raw"

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

// 	void *aenc_ctx;
// 	int channel;
// 	int rate;

// 	void *video_queue;
// 	void *audio_ringbuf;

// 	pthread_t thread_id;
// 	bool is_running;
// 	bool first_frame;

// 	void *user_data;
// } video_record_context;

// static int write_callback(int64_t offset, const void *buffer, size_t size, void *token)
// {
// 	FILE *f = (FILE *)token;
// 	if (!f || !buffer) {
// 		db_log_error("Invalid parameters in write_callback");
// 		return -1;
// 	}
	
// 	if (fseek(f, offset, SEEK_SET) != 0) {
// 		db_log_error("fseek failed in write_callback: %s", strerror(errno));
// 		return -1;
// 	}
	
// 	size_t written = fwrite(buffer, 1, size, f);
// 	if (written != size) {
// 		db_log_error("fwrite failed in write_callback: %s", strerror(errno));
// 		return -1;
// 	}
	
// 	return 0;
// }

// // static ssize_t get_nal_size(uint8_t *buf, ssize_t size)
// // {
// // 	ssize_t pos = 3;
// // 	while ((size - pos) > 3)
// // 	{
// // 		if (buf[pos] == 0 && buf[pos + 1] == 0 && buf[pos + 2] == 1)
// // 			return pos;
// // 		if (buf[pos] == 0 && buf[pos + 1] == 0 && buf[pos + 2] == 0 && buf[pos + 3] == 1)
// // 			return pos;
// // 		pos++;
// // 	}
// // 	return size;
// // }

// // static FILE *preload_file(const char *path, int *data_size)
// // {
// // 	FILE *file = fopen(path, "rb");
// // 	// uint8_t *data;
// // 	*data_size = 0;
// // 	if (!file)
// // 	{
// // 		return 0;
// // 	}

// // 	if (fseek(file, 0, SEEK_END))
// // 	{
// // 		exit(1);
// // 	}

// // 	*data_size = ftell(file);
// // 	if (*data_size < 0)
// // 	{
// // 		exit(1);
// // 	}

// // 	if (fseek(file, 0, SEEK_SET))
// // 	{
// // 		exit(1);
// // 	}

// // 	return file;
// // }

// // static void float_to_s16(const float *src, int16_t *dst, int num_samples)
// // {
// // 	for (int i = 0; i < num_samples; i++)
// // 	{
// // 		dst[i] = amplitude_limit(src[i] * 32768);
// // 	}
// // }

// // static int video_record_read_callback(int64_t offset, void *buffer, size_t size, void *token)
// // {
// // 	FILE *fp = (FILE *)token;
// // 	fseek(fp, offset, SEEK_SET);
// // 	return fread(buffer, 1, size, fp) != size;
// // }

// // static uint8_t *preload(const char *path, ssize_t *data_size)
// // {
// // 	FILE *file = fopen(path, "rb");
// // 	uint8_t *data;
// // 	*data_size = 0;
// // 	if (!file)
// // 		return 0;
// // 	if (fseek(file, 0, SEEK_END))
// // 		exit(1);
// // 	*data_size = (ssize_t)ftell(file);
// // 	if (*data_size < 0)
// // 		exit(1);
// // 	if (fseek(file, 0, SEEK_SET))
// // 		exit(1);
// // 	data = (unsigned char *)malloc(*data_size);
// // 	if (!data)
// // 		exit(1);
// // 	if ((ssize_t)fread(data, 1, *data_size, file) != *data_size)
// // 		exit(1);
// // 	fclose(file);
// // 	return data;
// // }

// static void rtsp_player_video_recv_cb(unsigned char *data, unsigned int size, unsigned long long pts, void *user_data)
// {
// 	if (!data || !user_data || size == 0) {
// 		db_log_error("Invalid parameters in rtsp_player_video_recv_cb");
// 		return;
// 	}
	
// 	video_record_context *vr_ctx = (video_record_context *)user_data;
// 	if (!vr_ctx->video_queue) {
// 		db_log_error("Video queue not initialized in rtsp_player_video_recv_cb");
// 		return;
// 	}
	
// 	db_hal_video_packet_t pkt = {
// 		.data = (unsigned char *)malloc(size),
// 		.size = size,
// 		.pts = pts,
// 	};
	
// 	if (!pkt.data) {
// 		db_log_error("Failed to allocate memory for video packet: %s", strerror(errno));
// 		return;
// 	}
	
// 	memcpy(pkt.data, data, size);
	
// 	if (db_queue_write(vr_ctx->video_queue, &pkt) != 0) {
// 		db_log_error("Failed to write video packet to queue");
// 		free(pkt.data);
// 		return;
// 	}
	
// 	db_log_debug("Received video packet, size: %u, pts: %llu", size, pts);
// }

// /**
//  * 将16位整型PCM转换为浮点型PCM
//  * @param input 输入的16位PCM数据
//  * @param output 输出的浮点PCM数据
//  * @param samples 采样点数
//  */
// static void pcm16_to_float(const int16_t *input, float *output, size_t samples)
// {
// 	const float scale = 1.0f / 32768.0f;

// 	for (size_t i = 0; i < samples; i++)
// 	{
// 		output[i] = input[i] * scale;

// 		// 钳位到[-1.0, 1.0]范围
// 		if (output[i] > 1.0f)
// 			output[i] = 1.0f;
// 		if (output[i] < -1.0f)
// 			output[i] = -1.0f;
// 	}
// }

// /**
//  * 线性插值重采样（8kHz → 16kHz）
//  * @param input 输入的8kHz浮点PCM数据
//  * @param output 输出的16kHz浮点PCM数据
//  * @param input_samples 输入采样点数
//  * @return 输出采样点数（input_samples * 2）
//  */
// static size_t resample_8k_to_16k_linear(const float *input, float *output, size_t input_samples)
// {
// 	if (input_samples < 1)
// 		return 0;

// 	size_t output_samples = input_samples * 2;

// 	// 处理第一个采样点
// 	output[0] = input[0];

// 	// 中间点使用线性插值
// 	for (size_t i = 1; i < input_samples; i++)
// 	{
// 		size_t out_idx = i * 2;
// 		output[out_idx] = input[i]; // 原始采样点

// 		// 在两个原始采样点之间插入新点
// 		float interpolated = (input[i - 1] + input[i]) * 0.5f;
// 		output[out_idx - 1] = interpolated;
// 	}

// 	// 处理最后一个插值点（如果需要）
// 	if (input_samples > 1)
// 	{
// 		output[output_samples - 1] = input[input_samples - 1];
// 	}

// 	return output_samples;
// }

// /**
//  * 更高质量的重采样（使用多抽头插值）
//  * @param input 输入的8kHz浮点PCM数据
//  * @param output 输出的16kHz浮点PCM数据
//  * @param input_samples 输入采样点数
//  * @return 输出采样点数
//  */
// static size_t resample_8k_to_16k_quality(const float *input, float *output, size_t input_samples)
// {
// 	if (input_samples < 4)
// 	{
// 		// 样本太少，回退到线性插值
// 		return resample_8k_to_16k_linear(input, output, input_samples);
// 	}

// 	size_t output_samples = input_samples * 2;

// 	// 使用简单的4点插值滤波器
// 	for (size_t i = 0; i < output_samples; i++)
// 	{
// 		float pos = (float)i / 2.0f; // 在输入序列中的位置
// 		size_t base_idx = (size_t)pos;
// 		float frac = pos - base_idx;

// 		// 边界检查
// 		if (base_idx >= input_samples - 2)
// 		{
// 			if (base_idx >= input_samples - 1)
// 			{
// 				output[i] = input[input_samples - 1];
// 			}
// 			else
// 			{
// 				// 线性插值
// 				output[i] = input[base_idx] * (1.0f - frac) + input[base_idx + 1] * frac;
// 			}
// 			continue;
// 		}

// 		if (base_idx == 0)
// 		{
// 			// 开始部分使用线性插值
// 			output[i] = input[0] * (1.0f - frac) + input[1] * frac;
// 			continue;
// 		}

// 		// 4点sinc插值（简化版）
// 		float y0 = input[base_idx - 1];
// 		float y1 = input[base_idx];
// 		float y2 = input[base_idx + 1];
// 		float y3 = input[base_idx + 2];

// 		// 使用立方插值公式
// 		float a0 = y3 - y2 - y0 + y1;
// 		float a1 = y0 - y1 - a0;
// 		float a2 = y2 - y0;
// 		float a3 = y1;

// 		output[i] = ((a0 * frac + a1) * frac + a2) * frac + a3;
// 	}

// 	return output_samples;
// }

// /**
//  * 完整的8k S16到16k Float转换函数
//  * @param input_8k_s16 输入的8kHz 16位PCM数据
//  * @param output_16k_float 输出的16kHz浮点PCM数据
//  * @param input_samples 输入采样点数
//  * @return 输出采样点数
//  */
// static size_t convert_8k_s16_to_16k_float(const int16_t *input_8k_s16,
// 					   float *output_16k_float,
// 					   size_t input_samples)
// {
// 	// 第一步：分配临时缓冲区用于浮点转换
// 	float *temp_float = (float *)malloc(input_samples * sizeof(float));
// 	if (!temp_float)
// 	{
// 		return 0;
// 	}

// 	// 第二步：S16转Float
// 	pcm16_to_float(input_8k_s16, temp_float, input_samples);

// 	// 第三步：8kHz转16kHz重采样
// 	size_t output_samples = resample_8k_to_16k_quality(temp_float,
// 													   output_16k_float,
// 													   input_samples);

// 	free(temp_float);
// 	return output_samples;
// }

// static void rtsp_player_audio_recv_cb(unsigned char *data, unsigned int size, unsigned long long pts, void *user_data)
// {
// 	if (!data || !user_data || size == 0) {
// 		db_log_error("Invalid parameters in rtsp_player_audio_recv_cb");
// 		return;
// 	}
	
// 	video_record_context *vr_ctx = (video_record_context *)user_data;
// 	if (!vr_ctx->audio_ringbuf) {
// 		db_log_error("Audio ringbuffer not initialized in rtsp_player_audio_recv_cb");
// 		return;
// 	}
	
// 	unsigned char *pcm_8k_s16 = (unsigned char *)malloc(size * 2);
// 	if (!pcm_8k_s16) {
// 		db_log_error("Failed to allocate memory for pcm_8k_s16: %s", strerror(errno));
// 		return;
// 	}
	
// 	unsigned int out_len = 0;
// 	int decode_result = tuya_g711_decode(TUYA_G711_MU_LAW, (unsigned short *)data, size, pcm_8k_s16, &out_len);
// 	if (decode_result != 0) {
// 		db_log_error("G711 decode failed: %d", decode_result);
// 		free(pcm_8k_s16);
// 		return;
// 	}
	
// 	unsigned char *pcm_16k_float = (unsigned char *)malloc(size * 8);
// 	if (!pcm_16k_float) {
// 		db_log_error("Failed to allocate memory for pcm_16k_float: %s", strerror(errno));
// 		free(pcm_8k_s16);
// 		return;
// 	}
	
// 	size_t output_samples = convert_8k_s16_to_16k_float((int16_t *)pcm_8k_s16,
// 					   (float *)pcm_16k_float,
// 					   size);
	
// 	if (output_samples == 0) {
// 		db_log_error("Audio conversion failed");
// 		free(pcm_8k_s16);
// 		free(pcm_16k_float);
// 		return;
// 	}
	
// 	if (db_ringbuffer_write(vr_ctx->audio_ringbuf, pcm_16k_float, size * 8) != 0) {
// 		db_log_error("Failed to write audio data to ringbuffer");
// 		free(pcm_8k_s16);
// 		free(pcm_16k_float);
// 		return;
// 	}
	
// 	free(pcm_8k_s16);
// 	free(pcm_16k_float);
	
// 	db_log_debug("Received audio packet, size: %u, pts: %llu", size, pts);
// }

// static void *video_record_thread(void *arg)
// {
// 	if (!arg) {
// 		db_log_error("Invalid argument in video_record_thread");
// 		pthread_exit(NULL);
// 		return NULL;
// 	}
	
// 	video_record_context *vr_ctx = (video_record_context *)arg;
// 	db_hal_video_packet_t v_packet = {0};
// 	db_hal_audio_packet_t a_packet = {0};
// 	unsigned long long timestamp = 0;
// 	int sequential_mode = 0;
// 	int fragmentation_mode = 0;
// 	int video_count = 0;
// 	int audio_count = 0;

// 	// Open input file (this seems to be a test file, might be removed in production)
// 	FILE *fp_in = fopen(DEFAULT_AUDIO_FILE, "rb");
// 	if (!fp_in) {
// 		db_log_warn("Failed to open audio input file %s: %s", DEFAULT_AUDIO_FILE, strerror(errno));
// 		// Continue without input file, might be using RTSP instead
// 	}

// 	// Open output MP4 file
// 	FILE *fp_out = fopen(vr_ctx->path ? vr_ctx->path : DEFAULT_OUTPUT_FILENAME, "wb");
// 	if (!fp_out) {
// 		db_log_error("Failed to open output MP4 file %s: %s", 
// 			vr_ctx->path ? vr_ctx->path : DEFAULT_OUTPUT_FILENAME, strerror(errno));
// 		if (fp_in) fclose(fp_in);
// 		pthread_exit(NULL);
// 		return NULL;
// 	}

// 	// Allocate audio buffer
// 	unsigned char *buffer = (unsigned char *)malloc(1024 * sizeof(float));
// 	if (!buffer) {
// 		db_log_error("Failed to allocate audio buffer: %s", strerror(errno));
// 		fclose(fp_out);
// 		if (fp_in) fclose(fp_in);
// 		pthread_exit(NULL);
// 		return NULL;
// 	}

// 	// Open audio encoder
// 	db_audio_encoder_config aenc_cfg = {
// 		.rate = vr_ctx->rate > 0 ? vr_ctx->rate : AUDIO_RATE,
// 	};
// 	vr_ctx->aenc_ctx = db_hal_audio_encoder_open(&aenc_cfg);
// 	if (!vr_ctx->aenc_ctx) {
// 		db_log_error("Failed to open audio encoder");
// 		free(buffer);
// 		fclose(fp_out);
// 		if (fp_in) fclose(fp_in);
// 		pthread_exit(NULL);
// 		return NULL;
// 	}

// 	// Initialize video queue
// 	vr_ctx->video_queue = db_queue_open(sizeof(db_hal_video_packet_t), 5);
// 	if (!vr_ctx->video_queue) {
// 		db_log_error("Failed to create video queue");
// 		db_hal_audio_encoder_close(vr_ctx->aenc_ctx);
// 		free(buffer);
// 		fclose(fp_out);
// 		if (fp_in) fclose(fp_in);
// 		pthread_exit(NULL);
// 		return NULL;
// 	}

// 	// Initialize audio ringbuffer
// 	vr_ctx->audio_ringbuf = db_ringbuffer_open(AUDIO_RATE * 4);
// 	if (!vr_ctx->audio_ringbuf) {
// 		db_log_error("Failed to create audio ringbuffer");
// 		db_queue_close(vr_ctx->video_queue);
// 		db_hal_audio_encoder_close(vr_ctx->aenc_ctx);
// 		free(buffer);
// 		fclose(fp_out);
// 		if (fp_in) fclose(fp_in);
// 		pthread_exit(NULL);
// 		return NULL;
// 	}

// 	// Open RTSP client (this is hardcoded for now, should be configurable)
// 	rtsp_media_client_config rtsp_cfg = {
// 		.url = "rtsp://admin:hk123456@192.168.7.21:554/Streaming/Channels/102",
// 		.v_recv_cb = rtsp_player_video_recv_cb,
// 		.a_recv_cb = rtsp_player_audio_recv_cb,
// 		.user_data = vr_ctx,
// 	};
// 	void *rtsp_ctx = rtsp_media_client_open(&rtsp_cfg);
// 	if (!rtsp_ctx) {
// 		db_log_warn("Failed to open RTSP client");
// 		// Continue without RTSP, might be using manual NAL injection
// 	}

// 	// Initialize MP4 muxer
// 	MP4E_mux_t *mux;
// 	mp4_h26x_writer_t mp4wr;
// 	mux = MP4E_open(sequential_mode, fragmentation_mode, fp_out, write_callback);
// 	if (!mux) {
// 		db_log_error("Failed to open MP4 muxer");
// 		goto fail;
// 	}
	
// 	if (MP4E_STATUS_OK != mp4_h26x_write_init(&mp4wr, mux, DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, /* is_hevc */ 0))
// 	{
// 		db_log_error("mp4_h26x_write_init failed");
// 		MP4E_close(mux);
// 		goto fail;
// 	}

// #if ENABLE_AUDIO
// 	uint64_t ts = 0, ats = 0;

// 	MP4E_track_t tr;
// 	tr.track_media_kind = e_audio;
// 	tr.language[0] = 'u';
// 	tr.language[1] = 'n';
// 	tr.language[2] = 'd';
// 	tr.language[3] = 0;
// 	tr.object_type_indication = MP4_OBJECT_TYPE_AUDIO_ISO_IEC_14496_3;
// 	tr.time_scale = 90000;
// 	tr.default_duration = 0;
// 	tr.u.a.channelcount = vr_ctx->channel > 0 ? vr_ctx->channel : 1;
	
// 	int audio_track_id = MP4E_add_track(mux, &tr);
// 	if (audio_track_id < 0) {
// 		db_log_error("Failed to add audio track");
// 		goto fail;
// 	}
	
// 	static unsigned char aac_conf[4] = {0x14, 0x08, 0x0, 0x0};
// 	if (MP4E_set_dsi(mux, audio_track_id, aac_conf, 4) != MP4E_STATUS_OK) {
// 		db_log_error("Failed to set DSI for audio track");
// 		goto fail;
// 	}
	
// 	db_log_info("Audio track added successfully, channel count: %d", tr.u.a.channelcount);
// #endif
// 	while (vr_ctx->is_running)
// 	{
// 		// Process video packets
// 		if (db_queue_read(vr_ctx->video_queue, &v_packet) == 0)
// 		{
// 			if (v_packet.data == NULL || v_packet.size == 0) {
// 				db_log_error("Invalid video packet received");
// 				continue;
// 			}
			
// 		db_log_debug("v_packet.times:%llu video_count:%d", v_packet.pts - timestamp, video_count);
// 			if (timestamp == 0)
// 			{
// 				timestamp = v_packet.pts;
// 			}
			
// 			int duration = v_packet.pts - timestamp;
// 			if (MP4E_STATUS_OK != mp4_h26x_write_nal(&mp4wr, v_packet.data, v_packet.size, duration ? 90000 / (1000 / duration) : 0))
// 			{
// 				db_log_error("mp4_h26x_write_nal failed");
// 				// Continue processing other packets instead of failing completely
// 			}
// 			else
// 			{
// 				ts += duration ? 90000 / (1000 / duration) : 0;
// 				video_count++;
// 			}
			
// 			free(v_packet.data);
// 			timestamp = v_packet.pts;
			
// 			// Limiting to 500 frames for testing purposes
// 			if (video_count >= 500)
// 			{
// 				db_log_info("Reached 500 video frames, stopping recording");
// 				break;
// 			}
// 		}

// #if ENABLE_AUDIO
// 		// Process audio packets
// 		while (ats < ts)
// 		{
// 			int vaild_size = 0;
// 			db_ringbuffer_ioctl(vr_ctx->audio_ringbuf, DB_RINGBUFFER_CMD_VALID_GET, &vaild_size);
// 			if (vaild_size >= (1024 * sizeof(float)))
// 			{
// 				db_log_debug("audio_count:%d", audio_count);
// 				ats += (uint64_t)1024 * 90000 / AUDIO_RATE;

// 				// Read audio data from ringbuffer
// 				if (db_ringbuffer_read(vr_ctx->audio_ringbuf, buffer, 1024 * sizeof(float)) != 0) {
// 					db_log_error("Failed to read audio data from ringbuffer");
// 					break;
// 				}

// 				// Encode audio
// 				if (db_hal_audio_encoder_write(vr_ctx->aenc_ctx, buffer, 1024 * sizeof(float)) != 0) {
// 					db_log_error("Failed to write audio data to encoder");
// 					break;
// 				}
				
// 				if (db_hal_audio_encoder_read(vr_ctx->aenc_ctx, &a_packet, sizeof(db_hal_audio_packet_t)) != 0) {
// 					db_log_error("Failed to read audio packet from encoder");
// 					break;
// 				}
				
// 				if (a_packet.data && a_packet.size > 0) {
// 					if (MP4E_STATUS_OK != MP4E_put_sample(mux, audio_track_id, a_packet.data, a_packet.size, 1024 * 90000 / AUDIO_RATE, MP4E_SAMPLE_RANDOM_ACCESS))
// 					{
// 						db_log_error("MP4E_put_sample failed");
// 					}
// 					else
// 					{
// 						audio_count++;
// 					}
// 					// Release the packet
// 					db_hal_audio_encoder_ioctl(vr_ctx->aenc_ctx, AUDIO_ENCODER_PACKET_RELEASE_CMD, NULL, NULL);
// 				}
// 			} else {
// 				// Small delay to avoid busy waiting
// 				usleep(1000);
// 			}
// 		}
// #endif
// 		// Small delay to avoid busy waiting
// 		usleep(1000);
// 	}

// fail:
// 	// Clean up resources
// 	if (fp_in) {
// 		fclose(fp_in);
// 	}
	
// 	if (rtsp_ctx) {
// 		rtsp_media_client_close(rtsp_ctx);
// 	}
	
// 	MP4E_close(mux);
// 	mp4_h26x_write_close(&mp4wr);
	
// 	if (fp_out) {
// 		fclose(fp_out);
// 	}
	
// 	if (vr_ctx->audio_ringbuf) {
// 		db_ringbuffer_close(vr_ctx->audio_ringbuf);
// 	}
	
// 	if (vr_ctx->video_queue) {
// 		db_queue_close(vr_ctx->video_queue);
// 	}
	
// 	if (vr_ctx->aenc_ctx) {
// 		db_hal_audio_encoder_close(vr_ctx->aenc_ctx);
// 	}
	
// 	if (buffer) {
// 		free(buffer);
// 	}
	
// 	db_log_info("Video recording thread exited");
// 	pthread_exit(NULL);
// 	return NULL;
// }

// void *db_video_record_start(db_video_record_config *cfg)
// {
// 	if (cfg == NULL) {
// 		db_log_error("Invalid configuration in db_video_record_start");
// 		return NULL;
// 	}
	
// 	video_record_context *vr_ctx = malloc(sizeof(video_record_context));
// 	if (vr_ctx == NULL) {
// 		db_log_error("Failed to allocate memory for video_record_context: %s", strerror(errno));
// 		return NULL;
// 	}
	
// 	// Initialize context
// 	memset(vr_ctx, 0, sizeof(video_record_context));
	
// 	// Set path
// 	if (cfg->path) {
// 		snprintf(vr_ctx->path, sizeof(vr_ctx->path), "%s", cfg->path);
// 	}
	
// 	vr_ctx->rate = cfg->rate;
// 	vr_ctx->channel = cfg->channel;
// 	vr_ctx->user_data = cfg->user_data;
// 	vr_ctx->is_running = true;
	
// 	// Create thread
// 	if (pthread_create(&(vr_ctx->thread_id), NULL, video_record_thread, vr_ctx) != 0) {
// 		db_log_error("Failed to create video recording thread: %s", strerror(errno));
// 		free(vr_ctx);
// 		return NULL;
// 	}
	
// 	db_log_info("Video recording started, path: %s, rate: %d, channel: %d", 
// 		vr_ctx->path, vr_ctx->rate, vr_ctx->channel);
	
// 	return vr_ctx;
// }

// int db_video_record_write_nal(void *context, unsigned char *data, int size)
// {
// 	if (!context || !data || size <= 0) {
// 		db_log_error("Invalid parameters in db_video_record_write_nal");
// 		return -1;
// 	}
	
// 	video_record_context *vr_ctx = (video_record_context *)context;
// 	if (!vr_ctx->video_queue) {
// 		db_log_error("Video queue not initialized in db_video_record_write_nal");
// 		return -1;
// 	}

// 	db_hal_video_packet_t pkt = {
// 		.data = (unsigned char *)malloc(size),
// 		.size = size,
// 		.pts = 0,  // TODO: Add support for timestamp parameter
// 	};
	
// 	if (!pkt.data) {
// 		db_log_error("Failed to allocate memory for NAL unit: %s", strerror(errno));
// 		return -1;
// 	}
	
// 	memcpy(pkt.data, data, size);
	
// 	int result = db_queue_write(vr_ctx->video_queue, &pkt);
// 	if (result != 0) {
// 		db_log_error("Failed to write NAL unit to queue: %d", result);
// 		free(pkt.data);
// 		return -1;
// 	}
	
// 	db_log_debug("NAL unit written to queue, size: %d", size);
// 	return 0;
// }

// int db_video_record_stop(void *context)
// {
// 	if (!context) {
// 		db_log_error("Invalid context in db_video_record_stop");
// 		return -1;
// 	}
	
// 	video_record_context *vr_ctx = (video_record_context *)context;
// 	vr_ctx->is_running = false;
	
// 	// Join thread
// 	if (pthread_join(vr_ctx->thread_id, NULL) != 0) {
// 		db_log_error("Failed to join video recording thread: %s", strerror(errno));
// 		return -1;
// 	}
	
// 	// Free context
// 	free(vr_ctx);
	
// 	db_log_info("Video recording stopped");
// 	return 0;
// }
