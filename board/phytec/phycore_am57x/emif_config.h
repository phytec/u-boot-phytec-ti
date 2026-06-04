/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015 PHYTEC America, LLC
 * Author: Russell Robinson <rrobinson@phytec.com>
 *
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#ifndef _EMIF_CONFIG_PHYCORE_AM57X_
#define _EMIF_CONFIG_PHYCORE_AM57X_

const struct dmm_lisa_map_regs am57xx_512mx1_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80500100,
	.is_ma_present  = 0x1
};

const struct dmm_lisa_map_regs am57xx_1gx1_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80600100,
	.is_ma_present  = 0x1
};

const struct dmm_lisa_map_regs am57xx_1gx2_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80780300,
	.is_ma_present  = 0x1
};

const struct dmm_lisa_map_regs am57xx_2gx1_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80700100,
	.is_ma_present  = 0x1
};

const struct dmm_lisa_map_regs am574x_1gx1_ecc_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80600100,
	.is_ma_present  = 0x1
};

const struct dmm_lisa_map_regs am574x_2gx1_ecc_lisa_regs = {
	.dmm_lisa_map_3 = 0xFF020100,
	.dmm_lisa_map_2 = 0x80700100,
	.is_ma_present  = 0x1
};

/* MT41K128M16JT-125 memory timings */
const struct emif_regs am57xx_emif_532mhz_128m16_regs = {
	.sdram_config_init	= 0x61851ab2,
	.sdram_config		= 0x61851ab2,
	.sdram_config2		= 0x08000000,
	.ref_ctrl		= 0x000040f1,
	.ref_ctrl_final		= 0x00001035,
	.sdram_tim1		= 0xeeef2673,
	.sdram_tim2		= 0x305c7fda,
	.sdram_tim3		= 0x409f8558,
	.read_idle_ctrl		= 0x00090000,
	.zq_config		= 0x5007190b,
	.temp_alert_config	= 0x00000000,
	.emif_ddr_phy_ctlr_1_init = 0x0024400b,
	.emif_ddr_phy_ctlr_1	= 0x0e24400b,
	.emif_ddr_ext_phy_ctrl_1 = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2 = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3 = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4 = 0x009b009b,
	.emif_ddr_ext_phy_ctrl_5 = 0x009e009e,
	.emif_rd_wr_lvl_rmp_win	= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl	= 0x80000000,
	.emif_rd_wr_lvl_ctl	= 0x00000000,
	.emif_rd_wr_exec_thresh	= 0x00000305
};

/* MT41K256M16TW-107 memory timings */
const struct emif_regs am57xx_emif_532mhz_256m16_regs = {
	.sdram_config_init	= 0x61851b32,
	.sdram_config		= 0x61851b32,
	.sdram_config2		= 0x08000000,
	.ref_ctrl		= 0x000040f1,
	.ref_ctrl_final		= 0x00001035,
	.sdram_tim1		= 0xeeef265b,
	.sdram_tim2		= 0x308f7fda,
	.sdram_tim3		= 0x409f88a8,
	.read_idle_ctrl		= 0x00090000,
	.zq_config		= 0x5007190b,
	.temp_alert_config	= 0x00000000,
	.emif_ddr_phy_ctlr_1_init = 0x0024400b,
	.emif_ddr_phy_ctlr_1	= 0x0e24400b,
	.emif_ddr_ext_phy_ctrl_1 = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2 = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3 = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4 = 0x009b009b,
	.emif_ddr_ext_phy_ctrl_5 = 0x009e009e,
	.emif_rd_wr_lvl_rmp_win	= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl	= 0x80000000,
	.emif_rd_wr_lvl_ctl	= 0x00000000,
	.emif_rd_wr_exec_thresh	= 0x00000305
};

/* MT41K256M16TW-107 (with ECC enabled) memory timings */
const struct emif_regs am574x_emif_532mhz_256m16_ecc_regs = {
	.sdram_config_init		= 0x61852332,
	.sdram_config			= 0x61852332,
	.sdram_config2			= 0x00000000,
	.ref_ctrl			= 0x100040F1,
	.ref_ctrl_final			= 0x10001106,
	.sdram_tim1			= 0xCEEF36F3,
	.sdram_tim2			= 0x308F7FDA,
	.sdram_tim3			= 0x407F88A8,
	.read_idle_ctrl			= 0x00050000,
	.zq_config			= 0x5007190B,
	.temp_alert_config		= 0x00000000,
	.emif_ddr_phy_ctlr_1_init	= 0x0024400C,
	.emif_ddr_phy_ctlr_1		= 0x0E24400C,
	.emif_ddr_ext_phy_ctrl_1	= 0x10040100,
	.emif_ddr_ext_phy_ctrl_2	= 0x00910091,
	.emif_ddr_ext_phy_ctrl_3	= 0x00950095,
	.emif_ddr_ext_phy_ctrl_4	= 0x009b009b,
	.emif_ddr_ext_phy_ctrl_5	= 0x009e009e,
	.emif_rd_wr_lvl_rmp_win		= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl		= 0x80000000,
	.emif_rd_wr_lvl_ctl		= 0x00000000,
	.emif_rd_wr_exec_thresh		= 0x00000305,
	.emif_ecc_ctrl_reg		= 0xD0000001,
	.emif_ecc_address_range_1	= 0x3FFF0000,
	.emif_ecc_address_range_2	= 0x00000000,
};

/* MT41K512M16HA-125 memory timings */
const struct emif_regs am57xx_emif_532mhz_512m16_regs = {
	.sdram_config_init	= 0x61851bb2,
	.sdram_config		= 0x61851bb2,
	.sdram_config2		= 0x08000000,
	.ref_ctrl		= 0x000040f1,
	.ref_ctrl_final		= 0x00001035,
	.sdram_tim1		= 0xeeef265b,
	.sdram_tim2		= 0x308f7fda,
	.sdram_tim3		= 0x409f8ba8,
	.read_idle_ctrl		= 0x00090000,
	.zq_config		= 0x5007190b,
	.temp_alert_config	= 0x00000000,
	.emif_ddr_phy_ctlr_1_init = 0x0024400b,
	.emif_ddr_phy_ctlr_1	= 0x0e24400b,
	.emif_ddr_ext_phy_ctrl_1 = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2 = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3 = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4 = 0x009b009b,
	.emif_ddr_ext_phy_ctrl_5 = 0x009e009e,
	.emif_rd_wr_lvl_rmp_win	= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl	= 0x80000000,
	.emif_rd_wr_lvl_ctl	= 0x00000000,
	.emif_rd_wr_exec_thresh	= 0x00000305
};

/*
 * MT41K512M16VRN-107 (with ECC enabled) memory timings
 * This is a twin die of MT41K512M8 (also used for the ECC), so see that
 * datasheet for most timings.
 */
const struct emif_regs am57xx_emif_532mhz_512m16_ecc_regs = {
	.sdram_config_init		= 0x61851bb2,
	.sdram_config			= 0x61851bb2,
	.sdram_config2			= 0x08000000,
	.ref_ctrl			= 0x000040f1,
	.ref_ctrl_final			= 0x00001035,
	.sdram_tim1			= 0xeeef265b,
	.sdram_tim2			= 0x308f7fda,
	.sdram_tim3			= 0x409f88a8,
	.read_idle_ctrl			= 0x00090000,
	.zq_config			= 0x5007190b,
	.temp_alert_config		= 0x00000000,
	.emif_ddr_phy_ctlr_1_init	= 0x0024400b,
	.emif_ddr_phy_ctlr_1		= 0x0e24400b,
	.emif_ddr_ext_phy_ctrl_1	= 0x10040100,
	.emif_ddr_ext_phy_ctrl_2	= 0x00910091,
	.emif_ddr_ext_phy_ctrl_3	= 0x00950095,
	.emif_ddr_ext_phy_ctrl_4	= 0x009b009b,
	.emif_ddr_ext_phy_ctrl_5	= 0x009e009e,
	.emif_rd_wr_lvl_rmp_win		= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl		= 0x80000000,
	.emif_rd_wr_lvl_ctl		= 0x00000000,
	.emif_rd_wr_exec_thresh		= 0x00000305,
	.emif_ecc_ctrl_reg		= 0xd0000001,
	// This needs to be not quite 2G due to LISA3 carve out
	.emif_ecc_address_range_1	= 0x7EFF0000,
	.emif_ecc_address_range_2	= 0x00000000,
};

/* Ext phy ctrl regs 1-35 */
const u32 am57xx_emif_ext_phy_ctrl_const_regs[] = {
	0x10040100,
	0x00910091,
	0x00950095,
	0x009b009b,
	0x009e009e,
	0x00980098,
	0x00340034,
	0x00350035,
	0x00340034,
	0x00310031,
	0x00340034,
	0x007f007f,
	0x007f007f,
	0x007f007f,
	0x007f007f,
	0x007f007f,
	0x00480048,
	0x004a004a,
	0x00520052,
	0x00550055,
	0x00500050,
	0x00000000,
	0x00600020,
	0x40011080,
	0x08102040,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0
};

#endif /* _EMIF_CONFIG_PHYCORE_AM57X_ */
