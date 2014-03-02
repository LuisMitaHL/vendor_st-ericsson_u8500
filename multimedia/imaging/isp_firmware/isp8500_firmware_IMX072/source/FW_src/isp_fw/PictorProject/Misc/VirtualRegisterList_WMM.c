/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      VirtualRegisterList_WMM.c

 \brief     This file contains all the page definitions with their properties.
            The page table is generated automatically using a script which parses
            the files and creates the corresponding page table entries.

 \ingroup   Miscellaneous
*/
#include "VirtualRegisterList_WMM.h"

// PageElementInfo Starts Here.
// This comment is for PageElementInfo[].
// Please specify the PageName, Mode Static Property ,  ReadWrite Property is the mentioned format.
// CreatePageElementInfo.py creates the PageElementInfo[] and initializes it accordingly.
//
// The entries should be added between the Boundary Lines
//
//  PageName    NON_MODE_STATIC_PAGE or MODE_STATIC_PAGE    READ_ONLY_PAGE or READ_WRITE_PAGE
//-------------------------------------------------------------------------------------------------
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Zoom_Status_LLA                                             NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_RunMode_Control                                             NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_RunMode_Status                                              NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_ReadLLAConfig_Control                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_ReadLLAConfig_Status                                        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_FrameParamStatus                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_ApertureConfig_Control                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_ApertureConfig_Status                                       NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_FlashControl                                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_FlashStatus                                                 NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Sensor_Tuning_Control                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Sensor_Tuning_Status                                        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Pipe_RAW                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Zoom_Control1                                               NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_GrabNotify                                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_FrameParamStatus_Extn                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_SensorPowerManagement_Control                               NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_SensorPowerManagement_Status                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_GridironStatus                                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_SMS_Control                                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_SMS_Status                                                  NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_TraceLogsControl                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Pipe_Scalar_UserParams[0]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Pipe_Scalar_UserParams[1]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_FrameParamStatus_Af                                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_ErrorHandler                                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_CustomStockMatrix[0]                                        NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CustomStockMatrix[1]                                        NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_SensorPipeSettings_Control                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_SensorPipeSettings_Status                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_VariableFrameRateControl                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_VariableFrameRateStatus                                     MODE_STATIC_PAGE        READ_ONLY_PAGE
//
//  g_Mozart_Ctrl                                                 MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Norcos_Ctrl[0]                                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Norcos_Ctrl[1]                                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HDR_Control                                                 MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_HDR_GainControl                                             MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_HDR_Status                                                  MODE_STATIC_PAGE        READ_ONLY_PAGE

//  g_CE_GammaStatus[0]                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_GammaStatus[1]                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE

//  g_CE_GammaMemLutAddress[0]                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_GammaMemLutAddress[1]                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE

//
//  g_BML_Framedimension                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//
//-------------------------------------------------------------------------------------------------
// The Array will be placed after the below Boundary Line.
// Donot place any piece of code after this line. Any line after this boundary will be changed to white space.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


const PageElementInfo_ts g_PageElementInfo_WMM[ ] = {
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Zoom_Status_LLA,	0,	16,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_RunMode_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_RunMode_Status,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_ReadLLAConfig_Control,	4,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ReadLLAConfig_Status,	4,	14,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_FrameParamStatus,	64,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ApertureConfig_Control,	0,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ApertureConfig_Status,	0,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_FlashControl,	8,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FlashStatus,	12,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Sensor_Tuning_Control,	8,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Sensor_Tuning_Status,	0,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe_RAW,	0,	8,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Zoom_Control1,	4,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_GrabNotify,	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FrameParamStatus_Extn,	24,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SensorPowerManagement_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SensorPowerManagement_Status,	4,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_GridironStatus,	12,	14,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SMS_Control,	4,	10,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SMS_Status,	24,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_TraceLogsControl,	12,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_UserParams[0],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_UserParams[1],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FrameParamStatus_Af,	20,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ErrorHandler,	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CustomStockMatrix[0],	36,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CustomStockMatrix[1],	36,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SensorPipeSettings_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SensorPipeSettings_Status,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_VariableFrameRateControl,	8,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_VariableFrameRateStatus,	8,	0,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Mozart_Ctrl,	0,	6,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Norcos_Ctrl[0],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Norcos_Ctrl[1],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HDR_Control,	32,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HDR_GainControl,	48,	8,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HDR_Status,	32,	8,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_GammaStatus[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_GammaStatus[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_GammaMemLutAddress[0],	24,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_GammaMemLutAddress[1],	24,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_BML_Framedimension,	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},

};
