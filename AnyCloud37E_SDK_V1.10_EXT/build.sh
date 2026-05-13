#!/bin/bash
# upgrade_bin_version=$(date +"%Y%m%d%H%M%S")
# export upgrade_bin_version

image_install_dir=`pwd`/upgrade/platform

build_uboot()
{
	cd os/
    ./build_uboot.sh
    ./build_uboot.sh -i
    cd -
    cp -rf image/u-boot.bin $image_install_dir
}

build_kernel()
{
    cp -rf bridge/EVB_CBDM_AK3760E_V1.0.1.dts os/kernel/arch/arm/boot/dts/EVB_CBDM_AK3760E_V1.0.1.dts
    cd os/
    ./build_kernel.sh
    ./build_kernel.sh -i
    cd -
    cp -rf image/uImage $image_install_dir
    cp -rf image/EVB_CBDM_AK3760E_V1.0.1.dtb $image_install_dir/cloudOS.dtb
}

build_env()
{
    cd tools/envtool/
    ./build_env.sh
    cd -
    cp -rf tools/envtool/env.img $image_install_dir/env_ak3760e_nor.img
}

build_rootfs()
{
    if [ -d ../indoor ]; then
        make -C ../indoor clean
        make -C ../indoor
        cp -rf ../indoor/build/DOORBELL_anyka_release.BIN rootfs/resource/app/app/SAT_ANYKA.BIN
    fi
    # 启动加载脚本文件，用于挂载文件系统分区
    cp -rf bridge/rc.local.nor          rootfs/scripts/flash/rc.local.nor
    # 平台模块加载脚本，为应用层启动做准备
    cp -rf bridge/main.sh               rootfs/utils/usr/sbin/main.sh
    # 应用层启动脚本
    # cp -rf bridge/app.sh                rootfs/utils/usr/sbin/app.sh
    # 守护进程脚本
    cp -rf bridge/daemon_client         rootfs/utils/usr/sbin/daemon_client
    # 将驱动KO安装到文件系统中
    cp -rf os/driver/external/* rootfs/ko/external
    cd rootfs
    ./build_rootfs.sh
    cd -
}

make_image()
{
    cd upgrade
    ./make_image.sh $1
    cd -
}

copy_platform()
{
    rm -rf tools/burntool/platform
    cp -rf $image_install_dir tools/burntool/
}


usage()
{
	echo "========================================================"
	echo "Usage : "
	echo "  build.sh -a     全部编译打包"
	echo "  build.sh -s     选择性编译打包"
	echo "========================================================"
}

build_usage()
{
	echo "========================================================"
	echo "build Usage : "
	echo "  bu          (build uboot)编译uboot，尽量少编译"
	echo "  bk          (build kernel)编译内核，尽量少编译"
    echo "  be          (build env)制作分区表"
	echo "  br          (build rootfs)构建文件系统"
	echo "  mi (arg)    (make image)制作升级包 arg: all(所有分区) app(app分区) app(res分区) other(选择分区)"
	echo "  cp          拷贝升级文件到burntool"
	echo "  q           退出"
	echo "========================================================"
    read -p "请选择以上参数:" option arg
}


build()
{
    build_usage
    clear
    case $option in
        "bu")
            echo "编译uboot"
            build_uboot
            ;;
        "bk")
            echo "编译内核"
            build_kernel
            ;;
        "be")
            echo "制作分区表"
            build_env
            ;;
        "br")
            echo "构建文件系统"
            build_rootfs
            ;;
        "mi")
            echo "制作升级包" $arg
            build_env
            make_image $arg
            ;;
        "cp")
            echo "拷贝升级文件到burntool"
            copy_platform
            ;;
        "q")
            echo "退出"
            exit
            ;;
        *)
            echo "无效参数"
            ;;
    esac
    build
}

main() {

case $option in
	"-a")
		echo "全部编译全部打包"
        # build_uboot
        # build_kernel
        build_env
        build_rootfs
        make_image all
        copy_platform
		;;
	"-s")
		echo "选择性编译打包"
		build
		;;
	*)
		echo "无效参数"
		usage
		;;
esac
}

option=$1
main
