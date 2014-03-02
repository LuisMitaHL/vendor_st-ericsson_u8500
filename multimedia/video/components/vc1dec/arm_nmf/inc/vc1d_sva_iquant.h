/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VC1D_SVA_IQUANT_H_
#define _VC1D_SVA_IQUANT_H_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

#ifdef DSP_BUILD
INLINE  static t_sint16 AC_IQ_UNIFORM(t_sint16 coeff ,t_sint16 double_quant)
{
    /* On the DSP the coeff are <<4 prior to fixed point inverse transform */
    return (coeff * double_quant) << 4;

}

INLINE  static t_sint16 AC_IQ_NOT_UNIFORM(t_sint16 coeff ,t_sint16 double_quant, t_sint16 mquant)
{
    /* On the DSP the coeff are <<4 prior to fixed point inverse transform */
    return (coeff * double_quant + (coeff >0 ? mquant:-mquant)) << 4;

}
#else /* GCC */
INLINE  t_sint16 AC_IQ_UNIFORM(t_sint16 coeff ,t_sint16 double_quant);
INLINE  t_sint16 AC_IQ_NOT_UNIFORM(t_sint16 coeff ,t_sint16 double_quant, t_sint16 mquant);

#endif
void iq_init_i_mb_quantizer(tps_picture_quant p_pict_quant,tps_macro_blk p_mb);
void iq_init_pb_mb_quantizer(tps_dimension p_dimension,tps_picture_quant p_pict_quant, tps_macro_blk p_mb);


void iq_finish_intra_block_iq(tps_macro_blk p_mb, 
                              t_sint16 p_blk_noiq[],
                              t_sint16 p_blk_iqued[],
                              t_vc1_intra_pred_dir pred_dir,
                              t_sint16 blk_num);

void iq_iquant_intra_block(tps_blk p_blk, tps_mb_quant p_mb_quant);
void iq_iquant_inter_block(tps_blk p_blk, tps_mb_quant p_mb_quant);


#endif /* _VC1D_SVA_IQUANT_H_ */
