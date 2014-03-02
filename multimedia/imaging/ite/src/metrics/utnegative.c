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
 * Negative Characterization Unit Test Cases.
 */
TInt32 test_metric_negative_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
    TAlgoMetricRange metricRange;
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];  

	metricRange.iRangeR = 0;
    metricRange.iRangeG = 0;
    metricRange.iRangeB = 0;

    aMetricParams.iErrorTolerance.iErrorToleranceR = 2;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 2;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 2;
    aMetricParams.iType = ETypeNegativeImage;
    aMetricParams.iMetricRange = metricRange;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 2048;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 1536;

    ALGO_Log_0("*************NEGATIVE METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg1\\Output1_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************NEGATIVE METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg1\\Output2_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
   
    ALGO_Log_0("*************NEGATIVE METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg1\\Input_rgb16_2048_1536.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg1\\Output3_rgb16_2048_1536.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************NEGATIVE METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg2\\2592_1944_ColorTone_Normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg2\\2592_1944_ColorTone_Negative.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    
    ALGO_Log_0("*************NEGATIVE METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg3\\Image_no_effect.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg3\\Image_negative_enabled.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************NEGATIVE METRIC TEST 6************ \n");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg4\\Image_normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg4\\Image_negative.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************NEGATIVE METRIC TEST 7************ \n");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg5\\Image_normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg5\\Image_negative.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************NEGATIVE METRIC TEST 8************ \n");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg6\\Image_Normal.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg6\\Image_Negative.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
    
    ALGO_Log_0("*************NEGATIVE METRIC TEST 9************ \n");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Negative\\neg7\\Image_normal.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Negative\\neg7\\Image_negative.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
    
	return 0;
    }
