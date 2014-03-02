/*****************************************************************************/
/*
* Copyright (C) ST-Ericsson SA 2011. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef _EXTENSION_H_
#define _EXTENSION_H_
//
//              file: extension.h
//  FW_3.0.3_SENSOR_3_LLA_2.0_LLCD_4.0 (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MAJOR : 3 Incremented With Compatibility break (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MINOR : 0 Incremented With Feature addition w/o any break in compatibility (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MICRO : 3 Incremented WithBug fixes (parsed from source files)
//  version info ->         LOW_LEVEL_API_SENSOR : 3 customer identification (parsed from source files)
//  version info ->  LOW_LEVEL_API_MAJOR_VERSION : 2 Incremented when there is compatibility break in API and ISP FW update is necessary (parsed from source files)
//  version info ->  LOW_LEVEL_API_MINOR_VERSION : 0 Incremented when there is change in API or feature addition that is backward compatible (parsed from source files)
//  version info ->LOW_LEVEL_CAMERA_DRIVER_MAJOR : 4 Incremented with every release (parsed from source files)
//  version info ->LOW_LEVEL_CAMERA_DRIVER_MINOR : 0 Incremented each time there is bug fix on old API (parsed from source files)
// copies of all hash defines used in page element type code definitions...
//
// copies of all enum types used in page definitions...
/*****************************************************************************/
/*
*  © ST Microelectronics Pvt. Ltd, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Microelectronics
*
*/
/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* The features are joint developement between ST-Ericsson SA and ST Microelectronics Pvt. Ltd. 
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

typedef enum
{
	SensorModeCapture_e_STILL = 0,
	SensorModeCapture_e_STILL_NIGHT = 2,
	SensorModeCapture_e_STILL_SEQ = 3,
	SensorModeCapture_e_FLASH = 4,
} SensorModeCapture_te;

typedef enum
{
	Usecase_e_STILL = 0,
	Usecase_e_VIDEO = 1,
} Usecase_te;

typedef enum
{
	FlashError_e_FLASH_ERROR_NONE = 0,
	FlashError_e_FLASH_ERROR_CONFIGURATION = 1,
	FlashError_e_FLASH_ERROR_STROBE_DID_NOT_TRIGGER = 2,
	FlashError_e_FLASH_ERROR_OUT_OF_FLASH_API_CONTEXT = 3,
} FlashError_te;

typedef enum
{
	Orientation_e_NORMAL = 0,
	Orientation_e_ROTATED = 1,
} Orientation_te;

typedef enum
{
	PowerCommand_e_voltage_on = 0,
	PowerCommand_e_voltage_off = 1,
	PowerCommand_e_ext_clk_on = 2,
	PowerCommand_e_ext_clk_off = 3,
	PowerCommand_e_x_shutdown_on = 4,
	PowerCommand_e_x_shutdown_off = 5,
} PowerCommand_te;

typedef enum
{
	SensorModeVF_e_STILL = 0,
	SensorModeVF_e_VIDEO = 1,
	SensorModeVF_e_STILL_NIGHT = 2,
	SensorModeVF_e_STILL_SEQ = 3,
	SensorModeVF_e_AF = 5,
} SensorModeVF_te;

typedef enum
{
	StreamMode_e_VF = 0,
	StreamMode_e_Capture = 1,
} StreamMode_te;

typedef enum
{
	StrobeStartPoint_e_EXPOSURE_START = 0,
	StrobeStartPoint_e_READOUT_START = 1,
} StrobeStartPoint_te;

typedef enum
{
	DataFormat_e_RAW10 = 0,
	DataFormat_e_RAW8 = 1,
	DataFormat_e_RAW8_DPCM = 2,
	DataFormat_e_RAW10TO6_DPCM = 3,
} DataFormat_te;

typedef enum
{
	PowerAction_e_idle = 0,
	PowerAction_e_requested = 1,
	PowerAction_e_complete = 2,
} PowerAction_te;

typedef struct
{
	isp_uint32_t	* ptru32_SensorParametersTargetAddress;
	isp_uint32_t	u32_ExposureTime_us;
	isp_uint32_t	u32_AnalogGain_x256;
	isp_uint32_t	u32_RedGain_x1000;
	isp_uint32_t	u32_GreenGain_x1000;
	isp_uint32_t	u32_BlueGain_x1000;
	isp_uint32_t	u32_frame_counter;
	isp_uint32_t	u32_frameRate_x100;
	isp_uint32_t	u32_flash_fired;
	isp_uint32_t	u32_NDFilter_Transparency_x100;
	isp_uint32_t	u32_Flag_NDFilter;
	isp_uint32_t	u32_ExposureQuantizationStep_us;
	isp_uint32_t	u32_ActiveData_ReadoutTime_us;
	isp_uint32_t	u32_SensorExposureTimeMin_us;
	isp_uint32_t	u32_SensorExposureTimeMax_us;
	isp_uint32_t	u32_applied_f_number_x100;
} FrameParamStatus_ts;

typedef struct
{
	isp_uint32_t	u32_SensorParametersAnalogGainMin_x256;
	isp_uint32_t	u32_SensorParametersAnalogGainMax_x256;
	isp_uint32_t	u32_SensorParametersAnalogGainStep_x256;
	isp_uint32_t	u32_StatsInvalid;
	isp_uint32_t	u32_SizeOfFrameParamStatus;
} FrameParamStatus_Extn_ts;

typedef struct
{
	isp_uint32_t	u32_woi_res_width;
	isp_uint32_t	u32_woi_res_height;
	isp_uint32_t	u32_output_res_width;
	isp_uint32_t	u32_output_res_height;
	isp_uint32_t	u32_data_format;
	isp_uint32_t	u32_usage_restriction_bitmask;
	isp_uint32_t	u32_max_frame_rate_x100;
} Sensor_Output_Mode_ts;


// *****************************************
//       Element locations
// *****************************************
// *****************************************

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Zoom_Status_LLA'

#define Zoom_Status_LLA_u16_Ex_woi_resX_Byte0		0x8080
#define Zoom_Status_LLA_u16_Ex_woi_resX_Byte1		0x8081
#define Zoom_Status_LLA_u16_Ex_woi_resY_Byte0		0x8082
#define Zoom_Status_LLA_u16_Ex_woi_resY_Byte1		0x8083
#define Zoom_Status_LLA_u16_Ex_output_resX_Byte0		0x8084
#define Zoom_Status_LLA_u16_Ex_output_resX_Byte1		0x8085
#define Zoom_Status_LLA_u16_Ex_output_resY_Byte0		0x8086
#define Zoom_Status_LLA_u16_Ex_output_resY_Byte1		0x8087
#define Zoom_Status_LLA_u16_woi_resX_Byte0		0x8088
#define Zoom_Status_LLA_u16_woi_resX_Byte1		0x8089
#define Zoom_Status_LLA_u16_woi_resY_Byte0		0x808a
#define Zoom_Status_LLA_u16_woi_resY_Byte1		0x808b
#define Zoom_Status_LLA_u16_output_resX_Byte0		0x808c
#define Zoom_Status_LLA_u16_output_resX_Byte1		0x808d
#define Zoom_Status_LLA_u16_output_resY_Byte0		0x808e
#define Zoom_Status_LLA_u16_output_resY_Byte1		0x808f

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0		0x3081
// page 'g_RunMode_Control'

#define RunMode_Control_e_StreamMode_Byte0		0x8140
#define RunMode_Control_e_SensorModeVF_Byte0		0x8141
#define RunMode_Control_e_SensorModeCapture_Byte0		0x8142
#define RunMode_Control_e_Flag_MechanicalShutterUsed_Byte0		0x8143
#define RunMode_Control_e_Coin_Ctrl_Byte0		0x8144
#define RunMode_Control_e_Usecase_Byte0		0x8145
#define RunMode_Control_e_Flag_isTimeNudgeActivated_Byte0		0x8146
#define RunMode_Control_e_Flag_isStillSportMode_Byte0		0x8147
#define RunMode_Control_e_Orientation_Byte0		0x8148

// page 'g_RunMode_Status'

#define RunMode_Status_e_StreamMode_Byte0		0x8180
#define RunMode_Status_e_Coin_Status_Byte0		0x8181

// page 'g_ReadLLAConfig_Control'

#define ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte0		0x81c0
#define ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte1		0x81c1
#define ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte2		0x81c2
#define ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte3		0x81c3
#define ReadLLAConfig_Control_u16_SelectMode_Byte0		0x81c4
#define ReadLLAConfig_Control_u16_SelectMode_Byte1		0x81c5
#define ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0		0x81c6

// page 'g_ReadLLAConfig_Status'

#define ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0		0x8200
#define ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte1		0x8201
#define ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte2		0x8202
#define ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte3		0x8203
#define ReadLLAConfig_Status_u16_number_of_modes_Byte0		0x8204
#define ReadLLAConfig_Status_u16_number_of_modes_Byte1		0x8205
#define ReadLLAConfig_Status_u16_woi_res_width_Byte0		0x8206
#define ReadLLAConfig_Status_u16_woi_res_width_Byte1		0x8207
#define ReadLLAConfig_Status_u16_woi_res_height_Byte0		0x8208
#define ReadLLAConfig_Status_u16_woi_res_height_Byte1		0x8209
#define ReadLLAConfig_Status_u16_output_res_width_Byte0		0x820a
#define ReadLLAConfig_Status_u16_output_res_width_Byte1		0x820b
#define ReadLLAConfig_Status_u16_output_res_height_Byte0		0x820c
#define ReadLLAConfig_Status_u16_output_res_height_Byte1		0x820d
#define ReadLLAConfig_Status_u16_max_frame_rate_x100_Byte0		0x820e
#define ReadLLAConfig_Status_u16_max_frame_rate_x100_Byte1		0x820f
#define ReadLLAConfig_Status_u16_data_format_Byte0		0x8210
#define ReadLLAConfig_Status_u16_data_format_Byte1		0x8211
#define ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0		0x8212

// page 'g_FrameParamStatus'

#define FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte0		0x8240
#define FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte1		0x8241
#define FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte2		0x8242
#define FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte3		0x8243
#define FrameParamStatus_u32_ExposureTime_us_Byte0		0x8244
#define FrameParamStatus_u32_ExposureTime_us_Byte1		0x8245
#define FrameParamStatus_u32_ExposureTime_us_Byte2		0x8246
#define FrameParamStatus_u32_ExposureTime_us_Byte3		0x8247
#define FrameParamStatus_u32_AnalogGain_x256_Byte0		0x8248
#define FrameParamStatus_u32_AnalogGain_x256_Byte1		0x8249
#define FrameParamStatus_u32_AnalogGain_x256_Byte2		0x824a
#define FrameParamStatus_u32_AnalogGain_x256_Byte3		0x824b
#define FrameParamStatus_u32_RedGain_x1000_Byte0		0x824c
#define FrameParamStatus_u32_RedGain_x1000_Byte1		0x824d
#define FrameParamStatus_u32_RedGain_x1000_Byte2		0x824e
#define FrameParamStatus_u32_RedGain_x1000_Byte3		0x824f
#define FrameParamStatus_u32_GreenGain_x1000_Byte0		0x8250
#define FrameParamStatus_u32_GreenGain_x1000_Byte1		0x8251
#define FrameParamStatus_u32_GreenGain_x1000_Byte2		0x8252
#define FrameParamStatus_u32_GreenGain_x1000_Byte3		0x8253
#define FrameParamStatus_u32_BlueGain_x1000_Byte0		0x8254
#define FrameParamStatus_u32_BlueGain_x1000_Byte1		0x8255
#define FrameParamStatus_u32_BlueGain_x1000_Byte2		0x8256
#define FrameParamStatus_u32_BlueGain_x1000_Byte3		0x8257
#define FrameParamStatus_u32_frame_counter_Byte0		0x8258
#define FrameParamStatus_u32_frame_counter_Byte1		0x8259
#define FrameParamStatus_u32_frame_counter_Byte2		0x825a
#define FrameParamStatus_u32_frame_counter_Byte3		0x825b
#define FrameParamStatus_u32_frameRate_x100_Byte0		0x825c
#define FrameParamStatus_u32_frameRate_x100_Byte1		0x825d
#define FrameParamStatus_u32_frameRate_x100_Byte2		0x825e
#define FrameParamStatus_u32_frameRate_x100_Byte3		0x825f
#define FrameParamStatus_u32_flash_fired_Byte0		0x8260
#define FrameParamStatus_u32_flash_fired_Byte1		0x8261
#define FrameParamStatus_u32_flash_fired_Byte2		0x8262
#define FrameParamStatus_u32_flash_fired_Byte3		0x8263
#define FrameParamStatus_u32_NDFilter_Transparency_x100_Byte0		0x8264
#define FrameParamStatus_u32_NDFilter_Transparency_x100_Byte1		0x8265
#define FrameParamStatus_u32_NDFilter_Transparency_x100_Byte2		0x8266
#define FrameParamStatus_u32_NDFilter_Transparency_x100_Byte3		0x8267
#define FrameParamStatus_u32_Flag_NDFilter_Byte0		0x8268
#define FrameParamStatus_u32_Flag_NDFilter_Byte1		0x8269
#define FrameParamStatus_u32_Flag_NDFilter_Byte2		0x826a
#define FrameParamStatus_u32_Flag_NDFilter_Byte3		0x826b
#define FrameParamStatus_u32_ExposureQuantizationStep_us_Byte0		0x826c
#define FrameParamStatus_u32_ExposureQuantizationStep_us_Byte1		0x826d
#define FrameParamStatus_u32_ExposureQuantizationStep_us_Byte2		0x826e
#define FrameParamStatus_u32_ExposureQuantizationStep_us_Byte3		0x826f
#define FrameParamStatus_u32_ActiveData_ReadoutTime_us_Byte0		0x8270
#define FrameParamStatus_u32_ActiveData_ReadoutTime_us_Byte1		0x8271
#define FrameParamStatus_u32_ActiveData_ReadoutTime_us_Byte2		0x8272
#define FrameParamStatus_u32_ActiveData_ReadoutTime_us_Byte3		0x8273
#define FrameParamStatus_u32_SensorExposureTimeMin_us_Byte0		0x8274
#define FrameParamStatus_u32_SensorExposureTimeMin_us_Byte1		0x8275
#define FrameParamStatus_u32_SensorExposureTimeMin_us_Byte2		0x8276
#define FrameParamStatus_u32_SensorExposureTimeMin_us_Byte3		0x8277
#define FrameParamStatus_u32_SensorExposureTimeMax_us_Byte0		0x8278
#define FrameParamStatus_u32_SensorExposureTimeMax_us_Byte1		0x8279
#define FrameParamStatus_u32_SensorExposureTimeMax_us_Byte2		0x827a
#define FrameParamStatus_u32_SensorExposureTimeMax_us_Byte3		0x827b
#define FrameParamStatus_u32_applied_f_number_x100_Byte0		0x827c
#define FrameParamStatus_u32_applied_f_number_x100_Byte1		0x827d
#define FrameParamStatus_u32_applied_f_number_x100_Byte2		0x827e
#define FrameParamStatus_u32_applied_f_number_x100_Byte3		0x827f

// page 'g_ApertureConfig_Control'

#define ApertureConfig_Control_u16_requested_f_number_x_100_Byte0		0x8280
#define ApertureConfig_Control_u16_requested_f_number_x_100_Byte1		0x8281
#define ApertureConfig_Control_u16_SelectAperture_Byte0		0x8282
#define ApertureConfig_Control_u16_SelectAperture_Byte1		0x8283
#define ApertureConfig_Control_e_Coin_Ctrl_Byte0		0x8284

// page 'g_ApertureConfig_Status'

#define ApertureConfig_Status_u16_applied_f_number_x_100_Byte0		0x82c0
#define ApertureConfig_Status_u16_applied_f_number_x_100_Byte1		0x82c1
#define ApertureConfig_Status_u16_number_of_apertures_Byte0		0x82c2
#define ApertureConfig_Status_u16_number_of_apertures_Byte1		0x82c3
#define ApertureConfig_Status_u16_f_number_x_100_Byte0		0x82c4
#define ApertureConfig_Status_u16_f_number_x_100_Byte1		0x82c5
#define ApertureConfig_Status_e_Coin_Status_Byte0		0x82c6

// page 'g_FlashControl'

#define FlashControl_s32_DelayFromStartPoint_lines_Byte0		0x8300
#define FlashControl_s32_DelayFromStartPoint_lines_Byte1		0x8301
#define FlashControl_s32_DelayFromStartPoint_lines_Byte2		0x8302
#define FlashControl_s32_DelayFromStartPoint_lines_Byte3		0x8303
#define FlashControl_u32_StrobeLength_us_Byte0		0x8304
#define FlashControl_u32_StrobeLength_us_Byte1		0x8305
#define FlashControl_u32_StrobeLength_us_Byte2		0x8306
#define FlashControl_u32_StrobeLength_us_Byte3		0x8307
#define FlashControl_u8_StrobesPerFrame_Byte0		0x8308
#define FlashControl_u8_FrameCount_Byte0		0x8309
#define FlashControl_u8_MaxFramesToWaitForFlashTrigger_Byte0		0x830a
#define FlashControl_e_StrobeStartPoint_Frame_Byte0		0x830b
#define FlashControl_e_Flag_GlobalResetFrameOnly_Byte0		0x830c
#define FlashControl_e_Flag_DoStrobeModulation_Byte0		0x830d
#define FlashControl_e_Flag_FlashMode_Byte0		0x830e

// page 'g_FlashStatus'

#define FlashStatus_u32_MinStrobeLength_us_Byte0		0x8340
#define FlashStatus_u32_MinStrobeLength_us_Byte1		0x8341
#define FlashStatus_u32_MinStrobeLength_us_Byte2		0x8342
#define FlashStatus_u32_MinStrobeLength_us_Byte3		0x8343
#define FlashStatus_u32_MaxStrobeLength_us_Byte0		0x8344
#define FlashStatus_u32_MaxStrobeLength_us_Byte1		0x8345
#define FlashStatus_u32_MaxStrobeLength_us_Byte2		0x8346
#define FlashStatus_u32_MaxStrobeLength_us_Byte3		0x8347
#define FlashStatus_u32_StrobeLengthStep_Byte0		0x8348
#define FlashStatus_u32_StrobeLengthStep_Byte1		0x8349
#define FlashStatus_u32_StrobeLengthStep_Byte2		0x834a
#define FlashStatus_u32_StrobeLengthStep_Byte3		0x834b
#define FlashStatus_u8_MaxStrobesPerFrame_Byte0		0x834c
#define FlashStatus_u8_FlashFiredFrameCount_Byte0		0x834d
#define FlashStatus_u8_NumberOfForcedInputProcUpdates_Byte0		0x834e
#define FlashStatus_u8_NumberOfConsecutiveDelayedFrames_Byte0		0x834f
#define FlashStatus_u8_FlashSyncErrorCount_Byte0		0x8350
#define FlashStatus_e_Flag_ForceInputProcUpdation_Byte0		0x8351
#define FlashStatus_e_FlashError_Info_Byte0		0x8352
#define FlashStatus_e_Flag_StrobeModulationSupported_Byte0		0x8353

// page 'g_Sensor_Tuning_Control'

#define Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte0		0x8380
#define Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte1		0x8381
#define Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte2		0x8382
#define Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte3		0x8383
#define Sensor_Tuning_Control_u32_NVM_Data_Address_Byte0		0x8384
#define Sensor_Tuning_Control_u32_NVM_Data_Address_Byte1		0x8385
#define Sensor_Tuning_Control_u32_NVM_Data_Address_Byte2		0x8386
#define Sensor_Tuning_Control_u32_NVM_Data_Address_Byte3		0x8387
#define Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0		0x8388
#define Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte1		0x8389
#define Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0		0x838a
#define Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0		0x838b
#define Sensor_Tuning_Control_e_Flag_ConfigurationDone_Byte0		0x838c
#define Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0		0x838d

// page 'g_Sensor_Tuning_Status'

#define Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0		0x83c0
#define Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte1		0x83c1
#define Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0		0x83c2
#define Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte1		0x83c3
#define Sensor_Tuning_Status_u16_NVM_Data_Size_Byte0		0x83c4
#define Sensor_Tuning_Status_u16_NVM_Data_Size_Byte1		0x83c5
#define Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0		0x83c6
#define Sensor_Tuning_Status_e_Coin_NVM_Status_Byte0		0x83c7

// page 'g_Pipe_RAW'

#define Pipe_RAW_u16_output_res_X_size_Byte0		0x8400
#define Pipe_RAW_u16_output_res_X_size_Byte1		0x8401
#define Pipe_RAW_u16_output_res_Y_size_Byte0		0x8402
#define Pipe_RAW_u16_output_res_Y_size_Byte1		0x8403
#define Pipe_RAW_u16_woi_res_X_Byte0		0x8404
#define Pipe_RAW_u16_woi_res_X_Byte1		0x8405
#define Pipe_RAW_u16_woi_res_Y_Byte0		0x8406
#define Pipe_RAW_u16_woi_res_Y_Byte1		0x8407

// page 'g_Zoom_Control1'

#define Zoom_Control1_f_MaxDZ_Byte0		0x8440
#define Zoom_Control1_f_MaxDZ_Byte1		0x8441
#define Zoom_Control1_f_MaxDZ_Byte2		0x8442
#define Zoom_Control1_f_MaxDZ_Byte3		0x8443

// page 'g_GrabNotify'

#define GrabNotify_u32_DMA_GRAB_Indicator_Byte0		0x8480
#define GrabNotify_u32_DMA_GRAB_Indicator_Byte1		0x8481
#define GrabNotify_u32_DMA_GRAB_Indicator_Byte2		0x8482
#define GrabNotify_u32_DMA_GRAB_Indicator_Byte3		0x8483
#define GrabNotify_u32_DMA_GRAB_Frame_Id_Byte0		0x8484
#define GrabNotify_u32_DMA_GRAB_Frame_Id_Byte1		0x8485
#define GrabNotify_u32_DMA_GRAB_Frame_Id_Byte2		0x8486
#define GrabNotify_u32_DMA_GRAB_Frame_Id_Byte3		0x8487
#define GrabNotify_u32_DMA_GRAB_Indicator_For_VideoStab_Byte0		0x8488
#define GrabNotify_u32_DMA_GRAB_Indicator_For_VideoStab_Byte1		0x8489
#define GrabNotify_u32_DMA_GRAB_Indicator_For_VideoStab_Byte2		0x848a
#define GrabNotify_u32_DMA_GRAB_Indicator_For_VideoStab_Byte3		0x848b

// page 'g_FrameParamStatus_Extn'

#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMin_x256_Byte0		0x84c0
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMin_x256_Byte1		0x84c1
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMin_x256_Byte2		0x84c2
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMin_x256_Byte3		0x84c3
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMax_x256_Byte0		0x84c4
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMax_x256_Byte1		0x84c5
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMax_x256_Byte2		0x84c6
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainMax_x256_Byte3		0x84c7
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainStep_x256_Byte0		0x84c8
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainStep_x256_Byte1		0x84c9
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainStep_x256_Byte2		0x84ca
#define FrameParamStatus_Extn_u32_SensorParametersAnalogGainStep_x256_Byte3		0x84cb
#define FrameParamStatus_Extn_u32_StatsInvalid_Byte0		0x84cc
#define FrameParamStatus_Extn_u32_StatsInvalid_Byte1		0x84cd
#define FrameParamStatus_Extn_u32_StatsInvalid_Byte2		0x84ce
#define FrameParamStatus_Extn_u32_StatsInvalid_Byte3		0x84cf
#define FrameParamStatus_Extn_u32_SizeOfFrameParamStatus_Byte0		0x84d0
#define FrameParamStatus_Extn_u32_SizeOfFrameParamStatus_Byte1		0x84d1
#define FrameParamStatus_Extn_u32_SizeOfFrameParamStatus_Byte2		0x84d2
#define FrameParamStatus_Extn_u32_SizeOfFrameParamStatus_Byte3		0x84d3

// page 'g_SensorPowerManagement_Control'

#define SensorPowerManagement_Control_e_PowerAction_Byte0		0x8500
#define SensorPowerManagement_Control_e_Flag_Result_Byte0		0x8501
#define SensorPowerManagement_Control_e_Flag_EnableSMIAPP_PowerUpSequence_Byte0		0x8502

// page 'g_SensorPowerManagement_Status'

#define SensorPowerManagement_Status_u32_ExternalClockFreq_MHz_x100_Byte0		0x8540
#define SensorPowerManagement_Status_u32_ExternalClockFreq_MHz_x100_Byte1		0x8541
#define SensorPowerManagement_Status_u32_ExternalClockFreq_MHz_x100_Byte2		0x8542
#define SensorPowerManagement_Status_u32_ExternalClockFreq_MHz_x100_Byte3		0x8543
#define SensorPowerManagement_Status_u16_VoltageAnalog_x100_Byte0		0x8544
#define SensorPowerManagement_Status_u16_VoltageAnalog_x100_Byte1		0x8545
#define SensorPowerManagement_Status_u16_VoltageDigital_x100_Byte0		0x8546
#define SensorPowerManagement_Status_u16_VoltageDigital_x100_Byte1		0x8547
#define SensorPowerManagement_Status_u16_VoltageIO_x100_Byte0		0x8548
#define SensorPowerManagement_Status_u16_VoltageIO_x100_Byte1		0x8549
#define SensorPowerManagement_Status_e_PowerCommand_Byte0		0x854a

// page 'g_SMS_Control'

#define SMS_Control_u32_FrameRate_x100_Byte0		0x85c0
#define SMS_Control_u32_FrameRate_x100_Byte1		0x85c1
#define SMS_Control_u32_FrameRate_x100_Byte2		0x85c2
#define SMS_Control_u32_FrameRate_x100_Byte3		0x85c3
#define SMS_Control_u16_WOI_X_size_Byte0		0x85c4
#define SMS_Control_u16_WOI_X_size_Byte1		0x85c5
#define SMS_Control_u16_WOI_Y_size_Byte0		0x85c6
#define SMS_Control_u16_WOI_Y_size_Byte1		0x85c7
#define SMS_Control_u16_X_size_Byte0		0x85c8
#define SMS_Control_u16_X_size_Byte1		0x85c9
#define SMS_Control_u16_Y_size_Byte0		0x85ca
#define SMS_Control_u16_Y_size_Byte1		0x85cb
#define SMS_Control_u16_CsiRawFormat_Byte0		0x85cc
#define SMS_Control_u16_CsiRawFormat_Byte1		0x85cd
#define SMS_Control_e_Coin_Ctrl_Byte0		0x85ce

// page 'g_SMS_Status'

#define SMS_Status_u32_LineLength_pck_Byte0		0x8600
#define SMS_Status_u32_LineLength_pck_Byte1		0x8601
#define SMS_Status_u32_LineLength_pck_Byte2		0x8602
#define SMS_Status_u32_LineLength_pck_Byte3		0x8603
#define SMS_Status_u32_FrameLength_lines_Byte0		0x8604
#define SMS_Status_u32_FrameLength_lines_Byte1		0x8605
#define SMS_Status_u32_FrameLength_lines_Byte2		0x8606
#define SMS_Status_u32_FrameLength_lines_Byte3		0x8607
#define SMS_Status_u32_Min_ExposureTime_us_Byte0		0x8608
#define SMS_Status_u32_Min_ExposureTime_us_Byte1		0x8609
#define SMS_Status_u32_Min_ExposureTime_us_Byte2		0x860a
#define SMS_Status_u32_Min_ExposureTime_us_Byte3		0x860b
#define SMS_Status_u32_Max_ExposureTime_us_Byte0		0x860c
#define SMS_Status_u32_Max_ExposureTime_us_Byte1		0x860d
#define SMS_Status_u32_Max_ExposureTime_us_Byte2		0x860e
#define SMS_Status_u32_Max_ExposureTime_us_Byte3		0x860f
#define SMS_Status_u32_ExposureQuantizationStep_us_Byte0		0x8610
#define SMS_Status_u32_ExposureQuantizationStep_us_Byte1		0x8611
#define SMS_Status_u32_ExposureQuantizationStep_us_Byte2		0x8612
#define SMS_Status_u32_ExposureQuantizationStep_us_Byte3		0x8613
#define SMS_Status_u32_ActiveData_ReadoutTime_us_Byte0		0x8614
#define SMS_Status_u32_ActiveData_ReadoutTime_us_Byte1		0x8615
#define SMS_Status_u32_ActiveData_ReadoutTime_us_Byte2		0x8616
#define SMS_Status_u32_ActiveData_ReadoutTime_us_Byte3		0x8617
#define SMS_Status_e_Coin_Status_Byte0		0x8618
// page 'g_GridironStatus'

#define GridironStatus_f_LiveCast_Byte0		0x8580
#define GridironStatus_f_LiveCast_Byte1		0x8581
#define GridironStatus_f_LiveCast_Byte2		0x8582
#define GridironStatus_f_LiveCast_Byte3		0x8583
#define GridironStatus_f_Sensor_HScale_Byte0		0x8584
#define GridironStatus_f_Sensor_HScale_Byte1		0x8585
#define GridironStatus_f_Sensor_HScale_Byte2		0x8586
#define GridironStatus_f_Sensor_HScale_Byte3		0x8587
#define GridironStatus_f_Sensor_VScale_Byte0		0x8588
#define GridironStatus_f_Sensor_VScale_Byte1		0x8589
#define GridironStatus_f_Sensor_VScale_Byte2		0x858a
#define GridironStatus_f_Sensor_VScale_Byte3		0x858b
#define GridironStatus_u16_Sensor_HScale_x256_Byte0		0x858c
#define GridironStatus_u16_Sensor_HScale_x256_Byte1		0x858d
#define GridironStatus_u16_Sensor_VScale_x256_Byte0		0x858e
#define GridironStatus_u16_Sensor_VScale_x256_Byte1		0x858f
#define GridironStatus_u16_Crop_HStart_Byte0		0x8590
#define GridironStatus_u16_Crop_HStart_Byte1		0x8591
#define GridironStatus_u16_Crop_VStart_Byte0		0x8592
#define GridironStatus_u16_Crop_VStart_Byte1		0x8593
#define GridironStatus_u16_Image_HSize_Byte0		0x8594
#define GridironStatus_u16_Image_HSize_Byte1		0x8595
#define GridironStatus_u16_Image_VSize_Byte0		0x8596
#define GridironStatus_u16_Image_VSize_Byte1		0x8597
#define GridironStatus_u16_Sensor_HSize_Byte0		0x8598
#define GridironStatus_u16_Sensor_HSize_Byte1		0x8599
#define GridironStatus_u16_Phase_Byte0		0x859a
#define GridironStatus_u8_LogGridPitch_Byte0		0x859b
#define GridironStatus_e_Flag_Cast0_Byte0		0x859c
#define GridironStatus_e_Flag_Cast1_Byte0		0x859d
#define GridironStatus_e_Flag_Cast2_Byte0		0x859e
#define GridironStatus_e_Flag_Cast3_Byte0		0x859f
#define GridironStatus_e_Flag_Enable_Byte0		0x85a0
#define GridironStatus_e_Flag_VerFlip_Byte0		0x85a1
#define GridironStatus_e_Flag_HorFlip_Byte0		0x85a2
#endif	// _EXTENSION_H_

