/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */
#ifdef __cplusplus
extern "C"
{
#endif 

#ifndef __INCLUDE_ALGO_LUMINANCE_MEAN_ROUTINES_H__
#define __INCLUDE_ALGO_LUMINANCE_MEAN_ROUTINES_H__


/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"

/**
 * Validation Metric for the Luminance Mean.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricLuminanceMean( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams );

#endif	//__INCLUDE_ALGO_LUMINANCE_MEAN_ROUTINES_H__

#ifdef __cplusplus
}
#endif 
