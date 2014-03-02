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
 * Auto Focus Unit Test Cases.
 */
TInt32 test_metric_autofocus(char *aFileNameImageEffectDisable, char *aFileNameImageEffectEnable)
   {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
	
    ALGO_Log_0("************* AUTO FOCUS METRIC TEST ************ \n");

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeAutoFocus;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, aFileNameImageEffectDisable, aFileNameImageEffectEnable, &aMetricParams);
	Metrcis_Print_Result(ret);

	return 0;
    }

TInt32 test_metric_autofocus_line(char *aFileNameImageTop, char *aFileNameImageMiddle, char *aFileNameImageBottom)
   {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
 	TAlgoImageParams aImageParamsDefault;
    TAlgoImageParams imageParams;

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST LINE ************ \n");

	//GetWithHeightFormatHeadersizeFromHeader((const TUint8*) aFileNameImageTop, &Width, &Height, &ImageFormat, &HeaderSize);
    ret = ReadFileHeader( aFileNameImageTop, &imageParams, &aImageParamsDefault );
	if(ret != EErrorNone)
	{
		Metrcis_Print_Result(ret);
		return 1;
	}

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeAutoFocus;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight/3;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageMiddle, (const TUint8 *)aFileNameImageTop, &aMetricParams );
	Metrcis_Print_Result(ret);

    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = imageParams.iImageHeight/3;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight/3;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageBottom, (const TUint8 *)aFileNameImageMiddle, &aMetricParams );
	Metrcis_Print_Result(ret);

    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 2*imageParams.iImageHeight/3;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight/3;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageMiddle, (const TUint8 *)aFileNameImageBottom, &aMetricParams );
	Metrcis_Print_Result(ret);

	return 0;
    }

TInt32 test_metric_autofocus_colon(char *aFileNameImageLeft, char *aFileNameImageMiddle, char *aFileNameImageRight)
   {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
 	TAlgoImageParams aImageParamsDefault;
    TAlgoImageParams imageParams;

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST COLON ************ \n");

	//GetWithHeightFormatHeadersizeFromHeader((const TUint8*) aFileNameImageLeft, &Width, &Height, &ImageFormat, &HeaderSize);
    ReadFileHeader( aFileNameImageLeft, &imageParams, &aImageParamsDefault );
	if(ret != EErrorNone)
	{
		Metrcis_Print_Result(ret);
		return 1;
	}

	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeAutoFocus;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth/3;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageMiddle, (const TUint8 *)aFileNameImageLeft, &aMetricParams );
	Metrcis_Print_Result(ret);

    aMetricParams.iTestCoordinates.iStartIndexX = imageParams.iImageWidth/3;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth/3;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageRight, (const TUint8 *)aFileNameImageMiddle, &aMetricParams );
	Metrcis_Print_Result(ret);

    aMetricParams.iTestCoordinates.iStartIndexX = 2*imageParams.iImageWidth/3;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = imageParams.iImageWidth/3;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = imageParams.iImageHeight;
	aMetricParams.iParams = (TAny*)NULL;

	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, (const TUint8 *)aFileNameImageMiddle, (const TUint8 *)aFileNameImageRight, &aMetricParams );
	Metrcis_Print_Result(ret);

	return 0;
    }

/**
 * Auto Focus Unit Test Cases with Spatial Check.
 */
TInt32 test_metric_autofocus_with_spatial_test(TUint8* aDirPath)
    {
    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
    
    TUint8 dir[128];
    TUint8 dir1[128];    


	aMetricParams.iErrorTolerance.iErrorToleranceR = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
    aMetricParams.iType = ETypeAutoFocus;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    
    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 1************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM1_Disable.bmp");
    //AlgoStrcat(dir, "\\AutoFocus\\AF_IM1_Disable.gam");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM1_Enable.bmp");
    //AlgoStrcat(dir1, "\\AutoFocus\\AF_IM1_Enable.gam");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 2************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM2_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM2_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 3************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM3_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM3_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 4************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM4_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM4_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 5************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM5_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM5_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 6************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM6_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM6_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 7************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM7_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM7_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 8************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM8_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM8_Enable.bmp");
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 9************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM9_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM9_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 400;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 278;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 10************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM10_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM10_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 2592;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 1944;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 11************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM11_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM11_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 12 - Both images are same************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM11_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM11_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 13 - Both images are focussed************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM11_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM11_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }

    ALGO_Log_0("*************AUTO FOCUS METRIC TEST 13 - Both images are unfocussed************ \n");
    AlgoStrcpy(dir, aDirPath);
    AlgoStrcat(dir, "\\AutoFocus\\AF_IM11_Disable.bmp");
    AlgoStrcpy(dir1, aDirPath);
    AlgoStrcat(dir1, "\\AutoFocus\\AF_IM11_Enable.bmp");
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 640;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 480;
    ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    if(ret == EErrorImageNotRelevant)
        {
        ALGO_Log_0("===================TEST IMAGE NOT RELEVANT================= \n\n"); 
        }
    else
        {
        Metrcis_Print_Result(ret); 
        }
	return 0;
    }
