#include "layout_common.h"

#define SIP_SERVER_IP "121.157.1.71"
#define SIP_SERVER_PORT 5060
#define SIP_USERNAME "007102720200"
#define SIP_PASSWORD "BqPfhBDE2b"
#define SIP_URI "sip:121.157.1.71:5060"
#define RTP_AUDIO_PORT 4000 // 音频端口
#define RTP_VIDEO_PORT 4002 // 视频端口

#define SIP_REGIST_SIGNALLING "REGISTER sip:121.157.1.71:5060 SIP/2.0\r\n"                                                           \
							  "Via: SIP/2.0/UDP 192.168.2.7:5060;branch=z9hG4bKPjc548c27c57af47ea92dabbc82a02756f\r\n"               \
							  "Route: <sip:121.157.1.71:5060;lr>\r\n"                                                                \
							  "Max-Forwards: 70\r\n"                                                                                 \
							  "From: \"007102720200\" <sip:007102720200@121.157.1.71>;tag=2a12ea2b50cd448bac065784ab649d5f\r\n"      \
							  "To: \"007102720200\" <sip:007102720200@121.157.1.71>\r\n"                                             \
							  "Call-ID: 0edbbc1b69af48b5905da0d77c15a285\r\n"                                                        \
							  "CSeq: 25550 REGISTER\r\n"                                                                             \
							  "User-Agent: MicroSIP/3.21.3\r\n"                                                                      \
							  "Contact: \"007102720200\" <sip:007102720200@192.168.2.7:5060;ob>\r\n"                                 \
							  "Expires: 300\r\n"                                                                                     \
							  "Allow: PRACK, INVITE, ACK, BYE, CANCEL, UPDATE, INFO, SUBSCRIBE, NOTIFY, REFER, MESSAGE, OPTIONS\r\n" \
							  "%s"                                                                                                   \
							  "Content-Length:  0\r\n"                                                                               \
							  "\r\n"

#define SIP_NOTIFY_OK_SIGNALLING "SIP/2.0 200 OK\r\n"                                                                \
								 "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n" \
								 "Call-ID: %s@121.157.1.71:5060\r\n"                                                 \
								 "From: \"asterisk\" <sip:asterisk@121.157.1.71>;tag=%s\r\n"                         \
								 "To: <sip:007102720200@192.168.2.7;ob>;tag=%s\r\n"                                  \
								 "CSeq: 102 NOTIFY\r\n"                                                              \
								 "Content-Length:  0\r\n"                                                            \
								 "\r\n"

#define SIP_TRYING_SIGNALLING "SIP/2.0 100 Trying\r\n"                                                            \
							  "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n" \
							  "Call-ID: %s@121.157.1.71:5060\r\n"                                                 \
							  "From: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                 \
							  "To: <sip:007102720200@192.168.2.7;ob>\r\n"                                         \
							  "CSeq: 102 INVITE\r\n"                                                              \
							  "Content-Length:  0\r\n"                                                            \
							  "\r\n"

#define SIP_RINGING_SIGNALLING "SIP/2.0 180 Ringing\r\n"                                                                              \
							   "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n"                    \
							   "Call-ID: %s@121.157.1.71:5060\r\n"                                                                    \
							   "From: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                                    \
							   "To: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"                       \
							   "CSeq: 102 INVITE\r\n"                                                                                 \
							   "Contact: \"007102720200\" <sip:007102720200@192.168.2.7:5060;ob>\r\n"                                 \
							   "Allow: PRACK, INVITE, ACK, BYE, CANCEL, UPDATE, INFO, SUBSCRIBE, NOTIFY, REFER, MESSAGE, OPTIONS\r\n" \
							   "Content-Length:  0\r\n"                                                                               \
							   "\r\n"

#define SIP_ANSWERING_SIGNALLING "SIP/2.0 200 OK\r\n"                                                                                   \
								 "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n"                    \
								 "Call-ID: %s@121.157.1.71:5060\r\n"                                                                    \
								 "From: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                                    \
								 "To: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"                       \
								 "CSeq: 102 INVITE\r\n"                                                                                 \
								 "Allow: PRACK, INVITE, ACK, BYE, CANCEL, UPDATE, INFO, SUBSCRIBE, NOTIFY, REFER, MESSAGE, OPTIONS\r\n" \
								 "Contact: \"007102720200\" <sip:007102720200@192.168.2.7:5060;ob>\r\n"                                 \
								 "Supported: replaces, 100rel, timer, norefersub\r\n"                                                   \
								 "Content-Type: application/sdp\r\n"                                                                    \
								 "Content-Length:   510\r\n"                                                                            \
								 "\r\n"                                                                                                 \
								 "v=0\r\n"                                                                                              \
								 "o=- 3966137445 3966137446 IN IP4 192.168.2.7\r\n"                                                     \
								 "s=pjmedia\r\n"                                                                                        \
								 "b=AS:1159\r\n"                                                                                        \
								 "t=0 0\r\n"                                                                                            \
								 "a=X-nat:0\r\n"                                                                                        \
								 "m=audio 4000 RTP/AVP 0\r\n"                                                                           \
								 "c=IN IP4 192.168.2.7\r\n"                                                                             \
								 "a=sendrecv\r\n"                                                                                       \
								 "a=rtpmap:0 PCMU/8000\r\n"                                                                             \
								 "a=ssrc:752251566 cname:428b26a6701f5d03\r\n"                                                          \
								 "m=video 4002 RTP/AVP 99\r\n"                                                                          \
								 "c=IN IP4 192.168.2.7\r\n"                                                                             \
								 "a=sendrecv\r\n"                                                                                       \
								 "a=rtpmap:99 H264/90000\r\n"                                                                           \
								 "a=fmtp:99 profile-level-id=42801F; packetization-mode=0\r\n"                                          \
								 "a=ssrc:1767006096 cname:428b26a6701f5d03\r\n"                                                         \
								 "a=rtcp-fb:* nack pli\r\n"                                                                             \
								 "\r\n"

#define SIP_INFO_SIGNALLING "INFO sip:000010200004@121.157.1.71:5060 SIP/2.0\r\n"                                    \
							"Via: SIP/2.0/UDP 192.168.2.7:5060;branch=z9hG4bKPjbb4e84bdb03d4fef818cd8c5f7b0c7a1\r\n" \
							"Max-Forwards: 70\r\n"                                                                   \
							"From: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"       \
							"To: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                        \
							"Call-ID: %s@121.157.1.71:5060\r\n"                                                      \
							"CSeq: 30333 INFO\r\n"                                                                   \
							"User-Agent: MicroSIP/3.21.3\r\n"                                                        \
							"Content-Type: application/media_control+xml\r\n"                                        \
							"Content-Length:   146\r\n"                                                              \
							"\r\n"                                                                                   \
							"<?xml version=\"1.0\" encoding=\"utf-8\" ?><media_control><vc_primitive><to_encoder><picture_fast_update/></to_encoder></vc_primitive></media_control>"

#define SIP_INFO_OK_SIGNALLING "SIP/2.0 200 OK\r\n"                                                                \
							   "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n" \
							   "Call-ID: %s@121.157.1.71:5060\r\n"                                                 \
							   "From: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                 \
							   "To: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"    \
							   "CSeq: 103 INFO\r\n"                                                                \
							   "Content-Length:  0\r\n"                                                            \
							   "\r\n"

#define SIP_BYE_OK_SIGNALLING "SIP/2.0 200 OK\r\n"                                                                \
							  "Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=%s\r\n" \
							  "Call-ID: %s@121.157.1.71:5060\r\n"                                                 \
							  "From: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                 \
							  "To: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"    \
							  "CSeq: 103 BYE\r\n"                                                                 \
							  "Content-Length:  0\r\n"                                                            \
							  "\r\n"

#define SIP_REINVITE_SIGNALLING "INVITE sip:000010200004@121.157.1.71:5060 SIP/2.0\r\n"                                  \
								"Via: SIP/2.0/UDP 192.168.2.7:5060;branch=z9hG4bKPjbb4e84bdb03d4fef818cd8c5f7b0c7a1\r\n" \
								"Max-Forwards: 70\r\n"                                                                   \
								"From: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"       \
								"To: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                        \
								"Call-ID: %s@121.157.1.71:5060\r\n"                                                      \
								"CSeq: 111 INVITE\r\n"                                                                   \
								"User-Agent: MicroSIP/3.21.3\r\n"                                                        \
								"Content-Type: application/sdp\r\n"                                                      \
								"Content-Length:   904\r\n"                                                              \
								"\r\n"                                                                                   \
								"v=0\r\n"                                                                                \
								"o=007102720200 1668 3804 IN IP4 192.168.2.7\r\n"                                        \
								"s=Talk\r\n"                                                                             \
								"c=IN IP4 192.168.2.7\r\n"                                                               \
								"t=0 0\r\n"                                                                              \
								"a=rtcp-xr:rcvr-rtt=all:10000 stat-summary=loss,dup,jitt,TTL voip-metrics\r\n"           \
								"a=record:off\r\n"                                                                       \
								"m=audio 4000 RTP/AVP 96 97 98 0 8 18 101 99 100\r\n"                                    \
								"a=rtpmap:96 opus/48000/2\r\n"                                                           \
								"a=fmtp:96 useinbandfec=1\r\n"                                                           \
								"a=rtpmap:97 speex/16000\r\n"                                                            \
								"a=fmtp:97 vbr=on\r\n"                                                                   \
								"a=rtpmap:98 speex/8000\r\n"                                                             \
								"a=fmtp:98 vbr=on\r\n"                                                                   \
								"a=fmtp:18 annexb=yes\r\n"                                                               \
								"a=rtpmap:101 telephone-event/48000\r\n"                                                 \
								"a=rtpmap:99 telephone-event/16000\r\n"                                                  \
								"a=rtpmap:100 telephone-event/8000\r\n"                                                  \
								"a=rtcp-fb:* trr-int 1000\r\n"                                                           \
								"a=rtcp-fb:* ccm tmmbr\r\n"                                                              \
								"m=video 4002 RTP/AVP 96 99 97\r\n"                                                      \
								"a=rtpmap:96 AV1/90000\r\n"                                                              \
								"a=rtpmap:99 H264/90000\r\n"                                                             \
								"a=fmtp:99 profile-level-id=42801F\r\n"                                                  \
								"a=rtpmap:97 VP8/90000\r\n"                                                              \
								"a=rtcp-fb:* trr-int 1000\r\n"                                                           \
								"a=rtcp-fb:* ccm tmmbr\r\n"                                                              \
								"a=rtcp-fb:96 nack pli\r\n"                                                              \
								"a=rtcp-fb:96 ccm fir\r\n"                                                               \
								"a=rtcp-fb:99 nack pli\r\n"                                                              \
								"a=rtcp-fb:99 ccm fir\r\n"                                                               \
								"a=rtcp-fb:97 nack pli\r\n"                                                              \
								"a=rtcp-fb:97 nack sli\r\n"                                                              \
								"a=rtcp-fb:97 ack rpsi\r\n"                                                              \
								"a=rtcp-fb:97 ccm fir\r\n"                                                               \
								"a=rtcp-fb:* nack pli\r\n"                                                               \
								"\r\n"

#define SIP_REINVITE_ACK_SIGNALLING "ACK sip:000010200004@121.157.1.71:5060 SIP/2.0\r\n"                                                                       \
									"Via: SIP/2.0/UDP 121.157.1.71:5060;rport=5060;received=121.157.1.71;branch=z9hG4bKPjbb4e84bdb03d4fef818cd8c5f7b0c7a1\r\n" \
									"Call-ID: %s@121.157.1.71:5060\r\n"                                                                                        \
									"To: \"000010200004\" <sip:000010200004@121.157.1.71>;tag=%s\r\n"                                                          \
									"From: <sip:007102720200@192.168.2.7;ob>;tag=6e3ed61b6a294931bb394249f740dadc\r\n"                                         \
									"CSeq: 111 ACK\r\n"                                                                                                        \
									"Content-Length:  0\r\n"                                                                                                   \
									"\r\n"
// typedef struct
// {
// 	char branch[32];
// 	char tag[32];
// 	char callid[64];
// 	int audio_send_port;
// 	int video_send_port;
// 	bool rtp_recv_ing;
// } session_context_t;

// // RTP头部结构
// typedef struct
// {
// 	unsigned char version;
// 	unsigned char payloadtype; // 载荷类型 (7 bits)
// 	unsigned short sequencenum;	   // 序列号
// 	unsigned int timestamp;		   // 时间戳
// 	unsigned int ssrc;			   // 同步源标识符
// } rtp_header_t;

// // 全局变量
// int audio_send_sock;
// int video_send_sock;
// struct sockaddr_in audio_server_addr;
// struct sockaddr_in video_server_addr;
// unsigned short audio_seq_num = 0;
// unsigned short video_seq_num = 0;
// unsigned int audio_ssrc = 0x12345678;
// unsigned int video_ssrc = 0x87654321;
// int running = 1;

// static int sip_socket_fd = -1;
// static pthread_t sip_recv_thread_id;
// static pthread_t audio_recv_thread_id;
// static pthread_t video_recv_thread_id;
// static session_context_t session_ctx;

// // 初始化RTP头部
// void init_rtp_header(rtp_header_t *header, int is_audio, int marker)
// {
// 	header->version = 0x80;

// 	if (is_audio)
// 	{
// 		header->payloadtype = 0; // PCMU音频
// 		header->sequencenum = htons(audio_seq_num++);
// 		header->ssrc = htonl(audio_ssrc);
// 	}
// 	else
// 	{
// 		header->payloadtype = 99; // H264视频
// 		header->sequencenum = htons(video_seq_num++);
// 		header->ssrc = htonl(video_ssrc);
// 	}

// 	// 使用当前时间作为时间戳
// 	struct timespec ts;
// 	clock_gettime(CLOCK_REALTIME, &ts);
// 	header->timestamp = htonl(ts.tv_nsec / 1000); // 转换为微秒
// }

// // 创建空的音频RTP包 (PCMU静音包)
// int create_empty_audio_rtp(char *buffer, int marker)
// {
// 	rtp_header_t *header = (rtp_header_t *)buffer;
// 	init_rtp_header(header, 1, marker);

// 	// PCMU静音数据 (160字节，20ms的音频)
// 	unsigned char silence[160];
// 	memset(silence, 0xFF, sizeof(silence)); // PCMU静音是0xFF

// 	// 复制静音数据到包中
// 	memcpy(buffer + sizeof(rtp_header_t), silence, sizeof(silence));

// 	return sizeof(rtp_header_t) + sizeof(silence);
// }

// // 创建空的视频RTP包
// int create_empty_video_rtp(char *buffer, int marker)
// {
// 	rtp_header_t *header = (rtp_header_t *)buffer;
// 	init_rtp_header(header, 0, marker);
// 	char ual[5] = {0x00, 0x00, 0x00, 0x01, 0x41};
// 	memcpy(buffer + sizeof(rtp_header_t), ual, sizeof(ual));
// 	// 对于H.264，我们可以发送一个空的NAL单元
// 	// 这里我们只发送RTP头部，不包含有效载荷

// 	return sizeof(rtp_header_t) + sizeof(ual);
// }

// // 发送音频RTP包
// void *send_audio_rtp(void *arg)
// {
// 	char packet[1024];
// 	int packet_size;

// 	printf("Audio RTP sender started\n");

// 	while (running)
// 	{
// 		// // 创建空的音频RTP包
// 		// packet_size = create_empty_audio_rtp(packet, 0);

// 		// // 发送RTP包到服务器
// 		// sendto(audio_send_sock, packet, packet_size, 0,
// 		// 	   (struct sockaddr *)&audio_server_addr, sizeof(audio_server_addr));

// 		// printf("Sent empty audio RTP packet, size: %d bytes\n", packet_size);

// 		// 每20ms发送一个包 (50包/秒)
// 		usleep(1000000);
// 	}

// 	return NULL;
// }

// // 发送视频RTP包
// void *send_video_rtp(void *arg)
// {
// 	char packet[1024];
// 	int packet_size;

// 	printf("Video RTP sender started\n");

// 	while (running)
// 	{
// 		// 创建空的视频RTP包
// 		packet_size = create_empty_video_rtp(packet, 0);

// 		// 发送RTP包到服务器
// 		sendto(video_send_sock, packet, packet_size, 0,
// 			   (struct sockaddr *)&video_server_addr, sizeof(video_server_addr));

// 		printf("Sent empty video RTP packet, size: %d bytes\n", packet_size);

// 		// 每100ms发送一个包 (10包/秒)
// 		usleep(1000000);
// 	}

// 	return NULL;
// }

// // 初始化网络
// void init_network()
// {
// 	// 创建音频发送socket
// 	audio_send_sock = socket(AF_INET, SOCK_DGRAM, 0);
// 	if (audio_send_sock < 0)
// 	{
// 		perror("Audio socket creation failed");
// 		exit(EXIT_FAILURE);
// 	}

// 	// 创建视频发送socket
// 	video_send_sock = socket(AF_INET, SOCK_DGRAM, 0);
// 	if (video_send_sock < 0)
// 	{
// 		perror("Video socket creation failed");
// 		exit(EXIT_FAILURE);
// 	}

// 	// 绑定音频socket到本地端口
// 	struct sockaddr_in audio_local_addr;
// 	memset(&audio_local_addr, 0, sizeof(audio_local_addr));
// 	audio_local_addr.sin_family = AF_INET;
// 	audio_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	audio_local_addr.sin_port = htons(RTP_AUDIO_PORT);

// 	if (bind(audio_send_sock, (struct sockaddr *)&audio_local_addr, sizeof(audio_local_addr)) < 0)
// 	{
// 		perror("Audio bind failed");
// 		close(audio_send_sock);
// 		exit(EXIT_FAILURE);
// 	}

// 	// 绑定视频socket到本地端口
// 	struct sockaddr_in video_local_addr;
// 	memset(&video_local_addr, 0, sizeof(video_local_addr));
// 	video_local_addr.sin_family = AF_INET;
// 	video_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	video_local_addr.sin_port = htons(RTP_VIDEO_PORT);

// 	if (bind(video_send_sock, (struct sockaddr *)&video_local_addr, sizeof(video_local_addr)) < 0)
// 	{
// 		perror("Video bind failed");
// 		close(video_send_sock);
// 		exit(EXIT_FAILURE);
// 	}

// 	// 设置服务器地址
// 	memset(&audio_server_addr, 0, sizeof(audio_server_addr));
// 	audio_server_addr.sin_family = AF_INET;
// 	audio_server_addr.sin_port = htons(session_ctx.audio_send_port);
// 	audio_server_addr.sin_addr.s_addr = inet_addr(SIP_SERVER_IP);

// 	memset(&video_server_addr, 0, sizeof(video_server_addr));
// 	video_server_addr.sin_family = AF_INET;
// 	video_server_addr.sin_port = htons(session_ctx.video_send_port);
// 	video_server_addr.sin_addr.s_addr = inet_addr(SIP_SERVER_IP);

// 	printf("Network initialized\n");
// }

// static void home_monitor_btn_click(lv_event_t *ev)
// {
// 	char buffer[4096] = {0};
// 	sprintf(buffer, SIP_REGIST_SIGNALLING, "");
// 	db_log_info("\n%s\n", buffer);
// 	db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// }

// // static void home_intercom_btn_click(lv_event_t *ev)
// // {
// // }

// // static void home_media_btn_click(lv_event_t *ev)
// // {
// // }

// // static void home_mute_btn_click(lv_event_t *ev)
// // {
// // }

// // static void home_security_btn_click(lv_event_t *ev)
// // {
// // }

// static void *audio_recv_thread(void *arg)
// {
// 	// int rtp_socket_fd = -1;
// 	char buffer[4096] = {0};
// 	struct sockaddr_in client_addr;
// 	int recv_size = 0;
// 	// db_socket_udp_open(&rtp_socket_fd, 4000, false);
// 	while (session_ctx.rtp_recv_ing)
// 	{
// 		recv_size = db_socket_udp_receive(audio_send_sock, buffer, sizeof(buffer), &client_addr, 100);
// 		if (recv_size)
// 		{
// 			db_log_debug("size:%d\n", recv_size);
// 		}
// 	}
// 	// db_socket_close(rtp_socket_fd);
// 	pthread_exit(NULL);
// 	return NULL;
// }

// static void *video_recv_thread(void *arg)
// {
// 	// int rtp_socket_fd = -1;
// 	char buffer[64 * 1024] = {0};
// 	struct sockaddr_in client_addr;
// 	int recv_size = 0;
// 	// db_socket_udp_open(&rtp_socket_fd, 4002, false);
// 	while (session_ctx.rtp_recv_ing)
// 	{
// 		recv_size = db_socket_udp_receive(video_send_sock, buffer, sizeof(buffer), &client_addr, 100);
// 		if (recv_size)
// 		{
// 			db_log_debug("size:%d\n", recv_size);
// 		}
// 	}
// 	// db_socket_close(rtp_socket_fd);
// 	pthread_exit(NULL);
// 	return NULL;
// }

// // 接收RTP数据包
// // void *rtp_receiver(void *arg)
// // {
// // 	int port = *(int *)arg;
// // 	int sockfd;
// // 	struct sockaddr_in addr;
// // 	char buffer[2048];

// // 	// 创建RTP接收socket
// // 	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
// // 	{
// // 		perror("RTP socket creation failed");
// // 		return NULL;
// // 	}

// // 	memset(&addr, 0, sizeof(addr));
// // 	addr.sin_family = AF_INET;
// // 	addr.sin_addr.s_addr = htonl(INADDR_ANY);
// // 	addr.sin_port = htons(port);

// // 	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
// // 	{
// // 		perror("RTP bind failed");
// // 		close(sockfd);
// // 		return NULL;
// // 	}

// // 	printf("RTP receiver listening on port %d\n", port);

// // 	while (1)
// // 	{
// // 		struct sockaddr_in sender_addr;
// // 		socklen_t sender_len = sizeof(sender_addr);
// // 		int len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
// // 						   (struct sockaddr *)&sender_addr, &sender_len);
// // 		if (len > 0)
// // 		{
// // 			printf("Received RTP packet on port %d, length: %d bytes\n", port, len);
// // 		}
// // 	}

// // 	close(sockfd);
// // 	return NULL;
// // }

// static void *sip_recv_thread(void *arg)
// {
// 	char buffer[4096] = {0};
// 	struct sockaddr_in client_addr;
// 	int recv_size = 0;
// 	while (1)
// 	{
// 		memset(buffer, 0, sizeof(buffer));
// 		recv_size = db_socket_udp_receive(sip_socket_fd, buffer, sizeof(buffer), &client_addr, 100);
// 		if (recv_size)
// 		{
// 			db_log_info("\n%s\n", buffer);
// 			if (strstr(buffer, "Unauthorized"))
// 			{
// 				char nonce[32] = {0};
// 				char md5_buf[128] = {0};
// 				char response[64] = {0};
// 				char auth[256] = {0};
// 				extract_string_end_char_prefix(buffer, "nonce=\"", '\"', nonce, sizeof(nonce));
// 				if (strlen(nonce))
// 				{
// 					sprintf(md5_buf, "0ee522cbbd58dae1a07bcfb6d2796560:%s:76a017a894880663010d16d0337352e8", nonce);
// 					dyc_common_md5_by_data((unsigned char *)md5_buf, strlen(md5_buf), response, sizeof(response));
// 					sprintf(auth, "Authorization: Digest username=\"007102720200\", realm=\"smcom_vpbx\", nonce=\"%s\", uri=\"sip:121.157.1.71:5060\", response=\"%s\", algorithm=MD5\r\n", nonce, response);
// 					memset(buffer, 0, sizeof(buffer));
// 					sprintf(buffer, SIP_REGIST_SIGNALLING, auth);
// 					memset(nonce, 0, sizeof(nonce));
// 					db_log_info("\n%s\n", buffer);
// 					db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 				}
// 			}
// 			else if (strstr(buffer, "NOTIFY sip"))
// 			{
// 				char branch[32] = {0};
// 				char tag[32] = {0};
// 				char callid[64] = {0};
// 				extract_string_end_char_prefix(buffer, "branch=", ';', branch, sizeof(branch));
// 				extract_string_end_char_prefix(buffer, "tag=", '\r', tag, sizeof(tag));
// 				extract_string_end_char_prefix(buffer, "Call-ID: ", '@', callid, sizeof(callid));
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_NOTIFY_OK_SIGNALLING, branch, callid, tag, branch);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 			}
// 			else if (strstr(buffer, "INVITE sip"))
// 			{
// 				char rtp_port[8] = {0};
// 				extract_string_end_char_prefix(buffer, "branch=", ';', session_ctx.branch, sizeof(session_ctx.branch));
// 				extract_string_end_char_prefix(buffer, "tag=", '\r', session_ctx.tag, sizeof(session_ctx.tag));
// 				extract_string_end_char_prefix(buffer, "Call-ID: ", '@', session_ctx.callid, sizeof(session_ctx.callid));
// 				extract_string_end_char_prefix(buffer, "m=audio ", ' ', rtp_port, sizeof(rtp_port));
// 				session_ctx.audio_send_port = atoi(rtp_port);
// 				extract_string_end_char_prefix(buffer, "m=video ", ' ', rtp_port, sizeof(rtp_port));
// 				session_ctx.video_send_port = atoi(rtp_port);

// 				// trying
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_TRYING_SIGNALLING, session_ctx.branch, session_ctx.callid, session_ctx.tag);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 				// ringing
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_RINGING_SIGNALLING, session_ctx.branch, session_ctx.callid, session_ctx.tag);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 				// answering
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_ANSWERING_SIGNALLING, session_ctx.branch, session_ctx.callid, session_ctx.tag);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);

// 				pthread_t audio_thread, video_thread;

// 				// 初始化网络
// 				init_network();

// 				// 创建发送线程
// 				pthread_create(&audio_thread, NULL, send_audio_rtp, NULL);
// 				pthread_create(&video_thread, NULL, send_video_rtp, NULL);
// 			}
// 			else if (strstr(buffer, "ACK sip"))
// 			{
// 				// memset(buffer, 0, sizeof(buffer));
// 				// sprintf(buffer, SIP_REINVITE_SIGNALLING, session_ctx.tag, session_ctx.callid);
// 				// db_log_info("\n%s\n", buffer);
// 				// db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 				if (1)
// 				{
// 					session_ctx.rtp_recv_ing = true;
// 					pthread_create(&audio_recv_thread_id, NULL, audio_recv_thread, NULL);
// 					pthread_create(&video_recv_thread_id, NULL, video_recv_thread, NULL);
// 				}
// 			}
// 			else if (strstr(buffer, "INFO sip"))
// 			{
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_INFO_OK_SIGNALLING, session_ctx.branch, session_ctx.callid, session_ctx.tag);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 			}
// 			else if (strstr(buffer, "BYE sip"))
// 			{
// 				memset(buffer, 0, sizeof(buffer));
// 				sprintf(buffer, SIP_BYE_OK_SIGNALLING, session_ctx.branch, session_ctx.callid, session_ctx.tag);
// 				db_log_info("\n%s\n", buffer);
// 				db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 			}
// 			// else if (strstr(buffer, "SIP/2.0 200 OK") && strstr(buffer, "INVITE\r\n"))
// 			// {
// 			// 	memset(buffer, 0, sizeof(buffer));
// 			// 	sprintf(buffer, SIP_REINVITE_ACK_SIGNALLING, session_ctx.callid, session_ctx.tag);
// 			// 	db_log_info("\n%s\n", buffer);
// 			// 	db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);

// 			// 	memset(buffer, 0, sizeof(buffer));
// 			// 	sprintf(buffer, SIP_INFO_SIGNALLING, session_ctx.tag, session_ctx.callid);
// 			// 	db_log_info("\n%s\n", buffer);
// 			// 	db_socket_udp_send(sip_socket_fd, buffer, strlen(buffer), SIP_SERVER_IP, SIP_SERVER_PORT, 1000);
// 			// }
// 		}
// 	}
// 	pthread_exit(NULL);
// 	return NULL;
// }

// LAYOUT_ENTER_FUNC(home)
// {
// 	layout_common_background_display(0x292e37, LV_OPA_COVER, NULL, NULL, false);

// 	db_socket_udp_open(&sip_socket_fd, SIP_SERVER_PORT, false);

// 	if (pthread_create(&sip_recv_thread_id, NULL, sip_recv_thread, NULL) != 0)
// 	{
// 		db_log_error("create failed\n");
// 		return;
// 	}
// 	memset(&session_ctx, 0, sizeof(session_ctx));
// 	// session_ctx.rtp_recv_ing = true;
// 	// pthread_create(&audio_recv_thread_id, NULL, audio_recv_thread, NULL);
// 	// pthread_create(&video_recv_thread_id, NULL, video_recv_thread, NULL);

// 	// 创建线程处理RTP音频流
// 	// int port1 = RTP_AUDIO_PORT;
// 	// pthread_create(&audio_recv_thread_id, NULL, rtp_receiver, &port1);

// 	// // 创建线程处理RTP视频流
// 	// int port2 = RTP_VIDEO_PORT;
// 	// pthread_create(&video_recv_thread_id, NULL, rtp_receiver, &port2);

// 	layout_common_text_btn_create(lv_scr_act(), -1, 20, 20, 100, 50, NULL, LV_ALIGN_DEFAULT,
// 								  home_monitor_btn_click, 0x0000FF, LV_OPA_COVER, 0x00FFFF, LV_OPA_COVER,
// 								  "register", 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_small);

// 	// layout_common_text_btn_create(lv_scr_act(), -1, 20, 20, 100, 50, NULL, LV_ALIGN_DEFAULT,
// 	// 							  home_monitor_btn_click, 0x0000FF, LV_OPA_COVER, 0x00FFFF, LV_OPA_COVER,
// 	// 							  "register", 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_small);
// }

// LAYOUT_QUIT_FUNC(home)
// {
// }
// LAYOUT_DEFINE(home);

// 计算HA1 = MD5(username:realm:password)
// HA1 = MD5(007102720200:smcom_vpbx:BqPfhBDE2b) = 0ee522cbbd58dae1a07bcfb6d2796560
// 计算HA2 = MD5(Method:URI) 例如：MD5(REGISTER:sip:121.157.1.71:5060)
// HA2 = MD5(REGISTER:sip:121.157.1.71:5060) = 76a017a894880663010d16d0337352e8
// 计算response = MD5(HA1:nonce:HA2) [d2e031d5bcbc6a98fa98f84d69ad1e9]
// response = MD5(0ee522cbbd58dae1a07bcfb6d2796560:53d7eb40:76a017a894880663010d16d0337352e8) = a3cbf8fcb83e8c2656cd0e1f0a0ee61b