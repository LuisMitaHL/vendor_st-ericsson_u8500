/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef TYPES_H
#define TYPES_H

#include <limits.h>

#ifndef NULL
#  define NULL    0
#endif

typedef  unsigned char       t_uint8; /**< \brief unsigned 8 bits type   */
typedef    signed char       t_sint8; /**< \brief signed 8 bits type   */

typedef  unsigned short     t_uint16; /**< \brief unsigned 16 bits type   */
typedef    signed short     t_sint16; /**< \brief signed 16 bits type   */

typedef  unsigned long int  t_uint32; /**< \brief unsigned 32 bits type   */
typedef    signed long int  t_sint32; /**< \brief signed 32 bits type   */
typedef  t_sint16              T_BOOL; /**< \brief boolean type   */

typedef t_uint32              t_address; /**< \brief used to exchange addresses between hamac and host   */

#ifndef __NMF
typedef t_uint32 t_ulong_value;
typedef t_sint32 t_long_value;
typedef t_uint16 t_ushort_value;
typedef t_sint16 t_short_value;
typedef t_uint32 t_ahb_address;
typedef t_long_value t_time_stamp;
#endif

#define ON      1
#define OFF     0
#endif  /* TYPES_H */
