/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VC1D_INTERFACE_H_
#define _VC1D_INTERFACE_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Typedef							       
 *----------------------------------------------------------------------*/

/** \brief Hamac decoder vc1 param in  
 *  \note Theses parameters are the decoder initialization metadata. See Annex J*/

typedef struct vdc_vc1_param_in
{
    t_uint32 frame_size;                /**< \brief size of the frame in bytes   */
    t_uint16 max_picture_width;         /**< \brief maximum width of the picture (Annex J HORIZ_SIZE)   */
    t_uint16 max_picture_height;        /**< \brief maximum height of the picture (Annex J VERT_SIZE)   */
    t_uint16 profile;                   /**< \brief profile: 0 == SIMPLE, 1 == MAIN   */
    t_uint16 quantizer;                 /**< \brief quantizer specifier (Annex J QUANTIZER)   */
    t_uint16 dquant;                    /**< \brief macro-bloc quantization (Annex J DQUANT)   */
    t_uint16 max_b_frames;              /**< \brief maximum number of consecutive b-frames (Annex J MAXBFRAMES)   */
    t_uint16 multires_coding_enabled;   /**< \brief multi resolution coding used (Annex J MULTIRES)   */
    t_uint16 extended_mv_enabled;       /**< \brief extended motion vectors used (Annex J EXTENDED_MV)   */
    t_uint16 overlap_transform_enabled; /**< \brief overlaping transform used (Annex J OVERLAP)   */
    t_uint16 syncmarker_enabled;        /**< \brief synchronisation markers used (Annex J SYNCMARKER)   */
    t_uint16 rangered_enabled;          /**< \brief range reduction used (Annex J RANGERED)   */
    t_uint16 frame_interpolation_enabled;       /**< \brief frame interpolation in picture header (Annex J FINTERPFLAG)   */
    t_uint16 variable_size_transform_enabled;   /**< \brief variable size inverse transform used (Annex J VSTRANSFORM)   */
    t_uint16 loop_filter_enabled;               /**< \brief in-the-loop filtering used (Annex J LOOPFILTER)   */
    t_uint16 fast_uvmc_enabled;                   /**< \brief fast chroma motion compensention (Annex J FASTUVMC)   */
    t_uint16 is_smpte_conformant;               /**< \brief flag stating that the stream is conformant to SMPTE (reset of MV history not done) default:TRUE   */
    t_uint32 padding2;
    t_uint32 padding3;
    t_uint32 padding4;    

} ts_t1xhv_vdc_vc1_param_in, *tps_t1xhv_vdc_vc1_param_in;

/** \brief Hamac decoder vc1 param in common (for full sequence)
 *  \note Theses parameters are the decoder initialization metadata. See Annex J*/
typedef struct vdc_vc1_param_in_common
{
    t_uint16   	max_picture_width;  /**  picture width */
    t_uint16    	max_picture_height; /**  picture height */
    
    t_uint16      profile;                   /** See standard */
    t_uint16      level;                     /** See standard */

    t_uint16   	quantizer;                 /** See standard */
    t_uint16      dquant;                     /** See standard */
    t_uint16      max_b_frames;                 /** See standard */
    t_uint16      q_framerate_for_postproc;      /** See standard */
    t_uint16      q_bitrate_for_postproc;        /** See standard */

    t_uint16      loop_filter_enabled;                 /** See standard */
    t_uint16      multires_coding_enabled;             /** See standard */
    t_uint16      fast_uvmc_enabled;                   /** See standard */
    t_uint16      extended_mv_enabled;                 /** See standard */
    t_uint16      variable_size_transform_enabled;                /** See standard */
    t_uint16      overlap_transform_enabled;    /** See standard */
    t_uint16      syncmarker_enabled;             /** See standard */
    t_uint16      rangered_enabled;               /** See standard */
    t_uint16      frame_interpolation_enabled;     /** See standard */

} ts_t1xhv_vdc_vc1_param_in_common, *tps_t1xhv_vdc_vc1_param_in_common;


/** \brief Hamac video vc1 decode output parameters 
 *  \note These parameters are picture layer parameters needed for post-processing*/ 
typedef struct vdc_vc1_param_out 
{
    t_uint16 error_type;          /**<\brief Error status */
    t_uint16 frame_interpolation_hint_enabled;  /**< \brief picture layer frame interpolation hint set (INTERPFRM)   */
    t_uint16 range_reduction_frame_enabled;     /**< \brief picture layer frame rangered flag  (RANGEREDFRM)   */
    t_uint16 b_fraction_numerator;              /**< \brief picture layer b fraction numerator (BFRACTION)  */
    t_uint16 b_fraction_denominator;            /**< \brief picture layer b fraction denominatror (BFRACTION)  */
    t_uint16 buffer_fullness;                   /**< \brief picture layer buffer fullness (BF)   */
    t_uint16 picture_res;                       /**< \brief picture resolution: 1x1 == 0 2x1 == 1,1x2 == 2, 2x2 = 3  */
    t_uint16 max_picture_width;                 /**< \brief maximum width of the picture (Annex J HORIZ_SIZE)   */
    t_uint16 max_picture_height;                /**< \brief maximum height of the picture (Annex J VERT_SIZE)   */
    t_uint16 picture_width;                     /**< \brief true width of the decoded picture (including res)   */
    t_uint16 picture_height;                    /**< \brief true height of the decoded picture   */
    t_uint16 picture_type;                      /**< \brief picture type: I==0, P==1,B==2,BI==3,SKIPPED==4   */
    t_uint32 padding1;
    t_uint32 padding2;
    

} ts_t1xhv_vdc_vc1_param_out, *tps_t1xhv_vdc_vc1_param_out;



/** \brief Hamac video vc1 decode in / out parameters */ 
typedef struct vdc_vc1_param_inout
{
    t_uint16 intensity_compensate_enabled;    /**< \brief true if last P frame decoded has intensity compensation set    */
    t_uint16 last_ref_rangered_enabled;                /**< \brief true if last reference decoded has range reduction  set    */
    t_uint16 previous_last_ref_rangered_enabled;                /**< \brief true if previous last reference decoded has range reduction  set    */
    t_uint16 last_ref_interpolation_hint_enabled; /**< \brief used to update output parameters of skipped images */
    t_uint16 last_ref_buffer_fullness;/**< \brief used to pass buffer fullness of last decoded picture to skipped pictures   */
    t_uint16 luma_scale;        /**< \brief LUMSCALE value of last P frame decoded   */
    t_uint16 luma_shift;        /**< \brief LUMSHIFT value of last P frame decoded   */
    t_uint16 rnd_ctrl;          /**< \brief RND control value (VC-1 8.3.7)   */
    t_uint16 reference_resolution;       /**< \brief reference picture resolution */
    t_uint16 padding1;
    t_uint32 padding2;

} ts_t1xhv_vdc_vc1_param_inout, *tps_t1xhv_vdc_vc1_param_inout;

/** \brief struct used to pass host buffer descriptors */
typedef struct interface_buffer
{
    t_address start_add;   /**< \brief byte address of first byte of buffer   */
    t_address end_add;     /**< \brief byte address of end of buffer   */
    t_uint32  size;        /**< \brief byte size   */

} ts_interface_buffer;

/** \brief struct used to pass position in host buffer descriptors */
typedef struct interface_position
{
    t_address add;   /**< \brief byte address of first byte of buffer   */
    t_uint32 offset;     /**< \brief offset of bit at add address   */

} ts_interface_position;


#endif /* _VC1D_INTERFACE_H_ */
