/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

/* Peripheral clusters */
#define PER3_BASE	0x80000000
#define PER2_BASE	0x80110000
#define PER1_BASE	0x80120000
#define PER4_BASE	0x80150000

#define PER6_BASE	0xa03c0000
#define PER7_BASE	0xa03d0000
#define PER5_BASE	0xa03e0000

/* Per1 */
#define UART0_BASE	(PER1_BASE + 0x0000)
#define UART1_BASE	(PER1_BASE + 0x1000)
#define CLKRST1_BASE	(PER1_BASE + 0xf000)

#define BOOTROM_BASE		0x90000000
#define ASIC_ID_LOC_ED_V1	(BOOTROM_BASE + 0x1FFF4)
#define ASIC_ID_LOC_V2		(BOOTROM_BASE + 0x1DBF4)

/* Per2 */
#define CLKRST2_BASE	(PER2_BASE + 0xf000)

/* Per3 */
#define UART2_BASE	(PER3_BASE + 0x7000)
#define CLKRST3_BASE	(PER3_BASE + 0xf000)

/* Per4 */
#define PRCMU_BASE	(PER4_BASE + 0x07000)
#define PRCMU_TCDM_BASE	(PER4_BASE + 0x0f000)

/* Per5 */
#define CLKRST5_BASE	(PER5_BASE + 0x1f000)

/* Per6 */
#define MTU0_BASE_V1	(PER6_BASE + 0x6000)
#define MTU1_BASE_v1	(PER6_BASE + 0x7000)
#define CLKRST6_BASE	(PER6_BASE + 0xf000)

/* GPIO */
#define GPIO0_BASE	(PER1_BASE + 0xE000)
#define GPIO1_BASE	(PER1_BASE + 0xE000 + 0x80)

#define GPIO2_BASE	(PER3_BASE + 0xE000)
#define GPIO3_BASE	(PER3_BASE + 0xE000 + 0x80)
#define GPIO4_BASE	(PER3_BASE + 0xE000 + 0x100)
#define GPIO5_BASE	(PER3_BASE + 0xE000 + 0x180)

#define GPIO6_BASE	(PER2_BASE + 0xE000)
#define GPIO7_BASE	(PER2_BASE + 0xE000 + 0x80)

#define GPIO8_BASE	(PER5_BASE + 0x1E000)

/* STM */
#define STM_BASE	(PER3_BASE + 0x100000)

/* Boot Info */
#define BOOT_INFO_BACKUPRAM1 (PER4_BASE + 0x1F7C)

/* AVS Backup Check */
#define AVS_BACKUP_BACKUPRAM1 (PER4_BASE + 0x1C30)

#endif /* __PERIPHERAL_H__ */

