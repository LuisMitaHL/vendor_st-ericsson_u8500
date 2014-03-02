/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _IPA_API_H_
#define _IPA_API_H_
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
   through MMDSP Tools, now we get IPA_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_IPA(a) (*(volatile MMIO t_uint16 *)(IPA_BASE+a))
#define HW_IPA_WAIT(a) (*(volatile __XIO t_uint16 *)(IPA_BASE+a))

/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/
typedef enum
{
	IPA_VERTICAL      = 0x0,
	IPA_HORIZONTAL    = 0x1,
	IPA_DC            = 0x2,
	IPA_DDL           = 0x3,
	IPA_PLANE         = 0x3,
	IPA_DDR           = 0x4,
	IPA_VR            = 0x5,
	IPA_HD            = 0x6,
	IPA_VL            = 0x7,
	IPA_HU            = 0x8
} t_ipa_mode;
/* Mode now sperated in INxN and I16x16 */
typedef enum
{
	IPA_INXN_VER          = 0x0,
	IPA_INXN_HOR          = 0x1,
	IPA_INXN_DC           = 0x2,
	IPA_INXN_DDL          = 0x3,
	IPA_INXN_DDR          = 0x4,
	IPA_INXN_VR           = 0x5,
	IPA_INXN_HD           = 0x6,
	IPA_INXN_VL           = 0x7,
	IPA_INXN_HU           = 0x8
} t_ipa_inxn_mode;
typedef enum
{
	IPA_I16X16_VER        = 0x0,
	IPA_I16X16_HOR        = 0x1,
	IPA_I16X16_DC         = 0x2,
	IPA_I16X16_PLANE      = 0x3
} t_ipa_i16x16_mode;
typedef enum
{
	IPA_CHROMA_DC         = 0x0,
	IPA_CHROMA_HOR        = 0x1,
	IPA_CHROMA_VER        = 0x2,
	IPA_CHROMA_PLANE      = 0x3
} t_ipa_chroma_mode;
typedef enum
{
	IPA_ENCODE_MODE       = 0x0,
	IPA_DECODE_MODE       = 0x1
}
t_ipa_decode_mode;
typedef enum
{
	IPA_CUP_EN            = 0x0,
	IPA_XBUS_EN           = 0x1
}
t_ipa_enxbus;
typedef enum
{
	IPA_MONOCHROME  = 0x0,
	IPA_420         = 0x1
}
t_ipa_chroma_format_idc;
typedef enum
{
	IPA_PREFETCH_EOR_OFF  = 0x0,
	IPA_PREFETCH_EOR_ON   = 0x1
}
t_ipa_prefetch_eor;
typedef enum
{
	IPA_CLIP1_OFF = 0x0,
	IPA_CLIP1_ON  = 0x1
}
t_ipa_clip1;
typedef enum
{
	IPA_NOT_I16X16  = 0x0,
	IPA_I16X16      = 0x1
}
t_ipa_i16x16;
typedef enum
{
	IPA_4x4T  = 0x0,
	IPA_8x8T  = 0x1
}
t_ipa_t8x8;
typedef enum
{
	IPA_NOT_IPCM  = 0x0,
	IPA_IPCM      = 0x1
}
t_ipa_ipcm;
typedef enum
{
	IPA_NOT_SKIP  = 0x0,
	IPA_SKIP      = 0x1
}
t_ipa_skip;
typedef enum
{
	IPA_INTER     = 0x0,
	IPA_INTRA     = 0x1
}
t_ipa_intra;


/*****************************************************************************/
/**
 * \brief 	IPA: reset(soft)
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_RESET()
{
	HW_IPA_WAIT(IPA_REG_SOFT_RESET) = 0x1;
}

/*****************************************************************************/
/**
 * \brief 	IPA: start
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_CFG(t_ipa_enxbus enxbus,t_ipa_decode_mode mode)
{
	HW_IPA_WAIT(IPA_REG_CFG) = (enxbus<<1)|mode;
}

/*****************************************************************************/
/**
 * \brief 	IPA: abort
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_ABORT()
{
	HW_IPA_WAIT(IPA_REG_DIR) = 0x1;
}

/*****************************************************************************/
/**
 * \brief 	IPA: status
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 IPA_STATUS()
{
	return HW_IPA_WAIT(IPA_REG_STA);
}
/*****************************************************************************/
/**
 * \brief 	IPA: set chroma_format_idc
 * \author 	Cyril Enault
 * \param   chroma_format_idc defines chroma format sampling structure
 *          0 monochrome
 *          1 4.2.0
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_CHROMA_FORMAT_IDC(t_ipa_chroma_format_idc chroma_format_idc)
{
	HW_IPA_WAIT(IPA_REG_CHR) = chroma_format_idc;
}

/*****************************************************************************/
/**
 * \brief 	IPA: ipa mbx max
 * \author 	Cyril Enault
 * \param       mbx: number of MB per line
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_MBX_MAX(t_uint16 mbx)
{
	HW_IPA_WAIT(IPA_REG_MBX_MAX) = mbx;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa mby max
 * \author 	Cyril Enault
 * \param       mby: number of MB per column
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_MBY_MAX(t_uint16 mby)
{
	HW_IPA_WAIT(IPA_REG_MBY_MAX) = mby;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa prefetch end of row
 * \author 	Cyril Enault
 * \param   prefetch_eor prefetch end of row flag 
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_PREFETCH_EOR(t_ipa_prefetch_eor prefetch_eor)
{
	HW_IPA_WAIT(IPA_REG_PREFETCH_EOR) = prefetch_eor;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa fifo mbx
 * \author 	Cyril Enault
 * \param   mba macroblock availability 
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MBA(t_uint16 mb_availability)
{
	HW_IPA_WAIT(IPA_FIFO_MBA) = mb_availability;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa mbx
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MBX(t_uint16 mbx)
{
	HW_IPA_WAIT(IPA_FIFO_MBX) = mbx;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa mby
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MBY(t_uint16 mby)
{
	HW_IPA_WAIT(IPA_FIFO_MBY) = mby;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa mb_type
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MB_TYPE( t_ipa_clip1 clip1,
		t_ipa_i16x16 i16x16,
		t_ipa_t8x8 t8x8,
		t_ipa_ipcm ipcm,
		t_ipa_skip skip,
		t_ipa_intra intra)
{
	HW_IPA_WAIT(IPA_FIFO_MB_TYPE) = (clip1<<5)|(i16x16<<4)|(t8x8<<3)|(ipcm<<2)|(skip<<1)|(intra<<0);
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa fifo mode
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MODE(t_ipa_mode mode)
{
	HW_IPA_WAIT(IPA_FIFO_MODE) = mode;
}/*****************************************************************************/
/**
 * \brief 	IPA: ipa fifo mode for i_nxn prediction
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MODE_INXN(t_ipa_inxn_mode mode)
{
	HW_IPA_WAIT(IPA_FIFO_MODE) = mode;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa fifo mode for i_16x16 prediction
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MODE_I16X16(t_ipa_i16x16_mode mode)
{
	HW_IPA_WAIT(IPA_FIFO_MODE) = mode;
}
/*****************************************************************************/
/**
 * \brief 	IPA: ipa fifo mode for chroma prediction
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void IPA_SET_FIFO_MODE_CHROMA(t_ipa_chroma_mode mode)
{
	HW_IPA_WAIT(IPA_FIFO_MODE) = mode;
}


#endif /* _IPA_API_H_ */


