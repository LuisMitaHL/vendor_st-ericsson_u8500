/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algonegativeeffect.c
* \brief    Contains Algorithm Library Negative Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algonegativeeffect.h"
#include "algoutilities.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"

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
					                       const TAlgoMetricParams* aMetricParams )
    {
#if 1
    TUint32 i = 0;
    TUint32 j = 0;
    TUint32 index = 0;
    TUint32 deviation = 0;
    TReal64 tolerance = 0;
    TReal64 deltaR = 0;
    TReal64 deltaG = 0;
    TReal64 deltaB = 0;

    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                            + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
    index = startingOffset;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            deltaR = ABS((255 - aImageEffectEnable->iImageData.iRChannel[index]) - (aImageEffectDisable->iImageData.iRChannel[index]));
            deltaG = ABS((255 - aImageEffectEnable->iImageData.iGChannel[index]) - (aImageEffectDisable->iImageData.iGChannel[index]));
            deltaB = ABS((255 - aImageEffectEnable->iImageData.iBChannel[index]) - (aImageEffectDisable->iImageData.iBChannel[index]));
            if((deltaR > NEGATIVE_THRESHOLD) ||
               (deltaG > NEGATIVE_THRESHOLD) ||
               (deltaB > NEGATIVE_THRESHOLD))
                {
                deviation++;
                }
            index++;
            }
        index = index + offset;
        }

    tolerance = ( aMetricParams->iTestCoordinates.iPixelsToGrabX *
                  aMetricParams->iTestCoordinates.iPixelsToGrabY *
                  aMetricParams->iErrorTolerance.iErrorToleranceR ) / 100;
    
    ALGO_Log_2("deviation = %d, Tolerance = %f\n", deviation, tolerance);

    if(deviation <= (tolerance))
        {
        return EErrorNone;
        }
    else
        {
	    return EErrorNotValidated;
        }
#else
    //TUint32 loop = 0;

    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;

 	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    if(((R_Avg_Im1 < (aImageEffectDisable->iImageParams.iBitMaxR/2 - aMetricParams->iMetricRange.iRangeR)) || (R_Avg_Im1 > (aImageEffectDisable->iImageParams.iBitMaxR/2 + aMetricParams->iMetricRange.iRangeR))) && 
       ((G_Avg_Im1 < (aImageEffectDisable->iImageParams.iBitMaxG/2 - aMetricParams->iMetricRange.iRangeG)) || (G_Avg_Im1 > (aImageEffectDisable->iImageParams.iBitMaxG/2 + aMetricParams->iMetricRange.iRangeG))) &&
       ((B_Avg_Im1 < (aImageEffectDisable->iImageParams.iBitMaxB/2 - aMetricParams->iMetricRange.iRangeB)) || (B_Avg_Im1 > (aImageEffectDisable->iImageParams.iBitMaxB/2 + aMetricParams->iMetricRange.iRangeB))))
        {
        if((ABS(R_Avg_Im1 - (aImageEffectDisable->iImageParams.iBitMaxR - R_Avg_Im2)) <= aMetricParams->iErrorTolerance.iErrorToleranceR ) &&
           (ABS(G_Avg_Im1 - (aImageEffectDisable->iImageParams.iBitMaxG - G_Avg_Im2)) <= aMetricParams->iErrorTolerance.iErrorToleranceG ) &&
           (ABS(B_Avg_Im1 - (aImageEffectDisable->iImageParams.iBitMaxB - B_Avg_Im2)) <= aMetricParams->iErrorTolerance.iErrorToleranceB ))
        //if((R_Avg_Im1 == 31 - R_Avg_Im2) & (G_Avg_Im1 == 63 - G_Avg_Im2)  &  (B_Avg_Im1 == 31 - B_Avg_Im2))
            {
		    return EErrorNone;
            }
        else
            {
		    return EErrorNotValidated;
            }
        
        }
    else
        {
        return EErrorImageNotRelevant;
        }
#endif    
    }

