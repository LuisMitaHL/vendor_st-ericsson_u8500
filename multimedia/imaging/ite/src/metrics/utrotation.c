/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "algoerror.h"
#include "algodebug.h"
#include "algotypedefs.h"
#include "algointerface.h"
#include "algostring.h"

/**
 * Rotation Unit Test Cases.
 */
TInt32 test_metric_rotation(char *aFileNameImageEffectDisable, char *aFileNameImageEffectEnable)
   {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
	
    ALGO_Log_0("************* ROTATION METRIC TEST ************ \n");

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = EType90degreesRotation;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, aFileNameImageEffectDisable, aFileNameImageEffectEnable, &aMetricParams);
	Metrcis_Print_Result(ret);

	return 0;
    }

/**
 * Rotation Unit Test Cases.
 */
TInt32 test_metric_rotation_characterization(TUint8* aDirPath)
    {

    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    
    TUint8 dir[128];
    TUint8 dir1[128];    


	aMetricParams.iErrorTolerance.iErrorToleranceR = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 1;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    
    ALGO_Log_0("*************ROTATION METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_90_Enable.bmp");
    aMetricParams.iType = EType90degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    aMetricParams.iTestCoordinates.iStartIndexX = 100;
    aMetricParams.iTestCoordinates.iStartIndexY = 100;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 200;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 200;
    
    ALGO_Log_0("*************ROTATION METRIC TEST 1.1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_90_Enable.bmp");
    aMetricParams.iType = EType90degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

	aMetricParams.iErrorTolerance.iErrorToleranceR = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 1;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;


    ALGO_Log_0("*************ROTATION METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_180_Enable.bmp");
    aMetricParams.iType = EType180degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

	aMetricParams.iErrorTolerance.iErrorToleranceR = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 1;
    aMetricParams.iTestCoordinates.iStartIndexX = 100;
    aMetricParams.iTestCoordinates.iStartIndexY = 100;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 100;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 100;


    ALGO_Log_0("*************ROTATION METRIC TEST 2.1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_180_Enable.bmp");
    aMetricParams.iType = EType180degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

	aMetricParams.iErrorTolerance.iErrorToleranceR = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 1;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 1;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    ALGO_Log_0("*************ROTATION METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_270_Enable.bmp");
    aMetricParams.iType = EType270degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

	aMetricParams.iTestCoordinates.iStartIndexX = 100;
    aMetricParams.iTestCoordinates.iStartIndexY = 100;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 200;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 200;

    ALGO_Log_0("*************ROTATION METRIC TEST 3.1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Rotation\\ROTATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Rotation\\ROTATION_270_Enable.bmp");
    aMetricParams.iType = EType270degreesRotation;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

  	return 0;
    }
