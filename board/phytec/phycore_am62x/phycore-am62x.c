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
#include <jffs2/load_kernel.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <extension_board.h>
#include <malloc.h>
#include <mtd_node.h>
#include <dm/uclass.h>
#include <k3-ddrss.h>
#include "phycore-ddr-data.h"

#include "../common/am62_som_detection.h"

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

enum {
	EEPROM_RAM_SIZE_512MB = 0,
	EEPROM_RAM_SIZE_1GB = 1,
	EEPROM_RAM_SIZE_2GB = 2,
	EEPROM_RAM_SIZE_4GB = 4
};

static u8 phytec_get_am62_ddr_size_default(void)
{
	int ret;
	struct phytec_eeprom_data data;

	if (IS_ENABLED(CONFIG_PHYCORE_AM62X_RAM_SIZE_FIX)) {
		if (IS_ENABLED(CONFIG_PHYCORE_AM62X_RAM_SIZE_1GB))
			return EEPROM_RAM_SIZE_1GB;
		else if (IS_ENABLED(CONFIG_PHYCORE_AM62X_RAM_SIZE_2GB))
			return EEPROM_RAM_SIZE_2GB;
		else if (IS_ENABLED(CONFIG_PHYCORE_AM62X_RAM_SIZE_4GB))
			return EEPROM_RAM_SIZE_4GB;
	}

	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);

	if (!ret && data.valid)
		return phytec_get_am62_ddr_size(&data);
	/* Default DDR size is 2GB */
	return EEPROM_RAM_SIZE_2GB;
}

int dram_init(void)
{
	u8 ram_size;

	ram_size = phytec_get_am62_ddr_size_default();

#if defined(CONFIG_K3_AM64_DDRSS)
	/*
	* HACK: ddrss driver support 2GB RAM by default
	* V2A_CTL_REG should be updated to support other RAM size
	*/
#define AM64_DDRSS_SS_BASE	0x0F300000
#define DDRSS_V2A_CTL_REG	0x0020
	if (ram_size == EEPROM_RAM_SIZE_4GB) {
		writel(0x00000210, AM64_DDRSS_SS_BASE + DDRSS_V2A_CTL_REG);
	}
#endif

	switch (ram_size) {
	case EEPROM_RAM_SIZE_1GB:
		gd->ram_size = 0x40000000;
		break;
	case EEPROM_RAM_SIZE_2GB:
		gd->ram_size = 0x80000000;
		break;
	case EEPROM_RAM_SIZE_4GB:
#ifdef CONFIG_PHYS_64BIT
		gd->ram_size = 0x100000000;
#else
		gd->ram_size = 0x80000000;
#endif
		break;
	default:
		gd->ram_size = 0x80000000;
	}

	return 0;
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
	u8 ram_size;

	ram_size = phytec_get_am62_ddr_size_default();
	switch (ram_size) {
	case EEPROM_RAM_SIZE_1GB:
		gd->bd->bi_dram[0].start = CFG_SYS_SDRAM_BASE;
		gd->bd->bi_dram[0].size = 0x40000000;
		gd->ram_size = 0x40000000;
		break;

	case EEPROM_RAM_SIZE_2GB:
		gd->bd->bi_dram[0].start = CFG_SYS_SDRAM_BASE;
		gd->bd->bi_dram[0].size = 0x80000000;
		gd->ram_size = 0x80000000;
		break;

	case EEPROM_RAM_SIZE_4GB:
		/* Bank 0 declares the memory available in the DDR low region */
		gd->bd->bi_dram[0].start = CFG_SYS_SDRAM_BASE;
		gd->bd->bi_dram[0].size = 0x80000000;
		gd->ram_size = 0x80000000;

#ifdef CONFIG_PHYS_64BIT
		/* Bank 1 declares the memory available in the DDR upper region */
		gd->bd->bi_dram[1].start = CFG_SYS_SDRAM_BASE1;
		gd->bd->bi_dram[1].size = 0x80000000;
		gd->ram_size = 0x100000000;
#endif
		break;
	default:
		/* Continue with default 2GB setup */
		gd->bd->bi_dram[0].start = CFG_SYS_SDRAM_BASE;
		gd->bd->bi_dram[0].size = 0x80000000;
		gd->ram_size = 0x80000000;
		printf("DDR size %d is not supported\n", ram_size);
	}

	return 0;
}

int qspi_fixup(void *blob, struct phytec_eeprom_data *data)
{
	ofnode node;
	int ret;

	if (!data)
		return 0;

	if (!phytec_am62_is_qspi(data))
		return 0;

	if (blob) {
		do_fixup_by_compat_u32(blob, "jedec,spi-nor", "spi-tx-bus-width", 1, 0);
		do_fixup_by_compat_u32(blob, "jedec,spi-nor", "spi-rx-bus-width", 4, 0);
	} else {
		node = ofnode_by_compatible(ofnode_null(), "jedec,spi-nor");
		if (!ofnode_valid(node))
			return -1;
		ret = ofnode_write_u32(node, "spi-tx-bus-width", 1);
		if (ret < 0)
			return ret;
		ret = ofnode_write_u32(node, "spi-rx-bus-width", 4);
		if (ret < 0)
			return ret;
	}
	return 0;
}

#if defined(CONFIG_SPL_BUILD)
#if defined(CONFIG_K3_AM64_DDRSS)

int update_ddrss_timings(void)
{
	int ret;
	u8 ram_size;
	struct ddrss *ddr_patch;
	void *fdt = (void *)gd->fdt_blob;

	ram_size = phytec_get_am62_ddr_size_default();
	switch (ram_size) {
	case EEPROM_RAM_SIZE_1GB:
		ddr_patch = &phycore_ddrss_data[PHYCORE_1GB];
		break;
	case EEPROM_RAM_SIZE_2GB:
		ddr_patch = NULL;
		break;
	case EEPROM_RAM_SIZE_4GB:
		ddr_patch = &phycore_ddrss_data[PHYCORE_4GB];
		break;
	default:
		break;
	}

	/* Nothing to patch */
	if (!ddr_patch)
		return 0;

	debug("Applying DDRSS timings patch for ram_size %d\n", ram_size);

	ret = fdt_apply_ddrss_timings_patch(fdt, ddr_patch);
	if (ret < 0) {
		printf("Failed to apply ddrs timings patch %d\n", ret);
		return ret;
	}

	return 0;
}

int do_board_detect(void)
{
	int ret;
	struct phytec_eeprom_data data;

	ret = update_ddrss_timings();
	if (ret)
		return ret;

	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);
	if (ret || !data.valid)
		return 0;

	return qspi_fixup(NULL, &data);
}

static void fixup_ddr_driver_for_ecc(struct spl_image_info *spl_image)
{
	struct udevice *dev;
	int ret;

	dram_init_banksize();

	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret)
		panic("Cannot get RAM device for ddr size fixup: %d\n", ret);

	ret = k3_ddrss_ddr_fdt_fixup(dev, spl_image->fdt_addr, gd->bd);
	if (ret)
		printf("Error fixing up ddr node for ECC use! %d\n", ret);
}
#else
static void fixup_memory_node(struct spl_image_info *spl_image)
{
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];
	int bank;
	int ret;

	dram_init();
	dram_init_banksize();

	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] =  gd->bd->bi_dram[bank].start;
		size[bank] = gd->bd->bi_dram[bank].size;
	}

	/* dram_init functions use SPL fdt, and we must fixup u-boot fdt */
	ret = fdt_fixup_memory_banks(spl_image->fdt_addr, start, size,
				     CONFIG_NR_DRAM_BANKS);
	if (ret)
		printf("Error fixing up memory node! %d\n", ret);
}
#endif

void spl_perform_fixups(struct spl_image_info *spl_image)
{
#if defined(CONFIG_K3_AM64_DDRSS)
	fixup_ddr_driver_for_ecc(spl_image);
#else
	fixup_memory_node(spl_image);
#endif
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

/* Functions borrowed from am625_init.c */
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

	case BOOT_DEVICE_SERIAL_NAND:
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

	if (phytec_get_am62_eth(&data) == 0) {
		extension = phytec_add_extension("Disable eth phy",
						 AM6X_DISABLE_ETH_PHY_OVERLAY, "");
		list_add_tail(&extension->list, extension_list);
		count++;
	}

	if (phytec_get_am62_spi(&data) == PHYTEC_EEPROM_VALUE_X) {
		extension = phytec_add_extension("Disable SPI NOR Flash",
						 AM6X_DISABLE_SPI_NOR_OVERLAY, "");
		list_add_tail(&extension->list, extension_list);
		count++;
	}

	if (phytec_get_am62_rtc(&data) == 0) {
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
	int ret;
	struct phytec_eeprom_data data;

	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);
	if (ret || !data.valid)
		return 0;
	fdt_copy_fixed_partitions(blob);
	return qspi_fixup(blob, &data);
}
#endif

#if IS_ENABLED(CONFIG_OF_BOARD_FIXUP)
int board_fix_fdt(void *blob)
{
	int ret;
	struct phytec_eeprom_data data;

	ret = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR);
	if (ret || !data.valid)
		return 0;
	return qspi_fixup(NULL, &data);
}
#endif
