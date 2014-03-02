/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_UTILITIES_H__
#define __INCLUDE_ALGO_UTILITIES_H__

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"


#define LUMINANCE_R_COEF (0.299)
#define LUMINANCE_G_COEF (0.587)
#define LUMINANCE_B_COEF (0.114)


/**
 * Absolute value.
 *
 * @param a         Value.
 *
 * @return          Absolute value.
 */
#define ABS(a)  ((a) < 0 ? -(a) : (a))

/**
 * Minimum of 2 values.
 *
 * @param a         Value.
 * @param b         Value.
 *
 * @return          Minimum value.
 */
#define MIN_NUM(a, b)  ((a) < (b) ? (a) : (b))

/**
 * Maximum of 2 values.
 *
 * @param a         Value.
 * @param b         Value.
 *
 * @return          Maximum value.
 */
#define MAX_NUM(a, b)  ((a) > (b) ? (a) : (b))

/**
 * Gets the number of pixels of an image.
 *
 * @param imageparams   [TAlgoImageParams] Image Parameters.
 *
 * @return              [TUint32] Number of pixels.
 */
#define IMG_PIXELS(imageparams)  ((imageparams).iAlgoImageWidth * (imageparams).iAlgoImageHeight)

/**
 * Gets the color difference of the two RGB's.
 *
 * @param r             R Value for first image
 * @param g             G Value for first image
 * @param b             B Value for first image
 * @param r1            R Value for second image
 * @param g1            G Value for second image
 * @param b1            B Value for second image
 *
 * @return              Color Difference
 */
#define IMG_COLOR_DIF(r,g,b,r1,g1,b1)	SQUAREROOT((r-r1)*(r-r1)+(g-g1)*(g-g1)+(b-b1)*(b-b1))


#ifdef __cplusplus
extern "C"
{
#endif 

/**
 * Small number of a number array
 *
 * @param aNumArray         [TReal64] Array of Numbers.
 * @param aCount            [TReal64] Count of numbers in the array.
 *
 * @return                  Smallest number of the arguments.
 */
TReal64 Smallest_Number( TReal64* aNumArray, TUint32 aCount); 

/**
 * Power of a number
 *
 * @param aBase             [TReal64] Base Value.
 * @param aPower            [TReal64] Power Value.
 *
 * @return                  x power y.
 */
TReal64 POWER( TReal64 aBase, TReal64 aPower ); 

/**
 * Squareroot of a number
 *
 * @param aValue            [TReal64] Value of the number.
 *
 * @return                  [TReal64] Square root of the number.
 */
TReal64 SQUAREROOT( TReal64 aValue );

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
                           TReal64* aBAvg );

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
TAlgoError CalculateHistogram( TAlgoImage* aImage, TUint8* aHistogramR, TUint8* aHistogramG, TUint8* aHistogramB );

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
TAlgoError CompareHistogramDeviation( TUint8* aHistogram1, TUint8* aHistogram2, 
                                      TUint32 aMaxValue, TUint32* aPercentageDeviation) ;

/**
 * To compute the histogram equalization of an image
 *
 * @param aImageInput           [TAlgoImage*] Input Image.
 * @param aImageOutput          [TAlgoImage*] Equalized Image.
 *
 * @return                      [TAlgoError] EErrorNone if the histogram equalization is computed successfully.
 */
TAlgoError ComputeHistogramEqualization( TAlgoImage* aImageInput, 
                                         TAlgoImage* aImageOutput); 

/**
 * To calculate the image horizontal pixel differences
 *
 * @param aImage            [TAlgoImage] Image with Channel.
 * @param aPixelDiffR       [TReal64*] Horizontal pixel difference of Channel R.
 * @param aPixelDiffG       [TReal64*] Horizontal pixel difference of Channel G.
 * @param aPixelDiffB       [TReal64*] Horizontal pixel difference of Channel B.
 * @param aPixelDiffLuminance    [TReal64*] Horizontal pixel difference of Luminance.
 * @param aTestCoordinates       [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                  [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageHorizontalDiff( const TAlgoImage* aImage, 
                                         TReal64* aPixelDiffR, 
                                         TReal64* aPixelDiffG, 
                                         TReal64* aPixelDiffB,
										 TReal64* aPixelDiffLuminance,
										 const TAlgoImageTestCoordinates* aTestCoordinates);

/**
 * To calculate the image vertical pixel differences
 *
 * @param aImage            [TAlgoImage] Image with Channel.
 * @param aPixelDiffR       [TReal64*] Vertical pixel difference of Channel R.
 * @param aPixelDiffG       [TReal64*] Vertical pixel difference of Channel G.
 * @param aPixelDiffB       [TReal64*] Vertical pixel difference of Channel B.
 * @param aPixelDiffLuminance    [TReal64*] Horizontal pixel difference of Luminance.
 * @param aTestCoordinates       [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                  [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageVerticalDiff( const TAlgoImage* aImage, 
                                       TReal64* aPixelDiffR, 
                                       TReal64* aPixelDiffG, 
                                       TReal64* aPixelDiffB,
									   TReal64* aPixelDiffLuminance,
									   const TAlgoImageTestCoordinates* aTestCoordinates);

/**
 * To calculate the image horizontal pixel differences R
 *
 * @param aImage            [TAlgoImage] Image with Channel.
 * @param aPixelDiffR       [TReal64*] Horizontal pixel difference of Channel R.
 * @param aTestCoordinates       [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                  [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageHorizontalDiff_R( const TAlgoImage* aImage, 
                                         TReal64* aPixelDiffR, 
										 const TAlgoImageTestCoordinates* aTestCoordinates);

/**
 * To calculate the image vertical pixel differences R
 *
 * @param aImage            [TAlgoImage] Image with Channel.
 * @param aPixelDiffR       [TReal64*] Vertical pixel difference of Channel R.
 * @param aTestCoordinates       [TAlgoImageTestCoordinates*] TestCoordinates.
 *
 * @return                  [TAlgoError] EErrorNone if the differences is computed successfully.
 */
TAlgoError CalculateImageVerticalDiff_R( const TAlgoImage* aImage, 
                                       TReal64* aPixelDiffR, 
 									   const TAlgoImageTestCoordinates* aTestCoordinates);


/**
 * The function returns non-zero if its argument is some sort of space 
 *
 * @param aChar             [TInt8] Base Value.
 *
 * @return                  The function returns non-zero if its argument is some sort of space 
 *                          (i.e. single space, tab, vertical tab, form feed, carriage return, or newline). 
 *                          Otherwise, zero is returned.
 */
TUint8 IsSpace(TInt8 aChar);

/**
 * The function returns the ceiling of the number passed. 
 *
 * @param aNumber           [TReal32] Base Value.
 *
 * @return                  The ceiling value of the argument.
 */
TReal32 Ceiling(TReal32 aNumber);

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
                                       TUint32 aOutputHeight);

/**
 * The function returns non-zero if its argument is some number i.e 0-9. 
 *
 * @param aChar             [TInt8] Base Value.
 *
 * @return                  The function returns non-zero if its argument is some number i.e 0-9. 
 *                          Otherwise, zero is returned.
 */
TUint8 IsDigit(TInt8 aChar);

/**
 * The function converts the character string into double. 
 *
 * @param aStr              [TInt8*] String Value.
 *
 * @return                  The function returns double conversion of the string value if its successfull. 
 *                          Otherwise, zero is returned.
 */
TReal64 ALGO_ATOF(const TInt8* aStr);

TAlgoError GetLumaOn_R_Channel( const TAlgoImage* aImageEffectDisable, const TAlgoImage* aImageEffectEnable, TReal64 *p_Avg_Disable, TReal64 *p_Avg_Enable);

TAlgoError GetLumaOn_R_Channel_And_Normalize( const TAlgoImage* aImageEffectDisable, const TAlgoImage* aImageEffectEnable );

TAlgoError writte_rectangle_in_channel( TUint8* iChannel, TUint8 value, TAlgo_RELATIVERECTTYPE* aRELATIVERECTTYPE);

#ifdef __cplusplus
}
#endif 

#endif	//__INCLUDE_ALGO_UTILITIES_H__


