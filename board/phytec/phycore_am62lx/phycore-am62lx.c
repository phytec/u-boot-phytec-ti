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
