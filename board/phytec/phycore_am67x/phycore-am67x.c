// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for phyCORE-AM67x
 *
 * Copyright (C) 2025 PHYTEC America LLC
 * Author: Nathan Morrisson <nmorrisson@phytec.com>
 *
 */

#include <asm/arch/hardware.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <dm/root.h>
#include <env.h>
#include <fdt_support.h>
#include <spl.h>

#include "../common/k3/k3_memory_fixups.h"

int dram_init(void)
{
	int ret;

	ret = fdtdec_setup_mem_size_base_lowest();
	if (ret)
		printf("Error setting up mem size and base. %d\n", ret);

	return ret;
}

int dram_init_banksize(void)
{
	int ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		printf("Error setting up memory banksize. %d\n", ret);

	return ret;
}

int board_init(void)
{
	return 0;
}

void spl_board_init(void)
{
	u32 val;

	/* We have 32k crystal, so lets enable it */
	val = readl(MCU_CTRL_LFXOSC_CTRL);
	val &= ~(MCU_CTRL_LFXOSC_32K_DISABLE_VAL);
	writel(val, MCU_CTRL_LFXOSC_CTRL);
	/* Add any TRIM needed for the crystal here.. */
	/* Make sure to mux up to take the SoC 32k from the crystal */
	writel(MCU_CTRL_DEVICE_CLKOUT_LFOSC_SELECT_VAL,
	       MCU_CTRL_DEVICE_CLKOUT_32K_CTRL);
}

#if defined(CONFIG_SPL_BUILD)
void spl_perform_fixups(struct spl_image_info *spl_image)
{
	if (IS_ENABLED(CONFIG_K3_DDRSS) && IS_ENABLED(CONFIG_K3_INLINE_ECC))
		fixup_ddr_driver_for_ecc(spl_image);
	else
		fixup_memory_node(spl_image);
}
#endif
