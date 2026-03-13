/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2024, Texas Instruments Incorporated - https://www.ti.com/
 */

#ifndef _K3_DDR_H_
#define _K3_DDR_H_

#include <spl.h>
#include <compiler.h>

/* We need 4 extra entries for:
 * 1. SoC peripherals
 * 2. Flash
 * 3. PCIe 4GB Windows for AM68, AM69, J7200, J721E, J721S2, J742S2 and J784S4 SoCs
 * 4. Sentinel value
 */
#define K3_MEM_MAP_LEN			((CONFIG_NR_DRAM_BANKS) + 4)
#define K3_MEM_MAP_FIRST_BANK_IDX	3

struct k3_ddrss_regs {
	u32 ctl_141;
	u32 phy_1305;
	u32 ctl_88;
	u32 pi_134;
	u32 pi_7;
	u32 ctl_20;
	u32 wdqlvl_f1;
	u32 wdqlvl_f2;
};

int dram_init(void);
int dram_init_banksize(void);

void fixup_ddr_driver_for_ecc(struct spl_image_info *spl_image);
void fixup_memory_node(struct spl_image_info *spl_image);

/* LPDDR4 power management functions */
void k3_ddrss_lpddr4_exit_retention(struct udevice *dev, struct k3_ddrss_regs *regs);
void k3_ddrss_lpddr4_change_freq(struct udevice *dev);
void k3_ddrss_lpddr4_exit_low_power(struct udevice *dev, struct k3_ddrss_regs *regs);
#endif /* _K3_DDR_H_ */
