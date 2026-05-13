#!/bin/bash

TARGET_DIR="$PWD"
BOOT_TOOLS=$TARGET_DIR/boot_tool.bin
UPGRADE_IMAGE_DIR=$TARGET_DIR/platform/
UBOOT_PARTTION="u-boot.bin"
ENV_PARTTION="env_ak3760e_nor.img"
ENVBK_PARTTION="env_ak3760e_nor_bk.img"
DTB_PARTTION="cloudOS.dtb"
KERNEL_PARTTION="uImage"
LOGO_PARTTION="anyka_logo.jpg"
ROOTFS_PARTTION="root.sqsh4"
USR_PARTTION="usr.sqsh4"
CONFIG_PARTTION="config.jffs2"
APP_PARTTION="app.sqsh4"
RESOURCE_PARTTION="res.sqsh4"
DATA_PARTTION="data.jffs2"
TUYA_PARTTION="tuya.jffs2"
# ASTERISK_PARTTION="asterisk.sqsh4"


uart_bin_install_dir=$TARGET_DIR/../tools/uart_burntool1.05/platform/
uart_bin_name=$TARGET_DIR/uart_uboot.bin
rm -rf $uart_bin_name
touch $uart_bin_name

# 获取uboot大小信息
uboot_part_size=327680 #320*1024
uboot_image_size=$(stat -c %s $UPGRADE_IMAGE_DIR$UBOOT_PARTTION)
uboot_free_size=$(($uboot_part_size - $uboot_image_size))

# 获取dtb大小信息
dtb_part_size=65536 #64*1024
dtb_image_size=$(stat -c %s $UPGRADE_IMAGE_DIR$DTB_PARTTION)
dtb_free_size=$(($dtb_part_size - $dtb_image_size))


dd if=$BOOT_TOOLS bs=512 count=1 >>$uart_bin_name

dd if=$UPGRADE_IMAGE_DIR$UBOOT_PARTTION bs=512 skip=1 >>$uart_bin_name

printf '\xFF%.0s' $(seq 1 $uboot_free_size) >> $uart_bin_name

dd if=$UPGRADE_IMAGE_DIR$ENV_PARTTION bs=512 conv=notrunc >>$uart_bin_name

dd if=$UPGRADE_IMAGE_DIR$ENV_PARTTION bs=512 conv=notrunc >>$uart_bin_name

dd if=$UPGRADE_IMAGE_DIR$DTB_PARTTION bs=512 conv=notrunc >>$uart_bin_name

printf '\xFF%.0s' $(seq 1 $dtb_free_size) >> $uart_bin_name


chmod 777 $uart_bin_name
sync
mv -f $uart_bin_name $uart_bin_install_dir
