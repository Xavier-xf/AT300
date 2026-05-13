CFLAGS = -g -O2 -static
CROSS_COMPILE=/usr/lchome/yuzhihuang/Mstar/325/arm-buildroot-linux-uclibcgnueabihf-4.9.4/bin/arm-buildroot-linux-uclibcgnueabihf-
#/usr/lchome/yuzhihuang/ankai/Linux/anyka37E/AK37E_SDK_V1.01_1/tools/arm-anykav500-linux-uclibcgnueabi/bin/arm-anykav500-linux-uclibcgnueabi-
CC=${CROSS_COMPILE}gcc
#CC = /usr/lchome/yuzhihuang/Mstar/IPC_I3/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf-gcc
#CC = /usr/lchome/yuzhihuang/Mstar/325/arm-buildroot-linux-uclibcgnueabihf-4.9.4/bin/arm-buildroot-linux-uclibcgnueabihf-gcc
#STRIP = /usr/lchome/yuzhihuang/Mstar/IPC_I3/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf-strip
#STRIP = /usr/lchome/yuzhihuang/Mstar/325/arm-buildroot-linux-uclibcgnueabihf-4.9.4/bin/arm-buildroot-linux-uclibcgnueabihf-strip
STRIP = ${CROSS_COMPILE}strip
LIBS += -lpthread -lm

OBJS=main.c  atbm_ioctrl.c

all:
	$(CC) $(CFLAGS) -o atbm_wext $(OBJS) -static $(LIBS)
	$(STRIP) -s atbm_wext
clean:
	rm -f *.o altm_wext
