#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include "codec_def.h"
#include "codec_app_def.h"
#include "codec_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "openh264_decoder.h"

#ifdef __cplusplus
}
#endif

typedef struct
{
    ISVCDecoder *decoder;
    unsigned long long timestamp;
    void *user_data;
} openh264_decoder_context;

static void frame_copy(unsigned char *buf, unsigned char *pData[3], int iStride[2], int iWidth, int iHeight)
{
    int i;
    unsigned char *pPtr = NULL;

    pPtr = pData[0];
    for (i = 0; i < iHeight; i++)
    {
        memcpy(buf, pPtr, iWidth);
        buf += iWidth;
        pPtr += iStride[0];
    }

    iHeight = iHeight / 2;
    iWidth = iWidth / 2;
    pPtr = pData[1];
    for (i = 0; i < iHeight; i++)
    {
        memcpy(buf, pPtr, iWidth);
        buf += iWidth;
        pPtr += iStride[1];
    }

    pPtr = pData[2];
    for (i = 0; i < iHeight; i++)
    {
        memcpy(buf, pPtr, iWidth);
        buf += iWidth;
        pPtr += iStride[1];
    }
}

extern "C" void *openh264_decoder_open(void)
{
    SDecodingParam sDecParam = {0};
    openh264_decoder_context *ctx = (openh264_decoder_context *)malloc(sizeof(openh264_decoder_context));

    if (ctx == NULL)
    {
        return NULL;
    }
    memset(ctx, 0, sizeof(openh264_decoder_context));
    if (WelsCreateDecoder(&ctx->decoder) || (NULL == ctx->decoder))
    {
        printf("Create Decoder failed.\n");
        goto end;
    }

    memset(&sDecParam, 0, sizeof(SDecodingParam));
    sDecParam.sVideoProperty.size = sizeof(sDecParam.sVideoProperty);
    sDecParam.eEcActiveIdc = ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE;
    sDecParam.uiTargetDqLayer = UCHAR_MAX;
    sDecParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    sDecParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
    sDecParam.bParseOnly = false;
    if (ctx->decoder->Initialize(&sDecParam))
    {
        printf("Init Decoder failed.\n");
        goto end;
    }
    return ctx;
end:
    free(ctx);
    return NULL;
}

extern "C" int openh264_decoder_decode_frame(void *context, const db_hal_video_packet_t *packet, db_hal_video_frame_t *frame)
{
    openh264_decoder_context *ctx = (openh264_decoder_context *)context;
    uint8_t *pData[3] = {NULL};
    uint8_t *pDst[3] = {NULL};
    SBufferInfo sDstBufInfo;

    pData[0] = NULL;
    pData[1] = NULL;
    pData[2] = NULL;
    memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
    sDstBufInfo.uiInBsTimeStamp = ctx->timestamp++;
    ctx->decoder->DecodeFrameNoDelay(packet->data, packet->size, pData, &sDstBufInfo);

    if (sDstBufInfo.iBufferStatus == 1)
    {
        pDst[0] = sDstBufInfo.pDst[0];
        pDst[1] = sDstBufInfo.pDst[1];
        pDst[2] = sDstBufInfo.pDst[2];
        // cOutputModule.Process((void **)pDst, &sDstBufInfo, pYuvFile);
        int width = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
        int height = sDstBufInfo.UsrData.sSystemBuffer.iHeight;

        int iStride[2];
        int iWidth = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
        int iHeight = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
        iStride[0] = sDstBufInfo.UsrData.sSystemBuffer.iStride[0];
        iStride[1] = sDstBufInfo.UsrData.sSystemBuffer.iStride[1];

        // Write2File(pOutputFile, (unsigned char **)pDst, iStride, iWidth, iHeight);
        unsigned char *buf = (unsigned char *)malloc(iWidth * iHeight * 3 / 2);
        frame_copy(buf, (unsigned char **)pDst, iStride, iWidth, iHeight);
        frame->data = buf;
        frame->width = iWidth;
        frame->hight = iHeight;
        return 0;
    }
    return -1;
}

extern "C" int openh264_decoder_close(void *context)
{
    openh264_decoder_context *ctx = (openh264_decoder_context *)context;
    if (ctx == NULL)
    {
        return -1;
    }
    ctx->decoder->Uninitialize();
    WelsDestroyDecoder(ctx->decoder);
    free(ctx);
    return 0;
}