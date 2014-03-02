/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI IRQ header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _DSI_IRQ_H_
#define _DSI_IRQ_H_

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
typedef enum
{
    DSI_IRQ_TYPE_MCTL_MAIN                   = 0x01,
    DSI_IRQ_TYPE_CMD_MODE                    = 0x02,
    DSI_IRQ_TYPE_DIRECT_CMD_MODE             = 0x03,
    DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE          = 0x04,
    DSI_IRQ_TYPE_VID_MODE                    = 0x05,
    DSI_IRQ_TYPE_TG                          = 0x06,
    DSI_IRQ_TYPE_DPHY_ERROR                  = 0x07,
    DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD            = 0x08
} t_dsi_irq_type;

#define DSI_NO_INTERRUPT   0x0

/*-----------------------------------------------------------------------------
	Events and interrupts management functions
-----------------------------------------------------------------------------*/
PUBLIC void     DSI_SetBaseAddress(t_logical_address base_address);
PUBLIC void     DSI_EnableIRQSrc(t_dsi_irq_type,t_uint32);
PUBLIC void     DSI_DisableIRQSrc(t_dsi_irq_type,t_uint32);
PUBLIC t_uint32 DSI_GetIRQSrc(t_dsi_irq_type);
PUBLIC void     DSI_ClearIRQSrc(t_dsi_irq_type,t_uint32);

#ifdef __cplusplus
}   /* Allow C++ to use this header */
#endif /* __cplusplus              */
#endif /* END _MCDE_IRQ_H_  */


