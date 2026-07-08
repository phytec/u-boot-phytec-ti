// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62L3 BeagleBadge
 *
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <config.h>
#include <efi_loader.h>
#include <env.h>
#include <fdtdec.h>
#include <fdt_support.h>

struct efi_fw_image fw_images[] = {
	{
		.image_type_id = BEAGLEBADGE_TIBOOT3_IMAGE_GUID,
		.fw_name = u"BEAGLEBADGE_TIBOOT3",
		.image_index = 1,
	},
	{
		.image_type_id = BEAGLEBADGE_SPL_IMAGE_GUID,
		.fw_name = u"BEAGLEBADGE_SPL",
		.image_index = 2,
	},
	{
		.image_type_id = BEAGLEBADGE_UBOOT_IMAGE_GUID,
		.fw_name = u"BEAGLEBADGE_UBOOT",
		.image_index = 3,
	}
};

struct efi_capsule_update_info update_info = {
	.dfu_string = "sf 0:0=tiboot3.bin part 0 1;tispl.bin part 0 2;"
		      "u-boot.img part 0 3",
	.num_images = ARRAY_SIZE(fw_images),
	.images = fw_images,
};

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	return fdtdec_setup_mem_size_base();
}

int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	char fdtfile[50];

	snprintf(fdtfile, sizeof(fdtfile), "%s.dtb", CONFIG_DEFAULT_DEVICE_TREE);

	env_set("fdtfile", fdtfile);

	return 0;
}
#endif
