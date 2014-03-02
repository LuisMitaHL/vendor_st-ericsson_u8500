/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	u32;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*
 * Define NULL pointer value.
 */
#ifndef NULL
#define NULL  (void *) 0
#endif

#endif /* __TYPES_H__ */

