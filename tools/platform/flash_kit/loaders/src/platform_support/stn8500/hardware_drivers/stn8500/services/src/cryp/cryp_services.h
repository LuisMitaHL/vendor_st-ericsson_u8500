/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Header files for CRYP Service initialization
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _SERVICES_CRYP_H_
#define _SERVICES_CRYP_H_

#include "memory_mapping.h"
#include "cryp.h"
#include "gic.h"
#include "services.h"

/*-----------------------------------------------------------------------
	Struct and Enum
------------------------------------------------------------------------*/
/* Typedefs */
typedef struct
{
    t_cryp_irq_src  irq_src;
    t_cryp_status   cryp_status;
    t_cryp_device_id cryp_device_id;
} t_ser_cryp_param;
PUBLIC void SER_CRYP0_InterruptHandler(t_uint32);
PUBLIC void SER_CRYP1_InterruptHandler(t_uint32);
PUBLIC int SER_CRYP_RegisterCallback(t_callback_fct , void *);


#endif /* End of File */
