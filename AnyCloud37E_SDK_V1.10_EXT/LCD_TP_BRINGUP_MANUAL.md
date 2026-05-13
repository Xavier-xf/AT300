# LCD/TP Bring-Up Notes

## SDK Selection

Use `AnyCloud37E_SDK_V1.10_EXT` for Linux, rootfs, LVGL, and TP testing. `AnyCloud37E_SDK_V1.10_TestBoard` is only used as the proven U-Boot LCD bring-up reference.

## LCD Configuration

The EXT LCD parameters were migrated from the TestBoard panel that already displays the blue SATOZ logo.

Source reference:

- `../AnyCloud37E_SDK_V1.10_TestBoard/os/kernel/arch/arm/boot/dts/anycloud_lcd.dtsi`
- node: `JD9165T_CSOT7P0_1024x600_4L`

EXT files:

- `os/kernel/arch/arm/boot/dts/anycloud_lcd.dtsi`
- `bridge/EVB_CBDM_AK3760E_V1.0.1.dts`
- `os/kernel/arch/arm/boot/dts/EVB_CBDM_AK3760E_V1.0.1.dts`
- `config.mk`

Key working values:

- Resolution: `1024x600`
- MIPI: 4 lanes, `panel-dsi-num-lane = <3>`
- Lane map: `txd0/1/2/3/4 = 0,1,4,2,3`
- Video mode: `panel-dsi-video-mode = <0>`
- DSI clock: `panel-dsi-clk = <400>`
- Timing: `hfp/hbp/hsa = 100/100/2`, `vfp/vbp/vsa = 95/12/2`

Do not add the old self-test command `0xC2 0x60` for normal display.

## TP Configuration

The Jadard JD9165T driver is imported under:

- `os/kernel/drivers/input/touchscreen/jadard/`

Kernel config enables it as a module:

- `CONFIG_TOUCHSCREEN_JADARD_CHIPSET=y`
- `CONFIG_TOUCHSCREEN_JADARD_MODULE=m`

Board DTS uses I2C2:

- `compatible = "jadard,jdcommon"`
- `reg = <0x68>`
- IRQ GPIO: `77`
- RESET GPIO: `76`
- coordinates: `<0 1024 0 600>`

`0x68` is from the target-side `i2cdetect -y 2` result. If probe still fails, re-check the bus, reset GPIO, power, and vendor TP firmware.

## Build Flow

From `AnyCloud37E_SDK_V1.10_EXT`:

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

Important: `./build.sh -a` builds env/rootfs/package/copy, but does not rebuild kernel. Always build kernel first after changing DTS or TP driver.

## Burn And Verify

Use `tools/burntool/config_64M_spinor.txt`. It contains:

- `platform/cloudOS.dtb` -> `DTB`
- `platform/uImage` -> `KERNEL`
- `platform/root.sqsh4` -> `ROOTFS`
- `platform/usr.sqsh4` -> `USR`
- `platform/app.sqsh4` -> `APP`

On target:

```sh
dmesg | grep -i jadard
lsmod
ls /dev/input
cat /proc/bus/input/devices
hexdump /dev/input/eventX
```

LVGL should read the matching `/dev/input/eventX` evdev node.
