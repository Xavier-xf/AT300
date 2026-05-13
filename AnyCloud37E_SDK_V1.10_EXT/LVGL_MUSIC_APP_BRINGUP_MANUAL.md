# LVGL Music App 调试手册

## 1. 当前目标

本阶段目标是让 `AnyCloud37E_SDK_V1.10_EXT` 启动后运行 `indoor` 应用，并显示 LVGL 官方示例中的音乐播放器界面。

当前 LCD 和 TP 已经调通：

- LCD 分辨率：`1024x600`
- TP 输入节点：`/dev/input/event0`
- 应用启动路径：`/app/app/SAT_ANYKA.BIN`

## 2. 问题背景

原 EXT 包中的 `SAT_ANYKA.BIN` 会反复报错：

```text
ak_vo_open: VO resolution param is invalid
ak_vo_open failed![134217735]
```

该问题属于旧应用使用 Anyka VO 接口时分辨率参数不匹配，不是 LCD 或 TP 驱动问题。新接入的 `indoor` 应用不走 `ak_vo_open`，而是通过 `/dev/fb0` 和 TDE 图形库刷新 LVGL 画面。

## 3. 数据和代码来源

应用工程来源：

```text
../indoor/
```

关键文件：

- `indoor/sources/user/user_app.c`
  - 调用 `lv_init()`
  - 调用 `lv_port_disp_init()`
  - 调用 `lv_port_indev_init()`
  - 调用 `lv_demo_music()`
- `indoor/3rdparty/lvgl-8.4.0/`
  - LVGL 8.4.0 源码和官方 demo
- `indoor/3rdparty/lvgl-8.4.0/lv_conf.h`
  - `LV_USE_DEMO_MUSIC = 1`
  - `LV_COLOR_DEPTH = 32`
- `indoor/driver/gui_display/driver_gui_display.h`
  - `MY_DISP_HOR_RES = 1024`
  - `MY_DISP_VER_RES = 600`
- `indoor/driver/input_event/driver_input_event_anyka.c`
  - 输入设备固定为 `/dev/input/event0`

## 4. 显示和触摸链路

显示链路：

```text
LVGL -> lv_port_disp.c -> db_hal_gui_display_write()
     -> driver_gui_display_anyka.c -> /dev/fb0 + libplat_tde.so
```

触摸链路：

```text
JD9165T TP -> jadard_touch.ko -> /dev/input/event0
           -> driver_input_event_anyka.c -> lv_port_indev.c -> LVGL
```

因此应用层依赖前面 LCD/TP 的结果：

- `/dev/fb0` 必须存在
- `/dev/input/event0` 必须存在
- 屏参和 TP 坐标都必须是 `1024x600`

## 5. 修改的文件

### `indoor/Makefile`

新增 Anyka 工具链路径：

```make
TOOLCHAIN_PATH ?= /opt/arm-anykav500-linux-uclibcgnueabi/bin
```

构建方式改为：

```sh
cmake -S . -B build
cmake --build build -j16
```

这样可以避免旧 `build/` 目录中残留 `/home/wxj/...` 路径。

### `indoor/CMakeLists.txt`

Anyka 编译器改为 EXT SDK 实际使用的工具链：

```cmake
arm-anykav500-linux-uclibcgnueabi-gcc
arm-anykav500-linux-uclibcgnueabi-g++
```

Release C++ 标准从 `c++17` 改成 `c++1y`，原因是当前工具链 GCC 4.9.4 不支持 `-std=c++17`。

### `AnyCloud37E_SDK_V1.10_EXT/build.sh`

在 `build_rootfs()` 中增加自动应用打包：

```sh
make -C ../indoor clean
make -C ../indoor
cp -rf ../indoor/build/DOORBELL_anyka_release.BIN rootfs/resource/app/app/SAT_ANYKA.BIN
```

这样 EXT 打包时会自动把 `indoor` 产物改名成系统默认启动的 `SAT_ANYKA.BIN`。

## 6. 编译方式

单独编译应用：

```sh
cd /home/xiaoxiao/workspace/QT300/indoor
make clean
make
```

输出文件：

```text
indoor/build/DOORBELL_anyka_release.BIN
```

完整打包 EXT：

```sh
cd /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT
./build.sh -a
```

该命令会：

1. 编译 `indoor`
2. 复制为 `rootfs/resource/app/app/SAT_ANYKA.BIN`
3. 重新生成 `app.sqsh4`
4. 同步到 `tools/burntool/platform/`

## 7. 烧录文件

使用：

```text
tools/burntool/config_64M_spinor.txt
```

重点烧录分区：

```text
platform/root.sqsh4 -> ROOTFS
platform/usr.sqsh4  -> USR
platform/app.sqsh4  -> APP
platform/uImage     -> KERNEL
platform/cloudOS.dtb -> DTB
```

本次应用层主要更新的是：

```text
tools/burntool/platform/app.sqsh4
```

如果 LCD/TP 内核部分没有变化，理论上只更新 `APP` 分区即可；首次整体验证建议全量烧录。

## 8. 板端验证

启动后确认应用不再报旧 VO 错误：

```sh
dmesg | grep -i "ak_vo_open\|jadard"
```

确认显示设备：

```sh
ls /dev/fb0
fbset
```

确认触摸设备：

```sh
ls /dev/input
cat /proc/bus/input/devices
hexdump /dev/input/event0
```

成功现象：

- 屏幕显示 LVGL 音乐播放器界面
- 点击播放、暂停、列表等 LVGL 控件有响应
- 串口不再持续刷 `ak_vo_open failed`

## 9. 维护注意事项

后续只改 LVGL 应用逻辑时，直接执行：

```sh
cd AnyCloud37E_SDK_V1.10_EXT
./build.sh -a
```

后续只改 LCD/TP DTS 或内核驱动时，必须先重新编内核：

```sh
cd AnyCloud37E_SDK_V1.10_EXT/os
./build_kernel.sh
./build_kernel.sh -i
cd ..
./build.sh -a
```

注意事项：

- 不要把应用启动名改掉，`daemon_client` 默认拉起 `/app/app/SAT_ANYKA.BIN`
- 不要把 `indoor` 的显示路径改回 `ak_vo_open`，当前验证路径是 `/dev/fb0 + TDE`
- 如果 TP 事件节点变成 `event1`，需要同步修改 `driver_input_event_anyka.c` 中的 `EVDEV_NAME`
- 如果屏幕旋转或坐标反向，优先检查 `driver_input_event_anyka.c` 中的坐标映射和镜像逻辑

