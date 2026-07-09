// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62Lx platforms
 *
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <cpu_func.h>
#include <dm/uclass.h>
#include <env.h>
#include <fdt_simplefb.h>
#include <fdt_support.h>
#include <splash.h>
#include <spl.h>
#include <video.h>

#include "../common/fdt_ops.h"

#if IS_ENABLED(CONFIG_SPL_BUILD)
void spl_board_init(void)
{
	enable_caches();
	if (IS_ENABLED(CONFIG_SPL_SPLASH_SCREEN) && IS_ENABLED(CONFIG_SPL_BMP))
		splash_display();
}
#endif

#if CONFIG_IS_ENABLED(SPLASH_SCREEN)
static struct splash_location default_splash_locations[] = {
	{
		.name		= "mmc",
		.storage	= SPLASH_STORAGE_MMC,
		.flags		= SPLASH_STORAGE_FS,
		.devpart	= "1:1",
	},
};

int splash_screen_prepare(void)
{
	return splash_source_load(default_splash_locations,
				  ARRAY_SIZE(default_splash_locations));
}
#endif

int board_init(void)
{
	return 0;
}

#if defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	int ret = -1;

	if (IS_ENABLED(CONFIG_FDT_SIMPLEFB))
		ret = fdt_simplefb_enable_and_mem_rsv(blob);

	if (IS_ENABLED(CONFIG_VIDEO) && IS_ENABLED(CONFIG_OF_LIBFDT)) {
		if (ret && video_is_active())
			return fdt_add_fb_mem_rsv(blob);
	}

	return 0;
}
#endif

#if IS_ENABLED(CONFIG_BOARD_LATE_INIT)
int board_late_init(void)
{
	ti_set_fdt_env(NULL, NULL);
	return 0;
}
#endif
