/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides the debug IDs.
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_DBG_H
#define __INC_DBG_H

/*--------------------------------------------------------------------------*
 * Includes                                                                    *
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

#ifdef __DEBUG
#include "logmsg.h"  
#include <stdio.h>
#endif

/*--------------------------------------------------------------------------*
 * C++                                                                       *
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*--------------------------------------------------------------------------*
 * Constants and new types                                                    *
 *--------------------------------------------------------------------------*/

/*Defines for Version */
#define DBG_HCL_VERSION_ID   1
#define DBG_HCL_MAJOR_ID     0
#define DBG_HCL_MINOR_ID     0

 
/* Store a submitter ID, unique for each HCL. */

typedef enum 
{
    UNKNOWN_HCL_DBG_ID,
    APPLI_DBG_ID,
    TEST_DBG_ID,
    DEBUG_HCL_DBG_ID,
    UART_HCL_DBG_ID,
    GIC_HCL_DBG_ID,
    DMA_HCL_DBG_ID,
    TMR_HCL_DBG_ID,
    GPIO_HCL_DBG_ID,
    PCRMU_HCL_DBG_ID,
    MMCSD_HCL_DBG_ID,        
    SSP_HCL_DBG_ID,
    PRCC_HCL_DBG_ID,
    USB_HCL_DBG_ID,
    AB8500_CODEC_HCL_DBG_ID,
    AB8500_TVOUT_HCL_DBG_ID,
    AB8500_CORE_HCL_DBG_ID
} t_dbg_id;


/* Define the debug levels. */

#define DEBUG_LEVEL0 DBGL_OFF
#define DEBUG_LEVEL1 ((t_uint32)DBGL_PUBLIC_FUNC_IN|(t_uint32)DBGL_PUBLIC_FUNC_OUT|(t_uint32)DBGL_ERROR|(t_uint32)DBGL_WARNING)
#define DEBUG_LEVEL2 ((t_uint32)DBGL_IN_ARGS|(t_uint32)DBGL_OUT_ARGS|(t_uint32)DBGL_RET_CODE)
#define DEBUG_LEVEL3 DBGL_INTERNAL
#define DEBUG_LEVEL4 DBGL_HCL_DEV


typedef enum 
{
 DBGL_OFF               = 0,
 DBGL_PUBLIC_FUNC_IN    = MASK_BIT0,
 DBGL_PUBLIC_FUNC_OUT   = MASK_BIT1,
 DBGL_ERROR             = MASK_BIT2,
 DBGL_WARNING           = MASK_BIT3,
 DBGL_IN_ARGS           = MASK_BIT4,
 DBGL_OUT_ARGS          = MASK_BIT5,
 DBGL_RET_CODE          = MASK_BIT6,
 DBGL_INTERNAL          = MASK_BIT7,
 DBGL_HCL_DEV           = MASK_BIT8,
 DBGL_PRIV_FUNC_IN      = MASK_BIT9,
 DBGL_PRIV_FUNC_OUT     = MASK_BIT10,
 DBGL_PRIV_IN_ARGS      = MASK_BIT11,
 DBGL_PRIV_OUT_ARGS     = MASK_BIT12,
 DBGL_USER_1            = MASK_BIT13,
 DBGL_USER_2            = MASK_BIT14,
 DBGL_USER_3            = MASK_BIT15,
 DBGL_USER_4            = MASK_BIT16,
 DBGL_USER_5            = MASK_BIT17,
 DBGL_USER_6            = MASK_BIT18,
 DBGL_USER_7            = MASK_BIT19,
 DBGL_USER_8            = MASK_BIT20,
 DBGL_USER_9            = MASK_BIT21,
 DBGL_RESERVED_0        = MASK_BIT22,
 DBGL_RESERVED_1        = MASK_BIT23,
 DBGL_RESERVED_2        = MASK_BIT24,
 DBGL_RESERVED_3        = MASK_BIT25,
 DBGL_RESERVED_4        = MASK_BIT26,
 DBGL_RESERVED_5        = MASK_BIT27,
 DBGL_RESERVED_6        = MASK_BIT28,
 DBGL_RESERVED_7        = MASK_BIT29,
 DBGL_RESERVED_8        = MASK_BIT30
} t_dbg_level;


 
#ifdef __DEBUG  
 
/*--------------------------------------------------------------------------*
 * Macro                                                                    *
 *--------------------------------------------------------------------------*/
 
/* Begin of Private definitions */

/* Compiler define __ARMCC_VERSION returns PVtbbb where:
 * P is the major version (1 for ADS and 2/3/4 for RVCT e.g v2.1)
 * V is the minor version
 * t is the patch release
 * bbb is the build*/

#if ((__ARMCC_VERSION >= 100000) && (__ARMCC_VERSION < 200000))
/* ADS Compiler */
#define DBGFUNCNAME __func__

#elif ( (__ARMCC_VERSION >= 200000) && (__ARMCC_VERSION < 500000) )
/* RVCT Compiler */
#define DBGFUNCNAME __func__

#else
/* To be added - depends on the compiler to be used. Currently is left as empty */
#define DBGFUNCNAME ""

#endif
/* End of Private definitions */

#endif /* __DEBUG */


#ifdef __RELEASE

#define DBGEXIT(cr)  
#define DBGEXIT0(cr) 
#define DBGEXIT1(cr,ch,p1) 
#define DBGEXIT2(cr,ch,p1,p2) 
#define DBGEXIT3(cr,ch,p1,p2,p3) 
#define DBGEXIT4(cr,ch,p1,p2,p3,p4) 
#define DBGEXIT5(cr,ch,p1,p2,p3,p4,p5)  
#define DBGEXIT6(cr,ch,p1,p2,p3,p4,p5,p6) 

#define DBGENTER() 
#define DBGENTER0()
#define DBGENTER1(ch,p1)                
#define DBGENTER2(ch,p1,p2)                
#define DBGENTER3(ch,p1,p2,p3)            
#define DBGENTER4(ch,p1,p2,p3,p4)        
#define DBGENTER5(ch,p1,p2,p3,p4,p5)    
#define DBGENTER6(ch,p1,p2,p3,p4,p5,p6)    

#define DBGPRINT(dbg_level,dbg_string)          
#define DBGPRINTHEX(dbg_level,dbg_string,uint32) 
#define DBGPRINTDEC(dbg_level,dbg_string,uint32) 

#endif /* __RELEASE  */


/*--------------------------------------------------------------------------*
 * C++                                                                       *
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */


#endif /* __INC_DBG_H */

/* End of file - debug.h */
