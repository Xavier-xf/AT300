#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Canyka,ak39ev330-ethernet*");
MODULE_ALIAS("of:N*T*Canyka,ak37d-ethernet*");
MODULE_ALIAS("of:N*T*Canyka,ak37e-ethernet*");
MODULE_ALIAS("of:N*T*Canyka,ak37e-ethernet1*");

MODULE_INFO(srcversion, "79B7CDD8F63FF5B8FD16CAC");
