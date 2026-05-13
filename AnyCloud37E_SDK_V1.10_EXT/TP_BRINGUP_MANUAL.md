# JD9165T TP 调试手册

## 1. 当前驱动 IC

当前 TP 驱动 IC 为 **Jadard JD9165T**，Linux input 设备名为 `jadard-touchscreen`，驱动模块名为 `jadard_touch.ko`。

最终板端验证结果：

```text
input: jadard-touchscreen as /devices/virtual/input/input0
jadard_input_register, input device registered.
jadard_ts_register_interrupt: irq enabled at IRQ: 133
/dev/input/event0
```

## 2. 数据来源

TP 原始资料位于仓库根目录的 `lcd_tp/`：

- 驱动源码：`lcd_tp/jdchipset-JD9165T_V02.0A_RK_IIC/`
- 厂商压缩包：`lcd_tp/jdchipset-JD9165T_V02.0A_RK_IIC.zip`
- 固件建议：`lcd_tp/JD9165T_CSOT7P0_VER3D_CID08_4L_LV_20260421.bin`
- 调试文档：`lcd_tp/Jadard TP调试指导 only for Linux && RTOS(1).pdf`
- 移植文档：`lcd_tp/Jadard_TP_Driver_Porting_Guide_SPRD&QM_V1.07(1).pdf`
- 手势唤醒文档：`lcd_tp/TP手势唤醒功能调试(1).pdf`

屏幕分辨率和坐标范围来自已点亮的同屏 LCD：`1024x600`。

## 3. 添加的目录和文件

把厂商 TP 驱动目录复制到 EXT 内核：

```text
os/kernel/drivers/input/touchscreen/jadard/
```

该目录包含：

```text
Kconfig
Makefile
jadard_common.c/.h
jadard_debug.c/.h
jadard_ic_JD9165T.c/.h
jadard_module.c/.h
jadard_platform.c/.h
jadard_sorting.c/.h
```

## 4. 修改的文件

内核驱动接入：

- `os/kernel/drivers/input/touchscreen/Kconfig`
  - 增加 `TOUCHSCREEN_JADARD_CHIPSET`
  - source `drivers/input/touchscreen/jadard/Kconfig`
- `os/kernel/drivers/input/touchscreen/Makefile`
  - 增加 `obj-$(CONFIG_TOUCHSCREEN_JADARD_MODULE) += jadard/`
- `os/kernel/arch/arm/configs/anycloud_ak37e_mini_defconfig`
  - `CONFIG_TOUCHSCREEN_JADARD_CHIPSET=y`
  - `CONFIG_TOUCHSCREEN_JADARD_MODULE=m`

板级 DTS：

- `bridge/EVB_CBDM_AK3760E_V1.0.1.dts`
- `os/kernel/arch/arm/boot/dts/EVB_CBDM_AK3760E_V1.0.1.dts`

最终 TP 节点：

```dts
&i2c2 {
    touch-screen@68 {
        compatible = "jadard,jdcommon";
        reg = <0x68>;
        pinctrl-names = "default";
        pinctrl-0 = <&goodx9xx_pins>;
        jadard,irq-gpio = <&gpio 77 1>;
        jadard,rst-gpio = <&gpio 76 1>;
        jadard,panel-max-points = <10>;
        jadard,int-is-edge = <1>;
        jadard,stylus = <0>;
        jadard,panel-coords = <0 1024 0 600>;
        interrupt-parent = <&gpio>;
        interrupts = <77 1>;
        interrupts-names = "irq-gpios";
        status = "okay";
    };
};
```

rootfs 和启动：

- `bridge/main.sh`
  - 增加 `insmod /usr/modules/jadard_touch.ko`
- `rootfs/build_rootfs.sh`
  - 把编译出的 `jadard_touch.ko` 复制到 `rootfs/usr/modules/`

驱动兼容补丁：

- `os/kernel/drivers/input/touchscreen/jadard/jadard_common.h`
  - 为 Linux 4.4 补空宏 `MODULE_IMPORT_NS`
  - 关闭 `JD_USB_DETECT_CALLBACK`，避免 `power_supply_*` 未导出导致模块链接失败

## 5. 调试过程

最初 DTS 使用 `reg = <0x14>`，模块能加载但 probe 失败：

```text
jadard_tp: probe of 2-0014 failed with error -7
Could not find Jadard Chipset
```

板端扫描 I2C2：

```sh
i2cdetect -y 2
```

结果只有 `0x68` 有 ACK，因此将 TP 节点改为 `touch-screen@68` 和 `reg = <0x68>`。重烧新 DTB 后，probe 成功，`i2cdetect` 中 `0x68` 显示 `UU`，表示该地址已被内核驱动占用。

不要使用 `new_device` 临时创建设备来验证该驱动。Jadard 驱动依赖 DTS 中的 GPIO 和坐标属性，动态创建的 I2C client 没有 `of_node`，会导致 `jadard_parse_dt()` 异常。

## 6. 编译和烧录

改 DTS 或驱动后，在 `AnyCloud37E_SDK_V1.10_EXT` 下执行：

```sh
cp -rf bridge/EVB_CBDM_AK3760E_V1.0.1.dts os/kernel/arch/arm/boot/dts/EVB_CBDM_AK3760E_V1.0.1.dts
cd os
./build_kernel.sh
./build_kernel.sh -i
cd ..
./build.sh -a
cp -f image/uImage upgrade/platform/uImage
cp -f image/EVB_CBDM_AK3760E_V1.0.1.dtb upgrade/platform/cloudOS.dtb
cp -f upgrade/platform/uImage tools/burntool/platform/uImage
cp -f upgrade/platform/cloudOS.dtb tools/burntool/platform/cloudOS.dtb
```

注意：`./build.sh -a` 不会重新编内核。改 DTS 或 TP 驱动后必须先跑 `os/build_kernel.sh`。

烧录配置使用：

```text
tools/burntool/config_64M_spinor.txt
```

关键分区：

```text
platform/cloudOS.dtb -> DTB
platform/uImage      -> KERNEL
platform/root.sqsh4  -> ROOTFS
platform/usr.sqsh4   -> USR
platform/app.sqsh4   -> APP
```

## 7. 验证方法

启动后先停掉刷屏应用：

```sh
killall daemon_client
killall SAT_ANYKA.BIN
```

检查驱动：

```sh
dmesg | grep -i "jadard\|0068\|0014"
ls -l /sys/bus/i2c/devices
i2cdetect -y 2
ls /dev/input
cat /proc/bus/input/devices
```

成功标志：

```text
touch-screen@68
2-0068
input: jadard-touchscreen
/dev/input/event0
```

验证触摸事件：

```sh
hexdump /dev/input/event0
```

手指点击或滑动屏幕后有数据滚动，即表示 Linux input 事件已正常上报。LVGL evdev 输入设备配置为：

```text
/dev/input/event0
```

## 8. 现存无关问题

启动日志中的：

```text
ak_vo_open: VO resolution param is invalid
```

属于应用层 VO 分辨率配置问题，不影响 TP 驱动 probe 和 input 事件验证。
