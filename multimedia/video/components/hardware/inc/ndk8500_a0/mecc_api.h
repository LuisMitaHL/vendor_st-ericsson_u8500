/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _T1XHV_MECC_H_
#define _T1XHV_MECC_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include <stwdsp.h>             /* for mmdsp intrinsics */
#include <assert.h>             /* for assertion function in debug mode */
#include "t1xhv_types.h"        /* for basic types definitions */
#include "t1xhv_hamac_xbus_regs.h"
#include "macros.h"
#include "mmdsp_api.h"          /* for nop */
/*------------------------------------------------------------------------
 * Defines			       
 *----------------------------------------------------------------------*/

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get MECC_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_IMC(a) (*(volatile MMIO t_uint16 *)(IMC_BASE+a))
#define HW_IMC_WAIT(a) (*(volatile __XIO t_uint16 *)(IMC_BASE+a))

#define HW_IME(a) (*(volatile MMIO t_uint16 *)(IME_BASE+a))
#define HW_IME_WAIT(a) (*(volatile __XIO t_uint16 *)(IME_BASE+a))

/*------------------------------------------------------------------------
 * Types							       
 *----------------------------------------------------------------------*/

/** @{ \name IMC_REG_ENR, IMC_REG_DIR, IMC_REG_STA bitfield values
 *     \warning DO NOT change enum values
 */
/** \brief Register status  */
typedef enum
{
    MECC_IDLE=0,
    MECC_RUNNING=1
} t_mecc_reg_status;
/** @} end of IMC_REG_ENR, IMC_REG_DIR, IMC_REG_STA bitfield values*/

/** \brief IME Register status  */
typedef enum
{
    MECC_IME_IDLE=0,
    MECC_IME_RUNNING=1
} t_mecc_ime_reg_status;

/** @{ \name IMC_REG_FILTER_MODE bitfield values
 *     \warning DO NOT change enum values
 */
/** \brief Precision of all coordinates received from MMDSP  */
typedef enum
{
    HALF_PIXEL=0,
    QUARTER_PIXEL=1,
    QUARTER_PIXEL_LUMA_BILINEAR_VC1=2,
    QUARTER_PIXEL_LUMA_BICUBIC_VC1=3,
    QUARTER_PIXEL_MPEG4=4
    
} t_mecc_coord_precision;
/** @} end of IMC_REG_FILTER_MODE bitfield values*/

/** @{ \name IMC_REG_RND bitfield values
 *     \warning DO NOT change enum values
 *     \warning Used only for MPEG4 ,H263 and VC1 (IMC_REG_FILTER_MODE = HALF_PIXEL)
 */

/** \brief Rounding factor used after horizontal and vertical filter  */
typedef enum
{
    RND_NOT_USED=0,
    RND_USED=1
} t_mecc_rounding;
/** @} end of IMC_REG_RND bitfield values*/

/** @{ \name MECC_FIFO_CMD bitfield values
 *     \warning DO NOT change enum values
 *     \warning At least one of the parameters luma or chroma must be set to used
 */

typedef enum
{
    WP_TYPE_DEFAULT  = 0,  /**< \brief default weighted prediction  */
    WP_TYPE_EXPLICIT = 1,  /**< \brief explicit weighted prediction */
    WP_TYPE_IMPLICIT = 2   /**< \brief implicit weighted prediction */
} t_imc_wp_type;


/** \brief Command to be executed in MECC  */
typedef enum
{
    SEND_1_MB=0,
    SEND_IPA=1, /* only when IMC_REG_FILTER_MODE =1 */
    GET_N_MC=3
} t_mecc_command;


/** \brief Set if luma must be read or written (depending of the command)   */
typedef enum
{
    LUMA_NOT_USED=0, /**< \brief not read/written  */
    LUMA_USED=1 /**< \brief read/written   */
} t_mecc_cmd_luma;

/** \brief Set if chroma must be read or written (depending of the command)   */
typedef enum
{
    CHROMA_NOT_USED=0,  /**< \brief not read/written  */
    CHROMA_USED=1 /**< \brief read/written   */
} t_mecc_cmd_chroma;

/** \brief Set if IT must be sent at the end of the command   */
typedef enum
{
    IT_NOT_USED=0,  /**< \brief not read/written  */
    IT_USED=1 /**< \brief read/written   */
} t_mecc_cmd_it;


/** \brief Case of command SEND_1_MB   */
typedef enum
{
    DUMMY=0x0,

    P2P_RASTER_MB=0x1, /**< \brief Predictor to Post-adder raster scan MB: 16x16 luma, 8x8 chroma   */
    P2P_RASTER_8x8=0x2, /**< \brief Predictor to Post-adder raster scan 8x8   */
    P2P_RASTER_4x4=0x3, /**< \brief Predictor to Post-adder raster scan 4x4   */
    P2P_RASTER_8x8_CH4x4=0x18, /**< \brief Predictor to Post-adder raster scan 8x8 chroma 4x4   */

    O2P_RASTER_MB=0x5, /**< \brief Original to Post-adder raster scan MB: 16x16 luma, 8x8 chroma   */
    O2P_RASTER_8x8=0x6, /**< \brief Original to Post-adder raster scan 8x8   */
    O2P_RASTER_4x4_CH4x4=0x7, /**< \brief Original to Post-adder raster scan 4x4 chroma 4x4   */
    O2P_RASTER_8x8_CH4x4=0x19, /**< \brief Original to Post-adder raster scan 8x8 chroma 4x4 */

    R2T_RASTER_8x8=0x8, /**< \brief Residual to DCT raster scan 8x8   */
    R2T_RASTER_4x4=0x9, /**< \brief Residual to DCT raster scan 4x4   */

    O2T_RASTER_8x8=0xA, /**< \brief Original to DCT raster scan 8x8   */
    O2T_RASTER_4x4=0xB, /**< \brief Original to DCT raster scan 4x4   */

    P2P_R2T_RASTER_8x8=0xC, /**< \brief 2 outputs: Predictor to Post-adder and Residual to DCT raster scan 8x8   */
    P2P_R2T_RASTER_4x4_CH4x4=0xD, /**< \brief 2 outputs: Predictor to Post-adder and Residual to DCT raster scan luma 4x4 chroma 4x4  */
    P2P_R2T_RASTER_8x8_CH4x4=0x1A, /**< \brief 2 outputs: Predictor to Post-adder and Residual to DCT raster scan luma 8x8 chroma 4x4   */

    B2P_RASTER_8x8=0xE, /**< \brief Bidirectional to Post-adder raster scan 8x8   */
    B2P_RASTER_4x4_CH4x4=0xF,  /**< \brief Bidirectional to Post-adder raster scan luma 4x4 chroma 4x4   */
    B2P_RASTER_8x8_CH4x4=0x1B,  /**< \brief Bidirectional to Post-adder raster scan luma 8x8 chroma 4x4   */
   
    IP2P_RASTER_8x8 = 0x11, /**< \brief Interlaced to Post-adder raster scan 8x8  */ 

    D2P_RASTER_8x8 = 0x12, /**< \brief  Deinterlaced to Post-adder raster scan 8x8  */ 
   
    IB2P_RASTER_8x8 = 0x1E, /**< \brief Bidirectional Interlaced to Post-adder raster scan 8x8  */ 

    DB2P_RASTER_8x8 = 0x1F  /**< \brief Bidirectional Deinterlaced to Post-adder raster scan 8x8  */ 
} t_mecc_cmd_parameters;

/** @} end of MECC_FIFO_CMD bitfield values*/

typedef enum
{
    WP_DISABLE = 0,  /**< \brief disable weighted prediction  */
    WP_ENABLE  = 1   /**< \brief enable weighted prediction   */
} t_mecc_cmd_wp;

typedef enum
{
    GET_LUMA_CHROMA = 0,  /**< \brief get_command: in memory will requiere luma then chroma  */
    GET_CHROMA_LUMA = 1   /**< \brief get_command: in memory will requiere chroma then luma : HW optims  */
} t_mecc_mem_access_order;

typedef enum
{
    LAST_DISABLE = 0,  /**< \brief disable the use of the CUP flag indicating the last partition parameters sent by CUP for the current macroblock  */
    LAST_ENABLE  = 1   /**< \brief enable the use of the CUP flag     */
} t_mecc_cmd_en_last;

/** @{ \name MECC_FIFO_REF_ID, MECC_FIFO_DST_BUF bitfield values
 *     \warning DO NOT change enum values
 */
typedef enum
{
    BUF_ORIGINAL =0,
    BUF_PREDICTOR=1,
    BUF_BACKWARD =3  /* ASP-backward predictor REF_ID for B VOP */
} t_mecc_buffer;

/** @} end of MECC_FIFO_REF_ID bitfield values*/

/** @{ \name MECC_FIFO_SIZE bitfield values
 *     \warning DO NOT change enum values
 *     \warning Using a size not allowed (see table2) gives random result
 */
typedef enum
{
    P16x16=0x0,
    P8x16 =0x4,
    P16x8 =0x1,
    P8x8  =0x5,
    P4x8  =0x9,
    P8x4  =0x6,
    P4x4  =0xA,
    P2x4  =0x7,
    P4x2  =0xB,
    P2x2  =0xF
} t_mecc_partition_size;

/** @} end of MECC_FIFO_SIZE bitfield values*/

typedef enum
{
    PREDFLAGL0_0   =  0,
    PREDFLAGL0_1   =  1,
    PREDFLAGL0_2   =  2,
    PREDFLAGL0_3   =  3,
    PREDFLAGL1_0   =  4,
    PREDFLAGL1_1   =  5,
    PREDFLAGL1_2   =  6,
    PREDFLAGL1_3   =  7,
    UPDATEFLAGL0_0 =  8,
    UPDATEFLAGL0_1 =  9,
    UPDATEFLAGL0_2 = 10,
    UPDATEFLAGL0_3 = 11,
    UPDATEFLAGL1_0 = 12,
    UPDATEFLAGL1_1 = 13,
    UPDATEFLAGL1_2 = 14,
    UPDATEFLAGL1_3 = 15
} t_imc_wp_mb_flags;

/** \brief Command dummy for MECC */

#define MECC_CMD_DUMMY  0

/** @} end of Command dummy for MECC */

/** @{ \name MECC FIFO bitfield values - Keep from MECC 1.0 
 */

/** \brief Sub-Command Original */
typedef enum
{
    MECC_FIFO_SUB_CMD_ORIG            =0,
    MECC_FIFO_SUB_CMD_ORIG_JPEG       =2,
    MECC_FIFO_SUB_CMD_ORIG_SKIP       =4,
    MECC_FIFO_SUB_CMD_ORIG_STAB       =6, /* NOT IN MECC 1.0  */
    MECC_FIFO_SUB_CMD_ORIG_JPEG_ITLV  =8,
    MECC_FIFO_SUB_CMD_ORIG_JPEG_ROTATE  =10
} t_mecc_sub_cmd_orig;



/** \brief Sub-Command Motion Compensation Decision */
typedef enum
{
     MECC_FIFO_SUB_CMD_MD_INTER_MC     =0,
     MECC_FIFO_SUB_CMD_MD_INTER_NOMC   =1, 
     MECC_FIFO_SUB_CMD_MD_INTRA        =2,
     MECC_FIFO_SUB_CMD_MD_INTRA_LUMA   =3,
     MECC_FIFO_SUB_CMD_MD_NC_NOTH      =4,
     MECC_FIFO_SUB_CMD_MD_NC_ZERO      =5,
     MECC_FIFO_SUB_CMD_MD_DUMMY        =6, 
     MECC_FIFO_SUB_CMD_MD_INTRA_H264E  =7
} t_mecc_sub_cmd_md;


/** \brief Sub-Command Motion Compensation Prediction */
typedef enum
{
      MECC_FIFO_SUB_CMD_MP_FW_1_MV     =0,
      MECC_FIFO_SUB_CMD_MP_FW_4_MV     =2
} t_mecc_sub_cmd_mp;

/** @} end of MECC FIFO  bitfield values*/

/** \brief Sub-Commandnb_mv_coord	Number of MV coordinates that will be received in IMC_REG_MPX and IMC_REG_MPY. Range 0 to 16. */
typedef enum
{ 
    MECC_FIFO_SUB_CMD_1_MV_COORD =1,
    MECC_FIFO_SUB_CMD_2_MV_COORD =2,
    MECC_FIFO_SUB_CMD_4_MV_COORD =4
} t_mecc_sub_cmd_nb_mv;

/** @{ \name VC1 specifics */
/** \brief IMC_REG_RANGE_REDUCTION bitfield   
 *  \warning: do not edit those values*/
typedef enum
{
    MECC_RANGERED_DISABLE = 0,
    MECC_RANGERED_DOWNSCALE_PRED =1,
    MECC_RANGERED_UPSCALE_PRED = 3

} t_mecc_range_reduction;
/** \brief IMC_REG_RANGE_REDUCTION bitfield   
 *  \warning: do not edit those values*/
typedef enum
{
    MECC_INTENSITY_COMP_DISABLE = 0,
    MECC_INTENSITY_COMP_ENABLE = 1
} t_mecc_intensity_comp;

/** \brief input for MECC FIFOs
 *  \warning: do not edit those values*/
typedef enum
{
    MECC_FIFO_FROM_FW  = 0,
    MECC_FIFO_FROM_CUP = 1
} t_mecc_fifo_input;

/** @}*/

/*------------------------------------------------------------------------
 * Locals functions							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Exported functions							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Access to IMC registers							       
 *----------------------------------------------------------------------*/
/*****************************************************************************/
/**
 * \brief 	mecc: reset(soft)
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/
#pragma inline 
static void MECC_RESET()
{
    HW_IMC(IMC_REG_SOFT_RESET) = 0x1;
    MMDSP_NOP();
}

/*****************************************************************************/
/**
 * \brief 	mecc: run (enable execution of command)
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_RUN()
{
    HW_IMC(IMC_REG_ENR) = 0x1;
}

/*****************************************************************************/
/**
 * \brief 	mecc: run with CUP inputs (enable execution of command)
 * \author 	Rebecca richard 
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_RUN_WITH_CUP()
{
    HW_IMC(IMC_REG_ENR) = 0x3;
}


/*****************************************************************************/
/**
 * \brief 	mecc: run (enable execution of command)
 * \param 	mecc_input defines xbus/cup as input
 * \author 	Cyril Enault
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_RUN_MUX(t_mecc_fifo_input mecc_input)
{
    HW_IMC(IMC_REG_ENR) = 0x1|(mecc_input<<1);
}

/****************************************************************************/
/**
 * \brief 	mecc: abort
 * \author 	Rebecca richard (grandvaux)
 **/
/****************************************************************************/

#pragma inline
static void MECC_ABORT()
{
    HW_IMC(IMC_REG_DIR) = 0x1;
}

/*****************************************************************************/
/**
 * \brief 	mecc: wait activity status
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/
#pragma inline 
static void MECC_WAIT_IDLE()
{
/*  trouble on STA status on MECC HV_HW_4.0.10 */
    while(HW_IMC(IMC_REG_STA)) {};
}

/*****************************************************************************/
/**
 * \brief 	mecc: acknowledge interrupt
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_ACK_INTERRUPT()
{
/*    HW_IMC(IMC_REG_ISR) = 0x1;*/
}


/*****************************************************************************/
/**
 * \brief 	mecc: set precision of coordinates received from MMDSP
 * \author 	Rebecca richard (grandvaux)
 * \param 	coord_precision : HALF_PIXEL (MPEG4/H263) / QUARTER_PIXEL (H264)
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_COORD_PRECISION(t_mecc_coord_precision coord_precision)
{
    HW_IMC(IMC_REG_FILTER_MODE) = coord_precision;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set/unset rounding factor used after horizontal and vertical filter
 * \warning Used only for MPEG4 and H263 (IMC_REG_FILTER_MODE = HALF_PIXEL)
 * \author 	Rebecca richard (grandvaux)
 * \param 	rounding : RND_NOT_USED / RND_USED
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_ROUNDING(t_mecc_rounding rounding)
{
    ASSERT(rounding == HALF_PIXEL);

    HW_IMC(IMC_REG_RND) = rounding;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set image horizontal width (in pixel)
 * \warning Set IMC_REG_FILTER_MODE register to right value by function MECC_SET_COORD_PRECISION(...) before calling this function
 * \warning for MPEG4/H263 (IMC_REG_FILTER_MODE = HALF_PIXEL) : last LSB is 0 ; for H264 (IMC_REG_FILTER_MODE = QUARTER_PIXEL) : 2 last LSB are 0.  
 * \author 	Rebecca richard (grandvaux)
 * \param 	image_width : in pixels (range 0 to 4095)
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_IMAGE_WIDTH(t_uint16 image_width)
{
    ASSERT( (image_width >=0) && (image_width <=4095) );

if( HW_IMC(IMC_REG_FILTER_MODE) ==4 )
     HW_IMC(IMC_REG_IMAGE_WIDTH) = (image_width & 0xFFF) << ( 1+(QUARTER_PIXEL_MPEG4>>2) );
else
    HW_IMC(IMC_REG_IMAGE_WIDTH) = (image_width & 0xFFF) << ( 1+ HW_IMC(IMC_REG_FILTER_MODE));
}

/*****************************************************************************/
/**
 * \brief  Programming of register IMC_REG_WP_TYPE
 * \author Victor Macela
 * \param  wp_type : Weighted prediction type
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_REG_WP_TYPE(t_uint16 wp_type)
{
    HW_IMC_WAIT(IMC_REG_WP_TYPE) = wp_type;

}

/*****************************************************************************/
/**
 * \brief  Programming of register IMC_REG_WP_LOGWD_L
 * \author Victor Macela
 * \param  luma_log2_weight_denom : base 2 logarithm of the denominator for all luma weighting factors
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_REG_LOGWD_L(t_uint16 luma_log2_weight_denom)
{
    HW_IMC_WAIT(IMC_REG_WP_LOGWD_L) = luma_log2_weight_denom;

}

/*****************************************************************************/
/**
 * \brief  Programming of register IMC_REG_WP_LOGWD_C
 * \author Victor Macela
 * \param  chroma_log2_weight_denom : base 2 logarithm of the denominator for all chroma weighting factors
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_REG_LOGWD_C(t_uint16 chroma_log2_weight_denom)
{
    HW_IMC_WAIT(IMC_REG_WP_LOGWD_C) = chroma_log2_weight_denom;

}

/*****************************************************************************/
/**
 * \brief 	mecc: command to execute - FIFO of depth 3.
 * \warning 
 * - At least one of the parameters luma or chroma must be set to used.
 * - Case of command send_1_MB : parameters = Direction of the transfer
 * - Case of command GET_N_MC : parameters = Number of coordinates for MB partition that will be received
 *   in MECC_FIFO_MPX, MECC_FIFO_MY (range 1 to 16, WARNING value 0 means 16) 
 * - Case of command GET_1_INTRAPRED : parameters = Reserved
 * \author 	Rebecca richard (grandvaux)
 * \param 	command : command to execute (SEND_1_MB, SEND_IPA, GET_N_MC)
 * \param 	luma : command to execute (LUMA_NOT_USED, LUMA_USED) depending on command means luma read/written or not
 * \param 	chroma : command to execute (CHROMA_NOT_USED, CHROMA_USED) depending on command means chroma read/written or not
 * \param 	it : command to execute (IT_NOT_USED, IT_USED) depending on command means it send or not at the end of the command
 * \param   parameters : parameters of transfer
 *          - Case of command send_1_MB:+
 *           Direction of the transfer :
 *           P2P_RASTER_MB, P2P_RASTER_8x8, P2P_RASTER_4x4,
 *           O2P_RASTER_MB, O2P_RASTER_8x8, O2P_RASTER_4x4,
 *           R2P_RASTER_8x8, R2P_RASTER_4x4,
 *           O2P_RASTER_8x8, O2P_RASTER_4x4,
 *           P2P_R2T_RASTER_8x8, P2P_R2T_RASTER_4x4,
 *           B2P_RASTER_8x8, B2P_RASTER_4x4
 *          - Case of command send_ipa:+
 *           Direction of the transfer :
 *           P2P_RASTER_8x8, P2P_RASTER_4x4,
 *           P2P_R2T_RASTER_8x8, P2P_R2T_RASTER_4x4,
 *          - Case of command GET_N_MC :
 *           Number of coordinates for MB partition that will be received in MECC_FIFO_MPX, MECC_FIFO_MPY (range 1 to 16,
 *           WARNING value 0 means 16)
 * \param   wp : flag allowing information in FIFO IMC_FIFO_WP_FLAG to be used by IMC
 * \param   en_last : flag enabling the use of the CUP flag indicating the last partition parameters sent by CUP for the current macroblock
 *
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_CMD_WPRED(t_mecc_command             command,
				    t_mecc_cmd_luma            luma,
				    t_mecc_cmd_chroma          chroma,
				    t_mecc_cmd_it              it,
				    t_mecc_cmd_parameters      parameters,
				    t_mecc_cmd_wp              wp,
                                    t_mecc_cmd_en_last         en_last)
{
    HW_IMC_WAIT(IMC_FIFO_CMD)= 
        ((command                   )
        |(luma                  << 2)
        |(chroma                << 3)
        |(it                    << 4)
        |(parameters            << 5)
        |(wp                    << 10)
        |(en_last               << 11));

} /* end of inline MECC_SET_FIFO_CMD */


#pragma inline
static void MECC_SET_FIFO_CMD(t_mecc_command             command,
                              t_mecc_cmd_luma            luma,
                              t_mecc_cmd_chroma          chroma,
                              t_mecc_cmd_it              it,
                              t_mecc_cmd_parameters      parameters)
{
    HW_IMC_WAIT(IMC_FIFO_CMD)= 
        ((command                   )
        |(luma                  << 2)
        |(chroma                << 3)
        |(it                    << 4)
        |((parameters & 0xFF)   << 5));

} /* end of inline MECC_SET_FIFO_CMD */


/*****************************************************************************/
/**
 * \brief   mecc: send command original 
 * \author 	Rebecca richard (grandvaux)
 * \param	sub_command_orig : precise parameter command
 *          - Case of MECC_FIFO_CMD_ORIGINAL: 
 *           MECC_SUB_CMD_ORIG , MECC_SUB_CMD_ORIG_JPEG , MECC_SUB_CMD_ORIG_SKIP
 * \param   it_used_flag : tells if IT is requested to MECC or not
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_ORIG(t_mecc_sub_cmd_orig sub_command_orig, t_mecc_cmd_it it_used_flag)
{

    if ( (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG) | (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG_STAB) )
    {
		 HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
    /*  \brief  DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE
 are also ready (fifo with same size, read at same time by MECC) */

		 HW_IMC(IMC_FIFO_SIZE) = P8x8;
		 HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ;
		 HW_IMC_WAIT(IMC_FIFO_CMD)=
		   ((GET_N_MC                              )
			|(LUMA_NOT_USED                     << 2)
			|(CHROMA_USED                       << 3)
			|(IT_NOT_USED                       << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD      << 5));

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
    /*  \brief  DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE
 are also ready (fifo with same size, read at same time by MECC) */

        HW_IMC(IMC_FIFO_SIZE) = P16x16;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ;
        HW_IMC_WAIT(IMC_FIFO_CMD)=
            ((GET_N_MC                              )
            |(LUMA_USED                         << 2)
            |(CHROMA_NOT_USED                   << 3)
            |(it_used_flag                       << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD      << 5));


    }
    else if (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG_JPEG)
    {
        /* first partition 16x8 put at coordinates (0,0) */

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
    /*  \brief  DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE
 are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_SIZE) = P16x8;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ;
        HW_IMC_WAIT(IMC_FIFO_CMD)=
            ((GET_N_MC                              )
            |(LUMA_USED                         << 2)
            |(CHROMA_NOT_USED                   << 3)
            |(IT_NOT_USED                       << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD      << 5));

        /* second partition 16x8 put at coordinates (0,2) */

        HW_IMC(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
        HW_IMC(IMC_FIFO_SIZE) = P16x8;
        HW_IMC(IMC_FIFO_DST_BUF)= (2<<3) ;
        HW_IMC_WAIT(IMC_FIFO_CMD)=
            ((GET_N_MC                              )
            |(LUMA_USED                         << 2)
            |(CHROMA_NOT_USED                   << 3)
            |(IT_NOT_USED                       << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD      << 5));
    }
    else if (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG_SKIP)
    {
        /* NOT USED */
    }
    else if (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG_JPEG_ROTATE)
    {
	HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
	HW_IMC(IMC_FIFO_SIZE) = P8x8;
	HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
	HW_IMC(IMC_FIFO_SIZE) = P8x8;
	HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
	HW_IMC(IMC_FIFO_SIZE) = P8x8;
	HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
	HW_IMC(IMC_FIFO_SIZE) = P8x8;
	
        /* Put the 4 parts at each place */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ; /* Y0, top left */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL + (2<<1);
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL + (2<<3);
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL + (2<<3) + (2<<1); /* Y3, bottom right */

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_4_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

      }
	else if (sub_command_orig == MECC_FIFO_SUB_CMD_ORIG_JPEG_ITLV)
	  {
	HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;		
        HW_IMC(IMC_FIFO_SIZE) = P16x16;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_ORIGINAL ;
        HW_IMC_WAIT(IMC_FIFO_CMD)=
		  ((GET_N_MC                              )
		   |(LUMA_USED                         << 2)
		   |(CHROMA_USED                       << 3)
		   |(it_used_flag                       << 4)
		   |(MECC_FIFO_SUB_CMD_1_MV_COORD      << 5));
	  }
}

/*****************************************************************************/
/**
 * \brief   mecc: send command motion compensation decision 
 * \author 	Rebecca richard (grandvaux)
 * \param	sub_command_md	: precise parameter command
 *          - Case of MECC_FIFO_CMD_MOTION_DECISION:
 *           MECC_SUB_CMD_MD_INTER_MC, MECC_SUB_CMD_MD_INTER_NOMC , MECC_SUB_CMD_MD_INTRA , MECC_SUB_CMD_MD_INTRA_LUMA , MECC_SUB_CMD_MD_NC_NOTH , MECC_SUB_CMD_MD_NC_ZERO , MECC_SUB_CMD_MD_DUMMY
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MD(t_mecc_sub_cmd_md sub_command_md)
{
    if (sub_command_md == MECC_FIFO_SUB_CMD_MD_INTER_MC) {

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P16x16;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

		/*   HW_IMC_WAIT(IMC_FIFO_CMD)= 
			 ((SEND_1_MB                            )
			 |(LUMA_USED                        << 2)
			 |(CHROMA_NOT_USED                  << 3)
			 |(IT_NOT_USED                      << 4)
			 |(P2P_R2T_RASTER_8x8               << 5)); */

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_R2T_RASTER_8x8               << 5));
    } 
    else if (sub_command_md == MECC_FIFO_SUB_CMD_MD_INTER_NOMC)
    {
        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_R2T_RASTER_8x8               << 5));
    }
    else if ( (sub_command_md == MECC_FIFO_SUB_CMD_MD_NC_ZERO) | (sub_command_md == MECC_FIFO_SUB_CMD_MD_NC_NOTH) )
    {
        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_RASTER_8x8                   << 5));
    }
    else if (sub_command_md == MECC_FIFO_SUB_CMD_MD_INTRA)
    {
        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(O2T_RASTER_8x8                   << 5));
    }
    else if (sub_command_md == MECC_FIFO_SUB_CMD_MD_INTRA_LUMA)
    {
        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(O2T_RASTER_8x8                   << 5));
    }
    else if (sub_command_md == MECC_FIFO_SUB_CMD_MD_DUMMY)
    {
        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(DUMMY                            << 5));
    }
    else if (sub_command_md == MECC_FIFO_SUB_CMD_MD_INTRA_H264E)
    {
        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(O2T_RASTER_4x4                   << 5));
    }
}
/*****************************************************************************/
/**
 * \brief   mecc: send command motion compensation prediction 
 * \author 	Rebecca richard (grandvaux)
 * \param	sub_command_mp	: precise parameter command
 *          - Case of MECC_FIFO_CMD_MOTION_PREDICTION: 
 *           MECC_SUB_CMD_MP_FW_1_MV , MECC_SUB_CMD_MP_FW_4_MV
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MP(t_mecc_sub_cmd_mp sub_command_mp)
{
    t_uint16 cpt;

    if (sub_command_mp == MECC_FIFO_SUB_CMD_MP_FW_1_MV) {
        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
        /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P16x16;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_RASTER_8x8                   << 5));
    } 
    else if (sub_command_mp == MECC_FIFO_SUB_CMD_MP_FW_4_MV)
    {
        for (cpt=0;cpt<4;cpt++) 
            {
                HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
                /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
                HW_IMC(IMC_FIFO_SIZE) = P8x8;
            }
        /* Put the 4 parts at each place */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR + (2<<1);
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR + (2<<3);
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR + (2<<3) + (2<<1);

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_4_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_RASTER_8x8                   << 5));
    }
}
/*****************************************************************************/
/**
 * \brief   mecc: send command motion compensation prediction for H264 encoder
 * \author 	Rebecca richard (grandvaux)
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MP_H264()
{
        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
        /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P8x8;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_NOT_USED                    << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_PREDICTOR ;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_REF_ID is ready then MECC_FIFO_DST_BUF and MECC_FIFO_SIZE are also ready (fifo with same size, read at same time by MECC) */
        HW_IMC(IMC_FIFO_DST_BUF)= BUF_PREDICTOR ;
        HW_IMC(IMC_FIFO_SIZE) = P16x16;

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((GET_N_MC                             )
            |(LUMA_USED                        << 2)
            |(CHROMA_NOT_USED                  << 3)
            |(IT_NOT_USED                      << 4)
            |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

        HW_IMC_WAIT(IMC_FIFO_CMD)= 
            ((SEND_1_MB                            )
            |(LUMA_USED                        << 2)
            |(CHROMA_USED                      << 3)
            |(IT_NOT_USED                      << 4)
            |(P2P_R2T_RASTER_4x4_CH4x4         << 5));
}

/*****************************************************************************/
/**
 * \brief   mecc: send command backward motion compensation prediction 
 * \author 	Valerie Pierson
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MP_BWD()
{
    HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_BACKWARD ;
    HW_IMC(IMC_FIFO_SIZE)       = P16x16;
    HW_IMC(IMC_FIFO_DST_BUF)    = BUF_PREDICTOR;

    HW_IMC_WAIT(IMC_FIFO_CMD)= 
        ((GET_N_MC                             )
        |(LUMA_USED                        << 2)
        |(CHROMA_NOT_USED                  << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_BACKWARD ;
    HW_IMC(IMC_FIFO_SIZE)       = P8x8;
    HW_IMC(IMC_FIFO_DST_BUF)    = BUF_PREDICTOR;

    HW_IMC_WAIT(IMC_FIFO_CMD)= 
        ((GET_N_MC                             )
        |(LUMA_NOT_USED                    << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    HW_IMC_WAIT(IMC_FIFO_CMD)= 
        ((SEND_1_MB                            )
        |(LUMA_USED                        << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(P2P_RASTER_8x8                   << 5));
}
    
/*****************************************************************************/
/**
 * \brief   mecc: send command bidirectional motion compensation prediction 
 * \author 	Valerie Pierson
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MP_BIDIR()
{
    /* forward */
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_PREDICTOR ;
    HW_IMC(IMC_FIFO_SIZE)    = P16x16;
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_PREDICTOR ;    
    HW_IMC_WAIT(IMC_FIFO_CMD)     =
        ((GET_N_MC                             )
        |(LUMA_USED                        << 2)
        |(CHROMA_NOT_USED                  << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));
    /* chroma */
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_PREDICTOR ;
    HW_IMC(IMC_FIFO_SIZE)    = P8x8;
	HW_IMC(IMC_FIFO_DST_BUF) = BUF_PREDICTOR ;
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_NOT_USED                    << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    /* backward */
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_BACKWARD;
    HW_IMC     (IMC_FIFO_SIZE)  = P16x16;
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_ORIGINAL;
    HW_IMC_WAIT(IMC_FIFO_CMD)     =
        ((GET_N_MC                             )
        |(LUMA_USED                        << 2)
        |(CHROMA_NOT_USED                  << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_BACKWARD;
    HW_IMC     (IMC_FIFO_SIZE)    = P8x8;
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_ORIGINAL;
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_NOT_USED                    << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    /* interpolate forward and backward predictors and send result to post adder */
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((SEND_1_MB                            )
        |(LUMA_USED                        << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(B2P_RASTER_8x8                   << 5));
}

/*****************************************************************************/
/**
 * \brief   mecc: send command direct mode motion compensation prediction 
 * \author 	Valerie Pierson
 * 
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_FIFO_CMD_MP_DIRECT()
{
    t_uint16 cpt;
    
    /* forward */
    for (cpt=0;cpt<4;cpt++)
    {
        HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_PREDICTOR;
        HW_IMC     (IMC_FIFO_SIZE)    = P8x8;                 
    }
    /* Put the 4 parts at each place */            
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_PREDICTOR ;
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_PREDICTOR + (2<<1);
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_PREDICTOR + (2<<3);
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_PREDICTOR + (2<<3) + (2<<1);
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_USED                        << 2)
        |(CHROMA_NOT_USED                  << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_4_MV_COORD     << 5));
    /* chroma */                   
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_PREDICTOR ;
    HW_IMC     (IMC_FIFO_SIZE)    = P8x8;
    HW_IMC     (IMC_FIFO_DST_BUF) = BUF_PREDICTOR ;
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_NOT_USED                    << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    /* backward */
    for (cpt=0;cpt<4;cpt++)
    {
        HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_BACKWARD;
        HW_IMC     (IMC_FIFO_SIZE)    = P8x8;
    }            
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_ORIGINAL;
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_ORIGINAL + (2<<1);
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_ORIGINAL + (2<<3);
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_ORIGINAL + (2<<3) + (2<<1);
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_USED                        << 2)
        |(CHROMA_NOT_USED                  << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_4_MV_COORD     << 5));
    /* chroma */
    HW_IMC_WAIT(IMC_FIFO_REF_ID)  = BUF_BACKWARD;
    HW_IMC(IMC_FIFO_SIZE)    = P8x8;    
    HW_IMC(IMC_FIFO_DST_BUF) = BUF_ORIGINAL;
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((GET_N_MC                             )
        |(LUMA_NOT_USED                    << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(MECC_FIFO_SUB_CMD_1_MV_COORD     << 5));

    /* interpolate forward and backward predictors and send result to post adder */
    HW_IMC_WAIT(IMC_FIFO_CMD)=
        ((SEND_1_MB                            )
        |(LUMA_USED                        << 2)
        |(CHROMA_USED                      << 3)
        |(IT_NOT_USED                      << 4)
        |(B2P_RASTER_8x8                   << 5));
}

/*****************************************************************************/
/**
 * \brief 	mecc: set MB partition horizontal and vertical coordinates. Precision is the one selected in IMC_REG_FILTER_MODE. - FIFO of depth 16.
 * \warning Set IMC_REG_FILTER_MODE register to right value by function MECC_SET_COORD_PRECISION(...) before calling this function
 * \author 	Rebecca richard (grandvaux)
 * \param 	horizontal_coord_X : range is [-2048.0 ; 2047.75] in pixels
 * \param 	vertical_coord_Y : range is [-2048.0 ; 2047.75] in pixels
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_MPX_MPY(t_sint16 horizontal_coord_X,t_sint16 vertical_coord_Y)
{
    HW_IMC_WAIT(IMC_FIFO_MPX) = horizontal_coord_X;
    /*  \brief 	DO NOT WAIT FOR XIORDY ON THIS SECOND FIFO BECAUSE IF MECC_FIFO_MPX is ready then MECC_FIFO_MPY is also ready (fifo with same size, read at same time by MECC) */
    HW_IMC(IMC_FIFO_MPY) = vertical_coord_Y;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set reference buffer type original. - FIFO of depth 16.
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_REF_BUF_ORIG()
{
    HW_IMC_WAIT(IMC_FIFO_REF_ID)= BUF_ORIGINAL ;

}

/*****************************************************************************/
/**
 * \brief 	mecc: set reference buffer type predictor (four reference buffer possible for predictor). - FIFO of depth 16.
 * \author 	Rebecca richard (grandvaux)
 * \param 	IDx : Specified which of the 4 reference buffer is to be used.  
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_REF_BUF_PRED(t_uint16  IDx)
{
    HW_IMC_WAIT(IMC_FIFO_REF_ID)= 
        ((BUF_PREDICTOR               )
        |((IDx & 0xF)             << 1));

}

/*****************************************************************************/
/**
 * \brief 	mecc: select size of current MB/partition width x height. - FIFO of depth 16 .
 * \warning Using a size not allowed (see table2) gives random result
 * \warning When luma and chroma are used in MECC command, chroma_width = luma_width/2 and chroma_height = luma_height/2
 * \author 	Rebecca richard (grandvaux)
 * \param 	partition_size : P16x16, P8x16, P16x8, P8x8, P4x8, P8x4, P4x4, P2x4, P4x2, P2x2
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_PART_SIZE(t_mecc_partition_size partition_size)
{
    HW_IMC_WAIT(IMC_FIFO_SIZE) = partition_size;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set internal buffer type (original or predictor). - FIFO of depth 16. 
 * \author 	Rebecca richard (grandvaux)
 * \param 	dst_buffer : select original or predictor buffer BUF_ORIGINAL / BUF_PREDICTOR
 * \param 	dst_position_x : select horizontal position in selected buffer. Position is given in 4x4 block unit for luma and in 2x2 block unit for chroma. Position must be the coordinate of the upper left 4x4 MB (2x2 MB) subpart to be placed in luma (chroma).
 * \param 	dst_position_y : select vertical position in selected buffer. Position is given in 4x4 block unit for luma and in 2x2 block unit for chroma. Position must be the coordinate of the upper left 4x4 MB (2x2 MB) subpart to be placed in luma (chroma).
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_FIFO_DST_BUF(t_mecc_buffer              dst_buffer,
                                  t_uint16                   dst_position_x,
                                  t_uint16                   dst_position_y)
{
    HW_IMC_WAIT(IMC_FIFO_DST_BUF)= 
        ((dst_buffer                  )
        |((dst_position_x & 0x3)  << 1)
        |((dst_position_y & 0x3)  << 3));

}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_FLAG
 * \author Victor Macela
 * \param  wp_mb_flags : Bit-field containing the values of PREDFLAG_Lx_y and UPDATEFLAG_Lx_y
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_FLAG(t_uint16 wp_mb_flags)
{
    HW_IMC_WAIT(IMC_FIFO_WP_FLAG) = wp_mb_flags;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W0_L
 * \author Victor Macela
 * \param  luma_weight_l0 : List 0 luma weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W0_L(t_sint16 luma_weight_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W0_L) = luma_weight_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W0_CB
 * \author Victor Macela
 * \param  chroma_cb_weight_l0 : List 0 chroma Cb weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W0_CB(t_sint16 chroma_cb_weight_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W0_CB) = chroma_cb_weight_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W0_CR
 * \author Victor Macela
 * \param  chroma_cr_weight_l0 : List 0 chroma Cr weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W0_CR(t_sint16 chroma_cr_weight_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W0_CR) = chroma_cr_weight_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O0_L
 * \author Victor Macela
 * \param  luma_offset_l0 : List 0 luma additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O0_L(t_sint16 luma_offset_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O0_L) = luma_offset_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O0_CB
 * \author Victor Macela
 * \param  chroma_cb_offset_l0 : List 0 chroma Cb additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O0_CB(t_sint16 chroma_cb_offset_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O0_CB) = chroma_cb_offset_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O0_CR
 * \author Victor Macela
 * \param  chroma_cr_offset_l0 : List 0 chroma Cr additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O0_CR(t_sint16 chroma_cr_offset_l0)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O0_CR) = chroma_cr_offset_l0;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W1_L
 * \author Victor Macela
 * \param  luma_weight_l1 : List 1 luma weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W1_L(t_sint16 luma_weight_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W1_L) = luma_weight_l1;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W1_CB
 * \author Victor Macela
 * \param  chroma_cb_weight_l1 : List 1 chroma Cb weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W1_CB(t_sint16 chroma_cb_weight_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W1_CB) = chroma_cb_weight_l1;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_W1_CR
 * \author Victor Macela
 * \param  chroma_cr_weight_l1 : List 1 chroma Cr weighting factor
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_W1_CR(t_sint16 chroma_cr_weight_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_W1_CR) = chroma_cr_weight_l1;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O1_L
 * \author Victor Macela
 * \param  luma_offset_l1 : List 1 luma additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O1_L(t_sint16 luma_offset_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O1_L) = luma_offset_l1;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O1_CB
 * \author Victor Macela
 * \param  chroma_cb_offset_l1 : List 1 chroma Cb additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O1_CB(t_sint16 chroma_cb_offset_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O1_CB) = chroma_cb_offset_l1;
}

/*****************************************************************************/
/**
 * \brief  Programming of FIFO IMC_FIFO_WP_O1_CR
 * \author Victor Macela
 * \param  chroma_cr_offset_l1 : List 1 chroma Cr additive offset
 */
/*****************************************************************************/
#pragma inline
static void IMC_SET_FIFO_WP_O1_CR(t_sint16 chroma_cr_offset_l1)
{
    HW_IMC_WAIT(IMC_FIFO_WP_O1_CR) = chroma_cr_offset_l1;
}

/*****************************************************************************/
/**
 * \brief 	mecc: Set a generic register into MECC
 * \author 	Maurizio Colombo
 *  
 * 
 */
/*****************************************************************************/

#pragma inline
static void  IMC_SET_REG(t_uint16 reg_name, t_uint16 value)
{
    HW_IMC(reg_name)=value;
}


/****************************************************************************/
/**
 * \brief 	enable the raster mode for chroma components (must be enabled in the DMA as well)
 * \author 	Jean-Marc VOLLE
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void MECC_ENABLE_RASTER_CHROMA(void)
{
/* intentionnally commented 8815/8820 incompatibility */
/*     MECC_SET_REG(IMC_REG_RASTER, 1); */
}

/****************************************************************************/
/**
 * \brief 	enable the intensity compensation mode for VC1 prediction
 * \author 	Jean-Marc VOLLE
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void MECC_ENABLE_INTENSITY_COMP(t_uint16 en_fwd,t_uint16 en_bwd )
{

    HW_IMC_WAIT(IMC_REG_INTENSITY_COMP)= (en_fwd |
                                           en_bwd<<1);
}

/****************************************************************************/
/**
 * \brief 	program the mecc to enable Intensity compensation for forward frames
 * \author 	Aroua Ben Daryoug
 * \param 	p_pred_coord: pointer on chroma blocks predictor
 * 
 **/
/****************************************************************************/
/* never needed to enable intensity compensation on backward frames */
#pragma inline
static void MECC_IC_ON_FORWARD_ENABLE(t_uint16 lumascale,t_uint16  ishift)
{

    HW_IMC(IMC_REG_IC_FWD)= (lumascale | ishift << 6); 
}


#pragma inline
static void MECC_IC_ON_BACKWARD_ENABLE(t_uint16 lumascale,t_uint16  ishift)
{
    HW_IMC(IMC_REG_IC_BWD)= (lumascale | ishift << 6); 
}


/*------------------------------------------------------------------------
 * Access to IME registers							       
 *----------------------------------------------------------------------*/

/** \brief Sub-Command Motion Estimation */
typedef enum
{
     MECC_FIFO_SUB_CMD_ME_NO_ORDER     =0,
     MECC_FIFO_SUB_CMD_ME_ORDER        =4
} t_mecc_sub_cmd_me;

/** @{ \name IME_REG_CFG bitfield values
 */
 
/** \brief Fast ME convergence setting */
typedef enum
{
    MECC_CFG_FMEC_0     =0, 
    MECC_CFG_FMEC_1     =1,
    MECC_CFG_FMEC_2     =2,
    MECC_CFG_FMEC_3     =3
} t_mecc_fmec;

/** \brief temporal MV field index */
typedef enum
{
     MECC_CFG_TMVID_1     =0, /* \brief 1st part of the buffer */
     MECC_CFG_TMVID_2     =1  /* \brief 2nd part of the buffer */
} t_mecc_tmvid;

/** \brief Prefetch accross MB lines */
typedef enum
{
     MECC_CFG_PREFETCH_OFF    =0, 
     MECC_CFG_PREFETCH_ON     =1  
} t_mecc_prefetch;

/** \brief Coding choice and metrics outputs */
typedef enum
{
     MECC_CFG_CCMEN_OFF    =0, /* \brief Intra and Motion estimation are performed */ 
     MECC_CFG_CCMEN_ON     =1  /* \brief Only intra estimation is performed */ 
} t_mecc_ccmen;

/** \brief Intra mode */
typedef enum
{
     MECC_CFG_IE_AND_ME    =0, /* \brief Intra and Motion estimation are performed */ 
     MECC_CFG_IE           =1  /* \brief Only intra estimation is performed */ 
} t_mecc_intra_mode;

/** \brief Complexity control */
typedef enum
{
     MECC_CFG_I16x16_P16x16    =0, /* \brief I16x16 and P16x16 only */ 
     MECC_CFG_Inxn_P16x16      =1, /* \brief I16x16, I4x4 , I8x8 (if high profile) and P16x16 */ 
     MECC_CFG_I16x16_P8x8      =2, /* \brief I16x16, and P16x16 and subpartitions down to P8x8*/ 
     MECC_CFG_Inxn_P8x8        =3  /* \brief I16x16, I4x4 , I8x8 (if high profile) and P16x16 and subpartitions down to P8x8 */ 
} t_mecc_comp_control;

/** \brief Standard mode */
typedef enum
{
     MECC_CFG_H264B      =0, /* \brief H264 baseline */ 
     MECC_CFG_H264H      =1, /* \brief H264 high profile */ 
     MECC_CFG_MPEG4SP    =2, /* \brief MPEG4 */ 
     MECC_CFG_MPEG4SH    =3  /* \brief MPEG4 Short header */
} t_mecc_std_mode;

/** @} end of IME_REF_CFG bitfield values*/

/** @{ \name IME_REG_CMD bitfield values
 */

/** \brief Predicted MV is skipped MV */
typedef enum
{
     MECC_CMD_PRED_IS_NOT_SKIP_MV     =0, 
     MECC_CMD_PRED_IS_SKIP_MV         =1  
} t_mecc_pred_mv;

/** \brief Macroblock top availability */
typedef enum
{
     MECC_CMD_MB_TOP_NOT_AVAILABLE     =1, /* Inversion to be confirmed with Jose */
     MECC_CMD_MB_TOP_AVAILABLE         =0  
} t_mecc_mb_top_availaibility;

/** \brief Macroblock left availability */
typedef enum
{
     MECC_CMD_MB_LEFT_NOT_AVAILABLE     =1, 
     MECC_CMD_MB_LEFT_AVAILABLE         =0  
} t_mecc_mb_left_availaibility;

/** \brief Perform motion estimation for current MB */
typedef enum
{
     MECC_CMD_ME_OFF     =0, 
     MECC_CMD_ME_ON      =1  
} t_mecc_me;

/** \brief Perform intra estimation for current MB */
typedef enum
{
     MECC_CMD_IE_OFF     =0, 
     MECC_CMD_IE_ON      =1  
} t_mecc_ie;

/** \brief Perform metrics calculation for current MB */
typedef enum
{
     MECC_CMD_METRICS_OFF     =0, 
     MECC_CMD_METRICS_ON      =1  
} t_mecc_metrics;

/** \brief Iteration mode */
typedef enum
{
     MECC_CMD_RASTER     =0, /* Normal mode, data is loaded and prefetched */ 
     MECC_CMD_SAME_MB    =1, /* Restimatiom of same MB, nothiong is loaded or prefetched */  
     MECC_CMD_RESTART    =2  /* Restart at new MB position, everything needed at current position is reloaded */
} t_mecc_iteration_mode;

/** @} end of IME_REF_CMD bitfield values*/

/** \brief  IME Recommended coding choice (IME_REG_RCC bitfields)
 *  \warning: do not edit those values*/
typedef enum
{
    MECC_RCC_I16x16  = 0,
    MECC_RCC_I4x4    = 1,
    MECC_RCC_I8x8    = 2,
    MECC_RCC_P16x16  = 4,
    MECC_RCC_P8x8    = 5,
    MECC_RCC_P16x8   = 6,
    MECC_RCC_P8x16   = 7
} t_mecc_coding_choice;


/*****************************************************************************/
/**
 * \brief       ime: reset(soft)
 * \author      Rebecca richard (grandvaux)
 *  
 * 
 */
/*****************************************************************************/
#pragma inline 
static void IME_INIT()
{
    HW_IME_WAIT(IME_REG_RST) = 0x1;
    MMDSP_NOP();
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MAD computed on the 16x16 pixels of the luma part.
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command LUMA enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_MAD_ORIG()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_LM_MAD);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get min SAD for intra estimation 16x16
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_16x16_SAD()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_IE_SAD);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 16x16
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=top, 1=left, 2=DC, 3=Planar is never used 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_16x16_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_IE_DIR);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 4x4 for four Y1 sub-blocks 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=vertical, 1=horizontal, 2=DC 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_4x4_Y1_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_I4x4_DY1);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 4x4 for four Y2 sub-blocks 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=vertical, 1=horizontal, 2=DC 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_4x4_Y2_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_I4x4_DY2);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 4x4 for four Y3 sub-blocks 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=vertical, 1=horizontal, 2=DC 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_4x4_Y3_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_I4x4_DY3);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 4x4 for four Y4 sub-blocks 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=vertical, 1=horizontal, 2=DC 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_4x4_Y4_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_I4x4_DY4);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get direction corresponding to min SAD for intra estimation 8x8 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTRA enable is set 
 * \brief       The encoding value for the direction is identical to H.264 LUMA direction
 * \brief       , i.e. 0=vertical, 1=horizontal, 2=DC 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_IE_8x8_DIR()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_I8x8_DIR);
}

/*****************************************************************************/
/**
 * \brief 	mecc: set predicted quan for ME biasing
 * \author 	rajneesh kumar soni (GND)
 *  
 * \brief       The register is used for ME SAD biasing.
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_ME_QUANT(t_sint16 quant)
{
	HW_IME_WAIT(IME_REG_ME_QUANT) = quant;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set predicted MVx coordinate for motion estimation 16x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is used for ME SAD biasing.
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_ME_16x16_MPX(t_sint16 horizontal_coord_X)
{
    HW_IME_WAIT(IME_REG_ME_PVX) = horizontal_coord_X;
}

/*****************************************************************************/
/**
 * \brief 	mecc: set predicted MVy coordinate for motion estimation 16x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is used for ME SAD biasing.
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_ME_16x16_MPY(t_sint16 vertical_coord_Y)
{
    HW_IME_WAIT(IME_REG_ME_PVY) = vertical_coord_Y;
}


/*****************************************************************************/
/**
 * \brief 	mecc: set predicted cost lambda parameter for motion estimation 16x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * 
 * \brief       The register is used for ME SAD biasing.
 * \brief       Setting it to 0 disables effectively the SAD biasing. The maximum value is 127
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_ME_16x16_CL(t_uint16 cost_lambda)
{
    HW_IME_WAIT(IME_REG_ME_CLB) = cost_lambda;
}

/*****************************************************************************/
/**
 * \brief 	mecc:  Get the LM sum from IME
 * \author 	rajneesh kumar soni (GND)
 *  
 * \brief       
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_LM_SUM()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_LM_SUM);
}


/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 16x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x16_MPX()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_ME_MVX);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 16x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x16_MPY()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_ME_MVY);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 16x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x8_MPX1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M16x8_V1X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 16x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x8_MPY1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M16x8_V1Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 16x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x8_MPX2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M16x8_V2X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 16x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_16x8_MPY2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M16x8_V2Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 8x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x16_MPX1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x16_V1X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x16_MPY1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x16_V1Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 8x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x16_MPX2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x16_V2X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x16. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x16_MPY2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x16_V2Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPX1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V1X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPY1()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V1Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPX2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V2X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVx coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPY2()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V2Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPX3()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V3X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPY3()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V3Y);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPX4()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V4X);
}

/*****************************************************************************/
/**
 * \brief 	mecc: get MVy coordinate corresponding to min_SAD for motion estimation 8x8. 
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_ME_8x8_MPY4()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_M8x8_V4Y);
}


/*****************************************************************************/
/**
 * \brief 	mecc: get min SAD for motion estimation 16x16
 * \author 	Rebecca richard (grandvaux)
 *  
 * \brief       The register is only updated when the command INTER enable is set 
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_ME_16x16_SAD()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_ME_SAD);
}

/*****************************************************************************/
/**
 * \brief       mecc: set image size in IME (in mb)
 * \author      Rebecca richard (grandvaux)
 * \param       image_width : in mbs (range 0 to 127)
 * \param       image_height : in mbs (range 0 to 127)
 *
 *
 */
/*****************************************************************************/

#pragma inline
static void MECC_SET_IMAGE_SIZE(t_uint16 image_width, t_uint16 image_height)
{
    ASSERT( (image_width >=0) && (image_width <=127) );
    ASSERT( (image_height >=0) && (image_height <=127) );

    HW_IME_WAIT(IME_REG_SFW) = image_width & 0x7F ;
    HW_IME_WAIT(IME_REG_SFH) = image_height & 0x7F ;
}

/*****************************************************************************/
/**
 * \brief       mecc: set stabilisations offsets in IME (in pxls)
 * \author      Rebecca richard (grandvaux)
 * \param       image_width : in pxls 
 * \param       image_height : in pxls
 *
 *
 */
/*****************************************************************************/

#pragma inline
static void MECC_STAB_OFFSETS(t_uint16 stab_x, t_uint16 stab_y)
{
    HW_IME_WAIT(IME_REG_SOX) = stab_x  ;
    HW_IME_WAIT(IME_REG_SOY) = stab_y ;
}

/*****************************************************************************/
/**
 * \brief       mecc: get original MB horizontal coordinates 
 * \author      Rebecca richard (grandvaux)
 *
 *
 */
/*****************************************************************************/

#pragma inline
static t_sint16  MECC_GET_MBX()
{
    return (t_sint16) HW_IME_WAIT(IME_REG_MBX);
}

/*****************************************************************************/
/**
 * \brief       mecc: get original MB vertical coordinate
 * \author      Rebecca richard (grandvaux)
 *
 *
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_MBY()
{
    return (t_uint16) HW_IME_WAIT(IME_REG_MBY);
}

/*****************************************************************************/
/**
 * \brief       mecc: Set original MB horizontal coordinates 
 * \author      Maurizio Colombo
 *
 *
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_MBX(t_uint16 mbx)
{
    HW_IME_WAIT(IME_REG_MBX)=mbx;
}

/*****************************************************************************/
/**
 * \brief       mecc: set original MB vertical coordinates 
 * \author      Maurizio Colombo
 *
 *
 */
/*****************************************************************************/

#pragma inline
static void  MECC_SET_MBY(t_uint16 mby)
{
    HW_IME_WAIT(IME_REG_MBY)=mby;
}

/*****************************************************************************/
/**
 * \brief       ime status register
 * \author      Vincent Migeotte
 *
 *
 */
/*****************************************************************************/

#pragma inline
static t_uint16 MECC_IME_GET_STATUS(void)
{
    return (t_uint16) HW_IME_WAIT(IME_REG_STS)&0x01;
}

/* RR: STUFF TO ENABLE COMPILATION */
#define MECC_SET_FIFO_CMD_ME MECC_SET_FIFO_CMD_ME1


/*****************************************************************************/
/**
 * \brief   mecc: configure motion estimation (Use block IME)
 * \author      Rebecca richard (grandvaux)
 * \param   mode: MECC_CFG_H264B, MECC_CFG_H264H, MECC_CFG_MPEG4SP    
 * \param   comp_control : MECC_CFG_16x16, MECC_CFG_8x8
 * \param   intra_mode: MECC_CFG_IE_AND_ME, MECC_CFG_IE    
 * \param   ccmen: MECC_CFG_CCMEN_OFF, MECC_CFG_CCMEN_ON    
 * \param   prefetch: MECC_CFG_PREFETCH_OFF, MECC_CFG_PREFETCH_ON    
 * \param   it: IT_NOT_USED, IT_USED    
 * \param   motion_field_index: MECC_CFG_TMVID_1, MECC_CFG_TMVID_2    
 *
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_ME_CONFIG(t_mecc_std_mode                mode,
                               t_mecc_comp_control            comp_control,
			       t_uint16						  mvdx,
			       t_uint16						  mvdy,
                               t_mecc_intra_mode              intra_mode,
                               t_mecc_ccmen                   ccmen,
                               t_mecc_prefetch                prefetch,
                               t_mecc_cmd_it                  it,
                               t_mecc_tmvid                   motion_field_index,
			       t_mecc_fmec                    fmec)
{
     HW_IME_WAIT(IME_REG_CFG)=
           ((mode                 )
           |(comp_control << 2)
           |(mvdx 	  << 4)
           |(mvdy 	  << 7)
           |(intra_mode   << 9)
           |(ccmen        << 10)
           |(prefetch     << 11)
           |(it           << 12)
           |(motion_field_index << 13)
	   |(fmec << 14)
	   );

}

/*****************************************************************************/
/**
 * \brief   mecc: configure command motion estimation (Use block IME)
 * \author      Rebecca richard (grandvaux)
 * \param   iteration_mode: MECC_CMD_RASTER, MECC_CMD_SAME_MB, MECC_CMD_RESTART     
 * \param   metrics: MECC_CMD_METRICS_OFF, MECC_CMD_METRICS_ON    
 * \param   ie: MECC_CMD_IE_OFF,MECC_CMD_IE_ON      
 * \param   me: MECC_CMD_ME_OFF, MECC_CMD_ME_ON    
 * \param   uleft: MECC_CMD_MB_LEFT_NOT_AVAILABLE, MECC_CMD_MB_LEFT_AVAILABLE     
 * \param   utop: MECC_CMD_MB_TOP_NOT_AVAILABLE, MECC_CMD_MB_TOP_AVAILABLE     
 * \param   pred_mv: MECC_CMD_PRED_IS_NOT_SKIP_MV, MECC_CMD_PRED_IS_SKIP_MV    
 *
 */
/*****************************************************************************/
#pragma inline
static void MECC_SET_ME_CMD(t_mecc_iteration_mode          iteration_mode,
                            t_mecc_metrics                 metrics,
                            t_mecc_ie                      ie,
                            t_mecc_me                      me,
                            t_mecc_mb_left_availaibility   uleft,
                            t_mecc_mb_top_availaibility    utop,
                            t_mecc_pred_mv                 pred_mv)
{
     HW_IME_WAIT(IME_REG_CMD)=
           ((iteration_mode                 )
           |(metrics    << 2)
           |(ie         << 3)
           |(me         << 4)
           |(uleft      << 5)
           |(utop       << 6)
           |(pred_mv    << 7));

}

/*****************************************************************************/
/**
 * \brief 	mecc: get recommended coding choice from IME
 * \author 	Rebecca richard (grandvaux)
 *  
 */
/*****************************************************************************/

#pragma inline
static t_uint16  MECC_GET_ME_CODING_CHOICE()
{
    return ( (t_uint16) HW_IME_WAIT(IME_REG_RCC) & 0x7);
}


#endif /* _MECC_API_H_ */

