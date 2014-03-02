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
 * Zoom Characterization Unit Test Cases.
 */
TInt32 test_metric_zoom_characterization(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    TUint8 dir[128];
    TUint8 dir1[128];  
	TReal64 zoomfactor = 1;

	aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 5
        ;
    aMetricParams.iType = ETypeZoom;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;

    ALGO_Log_0("*************ZOOM METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_850.bmp");
	zoomfactor = ((TReal64)2048/850);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1000.bmp");
	zoomfactor = ((TReal64)2048/1000);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1200.bmp");
	zoomfactor = ((TReal64)2048/1200);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
   
    ALGO_Log_0("*************ZOOM METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1250.bmp");
	zoomfactor = ((TReal64)2048/1250);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1400.bmp");
	zoomfactor = ((TReal64)2048/1400);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1600.bmp");
	zoomfactor = ((TReal64)2048/1600);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 7************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1700.bmp");
	zoomfactor = ((TReal64)2048/1700);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 8************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1800.bmp");
	zoomfactor = ((TReal64)2048/1800);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 9************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_1900.bmp");
	zoomfactor = ((TReal64)2048/1900);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

	ALGO_Log_0("*************ZOOM METRIC TEST 10************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_FOV_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_FOV_2048.bmp");
	zoomfactor = ((TReal64)2048/2048);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 11************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\ZOOM_IM1_Disable_ZF2.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\ZOOM_IM1_Enable_ZF2.bmp");
	zoomfactor = 2;
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 12************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\ZOOM_IM2_Disable_ZF2.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\ZOOM_IM2_Enable_ZF2.bmp");
	zoomfactor = 2;
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 13************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\Image_WOI_2048.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\Image_WOI_1800.bmp");
	zoomfactor = ((TReal64)2048/1800);
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);

    ALGO_Log_0("*************ZOOM METRIC TEST 14************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\Zoom\\ZOOM_IM3_Disable_ZF2.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\Zoom\\ZOOM_IM3_Enable_ZF2.bmp");
	zoomfactor = 2;
	aMetricParams.iParams = &zoomfactor;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
	Metrcis_Print_Result(ret);
   
	return 0;
    }
