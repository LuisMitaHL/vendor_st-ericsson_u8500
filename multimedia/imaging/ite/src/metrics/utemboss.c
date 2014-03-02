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
 * Emboss Characterization Unit Test Cases.
 */
TInt32 test_metric_emboss_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];    

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeEmboss;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    ALGO_Log_0("*************EMBOSS METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM1_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM2_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM2_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM3_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM3_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM4_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM4_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM5_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM5_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM6_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM6_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************EMBOSS METRIC TEST 7************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Emboss\\EMBOSS_IM7_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Emboss\\EMBOSS_IM7_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	return 0;
    }
