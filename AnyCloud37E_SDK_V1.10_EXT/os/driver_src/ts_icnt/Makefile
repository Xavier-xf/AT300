########################################################################################################
#	@Copyright (C) 	Anyka 2019-6-10
#	@brief					Create Linux4.4.x  linux driver module for AnyCloudv500
#	@Version				V1.0.0 
#####################################GCC Compile Flags #################################################
EXTRA_CFLAGS += $(USER_EXTRA_CFLAGS)
EXTRA_CFLAGS += -O1

EXTRA_CFLAGS += -Wno-unused-variable
EXTRA_CFLAGS += -Wno-unused-value
EXTRA_CFLAGS += -Wno-unused-label
EXTRA_CFLAGS += -Wno-unused-parameter
EXTRA_CFLAGS += -Wno-unused-function
EXTRA_CFLAGS += -Wno-unused

EXTRA_LDFLAGS += --strip-debug

#####################################GCC Objects #######################################################
#定义当前的工作目录
CUR_DIR=$(shell pwd)  
SRC_DIR=$(CUR_DIR)

#INC_DIR=$(CUR_DIR)/include
#EXTRA_CFLAGS += -Wall -I$(INC_DIR)
EXTRA_CFLAGS += -Wall

#找到当前模块目录下的所有.c的源码文件
#SRCS		=	${wildcard *.c}
#CFILE 	= $(wildcard *.c)	

#将当前的所有源码文件，替换成不带目录路径的.o目标文件
#OBJS		=	${patsubst %.c, %.o, ${SRCS}}
#OBJS  	= $(CFILE:%.c=%.o)		#模式替换函数
OBJS  	= ts_icn85xx.o

MODULE_NAME := ts_icn85xx

#####################################Platform Related ###################################################

CONFIG_PLATFORM_ARM_ANYKA = y

ifeq ($(CONFIG_PLATFORM_ARM_ANYKA), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE ?= arm-anykav500-linux-uclibcgnueabi-
KVER := $(shell uname -r)
ifndef KSRC
    KSRC := $(shell pwd)/../../bd
endif
endif

USER_MODULE_NAME ?= ts_icn85xx
ifneq ($(USER_MODULE_NAME),)
MODULE_NAME := $(USER_MODULE_NAME)
endif

#####################################Make Compile Module ########################################################

ifneq ($(KERNELRELEASE),)

#$(MODULE_NAME)-objs  := $(OBJS)
obj-m := $(MODULE_NAME).o

else

all: modules
	@echo "make all first step!"
modules: icn85xx.ko icn86xx.ko icn85xx_mipi.ko
	
icn86xx.ko:
	@echo "make modules!"
	@echo $(SRCS)
	@sed -i "s/^\/\/#define ICNT86XX/#define ICNT86XX/" ts_icn85xx.c
	@mv ts_icn85xx.c ts_icn86xx.c
	$(MAKE) ARCH=$(ARCH) MODULE_NAME=ts_icn86xx CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules
	@sed -i "s/^#define ICNT86XX/\/\/#define ICNT86XX/" ts_icn86xx.c
	@mv ts_icn86xx.c ts_icn85xx.c

icn85xx_mipi.ko:
	@echo "make modules!"
	@echo $(SRCS)
	@sed -i "s/^\/\/#define ICNT85XX_FOR_MIPI/#define ICNT85XX_FOR_MIPI/" ts_icn85xx.c
	@mv ts_icn85xx.c ts_icn85xx_mipi.c
	$(MAKE) ARCH=$(ARCH) MODULE_NAME=ts_icn85xx_mipi CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules
	@sed -i "s/^#define ICNT85XX_FOR_MIPI/\/\/#define ICNT85XX_FOR_MIPI/" ts_icn85xx_mipi.c
	@mv ts_icn85xx_mipi.c ts_icn85xx.c

icn85xx.ko:
	@echo "make modules!"
	@echo $(SRCS)
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded
	
.PHONY: modules clean test

clean:
	rm -rf *.o* *.ko* *.mod.c *.mod.* *.bak *.order *.symvers 
	rm -rf .*.o.cmd .*.ko.cmd .*.mod.o.cmd
	rm -rf .tmp_versions 

test:
	@echo $(CUR_DIR)
	@echo $(SRCS)
	@echo $(CFILE)
	@echo $(OBJS)

endif








