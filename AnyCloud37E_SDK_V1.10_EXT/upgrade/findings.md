# 显示屏异常排查分析结论

## 已知基础情况
- 用户反馈：纯黑、纯白、彩条画面显示均正常。
- 用户反馈：抗锯齿Logo边缘、黑色字体内部出现白色小点异常。
- 用户确认：相同应用层在其他SDK显示正常，因此排除应用渲染问题。
- `lcd_tp` 目录下相关文件：JD9165T数据手册、SAT070CS40I21Y03屏幕规格书、MTK平台JD9165T初始化配置、JD9165T华星光电固件/二进制文件及调试工具包。

## 提取的屏幕参数
- `SAT070CS40I21Y03-26100M031IN-1185_规格书_1010701185.pdf`：7.0英寸，1024×3(RGB)×600，RGB排列，常黑模式，1670万色，MIPI接口，驱动芯片JD9165/JD9165T。
- 电气参数：LCD_VDD典型值1.8V，AVDD典型值+5.9V，AVEE典型值-5.9V，背光8.4-10.5V/140mA。
- `初始化MTK_JD9165T_Panel_CSOT_MIPI_20251127-1(1).txt`：DSI视频模式，1024x600，VS=2，VBP=12，VFP=95，HS=2，HBP=100，HFP=100，4通道DSI，像素时钟52MHz，IO电压1.8V。
- 初始化指令包含伽马表指令`0xC8`及大量GIP/源极时序指令（`0xD0`-`0xD6`等），因此即使纯色正常，错误/过时的初始化表也可能导致灰度/边缘显示异常。

## 当前SDK配置
- `AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_lcd.dtsi` 已启用`JD9165T_CSOT7P0_1024x600_4L`节点：1024x600，4通道MIPI，RGB序列，24位DSI色彩格式，HFP/HBP/HSA 100/100/2，VBP/VFP/VSA 12/95/2。
- 配置基本与厂商初始化文本一致，仅SDK使用`panel-dsi-clk = <360>`，测试板同款JD9165T使用`panel-dsi-clk = <400>`。
- U-Boot显示驱动存在MIPI设备树解析流程（`mipi_panel_init`调用`ak_lcd_parameter_parse_dt2`），设备树配置生效。
- `indoor/3rdparty/lvgl-8.4.0/lv_conf.h` 使用`LV_COLOR_DEPTH 32`和`LV_COLOR_SCREEN_TRANSP 1`。
- `indoor/driver/gui_display/driver_gui_display_anyka.c` 通过安凯TDE将LVGL的`GP_FORMAT_ARGB8888`转换为帧缓冲`GP_FORMAT_RGB888`，通过`/dev/fb0`以24位RGB888输出。半透明/抗锯齿像素在此转换环节最易出现异常。

## 故障原因推理
- 纯黑、纯白、彩条正常 → 排除屏幕玻璃损坏、坏点、MIPI线路严重错误、分辨率错误、基础上电时序异常。
- 异常仅出现在Logo抗锯齿边缘、字体内部 → 指向灰度/透明通道/格式处理、伽马/驱动调校、DSI时序裕量问题，非屏幕硬件故障。
- 最高概率原因：SDK显示链路配置问题，包括ARGB/RGBA/ABGR解析、RGB/BGR顺序、TDE格式转换、24位帧缓冲封装、DSI时钟裕量。
- 屏幕初始化/伽马仍有可能性，但当前配置与`lcd_tp`中的厂商文本一致，暂不优先排查；仅在厂商提供更新的固件/参数表时复核。

## 实验一
- 在`indoor/driver/gui_display/driver_gui_display_anyka.c`中添加`GUI_DISPLAY_FORCE_SOFT_ARGB_TO_FB 1`。
- 启用后，`_driver_gui_display_write()`绕过所有TDE转换，强制调用`_lv_argb_to_fb_back_area_soft()`处理LVGL帧数据。
- 执行`make -C /home/xiaoxiao/workspace/QT300/indoor`编译成功。
- 测试固件：`indoor/build/DOORBELL_anyka_release.BIN`。
- 判定逻辑：若异常消失 → 安凯TDE格式转换问题；若不变 → 继续测试字节序。

## 板级测试一结果
- 用户通过`make sd PARTS="app"`打包测试，问题依旧存在。
- 结论：排除TDE转换为唯一原因（实验一已强制软件转换）。

## 实验二配置
- `indoor/3rdparty/lvgl-8.4.0/src/misc/lv_color.h` 定义`lv_color32_t`内存顺序为`blue, green, red, alpha`。
- 原有`_lv_argb_to_fb_back_area_soft()`将输入像素按`red, green, blue, alpha`处理。
- 下一测试：按BGRA读取源像素，输出RGB888帧缓冲。
- 实验二保持绕过TDE，仅修改软件像素解析：源BGRA → 目标RGB888。若改善 → 格式不匹配；若不变 → 排查帧缓冲字节序/RGB/BGR。

## 实验二结果
- 用户反馈：异常仍存在，且整体颜色错乱。
- 结论：BGRA解析不适用于当前应用/显示适配链路，恢复应用层原有逻辑，准备内核测试。

## 内核/根文件系统重新初始化发现
- 运行脚本`AnyCloud37E_SDK_V1.10_EXT/bridge/main.sh`加载`/usr/modules/ak_fb.ko`。
- `ak_fb.ko`支持参数`lcd_ctl_force_init`，脚本中已有注释示例`insmod /usr/modules/ak_fb.ko lcd_ctl_force_init=1`。
- 作用：让Linux内核重新初始化LCD控制器/屏幕，而非继承U-Boot状态。
- 实验三固件已编译：`br`重建应用/根文件系统，`mi all`打包全分区镜像。
- 板级结果：黑屏。停止使用`lcd_ctl_force_init=1`，该参数与主板时序不兼容。

## 帧缓冲翻转发现
- 显示适配使用双缓冲，调用`_lv_fb_layer_phyaddr_swap()`后立即拷贝脏区域。
- 启用`FB_ACTIVATE_VBL`后，`FBIOPUT_VSCREENINFO`仅调度垂直同步翻转，不保证完成。
- 若翻转调度后立即同步缓冲，可能导致抗锯齿/透明像素异常，纯色正常。
- 翻转后增加等待测试：异常仍存在。排除简单的VBL竞争问题。

## LCDC RGB序列发现
- `bridge/EVB_CBDM_AK3760E_V1.0.1.dts` 设置`lcd-logo-rgb-seq = <0>`，注释：0=BGR，1=RGB。
- `os/uboot/drivers/video/ak_fb.c` 的`lcdc_rgb_display()`将该值写入LCDC寄存器，注释：0=RGB，1=BGR。
- 两处注释冲突，该配置用于DMA显示，不只是开机Logo，是合理的下一实验方向。

## 实验五结果
- 用户反馈：修改`lcd-logo-rgb-seq`后整体颜色不对，Logo边缘、字体白点/绿点和锯齿仍存在。
- 结论：LCDC输入RGB/BGR顺序不是根因，且该改动会引入新的颜色错误；已恢复`lcd-logo-rgb-seq = <0>`。
- 注意：`build_kernel()` 会从 `bridge/EVB_CBDM_AK3760E_V1.0.1.dts` 复制到 `os/kernel/arch/arm/boot/dts/EVB_CBDM_AK3760E_V1.0.1.dts`。两处都需要保持一致；本轮已确认生成的 `cloudOS.dtb` 内为 `<0x00>`。

## 实验六假设
- 当前显示适配使用双缓冲和局部脏区刷新。
- 若LVGL的抗锯齿/透明像素需要和当前屏幕背景混合，而后台缓冲对应区域仍是旧内容，就可能只在边缘灰阶、文字内部细节处出现白点/绿点，纯色和彩条仍正常。
- 本实验在每批脏区开始前，将当前前台可见缓冲完整同步到后台缓冲，再执行本批局部绘制，验证是否为后台缓冲陈旧导致。

## 实验六结果
- 用户反馈：颜色恢复正常，但Logo边缘、`your skills`字体白/绿点和锯齿仍然存在。
- 结论：前后台framebuffer同步不是根因。已撤销该实验代码，同时撤销实验四的VBL等待改动，避免后续实验混入无效变量。

## 实验七假设
- 当前`LV_COLOR_DEPTH=32`且`LV_COLOR_SCREEN_TRANSP=1`，LVGL flush buffer 带透明通道。
- 若当前SDK的显示端对抗锯齿/透明像素到RGB888 framebuffer的混合处理与LVGL期望不一致，纯色和彩条会正常，但字体内部和图片边缘会出现白/绿点或异常锯齿。
- 本实验将`LV_COLOR_SCREEN_TRANSP`改为`0`，让LVGL输出不透明RGB结果，验证透明链路是否为根因。

## 实验七结果
- 用户反馈：异常不变。
- 结论：LVGL屏幕透明输出不是根因。已恢复`LV_COLOR_SCREEN_TRANSP=1`。

## 实验八取证方案
- 在显示驱动中增加一次性dump：启动后约5秒，将当前可见front framebuffer保存为`/app/data/fb0_visible.ppm`。
- 判定逻辑：
  - 若PPM图片中也能看到Logo边缘异常、字体白/绿点，说明异常已经写入framebuffer内存，继续查SDK显示转换/缓存/flush链路。
  - 若PPM图片正常但屏上异常，说明framebuffer内容正常，问题在LCDC输出、MIPI传输、JD9165T灰阶/伽马/源极调校或面板侧。

## 实验八结果
- 用户将`fb0_visible.ppm`拷贝到工作区根目录。
- 该PPM头部为1024x600 RGB888，但文件长度略短于完整帧；补齐后查看有效画面区域，`Your skills`文字和左上LVGL logo的像素正常，没有屏上反馈的白/绿点。
- 结论：异常不是应用、LVGL、TDE或framebuffer内存内容造成的；问题发生在framebuffer之后的LCDC/MIPI/屏IC灰阶输出链路。`anyka_logo`也有锯齿，进一步支持问题在启动显示链路/面板输出，而非app。

## 实验九假设
- 当前`panel-dsi-bllp-mode=<1>`表示BLLP空白段进入LP模式。
- 若DSI在行/帧空白期间频繁LP/HS切换导致灰阶边缘采样不稳，纯黑、纯白和彩条仍可能正常，但抗锯齿边缘和文字细节会出现白/绿点。
- 本实验将`panel-dsi-bllp-mode`改为`<0>`，让空白段发送blanking packet，减少LP/HS切换影响。

## 实验九结果
- 用户反馈：异常仍然一样。
- 结论：BLLP空白段LP/blanking packet模式不是根因，恢复`panel-dsi-bllp-mode=<1>`。

## 实验十假设
- framebuffer为RGB888且内容正常，但屏上灰阶边缘出现白/绿点。
- 若面板/桥接链路实际按18-bit RGB666接收，而SDK按24-bit RGB888发送，纯黑、纯白、纯色块可能仍看起来正常，灰阶和抗锯齿边缘更容易出现颜色量化/通道错位噪点。
- 本实验将`panel-dsi-if-color-coding`从`0x05`改为`0x04`，`panel-dsi-pix-format`从`0x03`改为`0x02`，测试18-bit loosely packed输出。

## 实验十结果
- 用户反馈异常仍然一样。
- 结论：DSI 18-bit loosely packed 输出不是根因，恢复 `panel-dsi-if-color-coding=<0x05>` 和 `panel-dsi-pix-format=<0x03>`。

## 实验十一结果
- 在应用显示适配层使用 `O_SYNC` 打开 `/dev/fb0`，并在翻转前对脏区执行 `msync(MS_SYNC)`，验证是否为用户态 mmap 缓存同步问题。
- 用户反馈异常仍然一样。
- 结合 `ak_fb.ko` 使用 `dma_alloc_from_coherent` 的符号信息，应用侧 mmap 缓存刷新不是当前高概率根因；已撤销该实验代码。

## 实验十二假设
- `fb0_visible.ppm` 内容正常，且 U-Boot/启动 logo 与应用字体边缘都异常，问题继续定位在 framebuffer 之后。
- `ak_fb.ko` 模块未暴露 gamma/dither 参数，但 DTS 中 `panel-dsi-pix-fifo-send-level=<512>` 为最高 FIFO 水位。
- 若像素 FIFO 水位/DSI 取数裕量在灰阶快速变化处不稳定，纯色和彩条可能正常，但抗锯齿边缘、细字体和 logo 过渡色会出现白/绿点。
- 本实验将 `panel-dsi-pix-fifo-send-level` 降为 `<256>`，只改 DTB，验证 DSI FIFO 水位影响。

## 实验十二结果
- 用户反馈异常仍然一样。
- 结论：DSI 像素 FIFO 发送水位不是根因，恢复 `panel-dsi-pix-fifo-send-level=<512>`。

## 实验十三假设
- 前面实验十只改变了主控 DSI packet 色深/打包方式，但当前 JD9165T init-list 没有显式发送 MIPI DCS `0x3A` 设置面板侧 pixel format。
- 若面板内部默认格式或上电残留状态与主控24-bit RGB888不完全一致，纯色可能正常，但灰阶边缘、抗锯齿和细字体更容易出现白/绿点。
- 本实验在 `Sleep Out (0x11)` 前增加 `0x3A 0x77`，显式设置面板侧为 24-bit RGB888；只改 DTB。

## 实验十三结果
- 用户反馈区别不大，感觉不到有变化。
- 结论：面板侧 DCS pixel format 默认值不是主因，撤销 `0x3A 0x77`，避免保留无效初始化命令。

## 实验十四假设
- 当前 `fb0` 内容正常，图像格式/RGB顺序/色深/FIFO/BLLP/应用缓存实验均无效，问题继续集中在 LCDC/MIPI/面板模拟输出链路。
- 若 MIPI DPHY 在 LP/HS 切换、时钟/数据 lane 时序边缘裕量不足，可能在灰阶细节和抗锯齿边缘出现轻微彩点/白点，而纯色和彩条仍看起来正常。
- 本实验将 `panel-dsi-t-pre`、`panel-dsi-t-post`、`panel-dsi-tx-gap` 从 `<1>` 提到 `<3>`，验证物理时序裕量方向。

## 实验十四结果
- 用户反馈还是没有感觉。
- 结论：常规 DSI HS/LP 物理时序裕量不是主因，恢复 `panel-dsi-t-pre/t-post/tx-gap=<1>`。

## 当前阶段结论
- 已确认 framebuffer 抓图正常而实物屏异常，问题在 framebuffer 之后。
- 多轮实验已排除或明显降权：应用渲染/TDE、LVGL透明、前后缓冲同步、mmap cache、RGB/BGR、DSI 18/24-bit打包、BLLP、DSI FIFO水位、面板 DCS pixel format、常规 DSI HS/LP时序裕量。
- 现在剩余高概率方向只剩两类：屏厂 JD9165T gamma/source/GIP 初始化调参不匹配，或屏/FPC/主板 MIPI/电源/面板本体存在硬件一致性问题。
- 区分这两类最有效的是硬件三角互换：同板换同型号屏/排线，同屏换板，或同板同屏换一条FPC。

## 实验十五假设
- `ak_fb.ko` 暴露 `fb_shadow` 和 `fb_shadow_dmasize` 参数，模块字符串显示会注册 `ak-lcd-shadow`，并有 `shadow fb%d` 初始化日志。
- 应用原本硬编码 `/dev/fb0`，若只启用 `fb_shadow=1` 可能仍写普通 framebuffer，测不到 shadow 路径。
- 本实验同时启用 `ak_fb.ko fb_shadow=1` 并将应用 framebuffer 路径改为 `/dev/fb1`，验证内核 shadow framebuffer/LCDC 缓冲路径是否改善屏上异常。
- 判定：若明显改善，问题在普通 framebuffer 到 LCDC 的取数/缓冲路径；若黑屏，说明 `/dev/fb1` shadow 路径不适合当前应用；若无变化，再撤销并转向 gamma/source/GIP 软件调参。

## 实验十五结果
- 用户反馈 satozlogo 能显示但锯齿没有变化，应用日志出现 `[ERROR][_driver_gui_display_open:0376] open`。
- 代码第376行对应 `open(FB_PATH, O_RDWR)`，本实验中 `FB_PATH=/dev/fb1`，说明应用没有真正打开 shadow framebuffer。
- 结论：`fb_shadow=1 + /dev/fb1` 不能作为有效显示路径验证；已恢复应用 `/dev/fb0` 和 rootfs 正常 `insmod /usr/modules/ak_fb.ko`。

## 实验十六假设
- 当前 framebuffer 内容正常，且前面 DSI格式/时序/缓存实验均无效，继续从面板 IC 灰阶路径排查。
- JD9165T 规格书说明标准 DCS `GAMSET(0x26)` 可选择 4 条 Gamma 曲线，参数 `0x01/0x02/0x04/0x08` 分别对应 Curve 1-4。
- 本实验在 `Sleep Out (0x11)` 前增加 `0x00 0x15 0x26 0x02 0xFFF`，只选择 Gamma Curve 2，不直接改私有 `0xC8` gamma 表。
- 判定：若锯齿/白绿点或灰阶过渡明显变化，说明面板 gamma 灰阶路径参与问题；若完全不变，再转向 source/GIP 私有参数或直接改 `0xC8` 表。

## 实验十六结果
- 用户反馈：实验十六固件能正常使用，但显示效果没有太大区别。
- 结论：标准 DCS `GAMSET(0x26)=0x02` 对当前白/绿点和锯齿问题影响很小，Gamma Curve 2 方向降权。
- 已撤销 `0x00 0x15 0x26 0x02 0xFFF`，当前 app/rootfs 已恢复 `/dev/fb0` 和正常 `ak_fb.ko`，DTB 仅保留用户已有的 `panel-dsi-clk=<360>`。
