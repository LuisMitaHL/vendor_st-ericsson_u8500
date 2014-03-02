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

#ifndef __INCLUDE_ALGO_SOLARIZE_EFFECTS_ROUTINES_H__
#define __INCLUDE_ALGO_SOLARIZE_EFFECTS_ROUTINES_H__

#define SOLARIZE_THRESHOLD  3
#define SOLARIZE_CHECK_NEG_THRESHOLD    10
#define SOLARIZE_CHECK_POS_THRESHOLD    5

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"

/**
 * Validation Metric for the Soalrize Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSolarizeEffect( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams );

/**
 * Validation Metric for the Soalrize Effect basic.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSolarizeEffect_basic( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams );

#endif	//__INCLUDE_ALGO_SOLARIZE_EFFECTS_ROUTINES_H__

#ifdef __cplusplus
}
#endif 
