cmd_arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb := mkdir -p arch/arm/boot/dts/ ; arm-anykav500-linux-uclibcgnueabi-gcc -E -Wp,-MD,arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.d.pre.tmp -nostdinc -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/include -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/drivers/of/testcase-data -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.dts.tmp /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb -b 0 -i /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/  -d arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.d.dtc.tmp arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.dts.tmp ; cat arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.d.pre.tmp arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.d.dtc.tmp > arch/arm/boot/dts/.EVB_CBDR_AK3761E_V1.0.0_camera.dtb.d

source_arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb := /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dts

deps_arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb := \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_ak37e1.dtsi \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_ak37e_common.dtsi \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/include/dt-bindings/gpio/gpio.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/include/dt-bindings/clock/ak37e-clock.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_ak37e_pinctrl.dtsi \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/include/dt-bindings/pinctrl/ak_37e_pinctrl.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_norflash.dtsi \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_nandflash.dtsi \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/boot/dts/anycloud_lcd.dtsi \

arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb: $(deps_arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb)

$(deps_arch/arm/boot/dts/EVB_CBDR_AK3761E_V1.0.0_camera.dtb):
