// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 PHYTEC Messtechnik GmbH
 * Author: Wadim Egorov <w.egorov@phytec.de>
 */

#include <common.h>
#include <asm/io.h>
#include <env.h>
#include <env_internal.h>
#include <spl.h>
#include <fdt_support.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <extension_board.h>
#include <malloc.h>

#include "../common/am62_som_detection.h"

DECLARE_GLOBAL_DATA_PTR;

#define EEPROM_ADDR             0x50
#define EEPROM_ADDR_FALLBACK    -1

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = 0x80000000;

	return 0;
}

int dram_init_banksize(void)
{
	/* Bank 0 declares the memory available in the DDR low region */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = 0x80000000;
	gd->ram_size = 0x80000000;

	return 0;
}

#if defined(CONFIG_SPL_LOAD_FIT)
int board_fit_config_name_match(const char *name)
{
	if (!strcmp(name, "k3-am625-phycore-som-r5") || !strcmp(name, "k3-am625-phycore-som"))
		return 0;
	return -1;
}
#endif

#define CTRLMMR_USB0_PHY_CTRL   0x43004008
#define CTRLMMR_USB1_PHY_CTRL   0x43004018
#define CORE_VOLTAGE            0x80000000

#ifdef CONFIG_SPL_BOARD_INIT
void spl_board_init(void)
{
	u32 val;

	/* Set USB0 PHY core voltage to 0.85V */
	val = readl(CTRLMMR_USB0_PHY_CTRL);
	val &= ~(CORE_VOLTAGE);
	writel(val, CTRLMMR_USB0_PHY_CTRL);

	/* Set USB1 PHY core voltage to 0.85V */
	val = readl(CTRLMMR_USB1_PHY_CTRL);
	val &= ~(CORE_VOLTAGE);
	writel(val, CTRLMMR_USB1_PHY_CTRL);

	/* We have 32k crystal, so lets enable it */
	val = readl(MCU_CTRL_LFXOSC_CTRL);
	val &= ~(MCU_CTRL_LFXOSC_32K_DISABLE_VAL);
	writel(val, MCU_CTRL_LFXOSC_CTRL);
	/* Add any TRIM needed for the crystal here.. */
	/* Make sure to mux up to take the SoC 32k from the crystal */
	writel(MCU_CTRL_DEVICE_CLKOUT_LFOSC_SELECT_VAL,
	       MCU_CTRL_DEVICE_CLKOUT_32K_CTRL);

	/* Init DRAM size for R5/A53 SPL */
	dram_init_banksize();
}
#endif

#ifdef CONFIG_ENV_IS_IN_FAT
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
		break;
	case BOOT_DEVICE_MMC2:
		env_set_ulong("mmcdev", 1);
		break;
	};

	return 0;
}

#if IS_ENABLED(CONFIG_CMD_EXTENSION)
int extension_board_scan(struct list_head *extension_list)
{
	struct extension *extension = NULL;
	struct phytec_eeprom_data data;
	int count = 0;
	int ret;
	u8 option;

	ret = phytec_eeprom_data_setup(&data, 1, EEPROM_ADDR,
				       EEPROM_ADDR_FALLBACK);
	if (ret)
		return count;
	phytec_print_som_info(&data);

	return count;
}
#endif
