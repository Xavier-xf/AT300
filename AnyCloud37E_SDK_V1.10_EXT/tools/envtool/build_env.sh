#!/bin/sh
# 一键制作分区表

rm env.img
tr '\000' '\377' < /dev/zero | dd of=./env.img bs=1024 count=4
./fw_setenv -s env_ak3760e_nor.cfg
./fw_printenv
