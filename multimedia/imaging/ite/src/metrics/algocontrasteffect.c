/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 /*
 * \file     algocontrasteffect.c
 * \brief    Contains Algorithm Library Contrast Effect validation functions
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
#include "algocontrasteffect.h"

/**
 * Validation Metric for the Contrast Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricContrastEffect( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams )
    {
#if 1
    TUint32 index = 0;
    TUint32 x = 0;
    TUint32 y = 0;
    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);
    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    //TInt32 DiffInput = 0, DiffOutput = 0; // don't use TInt32 but TReal64 as for big images we can have overflow
    TReal64 DiffInput = 0, DiffOutput = 0;
    TReal64 SDInput = 0, SDOutput = 0, SDDiff = 0;
    TReal64 tolerance = 0;
	TUint32 mean_input = 0, mean_output = 0;
    TAlgoParamContrast *paramContrast = (TAlgoParamContrast*)aMetricParams->iParams;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_2("Contrast Metric : iPrContrast_1 %d, iPrContrast_2 %d\n\n", paramContrast->iPrContrast_1, paramContrast->iPrContrast_2);

    for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * 
         aImageEffectDisable->iImageParams.iImageWidth); index++ )
        {
        aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);

        aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
        }

    //ALGO_Log_2("LumaInputMean = %10d, LumaOutputMean = %10d\n", InputMean, OutputMean);

    mean_input = 0;
	mean_output = 0;
	iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
    {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		{
            mean_input +=  *iR;
            mean_output += *oR;
            iR++;		
			oR++;				
        }
	    iR += offset;		
		oR += offset;
    }
    mean_input = mean_input /(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    mean_output = mean_output /(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);

    ALGO_Log_2("mean_input = %d, mean_output = %d\n", mean_input, mean_output);

    iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		    {
            DiffInput = DiffInput + ((*iR - mean_input) * (*iR - mean_input));
            DiffOutput = DiffOutput + ((*oR - mean_output) * (*oR - mean_output));

            iR++;		
			oR++;				
            }
	    iR += offset;		
		oR += offset;
        }
    SDInput = SQUAREROOT(DiffInput/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY));
    SDOutput = SQUAREROOT(DiffOutput/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY));

    ALGO_Log_2("SDInput = %10f, SDOutput = %10f\n", SDInput, SDOutput);

    tolerance = (aMetricParams->iErrorTolerance.iErrorToleranceR * CONTRAST_THRESHOLD)/100;
    ALGO_Log_2("CONTRAST_THRESHOLD = %d, (CONTRAST_THRESHOLD - tolerance) = %10f\n", CONTRAST_THRESHOLD, (CONTRAST_THRESHOLD - tolerance));
    if(paramContrast->iPrContrast_1 < paramContrast->iPrContrast_2)
	{
		SDDiff = SDOutput - SDInput;
	}
	else
	{
		SDDiff = SDInput - SDOutput;
	}
	ALGO_Log_1("SDDiff = %10f\n", SDDiff);

    if( SDDiff > (CONTRAST_THRESHOLD - tolerance))
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
#else
    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;
    TReal64 Intensity_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;
    TReal64 Intensity_Im2 = 0;

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    Intensity_Im1 = (R_Avg_Im1 + G_Avg_Im1 + B_Avg_Im1)/3.255;
    Intensity_Im2 = (R_Avg_Im2 + G_Avg_Im2 + B_Avg_Im2)/3.255;

    ALGO_Log_2("Intensity_Im1 = %10f, Intensity_Im2 %10f\n", Intensity_Im1, Intensity_Im2);
    
    if( Intensity_Im1 > Intensity_Im2)
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
#endif
    }
