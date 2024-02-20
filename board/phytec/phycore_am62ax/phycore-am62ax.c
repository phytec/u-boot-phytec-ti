// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 PHYTEC America LLC
 * Author: Garrett Giordano <ggiordano@phytec.com>
 */

#include <common.h>
#include <asm/io.h>
#include <env.h>
#include <env_internal.h>
#include <spl.h>
#include <fdt_support.h>
#include <jffs2/load_kernel.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <extension_board.h>
#include <malloc.h>
#include <mtd_node.h>

#include "../common/am62a_som_detection.h"

DECLARE_GLOBAL_DATA_PTR;

#define EEPROM_ADDR             0x50
#define EEPROM_ADDR_FALLBACK    -1

#define AM6X_DISABLE_ETH_PHY_OVERLAY "k3-am6-phycore-disable-eth-phy.dtbo"
#define AM6X_DISABLE_SPI_NOR_OVERLAY "k3-am6-phycore-disable-spi-nor.dtbo"
#define AM6X_DISABLE_RTC_OVERLAY "k3-am6-phycore-disable-rtc.dtbo"

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	return fdtdec_setup_mem_size_base();
}

int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

#if defined(CONFIG_SPL_BUILD)
int do_board_detect(void)
{
	return 0;
}
#endif

#if defined(CONFIG_SPL_LOAD_FIT)
int board_fit_config_name_match(const char *name)
{
	if (!strcmp(name, "k3-am62a7-r5-phycore-som-2gb") || !strcmp(name, "k3-am62a7-phyboard-lyra-rdk"))
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

/* Functions borrowed from am62a7_init.c */
static u32 __get_backup_bootmedia(u32 devstat)
{
	u32 bkup_bootmode = (devstat & MAIN_DEVSTAT_BACKUP_BOOTMODE_MASK) >>
				MAIN_DEVSTAT_BACKUP_BOOTMODE_SHIFT;
	u32 bkup_bootmode_cfg =
			(devstat & MAIN_DEVSTAT_BACKUP_BOOTMODE_CFG_MASK) >>
				MAIN_DEVSTAT_BACKUP_BOOTMODE_CFG_SHIFT;

	switch (bkup_bootmode) {
	case BACKUP_BOOT_DEVICE_UART:
		return BOOT_DEVICE_UART;

	case BACKUP_BOOT_DEVICE_USB:
		return BOOT_DEVICE_USB;

	case BACKUP_BOOT_DEVICE_ETHERNET:
		return BOOT_DEVICE_ETHERNET;

	case BACKUP_BOOT_DEVICE_MMC:
		if (bkup_bootmode_cfg)
			return BOOT_DEVICE_MMC2;
		return BOOT_DEVICE_MMC1;

	case BACKUP_BOOT_DEVICE_SPI:
		return BOOT_DEVICE_SPI;

	case BACKUP_BOOT_DEVICE_I2C:
		return BOOT_DEVICE_I2C;

	case BACKUP_BOOT_DEVICE_DFU:
		if (bkup_bootmode_cfg & MAIN_DEVSTAT_BACKUP_USB_MODE_MASK)
			return BOOT_DEVICE_USB;
		return BOOT_DEVICE_DFU;
	};

	return BOOT_DEVICE_RAM;
}

static u32 __get_primary_bootmedia(u32 devstat)
{
	u32 bootmode = (devstat & MAIN_DEVSTAT_PRIMARY_BOOTMODE_MASK) >>
				MAIN_DEVSTAT_PRIMARY_BOOTMODE_SHIFT;
	u32 bootmode_cfg = (devstat & MAIN_DEVSTAT_PRIMARY_BOOTMODE_CFG_MASK) >>
				MAIN_DEVSTAT_PRIMARY_BOOTMODE_CFG_SHIFT;

	switch (bootmode) {
	case BOOT_DEVICE_OSPI:
		fallthrough;
	case BOOT_DEVICE_QSPI:
		fallthrough;
	case BOOT_DEVICE_XSPI:
		fallthrough;
	case BOOT_DEVICE_SPI:
		return BOOT_DEVICE_SPI;

	case BOOT_DEVICE_ETHERNET_RGMII:
		fallthrough;
	case BOOT_DEVICE_ETHERNET_RMII:
		return BOOT_DEVICE_ETHERNET;

	case BOOT_DEVICE_EMMC:
		return BOOT_DEVICE_MMC1;

	case BOOT_DEVICE_SPI_NAND:
		return BOOT_DEVICE_SPINAND;

	case BOOT_DEVICE_MMC:
		if ((bootmode_cfg & MAIN_DEVSTAT_PRIMARY_MMC_PORT_MASK) >>
				MAIN_DEVSTAT_PRIMARY_MMC_PORT_SHIFT)
			return BOOT_DEVICE_MMC2;
		return BOOT_DEVICE_MMC1;

	case BOOT_DEVICE_DFU:
		if ((bootmode_cfg & MAIN_DEVSTAT_PRIMARY_USB_MODE_MASK) >>
		    MAIN_DEVSTAT_PRIMARY_USB_MODE_SHIFT)
			return BOOT_DEVICE_USB;
		return BOOT_DEVICE_DFU;

	case BOOT_DEVICE_NOBOOT:
		return BOOT_DEVICE_RAM;
	}

	return bootmode;
}

u32 get_boot_device(void)
{
	u32 devstat = readl(CTRLMMR_MAIN_DEVSTAT);
	u32 bootindex = *(u32 *)(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX);
	u32 bootmedia;

	if (bootindex == K3_PRIMARY_BOOTMODE)
		bootmedia = __get_primary_bootmedia(devstat);
	else
		bootmedia = __get_backup_bootmedia(devstat);

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

#if IS_ENABLED(CONFIG_BOARD_LATE_INIT)
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
	case BOOT_DEVICE_ETHERNET:
		env_set("boot", "net");
		break;
	};

	return 0;
}
#endif

#if IS_ENABLED(CONFIG_CMD_EXTENSION)
int extension_board_scan(struct list_head *extension_list)
{
	struct extension *extension = NULL;
	struct phytec_eeprom_data data;
	int count = 0;
	int ret;

	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);
	if (ret || !data.valid)
		return count;

	phytec_print_som_info(&data);

	if (phytec_get_am62a_eth(&data) == 0) {
		extension = phytec_add_extension("Disable eth phy",
						 AM6X_DISABLE_ETH_PHY_OVERLAY, "");
		list_add_tail(&extension->list, extension_list);
		count++;
	}

	if (phytec_get_am62a_spi(&data) == PHYTEC_EEPROM_VALUE_X) {
		extension = phytec_add_extension("Disable SPI NOR Flash",
						 AM6X_DISABLE_SPI_NOR_OVERLAY, "");
		list_add_tail(&extension->list, extension_list);
		count++;
	}

	if (phytec_get_am62a_rtc(&data) == 0) {
		extension = phytec_add_extension("Disable RTC",
						 AM6X_DISABLE_RTC_OVERLAY, "");
		list_add_tail(&extension->list, extension_list);
		count++;
	}

	return count;
}
#endif

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	fdt_copy_fixed_partitions(blob);
	return 0;
}
#endif
