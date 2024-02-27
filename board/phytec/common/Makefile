# SPDX-License-Identifier: GPL-2.0+
# Copyright (C) 2023 PHYTEC Messtechnik GmbH
# Author: Teresa Remmet <t.remmet@phytec.de>

ifdef CONFIG_SPL_BUILD
# necessary to create built-in.o
obj-$(CONFIG_K3_AM64_DDRSS) += k3_ddrss_patch.o
endif

obj-y += phytec_som_detection.o phytec_som_detection_blocks.o
obj-y += am62a_som_detection.o
obj-y += am62_som_detection.o
obj-y += am64_som_detection.o
