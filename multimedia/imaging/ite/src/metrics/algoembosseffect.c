/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoembosseffect.c
* \brief    Contains Algorithm Library Emboss Effect validation functions
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
#include "algoembosseffect.h"

/**
 * Validation Metric for the Emboss Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricEmbossEffect( const TAlgoImage* aImageEffectDisable, 
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

    TUint32 InputMean = 0;
    TUint32 OutputMean = 0;

    TInt32 DiffInput = 0;
    TInt32 DiffOutput = 0;

    TReal64 SDInput = 0;
    TReal64 SDOutput = 0;

    TReal64 tolerance = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
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

    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		    {
            InputMean = InputMean + *iR;
            OutputMean = OutputMean + *oR;

            iR++;		
			oR++;				
            }
	    iR += offset;		
		oR += offset;
        }

    InputMean = InputMean/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    OutputMean = OutputMean/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);

    //ALGO_Log_2("LumaInputMean = %10d, LumaOutputMean = %10d\n", InputMean, OutputMean);

    iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;

    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		    {
            DiffInput = DiffInput + ((*iR - InputMean) * (*iR - InputMean));
            DiffOutput = DiffOutput + ((*oR - OutputMean) * (*oR - OutputMean));

            iR++;		
			oR++;				
            }
	    iR += offset;		
		oR += offset;
        }

    SDInput = SQUAREROOT(DiffInput/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY));
    SDOutput = SQUAREROOT(DiffOutput/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY));

    //ALGO_Log_2("SDInput = %10f, SDOutput = %10f\n", SDInput, SDOutput);

    tolerance = (aMetricParams->iErrorTolerance.iErrorToleranceR * EMBOSS_THRESHOLD)/100; 

    if( (SDInput/SDOutput) > (EMBOSS_THRESHOLD - tolerance))
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }

#else
    TUint32 index = 0;
    TUint32 x = 0;
    TUint32 y = 0;
    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

    TInt32 disableImageCount = 0;
    TInt32 enableImageCount = 0;

    TUint32 inHorDiff = 0;
    TUint32 inVerDiff = 0;
    TUint32 outHorDiff = 0;
    TUint32 outVerDiff = 0;

    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
    for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * 
         aImageEffectDisable->iImageParams.iImageWidth); index++)
        {
        aImageEffectDisable->iImageData.iRChannel[index] = LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                     + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                         + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index];

        aImageEffectEnable->iImageData.iRChannel[index] = LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                    + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                        + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index];
        }

    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		    {
            inHorDiff = ABS(*iR - *(iR + 1));
            inVerDiff = ABS(*iR - *(iR + aImageEffectDisable->iImageParams.iImageWidth));

            if((inHorDiff > EDGE_DETECTION_THRESHOLD) 
                || (inVerDiff > EDGE_DETECTION_THRESHOLD))
			    {
			    disableImageCount++;		
				}
		    
            outHorDiff = ABS(*oR - *(oR + 1));
            outVerDiff = ABS(*oR - *(oR + aImageEffectEnable->iImageParams.iImageWidth));
          
            if(( outHorDiff > EDGE_DETECTION_THRESHOLD) 
                || (outVerDiff > EDGE_DETECTION_THRESHOLD))
		        {
				enableImageCount++;		
			    }
            iR++;		
			oR++;				
            }
	    iR += offset;		
		oR += offset;
        }

    ALGO_Log_2("enableImageCount = %10d, disableImageCount %10d\n", enableImageCount, disableImageCount);

    if( (enableImageCount - disableImageCount) > EMBOSS_THRESHOLD)
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
#endif
    }
