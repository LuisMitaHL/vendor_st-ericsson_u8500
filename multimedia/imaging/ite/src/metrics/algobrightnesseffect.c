/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algobrightnesseffect.c
* \brief    Contains Algorithm Library Brightness Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"
#include "algoutilities.h"
#include "algobrightnesseffect.h"

/**
 * Validation Metric for the Brightness Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricBrightnessEffect( const TAlgoImage* aImageEffectDisable, 
					                         const TAlgoImage* aImageEffectEnable,
					                         const TAlgoMetricParams* aMetricParams )
    {
    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;
    TReal64 Intensity_Im1 = 0;
    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;
    TReal64 Intensity_Im2 = 0;
    TReal64 tolerance = 0;
    TAlgoParamBrightness *paramBrightness = (TAlgoParamBrightness*)aMetricParams->iParams;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_2("Brightness Metric : iPrBrightness_1 %d, iPrBrightness_2 %d\n\n", paramBrightness->iPrBrightness_1, paramBrightness->iPrBrightness_2);

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    Intensity_Im1 = (R_Avg_Im1 + G_Avg_Im1 + B_Avg_Im1)/3.255;
    Intensity_Im2 = (R_Avg_Im2 + G_Avg_Im2 + B_Avg_Im2)/3.255;

    ALGO_Log_2("Intensity_Im1 = %10f, Intensity_Im2 %10f\n", Intensity_Im1, Intensity_Im2);

    tolerance = (aMetricParams->iErrorTolerance.iErrorToleranceR * BRIGHTNESS_THRESHOLD)/100;
    ALGO_Log_1("BRIGHTNESS_THRESHOLD = %d\n", BRIGHTNESS_THRESHOLD);
    ALGO_Log_1("(BRIGHTNESS_THRESHOLD - tolerance) = %10f\n", (TReal64)(BRIGHTNESS_THRESHOLD - tolerance));

    if( (ABS(Intensity_Im1 - Intensity_Im2)) > (BRIGHTNESS_THRESHOLD - tolerance))
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
    }
