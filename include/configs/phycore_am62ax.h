/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for PHYTEC phyCORE-AM62Ax kit
 *
 * Copyright (C) 2023 PHYTEC America LLC
 * Author: Garrett Giordano <ggiordano@phytec.com>
 */

#ifndef __PHYCORE_AM62AX_H
#define __PHYCORE_AM62AX_H

#include <linux/sizes.h>

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1		0x880000000

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __PHYCORE_AM62AX_H */
