/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algozoomeffect.c
* \brief    Contains Algorithm Library Zoom Effect validation functions
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
#include "algozoomeffect.h"

#include "algointerface.h"

/**
 * Validation Metric for the Zoom Effect.
 *
 * @param aImage_1       [TAlgoImage] The image with Effect Disable.
 * @param aImage_2        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricZoomEffect( const TAlgoImage* aImage_01, 
					                   const TAlgoImage* aImage_02,
					                   const TAlgoMetricParams* aMetricParams )
    {
    TUint32 i = 0;
    TUint32 j = 0;
    TUint32 index = 0;
    TUint32 index1 = 0;
    TInt32 error1 = 0;
	TInt32 error_mean_row = 0, error_mean_col = 0, error_mean_row_total = 0, error_mean_col_total = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 woi_width = 0;
	TUint32 woi_height = 0;
    TAlgoImageTestCoordinates inputRect;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
#ifdef ZOOM_OLD_WAY
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
#endif
	TReal64 Avg_Disable = 0;
    TReal64 Avg_Enable = 0;
    TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
    TReal64 Average = 0;
    TAlgoParamZoomFactor *zoomfactor = (TAlgoParamZoomFactor*)aMetricParams->iParams;
	const TAlgoImage* aImage_1 = (const TAlgoImage*)NULL; 
	const TAlgoImage* aImage_2 = (const TAlgoImage*)NULL;
	TReal64 zoomFact = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	if(zoomfactor->iZoomFactor >=1)
	{
		aImage_1 = aImage_01; 
		aImage_2 = aImage_02;
		zoomFact = zoomfactor->iZoomFactor;
		ALGO_Log_1("Zoom Metric expected zoom factor : %1.2lf\n\n", (double)zoomFact);
	}
	else
	{
		aImage_1 = aImage_02; 
		aImage_2 = aImage_01;
		zoomFact =  1 / zoomfactor->iZoomFactor;
		ALGO_Log_0("Zoom Metric : warning : zoom need to be inversed as it is < 1\n\n");
		ALGO_Log_1("Zoom Metric expected zoom factor : %1.2lf\n\n", (double)zoomFact);
	}

	ALGO_Log_2("reso zoom : %d x %d\n", aImage_1->iImageParams.iImageWidth, aImage_1->iImageParams.iImageHeight);
    woi_width = (TInt32)(aImage_1->iImageParams.iImageWidth/zoomFact);
	woi_height = (TInt32)(aImage_1->iImageParams.iImageHeight/zoomFact);

    //Getting the luma component for both images in the R Channel.
    for( index=0; index<(aImage_1->iImageParams.iImageHeight * 
         aImage_1->iImageParams.iImageWidth); index++)
        {
     	//ALGO_Log_3("\t 1_iRChannel = %d, 1_iGChannel = %d, 1_iBChannel = %d\n", aImage_1->iImageData.iRChannel[index], aImage_1->iImageData.iGChannel[index], aImage_1->iImageData.iBChannel[index]);
     	//ALGO_Log_3("\t 2_iRChannel = %d, 2_iGChannel = %d, 2_iBChannel = %d\n", aImage_2->iImageData.iRChannel[index], aImage_2->iImageData.iGChannel[index], aImage_2->iImageData.iBChannel[index]);
       aImage_1->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImage_1->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImage_1->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImage_1->iImageData.iBChannel[index]);

        aImage_2->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImage_2->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImage_2->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImage_2->iImageData.iBChannel[index]);
    			//ALGO_Log_2("\t luma pixel_1 = %d\n", aImage_1->iImageData.iRChannel[index], index);
     			//ALGO_Log_2("\t luma pixel_2 = %d\n", aImage_2->iImageData.iRChannel[index], index);
      }

	inputRect.iStartIndexX = (TInt32)((aImage_1->iImageParams.iImageWidth - woi_width)/2);  
    inputRect.iStartIndexY = (TInt32)((aImage_1->iImageParams.iImageHeight - woi_height)/2);
    inputRect.iPixelsToGrabX = woi_width;
    inputRect.iPixelsToGrabY = woi_height;

    //Saving the window of interest of disabled image in G Channel of disabled image.
    index = inputRect.iStartIndexY * aImage_1->iImageParams.iImageWidth + inputRect.iStartIndexX;
    index1 = 0;
    for( i=0; i<woi_height; i++)
        {
        for( j=0; j<woi_width; j++ )
            {
            aImage_1->iImageData.iGChannel[index1] = aImage_1->iImageData.iRChannel[index]; 
            index++;
            index1++;
            }
        index = index + (aImage_1->iImageParams.iImageWidth - woi_width);
        }

    //Resizing the disabled image and saving the result in B Channel.
    ResizeBilinearInterpolation( aImage_1->iImageData.iGChannel,
                                 aImage_1->iImageData.iBChannel,
                                 woi_width,
                                 woi_height,
                                 aImage_1->iImageParams.iImageWidth,
                                 aImage_1->iImageParams.iImageHeight);

   /*
	// for degug : save intermediate numerical zoomed image
	// put all RGB channel to same value (grey image
	index = 0;
	for( i=0; i<aImage_1->iImageParams.iImageWidth; i++ )
	{
		for(j=0; j<aImage_1->iImageParams.iImageHeight; j++)
		{
			aImage_1->iImageData.iRChannel[index] = aImage_1->iImageData.iBChannel[index];
			aImage_1->iImageData.iGChannel[index] = aImage_1->iImageData.iBChannel[index];
			index++;
		}
	}
	// save in bmp
	{
		TAlgoError errCod = EErrorNone;
		TAlgoImageParams aImageParams;
		aImageParams.iImageWidth = aImage_1->iImageParams.iImageWidth;
		aImageParams.iImageHeight = aImage_1->iImageParams.iImageHeight;
		errCod = convertToBMPandSaveWithBuffers_create_bmp(&aImageParams, (TAlgoImage *)aImage_01, (const TUint8*)"tmp_zoom.bmp");
	}
	*/

    //Calculating the average of luma components of both images for normalization.
    index = 0;
    for( i=0; i<aImage_2->iImageParams.iImageWidth; i++ )
        {
        for(j=0; j<aImage_2->iImageParams.iImageHeight; j++)
            {
            Avg_Disable = Avg_Disable + aImage_1->iImageData.iBChannel[index];
            Avg_Enable = Avg_Enable + aImage_2->iImageData.iRChannel[index]; 
            index++;
            }
			//ALGO_Log_2("index %i Avg_Enable =  %1.2lf \n", i, (double)(Avg_Enable));
 			//ALGO_Log_2("index %i Avg_Disable =  %1.2lf \n", i, (double)(Avg_Disable));
       }

    Avg_Disable = Avg_Disable/(aImage_1->iImageParams.iImageWidth * aImage_1->iImageParams.iImageHeight);
    Avg_Enable = Avg_Enable/(aImage_2->iImageParams.iImageWidth * aImage_2->iImageParams.iImageHeight);
	   
	ALGO_Log_1("Avg_Enable =  %1.6lf \n", (double)(Avg_Enable));
	ALGO_Log_1("Avg_Disable =  %1.6lf \n", (double)(Avg_Disable));
    Average = (Avg_Enable/Avg_Disable);
	ALGO_Log_1("Avg_Enable/Avg_Disable =  %1.6lf \n", (double)(Average));


	// normalize image
    index = 0;
	if(Average <= 1.0)
	{
		for( i=0; i<aImage_2->iImageParams.iImageWidth; i++ )
		{
			for(j=0; j<aImage_2->iImageParams.iImageHeight; j++)
			{
				aImage_1->iImageData.iBChannel[index] = (TUint8)(Average * (aImage_1->iImageData.iBChannel[index])); 
				index++;
			}
		}
	}
	else
	{
		TUint16 tmp_Value=0;
		for( i=0; i<aImage_2->iImageParams.iImageWidth; i++ )
		{
			for(j=0; j<aImage_2->iImageParams.iImageHeight; j++)
			{
				tmp_Value =(TUint16)(Average * (aImage_1->iImageData.iBChannel[index])); 
				if(0xFF >= tmp_Value)
				{
					aImage_1->iImageData.iBChannel[index] = (TUint8)tmp_Value; 
				}
				else
				{
					// clip
					aImage_1->iImageData.iBChannel[index] = (TUint8)0xFF; 
				}
				index++;
			}
		}
	}

    index1 = 0;
    index = index1;
	error_mean_col = 0;
	error_mean_col_total = 0;
    for( i=0; i<aImage_2->iImageParams.iImageWidth; i++ )
        {
  			//ALGO_Log_3("\t\t\t pixel_1=%d, pixel_2=%d, index=%d \n", aImage_1->iImageData.iBChannel[index], aImage_2->iImageData.iRChannel[index], index);
        for(j=0; j<aImage_2->iImageParams.iImageHeight; j++)
            {
            colonimg1 = colonimg1 + aImage_1->iImageData.iBChannel[index]; 
            colonimg2 = colonimg2 + aImage_2->iImageData.iRChannel[index]; 
           index = index + aImage_2->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/aImage_1->iImageParams.iImageHeight;
        colonimg2 = colonimg2/aImage_2->iImageParams.iImageHeight;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
		error_mean_col_total += error1;
        if(error1 > ZOOM_THRESHOLD)
            {
#ifdef ZOOM_OLD_WAY
 			ALGO_Log_3("\t\t deviationCol++, w_i=%d, error1 = %d > %d = ZOOM_THRESHOLD\n", i, error1, ZOOM_THRESHOLD);
#endif
			deviationCol++;
			error_mean_col += error1;
            }

        colonimg1 = 0;
        colonimg2 = 0;
        index1++;
        index = index1;
        }
	if(deviationCol != 0)
	{
		error_mean_col = error_mean_col / deviationCol;
	}
	error_mean_col_total = error_mean_col_total / i;

    index = 0;
	error_mean_row = 0;
	error_mean_row_total = 0;
    for( i=0; i<aImage_2->iImageParams.iImageHeight; i++ )
        {
        for(j=0; j<aImage_2->iImageParams.iImageWidth; j++)
            {
            rowimg1 = rowimg1 + aImage_1->iImageData.iBChannel[index]; 
            rowimg2 = rowimg2 + aImage_2->iImageData.iRChannel[index]; 
            index++;
            }
        rowimg1 = rowimg1/aImage_1->iImageParams.iImageWidth;
        rowimg2 = rowimg2/aImage_2->iImageParams.iImageWidth;

        rowDiff = rowimg1 - rowimg2;
        error1 = ABS(rowDiff);
		error_mean_row_total += error1;
        if(error1 > ZOOM_THRESHOLD)
            {
#ifdef ZOOM_OLD_WAY
  			ALGO_Log_3("\t\t deviationRow++, h_i=%d, error1 = %d > %d = ZOOM_THRESHOLD\n", i, error1, ZOOM_THRESHOLD);
#endif
			deviationRow++;
 			error_mean_row += error1;
           }

        rowimg1 = 0;
        rowimg2 = 0;
        }
	if(deviationRow != 0)
	{
		error_mean_row = error_mean_row / deviationRow;
	}
	error_mean_row_total = error_mean_row_total / i;

#ifdef ZOOM_OLD_WAY
    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aImage_1->iImageParams.iImageWidth )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aImage_1->iImageParams.iImageHeight )/100;
	ALGO_Log_4("deviationCol = %10d, deviationRow = %10d, toleranceCol = %10f, toleranceRow = %10f\n", deviationCol, deviationRow, toleranceCol, toleranceRow);
    ALGO_Log_3("error_mean_col = %d, error_mean_row = %d, ZOOM_THRESHOLD = %d\n", error_mean_col, error_mean_row, ZOOM_THRESHOLD);
	if((deviationCol <= toleranceCol)&&(deviationRow <= toleranceRow))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#else
    ALGO_Log_3("error_mean_col_total = %d, error_mean_row_total = %d, ZOOM_THRESHOLD = %d\n", error_mean_col_total, error_mean_row_total, ZOOM_THRESHOLD);
	if((error_mean_col_total <= ZOOM_THRESHOLD)&&(error_mean_row_total <= ZOOM_THRESHOLD))
	{
		return EErrorNone;
	}
	else
	{
		return EErrorNotValidated;;
	}
#endif
}
