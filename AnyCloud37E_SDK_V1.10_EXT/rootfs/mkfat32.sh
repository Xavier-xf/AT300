#!/bin/bash

usage()
{
    echo "*****************************************************"
    echo "*****************************************************"
	echo "Usage : $0  size(KB)  src_dir    dest_img"
	echo "e.g.  : $0  1024      app/data/  fat32.img"
    echo "*****************************************************"
    echo "*****************************************************"
}

fat32_size=$1
fat32_src_dir=$2
fat32_dest_img=$3


if [ $# -ne 3 ]; then
    usage
    exit
fi

# 检查传入的第一个参数是否是数值
if [[ $fat32_size =~ ^[0-9]+$ ]]; then
    echo "fat32 size : ${fat32_size}KB"
else
    echo "fat32 size error! ${fat32_size}"
    exit
fi

# 检查传入的源目录是否存在
if [ -d "$fat32_src_dir" ]; then
    echo "fat32 src dir : ${fat32_src_dir}"
else
    echo "fat32 src dir does not exist! ${fat32_src_dir}"
    exit
fi

echo "fat32 dest img : ${fat32_dest_img}"

# 获取空闲循环设备
idle_loop_dev=$(sudo losetup -f)

echo "idle loop dev : ${idle_loop_dev}"

rm -rf $fat32_dest_img
touch $fat32_dest_img

# 创建磁盘镜像文件
dd if=/dev/zero of=$fat32_dest_img bs=1024 count=$fat32_size

# 格式化镜像文件为FAT32
mkfs.vfat -F 32 -S 4096 $fat32_dest_img

# 创建一个临时挂载点
mkdir fat_mount_tmp
rm -rf fat_mount_tmp/*

# 设置循环设备
sudo losetup $idle_loop_dev $fat32_dest_img

# 挂载FAT文件系统
sudo mount -t vfat $idle_loop_dev fat_mount_tmp/

# 将源目录的文件拷贝到fat文件系统
sudo cp -rf $fat32_src_dir/* fat_mount_tmp/

# 卸载FAT文件系统
sudo umount fat_mount_tmp/

# 释放循环设备
sudo losetup -d $idle_loop_dev

# 删除临时挂载点
rm -rf fat_mount_tmp