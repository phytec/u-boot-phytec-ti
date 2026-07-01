// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62L3 BeagleBadge
 *
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <env.h>
#include <fdtdec.h>
#include <fdt_support.h>

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

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	char fdtfile[50];

	snprintf(fdtfile, sizeof(fdtfile), "%s.dtb", CONFIG_DEFAULT_DEVICE_TREE);

	env_set("fdtfile", fdtfile);

	return 0;
}
#endif
