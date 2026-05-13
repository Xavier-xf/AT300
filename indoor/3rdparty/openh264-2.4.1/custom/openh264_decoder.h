#ifndef _OPENH264_DECODER_H_
#define _OPENH264_DECODER_H_

#include "db_hal_driver.h"

void *openh264_decoder_open(void);

int openh264_decoder_decode_frame(void *context, const db_hal_video_packet_t *packet, db_hal_video_frame_t *frame);

int openh264_decoder_close(void *context);

#endif // _OPENH264_DECODER_H_