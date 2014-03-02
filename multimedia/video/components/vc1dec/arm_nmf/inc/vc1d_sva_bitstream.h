/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VC1D_SVA_BITSTREAM_H_
#define _VC1D_SVA_BITSTREAM_H_

/*
 * Includes							       
 */
#include "vc1d_co_bitstream.h"  /* for types definitions */

/*
 * Functions
 */
void btst_enable_byte_stuffing_removal(tps_bitstream_buffer p_bitstream); 
void btst_disable_byte_stuffing_removal(tps_bitstream_buffer p_bitstream); 
void btst_init_from_host_address(tps_bitstream_buffer p_bitstream,
                                 t_address start_add,
                                 t_address end_add, 
                                 t_address pos, 
                                 t_uint32 offset);
#endif /* _VC1D_SVA_BITSTREAM_H_ */
