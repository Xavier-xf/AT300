#!/bin/bash
# 构建文件系统镜像

cd ..
#镜像文件保存路径
image_install_dir=`pwd`/upgrade/platform
#平台库文件路径
platform_dir=`pwd`/platform/
#根文件系统路径
rootfs_dir=`pwd`/rootfs/rootfs

echo 镜像文件保存路径：$image_install_dir
echo 平台库文件路径：$platform_dir
echo 根文件系统路径：$rootfs_dir
cd -

get_config_info()
{
	FLASH_TYPE=`awk -F= '/^FLASH_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	SCREEN_TYPE=`awk -F= '/^SCREEN_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	BOARD_TYPE=`awk -F= '/^BOARD_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	CHIP_TYPE=`awk -F= '/^CHIP_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`
	WIFI_TYPE=`awk -F= '/^WIFI_TYPE/{gsub(" ","",$2); printf $2}' ../config.mk`

	echo "FLASH_TYPE="$FLASH_TYPE", BOARD_TYPE="$BOARD_TYPE", CHIP_TYPE="$CHIP_TYPE
}

extract_fs()
{
    # 解压缩根文件系统
    ./extract.sh
}

process_wifi_driver()
{
	echo "WIFI_TYPE="$WIFI_TYPE
    # 根据选择的WIFI类型，清理掉无用的驱动
    if test $WIFI_TYPE = "NO_WIFI" ; then
		true
    elif test $WIFI_TYPE = "rtl8188ftv" ; then
        cp -rf ko/external/rtl8188ftv.ko rootfs/usr/modules/
    elif test $WIFI_TYPE = "rtl8189ftv" ; then
        cp -rf ko/external/rtl8189ftv.ko rootfs/usr/modules/
    elif test $WIFI_TYPE = "atbm6031" ; then
		cp -rf ko/external/atbm6031.ko rootfs/usr/modules/
	elif test $WIFI_TYPE = "atbm6032" ; then
		cp -rf ko/external/atbm6032.ko rootfs/usr/modules/
    elif test $WIFI_TYPE = "ssv6x5x" ; then
        cp -rf ko/external/ssv6x5x.ko rootfs/usr/modules/
    elif test $WIFI_TYPE = "hi3881" ; then
		cp -rf ../bridge/hi3881/hi3881.ko rootfs/usr/modules/
        cp -rf ../bridge/hi3881/ak_mci.ko rootfs/usr/modules/
        cp -rf ../bridge/hi3881/firmware rootfs/usr/share/
    elif test $WIFI_TYPE = "atbm606x" ; then
        cp -rf ko/external/ATBM606x_wifi_sdio.ko rootfs/usr/modules/
    fi
}

process_screen_driver()
{
    if test -z $SCREEN_TYPE ; then
        return
    fi

    if test $SCREEN_TYPE = "NO_SCREEN" ; then
        true
    elif test $SCREEN_TYPE = "RGB" ; then
        cp -rf ko/external/ts_icn85xx.ko rootfs/usr/modules/
        true
    elif test $SCREEN_TYPE = "MIPI" ; then
        cp -rf ko/external/ts_icn85xx_mipi.ko rootfs/usr/modules/
        true
    else
        echo "Screen Type Error, please check!"
        exit 1
    fi
}

process_sensor_driver()
{
    #cp -rf ko/external/sensor_*.ko rootfs/usr/modules/
    cp -rf ko/external/sensor_gc0308.ko rootfs/usr/modules/
    cp -rf ko/external/sensor_gc0328.ko rootfs/usr/modules/
    cp -rf ko/external/sensor_gc032a.ko rootfs/usr/modules/
    cp -rf ko/external/sensor_tp9950.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_ar0230.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_pr2000.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_sc4236.ko rootfs/usr/modules/
#    if test $SENSOR_TYPE = "NO_SENSOR" ; then
#       true
#   elif test $SENSOR_TYPE = "gc4653" ; then
#       cp scripts/sensor/gc4653.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "pr2000" ; then
#       cp scripts/sensor/pr2000.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "ar0230" ; then
#       cp scripts/sensor/ar0230.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "sc4236" ; then
#       cp scripts/sensor/sc4236.sh rootfs/usr/sbin/sensor.sh
#   else
#       echo "Sensor Type Error, please check!"
#       exit 1
#   fi
}

# 安装驱动到文件系统
install_driver()
{
	if test -d rootfs/usr/modules ; then
        # 删除文件统中遗留的文件
        rm -rf rootfs/usr/modules/*
        rm -rf rootfs/lib/modules/*
    else
		mkdir rootfs/usr/modules
	fi

    # 拷贝新的驱动到文件系统
    # cp -rf ko/external/ak_gpio_keys.ko rootfs/usr/modules/
#    cp -rf ko/external/ak_saradc.ko rootfs/usr/modules/
    # cp -rf ko/external/ak_adc_key.ko rootfs/usr/modules/
#    cp -rf ko/external/ak_crypto.ko rootfs/usr/modules/
    cp -rf ko/external/ak_efuse.ko rootfs/usr/modules/
    cp -rf ko/external/ak_eth.ko rootfs/usr/modules/
    cp -rf ko/external/ak_hcd.ko rootfs/usr/modules/
    cp -rf ko/external/ak_i2c.ko rootfs/usr/modules/
    cp -rf ko/external/ak_ion.ko rootfs/usr/modules/
#    cp -rf ko/external/ak_isp.ko rootfs/usr/modules/
		cp -rf ko/external/ak_image_capture.ko rootfs/usr/modules/
    # cp -rf ko/external/ak_leds.ko rootfs/usr/modules/
    cp -rf ko/external/ak_mci.ko rootfs/usr/modules/
    # cp -rf ko/external/ak_motor.ko rootfs/usr/modules/
    cp -rf ko/external/ak_pcm.ko rootfs/usr/modules/
    cp -rf ko/external/ak_pwm_char.ko rootfs/usr/modules/
    cp -rf ko/external/ak_ramdisk.ko rootfs/usr/modules/
    cp -rf ko/external/ak_rtc.ko rootfs/usr/modules/
    cp -rf ko/external/ak_saradc.ko rootfs/usr/modules/
    cp -rf ko/external/ak_udc.ko rootfs/usr/modules/
    cp -rf ko/external/ak_uio.ko rootfs/usr/modules/
    cp -rf ko/external/ak_fb.ko rootfs/usr/modules/
    cp -rf ko/external/ak_gui.ko rootfs/usr/modules/
    cp -rf ko/external/ak_cs42l52.ko rootfs/usr/modules/
#    cp -rf ko/external/cryptodev.ko rootfs/usr/modules/
    cp -rf ko/external/exfat.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_ar0230.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_f22_f23_f28_f35_f37.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_gc2063.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_gc4653.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_pr2000.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_sc2145.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_sc2335.ko rootfs/usr/modules/
#    cp -rf ko/external/sensor_sc4236.ko rootfs/usr/modules/
    cp -rf ko/internal/lib/modules/4.4.*/kernel/drivers/input/touchscreen/jadard/jadard_touch.ko rootfs/usr/modules/

    cp -rf ko/internal/lib rootfs/
    rm -rf rootfs/lib/modules/4.4.*/kernel/crypto/
    rm -rf rootfs/lib/modules/4.4.*/kernel/drivers/staging/rtl8188eu
    rm -rf rootfs/lib/modules/4.4.*/kernel/net/wireless/mac80211.ko

    process_wifi_driver

    process_screen_driver
    # 数字室内机没有sensor
    # process_sensor_driver
}

# 安装平台中间件的库文件到文件系统
install_platform()
{
    # 设置库文件的安装目录
	# rootfs_dir=`pwd`/rootfs/rootfs
    #库文件安装路径
    lib_install_dir=$rootfs_dir/usr/lib
    #根文件系统中间件例程路径（没用）
	# sample_install_dir=$rootfs_dir/usr/bin
    #拷贝平台库文件 至 文件系统
    cp -rf $platform_dir/lib/libplat_tde.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_common.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_thread.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_mem.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_osal.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_log.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_dbg.so $lib_install_dir
    cp -rf $platform_dir/lib/libmpi_vdec.so $lib_install_dir
    cp -rf $platform_dir/lib/libakv_decode.so $lib_install_dir
    cp -rf $platform_dir/lib/libmpi_venc.so $lib_install_dir
    cp -rf $platform_dir/lib/libakv_encode.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_ao.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_vqe.so $lib_install_dir
    cp -rf $platform_dir/lib/libakaudiofilter.so $lib_install_dir
    cp -rf $platform_dir/lib/libplat_ai.so $lib_install_dir

    #编译中间件例程代码并拷贝到文件系统（没用）
	# cd platform/sample
	# make 
	# make install SAMPLE_INSTALL_DIR=$sample_install_dir
	# cd -
}

# 构建驱动例程代码并拷贝到文件系统（没用）
build_drv_sample()
{
    # 设置库文件的安装目录
	CROSS_COMPILE=`awk -F= '/^CROSS_COMPILE/{gsub(" ","",$2); printf $2}' ./rules.mk`
	cd os/driver/external/sample
	make CROSS_COMPILE=$CROSS_COMPILE
	make install INSTALL=$sample_install_dir
	cd -
}

process_flash_scripts()
{
    if test $FLASH_TYPE = "NOR" ; then
        cp ./scripts/flash/rc.local.nor rootfs/etc/init.d/rc.local
        cp ./scripts/flash/update.nor.sh rootfs/sbin/update.sh
    elif test $FLASH_TYPE = "NAND" ; then
        cp ./scripts/flash/rc.local.nand rootfs/etc/init.d/rc.local
        cp ./scripts/flash/update.nand.sh rootfs/sbin/update.sh
		cp ./scripts/flash/nand_update rootfs/bin/nand_update
    else
        echo "Flash type error, please check!"
        exit 1
    fi
}

process_sensor_scripts()
{
    #cp scripts/sensor/*.sh rootfs/usr/sbin/
    cp scripts/sensor/gc0308.sh rootfs/usr/sbin/
    cp scripts/sensor/gc0328.sh rootfs/usr/sbin/
    cp scripts/sensor/gc032a.sh rootfs/usr/sbin/
    cp scripts/sensor/tp9950.sh rootfs/usr/sbin/
#    cp scripts/sensor/pr2000.sh rootfs/usr/sbin/
#    cp scripts/sensor/ar0230.sh rootfs/usr/sbin/
#    cp scripts/sensor/sc4236.sh rootfs/usr/sbin/
#   if test $SENSOR_TYPE = "NO_SENSOR" ; then
#       true
#   elif test $SENSOR_TYPE = "gc4653" ; then
#       cp scripts/sensor/gc4653.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "pr2000" ; then
#       cp scripts/sensor/pr2000.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "ar0230" ; then
#       cp scripts/sensor/ar0230.sh rootfs/usr/sbin/sensor.sh
#   elif test $SENSOR_TYPE = "sc4236" ; then
#       cp scripts/sensor/sc4236.sh rootfs/usr/sbin/sensor.sh
#   else
#       echo "Sensor Type Error, please check!"
#       exit 1
#   fi
}

process_screen_scripts()
{
    if test -z $SCREEN_TYPE ; then
        return
    fi

    if test $SCREEN_TYPE = "NO_SCREEN" ; then
        true
    elif test $SCREEN_TYPE = "RGB" ; then
        cp scripts/screen/screen.sh rootfs/usr/sbin/
    elif test $SCREEN_TYPE = "MIPI" ; then
        cp scripts/screen/screen_mipi.sh rootfs/usr/sbin/
    else
        echo "Screen Type Error, please check!"
        exit 1
    fi
}

process_wifi_scripts()
{
    # 根据选择的WIFI类型，清理掉无用的驱动
    if test $WIFI_TYPE = "NO_WIFI" ; then
        true
    elif test $WIFI_TYPE = "rtl8188ftv" ; then
        cp scripts/wifi/rtl8188.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    elif test $WIFI_TYPE = "rtl8189ftv" ; then
        cp scripts/wifi/rtl8189.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    elif test $WIFI_TYPE = "atbm6031" ; then
        cp scripts/wifi/atbm6031.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
	elif test $WIFI_TYPE = "atbm6032" ; then
        cp scripts/wifi/atbm6032.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    elif test $WIFI_TYPE = "ssv6x5x" ; then
        cp scripts/wifi/ssv6x5x.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    elif test $WIFI_TYPE = "hi3881" ; then
        cp ../bridge/hi3881/hi3881.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    elif test $WIFI_TYPE = "atbm606x" ; then
        cp scripts/wifi/atbm6032.sh rootfs/usr/sbin/wifi_driver.sh
        cp -rf ./wifi/* rootfs/
    else
        echo "Wifi Type Error, please check!"
        exit 1
    fi
}

process_fs_scripts()
{
    cp -rf scripts/fs/* rootfs/usr/sbin/
}

process_other_scripts()
{
    cp -rf scripts/others/* rootfs/usr/sbin/
}

# 安装各类脚本
install_scripts()
{
    process_flash_scripts

    process_screen_scripts

    # process_sensor_scripts

    process_wifi_scripts

    process_fs_scripts

    process_other_scripts
	UTILS_SUPPORT=1

    if test $UTILS_SUPPORT -eq 1 ; then
        cp -rf utils/* rootfs/
    fi

}

# 制作config、tuya、data可读写分区的镜像文件
make_jffs2_images()
{
    ./mkfs.jffs2 -d rootfs/etc/config -s 256 -e 4096 -m none -o config.jffs2
    ./mkfs.jffs2 -d rootfs/app/data -s 256 -e 4096 -m none -o data.jffs2
    # ./mkfat32.sh 2624 rootfs/app/data data.jffs2
    ./mkfs.jffs2 -d rootfs/app/tuya -s 256 -e 4096 -m none -o tuya.jffs2
}

# 制作usr、app、root只读分区的镜像文件
make_squashfs_images()
{
    mkdir -p tmpfs
    cp -af rootfs/* tmpfs
    rm -rf tmpfs/usr/* tmpfs/etc/config/* tmpfs/app/app/* tmpfs/app/data/* tmpfs/app/tuya/* 

    ./mksquashfs tmpfs/ root.sqsh4 -noappend -comp xz

    ./mksquashfs rootfs/usr usr.sqsh4 -noappend -comp xz
    ./mksquashfs rootfs/app/app app.sqsh4 -noappend -comp xz

    rm -rf tmpfs
}

# 制作烧录镜像
make_image()
{
    # 根据选择的Flash类型制作烧录镜像
    if test $FLASH_TYPE = "NOR" ; then
        make_jffs2_images
        make_squashfs_images

        # 将制作好的烧录镜像放到安装目录下
        mv -f root.sqsh4 $image_install_dir
        mv -f config.jffs2 $image_install_dir
        mv -f usr.sqsh4 $image_install_dir
        mv -f app.sqsh4 $image_install_dir
        mv -f tuya.jffs2 $image_install_dir
        mv -f data.jffs2 $image_install_dir
    elif test $FLASH_TYPE = "NAND" ; then
        ./create_yaffs2fs.sh
        cp -rf *.yaffs2 $image_install_dir
    fi
}

# 从config.mk获取配置信息
get_config_info

# 解压根文件系统
extract_fs

# wxj:更新busybox到文件系统
BUSYBOX_FILE="../tools/busybox-1.30.1/busybox"
if [ -f "$BUSYBOX_FILE" ]; then
    cp -rf $BUSYBOX_FILE rootfs/bin/
fi

# 安装平台中间件的库文件到文件系统
install_platform

# 构建驱动例程代码并拷贝到文件系统（没用）
# build_drv_sample

# 安装驱动到文件系统
install_driver

# 安装各类脚本
install_scripts

# 制作烧录镜像
make_image
