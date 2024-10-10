// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2024 PHYTEC Messtechnik GmbH
 * Author: Dominik Haller <d.haller@phytec.de>
 */

#include <common.h>
#include <asm/arch/hardware.h>

#include "am68_som_detection.h"

extern struct phytec_eeprom_data eeprom_data;

#if IS_ENABLED(CONFIG_PHYTEC_AM68_SOM_DETECTION)

/* Check if the SoM is actually one of the following products:
 * - AM68x
 *
 * Returns 0 in case it's a known SoM. Otherwise, returns -1.
 */
u8 phytec_am68_detect(u8 som, char *opt)
{
	if (som == PHYTEC_AM68X_SOM && soc_is_j721s2())
		return 0;

	return -1;
}

/*
 * Filter LPDDR4 ram size.
 *
 * Returns:
 *  - The size
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am68_ddr_size(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 ddr_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		ddr_id = PHYTEC_GET_OPTION(opt[3]);
	else
		ddr_id = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: ddr id: %u\n", __func__, ddr_id);
	return ddr_id;
}

#else

inline u8 phytec_am68_detect(u8 som, char *opt)
{
	return -1;
}

inline u8 __maybe_unused phytec_get_am68_ddr_size(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

#endif
