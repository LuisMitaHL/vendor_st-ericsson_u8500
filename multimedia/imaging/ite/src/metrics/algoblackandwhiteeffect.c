/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algoblackandwhiteeffect.c
* \brief    Contains Algorithm Library Black and White Effect validation functions
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
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"
#include "algoblackandwhiteeffect.h"

/**
 * Validation Metric for the Black and White Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricBlackAndWhiteEffect( const TAlgoImage* aImageEffectDisable, 
					                            const TAlgoImage* aImageEffectEnable,
					                            const TAlgoMetricParams* aMetricParams )
{
    TAlgoError errorCode = EErrorNotValidated;
	TReal64 tolerance = 0;    
	TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;
	TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
	Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
	Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

	//ALGO_Log_1("R_Average %f\n",R_Avg_Im1);
	//ALGO_Log_1("G_Average %f\n",G_Avg_Im1);
	//ALGO_Log_1("B_Average %f\n",B_Avg_Im1);

	tolerance = BLACKANDWHITE_THRESHOLD + ((aMetricParams->iErrorTolerance.iErrorToleranceR * BLACKANDWHITE_THRESHOLD)/100);

	if(
		(ABS(R_Avg_Im2 - G_Avg_Im2) <= tolerance)&&
		(ABS(G_Avg_Im2 - B_Avg_Im2) <= tolerance) &&
		(ABS(B_Avg_Im2 - R_Avg_Im2) <= tolerance)
		)
	//if(R_Avg_Im2 == G_Avg_Im2 == B_Avg_Im2)
	{
		ALGO_Log_1("R_Average2 %f\n",R_Avg_Im2);
		ALGO_Log_1("G_Average2 %f\n",G_Avg_Im2);
		ALGO_Log_1("B_Average2 %f\n",B_Avg_Im2);				
		ALGO_Log_2("ABS(R_Avg_Im2 - G_Avg_Im2) %f, tolerance %f\n", ABS(R_Avg_Im2 - G_Avg_Im2), tolerance);				
		ALGO_Log_2("ABS(G_Avg_Im2 - B_Avg_Im2) %f, tolerance %f\n", ABS(G_Avg_Im2 - B_Avg_Im2), tolerance);				
		ALGO_Log_2("ABS(B_Avg_Im2 - R_Avg_Im2) %f, tolerance %f\n", ABS(B_Avg_Im2 - R_Avg_Im2), tolerance);				
		ALGO_Log_0("Metric BlackAndWhiteEffect : EErrorNone\n");
		errorCode = EErrorNone;
	}
	else
	{
		ALGO_Log_1("R_Average2 %f\n",R_Avg_Im2);
		ALGO_Log_1("G_Average2 %f\n",G_Avg_Im2);
		ALGO_Log_1("B_Average2 %f\n",B_Avg_Im2);				
		ALGO_Log_2("ABS(R_Avg_Im2 - G_Avg_Im2) %f, tolerance %f\n", ABS(R_Avg_Im2 - G_Avg_Im2), tolerance);				
		ALGO_Log_2("ABS(G_Avg_Im2 - B_Avg_Im2) %f, tolerance %f\n", ABS(G_Avg_Im2 - B_Avg_Im2), tolerance);				
		ALGO_Log_2("ABS(B_Avg_Im2 - R_Avg_Im2) %f, tolerance %f\n", ABS(B_Avg_Im2 - R_Avg_Im2), tolerance);				
		ALGO_Log_0("Metric BlackAndWhiteEffect : EErrorNotValidated\n");
		errorCode = EErrorNotValidated;
	}

	return errorCode;
}
