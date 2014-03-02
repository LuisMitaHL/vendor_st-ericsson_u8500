/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoluminancemean.c
* \brief    Contains Algorithm Library Luminance Mean comparison validation functions
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
#include "algoluminancemean.h"

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
					                       const TAlgoMetricParams* aMetricParams )
{

	TAlgoParamLuminanceMeanValue *paramLuminanceMeanValue = (TAlgoParamLuminanceMeanValue*)aMetricParams->iParams;
	
	TUint32 i = 0;
	TUint32 j = 0;

	TUint32 index = 0;

	TReal64 Avg_Disable = 0;
	TReal64 Avg_Enable = 0;

	TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
							 + aMetricParams->iTestCoordinates.iStartIndexX;
	TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

	TReal32 lowThresold = 0, highThresold = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_3("Luminance Mean Metric : iMinFactor %d, iMaxFactor %d, iTolerance %d\n\n", paramLuminanceMeanValue->iMinFactor, paramLuminanceMeanValue->iMaxFactor, paramLuminanceMeanValue->iTolerance);
	ALGO_Log_1("iIsFirstImageDarker %d\n", paramLuminanceMeanValue->iIsFirstImageDarker);

	if(paramLuminanceMeanValue->iIsFirstImageDarker == 0)
	{
		const TAlgoImage* aImageTmp = aImageEffectDisable;
		aImageEffectDisable = aImageEffectEnable;
		aImageEffectEnable = aImageTmp;
		ALGO_Log_0(" inversion of images order done\n");
	}
	else
	{
		ALGO_Log_0(" inversion of images order not needed\n");
	}
	
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
	lowThresold		= paramLuminanceMeanValue->iMinFactor * (100 - paramLuminanceMeanValue->iTolerance) / 100.0;
	if(paramLuminanceMeanValue->iMaxFactor != 0)
		highThresold	= paramLuminanceMeanValue->iMaxFactor * (100 + paramLuminanceMeanValue->iTolerance) / 100.0;
	else
		highThresold	= 0;

	ALGO_Log_2("lowThresold %.3f, highThresold %.3f\n", lowThresold, highThresold);

	if(  ((Avg_Enable/Avg_Disable) >= lowThresold)
	  && ( ((Avg_Enable/Avg_Disable) <= highThresold) || (highThresold == 0) ) 
	  )
	{
		ALGO_Log_0("Luminance Mean Metric : PASSED in Ratio Test \n\n");
		ALGO_Log_1("Luminance Mean Metric : Avg_Enable/Avg_Disable = %f\n\n", (Avg_Enable/Avg_Disable));
		return EErrorNone;
	}
	else
	{
		ALGO_Log_0("Luminance Mean Metric : FAILED in Ratio Test \n\n");
		ALGO_Log_1("Luminance Mean Metric : Avg_Enable/Avg_Disable = %f\n\n", (Avg_Enable/Avg_Disable));
		return EErrorNotValidated;
	}
	
}
