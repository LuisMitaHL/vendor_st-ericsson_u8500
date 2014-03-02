/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/
#ifndef _VC1D_CO_COVERAGE_H_
#define _VC1D_CO_COVERAGE_H_

#ifdef COVERAGE

#define COV(entry) cov_log_standard_entry((entry))
#define COV_FLUSH()      cov_flush()

void cov_log_standard_entry(char *entry);
void cov_flush(void);


#else

#define COV(entry)
#define COV_FLUSH()
#endif


#endif /* _VC1D_CO_COVERAGE_H_ */
