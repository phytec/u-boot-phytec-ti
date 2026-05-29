// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for J722S platforms
 *
 * Copyright (C) 2024 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <asm/arch/hardware.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <cpu_func.h>
#include <dm.h>
#include <i2c.h>
#include <dm/uclass-internal.h>
#include <dm/root.h>
#include <env.h>
#include <fdt_support.h>
#include <spl.h>
#include <wait_bit.h>
#include <asm/arch/k3-ddr.h>
#include <asm/arch/am62xx-j722s-lpm-hardware.h>
#include "../common/fdt_ops.h"

#if IS_ENABLED(CONFIG_SPL_BUILD)
void spl_board_init(void)
{
	enable_caches();
}
#endif

ofnode cadence_qspi_get_subnode(struct udevice *dev)
{
	if (IS_ENABLED(CONFIG_SPL_BUILD) &&
	    IS_ENABLED(CONFIG_TARGET_J721S2_R5_EVM)) {
		if (spl_boot_device() == BOOT_DEVICE_SPINAND)
			return ofnode_by_compatible(dev_ofnode(dev), "spi-nand");
	}

	return dev_read_first_subnode(dev);
}

/* Enables the spi-nand dts node, if onboard mux is set to spinand */
static void __maybe_unused detect_enable_spinand(void *blob)
{
	if (IS_ENABLED(CONFIG_DM_GPIO) && IS_ENABLED(CONFIG_OF_LIBFDT)) {
		struct gpio_desc desc = {0};
		char *ospi_mux_sel_gpio = "gpio@23_1";
		int nand_offset, nor_offset;

		if (dm_gpio_lookup_name(ospi_mux_sel_gpio, &desc))
			return;

		if (dm_gpio_request(&desc, ospi_mux_sel_gpio))
			return;

		if (dm_gpio_set_dir_flags(&desc, GPIOD_IS_IN))
			return;

		nand_offset = fdt_node_offset_by_compatible(blob, -1, "spi-nand");
		if (nand_offset < 0)
			return;

		nor_offset = fdt_node_offset_by_compatible(blob,
							   fdt_parent_offset(blob, nand_offset),
							   "jedec,spi-nor");

		if (dm_gpio_get_value(&desc)) {
			fdt_status_okay(blob, nand_offset);
			fdt_del_node(blob, nor_offset);
		} else {
			fdt_del_node(blob, nand_offset);
		}
	}
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	detect_enable_spinand(blob);

	return 0;
}
#endif

#if (IS_ENABLED(CONFIG_SPL_BUILD) && IS_ENABLED(CONFIG_TARGET_J722S_R5_EVM))

extern void ctrl_mmr_unlock(void);

#define SCRATCH_PAD_REG_3 0xCB
#define MAGIC_SUSPEND 0xBA
#define LPM_WAKE_SOURCE_PMIC_GPIO 0x91
#define LPM_WAKE_SOURCE_MCU_IO    0x81

static int clear_io_isolation(void)
{
	const void *wait_reg = (const void *)(WKUP_CTRL_MMR0_BASE +
					      WKUP_CTRL_MMR_CANUART_WAKE_STAT1);
	int ret;
	u32 reg = 0;

	/* Program magic word */
	reg = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_CANUART_WAKE_CTRL);
	reg |= WKUP_CTRL_MMR_CANUART_WAKE_CTRL_MW << WKUP_CTRL_MMR_CANUART_WAKE_CTRL_MW_SHIFT;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_CANUART_WAKE_CTRL);

	/* Set enable bit. */
	reg |= WKUP_CTRL_MMR_CANUART_WAKE_CTRL_MW_LOAD_EN;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_CANUART_WAKE_CTRL);

	/* Clear enable bit. */
	reg &= ~WKUP_CTRL_MMR_CANUART_WAKE_CTRL_MW_LOAD_EN;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_CANUART_WAKE_CTRL);

	/* wait for CAN_ONLY_IO signal to be 0 */
	ret = wait_for_bit_32(wait_reg,
			      WKUP_CTRL_MMR_CANUART_WAKE_STAT1_CANUART_IO_MODE,
			      false,
			      CLKSTOP_TRANSITION_TIMEOUT_MS,
			      false);
	if (ret < 0)
		return ret;

	/* Reset magic word */
	writel(0, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_CANUART_WAKE_CTRL);

	/* Remove WKUP IO isolation */
	reg = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_0);
	reg = reg & WKUP_CTRL_MMR_PMCTRL_IO_0_WRITE_MASK & ~WKUP_CTRL_MMR_PMCTRL_IO_0_GLOBAL_WUEN_0;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_0);

	/* clear global IO isolation */
	reg = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_0);
	reg = reg & WKUP_CTRL_MMR_PMCTRL_IO_0_WRITE_MASK & ~WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_CTRL_0;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_0);

	/* Remove main domain IO isolation */
	reg = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_1);
	reg = reg & WKUP_CTRL_MMR_PMCTRL_IO_0_WRITE_MASK & ~WKUP_CTRL_MMR_PMCTRL_IO_0_GLOBAL_WUEN_0;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_1);

	/* clear global IO isolation for main domain IOs */
	reg = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_1);
	reg = reg & WKUP_CTRL_MMR_PMCTRL_IO_0_WRITE_MASK & ~WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_CTRL_0;
	writel(reg, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_1);

	/* Release all IOs from deepsleep mode and clear IO daisy chain control */
	writel(0, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_DEEPSLEEP_CTRL);
	writel(0, WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_GLB);

	return 0;
}

/* in board_init_f(), there's no BSS, so we can't use global/static variables */
bool j7xx_board_is_resuming(void)
{
	struct udevice *pmic, *i2c;
	int err;
	u32 pmctrl0_val = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_0);
	u32 pmctrl1_val = readl(WKUP_CTRL_MMR0_BASE + WKUP_CTRL_MMR_PMCTRL_IO_1);

	if (gd_k3_resuming() != K3_RESUME_STATE_UNKNOWN)
		goto end;

	if (((pmctrl0_val & WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_STATUS_0) ==
	     WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_STATUS_0) ||
	    ((pmctrl1_val & WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_STATUS_0) ==
	     WKUP_CTRL_MMR_PMCTRL_IO_0_IO_ISO_STATUS_0)) {
		debug("%s: board is resuming from IO_DDR mode\n", __func__);
		clear_io_isolation();
		gd_set_k3_resuming(K3_RESUME_STATE_RESUMING);
		goto end;
	}

	/*
	 * On HS-SE devices, i2c access fails unless MMR registers are unlocked.
	 * Moreover, it fails also if we use PMIC API instead of I2C API.
	 */
	ctrl_mmr_unlock();
	err = uclass_get_device_by_name(UCLASS_I2C,
					"i2c@2b200000", &i2c);
	if (err) {
		printf("Getting I2C failed: %d\n", err);
		goto end;
	}
	err = dm_i2c_probe(i2c, 0x48, 0, &pmic);
	if (err) {
		printf("Getting PMIC failed: %d\n", err);
		goto end;
	}

	debug("%s: PMIC is detected (%s)\n", __func__, pmic->name);

	if (dm_i2c_reg_read(pmic, SCRATCH_PAD_REG_3) == MAGIC_SUSPEND) {
		debug("%s: board is resuming from SOC_OFF mode\n", __func__);
		gd_set_k3_resuming(K3_RESUME_STATE_RESUMING);

		/* clean magic suspend */
		if (dm_i2c_reg_write(pmic, SCRATCH_PAD_REG_3, 0))
			printf("Failed to clean magic value for suspend detection in PMIC\n");
	} else {
		debug("%s: board is booting (no resume detected)\n", __func__);
		gd_set_k3_resuming(K3_RESUME_STATE_BOOTING);
	}
end:
	return gd_k3_resuming() == K3_RESUME_STATE_RESUMING;
}
#endif /* CONFIG_SPL_BUILD && CONFIG_TARGET_J722S_R5_EVM */

#if defined(CONFIG_XPL_BUILD)
void spl_perform_board_fixups(struct spl_image_info *spl_image)
{
	if (IS_ENABLED(CONFIG_K3_DDRSS)) {
		if (IS_ENABLED(CONFIG_K3_INLINE_ECC))
			fixup_ddr_driver_for_ecc(spl_image);
	} else {
		fixup_memory_node(spl_image);
	}

	detect_enable_spinand(spl_image->fdt_addr);
}
#endif

#if IS_ENABLED(CONFIG_BOARD_LATE_INIT)
int board_late_init(void)
{
	ti_set_fdt_env(NULL, NULL);
	return 0;
}
#endif
