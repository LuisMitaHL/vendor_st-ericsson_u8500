/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
  * This code is ST-Ericsson proprietary and confidential.
     * Any use of the code for whatever purpose is subject to
      * specific written permission of ST-Ericsson SA.
       */
       /*
       * \file     algowhitebalanceeffect.c
       * \brief    Contains Algorithm Library White Balance Effect validation functions
       * \author   ST Ericsson
       */
       
/*
 * Defines
 */

/*
 * Includes 
 */
#include "algodebug.h"
#include "algowhitebalanceeffect.h"
#include "algoutilities.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"

//#define METRICS_TRACE_WB

 /**
 * Validation Metric for the White Balance Effect.
 *
 * @param aImageEffectDisable           [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable            [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams                 [TAlgoMetricParams] The metric parameters to be applied.
 *
 * @return                              [TAlgoError] EErrorNone if the validation is passed.
 */
/* TAlgoError ValidationMetricWhiteBalanceEffect( const TAlgoImage* aImageEffectDisable, 
					                           const TAlgoImage* aImageEffectEnable,
					                           const TAlgoMetricParams* aMetricParams )
    {
#if 0

    TAlgoWBParams* wbParams = (TAlgoWBParams*)aMetricParams->iParams;
    TAlgoImagesGain imageGain = wbParams->iImageGain;
    TAlgoMatrix colorMatrix = wbParams->iColorMatrix;

    //TReal64 R_Gain_Im1 = imageGain.iGainR_Disable;
    //TReal64 G_Gain_Im1 = imageGain.iGainG_Disable;
    //TReal64 B_Gain_Im1 = imageGain.iGainB_Disable;
    TReal64 R_Gain_Im2 = imageGain.iGainR_Enable;
    TReal64 G_Gain_Im2 = imageGain.iGainG_Enable;
    TReal64 B_Gain_Im2 = imageGain.iGainB_Enable;

    TReal64 constrainerRedA = wbParams->iConstrainerBlueA;
    TReal64 constrainerBlueA = wbParams->iConstrainerBlueA;
    TReal64 constrainerRedB = wbParams->iConstrainerRedB;
    TReal64 constrainerBlueB = wbParams->iConstrainerBlueB;
    TReal64 distanceFromLocus = wbParams->iDistanceFromLocus;

    TReal64 tolerance = 0;

    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;
    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;

    TReal64 inverseGainR_Im1 = 0;
	TReal64 inverseGainG_Im1 = 0;
	TReal64 inverseGainB_Im1 = 0;
    TReal64 inverseGainR_Im2 = 0;
	TReal64 inverseGainG_Im2 = 0;
	TReal64 inverseGainB_Im2 = 0;

    TReal64 channelGainR = 0;
    TReal64 channelGainG = 0;
    TReal64 channelGainB = 0;

    //TReal64 normGain_R_Im1 = 0;
    //TReal64 normGain_B_Im1 = 0;
    TReal64 normGain_R_Im2 = 0;
    TReal64 normGain_B_Im2 = 0;

    TReal64 determinantA = 0;
    TReal64 coFactorMatrix[N][N];
    TReal64 adjMatrix[N][N];
    TReal64 inverseMatrix[N][N];
    
    //TReal32 checkR = 0;
    //TReal32 checkB = 0;


    TReal64 actualdistancefromlocus = 0;
    TReal64 a = 0;
    TReal64 b = 0;
    TReal64 c = 0;

    TReal64 nRgain = R_Gain_Im2 / (R_Gain_Im2 + G_Gain_Im2 + B_Gain_Im2 );
    TReal64 nBgain = B_Gain_Im2 / (R_Gain_Im2 + G_Gain_Im2 + B_Gain_Im2 );
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);

    //First step to check if the that the gains are well constraint in the locus value.
    //If Auto mode, calculate the distance from locus from the height of traiangle
    if( wbParams->iIsAutoMode == 1)
        {
        a = SQUAREROOT(((constrainerRedB - constrainerRedA) * (constrainerRedB - constrainerRedA)) +
                       ((constrainerBlueB - constrainerBlueA) * (constrainerBlueB - constrainerBlueA)));

        b = SQUAREROOT(((constrainerRedB - nRgain) * (constrainerRedB - nRgain)) +
                       ((constrainerBlueB - nBgain) * (constrainerBlueB - nBgain)));

        c = SQUAREROOT(((constrainerRedA - nRgain) * (constrainerRedA - nRgain)) +
                       ((constrainerBlueA - nBgain) * (constrainerBlueA - nBgain)));
      
        actualdistancefromlocus = (1/(2*a))*(SQUAREROOT((a+b+c)*(b+c-a)*(a-b+c)*(a+b-c)));
        }
    //Otherwise calculate the distance between the two values.
    else
        {
        actualdistancefromlocus = SQUAREROOT(((constrainerRedB - nRgain) * (constrainerRedB - nRgain)) +
                                             ((constrainerBlueB - nBgain) * (constrainerBlueB - nBgain)));
        }

    //If the calculated distance is more than the locus, gains are not weel constrained and the test fails.
    if(actualdistancefromlocus > distanceFromLocus)
        {
        return EErrorNotValidated;
        }

    //Next step is to check that the gains are well applied to the image.
    //Calculate the channel average of the two images.
    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    //a1 = colorMatrix[0][0] = a11
    //b1 = colorMatrix[0][1] = a12
    //c1 = colorMatrix[0][2] = a13
    //a2 = colorMatrix[1][0] = a21
    //b2 = colorMatrix[1][1] = a22
    //c2 = colorMatrix[1][2] = a23
    //a3 = colorMatrix[2][0] = a31
    //b3 = colorMatrix[2][1] = a32
    //c3 = colorMatrix[2][2] = a33

    determinantA = ((colorMatrix.iMatrix[0][0])*(((colorMatrix.iMatrix[1][1])*(colorMatrix.iMatrix[2][2]))-((colorMatrix.iMatrix[1][2])*(colorMatrix.iMatrix[2][1]))))
                  -((colorMatrix.iMatrix[0][1])*(((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[2][2]))-((colorMatrix.iMatrix[1][2])*(colorMatrix.iMatrix[2][0]))))
                  +((colorMatrix.iMatrix[0][2])*(((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[2][1]))-((colorMatrix.iMatrix[1][1])*(colorMatrix.iMatrix[2][0]))));
    //determinantA =    a1(b2c3-c2b3) - b1(a2c3-c2a3) + c1(a2b3-b2a3) 

    if(determinantA == 0)
        {
        return EErrorImageNotRelevant;
        }

    //Calculate the minor and cofactor of the matrix.
    coFactorMatrix[0][0] = ((colorMatrix.iMatrix[1][1])*(colorMatrix.iMatrix[2][2])) 
                         - ((colorMatrix.iMatrix[2][1])*(colorMatrix.iMatrix[1][2]));    //a22xa33 - a32xa23
    coFactorMatrix[0][1] = ((colorMatrix.iMatrix[2][0])*(colorMatrix.iMatrix[1][2])) 
                         - ((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[2][2]));    //-(a21xa33 - a31xa23)
    coFactorMatrix[0][2] = ((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[2][1])) 
                         - ((colorMatrix.iMatrix[2][0])*(colorMatrix.iMatrix[1][1]));    //a21xa32 - a31xa22 
    coFactorMatrix[1][0] = ((colorMatrix.iMatrix[2][1])*(colorMatrix.iMatrix[0][2]))
                         - ((colorMatrix.iMatrix[0][1])*(colorMatrix.iMatrix[2][2]));    //-(a12xa33 - a32xa13)
    coFactorMatrix[1][1] = ((colorMatrix.iMatrix[0][0])*(colorMatrix.iMatrix[2][2])) 
                         - ((colorMatrix.iMatrix[2][0])*(colorMatrix.iMatrix[0][2]));    //a11xa33 - a31xa13
    coFactorMatrix[1][2] = ((colorMatrix.iMatrix[2][0])*(colorMatrix.iMatrix[0][1]))
                         - ((colorMatrix.iMatrix[0][0])*(colorMatrix.iMatrix[2][1]));    //-(a11xa32 - a31xa12)
    coFactorMatrix[2][0] = ((colorMatrix.iMatrix[0][1])*(colorMatrix.iMatrix[1][2])) 
                         - ((colorMatrix.iMatrix[1][1])*(colorMatrix.iMatrix[0][2]));    //a12xa23 - a22xa13
    coFactorMatrix[2][1] = ((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[0][2]))
                         - ((colorMatrix.iMatrix[0][0])*(colorMatrix.iMatrix[1][2]));    //-(a11xa23 - a21xa13)
    coFactorMatrix[2][2] = ((colorMatrix.iMatrix[0][0])*(colorMatrix.iMatrix[1][1])) 
                         - ((colorMatrix.iMatrix[1][0])*(colorMatrix.iMatrix[0][1]));    //a11xa22 - a21xa12         


    //Calculate the adjoint of the color matrix.
    adjMatrix[0][0] = coFactorMatrix[0][0];
    adjMatrix[0][1] = coFactorMatrix[1][0];
    adjMatrix[0][2] = coFactorMatrix[2][0];
    adjMatrix[1][0] = coFactorMatrix[0][1];
    adjMatrix[1][1] = coFactorMatrix[1][1];
    adjMatrix[1][2] = coFactorMatrix[2][1];
    adjMatrix[2][0] = coFactorMatrix[0][2];
    adjMatrix[2][1] = coFactorMatrix[1][2];
    adjMatrix[2][2] = coFactorMatrix[2][2];

    //Calculate the inverse of the color matrix.
    inverseMatrix[0][0] = (adjMatrix[0][0]) / determinantA;
    inverseMatrix[0][1] = (adjMatrix[0][1]) / determinantA;
    inverseMatrix[0][2] = (adjMatrix[0][2]) / determinantA;
    inverseMatrix[1][0] = (adjMatrix[1][0]) / determinantA;
    inverseMatrix[1][1] = (adjMatrix[1][1]) / determinantA;
    inverseMatrix[1][2] = (adjMatrix[1][2]) / determinantA;
    inverseMatrix[2][0] = (adjMatrix[2][0]) / determinantA;
    inverseMatrix[2][1] = (adjMatrix[2][1]) / determinantA;
    inverseMatrix[2][2] = (adjMatrix[2][2]) / determinantA;

    //Apply the inverse color matrix on the gains of the images.
    inverseGainR_Im1 = inverseMatrix[0][0] * (R_Avg_Im1) + 
                       inverseMatrix[0][1] * (G_Avg_Im1) + 
                       inverseMatrix[0][2] * (B_Avg_Im1);   

    inverseGainG_Im1 = inverseMatrix[1][0] * (R_Avg_Im1) + 
                       inverseMatrix[1][1] * (G_Avg_Im1) + 
                       inverseMatrix[1][2] * (B_Avg_Im1);   

    inverseGainB_Im1 = inverseMatrix[2][0] * (R_Avg_Im1) + 
                       inverseMatrix[2][1] * (G_Avg_Im1) + 
                       inverseMatrix[2][2] * (B_Avg_Im1);

    inverseGainR_Im2 = inverseMatrix[0][0] * (R_Avg_Im2) + 
                       inverseMatrix[0][1] * (G_Avg_Im2) + 
                       inverseMatrix[0][2] * (B_Avg_Im2);   

    inverseGainG_Im2 = inverseMatrix[1][0] * (R_Avg_Im2) + 
                       inverseMatrix[1][1] * (G_Avg_Im2) + 
                       inverseMatrix[1][2] * (B_Avg_Im2);   

    inverseGainB_Im2 = inverseMatrix[2][0] * (R_Avg_Im2) + 
                       inverseMatrix[2][1] * (G_Avg_Im2) + 
                       inverseMatrix[2][2] * (B_Avg_Im2);


    channelGainR = inverseGainR_Im2 / inverseGainR_Im1;
    channelGainG = inverseGainG_Im2 / inverseGainG_Im1;
    channelGainB = inverseGainB_Im2 / inverseGainB_Im1;
       
    normGain_R_Im2 =  channelGainR / (channelGainR + channelGainG + channelGainB);
    normGain_B_Im2 =  channelGainB / (channelGainR + channelGainG + channelGainB);

    ALGO_Log_2("normGain_R_Im2 = %10f, normGain_B_Im2 = %10f\n", normGain_R_Im2, normGain_B_Im2);
    ALGO_Log_2("nRgain = %10f, nBgain = %10f\n", nRgain, nBgain);

    tolerance = aMetricParams->iErrorTolerance.iErrorToleranceR / 100;

    if((ABS(normGain_R_Im2 - nRgain) < tolerance) &&
       (ABS(normGain_B_Im2 - nBgain) < tolerance))
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }

#else
    TAlgoImagesGain* iImageGain = (TAlgoImagesGain*)aMetricParams->iParams; 

    TReal64 R_Gain_Im1 = iImageGain->iGainR_Disable;
    TReal64 G_Gain_Im1 = iImageGain->iGainG_Disable;
    TReal64 B_Gain_Im1 = iImageGain->iGainB_Disable;

    TReal64 R_Gain_Im2 = iImageGain->iGainR_Enable;
    TReal64 G_Gain_Im2 = iImageGain->iGainG_Enable;
    TReal64 B_Gain_Im2 = iImageGain->iGainB_Enable;

    TReal64 t_Gain_R = R_Gain_Im2/R_Gain_Im1;
    TReal64 t_Gain_G = G_Gain_Im2/G_Gain_Im1;
    TReal64 t_Gain_B = B_Gain_Im2/B_Gain_Im1;

    TReal64 e_Gain_R = 0;
    TReal64 e_Gain_G = 0;
    TReal64 e_Gain_B = 0;

    TReal64 R_Avg_Im1 = 0;
	TReal64 G_Avg_Im1 = 0;
	TReal64 B_Avg_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
	TReal64 G_Avg_Im2 = 0;
	TReal64 B_Avg_Im2 = 0;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);

#ifdef METRICS_TRACE_WB
    ALGO_Log_3("t_Gain_R = %10f, t_Gain_G %10f, t_Gain_B %10f\n", t_Gain_R, t_Gain_G, t_Gain_B);
#endif

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );

    e_Gain_R = R_Avg_Im2/R_Avg_Im1;
    e_Gain_G = G_Avg_Im2/G_Avg_Im1;
    e_Gain_B = B_Avg_Im2/B_Avg_Im1;

#ifdef METRICS_TRACE_WB
	ALGO_Log_3("e_Gain_R = %10f, e_Gain_G %10f, e_Gain_B %10f\n", e_Gain_R, e_Gain_G, e_Gain_B);
#endif

    if((t_Gain_R >= t_Gain_G) && (t_Gain_G >= t_Gain_B))
        {
        //if((e_Gain_R >= e_Gain_G) && (e_Gain_G >= e_Gain_B))
        if(((e_Gain_R > e_Gain_G)||(ABS(e_Gain_R - e_Gain_G) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_G > e_Gain_B)||(ABS(e_Gain_G - e_Gain_B) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

    if((t_Gain_R >= t_Gain_G) && (t_Gain_G <= t_Gain_B) && (t_Gain_R >= t_Gain_B))
        {
        //if((e_Gain_R >= e_Gain_G) && (e_Gain_G <= e_Gain_B) && (e_Gain_R >= e_Gain_B))
        if(((e_Gain_R > e_Gain_G)||(ABS(e_Gain_R - e_Gain_G) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_B > e_Gain_G)||(ABS(e_Gain_B - e_Gain_G) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_R > e_Gain_B)||(ABS(e_Gain_R - e_Gain_B) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

    if((t_Gain_G >= t_Gain_R) && (t_Gain_R >= t_Gain_B))
        {
        //if((e_Gain_G >= e_Gain_R) && (e_Gain_R >= e_Gain_B))
        if(((e_Gain_G > e_Gain_R)||(ABS(e_Gain_G - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_R > e_Gain_B)||(ABS(e_Gain_R - e_Gain_B) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

    if((t_Gain_G >= t_Gain_R) && (t_Gain_R <= t_Gain_B) && (t_Gain_G >= t_Gain_B))
        {
        //if((e_Gain_G >= e_Gain_R) && (e_Gain_R <= e_Gain_B) && (e_Gain_G >= e_Gain_B))
        if(((e_Gain_G > e_Gain_R)||(ABS(e_Gain_G - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_B > e_Gain_R)||(ABS(e_Gain_B - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_G > e_Gain_B)||(ABS(e_Gain_G - e_Gain_B) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

    if((t_Gain_B >= t_Gain_R) && (t_Gain_R >= t_Gain_G))
        {
        //if((e_Gain_B >= e_Gain_R) && (e_Gain_R >= e_Gain_G))
        if(((e_Gain_B > e_Gain_R)||(ABS(e_Gain_B - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_R > e_Gain_G)||(ABS(e_Gain_R - e_Gain_G) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

    if((t_Gain_B >= t_Gain_R) && (t_Gain_R <= t_Gain_G) && (t_Gain_B >= t_Gain_G))
        {
        //if((e_Gain_B >= e_Gain_R) && (e_Gain_R <= e_Gain_G) && (e_Gain_B >= e_Gain_G))
        if(((e_Gain_B > e_Gain_R)||(ABS(e_Gain_B - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_G > e_Gain_R)||(ABS(e_Gain_G - e_Gain_R) <= (aMetricParams->iErrorTolerance.iErrorToleranceR)))&&
           ((e_Gain_B > e_Gain_G)||(ABS(e_Gain_B - e_Gain_G) <= (aMetricParams->iErrorTolerance.iErrorToleranceR))))
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNone;
            }
        else
            {
#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
            return EErrorNotValidated;
            }
        }

#ifdef METRICS_TRACE_WB
				ALGO_Log_1("\t WB exits on line : %d\n", __LINE__);
#endif
    return EErrorImageNotRelevant;  

#endif
    }

*/
 TAlgoError ValidationMetricWhiteBalanceEffect( const TAlgoImage* aImageEffectDisable, 
					        const TAlgoImage* aImageEffectEnable,
					        const TAlgoMetricParams* aMetricParams )
    {

    TAlgoWBType* iImageWbPreset = (TAlgoWBType*)aMetricParams->iParams; 

    TReal64 e_Gain_R = 0;
    TReal64 e_Gain_G = 0;
    TReal64 e_Gain_B = 0;

    TReal64 R_Avg_Im1 = 0;
    TReal64 G_Avg_Im1 = 0;
    TReal64 B_Avg_Im1 = 0;

    TReal64 R_Avg_Im2 = 0;
    TReal64 G_Avg_Im2 = 0;
    TReal64 B_Avg_Im2 = 0;
    TAlgoError Error = EErrorNotValidated;
    TUint32 temp;
	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);

    Channel_Average( aImageEffectDisable, &(aMetricParams->iTestCoordinates), &R_Avg_Im1, &G_Avg_Im1, &B_Avg_Im1 );
    //ALGO_Log_3("R_Avg_Im1 = %10f, G_Avg_Im1 %10f, B_Avg_Im1 %10f\n", R_Avg_Im1, G_Avg_Im1, B_Avg_Im1);
    Channel_Average( aImageEffectEnable, &(aMetricParams->iTestCoordinates), &R_Avg_Im2, &G_Avg_Im2, &B_Avg_Im2 );
    //ALGO_Log_3("R_Avg_Im2 = %10f, G_Avg_Im2 %10f, B_Avg_Im2 %10f\n", R_Avg_Im2, G_Avg_Im2, B_Avg_Im2);

    e_Gain_R = R_Avg_Im2/R_Avg_Im1;
    e_Gain_G = G_Avg_Im2/G_Avg_Im1;
    e_Gain_B = B_Avg_Im2/B_Avg_Im1;

    ALGO_Log_3(" e_Gain_R = %10f, e_Gain_G %10f, e_Gain_B %10f\n", e_Gain_R, e_Gain_G, e_Gain_B);

   switch(*iImageWbPreset)
    {
   	case EWhiteBalControlOff:
   	
		Error = EErrorNotValidated;
	break;
	
	case EWhiteBalControlAuto:
		// auto mode	
		temp = (TUint32)(aMetricParams->iErrorTolerance.iErrorToleranceR);
		ALGO_Log_0(" Metric  EWhiteBalControlAuto:\n");
		switch(temp)
		{
	
			// compare OFF and AUTO images
			case 1:
			if( (e_Gain_R>e_Gain_B) && (e_Gain_R>e_Gain_G) ) Error = EErrorNone;
			//if(e_Gain_R>5) Error = EErrorNone;
			else Error = EErrorNotValidated;
			break;
		
			default:
			Error = EErrorNotValidated;
			break;
		} 
    		/*
		temp = (TUint32)(aMetricParams->iErrorTolerance.iErrorToleranceR);

		switch(temp)
		{
			// test with blue ligth
			// blue average is more important, so WB apply hight gain for Red color
			case 1:
			if( (e_Gain_R>e_Gain_G) && (e_Gain_R>e_Gain_B)) Error = EErrorNone;
			else Error = EErrorNotValidated;
			break;
			// test with red ligth
			// red average is more important, so WB apply hight gain for blue color			
			case 2:
			if( (e_Gain_B>e_Gain_R) && (e_Gain_B>e_Gain_G)) Error = EErrorNone;
			else Error = EErrorNotValidated;
			break;
			// test with green ligth
			// blue average is more important, so WB apply hight gain fo Red color
		
			default:
			Error = EErrorNotValidated;
			break;
		} 
		*/
	break;
	
	case EWhiteBalControlSunLight:
                                // simple test to check: we use white light in the drakbox.
                                // we apply the auto wb for the first image
                                // and sun light preset for the second
                                // SunLight is difficult to test with RGB gain
                                // We can measure picture temperature or just read exif temperature data...
                                ALGO_Log_0(" Metric  EWhiteBalControlSunLight:\n");
                                //if ( (e_Gain_R<0.8) Error = EErrorNone;
                                //if ( (e_Gain_G>e_Gain_R) && (e_Gain_G>e_Gain_B)) 
                                Error = EErrorNone;
                                //else Error = EErrorNotValidated;
        break;
                
        case EWhiteBalControlCloudy:
                                // simple test to check: we use white light in the drakbox.
                                // we apply the auto wb for the first image
                                // and cloudy preset for the second
                                // Cloudy is difficult to test with RGB gain
                                // We can measure picture temperature or just read exif temperature data...
                                ALGO_Log_0(" Metric  EWhiteBalControlCloudy:\n");                     
                                //if ( (e_Gain_G>e_Gain_R) && (e_Gain_B>e_Gain_R) )
                                Error = EErrorNone;
                                //else Error = EErrorNotValidated;
       break;
                
       case EWhiteBalControlShade:
                                Error = EErrorNotValidated;
       break;
                                                                
       case EWhiteBalControlTungsten:
                                // simple test to check: we use white light in the drakbox.
                                // we apply the auto wb for the first image
                                // and sun light preset for the second
                                // We check that R < G < B
                                // It is better to measure picture temperature or just read exif temperature data...
                                ALGO_Log_0(" Metric  EWhiteBalControlTungsten:\n");                                
                                if ( (e_Gain_R < e_Gain_G) && (e_Gain_G < e_Gain_B) )Error = EErrorNone;
                                else Error = EErrorNotValidated;
       break;
                
       case EWhiteBalControlFluorescent:
                                // simple test to check: we use white light in the drakbox.
                                // we apply the auto wb for the first image
                                // and sun light preset for the second
                                // We check that R < G and R< 1
                                // It is better to measure picture temperature or just read exif temperature data...
                                ALGO_Log_0(" Metric  EWhiteBalControlFluorescent:\n");                           
                                if ( (e_Gain_R < e_Gain_B) && (e_Gain_R < 1) )Error = EErrorNone;
                                else Error = EErrorNotValidated;
       break;

	
	case EWhiteBalControlIncandescent:
		Error = EErrorNotValidated;
	break;
	
	case EWhiteBalControlFlash:
		Error = EErrorNotValidated;
	break;
	
	case EWhiteBalControlHorizon:
		Error = EErrorNotValidated;
	break;
	
	default:
	Error = EErrorNotValidated;
	break;
   }
        
    return Error;
     

    }

