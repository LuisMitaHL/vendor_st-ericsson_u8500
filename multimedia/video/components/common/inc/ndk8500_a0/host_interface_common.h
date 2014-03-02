/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
 * Structure used for Host Interface
 * Are defined : 
 * \arg parameters for tasks
 * \arg parameters for buffer
 * Types must be defined in t1xhv_retarget.h with following convention
 * \arg t_ulong_value  = 32 bit unsigned integer (Little endian for 16 bit word)
 * \arg t_long_value   = 32 bit signed integer (Little endian for 16 bit word)
 * \arg t_ushort_value = 16 bit unsigned integer
 * \arg t_short_value  = 16 bit signed integer
 * \arg t_ahb_address  = 32 bit unsigned integer to define AHB address
 * \arg t_time stamp   = 32 bit unsigned integer to define a time stamp
 */
/*****************************************************************************/

#ifndef _T1XHV_HOST_INTERFACE_COMMON_H_
#define _T1XHV_HOST_INTERFACE_COMMON_H_

/*
 * Includes       
 */

#ifdef __T1XHV_NMF_ENV

#include "inc/type.h"
#include <t1xhv_common.idt>         /* Include common data structure  */
// #include <t1xhv_vdc_mpeg4.idt>      /* And MPEG4 decoder specific ones. */
// #include <t1xhv_vec_mpeg4.idt>      /* And MPEG4 encoder specific ones. */
// #include <t1xhv_vec_h264.idt>       /* And H264 encoder specific ones. */
// #include <t1xhv_vec_jpeg.idt>       /* And JPEGenecoder specific ones. */
                                    /* and so on for all others codecs. */

#else /* __T1XHV_NMF_ENV */

#include "t1xhv_retarget.h"

/*
 * Types
 */

/*****************************************************************************/
/**
 * \brief  Parameter common structure
 * \author Philippe Rochette
 *
 * Parameter common structure
 */
/*****************************************************************************/

/** 
 * \brief This structure define one link to a subtask description. 
 */
typedef struct t1xhv_subtask_link {

    t_ahb_address addr;           /**<\brief Add. of associated subtask */
    t_ulong_value type;           /**<\brief Define type for subtask */
    t_time_stamp  execution_time_stamp; /**<\brief Define time to start subtask */
    t_ulong_value dependency;     /**<\brief Coded depend. between tasks */

} ts_t1xhv_subtask_link, *tps_t1xhv_subtask_link;


/** 
 * \brief This structure define Parameters in Memory for Subtask parameters. 
 */
typedef struct t1xhv_subtask_descriptor {

    ts_t1xhv_subtask_link s_next_subtask;      /**<\brief Link to next subtask 
                                                * -- ts_t1xhv_subtask_link      */
    ts_t1xhv_subtask_link s_current_subtask;   /**<\brief Link to current subtask  
                                                * -- ts_t1xhv_subtask_link      */
    ts_t1xhv_subtask_link s_interrupt_subtask; /**<\brief Link to interrupt subtask  
                                                * -- ts_t1xhv_subtask_link      */
    t_ulong_value         task_count;          /**<\brief Task counter (0 if no 
                                                *          more task) 
                                                */

} ts_t1xhv_subtask_descriptor, *tps_t1xhv_subtask_descriptor;


/** \brief This structure define Parameters needed to For begin/end of buffer. */
typedef struct t1xhv_bitstream_buf_pos {

    t_ahb_address   addr_bitstream_buf_struct; /**<\brief Choose buffer structure            */
    t_ahb_address   addr_bitstream_start;      /**<\brief Bitstream Start add. inside buffer */
    t_ulong_value   bitstream_offset;          /**<\brief Bitstream offset in bits           */
    t_ulong_value   reserved_1;                /**<\brief Reserved 32                        */

} ts_t1xhv_bitstream_buf_pos, *tps_t1xhv_bitstream_buf_pos;


/** \brief This structure define a bitstream buffer. */
typedef struct t1xhv_bitstream_buf {

    t_ahb_address addr_buffer_start; /**<\brief Buffer start                 */
    t_ahb_address addr_buffer_end;   /**<\brief Buffer end                   */
    t_ahb_address addr_window_start; /**<\brief Window start (inside buffer) */
    t_ahb_address addr_window_end;   /**<\brief Windows end  (inside buffer) */

} ts_t1xhv_bitstream_buf, *tps_t1xhv_bitstream_buf;


/** \brief This structure define a link buffer. */
typedef struct t1xhv_bitstream_buf_link {

    t_ahb_address addr_next_buf_link;   /**<\brief Address next structure */
    t_ahb_address addr_prev_buf_link;   /**<\brief Address prev structure */
    t_ahb_address addr_buffer_start;    /**<\brief Bitstream buffer start */
    t_ahb_address addr_buffer_end;      /**<\brief Bitstream buffer end   */

} ts_t1xhv_bitstream_buf_link, *tps_t1xhv_bitstream_buf_link;


/** \brief This structure define an header buffer. */
typedef struct t1xhv_header_buf {

    t_ahb_address  addr_header_buffer;  /**<\brief Start add. of the header buffer */
    t_ulong_value  header_size;         /**<\brief Header size                     */
    t_ahb_address  reserved_1;          /**<\brief Reserved 32                     */
    t_ahb_address  reserved_2;          /**<\brief Reserved 32                     */

} ts_t1xhv_header_buf, *tps_t1xhv_header_buf;

/*****************************************************************************/
/**
 * \brief  Parameter structure decode
 * \author Philippe Rochette
 *
 * Parameter structure for decode H264, MPEG4, JPEG and H263. 
 * Hamac Video Spec v0.1 sections 6.5 and 13.2
 */
/*****************************************************************************/

/** \brief This structure define description of a subtask decode. */
typedef struct t1xhv_vdc_subtask_param {

    ts_t1xhv_subtask_link s_link;                        /**<\brief Link to next subtask (chained list) 
                                                          *             -- ts_t1xhv_subtask_link
                                                          */
    t_ahb_address         addr_in_frame_buffer;          /**<\brief Add. of struct for input frame buffer
                                                          *             -- ts_t1xhv_vdc_frame_buffer_in
                                                          */
    t_ahb_address         addr_out_frame_buffer;         /**<\brief Add. of struct for output frame buffer 
                                                          *             -- ts_t1xhv_vdc_frame_buffer_out
                                                          */
    t_ahb_address         addr_internal_buffer;          /**<\brief  Add. of struct for internal buffer 
                                                          *              -- ts_t1xhv_vdc_internal_buf
                                                          */
    t_ahb_address         addr_in_bitstream_buffer;      /**<\brief Add. of struct for in bitstream buffer 
                                                          *         -- ts_t1xhv_bitstream_buffer
                                                          */
    t_ahb_address         addr_out_bitstream_buffer;     /**<\brief Add. of struct for out bitstream buffer
                                                          *          -- ts_t1xhv_bitstream_buffer_position
                                                          */
    t_ahb_address         addr_in_parameters;            /**<\brief Add. of struct for input parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_out_parameters;           /**<\brief Add. of struct for output parameters
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_in_frame_parameters;      /**<\brief Add. of struct for input frame parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_out_frame_parameters;     /**<\brief Add. of struct for output frame parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_vpp_dummy;                /**<\brief Reserved 32                          */
    t_ahb_address         addr_dma_cup_context;          /**<\brief Reserved 32                          */
    t_ahb_address         reserved_3;                    /**<\brief Reserved 32                          */

} ts_t1xhv_vdc_subtask_param, *tps_t1xhv_vdc_subtask_param;



/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vdc_frame_buf_in {

    t_ahb_address   addr_fwd_ref_buffer;  /**<\brief Address of Forward reference buffer. */
    t_ahb_address   addr_bwd_ref_buffer; /**<\brief Address of backward reference buffer */
    t_ahb_address  addr_source_buffer;                      /*Source buffer start address for deblocking */
    t_ahb_address  addr_deblocking_param_buffer_in;    /*Deblocking parameters will used  as a input of deblocking */
} ts_t1xhv_vdc_frame_buf_in, *tps_t1xhv_vdc_frame_buf_in;


/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vdc_internal_buf {

    t_ahb_address   addr_vpp_dummy_buffer;     /**<\brief Address of temporary buffer used by VPP. */
    t_ahb_address   reserved32_0;              /**<\brief Address of block_info in ED              */
    t_ahb_address   addr_h264d_mb_slice_map;   /**<\brief Address of mb_slice_map                           */
    t_ahb_address   addr_mv_history_buffer;                /**<\brief VC1 motion vector history buffer (for B framesdecoding)    */
    t_ahb_address   addr_mv_type_buffer;     /*  This buffer is used for mpeg4 decoder to keep information of current MB is of 1MV of 4MV */   
    t_ahb_address   addr_mb_not_coded;        /* This buffer is used for mpeg4 decoder to keep information of the current MB is coded ot not-coded*/         
    t_ahb_address   addr_x_err_res_buffer;   /* This buffer is used for mpeg4 decoder to keep MV dx of the current MB that will be used for error concealment*/
    t_ahb_address   addr_y_err_res_buffer;   /* This buffer is used for mpeg4 decoder to keep MV dy of the current MB that will be used for error concealment*/

} ts_t1xhv_vdc_internal_buf, *tps_t1xhv_vdc_internal_buf;

/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vdc_frame_buf_out {

    t_ahb_address   addr_dest_buffer;             /**<\brief Address of output frame buffer.   */
    t_ahb_address   addr_deblocking_param_buffer; /**<\brief Address of parameters for PPP.    */
    t_ahb_address   addr_motion_vector_buffer;    /**<\brief Start add of motion vector buffer */
    t_ahb_address   addr_jpeg_coef_buffer;        /**<\brief Start address of JPEG Coef buffer */
    t_ahb_address   addr_jpeg_line_buffer;        /**<\brief Start address of JPEG line buffer */
    t_ahb_address   addr_dest_local_rec_buffer;   /**<\brief Address of reconstructed local buffer */
    t_ahb_address   addr_dest_buffer_deblock;       /*Destination buffer start address for deblocking */
    t_ulong_value   reserved_3;                   /**<\brief Reserved 32                       */

} ts_t1xhv_vdc_frame_buf_out, *tps_t1xhv_vdc_frame_buf_out;

/** @{ \name enable_annexes parameter bitfield definition  
 *     \author Jean-Marc Volle
 *     \note Spec V0.95 p348
 */

/** \brief Enable Annex D.1:
 * As an input,EAD allows to enable the annex D.1
 * (motion vectors over picture boundaries)for an H263 decode
 * subtask.It is not used if picture_coding_type=0.In the profiles
 * that are currently supported,it must be equal to EAJ,otherwise
 * error_type is set to 0xc4. As an output,EAD returns the annex D.1
 * enable  ag for the next frame found in the bitstream,if enable_scd=1
 * (if enable_scd=0,this  eld is unde  ned).It is equal to the EAJ
 * output. 0 =annex disabled 1 =annex enabled   
 */
#define ENABLE_ANNEXES_EAD 0x0001 

/** \brief Enable Annex F.2:
 * As an input,EAF allows to enable the annex F.2 (four motion 
 * vectors per macroblock)for an H263 decode subtask.It is not 
 * used if picture_coding_type=0.In the pro  les that are currently
 * supported,it must be equal to EAJ,otherwise error_type is set to 
 * 0xc5. As an output,EAF returns the annex F.2 enable  ag for the 
 * next frame found in the bitstream,if enable_scd=1 (if enable_scd=0,
 * this  field is undefined).It is equal to the EAJ output.
 */
#define ENABLE_ANNEXES_EAF 0x0002 

/** \brief Enable Annex I:  
 * As an input,EAI allows to enable the annex I (advanced intra coding)
 * for an H263 decode subtask. As an output,EAI returns the annex I 
 * enable flag for the next frame found in the bitstream,if enable_scd=1
 * (if enable_scd=0,this field is undefined).It is obtained from the 
 * OPPTYPE  eld of the H263 bitstream.
 */
#define ENABLE_ANNEXES_EAI 0x0004 

/** \brief Enable Annex J:
 * As an input,EAJ allows to enable the annex J (deblocking  lter)for
 * an H263 decode subtask. As an output,EAJ returns the annex J enable
 * flag for the next frame found in the bitstream,if enable_scd=1 
 * (if enable_scd=0,this  eld is undefined).It is obtained from the 
 * OPPTYPE  eld of the H263 bitstream.
 */
#define ENABLE_ANNEXES_EAJ 0x0008 

/** \brief Enable Annex K:
 * As an input,EAK allows to enable the annex K (slice structured 
 * coding,with- out submodes)for an H263 decode subtask. As an output,
 * EAK returns the annex K enable flag for the next frame found in the
 * bitstream,if enable_scd=1 (if enable_scd=0,this  eld is undefined).
 * It is obtained from the OPPTYPE  eld of the H263 bitstream.   
 */
#define ENABLE_ANNEXES_EAK 0x0010 

/** \brief Enable Annex T:
 * As an input,EAT allows to enable the annex T (modi  ed quantization)
 * for an H263 decode subtask. As an output,EAT returns the annex T enable
 * flag for the next frame found in the bitstream,if enable_scd=1 
 * (if enable_scd=0,this  eld is undefined).It is obtained from 
 * the OPPTYPE  filed of the H263 bitstream.   */
#define ENABLE_ANNEXES_EAT 0x0020 
/** @}end of enable_annexes parameter bitfield definition*/



/*****************************************************************************/
/**
 * \brief  Parameter structure encode
 * \author Philippe Rochette
 *
 * Parameter structure for encode. Hamac Video Spec v0.1 sections 7.6
 **/
/*****************************************************************************/

/** \brief This structure define description of a subtask encode. */
typedef struct t1xhv_vec_subtask_param {

    ts_t1xhv_subtask_link s_link;                        /**<\brief  Link to next subtask (chained list) 
                                                          *              -- ts_t1xhv_subtask_link
                                                          */
    t_ahb_address         addr_in_frame_buffer;          /**<\brief  Add. of struct for input frame buffer
                                                          *              -- ts_t1xhv_vec_frame_buffer_in
                                                          */
    t_ahb_address         addr_out_frame_buffer;         /**<\brief  Add. of struct for output frame buffer 
                                                          *              -- ts_t1xhv_vec_frame_buffer_out
                                                          */
    t_ahb_address         addr_internal_buffer;          /**<\brief  Add. of struct for internal buffer 
                                                          *              -- ts_t1xhv_vec_internal_buf
                                                          */
    t_ahb_address         addr_in_header_buffer;         /**<\brief  Add. of struct for header buffer 
                                                          *              -- ts_t1xhv_bitstream_buf_header
                                                          */
    t_ahb_address         addr_in_bitstream_buffer;      /**<\brief  Add. of struct for in bitstr. buffer 
                                                          *              -- ts_t1xhv_init_bitstream_buffer
                                                          */
    t_ahb_address         addr_out_bitstream_buffer;     /**<\brief  Add. of struct for output bitstream buffer
                                                          *              -- ts_t1xhv_bitstream_buffer
                                                          */
    t_ahb_address         addr_in_parameters;            /**<\brief  Add. of struct for input parameters 
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_out_parameters;           /**<\brief  Add. of struct for output parameters 
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_in_frame_parameters;      /**<\brief  Add. of struct for inout parameters
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_out_frame_parameters;     /**<\brief  Add. of struct for inout parameters
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         reserved_1;                    /**<\brief Reserved 32                            */
    t_ahb_address         reserved_2;                    /**<\brief Reserved 32                            */

} ts_t1xhv_vec_subtask_param, *tps_t1xhv_vec_subtask_param;


/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vec_frame_buf_in {

    t_ahb_address  addr_source_buffer;        /**<\brief Buffer to encode.                    */
    t_ahb_address  addr_fwd_ref_buffer;       /**<\brief Address of prev reconstructed buffer */
    t_ahb_address  addr_grab_ref_buffer;      /**<\brief Address of buffer from grab          */
    t_ahb_address  addr_intra_refresh_buffer; /**<\brief Add. of intra refresh buffer         */

} ts_t1xhv_vec_frame_buf_in, *tps_t1xhv_vec_frame_buf_in;


/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vec_frame_buf_out {

    t_ahb_address  addr_dest_buffer;             /**<\brief Add. of output frame buffer  */
    t_ahb_address  addr_deblocking_param_buffer; /**<\brief Add. of parameters for PPP   */
    t_ahb_address  addr_motion_vector_buffer;    /**<\brief Add. of motion vector        */
    t_ahb_address  addr_intra_refresh_buffer;    /**<\brief Add. of intra refresh buffer */
    t_ahb_address  addr_ime_mv_field_buffer;     /**<\brief Add. of MV field buffer of IME */
    t_ahb_address  addr_ime_mv_field_buffer_end;     /**<\brief Add. of MV field buffer of IME */    
    t_ahb_address  reserved_2;     /**<\brief Add. of MV field buffer of IME */    
    t_ahb_address  reserved_3;     /**<\brief Add. of MV field buffer of IME */   
} ts_t1xhv_vec_frame_buf_out, *tps_t1xhv_vec_frame_buf_out;


/** \brief This structure define an internal frame buffer. */
typedef struct t1xhv_vec_internal_buf {

    t_ahb_address  addr_search_window_buffer;  /**<\brief Start add. of buffer for Search Window */
    t_ahb_address  addr_search_window_end;     /**<\brief End add. of buffer for Search Window   */
    t_ahb_address  addr_jpeg_run_level_buffer; /**<\brief Start add. of JPEG run level buffer    */
    t_ahb_address  addr_h264e_H4D_buffer;      /**<\brief Address of temporary buffer used by H4D for H264. */
    t_ahb_address  addr_h264e_rec_local;       /**<\brief Address of temporary buffer used for reconstruction local buffer for H264. */
    t_ahb_address  addr_h264e_metrics;         /**<\brief Address of temporary buffer used for coding choice and metrics for H264. */
    t_ahb_address  addr_h264e_cup_context;     /**<\brief Add for context of CUP HW block                   */
    t_ahb_address  reserved_1;                 /**<\brief Reserved 32  */
} ts_t1xhv_vec_internal_buf, *tps_t1xhv_vec_internal_buf;

/*****************************************************************************/
/**
 * \brief  Parameter structure for display
 * \author Jean-Marc Volle
 *
 * Parameter structure for display. Hamac Video Spec v0.1 
 */
/*****************************************************************************/
typedef struct t1xhv_dpl_subtask_param {
     
    ts_t1xhv_subtask_link s_link;            /**<\brief  Link to next subtask 
                                              * (chained list) same for all tasks
                                              * -- ts_t1xhv_subtask_link */
    t_ahb_address addr_in_frame_buffer;      /**<\brief  Address of structure for 
                                              * input frame buffer
                                              * -- ts_t1xhv_dpl_frame_buffer_in */
    t_ahb_address addr_out_frame_buffer;     /**<\brief  Address of structure for 
                                              * output frame buffer    
                                              * -- ts_t1xhv_dpl_frame_buffer_out */
    t_ahb_address addr_internal_buffer;      /**<\brief  Address of structure for 
                                              * internal buffer 
                                              * --  ts_t1xhv_dpl_interna_buf */
    t_ahb_address addr_in_parameters;        /**<\brief  Address of structure for 
                                              * input parameters 
                                              * -- ts_t1xhv_dpl_parameters_in */
    t_ahb_address addr_out_parameters;       /**<\brief  Address of structure for 
                                              * output parameters
                                              * -- ts_t1xhv_dpl_parameters_out */
    t_ahb_address addr_in_frame_parameters;  /**<\brief  Add. of struct for inout parameters
                                              * of display                     */
    t_ahb_address addr_out_frame_parameters; /**<\brief  Add. of struct for inout parameters
                                              * of display                     */

    t_ulong_value reserved_1;                /**<\brief reserved  32  */

} ts_t1xhv_dpl_subtask_param, *tps_t1xhv_dpl_subtask_param;


/**
 * \brief t1xhv_dpl_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_dpl_frame_buf_in {

    t_ahb_address  addr_source_buffer;           /**<\brief Source buffer start address    */
    t_ahb_address  addr_deblocking_param_buffer; /**<\brief Deblocing parameters
                                                  * buffer start address                   */
    t_ulong_value reserved_1;                   /**<\brief reserved 32                    */
    t_ulong_value reserved_2;                   /**<\brief reserved 32                    */

} ts_t1xhv_dpl_frame_buf_in, *tps_t1xhv_dpl_frame_buf_in;

/**
 * \brief t1xhv_dpl_frame_buf_out, pointed by the third field of the parameter 
 * structure, same as the one for decoder, necessary to duplicate ?
 */
typedef struct t1xhv_dpl_frame_buf_out {

    t_ahb_address  addr_dest_buffer; /**<\brief Destination buffer start address */
    t_ulong_value reserved_1;        /**<\brief reserved 32                      */
    t_ulong_value reserved_2;        /**<\brief reserved 32                      */
    t_ulong_value reserved_3;        /**<\brief reserved 32                      */

} ts_t1xhv_dpl_frame_buf_out, *tps_t1xhv_dpl_frame_buf_out;

/**
 * \brief t1xhv_dpl_internal_buf, pointed by the fourth field of the parameter 
 * structure, same as the one for decoder, necessary to duplicate ?
 */
typedef struct t1xhv_dpl_internal_buf {

    t_ahb_address  addr_temp_buffer; /**<\brief temporary buffer start address */
    t_ulong_value reserved_1;        /**<\brief reserved 32                      */
    t_ulong_value reserved_2;        /**<\brief reserved 32                      */
    t_ulong_value reserved_3;        /**<\brief reserved 32                      */

} ts_t1xhv_dpl_internal_buf, *tps_t1xhv_dpl_internal_buf;

/**
 * \brief t1xhv_dpl_parameters_in, pointed by the fifth field of the parameter 
 * structure. Read from Host
 */
typedef struct t1xhv_dpl_param_in {

    t_ushort_value source_frame_width;                   /**<\brief YCbCr input pict width in pixels  */
    t_ushort_value source_frame_height;                  /**<\brief YCbCr input pict height in pixels */
    t_ushort_value output_format;                       /**<\brief Output format */
    t_ushort_value reserved;
  
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;

} ts_t1xhv_dpl_param_in, *tps_t1xhv_dpl_param_in;

/**
 * \brief t1xhv_dpl_parameters_out, pointed by the sixth field of the parameter 
 * structure.
 */
typedef struct t1xhv_dpl_param_out {

    t_ushort_value error_type;  /**<\brief Error type    */
    t_ushort_value reserved_1;  /**<\brief reserved  16  */
    t_ulong_value  reserved_2;  /**<\brief reserved  32  */
    t_ulong_value  reserved_3;  /**<\brief reserved  32  */
    t_ulong_value  reserved_4;  /**<\brief reserved  32  */

} ts_t1xhv_dpl_param_out, *tps_t1xhv_dpl_param_out;

typedef struct t1xhv_dpl_param_inout {
    t_ulong_value  reserved_1;    /**<\brief reserved  32        */
    t_ulong_value  reserved_2;    /**<\brief reserved  32        */
    t_ulong_value  reserved_3;    /**<\brief reserved  32        */
    t_ulong_value  reserved_4;    /**<\brief reserved  32        */
} ts_t1xhv_dpl_param_inout, *tps_t1xhv_dpl_param_inout;

/*****************************************************************************/
/**
 * \brief  Parameter structure for image stabilization
 * \author Serge Backert
 *
 * Parameter structure for image stabilization
 */
/*****************************************************************************/

/** \brief Structure for parameters FROM Host for an image stab. encode task */
typedef struct t1xhv_vec_stab_param_in {

    t_ushort_value frame_width;                 /**<\brief Nb of pixel per line   */  
    t_ushort_value frame_height;                /**<\brief Nb of line             */ 
    t_ushort_value zone_of_interest_bitmap[84]; /**<\brief Zone to consider for
                                                            stab vect computation */
    t_ulong_value  reserved_1;                  /**<\brief To align struct on 32b */
   
} ts_t1xhv_vec_stab_param_in, *tps_t1xhv_vec_stab_param_in;

/** \brief Structure for parameters TO Host from an image stab. encode task */
typedef struct t1xhv_vec_stab_param_out {

    t_ushort_value error_type;       /**<\brief Error status                      */
    t_ushort_value reserved_1;       /**<\brief reserved 16                 */
    t_short_value  stab_vector_x;    /**<\brief Stabilization vector x coordinate */
    t_short_value  stab_vector_y;    /**<\brief Stabilization vector y coordinate */
    t_ulong_value  reserved_2;       /**<\brief To align struct on 16 bytes       */
    t_ulong_value  reserved_3;       /**<\brief To align struct on 16 bytes       */
  
} ts_t1xhv_vec_stab_param_out, *tps_t1xhv_vec_stab_param_out;


/*****************************************************************************/
/**
 * \brief  Parameter structure for grab
 * \author Serge Backert, Loic Habrial
 *
 * Parameter structure for grab. Hamac Video Spec v0.1
 */
/*****************************************************************************/
typedef struct t1xhv_grb_subtask_param {

    ts_t1xhv_subtask_link s_link;             /**<\brief Link to next subtask (chained 
                                               *          list) same for all tasks
                                               * -- ts_t1xhv_subtask_link                     */
    t_ahb_address addr_in_frame_buffer;       /**<\brief Add. of structure for input 
                                               *          frame buffer 
                                               * -- ts_t1xhv_grb_frame_buffer_in              */
    t_ahb_address addr_out_frame_buffer;      /**<\brief Add. of structure for output 
                                               *         frame buffer
                                               * -- ts_t1xhv_grb_frame_buffer_out             */
    t_ahb_address addr_internal_buffer;       /**<\brief Add. of structure for internal 
                                               *         buffer    
                                               * -- ts_t1xhv_grb_internal_buffer              */
    t_ahb_address addr_in_parameters;         /**<\brief Add. of structure for input 
                                               *          parameters   
                                               * -- ts_t1xhv_grb_parameters_in                */
    t_ahb_address addr_out_parameters;        /**<\brief Add. of structure for output 
                                               *          parameters  
                                               * -- ts_t1xhv_grb_parameters_out               */
    t_ahb_address addr_in_frame_parameters;   /**<\brief  Add. of struct for inout parameters
                                               *           of encode (depend on standard)     */
    t_ahb_address addr_out_frame_parameters;  /**<\brief  Add. of struct for inout parameters
                                               *           of encode (depend on standard)     */

    t_ahb_address reserved_1;                 /**<\brief reserved 32                          */
     
} ts_t1xhv_grb_subtask_param, *tps_t1xhv_grb_subtask_param;

/**
 * \brief t1xhv_grb_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_grb_frame_buf_in {

    t_ulong_value reserved_1;   /**<\brief reserved 32 */
    t_ulong_value reserved_2;   /**<\brief reserved 32 */
    t_ulong_value reserved_3;   /**<\brief reserved 32 */
    t_ulong_value reserved_4;   /**<\brief reserved 32 */

} ts_t1xhv_grb_frame_buf_in, *tps_t1xhv_grb_frame_buf_in;

/**
 * \brief t1xhv_grb_frame_buf_out, pointed by the third field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_frame_buf_out {

    t_ahb_address addr_dest_lc_buffer;       /**<\brief Start address of destination buffer
                                              * for a grab macroblock  */
    t_ahb_address addr_dest_raw_data_buffer; /**<\brief Start address of destination buffer
                                              * for a grab raw data    */
    t_ahb_address addr_dest_raw_data_end;    /**<\brief End address of destination buffer
                                              * for a grab raw data    */
    t_ulong_value reserved;                  /**<\brief reserved 32 */
    
} ts_t1xhv_grb_frame_buf_out, *tps_t1xhv_grb_frame_buf_out;

/**
 * \brief t1xhv_grb_internal_buf, pointed by the fourth field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_internal_buf {

    t_ulong_value reserved_1;   /**<\brief reserved 32 */
    t_ulong_value reserved_2;   /**<\brief reserved 32 */
    t_ulong_value reserved_3;   /**<\brief reserved 32 */
    t_ulong_value reserved_4;   /**<\brief reserved 32 */

} ts_t1xhv_grb_internal_buf, *tps_t1xhv_grb_internal_buf;

/**
 * \brief t1xhv_grb_parameters_in, pointed by the fifth field of the parameter 
 * structure. Read from Host.
 */
typedef struct t1xhv_grb_param_in {

    t_ushort_value source_frame_width;              /**<\brief Width of the source frame              */
    t_ushort_value source_frame_height;             /**<\brief Height of the source frame             */
    t_ushort_value source_window_width;             /**<\brief Width of the source window             */
    t_ushort_value source_window_height;            /**<\brief Height of the source window            */
    t_ushort_value source_window_horizontal_offset; /**<\brief Horizontal offset of the source window */
    t_ushort_value source_window_vertical_offset;   /**<\brief Vertical offset of the source window   */
    t_ushort_value resized_window_width;            /**<\brief Width of the resized window            */
    t_ushort_value resized_window_height;           /**<\brief Height of the resized window           */
    t_ushort_value interface_configuration;         /**<\brief Camera interface usage                 */
    t_ushort_value grab_sync_line;                  /**<\brief Grab_sync trigger line index           */
    t_ushort_value chroma_sampling_format;          /**<\brief Processing of chroma components        */
    t_ushort_value ace_enable;                      /**<\brief ace enable                             */
    t_ushort_value ace_strength;                    /**<\brief ace correction strength from 1 to 8    */
    t_ushort_value ace_range;                       /**<\brief ace range: 0=full, 1=reduced(BT601)    */
    t_ushort_value output_range;                    /**<\brief ace output range of the grab subtask   */
    t_ushort_value interlace_enable;                /**<\brief ccir interlace mode enable             */
    t_ushort_value field_sync;                      /**<\brief ccir field synchronization selection   */
    t_ushort_value raw_data_bpp;                    /**<\brief ccir 10-bit mode enable                */
    t_ulong_value  reserved_1;                      /**<\brief reserved 32                            */
    t_ulong_value  reserved_2;                      /**<\brief reserved 32                            */
    t_ulong_value  reserved_3;                        /**<\brief reserved 32                            */

} ts_t1xhv_grb_param_in, *tps_t1xhv_grb_param_in;


/** \brief Structure for parameters FROM and TO Host for a grab task */
typedef struct t1xhv_grb_param_inout {

    t_ulong_value  reserved_1;    /**<\brief reserved 32         */
    t_ushort_value ace_offset0;   /**<\brief ace output offset 0 */
    t_ushort_value ace_offset1;   /**<\brief ace output offset 1 */
    t_ushort_value ace_offset2;   /**<\brief ace output offset 2 */
    t_ushort_value ace_offset3;   /**<\brief ace output offset 3 */
    t_ulong_value  reserved_2;    /**<\brief reserved 32         */

} ts_t1xhv_grb_param_inout, *tps_t1xhv_grb_param_inout;


/**
 * \brief t1xhv_grb_parameters_out, pointed by the sixth field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_param_out {

    t_ushort_value error_type;    /**<\brief Error status                         */
    t_ushort_value field_number;  /**<\brief the number of the ccir field grabbed */
    t_time_stamp   time_stamp;    /**<\brief Execution time stamp                 */
    t_ulong_value  reserved_1;    /**<\brief reserved 32                          */
    t_ulong_value  reserved_2;    /**<\brief reserved 32                          */

} ts_t1xhv_grb_param_out, *tps_t1xhv_grb_param_out;


/*****************************************************************************/
/** @{ \name Parameters structures for tvout
 *     \author Jean-Marc Volle
 *     \note Spec V0.1
 */
/*****************************************************************************/

/** \brief This structure define description of a subtask tvout */

typedef struct t1xhv_tvd_subtask_param {

    ts_t1xhv_subtask_link s_link;        /**<\brief  Link to next subtask 
                                          * (chained list) same for all tasks
                                          * -- ts_t1xhv_subtask_link */
    t_ahb_address addr_in_frame_buffer;  /**<\brief  Address of structure for 
                                          * input frame buffer
                                          * -- ts_t1xhv_tvd_frame_buf_in */
    t_ahb_address addr_init_parameters;  /**<\brief  Address of structure for 
                                          * init parameters
                                          * -- ts_t1xhv_tvd_param_init */
    t_ahb_address addr_in_parameters;    /**<\brief  Address of structure for 
                                          * in parameters
                                          * -- ts_t1xhv_tvd_param_in */
    t_ahb_address reserved;              /**<\brief  Reserved 32         */

} ts_t1xhv_tvd_subtask_param, *tps_t1xhv_tvd_subtask_param;


/**
 * \brief t1xhv_tvd_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_tvd_frame_buf_in {

    t_ahb_address addr_source_buffer;      /**<\brief Source buffer start address */ 
    t_ahb_address reserved_1;              /**<\brief Reserved 32                 */
    t_ahb_address reserved_2;              /**<\brief Reserved 32                 */
    t_ahb_address reserved_3;              /**<\brief Reserved 32                 */
    
} ts_t1xhv_tvd_frame_buf_in, *tps_t1xhv_tvd_frame_buf_in;
/**
 * \brief t1xhv_tvd_param_init, pointed by the third field of the parameter 
 * structure, 
 */

typedef struct t1xhv_tvd_param_init {
    t_ushort_value clock_signal_selection;            /**<\brief Clock edge selection signal      */
    t_ushort_value clock_edge_selection;             /**<\brief  Clock edge selection             */
    t_ushort_value interlace_enable;                 /**<\brief  Interlacing enable flag          */
    t_ushort_value number_of_lines;                  /**<\brief  Number of lines                  */
    t_ushort_value field1_blanking_start_line;       /**<\brief  Field1 blanking start line       */
    t_ushort_value field1_blanking_end_line;         /**<\brief  Field1 blanking end line         */
    t_ushort_value field2_blanking_start_line;       /**<\brief  Field2 blanking start line       */
    t_ushort_value field2_blanking_end_line;         /**<\brief  Field2 blanking end line         */
    t_ushort_value field1_identification_start_line; /**<\brief  Field1 identification start line */
    t_ushort_value field2_identification_start_line; /**<\brief  Field2 identification start line */
    t_ushort_value line_blanking_witdh;              /**<\brief  Line blanking width              */
    t_ushort_value active_line_width;                /**<\brief  Active line width                */
    t_ulong_value  reserved_1;                       /**<\brief  Reserved 32                      */
    t_ulong_value  reserved_2;                       /**<\brief  Reserved 32                      */

} ts_t1xhv_tvd_param_init, *tps_t1xhv_tvd_param_init;

/**
 * \brief t1xhv_tvd_parameters_in, pointed by the fourth field of the parameter 
 * structure. Read from Host
 */
typedef struct t1xhv_tvd_param_in {
    t_ushort_value source_frame_width;                        /**<\brief Source frame width                        */
    t_ushort_value source_frame_height;                       /**<\brief Source frame height                       */
    t_ushort_value source_window_width;                       /**<\brief Source window width                       */
    t_ushort_value field1_source_window_height;               /**<\brief Field1 source window height               */
    t_ushort_value field2_source_window_height;               /**<\brief Field2 source window height               */
    t_ushort_value source_window_horizontal_offset;           /**<\brief Source window horizontal offset           */
    t_ushort_value field1_source_window_vertical_offset;      /**<\brief Field1 source window vertical offset      */
    t_ushort_value field2_source_window_vertical_offset;      /**<\brief Field2 source window vertical offset      */
    t_ushort_value destination_window_horizontal_offset;      /**<\brief Destination window horizontal offset      */
    t_ushort_value field1_destination_window_vertical_offset; /**<\brief Field1 destination window vertical offset */
    t_ushort_value field2_destination_window_vertical_offset; /**<\brief Field2 destination window vertical offset */
    t_ushort_value background_y;                              /**<\brief Background luminance value                */
    t_ushort_value background_cb;                             /**<\brief Background Cb chrominance value           */
    t_ushort_value background_cr;                             /**<\brief Background Cr chrominance value           */
    t_ulong_value  reserved_1;                                /**<\brief  Reserved 32                              */
    
} ts_t1xhv_tvd_param_in, *tps_t1xhv_tvd_param_in;

/** @}end of tvout subtask structures definition */

/** \brief This structure define parameters of a subtask encode for H263. */
typedef struct t1xhv_vec_h263_param_in {

    t_ushort_value picture_coding_type;      /**<\brief Type I or P of actual frame */
    t_ushort_value frame_width;              /**<\brief Width  in pixels from current frame */
    t_ushort_value frame_height;             /**<\brief Height in pixels from current frame */
    t_ushort_value window_width;             /**<\brief Width  in pixels from current Window */
    t_ushort_value window_height;            /**<\brief Height in pixels from current Window */
    t_ushort_value window_horizontal_offset; /**<\brief Horizontal offset from current Window */
    t_ushort_value window_vertical_offset;   /**<\brief Vertical offset from current Window */
    t_ushort_value enable_annexes;           /**<\brief - Enable mv over picture boundary 
                                              *         - Enable AC/DC prediction
                                              *         - Enable deblocking filter 
                                              *         - Enable slice structure
                                              *         - Enable modified quantization 
                                              */
    t_ushort_value gob_header_freq;          /**<\brief Frequency of GOB headers */
    t_ushort_value gob_frame_id;             /**<\brief GOB frame id (to be written into GOB hdrs) */
    t_ushort_value slice_size_type;          /**<\brief Parameter for annex k */
    t_ushort_value slice_bit_size;           /**<\brief */
    t_ushort_value slice_mb_size;            /**<\brief */
    t_ushort_value init_me;                  /**<\brief Allows to initialize the motion estimation 
                                              * data at the beginning of an MPEG4/H263 encode
                                              * subtask (e.g.after a scene change detection)
                                              */
    t_ushort_value me_type;                  /**<\brief Selects motion est algo */
    t_ushort_value reserved_1;
    t_ushort_value rounding_type;            /**<\brief Used for motion comp */
    t_ushort_value intra_refresh_type;       /**<\brief Intra refresh: AIR/CIR */
    t_ushort_value air_mb_num;               /**<\brief Nbr of AIR MBs */
    t_ushort_value cir_period_max;           /**<\brief CIR period */
    t_ushort_value quant;                    /**<\brief Initial value of the quantization parameter 
                                              * for an MPEG4 or an H263 encode subtask. It must be 
                                              * different from 0, otherwise error_type is set to 0xc0.
                                              */
    t_ushort_value brc_type;                 /**<\brief Method for bit rate control  */
    t_ulong_value  brc_frame_target;          /**<\brief Target size in bits for current frame. 
                                               * It is used if brc_type=1.
                                               */
    t_ulong_value  brc_target_min_pred;      /**<\brief internal variable */
    t_ulong_value  brc_target_max_pred;      /**<\brief internal variable */
    t_ulong_value  skip_count;               /**<\brief nb of consecutive skipped images */
    t_ulong_value  bitrate;                   /**<\brief target bitrate */
    t_ushort_value framerate;                 /**<\brief framerate  */
    t_ushort_value ts_modulo;                 /**<\brief current TS  */
    t_ushort_value ts_seconds;                /**<\brief current TS  */
    t_ushort_value air_thr;                   /**<\brief threshold for AIR  */
    t_ulong_value  delta_target;              /**<\brief internal */
    t_ushort_value minQp;                     /**<\brief min Qp  */
    t_ushort_value maxQp;                     /**<\brief max Qp  */
    t_ushort_value vop_time_increment_resolution; /**<\brief internal  */
    t_ushort_value fixed_vop_time_increment;      /**<\brief internal  */
    t_ulong_value  Smax;                       /**<\brief internal */
    t_ushort_value min_base_quality;           /**<\brief internal  */
    t_ushort_value min_framerate;             /**<\brief internal  */
    t_ulong_value  max_buff_level;              /**<\brief internal */
    t_ushort_value slice_loss_first_mb[8];   /**<\brief the positions of the first macroblock of 
                                              * slices that have been concealed                     */
    t_ushort_value slice_loss_mb_num[8];     /**<\brief number of macroblocks of slices that have
                                              *   been concealed                     */
} ts_t1xhv_vec_h263_param_in, *tps_t1xhv_vec_h263_param_in;

/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for H263. */
typedef struct t1xhv_vec_h263_param_inout {
    t_ulong_value bitstream_size;            /**<\brief size of encoded stream */
    t_ulong_value stuffing_bits;             /**<\brief stuffing bits */
    t_ulong_value pictCount;                 /**<\brief internal */
    t_ushort_value I_Qp;                     /**<\brief internal */
    t_ushort_value P_Qp;                     /**<\brief internal */
    t_ulong_value last_I_size;               /**<\brief internal */
    t_ulong_value comp_SUM;                  /**<\brief internal */
    t_ulong_value comp_count;                /**<\brief internal */
    t_ushort_value BUFFER_mod;               /**<\brief internal */
    t_ushort_value ts_modulo_old;            /**<\brief internal */
    t_ulong_value ts_seconds_old;            /**<\brief internal */
    t_ulong_value avgSAD;                    /**<\brief internal */
    t_ulong_value seqSAD;                    /**<\brief internal */
    t_ushort_value min_pict_quality;         /**<\brief internal */
    t_ushort_value diff_min_quality;         /**<\brief internal */
    t_ulong_value TotSkip;                   /**<\brief internal */
    t_ulong_value SkipCurrent;               /**<\brief internal */
    t_ushort_value Cprev;                    /**<\brief internal */
    t_ushort_value BPPprev;                  /**<\brief internal */
    t_ulong_value PictQpSum;                 /**<\brief internal */
    t_ulong_value S_overhead;                /**<\brief Texture size overhead in CBR */  
    t_long_value  ts_vector[6];              /**<\brief for TS moving average */  
    t_long_value  buffer_fullness;           /**<\brief for CBR */
} ts_t1xhv_vec_h263_param_inout, *tps_t1xhv_vec_h263_param_inout;


/** \brief This structure define parameters output of H263 encode subtask */
typedef struct t1xhv_vec_h263_param_out {

    t_ushort_value error_type;      /**<\brief Error status */
    t_ushort_value slice_num;       /**<\brief */
    t_ushort_value slice_pos[32];   /**<\brief positions of the 1st slices (up to 32) */
    t_ushort_value  brc_skip_prev;   /**<\brief skip decided by BRC  */
    t_ushort_value  reserved_1;      /**<\brief reserved  16  */
    t_ulong_value  reserved_2;      /**<\brief reserved  32  */
    t_ulong_value  reserved_3;      /**<\brief reserved  32  */
} ts_t1xhv_vec_h263_param_out, *tps_t1xhv_vec_h263_param_out;

#endif /* __T1XHV_NMF_ENV */

#endif /* _T1XHV_HOST_INTERFACE_COMMON_H_ */

