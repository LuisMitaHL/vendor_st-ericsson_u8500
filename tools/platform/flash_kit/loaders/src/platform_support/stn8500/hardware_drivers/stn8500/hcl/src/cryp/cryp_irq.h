/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides prototype for CRYP Public functions and structrue
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _CRYP_IRQ_H_
#define _CRYP_IRQ_H_
    
#include "debug.h"

#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif

/* Cryp Device ID */
typedef enum
{
    CRYP_DEVICE_ID_0 = 0,
    CRYP_DEVICE_ID_1  
} t_cryp_device_id;

typedef enum 
{
    CRYP_IRQ_SRC_INPUT_FIFO     = 0x1,
    CRYP_IRQ_SRC_OUTPUT_FIFO	= 0x2,
    CRYP_IRQ_SRC_ALL            = 0x3
}t_cryp_irq_src_id;


typedef t_uint32 t_cryp_irq_src;
/*-----------------------------------------------------------------------------
    M0 Funtions.
-----------------------------------------------------------------------------*/
PUBLIC  void	            CRYP_SetBaseAddress (IN t_cryp_device_id, IN t_logical_address);
PUBLIC	void		        CRYP_EnableIRQSrc   (IN t_cryp_device_id, IN t_cryp_irq_src);
PUBLIC	void		        CRYP_DisableIRQSrc  (IN t_cryp_device_id, IN t_cryp_irq_src);
PUBLIC	t_cryp_irq_src      CRYP_GetIRQSrc	    (IN t_cryp_device_id);
PUBLIC	t_bool		        CRYP_IsPendingIRQSrc(IN t_cryp_device_id, IN t_cryp_irq_src);
								     	

#ifdef __cplusplus
} /*Allow C++ to use this header */
#endif  /* __cplusplus   */

#endif	/* _CRYP_IRQ_H_*/
