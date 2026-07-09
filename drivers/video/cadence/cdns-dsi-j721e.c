// SPDX-License-Identifier: GPL-2.0
/*
 * TI j721e Cadence DSI wrapper
 *
 * Based on Linux Kernel drivers/gpu/drm/bridge/cadence/cdns-dsi-j721e.c
 * Ported to U-Boot by Rahul Sharma <r-sharma3@ti.com>
 *
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 * Author: Rahul T R <r-ravikumar@ti.com>
 */

#include <linux/io.h>
#include "cdns-dsi-j721e.h"

#define DSI_WRAP_REVISION		0x0
#define DSI_WRAP_DPI_CONTROL		0x4
#define DSI_WRAP_DSC_CONTROL		0x8
#define DSI_WRAP_DPI_SECURE		0xc
#define DSI_WRAP_DSI_0_ASF_STATUS	0x10

#define DSI_WRAP_DPI_0_EN		BIT(0)
#define DSI_WRAP_DSI2_MUX_SEL		BIT(4)

static int cdns_dsi_j721e_init(struct cdns_dsi *dsi)
{
	struct udevice *dev = (struct udevice *)dsi->host.dev;

	dsi->j721e_base = dev_remap_addr_index(dev, 1);
	if (!dsi->j721e_base)
		return -EINVAL;
	return 0;
}

static void cdns_dsi_j721e_enable(struct cdns_dsi *dsi)
{
	/*
	 * Enable DPI0 as its input. DSS0 DPI2 is connected
	 * to DSI DPI0. This is the only supported configuration on
	 * J721E.
	 */
	writel(DSI_WRAP_DPI_0_EN, dsi->j721e_base + DSI_WRAP_DPI_CONTROL);
}

static void cdns_dsi_j721e_disable(struct cdns_dsi *dsi)
{
	/* Put everything to defaults  */
	writel(0, dsi->j721e_base + DSI_WRAP_DPI_CONTROL);
}

const struct cdns_dsi_platform_ops dsi_ti_j721e_ops = {
	.init = cdns_dsi_j721e_init,
	.enable = cdns_dsi_j721e_enable,
	.disable = cdns_dsi_j721e_disable,
};
