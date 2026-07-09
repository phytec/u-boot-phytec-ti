// SPDX-License-Identifier: GPL-2.0+
/*
 * Based on Linux drivers/phy/cadence/cdns-dphy.c
 *
 * Copyright: 2017-2018 Cadence Design Systems, Inc.
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 */

#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <generic-phy.h>
#include <phy-mipi-dphy.h>
#include <asm/io.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/math64.h>

#define REG_WAKEUP_TIME_NS		800
#define DPHY_PLL_RATE_HZ		108000000
#define POLL_TIMEOUT_US			100000

/* DPHY registers */
#define DPHY_PMA_CMN(reg)		(reg)
#define DPHY_PMA_LCLK(reg)		(0x100 + (reg))
#define DPHY_PMA_LDATA(lane, reg)	(0x200 + ((lane) * 0x100) + (reg))
#define DPHY_PMA_RCLK(reg)		(0x600 + (reg))
#define DPHY_PMA_RDATA(lane, reg)	(0x700 + ((lane) * 0x100) + (reg))
#define DPHY_PCS(reg)			(0xb00 + (reg))

#define DPHY_CMN_SSM			DPHY_PMA_CMN(0x20)
#define DPHY_CMN_SSM_EN			BIT(0)
#define DPHY_CMN_SSM_CAL_WAIT_TIME	GENMASK(8, 1)
#define DPHY_CMN_TX_MODE_EN		BIT(9)

#define DPHY_CMN_PWM			DPHY_PMA_CMN(0x40)
#define DPHY_CMN_PWM_DIV(x)		((x) << 20)
#define DPHY_CMN_PWM_LOW(x)		((x) << 10)
#define DPHY_CMN_PWM_HIGH(x)		(x)

#define DPHY_CMN_FBDIV			DPHY_PMA_CMN(0x4c)
#define DPHY_CMN_FBDIV_VAL(low, high)	(((high) << 11) | ((low) << 22))
#define DPHY_CMN_FBDIV_FROM_REG		(BIT(10) | BIT(21))

#define DPHY_CMN_OPIPDIV		DPHY_PMA_CMN(0x50)
#define DPHY_CMN_IPDIV_FROM_REG		BIT(0)
#define DPHY_CMN_IPDIV(x)		((x) << 1)
#define DPHY_CMN_OPDIV_FROM_REG		BIT(6)
#define DPHY_CMN_OPDIV(x)		((x) << 7)

#define DPHY_BAND_CFG			DPHY_PCS(0x0)
#define DPHY_BAND_CFG_LEFT_BAND		GENMASK(4, 0)
#define DPHY_BAND_CFG_RIGHT_BAND	GENMASK(9, 5)

#define DPHY_PSM_CFG			DPHY_PCS(0x4)
#define DPHY_PSM_CFG_FROM_REG		BIT(0)
#define DPHY_PSM_CLK_DIV(x)		((x) << 1)

#define DPHY_TX_J721E_WIZ_PLL_CTRL	0xF04
#define DPHY_TX_J721E_WIZ_STATUS	0xF08
#define DPHY_TX_J721E_WIZ_RST_CTRL	0xF0C
#define DPHY_TX_J721E_WIZ_PSM_FREQ	0xF10

#define DPHY_TX_J721E_WIZ_IPDIV		GENMASK(4, 0)
#define DPHY_TX_J721E_WIZ_OPDIV		GENMASK(13, 8)
#define DPHY_TX_J721E_WIZ_FBDIV		GENMASK(25, 16)
#define DPHY_TX_J721E_WIZ_LANE_RSTB	BIT(31)
#define DPHY_TX_WIZ_PLL_LOCK		BIT(31)
#define DPHY_TX_WIZ_O_CMN_READY		BIT(31)

struct cdns_dphy_cfg {
	u8 pll_ipdiv;
	u8 pll_opdiv;
	u16 pll_fbdiv;
	u32 hs_clk_rate;
	unsigned int nlanes;
};

enum cdns_dphy_clk_lane_cfg {
	DPHY_CLK_CFG_LEFT_DRIVES_ALL = 0,
	DPHY_CLK_CFG_LEFT_DRIVES_RIGHT = 1,
	DPHY_CLK_CFG_LEFT_DRIVES_LEFT = 2,
	DPHY_CLK_CFG_RIGHT_DRIVES_ALL = 3,
};

struct cdns_dphy;
struct cdns_dphy_ops {
	int (*probe)(struct cdns_dphy *dphy);
	void (*remove)(struct cdns_dphy *dphy);
	void (*set_psm_div)(struct cdns_dphy *dphy, u8 div);
	void (*set_clk_lane_cfg)(struct cdns_dphy *dphy,
				 enum cdns_dphy_clk_lane_cfg cfg);
	void (*set_pll_cfg)(struct cdns_dphy *dphy,
			    const struct cdns_dphy_cfg *cfg);
	unsigned long (*get_wakeup_time_ns)(struct cdns_dphy *dphy);
	int (*wait_for_pll_lock)(struct cdns_dphy *dphy);
	int (*wait_for_cmn_ready)(struct cdns_dphy *dphy);
};

struct cdns_dphy {
	struct cdns_dphy_cfg cfg;
	void __iomem *regs;
	struct clk psm_clk;
	struct clk pll_ref_clk;
	const struct cdns_dphy_ops *ops;
	bool is_configured;
	bool is_powered;
};

/* Order of bands is important since the index is the band number. */
static const unsigned int tx_bands[] = {
	80, 100, 120, 160, 200, 240, 320, 390, 450, 510, 560, 640, 690, 770,
	870, 950, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2500
};

static int cdns_dphy_get_pll_cfg(struct cdns_dphy *dphy,
				 struct cdns_dphy_cfg *cfg,
				 struct phy_configure_opts_mipi_dphy *opts)
{
	unsigned long pll_ref_hz = clk_get_rate(&dphy->pll_ref_clk);
	u64 dlane_bps;

	memset(cfg, 0, sizeof(*cfg));

	if (pll_ref_hz < 9600000 || pll_ref_hz >= 150000000) {
		pr_err("%s: pll_ref_hz %lu out of range [9.6MHz, 150MHz)\n",
		       __func__, pll_ref_hz);
		return -EINVAL;
	} else if (pll_ref_hz < 19200000) {
		cfg->pll_ipdiv = 1;
	} else if (pll_ref_hz < 38400000) {
		cfg->pll_ipdiv = 2;
	} else if (pll_ref_hz < 76800000) {
		cfg->pll_ipdiv = 4;
	} else {
		cfg->pll_ipdiv = 8;
	}

	dlane_bps = opts->hs_clk_rate;

	if (dlane_bps > 2500000000UL || dlane_bps < 80000000UL) {
		pr_err("%s: dlane_bps %llu out of range [80Mbps, 2500Mbps]\n",
		       __func__, dlane_bps);
		return -EINVAL;
	} else if (dlane_bps >= 1250000000) {
		cfg->pll_opdiv = 1;
	} else if (dlane_bps >= 630000000) {
		cfg->pll_opdiv = 2;
	} else if (dlane_bps >= 320000000) {
		cfg->pll_opdiv = 4;
	} else if (dlane_bps >= 160000000) {
		cfg->pll_opdiv = 8;
	} else if (dlane_bps >= 80000000) {
		cfg->pll_opdiv = 16;
	}

	cfg->pll_fbdiv = DIV_ROUND_UP_ULL(dlane_bps * 2 * cfg->pll_opdiv *
					  cfg->pll_ipdiv,
					  pll_ref_hz);

	cfg->hs_clk_rate = div_u64((u64)pll_ref_hz * cfg->pll_fbdiv,
				   2 * cfg->pll_opdiv * cfg->pll_ipdiv);

	return 0;
}

static int cdns_dphy_setup_psm(struct cdns_dphy *dphy)
{
	unsigned long psm_clk_hz = clk_get_rate(&dphy->psm_clk);
	unsigned long psm_div;

	if (!psm_clk_hz || psm_clk_hz > 100000000) {
		pr_err("%s: psm_clk_hz %lu invalid (must be 1..100MHz)\n",
		       __func__, psm_clk_hz);
		return -EINVAL;
	}

	psm_div = DIV_ROUND_CLOSEST(psm_clk_hz, 1000000);
	if (dphy->ops->set_psm_div)
		dphy->ops->set_psm_div(dphy, psm_div);

	return 0;
}

static void cdns_dphy_set_clk_lane_cfg(struct cdns_dphy *dphy,
				       enum cdns_dphy_clk_lane_cfg cfg)
{
	if (dphy->ops->set_clk_lane_cfg)
		dphy->ops->set_clk_lane_cfg(dphy, cfg);
}

static void cdns_dphy_set_pll_cfg(struct cdns_dphy *dphy,
				  const struct cdns_dphy_cfg *cfg)
{
	if (dphy->ops->set_pll_cfg)
		dphy->ops->set_pll_cfg(dphy, cfg);
}

static unsigned long cdns_dphy_get_wakeup_time_ns(struct cdns_dphy *dphy)
{
	return dphy->ops->get_wakeup_time_ns(dphy);
}

static int cdns_dphy_wait_for_pll_lock(struct cdns_dphy *dphy)
{
	return dphy->ops->wait_for_pll_lock ? dphy->ops->wait_for_pll_lock(dphy) : 0;
}

static int cdns_dphy_wait_for_cmn_ready(struct cdns_dphy *dphy)
{
	return dphy->ops->wait_for_cmn_ready ? dphy->ops->wait_for_cmn_ready(dphy) : 0;
}

static unsigned long cdns_dphy_ref_get_wakeup_time_ns(struct cdns_dphy *dphy)
{
	/* Default wakeup time is 800 ns (in a simulated environment). */
	return 800;
}

static void cdns_dphy_ref_set_pll_cfg(struct cdns_dphy *dphy,
				      const struct cdns_dphy_cfg *cfg)
{
	u32 fbdiv_low, fbdiv_high;

	fbdiv_low = (cfg->pll_fbdiv / 4) - 2;
	fbdiv_high = cfg->pll_fbdiv - fbdiv_low - 2;

	writel(DPHY_CMN_IPDIV_FROM_REG | DPHY_CMN_OPDIV_FROM_REG |
	       DPHY_CMN_IPDIV(cfg->pll_ipdiv) |
	       DPHY_CMN_OPDIV(cfg->pll_opdiv),
	       dphy->regs + DPHY_CMN_OPIPDIV);
	writel(DPHY_CMN_FBDIV_FROM_REG |
	       DPHY_CMN_FBDIV_VAL(fbdiv_low, fbdiv_high),
	       dphy->regs + DPHY_CMN_FBDIV);
	writel(DPHY_CMN_PWM_HIGH(6) | DPHY_CMN_PWM_LOW(0x101) |
	       DPHY_CMN_PWM_DIV(0x8),
	       dphy->regs + DPHY_CMN_PWM);
}

static void cdns_dphy_ref_set_psm_div(struct cdns_dphy *dphy, u8 div)
{
	writel(DPHY_PSM_CFG_FROM_REG | DPHY_PSM_CLK_DIV(div),
	       dphy->regs + DPHY_PSM_CFG);
}

static unsigned long cdns_dphy_j721e_get_wakeup_time_ns(struct cdns_dphy *dphy)
{
	/* Minimum wakeup time as per MIPI D-PHY spec v1.2 */
	return 1000000;
}

static void cdns_dphy_j721e_set_pll_cfg(struct cdns_dphy *dphy,
					const struct cdns_dphy_cfg *cfg)
{
	writel(DPHY_CMN_PWM_HIGH(6) | DPHY_CMN_PWM_LOW(0x101) |
	       DPHY_CMN_PWM_DIV(0x8),
	       dphy->regs + DPHY_CMN_PWM);

	writel((FIELD_PREP(DPHY_TX_J721E_WIZ_IPDIV, cfg->pll_ipdiv) |
		FIELD_PREP(DPHY_TX_J721E_WIZ_OPDIV, cfg->pll_opdiv) |
		FIELD_PREP(DPHY_TX_J721E_WIZ_FBDIV, cfg->pll_fbdiv)),
		dphy->regs + DPHY_TX_J721E_WIZ_PLL_CTRL);

	udelay(10);
	writel(DPHY_TX_J721E_WIZ_LANE_RSTB,
	       dphy->regs + DPHY_TX_J721E_WIZ_RST_CTRL);
}

static void cdns_dphy_j721e_set_psm_div(struct cdns_dphy *dphy, u8 div)
{
	writel(div, dphy->regs + DPHY_TX_J721E_WIZ_PSM_FREQ);
}

static int cdns_dphy_j721e_wait_for_pll_lock(struct cdns_dphy *dphy)
{
	u32 status;
	int ret;

	ret = readl_poll_timeout(dphy->regs + DPHY_TX_J721E_WIZ_PLL_CTRL, status,
				 status & DPHY_TX_WIZ_PLL_LOCK, POLL_TIMEOUT_US);
	return ret;
}

static int cdns_dphy_j721e_wait_for_cmn_ready(struct cdns_dphy *dphy)
{
	u32 status;
	int ret;

	ret = readl_poll_timeout(dphy->regs + DPHY_TX_J721E_WIZ_STATUS, status,
				 status & DPHY_TX_WIZ_O_CMN_READY,
				 POLL_TIMEOUT_US);
	return ret;
}

/*
 * This is the reference implementation of DPHY hooks. Specific integration of
 * this IP may have to re-implement some of them depending on how they decided
 * to wire things in the SoC.
 */
static const struct cdns_dphy_ops ref_dphy_ops = {
	.get_wakeup_time_ns = cdns_dphy_ref_get_wakeup_time_ns,
	.set_pll_cfg = cdns_dphy_ref_set_pll_cfg,
	.set_psm_div = cdns_dphy_ref_set_psm_div,
};

static const struct cdns_dphy_ops j721e_dphy_ops = {
	.get_wakeup_time_ns = cdns_dphy_j721e_get_wakeup_time_ns,
	.set_pll_cfg = cdns_dphy_j721e_set_pll_cfg,
	.set_psm_div = cdns_dphy_j721e_set_psm_div,
	.wait_for_pll_lock = cdns_dphy_j721e_wait_for_pll_lock,
	.wait_for_cmn_ready = cdns_dphy_j721e_wait_for_cmn_ready,
};

static int cdns_dphy_config_from_opts(struct phy *phy,
				      struct phy_configure_opts_mipi_dphy *opts,
				      struct cdns_dphy_cfg *cfg)
{
	struct cdns_dphy *dphy = dev_get_priv(phy->dev);
	int ret;

	ret = phy_mipi_dphy_config_validate(opts);
	if (ret) {
		pr_err("%s: config validate failed: %d\n", __func__, ret);
		return ret;
	}

	ret = cdns_dphy_get_pll_cfg(dphy, cfg, opts);
	if (ret) {
		pr_err("%s: get_pll_cfg failed: %d\n", __func__, ret);
		return ret;
	}

	opts->hs_clk_rate = cfg->hs_clk_rate;
	opts->wakeup = cdns_dphy_get_wakeup_time_ns(dphy) / 1000;

	return 0;
}

static int cdns_dphy_tx_get_band_ctrl(unsigned long hs_clk_rate)
{
	unsigned int rate;
	int i;

	rate = hs_clk_rate / 1000000UL;

	if (rate < tx_bands[0])
		return -EOPNOTSUPP;

	for (i = 0; i < ARRAY_SIZE(tx_bands) - 1; i++) {
		if (rate >= tx_bands[i] && rate < tx_bands[i + 1])
			return i;
	}

	return -EOPNOTSUPP;
}

static int cdns_dphy_validate(struct phy *phy, enum phy_mode mode, int submode,
			      void *params)
{
	struct cdns_dphy_cfg cfg = { 0 };

	if (mode != PHY_MODE_MIPI_DPHY)
		return -EINVAL;

	return cdns_dphy_config_from_opts(phy, params, &cfg);
}

static int cdns_dphy_configure(struct phy *phy, void *params)
{
	struct cdns_dphy *dphy = dev_get_priv(phy->dev);
	struct phy_configure_opts_mipi_dphy *opts = params;
	int ret;

	ret = cdns_dphy_config_from_opts(phy, opts, &dphy->cfg);
	if (!ret)
		dphy->is_configured = true;
	else
		dev_err(phy->dev, "%s: configure failed: %d\n", __func__, ret);

	return ret;
}

static int cdns_dphy_power_on(struct phy *phy)
{
	struct cdns_dphy *dphy = dev_get_priv(phy->dev);
	int ret;
	u32 reg;

	if (!dphy->is_configured || dphy->is_powered) {
		dev_err(phy->dev, "%s: bad state (configured=%d powered=%d)\n",
			__func__, dphy->is_configured, dphy->is_powered);
		return -EINVAL;
	}

	/*
	 * Configure the internal PSM clk divider so that the DPHY has a
	 * 1MHz clk (or something close).
	 */
	clk_prepare_enable(&dphy->psm_clk);
	clk_prepare_enable(&dphy->pll_ref_clk);
	ret = cdns_dphy_setup_psm(dphy);
	if (ret) {
		dev_err(phy->dev, "Failed to setup PSM with error %d\n", ret);
		goto err_power_on;
	}

	/*
	 * Configure attach clk lanes to data lanes: the DPHY has 2 clk lanes
	 * and 8 data lanes, each clk lane can be attache different set of
	 * data lanes. The 2 groups are named 'left' and 'right', so here we
	 * just say that we want the 'left' clk lane to drive the 'left' data
	 * lanes.
	 */
	cdns_dphy_set_clk_lane_cfg(dphy, DPHY_CLK_CFG_LEFT_DRIVES_LEFT);
	cdns_dphy_set_pll_cfg(dphy, &dphy->cfg);
	ret = cdns_dphy_tx_get_band_ctrl(dphy->cfg.hs_clk_rate);
	if (ret < 0) {
		dev_err(phy->dev, "Failed to get band control value with error %d\n", ret);
		goto err_power_on;
	}
	reg = FIELD_PREP(DPHY_BAND_CFG_LEFT_BAND, ret) |
	      FIELD_PREP(DPHY_BAND_CFG_RIGHT_BAND, ret);
	writel(reg, dphy->regs + DPHY_BAND_CFG);

	/* Start TX state machine. */
	reg = readl(dphy->regs + DPHY_CMN_SSM);
	writel((reg & DPHY_CMN_SSM_CAL_WAIT_TIME) | DPHY_CMN_SSM_EN | DPHY_CMN_TX_MODE_EN,
	       dphy->regs + DPHY_CMN_SSM);
	ret = cdns_dphy_wait_for_pll_lock(dphy);
	if (ret) {
		dev_err(phy->dev, "%s: PLL lock failed: %d\n", __func__, ret);
		goto err_power_on;
	}

	ret = cdns_dphy_wait_for_cmn_ready(dphy);
	if (ret) {
		dev_err(phy->dev, "%s: O_CMN_READY timeout: %d\n", __func__, ret);
		goto err_power_on;
	}

	dphy->is_powered = true;

	return 0;

err_power_on:
	clk_disable_unprepare(&dphy->pll_ref_clk);
	clk_disable_unprepare(&dphy->psm_clk);

	return ret;
}

static int cdns_dphy_power_off(struct phy *phy)
{
	struct cdns_dphy *dphy = dev_get_priv(phy->dev);
	u32 reg;

	clk_disable_unprepare(&dphy->pll_ref_clk);
	clk_disable_unprepare(&dphy->psm_clk);

	/* Stop TX state machine. */
	reg = readl(dphy->regs + DPHY_CMN_SSM);
	writel(reg & ~DPHY_CMN_SSM_EN, dphy->regs + DPHY_CMN_SSM);

	dphy->is_powered = false;

	return 0;
}

static const struct phy_ops cdns_dphy_ops = {
	.validate	= cdns_dphy_validate,
	.configure	= cdns_dphy_configure,
	.power_on	= cdns_dphy_power_on,
	.power_off	= cdns_dphy_power_off,
};

static int cdns_dphy_probe(struct udevice *dev)
{
	struct cdns_dphy *dphy = dev_get_priv(dev);
	int ret;

	dphy->ops = (const struct cdns_dphy_ops *)dev_get_driver_data(dev);
	if (!dphy->ops) {
		dev_err(dev, "%s: no ops (missing driver data)\n", __func__);
		return -EINVAL;
	}

	dphy->regs = dev_read_addr_ptr(dev);
	if (!dphy->regs) {
		dev_err(dev, "%s: failed to get base address\n", __func__);
		return -EINVAL;
	}

	ret = clk_get_by_name(dev, "psm", &dphy->psm_clk);
	if (ret) {
		dev_err(dev, "%s: failed to get psm clock: %d\n", __func__, ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "pll_ref", &dphy->pll_ref_clk);
	if (ret) {
		dev_err(dev, "%s: failed to get pll_ref clock: %d\n", __func__, ret);
		return ret;
	}

	if (dphy->ops->probe) {
		ret = dphy->ops->probe(dphy);
		if (ret)
			return ret;
	}

	return 0;
}

static int cdns_dphy_remove(struct udevice *dev)
{
	struct cdns_dphy *dphy = dev_get_priv(dev);

	if (dphy->ops->remove)
		dphy->ops->remove(dphy);

	return 0;
}

static const struct udevice_id cdns_dphy_of_match[] = {
	{ .compatible = "cdns,dphy", .data = (ulong)&ref_dphy_ops },
	{ .compatible = "ti,j721e-dphy", .data = (ulong)&j721e_dphy_ops },
	{ /* sentinel */ },
};

U_BOOT_DRIVER(cdns_dphy) = {
	.name		= "cdns-mipi-dphy",
	.id		= UCLASS_PHY,
	.of_match	= cdns_dphy_of_match,
	.probe		= cdns_dphy_probe,
	.remove		= cdns_dphy_remove,
	.priv_auto	= sizeof(struct cdns_dphy),
	.ops		= &cdns_dphy_ops,
};
