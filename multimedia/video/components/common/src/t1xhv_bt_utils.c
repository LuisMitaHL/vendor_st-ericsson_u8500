/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* Compilation only if BIT_TRUE is defined. */
#ifdef BIT_TRUE


#define BT_GLOBAL /**<\brief To allow global var. from bt utils to be set. */

/**
 * Includes							       
 */
#include <stdlib.h>
#include <stdio.h>

/* Include common FW . */
#include "t1xhv_types.h"
 
#include "mtc_file_api.h"
#include "t1xhv_hw_command.h"
#include "t1xhv_bt_utils.h"
#include "host_interface_common.h"

/**
 * Defines							       
 */

/* Common parameters */

#define T1XHV_BT_ENV_VARIABLE "VIP_DATA" /**<\brief Env. var. to get pf from. */

/* H263 parameters */
#define H263_BT_DEFAULT_OUTPUT_FILE		"bt_h263_out"
#define H263_BT_MTF_IN_VAR_NAME			"MTF_IN"
#define H263_BT_MTF_OUT_VAR_NAME	    "MTF_OUT"

/* Common  parameters */
#define T1XHV_BT_HOST_IN_PARAM_NAME		   "Parameters_in"
#define T1XHV_BT_HOST_IN_FRAME_PARAM_NAME  "Parameters_io_before"
#define T1XHV_BT_HOST_OUT_FRAME_PARAM_NAME "Parameters_io_after"
#define T1XHV_BT_HOST_BUFFER_NAME		   "Init"
#define T1XHV_BT_HOST_HEADER_NAME		   "Header"
#define T1XHV_BT_MTF_NAME				   "Buffer_Header"


/* Encoder parameters */
#define M4E_BT_DEFAULT_INPUT_FILE		"encoder"
#define M4E_BT_DEFAULT_OUTPUT_FILE		"bt_encoder_out"

/* Variable name */
#define M4E_BT_HOST_VAR_NAME			"HostIF"
#define M4E_BT_BPU_IN_VAR_NAME			"BPU_IN"
#define M4E_BT_BPU_OUT_VAR_NAME			"BPU_OUT"
#define M4E_BT_MECC_IN_VAR_NAME			"MECC_IN"
#define M4E_BT_MECC_OUT_VAR_NAME		"MECC_OUT"
#define M4E_BT_CFG_MECC_VAR_NAME		"MECC_CFG"

#define M4E_BT_PXP_REG_DEC_FRAME_NAME	"PXP_ENC_FRAME_CFG"
#define M4E_BT_PXP_REG_ENC_QTZ_PARAM	"PXP_ENC_QTZ_PARAM"
#define M4E_BT_PXP_CMD_MB_NAME			"PXP_MBLOCK_CFG"
#define M4E_BT_PXP_CMD_BLOCK_NAME		"PXP_BLOCK_CFG"
#define M4E_BT_PXP_CMD_PA_NAME			"PXP_PA_CMD"
#define M4E_BT_PXP_QP_VAR_NAME			"PXP_QP"
#define M4E_BT_PXP_QP_LUMA_VAR_NAME		"PXP_QP_L"
#define M4E_BT_PXP_QP_CHRO_VAR_NAME 	"PXP_QP_C"
#define M4E_BT_RLC_VAR_NAME		    	"PXP_ENC_OUT"

/* Decoder parameters */
#define M4D_BT_DEFAULT_INPUT_FILE		"decoder"
#define M4D_BT_DEFAULT_OUTPUT_FILE		"bt_decoder_out"
#define M4D_BT_ENV_VARIABLE				"VIP_DATA"

/* Variable name */
#define M4D_BT_CP_DATA_VAR_NAME			"Compressed_data"
#define M4D_BT_IACDC_RES_VAR_NAME		"PXP_DEC_ACDC_OUT"
#define M4D_BT_IACDC_PRED_VAR_NAME		"PXP_DEC_ACDC_IN"
#define M4D_BT_IACDC_RL_VAR_NAME   		"PXP_BDU_DATA_IN"
#define M4D_BT_MECC_VAR_NAME			"MECC_IN"

#define M4D_BT_PXP_REG_DEC_FRAME_NAME	"PXP_DEC_FRAME_CFG"
#define M4D_BT_PXP_CMD_MB_NAME			M4E_BT_PXP_CMD_MB_NAME
#define M4D_BT_PXP_CMD_BLOCK_NAME		M4E_BT_PXP_CMD_BLOCK_NAME
#define M4D_BT_PXP_CMD_PA_NAME			M4E_BT_PXP_CMD_PA_NAME
#define M4D_BT_PXP_QP_VAR_NAME			"PXP_QP"
#define M4D_BT_PXP_QP_LUMA_VAR_NAME		"PXP_QP_L"
#define M4D_BT_PXP_QP_CHRO_VAR_NAME 	"PXP_QP_C"
#define M4D_BT_PXP_REG_DEC_IQTZ_PARAM	"PXP_DEC_IQTZ_PARAM"
#define M4D_BT_PXP_REG_QP_TABLE         "PXP_QP_TABLE"

/* Packer parameters */
#define MAX_LEN_FIFO 	8192
#define PACKER_FIFO_NB	3

/**
 * External variables
 */
extern int G_flag_ref_compliant; /**<\brief If 1 => PF is compliant to ref. */

/**
 * Global variables                                                      
 */
/** Array for packer : codes (length, value) / buffer info (size) / 
 * array of pointers on buffer
 * to point directly on good buffer depending of the command. 
 */
ts_packer_bt gsa_packer_bt_hdr1[MAX_LEN_FIFO];
ts_packer_bt gsa_packer_bt_hdr2[MAX_LEN_FIFO];
ts_packer_bt gsa_packer_bt_data[MAX_LEN_FIFO];

ts_fifo_packer_bt gs_fifo_hdr1={gsa_packer_bt_hdr1,0};
ts_fifo_packer_bt gs_fifo_hdr2={gsa_packer_bt_hdr2,0};
ts_fifo_packer_bt gs_fifo_data={gsa_packer_bt_data,0};

ts_fifo_packer_bt *gsa_fifo_packer[PACKER_FIFO_NB] = {&gs_fifo_hdr1,&gs_fifo_hdr2,&gs_fifo_data};

void bt_error(char *s) { printf("%s\n",s); exit(0);}

/*****************************************************************************/
/** 
 * INIT       
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * \brief 	Open files for MTC_API
 * \author 	Philippe Rochette
 * \note    Use of default name for files. Call to env variable.
 */
/*****************************************************************************/
void m4d_bt_init_api(void)
{
	printf("init\n");
	/**
	 * Open input file          
	 */
	Gs_t1xhv_bt_api.p_file_in = MTC_file_OpenRead(getenv(M4D_BT_ENV_VARIABLE), 
												  M4D_BT_DEFAULT_INPUT_FILE,NULL);
	if ( Gs_t1xhv_bt_api.p_file_in == (t_pt_file_mgt)NULL) {
		bt_error("m4d_bt_init_api(): Can't open input file \n");
	}
		
	/**
     * Initialize IDs 
     */
 	Gs_t1xhv_bt_api.id_param_in = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_IN_PARAM_NAME);
	if(Gs_t1xhv_bt_api.id_param_in == -1) {
		bt_error("m4d_bt_init_api(): No Host input parameters available\n");
		exit(1);
	}

 	Gs_t1xhv_bt_api.id_param_inout = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_IN_FRAME_PARAM_NAME);
	if(Gs_t1xhv_bt_api.id_param_inout == -1) {
		bt_error("m4d_bt_init_api(): No Host inout parameters available\n");
		exit(1);
	}

 	Gs_t1xhv_bt_api.id_buffer    = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_BUFFER_NAME);
	if(Gs_t1xhv_bt_api.id_buffer == -1) {
		bt_error("m4d_bt_init_api(): No Init provided\n");
		exit(1);
	}

 	Gs_t1xhv_bt_api.id_cp_data = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   M4D_BT_CP_DATA_VAR_NAME);

 	Gs_t1xhv_bt_api.id_iacdc_res = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   M4D_BT_IACDC_RES_VAR_NAME);


	/**
     * Open output file         
	 */
 	Gs_t1xhv_bt_api.p_file_out = MTC_file_OpenWrite(getenv(M4D_BT_ENV_VARIABLE), 
													  M4D_BT_DEFAULT_OUTPUT_FILE,2);
	if ( Gs_t1xhv_bt_api.p_file_out == (t_pt_file_mgt)NULL) {
		bt_error("m4d_bt_init_api(): Can't open output file \n");
	}
	
	/**
	 * Initialize (IDs, structure and timer )
     */
    /** MECC configuration */
 	Gs_t1xhv_bt_api.id_mecc_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
                                                  M4E_BT_CFG_MECC_VAR_NAME,
                                                  MTC_ID_CONFIG_MECC);


	/** IACDC Pred */
 	Gs_t1xhv_bt_api.id_iacdc_pred = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 M4D_BT_IACDC_PRED_VAR_NAME,
												 MTC_ID_TABINT16);
	Gs_t1xhv_bt_api.iacdc_pred.nb=0;

	/** RL Event */
 	Gs_t1xhv_bt_api.id_rld = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 M4D_BT_IACDC_RL_VAR_NAME,
												 MTC_ID_RL_EVENT);
	Gs_t1xhv_bt_api.rld.nb_evt=0;
	Gs_t1xhv_bt_api.rld.block_number=0;
	Gs_t1xhv_bt_api.rld.block_type=0;

	/** MECC command */
 	Gs_t1xhv_bt_api.id_mecc_in = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 M4D_BT_MECC_VAR_NAME,
												 MTC_ID_TO_MECC);
	Gs_t1xhv_bt_api.mecc_in.nb_mv=0;
	Gs_t1xhv_bt_api.mecc_in.command=0;
	Gs_t1xhv_bt_api.mecc_in.decision=0;

	/** Regs */
	/** Frame Cfg */
 	Gs_t1xhv_bt_api.id_pxp_reg_frame_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_REG_DEC_FRAME_NAME,
													   MTC_ID_VAL);
	Gs_t1xhv_bt_api.pxp_reg_frame_cfg.value=0;

	/** Macroblock Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_mb_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_CMD_MB_NAME,
													   MTC_ID_VAL);

	/** Block Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_block_cfg = MTC_file_AddVar( Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_CMD_BLOCK_NAME,
													   MTC_ID_VAL);

	/** Post-Adder Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_pa_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_CMD_PA_NAME,
													   MTC_ID_VAL);

	/** QP Luma */
 	Gs_t1xhv_bt_api.id_pxp_fifo_qp_luma = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_QP_LUMA_VAR_NAME,
													   MTC_ID_VAL);
	/** QP Chroma */
 	Gs_t1xhv_bt_api.id_pxp_fifo_qp_chroma = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_QP_CHRO_VAR_NAME,
													   MTC_ID_VAL);

	/** QP Inter */
 	Gs_t1xhv_bt_api.id_pxp_fifo_enc_qp = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_QP_VAR_NAME,
													   MTC_ID_VAL);

	/** IQTZ param */
 	Gs_t1xhv_bt_api.id_pxp_reg_iqtz_param = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_REG_DEC_IQTZ_PARAM,
													   MTC_ID_VAL);

	/** QP table param */
 	Gs_t1xhv_bt_api.id_pxp_reg_qp_table = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4D_BT_PXP_REG_QP_TABLE,
													   MTC_ID_VAL);
	/** Timer */
	Gs_t1xhv_bt_api.time_out=0;

	/** Bitstream */
    Gs_t1xhv_bt_api.bitstream_flag=0;

}

/*****************************************************************************/
/**
 * \brief 	Open files for MTC_API for encoder
 * \author 	Philippe Rochette
 * \none    Use of default name for files. Call to env variable.
 */
/*****************************************************************************/
void m4e_bt_init_api(void)
{
	/**
	 * Open input file          
	 */
	Gs_t1xhv_bt_api.p_file_in = MTC_file_OpenRead(getenv(T1XHV_BT_ENV_VARIABLE), 
												M4E_BT_DEFAULT_INPUT_FILE,
                                                NULL);

	if ( Gs_t1xhv_bt_api.p_file_in == (t_pt_file_mgt)NULL) {
		bt_error("m4e_bt_init_api(): Can't open input file \n");
	}
		
	/** 
     * Initialise IDs 
     */
 	Gs_t1xhv_bt_api.id_param_in  = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_IN_PARAM_NAME);

	if(Gs_t1xhv_bt_api.id_param_in == -1) {
		bt_error("m4e_bt_init_api(): No Host input parameters available\n");
		exit(1);
	}

 	Gs_t1xhv_bt_api.id_param_inout = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_IN_FRAME_PARAM_NAME);


 	Gs_t1xhv_bt_api.id_buffer    = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_HOST_HEADER_NAME);

	if(Gs_t1xhv_bt_api.id_param_inout == -1) {
		bt_error("m4e_bt_init_api(): No Host inout parameters available\n");
		exit(1);
	}

	if(Gs_t1xhv_bt_api.id_buffer == -1) {
		bt_error("m4e_bt_init_api(): No Init provided\n");
		exit(1);
	}

	Gs_t1xhv_bt_api.id_mtf    = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   T1XHV_BT_MTF_NAME);

 	Gs_t1xhv_bt_api.id_mecc_out = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   M4E_BT_MECC_OUT_VAR_NAME);

 	Gs_t1xhv_bt_api.id_packer_out = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   M4E_BT_BPU_OUT_VAR_NAME);

 	Gs_t1xhv_bt_api.id_rlc = MTC_file_GetId(Gs_t1xhv_bt_api.p_file_in,  
											   M4E_BT_RLC_VAR_NAME);

	/**
	 * Open output file         
	 */
 	Gs_t1xhv_bt_api.p_file_out = MTC_file_OpenWrite(getenv(T1XHV_BT_ENV_VARIABLE), 
													  M4E_BT_DEFAULT_OUTPUT_FILE,2);
	if ( Gs_t1xhv_bt_api.p_file_out == (t_pt_file_mgt)NULL) {
		bt_error("m4e_bt_init_api(): Can't open output file \n");
	}
	
	/**
     * Initialize (IDs, structure and timer )
     */
	/** MECC configuration */
 	Gs_t1xhv_bt_api.id_mecc_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
                                                  M4E_BT_CFG_MECC_VAR_NAME,
                                                  MTC_ID_CONFIG_MECC);

	/** MECC MV command */
 	Gs_t1xhv_bt_api.id_mecc_in = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 M4E_BT_MECC_IN_VAR_NAME,
												 MTC_ID_TO_MECC);

	Gs_t1xhv_bt_api.mecc_in.nb_mv=0;
	Gs_t1xhv_bt_api.mecc_in.command=0;
	Gs_t1xhv_bt_api.mecc_in.decision=0;

	/** Packer  command */
 	Gs_t1xhv_bt_api.id_packer_in = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 M4E_BT_BPU_IN_VAR_NAME,
												 MTC_ID_PACKER);

	/**
     * PXP Registers
     */
	/** Frame Cfg */
 	Gs_t1xhv_bt_api.id_pxp_reg_frame_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_REG_DEC_FRAME_NAME,
													   MTC_ID_VAL);
	Gs_t1xhv_bt_api.pxp_reg_frame_cfg.value=0;

	/** Enc QTZ param */
 	Gs_t1xhv_bt_api.id_pxp_reg_qtz_param = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_REG_ENC_QTZ_PARAM,
													   MTC_ID_VAL);
	Gs_t1xhv_bt_api.pxp_reg_frame_cfg.value=0;

	/** Macroblock Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_mb_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_CMD_MB_NAME,
													   MTC_ID_VAL);

	/** Block Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_block_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_CMD_BLOCK_NAME,
													   MTC_ID_VAL);

	/** Post-Adder Cfg */
 	Gs_t1xhv_bt_api.id_pxp_fifo_pa_cfg = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_CMD_PA_NAME,
													   MTC_ID_VAL);

	/** QP Luma */
 	Gs_t1xhv_bt_api.id_pxp_fifo_qp_luma = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_QP_LUMA_VAR_NAME,
													   MTC_ID_VAL);
	/** QP Chroma */
 	Gs_t1xhv_bt_api.id_pxp_fifo_qp_chroma = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_QP_CHRO_VAR_NAME,
													   MTC_ID_VAL);

	/** QP Inter */
 	Gs_t1xhv_bt_api.id_pxp_fifo_enc_qp = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
													   M4E_BT_PXP_QP_VAR_NAME,
													   MTC_ID_VAL);

	/** Timer */
	Gs_t1xhv_bt_api.time_out=0;

	/** Bits in Bitstream (updated by packer) */
    Gs_t1xhv_bt_api.bitstream_bits = 0;

}

/*****************************************************************************/
/**
 * \brief 	Open files for MTC_API for h263 demo
 * \author 	Serge Backert
 * \warning Used only for H263 demo
 * \note    Use of default name for files. Call to env variable.
 * 
 */
/*****************************************************************************/
void h263_bt_init_api(void)
{

	/**
	 * Open output file         
	 */
 	Gs_t1xhv_bt_api.p_file_out = MTC_file_OpenWrite(getenv(T1XHV_BT_ENV_VARIABLE), 
													H263_BT_DEFAULT_OUTPUT_FILE,
                                                    16);
	if ( Gs_t1xhv_bt_api.p_file_out == (t_pt_file_mgt)NULL) {
		bt_error("h263_bt_init_api(): Can't open output file \n");
	}
	
	/**
     * Initialise (IDs, structure and timer )
     */
	/** MTF_IN  */
 	Gs_t1xhv_bt_api.id_mtf_in = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
                                                H263_BT_MTF_IN_VAR_NAME,
                                                MTC_ID_TABINT16);
	Gs_t1xhv_bt_api.mtf_in.nb=0;

	/** MTF_OUT */
 	Gs_t1xhv_bt_api.id_mtf_out = MTC_file_AddVar(Gs_t1xhv_bt_api.p_file_out,  
												 H263_BT_MTF_OUT_VAR_NAME,
												 MTC_ID_TABINT16);

	Gs_t1xhv_bt_api.mtf_out.nb=0;

}

/*****************************************************************************/
/**
 * \brief 	Close files for MTC_API
 * \author 	Serge Backert
 * \param 	flag_short_header
 * \param 	ac_pred_flag
 * \param 	data_partitioned
 * \param 	reversible_vlc
 * \param 	image_width
 * \param 	image_height
 * \param 	frame_number
 *  
 */
/*****************************************************************************/
void t1xhv_bt_close_api(int flag_short_header, 
                        int ac_pred_flag,
                        int data_partitioned, 
                        int reversible_vlc,
                        int image_width, 
                        int image_height, 
                        int frame_number)
{
	int option;
    
	option=MTC_NO_OPTION |
		(flag_short_header?MTC_SHORT_HEAD:0) |
		(ac_pred_flag?MTC_ACDC_PRED:0) |
		(data_partitioned?MTC_DATA_PARTITION:0) |
		(reversible_vlc?MTC_RVLC:0);

	/** Initialize Header of output file */
	MTC_file_SetHeader(Gs_t1xhv_bt_api.p_file_out,
					   "Created with bit true version of firmware",	/* comment 			   */
					   getenv(T1XHV_BT_ENV_VARIABLE),		/* name of sequence 		   */
					   MTC_MPEG4,							/* Norme 					   */
					   option,								/* Options 					   */
					   MTC_YUV420,							/* Type of block (default) 	   */
					   (image_width==352)?MTC_CIF:MTC_QCIF,	/* Format 					   */
					   MTC_15HZ,							/* Framerate (not known) 	   */
					   frame_number,						/* Number of frame in the file */
					   image_width,							/* Width 					   */
					   image_height,						/* Height 					   */
					   0);									/* Not used 				   */

	MTC_file_Close(Gs_t1xhv_bt_api.p_file_in);
	MTC_file_Close(Gs_t1xhv_bt_api.p_file_out);
}

/*****************************************************************************/
/**
 * \brief 	Close files for MTC_API 
 * \author 	Serge Backert
 * \warning Used by H263 demo only
 */
/*****************************************************************************/
void h263_bt_close_api(void)
{
	/** Initialize Header of output file */
	MTC_file_SetHeader(Gs_t1xhv_bt_api.p_file_out,
					   "Created with h263 codec from AST",	/* comment 				   */
					   getenv(T1XHV_BT_ENV_VARIABLE),		/* name of sequence 	   */
					   MTC_H263,							/* Norme 				   */
					   MTC_NO_OPTION,						/* Options 				   */
					   MTC_YUV420,							/* Type of block (default) */
					   MTC_QCIF,							/* Format 				   */
					   MTC_15HZ,							/* Framerate (not known)   */
					   0,						/* Number of frame in the file 	*/
					   176,						/* Width 						*/
					   144,						/* Height 						*/
					   0);						/* Not used 					*/

	MTC_file_Close(Gs_t1xhv_bt_api.p_file_out);
}


/*****************************************************************************/
/** 
 * READ
 */
/*****************************************************************************/

/*****************************************************************************/
/** 
 * READ HOST
 */
/*****************************************************************************/


int t1xhv_bt_read_host(int id, int nb, int addr)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : none
 * Read DBR struct from input pf file
 *-----------------------------------------------------------------------------
 * Fill global variable gp_host_reg
 * return 0 if end of simulation has occured
 * else return 1.
 ******************************************************************************/
{
    t_pt_dval_tabint16 ps_parameters;
    t_uint16 *p_out = (t_uint16 *)addr;
    t_uint16 *p_in;
    
    /* Read data */
	ps_parameters =
        (t_pt_dval_tabint16)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
                                            id,
                                            &Gs_t1xhv_bt_api.time_in);  

    /* If read is NULL => End of file => End of simulation */
    if(ps_parameters == (t_pt_dval_tabint16)NULL) {
        printf("Read Host No more values - Simulation ending\n");
            memset(p_out, 0, nb);

        return 0;	   
    }

    p_in = (t_uint16 *)&ps_parameters->value;

    memcpy(p_out, p_in, nb);

    return 1;
}


/*****************************************************************************/
int t1xhv_bt_read_mtf(int id, int nb, t_uint16 *ptr)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : none
 * Read DBR struct from input pf file
 *-----------------------------------------------------------------------------
 * Fill global variable gp_host_reg
 * return 0 if end of simulation has occured
 * else return 1.
 ******************************************************************************/
{
    t_pt_dval_tabbyte ps_parameters;
    t_uint16 *p_in;

    static t_uint16 nb_read = 0;

    /** Read data the first time only, made necessary since HV_FW_2.4.8.3
     * because of the use of the MTF in sevral 4 32bit bursts
     */
    if(nb_read<=0)
    {
        ps_parameters =
            (t_pt_dval_tabbyte)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
                                                id,
                                                &Gs_t1xhv_bt_api.time_in);  

        /* If read is NULL => End of file => End of simulation */
        if(ps_parameters == (t_pt_dval_tabbyte)NULL) {
            printf("Read MTF No more values - Simulation ending\n");
            memset(ptr, 0, nb);

            return 0;	   
        }

        nb_read = ps_parameters->nb;
/*         printf("Debug: Nb read = %d\n",nb_read); */

        p_in = (t_uint16 *)&ps_parameters->value;

    } /* if */

    if(nb_read>=16)
    {
        memcpy(ptr,p_in,16);
        nb_read -= 16;
        p_in += 8;
    } 
    else
    {
        memcpy(ptr,p_in,nb_read);
        nb_read = 0;
    }

/*     {int i; */
/*     for(i=0;i<nb;i++) */
/*     { */
/*     printf("Debug bt_utils: %X\n",*(p_in+i)); */
/*     } */
/*     } */

    return 1;
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * READ RLC
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
void m4e_bt_read_rlc(t_uint16 *last, t_uint16 *run, t_sint16 *level)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : last, run, level = RL event.
 * Get a run level event from the pf file.
 *-----------------------------------------------------------------------------
 * Use MTC_API
 ******************************************************************************/
{
    static int nb_event=0;
    static int index;

    if(nb_event==0) {
        /* Read data if no more event */
        Gs_t1xhv_bt_api.p_rlc =
			(t_pt_dval_rl_event)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
												 Gs_t1xhv_bt_api.id_rlc,
												 &Gs_t1xhv_bt_api.time_in);  

        if(Gs_t1xhv_bt_api.p_rlc == (t_pt_dval_rl_event) NULL) {
            printf("Error, no more RLC in file\n");
            exit(1);
        }
        nb_event=Gs_t1xhv_bt_api.p_rlc->nb_evt;

        index=0;
    }
    /* Read 1 event */
    nb_event--;
    
    *last  = Gs_t1xhv_bt_api.p_rlc->last[index];
    *run   = Gs_t1xhv_bt_api.p_rlc->run[index];
    *level = Gs_t1xhv_bt_api.p_rlc->level[index];

    index++;

}



/*****************************************************************************/
void t1xhv_bt_read_clip_out(t_sint16 *value)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : value of clip
 *-----------------------------------------------------------------------------
 ******************************************************************************/
{
    Gs_t1xhv_bt_api.p_pxp_reg_clip_out =
	    (t_pt_dval)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
									Gs_t1xhv_bt_api.id_pxp_reg_clip_out,
									&Gs_t1xhv_bt_api.time_in);  

    if(Gs_t1xhv_bt_api.p_pxp_reg_clip_out == (t_pt_dval) NULL) {
        printf("Error, no more clip out in file\n");
        exit(1);
    }
    *value= Gs_t1xhv_bt_api.p_pxp_reg_clip_out->value;
 
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * READ BITSTREAM
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
t_uint16 m4d_bt_read_bitstream(void)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : value read from compressed data fifo
 * Use MTC_API to read from the Data compressed fifo
 *-----------------------------------------------------------------------------
 * Data is on 16 bits. 
 * if Gs_t1xhv_bt_api.bitstream_flag is set,
 *	Read next cp_data. Status is returned instead of value
 ******************************************************************************/
{
    static t_uint16 *p_value;
    static t_uint16 nb=0;
    t_uint16 value;

	/* If null variable (first time call) or all values have
	 * been read, load next value from file. */
    if(Gs_t1xhv_bt_api.bitstream_flag)	{
	    Gs_t1xhv_bt_api.p_tabint16=
			(t_pt_dval_tabint16)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
												Gs_t1xhv_bt_api.id_cp_data,
												&Gs_t1xhv_bt_api.time_in);  
	    if(Gs_t1xhv_bt_api.p_tabint16==(t_pt_dval_tabint16)NULL) {	   
			printf("Read Bitstream : No more value in bitstream - Simulation ending\n");
			return 0; 
	    } 

		/* Init pointer */
	    p_value=(t_uint16 *)&Gs_t1xhv_bt_api.p_tabint16->value;
		nb=0; 
             printf("Updated structure %.4x\n", *p_value);
       
        return 1;
	} else {
    
        if((Gs_t1xhv_bt_api.p_tabint16==(t_pt_dval_tabint16)NULL) 
           || (nb==(Gs_t1xhv_bt_api.p_tabint16->nb/2))) {
            printf("Dummy value\n");
            value = 0x0000;
        } else {
            /*Read value */
            value=p_value[nb++]&0xFFFF;
        }
    }

    return  value;          
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * READ IACDC
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
void m4d_bt_read_iacdc_res(t_sint16 *value)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : predicted value read from iacdc
 * Use MTC_API to read from the Data compressed fifo
 *-----------------------------------------------------------------------------
 ******************************************************************************/
{
	static t_int16 *p_value;
    static t_uint16 nb=0;

	/* If null variable (first time call) or all values have
	 * been read, load next value from file. */
    if((Gs_t1xhv_bt_api.p_iacdc_res==(t_pt_dval_tabint16)NULL) 
	   || (nb==Gs_t1xhv_bt_api.p_iacdc_res->nb))
	{
	    Gs_t1xhv_bt_api.p_iacdc_res=
			(t_pt_dval_tabint16)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
												Gs_t1xhv_bt_api.id_iacdc_res,
												&Gs_t1xhv_bt_api.time_in);  
	    if(Gs_t1xhv_bt_api.p_iacdc_res==(t_pt_dval_tabint16)NULL) {	   
			*value=0;
	    }

		/* Init pointer */
	    p_value=(t_int16 *)&Gs_t1xhv_bt_api.p_iacdc_res->value;
		nb=0; 
	}

	/*Read predicted value */
    *value=p_value[nb++];
 
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * READ MECC
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
t_sint16 t1xhv_bt_read_mecc(t_uint16 address)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : selected value 
 * Manage structure mecc_out
 *-----------------------------------------------------------------------------
 ******************************************************************************/
{
/* 	static t_int16 *p_value; */
/*     static t_uint16 nb=0; */
/*     t_uint16 value; */

    /* Case on address access : */
    switch(address) {
    case 0xFFF:  /* Load next value from file */
	    Gs_t1xhv_bt_api.p_mecc_out = 
			(t_pt_dval_from_mecc)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
                                                  Gs_t1xhv_bt_api.id_mecc_out,
                                                  &Gs_t1xhv_bt_api.time_in);  
	    if(Gs_t1xhv_bt_api.p_mecc_out==(t_pt_dval_from_mecc)NULL) {	   
#ifdef M4E_DEBUG
            printf("M4E Warning: No more value in MECC_OUT\n");
#endif
	    }
        return 0; 

        break;

    case MECC_REG_MEAN:
        return Gs_t1xhv_bt_api.p_mecc_out->mean;
        break;

    case MECC_REG_MAD:
        return Gs_t1xhv_bt_api.p_mecc_out->mad;
        break;

    case MECC_REG_SAD:
        return Gs_t1xhv_bt_api.p_mecc_out->sad;
        break;

    case MECC_REG_PCX:
        return Gs_t1xhv_bt_api.p_mecc_out->pc_out[0];
        break;

    case MECC_REG_PCY:
        return Gs_t1xhv_bt_api.p_mecc_out->pc_out[1];
        break;

    case MECC_REG_SAD_DELAYED:
        return Gs_t1xhv_bt_api.p_mecc_out->sad_d;
        break;

    case MECC_REG_PCX_DELAYED:
        return Gs_t1xhv_bt_api.p_mecc_out->pc_out_d[0];
        break;

    case MECC_REG_PCY_DELAYED:
        return Gs_t1xhv_bt_api.p_mecc_out->pc_out_d[1];
        break;

    case MECC_REG_LAST_SAD:
        return Gs_t1xhv_bt_api.p_mecc_out->last_sad;
        break;

    default:
#ifdef M4E_DEBUG
        printf("M4E Warning: Bad address in read mecc %d\n", address);
#endif
        break;
    }
    return 0;
        
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------
  ---------------------------     WRITE      ---------------------------
  ----------------------------------------------------------------------
  ----------------------------------------------------------------------
*/

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE MTF							       
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/*****************************************************************************/
void t1xhv_bt_write_mtf_to_file(int id_mtf, t_dval_tabint16 *ps_mtf, t_uint16 nb, t_uint16 *p_value)
/*-----------------------------------------------------------------------------
 * IN  : cmd = Fifo to be written
 *       length = Number of bits of the value
 *		 vallue = value to be written in the fifo.
 * OUT : none
 * Write a command to the packer, send to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * length must be < 16 as XBUS is on 16 bits.
 ******************************************************************************/
{
    /* Store values in structure */
    memcpy((void *)(ps_mtf->value+ps_mtf->nb), (void *)p_value, nb*2);	
    ps_mtf->nb    	+= nb;

    if(ps_mtf->nb > 6000) {
        /* Update time */
        BT_ADD_TIME(1);
       
        /* Write to File */
        MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, id_mtf, 
                       (void *)ps_mtf, Gs_t1xhv_bt_api.time_out);
    ps_mtf->nb    	= 0;
    }
}


/*****************************************************************************/
void t1xhv_bt_write_last_mtf_to_file(int id_mtf, t_dval_tabint16 *ps_mtf)
/*-----------------------------------------------------------------------------
 * IN  : cmd = Fifo to be written
 *       length = Number of bits of the value
 *		 vallue = value to be written in the fifo.
 * OUT : none
 * Write a command to the packer, send to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * length must be < 16 as XBUS is on 16 bits.
 ******************************************************************************/
{
    if(ps_mtf->nb>0) {
        /* Update time */
        BT_ADD_TIME(1);
     
        /* Write to File */
         MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, id_mtf, 
                       (void *)ps_mtf, Gs_t1xhv_bt_api.time_out);
        
        ps_mtf->nb    	= 0;
    }
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE PACKER							       
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/*****************************************************************************/
void m4e_bt_write_packer_to_file(t_uint16 cmd, t_uint16 length, t_uint16 value)
/*-----------------------------------------------------------------------------
 * IN  : cmd = Fifo to be written
 *       length = Number of bits of the value
 *		 vallue = value to be written in the fifo.
 * OUT : none
 * Write a command to the packer, send to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * length must be < 16 as XBUS is on 16 bits.
 ******************************************************************************/
{

        /* Store values in structure */
        Gs_t1xhv_bt_api.packer_in.cmd    = cmd;
        Gs_t1xhv_bt_api.packer_in.length = length;
        Gs_t1xhv_bt_api.packer_in.value  = value;

       /* Update time */
        BT_ADD_TIME(1);

       /* Write to File */
        MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_packer_in, 
                       (void *)&Gs_t1xhv_bt_api.packer_in, Gs_t1xhv_bt_api.time_out);
}

/*****************************************************************************/
t_uint16 m4e_bt_write_packer_data(t_uint16 cmd,t_uint16 length,t_uint32 value)
/*-----------------------------------------------------------------------------
 * IN  : cmd = Fifo to be written
 *       length = Number of bits of the value
 *		 vallue = value to be written in the fifo.
 * OUT : none
 * Write a command to the packer
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * If length > 16, command is splitted as XBUS is on 16 bits.
 * If reference must be compared, reorder command, by storing value in internal
 * fifo. Send to file when flush command is sent. (Flush command is not sent to file).
 * Else, store directly in file.
 * Returns allways 0 as there is no notion of offset in "bit-true", this is to 
 * have prototype compatibility with msim version
 ******************************************************************************/
{
    if(length>16) { /* Special case, 2 commands are generated */
        m4e_bt_write_packer_data(cmd, length-16, value>>16); /* MSB */
        m4e_bt_write_packer_data(cmd, 16, value);	 /* LSB */

    } else {

        value &= MASK(length);
        
        if(!G_flag_ref_compliant) {        
            if(cmd!=BPU_CTRL_FLUSH) {
                /* Write to File */
                m4e_bt_write_packer_to_file(cmd, length, value);

                /* Update bit count. */
                Gs_t1xhv_bt_api.bitstream_bits += length;
            }

        } else {
            /* Must store in three buffer (as in reversible mode) but send to file in
             * correct order (buffer header, then buffer data) */
            if(cmd==BPU_CTRL_FLUSH) { /**/
                int buffer_index; /* indexes for loop */
                int code_index;
                ts_fifo_packer_bt *ps_fifo; /* pointer on current buffer. */

                /* Loop on all buffer in correct order */
                for(buffer_index=0; buffer_index<PACKER_FIFO_NB; buffer_index++) {

                    /* Get pointer on current buffer */
                    ps_fifo=gsa_fifo_packer[buffer_index];

                    /* Loop on all codes of current buffer*/
                    for(code_index=0; code_index < ps_fifo->nb_elt; code_index++) {

                        /* Write current code (command is 0.) */
                        m4e_bt_write_packer_to_file(buffer_index,
                                                    ps_fifo->ps_packer_bt[code_index].length,
                                                    ps_fifo->ps_packer_bt[code_index].value);
                    }

                    /* Init current buffer. */
                    ps_fifo->nb_elt=0;
                }
            } else { /* Store current values in buffer indicated by cmd. */
                ts_fifo_packer_bt *ps_fifo=gsa_fifo_packer[cmd];

                if(ps_fifo->nb_elt < MAX_LEN_FIFO) {
                    ps_fifo->ps_packer_bt[ps_fifo->nb_elt].length = length;
                    ps_fifo->ps_packer_bt[ps_fifo->nb_elt].value  = value;
                    ps_fifo->nb_elt++;
                
                    /* Update bit count. */
                    Gs_t1xhv_bt_api.bitstream_bits += length;

                } else {
#ifdef M4E_DEBUG
                    printf("m4e_warning: Fifo %d Overflow\n",cmd);
#endif
                }
            }
        } /* COMPARE_WITH_REF */

    }
    return 0;
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE MECC
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
void t1xhv_bt_write_mecc_command(t_uint16 command)
/*-----------------------------------------------------------------------------
 * IN  : command = command to write
 * OUT : none
 * Write command to MECC structure
 *-----------------------------------------------------------------------------
 * Just update structure. Save will be done with the last MV
 ******************************************************************************/
{
	Gs_t1xhv_bt_api.mecc_in.command=command;

    BT_ADD_TIME(1);

}


/*****************************************************************************/
void t1xhv_bt_write_mecc_mv_x(t_sint16 mv_x)
/*-----------------------------------------------------------------------------
 * IN  : mv_x = X comp. of Motion Vector
 * OUT : none
 * Write X composante Motion Vector to MECC structure
 *-----------------------------------------------------------------------------
 * Save will be done with the last MV Y
 ******************************************************************************/
{
	Gs_t1xhv_bt_api.mecc_in.pc_in[Gs_t1xhv_bt_api.mecc_in.nb_mv][0]=mv_x;

    BT_ADD_TIME(1);
}


/*****************************************************************************/
void t1xhv_bt_write_mecc_mv_y(t_sint16 mv_y)
/*-----------------------------------------------------------------------------
 * IN  : mv_y = Y comp. of Motion Vector
 * OUT : none
 * Write Y composante Motion Vector to MECC structure
 *-----------------------------------------------------------------------------
 * Save will be done with the last MV 
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.mecc_in.pc_in[Gs_t1xhv_bt_api.mecc_in.nb_mv++][1]=mv_y;

    BT_ADD_TIME(1);


}


/*****************************************************************************/
void t1xhv_bt_save_mecc_mv(void)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : none
 * save to file mv
 *-----------------------------------------------------------------------------
 ******************************************************************************/
{
		MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_mecc_in, 
					   (void *)&Gs_t1xhv_bt_api.mecc_in, Gs_t1xhv_bt_api.time_out);

		Gs_t1xhv_bt_api.mecc_in.nb_mv=0;
	
}

/*****************************************************************************/
void t1xhv_bt_write_mecc_rnd(t_uint16 rnd)
/*-----------------------------------------------------------------------------
 * IN  : rnd = Rounding information for half pixel interpolation
 * OUT : none
 * Write rnd to mecc_cfg structure
 *-----------------------------------------------------------------------------
 * Save will be done with stx_stab
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.mecc_cfg.rnd=rnd;

    BT_ADD_TIME(1);
}

/*****************************************************************************/
void t1xhv_bt_write_mecc_image_width(t_uint16 iw)
/*-----------------------------------------------------------------------------
 * IN  : iw = Image Width
 * OUT : none
 * Write Image Width to mecc_cfg structure
 *-----------------------------------------------------------------------------
 * Save will be done with stx_stab
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.mecc_cfg.image_width=iw;

    BT_ADD_TIME(1);
}


/*****************************************************************************/
void t1xhv_bt_write_mecc_scx_stab(t_uint16 scx_stab)
/*-----------------------------------------------------------------------------
 * IN  : scx_stab = Horiz. stabilization coordinates
 * OUT : none
 * Write scx_stab to mecc_cfg structure
 *-----------------------------------------------------------------------------
 * Save is done to file .pf
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.mecc_cfg.scx_stab=scx_stab;

/*     BT_ADD_TIME(1); */

/*     MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_mecc_cfg,  */
/*                    (void *)&Gs_t1xhv_bt_api.mecc_cfg, Gs_t1xhv_bt_api.time_out); */

}

/*****************************************************************************/
void t1xhv_bt_write_mecc_scy_stab(t_uint16 scy_stab)
/*-----------------------------------------------------------------------------
 * IN  : scy_stab = Vert. stabilization coordinates
 * OUT : none
 * \todo Update t_dval_config_mecc with scy
 * Write scy_stab to mecc_cfg structure

*-----------------------------------------------------------------------------
 * Save is done to file .pf

******************************************************************************/
{
 /*     Gs_t1xhv_bt_api.mecc_cfg.scy_stab=scy_stab;  */
 /*    BT_ADD_TIME(1); */
/*     MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, */
/*                    Gs_t1xhv_bt_api.id_mecc_cfg,  */
/*                    (void *)&Gs_t1xhv_bt_api.mecc_cfg, */
/*                    Gs_t1xhv_bt_api.time_out); */
} 

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE IACDC
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
void m4d_bt_set_rl(t_uint16 block_number)
/*-----------------------------------------------------------------------------
 * IN  : block_number = position in MB (0-6 <=> Y1-Y4,U,V)
 * OUT : none
 * Write RL event to IAC_DC prediction
 *-----------------------------------------------------------------------------
 * Use MTC_API
 ******************************************************************************/
{
	/* Store values */
    block_number=0;
	Gs_t1xhv_bt_api.rld.block_number=block_number;
}

/*****************************************************************************/
void m4d_bt_write_rl(t_uint16 last, t_uint16 run, t_sint16 level)
/*-----------------------------------------------------------------------------
 * IN  : last      - Flag in case of last coefficient
 *       run       - nb of 0 value before level
 *       level     - level of current coefficient
 *       luma      - if =1 current block is luma, else chroma
 * OUT : none
 * Write RL event to IAC_DC prediction
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Remove dummy block if comparison is done with reference code...
 ******************************************************************************/
{
    if((last!=1) || (run!=0) || (level!=0) || (G_flag_ref_compliant == 0)) {

        /* Store values */
        Gs_t1xhv_bt_api.rld.run[Gs_t1xhv_bt_api.rld.nb_evt]=run;
        Gs_t1xhv_bt_api.rld.level[Gs_t1xhv_bt_api.rld.nb_evt]=level;
        Gs_t1xhv_bt_api.rld.last[Gs_t1xhv_bt_api.rld.nb_evt]=last;
        
        /* Update size of event */
        Gs_t1xhv_bt_api.rld.nb_evt++;
        
        /* Update time */
        Gs_t1xhv_bt_api.time_out++;
        
        if(last || (Gs_t1xhv_bt_api.rld.nb_evt == 64)) { /* Save and init structure rld, 
                                                            error if nb_evt==64 and no last... */
            MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_rld, 
                           (void *)&Gs_t1xhv_bt_api.rld, Gs_t1xhv_bt_api.time_out);
            Gs_t1xhv_bt_api.rld.nb_evt=0;
        }

    }
}

/*****************************************************************************/
void m4d_bt_write_iacdc_pred(t_sint16 coeff)
/*-----------------------------------------------------------------------------
 * IN  : coeff     - Value of the predictor (DC must be send first, then AC)
 * OUT : none
 * Write Predictor to IAC_DC prediction
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	static t_int16 value[8];

	/* Store values & update size of table  */
	value[Gs_t1xhv_bt_api.iacdc_pred.nb++]=coeff;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* 8 coeffs needed Save and init structure */
	if(Gs_t1xhv_bt_api.iacdc_pred.nb==8) {
		Gs_t1xhv_bt_api.iacdc_pred.value=value;
		MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_iacdc_pred, 
					   (void *)&Gs_t1xhv_bt_api.iacdc_pred, Gs_t1xhv_bt_api.time_out);
		Gs_t1xhv_bt_api.iacdc_pred.nb=0;
	}
}



/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE PXP CONFIGURATION
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*****************************************************************************/
void t1xhv_bt_set_pxp_soft_reset(t_uint16 value)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : none
 * Write soft reset inside pf file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{  
	Gs_t1xhv_bt_api.pxp_soft_reset.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_soft_reset, 
				   (void *)&Gs_t1xhv_bt_api.pxp_soft_reset, Gs_t1xhv_bt_api.time_out);

}


/*****************************************************************************/
void m4e_bt_set_pxp_enc_frame_cfg(t_uint16 value)
/*-----------------------------------------------------------------------------
 * IN  : value = command already compacted
 * OUT : none
 * Write Pixel Pipeline configuration register to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.pxp_reg_frame_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_reg_frame_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pxp_reg_frame_cfg, Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
void m4d_bt_set_pxp_dec_frame_cfg(t_uint16 value)
/*-----------------------------------------------------------------------------
 * IN  : value = command already compacted
 * OUT : none
 * Write Pixel Pipeline configuration register to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{

	Gs_t1xhv_bt_api.pxp_reg_frame_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_reg_frame_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pxp_reg_frame_cfg, Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
void t1xhv_bt_set_pxp_reg_qtz_param(t_uint16 qtz_param)
/*-----------------------------------------------------------------------------
 * IN  : value = command already compacted
 * OUT : none
 * Write Pixel Pipeline configuration register to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{

	/* Set value */
	Gs_t1xhv_bt_api.pxp_reg_qtz_param.value = qtz_param;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_pxp_reg_qtz_param, 
				   (void *)&Gs_t1xhv_bt_api.pxp_reg_qtz_param,Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
void t1xhv_bt_set_pxp_reg_qp_table(t_uint16 quant_param , t_uint16 number)
/*-----------------------------------------------------------------------------
 * IN  : value = command already compacted
 * OUT : none
 * Write Pixel Pipeline configuration register to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{

	/* Set value */
	Gs_t1xhv_bt_api.pxp_reg_qp_table.value = quant_param;
	Gs_t1xhv_bt_api.pxp_reg_qp_table.nb = number;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_pxp_reg_qp_table, 
				   (void *)&Gs_t1xhv_bt_api.pxp_reg_qp_table,Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
void t1xhv_bt_set_pxp_cmd_macroblock(t_uint16 enc_rl_sep, t_uint16 dec_intra)
/*-----------------------------------------------------------------------------
 * IN  : enc_rl_sep   = (1 bit ) 1 to separate INTRA/DC at output of RLC
 *		 dec_intra    = (1 bit ) To know if Intra mode
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	t_uint16 value;

	/* Set value */
	value = PXP_COMPACT_CMD_MB(enc_rl_sep,
							   dec_intra);

	Gs_t1xhv_bt_api.pxp_fifo_mb_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_mb_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_mb_cfg, Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
void t1xhv_bt_set_pxp_cmd_block(t_uint16 scn_dir, 
							  t_uint16 dec_rl_luma)
/*-----------------------------------------------------------------------------
 * IN  : scn_dir       = (2 bits) Direction of Scan in decode mode
 *		 dec_rl_luma   = (1 bit)  Current block belong to luma if = 1
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	t_uint16 value;

	/* Set value */
	value = PXP_COMPACT_CMD_BLOCK(scn_dir,
								  dec_rl_luma);

	Gs_t1xhv_bt_api.pxp_fifo_block_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_block_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_block_cfg, Gs_t1xhv_bt_api.time_out);
	
}


/*****************************************************************************/
void t1xhv_bt_set_pxp_fifo_enc_qp(t_uint16 qp)
/*-----------------------------------------------------------------------------
 * IN  : qp = quantification parameter
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	/* Set value */
	Gs_t1xhv_bt_api.pxp_fifo_enc_qp.value = qp;

	/* Update time */
    BT_ADD_TIME(1);

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_enc_qp, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_enc_qp, Gs_t1xhv_bt_api.time_out);
	
}




/*****************************************************************************/
void t1xhv_bt_set_pxp_fifo_qp_luma(t_uint16 qp)
/*-----------------------------------------------------------------------------
 * IN  : qp = quantification parameter
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	/* Set value */
	Gs_t1xhv_bt_api.pxp_fifo_qp_luma.value = qp;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_qp_luma, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_qp_luma, Gs_t1xhv_bt_api.time_out);
	
}


/*****************************************************************************/
void t1xhv_bt_set_pxp_fifo_qp_chroma(t_uint16 qp)
/*-----------------------------------------------------------------------------
 * IN  : qp = quantification parameter
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	/* Set value */
	Gs_t1xhv_bt_api.pxp_fifo_qp_chroma.value = qp;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_qp_chroma, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_qp_chroma, Gs_t1xhv_bt_api.time_out);
	
}


/*****************************************************************************/
/*****************************************************************************/
/**
 * \brief  Write Pixel Pipeline iqtz parameter to .pf file
 * \author Serge Backert
 * \in     iqtz_param
 *
 * 
 **/
/*****************************************************************************/
void t1xhv_bt_set_pxp_reg_iqtz_param(t_uint16 iqtz_param)
/*****************************************************************************/
{
	/* Set value */
	Gs_t1xhv_bt_api.pxp_reg_iqtz_param.value = iqtz_param;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_pxp_reg_iqtz_param, 
				   (void *)&Gs_t1xhv_bt_api.pxp_reg_iqtz_param,
                    Gs_t1xhv_bt_api.time_out);

}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * WRITE POST ADDER
 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/*****************************************************************************/
void t1xhv_bt_set_pxp_cmd_post_adder(t_uint16 pa_mecc_cmd, t_uint16 pa_idct_cmd , t_uint16 interlace)
/*-----------------------------------------------------------------------------
 * IN  : pa_idct_cmd   = (2 bits) Idle/Get/Skip on idct
 *		 pa_mecc_cmd   = (2 bits) Idle/Get/Skip on predictor
 * OUT : none
 * Write Pixel Pipeline command to file
 *-----------------------------------------------------------------------------
 * Use MTC_API
 * Described in PXP spec
 ******************************************************************************/
{
	t_uint16 value;

	/* Set value */
	value = PA_COMPACT_CMD(pa_mecc_cmd, pa_idct_cmd, interlace);

	Gs_t1xhv_bt_api.pxp_fifo_pa_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pxp_fifo_pa_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pxp_fifo_pa_cfg, Gs_t1xhv_bt_api.time_out);
	
}


/*****************************************************************************/
/**
 * \BRIEF  Write PPP configuration register to file (on sun)
 * \AUTHOR Serge Backert
 * \IN     value = command already compacted
 * \OUT    none
 * 
 * Write PPP configuration register to file. Use MTC_API. Described in PPP 
 * specification.
 **/
/*****************************************************************************/
void dpl_bt_set_ppp_cfr(t_uint16 value)
/*****************************************************************************/
{

	Gs_t1xhv_bt_api.ppp_cfr.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_ppp_cfr, 
				   (void *)&Gs_t1xhv_bt_api.ppp_cfr, Gs_t1xhv_bt_api.time_out);
	
}
/*****************************************************************************/
/**
 * \BRIEF  Write PPP luma/chroma vertical/horizontal resize control register 
 *         to file (on sun)
 * \AUTHOR Serge Backert
 * \IN     value = command already compacted
 * \OUT    none
 * 
 * Write PPP luma/chroma vertical/horizontal resize control register to file. 
 * Use MTC_API. Described in PPP specification.
 **/
/*****************************************************************************/
void dpl_bt_set_ppp_lvc(t_uint16 value)
/*****************************************************************************/
{
	Gs_t1xhv_bt_api.ppp_lvc.value = value;
	/* Update time */
	Gs_t1xhv_bt_api.time_out++;
	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_ppp_lvc, 
				   (void *)&Gs_t1xhv_bt_api.ppp_lvc,Gs_t1xhv_bt_api.time_out);
}
void dpl_bt_set_ppp_lhc(t_uint16 value)
/*****************************************************************************/
{
	Gs_t1xhv_bt_api.ppp_lhc.value = value;
	/* Update time */
	Gs_t1xhv_bt_api.time_out++;
	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_ppp_lhc, 
				   (void *)&Gs_t1xhv_bt_api.ppp_lhc,Gs_t1xhv_bt_api.time_out);
}
void dpl_bt_set_ppp_cvc(t_uint16 value)
/*****************************************************************************/
{
	Gs_t1xhv_bt_api.ppp_cvc.value = value;
	/* Update time */
	Gs_t1xhv_bt_api.time_out++;
	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_ppp_cvc, 
				   (void *)&Gs_t1xhv_bt_api.ppp_cvc,Gs_t1xhv_bt_api.time_out);
}
void dpl_bt_set_ppp_chc(t_uint16 value)
/*****************************************************************************/
{
	Gs_t1xhv_bt_api.ppp_chc.value = value;
	/* Update time */
	Gs_t1xhv_bt_api.time_out++;
	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out,Gs_t1xhv_bt_api.id_ppp_chc, 
				   (void *)&Gs_t1xhv_bt_api.ppp_chc,Gs_t1xhv_bt_api.time_out);
}

/*****************************************************************************/
/**
 * \brief  Write PIF configuration register to file (on sun)
 * \author Serge Backert
 * \in     value = command already compacted
 * \out    none
 * 
 * Write PIF configuration register to file. Use MTC_API. Described in PIF
 * specification.
 **/
/*****************************************************************************/
void dpl_bt_set_pif_cfg(t_uint16 value)
/*****************************************************************************/
{

	Gs_t1xhv_bt_api.pif_cfg.value = value;

	/* Update time */
	Gs_t1xhv_bt_api.time_out++;

	/* Save structure to file */
	MTC_file_Trace(Gs_t1xhv_bt_api.p_file_out, Gs_t1xhv_bt_api.id_pif_cfg, 
				   (void *)&Gs_t1xhv_bt_api.pif_cfg, Gs_t1xhv_bt_api.time_out);
	
}

/*****************************************************************************/
t_uint16 h263_bt_read_bitstream(void)
/*-----------------------------------------------------------------------------
 * IN  : none
 * OUT : value read from compressed data fifo
 * Use MTC_API to read from the Data compressed fifo
 *-----------------------------------------------------------------------------
 * Data is on 16 bits. 
 ******************************************************************************/
{
    Gs_t1xhv_bt_api.p_tabint16=
	(t_pt_dval_tabint16)MTC_file_ReadVal(Gs_t1xhv_bt_api.p_file_in,
					Gs_t1xhv_bt_api.id_cp_data,
					&Gs_t1xhv_bt_api.time_in);  

    if(Gs_t1xhv_bt_api.p_tabint16==(t_pt_dval_tabint16)NULL) {	   
	printf("Read Bitstream : No more value in bitstream - Simulation ending\n");
	return 0; 
    } else { return 1; }
}

#endif /* BIT TRUE */
/* END of t1xhv_bt_utils.c */
