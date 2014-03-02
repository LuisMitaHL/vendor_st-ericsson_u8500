/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __GPIO_H__
#define __GPIO_H__

/* Register in the logic block */
#define GPIO_DAT	0x00
#define GPIO_DATS	0x04
#define GPIO_DATC	0x08
#define GPIO_PDIS	0x0c
#define GPIO_DIR		0x10
#define GPIO_DIRS	0x14
#define GPIO_DIRC	0x18
#define GPIO_SLPM	0x1c
#define GPIO_AFSLA	0x20
#define GPIO_AFSLB	0x24

#endif /* __GPIO_H__ */
