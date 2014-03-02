/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef COMMON_BITSTREAM_H
#define COMMON_BITSTREAM_H

#include "types.h"

#define EXPGOLOMB_UE          0
#define EXPGOLOMB_SE          1
#define EXPGOLOMB_ME_INTRA4   2
#define EXPGOLOMB_ME_INTER    3


t_uint16 ShowBits(t_bit_buffer *p_b, t_sint16 n, t_uint32 *p_val);
t_uint16 FlushBits(t_bit_buffer *p_b, t_sint16 n);
t_uint16 GetExpGolomb(t_bit_buffer *p_b, t_uint16 mode, t_sint32 *p_val);

#endif
