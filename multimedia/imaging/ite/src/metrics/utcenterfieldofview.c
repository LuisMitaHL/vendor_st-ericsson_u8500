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
 * centerfieldofview Unit Test Cases.
 */
TInt32 test_metric_centerfieldofview_characterization(TUint8* aDirPath)
    {

    TAlgoError ret = EErrorNone; 
	TAlgoMetricParams aMetricParams;
	TInt8 *pString = NULL;
	TInt8 *pHandle = NULL;
	TInt32 nX = 0, nY = 0, nX1 = 0, nY1 = 0;
    
    TUint8 dir[128];
    TUint8 dir1[128];    
	TAlgoParamCenterFieldOfView paramCenterFieldOfView;
	char cfow_patern[]="_cfow_";
    
    ALGO_Log_0("*************CENTERFIELDOFVIEW METRIC TEST 1************ \n");
	pString = dir;
    AlgoStrcpy(pString, aDirPath);
    AlgoStrcat(pString, "cfow//frame_1__reso_320_x_240_id_001_format_bmp_cfow_0_0.bmp");
	pHandle = strstr(pString, cfow_patern);
	if(pHandle)
	{
		pHandle += strlen(cfow_patern);
		sscanf (pHandle,"%d_%d", &nX, &nY);
		ALGO_Log_2 ("detected nX %d , nY %d\n", nX, nY);
	}
	else
	{
		ALGO_Log_0("cfow not recognazied\n");
	}
	pString = dir1;
    AlgoStrcpy(pString, aDirPath);
    AlgoStrcat(pString, "cfow//frame_1__reso_320_x_240_id_002_format_bmp_cfow_16_32.bmp");
	pHandle = strstr(pString, cfow_patern);
	if(pHandle)
	{
		pHandle += strlen(cfow_patern);
		sscanf (pHandle,"%d_%d", &nX1, &nY1);
		ALGO_Log_2 ("detected nX %d , nY %d\n", nX1, nY1);
	}
	else
	{
		ALGO_Log_0("cfow not recognazied\n");
	}
    aMetricParams.iType = ETypeCenterFieldOfView;
 	aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iTestCoordinates.iStartIndexX = 0; // not used
    aMetricParams.iTestCoordinates.iStartIndexY = 0; // not used
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 0; // size max of common widow is computed by metric
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 0; // size max of common widow is computed by metric
	
	paramCenterFieldOfView.iX		= nX1 - nX;
	paramCenterFieldOfView.iY		= nY1 - nY;
	paramCenterFieldOfView.iTolerance		= 5;
	aMetricParams.iParams = &paramCenterFieldOfView;   
	
	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 



    ALGO_Log_0("*************CENTERFIELDOFVIEW METRIC TEST 2************ \n");
	pString = dir;
    AlgoStrcpy(pString, aDirPath);
    AlgoStrcat(pString, "cfow//frame_1__reso_320_x_240_id_001_format_bmp_cfow_0_0.bmp");
	pHandle = strstr(pString, cfow_patern);
	if(pHandle)
	{
		pHandle += strlen(cfow_patern);
		sscanf (pHandle,"%d_%d", &nX, &nY);
		ALGO_Log_2 ("detected nX %d , nY %d\n", nX, nY);
	}
	else
	{
		ALGO_Log_0("cfow not recognazied\n");
	}
	pString = dir1;
    AlgoStrcpy(pString, aDirPath);
    AlgoStrcat(pString, "cfow//frame_1__reso_320_x_240_id_002_format_bmp_cfow_16_-32.bmp");
	pHandle = strstr(pString, cfow_patern);
	if(pHandle)
	{
		pHandle += strlen(cfow_patern);
		sscanf (pHandle,"%d_%d", &nX1, &nY1);
		ALGO_Log_2 ("detected nX %d , nY %d\n", nX1, nY1);
	}
	else
	{
		ALGO_Log_0("cfow not recognazied\n");
	}
    aMetricParams.iType = ETypeCenterFieldOfView;
 	aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 0;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 0;
	aMetricParams.iMetricRange.iRangeR = 0;
	aMetricParams.iMetricRange.iRangeG = 0;
	aMetricParams.iMetricRange.iRangeB = 0;
    aMetricParams.iTestCoordinates.iStartIndexX = 0; // not used
    aMetricParams.iTestCoordinates.iStartIndexY = 0; // not used
    aMetricParams.iTestCoordinates.iPixelsToGrabX = 0; // size max of common widow is computed by metric
    aMetricParams.iTestCoordinates.iPixelsToGrabY = 0; // size max of common widow is computed by metric
	
	paramCenterFieldOfView.iX		= nX1 - nX;
	paramCenterFieldOfView.iY		= nY1 - nY;
	paramCenterFieldOfView.iTolerance		= 5;
	aMetricParams.iParams = &paramCenterFieldOfView;   
	
	ret = ValidationMetricInterfaceWithFiles(NULL, NULL, dir, dir1, &aMetricParams );
    Metrcis_Print_Result(ret); 
	return 0;
    }
