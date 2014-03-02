/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _T1XHV_HOST_INTERFACE_VC1DEC_H_
#define _T1XHV_HOST_INTERFACE_VC1DEC_H_

#include "t1xhv_retarget.h"

typedef struct t1xhv_vdc_vc1_param_in {
    t_ulong_value  frame_size;                /**< \brief size of the frame in bytes   */
    t_ushort_value max_picture_width;         /**< \brief maximum width of the picture (Annex J HORIZ_SIZE)   */
    t_ushort_value max_picture_height;        /**< \brief maximum height of the picture (Annex J VERT_SIZE)   */
    t_ushort_value profile;                   /**< \brief profile: 0 == SIMPLE, 1 == MAIN   */
    t_ushort_value quantizer;                 /**< \brief quantizer specifier (Annex J QUANTIZER)   */
    t_ushort_value dquant;                    /**< \brief macro-bloc quantization (Annex J DQUANT)   */
    t_ushort_value max_b_frames;              /**< \brief maximum number of consecutive b-frames (Annex J MAXBFRAMES)   */
    t_ushort_value multires_coding_enabled;   /**< \brief multi resolution coding used (Annex J MULTIRES)   */
    t_ushort_value extended_mv_enabled;       /**< \brief extended motion vectors used (Annex J EXTENDED_MV)   */
    t_ushort_value overlap_transform_enabled; /**< \brief overlaping transform used (Annex J OVERLAP)   */
    t_ushort_value syncmarker_enabled;        /**< \brief synchronisation markers used (Annex J SYNCMARKER)   */
    t_ushort_value rangered_enabled;          /**< \brief range reduction used (Annex J RANGERED)   */
    t_ushort_value frame_interpolation_enabled;       /**< \brief frame interpolation in picture header (Annex J FINTERPFLAG)   */
    t_ushort_value variable_size_transform_enabled;   /**< \brief variable size inverse transform used (Annex J VSTRANSFORM)   */
    t_ushort_value loop_filter_enabled;               /**< \brief in-the-loop filtering used (Annex J LOOPFILTER)   */
    t_ushort_value fast_uvmc_enabled;                   /**< \brief fast chroma motion compensention (Annex J FASTUVMC)   */
    t_ushort_value is_smpte_conformant;                 /**< \brief flag stating that the stream is conformant to SMPTE (reset of MV history not done) default:TRUE   */
    t_ulong_value  padding2;
    t_ulong_value  padding3;
    t_ulong_value  padding4;    

} ts_t1xhv_vdc_vc1_param_in, *tps_t1xhv_vdc_vc1_param_in;


/** \brief Hamac video vc1 decode output parameters 
 *  \note These parameters are picture layer parameters needed for post-processing*/ 
typedef struct t1xhv_vdc_vc1_param_out {
    t_ushort_value error_type;          /**<\brief Error status */
    t_ushort_value frame_interpolation_hint_enabled;  /**< \brief picture layer frame interpolation hint set (INTERPFRM)   */
    t_ushort_value range_reduction_frame_enabled;     /**< \brief picture layer frame rangered flag  (RANGEREDFRM)   */
    t_ushort_value b_fraction_numerator;              /**< \brief picture layer b fraction numerator (BFRACTION)  */
    t_ushort_value b_fraction_denominator;            /**< \brief picture layer b fraction denominatror (BFRACTION)  */
    t_ushort_value buffer_fullness;                   /**< \brief picture layer buffer fullness (BF)   */
    t_ushort_value picture_res;                       /**< \brief picture resolution: 1x1 == 0 2x1 == 1,1x2 == 2, 2x2 = 3  */
    t_ushort_value max_picture_width;                     /**< \brief true width of the decoded picture (including res)   */
    t_ushort_value max_picture_height;                    /**< \brief true height of the decoded picture   */
    t_ushort_value picture_width;                     /**< \brief true width of the decoded picture (including res)   */
    t_ushort_value picture_height;                    /**< \brief true height of the decoded picture   */
    t_ushort_value picture_type;                      /**< \brief picture type: I==0, P==1,B==2,BI==3,SKIPPED==4   */
    t_ulong_value  padding1;                        /**< \brief  Reserved 32                              */
    t_ulong_value  padding2;                        /**< \brief  Reserved 32                              */

} ts_t1xhv_vdc_vc1_param_out, *tps_t1xhv_vdc_vc1_param_out;

/** \brief Hamac video vc1 decode in / out parameters */ 
typedef struct t1xhv_vdc_vc1_param_inout {
    t_ushort_value intensity_compensate_enabled;             /**< \brief true if last P frame decoded has intensity compensation set    */
    t_ushort_value last_ref_rangered_enabled;                /**< \brief true if last reference decoded has range reduction  set    */
    t_ushort_value previous_last_ref_rangered_enabled;       /**< \brief true if previous last reference decoded has range reduction  set    */
    t_ushort_value last_ref_interpolation_hint_enabled;      /**< \brief used to update output parameters of skipped images */
    t_ushort_value last_ref_buffer_fullness;/**< \brief used to pass buffer fullness of last decoded picture to skipped pictures   */
    t_ushort_value luma_scale;                               /**< \brief LUMSCALE value of last P frame decoded   */
    t_ushort_value luma_shift;                               /**< \brief LUMSHIFT value of last P frame decoded   */
    t_ushort_value rnd_ctrl;                                 /**< \brief RND control value (VC-1 8.3.7)   */
    t_ushort_value reference_resolution;                     /**< \brief reference picture resolution same type than output param*/          
    t_ushort_value padding1;
    t_ulong_value  padding2;
    t_ulong_value  padding3;
    t_ulong_value  padding4;

} ts_t1xhv_vdc_vc1_param_inout, *tps_t1xhv_vdc_vc1_param_inout;

#endif /* _T1XHV_HOST_INTERFACE_VC1DEC_H_ */

