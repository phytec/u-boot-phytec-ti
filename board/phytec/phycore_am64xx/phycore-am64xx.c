// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 PHYTEC America, LLC - https://www.phytec.com
 *
 * Based on board/ti/am64x/evm.c
 *
 */

#include <common.h>
#include <asm/io.h>
#include <spl.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

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
#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	if (!strcmp(name, "k3-am64xx-phycore-som-r5") || !strcmp(name, "k3-am64xx-phycore-som"))
		return 0;

	return -1;
}
#endif

#ifdef CONFIG_SPL_BOARD_INIT
#define CTRLMMR_USB0_PHY_CTRL	0x43004008
#define CORE_VOLTAGE		0x80000000

void spl_board_init(void)
{
	u32 val;

	/* Set USB PHY core voltage to 0.85V */
	val = readl(CTRLMMR_USB0_PHY_CTRL);
	val &= ~(CORE_VOLTAGE);
	writel(val, CTRLMMR_USB0_PHY_CTRL);
}
#endif

int board_late_init(void)
{
	u32 boot_device = get_boot_device();
	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		env_set_ulong("mmcdev", 0);
		env_set("bootpart", "0:2");
		break;
	case BOOT_DEVICE_MMC2:
		env_set_ulong("mmcdev", 1);
		env_set("bootpart", "1:2");
		break;
	};

	return 0;
}
