// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015 PHYTEC America, LLC
 * Author: Russell Robinson <rrobinson@phytec.com>
 *
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#include <common.h>
#include <env.h>
#include <env_internal.h>
#include <fastboot.h>
#include <fdt_support.h>
#include <image.h>
#include <init.h>
#include <malloc.h>
#include <net.h>
#include <palmas.h>
#include <sata.h>
#include <serial.h>
#include <usb.h>
#include <errno.h>
#include <asm/omap_common.h>
#include <asm/omap_sec_common.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/clock.h>
#include <asm/arch/dra7xx_iodelay.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sata.h>
#include <asm/arch/gpio.h>
#include <asm/arch/omap.h>
#include <asm/arch-omap5/spl.h>
#include <usb.h>
#include <linux/usb/gadget.h>
#include <dwc3-uboot.h>
#include <dwc3-omap-uboot.h>
#include <ti-usb-phy-uboot.h>
#include <mmc.h>
#include <dm/uclass.h>
#include <hang.h>

#include "emif_config.h"
#include "mux_data.h"

#include "../common/am57_som_detection.h"

#ifdef CONFIG_DRIVER_TI_CPSW
#include <cpsw.h>
#endif

#ifdef CONFIG_MTD_RAW_NAND
#include <nand.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define TPS65903X_PRIMARY_SECONDARY_PAD2	0xFB
#define TPS65903X_PAD2_POWERHOLD_MASK		0x20

#define GPIO_DDR_VTT_EN		GPIO_TO_PIN(4, 8) /* vin2a_d7.gpio4_8 */

#define EEPROM_ADDR		0x50
#define EEPROM_ADDR_FALLBACK	-1

/* Store phytec_eeprom_data struct in SRAM scratch space on AM57x */
#define EEPROM_DATA *(struct phytec_eeprom_data *) \
		(SRAM_SCRATCH_SPACE_ADDR + 0x28)

__weak void gpi2c_init(void) { }

const struct omap_sysinfo sysinfo = {
	"SoM:   PHYTEC phyCORE-AM57x (UNDEFINED)\n"
};

static void ddr3_err(const char *func)
{
	printf("%s: DDR3 option not supported!\n"
		"Please flash your SOM's EEPROM with valid information.\n",
		func);
}

static bool is_am574x(struct phytec_eeprom_data *data)
{
	u8 soc_id = phytec_get_am57_soc(data);

	return (soc_id == 2 || soc_id == 4 || soc_id == 5);
}

void emif_get_dmm_regs(const struct dmm_lisa_map_regs **dmm_lisa_regs)
{
	struct phytec_eeprom_data data = EEPROM_DATA;
	u8 ddr3_opt;
	u8 ecc_opt;

	ddr3_opt = phytec_get_am57_ddr_size(&data);
	ecc_opt = phytec_get_am57_ecc(&data);

	switch (ecc_opt) {
	case 0:
		switch (ddr3_opt) {
		case 0:
			*dmm_lisa_regs = &am57xx_512mx1_lisa_regs;
			break;
		case 1:
			*dmm_lisa_regs = &am57xx_1gx1_lisa_regs;
			break;
		case 3:
			*dmm_lisa_regs = &am57xx_2gx1_lisa_regs;
			break;
		case 4:
		case 5:
			*dmm_lisa_regs = &am57xx_1gx2_lisa_regs;
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	case 1:
		switch (ddr3_opt) {
		case 1:
			*dmm_lisa_regs = &am574x_1gx1_ecc_lisa_regs;
			break;
		case 4:
			if (!is_am574x(&data))
				*dmm_lisa_regs = &am57xx_1gx2_lisa_regs;
			else
				ddr3_err(__func__);
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	case 2:
		switch (ddr3_opt) {
		case 3:
			*dmm_lisa_regs = &am574x_2gx1_ecc_lisa_regs;
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	default:
#if defined(CONFIG_PHYCORE_AM57X_128M16_x2_DDR)
		*dmm_lisa_regs = &am57xx_512mx1_lisa_regs;
#elif defined(CONFIG_PHYCORE_AM57X_256M16_x2_DDR)
		*dmm_lisa_regs = &am57xx_1gx1_lisa_regs;
#elif defined(CONFIG_PHYCORE_AM57X_512M16_x2_DDR)
		*dmm_lisa_regs = &am57xx_2gx1_lisa_regs;
#elif (defined(CONFIG_PHYCORE_AM57X_256M16_x4_DDR) || \
	defined(CONFIG_PHYCORE_AM57X_512M16_x4_DDR))
		*dmm_lisa_regs = &am57xx_1gx2_lisa_regs;
#elif defined(CONFIG_PHYCORE_AM57X_512M16_x2_ECC_DDR)
		*dmm_lisa_regs = &am574x_2gx1_ecc_lisa_regs;
#endif
		break;
	}
}

void emif_get_reg_dump(u32 emif_nr, const struct emif_regs **regs)
{
	struct phytec_eeprom_data data = EEPROM_DATA;
	u8 ddr3_opt;
	u8 ecc_opt;

	ddr3_opt = phytec_get_am57_ddr_size(&data);
	ecc_opt = phytec_get_am57_ecc(&data);

	switch (ecc_opt) {
	case 0:
		switch (ddr3_opt) {
		case 0:
			*regs = &am57xx_emif_532mhz_128m16_regs;
			break;
		case 1:
		case 4:
			*regs = &am57xx_emif_532mhz_256m16_regs;
			break;
		case 3:
		case 5:
			*regs = &am57xx_emif_532mhz_512m16_regs;
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	case 1:
		switch (ddr3_opt) {
		case 1:
			*regs = &am574x_emif_532mhz_256m16_ecc_regs;
			break;
		case 4:
			if (!is_am574x(&data))
				*regs = &am57xx_emif_532mhz_256m16_regs;
			else
				ddr3_err(__func__);
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	case 2:
		switch (ddr3_opt) {
		case 3:
			*regs = &am57xx_emif_532mhz_512m16_ecc_regs;
			break;
		default:
			ddr3_err(__func__);
		}
		break;
	default:
#if defined(CONFIG_PHYCORE_AM57X_128M16_x2_DDR)
		*regs = &am57xx_emif_532mhz_128m16_regs;
#elif (defined(CONFIG_PHYCORE_AM57X_256M16_x2_DDR) || \
	defined(CONFIG_PHYCORE_AM57X_256M16_x4_DDR))
		*regs = &am57xx_emif_532mhz_256m16_regs;
#elif (defined(CONFIG_PHYCORE_AM57X_512M16_x2_DDR) || \
	defined(CONFIG_PHYCORE_AM57X_512M16_x4_DDR))
		*regs = &am57xx_emif_532mhz_512m16_regs;
#elif defined(CONFIG_PHYCORE_AM57X_512M16_x2_ECC_DDR)
		*regs = &am57xx_emif_532mhz_512m16_ecc_regs;
#endif
		break;
	}
}

void emif_get_ext_phy_ctrl_const_regs(u32 emif_nr, const u32 **regs, u32 *size)
{
	*regs = am57xx_emif_ext_phy_ctrl_const_regs;
	*size = ARRAY_SIZE(am57xx_emif_ext_phy_ctrl_const_regs);
}

struct vcores_data am57xx_phycore_volts = {
	.mpu.value[OPP_NOM]	= VDD_MPU_DRA7_NOM,
	.mpu.efuse.reg[OPP_NOM]	= STD_FUSE_OPP_VMIN_MPU_NOM,
	.mpu.efuse.reg_bits	= DRA752_EFUSE_REGBITS,
	.mpu.addr		= TPS659038_REG_ADDR_SMPS12,
	.mpu.pmic		= &tps659038,
	.mpu.abb_tx_done_mask	= OMAP_ABB_MPU_TXDONE_MASK,

	.eve.value[OPP_NOM]	= VDD_EVE_DRA7_NOM,
	.eve.value[OPP_OD]	= VDD_EVE_DRA7_OD,
	.eve.value[OPP_HIGH]	= VDD_EVE_DRA7_HIGH,
	.eve.efuse.reg[OPP_NOM]	= STD_FUSE_OPP_VMIN_DSPEVE_NOM,
	.eve.efuse.reg[OPP_OD]	= STD_FUSE_OPP_VMIN_DSPEVE_OD,
	.eve.efuse.reg[OPP_HIGH]	= STD_FUSE_OPP_VMIN_DSPEVE_HIGH,
	.eve.efuse.reg_bits	= DRA752_EFUSE_REGBITS,
	.eve.addr		= TPS659038_REG_ADDR_SMPS45,
	.eve.pmic		= &tps659038,
	.eve.abb_tx_done_mask	= OMAP_ABB_EVE_TXDONE_MASK,

	.gpu.value[OPP_NOM]	= VDD_GPU_DRA7_NOM,
	.gpu.value[OPP_OD]	= VDD_GPU_DRA7_OD,
	.gpu.value[OPP_HIGH]	= VDD_GPU_DRA7_HIGH,
	.gpu.efuse.reg[OPP_NOM]	= STD_FUSE_OPP_VMIN_GPU_NOM,
	.gpu.efuse.reg[OPP_OD]	= STD_FUSE_OPP_VMIN_GPU_OD,
	.gpu.efuse.reg[OPP_HIGH]	= STD_FUSE_OPP_VMIN_GPU_HIGH,
	.gpu.efuse.reg_bits	= DRA752_EFUSE_REGBITS,
	.gpu.addr		= TPS659038_REG_ADDR_SMPS45,
	.gpu.pmic		= &tps659038,
	.gpu.abb_tx_done_mask	= OMAP_ABB_GPU_TXDONE_MASK,

	.core.value[OPP_NOM]	= VDD_CORE_DRA7_NOM,
	.core.efuse.reg[OPP_NOM]	= STD_FUSE_OPP_VMIN_CORE_NOM,
	.core.efuse.reg_bits	= DRA752_EFUSE_REGBITS,
	.core.addr		= TPS659038_REG_ADDR_SMPS6,
	.core.pmic		= &tps659038,

	.iva.value[OPP_NOM]	= VDD_IVA_DRA7_NOM,
	.iva.value[OPP_OD]	= VDD_IVA_DRA7_OD,
	.iva.value[OPP_HIGH]	= VDD_IVA_DRA7_HIGH,
	.iva.efuse.reg[OPP_NOM]	= STD_FUSE_OPP_VMIN_IVA_NOM,
	.iva.efuse.reg[OPP_OD]	= STD_FUSE_OPP_VMIN_IVA_OD,
	.iva.efuse.reg[OPP_HIGH]	= STD_FUSE_OPP_VMIN_IVA_HIGH,
	.iva.efuse.reg_bits	= DRA752_EFUSE_REGBITS,
	.iva.addr		= TPS659038_REG_ADDR_SMPS45,
	.iva.pmic		= &tps659038,
	.iva.abb_tx_done_mask	= OMAP_ABB_IVA_TXDONE_MASK,
};

int get_voltrail_opp(int rail_offset)
{
	int opp;

	switch (rail_offset) {
	case VOLT_MPU:
		opp = DRA7_MPU_OPP;
		break;
	case VOLT_CORE:
		opp = DRA7_CORE_OPP;
		break;
	case VOLT_GPU:
		opp = DRA7_GPU_OPP;
		break;
	case VOLT_EVE:
		opp = DRA7_DSPEVE_OPP;
		break;
	case VOLT_IVA:
		opp = DRA7_IVA_OPP;
		break;
	default:
		opp = OPP_NOM;
	}

	return opp;
}

void vcores_init(void)
{
	*omap_vcores = &am57xx_phycore_volts;
}

void hw_data_init(void)
{
	*prcm = &dra7xx_prcm;
	*dplls_data = &dra7xx_dplls;
	*ctrl = &dra7xx_ctrl;
}

int board_init(void)
{
	gpmc_init();
	gd->bd->bi_boot_params = (CONFIG_SYS_SDRAM_BASE + 0x100);

	return 0;
}

int dram_init_banksize(void)
{
	struct phytec_eeprom_data data = EEPROM_DATA;
	u8 ddr3_opt;
	u64 ram_size;

	ddr3_opt = phytec_get_am57_ddr_size(&data);

	switch (ddr3_opt) {
	case 0:
		ram_size = 0x20000000;
		break;
	case 1:
		ram_size = 0x40000000;
		break;
	case 3:
	case 4:
		ram_size = 0x80000000;
		break;
	case 5:
		ram_size = 0x100000000;
		break;
	default:
#if defined(CONFIG_PHYCORE_AM57X_128M16_x2_DDR)
		ram_size = 0x20000000;
#elif defined(CONFIG_PHYCORE_AM57X_256M16_x2_DDR)
		ram_size = 0x40000000;
#elif (defined(CONFIG_PHYCORE_AM57X_256M16_x4_DDR) || \
	defined(CONFIG_PHYCORE_AM57X_512M16_x2_DDR) || \
	defined(CONFIG_PHYCORE_AM57X_512M16_x2_ECC_DDR))
		ram_size = 0x80000000;
#elif defined(CONFIG_PHYCORE_AM57X_512M16_x4_DDR)
		ram_size = 0x100000000;
#elif defined(CONFIG_PHYCORE_AM57X_512M16_x2_ECC_DDR)
#endif
		break;
	}

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = get_effective_memsize();
	if (ram_size > CONFIG_MAX_MEM_MAPPED) {
		gd->bd->bi_dram[1].start = 0x200000000;
		gd->bd->bi_dram[1].size = ram_size - CONFIG_MAX_MEM_MAPPED;
	}

	return 0;
}

#ifdef CONFIG_PHYTEC_AM57_SOM_DETECTION
void do_board_detect(void)
{
	int rc;
	struct phytec_eeprom_data data;

	/* Read I2C EEPROM */
	rc = phytec_eeprom_data_setup(&data, 0, EEPROM_ADDR, EEPROM_ADDR_FALLBACK);
	if (rc) {
		printf("%s: I2C read failed or EEPROM information is invalid!\n"
			"Please flash your SOM's EEPROM with valid information.\n",
				__func__);
	} else {
		/* Store I2C EEPROM data in SRAM to avoid multiple I2C reads */
		EEPROM_DATA = data;

		snprintf(sysinfo.board_string, 45,
			"SoM:   PHYTEC phyCORE-AM57x (%s)\n", phytec_get_am57_opt(&data));
	}
}
#else
void do_board_detect(void) { }
#endif

#ifdef CONFIG_SPL_BUILD
static inline void eeprom_set_board_env(void) { }
#else
void eeprom_set_board_env(void)
{
	struct phytec_eeprom_data data = EEPROM_DATA;
	char *soc, *opt;
	u8 soc_id;

	/* Handle SoC variant */
	soc_id = phytec_get_am57_soc(&data);
	if (soc_id == PHYTEC_EEPROM_INVAL)
		goto err;

	switch (soc_id) {
	case 0:
		soc = "am5728";
		break;
	case 1:
		soc = "am5726";
		break;
	case 2:
		soc = "am5749";
		break;
	case 3:
		soc = "am5716";
		break;
	case 4:
		soc = "am5748";
		break;
	case 5:
		soc = "am5746";
		break;
	case 6:
		soc = "am5729";
		break;
	default:
		goto err;
	}

	/* Handle options */
	opt = phytec_get_am57_opt(&data);
	if (!opt)
		goto err;

	/*
	 * Only the AM574x supports ECC. Unset the ECC option in all other
	 * SoC configurations because there're no device-tree available.
	 */
	if (soc_id == 0 || soc_id == 1 || soc_id == 3 || soc_id == 6)
		opt[1] = '0';

	/* Set environment variables */
	env_set("board_soc", soc);
	env_set("board_opt", opt);
	env_set("board_rev", "0");

	return;
err:
	env_set("board_soc", "am57xx");
	env_set("board_opt", "undefined");
	env_set("board_rev", "0");
}
#endif /* CONFIG_SPL_BUILD */

#if CONFIG_IS_ENABLED(DM_USB) && CONFIG_IS_ENABLED(OF_CONTROL)
static int device_okay(const char *path)
{
	int node;

	node = fdt_path_offset(gd->fdt_blob, path);
	if (node < 0)
		return 0;

	return fdtdec_get_is_enabled(gd->fdt_blob, node);
}
#endif

int board_late_init(void)
{
	u8 val;

	if (env_get_yesno("board_override") != 1)
		eeprom_set_board_env();

	/*
	 * DEV_CTRL.DEV_ON = 1 please - else palmas switches off in 8 seconds
	 * This is the POWERHOLD-in-Low behavior.
	 */
	palmas_i2c_write_u8(TPS65903X_CHIP_P1, 0xA0, 0x1);

	/*
	 * Default FIT boot on HS devices. Non FIT images are not allowed
	 * on HS devices.
	 */
	if (get_device_type() == HS_DEVICE)
		env_set("boot_fit", "1");

	/*
	 * Set the GPIO7 Pad to POWERHOLD. This has higher priority
	 * over DEV_CTRL.DEV_ON bit. This can be reset in case of
	 * PMIC Power off. So to be on the safer side set it back
	 * to POWERHOLD mode irrespective of the current state.
	 */
	palmas_i2c_read_u8(TPS65903X_CHIP_P1, TPS65903X_PRIMARY_SECONDARY_PAD2,
			   &val);
	val = val | TPS65903X_PAD2_POWERHOLD_MASK;
	palmas_i2c_write_u8(TPS65903X_CHIP_P1, TPS65903X_PRIMARY_SECONDARY_PAD2,
			    val);

	omap_die_id_serial();
	omap_set_fastboot_vars();

#if CONFIG_IS_ENABLED(DM_USB) && CONFIG_IS_ENABLED(OF_CONTROL)
	if (device_okay("/ocp/omap_dwc3_1@48880000"))
		enable_usb_clocks(0);
	if (device_okay("/ocp/omap_dwc3_2@488c0000"))
		enable_usb_clocks(1);
#endif

	return 0;
}

void set_muxconf_regs(void)
{
	do_set_mux32((*ctrl)->control_padconf_core_base,
			 early_padconf, ARRAY_SIZE(early_padconf));
}

#ifdef CONFIG_IODELAY_RECALIBRATION
void recalibrate_iodelay(void)
{
	const struct iodelay_cfg_entry *iod;
	int iod_sz;
	int ret;

	/* Setup I/O isolation */
	ret = __recalibrate_iodelay_start();
	if (ret)
		goto err;

	/* Do the muxing here */
	do_set_mux32((*ctrl)->control_padconf_core_base,
			core_padconf_array_essential,
			ARRAY_SIZE(core_padconf_array_essential));

	if (omap_revision() == DRA752_ES1_1) {
		iod = iodelay_cfg_array_sr1_1;
		iod_sz = ARRAY_SIZE(iodelay_cfg_array_sr1_1);
	} else {
		iod = iodelay_cfg_array_sr2_0;
		iod_sz = ARRAY_SIZE(iodelay_cfg_array_sr2_0);
	}

	/* Setup IOdelay configuration */
	ret = do_set_iodelay((*ctrl)->iodelay_config_base, iod, iod_sz);

err:
	/* Closeup.. remove isolation */
	__recalibrate_iodelay_end(ret);
}
#endif

#if defined(CONFIG_MMC)
int board_mmc_init(struct bd_info *bis)
{
	/* init SD */
	omap_mmc_init(0, 0, 0, -1, -1);

#if !defined(CONFIG_MTD_RAW_NAND)
	/* init eMMC */
	omap_mmc_init(1, 0, 0, -1, -1);
#endif

	return 0;
}

static const struct mmc_platform_fixups am57x_es1_1_mmc1_fixups = {
	.hw_rev = "rev11",
	.unsupported_caps = MMC_CAP(MMC_HS_200) |
			    MMC_CAP(UHS_SDR104),
	.max_freq = 96000000,
};

static const struct mmc_platform_fixups am57x_es1_1_mmc23_fixups = {
	.hw_rev = "rev11",
	.unsupported_caps = MMC_CAP(MMC_HS_200) |
			    MMC_CAP(UHS_SDR104) |
			    MMC_CAP(UHS_SDR50),
	.max_freq = 48000000,
};

const struct mmc_platform_fixups *platform_fixups_mmc(uint32_t addr)
{
	switch (omap_revision()) {
	case DRA752_ES1_0:
	case DRA752_ES1_1:
		if (addr == OMAP_HSMMC1_BASE)
			return &am57x_es1_1_mmc1_fixups;
		else
			return &am57x_es1_1_mmc23_fixups;
	default:
		return NULL;
	}
}
#endif

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_OS_BOOT)
int spl_start_uboot(void)
{
	/* break into full u-boot on 'c' */
	if (serial_tstc() && serial_getc() == 'c')
		return 1;

#ifdef CONFIG_SPL_ENV_SUPPORT
	env_init();
	env_load();
	if (env_get_yesno("boot_os") != 1)
		return 1;
#endif

	return 0;
}
#endif

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 1,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= 2,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 2,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};

int board_eth_init(struct bd_info *bis)
{
	int ret;
	u8 mac_addr[6];
	u32 mac_hi, mac_lo;
	u32 ctrl_val;

	/* try reading mac address from efuse */
	mac_lo = readl((*ctrl)->control_core_mac_id_0_lo);
	mac_hi = readl((*ctrl)->control_core_mac_id_0_hi);
	mac_addr[0] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = mac_hi & 0xFF;
	mac_addr[3] = (mac_lo & 0xFF0000) >> 16;
	mac_addr[4] = (mac_lo & 0xFF00) >> 8;
	mac_addr[5] = mac_lo & 0xFF;

	if (!env_get("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ethaddr(mac_addr))
			eth_env_set_enetaddr("ethaddr", mac_addr);
	}

	mac_lo = readl((*ctrl)->control_core_mac_id_1_lo);
	mac_hi = readl((*ctrl)->control_core_mac_id_1_hi);
	mac_addr[0] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = mac_hi & 0xFF;
	mac_addr[3] = (mac_lo & 0xFF0000) >> 16;
	mac_addr[4] = (mac_lo & 0xFF00) >> 8;
	mac_addr[5] = mac_lo & 0xFF;

	if (!env_get("eth1addr")) {
		if (is_valid_ethaddr(mac_addr))
			eth_env_set_enetaddr("eth1addr", mac_addr);
	}

	ctrl_val = readl((*ctrl)->control_core_control_io1) & (~0x33);
	ctrl_val |= 0x22;
	writel(ctrl_val, (*ctrl)->control_core_control_io1);

	ret = cpsw_register(&cpsw_data);
	if (ret < 0)
		printf("Error %d registering CPSW switch\n", ret);

	return ret;
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
/* VTT regulator enable */
static inline void vtt_regulator_enable(void)
{
	if (omap_hw_init_context() == OMAP_INIT_CONTEXT_UBOOT_AFTER_SPL)
		return;

	gpio_request(GPIO_DDR_VTT_EN, "ddr_vtt_en");
	gpio_direction_output(GPIO_DDR_VTT_EN, 1);
}

int board_early_init_f(void)
{
	vtt_regulator_enable();
	return 0;
}
#endif

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	ft_cpu_setup(blob, bd);

	return 0;
}
#endif

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	if (!strcmp(name, "am57xx-phytec-pcm-948"))
		return 0;

	return -1;
}
#endif

#if CONFIG_IS_ENABLED(FASTBOOT) && !CONFIG_IS_ENABLED(ENV_IS_NOWHERE)
int fastboot_set_reboot_flag(enum fastboot_reboot_reason reason)
{
	if (reason != FASTBOOT_REBOOT_REASON_BOOTLOADER)
		return -ENOTSUPP;

	printf("Setting reboot to fastboot flag ...\n");
	env_set("dofastboot", "1");
	env_save();
	return 0;
}
#endif

#ifdef CONFIG_TI_SECURE_DEVICE
void board_fit_image_post_process(const void *fit, int node, void **p_image,
				  size_t *p_size)
{
	secure_boot_verify_image(p_image, p_size);
}

void board_tee_image_process(ulong tee_image, size_t tee_size)
{
	secure_tee_install((u32)tee_image);
}

U_BOOT_FIT_LOADABLE_HANDLER(IH_TYPE_TEE, board_tee_image_process);
#endif

enum env_location env_get_location(enum env_operation op, int prio)
{
	u32 boot_device = omap_sys_boot_device();

	if (prio)
		return ENVL_UNKNOWN;

	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_FAT))
			return ENVL_FAT;
	case BOOT_DEVICE_MMC2:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC))
			return ENVL_MMC;
	case BOOT_DEVICE_NAND:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_NAND))
			return ENVL_NAND;
	default:
		return ENVL_NOWHERE;
	}
}
