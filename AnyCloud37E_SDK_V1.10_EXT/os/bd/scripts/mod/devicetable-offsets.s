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
	.file	"devicetable-offsets.c"
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
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod
@ -I scripts/mod
@ -I /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/arch/arm/mach-anycloud/include
@ -iprefix /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/
@ -isysroot /opt/arm-anykav500-linux-uclibcgnueabi/arm-anykav500-linux-uclibcgnueabi/sysroot
@ -D __KERNEL__ -D __LINUX_ARM_ARCH__=5 -U arm -D CC_HAVE_ASM_GOTO
@ -D KBUILD_STR(s)=#s -D KBUILD_BASENAME=KBUILD_STR(devicetable_offsets)
@ -D KBUILD_MODNAME=KBUILD_STR(devicetable_offsets)
@ -isystem /opt/arm-anykav500-linux-uclibcgnueabi/bin/../lib/gcc/arm-anykav500-linux-uclibcgnueabi/4.9.4/include
@ -include /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/include/linux/kconfig.h
@ -MD scripts/mod/.devicetable-offsets.s.d
@ /home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c
@ -mlittle-endian -mapcs -mno-sched-prolog -mabi=aapcs-linux
@ -mno-thumb-interwork -mfpu=vfp -marm -march=armv5te -mtune=arm9tdmi
@ -mfloat-abi=soft -mtls-dialect=gnu
@ -auxbase-strip scripts/mod/devicetable-offsets.s -O2 -Wall -Wundef
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
@ 10 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_usb_device_id #24 sizeof(struct usb_device_id)"	@
@ 0 "" 2
@ 11 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_match_flags #0 offsetof(struct usb_device_id, match_flags)"	@
@ 0 "" 2
@ 12 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_idVendor #2 offsetof(struct usb_device_id, idVendor)"	@
@ 0 "" 2
@ 13 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_idProduct #4 offsetof(struct usb_device_id, idProduct)"	@
@ 0 "" 2
@ 14 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bcdDevice_lo #6 offsetof(struct usb_device_id, bcdDevice_lo)"	@
@ 0 "" 2
@ 15 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bcdDevice_hi #8 offsetof(struct usb_device_id, bcdDevice_hi)"	@
@ 0 "" 2
@ 16 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bDeviceClass #10 offsetof(struct usb_device_id, bDeviceClass)"	@
@ 0 "" 2
@ 17 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bDeviceSubClass #11 offsetof(struct usb_device_id, bDeviceSubClass)"	@
@ 0 "" 2
@ 18 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bDeviceProtocol #12 offsetof(struct usb_device_id, bDeviceProtocol)"	@
@ 0 "" 2
@ 19 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bInterfaceClass #13 offsetof(struct usb_device_id, bInterfaceClass)"	@
@ 0 "" 2
@ 20 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bInterfaceSubClass #14 offsetof(struct usb_device_id, bInterfaceSubClass)"	@
@ 0 "" 2
@ 21 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bInterfaceProtocol #15 offsetof(struct usb_device_id, bInterfaceProtocol)"	@
@ 0 "" 2
@ 22 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_usb_device_id_bInterfaceNumber #16 offsetof(struct usb_device_id, bInterfaceNumber)"	@
@ 0 "" 2
@ 24 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_hid_device_id #16 sizeof(struct hid_device_id)"	@
@ 0 "" 2
@ 25 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hid_device_id_bus #0 offsetof(struct hid_device_id, bus)"	@
@ 0 "" 2
@ 26 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hid_device_id_group #2 offsetof(struct hid_device_id, group)"	@
@ 0 "" 2
@ 27 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hid_device_id_vendor #4 offsetof(struct hid_device_id, vendor)"	@
@ 0 "" 2
@ 28 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hid_device_id_product #8 offsetof(struct hid_device_id, product)"	@
@ 0 "" 2
@ 30 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ieee1394_device_id #24 sizeof(struct ieee1394_device_id)"	@
@ 0 "" 2
@ 31 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ieee1394_device_id_match_flags #0 offsetof(struct ieee1394_device_id, match_flags)"	@
@ 0 "" 2
@ 32 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ieee1394_device_id_vendor_id #4 offsetof(struct ieee1394_device_id, vendor_id)"	@
@ 0 "" 2
@ 33 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ieee1394_device_id_model_id #8 offsetof(struct ieee1394_device_id, model_id)"	@
@ 0 "" 2
@ 34 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ieee1394_device_id_specifier_id #12 offsetof(struct ieee1394_device_id, specifier_id)"	@
@ 0 "" 2
@ 35 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ieee1394_device_id_version #16 offsetof(struct ieee1394_device_id, version)"	@
@ 0 "" 2
@ 37 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_pci_device_id #28 sizeof(struct pci_device_id)"	@
@ 0 "" 2
@ 38 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_vendor #0 offsetof(struct pci_device_id, vendor)"	@
@ 0 "" 2
@ 39 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_device #4 offsetof(struct pci_device_id, device)"	@
@ 0 "" 2
@ 40 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_subvendor #8 offsetof(struct pci_device_id, subvendor)"	@
@ 0 "" 2
@ 41 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_subdevice #12 offsetof(struct pci_device_id, subdevice)"	@
@ 0 "" 2
@ 42 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_class #16 offsetof(struct pci_device_id, class)"	@
@ 0 "" 2
@ 43 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pci_device_id_class_mask #20 offsetof(struct pci_device_id, class_mask)"	@
@ 0 "" 2
@ 45 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ccw_device_id #12 sizeof(struct ccw_device_id)"	@
@ 0 "" 2
@ 46 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ccw_device_id_match_flags #0 offsetof(struct ccw_device_id, match_flags)"	@
@ 0 "" 2
@ 47 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ccw_device_id_cu_type #2 offsetof(struct ccw_device_id, cu_type)"	@
@ 0 "" 2
@ 48 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ccw_device_id_cu_model #6 offsetof(struct ccw_device_id, cu_model)"	@
@ 0 "" 2
@ 49 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ccw_device_id_dev_type #4 offsetof(struct ccw_device_id, dev_type)"	@
@ 0 "" 2
@ 50 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ccw_device_id_dev_model #7 offsetof(struct ccw_device_id, dev_model)"	@
@ 0 "" 2
@ 52 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ap_device_id #8 sizeof(struct ap_device_id)"	@
@ 0 "" 2
@ 53 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ap_device_id_dev_type #2 offsetof(struct ap_device_id, dev_type)"	@
@ 0 "" 2
@ 55 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_css_device_id #8 sizeof(struct css_device_id)"	@
@ 0 "" 2
@ 56 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_css_device_id_type #1 offsetof(struct css_device_id, type)"	@
@ 0 "" 2
@ 58 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_serio_device_id #4 sizeof(struct serio_device_id)"	@
@ 0 "" 2
@ 59 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_serio_device_id_type #0 offsetof(struct serio_device_id, type)"	@
@ 0 "" 2
@ 60 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_serio_device_id_proto #3 offsetof(struct serio_device_id, proto)"	@
@ 0 "" 2
@ 61 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_serio_device_id_id #2 offsetof(struct serio_device_id, id)"	@
@ 0 "" 2
@ 62 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_serio_device_id_extra #1 offsetof(struct serio_device_id, extra)"	@
@ 0 "" 2
@ 64 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_acpi_device_id #24 sizeof(struct acpi_device_id)"	@
@ 0 "" 2
@ 65 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_acpi_device_id_id #0 offsetof(struct acpi_device_id, id)"	@
@ 0 "" 2
@ 66 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_acpi_device_id_cls #16 offsetof(struct acpi_device_id, cls)"	@
@ 0 "" 2
@ 67 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_acpi_device_id_cls_msk #20 offsetof(struct acpi_device_id, cls_msk)"	@
@ 0 "" 2
@ 69 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_pnp_device_id #12 sizeof(struct pnp_device_id)"	@
@ 0 "" 2
@ 70 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pnp_device_id_id #0 offsetof(struct pnp_device_id, id)"	@
@ 0 "" 2
@ 72 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_pnp_card_device_id #76 sizeof(struct pnp_card_device_id)"	@
@ 0 "" 2
@ 73 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pnp_card_device_id_devs #12 offsetof(struct pnp_card_device_id, devs)"	@
@ 0 "" 2
@ 75 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_pcmcia_device_id #52 sizeof(struct pcmcia_device_id)"	@
@ 0 "" 2
@ 76 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_match_flags #0 offsetof(struct pcmcia_device_id, match_flags)"	@
@ 0 "" 2
@ 77 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_manf_id #2 offsetof(struct pcmcia_device_id, manf_id)"	@
@ 0 "" 2
@ 78 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_card_id #4 offsetof(struct pcmcia_device_id, card_id)"	@
@ 0 "" 2
@ 79 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_func_id #6 offsetof(struct pcmcia_device_id, func_id)"	@
@ 0 "" 2
@ 80 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_function #7 offsetof(struct pcmcia_device_id, function)"	@
@ 0 "" 2
@ 81 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_device_no #8 offsetof(struct pcmcia_device_id, device_no)"	@
@ 0 "" 2
@ 82 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_pcmcia_device_id_prod_id_hash #12 offsetof(struct pcmcia_device_id, prod_id_hash)"	@
@ 0 "" 2
@ 84 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_of_device_id #196 sizeof(struct of_device_id)"	@
@ 0 "" 2
@ 85 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_of_device_id_name #0 offsetof(struct of_device_id, name)"	@
@ 0 "" 2
@ 86 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_of_device_id_type #32 offsetof(struct of_device_id, type)"	@
@ 0 "" 2
@ 87 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_of_device_id_compatible #64 offsetof(struct of_device_id, compatible)"	@
@ 0 "" 2
@ 89 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_vio_device_id #64 sizeof(struct vio_device_id)"	@
@ 0 "" 2
@ 90 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_vio_device_id_type #0 offsetof(struct vio_device_id, type)"	@
@ 0 "" 2
@ 91 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_vio_device_id_compat #32 offsetof(struct vio_device_id, compat)"	@
@ 0 "" 2
@ 93 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_input_device_id #160 sizeof(struct input_device_id)"	@
@ 0 "" 2
@ 94 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_flags #0 offsetof(struct input_device_id, flags)"	@
@ 0 "" 2
@ 95 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_bustype #4 offsetof(struct input_device_id, bustype)"	@
@ 0 "" 2
@ 96 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_vendor #6 offsetof(struct input_device_id, vendor)"	@
@ 0 "" 2
@ 97 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_product #8 offsetof(struct input_device_id, product)"	@
@ 0 "" 2
@ 98 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_version #10 offsetof(struct input_device_id, version)"	@
@ 0 "" 2
@ 99 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_evbit #12 offsetof(struct input_device_id, evbit)"	@
@ 0 "" 2
@ 100 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_keybit #16 offsetof(struct input_device_id, keybit)"	@
@ 0 "" 2
@ 101 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_relbit #112 offsetof(struct input_device_id, relbit)"	@
@ 0 "" 2
@ 102 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_absbit #116 offsetof(struct input_device_id, absbit)"	@
@ 0 "" 2
@ 103 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_mscbit #124 offsetof(struct input_device_id, mscbit)"	@
@ 0 "" 2
@ 104 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_ledbit #128 offsetof(struct input_device_id, ledbit)"	@
@ 0 "" 2
@ 105 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_sndbit #132 offsetof(struct input_device_id, sndbit)"	@
@ 0 "" 2
@ 106 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_ffbit #136 offsetof(struct input_device_id, ffbit)"	@
@ 0 "" 2
@ 107 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_input_device_id_swbit #152 offsetof(struct input_device_id, swbit)"	@
@ 0 "" 2
@ 109 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_eisa_device_id #12 sizeof(struct eisa_device_id)"	@
@ 0 "" 2
@ 110 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_eisa_device_id_sig #0 offsetof(struct eisa_device_id, sig)"	@
@ 0 "" 2
@ 112 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_parisc_device_id #8 sizeof(struct parisc_device_id)"	@
@ 0 "" 2
@ 113 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_parisc_device_id_hw_type #0 offsetof(struct parisc_device_id, hw_type)"	@
@ 0 "" 2
@ 114 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_parisc_device_id_hversion #2 offsetof(struct parisc_device_id, hversion)"	@
@ 0 "" 2
@ 115 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_parisc_device_id_hversion_rev #1 offsetof(struct parisc_device_id, hversion_rev)"	@
@ 0 "" 2
@ 116 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_parisc_device_id_sversion #4 offsetof(struct parisc_device_id, sversion)"	@
@ 0 "" 2
@ 118 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_sdio_device_id #12 sizeof(struct sdio_device_id)"	@
@ 0 "" 2
@ 119 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_sdio_device_id_class #0 offsetof(struct sdio_device_id, class)"	@
@ 0 "" 2
@ 120 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_sdio_device_id_vendor #2 offsetof(struct sdio_device_id, vendor)"	@
@ 0 "" 2
@ 121 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_sdio_device_id_device #4 offsetof(struct sdio_device_id, device)"	@
@ 0 "" 2
@ 123 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ssb_device_id #6 sizeof(struct ssb_device_id)"	@
@ 0 "" 2
@ 124 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ssb_device_id_vendor #0 offsetof(struct ssb_device_id, vendor)"	@
@ 0 "" 2
@ 125 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ssb_device_id_coreid #2 offsetof(struct ssb_device_id, coreid)"	@
@ 0 "" 2
@ 126 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ssb_device_id_revision #4 offsetof(struct ssb_device_id, revision)"	@
@ 0 "" 2
@ 128 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_bcma_device_id #6 sizeof(struct bcma_device_id)"	@
@ 0 "" 2
@ 129 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_bcma_device_id_manuf #0 offsetof(struct bcma_device_id, manuf)"	@
@ 0 "" 2
@ 130 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_bcma_device_id_id #2 offsetof(struct bcma_device_id, id)"	@
@ 0 "" 2
@ 131 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_bcma_device_id_rev #4 offsetof(struct bcma_device_id, rev)"	@
@ 0 "" 2
@ 132 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_bcma_device_id_class #5 offsetof(struct bcma_device_id, class)"	@
@ 0 "" 2
@ 134 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_virtio_device_id #8 sizeof(struct virtio_device_id)"	@
@ 0 "" 2
@ 135 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_virtio_device_id_device #0 offsetof(struct virtio_device_id, device)"	@
@ 0 "" 2
@ 136 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_virtio_device_id_vendor #4 offsetof(struct virtio_device_id, vendor)"	@
@ 0 "" 2
@ 138 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_hv_vmbus_device_id #20 sizeof(struct hv_vmbus_device_id)"	@
@ 0 "" 2
@ 139 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hv_vmbus_device_id_guid #0 offsetof(struct hv_vmbus_device_id, guid)"	@
@ 0 "" 2
@ 141 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_i2c_device_id #24 sizeof(struct i2c_device_id)"	@
@ 0 "" 2
@ 142 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_i2c_device_id_name #0 offsetof(struct i2c_device_id, name)"	@
@ 0 "" 2
@ 144 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_spi_device_id #36 sizeof(struct spi_device_id)"	@
@ 0 "" 2
@ 145 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_spi_device_id_name #0 offsetof(struct spi_device_id, name)"	@
@ 0 "" 2
@ 147 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_dmi_system_id #332 sizeof(struct dmi_system_id)"	@
@ 0 "" 2
@ 148 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_dmi_system_id_matches #8 offsetof(struct dmi_system_id, matches)"	@
@ 0 "" 2
@ 150 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_platform_device_id #24 sizeof(struct platform_device_id)"	@
@ 0 "" 2
@ 151 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_platform_device_id_name #0 offsetof(struct platform_device_id, name)"	@
@ 0 "" 2
@ 153 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_mdio_device_id #8 sizeof(struct mdio_device_id)"	@
@ 0 "" 2
@ 154 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mdio_device_id_phy_id #0 offsetof(struct mdio_device_id, phy_id)"	@
@ 0 "" 2
@ 155 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mdio_device_id_phy_id_mask #4 offsetof(struct mdio_device_id, phy_id_mask)"	@
@ 0 "" 2
@ 157 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_zorro_device_id #8 sizeof(struct zorro_device_id)"	@
@ 0 "" 2
@ 158 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_zorro_device_id_id #0 offsetof(struct zorro_device_id, id)"	@
@ 0 "" 2
@ 160 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_isapnp_device_id #12 sizeof(struct isapnp_device_id)"	@
@ 0 "" 2
@ 161 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_isapnp_device_id_vendor #4 offsetof(struct isapnp_device_id, vendor)"	@
@ 0 "" 2
@ 162 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_isapnp_device_id_function #6 offsetof(struct isapnp_device_id, function)"	@
@ 0 "" 2
@ 164 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ipack_device_id #12 sizeof(struct ipack_device_id)"	@
@ 0 "" 2
@ 165 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ipack_device_id_format #0 offsetof(struct ipack_device_id, format)"	@
@ 0 "" 2
@ 166 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ipack_device_id_vendor #4 offsetof(struct ipack_device_id, vendor)"	@
@ 0 "" 2
@ 167 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ipack_device_id_device #8 offsetof(struct ipack_device_id, device)"	@
@ 0 "" 2
@ 169 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_amba_id #12 sizeof(struct amba_id)"	@
@ 0 "" 2
@ 170 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_amba_id_id #0 offsetof(struct amba_id, id)"	@
@ 0 "" 2
@ 171 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_amba_id_mask #4 offsetof(struct amba_id, mask)"	@
@ 0 "" 2
@ 173 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_mips_cdmm_device_id #1 sizeof(struct mips_cdmm_device_id)"	@
@ 0 "" 2
@ 174 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mips_cdmm_device_id_type #0 offsetof(struct mips_cdmm_device_id, type)"	@
@ 0 "" 2
@ 176 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_x86_cpu_id #12 sizeof(struct x86_cpu_id)"	@
@ 0 "" 2
@ 177 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_x86_cpu_id_feature #6 offsetof(struct x86_cpu_id, feature)"	@
@ 0 "" 2
@ 178 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_x86_cpu_id_family #2 offsetof(struct x86_cpu_id, family)"	@
@ 0 "" 2
@ 179 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_x86_cpu_id_model #4 offsetof(struct x86_cpu_id, model)"	@
@ 0 "" 2
@ 180 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_x86_cpu_id_vendor #0 offsetof(struct x86_cpu_id, vendor)"	@
@ 0 "" 2
@ 182 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_cpu_feature #2 sizeof(struct cpu_feature)"	@
@ 0 "" 2
@ 183 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_cpu_feature_feature #0 offsetof(struct cpu_feature, feature)"	@
@ 0 "" 2
@ 185 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_mei_cl_device_id #56 sizeof(struct mei_cl_device_id)"	@
@ 0 "" 2
@ 186 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mei_cl_device_id_name #0 offsetof(struct mei_cl_device_id, name)"	@
@ 0 "" 2
@ 187 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mei_cl_device_id_uuid #32 offsetof(struct mei_cl_device_id, uuid)"	@
@ 0 "" 2
@ 188 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_mei_cl_device_id_version #48 offsetof(struct mei_cl_device_id, version)"	@
@ 0 "" 2
@ 190 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_rio_device_id #8 sizeof(struct rio_device_id)"	@
@ 0 "" 2
@ 191 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_rio_device_id_did #0 offsetof(struct rio_device_id, did)"	@
@ 0 "" 2
@ 192 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_rio_device_id_vid #2 offsetof(struct rio_device_id, vid)"	@
@ 0 "" 2
@ 193 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_rio_device_id_asm_did #4 offsetof(struct rio_device_id, asm_did)"	@
@ 0 "" 2
@ 194 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_rio_device_id_asm_vid #6 offsetof(struct rio_device_id, asm_vid)"	@
@ 0 "" 2
@ 196 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_ulpi_device_id #8 sizeof(struct ulpi_device_id)"	@
@ 0 "" 2
@ 197 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ulpi_device_id_vendor #0 offsetof(struct ulpi_device_id, vendor)"	@
@ 0 "" 2
@ 198 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_ulpi_device_id_product #2 offsetof(struct ulpi_device_id, product)"	@
@ 0 "" 2
@ 200 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->SIZE_hda_device_id #20 sizeof(struct hda_device_id)"	@
@ 0 "" 2
@ 201 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hda_device_id_vendor_id #0 offsetof(struct hda_device_id, vendor_id)"	@
@ 0 "" 2
@ 202 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hda_device_id_rev_id #4 offsetof(struct hda_device_id, rev_id)"	@
@ 0 "" 2
@ 203 "/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/os/kernel/scripts/mod/devicetable-offsets.c" 1
	
.ascii "->OFF_hda_device_id_api_version #8 offsetof(struct hda_device_id, api_version)"	@
@ 0 "" 2
	mov	r0, #0	@,
	ldmfd	sp, {fp, sp, pc}	@
	.size	main, .-main
	.ident	"GCC: (Buildroot 2018.02.7_V1.0.03-g9ff3371) 4.9.4"
	.section	.note.GNU-stack,"",%progbits
