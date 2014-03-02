/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algosolarizeeffect.c
* \brief    Contains Algorithm Library Solarize Effect validation functions
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
#include "algosolarizeeffect.h"

/**
 * Validation Metric for the Solarize Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSolarizeEffect( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams )
    {
#if 1
    TUint32 i = 0;
    TUint32 j = 0;

    TUint32 index = 0;
    TUint32 index1 = 0;

    TInt32 negCheck = 0;
    TInt32 posCheck = 0;

    //TUint32 x = 0;
    //TUint32 y = 0;
     
    TReal64 negTolerance = 0;
    TReal64 posTolerance = 0;

    TReal64 colonCurrentRImg1 = 0;
    TReal64 colonCurrentRImg2 = 0;
    TReal64 colonPreviousImg1 = 0;
    TReal64 colonPreviousImg2 = 0;

    TInt32 posDeviationImg1 = 0;
    TInt32 negDeviationImg1 = 0;
    TInt32 posDeviationImg2 = 0;
    TInt32 negDeviationImg2 = 0;

    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
   
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
    
    index1 = startingOffset;
    index = index1;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i=i+10 )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonCurrentRImg1 = colonCurrentRImg1 + aImageEffectDisable->iImageData.iRChannel[index]; 
            index = index + aImageEffectDisable->iImageParams.iImageWidth;
            }

        colonCurrentRImg1 = colonCurrentRImg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        if((colonCurrentRImg1 - colonPreviousImg1) > SOLARIZE_THRESHOLD)
            {
            posDeviationImg1++;
            colonPreviousImg1 = colonCurrentRImg1;
            }
        if((colonPreviousImg1 - colonCurrentRImg1) > SOLARIZE_THRESHOLD)
            {
            negDeviationImg1++;
            colonPreviousImg1 = colonCurrentRImg1;
            }

        colonCurrentRImg1 = 0;
        index1 = index1 + 10;
        index = index1;
        }

    index1 = startingOffset;
    index = index1;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i=i+10 )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonCurrentRImg2 = colonCurrentRImg2 + aImageEffectEnable->iImageData.iRChannel[index]; 
            index = index + aImageEffectEnable->iImageParams.iImageWidth;
            }

        colonCurrentRImg2 = colonCurrentRImg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;
     
        if((colonCurrentRImg2 - colonPreviousImg2) > SOLARIZE_THRESHOLD)
            {
            posDeviationImg2++;
            colonPreviousImg2 = colonCurrentRImg2;
            }
        if((colonPreviousImg2 - colonCurrentRImg2) > SOLARIZE_THRESHOLD)
            {
            negDeviationImg2++;
            colonPreviousImg2 = colonCurrentRImg2;
            }

        colonCurrentRImg2 = 0;
        index1 = index1 + 10;
        index = index1;
        }

    ALGO_Log_2("posDeviationImg1 = %2d, negDeviationImg1 = %2d\n", posDeviationImg1, negDeviationImg1);
    ALGO_Log_2("posDeviationImg2 = %2d, negDeviationImg2 = %2d\n", posDeviationImg2, negDeviationImg2);

    negTolerance = SOLARIZE_CHECK_NEG_THRESHOLD - ((SOLARIZE_CHECK_NEG_THRESHOLD * aMetricParams->iErrorTolerance.iErrorToleranceR)/100);
    posTolerance = SOLARIZE_CHECK_POS_THRESHOLD - ((SOLARIZE_CHECK_NEG_THRESHOLD * aMetricParams->iErrorTolerance.iErrorToleranceR)/100);

    negCheck =(negDeviationImg2 - negDeviationImg1);  
    posCheck =(posDeviationImg1 - posDeviationImg2);  
    if((negCheck > negTolerance) && (posCheck > posTolerance))
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

    TUint32 InputMean = 0;
    TUint32 OutputMean = 0;

    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
    for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * 
         aImageEffectDisable->iImageParams.iImageWidth); index++ )
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
             if(*iR < 128)
                {
                *iR = (*iR) * 2;
                }
            else 
                {
                *iR = 512 - ((*iR) * 2);
                }

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

    ALGO_Log_2("LumaInputMean = %10d, LumaOutputMean = %10d\n", InputMean, OutputMean);

    if( InputMean/OutputMean > 2)
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }



//////////////////////////////////////Second Approach//////////////////////////////////////
    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;
    TReal64 Intensity_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;
    TReal64 Intensity_Im2 = 0;

    TUint32 index = 0;
    TUint32 x = 0;
    TUint32 y = 0;

    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
	TUint8 *iG = aImageEffectDisable->iImageData.iGChannel + startingOffset;
	TUint8 *iB = aImageEffectDisable->iImageData.iBChannel + startingOffset;
    //TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    //TUint8 *oG = aImageEffectEnable->iImageData.iGChannel + startingOffset;
    //TUint8 *oB = aImageEffectEnable->iImageData.iBChannel + startingOffset;
    TUint8 temp = 0;

    //TUint32 itemp = 0;
    //TUint32 otemp = 0;

    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY; y++)
        {
        for(x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX; x++)
            {
            
            if(*iR < 128)
                {
                *iR = (*iR) * 2;
                }
            else 
                {
                if(*iR != 128)
                    {
                    *iR = 510 - ((*iR) * 2);
                    }
                }

            if(*iG < 128)
                {
                *iG = (*iG) * 2;
                }
            else 
                {
                if(*iG != 128)
                    {
                    *iG = 510 - ((*iG) * 2);
                    }
                }

            if(*iB < 128)
                {
                *iB = (*iB) * 2;
                }
            else 
                {
                if(*iB != 128)
                    {
                    *iB = 510 - ((*iB) * 2);
                    }
                }
            /*
            itemp = ((*iR)+(*iG)+(*iB))/3;
             if(itemp < 128)
                {
                itemp = itemp * 2;
                }
            else 
                {
                itemp = 512 - (itemp * 2);
                }
            
            otemp = ((*oR)+(*oG)+(*oB))/3;

            Intensity_Im1 = Intensity_Im1 + itemp;
            Intensity_Im2 = Intensity_Im2 + otemp;
            */

            iR++;		
		    iG++;		
		    iB++;	
            //oR++;		
		    //oG++;		
		    //oB++;	
            }

        iR += offset;		
		iG += offset;		
		iB += offset;	
        //oR += offset;		
		//oG += offset;		
		//oB += offset;	
        }

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    //Intensity_Im1 = Intensity_Im1/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    //Intensity_Im2 = Intensity_Im2/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    Intensity_Im1 = (R_Avg_Im1 + G_Avg_Im1 + B_Avg_Im1)/3.255;
    Intensity_Im2 = (R_Avg_Im2 + G_Avg_Im2 + B_Avg_Im2)/3.255;

    ALGO_Log_2("Intensity_Im1 = %10f, Intensity_Im2 %10f\n", Intensity_Im1, Intensity_Im2);
    
    if( Intensity_Im1 == Intensity_Im2)
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
#endif
    }


/**
 * Validation Metric for the Solarize Effect (basic).
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSolarizeEffect_basic( const TAlgoImage* aImageEffectDisable, 
					                       const TAlgoImage* aImageEffectEnable,
					                       const TAlgoMetricParams* aMetricParams )
{
    TUint32 index = 0;
    TReal64 negTolerance = 0;
    TReal64 posTolerance = 0;

	TInt32 colorBar_NbColon = 8;
	TInt32 colorBar_ColonWidth = 0;
	TUint8 disableFirstColon_white_PixelValue = 0, enableFirstColon_white_PixelValue = 0;
	TUint8 disableLastColon_black_PixelValue = 0, enableLastColon_black_PixelValue = 0;
   
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
	disableFirstColon_white_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableFirstColon_white_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];
	disableLastColon_black_PixelValue	= aImageEffectDisable->iImageData.iRChannel[ (colorBar_ColonWidth*(2*colorBar_NbColon-1)/2) + (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight / 2)];
	enableLastColon_black_PixelValue	= aImageEffectEnable->iImageData.iRChannel[	(colorBar_ColonWidth*(2*colorBar_NbColon-1)/2) + (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight / 2)];

	ALGO_Log_1("disableFirstColon_white_PixelValue = %d\n", disableFirstColon_white_PixelValue);
	ALGO_Log_1("enableFirstColon_white_PixelValue = %d\n", enableFirstColon_white_PixelValue);
    negTolerance = SOLARIZE_CHECK_NEG_THRESHOLD - ((SOLARIZE_CHECK_NEG_THRESHOLD * aMetricParams->iErrorTolerance.iErrorToleranceR)/100);
    ALGO_Log_1("negTolerance = %10f\n", negTolerance);
	
	ALGO_Log_1("disableLastColon_black_PixelValue = %d\n", disableLastColon_black_PixelValue);
	ALGO_Log_1("enableLastColon_black_PixelValue = %d\n", enableLastColon_black_PixelValue);
    posTolerance = SOLARIZE_CHECK_POS_THRESHOLD - ((SOLARIZE_CHECK_NEG_THRESHOLD * aMetricParams->iErrorTolerance.iErrorToleranceR)/100);
    ALGO_Log_1("posTolerance = %10f\n", posTolerance);

	if( 
		(enableFirstColon_white_PixelValue <= (disableFirstColon_white_PixelValue - negTolerance) )
		&& 
		(enableLastColon_black_PixelValue >= (disableLastColon_black_PixelValue + posTolerance) )
		)
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;
	}
}
