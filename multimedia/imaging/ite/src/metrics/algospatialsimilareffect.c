/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algospatialsimilareffect.c
* \brief    Contains Algorithm Library Spatial Similarity Effect validation functions
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algospatialsimilareffect.h"
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
TAlgoError ValidationMetricSpatialSimilarEffect( const TAlgoImage* aImageEffectDisable, 
					                             const TAlgoImage* aImageEffectEnable,
					                             const TAlgoMetricParams* aMetricParams )
    {
#if 1
    TUint32 i = 0;
    TUint32 j = 0;
    TUint32 index = 0;
    TUint32 index1 = 0;
    TInt32 error1 = 0;
    TInt32 deviationCol = 0;
    TInt32 deviationRow = 0;
    TUint32 colonimg1 = 0;
    TUint32 colonimg2 = 0;
    TUint32 rowimg1 = 0;
    TUint32 rowimg2 = 0;
    TReal64 toleranceCol = 0;
    TReal64 toleranceRow = 0;
    //TReal64 threshold = 0;
    TReal64 Avg_Disable = 0;
    TReal64 Avg_Enable = 0;
    TInt32 colonDiff = 0;
    TInt32 rowDiff = 0;
    TReal64 Average = 0;

    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

 	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
   //Getting the luma component for both images in the R Channel.
    for( index=0; index<(aImageEffectDisable->iImageParams.iImageHeight * 
         aImageEffectDisable->iImageParams.iImageWidth); index++)
        {
        aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectDisable->iImageData.iRChannel[index]
		    		                                              + LUMINANCE_G_COEF * aImageEffectDisable->iImageData.iGChannel[index]
			   	                                                  + LUMINANCE_B_COEF * aImageEffectDisable->iImageData.iBChannel[index]);

        aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(LUMINANCE_R_COEF * aImageEffectEnable->iImageData.iRChannel[index]
		    		                                             + LUMINANCE_G_COEF * aImageEffectEnable->iImageData.iGChannel[index]
			   	                                                 + LUMINANCE_B_COEF * aImageEffectEnable->iImageData.iBChannel[index]);
        }

  
    //Calculating the average of luma components of both images for  normalization.
    index = startingOffset;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            Avg_Disable = Avg_Disable + aImageEffectDisable->iImageData.iRChannel[index];
            Avg_Enable = Avg_Enable + aImageEffectEnable->iImageData.iRChannel[index]; 
            index++;
            }
            index = index + offset;
        }

    Avg_Disable = Avg_Disable/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    Avg_Enable = Avg_Enable/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
	   
	//ALGO_Log_1("Avg_Enable/Avg_Disable =  %1.2lf \n", (double)(Avg_Enable/Avg_Disable));

    Average = (Avg_Enable/Avg_Disable);

    //Normalizing the resized disabled image.
    index = startingOffset;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            aImageEffectDisable->iImageData.iRChannel[index] = (TUint8)(Average * (aImageEffectDisable->iImageData.iRChannel[index])); 
            aImageEffectEnable->iImageData.iRChannel[index] = (TUint8)(Average * (aImageEffectEnable->iImageData.iRChannel[index])); 
            index++;
            }
        index = index + offset;
        }

    index1 = startingOffset;
    index = index1;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabX; i++ )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabY; j++)
            {
            colonimg1 = colonimg1 + aImageEffectDisable->iImageData.iRChannel[index]; 
            colonimg2 = colonimg2 + aImageEffectEnable->iImageData.iRChannel[index]; 
            index = index + aImageEffectEnable->iImageParams.iImageWidth;
            }
        colonimg1 = colonimg1/aMetricParams->iTestCoordinates.iPixelsToGrabY;
        colonimg2 = colonimg2/aMetricParams->iTestCoordinates.iPixelsToGrabY;

        colonDiff = colonimg1 - colonimg2;
        error1 = ABS(colonDiff);
        if(error1 > 10)
            {
            deviationCol++;
            }

        colonimg1 = 0;
        colonimg2 = 0;
        index1++;
        index = index1;
        }

    index = startingOffset;
    for( i=0; i<aMetricParams->iTestCoordinates.iPixelsToGrabY; i++ )
        {
        for(j=0; j<aMetricParams->iTestCoordinates.iPixelsToGrabX; j++)
            {
            rowimg1 = rowimg1 + aImageEffectDisable->iImageData.iRChannel[index]; 
            rowimg2 = rowimg2 + aImageEffectEnable->iImageData.iRChannel[index]; 
            index++;
            }
        rowimg1 = rowimg1/aImageEffectDisable->iImageParams.iImageWidth;
        rowimg2 = rowimg2/aImageEffectEnable->iImageParams.iImageWidth;

        rowDiff = rowimg1 - rowimg2;
        error1 = ABS(rowDiff);
        if(error1 > 10)
            {
            deviationRow++;
            }

        rowimg1 = 0;
        rowimg2 = 0;
        index = index + offset;
        }

    toleranceCol = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabX )/100;
    toleranceRow = ( aMetricParams->iErrorTolerance.iErrorToleranceR * aMetricParams->iTestCoordinates.iPixelsToGrabY )/100;
  
	//ALGO_Log_1("Zoom Metric expected zoom factor : %1.2lf\n\n", (double)(*zoomfactor));
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
#else
    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;

    TReal64 toleranceR = 0;
    TReal64 toleranceG = 0;
    TReal64 toleranceB = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    toleranceR = (aMetricParams->iErrorTolerance.iErrorToleranceR * SPATIAL_SIMILAR_THRESHOLD)/100;
    toleranceG = (aMetricParams->iErrorTolerance.iErrorToleranceG * SPATIAL_SIMILAR_THRESHOLD)/100;
    toleranceB = (aMetricParams->iErrorTolerance.iErrorToleranceB * SPATIAL_SIMILAR_THRESHOLD)/100;

    //ALGO_Log_3("toleranceR = %10f, toleranceG %10f, toleranceB %10f\n", toleranceR, toleranceG, toleranceB);
    ALGO_Log_3("ABS(R_Avg_Im1-R_Avg_Im2) = %10f, ABS(G_Avg_Im1-G_Avg_Im2) %10f, ABS(B_Avg_Im1-B_Avg_Im2) %10f\n", 
        ABS(R_Avg_Im1-R_Avg_Im2), ABS(G_Avg_Im1-G_Avg_Im2), ABS(B_Avg_Im1-B_Avg_Im2));

    if((ABS(R_Avg_Im1-R_Avg_Im2) < SPATIAL_SIMILAR_THRESHOLD + toleranceR) && 
       (ABS(G_Avg_Im1-G_Avg_Im2) < SPATIAL_SIMILAR_THRESHOLD + toleranceG) && 
       (ABS(B_Avg_Im1-B_Avg_Im2) < SPATIAL_SIMILAR_THRESHOLD + toleranceB) )
        {  
        return EErrorNone; // Images are spatially similar
        }
    else
        {
        return EErrorNotValidated;
        }
#endif
	}
