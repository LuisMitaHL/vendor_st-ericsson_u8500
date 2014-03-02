/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algosepiaeffect.c
* \brief    Contains Algorithm Library Sepia Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algosepiaeffect.h"
#include "algoutilities.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"

/**
 * Validation Metric for the Sepia Effect through Channel Average.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSepiaEffect_ChannelAverage( const TAlgoImage* aImageEffectDisable, 
					                    const TAlgoImage* aImageEffectEnable,
					                    const TAlgoMetricParams* aMetricParams )
    {
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

    if((R_Avg_Im1 < G_Avg_Im1) && (G_Avg_Im1 < B_Avg_Im1)) 
        {
        if((R_Avg_Im2 >= G_Avg_Im2) && ( G_Avg_Im2 > B_Avg_Im2))
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
    }

/**
 * Validation Metric for the Sepia Effect through Hue.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSepiaEffect_Hue( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
    {
    TUint32 iNonSepiaPixelCountDisableImage = 0;
    TUint32 iNonSepiaPixelCountEnableImage = 0;
    TUint32 x=0;
    TUint32 y=0;

    TReal32 NonSepiaPercentDisableImage = 0.0;
    TReal32 NonSepiaPercentEnableImage = 0.0;
    
    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);
 
    //TReal64 temp;
    //TUint32 satCount = 0;
    TReal64 hue = 0;
    TUint8 max, min, delta;
    TReal64 r;
    TReal64 g;
    TReal64 b;
 
    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    TUint8 *iG = aImageEffectDisable->iImageData.iGChannel + startingOffset;
    TUint8 *iB = aImageEffectDisable->iImageData.iBChannel + startingOffset;
    TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    TUint8 *oG = aImageEffectEnable->iImageData.iGChannel + startingOffset;
    TUint8 *oB = aImageEffectEnable->iImageData.iBChannel + startingOffset;
 
    for (y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX; x++)
            {
            r = *iR;
            g = *iG;
            b = *iB;
 
            if ((r >= g) && (r >= b))
                {
                max = (TUint8)(r);    
                }
            else if ((g >= r) && (g >= b))
                {
                max = (TUint8)(g);    
                }
            else
                {
                max = (TUint8)(b);
                }
    
            if ((r <= g) && (r <= b))
                {
                min = (TUint8)(r);    
                }
            else if ((g <= r) && (g <= b))
                {
                min = (TUint8)(g);    
                }
            else
                {
                min = (TUint8)(b);
                }

            delta = max - min;
            
            /* Find hue and check hue is between 10 - 80 degress */
            if ((max != 0) && (delta != 0))
                {
                if( r == max )
                    {
                    hue = ( g-b ) / delta;  // between yellow & magenta
                    }
                else if( g == max )
                    {
                    hue = 2 + ( b-r) / delta; // between cyan & yellow
                    }
                    else
                    {
                    hue = 4 + (r-g) / delta; // between magenta & cyan
                    }
                    hue *= 60;    // degrees
                    if( hue < 0 )
                        hue += 360;
         
            if (!((hue >= 30) && (hue <= 60)))
                {
                iNonSepiaPixelCountDisableImage++;
                }
            }
        else
            {
            iNonSepiaPixelCountDisableImage++;
            }
            iR++;  
            iG++;  
            iB++; 
            }
        iR += offset;  
        iG += offset;  
        iB += offset; 
        }
    
    for (y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX; x++)
            {
            r = *oR;
            g = *oG;
            b = *oB;
 
            if ((r >= g) && (r >= b))
                {
                max = (TUint8)(r);    
                }
            else if ((g >= r) && (g >= b))
                {
                max = (TUint8)(g);    
                }
            else
                {
                max = (TUint8)(b);
                }
    
            if ((r <= g) && (r <= b))
                {
                min = (TUint8)(r);    
                }
            else if ((g <= r) && (g <= b))
                {
                min = (TUint8)(g);    
                }
            else
                {
                min = (TUint8)(b);
                }

            delta = max - min;
            
            /* Find hue and check hue is between 10 - 80 degress */
            if ((max != 0) && (delta != 0))
                {
                if( r == max )
                    {
                    hue = ( g-b ) / delta;  // between yellow & magenta
                    }
                else if( g == max )
                    {
                    hue = 2 + ( b-r) / delta; // between cyan & yellow
                    }
                    else
                    {
                    hue = 4 + (r-g) / delta; // between magenta & cyan
                    }
                    hue *= 60;    // degrees
                    if( hue < 0 )
                        hue += 360;
         
            if (!((hue >= 30) && (hue <= 60)))
                {
                iNonSepiaPixelCountEnableImage++;
                }
            }
        else
            {
            iNonSepiaPixelCountEnableImage++;
            }
            oR++;  
            oG++;  
            oB++; 
            }
        oR += offset;  
        oG += offset;  
        oB += offset; 
        }

    NonSepiaPercentDisableImage = (TReal32)iNonSepiaPixelCountDisableImage * 100 / 
                               ((TReal32)(aMetricParams->iTestCoordinates.iPixelsToGrabX * 
                                          aMetricParams->iTestCoordinates.iPixelsToGrabY));

    NonSepiaPercentEnableImage = (TReal32)iNonSepiaPixelCountEnableImage * 100 / 
                               ((TReal32)(aMetricParams->iTestCoordinates.iPixelsToGrabX * 
                                          aMetricParams->iTestCoordinates.iPixelsToGrabY));

	if( NonSepiaPercentDisableImage > 90 )
	{
		if( NonSepiaPercentEnableImage < 1 )
		{
			ALGO_Log_1("\n iNonSepiaPixelCountEnableImage: %d", iNonSepiaPixelCountEnableImage);
			ALGO_Log_1("\n Percentage of Non Sepia colors: %0.2f ", (TReal32)iNonSepiaPixelCountEnableImage *100 / (TReal32) (aMetricParams->iTestCoordinates.iPixelsToGrabY * aMetricParams->iTestCoordinates.iPixelsToGrabX) );
			ALGO_Log_0("MetricsEPIAEffect : EErrorNone\n");
			return EErrorNone;
		}
		else
		{
			ALGO_Log_1("\n iNonSepiaPixelCountEnableImage: %d", iNonSepiaPixelCountEnableImage);
			ALGO_Log_1("\n Percentage of Non Sepia colors: %0.2f ", (TReal32)iNonSepiaPixelCountEnableImage *100 / (TReal32) (aMetricParams->iTestCoordinates.iPixelsToGrabY * aMetricParams->iTestCoordinates.iPixelsToGrabX) );
			ALGO_Log_0("MetricsEPIAEffect : EErrorNotValidated\n");
			return EErrorNotValidated;
		}
	}
	else
	{
		ALGO_Log_1("\n NonSepiaPercentDisableImage: %d", NonSepiaPercentDisableImage);
		ALGO_Log_0("MetricsEPIAEffect : EErrorImageNotRelevant\n");
		return EErrorImageNotRelevant;
	}

}
