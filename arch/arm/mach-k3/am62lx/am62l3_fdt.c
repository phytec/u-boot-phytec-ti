// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 */

#include <asm/arch/hardware.h>
#include <asm/arch/k3-common-fdt.h>
#include <fdt_support.h>

static void fdt_fixup_cores_nodes_am62l3(void *blob, int core_nr)
{
	char node_path[32];

	if (core_nr < 1)
		return;

	for (; core_nr < AM62L_MAX_CORE_NR; core_nr++) {
		snprintf(node_path, sizeof(node_path), "/cpus/cpu@%d", core_nr);
		fdt_del_node_path(blob, node_path);
		snprintf(node_path, sizeof(node_path), "/cpus/cpu-map/cluster0/core%d", core_nr);
		fdt_del_node_path(blob, node_path);
	}
}

int ft_system_setup(void *blob, struct bd_info *bd)
{
	fdt_fixup_cores_nodes_am62l3(blob, k3_get_core_nr());

	return fdt_fixup_reserved(blob);
}
