/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoautofocuseffect.c
* \brief    Contains Algorithm Library Auto Focus Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algodebug.h"
#include "algoautofocuseffect.h"
#include "algoutilities.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"

/**
 * Validation Metric for the Auto Focus Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricAutoFocusEffect( const TAlgoImage* aImageEffectDisable, 
					                        const TAlgoImage* aImageEffectEnable,
					                        const TAlgoMetricParams* aMetricParams )
    {
    TReal64 HorizPixelDiffEffectEnableR = 0;
	//TReal64 HorizPixelDiffEffectEnableG = 0;
	//TReal64 HorizPixelDiffEffectEnableB = 0;

    TReal64 VertPixelDiffEffectEnableR = 0;
	//TReal64 VertPixelDiffEffectEnableG = 0;
	//TReal64 VertPixelDiffEffectEnableB = 0;
 
    TReal64 HorizPixelDiffEffectDisableR = 0;
	//TReal64 HorizPixelDiffEffectDisableG = 0;
	//TReal64 HorizPixelDiffEffectDisableB = 0;

    TReal64 VertPixelDiffEffectDisableR = 0;
	//TReal64 VertPixelDiffEffectDisableG = 0;
	//TReal64 VertPixelDiffEffectDisableB = 0;

    //TReal64 HorizPixelDiffEffectEnableLuminance = 0;
	//TReal64 VertPixelDiffEffectEnableLuminance = 0;
    //TReal64 HorizPixelDiffEffectDisableLuminance = 0;
	//TReal64 VertPixelDiffEffectDisableLuminance = 0;

    //TReal64 HorizPixelDiffEffectEnableLuminanceFullImage = 0;
	//TReal64 VertPixelDiffEffectEnableLuminanceFullImage = 0;
    //TReal64 HorizPixelDiffEffectDisableLuminanceFullImage = 0;
	//TReal64 VertPixelDiffEffectDisableLuminanceFullImage = 0;


    TReal64 tolerance = 0;
    TReal64 threshold = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_2("iPixelsToGrabX = %d, iPixelsToGrabY = %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);
	
/*
    CalculateImageHorizontalDiff( aImageEffectEnable,  
                                  &HorizPixelDiffEffectEnableR, 
                                  &HorizPixelDiffEffectEnableG, 
                                  &HorizPixelDiffEffectEnableB,
                                  &HorizPixelDiffEffectEnableLuminance,
								  &aMetricParams->iTestCoordinates );

    CalculateImageVerticalDiff( aImageEffectEnable,  
                                &VertPixelDiffEffectEnableR, 
                                &VertPixelDiffEffectEnableG, 
                                &VertPixelDiffEffectEnableB,
                                &VertPixelDiffEffectEnableLuminance,
								&aMetricParams->iTestCoordinates );

    CalculateImageHorizontalDiff( aImageEffectDisable,  
                                  &HorizPixelDiffEffectDisableR, 
                                  &HorizPixelDiffEffectDisableG, 
                                  &HorizPixelDiffEffectDisableB,
                                  &HorizPixelDiffEffectDisableLuminance,
								  &aMetricParams->iTestCoordinates );

    CalculateImageVerticalDiff( aImageEffectDisable,  
                                &VertPixelDiffEffectDisableR, 
                                &VertPixelDiffEffectDisableG, 
                                &VertPixelDiffEffectDisableB,
                                &VertPixelDiffEffectDisableLuminance,
								&aMetricParams->iTestCoordinates );

    ALGO_Log_1("HorizPixelDiffEffectEnableLuminance/HorizPixelDiffEffectDisableLuminance = %1.2lf\n\n", (double)(HorizPixelDiffEffectEnableLuminance/HorizPixelDiffEffectDisableLuminance));
    ALGO_Log_1("VertPixelDiffEffectEnableLuminance/VertPixelDiffEffectDisableLuminance = %1.2lf\n\n", (double)(VertPixelDiffEffectEnableLuminance/VertPixelDiffEffectDisableLuminance));

    tolerance = ((aMetricParams->iErrorTolerance.iErrorToleranceR) * (AUTO_FOCUS_THRESHOLD - 1))/100;
    threshold = AUTO_FOCUS_THRESHOLD - tolerance;
	ALGO_Log_1("threshold = %f\n", threshold);

    if(((HorizPixelDiffEffectEnableLuminance/HorizPixelDiffEffectDisableLuminance) > threshold) &&
       ((VertPixelDiffEffectEnableLuminance/VertPixelDiffEffectDisableLuminance) > threshold))
	    {
	    return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
*/

	{
		TAlgoError errorCode = EErrorNone ;
		errorCode = GetLumaOn_R_Channel_And_Normalize(aImageEffectDisable, aImageEffectEnable);
		if(errorCode!= EErrorNone)
		{
			return EErrorNotValidated;
		}
	}
    CalculateImageHorizontalDiff_R( aImageEffectEnable,  
                                  &HorizPixelDiffEffectEnableR, 
								  &aMetricParams->iTestCoordinates );

    CalculateImageVerticalDiff_R( aImageEffectEnable,  
                                &VertPixelDiffEffectEnableR, 
								&aMetricParams->iTestCoordinates );

    CalculateImageHorizontalDiff_R( aImageEffectDisable,  
                                  &HorizPixelDiffEffectDisableR, 
								  &aMetricParams->iTestCoordinates );

    CalculateImageVerticalDiff_R( aImageEffectDisable,  
                                &VertPixelDiffEffectDisableR, 
								&aMetricParams->iTestCoordinates );

    ALGO_Log_1("HorizPixelDiffEffectDisableR = %1.2lf\n\n", (double)(HorizPixelDiffEffectDisableR));
    ALGO_Log_1("VertPixelDiffEffectDisableR = %1.2lf\n\n", (double)(VertPixelDiffEffectDisableR));
    ALGO_Log_1("HorizPixelDiffEffectEnableR = %1.2lf\n\n", (double)(HorizPixelDiffEffectEnableR));
    ALGO_Log_1("VertPixelDiffEffectEnableR = %1.2lf\n\n", (double)(VertPixelDiffEffectEnableR));

    ALGO_Log_1("HorizPixelDiffEffectEnableR/HorizPixelDiffEffectDisableR = %1.2lf\n\n", (double)(HorizPixelDiffEffectEnableR/HorizPixelDiffEffectDisableR));
    ALGO_Log_1("VertPixelDiffEffectEnableR/VertPixelDiffEffectDisableR = %1.2lf\n\n", (double)(VertPixelDiffEffectEnableR/VertPixelDiffEffectDisableR));

    tolerance = ((aMetricParams->iErrorTolerance.iErrorToleranceR) * (AUTO_FOCUS_THRESHOLD - 1))/100;
    threshold = AUTO_FOCUS_THRESHOLD - tolerance;
	ALGO_Log_1("AUTO_FOCUS_THRESHOLD = %f\n", AUTO_FOCUS_THRESHOLD);
	ALGO_Log_1("tolerance_param in percent = %d\n", aMetricParams->iErrorTolerance.iErrorToleranceR);
	ALGO_Log_1("threshold = %f\n", threshold);

    if(((HorizPixelDiffEffectEnableR/HorizPixelDiffEffectDisableR) > threshold) &&
       ((VertPixelDiffEffectEnableR/VertPixelDiffEffectDisableR) > threshold))
	    {
	    return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }

	}
