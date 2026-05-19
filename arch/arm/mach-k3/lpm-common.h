/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * K3: LPM Architecture common definitions
 *
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 * Copyright (C) 2026 Bootlin
 */

#ifndef _LPM_COMMON_H_
#define _LPM_COMMON_H_

void __noreturn k3_do_resume(void);
void k3_lpm_process(void);

#endif
