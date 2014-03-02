/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_BLOCK_INFO_H
#define HOST_BLOCK_INFO_H

#include "types.h"
#include "host_types.h"

void resetBlockInfo(t_seq_par *p_sp, t_dec_buff *p_buff);
t_uint16 NotDecoded(t_uint16 mbaddr, t_block_info *p_b_info);
		
#endif
