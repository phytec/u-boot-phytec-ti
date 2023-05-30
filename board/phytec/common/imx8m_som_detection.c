// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 PHYTEC Messtechnik GmbH
 * Author: Teresa Remmet <t.remmet@phytec.de>
 */

#include <common.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/arch/sys_proto.h>
#include <dm/device.h>
#include <dm/uclass.h>
#include <i2c.h>
#include <u-boot/crc.h>

#include "imx8m_som_detection.h"

extern struct phytec_eeprom_data eeprom_data;

/* Check if the SoM is actually one of the following products:
 * - i.MX8MM
 * - i.MX8MN
 * - i.MX8MP
 * - i.MX8MQ
 *
 * Returns 0 in case it's an known SoM. Otherwise, returns -1.
 */
u8 phytec_imx8m_detect(u8 som, char *opt)
{
	if (som == PHYTEC_IMX8MP_SOM && is_imx8mp())
		return 0;

	if (som == PHYTEC_IMX8MM_SOM) {
		if (((opt[0] - '0') != 0) &&
		    ((opt[1] - '0') == 0) && is_imx8mm())
			return 0;
		else if (((opt[0] - '0') == 0) &&
			 ((opt[1] - '0') != 0) && is_imx8mn())
			return 0;
		return -1;
	}

	if (som == PHYTEC_IMX8MQ_SOM && is_imx8mq())
		return 0;

	return -1;
}

/*
 * So far all PHYTEC i.MX8M boards have RAM size definition at the
 * same location.
 */
u8 __maybe_unused phytec_get_imx8m_ddr_size(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 ddr_id;

	if (!data)
		data = &eeprom_data;

	opt = phytec_get_opt(data);
	if (opt)
		ddr_id = opt[2] - '0';
	else
		ddr_id = PHYTEC_EEPROM_INVAL;

	debug("%s: ddr id: %u\n", __func__, ddr_id);
	return ddr_id;
}

/*
 * Filter SPI-NOR flash information. All i.MX8M boards have this at
 * the same location.
 * returns: 0x0 if no SPI is poulated. Otherwise a board depended
 * code for the size. PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_imx8m_spi(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 spi;

	if (!data)
		data = &eeprom_data;

	if (data->api_rev < PHYTEC_API_REV2)
		return PHYTEC_EEPROM_INVAL;

	opt = phytec_get_opt(data);
	if (opt)
		spi = opt[4] - '0';
	else
		spi = PHYTEC_EEPROM_INVAL;

	debug("%s: spi: %u\n", __func__, spi);
	return spi;
}

/*
 * Filter ethernet phy information. All i.MX8M boards have this at
 * the same location.
 * returns: 0x0 if no ethernet phy is poulated. 0x1 if it is populated.
 * PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_imx8m_eth(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 eth;

	if (!data)
		data = &eeprom_data;

	if (data->api_rev < PHYTEC_API_REV2)
		return PHYTEC_EEPROM_INVAL;

	opt = phytec_get_opt(data);
	if (opt) {
		eth = opt[5] - '0';
		eth &= 0x1;
	} else {
		eth = PHYTEC_EEPROM_INVAL;
	}

	debug("%s: eth: %u\n", __func__, eth);
	return eth;
}

/*
 * Filter RTC information.
 * returns: 0 if no RTC is poulated. 1 if it is populated.
 * PHYTEC_EEPROM_INVAL when the data is invalid.
 */
u8 __maybe_unused phytec_get_imx8mp_rtc(struct phytec_eeprom_data *data)
{
	char *opt;
	u8 rtc;

	if (!data)
		data = &eeprom_data;

	if (data->api_rev < PHYTEC_API_REV2)
		return PHYTEC_EEPROM_INVAL;

	opt = phytec_get_opt(data);
	if (opt) {
		rtc = opt[5] - '0';
		rtc &= 0x4;
		rtc = !(rtc >> 2);
	} else {
		rtc = PHYTEC_EEPROM_INVAL;
	}
	debug("%s: rtc: %u\n", __func__, rtc);
	return rtc;
}
