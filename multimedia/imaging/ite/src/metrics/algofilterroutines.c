/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algofilterroutines.c
* \brief    Contains Algorithm Library Filter Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algofilterroutines.h"

/**
 * Routine to apply convolution filter.
 *
 * @param aInputChannel             [TUint8*] The input image channel.
 * @param aOutputChannel            [TUint8*] The output image channel after filtering.
 * @param aFilterParams             [TAlgoGenericFilterParams*] The generic parameters for filter.
 * @param aConvFilterParams         [TAlgoConvFilterParams*] The convolution parameters for filter.
 * @param aMaxPixelVal              [TUint8] The maximum pixel value.
 *
 * @return                          [TAlgoError] EErrorNone if the filtering is successful.
 */
TAlgoError ApplyConvFilter(TUint8* aInputChannel,
                           TUint8* aOutputChannel,
                           TAlgoGenericFilterParams* aFilterParams,
                           TAlgoConvFilterParams* aConvFilterParams,
                           TUint8 aMaxPixelVal)
    {

    TUint8 *inputPtr = aInputChannel + aFilterParams->iStartIndexY * aFilterParams->iImageParams.iImageWidth 
                        + aFilterParams->iStartIndexX;
	TReal64 temp;
	TUint8 *tempPtr;
	TUint32 satCount = 0;
    TUint32 y=0;
    TUint32 x=0;
    TUint32 i=0;
    TUint32 j=0;

	/* Assuming 3*3 matrix, for 5*5 following lines to be changed - TBD */
	TUint8 *outputPtr = aOutputChannel + 
					(1 + aFilterParams->iStartIndexY) * aFilterParams->iImageParams.iImageWidth + 
					(aFilterParams->iStartIndexX + 1);

	/* For applying the convolution, basic problem is about the (N/2) pixel
	 * boundary, as convolution assumes N*N-1 neighbor around it.
	 * For the purpose of testing we will not apply any filter on the first
	 * pixels
	 */
	for (y=0; y < aFilterParams->iPixelsToGrabY; y++)
	    {
		for (x=0; x < aFilterParams->iPixelsToGrabX; x++)
		    {
			temp = 0;
			tempPtr = inputPtr;

			if (aFilterParams->iFilterType == EFilterConvolution)
			    {
				for (i=0; i< N; i++)
				    {
					for (j=0; j< N; j++)
					    {
						temp += (*tempPtr * aConvFilterParams->iMatrix[i][j]);	
						tempPtr++;
					    }
					tempPtr += (aFilterParams->iImageParams.iImageWidth - N);				
				    }
			    }
			else if (aFilterParams->iFilterType == EFilterErosion)
			    {
				temp = *tempPtr;
				for (i=0; i< N; i++)
				    {
					for (j=0; j< N; j++)
					    {
						temp = (temp > *tempPtr) ? *tempPtr : temp;	
						tempPtr++;
					    }
					tempPtr += (aFilterParams->iImageParams.iImageWidth - N);				
				    }
			    }
			else if (aFilterParams->iFilterType == EFilterDilate)
			    {
				temp = *tempPtr;
				for (i=0; i< N; i++)
				    {
					for (j=0; j< N; j++)
					    {
						temp = (temp > *tempPtr) ? temp : *tempPtr;	
						tempPtr++;
					    }
					tempPtr += (aFilterParams->iImageParams.iImageWidth - N);				
				    }
			    }
			else 
			    {
				//ALGO_Log_0("\n Wrong type of Generic filter");
				return EErrorArgument;
			    }

			if (temp > aMaxPixelVal)
			    {
				*outputPtr = aMaxPixelVal;
				satCount++;
			    }
			else if (temp < 0x0)
			    {
				*outputPtr = 0;
			    }
			else
			    {
				*outputPtr = (TUint8)temp;
			    }

			outputPtr++;
			inputPtr++;
		    }
		outputPtr += (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);		
		inputPtr += (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);	
	    }

	//ALGO_Log_0("\n Saturation Count: %d", satCount);

	return EErrorNone;
    }

/**
 * Routine to apply local filter.
 *
 * @param aInputChannel             [TUint8*] The input image channel.
 * @param aOutputChannel            [TUint8*] The output image channel after filtering.
 * @param aFilterParams             [TAlgoGenericFilterParams*] The generic parameters for filter.
 * @param aMaxPixelVal              [TUint8] The maximum pixel value.
 *
 * @return                          [TAlgoError] EErrorNone if the filtering is successful.
 */
TAlgoError ApplyLocalFilter(TUint8* aInputChannel,
                            TUint8* aOutputChannel,
                            TAlgoGenericFilterParams* aFilterParams,
                            TUint8 aMaxPixelVal)
    {

    TUint8 *inputPtr = aInputChannel + aFilterParams->iStartIndexY * aFilterParams->iImageParams.iImageWidth 
                        + aFilterParams->iStartIndexX;
	TReal64 temp;
	TUint32 satCount = 0;
    TUint32 y=0;
    TUint32 x=0;

	/* Assuming 3*3 matrix, for 5*5 following lines to be changed - TBD */
	TUint8 *outputPtr = aOutputChannel 
                        + (aFilterParams->iStartIndexY) * aFilterParams->iImageParams.iImageWidth  
                        + (aFilterParams->iStartIndexX);

	/* For applying the convolution, basic problem is about the (N/2) pixel
	 * boundary, as convolution assumes N*N-1 neighbor around it.
	 * For the purpose of testing we will not apply any filter on the first
	 * pixels
	 */

	for (y=0; y < aFilterParams->iPixelsToGrabY; y++)
	    {
		for (x=0; x < aFilterParams->iPixelsToGrabX; x++)
		    {
			temp = 0;

			if (aFilterParams->iFilterType == EFilterBrightness)
			    {
				temp = *inputPtr + aFilterParams->iParam;
			    }
			else if (aFilterParams->iFilterType == EFilterGamma)
			    {
				temp = POWER(*inputPtr, aFilterParams->iParam);
			    }
			else if (aFilterParams->iFilterType == EFilterContrast)
			    {
				temp = *inputPtr * aFilterParams->iParam;
			    }
			else if (aFilterParams->iFilterType == EFilterNegative)
			    {
				temp = aMaxPixelVal - *inputPtr;
			    }
			else 
			    {
				//ALGO_Log_0("\n Wrong type of Local filter");
				return EErrorArgument;
			    }

			if (temp > aMaxPixelVal)
			    {
				*outputPtr = aMaxPixelVal;
				satCount++;
			    }
			else if (temp < 0x0)
			    {
				*outputPtr = 0;
			    }
			else
			    {
				*outputPtr = (TUint8)temp;
			    }

			outputPtr++;
			inputPtr++;
		    }
		outputPtr += (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);		
		inputPtr += (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);	
	    }

	//ALGO_Log_1("\n Saturation Count: %d", satCount);

	return EErrorNone;
    }

/**
 * Routine to apply full color filter.
 *
 * @param aInputChannel             [TAlgoImageDataRGB*] The input image in seperate R, G and B channel.
 * @param aOutputChannel            [TAlgoImageDataRGB*] The output image in seperate R, G and B channel.
 * @param aFilterParams             [TAlgoGenericFilterParams*] The generic parameters for filter.
 * @param aConvFilterParams         [TAlgoConvFilterParams*] The convolution parameters for filter.
 * @param aMaxPixelVal              [TUint8] The maximum pixel value.
 *
 * @return                          [TAlgoError] EErrorNone if the filtering is successful.
 */
TAlgoError ApplyFullColorEffects( TAlgoImageDataRGB* aInputChannel,
                                  TAlgoImageDataRGB* aOutputChannel,
						          TAlgoGenericFilterParams* aFilterParams,
						          TAlgoConvFilterParams * aConvFilterParams,
						          TUint8 aMaxPixelVal)
    {
    TUint32 startingOffset = aFilterParams->iStartIndexY * aFilterParams->iImageParams.iImageWidth
                             + aFilterParams->iStartIndexX;
	TUint32 offset = (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);

	//TReal64 temp = 0;
	//TUint32 satCount = 0;
    TUint32 x = 0;
    TUint32 y = 0;

	TUint8 *iR = aInputChannel->iRChannel + startingOffset;
	TUint8 *iG = aInputChannel->iGChannel + startingOffset;
	TUint8 *iB = aInputChannel->iBChannel + startingOffset;
	TUint8 *oR = aOutputChannel->iRChannel + startingOffset;
	TUint8 *oG = aOutputChannel->iGChannel + startingOffset;
	TUint8 *oB = aOutputChannel->iBChannel + startingOffset;

	/* For applying the convolution, basic problem is about the (N/2) pixel
	 * boundary, as convolution assumes N*N-1 neighbor around it.
	 * For the purpose of testing we will not apply any filter on the first
	 * pixels
	 */
    if (aFilterParams->iFilterType == EFilterEdgeDetection)
	    {
		for (y=0; y < aFilterParams->iPixelsToGrabY - 1; y++)
		    {
			for (x=0; x < aFilterParams->iPixelsToGrabX - 1; x++)
			    {
				//temp = 0;
				if( IMG_COLOR_DIF(*iR, *iG, *iB, *(oR + 1), *(oG + 1), *(oB + 1)) 
                    > aFilterParams->iParam 
                    ||
					IMG_COLOR_DIF(*iR, *iG, *iB, *(oR + aFilterParams->iImageParams.iImageWidth), 
                    *(oG + aFilterParams->iImageParams.iImageWidth), 
                    *(oB + aFilterParams->iImageParams.iImageWidth)) 
                    > aFilterParams->iParam)
				    {
					*oR = *oG = *oB = aMaxPixelVal;			
				    }
				else
				    {
					*oR = *oG = *oB = 0x0;			
				    }
				iR++;		
				iG++;		
				iB++;		
				oR++;		
				oG++;		
				oB++;			
			    }

			iR += offset;		
			iG += offset;		
			iB += offset;	
			oR += offset;
			oG += offset;
			oB += offset;	
		    }
	    }
	else if( aFilterParams->iFilterType == EFilterGrayScaleConversion)
	    {
		for (y=0; y < aFilterParams->iPixelsToGrabY; y++)
		    {   
			for (x=0; x < aFilterParams->iPixelsToGrabX; x++)
			    {
				*oR = *oG = *oB = (*iR + *iG + * iB)/3;			

				iR++;		
				iG++;		
				iB++;		
				oR++;		
				oG++;		
				oB++;			
			    }
			iR += offset;		
			iG += offset;		
			iB += offset;	
			oR += offset;
			oG += offset;
			oB += offset;	
		    }
	    }
    else if( aFilterParams->iFilterType == EFilterApplyColorTone)
	    {
		for (y=0; y < aFilterParams->iPixelsToGrabY; y++)
		    {   
			for (x=0; x < aFilterParams->iPixelsToGrabX; x++)
			    {
                *oR = (TUint8)(aConvFilterParams->iMatrix[0][0] * (*iR) + 
                               aConvFilterParams->iMatrix[0][1] * (*iG) + 
                               aConvFilterParams->iMatrix[0][2] * (*iB));   
                *oG = (TUint8)(aConvFilterParams->iMatrix[1][0] * (*iR) + 
                               aConvFilterParams->iMatrix[1][1] * (*iG) + 
                               aConvFilterParams->iMatrix[1][2] * (*iB));   
                *oB = (TUint8)(aConvFilterParams->iMatrix[2][0] * (*iR) + 
                               aConvFilterParams->iMatrix[2][1] * (*iG) + 
                               aConvFilterParams->iMatrix[2][2] * (*iB));   

			    iR++;		
			    iG++;		
			    iB++;		
			    oR++;		
			    oG++;		
			    oB++;			
                }
			iR += offset;		
			iG += offset;		
			iB += offset;	
			oR += offset;
			oG += offset;
			oB += offset;	
            }
        }

	
	else
	    {
		//ALGO_Log_1("\n ApplyFullColorEffects : Invalid Algo type : %d", aAlgoParams->iAlgoType);
		return EErrorArgument;
	    }

	//ALGO_Log_1("\n Saturation Count: %d", satCount);
    return EErrorNone;
    }

/**
 * Routine to find a sepia color tone.
 *
 * @param aInputChannel             [TAlgoImageDataRGB*] The input image in seperate R, G and B channel.
 * @param aOutputChannel            [TAlgoImageDataRGB*] The output image in seperate R, G and B channel.
 * @param aFilterParams             [TAlgoGenericFilterParams*] The generic parameters for filter.
 *
 * @return                          [TAlgoError] EErrorNone if the filtering is successful.
 */
TAlgoError FindSepiaColors (TAlgoImageDataRGB* aInputChannel,
                            TAlgoImageDataRGB* aOutputChannel,
                            TAlgoGenericFilterParams * aFilterParams)
    {
    TUint32 iNonSepiaPixelCount = 0;
    TUint32 x=0;
    TUint32 y=0;
    
    TUint32 startingOffset = aFilterParams->iStartIndexY * aFilterParams->iImageParams.iImageWidth + aFilterParams->iStartIndexX;
    TUint32 offset = (aFilterParams->iImageParams.iImageWidth - aFilterParams->iPixelsToGrabX);
 
    //TReal64 temp;
    //TUint32 satCount = 0;
    TReal64 hue = 0;
    TUint8 max, min, delta;
    TReal64 r;
    TReal64 g;
    TReal64 b;
 
    TUint8 *iR = aInputChannel->iRChannel + startingOffset;
    TUint8 *iG = aInputChannel->iGChannel + startingOffset;
    TUint8 *iB = aInputChannel->iBChannel + startingOffset;
    TUint8 *oR = aOutputChannel->iRChannel + startingOffset;
    TUint8 *oG = aOutputChannel->iGChannel + startingOffset;
    TUint8 *oB = aOutputChannel->iBChannel + startingOffset;
 
    /* For applying the convolution, basic problem is about the (N/2) pixel
    * boundary, as convolution assumes N*N-1 neighbor around it.
    * For the purpose of testing we will not apply any filter on the first
    * pixels
    */
 
    for (y=0; y < aFilterParams->iPixelsToGrabY; y++)
        {
        for (x=0; x < aFilterParams->iPixelsToGrabX; x++)
            {
#if 0
            if ((*iR >= *iG ) && (*iG > *iB) && ((*iG + *iB) > 0))
                {
                *oR = *iR;
                *oG = *iG;
                *oB = *iB;
                }
            else
                {
                *oR = 0;
                *oG = 0;
                *oB = 0;
                iNonSepiaPixelCount++;
                }
#else
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
         
            if ((hue >= 30) && (hue <= 60))
                {
                *oR = *iR;
                *oG = *iG;
                *oB = *iB;
                }
            else
                {
                iNonSepiaPixelCount++;
                *oR = 0;
                *oG = 0;
                *oB = 0;
                }
            }
        else
            {
            iNonSepiaPixelCount++;
            *oR = 0;
            *oG = 0;
            *oB = 0;
            }
#endif
            iR++;  
            iG++;  
            iB++;  
            oR++;  
            oG++;  
            oB++;   
            }
        iR += offset;  
        iG += offset;  
        iB += offset; 
        oR += offset;
        oG += offset;
        oB += offset; 
        }
    
    //ALGO_Log_1("\n iNonSepiaPixelCount: %d", iNonSepiaPixelCount);
    //ALGO_Log_1("\n Percentage of Non Sepia colors: %0.2f ", (TReal32)iNonSepiaPixelCount *100 / (TReal32) (aFileParams->iYPixelNum * aFileParams->iXPixelNum) );
 
    return EErrorNone;
    }

/**
 * Routine to apply a filter. Start point for all filters.
 *
 * @param aInputChannel             [TAlgoImageDataRGB*] The input image in seperate R, G and B channel.
 * @param aOutputChannel            [TAlgoImageDataRGB*] The output image in seperate R, G and B channel.
 * @param aFilterParams             [TAlgoGenericFilterParams*] The generic parameters for filter.
 * @param aConvFilterParams         [TAlgoConvFilterParams*] The convolution parameters for filter.
 *
 * @return                          [TAlgoError] EErrorNone if the filtering is successful.
 */
TAlgoError ApplyFilter(TAlgoImageDataRGB* aInputChannel,
                       TAlgoImageDataRGB* aOutputChannel,
                       TAlgoGenericFilterParams* aFilterParams,
				       TAlgoConvFilterParams * aConvFilterParams)
    {
	TUint8 maxRVal;
	TUint8 maxGVal;
	TUint8 maxBVal;
    TUint32 i = 0;
    TUint32 j = 0;

	/* Basic Parameter checks */
    /*
	if( aInputChannel == NULL ||
        aOutputChannel == NULL ||
		aFileParams == NULL ||
		aAlgoParams == NULL )
	        {
		    //ALGO_Log_0("\n ApplyFilter : Invalid input Params");
		    return EErrorArgument;
	        }
	*/

    if( aFilterParams->iImageParams.iImageFormat == EImageFormatBMP )
	    {
		maxRVal = maxGVal = maxBVal = 0xff;	
	    }
    else if( aFilterParams->iImageParams.iImageFormat == EImageFormat16bitRGB565 )
	    {
		maxRVal = maxBVal = 0x1f;		
		maxGVal = 0x3f;
	    }
    else if( aFilterParams->iImageParams.iImageFormat == EImageFormat16bitBGR565 )
	    {
		maxRVal = maxBVal = 0x1f;		
		maxGVal = 0x3f;
	    }
	else
	    {
		//ALGO_Log_0("\n ApplyFilter : Invalid Pixel Size");
		return EErrorArgument;
	    }

	switch (aFilterParams->iFilterType)
	    {
	    case EFilterConvolution:
		    {
		    for (i=0; i<N ; i++)
		        {
			    for (j=0; j<N ; j++)
			        {
				    aConvFilterParams->iMatrix[i][j] = matrix[aConvFilterParams->iConvFilterType][i][j];
			        }
		        }
		    ApplyConvFilter (aInputChannel->iRChannel, aOutputChannel->iRChannel, 
                                aFilterParams, aConvFilterParams, maxRVal);
		    ApplyConvFilter (aInputChannel->iGChannel, aOutputChannel->iGChannel, 
                                aFilterParams, aConvFilterParams, maxGVal);
		    ApplyConvFilter (aInputChannel->iBChannel, aOutputChannel->iBChannel, 
                                aFilterParams, aConvFilterParams, maxBVal);
		    }
		    break;

	    case EFilterDilate:
	    case EFilterErosion:
		    ApplyConvFilter (aInputChannel->iRChannel, aOutputChannel->iRChannel, 
                                aFilterParams, aConvFilterParams, maxRVal);
		    ApplyConvFilter (aInputChannel->iGChannel, aOutputChannel->iGChannel, 
                                aFilterParams, aConvFilterParams, maxGVal);
		    ApplyConvFilter (aInputChannel->iBChannel, aOutputChannel->iBChannel, 
                                aFilterParams, aConvFilterParams, maxBVal);
		    break;

	    case EFilterBrightness:
	    case EFilterContrast:
	    case EFilterGamma:
	    case EFilterNegative:
		    ApplyLocalFilter (aInputChannel->iRChannel, aOutputChannel->iRChannel, 
                                aFilterParams, maxRVal);
		    ApplyLocalFilter (aInputChannel->iGChannel, aOutputChannel->iGChannel, 
                                aFilterParams, maxGVal);
		    ApplyLocalFilter (aInputChannel->iBChannel, aOutputChannel->iBChannel, 
                                aFilterParams, maxBVal);
		    break;

	    case EFilterEdgeDetection:
	    case EFilterGrayScaleConversion:
        case EFilterApplyColorTone:
		    ApplyFullColorEffects (aInputChannel, aOutputChannel, aFilterParams, aConvFilterParams, maxGVal);
		    break;
        case EFilterFindSepiaColor:
             FindSepiaColors (aInputChannel, aOutputChannel, aFilterParams);
            break;

        default:
		    //ALGO_Log_1("\n ApplyFilter : Invalid Algo type : %d", aAlgoParams->iAlgoType);
		    return EErrorArgument;
	    }

	return EErrorNone;
    }
