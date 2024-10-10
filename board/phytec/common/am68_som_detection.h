/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2024 PHYTEC Messtechnik GmbH
 * Author: Dominik Haller <d.haller@phytec.de>
 */

#ifndef _PHYTEC_AM68_SOM_DETECTION_H
#define _PHYTEC_AM68_SOM_DETECTION_H

#include "phytec_som_detection.h"

u8 phytec_am68_detect(u8 som, char *opt);
u8 __maybe_unused phytec_get_am68_ddr_size(struct phytec_eeprom_data *data);

#endif /* _PHYTEC_AM68_SOM_DETECTION_H */
