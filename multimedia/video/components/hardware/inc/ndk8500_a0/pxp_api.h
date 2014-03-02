/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PXP_API_H_
#define _PXP_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/
#include "t1xhv_types.h"
#include "vcu_types.h"
#include "macros.h"                /* ASSERT */
#include "t1xhv_hamac_xbus_regs.h" /* register definition */
#include <stwdsp.h>
#include "vcu_blocks_api.h"
#include "dfi_api.h"
#include "dma_api.h"

/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/


/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/


/*****************************************************************************/
/*
 * Exported functions							       
 */
/*****************************************************************************/


/*****************************************************************************/
/**
 * \brief 	Program the register REG_ENC_FRAME_CFG of the pxp
 * \author 	Serge Backert
 * \param   no_rec           : enable/disable reconstruction (disable when 1)
 * \param 	scn_mode         : enable/disable scaning direction  => IGNORED
 * \param 	dct_shift        : enable/disable level shifting (+128) after idct
 * \param 	qtz_table        : enable/disable fetch of each coeff in the 
 *                             quantizer table.
 * \param 	enc_qtz_idc_clip : Clipping range of the INTRA DC quantizer 
 *                             output value
 * \param 	enc_qtz_clip     : Clipping range of all but INTRA DC quantizer 
 *                             output value
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_ENC_FRAME_CFG(t_no_rec       no_rec,
                                      t_scn_mode     scn_mode,
                                      t_dct_shift    dct_shift,
                                      t_qtz_table    qtz_table,
                                      t_enc_qtz_clip enc_qtz_idc_clip,
                                      t_enc_qtz_clip enc_qtz_clip)
{

	/* if table is fetched from qtab no need for any Q-cmd */
	if(qtz_table==QTZ_ENABLE)
	  {
		Q_SET_REG_DEF(1);
	  }
	else
	  {
		Q_SET_REG_DEF(0);		
	  }
	
	/* static cmd for SCN and RLC in encode the order of programmation of 
	   SCN block is done as per the specs*/
	SCAN_CONFIGURE(SCN_ENCODE_MODE);
	SCAN_SET_REG_CMD(SCN_CMD_ZZ);
	SCAN_SET_REG_DEF(1);
	
	/* static DFI macro cmd for JPEG encode */
	if(no_rec==RECONSTRUCTION_DISABLE)
	  {
		DFI_SET_REG(DFI_REG_MACRO_CMD,DFI_JPEG_NORESIZE); 
		DFI_SET_REG(DFI_REG_MACRO_DEF,1);
		RLC_SET_REG_CMD(SEPARATE_INTRA_DC_ENABLE);
		RLC_SET_REG_DEF(1);
	  }
	else
	  { /* MPEG-4 encode */
		IQ_CONFIGURE(qtz_table, QTZ_QP_DISABLE, CLIPPING_ENABLE,
				 dct_shift, IQTZ_CLIP_WARN_DISABLE, 6,
				 MISMATCH_DISABLE);
		IDCT_CONFIGURE(DCT_SHIFT_DISABLE);
	  }
		
	Q_CONFIGURE(qtz_table, enc_qtz_idc_clip, enc_qtz_clip);
	DCT_CONFIGURE(dct_shift);

} /* end of inline WRITE_PXP_REG_ENC_FRAME_CFG */

/*****************************************************************************/
/**
 * \brief 	program the register REG_DEC_FRAME_CFG of the pxp
 * \author 	Serge Backert
 * \param   intra_dc_r : INTRA DC r dead zone parameter 
 * \param   intra_dc_s : INTRA DC s divide by q or 2q in quantization formula
 * \param   intra_dc_t : INTRA DC t round or trunk result of quantization
 * \param   intra_ac_r : INTRA AC r dead zone parameter
 * \param   intra_ac_s : INTRA AC s divide by q or 2q in quantization formula
 * \param   intra_ac_t : INTRA AC t round or trunk result of quantization
 * \param   inter_r    : INTER    r dead zone parameter
 * \param   inter_ac_s : INTER    s divide by q or 2q in quantization formula
 * \param   inter_ac_t : INTER    t round or trunk result of quantization
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_ENC_QTZ_PARAM(t_sint16                   intra_dc_r,
                                      t_enc_qtz_param_intra_dc_s intra_dc_s,
                                      t_enc_qtz_param_intra_dc_t intra_dc_t,
                                      t_sint16                   intra_ac_r,
                                      t_enc_qtz_param_intra_ac_s intra_ac_s,
                                      t_enc_qtz_param_intra_ac_t intra_ac_t,
                                      t_sint16                   inter_r,
                                      t_enc_qtz_param_inter_s    inter_s,
                                      t_enc_qtz_param_inter_t    inter_t)
{
    /* sanity checks ( _USE_ASSERT_ only) */
    ASSERT((intra_dc_r>=-3) && (intra_dc_r<=3));
    ASSERT((intra_ac_r>=-3) && (intra_ac_r<=3));
    ASSERT((inter_r>=-3)    && (inter_r<=3));

	Q_CONFIGURE_2(intra_dc_r, intra_dc_s, intra_dc_t,
				  intra_ac_r, intra_ac_s, intra_ac_t,
				  inter_r, inter_s, inter_t);

} /* end of inline PXP_SET_REG_ENC_QTZ_PARAM */


/*****************************************************************************/
/**
 * \brief 	Program the register REG_DEC_FRAME_CFG of the pxp
 * \author 	Maurizio Colombo
 * \param 	path_iprd : Inverse quantization before or after prediction
 * \param 	scn_mode: enable/disable scaning direction
 * \param 	dct_shift: enable/disable level shifting (+128) after idct
 * \param 	dec_prediction: enable/disable inverse dc prediction  => IGNORED
 * \param 	nb_of_blocks_per_macroblock: number of blocks per macro-block
 * \param 	qtz_table: enable/disable quantization
 * \param 	iqtz_clip_warn: enable/disable warning for clipped values after iq
 * \param 	read_idct_out: enable/disable the output of idct on the Xbus => IGNORED
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_DEC_FRAME_CFG(t_path_iprd      path_iprd,
                                      t_scn_mode       scn_mode,
                                      t_dct_shift      dct_shift,
                                      t_dec_prediction dec_prediction,
                                      t_uint16         nb_of_blocks_per_macroblock,
                                      t_qtz_table      qtz_table,
                                      t_iqtz_clip_warn iqtz_clip_warn,
                                      t_read_idct_out  read_idct_out)


{
    /* sanity checks ( _USE_ASSERT_ only) */
    ASSERT(!((dec_prediction==1) && (scn_mode==0)));
    ASSERT(nb_of_blocks_per_macroblock<=8);
    ASSERT(nb_of_blocks_per_macroblock>0);
    ASSERT( !((read_idct_out == 1) && (dct_shift == 0)));

	IQ_CONFIGURE(qtz_table, QTZ_QP_DISABLE, CLIPPING_ENABLE,
				 dct_shift, iqtz_clip_warn, nb_of_blocks_per_macroblock,
				 MISMATCH_DISABLE);

	IDCT_CONFIGURE(dct_shift);

	IPRD_CONFIGURE(path_iprd,IPRD_XBUS);

	SCAN_CONFIGURE(SCN_DECODE_MODE);
	
	/* if scan is always the same, set it statically */
	if(scn_mode==SCN_DIR_DISABLE)
	  {
		SCAN_SET_REG_CMD(SCN_CMD_ZZ);
		SCAN_SET_REG_DEF(1);
	  }


} /* end of inline WRITE_PXP_REG_DEC_FRAME_CFG */

/*****************************************************************************/
/**
 * \brief 	Program the register REG_DEC_FRAME_CFG of the pxp
 * \author 	Maurizio Colombo
 * \param 	path_iprd : Inverse quantization before or after prediction
 * \param 	scn_mode: enable/disable scaning direction
 * \param 	dct_shift: enable/disable level shifting (+128) after idct
 * \param 	dec_prediction: enable/disable inverse dc prediction  => IGNORED
 * \param 	nb_of_blocks_per_macroblock: number of blocks per macro-block
 * \param 	qtz_table: enable/disable quantization
 * \param 	iqtz_clip_warn: enable/disable warning for clipped values after iq
 * \param 	read_idct_out: enable/disable the output of idct on the Xbus => IGNORED
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_DEC_FRAME_CFG_MP2(t_path_iprd      path_iprd,
										  t_scn_mode       scn_mode,
										  t_dct_shift      dct_shift,
										  t_dec_prediction dec_prediction,
										  t_uint16         nb_of_blocks_per_macroblock,
										  t_qtz_table      qtz_table,
										  t_iqtz_clip_warn iqtz_clip_warn,
										  t_read_idct_out  read_idct_out,
										  t_iq_mismatch    mismatch,
										  t_qtz_table_qp   qp_table_qp)
{
	IQ_CONFIGURE(qtz_table, qp_table_qp, CLIPPING_ENABLE,
				 dct_shift, iqtz_clip_warn, nb_of_blocks_per_macroblock,
				 mismatch);

	IDCT_CONFIGURE(dct_shift);

	IPRD_CONFIGURE(path_iprd,IPRD_XBUS);

	SCAN_CONFIGURE(SCN_DECODE_MODE);
	
	/* if scan is always the same, set it statically */
	if(scn_mode==SCN_DIR_DISABLE)
	  {
		SCAN_SET_REG_CMD(SCN_CMD_ZZ);
		SCAN_SET_REG_DEF(1);
	  }


} /* end of inline WRITE_PXP_REG_DEC_FRAME_CFG */



/*****************************************************************************/
/**
 * \brief 	Program the register REG_DEC_FRAME_CFG of the pxp
 * \author 	Aroua BEN DARYOUG
 * \param 	path_iprd : Inverse quantization before or after prediction
 * \param 	scn_mode: undirect way to choose scan direction
 * \param 	dct_shift: enable/disable level shifting (+128) after idct
 * \param 	dec_prediction: enable/disable inverse dc prediction  => IGNORED
 * \param 	nb_of_blocks_per_macroblock: number of blocks per macro-block
 * \param 	qtz_table: enable/disable quantization
 * \param 	iqtz_clip_warn: enable/disable warning for clipped values after iq
 * \param 	read_idct_out: enable/disable the output of idct on the Xbus => IGNORED
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_DEC_FRAME_CFG_PROG(t_path_iprd      path_iprd,
                                           t_scn_mode       scn_mode,
                                           t_dct_shift      dct_shift,
                                           t_dec_prediction dec_prediction,
                                           t_uint16         nb_of_blocks_per_macroblock,
                                           t_qtz_table      qtz_table,
                                           t_iqtz_clip_warn iqtz_clip_warn,
                                           t_read_idct_out  read_idct_out)


{
    /* sanity checks ( _USE_ASSERT_ only) */
    ASSERT(!((dec_prediction==1) && (scn_mode==0)));
    ASSERT(nb_of_blocks_per_macroblock<=8);
    ASSERT(nb_of_blocks_per_macroblock>0);
    ASSERT( !((read_idct_out == 1) && (dct_shift == 0)));

	IQ_CONFIGURE(qtz_table, QTZ_QP_DISABLE, CLIPPING_ENABLE,
				 dct_shift, iqtz_clip_warn, nb_of_blocks_per_macroblock,
				 MISMATCH_DISABLE);

	IDCT_CONFIGURE(dct_shift);

	IPRD_CONFIGURE(path_iprd,IPRD_XBUS);

	SCAN_CONFIGURE(SCN_DECODE_MODE);
	
	/* if scan is always the same, set it statically */
	if(scn_mode==SCN_DIR_DISABLE)
	  {
		SCAN_SET_REG_CMD(SCN_CMD_ROW);
		SCAN_SET_REG_DEF(1);
	  }


} /* end of inline WRITE_PXP_REG_DEC_FRAME_CFG_PROG */


/*****************************************************************************/
/**
 * \brief 	program the register PXP_REG_DEC_IQTZ_PARAM of the pxp
 * \author 	Maurizio Colombo
 * \param 	intra_dc_a: enable/disable coeff dc coeff a.
 * \param 	intra_dc_b: enable/disable coeff dc coeff b.
 * \param 	intra_dc_c: enable/disable coeff dc coeff c.
 * \param   intra_dc_d: value of coeff dc d
 * \param 	intra_dc_e: enable/disable coeff dc coeff e.
 * \param 	intra_dc_f: enable/disable coeff dc coeff f.
 * \param 	intra_ac_a: enable/disable coeff ac coeff a.
 * \param 	intra_ac_b: enable/disable coeff ac coeff b.
 * \param 	intra_ac_c: enable/disable coeff ac coeff c.
 * \param   intra_ac_d: value of coeff ac d
 * \param 	intra_ac_e: enable/disable coeff ac coeff e.
 * \param 	intra_ac_f: enable/disable coeff ac coeff f.
 * \param 	others_a  :enable/disable coeff others a
 * \param 	others_b  :enable/disable coeff others b
 * \param 	others_c  :enable/disable coeff others c
 * \param   others_d  : value of coeff others d 
 * \param 	others_e  :enable/disable coeff others e
 * \param 	others_f  :enable/disable coeff others f
 *  
 * Program the coefficients a,b,c for dc and others for generic inverse quantizer
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_DEC_IQTZ_PARAM(t_dec_iqtz_param_intra_dc_a intra_dc_a,
                                       t_dec_iqtz_param_intra_dc_b intra_dc_b,
                                       t_dec_iqtz_param_intra_dc_c intra_dc_c,
									   t_uint16                    intra_dc_d,
									   t_dec_iqtz_param_intra_dc_e intra_dc_e,
                                       t_dec_iqtz_param_intra_dc_f intra_dc_f,
									   t_dec_iqtz_param_intra_ac_a intra_ac_a,
                                       t_dec_iqtz_param_intra_ac_b intra_ac_b,
                                       t_dec_iqtz_param_intra_ac_c intra_ac_c,
									   t_uint16                    intra_ac_d,
									   t_dec_iqtz_param_intra_ac_e intra_ac_e,
                                       t_dec_iqtz_param_intra_ac_f intra_ac_f,
                                       t_dec_iqtz_param_others_a   others_a,
                                       t_dec_iqtz_param_others_b   others_b,
                                       t_dec_iqtz_param_others_c   others_c,
									   t_uint16                    others_d,
									   t_dec_iqtz_param_others_e   others_e,
                                       t_dec_iqtz_param_others_f   others_f)
{
	IQ_CONFIGURE_PARAMS_INTRA(intra_dc_a, intra_dc_b, intra_dc_c, 
							  intra_dc_d, intra_dc_e, intra_dc_f,
							  intra_ac_a, intra_ac_b, intra_ac_c,
							  intra_ac_d, intra_ac_e, intra_ac_f);
	IQ_CONFIGURE_PARAMS_INTER(others_a, others_b, others_c,
							  others_d, others_e, others_f);
  
} /* end of SET_PXP_REG_DEC_IQTZ_PARAM */

/*****************************************************************************/
/**
 * \brief 	program DFI at macroblock level + RLC 
 * \author 	Maurizio Colombo
 * \param 	enc_rl_sep: enable/disable separation of the intra dc 
 *              run/level at the output of the RLC 
 * \param 	dec_intra: intra/inter block decode  => IGNORED
 * \param   encode : DFI encode/decode 
 * \param   dfi_cmd_luma/chroma : dfi macro commands
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_MBLOCK_CFG( t_enc_rl_sep enc_rl_sep,
                                     t_dec_intra  dec_intra,
									 t_dfi_mode   encode,
									 t_dfi_mbtype dfi_cmd_luma,
									 t_dfi_mbtype dfi_cmd_chroma)
{
  DFI_MACRO_CMD(encode, 4, dfi_cmd_luma);
  DFI_MACRO_CMD(encode, 2, dfi_cmd_chroma);

} /* end of inline SET_PXP_FIFO_MBLOCK_CFG */


/*****************************************************************************/
/**
 * \brief 	program the VCU FIFOs at block level
 * \author 	Maurizio Colombo
 * \param 	scan_direction : type of scanning used
 * \param 	rl_luma: type of block (luma/chroma)
 * \param   flag short header: tells whether we are in short header or not 
 * 
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_BLOCK_CFG_VDCMP4( t_scn_dir           scan_direction,
										   t_dec_rl_luma       rl_luma,
										   t_uint16            flag_short_header)
{
    ASSERT(scan_direction <= 3);

	/* IPRD not in the short header data flow, SCN is set statically for SH */
	if(flag_short_header==0)
	  {
		/* AC/DC pred dir is inferred from scan direction */
		IPRD_WRITE_FIFO_CMD(scan_direction,1);  
	  
		switch(scan_direction)
		  {
		  case ZIG_ZAG_SCAN:
			SCAN_SEND_CMD(SCN_CMD_ZZ,1);
			break;
		  case VERTICAL_SCAN:
			SCAN_SEND_CMD(SCN_CMD_AH,1);
			break;
		  case HORIZONTAL_SCAN:
			SCAN_SEND_CMD(SCN_CMD_AV,1);
			break;
		  case ROW_SCAN:
			SCAN_SEND_CMD(SCN_CMD_ROW,1);
			break;
		  }

	  }

} /* end of SET_PXP_FIFO_BLOCK_CFG_VDCMP4 */ 


/*****************************************************************************/
/**
 * \brief 	programming of QTAB at block level
 * \author 	Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_BLOCK_CFG_VECJPG(void)
{
  QTAB_SEND_CMD(QTAB_ROW_SCAN,QTAB_TAB_0,4);
  QTAB_SEND_CMD(QTAB_ROW_SCAN,QTAB_TAB_1,2);
}


/*****************************************************************************/
/**
 * \brief 	Write predictor coefficient for the IPRD step
 * \author 	Maurizio Colombo
 * \param 	coeff predictor coefficient for the IPRD step
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_ACDC_COEFF(t_uint16 coeff)
{
    ASSERT(coeff <= 4095);
	IPRD_WRITE_FIFO_IN(coeff);
} /* end of PXP_SET_FIFO_ACDC_COEFF */


/*****************************************************************************/
/**
 * \brief 	Read predictor coefficient for the IPRD step
 * \author 	Maurizio Colombo
 * \return  coeff predictor coefficient for the IPRD step
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 PXP_GET_FIFO_ACDC_COEFF()
{
  return IPRD_GET_FIFO_OUT();
} /* end of PXP_GET_FIFO_ACDC_COEFF */



/*****************************************************************************/
/**
 * \brief 	Set QP for inverse quantization of the current 16x16 MACROBLOCK
 * \author 	Maurizio Colombo
 * \brief   This is different from 8815
 * \param 	quant_dc_l quantizer for luma DC coefficients
 * \param   quant_dc_c quantizer for chroma DC coefficients
 * \param 	quant_ac quantizer for luma/chroma AC coefficients
 * \param 	inter  current macroblock is intra/inter
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_IQUANT_MACROBLOCK(t_uint16 quant_dc_l,
										   t_uint16 quant_dc_c,
										   t_uint16 quant_ac,
										   t_dec_intra inter)
{

  IQ_SET_FIFO_QUANT(quant_dc_l,quant_ac,inter,4);
  IQ_SET_FIFO_QUANT(quant_dc_c,quant_ac,inter,2);

} /* end of PXP_SET_FIFO_QUANT */ 

/*****************************************************************************/
/**
 * \brief 	Set QP for quantization of the current 16x16 MACROBLOCK
 * \author 	Maurizio Colombo
 * \brief   This is different from 8815
 * \param 	quant_dc_l quantizer for luma DC coefficients
 * \param   quant_dc_c quantizer for chroma DC coefficients
 * \param 	quant_ac quantizer for luma/chroma AC coefficients
 * \param 	inter  current macroblock is intra/inter
 *  
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_FIFO_QUANT_MACROBLOCK(t_uint16 quant_dc_l,
										  t_uint16 quant_dc_c,
										  t_uint16 quant_ac,
										  t_enc_intra inter)
{

  Q_SET_FIFO_QUANT(quant_dc_l,quant_ac,inter,4);
  Q_SET_FIFO_QUANT(quant_dc_c,quant_ac,inter,2);

} /* end of PXP_SET_FIFO_QUANT */

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for one not coded macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_NOTCODED_MB()
{
    /* 4 luma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_IDLE,PA_PRED_GET,PA_RASTER_8x8,PA_RB_NO,DECODE_LUMA_BLOCK,4);
    /* 2 chroma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_IDLE,PA_PRED_GET,PA_RASTER_8x8,PA_RB_NO,DECODE_CHROMA_BLOCK,2);

	REC_WRITE_FIFO_CMD(REC_CMD_8x8_LUMA,4);
	REC_WRITE_FIFO_CMD(REC_CMD_8x8_CHROMA,2);
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for a coded macroblock after a not
            coded macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_AFTER_NOTCODED_MB()
{
  /* skip the 6 dummy blocks that have been pushed to flush IDCT */
  PA_WRITE_FIFO_CMD(PA_IDCT_SKIP,PA_PRED_IDLE,PA_RASTER_8x8,PA_RB_NO,DECODE_LUMA_BLOCK,4);
  PA_WRITE_FIFO_CMD(PA_IDCT_SKIP,PA_PRED_IDLE,PA_RASTER_8x8,PA_RB_NO,DECODE_CHROMA_BLOCK,2);
}


/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for intra macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_INTRA_MB()
{
    /* 4 luma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_GET,PA_PRED_IDLE,PA_RASTER_8x8,PA_RB_NO,DECODE_LUMA_BLOCK,4);
    /* 2 chroma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_GET,PA_PRED_IDLE,PA_RASTER_8x8,PA_RB_NO,DECODE_CHROMA_BLOCK,2);
	REC_WRITE_FIFO_CMD(REC_CMD_8x8_LUMA,4);
	REC_WRITE_FIFO_CMD(REC_CMD_8x8_CHROMA,2);
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for inter macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_INTER_MB()
{
    /* 4 luma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_GET,PA_PRED_GET,PA_RASTER_8x8,PA_RB_NO,DECODE_LUMA_BLOCK,4);
    /* 2 chroma blocks */
    PA_WRITE_FIFO_CMD(PA_IDCT_GET,PA_PRED_GET,PA_RASTER_8x8,PA_RB_NO,DECODE_CHROMA_BLOCK,2);
	REC_WRITE_FIFO_CMD(REC_CMD_8x8_LUMA,4);
	REC_WRITE_FIFO_CMD(REC_CMD_8x8_CHROMA,2);
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder for error conc procedure
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_ERC()
{
  /* 4 luma blocks */
  PA_WRITE_FIFO_CMD(PA_IDCT_SKIP,PA_PRED_SKIP,PA_RASTER_8x8,PA_RB_NO,DECODE_LUMA_BLOCK,4);
  /* 2 chroma blocks */
  PA_WRITE_FIFO_CMD(PA_IDCT_SKIP,PA_PRED_SKIP,PA_RASTER_8x8,PA_RB_NO,DECODE_CHROMA_BLOCK,2);

  /* No REC needed */

}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for one not coded macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_NOTCODED_MB_RASTER()
{
  /* raster is not supported on 8820 */
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for a coded macroblock after a not
            coded macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_AFTER_NOTCODED_MB_RASTER()
{
  /* raster is not supported on 8820 */
}


/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for intra macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_INTRA_MB_RASTER()
{
  /* raster is not supported on 8820 */
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder inside for inter macroblock
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_CODED_INTER_MB_RASTER()
{
  /* raster is not supported on 8820 */
}

/*****************************************************************************/
/**
 * \brief 	Program the Post-Adder for error concealment procedure in raster
 * \author  Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void PXP_PA_ERC_RASTER()
{
  /* raster is not supported on 8820 */
}


/*****************************************************************************/
/**
 * \brief 	read back an idcted coefficient from the Fifo(after the IDCT)
 * \author 	jean-marc volle
 * \return 	 coefficient value
 * 
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 PXP_GET_FIFO_IDCT_OUT()
{
  return (t_uint16)DFI_GET_REG_WAIT(DFI_FIFO_FIFO_DST_LSB);
  /* NOTE: How do we handle "last" ?     */

}

/*****************************************************************************/
/**
 * \brief 	reset the PXP XBUS registers
 * \author 	jean-marc volle
 *
 * This is not a real soft reset since it is not equivalent to a hardware
 * reset, i.e. the flip-flops inside the block are not reset. 
 */
/*****************************************************************************/
#pragma inline 
static void PXP_INIT()
{
  Q_INIT();
  DCT_INIT();
  QTAB_INIT();
  IQ_INIT();
  SCAN_INIT();
  IDCT_INIT();
  IPRD_INIT();
  PA_INIT();
  REC_INIT();
  RLC_INIT();
  FIFO_INIT();
  FIFO2_INIT();
  VLC_INIT();
} /* end of PXP_INIT */

/*****************************************************************************/
/**
 * \brief 	reset the PXP XBUS registers
 * \author 	vipul singh lodhi
 *
 * This is not a real soft reset since it is not equivalent to a hardware
 * reset, i.e. the flip-flops inside the block are not reset. 
 * 
 *This is a optimization used in jpegdecode
 */
/*****************************************************************************/

#pragma inline 
static void PXP_MINUS_QTAB_INIT()  
{
  Q_INIT();
  DCT_INIT();
  IQ_INIT();
  SCAN_INIT();
  IDCT_INIT();
  IPRD_INIT();
  PA_INIT();
  REC_INIT();
  RLC_INIT();
  FIFO_INIT();
  FIFO2_INIT();
  VLC_INIT();
}/* end of PXP_MINUS_QTAB_INIT */
/*****************************************************************************/
/**
 * \brief 	Reads (run,level,events) after scan
 * \author 	Serge Backert
 * \return  structure with (run,level,events)
 * 
 */
/*****************************************************************************/
#pragma inline
static ts_lrl PXP_GET_LAST_RUN_LEVEL()
{
  ts_lrl last_run_level;
  t_sint16 value; /**< \brief auxiliary variable for aggregate read in OUT1 */
  t_uint16 value1;
  
  value = DFI_GET_REG_WAIT(DFI_FIFO_FIFO2_DST_LSB);
  value1 = DFI_GET_REG_WAIT(DFI_FIFO_FIFO2_DST_MSB);
  
  last_run_level.last  = value1>>6; /* bit 22 */
  last_run_level.run   = value1&0x3F;
  last_run_level.level = value;

  return last_run_level;
} /* end of PXP_GET_LAST_RUN_LEVEL */



/*****************************************************************************/
/**
 * \brief 	Program quantization coeff at index to value
 * \author 	Serge Backert
 * \param   index of coefficient in table  => IGNORED IN 8820
 * \param   value to program
 * \param   last  set to 1 if it is the last access to QTAB from XBUS
 * 
 */
/*****************************************************************************/
#pragma inline
static void PXP_SET_REG_QP_TABLE(t_uint16 index,
                                 t_sint16 value,
								 t_uint16 last)
{
  QTAB_SET_COEFF(value,last);
} /* end of PXP_SET_REG_QP_TABLE */

/*****************************************************************************/
/**
 * \brief 	read back an idcted coefficient from the PXP
 * \author 	Maurizio Colombo
 * \return 	 coefficient value
 * 
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 PXP_GET_FIFO_CLIP_OUT()
{
  return IQ_GET_FIFO_CLIP_OUT();
}

#endif /* _PXP_API_H_ */
