
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE Services Header File
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _MCDE_SERVICES
#define _MCDE_SERVICES


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
      MCDE_PANEL_VGA = 0x0,
      MCDE_PANEL_QVGA = 0x01
  }t_mcde_lcd_panel;

/*--------------------------------------------------------------------------*
 * private type def and structures
 *--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC void SER_MCDE_Init(IN t_uint8);
PUBLIC void SER_MCDE_Close(void);
PUBLIC void SER_MCDE_InterruptHandler(IN t_uint32 irq);
PUBLIC t_mcde_error SER_MCDE_DefaultConfigureUIB(t_mcde_lcd_panel panel,t_mcde_bpp_ctrl bpp,t_uint32 buffer_address);
PUBLIC t_mcde_error SER_MCDE_LCDDefaultConfigure(t_mcde_lcd_panel panel,t_mcde_bpp_ctrl bpp,t_uint32 buffer_address);
PUBLIC void SER_MCDE_LCDRun(void);
PUBLIC void SER_MCDE_SmartPanel_Init(void);


#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif /* _DIF_SERVICES */

