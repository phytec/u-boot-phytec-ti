/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for PHYTEC phyCORE-AM64x kit
 *
 * Copyright (C) 2022 PHYTEC Messtechnik GmbH
 * Author: Wadim Egorov <w.egorov@phytec.de>
 */

#ifndef __PHYCORE_AM64X_H
#define __PHYCORE_AM64X_H

#include <linux/sizes.h>
#include <asm/arch/am64_hardware.h>

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

#ifndef CONFIG_CPU_V7R
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE
#if defined(CONFIG_TARGET_PHYCORE_AM64X_A53)
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SPL_TEXT_BASE + SZ_4M)
#else
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0x4000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location filled in by the boot ROM that we want to read out without any
 * interference from the C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(TI_SRAM_SCRATCH_BOARD_EEPROM_START -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SPL_BSS_START_ADDR
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START	0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_16M
#endif

#define CONFIG_SYS_BOOTM_LEN		SZ_64M

/* U-Boot general configuration */
#define EXTRA_ENV_SETTINGS \
	"default_device_tree=k3-am642-phyboard-electra-rdk.dtb\0" \
	"findfdt=" \
		"setenv name_fdt ${default_device_tree};" \
		"setenv fdtfile ${name_fdt}\0" \
	"name_kern=Image\0" \
	"console=ttyS2,115200n8\0" \
	"args_all=setenv optargs earlycon=ns16550a,mmio32,0x02800000 " \
		"${mtdparts}\0" \
	"run_kern=booti ${loadaddr} ${rd_spec} ${fdtaddr}\0"

/* U-Boot MMC-specific configuration */
#define EXTRA_ENV_SETTINGS_MMC \
	"boot=mmc\0" \
	"mmcdev=1\0" \
	"bootpart=2\0" \
	"bootdir=/boot\0" \
	"rd_spec=-\0" \
	"mmcrootfstype=ext4 rootwait\0" \
	"finduuid=part uuid ${boot} ${mmcdev}:${bootpart} uuid\0" \
	"args_mmc=run finduuid;setenv bootargs console=${console} " \
		"${optargs} " \
		"root=PARTUUID=${uuid} rw " \
		"rootfstype=${mmcrootfstype}\0" \
	"init_mmc=run args_all args_mmc\0" \
	"get_fdt_mmc=load mmc ${mmcdev}:${bootpart} ${fdtaddr} ${bootdir}/${name_fdt}\0" \
	"get_overlay_mmc=" \
		"fdt address ${fdtaddr};" \
		"fdt resize 0x100000;" \
		"for overlay in $overlays;" \
		"do;" \
		"load mmc ${mmcdev}:${bootpart} ${dtboaddr} ${bootdir}/${overlay} && " \
		"fdt apply ${dtboaddr};" \
		"done;\0" \
	"get_kern_mmc=load mmc ${mmcdev}:${bootpart} ${loadaddr} " \
		"${bootdir}/${name_kern}\0" \
	"get_fit_mmc=load mmc ${mmcdev}:${bootpart} ${addr_fit} " \
		"${bootdir}/${name_fit}\0"

/* Incorporate settings into the U-Boot environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	EXTRA_ENV_SETTINGS \
	EXTRA_ENV_SETTINGS_MMC

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#define CONFIG_SYS_USB_FAT_BOOT_PARTITION 1

#endif /* __PHYCORE_AM64X_H */
