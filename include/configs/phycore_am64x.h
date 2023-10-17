/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for PHYTEC phyCORE-AM64x kit
 *
 * Copyright (C) 2022 PHYTEC Messtechnik GmbH
 * Author: Wadim Egorov <w.egorov@phytec.de>
 */

#ifndef __PHYCORE_AM64X_H
#define __PHYCORE_AM64X_H

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1		0x880000000

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __PHYCORE_AM64X_H */
