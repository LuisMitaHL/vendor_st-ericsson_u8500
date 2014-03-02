
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI services header
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _DSI_SERVICES
#define _DSI_SERVICES


#ifndef _HCL_DEFS_H
#include "hcl_defs.h" 
#endif
#include "services.h"

#ifdef	__cplusplus
extern "C" {
#endif 

/*--------------------------------------------------------------------------*
 * type def and structures
 *--------------------------------------------------------------------------*/
    typedef enum
    {
        DSI_MOP500 = 0x0,
        DSI_PEPS   = 0x1
    }t_dsi_platform;

/*--------------------------------------------------------------------------*
 * private type def and structures
 *--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC void SER_DSI_Init(IN t_uint8);
PUBLIC void SER_DSI_Close(void);
PUBLIC void SER_DSI_InterruptHandler(IN t_uint32 irq);
PUBLIC t_dsi_error SER_DSI_Configure(t_dsi_platform platform);
PUBLIC void SER_DSI_TAALDisplayInit(void);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif /* _DSI_SERVICES */

