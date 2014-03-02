/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE IRQ header file
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _MCDE_IRQ_H_
#define _MCDE_IRQ_H_

#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#ifdef __cplusplus
extern "C"
{   /* In case C++ needs to use this header.*/
#endif

/*-----------------------------------------------------------------------------
	Typedefs
-----------------------------------------------------------------------------*/
#ifdef ST_8500ED
typedef enum
{
    MCDE_IRQ_SRC_FIFO_A_UNDERFLOW            = 0x01,
    MCDE_IRQ_SRC_FIFO_B_UNDERFLOW            = 0x02,
    MCDE_IRQ_SRC_CHANNEL_A_VERTICAL_COMPARE  = 0x04,
    MCDE_IRQ_SRC_CHANNEL_B_VERTICAL_COMPARE  = 0x08,
    MCDE_IRQ_SRC_CHANNEL_C0_VERTICAL_SYNC    = 0x10,
    MCDE_IRQ_SRC_CHANNEL_C1_VERTICAL_SYNC    = 0x20,
    MCDE_IRQ_SRC_CHANNEL_C0_VERTICAL_COMPARE = 0x40,
    MCDE_IRQ_SRC_CHANNEL_C1_VERTICAL_COMPARE = 0x80,
    MCDE_IRQ_SRC_SCHEDULER_BLOCKED           = 0x200,
    MCDE_IRQ_SRC_ROTATION_FETCH_DONE_A       = 0x400,
    MCDE_IRQ_SRC_ROTATION_FETCH_DONE_B       = 0x800,
    MCDE_IRQ_SRC_CHANNEL_READ_DONE           = 0x1000,
    MCDE_IRQ_SRC_CHANNEL_ABORT               = 0x2000,
    MCDE_IRQ_SRC_OVERLAY_READ_DONE           = 0x4000,
    MCDE_IRQ_SRC_OVERLAY_FETCH_DONE          = 0x8000,
    MCDE_IRQ_SRC_OVERLAY0_FETCH_ERROR        = 0x10000,
    MCDE_IRQ_SRC_OVERLAY1_FETCH_ERROR        = 0x20000,
    MCDE_IRQ_SRC_OVERLAY2_FETCH_ERROR        = 0x40000,
    MCDE_IRQ_SRC_OVERLAY3_FETCH_ERROR        = 0x80000,
    MCDE_IRQ_SRC_OVERLAY4_FETCH_ERROR        = 0x100000,
    MCDE_IRQ_SRC_OVERLAY5_FETCH_ERROR        = 0x200000,
    MCDE_IRQ_SRC_OVERLAY6_FETCH_ERROR        = 0x400000,
    MCDE_IRQ_SRC_OVERLAY7_FETCH_ERROR        = 0x800000,
    MCDE_IRQ_SRC_OVERLAY8_FETCH_ERROR        = 0x1000000,
    MCDE_IRQ_SRC_OVERLAY9_FETCH_ERROR        = 0x2000000,
    MCDE_IRQ_SRC_OVERLAY10_FETCH_ERROR        = 0x4000000,
    MCDE_IRQ_SRC_OVERLAY11_FETCH_ERROR        = 0x8000000,
    MCDE_IRQ_SRC_OVERLAY12_FETCH_ERROR        = 0x10000000,
    MCDE_IRQ_SRC_OVERLAY13_FETCH_ERROR        = 0x20000000,
    MCDE_IRQ_SRC_OVERLAY14_FETCH_ERROR        = 0x40000000,
    MCDE_IRQ_SRC_OVERLAY15_FETCH_ERROR        = (int)0x80000000,
} t_mcde_irq_src;

#else

typedef enum
{
	MCDE_IRQ_SRC_CHANNEL_A_VERTICAL_COMPARE  = 0x01,
	MCDE_IRQ_SRC_CHANNEL_B_VERTICAL_COMPARE  = 0x02,
	MCDE_IRQ_SRC_CHANNEL_C0_VERTICAL_SYNC    = 0x04,
	MCDE_IRQ_SRC_CHANNEL_C1_VERTICAL_SYNC    = 0x08,
	MCDE_IRQ_SRC_CHANNEL_C0_VERTICAL_COMPARE = 0x10,
	MCDE_IRQ_SRC_CHANNEL_C1_VERTICAL_COMPARE = 0x20,
	MCDE_IRQ_SRC_ROTATION_FETCH_DONE_A       = 0x40,
	MCDE_IRQ_SRC_ROTATION_FETCH_DONE_B       = 0x80,
	MCDE_IRQ_SRC_FIFO_A_UNDERFLOW            = 0x100,
    MCDE_IRQ_SRC_FIFO_B_UNDERFLOW            = 0x200,       
    MCDE_IRQ_SRC_SCHEDULER_BLOCKED           = 0x400,
    MCDE_IRQ_SRC_ROTATION_FETCH_ERROR_READ_A = 0x800,
    MCDE_IRQ_SRC_ROTATION_FETCH_ERROR_WRITE_A = 0x1000,
    MCDE_IRQ_SRC_ROTATION_FETCH_ERROR_READ_B = 0x2000,
    MCDE_IRQ_SRC_ROTATION_FETCH_ERROR_WRITE_B = 0x4000,
    MCDE_IRQ_SRC_FIFO_C0_UNDERFLOW            = 0x8000,
    MCDE_IRQ_SRC_FIFO_C1_UNDERFLOW            = 0x10000,
    MCDE_IRQ_SRC_OVERLAY0_FETCH_ERROR        = 0x100000,
    MCDE_IRQ_SRC_OVERLAY1_FETCH_ERROR        = 0x200000,
    MCDE_IRQ_SRC_OVERLAY2_FETCH_ERROR        = 0x400000,
    MCDE_IRQ_SRC_OVERLAY3_FETCH_ERROR        = 0x800000,
} t_mcde_irq_src;
#endif

typedef enum
{
	MCDE_IRQ_TYPE_PIXELPROCESSING,
	MCDE_IRQ_TYPE_OVERLAY,
	MCDE_IRQ_TYPE_CHANNEL,
	MCDE_IRQ_TYPE_ERROR
}t_mcde_irq_type;

/*-----------------------------------------------------------------------------
	Events and interrupts management functions
-----------------------------------------------------------------------------*/
PUBLIC void     MCDE_SetBaseAddress(t_logical_address base_address);
#ifdef ST_8500ED
PUBLIC void     MCDE_EnableIRQSrc(t_uint32);
PUBLIC void     MCDE_DisableIRQSrc(t_uint32);
PUBLIC t_uint32 MCDE_GetIRQSrc(void);
PUBLIC void     MCDE_ClearIRQSrc(t_uint32);
#else
PUBLIC void     MCDE_EnableIRQSrc(t_mcde_irq_type,t_uint32);
PUBLIC void     MCDE_DisableIRQSrc(t_mcde_irq_type,t_uint32);
PUBLIC t_uint32 MCDE_GetIRQSrc(t_mcde_irq_type);
PUBLIC void     MCDE_ClearIRQSrc(t_mcde_irq_type,t_uint32);
#endif
PUBLIC t_bool   MCDE_IsPendingIRQSrc(t_uint32);

#ifdef __cplusplus
}   /* Allow C++ to use this header */
#endif /* __cplusplus              */
#endif /* END _MCDE_IRQ_H_  */


