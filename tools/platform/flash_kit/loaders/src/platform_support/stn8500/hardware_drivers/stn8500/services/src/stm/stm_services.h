/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file STM Service initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _SERVICES_STM
#define _SERVICES_STM
#include "stm.h"
#include "services.h"


#define STM_CPU_REG_BASE_ADDR    0x80100000
#define STM_CTRL_REG_BASE_ADDR 	 0x8010F000
#define STM_END_CHANNEL          250
#define STM_ACTIVE_CHANNEL       224
#define MAX_DEBUG_STR            384


/*Function Prototypes */
PUBLIC void SER_STM_Init(t_uint8);
PUBLIC t_stm_error SER_STM_Send8(t_uint8);
PUBLIC t_stm_error SER_STM_initialize(void);
PUBLIC t_stm_error SER_STM_SendString(IN t_uint32,IN t_uint8 *);
PUBLIC t_stm_error SER_STM_SendStringMsg(t_uint8 *);


#endif /*End of file */



