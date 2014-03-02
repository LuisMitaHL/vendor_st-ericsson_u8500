/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INCLUDE_ALGO_TESTING_MODE_H__
#define __INCLUDE_ALGO_TESTING_MODE_H__

#include "algotypedefs.h"
#include "algoerror.h"


#ifdef __cplusplus
extern "C"
{
#endif

TAlgoError ValidationMetricImageContent( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams );

TAlgoError ValidationMetricImageContent_AllocationOptimized( TAlgoImageParams* aImageParams,
									                    TAny* bufferEffectDisable, 
					                                    TAny* bufferEffectEnable,
					                             TAlgoMetricParams* aMetricParams );

#ifdef __cplusplus
}
#endif


#endif // __INCLUDE_ALGO_TESTING_MODE_H__
