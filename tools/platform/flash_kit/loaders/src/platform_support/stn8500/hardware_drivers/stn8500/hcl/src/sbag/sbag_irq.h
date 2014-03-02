/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the SBAG
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SBAG_IRQ_H_
#define _SBAG_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

    /*--------------------------------------------------------------------------*
 * Data types																*
 *--------------------------------------------------------------------------*/
    typedef enum
    {
        SBAG_WPSAT  = 0,
        SBAG_PI     = 1,
        SBAG_TMSAT  = 2
    } t_sbag_device;

    /*-------------------------------------------------------------------------*
  *					Interrupts Definition									*
  *-------------------------------------------------------------------------*/
    typedef enum
    {
        /* WPSAT Message Interrupts */
        SBAG_WPSAT0_MSG_AVAILABLE   = 0x00001,
        SBAG_WPSAT1_MSG_AVAILABLE   = 0x00002,
        SBAG_WPSAT2_MSG_AVAILABLE   = 0x00004,
        SBAG_WPSAT3_MSG_AVAILABLE   = 0x00008,
        SBAG_WPSAT4_MSG_AVAILABLE   = 0x00010,
        SBAG_WPSAT5_MSG_AVAILABLE   = 0x00020,
        SBAG_WPSAT6_MSG_AVAILABLE   = 0x00040,
        SBAG_WPSAT7_MSG_AVAILABLE   = 0x00080,
        SBAG_WPSAT8_MSG_AVAILABLE   = 0x00100,
        SBAG_WPSAT9_MSG_AVAILABLE   = 0x00200,
        SBAG_WPSAT10_MSG_AVAILABLE  = 0x00400,
        SBAG_WPSAT11_MSG_AVAILABLE  = 0x00800,
        SBAG_WPSAT12_MSG_AVAILABLE  = 0x01000,
        SBAG_WPSAT13_MSG_AVAILABLE  = 0x02000,
        SBAG_WPSAT14_MSG_AVAILABLE  = 0x04000,
        SBAG_WPSAT15_MSG_AVAILABLE  = 0x08000,
        SBAG_WPSAT_IRQ_SRC_ALL      = 0x0FFFF,

        /* PI Message Interrupts */
        SBAG_PI0_MSG_AVAILABLE      = 0x10001,
        SBAG_PI1_MSG_AVAILABLE      = 0x10002,
        SBAG_PI2_MSG_AVAILABLE      = 0x10004,
        SBAG_PI3_MSG_AVAILABLE      = 0x10008,
        SBAG_PI4_MSG_AVAILABLE      = 0x10010,
        SBAG_PI5_MSG_AVAILABLE      = 0x10020,
        SBAG_PI6_MSG_AVAILABLE      = 0x10040,
        SBAG_PI7_MSG_AVAILABLE      = 0x10080,
        SBAG_PI8_MSG_AVAILABLE      = 0x10100,
        SBAG_PI9_MSG_AVAILABLE      = 0x10200,
        SBAG_PI10_MSG_AVAILABLE     = 0x10400,
        SBAG_PI11_MSG_AVAILABLE     = 0x10800,
        SBAG_PI12_MSG_AVAILABLE     = 0x11000,
        SBAG_PI13_MSG_AVAILABLE     = 0x12000,
        SBAG_PI14_MSG_AVAILABLE     = 0x14000,
        SBAG_PI15_MSG_AVAILABLE     = 0x18000,
        SBAG_PI_IRQ_SRC_ALL         = 0x1FFFF,

        /* TMSAT Message Interrupts */
        SBAG_TMSAT0_MSG_AVAILABLE   = 0x20001,
        SBAG_TMSAT1_MSG_AVAILABLE   = 0x20002,
        SBAG_TMSAT2_MSG_AVAILABLE   = 0x20004,
        SBAG_TMSAT3_MSG_AVAILABLE   = 0x20008,
        SBAG_TMSAT4_MSG_AVAILABLE   = 0x20010,
        SBAG_TMSAT5_MSG_AVAILABLE   = 0x20020,
        SBAG_TMSAT6_MSG_AVAILABLE   = 0x20040,
        SBAG_TMSAT7_MSG_AVAILABLE   = 0x20080,
        SBAG_TMSAT8_MSG_AVAILABLE   = 0x20100,
        SBAG_TMSAT9_MSG_AVAILABLE   = 0x20200,
        SBAG_TMSAT10_MSG_AVAILABLE  = 0x20400,
        SBAG_TMSAT11_MSG_AVAILABLE  = 0x20800,
        SBAG_TMSAT12_MSG_AVAILABLE  = 0x21000,
        SBAG_TMSAT13_MSG_AVAILABLE  = 0x22000,
        SBAG_TMSAT14_MSG_AVAILABLE  = 0x24000,
        SBAG_TMSAT15_MSG_AVAILABLE  = 0x28000,
        SBAG_TMSAT_IRQ_SRC_ALL      = 0x2FFFF,
    } t_sbag_irq_src_id;

    typedef t_uint32        t_sbag_irq_src;

    /*--------------------------------------------------------------------------*
 * Functions declaration													*
 *--------------------------------------------------------------------------*/
    /* Initialization functions *
 *--------------------------*/
    PUBLIC void             SBAG_SetBaseAddress(IN t_logical_address sbag_base_address);

    /* Interrupt Management	* 
 *----------------------*/
    PUBLIC t_sbag_device    SBAG_GetDeviceId(IN t_sbag_irq_src sbag_irq_src);
    PUBLIC void             SBAG_EnableIRQSrc(IN t_sbag_irq_src sbag_irq_src);
    PUBLIC void             SBAG_DisableIRQSrc(IN t_sbag_irq_src sbag_it_to_disable);
    PUBLIC t_sbag_irq_src   SBAG_GetIRQSrc(IN t_sbag_device sbag_device);
    PUBLIC void             SBAG_ClearIRQSrc(IN t_sbag_irq_src sbag_it_to_clear);
    PUBLIC t_bool           SBAG_IsPendingIRQSrc(t_sbag_irq_src sbag_pending_its);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _MSP_IRQ_H_ */

/*End of sbag_irq.h File*/
