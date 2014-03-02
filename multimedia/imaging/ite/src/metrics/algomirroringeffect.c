/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algomirroringeffect.c
* \brief    Contains Algorithm Library Spatial Similarity Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */
#define MIRROR_THRESOLD_VERTICAL	10
#define MIRROR_THRESOLD_HORIZONAL	10
#define MIRROR_THRESOLD_BOTH		10

/*
 * Includes 
 */
#include "algomirroringeffect.h"
#include "algoutilities.h"
#include "algodebug.h"

/**
 * Validation Metric for the Spatial Similarity Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricMirroringEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{		
		TUint32 index = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	    // compute luminance in order to work on it and store it in R channel
		for(index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * 
         aImageEffectDisable->iImageParams.iImageWidth); index++)
        {
        aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);

        aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
        }

		if (ETypeVerticalMirroring == aMetricParams->iType)
			return ValidationMetricVerticalMirroringEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
		else if (ETypeHorizontalMirroring == aMetricParams->iType)
			return ValidationMetricHorizontalMirroringEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
		else if (ETypeBothMirroring == aMetricParams->iType)
			return ValidationMetricBothMirroringEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
		return EErrorArgument;
}

/**
 * Validation Metric for the Horizontal Mirroring Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for horizontal mirroring.
	- Need to check nth row with (aMetricParams->iTestCoordinates.iPixelsToGrabY -1)-n th row.
	- Checking for column matrices is fine.
 */

TAlgoError ValidationMetricHorizontalMirroringEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;    
    TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
    TUint32 startingOffsetDisable = 0;
    TUint32 startingOffsetEnable = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);

     startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
     startingOffsetEnable = (aMetricParams->iTestCoordinates.iStartIndexY+1) * aImageEffectDisable->iImageParams.iImageWidth
                             - aMetricParams->iTestCoordinates.iStartIndexX -1;

	// check that linked columns are similar
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column
		indexEnable  = startingOffsetEnable;// Last Column
        colonimg1 = 0;
        colonimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
	        colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
			
            indexEnable = indexEnable + aImageEffectEnable->iImageParams.iImageWidth;
			indexDisable = indexDisable + aImageEffectDisable->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
        colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
        if(error1 > MIRROR_THRESOLD_HORIZONAL)
            {
            deviationCol++;
 			//ALGO_Log_2("MetricHorizontalMirroring : colonDiff = %d > MIRROR_THRESOLD_HORIZONAL = %d\n", error1, MIRROR_THRESOLD_HORIZONAL);
            }

	    startingOffsetDisable += 1; // next column
		startingOffsetEnable -= 1; // previous column
        }

    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aMetricParams->iTestCoordinates.iStartIndexY+1) * aImageEffectDisable->iImageParams.iImageWidth
                             - aMetricParams->iTestCoordinates.iStartIndexX - aMetricParams->iTestCoordinates.iPixelsToGrabX;

	// check that raws are similar
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Row
		indexEnable  = startingOffsetEnable;// Last Row  
        rowimg1 = 0;
        rowimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
            indexDisable++;
			indexEnable++;
            }
        rowimg1 = rowimg1/aImageEffectDisable->iImageParams.iImageWidth;
		rowimg2 = rowimg2/aImageEffectEnable->iImageParams.iImageWidth;

        rowDiff = rowimg1 - rowimg2;
        error1 = ABS(rowDiff);
        if(error1 > MIRROR_THRESOLD_HORIZONAL)
            {
            deviationRow++;
 			//ALGO_Log_2("MetricHorizontalMirroring : rowDiff = %d > MIRROR_THRESOLD_HORIZONAL = %d\n", error1, MIRROR_THRESOLD_HORIZONAL);
            }

		startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth; // next row
		startingOffsetEnable += aImageEffectEnable->iImageParams.iImageWidth; // next row
        }

    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
  
	ALGO_Log_4("DeviationCol = %10d, DeviationRow = %10d, ToleranceCol = %10f, ToleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);

    if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNone;
        }
    else
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNotValidated;
        }
}


/**
 * Validation Metric for the Vertical Mirroring Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for vertical mirroring.
	- Need to check nth column with (aMetricParams->iTestCoordinates.iPixelsToGrabX -1)-n th column.
	- Checking for row matrices is fine.
 */

TAlgoError ValidationMetricVerticalMirroringEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
    TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0;
    TUint32 startingOffsetEnable = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);

	startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY - aMetricParams->iTestCoordinates.iPixelsToGrabY) * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX ;

    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column
		indexEnable  = startingOffsetEnable;// First Column   
        colonimg1 = 0;
        colonimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
			indexEnable = indexEnable + aImageEffectEnable->iImageParams.iImageWidth;
			indexDisable = indexDisable + aImageEffectDisable->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
        colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
        if(error1 > MIRROR_THRESOLD_VERTICAL)
            {
            deviationCol++;
  			//ALGO_Log_2("MetricVerticalMirroring : colonDiff = %d > MIRROR_THRESOLD_VERTICAL = %d\n", error1, MIRROR_THRESOLD_VERTICAL);
            }

	    startingOffsetDisable += 1; // next column
		startingOffsetEnable += 1; // Same column

        }
  
    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY ) * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX -1;

    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Row
		indexEnable  = startingOffsetEnable;// Last Row   
        rowimg1 = 0;
        rowimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable];             
			indexDisable++;
			indexEnable--;
            }
        rowimg1 = rowimg1/aImageEffectDisable->iImageParams.iImageWidth;
        rowimg2 = rowimg2/aImageEffectEnable->iImageParams.iImageWidth;

        rowDiff = rowimg1 - rowimg2;
        error1 = ABS(rowDiff);
        if(error1 > MIRROR_THRESOLD_VERTICAL)
            {
            deviationRow++;
  			//ALGO_Log_2("MetricVerticalMirroring : rowDiff = %d > MIRROR_THRESOLD_VERTICAL = %d\n", error1, MIRROR_THRESOLD_VERTICAL);
            }

		startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth;
		startingOffsetEnable -= aImageEffectEnable->iImageParams.iImageWidth;

        }

    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
  
	ALGO_Log_4("DeviationCol = %10d, DeviationRow = %10d, ToleranceCol = %10f, ToleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);

    if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNone;
        }
    else
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNotValidated;;
        }
}

/**
 * Validation Metric for the Horizontal and Vertical Mirroring Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for Both mirroring.
	- Need to check nth column with (aMetricParams->iTestCoordinates.iPixelsToGrabX -1)-n th column.
	- Need to check nth row with (aMetricParams->iTestCoordinates.iPixelsToGrabY -1)-n th row.
 */

TAlgoError ValidationMetricBothMirroringEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
    TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0;
    TUint32 startingOffsetEnable = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);

	startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY - aMetricParams->iTestCoordinates.iPixelsToGrabY) * aImageEffectDisable->iImageParams.iImageWidth
                             + (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iStartIndexX -1);

    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column
		indexEnable  = startingOffsetEnable;// First Column 
        colonimg1 = 0;
        colonimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
            indexEnable = indexEnable + aImageEffectEnable->iImageParams.iImageWidth;
			indexDisable = indexDisable + aImageEffectEnable->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
        colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
        if(error1 > MIRROR_THRESOLD_BOTH)
            {
            deviationCol++;
  			//ALGO_Log_2("MetricBothMirroring : colonDiff = %d > MIRROR_THRESOLD_BOTH = %d\n", error1, MIRROR_THRESOLD_BOTH);
            }

	    startingOffsetDisable++; // next column
		startingOffsetEnable--; // Same column

        }
    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY ) * aImageEffectDisable->iImageParams.iImageWidth
                             + (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iStartIndexX - aMetricParams->iTestCoordinates.iPixelsToGrabX) -1;
   
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    
        rowimg1 = 0;
        rowimg2 = 0;
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable];             
			indexDisable++;
			indexEnable--;
            }
        rowimg1 = rowimg1/aImageEffectDisable->iImageParams.iImageWidth;
        rowimg2 = rowimg2/aImageEffectEnable->iImageParams.iImageWidth;

        rowDiff = rowimg1 - rowimg2;
        error1 = ABS(rowDiff);
        if(error1 > MIRROR_THRESOLD_BOTH)
            {
            deviationRow++;
   			//ALGO_Log_2("MetricBothMirroring : rowDiff = %d > MIRROR_THRESOLD_BOTH = %d\n", error1, MIRROR_THRESOLD_BOTH);
            }

        startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth;
		startingOffsetEnable -= aImageEffectEnable->iImageParams.iImageWidth;
        }

    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
  
	ALGO_Log_4("DeviationCol = %10d, DeviationRow = %10d, ToleranceCol = %10f, ToleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);

    if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNone;
        }
    else
        {
	    //ALGO_Log_3("DeviationCol = %10d, DeviationRow = %10d, Tolerance = %10f\n", deviationCol, deviationRow, tolerance);
        return EErrorNotValidated;;
        }
}
