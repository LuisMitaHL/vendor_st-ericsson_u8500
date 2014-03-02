/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VCU_API_H_
#define _VCU_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/
#include "t1xhv_hamac_xbus_regs.h"
#include "new_regs.h"

/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get xxx_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_Q(a) (*(volatile MMIO t_uint16 *)(Q_BASE+a))
#define HW_Q_WAIT(a) (*(volatile __XIO t_uint16 *)(Q_BASE+a))
#define HW_QTAB(a) (*(volatile MMIO t_uint16 *)(QTAB_BASE+a))
#define HW_QTAB_WAIT(a) (*(volatile __XIO t_uint16 *)(QTAB_BASE+a))
#define HW_DCT(a) (*(volatile MMIO t_uint16 *)(DCT_BASE+a))
#define HW_DCT_WAIT(a) (*(volatile __XIO t_uint16 *)(DCT_BASE+a))
#define HW_RLC(a) (*(volatile MMIO t_uint16 *)(RLC_BASE+a))
#define HW_RLC_WAIT(a) (*(volatile __XIO t_uint16 *)(RLC_BASE+a))
#define HW_SCN(a) (*(volatile MMIO t_uint16 *)(SCN_BASE+a))
#define HW_SCN_WAIT(a) (*(volatile __XIO t_uint16 *)(SCN_BASE+a))
#define HW_IQ(a) (*(volatile MMIO t_uint16 *)(IQ_BASE+a))
#define HW_IQ_WAIT(a) (*(volatile __XIO t_uint16 *)(IQ_BASE+a))
#define HW_IDCT(a) (*(volatile MMIO t_uint16 *)(IDCT_BASE+a))
#define HW_IDCT_WAIT(a) (*(volatile __XIO t_uint16 *)(IDCT_BASE+a))
#define HW_IPRD(a) (*(volatile MMIO t_uint16 *)(IPRD_BASE+a))
#define HW_IPRD_WAIT(a) (*(volatile __XIO t_uint16 *)(IPRD_BASE+a))
#define HW_PA(a) (*(volatile MMIO t_uint16 *)(PA_BASE+a))
#define HW_PA_WAIT(a) (*(volatile __XIO t_uint16 *)(PA_BASE+a))
#define HW_REC(a) (*(volatile MMIO t_uint16 *)(REC_BASE+a))
#define HW_REC_WAIT(a) (*(volatile __XIO t_uint16 *)(REC_BASE+a))
#define HW_VC1IT(a) (*(volatile MMIO t_uint16 *)(VC1IT_BASE+a))
#define HW_VC1IT_WAIT(a) (*(volatile __XIO t_uint16 *)(VC1IT_BASE+a))
#define HW_FIFO(a) (*(volatile MMIO t_uint16 *)(FIFO_BASE+a))
#define HW_FIFO2(a) (*(volatile MMIO t_uint16 *)(FIFO2_BASE+a))
#define HW_VBF(a) (*(volatile MMIO t_uint16 *)(VBF_BASE+a))
#define HW_VBF_WAIT(a) (*(volatile __XIO t_uint16 *)(VBF_BASE+a))
#define HW_H264RLC(a) (*(volatile MMIO t_uint16 *)(H264VLC_BASE+a))
#define HW_H264RLC_WAIT(a) (*(volatile __XIO t_uint16 *)(H264VLC_BASE+a))
#define HW_VLC(a) (*(volatile MMIO t_uint16 *)(VLC_BASE+a))   /* VLC for MPEG4 and JPEG */
#define HW_VLC_WAIT(a) (*(volatile __XIO t_uint16 *)(VLC_BASE+a))


/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/


#pragma inline 
static void FIFO_INIT()
{
  HW_FIFO(FIFO_REG_SOFT_RESET) = 13;
}

#pragma inline 
static void FIFO2_INIT()
{
  HW_FIFO2(FIFO2_REG_SOFT_RESET) = 13;
}

/*********************************************************************
        BLOCK Q 
**********************************************************************/
/*****************************************************************************/
/**
 * \brief 	Q soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void Q_INIT()
{
	HW_Q(Q_REG_SOFT_RESET) = 1;
	HW_Q(Q_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Q command input selection 
 * \author 	Maurizio Colombo
 * \param 	def 0: commands from FIFO, 1: commands from reg
 *  
 */
/*****************************************************************************/
#pragma inline
static void Q_SET_REG_DEF(t_uint16 def)
{
	HW_Q(Q_REG_DEF) = def;
}

/*****************************************************************************/
/**
 * \brief 	Q configuration
 * \author 	Maurizio Colombo
 * \param 	q_step_access : use Q table or single quant for all coeffs
 * \param   intra_dc_clip_range : clip range for intra DC coeff
 * \param   oth_clip_range      : clip range for all but INTRA DC  
 */
/*****************************************************************************/
#pragma inline
static void Q_CONFIGURE(t_qtz_table q_step_access, t_enc_qtz_clip intra_dc_clip_range,
						t_enc_qtz_clip oth_clip_range)
{
  HW_Q(Q_REG_CFG) = ( (q_step_access       << 1)|
					  (intra_dc_clip_range << 2)|
                      (oth_clip_range      << 5));
}


/*****************************************************************************/
/**
 * \brief 	Q configuration second function
 * \author 	Maurizio Colombo
 * \param   idc/iac/oth_r       : r param in the formula
 * \param   idc/iac/oth_s       : s param in the formula
 * \param   idc/iac/oth_t       : t param in the formula
 */
/*****************************************************************************/
#pragma inline
static void Q_CONFIGURE_2(t_uint16 idc_r, t_enc_qtz_param_intra_dc_s idc_s, t_enc_qtz_param_intra_dc_t idc_t,
						  t_uint16 iac_r, t_enc_qtz_param_intra_ac_s iac_s, t_enc_qtz_param_intra_ac_t iac_t,
						  t_uint16 inter_r, t_enc_qtz_param_inter_s  inter_s, t_enc_qtz_param_inter_t inter_t)
{                 
  HW_Q(Q_REG_CFG2) = ( (idc_t                   )|
                       (idc_s              << 1 )|
                       (idc_r              << 2 )|
                       (iac_t              << 5 )|
                       (iac_s              << 6 )|
                       (iac_r              << 7 )|
                       (inter_t            << 10)|
                       (inter_s            << 11)|
                       (inter_r            << 12));
}



/*****************************************************************************/
/**
 * \brief 	Write into quant FIFO 
 * \author 	Maurizio Colombo
 * \param 	quant_dc   DC coefficient quantizer step
 * \param   quant_ac   AC coefficient quantizer step
 * \param   inter      tells whether the block is intra or inter
 * \param   repeat     number of current command iteration   
  
 */
/*****************************************************************************/
#pragma inline
static void Q_SET_FIFO_QUANT(t_uint16 quant_dc,
							 t_uint16 quant_ac,
							 t_enc_intra inter,
							 t_uint16 repeat)
{
  HW_Q_WAIT(Q_FIFO_CMD) = (quant_dc          |
						  (quant_ac   <<6)  |
						   (inter     <<12)  |
						   ((repeat-1)<<13)  );
}





/*********************************************************************
        BLOCK DCT
**********************************************************************/



/*****************************************************************************/
/**
 * \brief 	DCT soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void DCT_INIT()
{
	HW_DCT(DCT_REG_SOFT_RESET) = 1;
	/*HW_DCT(DCT_REG_DEF) = 0;*/
}

/*****************************************************************************/
/**
 * \brief 	DCT configuration
 * \author 	Maurizio Colombo
 * \param 	dct_shift_enable : enable level shifting of -128 for input data  
 */
/*****************************************************************************/
#pragma inline
static void DCT_CONFIGURE(t_dct_shift dct_shift_enable)
{
  HW_DCT(DCT_REG_CFG) = dct_shift_enable;
}


/*********************************************************************
        BLOCK VLC MPEG4/JPEG
**********************************************************************/



/*****************************************************************************/
/**
 * \brief 	VLC soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VLC_INIT()
{
	HW_VLC(VLC_REG_SOFT_RESET) = 1;
	HW_VLC(VLC_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	VLC configuration
 * \author 	Maurizio Colombo
 * \param 	vlc_type: defines if JPEG or MPEG4 SH/SP 
 */
/*****************************************************************************/
#pragma inline
static void VLC_CONFIGURE(t_vlc_type vlc_type)
{
  HW_VLC(VLC_REG_CFG) = vlc_type;
}

/*****************************************************************************/
/**
 * \brief 	set DC predictor for the VLC 
 * \author 	Maurizio Colombo
 * \param 	dc_pred : value of the DC predictor 
 */
/*****************************************************************************/
#pragma inline
static void VLC_SET_DC_PRED(t_uint16 dc_pred)
{
  HW_VLC_WAIT(VLC_FIFO_DC_PRED) = dc_pred;
}


/*****************************************************************************/
/**
 * \brief 	get current DC coeff from the VLC 
 * \author 	Maurizio Colombo 
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VLC_GET_DC_CUR()
{
  return HW_VLC_WAIT(VLC_FIFO_DC_CUR);
}

/*****************************************************************************/
/**
 * \brief 	get current block CBP from the VLC 
 * \author 	Maurizio Colombo 
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VLC_GET_CBP()
{
  return HW_VLC_WAIT(VLC_FIFO_CBP);
}


/*****************************************************************************/
/**
 * \brief 	write VLC command into FIFO
 * \author 	Maurizio Colombo
 * \param 	component :  defines if luma or chroma
 * \param   intra     :  defines if intra or inter
 * \param   rvlc      :  UNUSED
 * \param   repeat    :  repeat
 */
/*****************************************************************************/
#pragma inline
static void VLC_WRITE_FIFO_CMD(t_vlc_component  component,
							   t_vlc_intra      intra,
							   t_vlc_rvlc       rvlc,
							   t_uint16         repeat)
{
  HW_VLC_WAIT(VLC_FIFO_CMD) = component   |
	                          (intra<<1)  |
                              ((repeat-1)<<2);
}







/*********************************************************************
        BLOCK QTAB
**********************************************************************/



/*****************************************************************************/
/**
 * \brief 	QTAB soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void QTAB_INIT()
{
	HW_QTAB(QTAB_REG_SOFT_RESET) = 1;
	HW_QTAB(QTAB_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	send a command to QTAB
 * \author 	Maurizio Colombo
 * \param 	scan_dir    :   gives the scanning direction of qtable coefficients
 * \param   table_index :   select between the three tables
 * \param   repeat      :   number of iterations for the current command  
 */
/*****************************************************************************/
#pragma inline
static void QTAB_SEND_CMD(t_qtab_scn_dir scan_dir, t_qtable_index table_index, t_uint16 repeat)
{
  HW_QTAB_WAIT(QTAB_FIFO_CMD) = (scan_dir            |
								 (table_index  << 2) |
								 ((repeat-1)   << 4));
}



/*****************************************************************************/
/**
 * \brief 	write a coeff into QTAB
 * \author 	Maurizio Colombo
 * \param 	coeff   :  quantizer step in the range 1,255
 * \param   last    :  last incremental access to QTAB from XBUS : resets the 
 *                     current address pointer
 * \note    last is only needed if you don't write all the three tables !
 */
/*****************************************************************************/
#pragma inline
static void QTAB_SET_COEFF(t_uint16 coeff, t_uint16 last)
{
  HW_QTAB_WAIT(QTAB_FIFO_IN) = (coeff   |  (last<<8));
}





/*********************************************************************
        BLOCK SCAN
**********************************************************************/



/*****************************************************************************/
/**
 * \brief 	SCAN soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void SCAN_INIT()
{
	HW_SCN(SCN_REG_SOFT_RESET) = 1;
	//Configuraion of register SCN_REG_DEF is done in PXP_SET_REG_DEC_FRAME_CFG API
	//HW_SCN(SCN_REG_DEF) = 0;
}


/*****************************************************************************/
/**
 * \brief 	SCAN command input selection
 * \author 	Maurizio Colombo
 * \param 	def  0=input from FIFO, 1=input from REG
 *  
 */
/*****************************************************************************/
#pragma inline
static void SCAN_SET_REG_DEF(t_uint16 def)
{
	HW_SCN(SCN_REG_DEF) = def;
}


/*****************************************************************************/
/**
 * \brief 	SCAN command register (static)
 * \author 	Maurizio Colombo
 * \param 	cmd: static command for SCN
 *  
 */
/*****************************************************************************/
#pragma inline
static void SCAN_SET_REG_CMD(t_scn_cmd cmd)
{
	HW_SCN(SCN_REG_CMD) = cmd;
}

/*****************************************************************************/
/**
 * \brief 	SCAN configuration
 * \author 	Maurizio Colombo
 * \param 	encode : select encode/decode mode in scan
 *  
 */
/*****************************************************************************/
#pragma inline
static void SCAN_CONFIGURE(t_scn_encode encode)
{
	HW_SCN(SCN_REG_CFG) = encode;
}

/*****************************************************************************/
/**
 * \brief 	send a command to scan FIFO
 * \author 	Maurizio Colombo
 * \param 	cmd : scan command
 * \param   repeat : number of iterations for current cmd
 */
/*****************************************************************************/
#pragma inline
static void SCAN_SEND_CMD(t_scn_cmd cmd, t_uint16 repeat)
{
  HW_SCN_WAIT(SCN_FIFO_CMD) = cmd | ((repeat-1)<<4);
}






/*********************************************************************
        BLOCK RLC
**********************************************************************/



/*****************************************************************************/
/**
 * \brief 	RLC soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void RLC_INIT()
{
	HW_RLC(RLC_REG_SOFT_RESET) = 1;
	HW_RLC(RLC_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	RLC command input selection
 * \author 	Maurizio Colombo
 * \param 	def : 0=input from FIFO, 1=input from reg
 *  
 */
/*****************************************************************************/
#pragma inline
static void RLC_SET_REG_DEF(t_uint16 def)
{
	HW_RLC(RLC_REG_DEF) = def;
}

/*****************************************************************************/
/**
 * \brief 	set a command in RLC reg
 * \author 	Maurizio Colombo
 * \param 	cmd : rlc command: defines if separated intra DC event
 */
/*****************************************************************************/
#pragma inline
static void RLC_SET_REG_CMD(t_enc_rl_sep cmd)
{
  HW_RLC(RLC_REG_CMD) = cmd;
}


/*****************************************************************************/
/**
 * \brief 	send a command to RLC fifo
 * \author 	Maurizio Colombo
 * \param 	cmd : rlc command: defines if separated intra DC event
 * \param   repeat : number of iterations for current cmd
 */
/*****************************************************************************/
#pragma inline
static void RLC_SEND_CMD(t_enc_rl_sep cmd, t_uint16 repeat)
{
  HW_RLC_WAIT(RLC_FIFO_CMD) = cmd | ((repeat-1)<<1);
}










/*********************************************************************
        BLOCK IQ 
**********************************************************************/








/*****************************************************************************/
/**
 * \brief 	IQ soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void IQ_INIT()
{
	HW_IQ(IQ_REG_SOFT_RESET) = 1;
	HW_IQ(IQ_REG_DEF) = 0;
}


/*****************************************************************************/
/**
 * \brief 	IQ command input selection 
 * \author 	Maurizio Colombo
 * \param 	def 0: commands from FIFO, 1: commands from reg
 *  
 */
/*****************************************************************************/
#pragma inline
static void IQ_SET_REG_DEF(t_uint16 def)
{
	HW_IQ(IQ_REG_DEF) = def;
}



/*****************************************************************************/
/**
 * \brief 	IQ frame configuration
 * \author 	Maurizio Colombo
 * \param 	qtz_table : QP table is used for each coeff or not
 * \param   qp_table_ap : QP table + QP are used for each coeff
 * \param   clipping:   enables clipping after IQ
 * \param   dct_shift : disable INTRA DC clipping at zero
 * \param   clip_warn : enable warning if out of range IQuantized coeff is detected
 * \param   nb_blocks : number of blocks/MB
 * \param   mismatch:  enables IDCT mismatch control after IQ
 *  
 */
/*****************************************************************************/
#pragma inline
static void IQ_CONFIGURE(t_qtz_table qtz_table, t_qtz_table_qp qp_table_qp,
						 t_iq_data_clip clipping, t_dct_shift dct_shift,
						 t_iqtz_clip_warn clip_warn, t_uint16 nb_blocks,
						 t_iq_mismatch mismatch)
{
	HW_IQ(IQ_REG_CFG) = (qp_table_qp |
						(qtz_table<<1) |
						(clipping <<2) |
						(dct_shift<<3) |
						(clip_warn<<4) |
						((nb_blocks-1)<<5) |
						(mismatch<<8));
}

/*****************************************************************************/
/**
 * \brief 	IQ formula params configuration for INTRA DC/AC
 * \author 	Maurizio Colombo
 * \param 	dc_a/b/c/d/e/f ac_a/b/c/d/e/f
 * \brief   for the formula refer to IQ design doc
 */
/*****************************************************************************/
#pragma inline
static void IQ_CONFIGURE_PARAMS_INTRA(t_uint16 dc_a, t_uint16 dc_b, t_uint16 dc_c,
									  t_uint16 dc_d, t_uint16 dc_e, t_uint16 dc_f,
									  t_uint16 ac_a, t_uint16 ac_b, t_uint16 ac_c,
									  t_uint16 ac_d, t_uint16 ac_e, t_uint16 ac_f)
{
  HW_IQ(IQ_REG_CFG2) = (dc_a      |
						(dc_b<<1) |
						(dc_c<<2) |
						(dc_d<<3) |
						(dc_e<<6) |
						(dc_f<<7) |
						(ac_a<<8) |
						(ac_b<<9) |
						(ac_c<<10)|
						(ac_d<<11)|
						(ac_e<<14)|
						(ac_f<<15));
}


/*****************************************************************************/
/**
 * \brief 	IQ formula params configuration for INTER 
 * \author 	Maurizio Colombo
 * \param 	a/b/c/d/e/f
 * \brief   for the formula refer to IQ design doc
 */
/*****************************************************************************/
#pragma inline
static void IQ_CONFIGURE_PARAMS_INTER(t_uint16 a, t_uint16 b, t_uint16 c,
									  t_uint16 d, t_uint16 e, t_uint16 f)
{
  HW_IQ(IQ_REG_CFG3) = (a      |
						(b<<1) |
						(c<<2) |
						(d<<3) |
						(e<<6) |
						(f<<7));
}

/*****************************************************************************/
/**
 * \brief 	Write into quant FIFO 
 * \author 	Maurizio Colombo
 * \param 	quant_dc   DC coefficient or VC1 quantizer step
 * \param   quant_ac   AC coefficient quantizer or VC1 quantizer scale
 * \param   inter      tells whether the block is intra or inter
 * \param   repeat     number of current command iteration   
 */
/*****************************************************************************/
#pragma inline
static void IQ_SET_FIFO_QUANT(t_uint16 quant_dc,
							  t_uint16 quant_ac,
							  t_dec_intra inter,
							  t_uint16 repeat)
{
  HW_IQ_WAIT(IQ_FIFO_CMD) = (quant_dc          |
							 (quant_ac   <<6)  |
							 (inter     <<12)  |
							 ((repeat-1)<<13)  );
}


/*****************************************************************************/
/**
 * \brief 	Write into CMD2 FIFO 
 * \author 	Maurizio Colombo
 * \param 	q_scale: bit 7 of quantizer_scale   
 */
/*****************************************************************************/
#pragma inline
static void IQ_SET_FIFO_QUANT_MP2(t_uint16 q_scale)
{
  HW_IQ_WAIT(IQ_FIFO_CMD2) = q_scale;
}

/*****************************************************************************/
/**
 * \brief   Read IQ clipping flag from FIFO
 * \author 	Maurizio Colombo 
 */
/*****************************************************************************/
#pragma inline
static t_uint16 IQ_GET_FIFO_CLIP_OUT()
{
  return HW_IQ_WAIT(IQ_FIFO_OUT);
}

/*****************************************************************************/
/**
 * \brief 	Write into quant  for VC1
 * \author 	Aroua BEN DARYOUG
 * \param 	q2_bit7  bit 7 of q2 for MPEG2 in the range [1,112]
 * \param   q3: quant scale for VC1 if MB is not uniform
 */
/*****************************************************************************/
#pragma inline
static void IQ_SET_FIFO_QUANT2(t_uint16 q2_bit7,
                               t_uint16 q3)
{
  HW_IQ_WAIT(IQ_FIFO_CMD2) = (q2_bit7          |
							 (q3   <<1 ));
}
/*****************************************************************************/
/**
 * \brief 	Write into quant REG for VC1 
 * \author 	Aroua BEN DARYOUG
 * \param 	q2_bit7  bit 7 of q2 for MPEG2 in the range [1,112]
 * \param   q3: quant scale for VC1 if MB is not uniform
 */
/*****************************************************************************/
#pragma inline
static void IQ_SET_REG_QUANT2(t_uint16 q2_bit7,
                               t_uint16 q3)
{
  HW_IQ_WAIT(IQ_REG_CMD2) = (q2_bit7          |
							 (q3   <<1 ));
}




/*********************************************************************
        BLOCK IDCT
**********************************************************************/






/*****************************************************************************/
/**
 * \brief 	IDCT soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void IDCT_INIT()
{
	HW_IDCT(IDCT_REG_SOFT_RESET) = 1;
	/*HW_IDCT(IDCT_REG_DEF) = 0;*/
}


/*****************************************************************************/
/**
 * \brief 	IDCT frame configuration
 * \author 	Maurizio Colombo
 * \param 	dct_shift : enable level shifting of +128 at output
 *  
 */
/*****************************************************************************/
#pragma inline
static void IDCT_CONFIGURE(t_dct_shift dct_shift)
{
	HW_IDCT(IDCT_REG_CFG) = dct_shift;
}







/*********************************************************************
        BLOCK IPRD
**********************************************************************/




/*****************************************************************************/
/**
 * \brief 	IPRD soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void IPRD_INIT()
{
	HW_IPRD(IPRD_REG_SOFT_RESET) = 1;
	HW_IPRD(IPRD_REG_DEF) = 0;
}


/*****************************************************************************/
/**
 * \brief 	IPRD frame configuration
 * \author 	Maurizio Colombo
 * \param   path_iprd : IQTZ performed before or after  IPRD
 *  
 */
/*****************************************************************************/
#pragma inline
static void IPRD_CONFIGURE(t_path_iprd iprd, t_iprd_cup cup_enable)
{
	HW_IPRD(IPRD_REG_CFG) = iprd|(cup_enable<<1);
}


/*****************************************************************************/
/**
 * \brief 	IPRD write FIFO command
 * \author 	Maurizio Colombo
 * \param   iprd_dir: select between DC, AC_V, AC_H (linked to the scan direction)
 * \param    
 */
/*****************************************************************************/
#pragma inline
static void IPRD_WRITE_FIFO_CMD(t_iprd_dir iprd_dir, t_uint16 repeat)
{
	HW_IPRD_WAIT(IPRD_FIFO_CMD) = (iprd_dir     |
							 ((repeat-1)<<4));
}


/*****************************************************************************/
/**
 * \brief 	IPRD write FIFO IN
 * \author 	Maurizio Colombo
 * \param   coeff to be written
 * \brief   DC,0,0,0,0,0,0,0 if DC pred   
 * \brief   DC0, AC1, AC2, AC3, AC10, AC11, AC12, AC13 if AC/DC pred    
 */
/*****************************************************************************/
#pragma inline
static void IPRD_WRITE_FIFO_IN(t_uint16 coeff)
{
	HW_IPRD_WAIT(IPRD_FIFO_IN) = coeff;
}


/*****************************************************************************/
/**
 * \brief 	IPRD read FIFO OUT, get predictor output coefficient
 * \author 	Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static t_uint16 IPRD_GET_FIFO_OUT()
{
	return HW_IPRD_WAIT(IPRD_FIFO_OUT);
}





/*********************************************************************
        BLOCK PA
**********************************************************************/





/*****************************************************************************/
/**
 * \brief 	PA soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_INIT()
{
	HW_PA(PA_REG_SOFT_RESET) = 1;
	HW_PA(PA_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	PA write REG def
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_SET_REG_DEF(t_pa_reg_sel pa_reg_sel)
{
  HW_PA_WAIT(PA_REG_DEF) = (pa_reg_sel);
}
/*****************************************************************************/
/**
 * \brief 	PA write REG command
 * \author 	Cyril Enault
 * \param   pa_idct_cmd: PA command for IDCT data
 * \param   pa_pred_cmd: PA command for pred data
 * \param   raster : enables raster output
 * \param   pa_rb: size of the square to be sent to IPA
 * \param   luma: luma flag
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_SET_REG_CMD(t_pa_idct_cmd pa_idct_cmd, t_pa_pred_cmd pa_pred_cmd,
							  t_pa_raster raster, t_pa_rb pa_rb, t_dec_rl_luma luma)
{
  HW_PA_WAIT(PA_REG_CMD) = (pa_idct_cmd      |
							 (pa_pred_cmd<<2) |
							 (raster<<4)      |
							 (pa_rb<<6)       |
							 (luma<<8));
}
/*****************************************************************************/
/**
 * \brief 	PA write REG command
 * \author 	Aroua BEN DARYOUG
 * \param   pa_idct_cmd: PA command for IDCT data
 * \param   pa_pred_cmd: PA command for pred data
 * \param   raster : enables raster output
 * \param   pa_rb: size of the square to be sent to IPA
 * \param   luma: luma flag
 * \param   pat_shift : shift if VC1 and overlap
 * \param   pat_clip_en : enable clipping for VC1(clipping is always enabled in VC1)
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_SET_REG_CMD_VC1(t_pa_idct_cmd pa_idct_cmd, t_pa_pred_cmd pa_pred_cmd,
                               t_pa_raster raster, t_pa_rb pa_rb, t_dec_rl_luma luma,
                               t_pa_shift_en pat_shift,t_pa_clip_en pat_clip_en)
{
  HW_PA_WAIT(PA_REG_CMD) = (pa_idct_cmd      |
							 (pa_pred_cmd<<2) |
							 (raster<<4)      |
							 (pa_rb<<6)       |
							 (luma<<8)        |
                                                         (pat_shift<<9)   |
                                                         (pat_clip_en<<10) );
}

/*****************************************************************************/
/**
 * \brief 	PA write FIFO command
 * \author 	Maurizio Colombo
 * \param   pa_idct_cmd: PA command for IDCT data
 * \param   pa_pred_cmd: PA command for pred data
 * \param   raster : enables raster output
 * \param   pa_rb: size of the square to be sent to IPA
 * \param   luma: luma flag
 * \param   repeat: number of iterations for the current cmd
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_WRITE_FIFO_CMD(t_pa_idct_cmd pa_idct_cmd, t_pa_pred_cmd pa_pred_cmd,
							  t_pa_raster raster, t_pa_rb pa_rb, t_dec_rl_luma luma,
							  t_uint16 repeat)
{
/* 2 bits added but only used for VC1 so there will be a specific macro for VC1 */
 HW_PA_WAIT(PA_FIFO_CMD) = (pa_idct_cmd      |
							 (pa_pred_cmd<<2) |
							 (raster<<4)      |
							 (pa_rb<<6)       |
							 (luma<<8)        |
                                                         (0<<9)           |
                                                         (0<<10)          |
							 ((repeat-1)<<11));
}

/*****************************************************************************/
/**
 * \brief 	PA write FIFO command for VC1
 * \author 	Aroua BEN DARYOUG
 * \param   pa_idct_cmd: PA command for IDCT data
 * \param   pa_pred_cmd: PA command for pred data
 * \param   raster : enables raster output
 * \param   pa_rb: size of the square to be sent to IPA
 * \param   luma: luma flag
 * \param   repeat: number of iterations for the current cmd
 *  
 */
/*****************************************************************************/
#pragma inline
static void PA_WRITE_FIFO_CMD_VC1(t_pa_idct_cmd pa_idct_cmd, t_pa_pred_cmd pa_pred_cmd,
                                  t_pa_raster raster, t_pa_rb pa_rb, t_dec_rl_luma luma,
                                  t_pa_shift_en pat_shift,t_pa_clip_en pat_clip_en,
                                  t_uint16 repeat)
{
/* 2 bits added but only used for VC1 so there will be a specific macro for VC1 */
 HW_PA_WAIT(PA_FIFO_CMD) = (pa_idct_cmd      |
							 (pa_pred_cmd<<2) |
							 (raster<<4)      |
							 (pa_rb<<6)       |
							 (luma<<8)        |
                                                         (pat_shift<<9)   |
                                                         (pat_clip_en<<10)|
							 ((repeat-1)<<11));
}





/*********************************************************************
        BLOCK REC
**********************************************************************/





/*****************************************************************************/
/**
 * \brief 	REC soft reset
 * \author 	Maurizio Colombo
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void REC_INIT()
{
	HW_REC(REC_REG_SOFT_RESET) = 1;
	HW_REC(REC_REG_DEF) = 0;
}



/*****************************************************************************/
/**
 * \brief 	REC command
 * \author 	Maurizio Colombo
 * \param 	cmd    :  reconstruction command
 * \param   repeat :  number of command iterations
 */
/*****************************************************************************/
#pragma inline
static void REC_WRITE_FIFO_CMD(t_rec_cmd cmd, t_uint16 repeat)
{
  /* field is hardcoded as always interlaced */
  HW_REC_WAIT(REC_FIFO_CMD) = (cmd | ((repeat-1)<<6));
}


/*****************************************************************************/
/**
 * \brief 	REC command for interlaced mode
 * \author 	Maurizio Colombo
 * \param 	cmd    :  reconstruction command
 * \param 	field  :  indicates TOP/BOTTOM/INTERLACED
 * \param   repeat :  number of command iterations
 */
/*****************************************************************************/
#pragma inline
static void REC_WRITE_FIFO_CMD_INTERLACED(t_rec_cmd cmd, t_rec_field field, t_uint16 repeat)
{
  /* field is hardcoded as always interlaced */
  HW_REC_WAIT(REC_FIFO_CMD) = (cmd | (field<<4) | ((repeat-1)<<6));
}

/*********************************************************************
        BLOCK H264RLC 
**********************************************************************/
/*****************************************************************************/
/**
 * \brief 	H264 RLC soft reset
 * \author 	Richard Rebecca
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void H264RLC_INIT()
{
	HW_H264RLC(H264RLC_REG_SOFT_RESET) = 1;
	HW_H264RLC(H264RLC_REG_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	H264RLC command input selection 
 * \author 	Richard Rebecca
 * \param 	def 0: commands from FIFO, 1: commands from reg
 *  
 */
/*****************************************************************************/
#pragma inline
static void H264RLC_SET_REG_DEF(t_uint16 def)
{
	HW_H264RLC(H264RLC_REG_DEF) = def;
}

/*****************************************************************************/
/**
 * \brief 	H264RLC configuration selection 
 * \author 	Richard Rebecca
 * \param 	config 0: commands from XBUS, 1: commands from CUP
 *  
 */
/*****************************************************************************/
#pragma inline
static void H264RLC_SET_REG_CFG(t_h264rlc_reg_cfg config)
{
	HW_H264RLC(H264RLC_REG_CFG) = config;
}

/*****************************************************************************/
/**
 * \brief 	H264RLC command fifo
 * \author 	Richard Rebecca
 * \param 	cmd
 * \param 	nc: Number derived from the number of non-zero transform coefficient 
		levels in the blocks located to the left of the current block and above
		the current block.
 * \param 	repeat
 *  
 */
/*****************************************************************************/
#pragma inline
static void H264RLC_SET_FIFO_CMD(t_h264rlc_cmd cmd, t_uint16 nc, t_uint16 repeat)
{
	HW_H264RLC(H264RLC_FIFO_CMD) = cmd
                             | ((nc&0x1F) <<2 )
                             | ((repeat-1)<<7);
}


#pragma inline
static void H264RLC_SET_REG_CMD(t_h264rlc_cmd cmd)
{
	HW_H264RLC(H264RLC_REG_CMD) = cmd;
}

/*********************************************************************

                              BLOCK VC1IT

**********************************************************************/
/*****************************************************************************/
/**
 * \brief 	VC1IT soft reset
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VC1IT_INIT()
{
	HW_VC1IT(VC1IT_REG_SOFT_RESET) = 1;
	HW_VC1IT(VC1IT_REG_DEF) = 0;
}
/*****************************************************************************/
/**
 * \brief 	VC1IT set reg def
 * \author 	Aroua BEN DARYOUG
 * \param       def 0: commands from FIFO, 1: commands from reg	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VC1IT_SET_REG_DEF(t_uint16 def)
{
	HW_VC1IT(VC1IT_REG_DEF) = def;
}
/*****************************************************************************/
/**
 * \brief 	VC1IT register command
 * \author 	Aroua BEN DARYOUG
 * \param       size : inverse transform size (8x8 , 8x4 , 4x8, 4x4 )	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VC1IT_SET_REG_CMD(t_vc1it_cmd size)
{
    HW_VC1IT_WAIT(VC1IT_REG_CMD) = size;

}
/*****************************************************************************/
/**
 * \brief 	VC1IT fifo command
 * \author 	Aroua BEN DARYOUG
 * \param       size : inverse transform size (8x8 , 8x4 , 4x8, 4x4 )
 * \param       repeat	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VC1IT_WRITE_FIFO_CMD(t_vc1it_cmd size,t_uint16 repeat)
{
    HW_VC1IT_WAIT(VC1IT_FIFO_CMD) = (size |(repeat-1)<<2) ;

}



/*********************************************************************

                              BLOCK VBF

**********************************************************************/

/*****************************************************************************/
/**
 * \brief 	VBF soft reset (by default we are in fifo mode)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_INIT()
{
    HW_VBF_WAIT(VBF_REG_SOFT_RESET) = 1;
}
/*****************************************************************************/
/**
 * \brief 	VBF configure in fifo or in block mode
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_CONFIGURE(t_vbf_enable_fifo enable_fifo)
{
    HW_VBF_WAIT(VBF_REG_CFG)= enable_fifo;
}
/*****************************************************************************/
/**
 * \brief 	VBF get fifo cmd for a block
 * \author 	Aroua BEN DARYOUG
 * \param       csbp : coded sub-block pattern	
 * \param       block_patition : block partition size 
 */
/*****************************************************************************/
#pragma inline
static void VBF_FIFO_CMD_GET( t_uint16 csbp,t_vbf_block_partition block_patition)
{
    HW_VBF_WAIT(VBF_FIFO_GET_CMD)= csbp | (block_patition << 4) ;
}
/*****************************************************************************/
/**
 * \brief 	VBF send fifo cmd for a n blocks
 * \author 	Aroua BEN DARYOUG
 * \param 	repeat: number of 8x8 blocks to be sent
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_FIFO_CMD_SEND(t_uint16 repeat)
{
    HW_VBF_WAIT(VBF_FIFO_SEND_CMD)=(repeat-1);
} 

/*****************************************************************************/
/**
 * \brief 	verify fullness of all VBF blocks
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VBF_REG_STATUS_FULL_READ()
{
    return HW_VBF_WAIT(VBF_REG_STS_FULL);
}

/*****************************************************************************/
/**
 * \brief 	verify emptiness of all VBF blocks
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VBF_REG_STATUS_EMPTY_READ()
{
    return HW_VBF_WAIT(VBF_REG_STS_EMPTY);
}
/*****************************************************************************/
/**
 * \brief 	read from block 0
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_0(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_0+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 1
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_1(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_1+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 2
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_2(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_2+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 3
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_3(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_3+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 4
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_4(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_4+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 5
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_5(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_5+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 6
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_6(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_6+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 7
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_7(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_7+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 8
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_8(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_8+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 9
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_9(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_9+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 10
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_10(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_10+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 11
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_11(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_BUF_11+index);
}
/*****************************************************************************/
/**
 * \brief 	write in block 0
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_0(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_0+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 1
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_1(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_1+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 2
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_2(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_2+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 3
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_3(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_3+index) = value;
}

/*****************************************************************************/
/**
 * \brief 	write in block 4
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_4(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_4+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 5
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_5(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_5+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 6
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_6(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_6+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 7
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_7(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_7+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 8
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_8(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_8+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 9
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_9(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_9+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 10
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_10(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_10+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 11
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_11(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_BUF_11+index) = value;
}

/*****************************************************************************/
/**
 * \brief 	FW tells VBF what is the overlap direction(ver/hor) and between which blocks
 * \author 	Aroua BEN DARYOUG
 * \param 	block_index_i:left or top block to be overlapped
 * \param 	block_index_j:rieght or bottom block to be overlapped
 * \param 	direction:overlap direction(vertival or horizontal)
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_FIFO_OVSM(t_uint16 block_index_i,t_uint16 block_index_j,t_vbf_direction direction)
{
    HW_VBF_WAIT(VBF_FIFO_OVSM_CMD) = block_index_i |
                                     (block_index_j << 5) |
                                     (direction << 10);
}

/*****************************************************************************/
/**
 * \brief 	return status of previous overlpa command
 * \author 	Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VBF_GET_OVSM_STATUS(void)
{
    return HW_VBF_WAIT(VBF_REG_STS_OVSM)  ;
}
/*****************************************************************************/
/**
 * \brief 	write in block 12(TOP 0)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_12(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_0+index) = value;
}

/*****************************************************************************/
/**
 * \brief 	write in block 13(TOP 1)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_13(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_1+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 14(TOP 2)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_14(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_2+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 15(TOP 3)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_15(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_3+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 16(TOP 4)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_16(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_4+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 17(TOP 5)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_17(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_5+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 18(TOP 6)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_18(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_6+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	write in block 19(TOP 7)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static void VBF_REG_WRITE_IN_BLOCK_19(t_uint16 index,t_sint16 value)
{
    HW_VBF_WAIT(VBF_REG_TOP_7+index) = value;
}
/*****************************************************************************/
/**
 * \brief 	read from block 12(top 0)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_12(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_0+index);
}

/*****************************************************************************/
/**
 * \brief 	read from block 13(top 1)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_13(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_1+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 14(top 2)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_14(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_2+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 15(top 3)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_15(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_3+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 16(top 4)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_16(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_4+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 17(top 5)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_17(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_5+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 18(top 6)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_18(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_6+index);
}
/*****************************************************************************/
/**
 * \brief 	read from block 19(top 7)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 *  
 */
/*****************************************************************************/
#pragma inline
static t_sint16 VBF_REG_READ_FROM_BLOCK_19(t_uint16 index)
{
    return HW_VBF_WAIT(VBF_REG_TOP_7+index);
}

#endif /* _VCU_API_H_ */
