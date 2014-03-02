/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _MYHED_API_H_
#define _MYHED_API_H_

/*
 * Includes							       
 */
#include <stwdsp.h>             /* for mmdsp intrinsics */
#include "macros.h"            /* for assertion function in debug mode */
#include "t1xhv_retarget.h"        /* for basic types definitions */
#include "t1xhv_hamac_xbus_regs.h"
/*
 * Defines							       
 */
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get HED_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_HED(a) (*(volatile MMIO t_uint16 *)(HED_BASE+a))
#define HW_HED_WAIT(a) (*(volatile __XIO t_uint16 *)(HED_BASE+a))

#define MONOCHROME_IS_NULL 0
#define HED_MBT_SZ     6
#define HED_TS8X8_SZ   1+1
#define HED_ICP_SZ   2+6
#define HED_CBP_SZ   6+2
#define HED_LENGTH_SZ  7+1
#define HED_MB_SKIP_RUN_SZ   12+4
#define HED_MB_QP_DELTA_SZ    8+0
#define HED_FI_SZ   1+7
#define HED_I4X4_SZ   16
#define HED_SUB_MBT_SZ   4+4
#define HED_REFIDX_SZ   8
#define SIZE_OF_SLICE_HEADER 2*sizeof(t_uint32)

typedef enum
{
    HED_START_STOP     = 0x0,
    HED_START_PAUSE    = 0x1,
    HED_RESTART_STOP   = 0x2,
    HED_RESTART_PAUSE  = 0x3
} t_hed_control_mode;
typedef enum
{
    HED_MBT0      = 0x0,
    HED_MBT1      = 0x1,
    HED_MBT2      = 0x2,
    HED_MBT_LAST  = 0x3F
} t_hed_mbt;


typedef struct 
{
    t_uint16 MBAddr;
    t_sint16 first_mb_in_slice;
    t_uint32 SlicePointerOffset;
    t_uint16 error_status;
    t_uint16 lastSlice;
} t_hed_slice_header;

/*
 * Defines			       
 */

/*------------------------------------------------------------------------
 * Functions       
 *----------------------------------------------------------------------*/
/*****************************************************************************/
/** \brief 	Write a register from HED - 16 bits
 *  \author 	Cyril Enault
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void HED_SET_REG(t_uint16 addr, t_uint16 value)
{
    HW_HED_WAIT(addr) = value;
}
/*****************************************************************************/
/** \brief 	Write a register from HED - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void HED_SET_REG_32(t_uint16 addrl,t_uint16 addrh, t_uint32 value)
{
    HW_HED_WAIT(addrl) = LS_SHORT(value);
    HW_HED_WAIT(addrh) = MS_SHORT(value); 
}
/*****************************************************************************/
/** \brief 	Write a register from HED - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void HED_SET_REG_AHB_ADDR(t_uint16 addrl,t_uint16 addrh, t_ahb_address value)
{
#ifndef __T1XHV_NMF_ENV
    HW_HED_WAIT(addrl) = value.lsb;
    HW_HED_WAIT(addrh) = value.msb; 
#else
    HW_HED_WAIT(addrl) = wextract_l(value);
    HW_HED_WAIT(addrh) = wextract_h(value); 
#endif
}
/*****************************************************************************/
/** \brief 	Read a register from HED - 16 bits
 *  \author 	Cyril Enault
 *  \param	addr	Address to read (offset) 
 *  \return Read value
 */
/*****************************************************************************/
#pragma inline
static t_uint16 HED_GET_REG(t_uint16 addr) 
{
    return HW_HED_WAIT(addr);
}
/*****************************************************************************/
/** \brief 	Read a register from HED - 16 bits
 *  \author 	Cyril Enault
 *  \param	addr	Address to read (offset) 
 *  \return Read value
 */
/*****************************************************************************/
#pragma inline
static void HED_GET_REG_AHB_ADDR(t_uint16 addrl,t_uint16 addrh,t_ahb_address *value) 
{
#ifndef __T1XHV_NMF_ENV
    value->lsb = HED_GET_REG(addrl);
    value->msb = HED_GET_REG(addrh);
#else
	*value = winsert_l(winsert_h(0L, HED_GET_REG(addrh)), HED_GET_REG(addrl));
#endif
}
/*****************************************************************************/
/** \brief 	Read a register from HED - 16 bits
 *  \author 	Cyril Enault
 *  \param	addr	Address to read (offset) 
 *  \return Read value
 */
/*****************************************************************************/
#pragma inline
static t_uint32 HED_GET_REG32_ADDR(t_uint16 addrl,t_uint16 addrh) 
{
    return ((t_uint32)MAKE_LONG(HED_GET_REG(addrh),HED_GET_REG(addrl)));
}
#pragma inline
static void HED_SET_CFG_REG(t_uint16 monochrome,
                     t_uint16 direct_8x8_inference_flag,
                     t_hed_control_mode control_mode,
                     t_uint16 transform_8x8_mode_flag,
                     t_uint16 pic_init_qp,
                     t_uint16 num_ref_idx_l0_active_minus1,
                     t_uint16 num_ref_idx_l1_active_minus1,
                     t_uint16 deblocking_filter_control_present_flag,
                     t_uint16 weighted_bipred_idc_flag,
                     t_uint16 weighted_pred_flag,
                     t_uint16 reserved,
                     t_uint16 delta_pic_order_always_zero_flag,
                     t_uint16 pic_order_present_flag,
                     t_uint16 pic_order_cnt_type,
                     t_uint16 frame_mbs_only_flag,
                     t_uint16 entropy_coding_mode_flag,
                     t_uint16 mb_adaptive_frame_field_flag)
{
    t_uint32 valueToWrite =                 ((t_uint32)monochrome<<31)|
                             ((t_uint32)direct_8x8_inference_flag<<30)|
                                          ((t_uint32)control_mode<<28)|
                               ((t_uint32)transform_8x8_mode_flag<<27)|
                                           ((t_uint32)pic_init_qp<<21)|
                          ((t_uint32)num_ref_idx_l1_active_minus1<<16)|
                          ((t_uint32)num_ref_idx_l0_active_minus1<<11)|
                ((t_uint32)deblocking_filter_control_present_flag<<10)|
                              ((t_uint32)weighted_bipred_idc_flag<< 9)|
                                    ((t_uint32)weighted_pred_flag<< 8)|
                      ((t_uint32)delta_pic_order_always_zero_flag<< 6)|
                                ((t_uint32)pic_order_present_flag<< 5)|
                                    ((t_uint32)pic_order_cnt_type<< 3)|
                                   ((t_uint32)frame_mbs_only_flag<< 2)|
                              ((t_uint32)entropy_coding_mode_flag<< 1)|
                          ((t_uint32)mb_adaptive_frame_field_flag<< 0);
    HED_SET_REG_32(PP_CFG_L,PP_CFG_H,valueToWrite);
}
/****************************************************************************/
/**
 * \brief   retrieve infos from Slice Error Status Buffer generated by HED.
 * \author  cyril enault
 * \param   addr_isbg
 * \param   num_slice slice number to read.
 * \param   ptr on structure t_slice_header.
 * \note        read Slice Error Status  
 * 
 **/
/****************************************************************************/

void HED_GET_SLICE_HEADER(t_uint32 addr_hed_isbg, t_hed_slice_header *ptr_hed_slice_header_info);
void HED_GET_SLICE_HEADER_LOC(t_uint32 *ptr_loc_isbg, t_hed_slice_header *ptr_hed_sesb);


#endif /* _HED_API_H_ */

