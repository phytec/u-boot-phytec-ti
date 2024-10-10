// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Dominik Haller <d.haller@phytec.de>
 */

#include <common.h>
#include <env.h>
#include <env_internal.h>
#include <fdt_support.h>
#include <generic-phy.h>
#include <image.h>
#include <init.h>
#include <log.h>
#include <net.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hardware.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <spl.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <dm/root.h>

#include "../common/am68_som_detection.h"

DECLARE_GLOBAL_DATA_PTR;

#define EEPROM_ADDR             0x50
#define EEPROM_ADDR_FALLBACK    -1

#define EEPROM_DATA *(struct phytec_eeprom_data *) \
		     (CONFIG_SYS_K3_MCU_SCRATCHPAD_BASE)

int board_init(void)
{
	return 0;
}

enum {
	EEPROM_RAM_SIZE_1X512MB,
	EEPROM_RAM_SIZE_1X1GB,
	EEPROM_RAM_SIZE_2X512MB,
	EEPROM_RAM_SIZE_1X2GB,
	EEPROM_RAM_SIZE_2X1GB,
	EEPROM_RAM_SIZE_1X4GB,
	EEPROM_RAM_SIZE_2X2GB,
	EEPROM_RAM_SIZE_1X8GB,
	EEPROM_RAM_SIZE_2X4GB,
	EEPROM_RAM_SIZE_2X8GB
};

static u8 phytec_get_am68_ddr_size_default(void)
{
	struct phytec_eeprom_data data = EEPROM_DATA;

	if (IS_ENABLED(CONFIG_PHYCORE_AM68X_RAM_SIZE_FIX)) {
		if (IS_ENABLED(CONFIG_PHYCORE_AM68X_RAM_SIZE_2X2GB))
			return EEPROM_RAM_SIZE_2X2GB;
		else if (IS_ENABLED(CONFIG_PHYCORE_AM68X_RAM_SIZE_2X4GB))
			return EEPROM_RAM_SIZE_2X4GB;
	}

	if (data.valid)
		return phytec_get_am68_ddr_size(&data);
	/* Default DDR size is 4GB */
	return EEPROM_RAM_SIZE_2X2GB;
}

int dram_init(void)
{
	s32 ret;
	ret = fdtdec_setup_mem_size_base_lowest();
	if (ret)
		printf("Error setting up mem size and base. %d\n", ret);
	return ret;
}

phys_size_t board_get_usable_ram_top(phys_size_t total_size)
{
#ifdef CONFIG_PHYS_64BIT
	/* Limit RAM used by U-Boot to the DDR low region */
	if (gd->ram_top > 0x100000000)
		return 0x100000000;
#endif

	return gd->ram_top;
}

int dram_init_banksize(void)
{
	s32 ret;
	ret = fdtdec_setup_memory_banksize();
	if (ret)
		printf("Error setting up memory banksize. %d\n", ret);

	return ret;
}

#if defined(CONFIG_SPL_LOAD_FIT) && defined(CONFIG_SPL_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	u8 ram_size;

	if (IS_ENABLED(CONFIG_CPU_V7R)) {
		ram_size = phytec_get_am68_ddr_size_default();
	switch (ram_size) {
	case EEPROM_RAM_SIZE_2X2GB:
		if (!strcmp(name, "k3-am68-r5-phycore-som-4gb"))
			return 0;
		break;
	case EEPROM_RAM_SIZE_2X4GB:
		if (!strcmp(name, "k3-am68-r5-phycore-som-8gb"))
			return 0;
		break;
	default:
		if (!strcmp(name, "k3-am68-r5-phycore-som-4gb"))
			return 0;
		break;
		}
	}

	return -1;
}
#endif
static u32 __get_backup_bootmedia(u32 main_devstat)
{
	u32 bkup_boot = (main_devstat & MAIN_DEVSTAT_BKUP_BOOTMODE_MASK) >>
			MAIN_DEVSTAT_BKUP_BOOTMODE_SHIFT;

	switch (bkup_boot) {
	case BACKUP_BOOT_DEVICE_USB:
		return BOOT_DEVICE_DFU;
	case BACKUP_BOOT_DEVICE_UART:
		return BOOT_DEVICE_UART;
	case BACKUP_BOOT_DEVICE_ETHERNET:
		return BOOT_DEVICE_ETHERNET;
	case BACKUP_BOOT_DEVICE_MMC2:
	{
		u32 port = (main_devstat & MAIN_DEVSTAT_BKUP_MMC_PORT_MASK) >>
			    MAIN_DEVSTAT_BKUP_MMC_PORT_SHIFT;
		if (port == 0x0)
			return BOOT_DEVICE_MMC1;
		return BOOT_DEVICE_MMC2;
	}
	case BACKUP_BOOT_DEVICE_SPI:
		return BOOT_DEVICE_SPI;
	case BACKUP_BOOT_DEVICE_I2C:
		return BOOT_DEVICE_I2C;
	}

	return BOOT_DEVICE_RAM;
}

static u32 __get_primary_bootmedia(u32 main_devstat, u32 wkup_devstat)
{
	u32 bootmode = (wkup_devstat & WKUP_DEVSTAT_PRIMARY_BOOTMODE_MASK) >>
			WKUP_DEVSTAT_PRIMARY_BOOTMODE_SHIFT;

	bootmode |= (main_devstat & MAIN_DEVSTAT_BOOT_MODE_B_MASK) <<
			BOOT_MODE_B_SHIFT;

	if (bootmode == BOOT_DEVICE_OSPI || bootmode == BOOT_DEVICE_QSPI ||
	    bootmode == BOOT_DEVICE_XSPI)
		bootmode = BOOT_DEVICE_SPI;

	if (bootmode == BOOT_DEVICE_MMC2) {
		u32 port = (main_devstat &
			    MAIN_DEVSTAT_PRIM_BOOTMODE_MMC_PORT_MASK) >>
			   MAIN_DEVSTAT_PRIM_BOOTMODE_PORT_SHIFT;
		if (port == 0x0)
			bootmode = BOOT_DEVICE_MMC1;
	}

	return bootmode;
}

u32 get_boot_device(void)
{
	u32 main_devstat = readl(CTRLMMR_MAIN_DEVSTAT);
	u32 wkup_devstat = readl(CTRLMMR_WKUP_DEVSTAT);
	u32 bootindex = *(u32 *)(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX);
	u32 bootmedia;

	if (bootindex == K3_PRIMARY_BOOTMODE)
		bootmedia = __get_primary_bootmedia(main_devstat, wkup_devstat);
	else
		bootmedia = __get_backup_bootmedia(main_devstat);

	return bootmedia;
}

#if IS_ENABLED(CONFIG_ENV_IS_IN_FAT) || IS_ENABLED(CONFIG_ENV_IS_IN_MMC)
int mmc_get_env_dev(void)
{
	u32 boot_device = get_boot_device();

	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		return 0;
	case BOOT_DEVICE_MMC2:
		return 1;
	};

	return CONFIG_SYS_MMC_ENV_DEV;
}
#endif

enum env_location env_get_location(enum env_operation op, int prio)
{
	u32 boot_device = get_boot_device();

	if (prio)
		return ENVL_UNKNOWN;

	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
	case BOOT_DEVICE_MMC2:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_FAT))
			return ENVL_FAT;
		if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC))
			return ENVL_MMC;
	case BOOT_DEVICE_SPI:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_SPI_FLASH))
			return ENVL_SPI_FLASH;
	default:
		return ENVL_NOWHERE;
	};
}

int board_late_init(void)
{
	u32 boot_device = get_boot_device();

	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		env_set_ulong("mmcdev", 0);
		env_set("boot", "mmc");
		break;
	case BOOT_DEVICE_MMC2:
		env_set_ulong("mmcdev", 1);
		env_set("boot", "mmc");
		break;
	case BOOT_DEVICE_SPI:
		env_set("boot", "spi");
		break;
	};

	return 0;
}

void spl_board_init(void)
{
}
#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_PHYS_64BIT
void spl_perform_fixups(struct spl_image_info *spl_image)
{
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];
	int bank;
	int ret;
	u8 ram_size;

	dram_init();
	dram_init_banksize();

	ram_size = phytec_get_am68_ddr_size_default();
	switch (ram_size) {
	case EEPROM_RAM_SIZE_2X2GB:
		gd->bd->bi_dram[1].start = 0x880000000;
		gd->bd->bi_dram[1].size = 0x80000000;
		gd->ram_size = 0x10000000;
		break;
	case EEPROM_RAM_SIZE_2X4GB:
		gd->bd->bi_dram[1].start = 0x880000000;
		gd->bd->bi_dram[1].size = 0x180000000;
		gd->ram_size = 0x20000000;
		break;
	default:
		gd->bd->bi_dram[1].start = 0x880000000;
		gd->bd->bi_dram[1].size = 0x80000000;
		gd->ram_size = 0x10000000;
	}

	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = gd->bd->bi_dram[bank].start;
		size[bank] = gd->bd->bi_dram[bank].size;
	}

	ret = fdt_fixup_memory_banks(spl_image->fdt_addr, start, size, CONFIG_NR_DRAM_BANKS);
	if (ret)
		printf("Error fixing up memory banks for A72 devicetree. %d\n", ret);
}
#endif
#ifdef CONFIG_PHYTEC_AM68_SOM_DETECTION
void do_board_detect(void)
{
	int ret;
	struct phytec_eeprom_data data;

	/* Read I2C EEPROM */
	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);
	if (ret) {
		printf("%s: I2C read failed or EEPROM information is invalid!\n"
		       "Please flash your SOM's EEPROM with valid information.\n",
		       __func__);
	} else {
		/* Store I2C EEPROM data in SRAM to avoid multiple I2C reads */
		EEPROM_DATA = data;
	}
}
#else
void do_board_detect(void) { }
#endif
#if defined(CONFIG_SPL_MULTI_DTB_FIT)
void embedded_dtb_select(void)
{
	int ret;

	ret = fdtdec_setup();
	if (ret)
		printf("Error setting up new devicetree!: %d\n", ret);
}
#endif
void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

	k3_spl_init();
	do_board_detect();
#if defined(CONFIG_SPL_MULTI_DTB_FIT)
	embedded_dtb_select();
#endif
	k3_mem_init();

	if (IS_ENABLED(CONFIG_K3_AVS0)) {
		ret = uclass_get_device_by_driver(UCLASS_MISC, DM_DRIVER_GET(k3_avs), &dev);
		if (ret)
			printf("AVS init failed: %d\n", ret);
	}
}
#endif
