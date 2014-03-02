/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algocolortoneeffect.c
* \brief    Contains Algorithm Library Color Tone Effect validation functions
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
#include "algocolortoneeffect.h"

/**
 * Validation Metric for the Color Tone Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricColorToneEffect( const TAlgoImage* aImageEffectDisable, 
					                        const TAlgoImage* aImageEffectEnable,
					                        const TAlgoMetricParams* aMetricParams )
    {
    TUint32 x = 0;
    TUint32 y = 0;
    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);

    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
	TUint8 *iG = aImageEffectDisable->iImageData.iGChannel + startingOffset;
	TUint8 *iB = aImageEffectDisable->iImageData.iBChannel + startingOffset;

    TUint8 R = 0;
    TUint8 G = 0;
    TUint8 B = 0;

    TAlgoMatrix* matrix = (TAlgoMatrix*)(aMetricParams->iParams);

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

	for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY; y++)
        {   
		for(x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX; x++)
		    {
            R = *iR;
            G = *iG;
            B = *iB;
            
            *iR = (TUint8)(matrix->iMatrix[0][0] * (R) + 
                           matrix->iMatrix[0][1] * (G) + 
                           matrix->iMatrix[0][2] * (B));   

            *iG = (TUint8)(matrix->iMatrix[1][0] * (R) + 
                           matrix->iMatrix[1][1] * (G) + 
                           matrix->iMatrix[1][2] * (B));   

            *iB = (TUint8)(matrix->iMatrix[2][0] * (R) + 
                           matrix->iMatrix[2][1] * (G) + 
                           matrix->iMatrix[2][2] * (B));   

		    iR++;		
		    iG++;		
		    iB++;				
            }
		iR += offset;		
		iG += offset;		
		iB += offset;	
		}

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    toleranceR = (aMetricParams->iErrorTolerance.iErrorToleranceR * COLORTONE_THRESHOLD)/100;
    toleranceG = (aMetricParams->iErrorTolerance.iErrorToleranceG * COLORTONE_THRESHOLD)/100;
    toleranceB = (aMetricParams->iErrorTolerance.iErrorToleranceB * COLORTONE_THRESHOLD)/100;

    //ALGO_Log_2("Threshold = %d, Tolerance = %f\n", COLORTONE_THRESHOLD, toleranceR);
    //ALGO_Log_3("Disable R_Avg = %10f, Disable G_Avg %10f, Disable B_Avg = %10f\n", R_Avg_Im1, G_Avg_Im1, B_Avg_Im1);
    //ALGO_Log_3("Enable R_Avg = %10f, Enable G_Avg %10f, Enable B_Avg = %10f\n", R_Avg_Im2, G_Avg_Im2, B_Avg_Im2);
    //ALGO_Log_1("ABS(R_Avg_Im1-R_Avg_Im2) = %10f\n", ABS(R_Avg_Im1-R_Avg_Im2));
    //ALGO_Log_1("ABS(G_Avg_Im1-G_Avg_Im2) = %10f\n", ABS(G_Avg_Im1-G_Avg_Im2));
    //ALGO_Log_1("ABS(GBAvg_Im1-B_Avg_Im2) = %10f\n", ABS(B_Avg_Im1-B_Avg_Im2));
    
    if((ABS(R_Avg_Im1-R_Avg_Im2) < COLORTONE_THRESHOLD + toleranceR) && 
        (ABS(G_Avg_Im1-G_Avg_Im2) < COLORTONE_THRESHOLD + toleranceG) && 
        (ABS(B_Avg_Im1-B_Avg_Im2) < COLORTONE_THRESHOLD + toleranceB) )
        {  
        return EErrorNone; 
        }
    else
        {
        return EErrorNotValidated;
        }
    }
