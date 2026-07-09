/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Based on Linux Kernel drivers/gpu/drm/bridge/cadence/cdns-dsi-core.c
 * Ported to U-Boot by Rahul Sharma <r-sharma3@ti.com>
 *
 * Copyright: 2017 Cadence Design Systems, Inc.
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 * Author: Boris Brezillon <boris.brezillon@bootlin.com>
 */

#ifndef __CDNS_DSI_H__
#define __CDNS_DSI_H__

#include <generic-phy.h>
#include <mipi_dsi.h>
#include <reset.h>
#include <phy-mipi-dphy.h>
#include <dm/read.h>
#include <clk.h>

struct cdns_dsi_output {
	struct mipi_dsi_device *dev;
	struct phy_configure_opts_mipi_dphy phy_opts;
};

enum cdns_dsi_input_id {
	CDNS_SDI_INPUT,
	CDNS_DPI_INPUT,
	CDNS_DSC_INPUT,
};

struct cdns_dsi_cfg {
	unsigned int hfp;
	unsigned int hsa;
	unsigned int hbp;
	unsigned int hact;
	unsigned int htotal;
};

struct cdns_dsi_input {
	enum cdns_dsi_input_id id;
};

struct cdns_dsi;

/**
 * struct cdns_dsi_platform_ops - CDNS DSI Platform operations
 * @init: Called in the CDNS DSI probe
 * @deinit: Called in the CDNS DSI remove
 * @enable: Called at the beginning of CDNS DSI bridge enable
 * @disable: Called at the end of CDNS DSI bridge disable
 */
struct cdns_dsi_platform_ops {
	int (*init)(struct cdns_dsi *dsi);
	void (*deinit)(struct cdns_dsi *dsi);
	void (*enable)(struct cdns_dsi *dsi);
	void (*disable)(struct cdns_dsi *dsi);
};

struct cdns_dsi {
	struct mipi_dsi_host host;
	void __iomem *base;
#if defined(CONFIG_CDNS_DSI_J721E) || defined(CONFIG_SPL_CDNS_DSI_J721E)
	void __iomem *j721e_base;
#endif
	const struct cdns_dsi_platform_ops *platform_ops;
	struct cdns_dsi_input input;
	struct cdns_dsi_output output;
	unsigned int direct_cmd_fifo_depth;
	unsigned int rx_fifo_depth;
	struct clk *dsi_p_clk;
	struct reset_ctl dsi_p_rst;
	struct clk *dsi_sys_clk;
	bool phy_initialized;
	bool link_initialized;
	struct phy dphy;
	struct cdns_dsi_cfg dsi_cfg;
};

struct cdns_dsi_host_priv {
	struct cdns_dsi dsi;
	struct mipi_dsi_device *device;
	struct display_timing *timings;
	unsigned int max_data_lanes;
	const struct mipi_dsi_phy_ops *phy_ops;
	u32 rounded_pclk_khz;
};

#endif /* !__CDNS_DSI_H__ */
