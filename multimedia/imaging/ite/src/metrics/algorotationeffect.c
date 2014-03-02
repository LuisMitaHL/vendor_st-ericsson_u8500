/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algorotationeffect.c
* \brief    Contains Algorithm Library Rotation Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*

 * Includes 
 */
#include "algorotationeffect.h"
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
TAlgoError ValidationMetricRotationEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TAlgoError errorCode = EErrorNone ;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	errorCode = GetLumaOn_R_Channel_And_Normalize(aImageEffectDisable, aImageEffectEnable);
	if(errorCode!= EErrorNone)
	{
		return EErrorNotValidated;
	}

	if (EType0degreesRotation == aMetricParams->iType)		 //For 0 degrees Rotation Image Metrices.
		return ValidationMetric0degreesRotationEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
	else if (EType90degreesRotation == aMetricParams->iType)		 //For 90 degrees Rotation Image Metrices.
		return ValidationMetric90degreesRotationEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
	else if (EType180degreesRotation == aMetricParams->iType) 		 //For 180 degrees Rotation Image Metrices.
		return ValidationMetric180degreesRotationEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
	else if (EType270degreesRotation == aMetricParams->iType) 		 //For 270 degrees Rotation Image Metrices.
		return ValidationMetric270degreesRotationEffect(aImageEffectDisable,aImageEffectEnable,aMetricParams);
	return EErrorNone;
}
/**
 * Validation Metric for the 270 degrees Rotation Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for vertical rotation.
	- Need to check nth column with (aMetricParams->iTestCoordinates.iPixelsToGrabX -1)-n th column.
	- Checking for row matrices is fine.
 */

TAlgoError ValidationMetric270degreesRotationEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
	TInt32 error_mean_row = 0, error_mean_col = 0, error_mean_row_total = 0, error_mean_col_total = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
#ifdef ROTATION_OLD_WAY
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;    
#endif
	TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0, startingOffsetEnable = 0;
	TUint32 EnableWidthWorking = 0;
	TUint32 EnableHeightWorking = 0;
	ALGO_Log_1("entering rotation %s\n", __FUNCTION__);
	ALGO_Log_2("iStartIndexX %d , iStartIndexY %d\n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("iPixelsToGrabX %d , iPixelsToGrabY %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);

	ALGO_Log_4("Disable width x height =  %d x %d, stride %d, sliceheight %d\n", aImageEffectDisable->iImageParams.iImageWidth, aImageEffectDisable->iImageParams.iImageHeight, aImageEffectDisable->iImageParams.iImageStride, aImageEffectDisable->iImageParams.iImageSliceHeight);
	ALGO_Log_4("Enable  width x height =  %d x %d, stride %d, sliceheight %d\n", aImageEffectEnable->iImageParams.iImageWidth,  aImageEffectEnable->iImageParams.iImageHeight, aImageEffectEnable->iImageParams.iImageStride,  aImageEffectEnable->iImageParams.iImageSliceHeight);
	EnableWidthWorking = aImageEffectDisable->iImageParams.iImageHeight;
	ALGO_Log_1("EnableWidthWorking %d\n", EnableWidthWorking);
	EnableHeightWorking = aImageEffectDisable->iImageParams.iImageWidth;
	ALGO_Log_1("EnableHeightWorking %d\n", EnableHeightWorking);

    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = EnableWidthWorking * (EnableHeightWorking - aMetricParams->iTestCoordinates.iStartIndexX -1)
		+ aMetricParams->iTestCoordinates.iStartIndexY;
	error_mean_col = 0;
	error_mean_col_total = 0;
	for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable = startingOffsetEnable;// row.		
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
			rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable];             
			indexDisable = indexDisable + aImageEffectDisable->iImageParams.iImageWidth;
			indexEnable++;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabX;
		rowimg2 = rowimg2/aMetricParams->iTestCoordinates.iPixelsToGrabX;

        rowDiff = colonimg1 - rowimg2;
        error1 = ABS(rowDiff);
		error_mean_col_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
  			ALGO_Log_3("\t\t deviationCol++, w_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationCol++;
			error_mean_col += error1;
            }

		colonimg1 = 0;
        rowimg2 = 0;
        startingOffsetDisable++; // Next Column
		startingOffsetEnable = startingOffsetEnable - EnableWidthWorking; // Next Row
        }
	if(deviationCol != 0)
	{
		error_mean_col = error_mean_col / deviationCol;
	}
	error_mean_col_total = error_mean_col_total / i;

    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = EnableWidthWorking * (EnableHeightWorking - aMetricParams->iTestCoordinates.iStartIndexX - 1)
		+ aMetricParams->iTestCoordinates.iStartIndexY;
 	error_mean_row = 0;
	error_mean_row_total = 0;
   for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Row.
		indexEnable = startingOffsetEnable;// Column.		

        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
			colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
            indexDisable ++;
			indexEnable = indexEnable - EnableWidthWorking;
            }
        rowimg1 = rowimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
		colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg2 - rowimg1;
        error1 = ABS(colonDiff);
		error_mean_row_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
   			ALGO_Log_3("\t\t deviationRow++, h_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationRow++;
 			error_mean_row += error1;
            }

        rowimg1 = 0;
		colonimg2 = 0;
        startingOffsetDisable = startingOffsetDisable + aImageEffectDisable->iImageParams.iImageWidth; // Next Row
		startingOffsetEnable++; // Column
        }
	if(deviationRow != 0)
	{
		error_mean_row = error_mean_row / deviationRow;
	}
	error_mean_row_total = error_mean_row_total / i;

  
#ifdef ROTATION_OLD_WAY
    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
	ALGO_Log_4("deviationCol = %10d, deviationRow = %10d, toleranceCol = %10f, toleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
    ALGO_Log_3("error_mean_col = %d, error_mean_row = %d, ROTATION_THRESHOLD = %d\n", error_mean_col, error_mean_row, ROTATION_THRESHOLD);
	if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#else
    ALGO_Log_3("error_mean_col_total = %d, error_mean_row_total = %d, ROTATION_THRESHOLD_TOTAL = %d\n", error_mean_col_total, error_mean_row_total, ROTATION_THRESHOLD_TOTAL);
	if((error_mean_col_total <= ROTATION_THRESHOLD_TOTAL)&&(error_mean_row_total <= ROTATION_THRESHOLD_TOTAL))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#endif
}

/**
 * Validation Metric for the 90 degrees Rotation Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for vertical rotation.
	- Need to check nth column with (aMetricParams->iTestCoordinates.iPixelsToGrabX -1)-n th column.
	- Checking for row matrices is fine.
 */

TAlgoError ValidationMetric90degreesRotationEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
	TInt32 error_mean_row = 0, error_mean_col = 0, error_mean_row_total = 0, error_mean_col_total = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
#ifdef ROTATION_OLD_WAY
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;    
#endif
	TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0, startingOffsetEnable = 0;
	TUint32 EnableWidthWorking = 0;
	ALGO_Log_1("entering rotation %s\n", __FUNCTION__);
	ALGO_Log_2("iStartIndexX %d , iStartIndexY %d\n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("iPixelsToGrabX %d , iPixelsToGrabY %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);

	ALGO_Log_4("Disable width x height =  %d x %d, stride %d, sliceheight %d\n", aImageEffectDisable->iImageParams.iImageWidth, aImageEffectDisable->iImageParams.iImageHeight, aImageEffectDisable->iImageParams.iImageStride, aImageEffectDisable->iImageParams.iImageSliceHeight);
	ALGO_Log_4("Enable  width x height =  %d x %d, stride %d, sliceheight %d\n", aImageEffectEnable->iImageParams.iImageWidth,  aImageEffectEnable->iImageParams.iImageHeight, aImageEffectEnable->iImageParams.iImageStride,  aImageEffectEnable->iImageParams.iImageSliceHeight);
	EnableWidthWorking = aImageEffectDisable->iImageParams.iImageHeight;
	ALGO_Log_1("EnableWidthWorking %d\n", EnableWidthWorking);

    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = EnableWidthWorking * (aMetricParams->iTestCoordinates.iStartIndexX + 1)
		- aMetricParams->iTestCoordinates.iStartIndexY -1;
	error_mean_col = 0;
	error_mean_col_total = 0;
	for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable = startingOffsetEnable;// row.		
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
			rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable];             
			indexDisable = indexDisable + aImageEffectDisable->iImageParams.iImageWidth;
			indexEnable--;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabX;
		rowimg2 = rowimg2/aMetricParams->iTestCoordinates.iPixelsToGrabX;

        rowDiff = colonimg1 - rowimg2;
        error1 = ABS(rowDiff);
		error_mean_col_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
  			ALGO_Log_3("\t\t deviationCol++, w_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationCol++;
			error_mean_col += error1;
            }

		colonimg1 = 0;
        rowimg2 = 0;
        startingOffsetDisable++; // Next Column
		startingOffsetEnable = startingOffsetEnable + EnableWidthWorking; // Next Row
        }
	if(deviationCol != 0)
	{
		error_mean_col = error_mean_col / deviationCol;
	}
	error_mean_col_total = error_mean_col_total / i;

    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = EnableWidthWorking * (aMetricParams->iTestCoordinates.iStartIndexX + 1)
		- aMetricParams->iTestCoordinates.iStartIndexY -1;
	error_mean_row = 0;
	error_mean_row_total = 0;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Row.
		indexEnable = startingOffsetEnable;// Column.		

        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
			colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
            indexDisable ++;
			indexEnable = indexEnable + EnableWidthWorking;
            }
        rowimg1 = rowimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
		colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg2 - rowimg1;
        error1 = ABS(colonDiff);
		error_mean_row_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
  			ALGO_Log_3("\t\t deviationRow++, h_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationRow++;
 			error_mean_row += error1;
            }

        rowimg1 = 0;
		colonimg2 = 0;
        startingOffsetDisable = startingOffsetDisable + aImageEffectDisable->iImageParams.iImageWidth; // Next Row
		startingOffsetEnable--; // Column
        }
	if(deviationRow != 0)
	{
		error_mean_row = error_mean_row / deviationRow;
	}
	error_mean_row_total = error_mean_row_total / i;
  
#ifdef ROTATION_OLD_WAY
    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
	ALGO_Log_4("deviationCol = %10d, deviationRow = %10d, toleranceCol = %10f, toleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
    ALGO_Log_3("error_mean_col = %d, error_mean_row = %d, ROTATION_THRESHOLD = %d\n", error_mean_col, error_mean_row, ROTATION_THRESHOLD);
	if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#else
    ALGO_Log_3("error_mean_col_total = %d, error_mean_row_total = %d, ROTATION_THRESHOLD_TOTAL = %d\n", error_mean_col_total, error_mean_row_total, ROTATION_THRESHOLD_TOTAL);
	if((error_mean_col_total <= ROTATION_THRESHOLD_TOTAL)&&(error_mean_row_total <= ROTATION_THRESHOLD_TOTAL))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#endif
}

/**
 * Validation Metric for the 180 degrees Rotation Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for vertical rotation.
	- Need to check nth column with (aMetricParams->iTestCoordinates.iPixelsToGrabX -1)-n th column.
	- Checking for row matrices is fine.
 */

TAlgoError ValidationMetric180degreesRotationEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
	TInt32 error_mean_row = 0, error_mean_col = 0, error_mean_row_total = 0, error_mean_col_total = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
#ifdef ROTATION_OLD_WAY
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
#endif
	TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0, startingOffsetEnable = 0;
	ALGO_Log_1("entering rotation %s\n", __FUNCTION__);
	ALGO_Log_2("iStartIndexX %d , iStartIndexY %d\n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("iPixelsToGrabX %d , iPixelsToGrabY %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);

	startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY - aMetricParams->iTestCoordinates.iPixelsToGrabY) * aImageEffectDisable->iImageParams.iImageWidth
                             + (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iStartIndexX -1);

	error_mean_col = 0;
	error_mean_col_total = 0;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    

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
		error_mean_col_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
 			ALGO_Log_3("\t\t deviationCol++, w_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationCol++;
 			error_mean_col += error1;
           }

        colonimg1 = 0;
        colonimg2 = 0;
	    startingOffsetDisable++; // next column
		startingOffsetEnable--; // Same column
        }
 	if(deviationCol != 0)
	{
		error_mean_col = error_mean_col / deviationCol;
	}
	error_mean_col_total = error_mean_col_total / i;
  
    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = (aImageEffectDisable->iImageParams.iImageHeight - aMetricParams->iTestCoordinates.iStartIndexY ) * aImageEffectDisable->iImageParams.iImageWidth
                             + (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iStartIndexX - aMetricParams->iTestCoordinates.iPixelsToGrabX);

	error_mean_row = 0;
	error_mean_row_total = 0;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    
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
		error_mean_row_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
  			ALGO_Log_3("\t\t deviationRow++, h_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationRow++;
 			error_mean_row += error1;
            }

        rowimg1 = 0;
        rowimg2 = 0;  
        startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth;
		startingOffsetEnable -= aImageEffectEnable->iImageParams.iImageWidth;

        }
	if(deviationRow != 0)
	{
		error_mean_row = error_mean_row / deviationRow;
	}
	error_mean_row_total = error_mean_row_total / i;

#ifdef ROTATION_OLD_WAY
    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
	ALGO_Log_4("deviationCol = %10d, deviationRow = %10d, toleranceCol = %10f, toleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
    ALGO_Log_3("error_mean_col = %d, error_mean_row = %d, ROTATION_THRESHOLD = %d\n", error_mean_col, error_mean_row, ROTATION_THRESHOLD);
	if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#else
    ALGO_Log_3("error_mean_col_total = %d, error_mean_row_total = %d, ROTATION_THRESHOLD_TOTAL = %d\n", error_mean_col_total, error_mean_row_total, ROTATION_THRESHOLD_TOTAL);
	if((error_mean_col_total <= ROTATION_THRESHOLD_TOTAL)&&(error_mean_row_total <= ROTATION_THRESHOLD_TOTAL))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#endif
}


/**
 * Validation Metric for the 0 degrees Rotation Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for no rotation.
 */

TAlgoError ValidationMetric0degreesRotationEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TUint32 i = 0;
    TUint32 j = 0;
	TUint32 indexDisable =0;
	TUint32 indexEnable =0;
    TInt32 error1 = 0;
	TInt32 error_mean_row = 0, error_mean_col = 0, error_mean_row_total = 0, error_mean_col_total = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
#ifdef ROTATION_OLD_WAY
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
#endif
	TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
	TUint32 startingOffsetDisable = 0, startingOffsetEnable = 0;
	ALGO_Log_1("entering rotation %s\n", __FUNCTION__);
	ALGO_Log_2("iStartIndexX %d , iStartIndexY %d\n", aMetricParams->iTestCoordinates.iStartIndexX, aMetricParams->iTestCoordinates.iStartIndexY);
	ALGO_Log_2("iPixelsToGrabX %d , iPixelsToGrabY %d\n", aMetricParams->iTestCoordinates.iPixelsToGrabX, aMetricParams->iTestCoordinates.iPixelsToGrabY);

	startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable  = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectEnable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
	error_mean_col = 0;
	error_mean_col_total = 0;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    

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
		error_mean_col_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
 			ALGO_Log_3("\t\t deviationCol++, w_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationCol++;
			error_mean_col += error1;
            }

        colonimg1 = 0;
        colonimg2 = 0;
	    startingOffsetDisable++; // next column
		startingOffsetEnable++; // next column
        }
	if(deviationCol != 0)
	{
		error_mean_col = error_mean_col / deviationCol;
	}
	error_mean_col_total = error_mean_col_total / i;
   
    startingOffsetDisable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    startingOffsetEnable = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectEnable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
	error_mean_row = 0;
	error_mean_row_total = 0;
	for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    
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
		error_mean_row_total += error1;
        if(error1 > ROTATION_THRESHOLD)
            {
#ifdef ROTATION_OLD_WAY
   			ALGO_Log_3("\t\t deviationRow++, h_i=%d, error1 = %d > %d = ROTATION_THRESHOLD\n", i, error1, ROTATION_THRESHOLD);
#endif
			deviationRow++;
  			error_mean_row += error1;
           }

        rowimg1 = 0;
        rowimg2 = 0;  
        startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth;
		startingOffsetEnable += aImageEffectEnable->iImageParams.iImageWidth;

        }
	if(deviationRow != 0)
	{
		error_mean_row = error_mean_row / deviationRow;
	}
	error_mean_row_total = error_mean_row_total / i;

 #ifdef ROTATION_OLD_WAY
    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
	ALGO_Log_4("deviationCol = %10d, deviationRow = %10d, toleranceCol = %10f, toleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
    ALGO_Log_3("error_mean_col = %d, error_mean_row = %d, ROTATION_THRESHOLD = %d\n", error_mean_col, error_mean_row, ROTATION_THRESHOLD);
	if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#else
    ALGO_Log_3("error_mean_col_total = %d, error_mean_row_total = %d, ROTATION_THRESHOLD_TOTAL = %d\n", error_mean_col_total, error_mean_row_total, ROTATION_THRESHOLD_TOTAL);
	if((error_mean_col_total <= ROTATION_THRESHOLD_TOTAL)&&(error_mean_row_total <= ROTATION_THRESHOLD_TOTAL))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#endif
}
