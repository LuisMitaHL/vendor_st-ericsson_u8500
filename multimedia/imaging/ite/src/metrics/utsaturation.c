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
 * Saturation Characterization Unit Test Cases.
 */
TInt32 test_metric_saturation_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNotValidated; 
	TAlgoMetricParams aMetricParams;
    TAlgoImageParams aImageParams;
    TUint8 dir[128];
    TUint8 dir1[128];   
   
	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeSaturation;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    ALGO_Log_0("*************SATURATION METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM1_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM2_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM2_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM3_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM3_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM4_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM4_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM5_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM5_Enable.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM6_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM6_Enable.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 7_20************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM7_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM7_Enable_20.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 7_40************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM7_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM7_Enable_40.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 7_60************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM7_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM7_Enable_60.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 7_80************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM7_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM7_Enable_80.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SATURATION METRIC TEST 7_100************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Saturation\\SATURATION_IM7_Disable.yuv");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Saturation\\SATURATION_IM7_Enable_100.yuv");
    aImageParams.iImageFormat = EImageFormatYUV422Interleaved;
    aImageParams.iImageHeight = 480;
    aImageParams.iImageWidth = 640;
    aImageParams.iImageHeaderLength = 0;
    ret = ValidationMetricInterfaceWithFiles(&aImageParams, &aImageParams, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

   	return 0;
    }
