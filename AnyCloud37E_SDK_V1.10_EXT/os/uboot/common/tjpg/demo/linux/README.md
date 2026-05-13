# Linux TJpgDec demo

使用 TJpgDec 对 jpg 图像进行解码，还原为 rgb 图像。

## 使用

**配置**

修改 tjpgd.h 文件中的配置项，可以配置是否缩放，缩放比例以及输出格式。

```
#define JD_SZBUF        512 /* Size of stream input buffer */
#define JD_FORMAT       1   /* Output pixel format 0:RGB888 (3 BYTE/pix), 1:RGB565 (1 WORD/pix) */
#define JD_USE_SCALE    1   /* Use descaling feature for output */
#define JD_TBLCLIP      1   /* Use table for saturation (might be a bit faster but increases 1K bytes of code size) */
#define JD_SCALE        2   /* 0:1/1  1:1/2  2:1/4  3:1/8 */
```

**编译**

```
paul@maziot:~/work/maziot/component/TJpgDec/demo/linux$ make
gcc main.o ../../tjpgd.o -lpthread -o tJpgDec
```

**运行**

```
paul@maziot:~/work/maziot/component/TJpgDec/demo/linux$ ./tJpgDec resource/1.jpg 
Image dimensions: 900 by 506. 2236 bytes used.
OK     
```

**验证**

使用 7yuv 或者其他 YUV 播放器查看 RGB 原图，确认解码是否正常。