/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * PHYTEC common board options for AM6x platforms.
 *
 * Copyright (C) 2022 PHYTEC Messtechnik GmbH
 * Author: Wadim Egorov <w.egorov@phytec.de>
 */

#ifndef __PHYTEC_AM6_COMMON_H
#define __PHYTEC_AM6_COMMON_H

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

/* U-Boot general configuration */
#define EXTRA_ENV_SETTINGS \
	"fdtfile" CONFIG_DEFAULT_FDT_FILE "\0" \
	"name_kern=Image\0" \
	"console=ttyS2,115200n8\0" \
	"args_all=setenv optargs earlycon=ns16550a,mmio32,0x02800000 " \
		"${mtdparts}\0" \
	"run_kern=booti ${loadaddr} ${rd_spec} ${fdtaddr}\0"

/* U-Boot MMC-specific configuration */
#define EXTRA_ENV_SETTINGS_MMC \
	"boot=mmc\0" \
	"mmcdev=1\0" \
	"mmcroot=2\0" \
	"bootpart=1\0" \
	"rd_spec=-\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"mmcrootfstype=ext4 rootwait\0" \
	"args_mmc=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=/dev/mmcblk${mmcdev}p${mmcroot} rw " \
		"rootfstype=${mmcrootfstype}\0" \
	"init_mmc=run args_all args_mmc\0" \
	"get_fdt_mmc=load mmc ${mmcdev}:${bootpart} ${fdtaddr} ${fdtfile}\0" \
	"get_overlay_mmc=" \
		"fdt address ${fdtaddr};" \
		"fdt resize 0x100000;" \
		"for overlay in $overlays;" \
		"do;" \
		"load mmc ${mmcdev}:${bootpart} ${dtboaddr} ${overlay} && " \
		"fdt apply ${dtboaddr};" \
		"done;" \
		"setenv extension_overlay_addr ${dtboaddr};" \
		"setenv extension_overlay_cmd \'load mmc " \
			"${mmcdev}:${bootpart} ${dtboaddr} " \
			"${extension_overlay_name}\';" \
		"extension scan;" \
		"extension apply all;\0" \
	"get_kern_mmc=load mmc ${mmcdev}:${bootpart} ${loadaddr} " \
		"${name_kern}\0" \
	"get_fit_mmc=load mmc ${mmcdev}:${bootpart} ${addr_fit} " \
		"${name_fit}\0"

/* Incorporate settings into the U-Boot environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	EXTRA_ENV_SETTINGS \
	EXTRA_ENV_SETTINGS_MMC

#endif /* ! __PHYTEC_AM6_COMMON_H */
