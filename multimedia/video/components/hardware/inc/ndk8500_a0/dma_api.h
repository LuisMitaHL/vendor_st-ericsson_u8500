/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _DMA_API_H_
#define _DMA_API_H_


/*
 * Includes							       
 */
#include "t1xhv_types.h" 	         /* Include for MMDSP types */
#include "macros.h"                /* macros, intrinsics      */
#include "t1xhv_hamac_xbus_regs.h" /* All register addresses  */
#include <stwdsp.h>

/*
 * Defines							       
 */
#ifdef T1XHV_TASK_ENCODE_H264
#define FIFO_DMA_CUP_CTX_R 1 
#else
#define FIFO_DMA_CUP_CTX_R 1
#endif

#ifdef _8500_V2_
/* For Cut2 */
#define FIFO_DMA_CUP_CTX_W 1
#else
#define FIFO_DMA_CUP_CTX_W 0
#endif /* _8500_V2_ */


/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get DMA_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_DMA(a) (*(volatile MMIO t_uint16 *)(DMA_BASE+a))
#define HW_DMA_WAIT(a) (*(volatile __XIO t_uint16 *)(DMA_BASE+a))

/* DMA processes mask, used in ENR_L/DIR_L/IMR_L */
#define MTF_R   0x0001U   /* MTF read process */
#define MTF_W   0x0002U   /* MTF write process */
#define CD_R    0x0004U   /* compressed data read process */
#define MECC_R  0x0008U  /* IMC read process */
#define REC_W   0x0010U  /* reconstruction process */
#define SWF_R   0x0020U  /* Search window FIFO read process */
#define SWF_W   0x0040U  /* Search window FIFO write process */
#define CD_W    0x0080U  /* Compressed data write process */
#define DEB_L_R 0x0100U  /* deblocking luma read process */
#define DEB_C_R 0x0200U  /* deblocking chroma read process */
#define PARAM_R 0x0400U  /* deblocking param read process */
#define DEB_L_W 0x0800U  /* deblocking luma write process */
#define DEB_C_W 0x1000U  /* deblocking chroma write process */
#define DMA_VPP_PROCESSES (DEB_L_R|DEB_C_R|PARAM_R|DEB_L_W|DEB_C_W)
#define CUP_CTX_R 0x2000U
#define CUP_CTX_W 0x4000U
#define CUP_DBX_W 0x8000U
#define CUP0_W  CUP_CTX_W  /*  */
#define CUP1_W  CUP_DBX_W  /*  */
#define CUP0_R  CUP_CTX_R  /*  */
/* DMA processes mask, used in ENR_H/DIR_H/IMR_H */
#define IME_CWL_R 0x0001U
#define IME_TPL_R 0x0002U
#define IME_SWL_R 0x0004U
#define IME_MVF_R 0x0008U
#define IME_MVF_W 0x0010U
#define IME_CCM_W 0x0020U
#define IPA_R     0x0040U
#define RECF_R    0x0080U
#define RECF_W    0x0100U

#define IMC_R MECC_R

#define DMA_MAX_SW_FCODE 2


/* the following defines are just name changes in the registers */
#define DMA_SOFT_RST DMA_SOFT_RST_L
#define DMA_ENR DMA_ENR_L
#define DMA_DIR DMA_DIR_L
#define DMA_IMR DMA_IMR_L
#define DMA_BSM DMA_BSM_L
#define DMA_ISR DMA_ISR_L
#define DMA_VCU_ENR DMA_ENR
#define DMA_VCU_DIR DMA_DIR
#define DMA_VCU_IMR DMA_IMR
#define DMA_VCU_BSM DMA_BSM

typedef enum
{
    CD_R_ENABLE  = 0,
    CD_R_FORWARD = 1,
    CD_R_REVERSE = 3
} t_cd_r_cmd;
typedef enum
{
    DMA_SYNC_NONE         = 0x000U,
    DMA_SYNC_VC1_DEC      = 0x400U,
    DMA_SYNC_VC1_DEC_DEB  = 0x800U,
    DMA_SYNC_H264_DEC     = 0x1000U,
    DMA_SYNC_H264_DEC_DEB = 0x2000U,
    DMA_SYNC_H264_ENC     = 0x4000U,
    DMA_SYNC_H264_ENC_DEB = -32768L   /* 0x8000U, avoid warning */

}t_recf_synchro;
typedef enum
{
    DMA_EOR_REC_W     = 0x1U<<0U,
    DMA_EOR_IPA_R     = 0x1U<<1U,
    DMA_EOR_IME_TPL_R = 0x1U<<2U,
    DMA_EOR_DEB_RW    = 0x1U<<3U,
    DMA_EOR_RECF_RW   = 0x1U<<4U,
    DMA_EOR_MTF_W     = 0x1U<<5U

}t_recf_synchro_eor;
typedef struct 
{
  t_uint16 pic_width_in_mbs;
  t_uint16 pic_height_in_mbs;
  t_uint32 curr_frame_l;
  t_uint32 curr_frame_c;
  t_uint32 vpp_parameters_addr;
  t_uint32 vpp_parameters_size;
  t_uint32 vpp_dummy_addr;
  t_uint16 it_mask_reg;
  t_uint16 byte_shift_mode;
} t_vpp_DMA_prog;

typedef enum
{
    DMA_CUP_H264_CTX_SIZE   = 24L*8L,
    DMA_CUP_VC1_CTX_SIZE    =    32L,
    DMA_CUP_H264E_CTX_SIZE  =  7L*8L,
    DMA_CUP_H264_DBX_SIZE   =  2L*8L
} t_dma_cup_size;

/*****************************************************************************/
/** \brief 	Read a register from DMA - 16 bits
 *  \param	addr	Address to read (offset) 
 *  \return Read value
 */
/*****************************************************************************/
#pragma inline
static t_uint16 DMA_GET_REG(t_uint16 addr) 
{
    return HW_DMA_WAIT(addr);
}
#pragma inline
static t_uint16 DMA_GET_REG_WAIT(t_uint16 addr)
{
    return HW_DMA_WAIT(addr);
}
/*****************************************************************************/
/** \brief 	Read a register from DMA - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param	addr_msb	Address to read (offset) MSB 
 *  \return Read value
 */
/*****************************************************************************/
#pragma inline
static t_uint32 DMA_GET_REG_32(t_uint16 addr_lsb,
                               t_uint16 addr_msb)
{
//    t_uint32 tmp =(t_uint32)HW_DMA(addr_lsb);
//    t_uint32 tmp2 =(t_uint32)((t_uint32)HW_DMA(addr_msb) << 16L);
//
//    return (t_uint32)((t_uint32)tmp | (t_uint32)tmp2);
    return (MAKE_LONG(HW_DMA_WAIT(addr_msb),HW_DMA_WAIT(addr_lsb)));
}
/*****************************************************************************/
/** \brief 	Write a register from DMA - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void DMA_SET_REG(t_uint16 addr, t_uint16 value)
{
    HW_DMA_WAIT(addr) = value;
}
#pragma inline
static void DMA_SET_REG_WAIT(t_uint16 addr, t_uint16 value)
{
    HW_DMA_WAIT(addr) = value;
}
/*****************************************************************************/
/** \brief 	Write a register from DMA - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param	addr_msb	Address to read (offset) MSB 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void DMA_SET_REG_32(t_uint16 addr_lsb, 
                           t_uint16 addr_msb, 
                           t_uint32 value)
{
    HW_DMA_WAIT(addr_lsb) = LS_SHORT(value);
    HW_DMA_WAIT(addr_msb) = MS_SHORT(value); 
}
#pragma inline
static void DMA_SET_REG_32_WAIT(t_uint16 addr_lsb, 
                                t_uint16 addr_msb, 
                                t_uint32 value)
{
    HW_DMA_WAIT(addr_lsb) = LS_SHORT(value);
    HW_DMA_WAIT(addr_msb) = MS_SHORT(value); 
}

/****************************************************************************/
/**
 * \brief 	program the dma with the reconstructed adresses for luma and chroma
 * \author 	jean-marc volle
 * \param 	luma_start_add: start addr of the luma part of the reconstructed frame
 * \param 	chroma_start_add:start addr of the chroma part of the reconstructed frame 
 * \warning addresses are respectively aligned on luma macroblocks or chroma 
 *          macroblocks
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_SET_RECONSTRUCTION_BUFFER_START_ADDR( t_uint32 luma_start_add,
                                                      t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REC_DLS_L,DMA_REC_DLS_H,luma_start_add);
    
    DMA_SET_REG_32(DMA_REC_DCS_L,DMA_REC_DCS_H,chroma_start_add);
}
/****************************************************************************/
/**
 * \brief 	program the dma with the address of reconstructed local buffer
 * \author 	Aroua BEN DARYOUG
 * \param 	luma_start_add: start addr of the luma part of the reconstructed frame
 * \param 	chroma_start_add:start addr of the chroma part of the reconstructed frame 
 * \warning addresses are respectively aligned on luma macroblocks or chroma 
 *          macroblocks
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_SET_LOCAL_RECONSTRUCTION_BUFFER_START_ADDR( t_uint32 luma_start_add,
                                                            t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_RECF_DLS_L,DMA_RECF_DLS_H,luma_start_add);
    
    DMA_SET_REG_32(DMA_RECF_DCS_L,DMA_RECF_DCS_H,chroma_start_add);
}


/****************************************************************************/
/**
 * \brief 	program the number of macro block to be reconstructed in the DMA
 * \author 	Maurizio Colombo
 * \param 	nb_of_mb_in_frame: number of macro block to be written by the
 *              DMA in reconstruction 
 * \warning     nb_of_mb_in_frame < 1024             
 *  
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_SET_REC_SIZE(t_uint16 width, t_uint16 height)
{
    DMA_SET_REG(DMA_REC_DFW,width);
    DMA_SET_REG(DMA_REC_DFH,height);
}


/****************************************************************************/
/**
 * \brief 	read the current MB counter in the DMA (from DMA_REC_DMN to 0)
 * \author 	jean-marc volle
 * \return 	 DMA_REC_DMC: count down  of the MB stored by the DMA
 *  
 * 
 **/
/****************************************************************************/

#pragma inline
static t_uint16 DMA_GET_REC_MB_COUNTER()
{
  return HW_DMA_WAIT(DMA_REC_DMC);
}


/****************************************************************************/
/**
 * \brief 	 start the reconstruction write process in the DMA
 * \author 	jean-marc volle
 *  
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_RECONSTRUCTION_WRITE_PROCESS_ENABLE()
{
    DMA_SET_REG(DMA_ENR_L,REC_W);
}
/****************************************************************************/
/**
 * \brief 	start the reconstruction in local buffer write process in the DMA
 * \author 	BEN DARYOUG Aroua
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_LOCAL_REC_BUFFER_ENABLE()
{
    DMA_SET_REG(DMA_ENR_H,RECF_W|RECF_R);
}
/****************************************************************************/
/**
 * \brief 	disable the reconstruction in local buffer 
 * \author 	BEN DARYOUG Aroua
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_LOCAL_REC_BUFFER_DISABLE()
{
    DMA_SET_REG(DMA_DIR_H,RECF_W|RECF_R);
}

#pragma inline
static void DMA_RECONSTRUCTION_WRITE_PROCESS_DISABLE()
{
    DMA_SET_REG(DMA_DIR_L,REC_W);
}
/****************************************************************************/
/**
 * \brief 	enable synchro between processes of reconstructed local buffer
 * \author 	BEN DARYOUG Aroua
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_SYNCHRONISATION_ENABLE(t_recf_synchro synchro)
{
    DMA_SET_REG(DMA_SYNCHRO_ENR,synchro);
}
/****************************************************************************/
/**
 * \brief 	DMA Synchro : wait for all the processes indicated with synchro_bits
 *                        to be finished (for the line)
 * \author 	ENAULT Cyril
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_SYNCHRO_WAIT_PROCESSES(t_recf_synchro_eor synchro_bits)
{
    while( (DMA_GET_REG(DMA_STA_EOR)&synchro_bits) !=synchro_bits)
    { ; }
}

/****************************************************************************/
/**
 * \brief 	say it is possible to pass to next line of MBS
 * \author 	BEN DARYOUG Aroua
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_RLCB_SYNCHRONISATION_ENABLE()
{
    DMA_SET_REG(DMA_RLCB_SYNC,1);
}
/****************************************************************************/
/**
 * \brief 	 start the compressed data read process (bitstream)
 * \author 	jean-marc volle
 *  
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_COMPRESSED_DATA_READ_PROCESS_ENABLE()
{
    t_uint32 tmp;
    DMA_SET_REG_32(DMA_ENR_L,DMA_ENR_H,CD_R);
    /* enable word64 swapping */
    tmp=DMA_GET_REG_32(DMA_BSM_L,DMA_BSM_H);
    tmp|=CD_R;
    DMA_SET_REG_32(DMA_BSM_L,DMA_BSM_H,tmp);
}


/****************************************************************************/
/**
 * \brief 	Get the video codec unit enabled processes.
 * \author 	jean-marc volle
 *  
 * 
 **/
/****************************************************************************/

#pragma inline
static t_uint32 DMA_GET_VCU_ENABLED_PROCESS()
{
    return MAKE_LONG(HW_DMA_WAIT(DMA_ENR_H),HW_DMA_WAIT(DMA_ENR_L));
}
/** @{ \name DMA processes and ITs programming/reading 
    DMA_PPP_ENR,DMA_PPP_IMR are ORed t_ppp_process values
 **/


#pragma inline
static void DMA_PROCESS_ENABLE_L(t_uint16 processes_to_enable)
{
    HW_DMA_WAIT(DMA_ENR_L) = processes_to_enable;
}
#pragma inline
static void DMA_PROCESS_ENABLE_H(t_uint16 processes_to_enable)
{
    HW_DMA_WAIT(DMA_ENR_H) = processes_to_enable;
}


#pragma inline
static void DMA_PPP_PROCESSES_ENABLE(t_uint16 processes_to_enable)
{
    HW_DMA_WAIT(DMA_ENR_L) = processes_to_enable;
}

#pragma inline
static void DMA_PPP_ITS_ENABLE_L(t_uint16 its_to_enable)
{
    HW_DMA_WAIT(DMA_IMR_L) = its_to_enable;
}

#pragma inline
static void DMA_PPP_ITS_ENABLE_H(t_uint16 its_to_enable)
{
    HW_DMA_WAIT(DMA_IMR_H) = its_to_enable;
}

#pragma inline
static void DMA_PPP_ITS_ENABLE(t_uint16 its_to_enable)
{
    HW_DMA_WAIT(DMA_IMR_L) = its_to_enable;
}


#pragma inline
static t_uint16 DMA_GET_PPP_ENABLED_IT_L()
{
    return HW_DMA_WAIT(DMA_IMR_L);
}

#pragma inline
static t_uint16 DMA_GET_PPP_ENABLED_IT_H()
{
    return HW_DMA_WAIT(DMA_IMR_H);
}

#pragma inline
static t_uint16 DMA_GET_PPP_ENABLED_IT()
{
    return HW_DMA_WAIT(DMA_IMR_L);
}
/** @}*/
/****************************************************************************/
/*
 * \brief   program the dma for original buffer of motion estimation with 
 *          source frame width and source frame height
 * \author  rebecca richard
 * \param   frame_width : IMC source frame width
 * \param   frame_height : IMC source frame height
 * \warning size must be MB aligned 
 *
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_ME_ORIG_SOURCE_SIZE(t_uint16 frame_width,
                                        t_uint16 frame_height)
{
    /* DMA_IMC_ORG_SFW and DMA_IMC_ORG_SFH < 4096 and 16 pixels aligned */
    ASSERT( (frame_height & 0xE00F) == 0 );
    ASSERT( (frame_width & 0xE00F) == 0 );
    
    DMA_SET_REG(DMA_IMC_ORG_SFW, frame_width);
    DMA_SET_REG(DMA_IMC_ORG_SFH, frame_height);
}
/****************************************************************************/
/*
 * \brief   program the dma for reference buffer of motion estimation with 
 *          source frame width and source frame height
 * \author  rebecca richard
 * \param   frame_width : IMC source frame width
 * \param   frame_height : IMC source frame height
 * \warning size must be MB aligned 
 *
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_ME_REF_SOURCE_SIZE(t_uint16 frame_width,
                                       t_uint16 frame_height)
{
    /* DMA_IMC_REF_SFW and DMA_IMC_REF_SFH < 4096 and 16 pixels aligned */
    ASSERT( (frame_height & 0xE00F) == 0 );
    ASSERT( (frame_width & 0xE00F) == 0 );
    
    DMA_SET_REG(DMA_IMC_REF_SFW, frame_width);
    DMA_SET_REG(DMA_IMC_REF_SFH, frame_height);
}
/****************************************************************************/
/*
 * \brief   program the dma with original refence buffer addresses for luma and
 *          chroma
 * \author  rebecca richard
 * \param   luma_start_add : start addr of the luma part of the original 
 *                           reference buffer
 * \param   chroma_start_add : start addr of the chroma part of the original
 *                           reference buffer  
 * \warning addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_ORIG_BUFFER_START_ADDR(t_uint32 luma_start_add,
                                               t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REF0_FLS_L,DMA_REF0_FLS_H,luma_start_add);
    DMA_SET_REG_32(DMA_REF0_FCS_L,DMA_REF0_FCS_H,chroma_start_add);
  
}
/****************************************************************************/
/*
 * \brief   program the dma with reference 1 buffer addresses for luma and
 *          chroma
 * \author  rebecca richard
 * \param   luma_start_add : start addr of the luma part of the reference 1 buffer
 * \param   chroma_start_add : start addr of the chroma part of the reference 1 buffer
 * \warning addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_REF_1_BUFFER_START_ADDR(t_uint32 luma_start_add,
                                            t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REF1_FLS_L,DMA_REF1_FLS_H,luma_start_add);
    DMA_SET_REG_32(DMA_REF1_FCS_L,DMA_REF1_FCS_H,chroma_start_add);
  
}
/****************************************************************************/
/*
 * \brief   program the dma with reference 2 buffer addresses for luma and
 *          chroma
 * \author  rebecca richard
 * \param   luma_start_add : start addr of the luma part of the reference 2 buffer
 * \param   chroma_start_add : start addr of the chroma part of the reference 2 buffer
 * \warning addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_REF_2_BUFFER_START_ADDR(t_uint32 luma_start_add,
                                            t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REF2_FLS_L,DMA_REF2_FLS_H,luma_start_add);
    DMA_SET_REG_32(DMA_REF2_FCS_L,DMA_REF2_FCS_H,chroma_start_add);
  
}
/****************************************************************************/
/*
 * \brief   program the dma with reference 3 buffer addresses for luma and
 *          chroma
 * \author  rebecca richard
 * \param   luma_start_add : start addr of the luma part of the reference 3 buffer
 * \param   chroma_start_add : start addr of the chroma part of the reference 3 buffer
 * \warning addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_REF_3_BUFFER_START_ADDR(t_uint32 luma_start_add,
                                            t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REF3_FLS_L,DMA_REF3_FLS_H,luma_start_add);
    DMA_SET_REG_32(DMA_REF3_FCS_L,DMA_REF3_FCS_H,chroma_start_add);
  
}
/****************************************************************************/
/*
 * \brief   program the dma with reference 4 buffer addresses for luma and
 *          chroma
 * \author  rebecca richard
 * \param   luma_start_add : start addr of the luma part of the reference 4 buffer
 * \param   chroma_start_add : start addr of the chroma part of the reference 4 buffer
 * \warning addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_REF_4_BUFFER_START_ADDR(t_uint32 luma_start_add,
                                            t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_REF4_FLS_L,DMA_REF4_FLS_H,luma_start_add);
    DMA_SET_REG_32(DMA_REF4_FCS_L,DMA_REF4_FCS_H,chroma_start_add);
  
}



/****************************************************************************/
/*
 * \brief   program the DMA with MTF READ parameters
 * \autor   cyril enault
 * \param   start_address : MTF read buffer start address
 * \param   inc : MTF read increment register 
 * \param   length : MTF Read transfer length
 * \warning address must be word aligned
 * \warnign length must be multiple of 4
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_MTF_READ_PARAM(t_uint32 start_address,
                                   t_uint16 incr,
                                   t_uint16 length)
{
    /* addresses is word aligned */
    ASSERT((start_address & 0x3) == 0);
    /* length is multiple of 4 32 bits words */
    ASSERT((length & 0x3) == 0);

    DMA_SET_REG_32(DMA_MTF_R_BSA_L,DMA_MTF_R_BSA_H,start_address);
    
    DMA_SET_REG(DMA_MTF_R_INCR,(incr<<3));
    DMA_SET_REG(DMA_MTF_R_TFL,(length<<3));
}

/****************************************************************************/
/*
 * \brief   program the DMA with MTF WRITE parameters
 * \autor   cyril enault
 * \param   start_address : MTF write buffer start address
 * \param   incr : MTF read increment register 
 * \param   length : MTF write transfer length
 * \warning address must be word aligned
 * \warnign length must be multiple of 4
 */
/****************************************************************************/
#pragma inline
static void DMA_SET_MTF_WRITE_PARAM(t_uint32 start_address,
                                    t_uint16 incr,
                                    t_uint16 length)
{
    /* addresses is word aligned */
    ASSERT((start_address & 0x3) == 0);
    /* length is multiple of 4 32 bits words */
    ASSERT((length & 0x3) == 0);

    DMA_SET_REG_32(DMA_MTF_W_BSA_L,DMA_MTF_W_BSA_H,start_address);
    
    DMA_SET_REG(DMA_MTF_W_INCR,(incr<<3));
    DMA_SET_REG(DMA_MTF_W_TFL,(length<<3));
}
/****************************************************************************/
/*
 * \brief   program the DMA for encoding bitsream
 * \autor   Maurizio Colombo
 * \param   start_addr : encode bistream buffer start address
 * \param   end_addr : encode bitstream buffer end address
 */
/****************************************************************************/
#pragma inline
static void DMA_COMPRESSED_DATA_WRITE_INIT(t_uint32 start_addr,
                                           t_uint32 end_addr)
{
    /* addresses are 4 word aligned */
    ASSERT( (start_addr      & 0x000F) == 0 );
    ASSERT( (end_addr        & 0x000F) == 0 );
    
    DMA_SET_REG_32(DMA_CDW_BWS_L,DMA_CDW_BWS_H,start_addr);
    DMA_SET_REG_32(DMA_CDW_BWE_L,DMA_CDW_BWE_H,end_addr);    
}
/****************************************************************************/
/*
 * \brief   program the DMA for decoding bitsream
 * \autor   Maurizio Colombo
 * \param   start_addr : decode bistream buffer start address
 * \param   end_addr : decode bitstream buffer end address
 */
/****************************************************************************/
#pragma inline
static void DMA_COMPRESSED_DATA_READ_INIT(t_uint32 start_addr,
                                          t_uint32 end_addr)
{
    /* addresses are 4 word aligned */
    ASSERT( (start_addr        & 0x000F) == 0 );
    ASSERT( (end_addr          & 0x000F) == 0 );
    
    DMA_SET_REG_32(DMA_CDR_BWS_L,DMA_CDR_BWS_H,start_addr);
    DMA_SET_REG_32(DMA_CDR_BWE_L,DMA_CDR_BWE_H,end_addr);
    
}



/****************************************************************************/
/*
 * \brief   program the DMA addresses for VPP in
 * \autor   Maurizio Colombo
 * \param   source_luma
 * \param   source_chroma
 */
/****************************************************************************/
#pragma inline
static void DMA_VPP_SET_SOURCE_FRAME_ADDR(t_uint32 source_luma,
                                          t_uint32 source_chroma,
										  t_uint32 source_chroma2)
{    
    DMA_SET_REG_32(DMA_DEB_SLS_L,DMA_DEB_SLS_H,source_luma);
    DMA_SET_REG_32(DMA_DEB_SCS_L,DMA_DEB_SCS_H,source_chroma);
    DMA_SET_REG_32(DMA_DEB_SCS2_L,DMA_DEB_SCS2_H,source_chroma2);    
}


/****************************************************************************/
/*
 * \brief   program the DMA addresses for VPP out
 * \autor   Maurizio Colombo
 * \param   dest_luma
 * \param   dest_chroma
 */
/****************************************************************************/
#pragma inline
static void DMA_VPP_SET_DEST_FRAME_ADDR(t_uint32 dest_luma,
										t_uint32 dest_chroma,
										t_uint32 dest_chroma2)
{    
    DMA_SET_REG_32(DMA_DEB_DLS_L,DMA_DEB_DLS_H,dest_luma);
    DMA_SET_REG_32(DMA_DEB_DCS_L,DMA_DEB_DCS_H,dest_chroma);
    DMA_SET_REG_32(DMA_DEB_DCS2_L,DMA_DEB_DCS2_H,dest_chroma2);    
}

/****************************************************************************/
/*
 * \brief   program the DMA registers for frame size (both in and out)
 * \autor   Maurizio Colombo
 * \param   frame width/height
 */
/****************************************************************************/
#pragma inline
static void DMA_VPP_SET_FRAME_SIZE(t_uint16 width,
								   t_uint16 height)
{    
  DMA_SET_REG(DMA_DEB_SFW,width);
  DMA_SET_REG(DMA_DEB_SFH,height);
  DMA_SET_REG(DMA_DEB_DFW,width);
  DMA_SET_REG(DMA_DEB_DFH,height);
}


/****************************************************************************/
/*
 * \brief   program the DMA deblocking param buffer address
 * \autor   Maurizio Colombo
 * \param   deblocking buffer address
 */
/****************************************************************************/
#pragma inline
static void DMA_VPP_SET_DEBLOCKING_PARAM_ADDR(t_uint32 deb_addr, t_uint32 deb_size)
{    
    while ((DMA_GET_REG(DMA_ENR) & PARAM_R) != 0)
    {;}
    DMA_SET_REG_32(DMA_DEB_PS_L,DMA_DEB_PS_H,deb_addr);
    DMA_SET_REG_32(DMA_DEB_PE_L,DMA_DEB_PE_H,deb_addr+deb_size);
    DMA_SET_REG(DMA_ENR_L,PARAM_R);
}

                                          
/*****************************************************************************/
/**
 * \brief 	Stop dma process and wait they are finished
 * \author 	Loic Habrial
 * \param   dma_dir DMA dir register for process abort
 * \param   dma_enr DMA enr register, process curently running
 * \param   dma_stop DMA process to stop
 * \param   dma_prog DMA process to wait end
 *  
 * Write DMA abort register to stop DMA processes and wait process
 * 
 */
/*****************************************************************************/
void dma_stop_wait_process(t_uint16 dma_dir_l,
                           t_uint16 dma_dir_h,
                           t_uint16 dma_enr_l,
                           t_uint16 dma_enr_h,
                           t_uint32 dma_stop,
                           t_uint32 dma_prog);


#pragma inline
static void DMA_WAIT_END_REC_W_PROCESS(void)
{
    while ((DMA_GET_REG(DMA_ENR_L)&REC_W)!=0)
      {;}    
}
#pragma inline
static void DMA_WAIT_END_DEB_PROCESSES(void)
{
    while ((DMA_GET_REG(DMA_ENR_L)&(DMA_VPP_PROCESSES)) != 0)
        {;}
}
#pragma inline
static void DMA_ENABLE_DEB_PROCESSES(void)
{
    DMA_SET_REG(DMA_ENR_L,DMA_VPP_PROCESSES);
}
#pragma inline
static void DMA_DISABLE_DEB_PROCESSES(void)
{
    DMA_SET_REG(DMA_DIR_L,DMA_VPP_PROCESSES);
}
/****************************************************************************/
/**
 * \brief 	program addresses and size for h4d process
 * \param   t_h4d_DMA_prog structure with required parameters    
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_PROGRAM_PARAM_VPP(t_vpp_DMA_prog *struct_param)
{
  
  DMA_SET_REG_32(DMA_DEB_PS_L,DMA_DEB_PS_H,struct_param->vpp_parameters_addr);
  DMA_SET_REG_32(DMA_DEB_PE_L,DMA_DEB_PE_H,struct_param->vpp_parameters_addr+struct_param->vpp_parameters_size);
  /* added for dummy */
  DMA_SET_REG_32(DMA_DEB_DUMMYS_L,DMA_DEB_DUMMYS_H,struct_param->vpp_dummy_addr);
  DMA_SET_REG_32(DMA_DEB_DUMMYE_L,DMA_DEB_DUMMYE_H,struct_param->vpp_dummy_addr+4*8);


  /* source frame*/
  DMA_SET_REG_32(DMA_DEB_SLS_L,DMA_DEB_SLS_H,struct_param->curr_frame_l);
  DMA_SET_REG_32(DMA_DEB_SCS_L,DMA_DEB_SCS_H,struct_param->curr_frame_c);
  /* source frame*/
  DMA_SET_REG_32(DMA_DEB_DLS_L,DMA_DEB_DLS_H,struct_param->curr_frame_l);
  DMA_SET_REG_32(DMA_DEB_DCS_L,DMA_DEB_DCS_H,struct_param->curr_frame_c);
  
  /* frame sizes */
  DMA_SET_REG(DMA_DEB_SFW,(struct_param->pic_width_in_mbs)<<4);
  DMA_SET_REG(DMA_DEB_SFH,(struct_param->pic_height_in_mbs)<<4);
  DMA_SET_REG(DMA_DEB_DFW,(struct_param->pic_width_in_mbs)<<4);
  DMA_SET_REG(DMA_DEB_DFH,(struct_param->pic_height_in_mbs)<<4);
 
  /* interupt mask */
  DMA_SET_REG(DMA_IMR_L,struct_param->it_mask_reg|(DMA_GET_REG(DMA_IMR_L)&(~DMA_VPP_PROCESSES)));
  DMA_SET_REG(DMA_BSM_L,struct_param->byte_shift_mode);
  
}
/****************************************************************************/
/**
 * \brief 	dma soft reset for processes proc.
 * \param   proc processes to be soft reseted    
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_SOFT_RESET(t_uint32 proc)
{
    DMA_SET_REG_32_WAIT(DMA_SOFT_RST_L,DMA_SOFT_RST_H,proc);
}  

/****************************************************************************/
/**
 * \brief 	program addresses and start process for DMA_CUP_CTX_R process
 * \param   addr_start start address
 * \param   size transfert size in bytes
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_PROGRAM_CUP_CTX_R(t_uint32 addr_start, t_uint32 size)
{
#if FIFO_DMA_CUP_CTX_R == 0
    while(DMA_GET_REG(DMA_VCU_ENR)&CUP_CTX_R) // Wait END of CUP_CTX_R
    {;}
#endif
    /* start address */
    DMA_SET_REG_32(DMA_CUP_CTX_R_SA_L,DMA_CUP_CTX_R_SA_H,addr_start);
    /* end address */
    DMA_SET_REG_32(DMA_CUP_CTX_R_EA_L,DMA_CUP_CTX_R_EA_H,addr_start+size);
    /* start process */
#if FIFO_DMA_CUP_CTX_R == 0
    DMA_SET_REG(DMA_VCU_ENR,CUP_CTX_R);
#endif

}

/****************************************************************************/
/**
 * \brief 	Start process for DMA_CUP_CTX_R process
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void START_DMA_CUP_CTX_R_FRAME()
{
#if FIFO_DMA_CUP_CTX_R == 0
#else
    DMA_SET_REG(DMA_VCU_ENR,CUP_CTX_R);
#endif
}/****************************************************************************/
/**
 * \brief 	program addresses and start process for DMA_CUP_CTX_W process
 * \param   addr_start start address
 * \param   size transfert size in bytes
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_PROGRAM_CUP_CTX_W(t_uint32 addr_start, t_uint32 size)
{
#if FIFO_DMA_CUP_CTX_W == 0
    while(DMA_GET_REG(DMA_VCU_ENR)&CUP_CTX_W) // Wait END of CUP_CTX_W
    {;}
#endif
    /* start address */
    DMA_SET_REG_32(DMA_CUP_CTX_W_SA_L,DMA_CUP_CTX_W_SA_H,addr_start);
    /* end address */
    DMA_SET_REG_32(DMA_CUP_CTX_W_EA_L,DMA_CUP_CTX_W_EA_H,addr_start+size);
#if FIFO_DMA_CUP_CTX_W == 0
    /* start process */
    DMA_SET_REG(DMA_VCU_ENR,CUP0_W);
#endif
}
/****************************************************************************/
/**
 * \brief 	Start process for DMA_CUP_CTX_W process
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void START_DMA_CUP_CTX_W_FRAME()
{
#if FIFO_DMA_CUP_CTX_W == 0
#else
    DMA_SET_REG(DMA_VCU_ENR,CUP_CTX_W);
#endif
}
/****************************************************************************/
/**
 * \brief 	program addresses and start process for DMA_CUP_DBX_W process
 * \param   addr_start start address
 * \param   size transfert size in bytes
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void DMA_PROGRAM_CUP_DBX_W(t_uint32 addr_start, t_uint32 size)
{
    while ((DMA_GET_REG(DMA_ENR) & CUP_DBX_W) != 0)
    {;}
    /* start address */
    DMA_SET_REG_32(DMA_CUP_DBP_W_SA_L,DMA_CUP_DBP_W_SA_H,addr_start);
    /* end address */
    DMA_SET_REG_32(DMA_CUP_DBP_W_EA_L,DMA_CUP_DBP_W_EA_H,addr_start+size);
    /* start process */
    DMA_SET_REG(DMA_VCU_ENR,CUP_DBX_W);

}
/****************************************************************************/
/**
 * \brief 	program the dma with the reconstructed frame adresses for luma and chroma
 * \author 	cyril enault
 * \param 	luma_start_add: start addr of the luma part of the reconstructed local buffer
 * \param 	chroma_start_add:start addr of the chroma part of the reconstructed buffer 
 * \warning addresses are respectively aligned on luma macroblocks or chroma 
 *          macroblocks
 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_SET_REC_LOCAL_BUFFER_START_ADDR( t_uint32 luma_start_add,
                                                      t_uint32 chroma_start_add)
{
    /* addresses are MB aligned ie 256(128) bytes aligned for luma(chroma)*/
    ASSERT((luma_start_add & 0xFF) == 0);
    ASSERT((chroma_start_add & 0x7F) == 0);

    DMA_SET_REG_32(DMA_RECF_DLS_L,DMA_RECF_DLS_H,luma_start_add);
    
    DMA_SET_REG_32(DMA_RECF_DCS_L,DMA_RECF_DCS_H,chroma_start_add);
}
/****************************************************************************/
/**
 * \brief 	Get Reconstructed MB Coordinate X
 * \author 	Aroua BEN DARYOUG

 * 
 **/
/****************************************************************************/

#pragma inline
static t_uint16 DMA_REC_GET_COORD_X()
{
    return (DMA_GET_REG(DMA_REC_PCX_STA));
}
/****************************************************************************/
/**
 * \brief 	Get Reconstructed MB Coordinate Y
 * \author 	Aroua BEN DARYOUG

 * 
 **/
/****************************************************************************/

#pragma inline
static t_uint16 DMA_REC_GET_COORD_Y()
{
    return (DMA_GET_REG(DMA_REC_PCY_STA));
}
/****************************************************************************/
/**
 * \brief 	Set Reconstructed MB Coordinate X
 * \author 	Aroua BEN DARYOUG

 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_REC_SET_COORD_X(t_uint16 mb_x)
{
    DMA_SET_REG(DMA_REC_XIF_PCX, (mb_x <<4));
}
/****************************************************************************/
/**
 * \brief 	Set Reconstructed MB Coordinate Y
 * \author 	Aroua BEN DARYOUG

 * 
 **/
/****************************************************************************/

#pragma inline
static void DMA_REC_SET_COORD_Y(t_uint16 mb_y)
{
    DMA_SET_REG(DMA_REC_XIF_PCY, (mb_y<<4));
}
#endif /* _DMA_API_H_ */

