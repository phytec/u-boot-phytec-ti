// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#include <common.h>

#include "am57_som_detection.h"

extern struct phytec_eeprom_data eeprom_data;

/*
 * Filter DDR3 ram option.
 *
 * Returns:
 *  - The DDR3 ram option
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_ddr_size(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 ddr_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		ddr_id = PHYTEC_GET_OPTION(opt[0]);
	else
		ddr_id = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: ddr id: %u\n", __func__, ddr_id);
	return ddr_id;
}

/*
 * Filter ram ECC option.
 *
 * Returns:
 *  - 0x0 if no ECC ram is populated.
 *  - The ram ECC option
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_ecc(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 ecc_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		ecc_id = PHYTEC_GET_OPTION(opt[1]);
	else
		ecc_id = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: ecc id: %u\n", __func__, ecc_id);
	return ecc_id;
}

/*
 * Filter storage option.
 *
 * Returns:
 *  - 0x0 if no eMMC/NAND storage is populated.
 *  - The storage option
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_storage(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 storage_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		storage_id = PHYTEC_GET_OPTION(opt[2]);
	else
		storage_id = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: storage id: %u\n", __func__, storage_id);
	return storage_id;
}

/*
 * Filter SPI-NOR flash information.
 *
 * returns:
 *  - 0x0 if no SPI is populated.
 *  - Otherwise a board depended code for the size.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_spi(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 spi;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		spi = PHYTEC_GET_OPTION(opt[3]);
	else
		spi = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: spi: %u\n", __func__, spi);
	return spi;
}

/*
 * Filter AM57x SoC variant.
 *
 * Returns:
 *  - The AM57x SoC variant option
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_soc(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 soc_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		soc_id = PHYTEC_GET_OPTION(opt[4]);
	else
		soc_id = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: soc id: %u\n", __func__, soc_id);
	return soc_id;
}

/*
 * Filter EEPROM information.
 *
 * returns:
 *  - 0x0 if no EEPROM is populated.
 *  - Otherwise a board depended code for the size.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_eeprom(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 eeprom;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		eeprom = PHYTEC_GET_OPTION(opt[5]);
	else
		eeprom = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: eeprom: %u\n", __func__, eeprom);
	return eeprom;
}

/*
 * Filter ethernet phy information.
 *
 * returns:
 *  - 0x0 no ethernet phy is populated.
 *  - 0x1 if 10/100/1000 MBit Phy is populated.
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_eth(struct phytec_eeprom_data *data)
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
u8 __maybe_unused phytec_get_am57_rtc(struct phytec_eeprom_data *data)
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

/*
 * Filter AM57x temperature grade.
 *
 * Returns:
 *  - The AM57x temperature grade option
 *  - PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_am57_temp(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 temp;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		temp = PHYTEC_GET_OPTION(opt[8]);
	else
		temp = PHYTEC_EEPROM_INVAL;

	pr_debug("%s: temp grade id: %u\n", __func__, temp);
	return temp;
}

/*
 * Get AM57x SOM option string.
 *
 * Returns:
 *  - The AM57x SOM option string
 *  - NULL when the data is invalid.
 */
char * __maybe_unused phytec_get_am57_opt(struct phytec_eeprom_data *data)
{
	char *opt;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt) {
		/* Always return first 9 chars */
		opt[9] = '\0';
	} else {
		opt = NULL;
	}

	pr_debug("%s: option: %s\n", __func__, opt);
	return opt;
}
