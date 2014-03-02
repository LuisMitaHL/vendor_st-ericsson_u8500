/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file HSI services
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HSI_SERVICES_H_
#define _HSI_SERVICES_H_

#include "hcl_defs.h"
#include "gic.h"
#include "hsi.h"
#include "services.h"

/* Defines */
#define SER_HSI_TX_INDEX		HSI_DEVICE_ID_TX
#define SER_HSI_RX_INDEX		HSI_DEVICE_ID_RX
#define SER_NO_OF_CONTROLLERS	0x02

#define SER_HSI_TX_LINE			GIC_HSI_TX_D0_LINE
#define SER_HSI_RX_LINE			GIC_HSI_RX_D0_LINE

#define HSIT_DIVISOR    		1
#define HSIT_IDLEHOLD   		1
#define HSIR_TIMEOUT    		0


#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))

#define SER_HSIT_PRCMU_OFFSET  0x00000050
#define SER_HSIT_PRCMU_ENABLE  0x00000146
#define SER_HSIR_PRCMU_OFFSET  0x00000054
#define SER_HSIR_PRCMU_ENABLE  0x00000123

#define SER_HSI_PRCC_PCKEN_ENABLE 0x00000600
#define SER_HSI_PRCC_KCKEN_OFFSET 0x00000008
#define SER_HSI_PRCC_KCKEN_ENABLE 0x000000C0
#endif
 
/* Typedefs */

typedef struct
{
	t_hsi_event	event;
	
} t_ser_hsi_param;


/* Function Prototypes */
PUBLIC void SER_HSI_WaitEnd(IN t_hsi_device_id, t_uint32 );
PUBLIC t_ser_error SER_HSI_RegisterCallback (IN t_hsi_device_id,IN t_callback_fct ,IN void *);
PUBLIC void SER_HSI_InstallDefaultHandler(IN t_hsi_device_id device_id);                                     

#endif /* _HSI_SERVICES */

