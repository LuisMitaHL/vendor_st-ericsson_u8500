/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides header file for PWL services
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#ifndef _SERVICES_PWL_
#define _SERVICES_PWL_
#include "hcl_defs.h"
#include "pwl.h"


#if((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_HREFV2)||(defined ST_8500V2))
#define PRCC_KERNEL_CLK_EN_OFFSET 0x8

#define PWL_AMBA_CLK_EN_VAL      0x00000008
#define PWL_KERNEL_CLK_EN_VAL    0x00000002


#endif


PUBLIC void         SER_PWL_Init(t_uint8 default_ser_mask);
PUBLIC void         SER_PWL_Close(void);
PUBLIC t_ser_error  SER_PWL_ConfigureDefault(void);
#endif /* END OF FILE*/

