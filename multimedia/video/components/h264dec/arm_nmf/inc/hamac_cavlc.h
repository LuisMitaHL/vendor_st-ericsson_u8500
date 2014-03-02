/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HAMAC_CAVLC_H
#define HAMAC_CAVLC_H

#include "types.h"
#include "common_bitstream.h"
#include "hamac_types.h"

#define LUMALEVEL 		0
#define CHROMA_DC	 	1
#define CHROMA_U 		2
#define CHROMA_V 		3
#define INTRA_DC		4
#define INTRA_AC		5


t_uint16 residual_block(t_uint16 code_num, t_bit_buffer *p_b, t_hamac_residual_info *p_hamac, t_sint16 *p_coeffLevel);

#endif
