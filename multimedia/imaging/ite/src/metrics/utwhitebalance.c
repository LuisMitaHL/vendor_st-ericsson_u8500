/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
  * This code is ST-Ericsson proprietary and confidential.
   * Any use of the code for whatever purpose is subject to
    * specific written permission of ST-Ericsson SA.
     */
#include "algoerror.h"
#include "algodebug.h"
#include "algotypedefs.h"
#include "algofileroutines.h"
#include "algomemoryroutines.h"
#include "algoextractionroutines.h"
#include "algointerface.h"
#include "algostring.h"


/**
 * Whitebalance Unit Test Cases.
 */
TInt32 test_metric_whitebalance(const TUint8* aFileNameImageEffectDisable, 
					            const TUint8* aFileNameImageEffectEnable,
					            TAlgoMetricParams* aMetricParams )
    {
    TAlgoError ret = EErrorNone; 
	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, aFileNameImageEffectDisable, aFileNameImageEffectEnable, aMetricParams );
	Metrcis_Print_Result(ret);
	return 0;
    }

/**
 * Whitebalance Characterization Unit Test Cases.
 */
TInt32 test_metric_whitebalance_characterization(TUint8* aDirPath)
    {
#if 0

    TAlgoError ret = EErrorNone; 
    TAlgoImagesGain aImagesGain;
	TAlgoMetricParams aMetricParams;
    TAlgoWBParams aWBParams;
    TAlgoMatrix matrix;
    
    TUint8 dir[256];
    TUint8 dir1[256];    

	aMetricParams.iErrorTolerance.iErrorToleranceR = (TReal32)5;
    aMetricParams.iErrorTolerance.iErrorToleranceG = (TReal32)5;
    aMetricParams.iErrorTolerance.iErrorToleranceB = (TReal32)5;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    aMetricParams.iType = ETypeWhiteBalance;

    matrix.iMatrix[0][0] = (TReal32)2.5415;
    matrix.iMatrix[0][1] = (TReal32)-1.5587;
    matrix.iMatrix[0][2] = (TReal32)0.0173;
    matrix.iMatrix[1][0] = (TReal32)-0.6080;
    matrix.iMatrix[1][1] = (TReal32)2.2818;
    matrix.iMatrix[1][2] = (TReal32)-0.6739;
    matrix.iMatrix[2][0] = (TReal32)-0.0365;
    matrix.iMatrix[2][1] = (TReal32)-0.6038;
    matrix.iMatrix[2][2] = (TReal32)1.6403;

    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;

    aWBParams.iColorMatrix = matrix;
    aWBParams.iIsAutoMode = 1;

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 1************ \n");   
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.000;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.256835938;
    aWBParams.iConstrainerBlueA = 0.479980469;
    aWBParams.iConstrainerRedB = 0.418945313;
    aWBParams.iConstrainerBlueB = 0.291015625;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_AUTO_640_480Wed_May_20_15_15_20_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    aWBParams.iIsAutoMode = 0;

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 1************ \n");  
    aImagesGain.iGainR_Enable = 1.533;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.047;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.413085938;
    aWBParams.iConstrainerBlueA = 0.297851563;
    aWBParams.iConstrainerRedB = 0.413085938;
    aWBParams.iConstrainerBlueB = 0.297851563;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_DAY_640_480Wed_May_20_15_15_24_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST CLOUDY 1************ \n");
    aImagesGain.iGainR_Enable = 1.461;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.117;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.394000000;
    aWBParams.iConstrainerBlueA = 0.319000000;
    aWBParams.iConstrainerRedB = 0.394000000;
    aWBParams.iConstrainerBlueB = 0.319000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_CLOUDY_640_480Wed_May_20_15_15_28_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST SHADE 1************ \n");  
    aImagesGain.iGainR_Enable = 1.891;
    aImagesGain.iGainG_Enable = 1.100;
    aImagesGain.iGainB_Enable = 1.000;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.459000000;
    aWBParams.iConstrainerBlueA = 0.244000000;
    aWBParams.iConstrainerRedB = 0.459000000;
    aWBParams.iConstrainerBlueB = 0.244000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_SHADE_640_480Wed_May_20_15_15_32_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 1************ \n");
    aImagesGain.iGainR_Enable = 1.094;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.729;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.273000000;
    aWBParams.iConstrainerBlueA = 0.461000000;
    aWBParams.iConstrainerRedB = 0.273000000;
    aWBParams.iConstrainerBlueB = 0.461000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_TUNGSTEN_640_480Wed_May_20_15_15_37_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 1************ \n");
    aImagesGain.iGainR_Enable = 1.293;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.525;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.325000000;
    aWBParams.iConstrainerBlueA = 0.408000000;
    aWBParams.iConstrainerRedB = 0.325000000;
    aWBParams.iConstrainerBlueB = 0.408000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_FLUO_640_480Wed_May_20_15_15_41_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST INCANDESCENT 1************ \n");
    aImagesGain.iGainR_Enable = 1.031;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.801;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.256835938;
    aWBParams.iConstrainerBlueA = 0.479980469;
    aWBParams.iConstrainerRedB = 0.256835938;
    aWBParams.iConstrainerBlueB = 0.479980469;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_INCAND_640_480Wed_May_20_15_15_45_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 1************ \n");
    aImagesGain.iGainR_Enable = 1.432;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.211;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.379000000;
    aWBParams.iConstrainerBlueA = 0.340000000;
    aWBParams.iConstrainerRedB = 0.379000000;
    aWBParams.iConstrainerBlueB = 0.340000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_FLASH_640_480Wed_May_20_15_15_50_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 1************ \n");
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.092;
    aImagesGain.iGainB_Enable = 2.145;

    aWBParams.iImageGain = aImagesGain;
    aWBParams.iConstrainerRedA = 0.224000000;
    aWBParams.iConstrainerBlueA = 0.517000000;
    aWBParams.iConstrainerRedB = 0.224000000;
    aWBParams.iConstrainerBlueB = 0.517000000;
    aWBParams.iDistanceFromLocus = 0.018;
    
    aMetricParams.iParams = &aWBParams;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_OFF_640_480Wed_May_20_15_15_15_2009_.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\frederique\\withGainConstrain\\frame_rgb16_WB_HORIZON_640_480Wed_May_20_15_15_54_2009_.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    return 0;

#else
    TAlgoError ret = EErrorNone; 
    TAlgoImagesGain aImagesGain;
	TAlgoMetricParams aMetricParams;
    
    TUint8 dir[128];
    TUint8 dir1[128];    

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    aMetricParams.iType = ETypeWhiteBalance;

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.090;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_FLUO_R1.190_G1.000_B1.090.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.100;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_FLUO_R1.199_G1.000_B1.100.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams ); 
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams ); 
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLUORESCENT 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_FLUO_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_DAY_R1.850_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_DAY_R1.849_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.850;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_DAY_R1.850_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_DAY_R1.849_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_DAY_R1.849_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.0083;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_DAY_R1.849_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST DAYLIGHT 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.849;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_DAY_R1.849_G1.199_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.190;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_HORI_R1.000_G1.190_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_HORI_R1.000_G1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.801;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_HORI_R1.000_G1.199_B1.801.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_HORI_R1.000_G1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_HORI_R1.000_G_1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_HORI_R1.000_G1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_HORI_R1.000_G1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST HORIZON 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.199;
    aImagesGain.iGainB_Enable = 1.800;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_HORI_R1.000_G1.199_B1.800.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.12;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.12;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.301;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_TUNG_R1.000_G1.000_B1.301.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.008;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.004;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.8;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_TUNG_R1.849_G1.119_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST TUNGSTEN 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.08;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.300;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_TUNG_R1.000_G1.000_B1.300.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 10;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.667;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.833;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_AUTO_R1.667_G1.000_B1.833.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 6;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.617;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.693;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_AUTO_R1.617_G1.000_B1.693.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.336;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.320;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_AUTO_R1.336_G1.000_B1.320.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.021;
    aImagesGain.iGainG_Enable = 1.021;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_AUTO_R1.021_G1.021_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.027;
    aImagesGain.iGainB_Enable = 1.156;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_AUTO_R1.000_G1.027_B1.156.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.02;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.058;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_AUTO_R1.000_G1.000_B1.058.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.039;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.013;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_AUTO_R1.000_G1.000_B1.013.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST AUTO 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 7;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.628;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.714;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_AUTO_R1.628_G1.000_B1.714.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.004;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.007;
    aImagesGain.iGainB_Enable = 1.007;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_MAN_R1.000_G1.007_B1.007.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.921;
    aImagesGain.iGainG_Enable = 1.414;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_MAN_R1.921_G1.414_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 3************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0.004;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.000;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM3_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM3_MAN_R1.000_G1.000_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 2.000;
    aImagesGain.iGainG_Enable = 1.734;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_MAN_R2.000_G1.734_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 2.000;
    aImagesGain.iGainG_Enable = 1.617;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_MAN_R2.000_G1.617_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.937;
    aImagesGain.iGainG_Enable = 1.601;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_MAN_R1.937_G1.601_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.875;
    aImagesGain.iGainG_Enable = 1.460;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_MAN_R1.875_G1.460_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST MANUAL 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.882;
    aImagesGain.iGainG_Enable = 1.625;
    aImagesGain.iGainB_Enable = 1.000;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_MAN_R1.882_G1.625_B1.000.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 1************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.190;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM1_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM1_FLASH_R1.190_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 2************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM2_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM2_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 4************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM4_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM4_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 5************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM5_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM5_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 6************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM6_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM6_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 7************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM7_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM7_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret);  

    ALGO_Log_0("*************WHITE BALANCE METRIC TEST FLASH 8************ \n");
    aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aImagesGain.iGainR_Disable = 1.0;
    aImagesGain.iGainG_Disable = 1.0;
    aImagesGain.iGainB_Disable = 1.0;
    aImagesGain.iGainR_Enable = 1.199;
    aImagesGain.iGainG_Enable = 1.000;
    aImagesGain.iGainB_Enable = 1.099;
    aMetricParams.iParams = &aImagesGain;
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\WhiteBalance\\WB_IM8_OFF_R1.000_G1.000_B1.000.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\WhiteBalance\\WB_IM8_FLASH_R1.199_G1.000_B1.099.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );   
    Metrcis_Print_Result(ret); 

    return 0;

#endif
    }
