/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*******************************************************************************
 * Created by Philippe Rochette on Tue Jun 25 10:05:57 2002
 ******************************************************************************/
#ifdef BIT_TRUE

#ifndef _T1XHV_BT_UTILS_H_
#define _T1XHV_BT_UTILS_H_

/*------------------------------------------------------------------------
 * Include							       
 *----------------------------------------------------------------------*/
#include <mtc_file_api.h>

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
#ifdef BT_GLOBAL
#define EXTERN
#else
#define EXTERN extern
#endif


/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/* Structure to store all datas to manage pf files. */
EXTERN struct s_t1xhv_bt_api {

    /* Ptr on files, input is from the C ref. model. */
    t_pt_file_mgt   p_file_in;  	
    t_pt_file_mgt   p_file_out;  	

    /* Define Ids OUT seen from HW bloc (input of MMDSP) */
    /* Refer to 
     * - PXP documentation Table 18
     * - PXP documentation Table 19
     * - PXP documentation Table 20
     * - PXP documentation Table 22
     * - MECC documentation v1.2 Table 10 
    */
    int id_host;				/* Host IF (via DBR) */
 	int id_cp_data;	  
 	int id_param_in;	  
 	int id_param_inout;	  
 	int id_buffer;	  
 	int id_mtf;	  
	int id_iacdc_res;
	int id_iacdc_pred;
	int id_rld;
    int id_mecc_out;			/* MECC output (SAD,...) */
    int id_packer_out;			/* Packer output ( nb bits) */
    int id_rlc;					/* Run Level input */
    int id_pxp_reg_clip_out;                /* PXP documentation Table 22 */
    int id_mtf_in;					/* MTC Input */
    int id_mtf_out;					/* MTC Output */
    int id_pxp_soft_reset;                  /* PXP documentation Table 27 */
	int id_pxp_reg_encode;					/* PXP documentation Table 18 */
	int id_pxp_reg_frame_cfg;				/* PXP documentation Table 19 */
	int id_pxp_reg_qtz_param;				/* PXP documentation Table 19 */
	int id_pxp_fifo_mb_cfg;					/* PXP documentation Table 20 */
	int id_pxp_fifo_block_cfg;				/* PXP documentation Table 21 */
	int id_pxp_fifo_pa_cfg;					/* PXP documentation Table 22 */
	int id_pxp_fifo_enc_qp;					/* PXP documentation Table 22 */
	int id_pxp_fifo_qp_luma;				/* PXP documentation Table 22 */
	int id_pxp_fifo_qp_chroma;				/* PXP documentation Table 22 */
	int id_pxp_reg_iqtz_param;				/* PXP documentation Table 26 */
	int id_pxp_reg_qp_table;				/* PXP documentation Table 39 */
	int id_ppp_cfr;				            /* PPP documentation */
	int id_ppp_lvc;				            /* PPP documentation */
	int id_ppp_lhc;				            /* PPP documentation */
	int id_ppp_cvc;				            /* PPP documentation */
	int id_ppp_chc;				            /* PPP documentation */
	int id_pif_cfg;				            /* PIF documentation */

    /* Define Ids IN seen from HW bloc (output of MMDSP) */
    /* Refer to 
     * - MECC documentation v1.2 Table 10
     */
    int id_mecc_cfg;						/* Cfg MECC (rounding type, image_with...)  */
    int id_mecc_in;							/* MECC input (MV, commands) */
    int id_packer_in;						/* Packer input (value, nb bits...) */

    /* Define pointers for data input of MMDSP. */
    t_pt_dval_blockreg  p_host;
	t_pt_dval_tabint16  p_tabint16;			/* For compressed data */
	t_pt_dval_tabint16  p_iacdc_res; 		/* For IACDC result    */
    t_pt_dval_from_mecc p_mecc_out;
    t_pt_dval			p_packer_out;
    t_pt_dval_rl_event	p_rlc;
	t_dval_tabint16 	mtf_in;			    /* For MTC File (h263)                  */
    t_pt_dval           p_pxp_reg_clip_out; /* Flag detecting out of range coeff    */


    /* Define structure to store datas out of MMDSP. */
    t_dval_config_mecc  mecc_cfg;		
    t_dval_to_mecc 		mecc_in;			/* For MECC command, decision and MV 	*/
    t_dval_pack			packer_in;
	t_dval_tabint16 	iacdc_pred;			/* To send predictor to iacdc 		    */
	t_dval_rl_event 	rld; 				/* For inv acdc run-level event			*/
	t_dval_tabint16 	mtf_out;			/* For MTC File (h263)                  */

	/* Variables for Configuration (output) */
    t_dval              pxp_soft_reset;     /* PXP Soft reset                       */
	t_dval				pxp_reg_encode;		/* Encode/decode register 				*/
	t_dval				pxp_reg_frame_cfg;	/* Frame configuration register 		*/
	t_dval				pxp_fifo_mb_cfg;	/* Macroblock configuration fifo 		*/
	t_dval				pxp_fifo_block_cfg;	/* Block configuration fifo 			*/
	t_dval				pxp_fifo_pa_cfg;	/* Post-Adder configuration fifo 		*/
	t_dval				pxp_fifo_enc_qp;	/* QP default 							*/
	t_dval				pxp_fifo_qp_luma;	/* QP DC Luma 							*/
	t_dval				pxp_fifo_qp_chroma;	/* QP DC chroma					 		*/
	t_dval				pxp_reg_qtz_param;	/* QTZ parameter used for annex I	    */
	t_dval				pxp_reg_iqtz_param;	/* IQTZ parameter used for annex I	    */
    t_dval_tabint16     pxp_reg_qp_table;   /* Quantizer table                      */

    /* Define time */
    t_tr_time 			time_in;
    long long 			time_mecc_in;
    long long 			time_out;
    
    /* to use later */
    t_uint32 				bitstream_bits;
    t_uint16 				total_error;
    t_uint16 				bitstream_flag;

    /*************************************************************************/
    /**
     * \BRIEF  Variables for PPP Configuration (output)
     **/
    /*************************************************************************/
    t_dval 				ppp_cfr;
    t_dval 				ppp_lvc;
    t_dval 				ppp_lhc;
    t_dval 				ppp_cvc;
    t_dval 				ppp_chc;
    /*************************************************************************/
    /**
     * \brief  Variables for PIF Configuration (output)
     **/
    /*************************************************************************/
    t_dval 				pif_cfg;


    /* For stabilization */
    int id_horiz_vect;
    int id_vert_vect;

    t_dval              out_mv_horiz; /* Out compensation horizontal vector */
    t_dval              out_mv_vert;  /* Out compensation vertical vector */


} Gs_t1xhv_bt_api;

typedef struct {
	t_uint16 length;  
	t_sint16 value;
} ts_packer_bt;

typedef struct {
	ts_packer_bt *ps_packer_bt; 
	t_uint16 nb_elt;
} ts_fifo_packer_bt;

/*------------------------------------------------------------------------
 * Public macros
 *----------------------------------------------------------------------*/
#define BT_ADD_TIME(a) {Gs_t1xhv_bt_api.time_out+=(a);}   

/*------------------------------------------------------------------------
 * Public functions
 *----------------------------------------------------------------------*/
/* -- Init -- */
void m4e_bt_init_api(void);
void m4d_bt_init_api(void);
void h263_bt_init_api(void); /**<\brief used only by the h263 demo */
void t1xhv_bt_close_api(int flag_short_header, 
                      int ac_pred_flag,
                      int data_partitioned, 
                      int reversible_vlc,
                      int image_width, 
                      int image_height, 
                      int frame_number);
void h263_bt_close_api(void); /**<\brief used only by the h263 demo */

/* -- DBR -- */
int t1xhv_bt_init_dbr(void);
t_uint16 t1xhv_bt_read_dbr(int address);
int t1xhv_bt_read_host(int id, int nb, int addr);

/* -- BPU -- */
void m4e_bt_write_packer_to_file(t_uint16 cmd, t_uint16 length, t_uint16 value);
void m4d_bt_write_packer_data(t_uint16 cmd, t_uint16 length, t_uint16 value);
/** m4e_bt_write_packer_data() returns allways 0 as there is no notion of
 * offset in "bit-true", this is to have prototype compatibility with
 * msim version
 */
t_uint16 m4e_bt_write_packer_data(t_uint16 cmd,t_uint16 length,t_uint32 value);

/* -- BDU -- */
t_uint16 m4d_bt_read_bitstream(void);


/* -- PXP -- */
void m4d_bt_set_pxp_enc_frame_cfg(t_uint16 value);
void m4d_bt_set_pxp_dec_frame_cfg(t_uint16 value);
void t1xhv_bt_set_pxp_cmd_macroblock(t_uint16 enc_rl_sep, t_uint16 dec_intra);
void t1xhv_bt_set_pxp_cmd_block(t_uint16 scn_dir, 
                                t_uint16 dec_rl_luma);
void t1xhv_bt_set_pxp_fifo_enc_qp(t_uint16 qp);
void t1xhv_bt_set_pxp_fifo_qp_luma(t_uint16 qp);
void t1xhv_bt_set_pxp_fifo_qp_chroma(t_uint16 qp);
void t1xhv_bt_set_pxp_reg_iqtz_param(t_uint16 iqtz_param);
void t1xhv_bt_set_pxp_reg_qtz_param(t_uint16 qtz_param);
void t1xhv_bt_set_pxp_reg_qp_table(t_uint16 quant_param , t_uint16 number);
void t1xhv_bt_set_pxp_soft_reset(t_uint16 value);


/* -- IACDC -- */
void m4d_bt_read_iacdc_res(t_sint16 *value);
void m4d_bt_set_rl(t_uint16 block_number);
void m4d_bt_write_rl(t_uint16 last, t_uint16 run, t_sint16 level);
void m4d_bt_write_iacdc_pred(t_sint16 coeff);

/* -- RLC -- */
void m4e_bt_read_rlc(t_uint16 *last, t_uint16 *run, t_sint16 *level);

/* -- MECC -- */
void t1xhv_bt_write_mecc_command(t_uint16 command);
void t1xhv_bt_write_mecc_mv_x(t_sint16 mv_x);
void t1xhv_bt_write_mecc_mv_y(t_sint16 mv_y);
void t1xhv_bt_save_mecc_mv(void);
void t1xhv_bt_write_mecc_rnd(t_uint16 rnd);
void t1xhv_bt_write_mecc_image_width(t_uint16 iw);
void t1xhv_bt_write_mecc_scx_stab(t_uint16 scx_stab);
void t1xhv_bt_write_mecc_scy_stab(t_uint16 scy_stab);
t_sint16 t1xhv_bt_read_mecc(t_uint16 address);

/* -- PA -- */
void t1xhv_bt_set_pxp_cmd_post_adder(t_uint16 pa_idct_cmd, t_uint16 pa_pred_cmd, t_uint16 interlace);

/* -- MTF -- */
void t1xhv_bt_write_mtf_to_file(int id_mtf, t_dval_tabint16 *ps_mtf, t_uint16 nb, t_uint16 *p_value);
void t1xhv_bt_write_last_mtf_to_file(int id_mtf, t_dval_tabint16 *ps_mtf);

/*****************************************************************************/
/**
 * \BRIEF  Write PPP configuration register to file (on sun)
 **/
/*****************************************************************************/
void dpl_bt_set_ppp_cfr(t_uint16 value);
void dpl_bt_set_ppp_lvc(t_uint16 value);
void dpl_bt_set_ppp_lhc(t_uint16 value);
void dpl_bt_set_ppp_cvc(t_uint16 value);
void dpl_bt_set_ppp_chc(t_uint16 value);

/*****************************************************************************/
/**
 * \BRIEF  Write PIF configuration register to file (on sun)
 **/
/*****************************************************************************/
void dpl_bt_set_pif_cfg(t_uint16 value);

/* GLOBAL VARIABLE */
EXTERN int G_flag_ref_compliant;


#undef EXTERN

#endif /* _M4E_BT_UTILS_H_ */

#endif /* BIT_TRUE */
