/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoutilities.c
* \brief    Contains Algorithm Library Utility Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algofileroutines.h"
#include "algoutilities.h"
#include "algodebug.h"

/**
 * Small number of a number array
 *
 * @param aNumArray         [TReal64] Array of Numbers.
 * @param aCount            [TReal64] Count of numbers in the array.
 *
 * @return                  Smallest number of the arguments.
 */
TReal64 Smallest_Number( TReal64* aNumArray, TUint32 aCount)
    {
    TUint32 x = 0;
    TReal64 num = *aNumArray;

    for( x=0; x<aCount; x++)
        {
        if( num > *aNumArray)
            {
            num = *aNumArray;
            }
        aNumArray++;
        }

    return num;
    }

/**
 * Power of a number
 *
 * @param aBase             [TReal64] Base Value.
 * @param aPower            [TReal64] Power Value.
 *
 * @return                  x power y.
 */
TReal64 POWER( TReal64 aBase, TReal64 aPower ) 
    {
    TReal64 result = 1;
    TReal64 loop = 0;          
    
    if( aPower > 0 )
        {
        for( loop=1; loop<=aPower; loop++ )
            { 
            result *= aBase;
            }
        }
    else if( aPower < 0 )
        {
        for( loop=aPower; loop<0; loop++)
            {
            result /= aBase; 
            }
        }
    return result;
    }  

/**
 * Squareroot of a number
 *
 * @param aValue            [TReal64] Value of the number.
 *
 * @return                  [TReal64] Square root of the number.
 */
TReal64 SQUAREROOT( TReal64 aValue )
    {
    TReal64 loop1 = 0;
    TReal64 x1,x2;
    TUint8 loop2 = 0;
    
    while( ( loop1 * loop1 ) <= aValue )
        {
        loop1+=0.1;
        }

    x1 = loop1;
   
    for( loop2=0; loop2<10; loop2++)
        {
        x2 = aValue;
        x2 /= x1;
        x2 += x1;
        x2 /= 2;
        x1 = x2;
        }

    return x2;
    }

/**
 * To calculate the average of the given channel
 *
 * @param aImage            [TAlgoImage*] Image with Channel.
 * @param aTestCoordinates  [TAlgoImageTestCoordinates*] Image Area to test.
 * @param aRAvg             [TReal64*] R channel average of the Image.
 * @param aGAvg             [TReal64*] G channel average of the Image.
 * @param aBAvg             [TReal64*] B channel average of the Image.
 *
 * @return                  [TAlgoError] EErrorNone if the average is computed successfully.
 */
TAlgoError Channel_Average(const TAlgoImage* aImage, 
                           const TAlgoImageTestCoordinates* aTestCoordinates,
                           TReal64* aRAvg, 
                           TReal64* aGAvg, 
                           TReal64* aBAvg )
    {
    TUint32 loopX = 0;
    TUint32 loopY = 0;

    TUint8* RChannel = (aImage->iImageData.iRChannel) + 
                       (((aTestCoordinates->iStartIndexY) * 
                        aImage->iImageParams.iImageWidth) + 
                        aTestCoordinates->iStartIndexX);
    TUint8* GChannel = aImage->iImageData.iGChannel + 
                       (((aTestCoordinates->iStartIndexY) * 
                        aImage->iImageParams.iImageWidth) + 
                        aTestCoordinates->iStartIndexX);
    TUint8* BChannel = aImage->iImageData.iBChannel + 
                       (((aTestCoordinates->iStartIndexY) * 
                        aImage->iImageParams.iImageWidth) + 
                        aTestCoordinates->iStartIndexX);

	*aRAvg = 0;
    *aGAvg = 0; 
    *aBAvg = 0;

    for( loopY=0; loopY<aTestCoordinates->iPixelsToGrabY; loopY++)
        {
        for( loopX=0; loopX<aTestCoordinates->iPixelsToGrabX; loopX++)
            {
            *aRAvg = *aRAvg + *RChannel;
            *aGAvg = *aGAvg + *GChannel;
            *aBAvg = *aBAvg + *BChannel;

            RChannel++;
            GChannel++;
            BChannel++;
            }

        RChannel = RChannel + (aImage->iImageParams.iImageWidth - aTestCoordinates->iPixelsToGrabX);
        GChannel = GChannel + (aImage->iImageParams.iImageWidth - aTestCoordinates->iPixelsToGrabX);;
        BChannel = BChannel + (aImage->iImageParams.iImageWidth - aTestCoordinates->iPixelsToGrabX);;
        }

    *aRAvg = *aRAvg/(aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
    *aGAvg = *aGAvg/(aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
    *aBAvg = *aBAvg/(aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);

    return EErrorNone;
    }

/**
 * To calculate the histogram of the given channel
 *
 * @param aImage            [TAlgoImage] Image with Channel.
 * @param aHistogramR       [TUint8*] Histogram of Channel R.
 * @param aHistogramG       [TUint8*] Histogram of Channel G.
 * @param aHistogramB       [TUint8*] Histogram of Channel B.
 *
 * @return                  [TAlgoError] EErrorNone if the histogram is computed successfully.
 */
TAlgoError CalculateHistogram( TAlgoImage* aImage,
				      TUint8* aHistogramR,
                      TUint8* aHistogramG,
                      TUint8* aHistogramB )
    {
    TUint32 loop = 0;
    
    TUint8* RChannel = aImage->iImageData.iRChannel;
    TUint8* GChannel = aImage->iImageData.iGChannel;
    TUint8* BChannel = aImage->iImageData.iBChannel;

    do
        {
        aHistogramR[*RChannel]++;
        aHistogramG[*GChannel]++;
        aHistogramB[*BChannel]++;

        RChannel++;
        GChannel++;
        BChannel++;

        loop++;
        }
        while(loop<(aImage->iImageParams.iImageWidth * aImage->iImageParams.iImageHeight));

    return EErrorNone;
    }

/**
 * To compare the histogram deviation between two histograms
 *
 * @param aHistogram1           [TUint8*] First Histogram.
 * @param aHistogram2           [TUint8*] Second Histogram.
 * @param aMaxValue             [TUint32] Max value of the Histogram.
 * @param aPercentageDeviation  [TReal32*] Percentage Deviation between Histograms.
 *
 * @return                      [TAlgoError] EErrorNone if the histogram deviation is computed successfully.
 */
TAlgoError CompareHistogramDeviation( TUint8* aHistogram1, 
                                      TUint8* aHistogram2, 
                                      TUint32 aMaxValue,
                                      TUint32* aPercentageDeviation) 
    {
    TUint32 loop = 0;
    TUint32 error = 0;
	TUint32 size = 0;

    do
        {
        if (aHistogram1[loop] > aHistogram2[loop]) 
		    {
			error += (aHistogram1[loop] - aHistogram2[loop]);
		    }
		else
		    {
			error -= (aHistogram1[loop] - aHistogram2[loop]);
		    }				
	
		size += aHistogram1[loop];
        
        loop++;
        }
        while( loop < aMaxValue );

    *aPercentageDeviation = (error * 100) / size;
	
	return EErrorNone;
    }

/**
 * To compute the histogram equalization of an image
 *
 * @param aImageInput           [TAlgoImage*] Input Image.
 * @param aImageOutput          [TAlgoImage*] Equalized Image.
 *
 * @return                      [TAlgoError] EErrorNone if the histogram equalization is computed successfully.
 */
TAlgoError ComputeHistogramEqualization( TAlgoImage* aImageInput, 
                                         TAlgoImage* aImageOutput) 
    {
    TUint32 loop = 0;
    TReal64 alpha = 0;

    TUint32 HistogramR[256] = {0};
    TUint32 HistogramG[256] = {0};
    TUint32 HistogramB[256] = {0};

    //TUint32 HistogramR1[256] = {0};

    do
        {
        HistogramR[aImageInput->iImageData.iRChannel[loop]]++;
        HistogramG[aImageInput->iImageData.iGChannel[loop]]++;
        HistogramB[aImageInput->iImageData.iBChannel[loop]]++;

        loop++;
        }
        while(loop<(aImageInput->iImageParams.iImageWidth * aImageInput->iImageParams.iImageHeight));
        
        //for(i=0; i<imlen; i++) {
        //      hist[im[i]]++;
        //}

     loop = 1;
     do
        {
        HistogramR[loop] += HistogramR[loop-1];
        HistogramG[loop] += HistogramG[loop-1];
        HistogramB[loop] += HistogramB[loop-1];

        loop++;
        }
        while(loop < 256);

        //for(i=1; i<256; i++) {
        //      hist[i] += hist[i-1];
        //}

    alpha = 255.0/(aImageInput->iImageParams.iImageWidth * aImageInput->iImageParams.iImageHeight);

    loop = 0;
    do
        {
        
        aImageOutput->iImageData.iRChannel[loop] = (TUint8)(HistogramR[aImageInput->iImageData.iRChannel[loop]] * alpha);
        aImageOutput->iImageData.iGChannel[loop] = (TUint8)(HistogramG[aImageInput->iImageData.iGChannel[loop]] * alpha);
        aImageOutput->iImageData.iBChannel[loop] = (TUint8)(HistogramB[aImageInput->iImageData.iBChannel[loop]] * alpha);
       
        loop++;
        }
        while(loop<(aImageInput->iImageParams.iImageWidth * aImageInput->iImageParams.iImageHeight));
        
        //for(i=0; i<imlen; i++) {
        //  newim[i] = 255 * hist[im[i]] / imlen;

    return EErrorNone;
    }

/**
 * To calculate the image horizontal pixel differences
 *
 * @param aImage                [TAlgoImage] Image with Channel.
 * @param aPixelDiffR           [TReal64*] Horizontal pixel difference of Channel R.
 * @param aPixelDiffG           [TReal64*] Horizontal pixel difference of Channel G.
 * @param aPixelDiffB           [TReal64*] Horizontal pixel difference of Channel B.
 * @param aPixelDiffLuminance   [TReal64*] Horizontal pixel difference of Luminance.
 * @param aTestCoordinates      [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                      [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageHorizontalDiff( const TAlgoImage* aImage, 
                                         TReal64* aPixelDiffR, 
                                         TReal64* aPixelDiffG, 
                                         TReal64* aPixelDiffB,
										 TReal64* aPixelDiffLuminance,
										 const TAlgoImageTestCoordinates* aTestCoordinates)
    {
    TUint32 width = 0;
    TUint32 height = 0;
    TUint32 index = 0;
	TUint32 counter = 0;
	TReal64 Luminance_A = 0, Luminance_B = 0; 

    *aPixelDiffR = 0;
    *aPixelDiffG = 0;
    *aPixelDiffB = 0;
	*aPixelDiffLuminance = 0;

    for (height = 0; height < aImage->iImageParams.iImageHeight; height++)
        {
        for (width = 0; width < aImage->iImageParams.iImageWidth; width++)
            {
            index = height * aImage->iImageParams.iImageWidth + width;
			if ( (index > 0) 
				&& (index - (height * aImage->iImageParams.iImageWidth) >  aTestCoordinates->iStartIndexX) 
				&& (index - (height * aImage->iImageParams.iImageWidth) <= aTestCoordinates->iStartIndexX + aTestCoordinates->iPixelsToGrabX)
				&& (height >  aTestCoordinates->iStartIndexY) 
				&& (height <= aTestCoordinates->iStartIndexY + aTestCoordinates->iPixelsToGrabY)
				)
                {
                *aPixelDiffR += ABS(aImage->iImageData.iRChannel[index] - aImage->iImageData.iRChannel[index-1]);
                *aPixelDiffG += ABS(aImage->iImageData.iGChannel[index] - aImage->iImageData.iGChannel[index-1]);
 				*aPixelDiffB += ABS(aImage->iImageData.iBChannel[index] - aImage->iImageData.iBChannel[index-1]);
				Luminance_A = LUMINANCE_R_COEF * aImage->iImageData.iRChannel[index]
							+ LUMINANCE_G_COEF * aImage->iImageData.iGChannel[index]
							+ LUMINANCE_B_COEF * aImage->iImageData.iBChannel[index];
				Luminance_B = LUMINANCE_R_COEF * aImage->iImageData.iRChannel[index-1]
							+ LUMINANCE_G_COEF * aImage->iImageData.iGChannel[index-1]
							+ LUMINANCE_B_COEF * aImage->iImageData.iBChannel[index-1];
				*aPixelDiffLuminance += ABS(Luminance_A - Luminance_B);
				counter++;
                //ALGO_Log_1("counter = %d\n", counter);
                }
		    }
        }

	//ALGO_Log_1("counter = %d\n", counter);
	//ALGO_Log_1("aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
    return EErrorNone;
    }

/**
 * To calculate the image vertical pixel differences
 *
 * @param aImage                [TAlgoImage] Image with Channel.
 * @param aPixelDiffR           [TReal64*] Vertical pixel difference of Channel R.
 * @param aPixelDiffG           [TReal64*] Vertical pixel difference of Channel G.
 * @param aPixelDiffB           [TReal64*] Vertical pixel difference of Channel B.
 * @param aPixelDiffLuminance   [TReal64*] Horizontal pixel difference of Luminance.
 * @param aTestCoordinates      [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                      [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageVerticalDiff( const TAlgoImage* aImage, 
                                       TReal64* aPixelDiffR, 
                                       TReal64* aPixelDiffG, 
                                       TReal64* aPixelDiffB,
									   TReal64* aPixelDiffLuminance,
									   const TAlgoImageTestCoordinates* aTestCoordinates)
    {
    TUint32 width = 0;
    TUint32 height = 0;
    TUint32 index = 0;
	TUint32 counter = 0;
	TReal64 Luminance_A = 0, Luminance_B = 0; 

    *aPixelDiffR = 0;
    *aPixelDiffG = 0;
    *aPixelDiffB = 0;
	*aPixelDiffLuminance = 0;

    for (height = 0; height < aImage->iImageParams.iImageHeight; height++)
        {
		for (width = 0; width < aImage->iImageParams.iImageWidth; width++)
            {
			index = height * aImage->iImageParams.iImageWidth + width;
			if ( (index > aImage->iImageParams.iImageWidth)
				&& (index - (height * aImage->iImageParams.iImageWidth) >  aTestCoordinates->iStartIndexX) 
				&& (index - (height * aImage->iImageParams.iImageWidth) <= aTestCoordinates->iStartIndexX + aTestCoordinates->iPixelsToGrabX)
				&& (height >  aTestCoordinates->iStartIndexY) 
				&& (height <= aTestCoordinates->iStartIndexY + aTestCoordinates->iPixelsToGrabY)
				)
			    {
				*aPixelDiffR += ABS(aImage->iImageData.iRChannel[index] - aImage->iImageData.iRChannel[index-aImage->iImageParams.iImageWidth]);
				*aPixelDiffG += ABS(aImage->iImageData.iGChannel[index] - aImage->iImageData.iGChannel[index-aImage->iImageParams.iImageWidth]);
				*aPixelDiffB += ABS(aImage->iImageData.iBChannel[index] - aImage->iImageData.iBChannel[index-aImage->iImageParams.iImageWidth]);
				Luminance_A = LUMINANCE_R_COEF * aImage->iImageData.iRChannel[index]
							+ LUMINANCE_G_COEF * aImage->iImageData.iGChannel[index]
							+ LUMINANCE_B_COEF * aImage->iImageData.iBChannel[index];
				Luminance_B = LUMINANCE_R_COEF * aImage->iImageData.iRChannel[index-aImage->iImageParams.iImageWidth]
							+ LUMINANCE_G_COEF * aImage->iImageData.iGChannel[index-aImage->iImageParams.iImageWidth]
							+ LUMINANCE_B_COEF * aImage->iImageData.iBChannel[index-aImage->iImageParams.iImageWidth];
				*aPixelDiffLuminance += ABS(Luminance_A - Luminance_B);
				counter++;
			    }
            }
        }

	//ALGO_Log_1("counter = %d\n", counter);
	//ALGO_Log_1("aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
	return EErrorNone;
 }


/**
 * To calculate the image horizontal pixel differences R
 *
 * @param aImage                [TAlgoImage] Image with Channel.
 * @param aPixelDiffR           [TReal64*] Horizontal pixel difference of Channel R.
 * @param aTestCoordinates      [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                      [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageHorizontalDiff_R( const TAlgoImage* aImage, 
                                         TReal64* aPixelDiffR, 
										 const TAlgoImageTestCoordinates* aTestCoordinates)
    {
    TUint32 width = 0;
    TUint32 height = 0;
    TUint32 index = 0;
	TUint32 counter = 0;

    *aPixelDiffR = 0;

    for (height = 0; height < aImage->iImageParams.iImageHeight; height++)
        {
        for (width = 0; width < aImage->iImageParams.iImageWidth; width++)
            {
            index = height * aImage->iImageParams.iImageWidth + width;
			if ( (index > 0) 
				&& (index - (height * aImage->iImageParams.iImageWidth) >  aTestCoordinates->iStartIndexX) 
				&& (index - (height * aImage->iImageParams.iImageWidth) <= aTestCoordinates->iStartIndexX + aTestCoordinates->iPixelsToGrabX)
				&& (height >  aTestCoordinates->iStartIndexY) 
				&& (height <= aTestCoordinates->iStartIndexY + aTestCoordinates->iPixelsToGrabY)
				)
                {
                *aPixelDiffR += ABS(aImage->iImageData.iRChannel[index] - aImage->iImageData.iRChannel[index-1]);
				counter++;
                //ALGO_Log_1("counter = %d\n", counter);
                }
		    }
        }

	ALGO_Log_1("fct %s\n", __FUNCTION__);
	ALGO_Log_1("counter = %d\n", counter);
	ALGO_Log_1("iPixelsToGrabX * iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
	ALGO_Log_2("iPixelsToGrabX = %d iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX, aTestCoordinates->iPixelsToGrabY);
    return EErrorNone;
    }

/**
 * To calculate the image vertical pixel differences R
 *
 * @param aImage                [TAlgoImage] Image with Channel.
 * @param aPixelDiffR           [TReal64*] Vertical pixel difference of Channel R.
 * @param aTestCoordinates      [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                      [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageVerticalDiff_R( const TAlgoImage* aImage, 
                                       TReal64* aPixelDiffR, 
									   const TAlgoImageTestCoordinates* aTestCoordinates)
    {
    TUint32 width = 0;
    TUint32 height = 0;
    TUint32 index = 0;
	TUint32 counter = 0;

    *aPixelDiffR = 0;

    for (height = 0; height < aImage->iImageParams.iImageHeight; height++)
        {
		for (width = 0; width < aImage->iImageParams.iImageWidth; width++)
            {
			index = height * aImage->iImageParams.iImageWidth + width;
			if ( (index > aImage->iImageParams.iImageWidth)
				&& (index - (height * aImage->iImageParams.iImageWidth) >  aTestCoordinates->iStartIndexX) 
				&& (index - (height * aImage->iImageParams.iImageWidth) <= aTestCoordinates->iStartIndexX + aTestCoordinates->iPixelsToGrabX)
				&& (height >  aTestCoordinates->iStartIndexY) 
				&& (height <= aTestCoordinates->iStartIndexY + aTestCoordinates->iPixelsToGrabY)
				)
			    {
				*aPixelDiffR += ABS(aImage->iImageData.iRChannel[index] - aImage->iImageData.iRChannel[index-aImage->iImageParams.iImageWidth]);
				counter++;
			    }
            }
        }

	ALGO_Log_1("fct %s\n", __FUNCTION__);
	ALGO_Log_1("counter = %d\n", counter);
	ALGO_Log_1("iPixelsToGrabX * iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX * aTestCoordinates->iPixelsToGrabY);
	ALGO_Log_2("iPixelsToGrabX = %d iPixelsToGrabY = %d\n", aTestCoordinates->iPixelsToGrabX, aTestCoordinates->iPixelsToGrabY);
	return EErrorNone;
 }

// Histogram Draw Function
TAlgoError DrawHistogram( TUint32* aHistogram, TUint32 aMaxValue )
    {
    TUint32 loop1 =0;
    TUint32 loop2 = 0;

    ALGO_Log_0("Array Element    Value in Element    Histogram \n");
    ALGO_Log_0("-------------    ----------------    --------- \n");
   
    for ( loop1 =0; loop1 < aMaxValue; loop1 = loop1+8)
        {
        ALGO_Log_1("%7d", loop1);
        ALGO_Log_1("%19d           ", aHistogram[loop1]);

        for (loop2 = 0; loop2 < aHistogram[loop1]; loop2 = loop2+8){
            ALGO_Log_0("*");                                 
		}
        ALGO_Log_0("\n");
        }

    ALGO_Log_0("\n\n");
    
    return EErrorNone;
    }

/**
 * The function returns non-zero if its argument is some sort of space 
 *
 * @param aChar             [TInt8] Base Value.
 *
 * @return                  The function returns non-zero if its argument is some sort of space 
 *                          (i.e. single space, tab, vertical tab, form feed, carriage return, or newline). 
 *                          Otherwise, zero is returned.
 */
TUint8 IsSpace(TInt8 aChar)
    {
    if( aChar == ' ' || aChar == '\t' || aChar == '\n' )
        return 1;
    else
        return 0;
    }

/**
 * The function returns the ceiling of the number passed. 
 *
 * @param aNumber           [TReal32] Base Value.
 *
 * @return                  The ceiling value of the argument.
 */
TReal32 Ceiling(TReal32 aNumber)
    {
    TInt32 temp = (TInt32)(aNumber);

    return (TReal32)temp;
    }

/**
 * The function resizes the input buffer using bilinear interpolation. 
 *
 * @param aInput             [TUint8*] Input Buffer.
 * @param aOutput            [TUint8*] Output Buffer.
 * @param aInputWidth        [TUint32] Width of the input Buffer.
 * @param aInputHeight       [TUint32] Height of the input Buffer.
 * @param aOutputWidth       [TUint32] Width of the output Buffer.
 * @param aOutputHeight      [TUint32] Height of the output Buffer.
 *
 * @return                   [TAlgoError] EErrorNone if the bilinear interpolation is computed successfully.
 */
TAlgoError ResizeBilinearInterpolation(TUint8* aInput, 
                                       TUint8* aOutput,
                                       TUint32 aInputWidth,
                                       TUint32 aInputHeight,
                                       TUint32 aOutputWidth,
                                       TUint32 aOutputHeight)
    {
    TReal64 nXFactor = (TReal64)aInputWidth/(TReal64)aOutputWidth;
    TReal64 nYFactor = (TReal64)aInputHeight/(TReal64)aOutputHeight;

    TReal64 fraction_x, fraction_y, one_minus_x, one_minus_y;
    TInt32 ceil_x, ceil_y, floor_x, floor_y;
    TUint8 color;
    TUint8 b1, b2;
    TUint8 c1, c2, c3, c4;
    TUint32 x = 0;
    TUint32 y = 0;
	TUint32 tmp =0;

    for ( x = 0; x < aOutputWidth; ++x)
	{
        for ( y = 0; y < aOutputHeight; ++y)
        {
            // Setup
            floor_x = (TInt32)(x * nXFactor);
            floor_y = (TInt32)(y * nYFactor);

            ceil_x = floor_x + 1;
            if (ceil_x >= aInputWidth) 
                ceil_x = floor_x;

            ceil_y = floor_y + 1;
            if (ceil_y >= aInputHeight) 
                ceil_y = floor_y;

            fraction_x = x * nXFactor - floor_x;
            fraction_y = y * nYFactor - floor_y;

            one_minus_x = 1.0 - fraction_x;
            one_minus_y = 1.0 - fraction_y;
            
            c1 = aInput[(floor_y * aInputWidth) + floor_x];
            c2 = aInput[(floor_y * aInputWidth) + ceil_x];
            c3 = aInput[(ceil_y * aInputWidth) + floor_x];
            c4 = aInput[(ceil_y * aInputWidth) + ceil_x];

            b1 = (TUint8)(one_minus_x * c1 + fraction_x * c2);
            b2 = (TUint8)(one_minus_x * c3 + fraction_x * c4);
            
            color = (TUint8)(one_minus_y * (TReal64)(b1) + fraction_y * (TReal64)(b2));
			tmp = (TUint32)(one_minus_y * (TReal64)(b1) + fraction_y * (TReal64)(b2));
			if(tmp != color)
			{
				ALGO_Log_2("\t error color = %d != %d = tmp\n", color, tmp);
			}

            aOutput[(y * aOutputWidth) + x] = color;
   			//ALGO_Log_1("\t resise color = %d\n", color);
       }
  		//ALGO_Log_3("\t\t\t  x=%d, y=%d, aOutput[(y * aOutputWidth) + x]=%d\n", x, y, aOutput[(y * aOutputWidth) + x]);
  		//ALGO_Log_4("c1=%d, c2=%d, c3=%d, c4=%d\n", c1, c2, c3, c4);
  		//ALGO_Log_4("one_minus_x=%2.f, c1=%d, fraction_x=%2.f, c2=%d\n", one_minus_x, c1, fraction_x, c2);
  		//ALGO_Log_4("one_minus_x=%2.f, c3=%d, fraction_x=%2.f, c4=%d\n", one_minus_x, c3, fraction_x, c4);
  		//ALGO_Log_4("one_minus_y=%2.f, b1=%d, fraction_y=%2.f, b2=%d\n", one_minus_y, b1, fraction_y, b2);
	}
    /*
    TUint32 i=0; 
    TUint32 j=0;
    TUint32 x1=0;
    TUint32 x2=0;
    TUint32 y1=0;
    TUint32 y2=0;

    TUint32 XX1 = 0;
    TUint32 XX2 = 0;
    TUint32 YY1 = 0;
    TUint32 YY2 = 0;

    TReal64 x = 0;
    TReal64 y = 0;
    TReal64 delta_11 = 0;
    TReal64 delta_12 = 0;
    TReal64 delta_21 = 0;
    TReal64 delta_22 = 0;
    
    TReal64 fac = 0;

    TUint32 m = aInputWidth;
    TUint32 n = aInputHeight;
    TUint32 aM = aOutputWidth;
    TUint32 aN = aOutputHeight;

    for( i=0; i<aM; i++)
        {
        for( j=0; j<aN; j++)
            {  
            x1 = (TInt32)(i*m/aM);
            x2 = x1 + 1;

            y1 = (TInt32)(j*n/aN);
            y2 = y1 + 1;

            x = ((TReal64)(i/aM))*m;
            y = ((TReal64)(j/aN))*n;

            delta_11 = (x2-x)*(y2-y);
            delta_12 = (x2-x)*(y-y1);
            delta_21 = (x-x1)*(y2-y);
            delta_22 = (x-x1)*(y-y1);

            fac=(x2-x1)*(y2-y1);

            XX1=MIN_NUM(x1+1,m);
            XX2=MIN_NUM(x2+1,m);
            YY1=MIN_NUM(y1+1,n);
            YY2=MIN_NUM(y2+1,n);

            //grid_out(i+1,j+1)=(delta_11*grid_in(X1,Y1)+delta_12*grid_in(X1,Y2)+delta_21*grid_in(X2,Y1)+delta_22*grid_in(X2,Y2))/fac;  
            aOutput[i+1,j+1]=(delta_11 * aInput[XX1,YY1]+
                              delta_12 * aInput[XX1,YY2]+
                              delta_21 * aInput[XX2,YY1]+
                              delta_22 * aInput[XX2,YY2])/fac;  
            }  
        }
    */
    return EErrorNone; 
    }

/**
 * The function returns non-zero if its argument is some number i.e 0-9. 
 *
 * @param aChar             [TInt8] Base Value.
 *
 * @return                  The function returns non-zero if its argument is some number i.e 0-9. 
 *                          Otherwise, zero is returned.
 */
TUint8 IsDigit(TInt8 aChar)
    {
    if( aChar >= 48 && aChar <= 57 )
        return 1;
    else
        return 0;
    }

/**
 * The function converts the character string into double. 
 *
 * @param aStr              [TInt8*] String Value.
 *
 * @return                  The function returns double conversion of the string value if its successfull. 
 *                          Otherwise, zero is returned.
 */
TReal64 ALGO_ATOF(const TInt8* aStr)
    {
    TReal64 number;
    TInt32 exponent;
    TInt32 negative;
    TInt8 *p = (TInt8 *) aStr;
    TReal64 p10;
    TInt32 n;
    TInt32 num_digits;
    TInt32 num_decimals;

    // Skip leading whitespace
    while (IsSpace(*p)) 
        {
        p++;
        }

    // Handle optional sign
    negative = 0;
    switch (*p) 
    {             
    case '-': 
        negative = 1; // Fall through to increment position
    case '+': 
        p++;
    }

    number = 0.;
    exponent = 0;
    num_digits = 0;
    num_decimals = 0;

    // Process string of digits
    while (IsDigit(*p))
        {
        number = number * 10. + (*p - '0');
        p++;
        num_digits++;
        }

    // Process decimal part
    if (*p == '.') 
        {
        p++;
        while (IsDigit(*p))
            {
            number = number * 10. + (*p - '0');
            p++;
            num_digits++;
            num_decimals++;
            }
        exponent -= num_decimals;
        }

    if (num_digits == 0)
        {
        return 0.0;
        }

    // Correct for sign
    if (negative) 
        {
        number = -number;
        }

    // Process an exponent string
    if (*p == 'e' || *p == 'E') 
        {
        // Handle optional sign
        negative = 0;
        switch(*++p) 
            {   
            case '-': 
                negative = 1;   // Fall through to increment pos
            case '+': 
                p++;
            }

    // Process string of digits
    n = 0;
    while (IsDigit(*p)) 
    {   
      n = n * 10 + (*p - '0');
      p++;
    }

    if (negative) 
      exponent -= n;
    else
      exponent += n;
    }

    // Scale the result
    p10 = 10.;
    n = exponent;
    if (n < 0) 
        {
        n = -n;
        }

    while (n) 
        {
        if (n & 1) 
            {   
            if (exponent < 0)
                {
                number /= p10;
                }
            else
                {
                number *= p10;
                }
            }
        n >>= 1;
        p10 *= p10;
        }

    return number;
    }

TAlgoError GetLumaOn_R_Channel( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
												 	TReal64 *p_Avg_Disable,
													TReal64 *p_Avg_Enable
)
{
	TUint32 index = 0;
	TReal64 Avg_Disable = 0;
	TReal64 Avg_Enable = 0;
    TReal64 Average = 0;
	TUint32 width = 0;
	TUint32 height = 0;

	*p_Avg_Disable = 0xff/2;
	*p_Avg_Enable = 0xff/2;
	if( (aImageEffectDisable->iImageParams.iImageWidth * aImageEffectDisable->iImageParams.iImageHeight) != (aImageEffectEnable->iImageParams.iImageWidth * aImageEffectEnable->iImageParams.iImageHeight) )
	{
		return EErrorImageNotRelevant;
	}
	width = aImageEffectDisable->iImageParams.iImageWidth; 
	height = aImageEffectDisable->iImageParams.iImageHeight;

    //Calculating the average of luma components of both images for normalization.
	for( index=0; index<(width * height); index++)
	{
		aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
    													  + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
	   													  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);
	    Avg_Disable += aImageEffectDisable->iImageData.iRChannel[index];
	}
	Avg_Disable = Avg_Disable/(width * height);
    ALGO_Log_1("GetLumaOn_R_Channel_And_Normalize Avg_Disable %1.6lf  \n", Avg_Disable);
	*p_Avg_Disable = Avg_Disable;

	for( index=0; index<(width * height); index++)
	{
		aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
    													 + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
	   													 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
	    Avg_Enable += aImageEffectEnable->iImageData.iRChannel[index];
	}
	Avg_Enable = Avg_Enable/(width * height);
    ALGO_Log_1("GetLumaOn_R_Channel_And_Normalize Avg_Enable %1.6lf  \n", Avg_Enable);
	*p_Avg_Enable = Avg_Enable;

	return EErrorNone;
}


TAlgoError GetLumaOn_R_Channel_And_Normalize( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable )
{
	TUint32 index = 0;
	TReal64 Avg_Disable = 0;
	TReal64 Avg_Enable = 0;
    TReal64 Average = 0;
	TReal64 Avg_EnableModified = 0;
	TUint32 width = 0;
	TUint32 height = 0;
	TAlgoError errorCode = EErrorNone;

	errorCode = GetLumaOn_R_Channel(aImageEffectDisable, aImageEffectEnable,&Avg_Disable,&Avg_Enable);
	if( EErrorNone != errorCode)
	{
		return EErrorImageNotRelevant; 
	}

    Average = (Avg_Disable/Avg_Enable);
	ALGO_Log_1("Avg_Disable/Avg_Enable =  %1.6lf \n", (double)(Average));

	// normalize image
	width = aImageEffectDisable->iImageParams.iImageWidth; 
	height = aImageEffectDisable->iImageParams.iImageHeight;
	if(Average <= 1.0)
	{
		for( index=0; index<(width * height); index++)
		{
			aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(Average* aImageEffectEnable->iImageData.iRChannel[index]);
			Avg_EnableModified += aImageEffectEnable->iImageData.iRChannel[index];
		}
	}
	else
	{
		TUint16 tmp_Value=0;
		for( index=0; index<(width * height); index++)
		{
			tmp_Value = (TUint16)(Average* aImageEffectEnable->iImageData.iRChannel[index]);
			if(0xFF >= tmp_Value)
			{
				aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)tmp_Value; 
			}
			else
			{
				// clip
				aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)0xFF; 
			}
			Avg_EnableModified += aImageEffectEnable->iImageData.iRChannel[index];
		}
	}
	Avg_EnableModified = Avg_EnableModified/(width * height);
    ALGO_Log_1("GetLumaOn_R_Channel_And_Normalize Avg_EnableModified %1.6lf  \n", Avg_EnableModified);

	return EErrorNone;
}

TAlgoError writte_rectangle_in_channel( TUint8* iChannel, TUint8 value, TAlgo_RELATIVERECTTYPE* aRELATIVERECTTYPE)
{
	TUint32 countW = 0, countH = 0;
	for(countH=0; countH < aRELATIVERECTTYPE->sReference_nHeight ; countH++)
	{
		for(countW=0; countW < aRELATIVERECTTYPE->sReference_nWidth ; countW++)
		{
			if(
				(countH == aRELATIVERECTTYPE->sRect_sTopLeft_nY) ||
				(countH == aRELATIVERECTTYPE->sRect_sTopLeft_nY + aRELATIVERECTTYPE->sRect_sSize_nHeight -1)
				)
			{
				if(
					(countW >= aRELATIVERECTTYPE->sRect_sTopLeft_nX) &&
					(countW <= aRELATIVERECTTYPE->sRect_sTopLeft_nX + aRELATIVERECTTYPE->sRect_sSize_nWidth -1)
					)
				{
					iChannel[countH*aRELATIVERECTTYPE->sReference_nWidth + countW] = value;
				}
			}
			else if(
				(countH > aRELATIVERECTTYPE->sRect_sTopLeft_nY) &&
				(countH < aRELATIVERECTTYPE->sRect_sTopLeft_nY + aRELATIVERECTTYPE->sRect_sSize_nHeight -1)
				)
			{
				if(
					(countW == aRELATIVERECTTYPE->sRect_sTopLeft_nX) ||
					(countW == aRELATIVERECTTYPE->sRect_sTopLeft_nX + aRELATIVERECTTYPE->sRect_sSize_nWidth -1)
					)
				{
					iChannel[countH*aRELATIVERECTTYPE->sReference_nWidth + countW] = value;
				}

			}
		}
	}
	return EErrorNone;
}
