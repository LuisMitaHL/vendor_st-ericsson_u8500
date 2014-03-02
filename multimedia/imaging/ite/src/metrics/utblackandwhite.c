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
 * Black and White Characterization Unit Test Cases.
 */
TInt32 test_metric_blackandwhite_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];    

	aMetricParams.iErrorTolerance.iErrorToleranceR = 2;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 2;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 2;
    aMetricParams.iType = ETypeBlackAndWhite;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 2048;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 1536;

    ALGO_Log_0("*************BLACK AND WHITE METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\BlackAndWhite\\bw1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\BlackAndWhite\\bw1\\Output1_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************BLACK AND WHITE METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\BlackAndWhite\\bw1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\BlackAndWhite\\bw1\\Output2_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************BLACK AND WHITE METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\BlackAndWhite\\bw1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\BlackAndWhite\\bw1\\Output3_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
   
    ALGO_Log_0("*************BLACK AND WHITE METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\BlackAndWhite\\bw2\\2592_1944_ColorTone_Normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\BlackAndWhite\\bw2\\2592_1944_ColorTone_GrayScale.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
   
	return 0;
    }
