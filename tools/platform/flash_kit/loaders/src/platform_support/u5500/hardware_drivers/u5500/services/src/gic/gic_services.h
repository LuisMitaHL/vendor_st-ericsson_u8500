/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file GIC services
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _GIC_SERVICES_H_
#define _GIC_SERVICES_H_

#include "hcl_defs.h"
#include "services.h"

/* Function Prototypes */
void SER_GIC_IntHandlerIRQ (void);
void SER_DummyHandler(void);

PUBLIC void SER_GIC_DefineStartFinishIRQ(t_gic_func_ptr start, t_gic_func_ptr finish);
#endif

/*end of _GIC_SERIVCES_H_  */

