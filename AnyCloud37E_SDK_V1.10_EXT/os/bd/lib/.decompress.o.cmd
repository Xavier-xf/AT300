cmd_lib/decompress.o := arm-anykav500-linux-uclibcgnueabi-gcc -Wp,-MD,lib/.decompress.o.d -nostdinc -isystem /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include -Iarch/arm/include/generated/uapi -Iarch/arm/include/generated  -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include -Iinclude -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi -Iinclude/generated/uapi -include /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kconfig.h  -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/lib -Ilib -D__KERNEL__ -mlittle-endian   -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/mach-anycloud/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-PIE -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -O2 --param=allow-store-data-races=0 -DCC_HAVE_ASM_GOTO -Wframe-larger-than=4096 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time    -D"KBUILD_STR(s)=$(pound)s" -D"KBUILD_BASENAME=KBUILD_STR(decompress)"  -D"KBUILD_MODNAME=KBUILD_STR(decompress)" -c -o lib/decompress.o /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/lib/decompress.c

source_lib/decompress.o := /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/lib/decompress.c

deps_lib/decompress.o := \
    $(wildcard include/config/decompress/gzip.h) \
    $(wildcard include/config/decompress/bzip2.h) \
    $(wildcard include/config/decompress/lzma.h) \
    $(wildcard include/config/decompress/xz.h) \
    $(wildcard include/config/decompress/lzo.h) \
    $(wildcard include/config/decompress/lz4.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/generic.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/bunzip2.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/unlzma.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/unxz.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/inflate.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/unlzo.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/decompress/unlz4.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/int-ll64.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitsperlong.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/bitsperlong.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/posix_types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/stddef.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/stddef.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/kasan.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/posix_types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/posix_types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /opt/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include/stdarg.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/string.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/string.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/lto.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kern_levels.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/linkage.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/stringify.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/linkage.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/cache.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/kernel.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/sysinfo.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/dynamic_debug.h \

lib/decompress.o: $(deps_lib/decompress.o)

$(deps_lib/decompress.o):
