cmd_drivers/net/ppp/ppp_deflate.mod.o := arm-anykav500-linux-uclibcgnueabi-gcc -Wp,-MD,drivers/net/ppp/.ppp_deflate.mod.o.d -nostdinc -isystem /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include -Iarch/arm/include/generated/uapi -Iarch/arm/include/generated  -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include -Iinclude -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi -Iinclude/generated/uapi -include /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kconfig.h  -D__KERNEL__ -mlittle-endian   -I/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/mach-anycloud/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-PIE -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -O2 --param=allow-store-data-races=0 -DCC_HAVE_ASM_GOTO -Wframe-larger-than=4096 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time  -D"KBUILD_STR(s)=$(pound)s" -D"KBUILD_BASENAME=KBUILD_STR(ppp_deflate.mod)"  -D"KBUILD_MODNAME=KBUILD_STR(ppp_deflate)" -DMODULE  -c -o drivers/net/ppp/ppp_deflate.mod.o drivers/net/ppp/ppp_deflate.mod.c

source_drivers/net/ppp/ppp_deflate.mod.o := drivers/net/ppp/ppp_deflate.mod.c

deps_drivers/net/ppp/ppp_deflate.mod.o := \
    $(wildcard include/config/module/unload.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/modules/tree/lookup.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
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
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/const.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/panic/timeout.h) \
  /opt/arm-anykav500-linux-uclibcgnueabi/lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include/stdarg.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/linkage.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/stringify.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/linkage.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/bitops.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/bits.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/bitops.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/typecheck.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/irqflags.h \
    $(wildcard include/config/cpu/v7m.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/hwcap.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/hwcap.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/irqflags.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/barrier.h \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arm/heavy/mb.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/non-atomic.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/fls64.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/sched.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/hweight.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/arch_hweight.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/const_hweight.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/lock.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/le.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/byteorder.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/byteorder/little_endian.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/byteorder/little_endian.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/swab.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/swab.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/swab.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/swab.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/byteorder/generic.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/lto.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kern_levels.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/kernel.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/sysinfo.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/dynamic_debug.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/stat.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/stat.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/stat.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/seqlock.h \
    $(wildcard include/config/debug/lock/alloc.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/preempt.h \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  arch/arm/include/generated/asm/preempt.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/preempt.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/bug.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/arm/lpae.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/crunch.h) \
    $(wildcard include/config/arm/thumbee.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/kuser/helpers.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/glue.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/pgtable-2level-types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/need/mach/memory/h.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/phys/offset.h) \
    $(wildcard include/config/virt/to/bus.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/sizes.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/pfn.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/getorder.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/bottom_half.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/spinlock_types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/spinlock_types_up.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rwlock_types.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/spinlock_up.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/processor.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/arm/errata/754327.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/hw_breakpoint.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rwlock.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/spinlock_api_up.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/atomic.h \
    $(wildcard include/config/generic/atomic64.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/atomic.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/prefetch.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/cmpxchg.h \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/cmpxchg-local.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/cmpxchg.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/atomic64.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/atomic-long.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/div64.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/compiler.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/time64.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/time.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/uidgid.h \
    $(wildcard include/config/multiuser.h) \
    $(wildcard include/config/user/ns.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/highuid.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kmod.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/deferred/struct/page/init.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/cma.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/zone/device.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/wait.h \
  arch/arm/include/generated/asm/current.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/current.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/wait.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/bitmap.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/string.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/string.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/notifier.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/errno.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/errno.h \
  arch/arm/include/generated/asm/errno.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/errno.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/errno-base.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/osq_lock.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  arch/arm/include/generated/asm/rwsem.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/rwsem.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/srcu.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rcupdate.h \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/rcu/nocb/cpu/all.h) \
    $(wildcard include/config/no/hz/full/sysidle.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/completion.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/ktime.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/jiffies.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/timex.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/timex.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/param.h \
  arch/arm/include/generated/asm/param.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/asm-generic/param.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/timex.h \
  include/generated/timeconst.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/timekeeping.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rcutiny.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
    $(wildcard include/config/no/hz/common.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/sched/smt.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/percpu.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/percpu.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/percpu-defs.h \
    $(wildcard include/config/page/table/isolation.h) \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/topology.h \
    $(wildcard include/config/arm/cpu/topology.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/topology.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rbtree.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/sysctl.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/elf.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/elf.h \
    $(wildcard include/config/vdso.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/auxvec.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi/asm/auxvec.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/vdso_datapage.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/user.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/elf.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi/linux/elf-em.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/sysfs.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/err.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/idr.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kobject_ns.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kref.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/jump_label.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/rbtree_latch.h \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/asm/module.h \
    $(wildcard include/config/arm/unwind.h) \
    $(wildcard include/config/arm/module/plts.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/vermagic.h \
  include/generated/utsrelease.h \

drivers/net/ppp/ppp_deflate.mod.o: $(deps_drivers/net/ppp/ppp_deflate.mod.o)

$(deps_drivers/net/ppp/ppp_deflate.mod.o):
