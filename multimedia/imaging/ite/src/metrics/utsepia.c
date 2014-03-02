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
 * Sepia Characterization Unit Test Cases.
 */
TInt32 test_metric_sepia_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];    

	aMetricParams.iErrorTolerance.iErrorToleranceR = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 1;
    aMetricParams.iType = ETypeSepia;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    ALGO_Log_0("*************SEPIA METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Sepia\\sep1\\2592_1944_ColorTone_Normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Sepia\\sep1\\2592_1944_ColorTone_Sepia.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SEPIA METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Sepia\\sep2\\Image_colortone_normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Sepia\\sep2\\Image_colortone_sepia.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SEPIA METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Sepia\\sep3\\t_recorder_st_core_normal.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Sepia\\sep3\\t_recorder_st_core_sepia.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);


	return 0;
    }
