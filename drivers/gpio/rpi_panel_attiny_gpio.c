// SPDX-License-Identifier: GPL-2.0+
/*
 * U-Boot driver for the Raspberry Pi 7" touchscreen panel ATtiny MCU
 * (I2C address 0x45). Powers on the panel and exports GPIO control for
 * the bridge and touchpad reset lines.
 *
 * Based on Linux Kernel drivers/regulator/rpi-panel-attiny-regulator.c
 * Ported to U-Boot by Rahul Sharma <r-sharma3@ti.com>
 * Copyright (C) 2020 Marek Vasut <marex@denx.de>
 * Copyright (C) 2026 Texas Instruments Incorporated
 */

#include <dm.h>
#include <dm/device_compat.h>
#include <i2c.h>
#include <log.h>
#include <asm/gpio.h>
#include <linux/delay.h>

#define REG_PORTA		0x81
#define REG_PORTB		0x82
#define REG_PORTC		0x83
#define REG_POWERON		0x85
#define REG_PWM			0x86
#define REG_ADDR_L		0x8c
#define REG_ADDR_H		0x8d
#define REG_WRITE_DATA_H	0x90
#define REG_WRITE_DATA_L	0x91

#define PA_LCD_LR		BIT(2)

#define PB_LCD_MAIN		BIT(7)

#define PC_LED_EN		BIT(0)
#define PC_RST_TP_N		BIT(1)
#define PC_RST_LCD_N		BIT(2)
#define PC_RST_BRIDGE_N		BIT(3)

/* GPIO indices */
#define RST_BRIDGE_N		0
#define RST_TP_N		1
#define LED_EN			2
#define NUM_GPIO		3

struct attiny_gpio_mapping {
	unsigned int reg;
	unsigned int mask;
};

static const struct attiny_gpio_mapping mappings[NUM_GPIO] = {
	[RST_BRIDGE_N] = { REG_PORTC, PC_RST_BRIDGE_N | PC_RST_LCD_N },
	[RST_TP_N]     = { REG_PORTC, PC_RST_TP_N },
	[LED_EN]       = { REG_PORTC, PC_LED_EN },
};

struct rpi_attiny_priv {
	u8 port_states[3]; /* PORTA, PORTB, PORTC */
	bool powered;
};

static int rpi_attiny_i2c_write(struct udevice *dev, u8 reg, u8 val)
{
	return dm_i2c_write(dev, reg, &val, 1);
}

static int rpi_attiny_set_port_state(struct udevice *dev, u8 reg, u8 val)
{
	struct rpi_attiny_priv *priv = dev_get_priv(dev);

	priv->port_states[reg - REG_PORTA] = val;
	return rpi_attiny_i2c_write(dev, reg, val);
}

static u8 rpi_attiny_get_port_state(struct udevice *dev, u8 reg)
{
	struct rpi_attiny_priv *priv = dev_get_priv(dev);

	return priv->port_states[reg - REG_PORTA];
}

static int rpi_attiny_power_on(struct udevice *dev)
{
	struct rpi_attiny_priv *priv = dev_get_priv(dev);
	int ret, i;

	for (i = 0; i < 10; i++) {
		ret = rpi_attiny_i2c_write(dev, REG_POWERON, 0);
		if (!ret)
			break;
		mdelay(20);
	}
	if (ret) {
		dev_err(dev, "ATtiny not responding after retries: %d\n", ret);
		return ret;
	}
	mdelay(30);
	rpi_attiny_i2c_write(dev, REG_PWM, 255);

	/* Ensure bridge and TP stay in reset */
	ret = rpi_attiny_set_port_state(dev, REG_PORTC, 0);
	if (ret)
		return ret;
	mdelay(5);

	/* Set LCD orientation (same as closed-source firmware) */
	ret = rpi_attiny_set_port_state(dev, REG_PORTA, PA_LCD_LR);
	if (ret)
		return ret;
	mdelay(5);

	/* Main regulator on, power to panel */
	ret = rpi_attiny_set_port_state(dev, REG_PORTB, PB_LCD_MAIN);
	if (ret)
		return ret;
	mdelay(5);

	priv->powered = true;
	dev_info(dev, "panel powered on\n");
	return 0;
}

static int rpi_attiny_ensure_powered(struct udevice *dev)
{
	struct rpi_attiny_priv *priv = dev_get_priv(dev);

	if (priv->powered)
		return 0;
	return rpi_attiny_power_on(dev);
}

static int rpi_attiny_gpio_set_value(struct udevice *dev, unsigned int offset,
				     int value)
{
	u8 last_val;
	int ret;

	ret = rpi_attiny_ensure_powered(dev);
	if (ret)
		return ret;

	last_val = rpi_attiny_get_port_state(dev, mappings[offset].reg);
	if (value)
		last_val |= mappings[offset].mask;
	else
		last_val &= ~mappings[offset].mask;

	ret = rpi_attiny_set_port_state(dev, mappings[offset].reg, last_val);
	if (ret)
		return ret;

	if (offset == RST_BRIDGE_N && value) {
		mdelay(5);
		rpi_attiny_i2c_write(dev, REG_ADDR_H, 0x04);
		mdelay(5);
		rpi_attiny_i2c_write(dev, REG_ADDR_L, 0x7c);
		mdelay(5);
		rpi_attiny_i2c_write(dev, REG_WRITE_DATA_H, 0x00);
		mdelay(5);
		rpi_attiny_i2c_write(dev, REG_WRITE_DATA_L, 0x00);
		mdelay(100);
	}

	return 0;
}

static int rpi_attiny_gpio_direction_output(struct udevice *dev,
					    unsigned int offset, int value)
{
	return rpi_attiny_gpio_set_value(dev, offset, value);
}

static int rpi_attiny_gpio_get_value(struct udevice *dev, unsigned int offset)
{
	u8 last_val = rpi_attiny_get_port_state(dev, mappings[offset].reg);

	return !!(last_val & mappings[offset].mask);
}

static int rpi_attiny_gpio_get_function(struct udevice *dev, unsigned int offset)
{
	return GPIOF_OUTPUT;
}

static const struct dm_gpio_ops rpi_attiny_gpio_ops = {
	.direction_output	= rpi_attiny_gpio_direction_output,
	.get_value		= rpi_attiny_gpio_get_value,
	.set_value		= rpi_attiny_gpio_set_value,
	.get_function		= rpi_attiny_gpio_get_function,
};

static int rpi_attiny_probe(struct udevice *dev)
{
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);

	uc_priv->gpio_count = NUM_GPIO;
	uc_priv->bank_name = "attiny";

	return 0;
}

static const struct udevice_id rpi_attiny_ids[] = {
	{ .compatible = "raspberrypi,7inch-touchscreen-panel-regulator" },
	{ }
};

U_BOOT_DRIVER(rpi_attiny_gpio) = {
	.name		= "rpi_attiny_gpio",
	.id		= UCLASS_GPIO,
	.of_match	= rpi_attiny_ids,
	.ops		= &rpi_attiny_gpio_ops,
	.probe		= rpi_attiny_probe,
	.priv_auto	= sizeof(struct rpi_attiny_priv),
};
