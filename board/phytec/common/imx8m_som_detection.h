/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2020 PHYTEC Messtechnik GmbH
 * Author: Teresa Remmet <t.remmet@phytec.de>
 */

#ifndef _PHYTEC_IMX8M_SOM_DETECTION_H
#define _PHYTEC_IMX8M_SOM_DETECTION_H

#include "phytec_som_detection.h"

#if IS_ENABLED(CONFIG_PHYTEC_IMX8M_SOM_DETECTION)

u8 phytec_imx8m_detect(u8 som, char *opt);
u8 __maybe_unused phytec_get_imx8m_ddr_size(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_imx8mp_rtc(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_imx8m_spi(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_imx8m_eth(struct phytec_eeprom_data *data);

#else

inline u8 phytec_imx8m_detect(u8 som, char *opt) {
	return -1;
};

inline u8 __maybe_unused phytec_get_imx8m_ddr_size(
		struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_imx8mp_rtc(
		struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_imx8m_spi(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_imx8m_eth(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

#endif /* IS_ENABLED(CONFIG_PHYTEC_IMX8M_SOM_DETECTION) */

#endif /* _PHYTEC_IMX8M_SOM_DETECTION_H */
