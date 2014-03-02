/*
 *  © ST Microelectronics Pvt. Ltd, 2010 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Microelectronics
 *
 */

/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * The features are joint developement between ST-Ericsson SA and ST Microelectronics Pvt. Ltd. 
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXTENSION_H_ 
#define _EXTENSION_H_
//
//  FW_7.37.0_SENSOR_2_LLA_3.1_LLCD_25.0 (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MAJOR : 7 Incremented With Compatibility break (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MINOR : 37 Incremented With Feature addition w/o any break in compatibility (parsed from source files)
//  version info ->       FIRMWARE_VERSION_MICRO : 0 Incremented WithBug fixes (parsed from source files)
//  version info ->         LOW_LEVEL_API_SENSOR : 2 customer identification (parsed from source files)
//  version info ->  LOW_LEVEL_API_MAJOR_VERSION : 3 Incremented when there is compatibility break in API and ISP FW update is necessary (parsed from source files)
//  version info ->  LOW_LEVEL_API_MINOR_VERSION : 1 Incremented when there is change in API or feature addition that is backward compatible (parsed from source files)
//  version info ->LOW_LEVEL_CAMERA_DRIVER_MAJOR : 25 Incremented with every release (parsed from source files)
//  version info ->LOW_LEVEL_CAMERA_DRIVER_MINOR : 0 Incremented each time there is bug fix on old API (parsed from source files)
//

// List all the page elements and enum types used in the page element definition
//



// *****************************************
//       Enums used by Page Elements
// *****************************************
// *****************************************

typedef enum
{
	Error_DISABLED = 0,
	Error_ENABLED = 1,
	Error_ENABLED_ABORT = 2,
	Error_ENABLED_RECOVER = 3,
} Error_Control_te;

typedef enum
{
	FlashError_e_FLASH_ERROR_NONE = 0,
	FlashError_e_FLASH_ERROR_CONFIGURATION = 1,
	FlashError_e_FLASH_ERROR_STROBE_DID_NOT_TRIGGER = 2,
	FlashError_e_FLASH_ERROR_OUT_OF_FLASH_API_CONTEXT = 3,
} FlashError_te;

typedef enum
{
	LLD_USAGE_MODE_VF = 0,
	LLD_USAGE_MODE_AF = 1,
	LLD_USAGE_MODE_STILL_CAPTURE = 2,
	LLD_USAGE_MODE_NIGHT_STILL_CAPTURE = 3,
	LLD_USAGE_MODE_STILL_SEQ_CAPTURE = 4,
	LLD_USAGE_MODE_VIDEO_CAPTURE = 5,
	LLD_USAGE_MODE_NIGHT_VIDEO_CAPTURE = 6,
	LLD_USAGE_MODE_HQ_VIDEO_CAPTURE = 7,
	LLD_USAGE_MODE_HS_VIDEO_CAPTURE = 8,
	CAM_DRV_USAGE_MODE_UNDEFINED = 255,
} LLD_USAGE_MODE_te;

typedef enum
{
	PowerAction_e_idle = 0,
	PowerAction_e_requested = 1,
	PowerAction_e_complete = 2,
} PowerAction_te;

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
	SensorModeCapture_e_STILL = 0,
	SensorModeCapture_e_STILL_NIGHT = 2,
	SensorModeCapture_e_STILL_SEQ = 3,
	SensorModeCapture_e_FLASH = 4,
} SensorModeCapture_te;

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
	TypeNVMExport_e_Parsed = 0,
	TypeNVMExport_e_Raw = 1,
} TypeNVMExport_te;

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
	isp_uint32_t	u32_focal_length_x100;
} FrameParamStatus_Extn_ts;

typedef struct
{
	isp_uint32_t	pu32_HostAssignedFrameStatusAddr;
	isp_uint32_t	u32_AfStatsValid;
	isp_uint32_t	u32_AfStatsLensPos;
	isp_uint32_t	u32_AfStatsFrameId;
	isp_uint32_t	u32_SizeOfFrameParamStatus;
} FrameParamStatus_Af_ts;

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
//       Page Elements
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

// page 'g_RunMode_Control'

#define RunMode_Control_e_StreamMode_Byte0		0x8140
#define RunMode_Control_e_SensorModeVF_Byte0		0x8141
#define RunMode_Control_e_SensorModeCapture_Byte0		0x8142
#define RunMode_Control_e_Flag_MechanicalShutterUsed_Byte0		0x8143
#define RunMode_Control_e_Coin_Ctrl_Byte0		0x8144
#define RunMode_Control_e_LLD_USAGE_MODE_usagemode_Byte0		0x8145

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
#define Sensor_Tuning_Control_e_TypeNVMExport_Byte0		0x838e

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
#define GrabNotify_u32_DMA_GRAB_Abort_Byte0		0x848c
#define GrabNotify_u32_DMA_GRAB_Abort_Byte1		0x848d
#define GrabNotify_u32_DMA_GRAB_Abort_Byte2		0x848e
#define GrabNotify_u32_DMA_GRAB_Abort_Byte3		0x848f

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
#define FrameParamStatus_Extn_u32_focal_length_x100_Byte0		0x84d4
#define FrameParamStatus_Extn_u32_focal_length_x100_Byte1		0x84d5
#define FrameParamStatus_Extn_u32_focal_length_x100_Byte2		0x84d6
#define FrameParamStatus_Extn_u32_focal_length_x100_Byte3		0x84d7

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
#define GridironStatus_u8_Active_ReferenceCast_Count_Byte0		0x85a3

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

// page 'g_TraceLogsControl'

#define TraceLogsControl_u32_BufferAddr_Byte0		0x8640
#define TraceLogsControl_u32_BufferAddr_Byte1		0x8641
#define TraceLogsControl_u32_BufferAddr_Byte2		0x8642
#define TraceLogsControl_u32_BufferAddr_Byte3		0x8643
#define TraceLogsControl_u32_BufferSize_Byte0		0x8644
#define TraceLogsControl_u32_BufferSize_Byte1		0x8645
#define TraceLogsControl_u32_BufferSize_Byte2		0x8646
#define TraceLogsControl_u32_BufferSize_Byte3		0x8647
#define TraceLogsControl_u32_LogLevels_Byte0		0x8648
#define TraceLogsControl_u32_LogLevels_Byte1		0x8649
#define TraceLogsControl_u32_LogLevels_Byte2		0x864a
#define TraceLogsControl_u32_LogLevels_Byte3		0x864b
#define TraceLogsControl_u8_LogEnable_Byte0		0x864c
#define TraceLogsControl_u8_TraceMechanismSelect_Byte0		0x864d

// page 'g_Pipe_Scalar_UserParams[0]'

#define Pipe_Scalar_UserParams_0_u16_PhysRegSize_Byte0		0x8680
#define Pipe_Scalar_UserParams_0_u16_PhysRegSize_Byte1		0x8681
#define Pipe_Scalar_UserParams_0_u8_PhysRegWidth_Byte0		0x8682
#define Pipe_Scalar_UserParams_0_u8_BSize_Byte0		0x8683
#define Pipe_Scalar_UserParams_0_u8_Crisp_Byte0		0x8684
#define Pipe_Scalar_UserParams_0_e_Flag_AntiZipEnable_Byte0		0x8685

// page 'g_Pipe_Scalar_UserParams[1]'

#define Pipe_Scalar_UserParams_1_u16_PhysRegSize_Byte0		0x86c0
#define Pipe_Scalar_UserParams_1_u16_PhysRegSize_Byte1		0x86c1
#define Pipe_Scalar_UserParams_1_u8_PhysRegWidth_Byte0		0x86c2
#define Pipe_Scalar_UserParams_1_u8_BSize_Byte0		0x86c3
#define Pipe_Scalar_UserParams_1_u8_Crisp_Byte0		0x86c4
#define Pipe_Scalar_UserParams_1_e_Flag_AntiZipEnable_Byte0		0x86c5

// page 'g_FrameParamStatus_Af'

#define FrameParamStatus_Af_pu32_HostAssignedFrameStatusAddr_Byte0		0x8700
#define FrameParamStatus_Af_pu32_HostAssignedFrameStatusAddr_Byte1		0x8701
#define FrameParamStatus_Af_pu32_HostAssignedFrameStatusAddr_Byte2		0x8702
#define FrameParamStatus_Af_pu32_HostAssignedFrameStatusAddr_Byte3		0x8703
#define FrameParamStatus_Af_u32_AfStatsValid_Byte0		0x8704
#define FrameParamStatus_Af_u32_AfStatsValid_Byte1		0x8705
#define FrameParamStatus_Af_u32_AfStatsValid_Byte2		0x8706
#define FrameParamStatus_Af_u32_AfStatsValid_Byte3		0x8707
#define FrameParamStatus_Af_u32_AfStatsLensPos_Byte0		0x8708
#define FrameParamStatus_Af_u32_AfStatsLensPos_Byte1		0x8709
#define FrameParamStatus_Af_u32_AfStatsLensPos_Byte2		0x870a
#define FrameParamStatus_Af_u32_AfStatsLensPos_Byte3		0x870b
#define FrameParamStatus_Af_u32_AfStatsFrameId_Byte0		0x870c
#define FrameParamStatus_Af_u32_AfStatsFrameId_Byte1		0x870d
#define FrameParamStatus_Af_u32_AfStatsFrameId_Byte2		0x870e
#define FrameParamStatus_Af_u32_AfStatsFrameId_Byte3		0x870f
#define FrameParamStatus_Af_u32_SizeOfFrameParamStatus_Byte0		0x8710
#define FrameParamStatus_Af_u32_SizeOfFrameParamStatus_Byte1		0x8711
#define FrameParamStatus_Af_u32_SizeOfFrameParamStatus_Byte2		0x8712
#define FrameParamStatus_Af_u32_SizeOfFrameParamStatus_Byte3		0x8713

// page 'g_ErrorHandler'

#define ErrorHandler_u16_CSI_EOF_Counter_Byte0		0x8740
#define ErrorHandler_u16_CSI_EOF_Counter_Byte1		0x8741
#define ErrorHandler_e_Error_Control_Byte0		0x8742
#define ErrorHandler_e_Flag_Error_Abort_Byte0		0x8743
#define ErrorHandler_e_Flag_Error_Recover_Byte0		0x8744
#define ErrorHandler_u8_CSI_Error_Counter_Byte0		0x8745
#define ErrorHandler_u8_CCP_Error_Counter_Byte0		0x8746

// page 'g_CustomStockMatrix[0]'

#define CustomStockMatrix_0_f_StockMatrix_0_Byte0		0x8780
#define CustomStockMatrix_0_f_StockMatrix_0_Byte1		0x8781
#define CustomStockMatrix_0_f_StockMatrix_0_Byte2		0x8782
#define CustomStockMatrix_0_f_StockMatrix_0_Byte3		0x8783
#define CustomStockMatrix_0_f_StockMatrix_1_Byte0		0x8784
#define CustomStockMatrix_0_f_StockMatrix_1_Byte1		0x8785
#define CustomStockMatrix_0_f_StockMatrix_1_Byte2		0x8786
#define CustomStockMatrix_0_f_StockMatrix_1_Byte3		0x8787
#define CustomStockMatrix_0_f_StockMatrix_2_Byte0		0x8788
#define CustomStockMatrix_0_f_StockMatrix_2_Byte1		0x8789
#define CustomStockMatrix_0_f_StockMatrix_2_Byte2		0x878a
#define CustomStockMatrix_0_f_StockMatrix_2_Byte3		0x878b
#define CustomStockMatrix_0_f_StockMatrix_3_Byte0		0x878c
#define CustomStockMatrix_0_f_StockMatrix_3_Byte1		0x878d
#define CustomStockMatrix_0_f_StockMatrix_3_Byte2		0x878e
#define CustomStockMatrix_0_f_StockMatrix_3_Byte3		0x878f
#define CustomStockMatrix_0_f_StockMatrix_4_Byte0		0x8790
#define CustomStockMatrix_0_f_StockMatrix_4_Byte1		0x8791
#define CustomStockMatrix_0_f_StockMatrix_4_Byte2		0x8792
#define CustomStockMatrix_0_f_StockMatrix_4_Byte3		0x8793
#define CustomStockMatrix_0_f_StockMatrix_5_Byte0		0x8794
#define CustomStockMatrix_0_f_StockMatrix_5_Byte1		0x8795
#define CustomStockMatrix_0_f_StockMatrix_5_Byte2		0x8796
#define CustomStockMatrix_0_f_StockMatrix_5_Byte3		0x8797
#define CustomStockMatrix_0_f_StockMatrix_6_Byte0		0x8798
#define CustomStockMatrix_0_f_StockMatrix_6_Byte1		0x8799
#define CustomStockMatrix_0_f_StockMatrix_6_Byte2		0x879a
#define CustomStockMatrix_0_f_StockMatrix_6_Byte3		0x879b
#define CustomStockMatrix_0_f_StockMatrix_7_Byte0		0x879c
#define CustomStockMatrix_0_f_StockMatrix_7_Byte1		0x879d
#define CustomStockMatrix_0_f_StockMatrix_7_Byte2		0x879e
#define CustomStockMatrix_0_f_StockMatrix_7_Byte3		0x879f
#define CustomStockMatrix_0_f_StockMatrix_8_Byte0		0x87a0
#define CustomStockMatrix_0_f_StockMatrix_8_Byte1		0x87a1
#define CustomStockMatrix_0_f_StockMatrix_8_Byte2		0x87a2
#define CustomStockMatrix_0_f_StockMatrix_8_Byte3		0x87a3

// page 'g_CustomStockMatrix[1]'

#define CustomStockMatrix_1_f_StockMatrix_0_Byte0		0x87c0
#define CustomStockMatrix_1_f_StockMatrix_0_Byte1		0x87c1
#define CustomStockMatrix_1_f_StockMatrix_0_Byte2		0x87c2
#define CustomStockMatrix_1_f_StockMatrix_0_Byte3		0x87c3
#define CustomStockMatrix_1_f_StockMatrix_1_Byte0		0x87c4
#define CustomStockMatrix_1_f_StockMatrix_1_Byte1		0x87c5
#define CustomStockMatrix_1_f_StockMatrix_1_Byte2		0x87c6
#define CustomStockMatrix_1_f_StockMatrix_1_Byte3		0x87c7
#define CustomStockMatrix_1_f_StockMatrix_2_Byte0		0x87c8
#define CustomStockMatrix_1_f_StockMatrix_2_Byte1		0x87c9
#define CustomStockMatrix_1_f_StockMatrix_2_Byte2		0x87ca
#define CustomStockMatrix_1_f_StockMatrix_2_Byte3		0x87cb
#define CustomStockMatrix_1_f_StockMatrix_3_Byte0		0x87cc
#define CustomStockMatrix_1_f_StockMatrix_3_Byte1		0x87cd
#define CustomStockMatrix_1_f_StockMatrix_3_Byte2		0x87ce
#define CustomStockMatrix_1_f_StockMatrix_3_Byte3		0x87cf
#define CustomStockMatrix_1_f_StockMatrix_4_Byte0		0x87d0
#define CustomStockMatrix_1_f_StockMatrix_4_Byte1		0x87d1
#define CustomStockMatrix_1_f_StockMatrix_4_Byte2		0x87d2
#define CustomStockMatrix_1_f_StockMatrix_4_Byte3		0x87d3
#define CustomStockMatrix_1_f_StockMatrix_5_Byte0		0x87d4
#define CustomStockMatrix_1_f_StockMatrix_5_Byte1		0x87d5
#define CustomStockMatrix_1_f_StockMatrix_5_Byte2		0x87d6
#define CustomStockMatrix_1_f_StockMatrix_5_Byte3		0x87d7
#define CustomStockMatrix_1_f_StockMatrix_6_Byte0		0x87d8
#define CustomStockMatrix_1_f_StockMatrix_6_Byte1		0x87d9
#define CustomStockMatrix_1_f_StockMatrix_6_Byte2		0x87da
#define CustomStockMatrix_1_f_StockMatrix_6_Byte3		0x87db
#define CustomStockMatrix_1_f_StockMatrix_7_Byte0		0x87dc
#define CustomStockMatrix_1_f_StockMatrix_7_Byte1		0x87dd
#define CustomStockMatrix_1_f_StockMatrix_7_Byte2		0x87de
#define CustomStockMatrix_1_f_StockMatrix_7_Byte3		0x87df
#define CustomStockMatrix_1_f_StockMatrix_8_Byte0		0x87e0
#define CustomStockMatrix_1_f_StockMatrix_8_Byte1		0x87e1
#define CustomStockMatrix_1_f_StockMatrix_8_Byte2		0x87e2
#define CustomStockMatrix_1_f_StockMatrix_8_Byte3		0x87e3

// page 'g_SensorPipeSettings_Control'

#define SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0		0x8800
#define SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0		0x8801

// page 'g_SensorPipeSettings_Status'

#define SensorPipeSettings_Status_e_Coin_SensorSettings_Byte0		0x8840
#define SensorPipeSettings_Status_e_Coin_ISPSettings_Byte0		0x8841

// page 'g_VariableFrameRateControl'

#define VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0		0x8880
#define VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte1		0x8881
#define VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte2		0x8882
#define VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte3		0x8883
#define VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte0		0x8884
#define VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte1		0x8885
#define VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte2		0x8886
#define VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte3		0x8887
#define VariableFrameRateControl_e_Flag_Byte0		0x8888

// page 'g_VariableFrameRateStatus'

#define VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte0		0x88c0
#define VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte1		0x88c1
#define VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte2		0x88c2
#define VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte3		0x88c3
#define VariableFrameRateStatus_f_MaximumFrameRate_Hz_Byte0		0x88c4
#define VariableFrameRateStatus_f_MaximumFrameRate_Hz_Byte1		0x88c5
#define VariableFrameRateStatus_f_MaximumFrameRate_Hz_Byte2		0x88c6
#define VariableFrameRateStatus_f_MaximumFrameRate_Hz_Byte3		0x88c7
#define VariableFrameRateStatus_e_Flag_Byte0		0x88c8

// page 'g_Mozart_Ctrl'

#define Mozart_Ctrl_u16_Mozart_M_SBPeaker_Byte0		0x8900
#define Mozart_Ctrl_u16_Mozart_M_SBPeaker_Byte1		0x8901
#define Mozart_Ctrl_u16_Mozart_SBCoring_min_Byte0		0x8902
#define Mozart_Ctrl_u16_Mozart_SBCoring_min_Byte1		0x8903
#define Mozart_Ctrl_u16_Mozart_H_SBPeaker_Byte0		0x8904
#define Mozart_Ctrl_u16_Mozart_H_SBPeaker_Byte1		0x8905
#define Mozart_Ctrl_e_Flag_MozartEnable_Byte0		0x8906
#define Mozart_Ctrl_u8_Mozart_Chroma_AntiAliasing_Byte0		0x8907
#define Mozart_Ctrl_u8_Mozart_M_SBCoring_gain_Byte0		0x8908
#define Mozart_Ctrl_u8_Mozart_H_SBCoring_gain_Byte0		0x8909
#define Mozart_Ctrl_u8_Mozart_LumaGain_Byte0		0x890a
#define Mozart_Ctrl_u8_Mozart_ColorGain_Byte0		0x890b

// page 'g_Norcos_Ctrl[0]'

#define Norcos_Ctrl_0_u16_Norcos_Luma_Control_Byte0		0x8940
#define Norcos_Ctrl_0_u16_Norcos_Luma_Control_Byte1		0x8941
#define Norcos_Ctrl_0_e_Flag_Norcos_Luma_Enable_Byte0		0x8942
#define Norcos_Ctrl_0_e_Flag_Norcos_Chroma_Enable_Byte0		0x8943
#define Norcos_Ctrl_0_e_Flag_Norcos_YUV422_Enable_Byte0		0x8944
#define Norcos_Ctrl_0_u8_Norcos_Output_Control_Byte0		0x8945
#define Norcos_Ctrl_0_u8_Norcos_Luma_FlatParam_Byte0		0x8946
#define Norcos_Ctrl_0_u8_Norcos_Luma_FlatDetectGrade_Byte0		0x8947
#define Norcos_Ctrl_0_u8_Norcos_Luma_SharpGrade_Byte0		0x8948
#define Norcos_Ctrl_0_u8_Norcos_Luma_UnsharpGrade_Byte0		0x8949
#define Norcos_Ctrl_0_u8_Norcos_Luma_ImpulseNoise_Byte0		0x894a
#define Norcos_Ctrl_0_u8_Norcos_Luma_ImpulseNoiseThreshold_Byte0		0x894b
#define Norcos_Ctrl_0_u8_Norcos_Luma_Tx_Byte0		0x894c
#define Norcos_Ctrl_0_u8_Norcos_Luma_PeakLowThreshold_Byte0		0x894d
#define Norcos_Ctrl_0_u8_Norcos_Luma_PeakHighThreshold_Byte0		0x894e
#define Norcos_Ctrl_0_u8_Norcos_Luma_PseudoNoise_Byte0		0x894f
#define Norcos_Ctrl_0_u8_Norcos_Luma_OverShootGain_Byte0		0x8950
#define Norcos_Ctrl_0_u8_Norcos_Chroma_Ctrl_Byte0		0x8951
#define Norcos_Ctrl_0_u8_Norcos_Chroma_FlatParam_Byte0		0x8952
#define Norcos_Ctrl_0_u8_Norcos_Chroma_FlatDetectGrade_Byte0		0x8953
#define Norcos_Ctrl_0_u8_Norcos_Chroma_ImpulseNoise_Byte0		0x8954
#define Norcos_Ctrl_0_u8_Norcos_Chroma_ImpulseThreshold_Byte0		0x8955

// page 'g_Norcos_Ctrl[1]'

#define Norcos_Ctrl_1_u16_Norcos_Luma_Control_Byte0		0x8980
#define Norcos_Ctrl_1_u16_Norcos_Luma_Control_Byte1		0x8981
#define Norcos_Ctrl_1_e_Flag_Norcos_Luma_Enable_Byte0		0x8982
#define Norcos_Ctrl_1_e_Flag_Norcos_Chroma_Enable_Byte0		0x8983
#define Norcos_Ctrl_1_e_Flag_Norcos_YUV422_Enable_Byte0		0x8984
#define Norcos_Ctrl_1_u8_Norcos_Output_Control_Byte0		0x8985
#define Norcos_Ctrl_1_u8_Norcos_Luma_FlatParam_Byte0		0x8986
#define Norcos_Ctrl_1_u8_Norcos_Luma_FlatDetectGrade_Byte0		0x8987
#define Norcos_Ctrl_1_u8_Norcos_Luma_SharpGrade_Byte0		0x8988
#define Norcos_Ctrl_1_u8_Norcos_Luma_UnsharpGrade_Byte0		0x8989
#define Norcos_Ctrl_1_u8_Norcos_Luma_ImpulseNoise_Byte0		0x898a
#define Norcos_Ctrl_1_u8_Norcos_Luma_ImpulseNoiseThreshold_Byte0		0x898b
#define Norcos_Ctrl_1_u8_Norcos_Luma_Tx_Byte0		0x898c
#define Norcos_Ctrl_1_u8_Norcos_Luma_PeakLowThreshold_Byte0		0x898d
#define Norcos_Ctrl_1_u8_Norcos_Luma_PeakHighThreshold_Byte0		0x898e
#define Norcos_Ctrl_1_u8_Norcos_Luma_PseudoNoise_Byte0		0x898f
#define Norcos_Ctrl_1_u8_Norcos_Luma_OverShootGain_Byte0		0x8990
#define Norcos_Ctrl_1_u8_Norcos_Chroma_Ctrl_Byte0		0x8991
#define Norcos_Ctrl_1_u8_Norcos_Chroma_FlatParam_Byte0		0x8992
#define Norcos_Ctrl_1_u8_Norcos_Chroma_FlatDetectGrade_Byte0		0x8993
#define Norcos_Ctrl_1_u8_Norcos_Chroma_ImpulseNoise_Byte0		0x8994
#define Norcos_Ctrl_1_u8_Norcos_Chroma_ImpulseThreshold_Byte0		0x8995

// page 'g_HDR_Control'

#define HDR_Control_f_FrameRate_1_Hz_Byte0		0x89c0
#define HDR_Control_f_FrameRate_1_Hz_Byte1		0x89c1
#define HDR_Control_f_FrameRate_1_Hz_Byte2		0x89c2
#define HDR_Control_f_FrameRate_1_Hz_Byte3		0x89c3
#define HDR_Control_f_FrameRate_2_Hz_Byte0		0x89c4
#define HDR_Control_f_FrameRate_2_Hz_Byte1		0x89c5
#define HDR_Control_f_FrameRate_2_Hz_Byte2		0x89c6
#define HDR_Control_f_FrameRate_2_Hz_Byte3		0x89c7
#define HDR_Control_f_FrameRate_3_Hz_Byte0		0x89c8
#define HDR_Control_f_FrameRate_3_Hz_Byte1		0x89c9
#define HDR_Control_f_FrameRate_3_Hz_Byte2		0x89ca
#define HDR_Control_f_FrameRate_3_Hz_Byte3		0x89cb
#define HDR_Control_f_FrameRate_4_Hz_Byte0		0x89cc
#define HDR_Control_f_FrameRate_4_Hz_Byte1		0x89cd
#define HDR_Control_f_FrameRate_4_Hz_Byte2		0x89ce
#define HDR_Control_f_FrameRate_4_Hz_Byte3		0x89cf
#define HDR_Control_u32_TargetExposureTime_1_us_Byte0		0x89d0
#define HDR_Control_u32_TargetExposureTime_1_us_Byte1		0x89d1
#define HDR_Control_u32_TargetExposureTime_1_us_Byte2		0x89d2
#define HDR_Control_u32_TargetExposureTime_1_us_Byte3		0x89d3
#define HDR_Control_u32_TargetExposureTime_2_us_Byte0		0x89d4
#define HDR_Control_u32_TargetExposureTime_2_us_Byte1		0x89d5
#define HDR_Control_u32_TargetExposureTime_2_us_Byte2		0x89d6
#define HDR_Control_u32_TargetExposureTime_2_us_Byte3		0x89d7
#define HDR_Control_u32_TargetExposureTime_3_us_Byte0		0x89d8
#define HDR_Control_u32_TargetExposureTime_3_us_Byte1		0x89d9
#define HDR_Control_u32_TargetExposureTime_3_us_Byte2		0x89da
#define HDR_Control_u32_TargetExposureTime_3_us_Byte3		0x89db
#define HDR_Control_u32_TargetExposureTime_4_us_Byte0		0x89dc
#define HDR_Control_u32_TargetExposureTime_4_us_Byte1		0x89dd
#define HDR_Control_u32_TargetExposureTime_4_us_Byte2		0x89de
#define HDR_Control_u32_TargetExposureTime_4_us_Byte3		0x89df
#define HDR_Control_u8_FramesCount_Byte0		0x89e0
#define HDR_Control_u8_FPSConfigCount_Byte0		0x89e1
#define HDR_Control_e_Coin_Byte0		0x89e2

// page 'g_HDR_GainControl'

#define HDR_GainControl_f_RedGain_1_Byte0		0x8a00
#define HDR_GainControl_f_RedGain_1_Byte1		0x8a01
#define HDR_GainControl_f_RedGain_1_Byte2		0x8a02
#define HDR_GainControl_f_RedGain_1_Byte3		0x8a03
#define HDR_GainControl_f_GreenGain_1_Byte0		0x8a04
#define HDR_GainControl_f_GreenGain_1_Byte1		0x8a05
#define HDR_GainControl_f_GreenGain_1_Byte2		0x8a06
#define HDR_GainControl_f_GreenGain_1_Byte3		0x8a07
#define HDR_GainControl_f_BlueGain_1_Byte0		0x8a08
#define HDR_GainControl_f_BlueGain_1_Byte1		0x8a09
#define HDR_GainControl_f_BlueGain_1_Byte2		0x8a0a
#define HDR_GainControl_f_BlueGain_1_Byte3		0x8a0b
#define HDR_GainControl_f_RedGain_2_Byte0		0x8a0c
#define HDR_GainControl_f_RedGain_2_Byte1		0x8a0d
#define HDR_GainControl_f_RedGain_2_Byte2		0x8a0e
#define HDR_GainControl_f_RedGain_2_Byte3		0x8a0f
#define HDR_GainControl_f_GreenGain_2_Byte0		0x8a10
#define HDR_GainControl_f_GreenGain_2_Byte1		0x8a11
#define HDR_GainControl_f_GreenGain_2_Byte2		0x8a12
#define HDR_GainControl_f_GreenGain_2_Byte3		0x8a13
#define HDR_GainControl_f_BlueGain_2_Byte0		0x8a14
#define HDR_GainControl_f_BlueGain_2_Byte1		0x8a15
#define HDR_GainControl_f_BlueGain_2_Byte2		0x8a16
#define HDR_GainControl_f_BlueGain_2_Byte3		0x8a17
#define HDR_GainControl_f_RedGain_3_Byte0		0x8a18
#define HDR_GainControl_f_RedGain_3_Byte1		0x8a19
#define HDR_GainControl_f_RedGain_3_Byte2		0x8a1a
#define HDR_GainControl_f_RedGain_3_Byte3		0x8a1b
#define HDR_GainControl_f_GreenGain_3_Byte0		0x8a1c
#define HDR_GainControl_f_GreenGain_3_Byte1		0x8a1d
#define HDR_GainControl_f_GreenGain_3_Byte2		0x8a1e
#define HDR_GainControl_f_GreenGain_3_Byte3		0x8a1f
#define HDR_GainControl_f_BlueGain_3_Byte0		0x8a20
#define HDR_GainControl_f_BlueGain_3_Byte1		0x8a21
#define HDR_GainControl_f_BlueGain_3_Byte2		0x8a22
#define HDR_GainControl_f_BlueGain_3_Byte3		0x8a23
#define HDR_GainControl_f_RedGain_4_Byte0		0x8a24
#define HDR_GainControl_f_RedGain_4_Byte1		0x8a25
#define HDR_GainControl_f_RedGain_4_Byte2		0x8a26
#define HDR_GainControl_f_RedGain_4_Byte3		0x8a27
#define HDR_GainControl_f_GreenGain_4_Byte0		0x8a28
#define HDR_GainControl_f_GreenGain_4_Byte1		0x8a29
#define HDR_GainControl_f_GreenGain_4_Byte2		0x8a2a
#define HDR_GainControl_f_GreenGain_4_Byte3		0x8a2b
#define HDR_GainControl_f_BlueGain_4_Byte0		0x8a2c
#define HDR_GainControl_f_BlueGain_4_Byte1		0x8a2d
#define HDR_GainControl_f_BlueGain_4_Byte2		0x8a2e
#define HDR_GainControl_f_BlueGain_4_Byte3		0x8a2f
#define HDR_GainControl_u16_TargetAnalogGain_1_x256_Byte0		0x8a30
#define HDR_GainControl_u16_TargetAnalogGain_1_x256_Byte1		0x8a31
#define HDR_GainControl_u16_TargetAnalogGain_2_x256_Byte0		0x8a32
#define HDR_GainControl_u16_TargetAnalogGain_2_x256_Byte1		0x8a33
#define HDR_GainControl_u16_TargetAnalogGain_3_x256_Byte0		0x8a34
#define HDR_GainControl_u16_TargetAnalogGain_3_x256_Byte1		0x8a35
#define HDR_GainControl_u16_TargetAnalogGain_4_x256_Byte0		0x8a36
#define HDR_GainControl_u16_TargetAnalogGain_4_x256_Byte1		0x8a37

// page 'g_HDR_Status'

#define HDR_Status_f_FrameRate_1_Hz_Byte0		0x8a40
#define HDR_Status_f_FrameRate_1_Hz_Byte1		0x8a41
#define HDR_Status_f_FrameRate_1_Hz_Byte2		0x8a42
#define HDR_Status_f_FrameRate_1_Hz_Byte3		0x8a43
#define HDR_Status_f_FrameRate_2_Hz_Byte0		0x8a44
#define HDR_Status_f_FrameRate_2_Hz_Byte1		0x8a45
#define HDR_Status_f_FrameRate_2_Hz_Byte2		0x8a46
#define HDR_Status_f_FrameRate_2_Hz_Byte3		0x8a47
#define HDR_Status_f_FrameRate_3_Hz_Byte0		0x8a48
#define HDR_Status_f_FrameRate_3_Hz_Byte1		0x8a49
#define HDR_Status_f_FrameRate_3_Hz_Byte2		0x8a4a
#define HDR_Status_f_FrameRate_3_Hz_Byte3		0x8a4b
#define HDR_Status_f_FrameRate_4_Hz_Byte0		0x8a4c
#define HDR_Status_f_FrameRate_4_Hz_Byte1		0x8a4d
#define HDR_Status_f_FrameRate_4_Hz_Byte2		0x8a4e
#define HDR_Status_f_FrameRate_4_Hz_Byte3		0x8a4f
#define HDR_Status_u32_TargetExposureTime_1_us_Byte0		0x8a50
#define HDR_Status_u32_TargetExposureTime_1_us_Byte1		0x8a51
#define HDR_Status_u32_TargetExposureTime_1_us_Byte2		0x8a52
#define HDR_Status_u32_TargetExposureTime_1_us_Byte3		0x8a53
#define HDR_Status_u32_TargetExposureTime_2_us_Byte0		0x8a54
#define HDR_Status_u32_TargetExposureTime_2_us_Byte1		0x8a55
#define HDR_Status_u32_TargetExposureTime_2_us_Byte2		0x8a56
#define HDR_Status_u32_TargetExposureTime_2_us_Byte3		0x8a57
#define HDR_Status_u32_TargetExposureTime_3_us_Byte0		0x8a58
#define HDR_Status_u32_TargetExposureTime_3_us_Byte1		0x8a59
#define HDR_Status_u32_TargetExposureTime_3_us_Byte2		0x8a5a
#define HDR_Status_u32_TargetExposureTime_3_us_Byte3		0x8a5b
#define HDR_Status_u32_TargetExposureTime_4_us_Byte0		0x8a5c
#define HDR_Status_u32_TargetExposureTime_4_us_Byte1		0x8a5d
#define HDR_Status_u32_TargetExposureTime_4_us_Byte2		0x8a5e
#define HDR_Status_u32_TargetExposureTime_4_us_Byte3		0x8a5f
#define HDR_Status_u16_TargetAnalogGain_1_x256_Byte0		0x8a60
#define HDR_Status_u16_TargetAnalogGain_1_x256_Byte1		0x8a61
#define HDR_Status_u16_TargetAnalogGain_2_x256_Byte0		0x8a62
#define HDR_Status_u16_TargetAnalogGain_2_x256_Byte1		0x8a63
#define HDR_Status_u16_TargetAnalogGain_3_x256_Byte0		0x8a64
#define HDR_Status_u16_TargetAnalogGain_3_x256_Byte1		0x8a65
#define HDR_Status_u16_TargetAnalogGain_4_x256_Byte0		0x8a66
#define HDR_Status_u16_TargetAnalogGain_4_x256_Byte1		0x8a67
#define HDR_Status_e_Coin_Byte0		0x8a68

// page 'g_CE_GammaStatus[0]'

#define CE_GammaStatus_0_e_GammaCurve_Byte0		0x8a80
#define CE_GammaStatus_0_u8_GammaPixelInShift_Sharp_Byte0		0x8a81
#define CE_GammaStatus_0_u8_GammaPixelInShift_UnSharp_Byte0		0x8a82
#define CE_GammaStatus_0_e_Coin_Status_Byte0		0x8a83

// page 'g_CE_GammaStatus[1]'

#define CE_GammaStatus_1_e_GammaCurve_Byte0		0x8ac0
#define CE_GammaStatus_1_u8_GammaPixelInShift_Sharp_Byte0		0x8ac1
#define CE_GammaStatus_1_u8_GammaPixelInShift_UnSharp_Byte0		0x8ac2
#define CE_GammaStatus_1_e_Coin_Status_Byte0		0x8ac3

// page 'g_CE_GammaMemLutAddress[0]'

#define CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte0		0x8b00
#define CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte1		0x8b01
#define CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte2		0x8b02
#define CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte3		0x8b03
#define CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte0		0x8b04
#define CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte1		0x8b05
#define CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte2		0x8b06
#define CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte3		0x8b07
#define CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte0		0x8b08
#define CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte1		0x8b09
#define CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte2		0x8b0a
#define CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte3		0x8b0b
#define CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte0		0x8b0c
#define CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte1		0x8b0d
#define CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte2		0x8b0e
#define CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte3		0x8b0f
#define CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte0		0x8b10
#define CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte1		0x8b11
#define CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte2		0x8b12
#define CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte3		0x8b13
#define CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte0		0x8b14
#define CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte1		0x8b15
#define CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte2		0x8b16
#define CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte3		0x8b17

// page 'g_CE_GammaMemLutAddress[1]'

#define CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte0		0x8b40
#define CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte1		0x8b41
#define CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte2		0x8b42
#define CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte3		0x8b43
#define CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte0		0x8b44
#define CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte1		0x8b45
#define CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte2		0x8b46
#define CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte3		0x8b47
#define CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte0		0x8b48
#define CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte1		0x8b49
#define CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte2		0x8b4a
#define CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte3		0x8b4b
#define CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte0		0x8b4c
#define CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte1		0x8b4d
#define CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte2		0x8b4e
#define CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte3		0x8b4f
#define CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte0		0x8b50
#define CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte1		0x8b51
#define CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte2		0x8b52
#define CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte3		0x8b53
#define CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte0		0x8b54
#define CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte1		0x8b55
#define CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte2		0x8b56
#define CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte3		0x8b57

// page 'g_BML_Framedimension'

#define BML_Framedimension_u16_BMLXOffset_Byte0		0x8b80
#define BML_Framedimension_u16_BMLXOffset_Byte1		0x8b81
#define BML_Framedimension_u16_BMLYOffset_Byte0		0x8b82
#define BML_Framedimension_u16_BMLYOffset_Byte1		0x8b83
#define BML_Framedimension_u16_BMLXSize_Byte0		0x8b84
#define BML_Framedimension_u16_BMLXSize_Byte1		0x8b85
#define BML_Framedimension_u16_BMLYSize_Byte0		0x8b86
#define BML_Framedimension_u16_BMLYSize_Byte1		0x8b87
#endif	// _EXTENSION_H_
