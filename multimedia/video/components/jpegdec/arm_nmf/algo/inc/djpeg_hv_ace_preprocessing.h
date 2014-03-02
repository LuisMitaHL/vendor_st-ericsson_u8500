/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_hv_ace_preprocessing.h
 * \brief 	ACE pre-processing functions declaration
 * \author  ST-Ericsson
 *  This file declares the pre-processing functions for the ACE algorithm
 */
/*****************************************************************************/


#ifndef _DJPEG_HV_ACE_PREPROCESSING_H_
#define _DJPEG_HV_ACE_PREPROCESSING_H_
/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_hamac.h"

#ifdef __NMF
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
//#include "t1xhv_vdc_jpeg.idt"
#include "t1xhv_common.idt"
#include "t1xhv_vdc.idt"
#else
#include "djpeg_interface.h"
#include "djpeg_global.h"
#endif

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/

#define OPTIMAL_GRAY_LEVEL       (125)   /**< \brief optimal gray level value for ACE   */
#define ACE_OFFSET_CLIP_MIN      (-2477) /**< \brief lower boundary for OFFSET clipping (LL)   */
#define ACE_OFFSET_CLIP_MAX      (5405)  /**< \brief upper boundary for OFFSET clipping (HL)   */

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
void jpegdec_ace_init(tps_frame p_frame,
              tps_analysis p_analysis, 
              t_uint16 y_h_sampling_factor,
              t_uint16 y_v_sampling_factor, 
              tps_ace_info p_ace_info);

void jpegdec_ace_accumulate_dc (t_sint16 dc,
                        t_uint16 mcu_index_in_frame,
                        t_uint16 block_index_in_mcu,
                        tps_ace_info p_ace_info);
void jpegdec_ace_compute_offset(tps_ace_info p_ace_info);

/* void accumulate_ACE_dc(t_sint16 dc_value, */
/* 		    t_uint16 blk_row, */
/* 		    t_uint16 blk_col, */
/* 		    tps_frame p_frame, */
/* 		    ts_component* p_y, */
/* 		    ts_analysis* p_analysis); */
		    
/* void compute_ACE_offset(tps_frame p_frame, */
/* 		   ts_component* p_y, */
/* 		   ts_analysis* p_analysis); */

#endif /* _DJPEG_HV_ACE_PREPROCESSING_H_ */
