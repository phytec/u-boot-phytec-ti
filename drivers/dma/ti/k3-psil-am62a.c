// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2022 Texas Instruments Incorporated - https://www.ti.com
 */

#include <linux/kernel.h>

#include "k3-psil-priv.h"

#define PSIL_PDMA_XY_TR(x)					\
	{							\
		.thread_id = x,					\
		.ep_config = {					\
			.ep_type = PSIL_EP_PDMA_XY,		\
			.mapped_channel_id = -1,		\
			.default_flow_id = -1,			\
		},						\
	}

#define PSIL_PDMA_XY_PKT(x)					\
	{							\
		.thread_id = x,					\
		.ep_config = {					\
			.ep_type = PSIL_EP_PDMA_XY,		\
			.mapped_channel_id = -1,		\
			.default_flow_id = -1,			\
			.pkt_mode = 1,				\
		},						\
	}

#define PSIL_ETHERNET(x, ch, flow_base, flow_cnt)		\
	{							\
		.thread_id = x,					\
		.ep_config = {					\
			.ep_type = PSIL_EP_NATIVE,		\
			.pkt_mode = 1,				\
			.needs_epib = 1,			\
			.psd_size = 16,				\
			.mapped_channel_id = ch,		\
			.flow_start = flow_base,		\
			.flow_num = flow_cnt,			\
			.default_flow_id = flow_base,		\
		},						\
	}

/* PSI-L source thread IDs, used for RX (DMA_DEV_TO_MEM) */
static struct psil_ep am62a_src_ep_map[] = {
	/* PDMA_MAIN1 - UART0-6 */
	PSIL_PDMA_XY_PKT(0x4400),
	PSIL_PDMA_XY_PKT(0x4401),
	PSIL_PDMA_XY_PKT(0x4402),
	PSIL_PDMA_XY_PKT(0x4403),
	PSIL_PDMA_XY_PKT(0x4404),
	PSIL_PDMA_XY_PKT(0x4405),
	PSIL_PDMA_XY_PKT(0x4406),
	/* CPSW3G */
	PSIL_ETHERNET(0x4600, 19, 19, 16),
};

/* PSI-L destination thread IDs, used for TX (DMA_MEM_TO_DEV) */
static struct psil_ep am62a_dst_ep_map[] = {
	/* PDMA_MAIN1 - UART0-6 */
	PSIL_PDMA_XY_PKT(0xc400),
	PSIL_PDMA_XY_PKT(0xc401),
	PSIL_PDMA_XY_PKT(0xc402),
	PSIL_PDMA_XY_PKT(0xc403),
	PSIL_PDMA_XY_PKT(0xc404),
	PSIL_PDMA_XY_PKT(0xc405),
	PSIL_PDMA_XY_PKT(0xc406),
	/* CPSW3G */
	PSIL_ETHERNET(0xc600, 19, 19, 8),
	PSIL_ETHERNET(0xc601, 20, 27, 8),
	PSIL_ETHERNET(0xc602, 21, 35, 8),
	PSIL_ETHERNET(0xc603, 22, 43, 8),
	PSIL_ETHERNET(0xc604, 23, 51, 8),
	PSIL_ETHERNET(0xc605, 24, 59, 8),
	PSIL_ETHERNET(0xc606, 25, 67, 8),
	PSIL_ETHERNET(0xc607, 26, 75, 8),
};

struct psil_ep_map am62a_ep_map = {
	.name = "am62a",
	.src = am62a_src_ep_map,
	.src_count = ARRAY_SIZE(am62a_src_ep_map),
	.dst = am62a_dst_ep_map,
	.dst_count = ARRAY_SIZE(am62a_dst_ep_map),
};
