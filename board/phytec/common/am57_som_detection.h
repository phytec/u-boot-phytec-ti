/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2023 PHYTEC Messtechnik GmbH
 * Author: Primoz Fiser <primoz.fiser@norik.com>
 */

#ifndef _PHYTEC_AM57_SOM_DETECTION_H
#define _PHYTEC_AM57_SOM_DETECTION_H

#include "phytec_som_detection.h"

#if IS_ENABLED(CONFIG_PHYTEC_AM57_SOM_DETECTION)

u8 __maybe_unused phytec_get_am57_ddr_size(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_ecc(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_storage(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_spi(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_soc(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_eeprom(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_eth(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_rtc(struct phytec_eeprom_data *data);
u8 __maybe_unused phytec_get_am57_temp(struct phytec_eeprom_data *data);
char * __maybe_unused phytec_get_am57_opt(struct phytec_eeprom_data *data);

#else

inline u8 __maybe_unused phytec_get_am57_ddr_size(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_ecc(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_storage(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_spi(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_soc(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_eeprom(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_eth(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_rtc(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline u8 __maybe_unused phytec_get_am57_temp(struct phytec_eeprom_data *data)
{
	return PHYTEC_EEPROM_INVAL;
}

inline char * __maybe_unused phytec_get_am57_opt(struct phytec_eeprom_data *data)
{
	return NULL;
}

#endif /* IS_ENABLED(CONFIG_PHYTEC_AM57_SOM_DETECTION) */

#endif /* _PHYTEC_AM57_SOM_DETECTION_H */
