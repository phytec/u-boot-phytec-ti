/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for PHYTEC phyCORE-AM62a7 kit
 *
 * Copyright (C) 2023 PHYTEC America LLC
 * Author: Garrett Giordano <ggiordano@phytec.com>
 */

#ifndef __PHYCORE_AM62AX_H
#define __PHYCORE_AM62AX_H

#include <linux/sizes.h>
#include <config_distro_bootcmd.h>
#include <environment/ti/mmc.h>
#include <environment/ti/k3_rproc.h>
#include <environment/ti/k3_dfu.h>

#include "phytec_am6_common.h"

#if defined(CONFIG_TARGET_PHYCORE_AM62A7_A53)
#define CONFIG_SPL_MAX_SIZE		SZ_1M
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SPL_TEXT_BASE + SZ_4M)
#else
#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0x3000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location allocated for Device Manager data and some memory filled in by
 * the boot ROM that we want to read out without any interference from the
 * C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(0x43c3e000 -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR         0x43c3a7f0
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START    0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE     SZ_16M
#endif

#if defined(CONFIG_TARGET_PHYCORE_AM62A7_A53)
#if defined(DEFAULT_RPROCS)
#undef DEFAULT_RPROCS
#endif
#define DEFAULT_RPROCS	""						\
		"0 /lib/firmware/am62a-mcu-r5f0_0-fw "			\
		"1 /lib/firmware/am62a-c71_0-fw "
#endif

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __PHYCORE_AM62A7_H */
