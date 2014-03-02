/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_FILTER_ROUTINES_H__
#define __INCLUDE_ALGO_FILTER_ROUTINES_H__

/*
 * Includes 
 */
#include "algotypedefs.h"
#include "algoerror.h"
#include "algoutilities.h"


#ifdef __cplusplus
extern "C"
{
#endif 

/**
 * Filter Matrix
 */
TReal64 matrix[NUM_FILTERS][N][N] =
{
	/* Blur */
	{
		{0.11,0.11,0.11}, 
		{0.11,0.11,0.11},
		{0.11,0.11,0.11}
	}, 

	/* Sharpness */
	{
		{0,-1,0}, 
		{-1,5,-1},
		{0,-1,0}
	}, 

	/* Emboss */
	{
		{-2,-1,0}, 
		{-1,1,1},
		{0,1,2}
	}, 

	/* Contrast */
	{
		{0,0,0}, 
		{0,0.7,0},
		{0,0,0}
	}, 
	/* Edge Detection */
	{
		{0,1,0}, 
		{1,-4,1},
		{0,1,0}
	}
};

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
                           TUint8 aMaxPixelVal);

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
                            TUint8 aMaxPixelVal);

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
						          TUint8 aMaxPixelVal);

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
				       TAlgoConvFilterParams * aConvFilterParams);

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
                            TAlgoGenericFilterParams * aFilterParams);


#ifdef __cplusplus
}
#endif 


#endif	//__INCLUDE_ALGO_FILTER_ROUTINES_H__

