/*------------------------------------------------*/
/* TJpgDec Quick Evaluation Program for PCs       */
/*------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tjpgd.h"
#include "integer.h"

#define _DEBUG_ 0

#if _DEBUG_ == 1
#define debug       printf
#else
#define debug
#endif

/* 用户定义设备标识 */
typedef struct
{
    FILE *fp;      /* 用于输入函数的文件指针 */
    BYTE *fbuf;    /* 用于输出函数的帧缓冲区的指针 */
    UINT wfbuf;    /* 帧缓冲区的图像宽度[像素] */
} IODEV;

int input_cnt = 0;
int output_cnt = 0;

/*------------------------------*/
/*      用户定义input funciton  */
/*------------------------------*/

UINT in_func (JDEC *jd, BYTE *buff, UINT nbyte)
{
    IODEV *dev = (IODEV *)jd->device;  /* Device identifier for the session (5th argument of jd_prepare function) */    

    input_cnt++;
    debug("input count = %d\r\n", input_cnt);

    if (buff)
    {
        /* 从输入流读取一字节 */
        return (UINT)fread(buff, 1, nbyte, dev->fp);
    }
    else
    {
        /* 从输入流移除一字节 */
        return fseek(dev->fp, nbyte, SEEK_CUR) ? 0 : nbyte;
    }
}


/*------------------------------*/
/*      用户定义output funciton */
/*------------------------------*/
#if (JD_FORMAT == 0)
// RGB888
UINT out_func (JDEC *jd, void *bitmap, JRECT *rect)
{
    IODEV *dev = (IODEV *)jd->device;
    BYTE *src, *dst;
    UINT y, bws, bwd;

    output_cnt++;
    debug("output count = %d\r\n", output_cnt);

    /* 输出进度 */
    if (rect->left == 0)
    {
        printf("%lu%%\r\n", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* 拷贝解码的RGB矩形范围到帧缓冲区(RGB888配置) */
    src = (BYTE *)bitmap;
    /* 目标矩形的左上 */
    dst = dev->fbuf + 3 * (rect->top * dev->wfbuf / (1 << JD_SCALE) + rect->left);
    bws = 3 * (rect->right - rect->left + 1);           /* 源矩形的宽度[字节] */
    bwd = 3 * dev->wfbuf / (1 << JD_SCALE);             /* 帧缓冲区宽度[字节] */
    for (y = rect->top; y <= rect->bottom; y++)
    {
        memcpy(dst, src, bws);   /* 拷贝一行 */
        src += bws;
        dst += bwd;  /* 定位下一行 */
    }

    return 1;    /* 继续解码 */
}
#elif (JD_FORMAT == 1)
// RGB565
UINT out_func (JDEC *jd, void *bitmap, JRECT *rect)
{
    IODEV *dev = (IODEV *)jd->device;
    BYTE *src, *dst;
    UINT y, bws, bwd;

    output_cnt++;
    debug("output count = %d\r\n", output_cnt);

    /* 输出进度 */
    if (rect->left == 0)
    {
        printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* 拷贝解码的RGB矩形范围到帧缓冲区(RGB565配置) */
    src = (BYTE *)bitmap;
    /* 目标矩形的左上 */
    dst = dev->fbuf + 2 * (rect->top * dev->wfbuf / (1 << JD_SCALE) + rect->left);
    bws = 2 * (rect->right - rect->left + 1);       /* 源矩形的宽度[字节] */
    bwd = 2 * dev->wfbuf / (1 << JD_SCALE);         /* 帧缓冲区宽度[字节] */
    for (y = rect->top; y <= rect->bottom; y++)
    {
        memcpy(dst, src, bws);   /* 拷贝一行 */
        src += bws;
        dst += bwd;  /* 定位下一行 */
    }

    return 1;    /* 继续解码 */
}
#endif

/*------------------------------*/
/*        主程序                */
/*------------------------------*/

int main (int argc, char *argv[])
{
    void *work;       /* 指向解码工作区域 */
    JDEC jdec;        /* 解码对象 */
    JRESULT res;      /* TJpgDec API的返回值 */
    IODEV devid;      /* 用户定义设备标识 */
    FILE *outfp;
    int cnt = 0;
    int len = 0;

    /* 打开一个JPEG文件 */
    if (argc < 2) return -1;
    devid.fp = fopen(argv[1], "rb");
    if (!devid.fp) return -1;

    /* 分配一个用于TJpgDec的工作区域 */
    work = malloc(3100);

    /* 准备解码 */
    res = jd_prepare(&jdec, in_func, work, 3100, &devid);
    if (res != JDR_OK)
    {
        printf("Failed to prepare: rc = %d\n", res);
        return -1;
    }

    /* 准备好解码。图像信息有效 */
    printf("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

#if (JD_FORMAT == 0)
    devid.fbuf = malloc(3 * jdec.width * jdec.height); /* 输出图像的帧缓冲区(RGB888配置) */
#elif (JD_FORMAT == 1)
    devid.fbuf = malloc(3 * jdec.width * jdec.height); /* 输出图像的帧缓冲区(RGB565配置) */
#endif
    devid.wfbuf = jdec.width;

    res = jd_decomp(&jdec, out_func, JD_SCALE);   /* 开始1/1缩放解码 */
    if (res != JDR_OK)
    {
        printf("Failed to decompress: rc=%d\n", res);
    }

    /* 解码成功。你在这里已经解码图像到帧缓冲区 */
    printf("\rOK     \n");

    /* 存图 */
    len = 3 * jdec.width * jdec.height;
    outfp = fopen("output.rgb888", "wb");
    cnt = fwrite(devid.fbuf, 1, len, outfp);
    if(cnt != len)
    {
        printf("Failed to decompress: cnt=%d\n", cnt);
        return -1;
    }
    fclose(outfp);

    free(devid.fbuf);       /* 释放帧缓冲区 */
    free(work);             /* 释放工作区域 */
    fclose(devid.fp);       /* 关闭JPEG文件 */
    return res;
}
