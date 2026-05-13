cmd_libbb/makedev.o := arm-anykav500-linux-uclibcgnueabi-gcc -Wp,-MD,libbb/.makedev.o.d  -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=KBUILD_STR(1.30.1)" -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin-printf -Os -fpic -fvisibility=hidden -z norelro    -D"KBUILD_STR(s)=#s" -D"KBUILD_BASENAME=KBUILD_STR(makedev)"  -D"KBUILD_MODNAME=KBUILD_STR(makedev)" -c -o libbb/makedev.o libbb/makedev.c

deps_libbb/makedev.o := \
  libbb/makedev.c \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config/big/endian.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/nommu.h) \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include-fixed/limits.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include-fixed/syslimits.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/limits.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/features.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_config.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/sys/cdefs.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/posix1_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/local_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/linux/limits.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_local_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/posix2_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/xopen_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/stdio_lim.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/byteswap.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/byteswap.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/byteswap-common.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/types.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/wordsize.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/typesizes.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/byteswap-16.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/endian.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/endian.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include/stdint.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/stdint.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/wchar.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include/stdbool.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/unistd.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/posix_opt.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_posix_opt.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/environments.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include/stddef.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/confname.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/bits/getopt.h \
  /home/wxj/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot/usr/include/sys/sysmacros.h \

libbb/makedev.o: $(deps_libbb/makedev.o)

$(deps_libbb/makedev.o):
