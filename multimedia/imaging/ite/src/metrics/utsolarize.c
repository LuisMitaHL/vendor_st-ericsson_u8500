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
 * Solarize Characterization Unit Test Cases.
 */
TInt32 test_metric_solarize_characterization(TUint8* aDirPath)
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
    aMetricParams.iType = ETypeSolarize;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    /*
    ALGO_Log_0("*************SOLARIZE METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM1_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM1_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);


    ALGO_Log_0("*************SOLARIZE METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM2_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM2_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM3_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM3_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM4_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM4_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM5_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM5_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM6_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM6_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 7************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM7_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM7_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 8************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM8_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM8_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 9************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM9_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM9_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 10************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM10_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM10_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 11************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM11_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM11_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 12************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM12_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM12_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 13************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM13_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM13_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 14************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM14_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM14_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 15************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM15_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM15_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
    */
    /*
    ALGO_Log_0("*************SOLARIZE METRIC TEST 16************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM16_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM16_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 17************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM17_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM17_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 18************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM18_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM18_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
    */
    ALGO_Log_0("*************SOLARIZE METRIC TEST 19************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM19_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM19_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 20************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM20_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM20_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 21************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM21_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM21_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************SOLARIZE METRIC TEST 22************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Solarize\\SOLARIZE_IM22_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Solarize\\SOLARIZE_IM22_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);



    return 0;
    }
