/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015 PHYTEC America, LLC
 * Author: Russell Robinson <rrobinson@phytec.com>
 *
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#ifndef _MUX_DATA_PHYCORE_AM57X_
#define _MUX_DATA_PHYCORE_AM57X_

#include <asm/arch/mux_dra7xx.h>

const struct pad_conf_entry core_padconf_array_essential[] = {
	/* SD Card Slot (MMC1) */
	{MMC1_CLK, (M0 | PIN_INPUT_PULLUP)},     /* mmc1_clk.clk */
	{MMC1_CMD, (M0 | PIN_INPUT_PULLUP)},     /* mmc1_cmd.cmd */
	{MMC1_DAT0, (M0 | PIN_INPUT_PULLUP)},    /* mmc1_dat0.dat0 */
	{MMC1_DAT1, (M0 | PIN_INPUT_PULLUP)},    /* mmc1_dat1.dat1 */
	{MMC1_DAT2, (M0 | PIN_INPUT_PULLUP)},    /* mmc1_dat2.dat2 */
	{MMC1_DAT3, (M0 | PIN_INPUT_PULLUP)},    /* mmc1_dat3.dat3 */
	{MMC1_SDCD, (M0 | PIN_INPUT_PULLUP)},    /* mmc1_sdcd.sdcd */
	{MMC1_SDWP, (M14 | PIN_INPUT_PULLDOWN)}, /* mmc1_sdwp.gpio6_28 */

	/* eMMC (MMC2) */
	{GPMC_A19, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a19.mmc2_dat4 */
	{GPMC_A20, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a20.mmc2_dat5 */
	{GPMC_A21, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a21.mmc2_dat6 */
	{GPMC_A22, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a22.mmc2_dat7 */
	{GPMC_A23, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a23.mmc2_clk */
	{GPMC_A24, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a24.mmc2_dat0 */
	{GPMC_A25, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a25.mmc2_dat1 */
	{GPMC_A26, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a26.mmc2_dat2 */
	{GPMC_A27, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_a27.mmc2_dat3 */
	{GPMC_CS1, (M1 | PIN_INPUT_PULLUP)}, /* gpmc_cs1.mmc2_cmd */

	/* NAND (GPMC) */
	{GPMC_AD0, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad0 */
	{GPMC_AD1, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad1 */
	{GPMC_AD2, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad2 */
	{GPMC_AD3, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad3 */
	{GPMC_AD4, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad4 */
	{GPMC_AD5, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad5 */
	{GPMC_AD6, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad6 */
	{GPMC_AD7, (M0 | PIN_INPUT_SLEW)},			/* gpmc_ad7 */
	{GPMC_CS0, (M0 | PIN_OUTPUT_PULLUP | SLEWCONTROL)},	/* gpmc_cs0 */
	{GPMC_ADVN_ALE, (M0 | PIN_OUTPUT | SLEWCONTROL)},	/* gpmc_advn_ale */
	{GPMC_OEN_REN, (M0 | PIN_OUTPUT | SLEWCONTROL)},	/* gpmc_oen_ren */
	{GPMC_WEN, (M0 | PIN_OUTPUT | SLEWCONTROL)},		/* gpmc_wen */
	{GPMC_BEN0, (M0 | PIN_OUTPUT | SLEWCONTROL)},		/* gpmc_ben0 */
	{GPMC_WAIT0, (M0 | PIN_INPUT)},			/* gpmc_wait0 */

	/* SDIO (MMC3) */
	{MMC3_CLK, (M0 | PIN_INPUT_PULLUP)},  /* mmc3_clk.clk */
	{MMC3_CMD, (M0 | PIN_INPUT_PULLUP)},  /* mmc3_cmd.cmd */
	{MMC3_DAT0, (M0 | PIN_INPUT_PULLUP)}, /* mmc3_dat0.dat0 */
	{MMC3_DAT1, (M0 | PIN_INPUT_PULLUP)}, /* mmc3_dat1.dat1 */
	{MMC3_DAT2, (M0 | PIN_INPUT_PULLUP)}, /* mmc3_dat2.dat2 */
	{MMC3_DAT3, (M0 | PIN_INPUT_PULLUP)}, /* mmc3_dat3.dat3 */

	/* BT_EN */
	{MMC3_DAT4, (M14 | PIN_OUTPUT_PULLDOWN)}, /* mmc3_dat4.gpio1_22 */
	/* WLAN EN */
	{MMC3_DAT6, (M14 | PIN_OUTPUT_PULLDOWN)}, /* mmc3_dat6.gpio1_24 */
	/* WLAN_IRQ */
	{MMC3_DAT7, (M14 | PIN_INPUT_PULLUP)},    /* mmc3_dat7.gpio1_15 */

	/* MDIO bus */
	{MDIO_MCLK, (M0 | PIN_OUTPUT)}, /* mdio_mclk  */
	{MDIO_D, (M0 | PIN_INPUT)},     /* mdio_d  */

	/* ETH0 (RGMII0) */
	{RGMII0_TXC, (M0 | PIN_OUTPUT | MANUAL_MODE)},   /* rgmii0_txc.rgmii0_txc */
	{RGMII0_TXCTL, (M0 | PIN_OUTPUT | MANUAL_MODE)}, /* rgmii0_txctl.rgmii0_txctl */
	{RGMII0_TXD3, (M0 | PIN_OUTPUT | MANUAL_MODE)},  /* rgmii0_txd3.rgmii0_txd3 */
	{RGMII0_TXD2, (M0 | PIN_OUTPUT | MANUAL_MODE)},  /* rgmii0_txd2.rgmii0_txd2 */
	{RGMII0_TXD1, (M0 | PIN_OUTPUT | MANUAL_MODE)},  /* rgmii0_txd1.rgmii0_txd1 */
	{RGMII0_TXD0, (M0 | PIN_OUTPUT | MANUAL_MODE)},  /* rgmii0_txd0.rgmii0_txd0 */
	{RGMII0_RXC, (M0 | PIN_INPUT | MANUAL_MODE)},    /* rgmii0_rxc.rgmii0_rxc */
	{RGMII0_RXCTL, (M0 | PIN_INPUT | MANUAL_MODE)},  /* rgmii0_rxctl.rgmii0_rxctl */
	{RGMII0_RXD3, (M0 | PIN_INPUT | MANUAL_MODE)},   /* rgmii0_rxd3.rgmii0_rxd3 */
	{RGMII0_RXD2, (M0 | PIN_INPUT | MANUAL_MODE)},   /* rgmii0_rxd2.rgmii0_rxd2 */
	{RGMII0_RXD1, (M0 | PIN_INPUT | MANUAL_MODE)},   /* rgmii0_rxd1.rgmii0_rxd1 */
	{RGMII0_RXD0, (M0 | PIN_INPUT | MANUAL_MODE)},   /* rgmii0_rxd0.rgmii0_rxd0 */
	/* nETH0_RST */
	{RMII_MHZ_50_CLK, (M14 | PIN_INPUT_PULLUP)},     /* rmii_mhz_50_clk.gpio5_17 */
	/* ETH0 IRQ */
	{VIN2A_CLK0, (M14 | PIN_INPUT)},                 /* vin2a_clk0.gpio3_28 */

	/* ETH1 (RGMII1) */
	{VIN2A_D12, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d12.rgmii1_txc */
	{VIN2A_D13, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d13.rgmii1_txctl */
	{VIN2A_D14, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d14.rgmii1_txd3 */
	{VIN2A_D15, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d15.rgmii1_txd2 */
	{VIN2A_D16, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d16.rgmii1_txd1 */
	{VIN2A_D17, (M3 | PIN_OUTPUT | MANUAL_MODE)}, /* vin2a_d17.rgmii1_txd0 */
	{VIN2A_D18, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d18.rgmii1_rxc */
	{VIN2A_D19, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d19.rgmii1_rxctl */
	{VIN2A_D20, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d20.rgmii1_rxd3 */
	{VIN2A_D21, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d21.rgmii1_rxd2 */
	{VIN2A_D22, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d22.rgmii1_rxd1 */
	{VIN2A_D23, (M3 | PIN_INPUT | MANUAL_MODE)},  /* vin2a_d23.rgmii1_rxd0 */
	/* ETH1 IRQ */
	{GPMC_A12, (M14 | PIN_INPUT)},                /* gpmc_a12.gpio2_2 */

	/* USB1 and USB2 DRVVBUS */
	{USB1_DRVVBUS, (M0 | PIN_OUTPUT)},          /* usb1_drvvbus.usb1_drvvbus */
	{USB2_DRVVBUS, (M0 | PIN_OUTPUT_PULLDOWN)}, /* usb2_drvvbus.usb2_drvvbus */

	/* QSPI NOR (QSPI1) */
	{GPMC_CS2, (M1 | PIN_OUTPUT | MANUAL_MODE)}, /* gpmc_cs2.qspi1_cs0 */
	{GPMC_CS3, (M1 | PIN_OUTPUT | MANUAL_MODE)}, /* gpmc_cs3.qspi1_cs1 */
	{GPMC_A3, (M1 | PIN_OUTPUT | MANUAL_MODE)},  /* gpmc_a3.qspi1_cs2 */
	{GPMC_A13, (M1 | PIN_INPUT | MANUAL_MODE)},  /* gpmc_a13.qspi1_rtclk */
	{GPMC_A18, (M1 | PIN_INPUT | MANUAL_MODE)},  /* gpmc_a18.qspi1_sclk */
	{GPMC_A16, (M1 | PIN_INPUT | MANUAL_MODE)},  /* gpmc_a16.qspi1_d0 */
	{GPMC_A17, (M1 | PIN_INPUT | MANUAL_MODE)},  /* gpmc_a17.qspi1_d1 */
	{GPMC_A15, (M1 | PIN_INPUT | MANUAL_MODE)}, /* gpmc_a15.qspi1_d2 */
	{GPMC_A14, (M1 | PIN_INPUT | MANUAL_MODE)}, /* gpmc_a14.qspi1_d3 */

	/* LCD display (vout2) */
	{VIN2A_DE0, (M4 | PIN_OUTPUT)},    /* vin2a_de0.vout2_de */
	{VIN2A_FLD0, (M4 | PIN_OUTPUT)},   /* vin2a_fld0.vout2_clk */
	{VIN2A_HSYNC0, (M4 | PIN_OUTPUT)}, /* vin2a_hsync0.vout2_hsync */
	{VIN2A_VSYNC0, (M4 | PIN_OUTPUT)}, /* vin2a_vsync0.vout2_vsync */
	{MCASP1_ACLKR, (M6 | PIN_OUTPUT)}, /* mcasp1_aclkr.vout2_d0 */
	{MCASP1_FSR, (M6 | PIN_OUTPUT)},   /* mcasp1_fsr.vout2_d1 */
	{MCASP1_AXR2, (M6 | PIN_OUTPUT)},  /* mcasp1_axr2.vout2_d2 */
	{MCASP1_AXR3, (M6 | PIN_OUTPUT)},  /* mcasp1_axr3.vout2_d3 */
	{MCASP1_AXR4, (M6 | PIN_OUTPUT)},  /* mcasp1_axr4.vout2_d4 */
	{MCASP1_AXR5, (M6 | PIN_OUTPUT)},  /* mcasp1_axr5.vout2_d5 */
	{MCASP1_AXR6, (M6 | PIN_OUTPUT)},  /* mcasp1_axr6.vout2_d6 */
	{MCASP1_AXR7, (M6 | PIN_OUTPUT)},  /* mcasp1_axr7.vout2_d7 */
	{MCASP2_ACLKR, (M6 | PIN_OUTPUT)}, /* mcasp2_aclkr.vout2_d8 */
	{MCASP2_FSR, (M6 | PIN_OUTPUT)},   /* mcasp2_fsr.vout2_d9 */
	{MCASP2_AXR0, (M6 | PIN_OUTPUT)},  /* mcasp2_axr0.vout2_d10 */
	{MCASP2_AXR1, (M6 | PIN_OUTPUT)},  /* mcasp2_axr1.vout2_d11 */
	{MCASP2_AXR4, (M6 | PIN_OUTPUT)},  /* mcasp2_axr4.vout2_d12 */
	{MCASP2_AXR5, (M6 | PIN_OUTPUT)},  /* mcasp2_axr5.vout2_d13 */
	{MCASP2_AXR6, (M6 | PIN_OUTPUT)},  /* mcasp2_axr6.vout2_d14 */
	{MCASP2_AXR7, (M6 | PIN_OUTPUT)},  /* mcasp2_axr7.vout2_d15 */
	{MCASP4_ACLKX, (M6 | PIN_OUTPUT)}, /* mcasp4_aclkx.vout2_d16 */
	{MCASP4_FSX, (M6 | PIN_OUTPUT)},   /* mcasp4_fsx.vout2_d17 */
	{MCASP4_AXR0, (M6 | PIN_OUTPUT)},  /* mcasp4_axr0.vout2_d18 */
	{MCASP4_AXR1, (M6 | PIN_OUTPUT)},  /* mcasp4_axr1.vout2_d19 */
	{MCASP5_ACLKX, (M6 | PIN_OUTPUT)}, /* mcasp5_aclkx.vout2_d20 */
	{MCASP5_FSX, (M6 | PIN_OUTPUT)},   /* mcasp5_fsx.vout2_d21 */
	{MCASP5_AXR0, (M6 | PIN_OUTPUT)},  /* mcasp5_axr0.vout2_d22 */
	{MCASP5_AXR1, (M6 | PIN_OUTPUT)},  /* mcasp5_axr1.vout2_d23 */
	/* LCD EN */
	{VOUT1_D23, (M14 | PIN_OUTPUT_PULLDOWN)}, /* vout1_d23.gpio8_23 */
	/* Backlight PWM */
	{VIN1A_D0, (M10 | PIN_OUTPUT_PULLDOWN)},  /* vin1a_d0.ehrpwm1A */
	/* EDT Touch IRQ */
	{VOUT1_D22, (M14 | PIN_INPUT_PULLUP)},    /* vout1_d22.gpio8_22 */

	/* PCIe PERSTn/PWRGD */
	{VOUT1_D4, (M14 | PIN_INPUT_PULLDOWN)}, /* vout1_d4.gpio8_4 */
	/* PCIe (U28-OE) */
	{VOUT1_D7, (M14 | PIN_INPUT_PULLUP)},   /* vout1_d7.gpio8_7 */

	/* VTT_EN */
	{VIN2A_D7, (M14 | PIN_OUTPUT)}, /* vin2a_d7.gpio4_8 */

	{ON_OFF, (M1 | PIN_OUTPUT)},  /* on_off.on_off */
	{RTC_PORZ, (M0 | PIN_INPUT)}, /* rtc_porz.rtc_porz */
	{RTCK, (M0 | PIN_OUTPUT)},    /* rtck.rtck */
	{EMU0, (M0 | PIN_INPUT)},     /* emu0.emu0 */
	{EMU1, (M0 | PIN_INPUT)},     /* emu1.emu1 */
	{NMIN_DSP, (M0 | PIN_INPUT)}, /* nmin_dsp.nmin_dsp */
	{RSTOUTN, (M0 | PIN_OUTPUT)}, /* rstoutn.rstoutn */

};

const struct pad_conf_entry early_padconf[] = {
	/* RS232 (UART3) */
	{UART3_RXD, (M0 | PIN_INPUT_SLEW)}, /* UART3_RXD */
	{UART3_TXD, (M0 | PIN_INPUT_SLEW)}, /* UART3_TXD */

	/* RS232 (UART5) */
	{VOUT1_D0, (M2 | PIN_INPUT_SLEW)},  /* VOUT1_D0.UART5_RXD */
	{VOUT1_D1, (M2 | PIN_INPUT_SLEW)},  /* VOUT1_D1.UART5_TXD */

	/* I2C1 */
	{I2C1_SDA, (M0 | PIN_INPUT_PULLUP)}, /* I2C1_SDA */
	{I2C1_SCL, (M0 | PIN_INPUT_PULLUP)}, /* I2C1_SCL */
};

#ifdef CONFIG_IODELAY_RECALIBRATION
const struct iodelay_cfg_entry iodelay_cfg_array_sr1_1[] = {
	{0x06F0, 480, 0},    /* CFG_RGMII0_RXC_IN */
	{0x06FC, 111, 1641}, /* CFG_RGMII0_RXCTL_IN */
	{0x0708, 272, 1116}, /* CFG_RGMII0_RXD0_IN */
	{0x0714, 243, 1260}, /* CFG_RGMII0_RXD1_IN */
	{0x0720, 0, 1614},   /* CFG_RGMII0_RXD2_IN */
	{0x072C, 105, 1673}, /* CFG_RGMII0_RXD3_IN */
	{0x0740, 531, 120},  /* CFG_RGMII0_TXC_OUT */
	{0x074C, 201, 60},   /* CFG_RGMII0_TXCTL_OUT */
	{0x0758, 229, 120},  /* CFG_RGMII0_TXD0_OUT */
	{0x0764, 141, 0},    /* CFG_RGMII0_TXD1_OUT */
	{0x0770, 495, 120},  /* CFG_RGMII0_TXD2_OUT */
	{0x077C, 660, 120},  /* CFG_RGMII0_TXD3_OUT */
	{0x0A70, 1551, 115}, /* CFG_VIN2A_D12_OUT */
	{0x0A7C, 816, 0},    /* CFG_VIN2A_D13_OUT */
	{0x0A88, 876, 0},    /* CFG_VIN2A_D14_OUT */
	{0x0A94, 312, 0},    /* CFG_VIN2A_D15_OUT */
	{0x0AA0, 58, 0},     /* CFG_VIN2A_D16_OUT */
	{0x0AAC, 0, 0},      /* CFG_VIN2A_D17_OUT */
	{0x0AB0, 702, 0},    /* CFG_VIN2A_D18_IN */
	{0x0ABC, 136, 976},  /* CFG_VIN2A_D19_IN */
	{0x0AD4, 210, 1357}, /* CFG_VIN2A_D20_IN */
	{0x0AE0, 189, 1462}, /* CFG_VIN2A_D21_IN */
	{0x0AEC, 232, 1278}, /* CFG_VIN2A_D22_IN */
	{0x0AF8, 0, 1397},   /* CFG_VIN2A_D23_IN */
};

const struct iodelay_cfg_entry iodelay_cfg_array_sr2_0[] = {
	{0x0218, 114, 0},     /* CFG_GPMC_A3_OUT */
	{0x0144, 0, 0},       /* CFG_GPMC_A13_IN */
	{0x0150, 2575, 966},  /* CFG_GPMC_A14_IN */
	{0x015C, 2503, 889},  /* CFG_GPMC_A15_IN */
	{0x0168, 2528, 1007}, /* CFG_GPMC_A16_IN */
	{0x0170, 0, 0},       /* CFG_GPMC_A16_OUT */
	{0x0174, 2533, 980},  /* CFG_GPMC_A17_IN */
	{0x0188, 590, 0},     /* CFG_GPMC_A18_OUT */
	{0x0374, 0, 0},       /* CFG_GPMC_CS2_OUT */
	{0x0380, 70, 0},      /* CFG_GPMC_CS3_OUT */
	{0x06F0, 260, 0},     /* CFG_RGMII0_RXC_IN */
	{0x06FC, 0, 1412},    /* CFG_RGMII0_RXCTL_IN */
	{0x0708, 123, 1047},  /* CFG_RGMII0_RXD0_IN */
	{0x0714, 139, 1081},  /* CFG_RGMII0_RXD1_IN */
	{0x0720, 195, 1100},  /* CFG_RGMII0_RXD2_IN */
	{0x072C, 239, 1216},  /* CFG_RGMII0_RXD3_IN */
	{0x0740, 89, 0},      /* CFG_RGMII0_TXC_OUT */
	{0x074C, 15, 125},    /* CFG_RGMII0_TXCTL_OUT */
	{0x0758, 339, 162},   /* CFG_RGMII0_TXD0_OUT */
	{0x0764, 146, 94},    /* CFG_RGMII0_TXD1_OUT */
	{0x0770, 0, 27},      /* CFG_RGMII0_TXD2_OUT */
	{0x077C, 291, 205},   /* CFG_RGMII0_TXD3_OUT */
	{0x0A70, 0, 0},       /* CFG_VIN2A_D12_OUT */
	{0x0A7C, 219, 101},   /* CFG_VIN2A_D13_OUT */
	{0x0A88, 92, 58},     /* CFG_VIN2A_D14_OUT */
	{0x0A94, 135, 100},   /* CFG_VIN2A_D15_OUT */
	{0x0AA0, 154, 101},   /* CFG_VIN2A_D16_OUT */
	{0x0AAC, 78, 27},     /* CFG_VIN2A_D17_OUT */
	{0x0AB0, 411, 0},     /* CFG_VIN2A_D18_IN */
	{0x0ABC, 0, 382},     /* CFG_VIN2A_D19_IN */
	{0x0AD4, 320, 750},   /* CFG_VIN2A_D20_IN */
	{0x0AE0, 192, 836},   /* CFG_VIN2A_D21_IN */
	{0x0AEC, 294, 669},   /* CFG_VIN2A_D22_IN */
	{0x0AF8, 5, 700},     /* CFG_VIN2A_D23_IN */
};
#endif

#endif /* _MUX_DATA_PHYCORE_AM57X_ */
