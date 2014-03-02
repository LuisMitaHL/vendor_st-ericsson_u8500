/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_interface.h
 * \brief 	host interface file (stand alone version of for t1xhv_host_interface.h)
 * \author  ST-Ericsson
 *  
 *  This file defines the structures exchanged between ARM and MMDSP in real
 *  Nomadik Jpeg decoder   
 */
/*****************************************************************************/

#ifndef _DJPEG_INTERFACE_H_
#define _DJPEG_INTERFACE_H_

#include "djpeg_common.h"

#ifndef __NMF



	typedef struct vdc_hw_tables
	{
    t_ushort_value      huffman_y_code_dc[12];  /**< \brief Y DC EHUFCO table   */
    t_ushort_value      huffman_y_size_dc[12];   /**< \brief Y DC EHUFSI table */
    t_ushort_value      huffman_y_code_ac[256];  /**< \brief Y AC EHUFCO table   */
    t_ushort_value      huffman_y_size_ac[256];  /**< \brief Y AC EHUFSI table */

    t_ushort_value      huffman_cb_code_dc[12]; /**< \brief Cb DC EHUFCO table   */
    t_ushort_value      huffman_cb_size_dc[12];  /**< \brief Cb DC EHUFSI table */
    t_ushort_value      huffman_cb_code_ac[256]; /**< \brief Cb AC EHUFCO table   */
    t_ushort_value      huffman_cb_size_ac[256]; /**< \brief Cb AC EHUFSI table */

    t_ushort_value      huffman_cr_code_dc[12]; /**< \brief Cr DC EHUFCO table   */
    t_ushort_value      huffman_cr_size_dc[12];  /**< \brief Cr DC EHUFSI table */
    t_ushort_value      huffman_cr_code_ac[256]; /**< \brief Cr AC EHUFCO table   */
    t_ushort_value      huffman_cr_size_ac[256]; /**< \brief Cr AC EHUFSI table */

	} ts_t1xhv_vdc_hw_table, *tps_t1xhv_vdc_hw_table;

	typedef struct vdc_dc_table
	{
	t_ushort_value      huffman_bits_dc[16];  /**< \brief Y DC EHUFCO table   */
	t_ushort_value      huffman_val_dc[12];   /**< \brief Y DC EHUFSI table */
	t_long_value      maxcode[18];		/* largest code of length k (-1 if none) */
	t_long_value      valoffset[17];		/* huffval[] offset for codes of length k */	
	t_short_value      look_nbits[256]; /* # bits, or 0 if too long */
	t_ushort_value      look_sym[256]; /* symbol, or unused */	

	} ts_t1xhv_vdc_sw_dc_table;

	typedef struct vdc_ac_table
	{
	t_ushort_value      huffman_bits_ac[16];  /**< \brief Y DC EHUFCO table   */
	t_ushort_value      huffman_val_ac[256];   /**< \brief Y DC EHUFSI table */
	t_long_value      maxcode[18];		/* largest code of length k (-1 if none) */
	t_long_value      valoffset[17];		/* huffval[] offset for codes of length k */	
	t_short_value      look_nbits[256]; /* # bits, or 0 if too long */
	t_ushort_value      look_sym[256]; /* symbol, or unused */	
		
	} ts_t1xhv_vdc_sw_ac_table;


	typedef struct 
	{

	ts_t1xhv_vdc_sw_dc_table  huffman_y_dc_table;
	ts_t1xhv_vdc_sw_ac_table  huffman_y_ac_table;
	ts_t1xhv_vdc_sw_dc_table  huffman_cb_dc_table;
	ts_t1xhv_vdc_sw_ac_table  huffman_cb_ac_table;
	ts_t1xhv_vdc_sw_dc_table  huffman_cr_dc_table;
	ts_t1xhv_vdc_sw_ac_table  huffman_cr_ac_table;	

	} ts_t1xhv_vdc_sw_table, *tps_t1xhv_vdc_sw_table;

	typedef  struct 
	{
	ts_t1xhv_vdc_hw_table  hw_huff_table;
	t_ulong_value 	sw_huff_table;

	}ts_t1xhv_vdc_huff_table;


/** \brief Hamac decoder jpeg param in  */
typedef struct 
{
    t_ushort_value frame_width;         
    t_ushort_value frame_height;        
    t_ushort_value nb_components;    
    t_ushort_value h_sampling_factor_y;
    t_ushort_value v_sampling_factor_y;
    t_ushort_value h_sampling_factor_cb;
    t_ushort_value v_sampling_factor_cb;
    t_ushort_value h_sampling_factor_cr; 
    t_ushort_value v_sampling_factor_cr; 
    t_ushort_value downsampling_factor;  
    t_ushort_value restart_interval;          
    t_ushort_value progressive_mode;         
    t_ushort_value nb_scan_components;    
    t_ushort_value component_selector_y;  
    t_ushort_value component_selector_cb;
    t_ushort_value component_selector_cr; 
    t_ushort_value start_spectral_selection;
    t_ushort_value end_spectral_selection;  
    t_ushort_value successive_approx_position;
     t_ushort_value ace_strength;             
     t_ushort_value ace_enable;                
    t_ushort_value noslice_enable;           
    t_ulong_value  buffer_size;			

    t_ushort_value quant_y[64];               
    t_ushort_value quant_cb[64];             
    t_ushort_value quant_cr[64];              
    ts_t1xhv_vdc_huff_table   huff_table;
      
      
    t_ushort_value window_width;            
    t_ushort_value window_height;           
    t_ushort_value window_horizontal_offset;
    t_ushort_value window_vertical_offset;    
   t_ulong_value reserved_3;                 /**<\brief reserved 32              */
    t_ulong_value reserved_4;                 /**<\brief reserved 32              */
} ts_t1xhv_vdc_jpeg_param_in, *tps_t1xhv_vdc_jpeg_param_in;


/** \brief This structure defines a Decode input frame buffers. */
typedef struct {
t_ulong_value   addr_fwd_ref_buffer;  
t_ulong_value   addr_bwd_ref_buffer; 
t_ulong_value  addr_source_buffer;    
t_ulong_value  addr_deblocking_param_buffer_in;  
} ts_t1xhv_vdc_frame_buf_in;

typedef ts_t1xhv_vdc_frame_buf_in *tps_t1xhv_vdc_frame_buf_in;

/** \brief This structure defines a Decode internal buffer. */
typedef struct {
t_ulong_value   addr_vpp_dummy_buffer;
t_ulong_value   addr_h264d_block_info;   
t_ulong_value   addr_h264d_mb_slice_map; 
t_ulong_value   addr_mv_history_buffer;    
t_ulong_value   addr_mv_type_buffer;     
t_ulong_value   addr_mb_not_coded;          
t_ulong_value   addr_x_err_res_buffer;      
t_ulong_value   addr_y_err_res_buffer;   

} ts_t1xhv_vdc_internal_buf;
typedef ts_t1xhv_vdc_internal_buf *tps_t1xhv_vdc_internal_buf;


/** \brief Hamac video jpeg decode output parameters */ 
typedef struct vdc_jpeg_param_out 
{
    t_ushort_value  error_type;  
    t_ushort_value last_slice;	
    t_ushort_value  ace_offset0; 
    t_ushort_value  ace_offset1; 
    t_ushort_value  ace_offset2; 
    t_ushort_value  ace_offset3; 
    t_ulong_value out_buff_size;


} ts_t1xhv_vdc_jpeg_param_out, *tps_t1xhv_vdc_jpeg_param_out;


/** \brief Hamac video jpeg decode in / out parameters */ 
typedef struct vdc_jpeg_param_inout
{

	t_ulong_value  mcu_index;           
	t_ulong_value  end_of_band_run; 
	t_ushort_value dc_predictor_y;     
	t_ushort_value dc_predictor_cb;   
	t_ushort_value dc_predictor_cr;    
	t_ushort_value  slice_pos;		
	t_ulong_value  ace_count0;          
	t_ulong_value  ace_count1;          
	t_ulong_value  ace_count2;          
	t_ulong_value  ace_count3;          
	t_ulong_value  crop_mcu_index;  
	t_ulong_value  crop_mcu_index_in_row; 
	t_ushort_value mcu_index_rst;			
	t_ushort_value rst_value;				
	t_ulong_value  reserved_1;				

} ts_t1xhv_vdc_jpeg_param_inout, *tps_t1xhv_vdc_jpeg_param_inout;



/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vdc_frame_buf_out {

    t_ulong_value   addr_dest_buffer;             /**<\brief Address of output frame buffer.   */
    t_ulong_value   addr_deblocking_param_buffer; /**<\brief Address of parameters for PPP.    */
    t_ulong_value   addr_motion_vector_buffer;    /**<\brief Start add of motion vector buffer */
    t_ulong_value   addr_jpeg_coef_buffer;        /**<\brief Start address of JPEG Coef buffer */
    t_ulong_value   addr_jpeg_line_buffer;        /**<\brief Start address of JPEG line buffer */
    t_ulong_value   reserved_1;                   /**<\brief Reserved 32                       */
    t_ulong_value   reserved_2;                   /**<\brief Reserved 32                       */
    t_ulong_value   reserved_3;                   /**<\brief Reserved 32                       */

} ts_t1xhv_vdc_frame_buf_out, *tps_t1xhv_vdc_frame_buf_out;

typedef struct 
{
    t_ulong_value   addr_bitstream_buf_struct; 
    t_ulong_value   addr_bitstream_start;    
    t_uint32   bitstream_offset;     
    t_uint32   reserved_1;                
} ts_t1xhv_bitstream_buf_pos,*tps_t1xhv_bitstream_buf_pos;

/** \brief This structure define a link buffer. */
typedef struct 
{
    t_ulong_value addr_next_buf_link;   
    t_ulong_value addr_prev_buf_link;   
    t_ulong_value addr_buffer_start;  
    t_ulong_value addr_buffer_end;    
} ts_t1xhv_bitstream_buf_link,*tps_t1xhv_bitstream_buf_link;


typedef enum {
  CMD_RESET,			/**<\brief (Not yet implemented) will cause the internal reset (FW+HW). */
  CMD_ABORT,			/**<\brief (Not yet implemented) will cause the abort to the current executing component. */
  CMD_START,			/**<\brief will cause the start of the already configured component. */ 
  CMD_UPDATE_BUFFER,    /**<\brief will reprograme the dma addresses to new buffer. This will be used in "updateAlgo" Method */ 
  CMD_STOP_SLICE        /**<\brief will reprograme the dma addresses to dummy buffer. This will be used in "updateAlgo" Method */
} t_t1xhv_command;

typedef enum {
  STATUS_JOB_COMPLETE,		/**<\brief The component run completely its job. errors should then be VDC_ERT_NONE. */
  STATUS_JOB_ABORTED,		/**<\brief The component has been aborted. */
  STATUS_JOB_UNKNOWN,		/**<\brief This should not happen. */
  STATUS_BUFFER_NEEDED		/**<\brief The component requires more buffer to complete its job*/
} t_t1xhv_status;

typedef enum {

  /**  Job complete with No error (provided with a status STATUS_JOB_COMPLETE)   */
  VDC_ERT_NONE,

  /**  Job complete with void execution of codec (provided with a status STATUS_JOB_COMPLETE)   */
  VDC_VOID,
  
  /**  All error codes following VDC_ERT are provided with a status STATUS_JOB_ABORTED */
  VDC_ERT,

  /**  Not enough data to be decoded (End Of Window internal interrupt, i.e. EOW) */
  VDC_ERT_END_OF_BITSREAM_REACHED,
  VDC_ERT_BEGINNING_OF_BITSREAM_REACHED,

  /**  The requested feature is not supported */
  VDC_ERT_NOT_SUPPORTED,                

  /**  One of the input parameter is wrong */
  VDC_ERT_BAD_PARAMETER,

  /** bitstream error detected while decoding */
  VDC_ERT_BITSTREAM_ERR                 ,

  /** addr_fwd_ref_buffer is not a multiple of 256   */
  VDC_ERT_BAD_ADD_FWD_REF_BUFFER        , 

  /** addr_dest_buffer is not a multiple of 256  */
  VDC_ERT_BAD_ADD_DEST_BUFFER           , 

  /** addr_deblocking_param_buffer is not a multiple of 4   */
  VDC_ERT_BAD_ADD_DEBLOCKING_PARAM_BUFFER, 

  /**  addr_bitstream_start is not a multiple of 16  */
  VDC_ERT_BAD_ADD_BITSTREAM_START       , 

  /** addr_buffer_start is not a multiple of 16   */
  VDC_ERT_BAD_ADD_BUFFER_START          , 

  /** addr_buffer_end is not a multiple of 16   */
  VDC_ERT_BAD_ADD_BUFFER_END            , 

  /** addr_window_start is not a multiple of 16   */
  VDC_ERT_BAD_ADD_WINDOW_START          , 

  /** addr_window_end is not a multiple of 16   */
  VDC_ERT_BAD_ADD_WINDOW_END            ,

  /** frame_width is not a multiple of 16   */ 
  VDC_ERT_BAD_FRAME_WIDTH               ,

  /** frame_height is not a multiple of 16  */ 
  VDC_ERT_BAD_FRAME_HEIGHT              , 

  /** quant is equal to 0    */
  VDC_ERT_BAD_QUANT                     ,

  /** vop_fcode_forward is equal to 0   */
  VDC_ERT_BAD_VOP_FCODE_FORWARD         ,

  /** vop_time_increment_resolution is equal to 0   */
  VDC_ERT_BAD_TIME_INCREMENT_RESOLUTION ,

  /** EAD is different from EAJ in enable_annexes   */
  VDC_ERT_EAD_DIFF_EAJ                  ,

  /** EAF is different from EAJ in enable_annexes    */
  VDC_ERT_EAF_DIFF_EAJ                  ,

  /** data_partitioned=1 while frame size is greater than CIF    */
  VDC_ERT_DATA_PARTITIONED_WITH_FS_GT_CIF

} t_t1xhv_decoder_info;

#endif

#endif /* _DJPEG_INTERFACE_H_ */
