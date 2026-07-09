/* SPDX-License-Identifier: GPL-2.0 */
/*
 * TI j721e Cadence DSI wrapper
 *
 * Based on Linux Kernel drivers/gpu/drm/bridge/cadence/cdns-dsi-j721e.c
 * Ported to U-Boot by Rahul Sharma <r-sharma3@ti.com>
 *
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 * Author: Rahul T R <r-ravikumar@ti.com>
 */

#ifndef __CDNS_DSI_J721E_H__
#define __CDNS_DSI_J721E_H__

#include "cdns-dsi-core.h"

#define DSI_WRAP_REVISION		0x0
#define DSI_WRAP_DPI_CONTROL		0x4

extern const struct cdns_dsi_platform_ops dsi_ti_j721e_ops;

#endif /* !__CDNS_DSI_J721E_H__ */
