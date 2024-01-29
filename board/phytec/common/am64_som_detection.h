/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Daniel Schultz <d.schultz@phytec.de>
 */

#ifndef _PHYTEC_AM64_SOM_DETECTION_H
#define _PHYTEC_AM64_SOM_DETECTION_H

#include "phytec_som_detection.h"

#if IS_ENABLED(CONFIG_PHYTEC_AM64_SOM_DETECTION)

u8 phytec_am64_detect(u8 som, char *opt);
u8 __maybe_unused phytec_get_am64_ddr_size(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am64_spi(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am64_eth(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am64_rtc(struct phytec_eeprom_data *data);

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

#endif /* IS_ENABLED(CONFIG_PHYTEC_AM64_SOM_DETECTION) */

#endif /* _PHYTEC_AM64_SOM_DETECTION_H */
