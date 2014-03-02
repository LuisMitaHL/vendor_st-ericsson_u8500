/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_NEGATIVE_EFFECTS_ROUTINES_H__
#define __INCLUDE_ALGO_NEGATIVE_EFFECTS_ROUTINES_H__

#define NEGATIVE_THRESHOLD   15

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"


#ifdef __cplusplus
extern "C"
{
#endif 

/**
 * Validation Metric for the Negative Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricNegativeEffect( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams );

#ifdef __cplusplus
}
#endif 


#endif	//__INCLUDE_ALGO_NEGATIVE_EFFECTS_ROUTINES_H__

