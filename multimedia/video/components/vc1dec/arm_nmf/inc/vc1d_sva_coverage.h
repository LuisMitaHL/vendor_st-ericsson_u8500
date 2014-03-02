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
#include "vc1d_sva.h"
/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/
#ifndef _VC1D_SVA_COVERAGE_H_
#define _VC1D_SVA_COVERAGE_H_


#ifdef COVERAGE
#define COV_LOG_PICTURE(p_pic_param)  cov_log_picture_param(p_pic_param)

void cov_log_picture_param(tps_picture_parameters p_pic_param);


#else

#define COV_LOG_PICTURE(p_pic_param)


#endif


#endif /* _VC1D_SVA_COVERAGE_H_ */
