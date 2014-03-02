/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _T1XHV_HOST_INTERFACE_MPEG4DEC_H_
#define _T1XHV_HOST_INTERFACE_MPEG4DEC_H_

#ifdef __T1XHV_NMF_ENV

#include "inc/type.h"
#include <t1xhv_common.idt>         /* Include common data structure  */
// #include <t1xhv_vdc_mpeg4.idt>      /* And MPEG4 decoder specific ones. */
// #include <t1xhv_vec_mpeg4.idt>      /* And MPEG4 encoder specific ones. */
// #include <t1xhv_vec_h264.idt>       /* And H264 encoder specific ones. */
// #include <t1xhv_vec_jpeg.idt>       /* And JPEGenecoder specific ones. */
                                    /* and so on for all others codecs. */

#else 

#include "t1xhv_retarget.h"

/** \brief Structure for parameters FROM Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_in {
  t_ushort_value picture_coding_type;           /**<\brief Current pict I,P or B       */
    t_ushort_value quant;                         /**<\brief Quantization parameter      */
    t_ushort_value quant_type                   ; /**<\brief ASP-Selects method 1 (1) or 2 (0) inverse quantisation */
    t_ushort_value intra_quant_mat[64]          ; /**<\brief ASP-inverse intra quantisation matrix                  */
    t_ushort_value nonintra_quant_mat[64]       ; /**<\brief ASP-inverse non intra quantisation matrix              */
    t_ushort_value low_delay                    ; /**<\brief ASP-if 0 => B frames        */
    t_ushort_value interlaced                   ; /**<\brief ASP-if 1 => interlaced mode */
    t_ushort_value rounding_type;                 /**<\brief Rounding type               */
    t_ushort_value intra_dc_vlc_thr;              /**<\brief Threshold to consider DC as AC coeff   */
    t_ushort_value vop_fcode_forward;             /**<\brief Fcode to decode MV          */
    t_ushort_value vop_fcode_backward           ; /**<\brief ASP-Fcode to decode MV      */
    t_ushort_value frame_width;                   /**<\brief Nb of pixel per line        */
    t_ushort_value frame_height;                  /**<\brief Nb of line                  */
    t_ushort_value flag_short_header;             /**<\brief Short Header mode if =1     */
    t_ushort_value modulo_time_base             ; /**<\brief ASP-needed for TRB and TRD computation */
    t_ushort_value vop_time_increment           ; /**<\brief ASP-needed for TRB and TRD computation */
    t_ushort_value vop_time_increment_resolution; /**<\brief VOP time increment          */
    t_ushort_value resync_marker_disable;         /**<\brief Resync Marker Disable       */
    t_ushort_value data_partitioned;              /**<\brief Data Partitioned            */
    t_ushort_value reversible_vlc;                /**<\brief Reversible VLC              */
    t_ushort_value error_concealment_config;      /**<\brief Error Concealment MPEG4     */
    t_ushort_value flag_sorenson;                 /**<\brief Sorenson stream if = 1      */
    t_ulong_value flv_version;                   /**<\brief FLV version 0(H263) or 1    */
   /* t_ulong_value reserved_3; */                //samanta: disabled by samanta for quarter pel
#ifdef __ndk8500_a0__ 
    t_ulong_value quarter_sample;               /* quarter pel falg for Version 2*/
#endif //#ifdef __ndk8500_a0__

} ts_t1xhv_vdc_mpeg4_param_in, *tps_t1xhv_vdc_mpeg4_param_in;

/** \brief Structure for parameters FROM and TO Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_inout {

    t_ulong_value  reserved_1;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_2;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_3;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_4;                     /**<\brief Reserved 32            */

} ts_t1xhv_vdc_mpeg4_param_inout, *tps_t1xhv_vdc_mpeg4_param_inout;


/** \brief Structure for parameters TO Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_out {

    t_ushort_value error_type;             /**<\brief Return bitstream error type    */
    t_ushort_value picture_loss;           /**<\brief Picturee loss flags            */
    t_ushort_value slice_loss_first_mb[8]; /**<\brief Slice lost first macroblock    */
    t_ushort_value slice_loss_mb_num[8];   /**<\brief Slice loss MB number           */
    t_ushort_value concealed_mb_num;       /**<\brief NB of Concealed MacroBlock     */
    t_ushort_value concealed_vp_num;       /**<\brief NB of Concealed video packets  */
    t_ushort_value decoded_vp_num;         /**<\brief NB of video packets decoded    */

    t_ushort_value  reserved_1;             /**<\brief Reserved 32                    */    
    t_ulong_value  reserved_2;             /**<\brief Reserved 32                    */    

} ts_t1xhv_vdc_mpeg4_param_out, *tps_t1xhv_vdc_mpeg4_param_out;

#endif

#endif /* _T1XHV_HOST_INTERFACE_MPEG4DEC_H_ */

