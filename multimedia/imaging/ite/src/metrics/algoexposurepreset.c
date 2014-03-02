/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoexposurepreset.c
* \brief    Contains Algorithm Library ExposurePreset validation functions
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
#include "algoexposurepreset.h"

/**
 * Validation Metric for the ExposurePreset.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricExposurePreset( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams )
	{

	TAlgoParamExposurePreset *paramExposurePreset = (TAlgoParamExposurePreset*)aMetricParams->iParams;
	TAlgoError error = EErrorNotValidated;

	TUint32 i = 0;
	TUint32 j = 0;
	TUint32 temp = 0;
	TUint32 medianArrayDisable[256] = {0};
	TUint32 medianArrayEnable[256] = {0};
	TUint32 medianDisable = 0;
	TUint32 medianEnable = 0;

	TUint32 index = 0;

	TReal64 Avg_Disable = 0;
	TReal64 Avg_Enable = 0;

	TReal64 diff=0, diffPercentage=0;

	TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
							 + aMetricParams->iTestCoordinates.iStartIndexX;
	TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_3("ExposurePreset Metric : iExpoPreset_1 %d, iExpoPreset_2 %d, iTolerance %d\n\n", paramExposurePreset->iExpoPreset_1, paramExposurePreset->iExpoPreset_2, paramExposurePreset->iTolerance);
	
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

	    medianArrayDisable[aImageEffectDisable->iImageData.iRChannel[index]]++;
		medianArrayEnable[aImageEffectEnable->iImageData.iRChannel[index]]++;
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

	//Calculating the median
	temp = 0;
	for( i=0; i<256; i++ )
		{
		temp = temp + medianArrayDisable[i];
		if(temp > (aImageEffectDisable->iImageParams.iImageHeight *  aImageEffectDisable->iImageParams.iImageWidth)/2)
			{
			break;
			}
		}
	medianDisable = i;

	temp = 0;
	for( i=0; i<256; i++ )
		{
		temp = temp + medianArrayEnable[i];
		if(temp > (aImageEffectEnable->iImageParams.iImageHeight *  aImageEffectEnable->iImageParams.iImageWidth)/2)
			{
			break;
			}
		}
	medianEnable = i;

	//Doing median Check
	if(((ABS(Avg_Disable - medianDisable)) < 10) && ((ABS(Avg_Enable - medianEnable)) < 10))
		{
		ALGO_Log_0("Exposure Preset Metric : Median Check PASSED\n");
		ALGO_Log_2("Exposure Preset Metric : Average Disable = %.3f, Median Disable = %d\n", Avg_Disable, medianDisable);
		ALGO_Log_2("Exposure Preset Metric : Average Enable  = %.3f, Median Enable  = %d\n\n", Avg_Enable, medianEnable);
		}
	else
		{
		ALGO_Log_0("Exposure Preset Metric : Median Check FAILED\n");
		ALGO_Log_2("Exposure Preset Metric : Average Disable = %.3f, Median Disable = %d\n", Avg_Disable, medianDisable);
		ALGO_Log_2("Exposure Preset Metric : Average Enable  = %.3f, Median Enable  = %d\n\n", Avg_Enable, medianEnable);
		}

	diff = ABS(Avg_Disable - Avg_Enable) ;
	diffPercentage = (diff / 256)*100;
	ALGO_Log_2("Avg_Disable = %.3f, Avg_Enable = %.3f\n", Avg_Disable, Avg_Enable);
	ALGO_Log_3("diff = %.3f, diffPercentage = %.3f, tolerance = %d\n", diff, diffPercentage, paramExposurePreset->iTolerance);
	if( diffPercentage < paramExposurePreset->iTolerance )
		{
		error = EErrorNone;
		}
	else
		{
		error = EErrorNotValidated;
		ALGO_Log_2("diffPercentage = %.1f, iTolerance = %d\n", diffPercentage, paramExposurePreset->iTolerance);
		ALGO_Log_1("Error in %s\n", __FUNCTION__);
		}

	return error;
	}
