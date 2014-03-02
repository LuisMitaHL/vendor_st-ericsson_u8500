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
 * Color Tone Characterization Unit Test Cases.
 */
TInt32 test_metric_colortone_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];   
    TAlgoMatrix matrix;
    TUint32 memDisable = 0;
    TUint32 memEnable = 0;
    //TUint8* bufferEffectDisable;
    //TUint8* bufferEffectEnable;
    //TAlgoImageParams aImageParams;
    //TAny* file;
    //TAny* file1;
    TUint64 size = 0;

	aMetricParams.iErrorTolerance.iErrorToleranceR = 20;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 20;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 20;
    aMetricParams.iType = ETypeColorTone;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    matrix.iMatrix[0][0] = (TReal32)2.5415;
    matrix.iMatrix[0][1] = (TReal32)-1.5587;
    matrix.iMatrix[0][2] = (TReal32)0.0173;
    matrix.iMatrix[1][0] = (TReal32)-0.6080;
    matrix.iMatrix[1][1] = (TReal32)2.2818;
    matrix.iMatrix[1][2] = (TReal32)-0.6739;
    matrix.iMatrix[2][0] = (TReal32)-0.0365;
    matrix.iMatrix[2][1] = (TReal32)-0.6038;
    matrix.iMatrix[2][2] = (TReal32)1.6403;

    aMetricParams.iParams = &matrix;

    ALGO_Log_0("*************COLOR TONE METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\ColorTone\\COLORTONE_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\ColorTone\\COLORTONE_IM1_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************COLOR TONE METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\ColorTone\\COLORTONE_IM2_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\ColorTone\\COLORTONE_IM2_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************COLOR TONE METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\ColorTone\\COLORTONE_IM3_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\ColorTone\\COLORTONE_IM3_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);


	Metrcis_Print_Result(ret);

   	return 0;
    }
