// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Daniel Schultz <d.schultz@phytec.de>
 */

#include <common.h>
#include <asm/arch/hardware.h>

#include "am64_som_detection.h"

extern struct phytec_eeprom_data eeprom_data;

#if IS_ENABLED(CONFIG_PHYTEC_AM64_SOM_DETECTION)

/* Check if the SoM is actually one of the following products:
 * - AM64xx
 *
 * Returns 0 in case it's a known SoM. Otherwise, returns -1.
 */
u8 phytec_am64_detect(u8 som, char *opt)
{
	if (som == PHYTEC_AM64XX_SOM && soc_is_am64x())
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
u8 __maybe_unused phytec_get_am64_ddr_size(struct phytec_eeprom_data *data)
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

/*
 * Filter SPI-NOR flash information.
 *
 * returns:
 *  - 0x0 if no SPI is poulated.
 *  - Otherwise a board depended code for the size.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am64_spi(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 spi;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		spi = PHYTEC_GET_OPTION(opt[5]);
	else
		spi = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: spi: %u\n", __func__, spi);
	return spi;
}

/*
 * Filter ethernet phy information.
 *
 * returns:
 *  - 0x0 no ethernet phy is poulated.
 *  - 0x1 if 10/100/1000 MBit Phy is populated.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am64_eth(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 eth;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		eth = PHYTEC_GET_OPTION(opt[6]);
	else
		eth = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: eth: %u\n", __func__, eth);
	return eth;
}

/*
 * Filter RTC information.
 *
 * returns:
 *  - 0 if no RTC is poulated.
 *  - 1 if it is populated.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am64_rtc(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 rtc;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		rtc = PHYTEC_GET_OPTION(opt[7]);
	else
		rtc = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: rtc: %u\n", __func__, rtc);
	return rtc;
}

#else

inline u8 phytec_am64_detect(u8 som, char *opt)
{
	return -1;
}

inline u8 __maybe_unused phytec_get_am64_ddr_size(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am64_spi(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am64_eth(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am64p_rtc(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

#endif
