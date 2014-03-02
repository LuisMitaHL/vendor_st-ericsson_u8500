/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_MB_DECODE_H_
#define _VC1D_SVA_MB_DECODE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"
#include "vc1d_sva.h" /* vc1 sva specific types */


/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Const                                                            
 *----------------------------------------------------------------------*/
extern XMEM const ts_vlc_entry C_vlc_mb_transform_type_1_4[];
extern XMEM const ts_vlc_entry C_vlc_mb_transform_type_5_12[];
extern XMEM const ts_vlc_entry C_vlc_mb_transform_type_13_31[];


/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/


void mb_decode_i_mb(tps_bitstream_buffer p_bitstream,
                    tps_picture_parameters p_pic_param,
                    tps_macro_blk p_mb);


void mb_decode_p_mb(tps_bitstream_buffer p_bitstream,
                                 tps_sequence_parameters p_seq_param,
                                 tps_picture_parameters p_pic_param,
                                 tps_macro_blk p_mb);

void  mb_decode_b_mb(tps_bitstream_buffer p_bitstream,
                                 tps_sequence_parameters p_seq_param,
                                 tps_picture_parameters p_pic_param,
                                 tps_macro_blk p_mb);


#endif /* _VC1D_SVA_MB_DECODE_H_ */
