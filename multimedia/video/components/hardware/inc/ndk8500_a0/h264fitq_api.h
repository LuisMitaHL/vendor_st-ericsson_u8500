/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _T1XHV_H264FITQ_H_
#define _T1XHV_H264FITQ_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include <stwdsp.h>             /* for mmdsp intrinsics */
#include <assert.h>             /* for assertion function in debug mode */
#include "t1xhv_types.h"        /* for basic types definitions */
#include "t1xhv_hamac_xbus_regs.h"
#include "macros.h"
#include "mmdsp_api.h"
/*------------------------------------------------------------------------
 * Defines			       
 *----------------------------------------------------------------------*/

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get H264FITQ_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_H264FITQ(a) (*(volatile MMIO t_uint16 *)(H264FITQ_BASE+a))
#define HW_H264FITQ_WAIT(a) (*(volatile __XIO t_uint16 *)(H264FITQ_BASE+a))


/*------------------------------------------------------------------------
 * Types							       
 *----------------------------------------------------------------------*/

/** @{ \name H264FITQ_REG_CFG bitfield values
 */

/** \brief Encode/Decode mode */
typedef enum
{
     H264FITQ_CFG_DECODE      =0, /* \brief Decode mode. the IP is configured to decode in_iscan coefficient to produce out_rsd data for pixel reconstruction by the post-adder. In this mode, in_rsd and out_scan interfaces are not used */
     H264FITQ_CFG_ENCODE      =1  /* \brief Encode mode. The IP is configured to encode in_rsd pixel data from prediction to produce out_scan coefficient for forward-scan. out_rsd data for pixel reconstruction by the post-adder are also generated. In this mode, in_iscan interface is not used. */
} t_h264fitq_mode;

/** \brief Scaling list present flag */
typedef enum
{
     H264FITQ_CFG_SCALING_LIST_OFF      =0, /* \brief No scaling list used in inverse quantization. Default scaling factor 16 is used */
     H264FITQ_CFG_SCALING_LIST_ON       =1  /* \brief Used scaling factor provided in SLM memory. */
} t_h264fitq_slpf;

/** \brief Intra slice flag */
typedef enum
{
     H264FITQ_CFG_P_OR_B_SLICE         =0, /* \brief May contain intra or inter MB */
     H264FITQ_CFG_I_SLICE              =1  /* \brief INtra MB */
} t_h264fitq_islice;

/** \brief Symbol mode encoder flag */
typedef enum
{
     H264FITQ_CFG_CAVLC              =0, /* \brief May contain intra or inter MB */
     H264FITQ_CFG_CABAC              =1  /* \brief INtra MB */
} t_h264fitq_sme;

/**  @} end of H264FITQ_REF_CFG bitfield values*/


/** @{ \name H264FITQ_FIFO_CMD bitfield values
 */

/** \brief Intra/Inter MB flag */
typedef enum
{
     H264FITQ_CMD_INTER              =0, /* \brief Inter MB */
     H264FITQ_CMD_INTRA              =1  /* \brief Intra MB */
} t_h264fitq_mb_mode;


/** \brief I16x16 flag */
typedef enum
{
     H264FITQ_CMD_NOT_I16x16         =0, /* \brief MB is NOT an intra 16x16 */
     H264FITQ_CMD_I16x16             =1  /* \brief MB is an intra 16x16 */
} t_h264fitq_mb_type;


/** \brief Luma transform size 8x8 flag */
typedef enum
{
     H264FITQ_CMD_4x4T              =0, /* \brief Use standard 4x4 transform */
     H264FITQ_CMD_8x8T              =1  /* \brief Use 8x8 transform */
} t_h264fitq_transform;

/**  @} end of H264FITQ_FIFO_CMD bitfield values*/

/*------------------------------------------------------------------------
 * Locals functions							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Exported functions							       
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief 	h264fitq: reset(soft)
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/
#pragma inline 
static void H264FITQ_INIT()
{
    HW_H264FITQ(H264FITQ_REG_RST) = 0x1;
    MMDSP_NOP();
}

/*****************************************************************************/
/**
 * \brief       get status of H264FITQ block 
 * \author      Rebecca RICHARD
 * \param
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 HW_H264FITQ_GET_STATUS()
{
    return HW_H264FITQ(H264FITQ_REG_STS);
}

/*****************************************************************************/
/**
 * \brief   h264fitq: configure block H264FITQ
 * \author  Rebecca Richard 
 * \param   mode: H264FITQ_CFG_DECODE, H264FITQ_CFG_ENCODE 
 * \param   scaling_list : H264FITQ_CFG_SCALING_LIST_OFF, H264FITQ_CFG_SCALING_LIST_ON
 * \param   intra_slice: H264FITQ_CFG_P_OR_B_SLICE, H264FITQ_CFG_I_SLICE
 * \param   symbol_mode: H264FITQ_CFG_CAVLC, H264FITQ_CFG_CABAC
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_CONFIG(t_h264fitq_mode                mode,
                                t_h264fitq_slpf                scaling_list,
                                t_h264fitq_islice              intra_slice,
                                t_h264fitq_sme                 symbol_mode)
{
     HW_H264FITQ(H264FITQ_REG_CFG)=
           ((mode                 )
           |(scaling_list    << 1)
           |(intra_slice     << 2)
           |(symbol_mode     << 3));

}


/*****************************************************************************/
/**
 * \brief   h264fitq: configure command of block H264FITQ
 * \author  Rebecca Richard 
 * \param   mb_mode: H264FITQ_CMD_INTER, H264FITQ_CMD_INTRA 
 * \param   mb_type : H264FITQ_CMD_NOT_I16x16, H264FITQ_CMD_I16x16
 * \param   luma_transform: H264FITQ_CMD_4x4T, H264FITQ_CMD_8x8T
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_CMD(t_h264fitq_mb_mode             mb_mode,
                             t_h264fitq_mb_type             mb_type,
                             t_h264fitq_transform           luma_transform)

{
     HW_H264FITQ_WAIT(H264FITQ_FIFO_CMD)=
           ((mb_mode                 )
           |(mb_type            << 1)
           |(luma_transform     << 2));

}

/*****************************************************************************/
/**
 * \brief   h264fitq: set luma QP 
 * \author  Rebecca Richard 
 * \param   qp: quantizer value for luma 
 * \warning Value should be in range of 0 to 51 
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_QPY(t_uint16 quant)

{
     HW_H264FITQ_WAIT(H264FITQ_FIFO_QPY)=
            ( (quant & 0x3F             ));

}

/*****************************************************************************/
/**
 * \brief   h264fitq: set luma QP Cb
 * \author  Rebecca Richard 
 * \param   qp: quantizer value for chroma 
 * \warning Value should be in range of 0 to 51 
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_QPCB(t_uint16 quant)

{
     HW_H264FITQ_WAIT(H264FITQ_FIFO_QPCB)=
            ( (quant & 0x3F             ));
}

/*****************************************************************************/
/**
 * \brief   h264fitq: set luma QP Cr
 * \author  Rebecca Richard 
 * \param   qp: quantizer value for chroma 
 * \warning Value should be in range of 0 to 51 
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_QPCR(t_uint16 quant)

{
     HW_H264FITQ_WAIT(H264FITQ_FIFO_QPCR)=
            ( (quant & 0x3F             ));
}

/*****************************************************************************/
/**
 * \brief   h264fitq: Write scaling list matrix
 * \author  Cyril Enault 
 * \param   addr of start of scaling list. 
 *
 */
/*****************************************************************************/
#pragma inline
static void H264FITQ_SET_SLM(t_uint16 *ptr_slm)

{
    t_uint16 i;

    for ( i = 0 ; i < (3*16+3*16+2*64) ; i++)
    {
        HW_H264FITQ_WAIT(H264FITQ_SLM_BASE+i)= *ptr_slm++;
    }
}


#endif /* _T1XHV_H264FITQ_H_ */

