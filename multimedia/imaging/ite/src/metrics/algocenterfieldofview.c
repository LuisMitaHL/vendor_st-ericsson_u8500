/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoluminancemean.c
* \brief    Contains Algorithm Library Center Fiels Of View comparison validation functions
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
#include "algocenterfieldofview.h"


/**
 * Validation Metric forcenter field of view.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 * Theory  - Check for no rotation.
 */

TAlgoError ValidationMetricCenterFieldOfView( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
{
	TAlgoError errorCode = EErrorNone ;
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
	TUint32 startingOffsetDisable = 0, startingOffsetEnable = 0;
	TUint32 pixels_ToGrabX = 0, pixels_ToGrabY = 0;
	TUint32	i_StartIndexX_1 = 0, i_StartIndexY_1 = 0;
 	TUint32	i_StartIndexX_2 = 0, i_StartIndexY_2 = 0;
    TAlgoParamCenterFieldOfView *paramCenterFieldOfView = (TAlgoParamCenterFieldOfView*)aMetricParams->iParams;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	ALGO_Log_2("param : width %d, height %d \n", aImageEffectDisable->iImageParams.iImageWidth, aImageEffectDisable->iImageParams.iImageHeight);
	ALGO_Log_3("param : iX %d, iY %d, iTolerance %d \n", paramCenterFieldOfView->iX, paramCenterFieldOfView->iY, paramCenterFieldOfView->iTolerance);

	pixels_ToGrabX = ((aImageEffectDisable->iImageParams.iImageWidth   / 2) - ABS(paramCenterFieldOfView->iX)) * 2 ;
	pixels_ToGrabY = ((aImageEffectDisable->iImageParams.iImageHeight / 2) - ABS(paramCenterFieldOfView->iY)) * 2 ;
	ALGO_Log_2("iPixelsToGrabX %d , iPixelsToGrabY %d \n", pixels_ToGrabX, pixels_ToGrabY);
	
	i_StartIndexX_1 = 1 * ABS(paramCenterFieldOfView->iX);
	i_StartIndexY_1 = 1 * ABS(paramCenterFieldOfView->iY);
	ALGO_Log_2("i_StartIndexX_1 %d , i_StartIndexY_1 %d \n", i_StartIndexX_1, i_StartIndexY_1);
	if(paramCenterFieldOfView->iX > 0)
		i_StartIndexX_2 = 2 * paramCenterFieldOfView->iX;
	else
		i_StartIndexX_2 = 0;
	if(paramCenterFieldOfView->iY > 0)
		i_StartIndexY_2 = 2 * paramCenterFieldOfView->iY;
	else
		i_StartIndexY_2 = 0;
	ALGO_Log_2("i_StartIndexX_2 %d , i_StartIndexY_2 %d \n", i_StartIndexX_2, i_StartIndexY_2);

	errorCode = GetLumaOn_R_Channel_And_Normalize(aImageEffectDisable, aImageEffectEnable);
	if(errorCode!= EErrorNone)
	{
		return EErrorNotValidated;
	}

	startingOffsetDisable = i_StartIndexY_1 * aImageEffectDisable->iImageParams.iImageWidth + i_StartIndexX_1;
    startingOffsetEnable  = i_StartIndexY_2 * aImageEffectEnable->iImageParams.iImageWidth  + i_StartIndexX_2;
    for( i=0; i<pixels_ToGrabX; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    

        for(j=0; j<pixels_ToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[indexDisable]; 
            colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[indexEnable]; 
			indexEnable = indexEnable + aImageEffectEnable->iImageParams.iImageWidth;
			indexDisable = indexDisable + aImageEffectEnable->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/pixels_ToGrabY;
        colonimg2 = colonimg2/pixels_ToGrabY;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
        if(error1 > CENTER_FIELD_OF_VIEW_THRESHOLD)
            {
            deviationCol++;
            }

        colonimg1 = 0;
        colonimg2 = 0;
	    startingOffsetDisable++; // next column
		startingOffsetEnable++; // next column
        }
   
	startingOffsetDisable = i_StartIndexY_1 * aImageEffectDisable->iImageParams.iImageWidth + i_StartIndexX_1;
    startingOffsetEnable  = i_StartIndexY_2 * aImageEffectEnable->iImageParams.iImageWidth  + i_StartIndexX_2;
	for( i=0; i<pixels_ToGrabY; i++ )
        {
		indexDisable = startingOffsetDisable;// First Column.
		indexEnable  = startingOffsetEnable;// First Column.    
        for(j=0; j<pixels_ToGrabX; j++)
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
        if(error1 > CENTER_FIELD_OF_VIEW_THRESHOLD)
            {
            deviationRow++;
            }

        rowimg1 = 0;
        rowimg2 = 0;  
        startingOffsetDisable += aImageEffectDisable->iImageParams.iImageWidth;
		startingOffsetEnable += aImageEffectEnable->iImageParams.iImageWidth;

        }

    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * pixels_ToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * pixels_ToGrabY )/100;
  
	ALGO_Log_4("DeviationCol = %10d, DeviationRow = %10d, ToleranceCol = %10f, ToleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
	
    if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
}
