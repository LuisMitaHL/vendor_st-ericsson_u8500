/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
  * \file 	djpeg_hv_idct.h
 * \brief   Inverse DCT functions declaration
 * \author  ST-Ericsson
  *  This file declares the functions that implement Inverse Quantization and
 *  Inverse DCT with scaling by 1,1/2,1/4,1/8.
 */
/*****************************************************************************/



#ifndef _DJPEG_HV_IDCT_H_
#define _DJPEG_HV_IDCT_H_
/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_hamac.h" /* for definition of tps_block and tps_mcus_set */
#include "djpeg_hv_postprocessing.h"

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
/** \brief Dequantize a coefficient by multiplying it by the multiplier-table
 * entry   */
#define DEQUANTIZE(coef,quantval)  ((coef) * (quantval))
 
/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/

void iq_idct_set_rom (void);

/* void iq_jpegdec_idct_hamac_8x8 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_jpegdec_idct_hamac_4x4 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_jpegdec_idct_hamac_2x2 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_jpegdec_idct_hamac_1x1 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */

/* void iq_idct_mmdsp_8x8 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_idct_mmdsp_4x4 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_idct_mmdsp_2x2 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
/* void iq_idct_mmdsp_1x1 (ts_hamac_info *p_hamac_info,t_sint16 comp,t_sint16 blk_num); */
//void jpegdec_idct_hamac (tps_block p_block, tps_mcus_set p_mcus_set, tps_post_process_info p_pp_info );

void jpegdec_idct_hamac (tps_block p_block, t_uint8 *idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_r2 (tps_block p_block, t_uint8 *idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_r4 (tps_block p_block, t_uint8 *idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_r8 (tps_block p_block, t_uint8 *idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);

void jpegdec_idct_hamac_opt (tps_block p_block,t_uint8 *p_idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_opt_r2 (tps_block p_block,t_uint8 *p_idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_opt_r4 (tps_block p_block,t_uint8 *p_idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);
void jpegdec_idct_hamac_opt_r8(tps_block p_block,t_uint8 *p_idct_array, tps_post_process_info p_pp_info,t_uint8 * p_dest,t_uint32 offset);


#endif /* _DJPEG_HV_IDCT_H_ */
