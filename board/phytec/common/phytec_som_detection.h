/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2020 PHYTEC Messtechnik GmbH
 * Author: Teresa Remmet <t.remmet@phytec.de>
 */

#ifndef _PHYTEC_SOM_DETECTION_H
#define _PHYTEC_SOM_DETECTION_H

#define PHYTEC_MAX_OPTIONS	17
#define PHYTEC_IMX8MQ_SOM	66
#define PHYTEC_IMX8MM_SOM	69
#define PHYTEC_IMX8MP_SOM	70
#define PHYTEC_AM62XX_SOM	71
#define PHYTEC_AM64XX_SOM	72

#define PHYTEC_EEPROM_INVAL	0xff

#define PHYTEC_GET_OPTION(option) \
	((option > '9') ? option - 'A' + 10 : option - '0')

enum {
	PHYTEC_API_REV0 = 0,
	PHYTEC_API_REV1,
	PHYTEC_API_REV2,
};

static const char * const phytec_som_type_str[] = {
	"PCM",
	"PCL",
	"KSM",
	"KSP",
};

struct phytec_api0_data {
	u8 pcb_rev;		/* PCB revision of SoM */
	u8 som_type;		/* SoM type */
	u8 ksp_no;		/* KSP no */
	char opt[16];		/* SoM options */
	u8 mac[6];		/* MAC address (optional) */
	u8 pad[5];		/* padding */
	u8 cksum;		/* checksum */
} __attribute__ ((__packed__));

struct phytec_api2_data {
	u8 pcb_rev;		/* PCB revision of SoM */
	u8 pcb_sub_opt_rev;	/* PCB subrevision and opt revision */
	u8 som_type;		/* SoM type */
	u8 som_no;		/* SoM number */
	u8 ksp_no;		/* KSP information */
	char opt[PHYTEC_MAX_OPTIONS]; /* SoM options */
	char bom_rev[2];	/* BOM revision */
	u8 mac[6];		/* MAC address (optional) */
	u8 crc8;		/* checksum */
} __attribute__ ((__packed__));

struct phytec_eeprom_data {
	u8 api_rev;
	union {
		struct phytec_api0_data data_api0;
		struct phytec_api2_data data_api2;
	} data;
} __attribute__ ((__packed__));

#if IS_ENABLED(CONFIG_PHYTEC_SOM_DETECTION)

int phytec_eeprom_data_setup(struct phytec_eeprom_data *data,
			     int bus_num, int addr, int addr_fallback);
int phytec_eeprom_data_init(struct phytec_eeprom_data *data,
			    int bus_num, int addr);
void __maybe_unused phytec_print_som_info(struct phytec_eeprom_data *data);

char * __maybe_unused phytec_get_opt(struct phytec_eeprom_data *data);

# else

inline int phytec_eeprom_data_setup(struct phytec_eeprom_data *data,
				    int bus_num, int addr, int addr_fallback)
{
	return PHYTEC_EEPROM_INVAL;
}

inline int phytec_eeprom_data_init(struct phytec_eeprom_data *data,
				   int bus_num, int addr)
{
	return PHYTEC_EEPROM_INVAL;
}

inline void __maybe_unused phytec_print_som_info(
		struct phytec_eeprom_data *data)
{
	return;
}

inline char * __maybe_unused phytec_get_opt(struct phytec_eeprom_data *data)
{
	return NULL;
}

#endif /* IS_ENABLED(CONFIG_PHYTEC_SOM_DETECTION) */

#endif /* _PHYTEC_SOM_DETECTION_H */
