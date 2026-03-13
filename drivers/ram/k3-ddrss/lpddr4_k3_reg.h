/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 */

#ifndef LPDDR4_K3_REG
#define LPDDR4_K3_REG

#define lpddr4_k3_readreg_ctl(ddrss, reg, pt) do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		TH_OFFSET_FROM_REG(reg, CTL_SHIFT, offset);			\
		result = ddrss->driverdt->readctlconfig(&ddrss->pd, pt, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to read %s\n", __func__, xstr(reg));	\
			hang();						\
		}			\
	} while (0)

#define lpddr4_k3_readreg_pi(ddrss, reg, pt) do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		TH_OFFSET_FROM_REG(reg, PI_SHIFT, offset);			\
		result = ddrss->driverdt->readphyindepconfig(&ddrss->pd, pt, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to read %s\n", __func__, xstr(reg));	\
			hang();						\
		}			\
	} while (0)

#define lpddr4_k3_readreg_phy(ddrss, reg, pt) do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		TH_OFFSET_FROM_REG(reg, PHY_SHIFT, offset);			\
		result = ddrss->driverdt->readphyconfig(&ddrss->pd, pt, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to read %s\n", __func__, xstr(reg));	\
			hang();						\
		}			\
	} while (0)

#define lpddr4_k3_writereg_ctl(ddrss, reg, value)  do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		u32 writeval = value;					\
		TH_OFFSET_FROM_REG(reg, CTL_SHIFT, offset);			\
		result = ddrss->driverdt->writectlconfig(&ddrss->pd, &writeval, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to write %s\n", __func__, xstr(reg)); \
			hang();						\
		}		\
	} while (0)

#define lpddr4_k3_writereg_pi(ddrss, reg, value)  do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		u32 writeval = value;					\
		TH_OFFSET_FROM_REG(reg, PI_SHIFT, offset);			\
		result = ddrss->driverdt->writephyindepconfig(&ddrss->pd, &writeval, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to write %s\n", __func__, xstr(reg)); \
			hang();						\
		}		\
	} while (0)

#define lpddr4_k3_writereg_phy(ddrss, reg, value) do {	\
		u16 offset = 0U;					\
		u32 result = 0U;					\
		u32 writeval = value;					\
		TH_OFFSET_FROM_REG(reg, PHY_SHIFT, offset);			\
		result = ddrss->driverdt->writephyconfig(&ddrss->pd, &writeval, (u16 *)(&offset), 1); \
		if (result > 0U) {					\
			printf("%s: Failed to write %s\n", __func__, xstr(reg)); \
			hang();						\
		}		\
	} while (0)

#define lpddr4_k3_set_ctl(k3_ddrss, reg, mask) do {	\
	u32 val;					\
	lpddr4_k3_readreg_ctl(ddrss, reg, &val);		\
	val |= mask;					\
	lpddr4_k3_writereg_ctl(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_ctl(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_ctl(ddrss, reg, &val);		\
		val &= ~(mask);					\
		lpddr4_k3_writereg_ctl(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_set_pi(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_pi(ddrss, reg, &val);		\
		val |= mask;					\
		lpddr4_k3_writereg_pi(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_pi(k3_ddrss, reg, mask) do {		\
		u32 val;					\
		lpddr4_k3_readreg_pi(ddrss, reg, &val);		\
		val &= ~(mask);					\
		lpddr4_k3_writereg_pi(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_set_phy(ddrss, reg, mask) do {		\
	u32 val;					\
	lpddr4_k3_readreg_phy(ddrss, reg, &val);		\
	val |= mask;					\
	lpddr4_k3_writereg_phy(ddrss, reg, val);		\
	} while (0)

#define lpddr4_k3_clr_phy(ddrss, reg, mask) do {		\
	u32 val;					\
	lpddr4_k3_readreg_phy(ddrss, reg, &val);		\
	val &= ~(mask);					\
	lpddr4_k3_writereg_phy(ddrss, reg, val);		\
	} while (0)

#endif  /* LPDDR4_K3_REG */
