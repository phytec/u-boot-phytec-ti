/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for PHYTEC phyCORE-AM68x kit
 *
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 *	Author: Dominik Haller <d.haller@phytec.de>
 */

#ifndef __PHYCORE_AM68X_H
#define __PHYCORE_AM68X_H

#include <linux/sizes.h>

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1             0x880000000

/* SPL Loader Configuration */
#if defined(CONFIG_TARGET_PHYCORE_AM68X_A72)
#define CFG_SYS_UBOOT_BASE              0x50280000
/* Image load address in RAM for DFU boot*/
#else
#define CFG_SYS_UBOOT_BASE              0x50080000
#endif

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __CONFIG_PHYCORE_AM68X_H */
