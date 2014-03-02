/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _T1XHV_HOST_INTERFACE_H_
#define _T1XHV_HOST_INTERFACE_H_


//NOTE : All t_ulong_value =>t_uint32
/*
 * Includes       
 */
#include "t1xhv_retarget.h"

/*
 * Types
 */


#define YUV_BUFFER_SIZE				  (1280*720*4)
//#define HEADER_SIZE_IN_BYTES		  (0x0400)
#define ENCODED_BITSTREAM_BUFFER_SIZE (0x8000)

/** \brief This structure define parameters of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_in {

    t_ushort_value picture_coding_type;      /**<\brief Type I or P of actual frame                 */
    t_ushort_value flag_short_header;        /**<\brief Short header mode if =1                     */
    t_ushort_value frame_width;              /**<\brief Width  in pixels from current frame         */
    t_ushort_value frame_height;             /**<\brief Height in pixels from current frame         */
    t_ushort_value window_width;             /**<\brief Width  in pixels from current Window        */
    t_ushort_value window_height;            /**<\brief Height in pixels from current Window        */
    t_ushort_value window_horizontal_offset; /**<\brief Horizontal offset from current Window       */
    t_ushort_value window_vertical_offset;   /**<\brief Vertical offset from current Window         */
    t_ushort_value gob_header_freq;          /**<\brief.Enables the use of GOB headers                */
    t_ushort_value gob_frame_id;             /**<\brief Species the gob_frame_id field of GOB headers */
    t_ushort_value data_partitioned;         /**<\brief enables data partitioning, for an MPEG4encode 
                                              * subtask. It is used only when flag_short_header=0.
                                              * It must be equal to 0 if the frame size is greater than 
                                              * CIF,otherwise error_type is set to 0xc6.
                                              */
    t_ushort_value reversible_vlc;           /**<\brief Enables the use of reversible codes,for an 
                                              * MPEG4encode subtask. It is used only when 
                                              * flag_short_header=0 and data_partitioned=1.
                                              */
    t_ushort_value hec_freq;                 /**<\brief Enables the use of Header Extension Codes and 
                                              * associated information,for an MPEG4encode subtask. It 
                                              * is used only when flag_short_header=0 and
                                              * data_partitioned=1. When hec_freq=0, no HEC information 
                                              * is inserted.Otherwise,HEC information is inserted in 
                                              * video packet headers once every hec_freq video packets.
                                              */
    t_ushort_value modulo_time_base;         /**<\brief the modulo_time_base field to be written in HEC   */
    t_ushort_value vop_time_increment;       /**<\brief the vop_time_increment field to be written in HEC */
    t_ushort_value vp_size_type;             /**<\brief Control of the video packet size,for an 
                                              * MPEG4encode subtask. It is used only when
                                              * flag_short_header=0 and data_partitioned=1.
                                              * It enables the use of vp_bit_size and vp_mb_size 
                                              * parameters.A video packet s closed as soon as it 
                                              * reaches the corresponding limit or the vp_size_max 
                                              * limit.Note that the last macroblock of the video packet
                                              * will be replaced by a "not coded" macroblock if the 
                                              * vp__size_max limit is reached.
                                              */
    t_ushort_value vp_size_max;              /**<\brief Maximum video packet size,in bits,for an 
                                              * MPEG4encode subtask. It is used only when 
                                              * flag_short_header=0 and data_partitioned=1.
                                              * If the vp_size_max limit is reached,the last macroblock 
                                              * is replaced by a "not coded" macroblock and the video
                                              * packet is closed,in order to respect the limit.
                                              */
    t_ushort_value vp_bit_size;              /**<\brief Minimum video packet size in bits,for an MPEG4
                                              * encode subtask.
                                              * It is used only when flag_short_header=0 and 
                                              * data_partitioned=1 and vp_size_type=0/2/3.
                                              * A video packet is closed as soon as it reaches the 
                                              * corresponding limit or the vp_size_max limit.
                                              * Note that the last macroblock of the video packet will 
                                              * be replaced by a "not coded" macroblock
                                              * if the vp_size_max limit is reached.
                                              */
    t_ushort_value vp_mb_size;               /**<\brief Minimum video packet size n macroblocks,
                                              * for an MPEG4encode subtask.It s used only when 
                                              * flag_short_header=0 and data_partitioned=1 and
                                              * vp_size_type=1/2/3.A video packet s closed as soon as 
                                              * it reaches the corresponding limit or the vp_size_max 
                                              * limit. Note that the last macroblock of the video 
                                              * packet will be replaced by a "not coded" macroblock 
                                              * if the vp__size_max limit is reached.
                                              */
    t_ushort_value init_me;                  /**<\brief Allows to initialize the motion estimation 
                                              * data at the beginning of an MPEG4/H263 encode
                                              * subtask (e.g.after a scene change detection)
                                              */
    t_ushort_value me_type;                  /**<\brief defines the motion estimation algorithm     */
    t_ushort_value vop_fcode_forward;        /**<\brief Fcode used (to determine Search window size */
    t_ushort_value rounding_type;            /**<\brief defines the value of the rounding control 
                                              * parameter used for pixel value interpolation
                                              * in motion compensation for P-frames. It is not 
                                              * used if picture_coding_type=0.
                                              */
    t_ushort_value intra_refresh_type;       /**<\brief enables the Adaptive Intra Refresh (AIR) 
                                              * and/or Cyclic Intra Refresh (CIR) algorithms,
                                              * for an MPEG4/H263 encode subtask.
                                              */
    t_ushort_value air_mb_num;               /**<\brief the number of macroblocks per frame to be 
                                              * refreshed in the AIR algorithm                 
                                              */
    t_ushort_value cir_period_max;           /**<\brief the maximum macroblock refresh period in
                                              * the CIR algorithm
                                              */
    t_ushort_value quant;                    /**<\brief Initial value of the quantization parameter 
                                              * for an MPEG4 or an H263 encode subtask. It must be 
                                              * different from 0, otherwise error_type is set to 0xc0.
                                              */
    t_ushort_value brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
    t_uint32  brc_frame_target;         /**<\brief Target size in bits for current frame. 
                                              * It is not used if brc_method=0/3.
                                              */
    t_uint32  brc_target_min_pred;      /**<\brief the predicted minimum number of bits to 
                                              * avoid buffer underflow
                                              */
    t_uint32  brc_target_max_pred;      /**<\brief the predicted maximum number of bits to 
                                              * avoid buffer overflow
                                              */
    t_uint32  skip_count;               /**<\brief the number of frames that have been 
                                              * skipped consecutively
                                              */

    t_uint32  bit_rate;                      /**<\brief Bitstream bit rate CBR/VBR */
    t_ushort_value framerate;                     /**<\brief Bitstream frame rate CBR/VBR */
    t_short_value  ts_modulo;           /**<\brief  vop time increment, signed */
    t_ushort_value ts_seconds;          /**<\brief  modulo time base */
    t_ushort_value air_thr;                  /**<\brief threshold for AIR */

    t_uint32  delta_target;                  /**<\brief Distance to target rate, signed */
    t_ushort_value minQp;                         /**<\brief Picture minimum allowed quantization parameter */
    t_ushort_value maxQp;                         /**<\brief Picture maximum allowed quantization parameter */
    t_ushort_value vop_time_increment_resolution; /**<\brief VOP time increment resolution CBR/VBR/HEC*/
    t_ushort_value fixed_vop_time_increment;      /**<\brief Fixed VOP time increment */
    t_uint32  Smax;                          /**<\brief Texture max size */
    t_ushort_value min_base_quality;              /**<\brief used in VBR only */
    t_ushort_value min_framerate;                 /**<\brief used in VBR only */
    t_uint32  max_buff_level;                /**<\brief used in CBR only */
  
	t_ushort_value first_I_skipped_flag;  /**<\brief from mainver24d */
	t_short_value  init_ts_modulo_old;      /**<\brief from mainver24d */
  
	t_ushort_value slice_loss_first_mb[8];   /**<\brief the positions of the first macroblock of 
					    * slices that have been concealed                     */
    t_ushort_value slice_loss_mb_num[8];     /**<\brief number of macroblocks of slices that have
                                              * been concealed                                      */
    t_uint32 yuvformat; // 0 - planar, 1 - semiplanar

	t_ushort_value adv_intra_coding;
	t_ushort_value mod_quant_mode;
	t_ushort_value slice_mode;
	t_ushort_value deblock_filter;
	t_ushort_value h263_p3;
//    t_ushort_value reserved1;
//	t_ushort_value reserved2;
} ts_t1xhv_vec_mpeg4_param_in, *tps_t1xhv_vec_mpeg4_param_in;


/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_inout {

    t_ulong_value  bitstream_size;   /**<\brief Size in bits of the bitstream that has been 
                                       * written by an encode subtask, including the header but 
                                       * not the stuffing bits.
                                       */
    t_ulong_value  stuffing_bits;    /**<\brief Number of stuffing bits added in the bitstream 
                                       *  during the encode subtask.
                                       * It is not used if brc_method=0/1/3.
                                       */
    t_ulong_value  pictCount;         /**<\brief Picture count */
    t_ushort_value I_Qp;              /**<\brief Initial quantization parameter for intra picture */
    t_ushort_value P_Qp;              /**<\brief Initial quantization parameter for inter picture */
    t_ulong_value  last_I_Size;       /**<\brief Last intra picture size */
    t_ulong_value  comp_SUM;          /**<\brief comp sum */
    t_ulong_value  comp_count;        /**<\brief comp count */
    t_ulong_value  BUFFER_mod;        /**<\brief Buffer mod */
    t_ulong_value  ts_seconds_old;    /**<\brief Old modulo time base */
    t_short_value  ts_modulo_old;     /**<\brief Previous vop time increment, signed */
    t_ushort_value gov_flag;          /**<\brief for CBR */
    t_ulong_value  avgSAD;            /**<\brief Average SAD in VBR               */
    t_ulong_value  seqSAD;            /**<\brief Sequential SAD in VBR               */
    t_ushort_value min_pict_quality;  /**<\brief Minimum picture quality in VBR     */
    t_ushort_value diff_min_quality;  /**<\brief Difference minimum quality in VBR, signed  */
    t_ulong_value  TotSkip;           /**<\brief Total skip in VBR              */
  
    t_ulong_value  Skip_Current;      /**<\brief Used in VBR and CBR */
  
    t_ushort_value Cprev;             /**<\brief Previous header size in CBR                */
    t_ushort_value BPPprev;           /**<\brief Previous bit per pixel parameter in CBR    */
    t_ulong_value  PictQpSum;         /**<\brief Picture quantization parameter sum in CBR  */
    t_ulong_value  S_overhead;        /**<\brief Texture size overhead in CBR           */
  
    t_long_value   ts_vector[6];         /**<\brief for TS moving average */
  
    t_long_value   buffer_fullness;            /**<\brief for CBR */
    t_long_value   buffer_fullness_fake_TS;/**<\brief for CBR */
  
    t_ulong_value	BUFFER_depletion; /**<\brief added from mainver2.4d */
	t_ushort_value  buffer_saved; /**<\brief added from mainver2.4d */
	t_ushort_value  intra_Qp_flag;   /**<\brief added for MAINVER2.5c */
    
	t_ulong_value	BUFFER_depletion_fake_TS; /**<\brief added for MAINVER2.5a */
	t_ushort_value	old_P_Qp_vbr;   /**<\brief added for MAINVER2.5c */
	t_ushort_value	mv_field_index;  /**<\brief Added for IME to indicate the MV_field index */
	t_ulong_value	pictCount_prev; /**<\brief added for MAINVER2.5c */
	t_ulong_value	PictQpSumIntra; /**<\brief added for segmented mode */
  
	t_ushort_value hec_count;     /**<\brief number of VP since last HEC */
	t_ushort_value reserved_1;    /**<\brief Reserved to make a long */
	t_long_value reserved_2;		/**<\brief Reserved to make multiple for 4*16 */
	t_long_value reserved_3;		/**<\brief Reserved to make multiple for 4*16 */
	t_long_value reserved_4;		/**<\brief Reserved to make multiple for 4*16 */

	/* GT: enanched MB layer control for better Qp distribution, 02 Apr 07 */
    t_ushort_value BPPmbPrevAct;/**<\brief added for Enhanced MB quant control */
    t_ushort_value BPPmbPrevEst;/**<\brief added for Enhanced MB quant control */
    t_ushort_value CodedMB;/**<\brief added for Enhanced MB quant control */
    t_ushort_value CodedMB_flag;/**<\brief added for Enhanced MB quant control */
    t_long_value Lprev;/**<\brief added for Enhanced MB quant control */
    t_long_value BPPmbErr;/**<\brief added for Enhanced MB quant control */


} ts_t1xhv_vec_mpeg4_param_inout, *tps_t1xhv_vec_mpeg4_param_inout;

/** \brief This structure define parameters output of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_out {

    t_ushort_value  error_type;     /**<\brief Error type if an error occurs during the encode 
                                     * subtask.
                                     */
    t_ushort_value  vp_num;         /**<\brief Number of video packets that have been written 
                                     * by an MPEG4 encode subtask.It is used only when 
                                     * flag_short_header=0. Note that there is no video packet
                                     * header for the first video packet.
                                     */
    t_ushort_value  vp_pos[32];      /**<\brief positions of the first video packets (up to 32)
                                     * that have been written by an MPEG4encode subtask. It is
                                     * used only when flag_short_header=0. The positions are
                                     * given in bytes,relatively to the beginning of the 
                                     * bitstream that has been written,including the header.
                                     */
    t_ushort_value  brc_skip_prev;   /**<\brief  Flag indicative when the encoded frame needs 
                                      * to be skipped */
    t_ushort_value  reserved_1;      /**<\brief reserved  16  */
    t_uint32   reserved_2;      /**<\brief reserved  32  */
    t_uint32   reserved_3;      /**<\brief reserved  32  */

 } ts_t1xhv_vec_mpeg4_param_out, *tps_t1xhv_vec_mpeg4_param_out;


/** \brief This structure define a link buffer. */
typedef struct t1xhv_bitstream_buf_link {

    t_ahb_address addr_next_buf_link;   /**<\brief Address next structure */
    t_ahb_address addr_prev_buf_link;   /**<\brief Address prev structure */
    t_ahb_address addr_buffer_start;    /**<\brief Bitstream buffer start */
    t_ahb_address addr_buffer_end;      /**<\brief Bitstream buffer end   */

} ts_t1xhv_bitstream_buf_link, *tps_t1xhv_bitstream_buf_link;


/** \brief This structure define Parameters needed to For begin/end of buffer. */
typedef struct t1xhv_bitstream_buf_pos {

    t_ahb_address   addr_bitstream_buf_struct; /**<\brief Choose buffer structure            */
    t_ahb_address   addr_bitstream_start;      /**<\brief Bitstream Start add. inside buffer */
    t_uint32   bitstream_offset;          /**<\brief Bitstream offset in bits           */
    t_uint32   reserved_1;                /**<\brief Reserved 32                        */

} ts_t1xhv_bitstream_buf_pos, *tps_t1xhv_bitstream_buf_pos;


/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vec_frame_buf_in {

    t_ahb_address  addr_source_buffer;        /**<\brief Buffer to encode.                    */
    t_ahb_address  addr_fwd_ref_buffer;       /**<\brief Address of prev reconstructed buffer */
    t_ahb_address  addr_grab_ref_buffer;      /**<\brief Address of buffer from grab          */
    t_ahb_address  addr_intra_refresh_buffer; /**<\brief Add. of intra refresh buffer         */
#if 0 //def H263_P3
	t_ahb_address  QP;					      /* QP values for deblocking */   
	t_ahb_address  not_coded_P3;			  /*Not coded values for deblockin*/   
	t_ahb_address  slice_num;			  /*Not coded values for deblockin*/   
#endif

} ts_t1xhv_vec_frame_buf_in, *tps_t1xhv_vec_frame_buf_in;


/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vec_frame_buf_out {

    t_ahb_address  addr_dest_buffer;             /**<\brief Add. of output frame buffer  */
    t_ahb_address  addr_deblocking_param_buffer; /**<\brief Add. of parameters for PPP   */
    t_ahb_address  addr_motion_vector_buffer;    /**<\brief Add. of motion vector        */
    t_ahb_address  addr_intra_refresh_buffer;    /**<\brief Add. of intra refresh buffer */

} ts_t1xhv_vec_frame_buf_out, *tps_t1xhv_vec_frame_buf_out;


/** \brief This structure define an internal frame buffer. */
typedef struct t1xhv_vec_internal_buf {

    t_ahb_address  addr_search_window_buffer;  /**<\brief Start add. of buffer for Search Window */
    t_ahb_address  addr_search_window_end;     /**<\brief End add. of buffer for Search Window   */
    t_ahb_address  addr_jpeg_run_level_buffer; /**<\brief Start add. of JPEG run level buffer    */
    t_ahb_address  addr_h264e_H4D_buffer;      /**<\brief Address of temporary buffer used by H4D for H264. */

} ts_t1xhv_vec_internal_buf, *tps_t1xhv_vec_internal_buf;


/** \brief This structure define an header buffer. */
typedef struct t1xhv_vec_header_buf {

    t_ahb_address  addr_header_buffer;  /**<\brief Start add. of the header buffer */
    t_uint32  header_size;         /**<\brief Header size                     */
    t_ahb_address  reserved_1;          /**<\brief Reserved 32                     */
    t_ahb_address  reserved_2;          /**<\brief Reserved 32                     */

} ts_t1xhv_vec_header_buf, *tps_t1xhv_vec_header_buf;


//should be in t1xhv_vec_mp4e.idt file
typedef struct t1xhv_vec_mp4e_global{
	struct BitStreamData   WriteBits;
	CBR_HAMAC_par		   CBR_hamac;
	VBR_HAMAC_par		   VBR_hamac;
	Motion_Estimation_Data Motion_Estimation; //predictors for temporal & spatial prediction
	Generate_MV_Data       Generate_MV; //array for MV pred
	ts_m4e_global_param    Gs_m4e_global_param;
	DC_Data				   DC;
	VBVData				   VBV; 
	Bitstream_Data         Bitstream;
    struct VLCDATA		   VLC;
}ts_t1xhv_vec_mp4e_global, *tps_t1xhv_vec_mp4e_global;

typedef struct
{
	tps_t1xhv_vec_mpeg4_param_out	Gps_vec_mpeg4_param_out;
	tps_t1xhv_vec_mpeg4_param_inout	Gps_vec_mpeg4_param_inout;
	tps_t1xhv_vec_mpeg4_param_in	Gps_vec_mpeg4_param_in;

	tps_t1xhv_bitstream_buf_pos		Gps_bitstream_buf_pos_out;
	tps_t1xhv_bitstream_buf_pos		Gps_bitstream_buf_pos;
	tps_t1xhv_vec_header_buf		Gps_bitstream_buf_header;
	tps_t1xhv_vec_internal_buf		Gps_vec_internal_buf;
	tps_t1xhv_vec_frame_buf_out		Gps_vec_frame_buf_out;
	tps_t1xhv_vec_frame_buf_in		Gps_vec_frame_buf_in;
	ts_t1xhv_vec_mp4e_global        Gs_vec_mp4e_global;

	t_uint32						G_addr_out_parameters;
	t_uint32						G_addr_out_frame_parameters;

	t_uint16						QP[7200];
	t_uint16						not_coded_P3[7200];
	t_uint16						slice_num[7200];
	t_sint16						mb_refresh_map[3600]; //720p resolution
}ts_t1xhv_vec_mp4e_session, *tps_t1xhv_vec_mp4e_session;

#ifdef _SESSION_DATA_
tps_t1xhv_vec_mp4e_session Gps_vec_mp4e_session;
#else
extern tps_t1xhv_vec_mp4e_session Gps_vec_mp4e_session;
#endif



#endif /* _T1XHV_HOST_INTERFACE_H_ */

