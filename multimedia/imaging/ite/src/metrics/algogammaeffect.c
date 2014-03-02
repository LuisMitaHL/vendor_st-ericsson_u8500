/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algogammaeffect.c
* \brief    Contains Algorithm Library Gamma Effect validation functions
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
#include "algogammaeffect.h"

/**
 * Validation Metric for the Gamma Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricGammaEffect( const TAlgoImage* aImageEffectDisable, 
					                    const TAlgoImage* aImageEffectEnable,
					                    const TAlgoMetricParams* aMetricParams )
    {
    TUint32 i = 0;
    TUint32 j = 0;
    TUint32 index = 0;
    TUint32 index1 = 0;
    TReal64 tolerance = 0;
    TReal64 colonCurrentRImg1 = 0;
    TReal64 colonCurrentRImg2 = 0;
    TReal64 colonPreviousImg1 = 0;
    TReal64 colonPreviousImg2 = 0;
    TInt32 posDeviationImg1 = 0;
    TInt32 negDeviationImg1 = 0;
    TInt32 posDeviationImg2 = 0;
    TInt32 negDeviationImg2 = 0;
    TReal64 prevDeviation = 0;
    TReal64 currDeviation = 0;
	TUint32 startingOffset = 0;
    TAlgoParamGamma *paramGamma = (TAlgoParamGamma*)aMetricParams->iParams;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_2("Gamma Metric : iPrGamma_1 %d, iPrGamma_2 %d\n\n", paramGamma->iPrGamma_1, paramGamma->iPrGamma_2);

    startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;

    for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * aImageEffectDisable->iImageParams.iImageWidth); index++ )
    {
        aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);

        aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
    }
    
    index1 = startingOffset;
    index = index1;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i=i+50 )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonCurrentRImg1 = colonCurrentRImg1 + aImageEffectDisable->iImageData.iRChannel[index]; 
            index = index + aImageEffectDisable->iImageParams.iImageWidth;
            }

        colonCurrentRImg1 = colonCurrentRImg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        if (i == 0)
            {
            colonPreviousImg1 = colonCurrentRImg1;
            }

        currDeviation = (colonCurrentRImg1 - colonPreviousImg1);

        ALGO_Log_1("currDeviation = %2f \n", currDeviation);

        if((currDeviation - prevDeviation) > 0)
            {
            posDeviationImg1++;
            //colonPreviousImg1 = colonCurrentRImg1;
            }

        if((prevDeviation - currDeviation) > 0)
            {
            negDeviationImg1++;
            //colonPreviousImg1 = colonCurrentRImg1;
            }

        prevDeviation = currDeviation;
        colonPreviousImg1 = colonCurrentRImg1;
        colonCurrentRImg1 = 0;
        index1 = index1 + 50;
        index = index1;
        }

    prevDeviation = 0;
    currDeviation = 0;
    index1 = startingOffset;
    index = index1;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i=i+50 )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonCurrentRImg2 = colonCurrentRImg2 + aImageEffectEnable->iImageData.iRChannel[index]; 
            index = index + aImageEffectEnable->iImageParams.iImageWidth;
            }

        colonCurrentRImg2 = colonCurrentRImg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;
     
         if (i == 0)
            {
            colonPreviousImg2 = colonCurrentRImg2;
            }

        currDeviation = (colonCurrentRImg2 - colonPreviousImg2);

        ALGO_Log_1("currDeviation = %2f \n", currDeviation);

        if((currDeviation - prevDeviation) > 0)
            {
            posDeviationImg2++;
            //colonPreviousImg2 = colonCurrentRImg2;
            }
        if((prevDeviation - currDeviation) > 0)
            {
            negDeviationImg2++;
            //colonPreviousImg2 = colonCurrentRImg2;
            }

        prevDeviation = currDeviation;
        colonPreviousImg2 = colonCurrentRImg2;
        colonCurrentRImg2 = 0;
        index1 = index1 + 50;
        index = index1;
        }

    ALGO_Log_2("posDeviationImg1 = %2d, negDeviationImg1 = %2d\n", posDeviationImg1, negDeviationImg1);
    ALGO_Log_2("posDeviationImg2 = %2d, negDeviationImg2 = %2d\n", posDeviationImg2, negDeviationImg2);

    tolerance = GAMMA_THRESHOLD - aMetricParams->iErrorTolerance.iErrorToleranceR;

    if(((ABS(negDeviationImg1 - posDeviationImg1)) - (ABS(negDeviationImg2 - posDeviationImg2))) > tolerance)
        {
	    return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
    }


/**
* Validation Metric for the Gamma Effect basic.
*
* @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
* @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
* @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
*
* @return                          [TAlgoError] EErrorNone if the validation is passed.
*/
TAlgoError ValidationMetricGammaEffect_basic( const TAlgoImage* aImageEffectDisable, 
										const TAlgoImage* aImageEffectEnable,
										const TAlgoMetricParams* aMetricParams )
{
    TUint32 index = 0;
    TReal64 toleranceGamma = 0;

	TInt32 colorBar_NbColon = 8;
	TInt32 colorBar_ColonWidth = 0;
	TUint8 disableColon_1_PixelValue = 0, enableColon_1_PixelValue = 0;
	TUint8 disableColon_2_PixelValue = 0, enableColon_2_PixelValue = 0;
	TUint8 disableColon_3_PixelValue = 0, enableColon_3_PixelValue = 0;
	TUint8 disableColon_4_PixelValue = 0, enableColon_4_PixelValue = 0;
	TUint8 disableColon_5_PixelValue = 0, enableColon_5_PixelValue = 0;
	TUint8 disableColon_6_PixelValue = 0, enableColon_6_PixelValue = 0;
	TUint8 disableColon_7_PixelValue = 0, enableColon_7_PixelValue = 0;
	TUint8 disableColon_8_PixelValue = 0, enableColon_8_PixelValue = 0;
   
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
    
	if(aMetricParams->iTestCoordinates.iPixelsToGrabX != aImageEffectDisable->iImageParams.iImageWidth)
	{
		ALGO_Log_0("Error, need to work on the whole image (color bar) Width \n");
		return EErrorNotValidated;
	}
	if(aMetricParams->iTestCoordinates.iPixelsToGrabY != aImageEffectDisable->iImageParams.iImageHeight)
	{
		ALGO_Log_0("Error, need to work on the whole image (color bar) Height \n");
		return EErrorNotValidated;
	}
	if( (aMetricParams->iTestCoordinates.iStartIndexY != 0) || (aMetricParams->iTestCoordinates.iStartIndexX != 0) )
	{
		ALGO_Log_0("Error, need to work on the whole image (color bar) StartIndex\n");
		return EErrorNotValidated;
	}

	colorBar_ColonWidth = aImageEffectDisable->iImageParams.iImageWidth / colorBar_NbColon;

	// middle of thh colon and middle of the height
	disableColon_1_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*1/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_1_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*1/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_2_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*3/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_2_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*3/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_3_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*5/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_3_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*5/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_4_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*7/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_4_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*7/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_5_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*9/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_5_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*9/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_6_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*11/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_6_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*11/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_7_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*13/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_7_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*13/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableColon_8_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*15/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableColon_8_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*15/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];

	ALGO_Log_1("disableColon_1_PixelValue = %d\n", disableColon_1_PixelValue);
	ALGO_Log_1("enableColon_1_PixelValue = %d\n", enableColon_1_PixelValue);
	ALGO_Log_1("disableColon_2_PixelValue = %d\n", disableColon_2_PixelValue);
	ALGO_Log_1("enableColon_2_PixelValue = %d\n", enableColon_2_PixelValue);
	ALGO_Log_1("disableColon_3_PixelValue = %d\n", disableColon_3_PixelValue);
	ALGO_Log_1("enableColon_3_PixelValue = %d\n", enableColon_3_PixelValue);
	ALGO_Log_1("disableColon_4_PixelValue = %d\n", disableColon_4_PixelValue);
	ALGO_Log_1("enableColon_4_PixelValue = %d\n", enableColon_4_PixelValue);
	ALGO_Log_1("disableColon_5_PixelValue = %d\n", disableColon_5_PixelValue);
	ALGO_Log_1("enableColon_5_PixelValue = %d\n", enableColon_5_PixelValue);
	ALGO_Log_1("disableColon_6_PixelValue = %d\n", disableColon_6_PixelValue);
	ALGO_Log_1("enableColon_6_PixelValue = %d\n", enableColon_6_PixelValue);
	ALGO_Log_1("disableColon_7_PixelValue = %d\n", disableColon_7_PixelValue);
	ALGO_Log_1("enableColon_7_PixelValue = %d\n", enableColon_7_PixelValue);
	ALGO_Log_1("disableColon_8_PixelValue = %d\n", disableColon_8_PixelValue);
	ALGO_Log_1("enableColon_8_PixelValue = %d\n", enableColon_8_PixelValue);


	toleranceGamma = GAMMA_THRESHOLD - ((GAMMA_THRESHOLD * aMetricParams->iErrorTolerance.iErrorToleranceR)/100);
    ALGO_Log_1("toleranceGamma = %10f\n", toleranceGamma);

	if( 
		(enableColon_2_PixelValue <= (disableColon_2_PixelValue - toleranceGamma) )
		&& 
		(enableColon_3_PixelValue <= (disableColon_3_PixelValue - toleranceGamma) )
		&& 
		(enableColon_4_PixelValue <= (disableColon_4_PixelValue - toleranceGamma) )
		&& 
		(enableColon_5_PixelValue <= (disableColon_5_PixelValue - toleranceGamma) )
		&& 
		(enableColon_6_PixelValue <= (disableColon_6_PixelValue - toleranceGamma) )
		&& 
		(enableColon_7_PixelValue <= (disableColon_7_PixelValue - toleranceGamma) )
		)
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;
	}
}
