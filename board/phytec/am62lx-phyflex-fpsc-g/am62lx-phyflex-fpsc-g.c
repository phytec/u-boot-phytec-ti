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

#define TUSB_PORT_POL_CRTL_REG  0xB
#define TUSB_CUSTOM_POL         BIT(7)
#define TUSB_P0_POL             BIT(0)

/*
 * WORKAROUND for PCM-937-L 1618.0, 1618.1.
 * USB HUB TUSB8042A has swapped upstream pin polarity.
 * Set i2c registers to inform the hub that the lines
 * are swapped.
 *
 * We also notice that the HUB i2c address might not be
 * as expected for unknown reasons. Test all 4 possible i2c
 * addresses to write to the device.
 *
 */

void tusb8042a_swap_lines(void)
{
	const u8 pol_swap_val = (TUSB_CUSTOM_POL | TUSB_P0_POL);
	const int addr[4] = {0x44, 0x45, 0x46, 0x47};
	struct udevice *dev;
	int i, ret;

	for (i = 0; i < 4; i++) {
		ret = i2c_get_chip_for_busnum(3, addr[i], 1, &dev);
		if (!ret) {
			dm_i2c_write(dev, TUSB_PORT_POL_CRTL_REG, &pol_swap_val, 1);
			break;
		};
	}

	if (ret)
		printf("TUSB8042A: Failed to fixup USB HUB.\n");
}

int board_init(void)
{
	tusb8042a_swap_lines();

	return 0;
}

int dram_init(void)
{
	int ret;

	ret = fdtdec_setup_mem_size_base();
	if (ret)
		return ret;

	return k3_mem_map_init();
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

	if (IS_ENABLED(CONFIG_PHYFLEX_AM62LX_RAM_SIZE_FIX)) {
		if (IS_ENABLED(CONFIG_PHYFLEX_AM62LX_RAM_SIZE_1GB))
			return EEPROM_RAM_SIZE_1GB;
		else if (IS_ENABLED(CONFIG_PHYFLEX_AM62LX_RAM_SIZE_2GB))
			return EEPROM_RAM_SIZE_2GB;
	}
	ret = phytec_eeprom_data_setup(&data, CONFIG_EEPROM_BUS, EEPROM_ADDR);
	if (!ret && data.valid)
		return phytec_get_am6_ddr_size(&data);

	/* Default DDR size is 1GB */
	return EEPROM_RAM_SIZE_1GB;
}

void spl_perform_fixups(struct spl_image_info *spl_image)
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
