/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algosaturationeffect.c
* \brief    Contains Algorithm Library Saturation Effect validation functions
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
#include "algosaturationeffect.h"

/**
 * Validation Metric for the Saturation Effect.
 *
 * @param aImageEffectDisable       [TAlgoImage] The image with Effect Disable.
 * @param aImageEffectEnable        [TAlgoImage] The image with Effect Enable.
 * @param aMetricParams             [TAlgoMetricParams]The metric parameters to be applied.
 *
 * @return                          [TAlgoError] EErrorNone if the validation is passed.
 */
TAlgoError ValidationMetricSaturationEffect( const TAlgoImage* aImageEffectDisable, 
					                         const TAlgoImage* aImageEffectEnable,
					                         const TAlgoMetricParams* aMetricParams )
    {
    TUint32 x = 0;
    TUint32 y = 0;
    TUint32 nImax1 = 0; 
    TUint32 nImin1 = 0;
    TUint32 nSum1 = 0;
    TUint32 nDifference1 = 0;
    TReal64 Intensity1 = 0;
    TReal64 Saturation1 = 0;
    TReal64 Sat1 = 0;
    TUint32 nImax2 = 0; 
    TUint32 nImin2 = 0;
    TUint32 nSum2 = 0;
    TUint32 nDifference2 = 0;
    TReal64 Intensity2 = 0;
    TReal64 Saturation2 = 0;
    TReal64 Sat2 = 0;
    TReal64 tolerance = 0;
    TUint32 startingOffset = aMetricParams->iTestCoordinates.iStartIndexY * aImageEffectDisable->iImageParams.iImageWidth
                             + aMetricParams->iTestCoordinates.iStartIndexX;
    TUint32 offset = (aImageEffectDisable->iImageParams.iImageWidth - aMetricParams->iTestCoordinates.iPixelsToGrabX);
    TUint8 *iR = aImageEffectDisable->iImageData.iRChannel + startingOffset;
    TUint8 *iG = aImageEffectDisable->iImageData.iGChannel + startingOffset;
    TUint8 *iB = aImageEffectDisable->iImageData.iBChannel + startingOffset;
    TUint8 *oR = aImageEffectEnable->iImageData.iRChannel + startingOffset;
    TUint8 *oG = aImageEffectEnable->iImageData.iGChannel + startingOffset;
    TUint8 *oB = aImageEffectEnable->iImageData.iBChannel + startingOffset;
// remove this line for the 2eme time (not used)    TAlgoParamSaturation *paramSaturation = (TAlgoParamSaturation*)aMetricParams->iParams;

	ALGO_Log_1("entering %s\n", __FUNCTION__);
	ALGO_Log_3("ToleranceR = %f, ToleranceG = %f, ToleranceB = %f\n", aMetricParams->iErrorTolerance.iErrorToleranceR, aMetricParams->iErrorTolerance.iErrorToleranceG, aMetricParams->iErrorTolerance.iErrorToleranceB);
// remove this line for the 2eme time (not used and crash) 	ALGO_Log_2("Saturation Metric : iPrSaturation_1 %d, iPrSaturation_2 %d\n\n", paramSaturation->iPrSaturation_1, paramSaturation->iPrSaturation_2);

    for(y=0; y < aMetricParams->iTestCoordinates.iPixelsToGrabY - 1; y++)
        {
        for (x=0; x < aMetricParams->iTestCoordinates.iPixelsToGrabX - 1; x++)
		    {        
            //if(( (*iR)<0 && (*iG)<0 && (*iB)<0 ) || ( (*iR)>255 ||(*iG)>255 || (*iG)>255) )
            //    {
            //    continue;
            //    }

            if((*iG) == (*iB))
                {
                if((*iB) < 255)
                    {
                    (*iB) = (*iB) + 1;
                    }
                else
                    {
                    (*iB) = (*iB) - 1;
                    }
                }

            nImax1 = MAX_NUM((*iR),(*iB));
            nImax1 = MAX_NUM(nImax1,(*iG));
            nImin1 = MIN_NUM((*iR),(*iB));
            nImin1 = MIN_NUM(nImin1,(*iG));
            nSum1 = nImin1 + nImax1;
            nDifference1 = nImax1 - nImin1;
  
            Intensity1 = (TReal64)nSum1/2;

            if(Intensity1 < 128)
                {
                Saturation1 = ( 255*((TReal64)nDifference1/nSum1));
                }
            else
                {
                Saturation1 = (TReal64)(255*((TReal64)nDifference1/(510-nSum1)));
                }

            Sat1 = Sat1 + Saturation1;

            //if(( (*oR)<0 && (*oG)<0 && (*oB)<0 ) || ( (*oR)>255 ||(*oG)>255 || (*oG)>255) )
            //    {
            //    continue;
            //    }

            if((*oG) == (*oB))
                {
                if((*oB) < 255)
                    {
                    (*oB) = (*oB) + 1;
                    }
                else
                    {
                    (*oB) = (*oB) - 1;
                    }
                }

            nImax2 = MAX_NUM((*oR),(*oB));
            nImax2 = MAX_NUM(nImax2,(*oG));
            nImin2 = MIN_NUM((*oR),(*oB));
            nImin2 = MIN_NUM(nImin2,(*oG));
            nSum2 = nImin2 + nImax2;
            nDifference2 = nImax2 - nImin2;
  
            Intensity2 = (TReal64)nSum2/2;

            if(Intensity2 < 128)
                {
                Saturation2 = (255*((TReal64)nDifference2/nSum2));
                }
            else
                {
                Saturation2 = (TReal64)(255*((TReal64)nDifference2/(510-nSum2)));
                }

            Sat2 = Sat2 + Saturation2;

            iR++;
            iG++;
            iB++;
			oR++;
            oG++;
            oB++;
            }
        //ALGO_Log_2(" x = %10d,  y = %10d\n", x, y);
        //ALGO_Log_2("Saturation Image1 = %10f, Saturation Image2 = %10f\n", Sat1, Sat2);
	    iR += offset;
        iG += offset;
        iB += offset;
		oR += offset;
        oG += offset;
        oB += offset;
        }

    Sat1 = Sat1/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);
    Sat2 = Sat2/(aMetricParams->iTestCoordinates.iPixelsToGrabX * aMetricParams->iTestCoordinates.iPixelsToGrabY);

    ALGO_Log_2("Saturation Image1 = %10f, Saturation Image2 = %10f\n", Sat1, Sat2);

    tolerance = SATURATION_THRESHOLD - ((aMetricParams->iErrorTolerance.iErrorToleranceR * SATURATION_THRESHOLD)/100);

    if((Sat2 - Sat1) > tolerance)
        {
        return EErrorNone;
        }
    else
        {
        return EErrorNotValidated;
        }
    }
