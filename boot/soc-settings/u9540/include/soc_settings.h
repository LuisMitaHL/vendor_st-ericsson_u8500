/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __SOC_SETTINGS_H__
#define __SOC_SETTINGS_H__

#define IMAD_CONFIG
#define IMAD_CONFIG_DDR_INIT
#define IMAD_CONFIG_READ_FUSES
#define IMAD_CONFIG_LVL0_FIREWALL

#define BOOT_OK				0
#define BOOT_INTERNAL_ERROR		0xA0010022

#define PRCMU_HW_REGS   ((volatile t_prcmu_hw_registers*)PRCMU_REG_START_ADDR)

#define XP70_IT_EVENT_MB0       (1 << 0)
#define XP70_IT_EVENT_MB4       (1 << 4)

#define BOARD_VERSION_MASK 0xff000000

/* Macro hack to stringify #defined integers */
#define _STR(s) #s
#define STR(s) _STR(s)

#define VERSION_STR	"SOC-SETTINGS v5.0 - db9540 v1 "__DATE__" "__TIME__
#define PRCMU_FW_MAJOR_VERSION 5

#define DDR_REGISTERS_LEN	16

#define AB9540_SETTINGS_LEN	88

#define AP9540_DDR0_OFFSET		0x00000000
#define AP9540_DDR0_SIZE		0x3FFFFFFF
#define AP9540_DDR1_OFFSET		0xC0000000
#define AP9540_DDR1_SIZE		0x3FEFFFFF

/* Offset for the firmware version within the TCPM */
#define PRCMU_FW_VERSION_OFFSET 0xA8
#define PRCMU_FW_VERSION API_FW_VERSIONING

enum section_type {
  SECTION_TYPE_OPP = 0,
  SECTION_TYPE_AB9540,
  SECTION_TYPE_DDR_DATA,
  SECTION_TYPE_EXECUTABLE,
  SECTION_TYPE_IMAD,
  SECTION_TYPE_C2C_INIT,
  SECTION_TYPE_CUSTOM_CONFIG
};
#endif /* __SOC_SETTINGS_H__ */

