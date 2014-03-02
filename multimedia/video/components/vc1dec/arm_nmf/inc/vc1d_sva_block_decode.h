/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_BLOCK_DECODE_H_
#define _VC1D_SVA_BLOCK_DECODE_H_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Const:                                                            
 *----------------------------------------------------------------------*/
extern XMEM const ts_vlc_entry C_vlc_sbp_4x4_pquant_1_4[];
extern XMEM const ts_vlc_entry C_vlc_sbp_4x4_pquant_5_12[];
extern XMEM const ts_vlc_entry C_vlc_sbp_4x4_pquant_13_31[];

extern XMEM const ts_vlc_entry C_vlc_blk_transform_type_pquant_1_4[];
extern XMEM const ts_vlc_entry C_vlc_blk_transform_type_pquant_5_12[];
extern XMEM const ts_vlc_entry C_vlc_blk_transform_type_pquant_13_31[];

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

void blk_decode_intra_block(tps_bitstream_buffer p_bitstream,
                            tps_picture_parameters p_pic_param,
                            tps_macro_blk p_mb,
                            t_vc1_block_id blk_id);
void   blk_decode_inter_block(tps_bitstream_buffer p_bitstream,
                              tps_picture_parameters p_pic_param,
                              tps_macro_blk p_mb,
                              t_sint16 blk_num,
                              t_bool is_first_coded);

#endif /* _VC1D_SVA_BLOCK_DECODE_H_ */
