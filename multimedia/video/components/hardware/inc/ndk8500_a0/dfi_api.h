/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DFI_API_H_
#define _DFI_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/
#include "t1xhv_types.h"
#include "t1xhv_hamac_xbus_regs.h" /* register definition */
#include <stwdsp.h>
#include "macros.h"                /* ASSERT */

/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get DFI_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_DFI(a) (*(volatile MMIO t_uint16 *)(DFI_BASE+a))
#define HW_DFI_WAIT(a) (*(volatile __XIO t_uint16 *)(DFI_BASE+a))


/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/
/* defines the unit id */
typedef enum
{
    DFI_BDU = DFI_BDU_ID,
    DFI_BPU = DFI_BPU_ID,
    DFI_IMC = DFI_IMC_ID,
    DFI_IPA = DFI_IPA_ID,
    DFI_DCT = DFI_DCT_ID,
    DFI_IDCT = DFI_IDCT_ID,
    DFI_H264FITQ = DFI_H264FITQ_ID,
    DFI_Q = DFI_Q_ID,
    DFI_IQ = DFI_IQ_ID,
    DFI_SCN = DFI_SCN_ID,
    DFI_RLC = DFI_RLC_ID,
    DFI_H264RLC = DFI_H264RLC_ID,
    DFI_IPRD = DFI_IPRD_ID,
    DFI_PA = DFI_PA_ID,
    DFI_REC = DFI_REC_ID,
    DFI_FIFO = DFI_FIFO_ID,
    DFI_QTAB = DFI_QTAB_ID,
    DFI_VC1IT = DFI_VC1IT_ID,
    DFI_VBF = DFI_VBF_ID,
    DFI_FIFO2 = DFI_FIFO2_ID,
    DFI_VLC = DFI_VLC_ID
} t_dfi_unit_id;


/* defines the coding mode in a macro command */
typedef enum
{
    DFI_DECODE_MODE = 0,
    DFI_ENCODE_MODE = 1
} t_dfi_mode;

typedef enum
{
    NOT_LAST = 0,    /**< \brief the current coeff is NOT the last of the current block   */
    LAST =1          /**< \brief the current coeff is the last of the current block   */

} t_coeff_last;

/* defines the current macroblock type at the component level */
typedef enum
{
  DFI_JPEG_NORESIZE_XBUS      = 0x0,   /* IMC-DCT-Q-SCN-RLC-XBUS for encode */
                                    /* XBUS-IQ-SCN-IDCT-PA-REC-DMA for decode */

  DFI_JPEG_NORESIZE           = 0x1,   /* BDU-IQ-SCN-IDCT-PA-REC-DMA for decode */

  DFI_JPEG_RESIZE_XBUS        = 0x2,   /* XBUS-IQ-SCN-IDCT-FIFO-XBUS for decode */
                                     /* Same as DFI_JPEG_NORESIZE_XBUS for encode */

  DFI_JPEG_RESIZE             = 0x3,   /* BDU-IQ-SCN-IDCT-FIFO-XBUS for decode */
  
  DFI_JPEG_FLUSH              = 0x4,   

  DFI_MPEG4_SH_INTRA_LUMA     = 0x5,   /* BDU-IQ-SCN-IDCT-FIFO-PA-REC-DMA for decode */
                                     /* IMC-DCT-Q-SCN-RLC-XBUS + IQ-IDCT-FIFO-PA-REC-DMA for encode */

  DFI_MPEG4_SH_INTRA_CHROMA   = 0x6,   /* BDU-IQ-SCN-IDCT-FIFO-PA-REC-DMA for decode */
                                     /* IMC-DCT-Q-SCN-RLC-XBUS + IQ-IDCT-FIFO-PA-REC-DMA for encode */

  DFI_MPEG4_SH_INTER_LUMA     = 0x7,   /* BDU-IQ-SCN-IDCT-FIFO-(IMC)PA-REC-DMA for decode */
                                     /* IMC-DCT-Q-RLC-SCN-XBUS + IQ-IDCT-FIFO-(IMC)PA-REC for enc */ 

  DFI_MPEG4_SH_INTER_CHROMA   = 0x8,   /* BDU-IQ-SCN-IDCT-FIFO-(IMC)PA-REC-DMA for decode */
                                     /* IMC-DCT-Q-RLC-SCN-XBUS + IQ-IDCT-FIFO-(IMC)PA-REC for enc */ 

  DFI_MPEG4_SH_FLUSH          = 0x9,
  
  DFI_MPEG4_SH_INTER_NOTCODED = 0xA,

  DFI_MPEG4_SP_INTRA_LUMA     = 0xB,   /* BDU-IPRD-IQ-SCN-IDCT-FIFO-PA-REC-DMA for decode */
  DFI_MPEG4_SP_INTRA_CHROMA   = 0xC, /* BDU-IPRD-IQ-SCN-IDCT-FIFO-PA-REC-DMA for decode */
  DFI_MPEG4_SP_INTER_LUMA     = 0xD, /* BDU-IQ-SCN-IDCT-FIFO-(IMC)PA-REC-DMA for decode */
  DFI_MPEG4_SP_INTER_CHROMA   = 0xE, /* BDU-IQ-SCN-IDCT-FIFO-(IMC)PA-REC-DMA for decode */
  
  DFI_MPEG4_SP_FLUSH          = 0xF, 

  DFI_MPEG4_SP_INTER_NOTCODED =0x10,

  DFI_H263_ANNEXI_INTRA_LUMA  = 0x11,
  DFI_H263_ANNEXI_INTRA_CHROMA= 0x12,

  DFI_VC1                     = 0x1D, 
  DFI_H264                    = 0x1E, 

  DFI_H264_DC_LUMA            = 0x28,     
  DFI_H264_DC_CHROMA          = 0x29,     
  DFI_H264_I_SKIP_LUMA        = 0x2A,     
  DFI_H264_I_SKIP_CHROMA      = 0x2B,     
  DFI_H264_B_LUMA             = 0x2C,     
  DFI_H264_B_CHROMA           = 0x2D,     

  DFI_MPEG4_SH_INTRA_LUMA_BPU     = 0x30,     
  DFI_MPEG4_SH_INTRA_CHROMA_BPU   = 0x31,     
  DFI_MPEG4_SH_INTER_LUMA_BPU     = 0x32,    
  DFI_MPEG4_SH_INTER_CHROMA_BPU   = 0x33,    
  DFI_MPEG4_SH_INTER_NOTCODED_BPU = 0x34, 
  DFI_MPEG4_SP_INTRA_LUMA_BPU     = 0x35,    
  DFI_MPEG4_SP_INTRA_CHROMA_BPU   = 0x36,    
  DFI_MPEG4_SP_INTER_LUMA_BPU     = 0x37,    
  DFI_MPEG4_SP_INTER_CHROMA_BPU   = 0x38,    
  DFI_MPEG4_SP_INTER_NOTCODED_BPU = 0x39,

  DFI_MPEG2_INTRA_LUMA            = 0x42,
  DFI_MPEG2_INTRA_CHROMA          = 0x43,
  DFI_MPEG2_INTER_LUMA            = 0x44,
  DFI_MPEG2_INTER_CHROMA          = 0x45,
  DFI_MPEG2_FLUSH                 = 0x46,
  DFI_MPEG2_INTER_NOTCODED        = 0x47,
  DFI_MPEG4_ASP_INTRA_LUMA        = 0x4A, 
  DFI_MPEG4_ASP_INTRA_CHROMA      = 0x4B,
  DFI_MPEG4_ASP_INTER_LUMA        = 0x4C,
  DFI_MPEG4_ASP_INTER_CHROMA      = 0x4D,
  DFI_MPEG4_ASP_FLUSH             = 0x4E,
  DFI_MPEG4_ASP_INTER_NOTCODED    = 0x4F
} t_dfi_mbtype;

/* Enable default command register */
typedef enum
{
  DFI_DEFAULT_SRC_DST_REGS = 1,
  DFI_SRC_DST_FIFO         = 0
}t_dfi_srcdst_selection;

/* unit status value */
typedef enum
{
  DFI_UNIT_STATUS_IDLE                = 0,
  DFI_UNIT_STATUS_SRC_FIFO_EMPTY      = 1,
  DFI_UNIT_STATUS_SRC_FIFO_NOT_READY  = 2,
  DFI_UNIT_STATUS_DST_FIFO_EMPTY      = 3,
  DFI_UNIT_STATUS_DST_FIFO_NOT_READY  = 4,
  DFI_UNIT_STATUS_RUNNING             = 5
}t_dfi_unit_status;


/*******************************************************
 *
 *    SRC ID LIST
 *
 *******************************************************/

/* list of the IDs corresponding to IMC sources */ 
/* NOT PROGRAMMABLE */
typedef enum 
{
  DFI_BPU_SRC_FROM_XBUS          = 0,        /* XBUS is BPU's source */
  DFI_BPU_SRC_FROM_VLC           = 1,        /* VLC is BPU's source */
  DFI_BPU_SRC_FROM_H264RLC       = 4,        /* H264RLC is BPU's source */
  DFI_BPU_SRC_FROM_FIFO2         = 5         /* FIFO2 is BPU's source */
}t_dfi_bpu_src_id;


/* list of the IDs corresponding to IPA sources */ 
/* not programable */

/* list of the IDs corresponding to DCT sources */ 
typedef enum 
{
  DFI_DCT_SRC_FROM_XBUS          = 0,        
  DFI_DCT_SRC_FROM_IMC_T         = 2   
}t_dfi_dct_src_id;

/* list of the IDs corresponding to IDCT sources */ 
typedef enum 
{
  DFI_IDCT_SRC_FROM_XBUS          = 0,        
  DFI_IDCT_SRC_FROM_SCN           = 2,   
  DFI_IDCT_SRC_FROM_IQ            = 3   
}t_dfi_idct_src_id;

/* list of the IDs corresponding to H264FITQ sources */ 
typedef enum 
{
  DFI_H264FITQ_SRC_FROM_XBUS        = 0,        
  DFI_H264FITQ_SRC_FROM_SCN         = 2   
}t_dfi_h264fitq_src_id;

/* list of the IDs corresponding to Q sources */ 
typedef enum 
{
  DFI_Q_SRC_FROM_XBUS          = 0,        
  DFI_Q_SRC_FROM_DCT           = 2   
}t_dfi_q_src_id;

/* list of the IDs corresponding to IQ sources */ 
typedef enum 
{
  DFI_IQ_SRC_FROM_XBUS          = 0,        
  DFI_IQ_SRC_FROM_BDU           = 2,   
  DFI_IQ_SRC_FROM_IPRD          = 3,   
  DFI_IQ_SRC_FROM_Q             = 4   
}t_dfi_iq_src_id;

/* list of the IDs corresponding to SCN sources */ 
typedef enum 
{
  DFI_SCN_SRC_FROM_XBUS          = 0,        
  DFI_SCN_SRC_FROM_Q             = 2,   
  DFI_SCN_SRC_FROM_H264FITQ      = 3,   
  DFI_SCN_SRC_FROM_BDU           = 4,   
  DFI_SCN_SRC_FROM_IQ            = 5,   
  DFI_SCN_SRC_FROM_IPRD          = 6,
  DFI_SCN_SRC_FROM_FIFO2         = 7   
}t_dfi_scn_src_id;

/* list of the IDs corresponding to RLC sources */ 
typedef enum 
{
  DFI_RLC_SRC_FROM_XBUS          = 0,        
  DFI_RLC_SRC_FROM_SCN           = 2
}t_dfi_rlc_src_id;

/* list of the IDs corresponding to H264 RLC sources */ 
typedef enum 
{
  DFI_H264RLC_SRC_FROM_XBUS          = 0,        
  DFI_H264RLC_SRC_FROM_SCN           = 2
}t_dfi_h264rlc_src_id;

/* list of the IDs corresponding to IPRD sources */ 
typedef enum 
{
  DFI_IPRD_SRC_FROM_XBUS          = 0,        
  DFI_IPRD_SRC_FROM_BDU           = 2,
  DFI_IPRD_SRC_FROM_IQ            = 3
}t_dfi_iprd_src_id;

/* list of the IDs corresponding to PA sources */ 
typedef enum 
{
  DFI_PA_SRC_FROM_BDU                 = 0x0,
  DFI_PA_SRC_FROM_VBF                 = 0x2,
  DFI_PA_SRC_FROM_IMC_P               = 0x3,
  DFI_PA_SRC_FROM_XBUS                = 0x4,        
  DFI_PA_SRC_FROM_XBUS_IMC_P          = 0x6,
  DFI_PA_SRC_FROM_FIFO                = 0x8,
  DFI_PA_SRC_FROM_FIFO_XBUS           = 0x9,
  DFI_PA_SRC_FROM_FIFO_IMC_P          = 0xA,
  DFI_PA_SRC_FROM_IDCT                = 0xC,
  DFI_PA_SRC_FROM_IDCT_XBUS           = 0xD,
  DFI_PA_SRC_FROM_VBF_IMC_P           = 0xE
}t_dfi_pa_src_id;

/* list of the IDs corresponding to REC sources */ 
typedef enum 
{
  DFI_REC_SRC_FROM_XBUS              = 0,        
  DFI_REC_SRC_FROM_PA                = 2,
  DFI_REC_SRC_FROM_IPA               = 3
}t_dfi_rec_src_id;


/* list of the IDs corresponding to FIFO sources */ 
typedef enum 
{
  DFI_FIFO_SRC_FROM_XBUS              = 0,        
  DFI_FIFO_SRC_FROM_IDCT              = 2,
  DFI_FIFO_SRC_FROM_DCT               = 5,
  DFI_FIFO_SRC_FROM_Q                 = 6,
  DFI_FIFO_SRC_FROM_QTAB              = 8,
  DFI_FIFO_SRC_FROM_VC1IT             = 9,
  DFI_FIFO_SRC_FROM_PA                = 11
}t_dfi_fifo_src_id;

/* list of the IDs corresponding to QTAB sources */ 
typedef enum 
{
  DFI_QTAB_SRC_FROM_XBUS              = 0,        
  DFI_QTAB_SRC_FROM_Q                 = 2,
  DFI_QTAB_SRC_FROM_IQ                = 3
}t_dfi_qtab_src_id;

/* list of the IDs corresponding to VC1IT sources */ 
typedef enum 
{
  DFI_VC1IT_SRC_FROM_XBUS              = 0,        
  DFI_VC1IT_SRC_FROM_SCN               = 2
}t_dfi_vc1it_src_id;

/* list of the IDs corresponding to VBF sources */ 
typedef enum 
{
  DFI_VBF_SRC_FROM_XBUS              = 0,        
  DFI_VBF_SRC_FROM_VC1IT             = 2
}t_dfi_vbf_src_id;

/* list of the IDs corresponding to FIFO2 sources */ 
typedef enum 
{
  DFI_FIFO2_SRC_FROM_XBUS              = 0,        
  DFI_FIFO2_SRC_FROM_RLC               = 2,         
  DFI_FIFO2_SRC_FROM_H264RLC           = 3,        
  DFI_FIFO2_SRC_FROM_H264FITQ          = 4,        
  DFI_FIFO2_SRC_FROM_IQ                = 5,        
  DFI_FIFO2_SRC_FROM_SCN               = 7,        
  DFI_FIFO2_SRC_FROM_IPRD              = 8
}t_dfi_fifo2_src_id;

/* list of the IDs corresponding to VLC sources */ 
typedef enum 
{
  DFI_VLC_SRC_FROM_XBUS              = 0,        
  DFI_VLC_SRC_FROM_FIFO2             = 2,
  DFI_VLC_SRC_FROM_RLC               = 3
}t_dfi_vlc_src_id;



/*******************************************************
 *
 *    DST ID LIST
 *
 *******************************************************/
/* list of the IDs corresponding to BDU destinations */ 
typedef enum 
{
  DFI_BDU_DST_TO_XBUS               = 0x0,        
  DFI_BDU_DST_TO_IQ                 = 0x2,
  DFI_BDU_DST_TO_SCN                = 0x3,
  DFI_BDU_DST_TO_PA                 = 0x4,
  DFI_BDU_DST_TO_IPRD               = 0x5
}t_dfi_bdu_dst_id;

/* list of the IDs corresponding to BPU destinations */ 
typedef enum 
{
  DFI_BPU_DST_TO_XBUS               = 0x0,        
  DFI_BPU_DST_TO_DMA                = 0x2
}t_dfi_bpu_dst_id;

/* list of the IDs corresponding to IMC destinations */ 
typedef enum 
{
  DFI_IMC_DST_TO_XBUS              = 0x0,        
  DFI_IMC_DST_TO_XBUS_PA           = 0x2,
  DFI_IMC_DST_TO_DCT               = 0x4,
  DFI_IMC_DST_TO_DCT_XBUS          = 0x5,
  DFI_IMC_DST_TO_DCT_PA            = 0x6,
  DFI_IMC_DST_TO_H264FITQ          = 0x8,
  DFI_IMC_DST_TO_IPA               = 0x9,
  DFI_IMC_DST_TO_H264FITQ_IPA      = 0xA,
  DFI_IMC_DST_TO_PA                = 0xE
}t_dfi_imc_dst_id;

/* list of the IDs corresponding to IPA destinations */ 
typedef enum 
{
  DFI_IPA_DST_TO_XBUS               = 0x0,        
  DFI_IPA_DST_TO_REC                = 0x2
}t_dfi_ipa_dst_id;

/* list of the IDs corresponding to DCT destinations */ 
typedef enum 
{
  DFI_DCT_DST_TO_XBUS               = 0x0,        
  DFI_DCT_DST_TO_FIFO               = 0x2,        
  DFI_DCT_DST_TO_Q                  = 0x3
}t_dfi_dct_dst_id;

/* list of the IDs corresponding to IDCT destinations */ 
typedef enum 
{
  DFI_IDCT_DST_TO_XBUS              = 0x0,        
  DFI_IDCT_DST_TO_PA                = 0x2,        
  DFI_IDCT_DST_TO_FIFO              = 0x3
}t_dfi_idct_dst_id;

/* list of the IDs corresponding to H264FITQ destinations */ 
typedef enum 
{
  DFI_H264FITQ_DST_TO_XBUS             = 0x0,        
  DFI_H264FITQ_DST_TO_SCN              = 0x2,        
  DFI_H264FITQ_DST_VOID                = 0x3
}t_dfi_h264fitq_dst_id;

/* list of the IDs corresponding to Q destinations */ 
typedef enum 
{
  DFI_Q_DST_TO_XBUS                 = 0x0,        
  DFI_Q_DST_DOUBLE_XBUS_IQ          = 0x2,        
  DFI_Q_DST_TO_SCN                  = 0x3,
  DFI_Q_DST_DOUBLE_SCN_IQ           = 0x4,
  DFI_Q_DST_TO_FIFO                 = 0x6
}t_dfi_q_dst_id;

/* list of the IDs corresponding to IQ destinations */ 
typedef enum 
{
  DFI_IQ_DST_TO_XBUS                = 0x0,        
  DFI_IQ_DST_TO_IPRD                = 0x2,        
  DFI_IQ_DST_TO_SCN                 = 0x3,
  DFI_IQ_DST_TO_IDCT                = 0x4,
  DFI_IQ_DST_TO_FIFO2               = 0x5
}t_dfi_iq_dst_id;

/* list of the IDs corresponding to SCN destinations */ 
typedef enum 
{
  DFI_SCN_DST_TO_XBUS               = 0x0,
  DFI_SCN_DST_TO_RLC                = 0x2,
  DFI_SCN_DST_TO_IDCT               = 0x3,
  DFI_SCN_DST_TO_H264RLC            = 0x4,
  DFI_SCN_DST_TO_H264FIT            = 0x5,
  DFI_SCN_DST_TO_VC1IT              = 0x6,
  DFI_SCN_DST_TO_FIFO2              = 0x7
}t_dfi_scn_dst_id;

/* list of the IDs corresponding to RLC destinations */ 
/* Not programmable */
typedef enum 
{
  DFI_RLC_DST_TO_XBUS              = 0x0,
  DFI_RLC_DST_TO_FIFO2             = 0x2,
  DFI_RLC_DST_TO_VLC               = 0x3
}t_dfi_rlc_dst_id;

/* list of the IDs corresponding to H264RLC destinations */ 
/* Not programmable */
typedef enum 
{
  DFI_H264RLC_DST_TO_XBUS              = 0x0,
  DFI_H264RLC_DST_TO_H264RLC           = 0x2,
  DFI_H264RLC_DST_TO_BPU               = 0x3
}t_dfi_h264rlc_dst_id;

/* list of the IDs corresponding to IPRD destinations */ 
typedef enum 
{
  DFI_IPRD_DST_TO_XBUS              = 0x0,
  DFI_IPRD_DST_TO_IQ                = 0x2,
  DFI_IPRD_DST_TO_SCN               = 0x3,
  DFI_IPRD_DST_TO_FIFO2             = 0x4
}t_dfi_iprd_dst_id;

/* list of the IDs corresponding to PA destinations */ 
typedef enum 
{
  DFI_PA_DST_TO_XBUS                = 0x0,
  DFI_PA_DST_TO_REC                 = 0x2,
  DFI_PA_DST_DOUBLE_REC_XBUS        = 0x3,
  DFI_PA_DST_TO_FIFO                = 0x4,
  DFI_PA_DST_DOUBLE_REC_BPU         = 0x5,
  DFI_PA_DST_VOID                   = 0x6
}t_dfi_pa_dst_id;

/* list of the IDs corresponding to REC destinations */ 
typedef enum 
{
  DFI_REC_DST_TO_XBUS               = 0x0,
  DFI_REC_DST_TO_DMA                = 0x2
}t_dfi_rec_dst_id;

/* list of the IDs corresponding to FIFO destinations */ 
typedef enum 
{
  DFI_FIFO_DST_TO_XBUS              = 0x0,
  DFI_FIFO_DST_TO_PA                = 0x2
}t_dfi_fifo_dst_id;

/* list of the IDs corresponding to QTAB destinations */ 
typedef enum 
{
  DFI_QTAB_DST_TO_XBUS              = 0x0,
  DFI_QTAB_DST_TO_IQ                = 0x2,
  DFI_QTAB_DST_TO_Q                 = 0x3
}t_dfi_qtab_dst_id;

/* list of the IDs corresponding to VC1IT destinations */ 
typedef enum 
{
  DFI_VC1IT_DST_TO_XBUS              = 0x0,
  DFI_VC1IT_DST_TO_VBF               = 0x2
}t_dfi_vc1it_dst_id;

/* list of the IDs corresponding to VBF destinations */ 
typedef enum 
{
  DFI_VBF_DST_TO_XBUS              = 0x0,
  DFI_VBF_DST_TO_PA                = 0x2
}t_dfi_vbf_dst_id;

/* list of the IDs corresponding to FIFO2 destinations */ 
typedef enum 
{
  DFI_FIFO2_DST_TO_XBUS              = 0x0,
  DFI_FIFO2_DST_TO_IPA               = 0x2,
  DFI_FIFO2_DST_TO_SCN               = 0x3,
  DFI_FIFO2_DST_TO_VLC               = 0x6,
  DFI_FIFO2_DST_TO_BPU               = 0x7
}t_dfi_fifo2_dst_id;

/* list of the IDs corresponding to VLC destinations */ 
typedef enum 
{
  DFI_VLC_DST_TO_XBUS              = 0x0,
  DFI_VLC_DST_TO_BPU               = 0x2
}t_dfi_vlc_dst_id;


/*****************************************************************************/
/*
 * Exported functions							       
 */
/*****************************************************************************/


/*****************************************************************************/
/**
 * \brief 	Write a generic DFI register (with XIORDY)
 * \author 	Maurizio Colombo
 * \param 	value   value to be written
 * \param   index   address offset of register
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SET_REG(t_uint16 reg, t_uint16 value)
{
    HW_DFI_WAIT(reg) = value;
}


/*****************************************************************************/
/**
 * \brief 	Read a generic DFI register (with XIORDY)
 * \author 	Maurizio Colombo
 * \param   index   address offset of register
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_GET_REG_WAIT(t_uint16 reg)
{
    return HW_DFI_WAIT(reg);
}

/*****************************************************************************/
/**
 * \brief 	Read a generic DFI register (without XIORDY)
 * \author 	Maurizio Colombo
 * \param   index   address offset of register
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_GET_REG(t_uint16 reg)
{
    return HW_DFI_WAIT(reg);
}

/*****************************************************************************/
/**
 * \brief 	Reset all the DFI registers and FIFOs
 * \author 	Maurizio Colombo
 */
/*****************************************************************************/
#pragma inline
static void DFI_INIT(void)
{
    HW_DFI_WAIT(DFI_REG_SOFT_RESET) = 1;
}
/*****************************************************************************/
/**
 * \brief 	Write DFI DEF register to fix fifo mode (with XIORDY)
 * \author 	Cyril Enault
 * Write 0 in DFI_REG_MACRO_DEF : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SET_REG_DEF_FIFO()
{
    HW_DFI_WAIT(DFI_REG_MACRO_DEF) = 0; /* FIFO */
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DEF register to fix register mode (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_MACRO_DEF : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SET_REG_DEF_REG()
{
    HW_DFI_WAIT(DFI_REG_MACRO_DEF) = 1; /* Register */
}

/*****************************************************************************/
/**
 * \brief 	Set macro configuration (encode or decode) in DFI
 * \author 	Rebecca RICHARD

 * \param   mode  DFI_DECODE_MODE or DFI_ENCODE_MODE 
 */
/*****************************************************************************/
#pragma inline
static void DFI_MACRO_CFG(t_dfi_mode mode)
{
    HW_DFI_WAIT(DFI_REG_MACRO_CFG) = mode;  
}

/*****************************************************************************/
/**
 * \brief 	Macro command to DFI
 * \author 	Maurizio Colombo

 * \param   repeat    number of blocks
 * \param   mb_type   block type
 */
/*****************************************************************************/
#pragma inline
static void DFI_MACRO_CMD(t_dfi_mode mode, t_uint16 repeat, t_dfi_mbtype mbtype)
{
    HW_DFI_WAIT(DFI_FIFO_MACRO_CMD) = mbtype          |
	                                  ((repeat-1)<<7);
}

/*****************************************************************************/
/**
 * \brief 	Macro command to DFI in register mode
 * \author 	Rebecca Richard

 * \param   t_dfi_mbtype  command 
 */
/*****************************************************************************/
#pragma inline
static void DFI_MACRO_CMD_REG(t_dfi_mbtype command)
{
    HW_DFI_WAIT(DFI_REG_MACRO_CMD) = command; 
}

/*****************************************************************************/
/**
 * \brief 	Write a generic DFI UNIT register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param   reg         address offset of register
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_UNIT_SET_REG(t_dfi_unit_id dfi_unit_id, t_uint16 reg, t_uint16 value)
{
    HW_DFI_WAIT(dfi_unit_id+reg) = value;
}


/*****************************************************************************/
/**
 * \brief 	Read a generic DFI UNIT register (with XIORDY)
 * \author 	Cyril Enault
 * \param   dfi_unit_id unit id
 * \param   reg         address offset of register
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_UNIT_GET_REG_WAIT(t_dfi_unit_id dfi_unit_id, t_uint16 reg)
{
    return HW_DFI_WAIT(dfi_unit_id+reg);
}

/*****************************************************************************/
/**
 * \brief 	Read a generic DFI UNIT register (without XIORDY)
 * \author 	Cyril Enault
 * \param   dfi_unit_id unit id
 * \param   reg         address offset of register
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_UNIT_GET_REG(t_dfi_unit_id dfi_unit_id, t_uint16 reg)
{
    return HW_DFI_WAIT(dfi_unit_id+reg);
}

/*****************************************************************************/
/**
 *
 *                      BDU unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI BDU CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_BDU_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BDU_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_BDU_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BDU CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_BDU_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_BDU_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_BDU_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BDU CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BDU_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_BDU_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BDU CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BDU_SET_REG_CMD(t_dfi_bdu_dst_id dst)
{
    HW_DFI_WAIT(DFI_REG_BDU_CMD) = dst;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BDU CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BDU_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_BDU_CMD);
}


/*****************************************************************************/
/**
 * \brief 	Read DFI BDU CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BDU_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_BDU_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BDU CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BDU_SET_FIFO_CMD(t_dfi_bdu_dst_id dst, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_BDU_CMD) = dst |
                                       (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BDU DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BDU_GET_FIFO_SRC_LSB(void)
{
    return (HW_DFI_WAIT(DFI_FIFO_BDU_SRC_LSB));
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BDU DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BDU_GET_FIFO_SRC_MSB(void)
{
    return (HW_DFI_WAIT(DFI_FIFO_BDU_SRC_MSB));
}

/*****************************************************************************/
/**
 *
 *                      BPU unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI BPU CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_BPU_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BPU_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_BPU_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BPU CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_BPU_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_BPU_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_BPU_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BPU CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BPU_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_BPU_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI BPU CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_BPU_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_BPU_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BPU CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BPU_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_BPU_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BPU SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BPU_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_BPU_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI BPU SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_BPU_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_BPU_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 *
 *                      IMC unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI IMC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_IMC_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_IMC_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IMC CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_IMC_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_IMC_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IMC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IMC_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_IMC_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IMC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_REG_CMD(t_dfi_imc_dst_id dst)
{
    HW_DFI_WAIT(DFI_REG_IMC_CMD) = dst ;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IMC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IMC_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_IMC_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IMC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_IMC_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IMC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IMC_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_IMC_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_FIFO_CMD(t_dfi_imc_dst_id dst, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_IMC_CMD) = dst |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IMC SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IMC_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IMC SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IMC_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IMC_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IMC DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IMC_GET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IMC_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IMC DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IMC_GET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IMC_DST_MSB);
}

/*****************************************************************************/
/**
 *
 *                      IPA unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI IPA CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_IPA_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_IPA_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPA CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_IPA_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_IPA_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPA CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPA_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_IPA_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_REG_CMD(t_dfi_ipa_dst_id dst)
{
    HW_DFI_WAIT(DFI_REG_IPA_CMD) = dst;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPA_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_IPA_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_IPA_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPA_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_IPA_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_FIFO_CMD(t_dfi_ipa_dst_id dst, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_IPA_CMD) = dst |
                                        (repeat<<8);
}
/*****************************************************************************/
/**
 * \brief 	Write DFI IMC SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IPA_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPA_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IPA_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPA_GET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IPA_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPA_GET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IPA_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      DCT unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI DCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_DCT_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_DCT_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_DCT_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_DCT_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_DCT_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_DCT_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_REG_CMD(t_dfi_dct_dst_id dst,t_dfi_dct_src_id src)
{
    HW_DFI_WAIT(DFI_REG_DCT_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_DCT_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_DCT_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_DCT_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_DCT_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_DCT_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_FIFO_CMD(t_dfi_dct_dst_id dst, t_dfi_dct_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_DCT_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}
/*****************************************************************************/
/**
 * \brief 	Write DFI DCT SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_DCT_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_DCT_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_DCT_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_DCT_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_DCT_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_DCT_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_DCT_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      IDCT unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI IDCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_IDCT_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_IDCT_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IDCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_IDCT_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_IDCT_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IDCT CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IDCT_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_IDCT_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IDCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_REG_CMD(t_dfi_idct_dst_id dst,t_dfi_idct_src_id src)
{
    HW_DFI_WAIT(DFI_REG_IDCT_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IDCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IDCT_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_IDCT_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IDCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_IDCT_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IDCT CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IDCT_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_IDCT_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_FIFO_CMD(t_dfi_idct_dst_id dst, t_dfi_idct_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_IDCT_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IDCT SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IDCT_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IDCT_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IDCT_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IDCT_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IDCT_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IDCT_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IDCT_DST_MSB);
}

/*****************************************************************************/
/**
 *
 *                      H264FITQ unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI H264FITQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_H264FITQ_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_H264FITQ_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264FITQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_H264FITQ_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_H264FITQ_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264FITQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264FITQ_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_H264FITQ_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264FITQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_REG_CMD(t_dfi_h264fitq_dst_id dst,t_dfi_h264fitq_src_id src)
{
    HW_DFI_WAIT(DFI_REG_H264FITQ_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264FITQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264FITQ_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_H264FITQ_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264FITQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_H264FITQ_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264FITQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264FITQ_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_H264FITQ_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_FIFO_CMD(t_dfi_h264fitq_dst_id dst, t_dfi_h264fitq_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_H264FITQ_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264FITQ SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_H264FITQ_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264FITQ_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_H264FITQ_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264FITQ_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_H264FITQ_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264FITQ_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_H264FITQ_DST_MSB);
}

/*****************************************************************************/
/**
 *
 *                      Q unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI Q CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_Q_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_Q_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI Q CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_Q_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_Q_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI Q CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_Q_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_Q_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI Q CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_REG_CMD(t_dfi_q_dst_id dst,t_dfi_q_src_id src)
{
    HW_DFI_WAIT(DFI_REG_Q_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI Q CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_Q_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_Q_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI Q CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_Q_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI Q CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_Q_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_Q_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_FIFO_CMD(t_dfi_q_dst_id dst, t_dfi_q_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_Q_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI Q SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_Q_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_Q_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_Q_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_Q_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_Q_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_Q_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_Q_DST_MSB);
}

/*****************************************************************************/
/**
 *
 *                      IQ unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI IQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_IQ_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_IQ_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_IQ_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_IQ_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IQ CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IQ_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_IQ_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_REG_CMD(t_dfi_iq_dst_id dst,t_dfi_iq_src_id src)
{
    HW_DFI_WAIT(DFI_REG_IQ_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IQ_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_IQ_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_IQ_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IQ CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IQ_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_IQ_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_FIFO_CMD(t_dfi_iq_dst_id dst, t_dfi_iq_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_IQ_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IQ SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IQ_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IQ_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IQ_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IQ_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IQ_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IQ_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IQ_DST_MSB);
}
/*****************************************************************************/
/**
 * \brief 	write level and position in the input of IQ (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline
static IQ_GET_LEVEL_POS(t_uint16 level, t_uint16 pos,t_uint16 last)
{
  HW_DFI_WAIT(DFI_REG_IQ_STA_LAST) = last;
  HW_DFI_WAIT(DFI_FIFO_IQ_SRC_MSB) = (pos & 0x0030) >> 4;
  HW_DFI_WAIT(DFI_FIFO_IQ_SRC_LSB) = (level & 0x0fff) | ((pos & 0x000f) << 12);
}
/*****************************************************************************/
/**
 *
 *                      SCN unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI SCN CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_SCN_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_SCN_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI SCN CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_SCN_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_SCN_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI SCN CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_SCN_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_SCN_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI SCN CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_REG_CMD(t_dfi_scn_dst_id dst,t_dfi_scn_src_id src)
{
    HW_DFI_WAIT(DFI_REG_SCN_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI SCN CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_SCN_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_SCN_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI SCN CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_SCN_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI SCN CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_SCN_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_SCN_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_FIFO_CMD(t_dfi_scn_dst_id dst, t_dfi_scn_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_SCN_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI SCN SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_SCN_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_SCN_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_SCN_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_SCN_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_SCN_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_SCN_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_SCN_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      RLC unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_RLC_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_RLC_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_RLC_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_RLC_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_RLC_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_RLC_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_REG_CMD(t_dfi_rlc_src_id src)
{
    HW_DFI_WAIT(DFI_REG_RLC_CMD) = (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_RLC_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_RLC_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_RLC_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_RLC_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_RLC_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_FIFO_CMD(t_dfi_rlc_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_RLC_CMD) = (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI RLC SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_RLC_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_RLC_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_RLC_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 *
 *                      H264RLC unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI H264RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_H264RLC_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_H264RLC_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_H264RLC_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_H264RLC_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264RLC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264RLC_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_H264RLC_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_REG_CMD(t_dfi_h264rlc_src_id src)
{
    HW_DFI_WAIT(DFI_REG_H264RLC_CMD) = (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264RLC_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_H264RLC_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_H264RLC_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI H264RLC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264RLC_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_H264RLC_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_FIFO_CMD(t_dfi_h264rlc_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_H264RLC_CMD) = (src<<4) |
                                                (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI H264RLC SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_H264RLC_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_H264RLC_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_H264RLC_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264RLC_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_H264RLC_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_H264RLC_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_H264RLC_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      IPRD unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI IPRD CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_IPRD_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_IPRD_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPRD CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_IPRD_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_IPRD_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPRD CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPRD_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_IPRD_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPRD CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_REG_CMD(t_dfi_iprd_dst_id dst,t_dfi_iprd_src_id src)
{
    HW_DFI_WAIT(DFI_REG_IPRD_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPRD CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPRD_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_IPRD_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPRD CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_IPRD_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI IPRD CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPRD_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_IPRD_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_FIFO_CMD(t_dfi_iprd_dst_id dst, t_dfi_iprd_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_IPRD_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI IPRD SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IPRD_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_IPRD_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_IPRD_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPRD_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IPRD_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_IPRD_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_IPRD_DST_MSB);
}
/*****************************************************************************/
/**
 * \brief 	write level and position in the input of IPRD (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline
static IPRD_GET_LEVEL_POS(t_uint16 level, t_uint16 pos,t_uint16 last)
{
  HW_DFI_WAIT(DFI_REG_IPRD_STA_LAST) = last;
  HW_DFI_WAIT(DFI_FIFO_IPRD_SRC_MSB) = (pos & 0x0030) >> 4;
  HW_DFI_WAIT(DFI_FIFO_IPRD_SRC_LSB) = (level & 0x0fff) | ((pos & 0x000f) << 12);
}
/*****************************************************************************/
/**
 *
 *                      PA unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI PA CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_PA_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_PA_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI PA CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_PA_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_PA_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI PA CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_PA_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_PA_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI PA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_REG_CMD(t_dfi_pa_dst_id dst,t_dfi_pa_src_id src)
{
    HW_DFI_WAIT(DFI_REG_PA_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI PA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_PA_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_PA_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI PA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_PA_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI PA CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_PA_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_PA_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_FIFO_CMD(t_dfi_pa_dst_id dst, t_dfi_pa_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_PA_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI PA SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_PA_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_PA_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_PA_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_PA_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_PA_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_PA_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_PA_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      REC unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI REC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_REC_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_REC_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_REC_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI REC CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_REC_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_REC_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_REC_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI REC CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_REC_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_REC_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI REC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_REC_SET_REG_CMD(t_dfi_pa_src_id src,t_dfi_rec_dst_id dst)
{
    HW_DFI_WAIT(DFI_REG_REC_CMD) = dst |
                                   (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI REC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_REC_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_REC_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI REC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_REC_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_REC_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI REC CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_REC_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_REC_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_REC_SET_FIFO_CMD(t_dfi_rec_dst_id dst, t_dfi_rec_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_REC_CMD) = dst |
	  (src<<4) |
	  (repeat<<8);
}


/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_REC_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_REC_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_REC_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_REC_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      FIFO unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_FIFO_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_FIFO_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_FIFO_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_FIFO_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_REG_CMD(t_dfi_fifo_dst_id dst,t_dfi_fifo_src_id src)
{
    HW_DFI_WAIT(DFI_REG_FIFO_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_FIFO_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_FIFO_CMD(t_dfi_fifo_dst_id dst, t_dfi_fifo_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_FIFO_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_FIFO_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_FIFO_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_FIFO_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_FIFO_DST_MSB);
}


/*****************************************************************************/
/**
 *
 *                      FIFO2 unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO2 CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_FIFO2_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_REG_CFG_FIFO2()
{
    HW_DFI_WAIT(DFI_REG_FIFO2_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO2 CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_FIFO2_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_FIFO2_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO2 CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO2_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO2_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO2 CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_REG_CMD(t_dfi_fifo2_dst_id dst,t_dfi_fifo2_src_id src)
{
    HW_DFI_WAIT(DFI_REG_FIFO2_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO2 CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO2_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO2_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO2 CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_FIFO2_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI FIFO2 CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO2_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_FIFO2_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo2 (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_FIFO2_CMD(t_dfi_fifo2_dst_id dst, t_dfi_fifo2_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_FIFO2_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI FIFO2 SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_FIFO2_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_FIFO2_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo2 (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_FIFO2_SET_FIFO2_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_FIFO2_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO2_SET_FIFO2_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_FIFO2_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo2 (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_FIFO2_SET_FIFO2_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_FIFO2_DST_MSB);
}


/*****************************************************************************/
/**
 *
 *                      QTAB unit
 *
 *
 */
/*****************************************************************************/



/*****************************************************************************/
/**
 * \brief 	Write DFI QTAB CFG register (with XIORDY)
 * \author 	Cyril Enault
 * Write 1 in DFI_REG_QTAB_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_QTAB_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI QTAB CFG register (with XIORDY)
 * \author 	Cyril Enault
 *
 * Write 1 in DFI_REG_QTAB_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_QTAB_DEF) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI QTAB CFG register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value       value to be written
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_QTAB_GET_REG_CFG(void)
{
    return HW_DFI_WAIT(DFI_REG_QTAB_DEF);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI QTAB CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_REG_CMD(t_dfi_qtab_dst_id dst,t_dfi_qtab_src_id src)
{
    HW_DFI_WAIT(DFI_REG_QTAB_CMD) = dst |
                                         (src<<4);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI QTAB CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_QTAB_GET_REG_CMD(void)
{
    return HW_DFI_WAIT(DFI_REG_QTAB_CMD);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI QTAB CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_REG_STA_LAST(void)
{
    HW_DFI_WAIT(DFI_REG_QTAB_STA_LAST) = 1;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI QTAB CMD register (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_QTAB_GET_REG_STA_LAST(void)
{
    return HW_DFI_WAIT(DFI_REG_QTAB_STA_LAST);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI UNIT CMD fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	dfi_unit_id unit id
 * \param 	dst       destination of the unit
 * \param 	repeat    number of current comment iteration
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_FIFO_CMD(t_dfi_qtab_dst_id dst, t_dfi_qtab_src_id src, t_uint16 repeat)
{
    ASSERT(repeat<=16);
    HW_DFI_WAIT(DFI_FIFO_QTAB_CMD) = dst |
                                        (src<<4) |
                                        (repeat<<8);
}

/*****************************************************************************/
/**
 * \brief 	Write DFI QTAB SRC_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	value
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_FIFO_SRC_LSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_QTAB_SRC_LSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI DFI SRC_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 * \param 	valuw
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_QTAB_SET_FIFO_SRC_MSB(t_uint16 value)
{
    HW_DFI_WAIT(DFI_FIFO_QTAB_SRC_MSB) = value;
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_LSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_QTAB_SET_FIFO_DST_LSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_QTAB_DST_LSB);
}

/*****************************************************************************/
/**
 * \brief 	Read DFI DFI DST_MSB fifo (with XIORDY)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DFI_QTAB_SET_FIFO_DST_MSB(void)
{
    return HW_DFI_WAIT(DFI_FIFO_QTAB_DST_MSB);
}
/*****************************************************************************/
/**
 *
 *                      VBF unit
 *
 *
 */
/*****************************************************************************/
/*****************************************************************************/
/**
 * \brief 	Write DFI VBF CFG register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 * Write 1 in DFI_REG_VBF_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_VBF_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_VBF_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI VBF CFG register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 *
 * Write 1 in DFI_REG_VBF_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_VBF_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_VBF_DEF) = 1;
}
/*****************************************************************************/
/**
 * \brief 	Write DFI VBF CMD register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_VBF_SET_REG_CMD(t_dfi_vbf_dst_id dst,t_dfi_vbf_src_id src)
{
    HW_DFI_WAIT(DFI_REG_VBF_CMD) = dst |
                                         (src<<4);
}
/*****************************************************************************/
/**
 *
 *                      VC1IT unit
 *
 *
 */
/*****************************************************************************/
/*****************************************************************************/
/**
 * \brief 	Write DFI VC1IT CFG register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 * Write 1 in DFI_REG_VC1IT_CFG : enable default source and destination defined
 *                              in fifos
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_VC1IT_SET_REG_CFG_FIFO()
{
    HW_DFI_WAIT(DFI_REG_VC1IT_DEF) = 0;
}

/*****************************************************************************/
/**
 * \brief 	Write DFI VC1IT CFG register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 *
 * Write 1 in DFI_REG_VC1IT_CFG : enable default source and destination defined
 *                              in registers
 */
/*****************************************************************************/
#pragma inline
static void DFI_VC1IT_SET_REG_CFG_REG()
{
    HW_DFI_WAIT(DFI_REG_VC1IT_DEF) = 1;
}
/*****************************************************************************/
/**
 * \brief 	Write DFI VC1IT CMD register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_VC1IT_SET_REG_CMD(t_dfi_vc1it_dst_id dst,t_dfi_vc1it_src_id src)
{
    HW_DFI_WAIT(DFI_REG_VC1IT_CMD) = dst |
                                         (src<<4);
}
/*****************************************************************************/
/**
 * \brief 	Read DFI VC1IT CMD register (with XIORDY)
 * \author 	Aroua BEN DARYOUG
 * \param 	dfi_unit_id unit id
 * \param 	value       destination of the unit
 * \param 	value       source of the unit
 *  
 */
/*****************************************************************************/
#pragma inline
static void DFI_VC1IT_SET_REG_STA_LAST(void)
{
   HW_DFI_WAIT(DFI_REG_VC1IT_STA_LAST)=1;
}

#endif /* _DFI_API_H_ */
