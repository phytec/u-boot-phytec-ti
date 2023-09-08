/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the PHYTEC phyCORE-AM57x
 * See ti_omap5_common.h for omap5 common settings.
 *
 * Copyright (C) 2015 PHYTEC America, LLC
 * Author: Russell Robinson <rrobinson@phytec.com>
 *
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#ifndef __CONFIG_PHYCORE_AM57X_H
#define __CONFIG_PHYCORE_AM57X_H

#include <linux/sizes.h>

#define CONFIG_IODELAY_RECALIBRATION

#define CONFIG_SYS_BOOTM_LEN		SZ_64M

#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		0x80000000

#define CONSOLEDEV			"ttyS2"		/* UART3 */

#define CONFIG_SYS_OMAP_ABE_SYSCK

/*
 * Environment setup
 */
#define DFU_ALT_INFO_MMC \
	"dfu_alt_info_mmc=" \
	"boot part 0 1;" \
	"rootfs part 0 2;" \
	"MLO fat 0 1;" \
	"MLO.raw raw 0x100 0x100;" \
	"u-boot.img.raw raw 0x300 0x800;" \
	"spl-os-args.raw raw 0x80 0x80;" \
	"spl-os-image.raw raw 0x1400 0x2000;" \
	"spl-os-args fat 0 1;" \
	"spl-os-image fat 0 1;" \
	"u-boot.img fat 0 1;" \
	"uEnv.txt fat 0 1\0"

#define DFU_ALT_INFO_EMMC \
	"dfu_alt_info_emmc=" \
	"rawemmc raw 0 3751936;" \
	"boot part 1 1;" \
	"rootfs part 1 2;" \
	"MLO fat 1 1;" \
	"MLO.raw raw 0x100 0x100;" \
	"u-boot.img.raw raw 0x300 0x800;" \
	"spl-os-args.raw raw 0x80 0x80;" \
	"spl-os-image.raw raw 0x1400 0x2000;" \
	"spl-os-args fat 1 1;" \
	"spl-os-image fat 1 1;" \
	"u-boot.img fat 1 1;" \
	"uEnv.txt fat 1 1\0"

#define DFU_ALT_INFO_RAM \
	"dfu_alt_info_ram=" \
	"kernel ram 0x80200000 0x4000000;" \
	"fdt ram 0x80f80000 0x80000;" \
	"ramdisk ram 0x81000000 0x4000000\0"

#ifndef CONFIG_SPL_BUILD
#define DFUARGS \
	"dfu_bufsiz=0x10000\0" \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_EMMC \
	DFU_ALT_INFO_RAM
#else
#ifdef CONFIG_SPL_DFU
#define DFUARGS \
	"dfu_bufsiz=0x10000\0" \
	DFU_ALT_INFO_RAM
#endif
#endif

/* Extra environment settings */
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	DEFAULT_MMC_TI_ARGS \
	DEFAULT_FIT_TI_ARGS \
	"console=" CONSOLEDEV ",115200n8\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"boot=mmc\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"usbtty=cdc_acm\0" \
	"vram=16M\0" \
	"optargs=\0" \
	"dofastboot=0\0" \
	"serverip=192.168.3.10\0" \
	"ipaddr=192.168.3.11\0" \
	"netmask=255.255.255.0\0" \
	"boot_net=run findfdt; " \
		"run netboot;\0" \
	"netboot=echo Booting from network ...; " \
		"tftp ${loadaddr} ${tftploc}${bootfile}; " \
		"tftp ${fdtaddr} ${tftploc}${fdtfile}; " \
		"run netargs; " \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"board_override=0\0" \
	"get_overlaystring=" \
		"for overlay in $overlays;" \
		"do;" \
		"setenv overlaystring ${overlaystring}'#'${overlay};" \
		"done;\0" \
	"get_overlay_mmc=" \
		"fdt address ${fdtaddr};" \
		"fdt resize 0x100000;" \
		"for overlay in $overlays;" \
		"do;" \
		"load mmc ${bootpart} ${dtboaddr} ${bootdir}/${overlay};" \
		"fdt apply ${dtboaddr};" \
		"done;\0" \
	"mmcloados=run args_mmc; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"run get_overlay_mmc; " \
				"bootz ${loadaddr} - ${fdtaddr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0" \
	"findfdt=" \
		"if test ${board_override} -eq 1; then " \
			"echo NOTE: board_override set, using FDT = ${fdtfile};" \
		"else " \
			"if test ${board_soc} = am57xx || test ${board_opt} = undefined; then " \
				"echo WARNING: Using generic FDT! Please update EEPROM!;" \
				"setenv fdtfile " CONFIG_DEFAULT_FDT_FILE ";" \
			"else " \
				"setenv fdtfile ${board_soc}-phytec-pcm-948-${board_opt}.dtb;" \
			"fi;" \
		"fi;\0" \
	DFUARGS \
	NETARGS \
	ENV_SETTINGS_EXTRA \

/* Storage media environment settings */
#ifdef CONFIG_MTD_RAW_NAND
/* NAND env settings */
#define ENV_SETTINGS_EXTRA \
	NANDARGS \
	"mtdids=" CONFIG_MTDIDS_DEFAULT "\0" \
	"mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0" \
	"nandloadfdt=ubifsload ${fdtaddr} ${bootdir}/${fdtfile}\0" \
	"get_overlay_nand=" \
		"fdt address ${fdtaddr};" \
		"fdt resize 0x100000;" \
		"for overlay in $overlays;" \
		"do;" \
		"ubifsload ${dtboaddr} ${bootdir}/${overlay};" \
		"fdt apply ${dtboaddr};" \
		"done;\0" \
	"nandroot=ubi0:rootfs rw ubi.mtd=NAND.file-system\0" \
	"nandboot=echo Booting from nand ...;" \
		"run nandargs;" \
		"ubi part NAND.file-system;" \
		"ubifsmount ubi0:rootfs;" \
		"ubifsload ${loadaddr} ${bootdir}/${bootfile};" \
		"if test ${boot_fit} -eq 1; then " \
			"run run_fit; " \
		"fi; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run nandloadfdt; then " \
				"run get_overlay_nand; " \
				"bootz ${loadaddr} - ${fdtaddr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARNING: Cannot load DTB; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0"

#define BOOTCOMMAND_EXTRA \
	"run nandboot;" \
	""
#else
/* eMMC env settings */
#define EMMC_PARTS_DEFAULT \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=env,start=2MiB,size=1MiB,uuid=${uuid_gpt_env};" \
	"name=rootfs,start=3MiB,size=-,uuid=${uuid_gpt_rootfs}"

#define ENV_SETTINGS_EXTRA \
	"partitions=" EMMC_PARTS_DEFAULT "\0" \
	""

#define BOOTCOMMAND_EXTRA \
	"setenv mmcdev 1; setenv bootpart 1:2; run mmcboot" \
	""
#endif /* CONFIG_MTD_RAW_NAND */

/* Boot command */
#define CONFIG_BOOTCOMMAND \
	"if test ${dofastboot} -eq 1; then " \
		"echo Boot fastboot requested, resetting dofastboot ...;" \
		"setenv dofastboot 0; saveenv;" \
		"echo Booting into fastboot ...; fastboot 0;" \
	"fi;" \
	"if test ${boot_fit} -eq 1; then " \
		"run update_to_fit;" \
		"run get_overlaystring;" \
	"fi;" \
	"run findfdt; " \
	"run envboot; " \
	"run mmcboot; " \
	BOOTCOMMAND_EXTRA \
	""

#include <configs/ti_omap5_common.h>

/* Enhance our eMMC support / experience. */
#define CONFIG_HSMMC2_8BIT

/* CPSW Ethernet */
#define CONFIG_NET_RETRY_COUNT		10
#define PHY_ANEG_TIMEOUT	8000	/* PHY needs longer aneg time at 1G */

/* USB xHCI HOST */
#define CONFIG_USB_XHCI_OMAP

#define CONFIG_OMAP_USB3PHY1_HOST

/* SATA */
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN		1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
						CONFIG_SYS_SCSI_MAX_LUN)

/* EEPROM */
#define CONFIG_SYS_I2C_EEPROM_BUS	0
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2

/* NAND */
#ifdef CONFIG_MTD_RAW_NAND
/* NAND: device related configs */
#define CONFIG_SYS_NAND_PAGE_SIZE	4096
#define CONFIG_SYS_NAND_OOBSIZE		224
#define CONFIG_SYS_NAND_BLOCK_SIZE	(64 * 4096)
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / \
					 CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE

/* NAND: driver related configs */
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_NAND_OMAP_ECCSCHEME	OMAP_ECC_BCH16_CODE_HW
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS		{ 2, 3, 4, 5, 6, 7, 8, 9, \
					 10, 11, 12, 13, 14, 15, 16, 17, \
					 18, 19, 20, 21, 22, 23, 24, 25, \
					 26, 27, 28, 29, 30, 31, 32, 33, \
					 34, 35, 36, 37, 38, 39, 40, 41, \
					 42, 43, 44, 45, 46, 47, 48, 49, \
					 50, 51, 52, 53, 54, 55, 56, 57, \
					 58, 59, 60, 61, 62, 63, 64, 65, \
					 66, 67, 68, 69, 70, 71, 72, 73, \
					 74, 75, 76, 77, 78, 79, 80, 81, \
					 82, 83, 84, 85, 86, 87, 88, 89, \
					 90, 91, 92, 93, 94, 95, 96, 97, \
					 98, 99, 100, 101, 102, 103, 104, \
					 110, 111, 112, 113, 114, 115, 116, \
					 117, 118, 119, 120, 121, 122, 123, \
					 124, 125, 126, 127, 128, 129, 130, \
					 131, 132, 133, 134, 135, 136, 137, \
					 138, 139, 140, 141, 142, 143, 144, \
					 145, 146, 147, 148, 149, 150, 151, \
					 152, 153, 154, 155, 156, 157, 158, \
					 159, 160, 161, 162, 163, 164, 165, \
					 166, 167, 168, 169, 170, 171, 172, \
					 173, 174, 175, 176, 177, 178, 179, \
					 180, 181, 182, 183, 184, 185, 186, \
					 187, 188, 189, 190, 191, 192, 193, \
					 194, 195, 196, 197, 198, 199, 200, \
					 201, 202, 203, 204, 205, 206, 207, \
					 208, 209,}

#define CONFIG_SYS_NAND_ECCSIZE		512
#define CONFIG_SYS_NAND_ECCBYTES	26

#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x00100000

/* NAND: SPL falcon mode configs */
#ifdef CONFIG_SPL_OS_BOOT
#define CONFIG_SYS_NAND_SPL_KERNEL_OFFS	0x00200000 /* kernel offset */
#endif

#endif /* CONFIG_MTD_RAW_NAND */

#endif /* __CONFIG_PHYCORE_AM57X_H */
