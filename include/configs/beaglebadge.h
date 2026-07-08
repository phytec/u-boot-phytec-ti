/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Configuration header file for BeagleBadge
 *
 * https://beaglebadge.org/
 *
 * Copyright (C) 2024-2025 Texas Instruments Incorporated - https://www.ti.com/
 */

#ifndef __CONFIG_BEAGLEBADGE_H
#define __CONFIG_BEAGLEBADGE_H

#include <configs/ti_armv7_common.h>

#define CFG_SYS_UBOOT_BASE CONFIG_TEXT_BASE

/**
 * define BEAGLEBADGE_TIBOOT3_IMAGE_GUID - firmware GUID for BeagleBadge
 *                                         tiboot3.bin
 * define BEAGLEBADGE_SPL_IMAGE_GUID     - firmware GUID for BeagleBadge SPL
 * define BEAGLEBADGE_UBOOT_IMAGE_GUID   - firmware GUID for BeagleBadge UBOOT
 *
 * These GUIDs are used in capsules updates to identify the corresponding
 * firmware object.
 *
 * Board developers using this as a starting reference should
 * define their own GUIDs to ensure that firmware repositories (like
 * LVFS) do not confuse them. Generated with "mkeficapsule guidgen".
 */
#define BEAGLEBADGE_TIBOOT3_IMAGE_GUID \
	EFI_GUID(0x0F498FDE, 0xD6A5, 0x53F6, 0xA9, 0x87, \
		0x34, 0xB1, 0x83, 0x67, 0xF8, 0x7F)

#define BEAGLEBADGE_SPL_IMAGE_GUID \
	EFI_GUID(0x0C989265, 0x2034, 0x54A9, 0xA2, 0xCB, \
		0x10, 0xCF, 0x4A, 0xD6, 0x21, 0xB9)

#define BEAGLEBADGE_UBOOT_IMAGE_GUID \
	EFI_GUID(0x783F68D7, 0x06A4, 0x5AEC, 0x83, 0xAA, \
		0xFA, 0xA1, 0xA8, 0x10, 0xEA, 0x88)

#endif /* __CONFIG_BEAGLEBADGE_H */
