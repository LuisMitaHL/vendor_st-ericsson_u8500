/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CUP_API_H_
#define _CUP_API_H_
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
   through MMDSP Tools, now we get CUP_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_CUP(a) (*(volatile MMIO t_uint16 *)(CUP_BASE+a))
//#define HW_CUP_WAIT(a) (*(volatile __XIO t_uint16 *)(CUP_BASE+a))
#define HW_CUP_WAIT(a) (*( (volatile __XIO t_uint16 *)(CUP_BASE) +a ))

/* COPIED FROM MODEL */
//#define CUP_CTX_X   256
//#define CUP_CTX_A   CUP_CTX_X   + 128
//#define CUP_CTX_B   CUP_CTX_A   + 128
//#define CUP_CTX_C   CUP_CTX_B   + 128
//#define CUP_CTX_D   CUP_CTX_C   + 128
//#define CUP_CTX_E   CUP_CTX_D   + 128
//
//#define CUP_CTX_MBTYP    0x0000
//#define CUP_CTX_CBP      0x0001
//#define CUP_CTX_QPY      0x0002
//#define CUP_CTX_QPC      0x0003
//#define CUP_CTX_NSLIC    0x0004
//#define CUP_CTX_CCEAL    0x0006
//#define CUP_CTX_ERROR    0x0007
//#define CUP_CTX_SOA      0x0008
//#define CUP_CTX_SOB      0x0009
//#define CUP_CTX_DISF     0x000A
//#define CUP_CTX_FREE     0x000B
//#define CUP_CTX_RFPIC    0x000C /* CUP_REG_CTX_A_RFPIC[i] = CUP_CTX_A+CUP_CTX_RFPIC + i (i=0..3) */
//#define CUP_CTX_I4X4     0x0010 /* CUP_REG_CTX_A_I4X4[i] = CUP_CTX_A+CUP_CTX_I4X4 + i (i=0..15) */
//#define CUP_CTX_SUB      0x0020 /* CUP_REG_CTX_A_SUB[i] = CUP_CTX_A+CUP_CTX_SUB + i (i=0..3) */
//#define CUP_CTX_RFIDX    0x0024 /* CUP_REG_CTX_A_RFIDX[i] = CUP_CTX_A+CUP_CTX_RFIDX + i (i=0..3) */
//#define CUP_CTX_NZC      0x0028 /* CUP_REG_CTX_A_NZC[i] = CUP_CTX_A+CUP_CTX_NZC + i (i=0..23) */
//#define CUP_CTX_MVX      0x0040 /* CUP_REG_CTX_A_MVX[i] = CUP_CTX_A+CUP_CTX_MVX + 4*i (i=0..15) */
//#define CUP_CTX_MVY      CUP_CTX_MVX + 2
/*****************************************************************************/
/*
 * Types       
 */
/*****************************************************************************/
typedef enum
{
    H264_MODE  = 0x0,
    VC1_MODE   = 0x1,
    H264E_MODE = 0x2
} t_cup_mode;
typedef enum
{
    IPRD_CUP = 0x0, // CUP -> IPRD
    IPRD_FW =  0x1  // CUP -> FW -> IPRD
} t_cup_iprd;
typedef enum
{
    MECC_CUP = 0x0, // CUP -> MECC
    MECC_FW =  0x1  // CUP -> FW -> MECC
} t_cup_mecc;
typedef enum
{
    RLC_CUP = 0x0, // CUP -> RLC
    RLC_FW =  0x1  // CUP -> FW -> RLC
} t_cup_rlc;
typedef enum
{
    IPA_CUP = 0x0, // CUP -> IPA
    IPA_FW =  0x1  // CUP -> FW -> IPA
} t_cup_ipa;
typedef enum
{
    CUP_CMD_LOAD_NONE = 0x00, // 000000
    CUP_CMD_LOAD_D    = 0x01, // 000001
    CUP_CMD_LOAD_B    = 0x02, // 000010
    CUP_CMD_LOAD_C    = 0x04, // 000100
    CUP_CMD_LOAD_E    = 0x08, // 001000
    CUP_CMD_LOAD_A    = 0x10, // 010000
    CUP_CMD_LOAD_X    = 0x20, // 100000
    CUP_CMD_LOAD_Y    = 0x40, // 100000
    CUP_CMD_LOAD_Z    = 0x80, // 100000
    CUP_CMD_LOAD_BC   = 0x06, // 00110
    CUP_CMD_LOAD_BCE  = 0x0E // 01110 for VC1 first row
} t_cup_load;
typedef enum
{
    CUP_CMD_SAVE_NONE   = 0x0, // 00
    CUP_CMD_SAVE_A      = 0x1, // 01
    CUP_CMD_SAVE_CUR    = 0x2 // 10
} t_cup_save;
typedef enum
{
    CUP_CMD_EXEC_NONE          = 0x00, // 000
    CUP_CMD_EXEC_ITER          = 0x01, // 001
    CUP_CMD_EXEC_INIT          = 0x02, // 010
    CUP_CMD_EXEC_ITERO         = 0x03, // 011
    CUP_CMD_EXEC_H264_PRED     = 0x04, // 100
    CUP_CMD_EXEC_H264_DBK      = 0x05, // 101
    CUP_CMD_EXEC_H264_MPI      = 0x06, // 101
    CUP_CMD_EXEC_H264_DBK_OPT  = 0x07, // 111
    CUP_CMD_EXEC_H264_RIP      = 0x08, // 101
    CUP_CMD_EXEC_VC1_IPRED     = 0x20, // 010000
    CUP_CMD_EXEC_VC1_MVPRED    = 0x30, // 110000 
    CUP_CMD_EXEC_VC1_IMCP      = 0x38, // 111000
    CUP_CMD_EXEC_VC1_IMCP_B    = 0x28, // 111001
    CUP_CMD_EXEC_VC1_DBK       = 0x29, 
    CUP_CMD_EXEC_H264E_CAVLC   = 0x2A, 
    CUP_CMD_EXEC_H264E_IMCP    = 0x2B, 
    CUP_CMD_EXEC_H264E_MB_PRED = 0x2C 
} t_cup_exec;
typedef enum
{
    LUMA_4x4        = 0x0, // 000
    LUMA_16x16      = 0x1, // 001
    CHROMA_4x4      = 0x2, // 010
    CHROMA_8x8      = 0x3, // 011
    ONE_LUMA_4x4    = 0x4 // 100
} t_cup_ipa_type;
typedef enum
{
    DC_FULL   = 0x0, // 0000
    DC_HOR    = 0x1, // 0001
    DC_CONST  = 0x2, // 0010
    DC_VERT   = 0x3, // 0011
    VER       = 0x4, // 0100
    HOR       = 0x5, // 0101
    PLANE     = 0x6, // 0110
    DDL       = 0x8, // 1000
    DDR       = 0x9, // 1001
    VR        = 0xA, // 1010
    HD        = 0xB, // 1011
    VL        = 0xC, // 1100
    HU        = 0xD // 1101
} t_cup_ipa_mode;
typedef enum
{ 
  REM_I4X4_0 = 0X0,
  REM_I4X4_1 = 0x1,
  REM_I4X4_2 = 0x2,
  REM_I4X4_3 = 0x3,
  REM_I4X4_4 = 0x4,
  REM_I4X4_5 = 0x5,
  REM_I4X4_6 = 0x6,
  REM_I4X4_7 = 0x7,
  PREV_I4x4 = 0x8  // 1000
}
t_prev_I4x4;
typedef enum
{ 
  SMB_8x8 = 0X0,
  SMB_8x4 = 0X1,
  SMB_4x8 = 0X2,
  SMB_4x4 = 0X3
}
t_sub_mb_type;

typedef enum
{
    D8X8I_FLAG_OFF  = 0x0,
    D8X8I_FLAG_ON   = 0x1
}
t_cup_d8x8i_flag;

typedef enum
{
    COIP_OFF  = 0x0,
    COIP_ON   = 0x1
}
t_cup_coip;
typedef enum
{
    IS_SHORT_TERM  = 0x0,
    IS_LONG_TERM   = 0x1
}
t_cup_col_ilt;
typedef enum
{
    DIRECT_SPATIAL  = 0x0,
    DIRECT_TEMPORAL = 0x1
}
t_cup_dsmpf;
typedef enum
{
    H264_FILTER_ON        = 0x0,
    H264_FILTER_OFF       = 0x1,
    H264_FILTER_NOEDGE    = 0x2
}
t_cup_ddfi;
typedef enum
{
    T4X4 = 0x0,
    T8X8 = 0x1
}
t_cup_t8x8;
typedef enum
{
    CUP_INTER_LEFT_PRED = 0,
    CUP_INTER_TOP_PRED = 1
}t_cup_inter_pred;
typedef enum
{
    CUP_MVRANGE_256_128 = 0,
    CUP_MVRANGE_512_256 = 2,
    CUP_MVRANGE_2048_512 = 6,
    CUP_MVRANGE_4096_1024 = 7
}t_cup_mvrange_quarter_pix;
typedef enum
{
    CUP_STATUS_BIT_MBA = 2,
    CUP_STATUS_BIT_IPN = 3
}t_cup_status_bit;

/* CUP status, register CUP_REG_STS */
#define CUP_IDLE      0
#define CUP_RUNNING   1
#define CUP_COMPLETED 2

/*****************************************************************************/
/**
 * \brief   CUP: reset(soft)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_RESET()
{
     HW_CUP_WAIT(CUP_REG_RST) = 0x1;
}
/*****************************************************************************/
/**
 * \brief   CUP: configure
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_CONFIGURE(t_cup_iprd iprd, t_cup_mecc mecc, t_cup_rlc rlc, t_cup_ipa ipa,t_cup_mode mode)
{
     HW_CUP_WAIT(CUP_REG_CFG) = (iprd<<12)|(mecc<<8)|(rlc<<7)|(ipa<<4)|mode;
}
/*****************************************************************************/
/**
 * \brief   CUP: configure
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_FIFO_CMD(t_cup_load load, t_cup_save save, t_cup_exec compute)
{
     HW_CUP_WAIT(CUP_FIF_CMD) =  (   compute | 
                              (save<<6) | 
                              (load<<8));

}
/*****************************************************************************/
/**
 * \brief   CUP: configure load
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_FIFO_CMD_LOAD(t_cup_load load)
{

     HW_CUP_WAIT(CUP_FIF_CMD) =     load<<8;

}
/*****************************************************************************/
/**
 * \brief   CUP: configure save
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_FIFO_CMD_SAVE(t_cup_save save)
{
     HW_CUP_WAIT(CUP_FIF_CMD) =     (save<<6);
}
/*****************************************************************************/
/**
 * \brief   CUP: configure compute
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_FIFO_CMD_COMPUTE(t_cup_exec compute)
{
     HW_CUP_WAIT(CUP_FIF_CMD) =     compute;

}
/*****************************************************************************/
/**
 * \brief   CUP: status
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_STS(void)
{
     return (HW_CUP_WAIT(CUP_REG_STS));
}
/*****************************************************************************/
/**
 * \brief   CUP: set frame widthin MB 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_REG_SFW(t_uint16 value)
{
     HW_CUP_WAIT(CUP_REG_SFW) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get frame width in MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_SFW(void)
{
     return (HW_CUP_WAIT(CUP_REG_SFW));
}
/*****************************************************************************/
/**
 * \brief   CUP: set frame height in MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_REG_SFH(t_uint16 value)
{
     HW_CUP_WAIT(CUP_REG_SFH) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set frame height in MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_SFH(void)
{
     return (HW_CUP_WAIT(CUP_REG_SFH));
}

/*****************************************************************************/
/**
 * \brief   CUP: set current MBX position
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_REG_MBX(t_uint16 value)
{
     HW_CUP_WAIT(CUP_REG_MBX) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get current MBX position
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_MBX(void)
{
     return (HW_CUP_WAIT(CUP_REG_MBX));
}
/*****************************************************************************/
/**
 * \brief   CUP: set current MBY position
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_REG_MBY(t_uint16 value)
{
     HW_CUP_WAIT(CUP_REG_MBY) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get current MBY position
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_MBY(void)
{
     return (HW_CUP_WAIT(CUP_REG_MBY));
}
/*****************************************************************************/
/**
 * \brief   CUP: get availability of surrounding MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_REG_MBA()
{
     return (HW_CUP_WAIT(CUP_REG_MBA));
}

/*****************************************************************************/
/**
 * \brief 	CUP: get IPA fifo Mode
 * \author 	Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_cup_ipa_mode CUP_GET_FIFO_IPA_MODE()
{
     return (HW_CUP_WAIT(CUP_FIF_IPAM));
}

/*****************************************************************************/
/**
 * \brief   CUP: get MPX (MECC)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_FIFO_MPX()
{
     return (HW_CUP_WAIT(CUP_FIF_PMPX));
}
/*****************************************************************************/
/**
 * \brief   CUP: get MPY (MECC)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_FIFO_MPY()
{
     return (HW_CUP_WAIT(CUP_FIF_PMPY));
}
/*****************************************************************************/
/**
 * \brief   CUP: get size of partition (MECC)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_FIFO_SIZE()
{
     return (HW_CUP_WAIT(CUP_FIF_PSIZ));
}
/*****************************************************************************/
/**
 * \brief   CUP: get destination (MECC)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_FIFO_PRED_DST()
{
     return (HW_CUP_WAIT(CUP_FIF_PDST));
}
/*****************************************************************************/
/**
 * \brief   CUP: get predictor for partition (MECC)
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_FIFO_PRED()
{
     return (HW_CUP_WAIT(CUP_FIF_PREF));
}
/*****************************************************************************/
/**
 * \brief   CUP: 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static CUP_SET_REG_SPP(t_cup_d8x8i_flag d8x8i, t_cup_coip coip)
{
     HW_CUP_WAIT(CUP_REG_SPP) = (d8x8i<<1)|coip;
}
/*****************************************************************************/
/**
 * \brief   CUP: 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static CUP_SET_REG_SHP(t_cup_col_ilt col_ilt, t_cup_dsmpf dsmpf, t_cup_ddfi ddfi, t_uint16 soa, t_uint16 sob)
{
     HW_CUP_WAIT(CUP_REG_SHP) = 
                          (col_ilt      <<11)
                        | (  dsmpf      <<10)
                        | (   ddfi      << 8)
                        | (    (sob&0xF)<< 4)
                        |      (soa&0xF)    ;
}
/*****************************************************************************/
/**
 * \brief   CUP: 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static CUP_SET_REG_ICP(t_uint16 icp)
{
     HW_CUP_WAIT(CUP_REG_ICP) = icp;
}

/*****************************************************************************/
/**
 * \brief   CUP: Set a generic register into CUP
 * \author      Victor Macela
 * 
 */
/*****************************************************************************/

#pragma inline
static void  CUP_SET_REG(t_uint16 reg_name, t_uint16 value)
{
    HW_CUP_WAIT(reg_name) = value;
}

/*****************************************************************************/
/**
 *
 *  CONTEXT
 * 
 */
/*****************************************************************************/

/* Write current context */
/* ie CTX */
/*****************************************************************************/
/**
 * \brief   CUP: set mb_type for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MBTYPE(t_cup_t8x8 t8x8, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_MBTYP) = (t8x8<<6) | value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set QPy (Luma) for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_QPY(t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_QPY) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set QPCb QPCr (Chroma) for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_QPC(t_uint16 qpcb,t_uint16 qpcr)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_QPCB) = qpcb;
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_QPCR) = qpcr;
}
/*****************************************************************************/
/**
 * \brief   CUP: set slice number for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_NSLIC(t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_NSLIC) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set slice number for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_sint16 CUP_GET_CTX_NSLIC(void)
{
     return HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_NSLIC);
}
/*****************************************************************************/
/**
 * \brief   CUP: set concealed code for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_CCEAL(t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_USER) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set slice number for current MB
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_ERASE_CTX_ERROR()
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_ERROR) = 0X0;
}

/*****************************************************************************/
/**
 * \brief   CUP: get error for current MB
 * \author  Victor Macela
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_CTX_ERROR(void)
{
     return HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_ERROR);
}

/*****************************************************************************/
/**
 * \brief   CUP: set refpic for current slice 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_REFPIC_L0(t_uint16 id, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFPIC_L0+id) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get refpic for current slice 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_CTX_REFPIC_L0(t_uint16 id)
{
     return (HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFPIC_L0+id));
}
/*****************************************************************************/
/**
 * \brief   CUP: set refpic for current slice 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_REFPIC_L1(t_uint16 id, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFPIC_L1+id) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get refpic for current slice 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_CTX_REFPIC_L1(t_uint16 id)
{
     return (HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFPIC_L1+id));
}
/*****************************************************************************/
/**
 * \brief   CUP: set 4x4 prediction mode for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_I4x4(t_uint16 block, t_prev_I4x4 mode)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_I4X4+2*block) = mode;
}
/*****************************************************************************/
/**
 * \brief   CUP: set sub mb type for partition 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_SUBMBTYPE(t_uint16 partition, t_sub_mb_type sub_mb_type)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_SUB+partition) = sub_mb_type;
}
/*****************************************************************************/
/**
 * \brief   CUP: set refid for partition 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_REFID_L0(t_uint16 partition, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFIDX_L0+partition) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set refid for partition 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_REFID_L1(t_uint16 partition, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_RFIDX_L1+partition) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: set 4x4 non zero coefficient for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_NZC(t_uint16 block, t_uint16 non_zero)
{
    //HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_NZC+block) = non_zero;
    volatile __XIO t_uint16 *ptr_nzc = (__XIO t_uint16 *)CUP_BASE+CUP_CTX_X+CUP_CTX_NZC;
    *(ptr_nzc+block) = non_zero;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVX for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MVX_L0(t_uint16 block, t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_MVX_L0+4*block) = mvx;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVY for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MVY_L0(t_uint16 block, t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_MVY_L0+4*block) = mvy;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVX for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MVX_L1(t_uint16 block, t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_MVX_L1+4*block) = mvx;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVY for block 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MVY_L1(t_uint16 block, t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_MVY_L1+4*block) = mvy;
}
/*****************************************************************************/
/**
 * \brief   CUP: set register reg for context ctx_x with value value 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_REG(t_uint16 ctx_x, t_uint16 reg, t_uint16 value)
{
    HW_CUP_WAIT(ctx_x+reg) = value;
}
/*****************************************************************************/
/**
 * \brief   CUP: get register reg for context ctx_x 
 * \author  Cyril Enault
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_CTX_REG(t_uint16 ctx_x, t_uint16 reg)
{
    return HW_CUP_WAIT(ctx_x+reg);
}
/*****************************************************************************/
/**
 * \brief   CUP: set VC1 MB type
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_VC1_MB_TYPE(t_uint16 mb_type)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMBTYP) =    mb_type;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVX for block for VC1
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_VMVX(t_uint16 block_num, t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVX+4*block_num) = mvx;
}
/*****************************************************************************/
/**
 * \brief   CUP: get MVX for block for VC1
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_sint16 CUP_GET_CTX_VMVX(t_uint16 block_num)
{
    return HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVX+4*block_num);
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVY for block for VC1
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_VMVY(t_uint16 block_num, t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVY+4*block_num) = mvy;
}
/*****************************************************************************/
/**
 * \brief   CUP: get MVX for block for VC1
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_sint16 CUP_GET_CTX_VMVY(t_uint16 block_num)
{
    return HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVY+4*block_num);
}

/*****************************************************************************/
/**
 * \brief   CUP: set MVX2 for B frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_VMVBX(t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVBX) = mvx;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MVX2 for B frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_VMVBY(t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VMVBY) = mvy;
}
/*****************************************************************************/
/**
 * \brief   CUP: get MV History x in P frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_sint16 CUP_GET_CTX_MV_HISTORY_X(void)
{
    return HW_CUP_WAIT(CUP_REG_COLMVX);
}
/*****************************************************************************/
/**
 * \brief   CUP: get MV History y in P frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_sint16 CUP_GET_CTX_MV_HISTORY_Y(void)
{
    return HW_CUP_WAIT(CUP_REG_COLMVY) ;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MV History x in P frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MV_HISTORY_X(t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_REG_COLMVX) = mvx;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MV History y in P frames(VC1)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_CTX_MV_HISTORY_Y(t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_REG_COLMVY) = mvy;
}

/*****************************************************************************/
/**
 * \brief   CUP: set VC1 block type
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_BLOCK_TYPE(t_uint16 block_num, t_uint16 csbp,t_uint16 blk_type)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VBLK+block_num) = (csbp|
                                                      blk_type<<4);
}
/*****************************************************************************/
/**
 * \brief   CUP: set VC1 block type
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_SCALE_FACTOR(t_uint16 scl_factor)
{
    HW_CUP_WAIT(CUP_REG_SCLFACT)=scl_factor;
}
/*****************************************************************************/
/**
 * \brief   CUP: get block type for VC1
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_BLOCK_TYPE(t_uint16 block_num)
{
    return (HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VBLK+block_num) >> 4);
}
/*****************************************************************************/
/**
 * \brief   CUP: set VC1 block type(only without csbp)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_ONLY_BLOCK_TYPE(t_uint16 block_num,t_uint16 blk_type)
{
    t_uint16 value;
    value = HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VBLK+block_num) ;
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_VBLK+block_num) = value | blk_type<<4;
}
/*****************************************************************************/
/**
 * \brief   CUP: set VC1 configuration at picture level
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_PICTURE_CFG(t_uint16 vste,t_uint16 frame_type,t_uint16 dc_predictor,t_cup_mvrange_quarter_pix mv_range,t_uint16 fastuvmv,t_uint16 half_pixel)
{
    HW_CUP_WAIT(CUP_REG_PIC)=vste |(frame_type << 1)|(dc_predictor<<4)|(mv_range <<5)|(fastuvmv<<8)|(half_pixel<<9);
}
/*****************************************************************************/
/**
 * \brief   CUP: set DC macroblock quantizer
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_DC_QUANT(t_sint16 dc_step_size)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_DC_QUANT)=dc_step_size;
}
/*****************************************************************************/
/**
 * \brief   CUP: set AC macroblock quantizer
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_AC_QUANT(t_sint16 double_quant)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_CTX_AC_QUANT)=double_quant;
}

/*****************************************************************************/
/**
 * \brief   CUP: set MB parameters in VC1(acpred+skipped)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_MB_PARAMS_NOT_SKIPPED(t_uint16 acpred)
{
    HW_CUP_WAIT(CUP_REG_MBP)=acpred ;
}
/*****************************************************************************/
/**
 * \brief   CUP: set MB parameters in VC1(acpred+skipped)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_MB_PARAMS_NO_ACPRED(t_uint16 is_skipped)
{
    HW_CUP_WAIT(CUP_REG_MBP)=is_skipped<<1 ;
}

/*****************************************************************************/
/**
 * \brief   CUP: get prediction direction if(acpred is enabled)
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_PRED_DIR(void)
{
    return HW_CUP_WAIT(CUP_REG_BLK);
}
/*****************************************************************************/
/**
 * \brief   CUP: Reads register to know if hybrid prediction exists
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static t_uint16 CUP_GET_HYBRID_PRED(void)
{
    return HW_CUP_WAIT(CUP_REG_HYB);
}
/*****************************************************************************/
/**
 * \brief   CUP: if hybrid prediction exists, set the prediction direction 
 * \author  Aroua BEN DARYOUG
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_INTER_PRED_DIRECTION(t_cup_inter_pred pred_dir)
{
    HW_CUP_WAIT(CUP_REG_HYB)=1|pred_dir<<1;
}
/*****************************************************************************/
/**
 *
 *  H264 encode dedicated macro
 * 
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * \brief 	CUP: set MB parameters in H264enc for first MB
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_MB_PARAMS_FIRST_IMC()
{
    HW_CUP_WAIT(CUP_REG_MBP)=1<<2 ;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set MB parameters in H264enc
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_MB_PARAMS_IMC()
{
    HW_CUP_WAIT(CUP_REG_MBP)=0 ;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set mb_type for current MB for context Y
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTY_MBTYPE(t_cup_t8x8 t8x8, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_Y+CUP_E_CTX_MBTYPE) = (t8x8<<6) | value;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set MVX for block for context Y
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTY_MVX(t_uint16 block, t_uint16 mvx)
{
    HW_CUP_WAIT(CUP_CTX_Y+CUP_E_CTX_MVX+4*block) = mvx;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set MVY for block  for context Y
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTY_MVY(t_uint16 block, t_uint16 mvy)
{
    HW_CUP_WAIT(CUP_CTX_Y+CUP_E_CTX_MVY+4*block) = mvy;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set I4x4 direction for block  for context Y
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTY_I4x4(t_uint16 block, t_uint16 direction)
{
    HW_CUP_WAIT(CUP_CTX_Y+CUP_E_CTX_I4X4+2*block) = direction;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set mb_type for current MB for context X
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTX_MBTYPE(t_cup_t8x8 t8x8, t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_E_CTX_MBTYPE) = (t8x8<<6) | value;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set QPy (Luma) for current MB
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTX_QPY(t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_E_CTX_QPY) = value;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set QPCb QPCr (Chroma) for current MB
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTX_QPC(t_uint16 qpc)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_E_CTX_QPC) = qpc;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set slice number for current MB
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTX_NSLICE(t_uint16 value)
{
     HW_CUP_WAIT(CUP_CTX_X+CUP_E_CTX_NSLICE) = value;
}
/*****************************************************************************/
/**
 * \brief 	CUP: set I4x4 direction for block  for context X
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_SET_E_CTX_I4x4(t_uint16 block, t_uint16 direction)
{
    HW_CUP_WAIT(CUP_CTX_X+CUP_E_CTX_I4X4+2*block) = direction;
}
/*****************************************************************************/
/**
 * \brief 	CUP: clean slice number for narrow MB
 * \author 	Rebecca RICHARD
 *  
 */
/*****************************************************************************/
#pragma inline 
static void CUP_CLEAN_NEIGHBOUR_NSLICE()
{
    HW_CUP_WAIT(CUP_CTX_C+CUP_E_CTX_NSLICE) = 0xffffU;
    HW_CUP_WAIT(CUP_CTX_B+CUP_E_CTX_NSLICE) = 0xffffU;
    HW_CUP_WAIT(CUP_CTX_D+CUP_E_CTX_NSLICE) = 0xffffU;
    HW_CUP_WAIT(CUP_CTX_A+CUP_E_CTX_NSLICE) = 0xffffU;
}

/*****************************************************************************/
/**
 * \brief       CUP: get MVDx
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static t_sint16 CUP_GET_MVDX(t_uint16 block)
{
    return (HW_CUP_WAIT(CUP_REG_MVDX+block));
}

/*****************************************************************************/
/**
 * \brief       CUP: get prev/rem for I4x4
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static t_sint16 CUP_GET_I4x4(t_uint16 block)
{
    return (HW_CUP_WAIT(CUP_REG_MVDX+block));
}

/*****************************************************************************/
/**
 * \brief       CUP: get MVDy
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static t_sint16 CUP_GET_MVDY(t_uint16 block)
{
    return (HW_CUP_WAIT(CUP_REG_MVDY+block));
}
/*****************************************************************************/
/**
 * \brief       CUP: get CBP 
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CBP()
{
    return (HW_CUP_WAIT(CUP_REG_CBP));
}

/*****************************************************************************/
/**
 * \brief       CUP: get availibility for IPA 
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_MBA_FOR_IPA()
{
    return ( (HW_CUP_WAIT(CUP_REG_MBA)&0x00F0)>>4);
}
/*****************************************************************************/
/**
 * \brief       CUP: read block type for left MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_A_BLK_TYPE(t_uint16 block_num)
{
    return ( (HW_CUP_WAIT(CUP_CTX_A+CUP_CTX_VBLK+block_num)&0x70)>>4);
}
/*****************************************************************************/
/**
 * \brief       CUP: read csbp for left MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_A_CSBP(t_uint16 block_num)
{
    return ( HW_CUP_WAIT(CUP_CTX_A+CUP_CTX_VBLK+block_num)&0x1);
}
/*****************************************************************************/
/**
 * \brief       CUP: read csbp for top left MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_D_CSBP(t_uint16 block_num)
{
    return ( HW_CUP_WAIT(CUP_CTX_D+CUP_CTX_VBLK+block_num)&0x1);
}
/*****************************************************************************/
/**
 * \brief       CUP: read csbp for top left MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_B_CSBP(t_uint16 block_num)
{
    return ( HW_CUP_WAIT(CUP_CTX_B+CUP_CTX_VBLK+block_num)&0x1);
}
/*****************************************************************************/
/**
 * \brief       CUP: read block type for topleft MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_D_BLK_TYPE(t_uint16 block_num)
{
    return ( (HW_CUP_WAIT(CUP_CTX_D+CUP_CTX_VBLK+block_num)&0x70)>>4);
}
/*****************************************************************************/
/**
 * \brief       CUP: read block type for top MB
 * \author      Aroua BEN DARYOUG
 *
 */
/*****************************************************************************/
#pragma inline
static t_uint16 CUP_GET_CTX_B_BLK_TYPE(t_uint16 block_num)
{
    return ( (HW_CUP_WAIT(CUP_CTX_B+CUP_CTX_VBLK+block_num)&0x70)>>4);
}

/*****************************************************************************/
/**
 * \brief       CUP: get STS status and check not running 
 * \author      Rebecca RICHARD
 *
 */
/*****************************************************************************/
#pragma inline
static void CUP_CHECK_NOT_RUNNING()
{
    while ( ( CUP_GET_REG_STS() &0x3) == CUP_RUNNING);
}


/*****************************************************************************/
/**
 * \brief       CUP: set stabilisations offsets (in pxls) ONLY IN 8500 V1 CUT 2  
 * \author      Rebecca richard (grandvaux)
 * \param       stab_x : in pxls
 * \param       stab_y : in pxls
 *
 *
 */
/*****************************************************************************/


#pragma inline
static void CUP_STAB_OFFSETS(t_uint16 stab_x, t_uint16 stab_y)
{
#ifdef _8500_V2_
    HW_CUP_WAIT(CUP_REG_SOX) = stab_x  ;
    HW_CUP_WAIT(CUP_REG_SOY) = stab_y ;
#else
#endif /*_8500_V2_ */
}
 

#endif /* _CUP_API_H_ */

