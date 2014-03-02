/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "algoerror.h"
#include "algotypedefs.h"
#include "algodebug.h"
#include "algointerface.h"
#include "algostring.h"

/**
 * Gamma Characterization Unit Test Cases.
 */
TInt32 test_metric_gamma_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128]; 
    TUint32 memDisable = 0;
    TUint32 memEnable = 0;
    //TAlgoImageParams aImageParams;
    TUint64 size = 0;
   
	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeGamma;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    /*
    ALGO_Log_0("*************GAMMA METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Gamma\\GAMMA_IM1_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Gamma\\GAMMA_IM1_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************GAMMA METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Gamma\\GAMMA_IM2_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Gamma\\GAMMA_IM2_Enable.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret =  ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );

    Metrcis_Print_Result(ret);
    */

    ALGO_Log_0("*************GAMMA METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Gamma\\GAMMA_IM3_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Gamma\\GAMMA_IM3_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    return 0;
    }
