	.arch armv5te
	.fpu softvfp
	.eabi_attribute 20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute 21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute 23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute 24, 1	@ Tag_ABI_align8_needed
	.eabi_attribute 25, 1	@ Tag_ABI_align8_preserved
	.eabi_attribute 26, 2	@ Tag_ABI_enum_size
	.eabi_attribute 30, 2	@ Tag_ABI_optimization_goals
	.eabi_attribute 34, 0	@ Tag_CPU_unaligned_access
	.eabi_attribute 18, 4	@ Tag_ABI_PCS_wchar_t
	.file	"asm-offsets.c"
@ GNU C (Buildroot 2018.02.7_V1.0.03-g9ff3371) version 4.9.4 (arm-anykav500-linux-uclibcgnueabi)
@	compiled by GNU C version 5.4.0 20160609, GMP version 6.1.2, MPFR version 3.1.6, MPC version 1.0.3
@ warning: GMP header version 6.1.2 differs from library version 6.3.0.
@ warning: MPC header version 1.0.3 differs from library version 1.3.1.
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:  -nostdinc
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include
@ -I arch/arm/include/generated/uapi -I arch/arm/include/generated
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include
@ -I include
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/include/uapi
@ -I arch/arm/include/generated/uapi
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/uapi
@ -I include/generated/uapi
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/.
@ -I .
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/mach-anycloud/include
@ -iprefix /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/
@ -isysroot /opt/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot
@ -D __KERNEL__ -D __LINUX_ARM_ARCH__=5 -U arm -D CC_HAVE_ASM_GOTO
@ -D KBUILD_STR(s)=#s -D KBUILD_BASENAME=KBUILD_STR(asm_offsets)
@ -D KBUILD_MODNAME=KBUILD_STR(asm_offsets)
@ -isystem /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include
@ -include /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kconfig.h
@ -MD arch/arm/kernel/.asm-offsets.s.d
@ /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c
@ -mlittle-endian -mapcs -mno-sched-prolog -mabi=aapcs-linux
@ -mno-thumb-interwork -mfpu=vfp -marm -march=armv5te -mtune=arm9tdmi
@ -mfloat-abi=soft -mtls-dialect=gnu
@ -auxbase-strip arch/arm/kernel/asm-offsets.s -O2 -Wall -Wundef
@ -Wstrict-prototypes -Wno-trigraphs -Werror=implicit-function-declaration
@ -Wno-format-security -Wno-maybe-uninitialized -Wframe-larger-than=4096
@ -Wno-unused-but-set-variable -Wdeclaration-after-statement
@ -Wno-pointer-sign -Werror=implicit-int -Werror=strict-prototypes
@ -Werror=date-time -std=gnu90 -fno-strict-aliasing -fno-common -fno-PIE
@ -fno-dwarf2-cfi-asm -fno-ipa-sra -fno-delete-null-pointer-checks
@ -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls
@ -fno-var-tracking-assignments -fno-strict-overflow
@ -fno-merge-all-constants -fmerge-constants -fstack-check=no
@ -fconserve-stack -fverbose-asm --param allow-store-data-races=0
@ options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
@ -fbranch-count-reg -fcaller-saves -fcombine-stack-adjustments
@ -fcompare-elim -fcprop-registers -fcrossjumping -fcse-follow-jumps
@ -fdefer-pop -fdevirtualize -fdevirtualize-speculatively -fearly-inlining
@ -feliminate-unused-debug-types -fexpensive-optimizations
@ -fforward-propagate -ffunction-cse -fgcse -fgcse-lm -fgnu-runtime
@ -fgnu-unique -fguess-branch-probability -fhoist-adjacent-loads -fident
@ -fif-conversion -fif-conversion2 -findirect-inlining -finline
@ -finline-atomics -finline-functions-called-once -finline-small-functions
@ -fipa-cp -fipa-profile -fipa-pure-const -fipa-reference
@ -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
@ -fisolate-erroneous-paths-dereference -fivopts -fkeep-static-consts
@ -fleading-underscore -flifetime-dse -fmath-errno -fmerge-constants
@ -fmerge-debug-strings -fmove-loop-invariants -foptimize-strlen
@ -fpartial-inlining -fpeephole -fpeephole2 -fprefetch-loop-arrays
@ -freg-struct-return -freorder-blocks -freorder-functions
@ -frerun-cse-after-loop -fsched-critical-path-heuristic
@ -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
@ -fsched-last-insn-heuristic -fsched-pressure -fsched-rank-heuristic
@ -fsched-spec -fsched-spec-insn-heuristic -fsched-stalled-insns-dep
@ -fschedule-insns -fschedule-insns2 -fsection-anchors -fshow-column
@ -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
@ -fstrict-volatile-bitfields -fsync-libcalls -fthread-jumps
@ -ftoplevel-reorder -ftrapping-math -ftree-bit-ccp -ftree-builtin-call-dce
@ -ftree-ccp -ftree-ch -ftree-coalesce-vars -ftree-copy-prop
@ -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts
@ -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-if-convert
@ -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
@ -ftree-parallelize-loops= -ftree-phiprop -ftree-pre -ftree-pta
@ -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra
@ -ftree-switch-conversion -ftree-tail-merge -ftree-ter -ftree-vrp
@ -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss -mapcs-frame
@ -marm -mlittle-endian -mlra -mpic-data-is-text-relative -muclibc
@ -mvectorize-with-neon-quad

#APP
	.macro	it, cond
	.endm
	.macro	itt, cond
	.endm
	.macro	ite, cond
	.endm
	.macro	ittt, cond
	.endm
	.macro	itte, cond
	.endm
	.macro	itet, cond
	.endm
	.macro	itee, cond
	.endm
	.macro	itttt, cond
	.endm
	.macro	ittte, cond
	.endm
	.macro	ittet, cond
	.endm
	.macro	ittee, cond
	.endm
	.macro	itett, cond
	.endm
	.macro	itete, cond
	.endm
	.macro	iteet, cond
	.endm
	.macro	iteee, cond
	.endm

	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp	@,
	stmfd	sp!, {fp, ip, lr, pc}	@,
	sub	fp, ip, #4	@,,
#APP
@ 61 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TSK_ACTIVE_MM #312 offsetof(struct task_struct, active_mm)"	@
@ 0 "" 2
@ 65 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 66 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_FLAGS #0 offsetof(struct thread_info, flags)"	@
@ 0 "" 2
@ 67 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_PREEMPT #4 offsetof(struct thread_info, preempt_count)"	@
@ 0 "" 2
@ 68 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_ADDR_LIMIT #8 offsetof(struct thread_info, addr_limit)"	@
@ 0 "" 2
@ 69 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_TASK #12 offsetof(struct thread_info, task)"	@
@ 0 "" 2
@ 70 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_CPU #16 offsetof(struct thread_info, cpu)"	@
@ 0 "" 2
@ 71 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_CPU_DOMAIN #20 offsetof(struct thread_info, cpu_domain)"	@
@ 0 "" 2
@ 72 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_CPU_SAVE #24 offsetof(struct thread_info, cpu_context)"	@
@ 0 "" 2
@ 73 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_USED_CP #76 offsetof(struct thread_info, used_cp)"	@
@ 0 "" 2
@ 74 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_TP_VALUE #92 offsetof(struct thread_info, tp_value)"	@
@ 0 "" 2
@ 75 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->TI_FPSTATE #104 offsetof(struct thread_info, fpstate)"	@
@ 0 "" 2
@ 91 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 92 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R0 #0 offsetof(struct pt_regs, ARM_r0)"	@
@ 0 "" 2
@ 93 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R1 #4 offsetof(struct pt_regs, ARM_r1)"	@
@ 0 "" 2
@ 94 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R2 #8 offsetof(struct pt_regs, ARM_r2)"	@
@ 0 "" 2
@ 95 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R3 #12 offsetof(struct pt_regs, ARM_r3)"	@
@ 0 "" 2
@ 96 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R4 #16 offsetof(struct pt_regs, ARM_r4)"	@
@ 0 "" 2
@ 97 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R5 #20 offsetof(struct pt_regs, ARM_r5)"	@
@ 0 "" 2
@ 98 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R6 #24 offsetof(struct pt_regs, ARM_r6)"	@
@ 0 "" 2
@ 99 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R7 #28 offsetof(struct pt_regs, ARM_r7)"	@
@ 0 "" 2
@ 100 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R8 #32 offsetof(struct pt_regs, ARM_r8)"	@
@ 0 "" 2
@ 101 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R9 #36 offsetof(struct pt_regs, ARM_r9)"	@
@ 0 "" 2
@ 102 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_R10 #40 offsetof(struct pt_regs, ARM_r10)"	@
@ 0 "" 2
@ 103 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_FP #44 offsetof(struct pt_regs, ARM_fp)"	@
@ 0 "" 2
@ 104 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_IP #48 offsetof(struct pt_regs, ARM_ip)"	@
@ 0 "" 2
@ 105 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_SP #52 offsetof(struct pt_regs, ARM_sp)"	@
@ 0 "" 2
@ 106 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_LR #56 offsetof(struct pt_regs, ARM_lr)"	@
@ 0 "" 2
@ 107 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_PC #60 offsetof(struct pt_regs, ARM_pc)"	@
@ 0 "" 2
@ 108 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_PSR #64 offsetof(struct pt_regs, ARM_cpsr)"	@
@ 0 "" 2
@ 109 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_OLD_R0 #68 offsetof(struct pt_regs, ARM_ORIG_r0)"	@
@ 0 "" 2
@ 110 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->S_FRAME_SIZE #72 sizeof(struct pt_regs)"	@
@ 0 "" 2
@ 111 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 127 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->VMA_VM_MM #32 offsetof(struct vm_area_struct, vm_mm)"	@
@ 0 "" 2
@ 128 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->VMA_VM_FLAGS #40 offsetof(struct vm_area_struct, vm_flags)"	@
@ 0 "" 2
@ 129 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 130 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->VM_EXEC #4 VM_EXEC"	@
@ 0 "" 2
@ 131 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 132 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->PAGE_SZ #4096 PAGE_SIZE"	@
@ 0 "" 2
@ 133 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 134 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->SYS_ERROR0 #10420224 0x9f0000"	@
@ 0 "" 2
@ 135 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 136 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->SIZEOF_MACHINE_DESC #100 sizeof(struct machine_desc)"	@
@ 0 "" 2
@ 137 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->MACHINFO_TYPE #0 offsetof(struct machine_desc, nr)"	@
@ 0 "" 2
@ 138 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->MACHINFO_NAME #4 offsetof(struct machine_desc, name)"	@
@ 0 "" 2
@ 139 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 140 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->PROC_INFO_SZ #52 sizeof(struct proc_info_list)"	@
@ 0 "" 2
@ 141 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->PROCINFO_INITFUNC #16 offsetof(struct proc_info_list, __cpu_flush)"	@
@ 0 "" 2
@ 142 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->PROCINFO_MM_MMUFLAGS #8 offsetof(struct proc_info_list, __cpu_mm_mmu_flags)"	@
@ 0 "" 2
@ 143 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->PROCINFO_IO_MMUFLAGS #12 offsetof(struct proc_info_list, __cpu_io_mmu_flags)"	@
@ 0 "" 2
@ 144 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 160 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->SLEEP_SAVE_SP_SZ #8 sizeof(struct sleep_save_sp)"	@
@ 0 "" 2
@ 161 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->SLEEP_SAVE_SP_PHYS #4 offsetof(struct sleep_save_sp, save_ptr_stash_phys)"	@
@ 0 "" 2
@ 162 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->SLEEP_SAVE_SP_VIRT #0 offsetof(struct sleep_save_sp, save_ptr_stash)"	@
@ 0 "" 2
@ 164 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 165 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->DMA_BIDIRECTIONAL #0 DMA_BIDIRECTIONAL"	@
@ 0 "" 2
@ 166 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->DMA_TO_DEVICE #1 DMA_TO_DEVICE"	@
@ 0 "" 2
@ 167 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->DMA_FROM_DEVICE #2 DMA_FROM_DEVICE"	@
@ 0 "" 2
@ 168 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 169 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->CACHE_WRITEBACK_ORDER #6 __CACHE_WRITEBACK_ORDER"	@
@ 0 "" 2
@ 170 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->CACHE_WRITEBACK_GRANULE #64 __CACHE_WRITEBACK_GRANULE"	@
@ 0 "" 2
@ 171 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
@ 209 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/kernel/asm-offsets.c" 1
	
.ascii "->"
@ 0 "" 2
	mov	r0, #0	@,
	ldmfd	sp, {fp, sp, pc}	@
	.size	main, .-main
	.ident	"GCC: (Buildroot 2018.02.7_V1.0.03-g9ff3371) 4.9.4"
	.section	.note.GNU-stack,"",%progbits
