/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoexposurevalue.c
* \brief    Contains Algorithm Library ExposureValue validation functions
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
#include "algoexposurevalue.h"

/**
 * Validation Metric for the ExposureValue.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricExposureValue( const TAlgoImage* aImageEffectDisable,
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams )
	{
	TUint32 i = 0;
    TUint32 j = 0;

    TUint32 index = 0;

	TReal64 Avg_Disable = 0;
    TReal64 Avg_Enable = 0;


	TAlgoParamExposureValue *paramExposureValue = (TAlgoParamExposureValue*)aMetricParams->iParams;

	TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	switch(paramExposureValue->iExpoValueType)
		{
		case EExpoValue_Metering:
			{

			}
		break;

		case EExpoValue_EVCompensation:
        case EExpoValue_Senstivity:
			{
			//Getting the luma component for both images in the R Channel.
			for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight *
				 aImageEffectDisable->iImageParams.iImageWidth); index++)
				{
				aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);

				aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
				}


			//Calculating the average of luma components of both images.
			index = startingOffset;
			for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
				{
				for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
					{
					Avg_Disable = Avg_Disable + aImageEffectDisable->iImageData.iRChannel[index];
					Avg_Enable = Avg_Enable + aImageEffectEnable->iImageData.iRChannel[index];
					index++;
					}
				index = index + offset;
				}

			Avg_Disable = Avg_Disable/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
			Avg_Enable = Avg_Enable/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);

			//Ratio Test
			if(((Avg_Enable/Avg_Disable) > 1.5) && ((Avg_Enable/Avg_Disable) < 2.5))
					{
					ALGO_Log_0("ExposureValue Metric : PASSED in Ratio Test \n\n");
					ALGO_Log_1("ExposureValue Metric : Avg_Enable/Avg_Disable = %f\n\n", (Avg_Enable/Avg_Disable));
					}
				else
					{
					ALGO_Log_0("ExposureValue Metric : FAILED in Ratio Test \n\n");
					ALGO_Log_1("ExposureValue Metric : Avg_Enable/Avg_Disable = %f\n\n", (Avg_Enable/Avg_Disable));
					}

			ALGO_Log_2("ExposureValue Metric : Avg_Disable = %.3f, Avg_Enable = %.3f\n\n", Avg_Disable, Avg_Enable);
			ALGO_Log_2("ExposureValue Metric : ExposureValue_Disable = %f, ExposureValue_Enable = %f\n\n", paramExposureValue->iExposureValue_Disable, paramExposureValue->iExposureValue_Enable);


			if(((Avg_Disable < Avg_Enable) && (paramExposureValue->iExposureValue_Disable < paramExposureValue->iExposureValue_Enable )) ||
			   ((Avg_Disable > Avg_Enable) && (paramExposureValue->iExposureValue_Disable > paramExposureValue->iExposureValue_Enable )))
				{
				return EErrorNone;
				}
			else
				{
				return EErrorNotValidated;
				}
			}
		//break; //set in comment to avoid warnings
				// break not needed because of returns above
				// when changing above code, break might be required

		case EExpoValue_ApertureFNumber:
			{

			}
		break;

		case EExpoValue_ApertureAuto:
			{

			}
		break;

		case EExpoValue_ShutterSpeedMsec:
			{

			}
		break;

		case EExpoValue_ShutterSpeedAuto:
			{

			}
		break;

		case EExpoValue_SenstivityAuto:
			{

			}
		break;

		default:
			{
			return EErrorNotSupported;
			}

		}
	ALGO_Log_2("ExposureValue Metric : iExposureValue_Disable %d, iExposureValue_Enable %d\n\n", paramExposureValue->iExposureValue_Disable, paramExposureValue->iExposureValue_Enable);
	return EErrorNotValidated;

	}
