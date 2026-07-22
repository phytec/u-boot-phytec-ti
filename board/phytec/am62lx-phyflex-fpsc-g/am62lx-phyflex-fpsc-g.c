// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 PHYTEC Messtechnik GmbH
 * Author: Dominik Haller <d.haller@phytec.de>
 */

#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <dm/uclass.h>
#include <env.h>
#include <fdt_support.h>
#include <spl.h>
#include <i2c.h>
#include <mach/k3-ddr.h>

#include "../common/am6_som_detection.h"

int board_init(void)
{
	return 0;
}

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
	return fdtdec_setup_memory_banksize();
}

#if IS_ENABLED(CONFIG_XPL_BUILD)
static u8 dram_get_size(void)
{
	struct phytec_eeprom_data data;
	int ret;

	ret = phytec_eeprom_data_setup(&data, CONFIG_PHYTEC_EEPROM_BUS, EEPROM_ADDR);
	if (!ret && data.valid)
		return phytec_get_am6_ddr_size(&data);

	/* Default DDR size is 1GB */
	return EEPROM_RAM_SIZE_1GB;
}

void spl_perform_board_fixups(struct spl_image_info *spl_image)
{
	u64 start = CFG_SYS_SDRAM_BASE;
	u64 size;
	u8 ram_size;
	int ret;

	ram_size = dram_get_size();

	switch (ram_size) {
	case EEPROM_RAM_SIZE_1GB:
		size = 0x40000000;
		break;
	case EEPROM_RAM_SIZE_2GB:
		size = 0x80000000;
		break;
	default:
		size = 0x40000000;
		pr_notice("DDR size %d is not supported, using default\n", ram_size);
		break;
	}

	ret = fdt_fixup_memory_banks(spl_image->fdt_addr, &start, &size, 1);
	if (ret < 0)
		pr_err("fdt_fixup_memory_banks failed (ret = %d)\n", ret);
}
#endif
