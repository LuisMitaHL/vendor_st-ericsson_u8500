#ifndef _DJPEG_GLOBAL_H_
#define _DJPEG_GLOBAL_H_

#include "djpeg_common.h"

#ifndef __NMF



#include "djpeg_interface.h"

#define MAX_BLOCS_PER_MCU 10
#define BLOCKS_PER_COMP_IN_MCU_SET	8
#define MAX_BLOCKS_PER_MB  12


	/** \brief typical subsampling factors */
	typedef enum
	{
	SF_420=0,
	SF_422,
	SF_444,
	SF_MONOCHROME,
	SF_ANY
	} t_subsampling;

	typedef struct analysis
	{
	t_uint32 ace_count0;  /**< \brief current accumulated value of luma in zone 0 of frame   */
	t_uint32 ace_count1;  /**< \brief current accumulated value of luma in zone 1 of frame   */
	t_uint32 ace_count2;  /**< \brief current accumulated value of luma in zone 2 of frame   */
	t_uint32 ace_count3;  /**< \brief current accumulated value of luma in zone 3 of frame   */
	t_sint16 ace_offset0; /**< \brief ace offset in zone 0 of frame   */
	t_sint16 ace_offset1; /**< \brief ace offset in zone 1 of frame   */
	t_sint16 ace_offset2; /**< \brief ace offset in zone 2 of frame   */
	t_sint16 ace_offset3; /**< \brief ace offset in zone 3 of frame   */
	t_uint16 ace_strength; /**< \brief strength value for ace algorithm */
	t_uint16 ace_enable;   /**< \brief flag to enable the ACE analysis */
	} ts_analysis, *tps_analysis;

	/** \brief Mcu stop condition state saving struct */
	typedef struct stop_state
	{
	t_uint32 mcu_index;   /**< \brief index of the first mcu of the next mcu set   */
	t_ulong_value mcu_address ;/**< \brief byte address of the first mcu of the next mcu set   */
	t_uint32 mcu_offset ;         /**< \brief bit offset within the byte address of the next mcu set   */
	t_sint16 dc_predictor_y;      /**< \brief dc predictor for first y block in next MCU set   */
	t_sint16 dc_predictor_cb;     /**< \brief dc predictor for first cb block in next MCU set   */
	t_sint16 dc_predictor_cr;     /**< \brief dc predictor for first cr block in next MCU set   */
	t_uint32 end_of_band_run;     /**< \brief end of band run in progressive mode   */
	t_uint32 crop_mcu_index;      /**< \brief mcu_index number when there is a crop */
	t_uint32 crop_mcu_index_in_row; /**< \brief mcu_index number in row when there is a crop */
	t_sint32 get_buffer;	/* current bit-extraction buffer */
	t_sint16 bits_left;
	t_uint8 * next_input_byte;

	} ts_stop_state;

	/** \brief Pointer on Mcu stop condition state saving struct */
	typedef ts_stop_state *tps_stop_state; 

	typedef struct window_cropping
	{
	t_uint16 pix_offset_x;
	t_uint16 pix_offset_y;
	t_uint16 pix_window_width;
	t_uint16 pix_window_height;
	t_uint16 crop_enable;
	t_uint16 mb_offset_x;
	t_uint16 mb_offset_y;
	t_uint16 mb_window_width;
	t_uint16 mb_window_height;

	}ts_crop,*tps_crop;


	typedef struct dc_table
	{
	t_uint16* bits;
	t_uint16* huffval;
	t_sint32 *maxcode;		/* largest code of length k (-1 if none) */
	t_sint32 *valoffset;		/* huffval[] offset for codes of length k */	
	t_sint16 *look_nbits; /* # bits, or 0 if too long */
	t_uint16 *look_sym; /* symbol, or unused */	

	}ts_dc_table,*tps_dc_table;

	typedef struct ac_table
	{
	t_uint16* bits;
	t_uint16* huffval;
	t_sint32 *maxcode;		/* largest code of length k (-1 if none) */
	t_sint32 *valoffset;		/* huffval[] offset for codes of length k */	
	t_sint16 *look_nbits; /* # bits, or 0 if too long */
	t_uint16 *look_sym; /* symbol, or unused */	

	}ts_ac_table,*tps_ac_table;

	/** \brief component id */
	typedef enum
	{
	COMP_Y=0,
	COMP_CB,
	COMP_CR
	} t_component_id;

	/** \brief struct holding component relevant informations   */
	typedef struct component
	{
	t_uint16 *quant;     /**< \brief quantification table of component in natural order   */
	t_uint16 h_sampling_factor;
	t_uint16 v_sampling_factor;
	tps_dc_table dc_table;
	tps_ac_table ac_table;
	t_component_id comp_id; /* component identifier */
	
	} ts_component, *tps_component;

	typedef struct mcu_predictor
	{
	t_sint16 dc_y;
	t_sint16 dc_cb;
	t_sint16 dc_cr;

	} ts_mcu_predictor,*tps_mcu_predictor;

	/** \brief struct holding scan relevant informations   */
	typedef struct scan
	{
	t_uint16 nb_components;
	t_uint16 restart_interval;
	t_uint16 start_spectral_selection;
	t_uint16 end_spectral_selection;
	t_uint16 successive_approx_position_high;
	t_uint16 successive_approx_position_low;
	t_uint32 mcu_index;           /**< \brief index of current processed mcu   */
	ts_mcu_predictor mcu_pred;  /**< \brief dc predictor of current mcu   */
	T_BOOL is_y_present;        /**< \brief TRUE is y is in present scan   */
	T_BOOL is_cb_present;       /**< \brief TRUE is cb is in present scan   */
	T_BOOL is_cr_present;       /**< \brief TRUE is cr is in present scan   */
	t_uint32 end_of_band_run;
	} ts_hv_scan, *tps_hv_scan;

	/** \brief struct holding frame relevant informations   */
	typedef struct frame
	{
	T_BOOL is_progressive;
	t_uint16 width;       /**< \brief real frame width (no resize)   */
	t_uint16 height;      /**< \brief real frame height (no resize)   */
	t_uint16 nb_components;
	t_uint16 downsampling_factor;

	/* for progressive/sequential mode handling any frame mcu information is equivalent */
	/*     to the one of the scan is sequential mode */
	t_uint32 mcu_index;           /**< \brief index of current processed mcu   */
	t_uint16 mcus_in_row;	/**< \brief nb of MCUs in one image row   */
	t_uint16 mcus_in_col;	/**< \brief nb of MCUs in one image column  */
	t_uint16 mcu_counter_in_row; /**< \brief nb of MCUs decoded on the current image row  */
	t_uint16 mcu_row; 	/**row of the current mcu being decoded */
	t_uint32 mcus_in_frame;
	t_uint16 max_h_samp_factor;  /**< \brief largest horizontal sampling factor  */
	t_uint16 max_v_samp_factor;  /**< \brief largest vertical sampling factor  */
	t_uint32 crop_mcu_index_in_row;     /**< \brief nb of MCUs decoded on the current image row  when there's a crop*/
	t_uint32 crop_mcu_index;            /**< \brief index of current processed mcu when there's a crop  */
	} ts_frame,*tps_frame;


	/** \brief struct block relevant informations   */
	typedef struct block
	{
	t_sint16 sample[64];          /**< \brief bloc samples in zig-zag order   */
	tps_component p_component;  /**< \brief pointer on bloc component description   */
	t_sint16 *p_dc_pred;          /**< \brief dc predictor from the previous block of the same component   */

	T_BOOL coeff_in_last_rows_cols; /**< coefficient in last 4 rows  */
	} ts_block, *tps_block;


	/** \brief struct mcu relevant informations   */
	typedef struct mcu
	{
	ts_block blocks[MAX_BLOCS_PER_MCU];
	t_uint16 nb_of_blocks;
	t_uint16 nb_y_blocks;         /**< \brief number of y blocks in the mcu   */
	t_uint16 nb_cb_blocks;        /**< \brief number of cb blocks in the mcu   */
	t_uint16 nb_cr_blocks;        /**< \brief number of cr blocks in the mcu   */
	} ts_mcu, *tps_mcu;


	typedef struct hamac_info
	{
	t_djpeg_error_code error_code;
	ts_frame frame;                     /**< \brief frame descriptor   */
	ts_hv_scan scan;                    /**< \brief scan descriptor   */
	ts_component y;                     /**< \brief component Y descriptor   */
	ts_component cb;                    /**< \brief component Cb descriptor   */
	ts_component cr;                    /**< \brief component Cr descriptor   */
	ts_stop_state stop_state;           /**< \brief stop state struct   */
	ts_analysis analysis;               /**< \brief ace analysis results   */
	ts_mcu      mcu;                    /**< \brief current mcu struct   */
	t_ulong_value   add_destination_buffer; /**< \brief adress of mb destination buffer in host address space   */
	t_ulong_value   add_line_buffer;        /**< \brief adress of line buffer in host address space   */
	t_ulong_value   add_coeff_buffer;       /**< \brief adress coefficient buffer for progressive mode */
	ts_crop     crop;                   /**< \brief crop parameters if crop is activated   */
	t_uint16 noslice_enable;
	} ts_hamac_info, *tps_hamac_info;





	/*------------------------------------------------------------------------
	* Types
	*----------------------------------------------------------------------*/
	/** \brief enumeration of scan types in progressive mode   */
	typedef enum
	{
	PROG_DC_FIRST_SCAN = 0, /**< \brief first dc scan in successive approximation mode   */
	PROG_DC_OTHERS,     /**< \brief remaining dc scans in successive approximation mode   */
	PROG_AC_FIRST_SCAN, /**< \brief first ac scan in successive approximation mode   */
	PROG_AC_OTHERS      /**< \brief remaining ac scans in successive approximation mode   */

	} t_progressive_scan_type;


	/** \brief struct used to point describe a mcu in progressive mode
	*         in progressive a mcu is either the same on as in sequential
	*         for interleaved dc scans or a simple block in ac scans or 
	*         not interleaved dc scans. For a given not interleaved  scan
	*         we describe only one block of data and we save its offset
	*         in the corresponding frame mcu.
	*/
	typedef struct prog_mcu
	{
	ts_block block; /**< \brief the mcu has just one block in this case   */
	t_uint16   component_offset_in_frame_mcu; /**< \brief offset of the block (ie) in number of blocks relative to the 
	                 *    frame mcu*/
	} ts_prog_mcu, *tps_prog_mcu;





	/** \brief struct information used by huffman sequential decoding */
	typedef struct huff_seq_info
	{
	t_uint16 restart_interval;    /**< \brief restart interval value (number of MCUs)   */
	T_BOOL is_restart_enabled;  /**< \brief flag stating if restart is enabled   */
	t_uint32 scan_mcu_index;      /**< \brief index of the current mcu in the scan   */
	t_uint32 mcus_in_scan;        /**< \brief number of mcus in the scan   */
	tps_mcu_predictor p_mcu_pred;  /**< \brief    pointer on scan mcu predictor */
	/*     t_bool is_error_concealed;  /\**< \brief provision: flag saying an error has been canceled   *\/ */
	/*     t_uint16 downsampling_factor; /\**< \brief downsampling factor used during iq/idct   *\/ */
	/*     t_bool is_block_to_be_replicated;  /\**< \brief flag stating if blocks are to be replicated   *\/ */
	} ts_huff_seq_info, *tps_huff_seq_info;




	/** \brief struct information used by huffman progressive decoding */
	typedef struct huff_prog_info
	{
	t_uint16 restart_interval;    /**< \brief restart interval value (number of MCUs)   */
	T_BOOL is_restart_enabled;  /**< \brief flag stating if restart is enabled   */
	t_uint16 start_spectral_selection;    /**< \brief Ss field of scan header   */
	t_uint16 end_spectral_selection;      /**< \brief Se field of scan header   */
	t_uint16 successive_approx_position_high;     /**< \brief Ah field of scan header   */
	t_uint16 successive_approx_position_low;      /**< \brief Al field of scan header   */
	T_BOOL is_interleaved;      /**< \brief flag stating if the scan is interleaved   */
	t_uint32 scan_mcu_index;      /**< \brief index of the currently processed mcu in the scan   */
	t_uint32 blocks_in_row;       /**< \brief number of blocks in a scan row   */
	t_uint32 blocks_in_col;       /**< \brief number of blocks in a scan column   */
	t_uint32 end_of_band_run;     /**< \brief end of band value   */
	t_ulong_value start_add_coeff_buffer;   /* start address of the coeff buffer */
	t_ulong_value add_mcu_in_coeff_buffer;         /* address of the current block of interest in the coeff buffer */
	t_progressive_scan_type scan_type;  /**< \brief type of the scan   */
	ts_mcu_predictor mcu_pred;          /**< \brief predictor descriptor for the current mcu   */
	tps_mcu p_mcu;                      /**< \brief mcu (frame one) content storage struct pointer   */
	ts_prog_mcu prog_mcu;               /**< \brief mcu (scan one) content storage struct   */
	T_BOOL is_error_concealed;          /**< \brief flag stating if an error was concealed   */
	t_uint32 mcus_in_scan;                /**< \brief number of mcus in the scan   */
	t_uint16 mcus_in_frame_row;           /**< \brief numbers of mcus in a frame row = frame.mcus_in_row   */
	tps_stop_state p_stop_state;        /**< \brief stop state saveing struct pointer (i/o suspension)   */
	/*     t_uint16 downsampling_factor; /\**< \brief downsampling factor used during iq/idct   *\/ */
	/*     t_bool is_block_to_be_replicated;  /\**< \brief flag stating if blocks are to be replicated   *\/ */
	} ts_huff_prog_info, *tps_huff_prog_info;


	/** \brief struct information used by ace processing */
	typedef struct ace_info
	{
	t_uint16 y_blocks_in_frame_row; /**< \brief number of y blocks in a frame row   */
	t_uint16 y_blocks_in_frame_col;/**< \brief number of y blocks in a frame column   */

	t_uint16 mcus_in_frame_row;   /**< \brief number of mcus in a frame row   */
	t_uint16 mcus_in_frame_col;   /**< \brief number of mcus in a frame col   */

	t_uint16 y_v_sampling_factor;
	t_uint16 y_h_sampling_factor;

	tps_analysis p_analysis;

	} ts_ace_info, *tps_ace_info;


	typedef struct partial_block_info
	{
	T_BOOL is_block_needed;
	t_uint16  blk_width; // number of sample 
	t_uint16  blk_height;	
	}ts_partial_block_info, *tps_partial_block_info;


	typedef struct last_mcu_info
	{
	tps_partial_block_info block_info;
	}ts_last_mcu_info, *tps_last_mcu_info;


	typedef struct image_info
	{


	t_uint32 num_last_mcu_in_row;
	t_uint32 num_last_mcu_in_col;

	t_uint32 crop_mcu_in_row; // too be used for crop

	t_uint32 downsampled_block_width;
	t_uint32 downsampled_block_height;

	t_uint32 y_block_offset[MAX_BLOCKS_PER_MB];
	t_uint32 block_offset[MAX_BLOCS_PER_MCU];

	ts_last_mcu_info  last_mcu_info[2][2];
	ts_partial_block_info last_mcu_in_row[MAX_BLOCS_PER_MCU];
	ts_partial_block_info last_mcu_in_col[MAX_BLOCS_PER_MCU];
	ts_partial_block_info last_mcu_in_row_col[MAX_BLOCS_PER_MCU];

	t_uint8 y_hrep;
	t_uint8 y_vrep;

	t_uint8 cb_hrep;
	t_uint8 cb_vrep;

	t_uint8 cr_hrep;
	t_uint8 cr_vrep;

	t_uint32 pix_width;
	t_uint32 pix_width_chroma;

	t_uint32 pix_block_width;
	t_uint32 pix_blkwidth_cb; 
	t_uint32 pix_blkwidth_cr; 

	t_uint32 pix_block_height;
	t_uint32 pix_blkheight_cb;
	t_uint32 pix_blkheight_cr; 

	t_uint32 pix_mcu_width;
	t_uint32 pix_mcu_width_chroma;

	t_uint32 y_buf_offset;
	t_uint32 cb_buf_offset;
	t_uint32 cr_buf_offset;

	t_uint8 * y_buffer_dest;		/* dynamic y mcu destination updation*/
	t_uint8 * cb_buffer_dest;		/* dynamic cb mcu destination updation*/
	t_uint8 *  cr_buffer_dest;		/* dynamic cr mcu destination updation*/
	t_uint8 *  add_destination_buffer; /**< \brief adress of mb destination buffer in host address space   */
	t_uint8*   add_line_buffer;        /**< \brief adress of line buffer in host address space   */
	t_uint8*   add_coeff_buffer;       /**< \brief adress coefficient buffer for progressive mode */	
	} ts_image_info, *tps_image_info;


	typedef struct post_process_info
	{
	t_uint16 mcus_in_frame_row;           /**< \brief numbers of mcus in a frame row   */
	t_uint16 mcus_in_frame_col;           /**< \brief numbers of mcus in a frame row   */
	t_subsampling subsampling;          /**< \brief video format */
	tps_component p_y;                  /**< \brief pointer on component y   */
	tps_component p_cb;                 /**< \brief pointer on component cb   */
	tps_component p_cr;                 /**< \brief pointer on component cr   */

	} ts_post_process_info, *tps_post_process_info; 


/* If long is > 32 bits on your machine, and shifting/masking longs is
* reasonably fast, making bit_buf_type be long and setting BIT_BUF_SIZE
* appropriately should be a win.  Unfortunately we can't define the size
* with something like  #define BIT_BUF_SIZE (sizeof(bit_buf_type)*8)
* because not all machines measure sizeof in 8-bit bytes.
*/

typedef struct {		/* Bitreading state saved across MCUs */
t_sint32 get_buffer;	/* current bit-extraction buffer */
t_sint16 bits_left;		/* # of unused bits in it */
} bitread_perm_state;


typedef struct s_bitstream_buffer_opt 
{
ts_t1xhv_bitstream_buf_pos buf_pos;
ts_t1xhv_bitstream_buf_link  p_buf_link;	
t_uint32  size_in_bits; /**< \brief bitstream size in bits   */
t_uint32  size_in_bytes; /**< \brief bitstream size in bits   */
t_uint16 index_current_bit;
t_uint32 threshold;
bitread_perm_state bitstate;	/* Bit buffer at start of MCU */
t_uint8 * next_input_byte;
t_uint32 bytes_in_buffer;
ts_t1xhv_bitstream_buf_link * latest_buf_link_info;

}ts_bitstream_buffer_opt,*tps_bitstream_buffer_opt;

/** \brief This structure defines the parameters which are used during the frame encoding*/
typedef struct vdc_jpeg_global
{
ts_hamac_info Hamac_info;
ts_bitstream_buffer_opt Hamac_bitstream_buffer_opt;
ts_huff_prog_info Huff_prog_info;
ts_huff_seq_info Huff_seq_info;
ts_ace_info      Ace_info;
ts_post_process_info Post_process_info;
ts_image_info  Image_info;
ts_dc_table dc_table_Y;
ts_ac_table ac_table_Y;
ts_dc_table dc_table_Cb;
ts_ac_table ac_table_Cb;
ts_dc_table dc_table_Cr;
ts_ac_table ac_table_Cr;

t_uint8 * y_buffer_dest;
t_uint8 * cb_buffer_dest;
t_uint8 * cr_buffer_dest;

t_djpeg_error_code (*jpegdec_decode_ptr)(tps_bitstream_buffer_opt, tps_mcu, tps_post_process_info, tps_huff_seq_info, tps_ace_info, tps_hamac_info,tps_image_info,void *);
t_djpeg_error_code (*jpegdec_crop_decode_ptr)(tps_bitstream_buffer_opt, tps_mcu, tps_post_process_info, tps_huff_seq_info, tps_ace_info, tps_hamac_info,tps_image_info,T_BOOL ,void *);
void (*jpegdec_ptr2image)(t_uint8 [][8],t_uint16,tps_image_info,t_uint32,t_uint32 ,t_component_id );
void (*jpegdec_idct_ptr) (tps_block ,t_uint8 [8][8], tps_post_process_info ,t_uint8 * ,t_uint32);
} ts_t1xhv_vdc_jpeg_global;

typedef ts_t1xhv_vdc_jpeg_global *tps_t1xhv_vdc_jpeg_global;

/** \brief This is the main structure*/
typedef struct
{
tps_t1xhv_vdc_frame_buf_in	Gps_vdc_in_frame_buffer;
tps_t1xhv_vdc_frame_buf_out      Gps_vdc_out_frame_buffer;
tps_t1xhv_vdc_internal_buf       Gps_vdc_internal_buffer;
tps_t1xhv_bitstream_buf_pos      Gps_vdc_in_out_bitstream_buffer; 
tps_t1xhv_bitstream_buf_pos      Gps_vdc_out_bitstream_buffer;
tps_t1xhv_vdc_jpeg_param_in     	Gps_vdc_in_parameters;       
tps_t1xhv_vdc_jpeg_param_out    	Gps_vdc_out_parameters;      
tps_t1xhv_vdc_jpeg_param_inout  	Gps_vdc_in_out_frame_parameters; 
tps_t1xhv_vdc_jpeg_param_inout  	Gps_vdc_out_frame_parameters;	
ts_t1xhv_vdc_jpeg_global       Gs_t1xhv_vdc_jpeg_global;

} ts_t1xhv_vdc_jpeg_session;

typedef ts_t1xhv_vdc_jpeg_session *tps_t1xhv_vdc_jpeg_session;

#endif

#endif

