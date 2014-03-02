/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      VirtualRegisterList.c

 \brief     This file contains all the page definitions with their properties.
            The page table is generated automatically using a script which parses
            the files and creates the corresponding page table entries.

 \ingroup   Miscellaneous
*/
#include "VirtualRegisterList.h"

Reserved_ts           g_Reserved = {0};

// PageElementInfo Starts Here.
// This comment is for PageElementInfo[].
// Please specify the PageName, Mode Static Property ,  ReadWrite Property is the mentioned format.
// CreatePageElementInfo.py creates the PageElementInfo[] and initializes it accordingly.
//
// The entries should be added between the Boundary Lines
//
//  PageName    NON_MODE_STATIC_PAGE or MODE_STATIC_PAGE    READ_ONLY_PAGE or READ_WRITE_PAGE
//-------------------------------------------------------------------------------------------------
//  g_DeviceParameters                                            NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_SensorInformation                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CRM_Status                                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_HostComms_Status                                            NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_HostInterface_Control                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HostInterface_Status                                        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Stream_InputControl                                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Stream_InputStatus                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_SystemSetup                                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_SystemConfig_Status                                         NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_ColdStartControl                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_ColdStartStatus                                             NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_DataPathControl                                             NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_DataPathStatus                                              NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Pipe[0]                                                     MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Pipe[1]                                                     MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_PipeStatus[0]                                               MODE_STATIC_PAGE        READ_ONLY_PAGE
//  g_PipeStatus[1]                                               MODE_STATIC_PAGE        READ_ONLY_PAGE
//
//  g_VideoTimingHostInputs                                       MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_VideoTimingOutput                                           MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_SensorFrameConstraints                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HostFrameConstraints                                        MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_FrameDimensionStatus                                        NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CurrentFrameDimension                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_RequestedFrameDimension                                     NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AntiFlicker_Status                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_FrameRateControl                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_FrameRateStatus                                             NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Zoom_Params                                                 MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Zoom_Control                                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Zoom_CommandControl                                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_ZoomTop_ParamAppicationControl                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Zoom_Status                                                 NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Zoom_CommandStatus                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_PipeState[0]                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_PipeState[1]                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Pipe_Scalar_Inputs[0]                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Pipe_Scalar_Inputs[1]                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Pipe_Scalar_Output[0]                                       NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Pipe_Scalar_Output[1]                                       NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_MasterI2CClockControl                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_MasterI2CStatus                                             NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_HostToMasterI2CAccessControl                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HostToMasterI2CAccessData                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HostToMasterI2CAccessStatus                                 NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_CE_ColourMatrixFloat[0]                                     NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_ColourMatrixDamped[0]                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_ColourMatrixFloat[1]                                     NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_ColourMatrixDamped[1]                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_YUVCoderControls[0]                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_CustomTransformOutputSignalRange[0]                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_FadeToBlack[0]                                           NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_OutputCoderMatrix[0]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_OutputCoderOffsetVector[0]                               NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_YUVCoderStatus[0]                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_YUVCoderControls[1]                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_CustomTransformOutputSignalRange[1]                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_FadeToBlack[1]                                           NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_OutputCoderMatrix[1]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_CE_OutputCoderOffsetVector[1]                               NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_YUVCoderStatus[1]                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_WhiteBalanceControl                                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_WhiteBalanceStatus                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_ChannelGains_Control                                        NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_ChannelGains_combined                                       NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Glace_Control                                               NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Glace_Status                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_GPIOControl                                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_gpio_control                                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_gpio_debug                                                  NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_HistStats_Ctrl                                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HistStats_Status                                            NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_ExpSensor_SensorProperties                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Exposure_CompilerStatus                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Exposure_ParametersApplied                                  NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Exposure_ErrorStatus                                        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Exposure_ErrorControl                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Exposure_DriverControls                                     NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_OffsetCompensationStatus                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_SensorSetup                                                 NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_RSO_Control                                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_RSO_DataCtrl                                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Interrupts_Count                                            NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Profile_BOOT                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_SensorCommit                                        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_LCO                                                 NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_ISPUpdate                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_AEC_Statistics                                      NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_AWB_Statistics                                      NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_VID0                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Profile_VID1                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Pipe_Scalar_StripeInputs[0]                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Pipe_Scalar_StripeOutputs[0]                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Pipe_Scalar_StripeInputs[1]                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Pipe_Scalar_StripeOutputs[1]                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_CSIControl                                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_CE_GammaControl[0]                                          NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_GammaControl[1]                                          NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_CE_ColourMatrixCtrl[0]                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_ColourMatrixCtrl[1]                                      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_FLADriver_NVMStoredData                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_FLADriver_Controls                                          NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_FLADriver_LLLCtrlStatusParam                                NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_FLADriver_Status                                            NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_FocusControl_Controls                                       NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_FocusControl_Status                                         NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_AFStats_Controls                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_Status                                              NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_AFZoneInt                                           NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_AFStats_Debug                                               NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[0]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[1]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[2]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[3]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[4]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[5]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[6]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[7]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[8]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfigPercentage[9]                         NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_AFStats_HostZoneConfig[0]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[1]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[2]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[3]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[4]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[5]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[6]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[7]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[8]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_AFStats_HostZoneConfig[9]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_AFStats_HostZoneStatus[0]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[1]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[2]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[3]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[4]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[5]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[6]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[7]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[8]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_HostZoneStatus[9]                                   NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_AFStats_ZoneHWStatus[0]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[1]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[2]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[3]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[4]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[5]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[6]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[7]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[8]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_AFStats_ZoneHWStatus[9]                                     NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Reserved                                                    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_TestPattern_Ctrl                                            MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_TestPattern_Status                                          MODE_STATIC_PAGE        READ_ONLY_PAGE
//
//  g_RSO_DataStatus                                              NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Babylon_Ctrl                                                MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Scorpio_Ctrl                                                MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_BinningRepair_Ctrl                                          MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_DusterControl                                               MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_Reserved                                                    MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_GridironControl                                             MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_SDL_Control                                                 MODE_STATIC_PAGE        READ_WRITE_PAGE
//  g_SDL_Status                                                  NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_Adsoc_PK_Ctrl[0]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Adsoc_PK_Ctrl[1]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Adsoc_RP_Ctrl[0]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Adsoc_RP_Ctrl[1]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Adsoc_RP_Status[0]                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Adsoc_RP_Status[1]                                          NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_DusterStatus                                                MODE_STATIC_PAGE        READ_ONLY_PAGE
//
//  g_DMASetup                                                    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_DMAControl                                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Scalar_StripeInternalParams[0]                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_Scalar_StripeInternalParams[1]                              NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_SpecialEffects_Control[0]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_SpecialEffects_Control[1]                                   NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_Event0_Count                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_CE_LumaOffset[0]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_LumaOffset[1]                                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_GammaLastPixelValueControl[0]                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_CE_GammaLastPixelValueControl[1]                            NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//  g_FLADriver_LensLLDParam                                      NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_SDL_ELT                                                     NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_HostComms_PEDump                                            MODE_STATIC_PAGE        READ_WRITE_PAGE
//
//  g_Event1_Count                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Event2_Count                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//  g_Event3_Count                                                NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
//
//  g_testpattern_SolidColor_data                                 NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//  g_testpattern_Cursors_values                                  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
//
//-------------------------------------------------------------------------------------------------
// The Array will be placed after the below Boundary Line.
// Donot place any piece of code after this line. Any line after this boundary will be changed to white space.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


const PageElementInfo_ts g_PageElementInfo[ ] = {
		{(void *) &g_DeviceParameters,	36,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_SensorInformation,	0,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CRM_Status,	0,	14,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostComms_Status,	4,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_HostInterface_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostInterface_Status,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Stream_InputControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Stream_InputStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_SystemSetup,	16,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SystemConfig_Status,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_ColdStartControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ColdStartStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_DataPathControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_DataPathStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe[0],	0,	4,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe[1],	0,	4,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_PipeStatus[0],	0,	0,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_PipeStatus[1],	0,	0,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_VideoTimingHostInputs,	8,	2,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_VideoTimingOutput,	36,	12,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SensorFrameConstraints,	0,	34,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostFrameConstraints,	4,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FrameDimensionStatus,	12,	10,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CurrentFrameDimension,	4,	34,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_RequestedFrameDimension,	4,	34,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AntiFlicker_Status,	8,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_FrameRateControl,	24,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FrameRateStatus,	8,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Zoom_Params,	4,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Zoom_Control,	8,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Zoom_CommandControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ZoomTop_ParamAppicationControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Zoom_Status,	32,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Zoom_CommandStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_PipeState[0],	16,	4,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_PipeState[1],	16,	4,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe_Scalar_Inputs[0],	12,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_Inputs[1],	12,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_Output[0],	20,	16,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe_Scalar_Output[1],	20,	16,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_MasterI2CClockControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_MasterI2CStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_HostToMasterI2CAccessControl,	0,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostToMasterI2CAccessData,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostToMasterI2CAccessStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_ColourMatrixFloat[0],	36,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_ColourMatrixDamped[0],	0,	24,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_ColourMatrixFloat[1],	36,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_ColourMatrixDamped[1],	0,	24,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_YUVCoderControls[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_CustomTransformOutputSignalRange[0],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_FadeToBlack[0],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_OutputCoderMatrix[0],	0,	18,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_OutputCoderOffsetVector[0],	0,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_YUVCoderStatus[0],	8,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_YUVCoderControls[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_CustomTransformOutputSignalRange[1],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_FadeToBlack[1],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_OutputCoderMatrix[1],	0,	18,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_OutputCoderOffsetVector[1],	0,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_YUVCoderStatus[1],	8,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_WhiteBalanceControl,	12,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_WhiteBalanceStatus,	12,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_ChannelGains_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_ChannelGains_combined,	16,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Glace_Control,	20,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Glace_Status,	4,	4,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_GPIOControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_gpio_control,	28,	6,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_gpio_debug,	12,	12,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_HistStats_Ctrl,	28,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HistStats_Status,	0,	26,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_ExpSensor_SensorProperties,	0,	22,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Exposure_CompilerStatus,	16,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Exposure_ParametersApplied,	8,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Exposure_ErrorStatus,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Exposure_ErrorControl,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Exposure_DriverControls,	8,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_OffsetCompensationStatus,	16,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_SensorSetup,	12,	6,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_RSO_Control,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_RSO_DataCtrl,	32,	12,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Interrupts_Count,	0,	50,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_BOOT,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_SensorCommit,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_LCO,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_ISPUpdate,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_AEC_Statistics,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_AWB_Statistics,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_VID0,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Profile_VID1,	20,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe_Scalar_StripeInputs[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_StripeOutputs[0],	0,	26,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Pipe_Scalar_StripeInputs[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Pipe_Scalar_StripeOutputs[1],	0,	26,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CSIControl,	0,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_GammaControl[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_GammaControl[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_ColourMatrixCtrl[0],	0,	6,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_ColourMatrixCtrl[1],	0,	6,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FLADriver_NVMStoredData,	24,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_FLADriver_Controls,	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FLADriver_LLLCtrlStatusParam,	0,	30,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FLADriver_Status,	0,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FocusControl_Controls,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FocusControl_Status,	0,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_Controls,	4,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_Status,	4,	10,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_AFZoneInt,	0,	20,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_Debug,	4,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[0],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[1],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[2],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[3],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[4],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[5],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[6],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[7],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[8],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfigPercentage[9],	16,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[0],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[1],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[2],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[3],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[4],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[5],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[6],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[7],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[8],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneConfig[9],	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[0],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[1],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[2],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[3],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[4],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[5],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[6],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[7],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[8],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_HostZoneStatus[9],	40,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[0],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[1],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[2],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[3],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[4],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[5],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[6],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[7],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[8],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_AFStats_ZoneHWStatus[9],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Reserved,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_TestPattern_Ctrl,	0,	8,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_TestPattern_Status,	0,	8,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_RSO_DataStatus,	32,	12,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Babylon_Ctrl,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Scorpio_Ctrl,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_BinningRepair_Ctrl,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_DusterControl,	0,	4,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Reserved,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_GridironControl,	20,	4,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SDL_Control,	0,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SDL_Status,	0,	0,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Adsoc_PK_Ctrl[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Adsoc_PK_Ctrl[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Adsoc_RP_Ctrl[0],	0,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Adsoc_RP_Ctrl[1],	0,	4,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Adsoc_RP_Status[0],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Adsoc_RP_Status[1],	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_DusterStatus,	0,	14,	MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_DMASetup,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_DMAControl,	0,	12,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Scalar_StripeInternalParams[0],	40,	16,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Scalar_StripeInternalParams[1],	40,	16,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SpecialEffects_Control[0],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_SpecialEffects_Control[1],	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Event0_Count,	0,	64,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_CE_LumaOffset[0],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_LumaOffset[1],	0,	2,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_GammaLastPixelValueControl[0],	0,	16,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_CE_GammaLastPixelValueControl[1],	0,	16,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_FLADriver_LensLLDParam,	16,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_SDL_ELT,	0,	10,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_HostComms_PEDump,	4,	0,	MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_Event1_Count,	0,	10,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Event2_Count,	0,	2,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_Event3_Count,	0,	8,	NON_MODE_STATIC_PAGE,	READ_ONLY_PAGE	},
		{(void *) &g_testpattern_SolidColor_data,	0,	8,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},
		{(void *) &g_testpattern_Cursors_values,	0,	0,	NON_MODE_STATIC_PAGE,	READ_WRITE_PAGE	},

};
