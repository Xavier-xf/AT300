# TJpgDec - 轻量级JPEG解码器

官网链接：http://elm-chan.org/fsw/tjpgd/00index.html

TJpgDec是一个通用的JPEG图像解压缩器模块，针对小型嵌入式系统进行了高度优化。它的内存消耗非常低，因此可以集成到微型微控制器中，例如AVR，8051，PIC，Z80，Cortex-M0等。

## 功能 

- 平台无关。用ANSI-C编写。

- 易于使用的主模式操作。

- 完全可重入的体系结构。

- 很小的内存空间：

    - 3K字节的RAM用于与图像尺寸无关的工作区域。

    - 文本和常量的ROM大小为3.5-8.5K字节。

- 输出格式：

    - 缩放比例：减压时可选择1 / 1、1 / 2、1 / 4或1/8。

    - 像素格式：RGB888或RGB565可预配置。

## 应用接口 

有两个API函数可以分析和解压缩JPEG图像。

- [jd_prepare](http://elm-chan.org/fsw/tjpgd/en/prepare.html) - jd_prepare - 准备解压缩JPEG图像

- [jd_decomp](http://elm-chan.org/fsw/tjpgd/en/decomp.html) - jd_decomp - 执行以解压缩JPEG图像

## I/O 函数 

为了输入JPEG数据并输出解压缩的像素，TJpgDec需要两个用户定义的I / O功能。

- [Input funciotn](http://elm-chan.org/fsw/tjpgd/en/input.html) - 输入功能 - 从输入流中读取JPEG数据

- [Output function](http://elm-chan.org/fsw/tjpgd/en/output.html) - 输出功能 - 将像素数据写入输出设备

## 资源 

TJpgDec模块是为教育，研究和开发开放的免费软件。您可以在个人项目或商业产品中使用，修改和/或重新分发它，而不受您的责任限制。

- Read first: [TJpgDec Application Note](http://elm-chan.org/fsw/tjpgd/en/appnote.html) August 13, 2012

- Download: [TJpgDec R0.01c](http://elm-chan.org/fsw/tjpgd/tjpgd1c.zip) March 16, 2019

- Download: [TJpgDec Sample Projects](http://elm-chan.org/fsw/tjpgd/tjsample.zip) (AVR, PIC24, LPC1114 and Win32) March 16, 2019

- [Demo Movie](http://elm-chan.org/fsw/tjpgd/tjdemo.mp4) (MP4/3MB)