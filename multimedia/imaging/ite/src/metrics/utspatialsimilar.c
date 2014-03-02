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
 * Spatial Similar Unit Test Cases.
 */
TInt32 test_metric_spatialsimilar(char *aFileNameImageEffectDisable, char *aFileNameImageEffectEnable)
   {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
	
    ALGO_Log_0("************* SPATIAL SIMILAR METRIC TEST ************ \n");

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeSpatialSimilar;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, aFileNameImageEffectDisable, aFileNameImageEffectEnable, &aMetricParams);
	Metrcis_Print_Result(ret);

	return 0;
    }

/**
 * Spatial Similar Unit Test Cases.
 */
TInt32 test_metric_spatialsimilar_characterization(TUint8* aDirPath)
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
    
    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM1_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM1_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM2_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM2_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 
    
    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM3_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM3_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 
    
    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM4_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM4_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 
   
    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM5_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM5_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************SPATIAL SIMILAR METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\SpatialSimilar\\SS_IM6_Enable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\SpatialSimilar\\SS_IM6_Disable.bmp");
    aMetricParams.iType = ETypeSpatialSimilar;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

  	return 0;
    }
