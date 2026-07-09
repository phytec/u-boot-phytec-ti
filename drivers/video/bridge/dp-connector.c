// SPDX-License-Identifier: GPL-2.0+
/*
 * DisplayPort connector stub driver for U-Boot.
 *
 * Ported from Linux kernel drivers/gpu/drm/bridge/display-connector.c
 * by Rahul Sharma <r-sharma3@ti.com>
 * Copyright (C) 2019 Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 */

#include <asm/gpio.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <log.h>
#include <video_bridge.h>
#include <power/regulator.h>

struct dp_connector_priv {
	struct udevice *dp_pwr;
	struct gpio_desc hpd_gpio;
};

static int dp_connector_probe(struct udevice *dev)
{
	struct dp_connector_priv *priv = dev_get_priv(dev);
	int ret;

	ret = device_get_supply_regulator(dev, "dp-pwr-supply", &priv->dp_pwr);
	if (!ret && priv->dp_pwr) {
		ret = regulator_set_enable(priv->dp_pwr, true);
		if (ret)
			dev_warn(dev, "failed to enable dp-pwr: %d\n", ret);
	}

	gpio_request_by_name(dev, "hpd-gpios", 0, &priv->hpd_gpio, GPIOD_IS_IN);

	dev_dbg(dev, "dp-connector probed\n");
	return 0;
}

static const struct video_bridge_ops dp_connector_ops = {
};

static const struct udevice_id dp_connector_ids[] = {
	{ .compatible = "dp-connector" },
	{ }
};

U_BOOT_DRIVER(dp_connector) = {
	.name		= "dp_connector",
	.id		= UCLASS_VIDEO_BRIDGE,
	.of_match	= dp_connector_ids,
	.probe		= dp_connector_probe,
	.ops		= &dp_connector_ops,
	.priv_auto	= sizeof(struct dp_connector_priv),
};
