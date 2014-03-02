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

#ifndef _BASELINE_H_ 
#define _BASELINE_H_
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
	AFStats_Error_e_AF_ERROR_OK = 0,
	AFStats_Error_e_AF_WARNING_HOST_ZONE_HEIGHT_WIDTH_LOW = 1,
	AFStats_Error_e_AF_ERROR_HOST_ADDRESS_NOT_SPECIFIED_FOR_STATS_EXPORTING = 2,
	AFStats_Error_e_AF_ERROR_WOI_INVALID = 8,
	AFStats_Error_e_AF_ERROR_WINDOWS_SYSTEM_NOT_VALID = 9,
	AFStats_Error_e_AF_ERROR_WINDOWSZONE_TOOMUCHLITTLE = 10,
	AFStats_Error_e_AF_ERROR_RATIO_NOTVALID = 11,
	AFStats_Error_e_AF_ERROR_HOST_NO_ZONE_ENABLED = 12,
	AFStats_Error_e_AF_ERROR_HOST_ZONES_OVERLAPPED = 13,
	AFStats_Error_e_AF_ERROR_HOST_ZONES_INVALID_PARAMETERS = 14,
	AFStats_Error_e_AF_ERROR_HOST_ZONES_PARAMETERS_OUT_OF_BOUNDARY = 15,
	AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_EXCEEDED_100 = 16,
	AFStats_Error_e_AF_ERROR_HOST_PER_VALUE_NEGATIVE = 17,
	AFStats_Error_e_AF_ERROR_HOST_ZONE_SIZE_EXCEEDED_1M_PIX = 18,
	AFStats_Error_e_AF_ERROR_HOST_ENTERED_PARAMETER_NOT_EQUAL_TO_PROGRAMMED_PARAMETER = 19,
} AFStats_Error_te;

typedef enum
{
	AFStats_HostCmd_e_NO_REQ = 0,
	AFStats_HostCmd_e_REQ_STATS_ONCE = 1,
	AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_VALID = 2,
	AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_WITHOUT_VALID_CHECK = 3,
} AFStats_HostCmd_te;

typedef enum
{
	AFStats_StatusHostCmd_e_NO_REQ = 0,
	AFStats_StatusHostCmd_e_STATS_COPY_DONE = 1,
	AFStats_StatusHostCmd_e_STATS_CONTINUOUS_AND_VALID = 2,
	AFStats_StatusHostCmd_e_STATS_CONTINUOUS_AND_WITHOUT_VALID_CHECK = 3,
	AFStats_StatusHostCmd_e_STATS_COPY_NOT_YET_DONE = 4,
	AFStats_StatusHostCmd_e_STATS_CANCELLED = 5,
} AFStats_StatusHostCmd_te;

typedef enum
{
	AFStats_WindowsSystem_e_AF_HOST_SYSTEM = 0,
	AFStats_WindowsSystem_e_AF_PRESET_DEFAULT_7_ZONES_SYSTEM = 1,
} AFStats_WindowsSystem_te;

typedef enum
{
	BabylonMode_e_Manual = 0,
	BabylonMode_e_Adaptive = 1,
} BabylonMode_te;

typedef enum
{
	BayerStore2Source_e_DusterOutput = 0,
	BayerStore2Source_e_BayerCrop = 1,
} BayerStore2Source_te;

typedef enum
{
	BinningRepairMode_e_Auto = 0,
	BinningRepairMode_e_Custom = 1,
} BinningRepairMode_te;

typedef enum
{
	BlackAndWhiteControl_Enable = 0,
	BlackAndWhiteControl_Disable = 1,
} BlackAndWhiteControl_te;

typedef enum
{
	CCI_EERROR_MGMT_e_No_Error = 0,
	CCI_EERROR_MGMT_e_device_id_lo_error = 1,
	CCI_EERROR_MGMT_e_device_id_hi_error = 2,
	CCI_EERROR_MGMT_e_index_hi_error = 3,
	CCI_EERROR_MGMT_e_index_lo_error = 4,
	CCI_EERROR_MGMT_e_data_error = 5,
} CCI_EERROR_MGMT_te;

typedef enum
{
	Coin_e_Heads = 0,
	Coin_e_Tails = 1,
} Coin_te;

typedef enum
{
	DMADataAccessAlignment_e_2Byte = 0,
	DMADataAccessAlignment_e_4Byte = 1,
	DMADataAccessAlignment_e_8Byte = 2,
} DMADataAccessAlignment_te;

typedef enum
{
	DMADataBitsInMemory_10 = 0,
	DMADataBitsInMemory_12 = 1,
	DMADataBitsInMemory_16 = 2,
	DMADataBitsInMemory_8 = 3,
} DMADataBitsInMemory_te;

typedef enum
{
	DeviceAddress_e_10BitDeviceAddress = 0,
	DeviceAddress_e_7BitDeviceAddress = 1,
} DeviceAddress_te;

typedef enum
{
	DeviceIndex_e_16BitDataIndex = 0,
	DeviceIndex_e_8BitDataIndex = 1,
} DeviceIndex_te;

typedef enum
{
	ExportStatus_e_COMPLETE = 0,
	ExportStatus_e_INCOMPLETE = 1,
} ExportStatus_te;

typedef enum
{
	FDSensorScalingMode_e_SENSOR_SCALING_NONE = 0,
	FDSensorScalingMode_e_SENSOR_SCALING_HORIZONTAL_ONLY = 1,
	FDSensorScalingMode_e_SENSOR_SCALING_HORIZONTAL_AND_VERTICAL = 2,
} FDSensorScalingMode_te;

typedef enum
{
	FLADriver_APIError_e_NO_ERROR = 0,
	FLADriver_APIError_e_TIMER_EXPIRE_BEFORE_REACHING_POS = 1,
	FLADriver_APIError_e_LENS_MOVED_TO_TARGET = 2,
	FLADriver_APIError_e_LENS_POSITION_MEASURED = 3,
	FLADriver_APIError_e_LENS_STOPPED = 4,
	FLADriver_APIError_e_CANT_MEASURE_LENS_POS = 5,
	FLADriver_APIError_e_LENS_MOVE_API_FAILURE = 6,
	FLADriver_APIError_e_LENS_MOVE_REACHED_FRAME_TOLERANCE = 7,
} FLADriver_APIError_te;

typedef enum
{
	FLADriver_ActuatorOrientation_e_DIRECTION_ALIGNED_TO_STANDARDS = 0,
	FLADriver_ActuatorOrientation_e_DIRECTION_REVERSED_WRT_TO_STANDARDS = 1,
} FLADriver_ActuatorOrientation_te;

typedef enum
{
	FLADriver_RangeDef_e_NVM_LEVEL_RANGE = 0,
	FLADriver_RangeDef_e_HOST_DEFINED_RANGE = 1,
} FLADriver_RangeDef_te;

typedef enum
{
	FLADriver_TimerId_e_TIMER_2_NOT_STARTED = 0,
	FLADriver_TimerId_e_TIMER_2_STARTED = 1,
	FLADriver_TimerId_e_TIMER_2_EXPIRED = 2,
	FLADriver_TimerId_e_TIMER_2_WAITING_FOR_STOP = 3,
	FLADriver_TimerId_e_TIMER_2_STOPPED = 4,
} FLADriver_Timer2Id_te;

typedef enum
{
	Flag_e_FALSE = 0,
	Flag_e_TRUE = 1,
} Flag_te;

typedef enum
{
	FocusControl_FocusMsg_e_NO_ERROR = 0,
	FocusControl_FocusMsg_e_NO_ACTUATOR_LENS = 1,
	FocusControl_FocusMsg_e_NO_NVM_DATA_DEFAULT_ASSIGNED = 2,
	FocusControl_FocusMsg_e_NO_HOST_DATA_NVM_ASSIGNED = 3,
	FocusControl_FocusMsg_e_NO_HOST_DATA_DEFAULT_ASSIGNED = 4,
} FocusControl_FocusMsg_te;

typedef enum
{
	FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY = 0,
	FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO = 1,
	FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END = 2,
	FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END = 3,
	FocusControl_LensCommand_e_LA_CMD_GOTO_REST = 4,
	FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION = 5,
	FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR = 6,
	FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR = 7,
	FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL = 8,
} FocusControl_LensCommand_te;

typedef enum
{
	FrameDimensionProgMode_e_Manual = 0,
	FrameDimensionProgMode_e_Auto = 1,
} FrameDimensionProgMode_te;

typedef enum
{
	FrameRateMode_e_SINGLE_STEP = 0,
	FrameRateMode_e_CONTINUOUS = 1,
} FrameRateMode_te;

typedef enum
{
	GammaCurve_Standard = 0,
	GammaCurve_Custom = 1,
	GammaCurve_Disable = 2,
} GammaCurve_te;

typedef enum
{
	GlaceDataSource_e_PostLensShading = 0,
	GlaceDataSource_e_PostChannelGain = 1,
} GlaceDataSource_te;

typedef enum
{
	GlaceExportStatus_e_Idle = 0,
	GlaceExportStatus_e_Busy = 1,
} GlaceExportStatus_te;

typedef enum
{
	GlaceOperationMode_e_Disable = 0,
	GlaceOperationMode_e_Continuous = 1,
	GlaceOperationMode_e_Once = 2,
} GlaceOperationMode_te;

typedef enum
{
	GrabMode_e_NORMAL = 0,
	GrabMode_e_FORCE_OK = 1,
	GrabMode_e_FORCE_NOK = 2,
} GrabMode_te;

typedef enum
{
	HistCmd_e_IDLE = 0,
	HistCmd_e_GRAB = 1,
	HistCmd_e_STILL = 2,
} HistCmd_te;

typedef enum
{
	HistInputSrc_e_PostLensShading = 0,
	HistInputSrc_e_PostChannelGains = 1,
} HistInputSrc_te;

typedef enum
{
	HistogramMode_e_IDLE = 0,
	HistogramMode_e_CONTINUOUS = 1,
	HistogramMode_e_ONCE = 2,
} HistogramMode_te;

typedef enum
{
	HostComms_Status_e_None = 0,
	HostComms_Status_e_ReadComplete = 1,
	HostComms_Status_e_WriteComplete = 2,
	HostComms_Status_e_ReadOnlyWriteDenied = 3,
	HostComms_Status_e_InvalidAccess = 4,
} HostComms_Status_te;

typedef enum
{
	HostInterfaceCommand_e_INIT_COMMAND = 0,
	HostInterfaceCommand_e_BOOT = 1,
	HostInterfaceCommand_e_RUN = 2,
	HostInterfaceCommand_e_STOP = 3,
	HostInterfaceCommand_e_STOP_ISP = 4,
	HostInterfaceCommand_e_AUTO_STOP = 5,
	HostInterfaceCommand_e_SLEEP = 6,
	HostInterfaceCommand_e_WAKEUP = 7,
	HostInterfaceCommand_e_SWITCH_SENSOR = 8,
	HostInterfaceCommand_e_RESET_ISP = 9,
} HostInterfaceCommand_te;

typedef enum
{
	HostInterfaceHighLevelState_e_INIT = 0,
	HostInterfaceHighLevelState_e_STOPPED = 16,
	HostInterfaceHighLevelState_e_SETUP = 32,
	HostInterfaceHighLevelState_e_RUNNING = 48,
	HostInterfaceHighLevelState_e_SLEPT = 64,
	HostInterfaceHighLevelState_e_ERROR = 80,
} HostInterfaceHighLevelState_te;

typedef enum
{
	HostInterfaceLowLevelState_e_RAW_UNINITIALISED = 0,
	HostInterfaceLowLevelState_e_BOOTING = 1,
	HostInterfaceLowLevelState_e_STOPPED = 16,
	HostInterfaceLowLevelState_e_RXSTOPPED = 19,
	HostInterfaceLowLevelState_e_DATA_PATH_SETUP = 32,
	HostInterfaceLowLevelState_e_ALGORITHM_UPDATE = 33,
	HostInterfaceLowLevelState_e_SENSOR_COMMIT = 34,
	HostInterfaceLowLevelState_e_ISP_SETUP = 35,
	HostInterfaceLowLevelState_e_COMMANDING_RUN = 36,
	HostInterfaceLowLevelState_e_WAITING_FOR_RUN = 37,
	HostInterfaceLowLevelState_e_RUNNING = 48,
	HostInterfaceLowLevelState_e_COMMANDING_STOP = 49,
	HostInterfaceLowLevelState_e_WAITING_FOR_STOP = 50,
	HostInterfaceLowLevelState_e_GOING_TO_SLEEP = 17,
	HostInterfaceLowLevelState_e_RESET_ISP = 18,
	HostInterfaceLowLevelState_e_SLEPT = 64,
	HostInterfaceLowLevelState_e_WAKING_UP = 65,
	HostInterfaceLowLevelState_e_ENTERING_ERROR = 80,
	HostInterfaceLowLevelState_e_ERROR = 81,
} HostInterfaceLowLevelState_te;

typedef enum
{
	HostInterfaceRunSubCommand_e_COLD_START = 0,
	HostInterfaceRunSubCommand_e_HALF_BUTTON_PRESS = 1,
	HostInterfaceRunSubCommand_e_HALF_BUTTON_RELEASE = 2,
	HostInterfaceRunSubCommand_e_FULL_BUTTON_PRESS = 3,
	HostInterfaceRunSubCommand_e_FULL_BUTTON_RELEASE = 4,
} HostInterfaceRunSubCommand_te;

typedef enum
{
	HostToMasterI2CRequest_e_ReadNBytes = 0,
	HostToMasterI2CRequest_e_WriteNBytes = 1,
} HostToMasterI2CRequest_te;

typedef enum
{
	IdleMode_e_IDLE_MODE_0 = 0,
	IdleMode_e_IDLE_MODE_1 = 1,
	IdleMode_e_IDLE_MODE_2 = 2,
	IdleMode_e_IDLE_MODE_3 = 3,
	IdleMode_e_IDLE_MODE_NONE = 255,
} IdleMode_te;

typedef enum
{
	InputImageInterface_CSI2_0 = 0,
	InputImageInterface_CSI2_1 = 1,
	InputImageInterface_CCP = 2,
} InputImageInterface_te;

typedef enum
{
	InputImageSource_e_Sensor0 = 0,
	InputImageSource_e_Sensor1 = 1,
	InputImageSource_e_Rx = 2,
	InputImageSource_e_BayerLoad1 = 3,
	InputImageSource_e_BayerLoad2 = 4,
	InputImageSource_e_RGBLoad = 5,
} InputImageSource_te;

typedef enum
{
	IntegrationCapability_e_COARSE_ONLY = 0,
	IntegrationCapability_e_COARSE_AND_FINE = 1,
} IntegrationCapability_te;

typedef enum
{
	Master_e_Pipe0 = 0,
	Master_e_Pipe1 = 1,
} Master_te;

typedef enum
{
	OutputFormat_e_RGB101010_PEAKED = 16,
	OutputFormat_e_RGB101010_UNPEAKED = 0,
	OutputFormat_e_RGB888_PEAKED = 17,
	OutputFormat_e_RGB888_UNPEAKED = 1,
	OutputFormat_e_RGB565_PEAKED = 18,
	OutputFormat_e_RGB565_UNPEAKED = 2,
	OutputFormat_e_RGB555_PEAKED = 19,
	OutputFormat_e_RGB555_UNPEAKED = 3,
	OutputFormat_e_RGB444_UNPACKED_PEAKED = 20,
	OutputFormat_e_RGB444_UNPACKED_UNPEAKED = 4,
	OutputFormat_e_RGB444_PACKED_PEAKED = 52,
	OutputFormat_e_RGB444_PACKED_UNPEAKED = 36,
	OutputFormat_e_YUV = 5,
} OutputFormat_te;

typedef enum
{
	PixelOrder_e_GrR_BGb = 0,
	PixelOrder_e_RGr_GbB = 1,
	PixelOrder_e_BGb_GrR = 2,
	PixelOrder_e_GbB_RGr = 3,
} PixelOrder_te;

typedef enum
{
	RGBStoreSource_e_BabylonOutput = 0,
	RGBStoreSource_e_ChannelMergeOutput = 1,
	RGBStoreSource_e_MozartOutput = 2,
} RGBStoreSource_te;

typedef enum
{
	RSO_Mode_e_Manual = 0,
	RSO_Mode_e_Adaptive = 1,
} RSO_Mode_te;

typedef enum
{
	Resource_e_ResourceAvailable = 0,
	Resource_e_ResourceBusy = 1,
} Resource_te;

typedef enum
{
	Result_e_Failure = 0,
	Result_e_Success = 1,
} Result_te;

typedef enum
{
	RxTestPattern_e_Normal = 0,
	RxTestPattern_e_ColourBars = 1,
	RxTestPattern_e_GraduatedColourBars = 2,
	RxTestPattern_e_DiagonalGreyscale = 3,
	RxTestPattern_e_PsuedoRandom = 4,
	RxTestPattern_e_HorizontalGreyscale = 5,
	RxTestPattern_e_VerticalGreyscale = 6,
	RxTestPattern_e_SolidColour = 7,
} RxTestPattern_te;

typedef enum
{
	SDL_Disable = 0,
	SDL_Linear = 1,
	SDL_Custom = 2,
} SDLMode_te;

typedef enum
{
	SFXNegativeControl_Enable = 0,
	SFXNegativeControl_Disable = 1,
} SFXNegativeControl_te;

typedef enum
{
	SFXSolariseControl_Enable = 0,
	SFXSolariseControl_Disable = 1,
} SFXSolarisControl_te;

typedef enum
{
	ScorpioMode_e_Manual = 0,
	ScorpioMode_e_Adaptive = 1,
} ScorpioMode_te;

typedef enum
{
	SensorBitsPerSystemClock_e_DATA_CLOCK = 1,
	SensorBitsPerSystemClock_e_DATA_STROBE = 2,
} SensorBitsPerSystemClock_te;

typedef enum
{
	SensorCSI2Version_0_81 = 0,
	SensorCSI2Version_0_90 = 1,
} SensorCSI2Version_te;

typedef enum
{
	SensorPrescaleType_e_SensorSubsample = 0,
	SensorPrescaleType_e_SensorScale = 1,
} SensorPrescaleType_te;

typedef enum
{
	SensorProfile_e_Profile0 = 0,
	SensorProfile_e_Profile1 = 1,
	SensorProfile_e_Profile2 = 2,
} SensorProfile_te;

typedef enum
{
	SepiaControl_Enable = 0,
	SepiaControl_Disable = 1,
} SepiaControl_te;

typedef enum
{
	SiliconVersion_e_8500v1 = 0,
	SiliconVersion_e_8500v2 = 1,
	SiliconVersion_e_9540v1 = 2,
	SiliconVersion_e_8540v1 = 3,
} SiliconVersion_te;

typedef enum
{
	StatisticsFov_e_Sensor = 0,
	StatisticsFov_e_Master_Pipe = 1,
} StatisticsFov_te;

typedef enum
{
	StreamCommand_e_Start = 0,
	StreamCommand_e_Stop = 1,
} StreamCommand_te;

typedef enum
{
	StreamResourceStatus_e_Stopped = 0,
	StreamResourceStatus_e_Starting = 1,
	StreamResourceStatus_e_WaitingToStart = 2,
	StreamResourceStatus_e_Running = 3,
	StreamResourceStatus_e_Stopping = 4,
	StreamResourceStatus_e_WaitingToStop = 5,
} StreamResourceStatus_te;

typedef enum
{
	SwapColour_e_RedSwap = 0,
	SwapColour_e_GreenSwap = 1,
	SwapColour_e_BlueSwap = 2,
} SwapColour_te;

typedef enum
{
	SystemMeteringMode_e_CONTINUOUS = 0,
	SystemMeteringMode_e_SINGLE_STEP = 1,
} SystemMeteringMode_te;

typedef enum
{
	TestPattern_e_Normal = 0,
	TestPattern_e_SolidColour = 1,
	TestPattern_e_SolidColourBars = 2,
	TestPattern_e_SolidColourBarsFade = 3,
	TestPattern_e_PN9 = 4,
	TestPattern_e_NotSupported = 5,
} TestPattern_te;

typedef enum
{
	Transform_e_YCbCr_JFIF = 0,
	Transform_e_YCbCr_Rec601 = 1,
	Transform_e_YCbCr_Rec709_FULL_RANGE = 2,
	Transform_e_YCbCr_Rec709 = 3,
	Transform_e_YCbCr_Custom = 4,
	Transform_e_Rgb_Standard = 5,
	Transform_e_Rgb_Custom = 6,
} Transform_te;

typedef enum
{
	UpdateStatus_e_Idle = 0,
	UpdateStatus_e_Updating = 1,
	UpdateStatus_e_WaitingForParams = 2,
	UpdateStatus_e_ParamsReceived = 3,
} UpdateStatus_te;

typedef enum
{
	ZoomCmdStatus_e_OK = 0,
	ZoomCmdStatus_e_FullyZoomedOut = 1,
	ZoomCmdStatus_e_FullyZoomedIn = 2,
	ZoomCmdStatus_e_SetOutOfRange = 3,
} ZoomCmdStatus_te;

typedef enum
{
	ZoomCommand_e_None = 0,
	ZoomCommand_e_In = 1,
	ZoomCommand_e_Out = 2,
	ZoomCommand_e_SetCenter = 3,
	ZoomCommand_e_SetFOV = 4,
	ZoomCommand_e_Reset = 5,
	ZoomCommand_e_Stop = 6,
	ZoomCommand_e_RefreshOutputSize = 7,
} ZoomCommand_te;

typedef enum
{
	ZoomRequestLLDStatus_e_Accepted = 0,
	ZoomRequestLLDStatus_e_Truncated = 1,
	ZoomRequestLLDStatus_e_ReProgeamRequired = 2,
} ZoomRequestLLDStatus_te;

typedef enum
{
	ZoomRequestStatus_e_None = 0,
	ZoomRequestStatus_e_Accepted = 1,
	ZoomRequestStatus_e_Denied = 2,
	ZoomRequestStatus_e_ReProgramRequired = 3,
} ZoomRequestStatus_te;


/*****************************************************************************/
/************************ Data Type Definitions ******************************/
/*****************************************************************************/

/// Unsigned 8 bit data type
typedef unsigned char       isp_uint8_t;

/// Signed 8 bit data type
typedef signed char         isp_int8_t;

/// Signed 16 bit data type
typedef short               isp_int16_t;

/// Unsigned 16 bit data type
typedef unsigned short      isp_uint16_t;

/// Signed 32 bit data type
typedef int                 isp_int32_t;

/// Unsigned 32 bit data type
typedef unsigned int        isp_uint32_t;

/// Signed 64 bit data type
typedef long long           isp_int64_t;

/// Unsigned 64 bit data type
typedef unsigned long long  isp_uint64_t;

/// 32 bit IEEE 754 floating point data type
typedef float               isp_float_t;

/// Boolean data type. Occupies 8 bits.
typedef isp_uint8_t             isp_bool_t;


typedef struct
{
	isp_float_t	f_HostAFZoneStartX_PER_wrt_WOIWidth;
	isp_float_t	f_HostAFZoneStartY_PER_wrt_WOIHeight;
	isp_float_t	f_HostAFZoneEndX_PER_wrt_WOIWidth;
	isp_float_t	f_HostAFZoneEndY_PER_wrt_WOIHeight;
	isp_uint8_t	e_Flag_Enabled;
} AFStats_HostZoneConfigPercentage_ts;

typedef struct
{
	isp_uint32_t	u32_Focus;
	isp_uint32_t	u32_AFZoneStartX;
	isp_uint32_t	u32_AFZoneStartY;
	isp_uint32_t	u32_AFZoneEndX;
	isp_uint32_t	u32_AFZoneEndY;
	isp_uint32_t	u32_AFZonesWidth;
	isp_uint32_t	u32_AFZonesHeight;
	isp_uint32_t	u32_Light;
	isp_uint32_t	u32_WeightAssigned;
	isp_uint32_t	u32_Enabled;
} AFStats_HostZoneStatus_ts;

typedef struct
{
	isp_uint32_t	u32_TargetRedAddress;
	isp_uint32_t	u32_TargetGreenAddress;
	isp_uint32_t	u32_TargetBlueAddress;
	isp_uint32_t	u32_TargetSaturationAddress;
	isp_uint32_t	u32_HGridSize;
	isp_uint32_t	u32_VGridSize;
} Glace_Statistics_ts;


// *****************************************
//       Page Elements
// *****************************************
// *****************************************

// page 'g_DeviceParameters'

#define DeviceParameters_u32_DeviceId_Byte0		0x0000
#define DeviceParameters_u32_DeviceId_Byte1		0x0001
#define DeviceParameters_u32_DeviceId_Byte2		0x0002
#define DeviceParameters_u32_DeviceId_Byte3		0x0003
#define DeviceParameters_u32_FirmwareVersionMajor_Byte0		0x0004
#define DeviceParameters_u32_FirmwareVersionMajor_Byte1		0x0005
#define DeviceParameters_u32_FirmwareVersionMajor_Byte2		0x0006
#define DeviceParameters_u32_FirmwareVersionMajor_Byte3		0x0007
#define DeviceParameters_u32_FirmwareVersionMinor_Byte0		0x0008
#define DeviceParameters_u32_FirmwareVersionMinor_Byte1		0x0009
#define DeviceParameters_u32_FirmwareVersionMinor_Byte2		0x000a
#define DeviceParameters_u32_FirmwareVersionMinor_Byte3		0x000b
#define DeviceParameters_u32_FirmwareVersionMicro_Byte0		0x000c
#define DeviceParameters_u32_FirmwareVersionMicro_Byte1		0x000d
#define DeviceParameters_u32_FirmwareVersionMicro_Byte2		0x000e
#define DeviceParameters_u32_FirmwareVersionMicro_Byte3		0x000f
#define DeviceParameters_u32_LLA_Sensor_Byte0		0x0010
#define DeviceParameters_u32_LLA_Sensor_Byte1		0x0011
#define DeviceParameters_u32_LLA_Sensor_Byte2		0x0012
#define DeviceParameters_u32_LLA_Sensor_Byte3		0x0013
#define DeviceParameters_u32_LLA_MajorVersion_Byte0		0x0014
#define DeviceParameters_u32_LLA_MajorVersion_Byte1		0x0015
#define DeviceParameters_u32_LLA_MajorVersion_Byte2		0x0016
#define DeviceParameters_u32_LLA_MajorVersion_Byte3		0x0017
#define DeviceParameters_u32_LLA_MinorVersion_Byte0		0x0018
#define DeviceParameters_u32_LLA_MinorVersion_Byte1		0x0019
#define DeviceParameters_u32_LLA_MinorVersion_Byte2		0x001a
#define DeviceParameters_u32_LLA_MinorVersion_Byte3		0x001b
#define DeviceParameters_u32_LLCD_MajorVersion_Byte0		0x001c
#define DeviceParameters_u32_LLCD_MajorVersion_Byte1		0x001d
#define DeviceParameters_u32_LLCD_MajorVersion_Byte2		0x001e
#define DeviceParameters_u32_LLCD_MajorVersion_Byte3		0x001f
#define DeviceParameters_u32_LLCD_MinorVersion_Byte0		0x0020
#define DeviceParameters_u32_LLCD_MinorVersion_Byte1		0x0021
#define DeviceParameters_u32_LLCD_MinorVersion_Byte2		0x0022
#define DeviceParameters_u32_LLCD_MinorVersion_Byte3		0x0023
#define DeviceParameters_e_SiliconVersion_Byte0		0x0024

// page 'g_SensorInformation'

#define SensorInformation_u16_model_id_Byte0		0x0040
#define SensorInformation_u16_model_id_Byte1		0x0041
#define SensorInformation_u8_revision_number_Byte0		0x0042
#define SensorInformation_u8_manufacturer_id_Byte0		0x0043
#define SensorInformation_u8_smia_version_Byte0		0x0044
#define SensorInformation_e_Flag_Available_Byte0		0x0045
#define SensorInformation_u8_serial_id_0_Byte0		0x0046
#define SensorInformation_u8_serial_id_1_Byte0		0x0047
#define SensorInformation_u8_serial_id_2_Byte0		0x0048
#define SensorInformation_u8_serial_id_3_Byte0		0x0049
#define SensorInformation_u8_serial_id_4_Byte0		0x004a
#define SensorInformation_u8_serial_id_5_Byte0		0x004b

// page 'g_CRM_Status'

#define CRM_Status_u16_Window_Byte0		0x0080
#define CRM_Status_u16_Window_Byte1		0x0081
#define CRM_Status_u16_interrupt_0_CKC_OK_ITS_Byte0		0x0082
#define CRM_Status_u16_interrupt_0_CKC_OK_ITS_Byte1		0x0083
#define CRM_Status_u16_interrupt_0_CKC_LOST_ITS_Byte0		0x0084
#define CRM_Status_u16_interrupt_0_CKC_LOST_ITS_Byte1		0x0085
#define CRM_Status_u16_interrupt_0_CKC_OK_3D_ITS_Byte0		0x0086
#define CRM_Status_u16_interrupt_0_CKC_OK_3D_ITS_Byte1		0x0087
#define CRM_Status_u16_interrupt_0_CKC_LOST_3D_ITS_Byte0		0x0088
#define CRM_Status_u16_interrupt_0_CKC_LOST_3D_ITS_Byte1		0x0089
#define CRM_Status_u16_interrupt_0_CKC_OK_ITS_warning_Byte0		0x008a
#define CRM_Status_u16_interrupt_0_CKC_OK_ITS_warning_Byte1		0x008b
#define CRM_Status_u16_interrupt_0_CKC_LOST_ITS_warning_Byte0		0x008c
#define CRM_Status_u16_interrupt_0_CKC_LOST_ITS_warning_Byte1		0x008d
#define CRM_Status_e_Flag_StartEnabled_Byte0		0x008e
#define CRM_Status_e_Flag_StopEnabled_Byte0		0x008f
#define CRM_Status_e_Flag_SensorClocksAvailable_Byte0		0x0090

// page 'g_HostComms_Status'

#define HostComms_Status_u32_PageNumberAndOffset_Byte0		0x00c0
#define HostComms_Status_u32_PageNumberAndOffset_Byte1		0x00c1
#define HostComms_Status_u32_PageNumberAndOffset_Byte2		0x00c2
#define HostComms_Status_u32_PageNumberAndOffset_Byte3		0x00c3
#define HostComms_Status_e_HostComms_Status_Byte0		0x00c4
#define HostComms_Status_bo_ModeStaticSetupChanged_Byte0		0x00c5

// page 'g_HostInterface_Control'

#define HostInterface_Control_e_Flag_StepThroughStates_Byte0		0x0100
#define HostInterface_Control_e_Coin_StepThroughControl_Byte0		0x0101
#define HostInterface_Control_e_HostInterfaceCommand_User_Byte0		0x0102
#define HostInterface_Control_e_HostInterfaceRunSubCommand_Byte0		0x0103

// page 'g_HostInterface_Status'

#define HostInterface_Status_e_HostInterfaceLowLevelState_Current_Byte0		0x0140
#define HostInterface_Status_e_HostInterfaceHighLevelState_Byte0		0x0141
#define HostInterface_Status_e_HostInterfaceLowLevelState_Next_Byte0		0x0142
#define HostInterface_Status_e_Coin_StepThroughStatus_Byte0		0x0143

// page 'g_Stream_InputControl'

#define Stream_InputControl_e_StreamCommand_Sensor_Byte0		0x0180
#define Stream_InputControl_e_StreamCommand_Rx_Byte0		0x0181

// page 'g_Stream_InputStatus'

#define Stream_InputStatus_e_StreamResourceStatus_Sensor_Byte0		0x01c0
#define Stream_InputStatus_e_StreamResourceStatus_Rx_Byte0		0x01c1
#define Stream_InputStatus_e_Flag_RxStopRequested_Byte0		0x01c2

// page 'g_SystemSetup'

#define SystemSetup_f_SensorInputClockFreq_Mhz_Byte0		0x0200
#define SystemSetup_f_SensorInputClockFreq_Mhz_Byte1		0x0201
#define SystemSetup_f_SensorInputClockFreq_Mhz_Byte2		0x0202
#define SystemSetup_f_SensorInputClockFreq_Mhz_Byte3		0x0203
#define SystemSetup_f_MCUClockFreq_Mhz_Byte0		0x0204
#define SystemSetup_f_MCUClockFreq_Mhz_Byte1		0x0205
#define SystemSetup_f_MCUClockFreq_Mhz_Byte2		0x0206
#define SystemSetup_f_MCUClockFreq_Mhz_Byte3		0x0207
#define SystemSetup_f_ClockHost_Mhz_Byte0		0x0208
#define SystemSetup_f_ClockHost_Mhz_Byte1		0x0209
#define SystemSetup_f_ClockHost_Mhz_Byte2		0x020a
#define SystemSetup_f_ClockHost_Mhz_Byte3		0x020b
#define SystemSetup_f_SensorStartDelay_us_Byte0		0x020c
#define SystemSetup_f_SensorStartDelay_us_Byte1		0x020d
#define SystemSetup_f_SensorStartDelay_us_Byte2		0x020e
#define SystemSetup_f_SensorStartDelay_us_Byte3		0x020f
#define SystemSetup_u16_SensorDeviceID_Byte0		0x0210
#define SystemSetup_u16_SensorDeviceID_Byte1		0x0211
#define SystemSetup_u16_SensorXshutdownDelay_us_Byte0		0x0212
#define SystemSetup_u16_SensorXshutdownDelay_us_Byte1		0x0213
#define SystemSetup_e_DeviceAddress_Type_Byte0		0x0214
#define SystemSetup_e_DeviceIndex_Type_Byte0		0x0215
#define SystemSetup_e_InputImageSource_Byte0		0x0216
#define SystemSetup_e_InputImageInterface_Byte0		0x0217
#define SystemSetup_u8_NumberOfCSI2DataLines_Byte0		0x0218
#define SystemSetup_e_RxTestPattern_Byte0		0x0219
#define SystemSetup_e_Flag_PerformIPPSetup_Byte0		0x021a
#define SystemSetup_u8_FrameCount_Byte0		0x021b
#define SystemSetup_u8_LinesToWaitAtRxStop_Byte0		0x021c
#define SystemSetup_e_SystemMeteringMode_Control_Byte0		0x021d
#define SystemSetup_e_Flag_InhibitExpMetering_Byte0		0x021e
#define SystemSetup_e_Flag_InhibitWbMetering_Byte0		0x021f
#define SystemSetup_e_Coin_Ctrl_Byte0		0x0220
#define SystemSetup_e_Flag_mirror_Byte0		0x0221
#define SystemSetup_e_Flag_flip_Byte0		0x0222
#define SystemSetup_e_Coin_Glace_Histogram_Ctrl_debug_Byte0		0x0223
#define SystemSetup_e_IdleMode_Ctrl_Byte0		0x0224
#define SystemSetup_u8_crm_clk_pip_in_div_Byte0		0x0225
#define SystemSetup_e_Flag_abortRx_OnStop_Byte0		0x0226
#define SystemSetup_e_Flag_AecStatsCancel_Byte0		0x0227
#define SystemSetup_u8_NumOfFramesTobeSkipped_Byte0		0x0228
#define SystemSetup_e_GrabMode_Ctrl_Byte0		0x0229
#define SystemSetup_e_Flag_ZoomdStatsOnly_Byte0		0x022a
#define SystemSetup_e_Flag_FastBML_Byte0		0x022b
#define SystemSetup_e_Flag_Request_ValidFrameNotification_Ctrl_Byte0		0x022c

// page 'g_SystemConfig_Status'

#define SystemConfig_Status_e_Flag_InputPipeUpdated_Byte0		0x0240
#define SystemConfig_Status_e_Flag_ExposureOutOfSync_Byte0		0x0241
#define SystemConfig_Status_e_Coin_Status_Byte0		0x0242
#define SystemConfig_Status_e_Flag_ZoomUpdateDone_Byte0		0x0243
#define SystemConfig_Status_e_Flag_mirror_Byte0		0x0244
#define SystemConfig_Status_e_Flag_flip_Byte0		0x0245
#define SystemConfig_Status_e_Flag_UpdateNotificationPending_Byte0		0x0246
#define SystemConfig_Status_e_UpdateStatus_Exposure_Byte0		0x0247
#define SystemConfig_Status_e_UpdateStatus_FrameRate_Byte0		0x0248
#define SystemConfig_Status_e_UpdateStatus_Flash_Byte0		0x0249
#define SystemConfig_Status_e_Coin_Glace_Histogram_Status_Byte0		0x024a
#define SystemConfig_Status_e_PixelOrder_Byte0		0x024b
#define SystemConfig_Status_e_Flag_RunBestSensormode_Byte0		0x024c
#define SystemConfig_Status_e_Flag_FireDummyVidComplete0_Byte0		0x024d
#define SystemConfig_Status_e_Flag_FireDummyVidComplete1_Byte0		0x024e
#define SystemConfig_Status_e_RxTestPattern_Status_Byte0		0x024f
#define SystemConfig_Status_e_Flag_ZoomdStatsOnly_Byte0		0x0250
#define SystemConfig_Status_e_Flag_FastBML_Byte0		0x0251
#define SystemConfig_Status_e_GrabMode_Status_Byte0		0x0252
#define SystemConfig_Status_e_Flag_Request_ValidFrameNotification_Status_Byte0		0x0253

// page 'g_ColdStartControl'

#define ColdStartControl_e_Coin_WhiteBalance_Byte0		0x0280
#define ColdStartControl_e_Coin_Exposure_Byte0		0x0281
#define ColdStartControl_e_Coin_FrameRate_Byte0		0x0282

// page 'g_ColdStartStatus'

#define ColdStartStatus_e_Coin_WhiteBalance_Byte0		0x02c0
#define ColdStartStatus_e_Coin_Exposure_Byte0		0x02c1
#define ColdStartStatus_e_Coin_FrameRate_Byte0		0x02c2

// page 'g_DataPathControl'

#define DataPathControl_e_Flag_Pipe0Enable_Byte0		0x0300
#define DataPathControl_e_Flag_Pipe1Enable_Byte0		0x0301
#define DataPathControl_e_Flag_BayerStore0Enable_Byte0		0x0302
#define DataPathControl_e_Flag_BayerStore1Enable_Byte0		0x0303
#define DataPathControl_e_Flag_BayerStore2Enable_Byte0		0x0304
#define DataPathControl_e_BayerStore2Source_Byte0		0x0305
#define DataPathControl_e_Flag_RGBStoreEnable_Byte0		0x0306
#define DataPathControl_e_RGBStoreSource_Byte0		0x0307
#define DataPathControl_e_Flag_PipeRAWEnable_Byte0		0x0308
#define DataPathControl_e_Coin_PipeEnable_Byte0		0x0309

// page 'g_DataPathStatus'

#define DataPathStatus_e_Flag_Pipe0Enable_Byte0		0x0340
#define DataPathStatus_e_Flag_Pipe1Enable_Byte0		0x0341
#define DataPathStatus_e_Flag_BayerStore0Enable_Byte0		0x0342
#define DataPathStatus_e_Flag_BayerStore1Enable_Byte0		0x0343
#define DataPathStatus_e_Flag_BayerStore2Enable_Byte0		0x0344
#define DataPathStatus_e_BayerStore2Source_Byte0		0x0345
#define DataPathStatus_e_Flag_RGBStoreEnable_Byte0		0x0346
#define DataPathStatus_e_RGBStoreSource_Byte0		0x0347
#define DataPathStatus_e_Flag_PipeRAWEnable_Byte0		0x0348
#define DataPathStatus_e_Coin_PipeEnable_Byte0		0x0349

// page 'g_Pipe[0]'

#define Pipe_0_u16_X_size_Byte0		0x0380
#define Pipe_0_u16_X_size_Byte1		0x0381
#define Pipe_0_u16_Y_size_Byte0		0x0382
#define Pipe_0_u16_Y_size_Byte1		0x0383
#define Pipe_0_e_OutputFormat_Pipe_Byte0		0x0384
#define Pipe_0_e_Flag_TogglePixValid_Byte0		0x0385
#define Pipe_0_u8_PixValidLineTypes_Byte0		0x0386
#define Pipe_0_e_Flag_Flip_Cb_Cr_Byte0		0x0387
#define Pipe_0_e_Flag_Flip_Y_CbCr_Byte0		0x0388
#define Pipe_0_e_Flag_Valid_Byte0		0x0389

// page 'g_Pipe[1]'

#define Pipe_1_u16_X_size_Byte0		0x03c0
#define Pipe_1_u16_X_size_Byte1		0x03c1
#define Pipe_1_u16_Y_size_Byte0		0x03c2
#define Pipe_1_u16_Y_size_Byte1		0x03c3
#define Pipe_1_e_OutputFormat_Pipe_Byte0		0x03c4
#define Pipe_1_e_Flag_TogglePixValid_Byte0		0x03c5
#define Pipe_1_u8_PixValidLineTypes_Byte0		0x03c6
#define Pipe_1_e_Flag_Flip_Cb_Cr_Byte0		0x03c7
#define Pipe_1_e_Flag_Flip_Y_CbCr_Byte0		0x03c8
#define Pipe_1_e_Flag_Valid_Byte0		0x03c9

// page 'g_PipeStatus[0]'

#define PipeStatus_0_u8_FramesStreamedInPipeLastRun_Byte0		0x0400
#define PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0		0x0401
#define PipeStatus_0_e_Flag_VideoCompleteInterruptPending_Byte0		0x0402
#define PipeStatus_0_e_Flag_Exceed_HW_Limitation_Byte0		0x0403

// page 'g_PipeStatus[1]'

#define PipeStatus_1_u8_FramesStreamedInPipeLastRun_Byte0		0x0440
#define PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0		0x0441
#define PipeStatus_1_e_Flag_VideoCompleteInterruptPending_Byte0		0x0442
#define PipeStatus_1_e_Flag_Exceed_HW_Limitation_Byte0		0x0443

// page 'g_VideoTimingHostInputs'

#define VideoTimingHostInputs_f_HostRxMaxDataRate_Mbps_Byte0		0x0480
#define VideoTimingHostInputs_f_HostRxMaxDataRate_Mbps_Byte1		0x0481
#define VideoTimingHostInputs_f_HostRxMaxDataRate_Mbps_Byte2		0x0482
#define VideoTimingHostInputs_f_HostRxMaxDataRate_Mbps_Byte3		0x0483
#define VideoTimingHostInputs_e_Flag_reserve01_Byte0		0x0484
#define VideoTimingHostInputs_e_Flag_reserve01_Byte1		0x0485
#define VideoTimingHostInputs_e_Flag_reserve01_Byte2		0x0486
#define VideoTimingHostInputs_e_Flag_reserve01_Byte3		0x0487
#define VideoTimingHostInputs_u16_CsiRawFormat_Byte0		0x0488
#define VideoTimingHostInputs_u16_CsiRawFormat_Byte1		0x0489
#define VideoTimingHostInputs_e_Flag_reserve02_Byte0		0x048a
#define VideoTimingHostInputs_e_SensorBitsPerSystemClock_DataType_Byte0		0x048b
#define VideoTimingHostInputs_e_Flag_reserve03_Byte0		0x048c
#define VideoTimingHostInputs_e_Flag_reserve04_Byte0		0x048d
#define VideoTimingHostInputs_e_Flag_reserve05_Byte0		0x048e
#define VideoTimingHostInputs_e_Flag_reserve06_Byte0		0x048f

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_VideoTimingOutput'

#define VideoTimingOutput_f_reserve01_Byte0		0x0540
#define VideoTimingOutput_f_reserve01_Byte1		0x0541
#define VideoTimingOutput_f_reserve01_Byte2		0x0542
#define VideoTimingOutput_f_reserve01_Byte3		0x0543
#define VideoTimingOutput_f_reserve02_Byte0		0x0544
#define VideoTimingOutput_f_reserve02_Byte1		0x0545
#define VideoTimingOutput_f_reserve02_Byte2		0x0546
#define VideoTimingOutput_f_reserve02_Byte3		0x0547
#define VideoTimingOutput_f_VTSystemClockFrequency_Mhz_Byte0		0x0548
#define VideoTimingOutput_f_VTSystemClockFrequency_Mhz_Byte1		0x0549
#define VideoTimingOutput_f_VTSystemClockFrequency_Mhz_Byte2		0x054a
#define VideoTimingOutput_f_VTSystemClockFrequency_Mhz_Byte3		0x054b
#define VideoTimingOutput_f_VTPixelClockFrequency_Mhz_Byte0		0x054c
#define VideoTimingOutput_f_VTPixelClockFrequency_Mhz_Byte1		0x054d
#define VideoTimingOutput_f_VTPixelClockFrequency_Mhz_Byte2		0x054e
#define VideoTimingOutput_f_VTPixelClockFrequency_Mhz_Byte3		0x054f
#define VideoTimingOutput_f_reserve03_Byte0		0x0550
#define VideoTimingOutput_f_reserve03_Byte1		0x0551
#define VideoTimingOutput_f_reserve03_Byte2		0x0552
#define VideoTimingOutput_f_reserve03_Byte3		0x0553
#define VideoTimingOutput_f_reserve04_Byte0		0x0554
#define VideoTimingOutput_f_reserve04_Byte1		0x0555
#define VideoTimingOutput_f_reserve04_Byte2		0x0556
#define VideoTimingOutput_f_reserve04_Byte3		0x0557
#define VideoTimingOutput_f_OPPixelClockFrequency_Mhz_Byte0		0x0558
#define VideoTimingOutput_f_OPPixelClockFrequency_Mhz_Byte1		0x0559
#define VideoTimingOutput_f_OPPixelClockFrequency_Mhz_Byte2		0x055a
#define VideoTimingOutput_f_OPPixelClockFrequency_Mhz_Byte3		0x055b
#define VideoTimingOutput_f_OutputTimingClockDerating_Byte0		0x055c
#define VideoTimingOutput_f_OutputTimingClockDerating_Byte1		0x055d
#define VideoTimingOutput_f_OutputTimingClockDerating_Byte2		0x055e
#define VideoTimingOutput_f_OutputTimingClockDerating_Byte3		0x055f
#define VideoTimingOutput_f_reserve05_Byte0		0x0560
#define VideoTimingOutput_f_reserve05_Byte1		0x0561
#define VideoTimingOutput_f_reserve05_Byte2		0x0562
#define VideoTimingOutput_f_reserve05_Byte3		0x0563
#define VideoTimingOutput_u16_reserve06_Byte0		0x0564
#define VideoTimingOutput_u16_reserve06_Byte1		0x0565
#define VideoTimingOutput_u16_reserve07_Byte0		0x0566
#define VideoTimingOutput_u16_reserve07_Byte1		0x0567
#define VideoTimingOutput_u16_reserve08_Byte0		0x0568
#define VideoTimingOutput_u16_reserve08_Byte1		0x0569
#define VideoTimingOutput_u16_reserve09_Byte0		0x056a
#define VideoTimingOutput_u16_reserve09_Byte1		0x056b
#define VideoTimingOutput_u16_reserve10_Byte0		0x056c
#define VideoTimingOutput_u16_reserve10_Byte1		0x056d
#define VideoTimingOutput_u16_reserve11_Byte0		0x056e
#define VideoTimingOutput_u16_reserve11_Byte1		0x056f

// page 'g_SensorFrameConstraints'

#define SensorFrameConstraints_u16_VTXAddrMin_Byte0		0x0580
#define SensorFrameConstraints_u16_VTXAddrMin_Byte1		0x0581
#define SensorFrameConstraints_u16_VTYAddrMin_Byte0		0x0582
#define SensorFrameConstraints_u16_VTYAddrMin_Byte1		0x0583
#define SensorFrameConstraints_u16_VTXAddrMax_Byte0		0x0584
#define SensorFrameConstraints_u16_VTXAddrMax_Byte1		0x0585
#define SensorFrameConstraints_u16_VTYAddrMax_Byte0		0x0586
#define SensorFrameConstraints_u16_VTYAddrMax_Byte1		0x0587
#define SensorFrameConstraints_u16_MinOPXOutputSize_Byte0		0x0588
#define SensorFrameConstraints_u16_MinOPXOutputSize_Byte1		0x0589
#define SensorFrameConstraints_u16_MinOPYOutputSize_Byte0		0x058a
#define SensorFrameConstraints_u16_MinOPYOutputSize_Byte1		0x058b
#define SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0		0x058c
#define SensorFrameConstraints_u16_MaxOPXOutputSize_Byte1		0x058d
#define SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0		0x058e
#define SensorFrameConstraints_u16_MaxOPYOutputSize_Byte1		0x058f
#define SensorFrameConstraints_u16_MinVTFrameLengthLines_Byte0		0x0590
#define SensorFrameConstraints_u16_MinVTFrameLengthLines_Byte1		0x0591
#define SensorFrameConstraints_u16_MaxVTFrameLengthLines_Byte0		0x0592
#define SensorFrameConstraints_u16_MaxVTFrameLengthLines_Byte1		0x0593
#define SensorFrameConstraints_u16_MinVTLineLengthPck_Byte0		0x0594
#define SensorFrameConstraints_u16_MinVTLineLengthPck_Byte1		0x0595
#define SensorFrameConstraints_u16_MaxVTLineLengthPck_Byte0		0x0596
#define SensorFrameConstraints_u16_MaxVTLineLengthPck_Byte1		0x0597
#define SensorFrameConstraints_u16_MinVTLineBlankingPck_Byte0		0x0598
#define SensorFrameConstraints_u16_MinVTLineBlankingPck_Byte1		0x0599
#define SensorFrameConstraints_u16_MinVTFrameBlanking_Byte0		0x059a
#define SensorFrameConstraints_u16_MinVTFrameBlanking_Byte1		0x059b
#define SensorFrameConstraints_u16_ScalerMMin_Byte0		0x059c
#define SensorFrameConstraints_u16_ScalerMMin_Byte1		0x059d
#define SensorFrameConstraints_u16_ScalerMMax_Byte0		0x059e
#define SensorFrameConstraints_u16_ScalerMMax_Byte1		0x059f
#define SensorFrameConstraints_u16_MaxOddInc_Byte0		0x05a0
#define SensorFrameConstraints_u16_MaxOddInc_Byte1		0x05a1
#define SensorFrameConstraints_e_SensorProfile_Byte0		0x05a2

// page 'g_HostFrameConstraints'

#define HostFrameConstraints_f_FOVMargin_Byte0		0x05c0
#define HostFrameConstraints_f_FOVMargin_Byte1		0x05c1
#define HostFrameConstraints_f_FOVMargin_Byte2		0x05c2
#define HostFrameConstraints_f_FOVMargin_Byte3		0x05c3
#define HostFrameConstraints_u8_MinimumPostScalar0LineBlanking_pixels_Byte0		0x05c4
#define HostFrameConstraints_u8_MinimumPostScalar1LineBlanking_pixels_Byte0		0x05c5
#define HostFrameConstraints_u8_MinimumInterFrame_lines_Byte0		0x05c6
#define HostFrameConstraints_u8_MaximumPreScale_Byte0		0x05c7
#define HostFrameConstraints_u8_MainsFrequency_Hz_Byte0		0x05c8
#define HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0		0x05c9
#define HostFrameConstraints_e_FrameDimensionProgMode_Byte0		0x05ca
#define HostFrameConstraints_e_Flag_AllowChangeOver_Byte0		0x05cb

// page 'g_FrameDimensionStatus'

#define FrameDimensionStatus_f_VTLineLength_us_Byte0		0x0600
#define FrameDimensionStatus_f_VTLineLength_us_Byte1		0x0601
#define FrameDimensionStatus_f_VTLineLength_us_Byte2		0x0602
#define FrameDimensionStatus_f_VTLineLength_us_Byte3		0x0603
#define FrameDimensionStatus_f_VTFrameLength_us_Byte0		0x0604
#define FrameDimensionStatus_f_VTFrameLength_us_Byte1		0x0605
#define FrameDimensionStatus_f_VTFrameLength_us_Byte2		0x0606
#define FrameDimensionStatus_f_VTFrameLength_us_Byte3		0x0607
#define FrameDimensionStatus_f_CurrentFrameRate_Byte0		0x0608
#define FrameDimensionStatus_f_CurrentFrameRate_Byte1		0x0609
#define FrameDimensionStatus_f_CurrentFrameRate_Byte2		0x060a
#define FrameDimensionStatus_f_CurrentFrameRate_Byte3		0x060b
#define FrameDimensionStatus_u16_VTFrameLengthPending_Lines_Byte0		0x060c
#define FrameDimensionStatus_u16_VTFrameLengthPending_Lines_Byte1		0x060d
#define FrameDimensionStatus_u16_MinVTLineLengthAtCurrentVTXSize_Pixels_Byte0		0x060e
#define FrameDimensionStatus_u16_MinVTLineLengthAtCurrentVTXSize_Pixels_Byte1		0x060f
#define FrameDimensionStatus_u16_MinVTFrameLengthAtCurrentVTYSize_Lines_Byte0		0x0610
#define FrameDimensionStatus_u16_MinVTFrameLengthAtCurrentVTYSize_Lines_Byte1		0x0611
#define FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0		0x0612
#define FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte1		0x0613
#define FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0		0x0614
#define FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte1		0x0615
#define FrameDimensionStatus_u8_TotalFOVXShift_Byte0		0x0616
#define FrameDimensionStatus_u8_TotalFOVYShift_Byte0		0x0617
#define FrameDimensionStatus_u8_FOVXShiftInSensor_Byte0		0x0618
#define FrameDimensionStatus_u8_FOVYShiftInSensor_Byte0		0x0619
#define FrameDimensionStatus_e_Flag_IsFrameLengthChangePending_Byte0		0x061a
#define FrameDimensionStatus_e_Flag_IsFrameLengthChangeInhibitedForCoarseExposure_Byte0		0x061b

// page 'g_CurrentFrameDimension'

#define CurrentFrameDimension_f_PreScaleFactor_Byte0		0x0640
#define CurrentFrameDimension_f_PreScaleFactor_Byte1		0x0641
#define CurrentFrameDimension_f_PreScaleFactor_Byte2		0x0642
#define CurrentFrameDimension_f_PreScaleFactor_Byte3		0x0643
#define CurrentFrameDimension_u16_VTFrameLengthLines_Byte0		0x0644
#define CurrentFrameDimension_u16_VTFrameLengthLines_Byte1		0x0645
#define CurrentFrameDimension_u16_VTLineLengthPck_Byte0		0x0646
#define CurrentFrameDimension_u16_VTLineLengthPck_Byte1		0x0647
#define CurrentFrameDimension_u16_VTXAddrStart_Byte0		0x0648
#define CurrentFrameDimension_u16_VTXAddrStart_Byte1		0x0649
#define CurrentFrameDimension_u16_VTYAddrStart_Byte0		0x064a
#define CurrentFrameDimension_u16_VTYAddrStart_Byte1		0x064b
#define CurrentFrameDimension_u16_VTXAddrEnd_Byte0		0x064c
#define CurrentFrameDimension_u16_VTXAddrEnd_Byte1		0x064d
#define CurrentFrameDimension_u16_VTYAddrEnd_Byte0		0x064e
#define CurrentFrameDimension_u16_VTYAddrEnd_Byte1		0x064f
#define CurrentFrameDimension_u16_OPXOutputSize_Byte0		0x0650
#define CurrentFrameDimension_u16_OPXOutputSize_Byte1		0x0651
#define CurrentFrameDimension_u16_OPYOutputSize_Byte0		0x0652
#define CurrentFrameDimension_u16_OPYOutputSize_Byte1		0x0653
#define CurrentFrameDimension_u16_VTXOutputSize_Byte0		0x0654
#define CurrentFrameDimension_u16_VTXOutputSize_Byte1		0x0655
#define CurrentFrameDimension_u16_VTYOutputSize_Byte0		0x0656
#define CurrentFrameDimension_u16_VTYOutputSize_Byte1		0x0657
#define CurrentFrameDimension_u16_XOddInc_Byte0		0x0658
#define CurrentFrameDimension_u16_XOddInc_Byte1		0x0659
#define CurrentFrameDimension_u16_YOddInc_Byte0		0x065a
#define CurrentFrameDimension_u16_YOddInc_Byte1		0x065b
#define CurrentFrameDimension_u16_Scaler_M_Byte0		0x065c
#define CurrentFrameDimension_u16_Scaler_M_Byte1		0x065d
#define CurrentFrameDimension_u16_NumberOfNonActiveColumnsAtTheLeftEdge_Byte0		0x065e
#define CurrentFrameDimension_u16_NumberOfNonActiveColumnsAtTheLeftEdge_Byte1		0x065f
#define CurrentFrameDimension_u16_NumberOfNonActiveColumnsAtTheRightEdge_Byte0		0x0660
#define CurrentFrameDimension_u16_NumberOfNonActiveColumnsAtTheRightEdge_Byte1		0x0661
#define CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0		0x0662
#define CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte1		0x0663
#define CurrentFrameDimension_u16_NumberofNonActiveLinesAtBottomEdge_Byte0		0x0664
#define CurrentFrameDimension_u16_NumberofNonActiveLinesAtBottomEdge_Byte1		0x0665
#define CurrentFrameDimension_u8_NumberOfStatusLines_Byte0		0x0666
#define CurrentFrameDimension_e_SensorPrescaleType_Byte0		0x0667
#define CurrentFrameDimension_e_FDSensorScalingMode_Byte0		0x0668

// page 'g_RequestedFrameDimension'

#define RequestedFrameDimension_f_PreScaleFactor_Byte0		0x0680
#define RequestedFrameDimension_f_PreScaleFactor_Byte1		0x0681
#define RequestedFrameDimension_f_PreScaleFactor_Byte2		0x0682
#define RequestedFrameDimension_f_PreScaleFactor_Byte3		0x0683
#define RequestedFrameDimension_u16_VTFrameLengthLines_Byte0		0x0684
#define RequestedFrameDimension_u16_VTFrameLengthLines_Byte1		0x0685
#define RequestedFrameDimension_u16_VTLineLengthPck_Byte0		0x0686
#define RequestedFrameDimension_u16_VTLineLengthPck_Byte1		0x0687
#define RequestedFrameDimension_u16_VTXAddrStart_Byte0		0x0688
#define RequestedFrameDimension_u16_VTXAddrStart_Byte1		0x0689
#define RequestedFrameDimension_u16_VTYAddrStart_Byte0		0x068a
#define RequestedFrameDimension_u16_VTYAddrStart_Byte1		0x068b
#define RequestedFrameDimension_u16_VTXAddrEnd_Byte0		0x068c
#define RequestedFrameDimension_u16_VTXAddrEnd_Byte1		0x068d
#define RequestedFrameDimension_u16_VTYAddrEnd_Byte0		0x068e
#define RequestedFrameDimension_u16_VTYAddrEnd_Byte1		0x068f
#define RequestedFrameDimension_u16_OPXOutputSize_Byte0		0x0690
#define RequestedFrameDimension_u16_OPXOutputSize_Byte1		0x0691
#define RequestedFrameDimension_u16_OPYOutputSize_Byte0		0x0692
#define RequestedFrameDimension_u16_OPYOutputSize_Byte1		0x0693
#define RequestedFrameDimension_u16_VTXOutputSize_Byte0		0x0694
#define RequestedFrameDimension_u16_VTXOutputSize_Byte1		0x0695
#define RequestedFrameDimension_u16_VTYOutputSize_Byte0		0x0696
#define RequestedFrameDimension_u16_VTYOutputSize_Byte1		0x0697
#define RequestedFrameDimension_u16_XOddInc_Byte0		0x0698
#define RequestedFrameDimension_u16_XOddInc_Byte1		0x0699
#define RequestedFrameDimension_u16_YOddInc_Byte0		0x069a
#define RequestedFrameDimension_u16_YOddInc_Byte1		0x069b
#define RequestedFrameDimension_u16_Scaler_M_Byte0		0x069c
#define RequestedFrameDimension_u16_Scaler_M_Byte1		0x069d
#define RequestedFrameDimension_u16_NumberOfNonActiveColumnsAtTheLeftEdge_Byte0		0x069e
#define RequestedFrameDimension_u16_NumberOfNonActiveColumnsAtTheLeftEdge_Byte1		0x069f
#define RequestedFrameDimension_u16_NumberOfNonActiveColumnsAtTheRightEdge_Byte0		0x06a0
#define RequestedFrameDimension_u16_NumberOfNonActiveColumnsAtTheRightEdge_Byte1		0x06a1
#define RequestedFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0		0x06a2
#define RequestedFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte1		0x06a3
#define RequestedFrameDimension_u16_NumberofNonActiveLinesAtBottomEdge_Byte0		0x06a4
#define RequestedFrameDimension_u16_NumberofNonActiveLinesAtBottomEdge_Byte1		0x06a5
#define RequestedFrameDimension_u8_NumberOfStatusLines_Byte0		0x06a6
#define RequestedFrameDimension_e_SensorPrescaleType_Byte0		0x06a7
#define RequestedFrameDimension_e_FDSensorScalingMode_Byte0		0x06a8

// page 'g_AntiFlicker_Status'

#define AntiFlicker_Status_f_FlickerFreePeriod_us_Byte0		0x06c0
#define AntiFlicker_Status_f_FlickerFreePeriod_us_Byte1		0x06c1
#define AntiFlicker_Status_f_FlickerFreePeriod_us_Byte2		0x06c2
#define AntiFlicker_Status_f_FlickerFreePeriod_us_Byte3		0x06c3
#define AntiFlicker_Status_f_GainedFlickerFreeTimePeriod_us_Byte0		0x06c4
#define AntiFlicker_Status_f_GainedFlickerFreeTimePeriod_us_Byte1		0x06c5
#define AntiFlicker_Status_f_GainedFlickerFreeTimePeriod_us_Byte2		0x06c6
#define AntiFlicker_Status_f_GainedFlickerFreeTimePeriod_us_Byte3		0x06c7
#define AntiFlicker_Status_u16_MaxFlickerFreeBunches_Byte0		0x06c8
#define AntiFlicker_Status_u16_MaxFlickerFreeBunches_Byte1		0x06c9

// page 'g_FrameRateControl'

#define FrameRateControl_f_SystemGainThresholdLow_Byte0		0x0700
#define FrameRateControl_f_SystemGainThresholdLow_Byte1		0x0701
#define FrameRateControl_f_SystemGainThresholdLow_Byte2		0x0702
#define FrameRateControl_f_SystemGainThresholdLow_Byte3		0x0703
#define FrameRateControl_f_SystemGainThresholdHigh_Byte0		0x0704
#define FrameRateControl_f_SystemGainThresholdHigh_Byte1		0x0705
#define FrameRateControl_f_SystemGainThresholdHigh_Byte2		0x0706
#define FrameRateControl_f_SystemGainThresholdHigh_Byte3		0x0707
#define FrameRateControl_f_UserMinimumFrameRate_Hz_Byte0		0x0708
#define FrameRateControl_f_UserMinimumFrameRate_Hz_Byte1		0x0709
#define FrameRateControl_f_UserMinimumFrameRate_Hz_Byte2		0x070a
#define FrameRateControl_f_UserMinimumFrameRate_Hz_Byte3		0x070b
#define FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0		0x070c
#define FrameRateControl_f_UserMaximumFrameRate_Hz_Byte1		0x070d
#define FrameRateControl_f_UserMaximumFrameRate_Hz_Byte2		0x070e
#define FrameRateControl_f_UserMaximumFrameRate_Hz_Byte3		0x070f
#define FrameRateControl_f_ColdStartFrameRate_Byte0		0x0710
#define FrameRateControl_f_ColdStartFrameRate_Byte1		0x0711
#define FrameRateControl_f_ColdStartFrameRate_Byte2		0x0712
#define FrameRateControl_f_ColdStartFrameRate_Byte3		0x0713
#define FrameRateControl_f_RelativeChange_Byte0		0x0714
#define FrameRateControl_f_RelativeChange_Byte1		0x0715
#define FrameRateControl_f_RelativeChange_Byte2		0x0716
#define FrameRateControl_f_RelativeChange_Byte3		0x0717
#define FrameRateControl_e_Flag_LinkMinFrameRateAndExposureMaxUserIntegrationTime_Byte0		0x0718
#define FrameRateControl_e_FrameRateMode_Byte0		0x0719
#define FrameRateControl_e_Coin_FrameRateChange_Ctrl_Byte0		0x071a

// page 'g_FrameRateStatus'

#define FrameRateStatus_f_ImpliedGain_Byte0		0x0740
#define FrameRateStatus_f_ImpliedGain_Byte1		0x0741
#define FrameRateStatus_f_ImpliedGain_Byte2		0x0742
#define FrameRateStatus_f_ImpliedGain_Byte3		0x0743
#define FrameRateStatus_f_DesiredFrameRate_Hz_Byte0		0x0744
#define FrameRateStatus_f_DesiredFrameRate_Hz_Byte1		0x0745
#define FrameRateStatus_f_DesiredFrameRate_Hz_Byte2		0x0746
#define FrameRateStatus_f_DesiredFrameRate_Hz_Byte3		0x0747
#define FrameRateStatus_u16_MinimumFrameLength_lines_Byte0		0x0748
#define FrameRateStatus_u16_MinimumFrameLength_lines_Byte1		0x0749
#define FrameRateStatus_u16_MaximumFrameLength_lines_Byte0		0x074a
#define FrameRateStatus_u16_MaximumFrameLength_lines_Byte1		0x074b
#define FrameRateStatus_u16_FrameLengthChange_lines_Byte0		0x074c
#define FrameRateStatus_u16_FrameLengthChange_lines_Byte1		0x074d
#define FrameRateStatus_u16_DesiredFrameLength_lines_Byte0		0x074e
#define FrameRateStatus_u16_DesiredFrameLength_lines_Byte1		0x074f
#define FrameRateStatus_e_Coin_FrameRateChange_Status_Byte0		0x0750

// page 'g_Zoom_Params'

#define Zoom_Params_f_ZoomRange_Byte0		0x0780
#define Zoom_Params_f_ZoomRange_Byte1		0x0781
#define Zoom_Params_f_ZoomRange_Byte2		0x0782
#define Zoom_Params_f_ZoomRange_Byte3		0x0783
#define Zoom_Params_e_Master_Byte0		0x0784

// page 'g_Zoom_Control'

#define Zoom_Control_f_ZoomStep_Byte0		0x07c0
#define Zoom_Control_f_ZoomStep_Byte1		0x07c1
#define Zoom_Control_f_ZoomStep_Byte2		0x07c2
#define Zoom_Control_f_ZoomStep_Byte3		0x07c3
#define Zoom_Control_f_SetFOVX_Byte0		0x07c4
#define Zoom_Control_f_SetFOVX_Byte1		0x07c5
#define Zoom_Control_f_SetFOVX_Byte2		0x07c6
#define Zoom_Control_f_SetFOVX_Byte3		0x07c7
#define Zoom_Control_s16_CenterOffsetX_Byte0		0x07c8
#define Zoom_Control_s16_CenterOffsetX_Byte1		0x07c9
#define Zoom_Control_s16_CenterOffsetY_Byte0		0x07ca
#define Zoom_Control_s16_CenterOffsetY_Byte1		0x07cb
#define Zoom_Control_e_Flag_AutoZoom_Byte0		0x07cc
#define Zoom_Control_e_Flag_PerformDynamicResolutionUpdate_Byte0		0x07cd

// page 'g_Zoom_CommandControl'

#define Zoom_CommandControl_u8_CommandCount_Byte0		0x0800
#define Zoom_CommandControl_e_ZoomCommand_Byte0		0x0801

// page 'g_ZoomTop_ParamAppicationControl'

#define ZoomTop_ParamAppicationControl_u8_ScalarParamsGenerated_Byte0		0x0840
#define ZoomTop_ParamAppicationControl_u8_ScalarParamsApplied_Byte0		0x0841
#define ZoomTop_ParamAppicationControl_u8_RefreshOutputSizeControlCount_Byte0		0x0842
#define ZoomTop_ParamAppicationControl_u8_RefreshOutputSizeStatusCount_Byte0		0x0843

// page 'g_Zoom_Status'

#define Zoom_Status_f_XCenter_Byte0		0x0880
#define Zoom_Status_f_XCenter_Byte1		0x0881
#define Zoom_Status_f_XCenter_Byte2		0x0882
#define Zoom_Status_f_XCenter_Byte3		0x0883
#define Zoom_Status_f_YCenter_Byte0		0x0884
#define Zoom_Status_f_YCenter_Byte1		0x0885
#define Zoom_Status_f_YCenter_Byte2		0x0886
#define Zoom_Status_f_YCenter_Byte3		0x0887
#define Zoom_Status_f_MaxFOVXAvailableToDevice_Byte0		0x0888
#define Zoom_Status_f_MaxFOVXAvailableToDevice_Byte1		0x0889
#define Zoom_Status_f_MaxFOVXAvailableToDevice_Byte2		0x088a
#define Zoom_Status_f_MaxFOVXAvailableToDevice_Byte3		0x088b
#define Zoom_Status_f_MaxFOVYAvailableToDevice_Byte0		0x088c
#define Zoom_Status_f_MaxFOVYAvailableToDevice_Byte1		0x088d
#define Zoom_Status_f_MaxFOVYAvailableToDevice_Byte2		0x088e
#define Zoom_Status_f_MaxFOVYAvailableToDevice_Byte3		0x088f
#define Zoom_Status_f_MinFOVXAtArrayCenter_Byte0		0x0890
#define Zoom_Status_f_MinFOVXAtArrayCenter_Byte1		0x0891
#define Zoom_Status_f_MinFOVXAtArrayCenter_Byte2		0x0892
#define Zoom_Status_f_MinFOVXAtArrayCenter_Byte3		0x0893
#define Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0		0x0894
#define Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte1		0x0895
#define Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte2		0x0896
#define Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte3		0x0897
#define Zoom_Status_f_FOVX_Byte0		0x0898
#define Zoom_Status_f_FOVX_Byte1		0x0899
#define Zoom_Status_f_FOVX_Byte2		0x089a
#define Zoom_Status_f_FOVX_Byte3		0x089b
#define Zoom_Status_f_FOVY_Byte0		0x089c
#define Zoom_Status_f_FOVY_Byte1		0x089d
#define Zoom_Status_f_FOVY_Byte2		0x089e
#define Zoom_Status_f_FOVY_Byte3		0x089f
#define Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte0		0x08a0
#define Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte1		0x08a1
#define Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte0		0x08a2
#define Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte1		0x08a3
#define Zoom_Status_s16_CenterOffsetX_Byte0		0x08a4
#define Zoom_Status_s16_CenterOffsetX_Byte1		0x08a5
#define Zoom_Status_s16_CenterOffsetY_Byte0		0x08a6
#define Zoom_Status_s16_CenterOffsetY_Byte1		0x08a7
#define Zoom_Status_e_Flag_AutoZooming_Byte0		0x08a8
#define Zoom_Status_e_ZoomRequestLLDStatus_Byte0		0x08a9
#define Zoom_Status_u8_CenterShiftX_Byte0		0x08aa
#define Zoom_Status_u8_CenterShiftY_Byte0		0x08ab

// page 'g_Zoom_CommandStatus'

#define Zoom_CommandStatus_u8_CommandCount_Byte0		0x08c0
#define Zoom_CommandStatus_e_ZoomCommand_Byte0		0x08c1
#define Zoom_CommandStatus_e_ZoomCmdStatus_Byte0		0x08c2
#define Zoom_CommandStatus_e_ZoomRequestStatus_Byte0		0x08c3

// page 'g_PipeState[0]'

#define PipeState_0_f_AspectRatio_Byte0		0x0900
#define PipeState_0_f_AspectRatio_Byte1		0x0901
#define PipeState_0_f_AspectRatio_Byte2		0x0902
#define PipeState_0_f_AspectRatio_Byte3		0x0903
#define PipeState_0_f_FOVX_Byte0		0x0904
#define PipeState_0_f_FOVX_Byte1		0x0905
#define PipeState_0_f_FOVX_Byte2		0x0906
#define PipeState_0_f_FOVX_Byte3		0x0907
#define PipeState_0_f_FOVY_Byte0		0x0908
#define PipeState_0_f_FOVY_Byte1		0x0909
#define PipeState_0_f_FOVY_Byte2		0x090a
#define PipeState_0_f_FOVY_Byte3		0x090b
#define PipeState_0_f_Step_Byte0		0x090c
#define PipeState_0_f_Step_Byte1		0x090d
#define PipeState_0_f_Step_Byte2		0x090e
#define PipeState_0_f_Step_Byte3		0x090f
#define PipeState_0_u16_OutputSizeX_Byte0		0x0910
#define PipeState_0_u16_OutputSizeX_Byte1		0x0911
#define PipeState_0_u16_OutputSizeY_Byte0		0x0912
#define PipeState_0_u16_OutputSizeY_Byte1		0x0913
#define PipeState_0_e_Flag_Enabled_Byte0		0x0914

// page 'g_PipeState[1]'

#define PipeState_1_f_AspectRatio_Byte0		0x0940
#define PipeState_1_f_AspectRatio_Byte1		0x0941
#define PipeState_1_f_AspectRatio_Byte2		0x0942
#define PipeState_1_f_AspectRatio_Byte3		0x0943
#define PipeState_1_f_FOVX_Byte0		0x0944
#define PipeState_1_f_FOVX_Byte1		0x0945
#define PipeState_1_f_FOVX_Byte2		0x0946
#define PipeState_1_f_FOVX_Byte3		0x0947
#define PipeState_1_f_FOVY_Byte0		0x0948
#define PipeState_1_f_FOVY_Byte1		0x0949
#define PipeState_1_f_FOVY_Byte2		0x094a
#define PipeState_1_f_FOVY_Byte3		0x094b
#define PipeState_1_f_Step_Byte0		0x094c
#define PipeState_1_f_Step_Byte1		0x094d
#define PipeState_1_f_Step_Byte2		0x094e
#define PipeState_1_f_Step_Byte3		0x094f
#define PipeState_1_u16_OutputSizeX_Byte0		0x0950
#define PipeState_1_u16_OutputSizeX_Byte1		0x0951
#define PipeState_1_u16_OutputSizeY_Byte0		0x0952
#define PipeState_1_u16_OutputSizeY_Byte1		0x0953
#define PipeState_1_e_Flag_Enabled_Byte0		0x0954

// page 'g_Pipe_Scalar_Inputs[0]'

#define Pipe_Scalar_Inputs_0_f_Step_Byte0		0x0980
#define Pipe_Scalar_Inputs_0_f_Step_Byte1		0x0981
#define Pipe_Scalar_Inputs_0_f_Step_Byte2		0x0982
#define Pipe_Scalar_Inputs_0_f_Step_Byte3		0x0983
#define Pipe_Scalar_Inputs_0_f_HCrop_Byte0		0x0984
#define Pipe_Scalar_Inputs_0_f_HCrop_Byte1		0x0985
#define Pipe_Scalar_Inputs_0_f_HCrop_Byte2		0x0986
#define Pipe_Scalar_Inputs_0_f_HCrop_Byte3		0x0987
#define Pipe_Scalar_Inputs_0_f_VCrop_Byte0		0x0988
#define Pipe_Scalar_Inputs_0_f_VCrop_Byte1		0x0989
#define Pipe_Scalar_Inputs_0_f_VCrop_Byte2		0x098a
#define Pipe_Scalar_Inputs_0_f_VCrop_Byte3		0x098b
#define Pipe_Scalar_Inputs_0_u16_HCropperInputSize_Byte0		0x098c
#define Pipe_Scalar_Inputs_0_u16_HCropperInputSize_Byte1		0x098d
#define Pipe_Scalar_Inputs_0_u16_VCropperInputSize_Byte0		0x098e
#define Pipe_Scalar_Inputs_0_u16_VCropperInputSize_Byte1		0x098f
#define Pipe_Scalar_Inputs_0_u16_HScalarOutputSize_Byte0		0x0990
#define Pipe_Scalar_Inputs_0_u16_HScalarOutputSize_Byte1		0x0991
#define Pipe_Scalar_Inputs_0_u16_VScalarOutputSize_Byte0		0x0992
#define Pipe_Scalar_Inputs_0_u16_VScalarOutputSize_Byte1		0x0993

// page 'g_Pipe_Scalar_Inputs[1]'

#define Pipe_Scalar_Inputs_1_f_Step_Byte0		0x09c0
#define Pipe_Scalar_Inputs_1_f_Step_Byte1		0x09c1
#define Pipe_Scalar_Inputs_1_f_Step_Byte2		0x09c2
#define Pipe_Scalar_Inputs_1_f_Step_Byte3		0x09c3
#define Pipe_Scalar_Inputs_1_f_HCrop_Byte0		0x09c4
#define Pipe_Scalar_Inputs_1_f_HCrop_Byte1		0x09c5
#define Pipe_Scalar_Inputs_1_f_HCrop_Byte2		0x09c6
#define Pipe_Scalar_Inputs_1_f_HCrop_Byte3		0x09c7
#define Pipe_Scalar_Inputs_1_f_VCrop_Byte0		0x09c8
#define Pipe_Scalar_Inputs_1_f_VCrop_Byte1		0x09c9
#define Pipe_Scalar_Inputs_1_f_VCrop_Byte2		0x09ca
#define Pipe_Scalar_Inputs_1_f_VCrop_Byte3		0x09cb
#define Pipe_Scalar_Inputs_1_u16_HCropperInputSize_Byte0		0x09cc
#define Pipe_Scalar_Inputs_1_u16_HCropperInputSize_Byte1		0x09cd
#define Pipe_Scalar_Inputs_1_u16_VCropperInputSize_Byte0		0x09ce
#define Pipe_Scalar_Inputs_1_u16_VCropperInputSize_Byte1		0x09cf
#define Pipe_Scalar_Inputs_1_u16_HScalarOutputSize_Byte0		0x09d0
#define Pipe_Scalar_Inputs_1_u16_HScalarOutputSize_Byte1		0x09d1
#define Pipe_Scalar_Inputs_1_u16_VScalarOutputSize_Byte0		0x09d2
#define Pipe_Scalar_Inputs_1_u16_VScalarOutputSize_Byte1		0x09d3

// page 'g_Pipe_Scalar_Output[0]'

#define Pipe_Scalar_Output_0_f_HGPSCrop_Byte0		0x0a00
#define Pipe_Scalar_Output_0_f_HGPSCrop_Byte1		0x0a01
#define Pipe_Scalar_Output_0_f_HGPSCrop_Byte2		0x0a02
#define Pipe_Scalar_Output_0_f_HGPSCrop_Byte3		0x0a03
#define Pipe_Scalar_Output_0_f_VGPSCrop_Byte0		0x0a04
#define Pipe_Scalar_Output_0_f_VGPSCrop_Byte1		0x0a05
#define Pipe_Scalar_Output_0_f_VGPSCrop_Byte2		0x0a06
#define Pipe_Scalar_Output_0_f_VGPSCrop_Byte3		0x0a07
#define Pipe_Scalar_Output_0_ptrs16_Cof_Byte0		0x0a08
#define Pipe_Scalar_Output_0_ptrs16_Cof_Byte1		0x0a09
#define Pipe_Scalar_Output_0_ptrs16_Cof_Byte2		0x0a0a
#define Pipe_Scalar_Output_0_ptrs16_Cof_Byte3		0x0a0b
#define Pipe_Scalar_Output_0_u32_LBorderCopies_Byte0		0x0a0c
#define Pipe_Scalar_Output_0_u32_LBorderCopies_Byte1		0x0a0d
#define Pipe_Scalar_Output_0_u32_LBorderCopies_Byte2		0x0a0e
#define Pipe_Scalar_Output_0_u32_LBorderCopies_Byte3		0x0a0f
#define Pipe_Scalar_Output_0_u32_RBorderCopies_Byte0		0x0a10
#define Pipe_Scalar_Output_0_u32_RBorderCopies_Byte1		0x0a11
#define Pipe_Scalar_Output_0_u32_RBorderCopies_Byte2		0x0a12
#define Pipe_Scalar_Output_0_u32_RBorderCopies_Byte3		0x0a13
#define Pipe_Scalar_Output_0_u16_HCropStart_Byte0		0x0a14
#define Pipe_Scalar_Output_0_u16_HCropStart_Byte1		0x0a15
#define Pipe_Scalar_Output_0_u16_HCropSize_Byte0		0x0a16
#define Pipe_Scalar_Output_0_u16_HCropSize_Byte1		0x0a17
#define Pipe_Scalar_Output_0_u16_VCropStart_Byte0		0x0a18
#define Pipe_Scalar_Output_0_u16_VCropStart_Byte1		0x0a19
#define Pipe_Scalar_Output_0_u16_VCropSize_Byte0		0x0a1a
#define Pipe_Scalar_Output_0_u16_VCropSize_Byte1		0x0a1b
#define Pipe_Scalar_Output_0_u16_LeftBorder_Byte0		0x0a1c
#define Pipe_Scalar_Output_0_u16_LeftBorder_Byte1		0x0a1d
#define Pipe_Scalar_Output_0_u16_RightBorder_Byte0		0x0a1e
#define Pipe_Scalar_Output_0_u16_RightBorder_Byte1		0x0a1f
#define Pipe_Scalar_Output_0_u16_TopBorder_Byte0		0x0a20
#define Pipe_Scalar_Output_0_u16_TopBorder_Byte1		0x0a21
#define Pipe_Scalar_Output_0_u16_BottomBorder_Byte0		0x0a22
#define Pipe_Scalar_Output_0_u16_BottomBorder_Byte1		0x0a23
#define Pipe_Scalar_Output_0_u8_NSize_Byte0		0x0a24
#define Pipe_Scalar_Output_0_u8_LogMSize_Byte0		0x0a25
#define Pipe_Scalar_Output_0_u8_CofCount_Byte0		0x0a26
#define Pipe_Scalar_Output_0_e_Flag_ScalarEnable_Byte0		0x0a27
#define Pipe_Scalar_Output_0_e_Flag_DownScaling_Byte0		0x0a28

// page 'g_Pipe_Scalar_Output[1]'

#define Pipe_Scalar_Output_1_f_HGPSCrop_Byte0		0x0a40
#define Pipe_Scalar_Output_1_f_HGPSCrop_Byte1		0x0a41
#define Pipe_Scalar_Output_1_f_HGPSCrop_Byte2		0x0a42
#define Pipe_Scalar_Output_1_f_HGPSCrop_Byte3		0x0a43
#define Pipe_Scalar_Output_1_f_VGPSCrop_Byte0		0x0a44
#define Pipe_Scalar_Output_1_f_VGPSCrop_Byte1		0x0a45
#define Pipe_Scalar_Output_1_f_VGPSCrop_Byte2		0x0a46
#define Pipe_Scalar_Output_1_f_VGPSCrop_Byte3		0x0a47
#define Pipe_Scalar_Output_1_ptrs16_Cof_Byte0		0x0a48
#define Pipe_Scalar_Output_1_ptrs16_Cof_Byte1		0x0a49
#define Pipe_Scalar_Output_1_ptrs16_Cof_Byte2		0x0a4a
#define Pipe_Scalar_Output_1_ptrs16_Cof_Byte3		0x0a4b
#define Pipe_Scalar_Output_1_u32_LBorderCopies_Byte0		0x0a4c
#define Pipe_Scalar_Output_1_u32_LBorderCopies_Byte1		0x0a4d
#define Pipe_Scalar_Output_1_u32_LBorderCopies_Byte2		0x0a4e
#define Pipe_Scalar_Output_1_u32_LBorderCopies_Byte3		0x0a4f
#define Pipe_Scalar_Output_1_u32_RBorderCopies_Byte0		0x0a50
#define Pipe_Scalar_Output_1_u32_RBorderCopies_Byte1		0x0a51
#define Pipe_Scalar_Output_1_u32_RBorderCopies_Byte2		0x0a52
#define Pipe_Scalar_Output_1_u32_RBorderCopies_Byte3		0x0a53
#define Pipe_Scalar_Output_1_u16_HCropStart_Byte0		0x0a54
#define Pipe_Scalar_Output_1_u16_HCropStart_Byte1		0x0a55
#define Pipe_Scalar_Output_1_u16_HCropSize_Byte0		0x0a56
#define Pipe_Scalar_Output_1_u16_HCropSize_Byte1		0x0a57
#define Pipe_Scalar_Output_1_u16_VCropStart_Byte0		0x0a58
#define Pipe_Scalar_Output_1_u16_VCropStart_Byte1		0x0a59
#define Pipe_Scalar_Output_1_u16_VCropSize_Byte0		0x0a5a
#define Pipe_Scalar_Output_1_u16_VCropSize_Byte1		0x0a5b
#define Pipe_Scalar_Output_1_u16_LeftBorder_Byte0		0x0a5c
#define Pipe_Scalar_Output_1_u16_LeftBorder_Byte1		0x0a5d
#define Pipe_Scalar_Output_1_u16_RightBorder_Byte0		0x0a5e
#define Pipe_Scalar_Output_1_u16_RightBorder_Byte1		0x0a5f
#define Pipe_Scalar_Output_1_u16_TopBorder_Byte0		0x0a60
#define Pipe_Scalar_Output_1_u16_TopBorder_Byte1		0x0a61
#define Pipe_Scalar_Output_1_u16_BottomBorder_Byte0		0x0a62
#define Pipe_Scalar_Output_1_u16_BottomBorder_Byte1		0x0a63
#define Pipe_Scalar_Output_1_u8_NSize_Byte0		0x0a64
#define Pipe_Scalar_Output_1_u8_LogMSize_Byte0		0x0a65
#define Pipe_Scalar_Output_1_u8_CofCount_Byte0		0x0a66
#define Pipe_Scalar_Output_1_e_Flag_ScalarEnable_Byte0		0x0a67
#define Pipe_Scalar_Output_1_e_Flag_DownScaling_Byte0		0x0a68

// page 'g_MasterI2CClockControl'

#define MasterI2CClockControl_u8_CountFall_Byte0		0x0a80
#define MasterI2CClockControl_u8_CountRise_Byte0		0x0a81
#define MasterI2CClockControl_u8_CountHigh_Byte0		0x0a82
#define MasterI2CClockControl_u8_CountBuffer_Byte0		0x0a83
#define MasterI2CClockControl_u8_CountHoldData_Byte0		0x0a84
#define MasterI2CClockControl_u8_CountSetupData_Byte0		0x0a85
#define MasterI2CClockControl_u8_CountHoldStart_Byte0		0x0a86
#define MasterI2CClockControl_u8_CountSetupStart_Byte0		0x0a87
#define MasterI2CClockControl_u8_CountSetupStop_Byte0		0x0a88
#define MasterI2CClockControl_u8_MaximumNumberOfGrabAttempts_Byte0		0x0a89

// page 'g_MasterI2CStatus'

#define MasterI2CStatus_e_Resource_Status_Byte0		0x0ac0
#define MasterI2CStatus_e_Resource_I2C_TrasactionStatus_Byte0		0x0ac1
#define MasterI2CStatus_e_CCI_EERROR_MGMT_Error_Byte0		0x0ac2
#define MasterI2CStatus_u8_NumberOfTransactionFailures_Byte0		0x0ac3
#define MasterI2CStatus_u8_NumberOfConsecutiveGrabFailures_Byte0		0x0ac4
#define MasterI2CStatus_u8_NumberOfForcedReleases_Byte0		0x0ac5
#define MasterI2CStatus_u8_WriteFifoUseCount_Byte0		0x0ac6

// page 'g_HostToMasterI2CAccessControl'

#define HostToMasterI2CAccessControl_u16_DeviceID_Byte0		0x0b00
#define HostToMasterI2CAccessControl_u16_DeviceID_Byte1		0x0b01
#define HostToMasterI2CAccessControl_u16_Index_Byte0		0x0b02
#define HostToMasterI2CAccessControl_u16_Index_Byte1		0x0b03
#define HostToMasterI2CAccessControl_e_Coin_Command_Byte0		0x0b04
#define HostToMasterI2CAccessControl_e_HostToMasterI2CRequest_Request_Byte0		0x0b05
#define HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0		0x0b06
#define HostToMasterI2CAccessControl_e_DeviceAddress_Type_Byte0		0x0b07
#define HostToMasterI2CAccessControl_e_DeviceIndex_Type_Byte0		0x0b08

// page 'g_HostToMasterI2CAccessData'

#define HostToMasterI2CAccessData_u8_arrData_0_Byte0		0x0b40
#define HostToMasterI2CAccessData_u8_arrData_1_Byte0		0x0b41
#define HostToMasterI2CAccessData_u8_arrData_2_Byte0		0x0b42
#define HostToMasterI2CAccessData_u8_arrData_3_Byte0		0x0b43
#define HostToMasterI2CAccessData_u8_arrData_4_Byte0		0x0b44
#define HostToMasterI2CAccessData_u8_arrData_5_Byte0		0x0b45
#define HostToMasterI2CAccessData_u8_arrData_6_Byte0		0x0b46
#define HostToMasterI2CAccessData_u8_arrData_7_Byte0		0x0b47
#define HostToMasterI2CAccessData_u8_arrData_8_Byte0		0x0b48
#define HostToMasterI2CAccessData_u8_arrData_9_Byte0		0x0b49
#define HostToMasterI2CAccessData_u8_arrData_10_Byte0		0x0b4a
#define HostToMasterI2CAccessData_u8_arrData_11_Byte0		0x0b4b
#define HostToMasterI2CAccessData_u8_arrData_12_Byte0		0x0b4c
#define HostToMasterI2CAccessData_u8_arrData_13_Byte0		0x0b4d
#define HostToMasterI2CAccessData_u8_arrData_14_Byte0		0x0b4e
#define HostToMasterI2CAccessData_u8_arrData_15_Byte0		0x0b4f

// page 'g_HostToMasterI2CAccessStatus'

#define HostToMasterI2CAccessStatus_e_Coin_Status_Byte0		0x0b80
#define HostToMasterI2CAccessStatus_e_Result_Driver_Byte0		0x0b81
#define HostToMasterI2CAccessStatus_u8_HostToMasterI2CAccessErrorCount_Byte0		0x0b82

// page 'g_CE_ColourMatrixFloat[0]'

#define CE_ColourMatrixFloat_0_f_RedInRed_Byte0		0x0bc0
#define CE_ColourMatrixFloat_0_f_RedInRed_Byte1		0x0bc1
#define CE_ColourMatrixFloat_0_f_RedInRed_Byte2		0x0bc2
#define CE_ColourMatrixFloat_0_f_RedInRed_Byte3		0x0bc3
#define CE_ColourMatrixFloat_0_f_GreenInRed_Byte0		0x0bc4
#define CE_ColourMatrixFloat_0_f_GreenInRed_Byte1		0x0bc5
#define CE_ColourMatrixFloat_0_f_GreenInRed_Byte2		0x0bc6
#define CE_ColourMatrixFloat_0_f_GreenInRed_Byte3		0x0bc7
#define CE_ColourMatrixFloat_0_f_BlueInRed_Byte0		0x0bc8
#define CE_ColourMatrixFloat_0_f_BlueInRed_Byte1		0x0bc9
#define CE_ColourMatrixFloat_0_f_BlueInRed_Byte2		0x0bca
#define CE_ColourMatrixFloat_0_f_BlueInRed_Byte3		0x0bcb
#define CE_ColourMatrixFloat_0_f_RedInGreen_Byte0		0x0bcc
#define CE_ColourMatrixFloat_0_f_RedInGreen_Byte1		0x0bcd
#define CE_ColourMatrixFloat_0_f_RedInGreen_Byte2		0x0bce
#define CE_ColourMatrixFloat_0_f_RedInGreen_Byte3		0x0bcf
#define CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0		0x0bd0
#define CE_ColourMatrixFloat_0_f_GreenInGreen_Byte1		0x0bd1
#define CE_ColourMatrixFloat_0_f_GreenInGreen_Byte2		0x0bd2
#define CE_ColourMatrixFloat_0_f_GreenInGreen_Byte3		0x0bd3
#define CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0		0x0bd4
#define CE_ColourMatrixFloat_0_f_BlueInGreen_Byte1		0x0bd5
#define CE_ColourMatrixFloat_0_f_BlueInGreen_Byte2		0x0bd6
#define CE_ColourMatrixFloat_0_f_BlueInGreen_Byte3		0x0bd7
#define CE_ColourMatrixFloat_0_f_RedInBlue_Byte0		0x0bd8
#define CE_ColourMatrixFloat_0_f_RedInBlue_Byte1		0x0bd9
#define CE_ColourMatrixFloat_0_f_RedInBlue_Byte2		0x0bda
#define CE_ColourMatrixFloat_0_f_RedInBlue_Byte3		0x0bdb
#define CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0		0x0bdc
#define CE_ColourMatrixFloat_0_f_GreenInBlue_Byte1		0x0bdd
#define CE_ColourMatrixFloat_0_f_GreenInBlue_Byte2		0x0bde
#define CE_ColourMatrixFloat_0_f_GreenInBlue_Byte3		0x0bdf
#define CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0		0x0be0
#define CE_ColourMatrixFloat_0_f_BlueInBlue_Byte1		0x0be1
#define CE_ColourMatrixFloat_0_f_BlueInBlue_Byte2		0x0be2
#define CE_ColourMatrixFloat_0_f_BlueInBlue_Byte3		0x0be3
#define CE_ColourMatrixFloat_0_e_SwapColour_Red_Byte0		0x0be4
#define CE_ColourMatrixFloat_0_e_SwapColour_Green_Byte0		0x0be5
#define CE_ColourMatrixFloat_0_e_SwapColour_Blue_Byte0		0x0be6

// page 'g_CE_ColourMatrixDamped[0]'

#define CE_ColourMatrixDamped_0_s16_RedInRed_Byte0		0x0c00
#define CE_ColourMatrixDamped_0_s16_RedInRed_Byte1		0x0c01
#define CE_ColourMatrixDamped_0_s16_GreenInRed_Byte0		0x0c02
#define CE_ColourMatrixDamped_0_s16_GreenInRed_Byte1		0x0c03
#define CE_ColourMatrixDamped_0_s16_BlueInRed_Byte0		0x0c04
#define CE_ColourMatrixDamped_0_s16_BlueInRed_Byte1		0x0c05
#define CE_ColourMatrixDamped_0_s16_RedInGreen_Byte0		0x0c06
#define CE_ColourMatrixDamped_0_s16_RedInGreen_Byte1		0x0c07
#define CE_ColourMatrixDamped_0_s16_GreenInGreen_Byte0		0x0c08
#define CE_ColourMatrixDamped_0_s16_GreenInGreen_Byte1		0x0c09
#define CE_ColourMatrixDamped_0_s16_BlueInGreen_Byte0		0x0c0a
#define CE_ColourMatrixDamped_0_s16_BlueInGreen_Byte1		0x0c0b
#define CE_ColourMatrixDamped_0_s16_RedInBlue_Byte0		0x0c0c
#define CE_ColourMatrixDamped_0_s16_RedInBlue_Byte1		0x0c0d
#define CE_ColourMatrixDamped_0_s16_GreenInBlue_Byte0		0x0c0e
#define CE_ColourMatrixDamped_0_s16_GreenInBlue_Byte1		0x0c0f
#define CE_ColourMatrixDamped_0_s16_BlueInBlue_Byte0		0x0c10
#define CE_ColourMatrixDamped_0_s16_BlueInBlue_Byte1		0x0c11
#define CE_ColourMatrixDamped_0_s16_Offset_R_Byte0		0x0c12
#define CE_ColourMatrixDamped_0_s16_Offset_R_Byte1		0x0c13
#define CE_ColourMatrixDamped_0_s16_Offset_G_Byte0		0x0c14
#define CE_ColourMatrixDamped_0_s16_Offset_G_Byte1		0x0c15
#define CE_ColourMatrixDamped_0_s16_Offset_B_Byte0		0x0c16
#define CE_ColourMatrixDamped_0_s16_Offset_B_Byte1		0x0c17

// page 'g_CE_ColourMatrixFloat[1]'

#define CE_ColourMatrixFloat_1_f_RedInRed_Byte0		0x0c40
#define CE_ColourMatrixFloat_1_f_RedInRed_Byte1		0x0c41
#define CE_ColourMatrixFloat_1_f_RedInRed_Byte2		0x0c42
#define CE_ColourMatrixFloat_1_f_RedInRed_Byte3		0x0c43
#define CE_ColourMatrixFloat_1_f_GreenInRed_Byte0		0x0c44
#define CE_ColourMatrixFloat_1_f_GreenInRed_Byte1		0x0c45
#define CE_ColourMatrixFloat_1_f_GreenInRed_Byte2		0x0c46
#define CE_ColourMatrixFloat_1_f_GreenInRed_Byte3		0x0c47
#define CE_ColourMatrixFloat_1_f_BlueInRed_Byte0		0x0c48
#define CE_ColourMatrixFloat_1_f_BlueInRed_Byte1		0x0c49
#define CE_ColourMatrixFloat_1_f_BlueInRed_Byte2		0x0c4a
#define CE_ColourMatrixFloat_1_f_BlueInRed_Byte3		0x0c4b
#define CE_ColourMatrixFloat_1_f_RedInGreen_Byte0		0x0c4c
#define CE_ColourMatrixFloat_1_f_RedInGreen_Byte1		0x0c4d
#define CE_ColourMatrixFloat_1_f_RedInGreen_Byte2		0x0c4e
#define CE_ColourMatrixFloat_1_f_RedInGreen_Byte3		0x0c4f
#define CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0		0x0c50
#define CE_ColourMatrixFloat_1_f_GreenInGreen_Byte1		0x0c51
#define CE_ColourMatrixFloat_1_f_GreenInGreen_Byte2		0x0c52
#define CE_ColourMatrixFloat_1_f_GreenInGreen_Byte3		0x0c53
#define CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0		0x0c54
#define CE_ColourMatrixFloat_1_f_BlueInGreen_Byte1		0x0c55
#define CE_ColourMatrixFloat_1_f_BlueInGreen_Byte2		0x0c56
#define CE_ColourMatrixFloat_1_f_BlueInGreen_Byte3		0x0c57
#define CE_ColourMatrixFloat_1_f_RedInBlue_Byte0		0x0c58
#define CE_ColourMatrixFloat_1_f_RedInBlue_Byte1		0x0c59
#define CE_ColourMatrixFloat_1_f_RedInBlue_Byte2		0x0c5a
#define CE_ColourMatrixFloat_1_f_RedInBlue_Byte3		0x0c5b
#define CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0		0x0c5c
#define CE_ColourMatrixFloat_1_f_GreenInBlue_Byte1		0x0c5d
#define CE_ColourMatrixFloat_1_f_GreenInBlue_Byte2		0x0c5e
#define CE_ColourMatrixFloat_1_f_GreenInBlue_Byte3		0x0c5f
#define CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0		0x0c60
#define CE_ColourMatrixFloat_1_f_BlueInBlue_Byte1		0x0c61
#define CE_ColourMatrixFloat_1_f_BlueInBlue_Byte2		0x0c62
#define CE_ColourMatrixFloat_1_f_BlueInBlue_Byte3		0x0c63
#define CE_ColourMatrixFloat_1_e_SwapColour_Red_Byte0		0x0c64
#define CE_ColourMatrixFloat_1_e_SwapColour_Green_Byte0		0x0c65
#define CE_ColourMatrixFloat_1_e_SwapColour_Blue_Byte0		0x0c66

// page 'g_CE_ColourMatrixDamped[1]'

#define CE_ColourMatrixDamped_1_s16_RedInRed_Byte0		0x0c80
#define CE_ColourMatrixDamped_1_s16_RedInRed_Byte1		0x0c81
#define CE_ColourMatrixDamped_1_s16_GreenInRed_Byte0		0x0c82
#define CE_ColourMatrixDamped_1_s16_GreenInRed_Byte1		0x0c83
#define CE_ColourMatrixDamped_1_s16_BlueInRed_Byte0		0x0c84
#define CE_ColourMatrixDamped_1_s16_BlueInRed_Byte1		0x0c85
#define CE_ColourMatrixDamped_1_s16_RedInGreen_Byte0		0x0c86
#define CE_ColourMatrixDamped_1_s16_RedInGreen_Byte1		0x0c87
#define CE_ColourMatrixDamped_1_s16_GreenInGreen_Byte0		0x0c88
#define CE_ColourMatrixDamped_1_s16_GreenInGreen_Byte1		0x0c89
#define CE_ColourMatrixDamped_1_s16_BlueInGreen_Byte0		0x0c8a
#define CE_ColourMatrixDamped_1_s16_BlueInGreen_Byte1		0x0c8b
#define CE_ColourMatrixDamped_1_s16_RedInBlue_Byte0		0x0c8c
#define CE_ColourMatrixDamped_1_s16_RedInBlue_Byte1		0x0c8d
#define CE_ColourMatrixDamped_1_s16_GreenInBlue_Byte0		0x0c8e
#define CE_ColourMatrixDamped_1_s16_GreenInBlue_Byte1		0x0c8f
#define CE_ColourMatrixDamped_1_s16_BlueInBlue_Byte0		0x0c90
#define CE_ColourMatrixDamped_1_s16_BlueInBlue_Byte1		0x0c91
#define CE_ColourMatrixDamped_1_s16_Offset_R_Byte0		0x0c92
#define CE_ColourMatrixDamped_1_s16_Offset_R_Byte1		0x0c93
#define CE_ColourMatrixDamped_1_s16_Offset_G_Byte0		0x0c94
#define CE_ColourMatrixDamped_1_s16_Offset_G_Byte1		0x0c95
#define CE_ColourMatrixDamped_1_s16_Offset_B_Byte0		0x0c96
#define CE_ColourMatrixDamped_1_s16_Offset_B_Byte1		0x0c97

// page 'g_CE_YUVCoderControls[0]'

#define CE_YUVCoderControls_0_e_Transform_Type_Byte0		0x0cc0
#define CE_YUVCoderControls_0_u8_Contrast_Byte0		0x0cc1
#define CE_YUVCoderControls_0_u8_MaxContrast_Byte0		0x0cc2
#define CE_YUVCoderControls_0_u8_ColourSaturation_Byte0		0x0cc3
#define CE_YUVCoderControls_0_u8_MaxColourSaturation_Byte0		0x0cc4

// page 'g_CE_CustomTransformOutputSignalRange[0]'

#define CE_CustomTransformOutputSignalRange_0_u16_LumaExcursion_Byte0		0x0d00
#define CE_CustomTransformOutputSignalRange_0_u16_LumaExcursion_Byte1		0x0d01
#define CE_CustomTransformOutputSignalRange_0_u16_LumaMidpointTimes2_Byte0		0x0d02
#define CE_CustomTransformOutputSignalRange_0_u16_LumaMidpointTimes2_Byte1		0x0d03
#define CE_CustomTransformOutputSignalRange_0_u16_ChromaExcursion_Byte0		0x0d04
#define CE_CustomTransformOutputSignalRange_0_u16_ChromaExcursion_Byte1		0x0d05
#define CE_CustomTransformOutputSignalRange_0_u16_ChromaMidpointTimes2_Byte0		0x0d06
#define CE_CustomTransformOutputSignalRange_0_u16_ChromaMidpointTimes2_Byte1		0x0d07

// page 'g_CE_FadeToBlack[0]'

#define CE_FadeToBlack_0_f_BlackValue_Byte0		0x0d40
#define CE_FadeToBlack_0_f_BlackValue_Byte1		0x0d41
#define CE_FadeToBlack_0_f_BlackValue_Byte2		0x0d42
#define CE_FadeToBlack_0_f_BlackValue_Byte3		0x0d43
#define CE_FadeToBlack_0_f_DamperLowThreshold_Byte0		0x0d44
#define CE_FadeToBlack_0_f_DamperLowThreshold_Byte1		0x0d45
#define CE_FadeToBlack_0_f_DamperLowThreshold_Byte2		0x0d46
#define CE_FadeToBlack_0_f_DamperLowThreshold_Byte3		0x0d47
#define CE_FadeToBlack_0_f_DamperHighThreshold_Byte0		0x0d48
#define CE_FadeToBlack_0_f_DamperHighThreshold_Byte1		0x0d49
#define CE_FadeToBlack_0_f_DamperHighThreshold_Byte2		0x0d4a
#define CE_FadeToBlack_0_f_DamperHighThreshold_Byte3		0x0d4b
#define CE_FadeToBlack_0_f_DamperOutput_Byte0		0x0d4c
#define CE_FadeToBlack_0_f_DamperOutput_Byte1		0x0d4d
#define CE_FadeToBlack_0_f_DamperOutput_Byte2		0x0d4e
#define CE_FadeToBlack_0_f_DamperOutput_Byte3		0x0d4f
#define CE_FadeToBlack_0_e_Flag_Disable_Byte0		0x0d50

// page 'g_CE_OutputCoderMatrix[0]'

#define CE_OutputCoderMatrix_0_s16_w0_0_Byte0		0x0d80
#define CE_OutputCoderMatrix_0_s16_w0_0_Byte1		0x0d81
#define CE_OutputCoderMatrix_0_s16_w0_1_Byte0		0x0d82
#define CE_OutputCoderMatrix_0_s16_w0_1_Byte1		0x0d83
#define CE_OutputCoderMatrix_0_s16_w0_2_Byte0		0x0d84
#define CE_OutputCoderMatrix_0_s16_w0_2_Byte1		0x0d85
#define CE_OutputCoderMatrix_0_s16_w1_0_Byte0		0x0d86
#define CE_OutputCoderMatrix_0_s16_w1_0_Byte1		0x0d87
#define CE_OutputCoderMatrix_0_s16_w1_1_Byte0		0x0d88
#define CE_OutputCoderMatrix_0_s16_w1_1_Byte1		0x0d89
#define CE_OutputCoderMatrix_0_s16_w1_2_Byte0		0x0d8a
#define CE_OutputCoderMatrix_0_s16_w1_2_Byte1		0x0d8b
#define CE_OutputCoderMatrix_0_s16_w2_0_Byte0		0x0d8c
#define CE_OutputCoderMatrix_0_s16_w2_0_Byte1		0x0d8d
#define CE_OutputCoderMatrix_0_s16_w2_1_Byte0		0x0d8e
#define CE_OutputCoderMatrix_0_s16_w2_1_Byte1		0x0d8f
#define CE_OutputCoderMatrix_0_s16_w2_2_Byte0		0x0d90
#define CE_OutputCoderMatrix_0_s16_w2_2_Byte1		0x0d91

// page 'g_CE_OutputCoderOffsetVector[0]'

#define CE_OutputCoderOffsetVector_0_s16_i0_Byte0		0x0dc0
#define CE_OutputCoderOffsetVector_0_s16_i0_Byte1		0x0dc1
#define CE_OutputCoderOffsetVector_0_s16_i1_Byte0		0x0dc2
#define CE_OutputCoderOffsetVector_0_s16_i1_Byte1		0x0dc3
#define CE_OutputCoderOffsetVector_0_s16_i2_Byte0		0x0dc4
#define CE_OutputCoderOffsetVector_0_s16_i2_Byte1		0x0dc5

// page 'g_YUVCoderStatus[0]'

#define YUVCoderStatus_0_f_Contrast_Byte0		0x0e00
#define YUVCoderStatus_0_f_Contrast_Byte1		0x0e01
#define YUVCoderStatus_0_f_Contrast_Byte2		0x0e02
#define YUVCoderStatus_0_f_Contrast_Byte3		0x0e03
#define YUVCoderStatus_0_f_Saturation_Byte0		0x0e04
#define YUVCoderStatus_0_f_Saturation_Byte1		0x0e05
#define YUVCoderStatus_0_f_Saturation_Byte2		0x0e06
#define YUVCoderStatus_0_f_Saturation_Byte3		0x0e07

// page 'g_CE_YUVCoderControls[1]'

#define CE_YUVCoderControls_1_e_Transform_Type_Byte0		0x0e40
#define CE_YUVCoderControls_1_u8_Contrast_Byte0		0x0e41
#define CE_YUVCoderControls_1_u8_MaxContrast_Byte0		0x0e42
#define CE_YUVCoderControls_1_u8_ColourSaturation_Byte0		0x0e43
#define CE_YUVCoderControls_1_u8_MaxColourSaturation_Byte0		0x0e44

// page 'g_CE_CustomTransformOutputSignalRange[1]'

#define CE_CustomTransformOutputSignalRange_1_u16_LumaExcursion_Byte0		0x0e80
#define CE_CustomTransformOutputSignalRange_1_u16_LumaExcursion_Byte1		0x0e81
#define CE_CustomTransformOutputSignalRange_1_u16_LumaMidpointTimes2_Byte0		0x0e82
#define CE_CustomTransformOutputSignalRange_1_u16_LumaMidpointTimes2_Byte1		0x0e83
#define CE_CustomTransformOutputSignalRange_1_u16_ChromaExcursion_Byte0		0x0e84
#define CE_CustomTransformOutputSignalRange_1_u16_ChromaExcursion_Byte1		0x0e85
#define CE_CustomTransformOutputSignalRange_1_u16_ChromaMidpointTimes2_Byte0		0x0e86
#define CE_CustomTransformOutputSignalRange_1_u16_ChromaMidpointTimes2_Byte1		0x0e87

// page 'g_CE_FadeToBlack[1]'

#define CE_FadeToBlack_1_f_BlackValue_Byte0		0x0ec0
#define CE_FadeToBlack_1_f_BlackValue_Byte1		0x0ec1
#define CE_FadeToBlack_1_f_BlackValue_Byte2		0x0ec2
#define CE_FadeToBlack_1_f_BlackValue_Byte3		0x0ec3
#define CE_FadeToBlack_1_f_DamperLowThreshold_Byte0		0x0ec4
#define CE_FadeToBlack_1_f_DamperLowThreshold_Byte1		0x0ec5
#define CE_FadeToBlack_1_f_DamperLowThreshold_Byte2		0x0ec6
#define CE_FadeToBlack_1_f_DamperLowThreshold_Byte3		0x0ec7
#define CE_FadeToBlack_1_f_DamperHighThreshold_Byte0		0x0ec8
#define CE_FadeToBlack_1_f_DamperHighThreshold_Byte1		0x0ec9
#define CE_FadeToBlack_1_f_DamperHighThreshold_Byte2		0x0eca
#define CE_FadeToBlack_1_f_DamperHighThreshold_Byte3		0x0ecb
#define CE_FadeToBlack_1_f_DamperOutput_Byte0		0x0ecc
#define CE_FadeToBlack_1_f_DamperOutput_Byte1		0x0ecd
#define CE_FadeToBlack_1_f_DamperOutput_Byte2		0x0ece
#define CE_FadeToBlack_1_f_DamperOutput_Byte3		0x0ecf
#define CE_FadeToBlack_1_e_Flag_Disable_Byte0		0x0ed0

// page 'g_CE_OutputCoderMatrix[1]'

#define CE_OutputCoderMatrix_1_s16_w0_0_Byte0		0x0f00
#define CE_OutputCoderMatrix_1_s16_w0_0_Byte1		0x0f01
#define CE_OutputCoderMatrix_1_s16_w0_1_Byte0		0x0f02
#define CE_OutputCoderMatrix_1_s16_w0_1_Byte1		0x0f03
#define CE_OutputCoderMatrix_1_s16_w0_2_Byte0		0x0f04
#define CE_OutputCoderMatrix_1_s16_w0_2_Byte1		0x0f05
#define CE_OutputCoderMatrix_1_s16_w1_0_Byte0		0x0f06
#define CE_OutputCoderMatrix_1_s16_w1_0_Byte1		0x0f07
#define CE_OutputCoderMatrix_1_s16_w1_1_Byte0		0x0f08
#define CE_OutputCoderMatrix_1_s16_w1_1_Byte1		0x0f09
#define CE_OutputCoderMatrix_1_s16_w1_2_Byte0		0x0f0a
#define CE_OutputCoderMatrix_1_s16_w1_2_Byte1		0x0f0b
#define CE_OutputCoderMatrix_1_s16_w2_0_Byte0		0x0f0c
#define CE_OutputCoderMatrix_1_s16_w2_0_Byte1		0x0f0d
#define CE_OutputCoderMatrix_1_s16_w2_1_Byte0		0x0f0e
#define CE_OutputCoderMatrix_1_s16_w2_1_Byte1		0x0f0f
#define CE_OutputCoderMatrix_1_s16_w2_2_Byte0		0x0f10
#define CE_OutputCoderMatrix_1_s16_w2_2_Byte1		0x0f11

// page 'g_CE_OutputCoderOffsetVector[1]'

#define CE_OutputCoderOffsetVector_1_s16_i0_Byte0		0x0f40
#define CE_OutputCoderOffsetVector_1_s16_i0_Byte1		0x0f41
#define CE_OutputCoderOffsetVector_1_s16_i1_Byte0		0x0f42
#define CE_OutputCoderOffsetVector_1_s16_i1_Byte1		0x0f43
#define CE_OutputCoderOffsetVector_1_s16_i2_Byte0		0x0f44
#define CE_OutputCoderOffsetVector_1_s16_i2_Byte1		0x0f45

// page 'g_YUVCoderStatus[1]'

#define YUVCoderStatus_1_f_Contrast_Byte0		0x0f80
#define YUVCoderStatus_1_f_Contrast_Byte1		0x0f81
#define YUVCoderStatus_1_f_Contrast_Byte2		0x0f82
#define YUVCoderStatus_1_f_Contrast_Byte3		0x0f83
#define YUVCoderStatus_1_f_Saturation_Byte0		0x0f84
#define YUVCoderStatus_1_f_Saturation_Byte1		0x0f85
#define YUVCoderStatus_1_f_Saturation_Byte2		0x0f86
#define YUVCoderStatus_1_f_Saturation_Byte3		0x0f87

// page 'g_WhiteBalanceControl'

#define WhiteBalanceControl_f_RedManualGain_Byte0		0x0fc0
#define WhiteBalanceControl_f_RedManualGain_Byte1		0x0fc1
#define WhiteBalanceControl_f_RedManualGain_Byte2		0x0fc2
#define WhiteBalanceControl_f_RedManualGain_Byte3		0x0fc3
#define WhiteBalanceControl_f_GreenManualGain_Byte0		0x0fc4
#define WhiteBalanceControl_f_GreenManualGain_Byte1		0x0fc5
#define WhiteBalanceControl_f_GreenManualGain_Byte2		0x0fc6
#define WhiteBalanceControl_f_GreenManualGain_Byte3		0x0fc7
#define WhiteBalanceControl_f_BlueManualGain_Byte0		0x0fc8
#define WhiteBalanceControl_f_BlueManualGain_Byte1		0x0fc9
#define WhiteBalanceControl_f_BlueManualGain_Byte2		0x0fca
#define WhiteBalanceControl_f_BlueManualGain_Byte3		0x0fcb

// page 'g_WhiteBalanceStatus'

#define WhiteBalanceStatus_f_RedGain_Byte0		0x1000
#define WhiteBalanceStatus_f_RedGain_Byte1		0x1001
#define WhiteBalanceStatus_f_RedGain_Byte2		0x1002
#define WhiteBalanceStatus_f_RedGain_Byte3		0x1003
#define WhiteBalanceStatus_f_GreenGain_Byte0		0x1004
#define WhiteBalanceStatus_f_GreenGain_Byte1		0x1005
#define WhiteBalanceStatus_f_GreenGain_Byte2		0x1006
#define WhiteBalanceStatus_f_GreenGain_Byte3		0x1007
#define WhiteBalanceStatus_f_BlueGain_Byte0		0x1008
#define WhiteBalanceStatus_f_BlueGain_Byte1		0x1009
#define WhiteBalanceStatus_f_BlueGain_Byte2		0x100a
#define WhiteBalanceStatus_f_BlueGain_Byte3		0x100b

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_ChannelGains_Control'

#define ChannelGains_Control_e_Flag_EnableChannelGains_Byte0		0x1280

// page 'g_ChannelGains_combined'

#define ChannelGains_combined_f_RedGain_Byte0		0x12c0
#define ChannelGains_combined_f_RedGain_Byte1		0x12c1
#define ChannelGains_combined_f_RedGain_Byte2		0x12c2
#define ChannelGains_combined_f_RedGain_Byte3		0x12c3
#define ChannelGains_combined_f_GreenInRedGain_Byte0		0x12c4
#define ChannelGains_combined_f_GreenInRedGain_Byte1		0x12c5
#define ChannelGains_combined_f_GreenInRedGain_Byte2		0x12c6
#define ChannelGains_combined_f_GreenInRedGain_Byte3		0x12c7
#define ChannelGains_combined_f_GreenInBlueGain_Byte0		0x12c8
#define ChannelGains_combined_f_GreenInBlueGain_Byte1		0x12c9
#define ChannelGains_combined_f_GreenInBlueGain_Byte2		0x12ca
#define ChannelGains_combined_f_GreenInBlueGain_Byte3		0x12cb
#define ChannelGains_combined_f_BlueGain_Byte0		0x12cc
#define ChannelGains_combined_f_BlueGain_Byte1		0x12cd
#define ChannelGains_combined_f_BlueGain_Byte2		0x12ce
#define ChannelGains_combined_f_BlueGain_Byte3		0x12cf

// page 'g_Glace_Control'

#define Glace_Control_f_HBlockSizeFraction_Byte0		0x1300
#define Glace_Control_f_HBlockSizeFraction_Byte1		0x1301
#define Glace_Control_f_HBlockSizeFraction_Byte2		0x1302
#define Glace_Control_f_HBlockSizeFraction_Byte3		0x1303
#define Glace_Control_f_VBlockSizeFraction_Byte0		0x1304
#define Glace_Control_f_VBlockSizeFraction_Byte1		0x1305
#define Glace_Control_f_VBlockSizeFraction_Byte2		0x1306
#define Glace_Control_f_VBlockSizeFraction_Byte3		0x1307
#define Glace_Control_f_HROIStartFraction_Byte0		0x1308
#define Glace_Control_f_HROIStartFraction_Byte1		0x1309
#define Glace_Control_f_HROIStartFraction_Byte2		0x130a
#define Glace_Control_f_HROIStartFraction_Byte3		0x130b
#define Glace_Control_f_VROIStartFraction_Byte0		0x130c
#define Glace_Control_f_VROIStartFraction_Byte1		0x130d
#define Glace_Control_f_VROIStartFraction_Byte2		0x130e
#define Glace_Control_f_VROIStartFraction_Byte3		0x130f
#define Glace_Control_ptrGlace_Statistics_Byte0		0x1310
#define Glace_Control_ptrGlace_Statistics_Byte1		0x1311
#define Glace_Control_ptrGlace_Statistics_Byte2		0x1312
#define Glace_Control_ptrGlace_Statistics_Byte3		0x1313
#define Glace_Control_u8_RedSaturationLevel_Byte0		0x1314
#define Glace_Control_u8_GreenSaturationLevel_Byte0		0x1315
#define Glace_Control_u8_BlueSaturationLevel_Byte0		0x1316
#define Glace_Control_u8_HGridSize_Byte0		0x1317
#define Glace_Control_u8_VGridSize_Byte0		0x1318
#define Glace_Control_e_GlaceOperationMode_Control_Byte0		0x1319
#define Glace_Control_e_GlaceDataSource_Byte0		0x131a
#define Glace_Control_u8_ParamUpdateCount_Byte0		0x131b
#define Glace_Control_u8_ControlUpdateCount_debug_Byte0		0x131c
#define Glace_Control_e_StatisticsFov_Byte0		0x131d

// page 'g_Glace_Status'

#define Glace_Status_u32_GlaceMultiplier_Byte0		0x1340
#define Glace_Status_u32_GlaceMultiplier_Byte1		0x1341
#define Glace_Status_u32_GlaceMultiplier_Byte2		0x1342
#define Glace_Status_u32_GlaceMultiplier_Byte3		0x1343
#define Glace_Status_u16_HROIStart_Byte0		0x1344
#define Glace_Status_u16_HROIStart_Byte1		0x1345
#define Glace_Status_u16_VROIStart_Byte0		0x1346
#define Glace_Status_u16_VROIStart_Byte1		0x1347
#define Glace_Status_u8_HGridSize_Byte0		0x1348
#define Glace_Status_u8_VGridSize_Byte0		0x1349
#define Glace_Status_u8_HBlockSize_Byte0		0x134a
#define Glace_Status_u8_VBlockSize_Byte0		0x134b
#define Glace_Status_u8_GlaceShift_Byte0		0x134c
#define Glace_Status_e_GlaceOperationMode_Status_Byte0		0x134d
#define Glace_Status_u8_ParamUpdateCount_Byte0		0x134e
#define Glace_Status_e_GlaceExportStatus_Byte0		0x134f
#define Glace_Status_u8_ControlUpdateCount_Byte0		0x1350
#define Glace_Status_e_Flag_GlaceEnablePending_Byte0		0x1351
#define Glace_Status_e_StatisticsFov_Byte0		0x1352

// page 'g_GPIOControl'

#define GPIOControl_u8_Sensor0XShutdownGPO_Byte0		0x1380
#define GPIOControl_u8_Sensor1XShutdownGPO_Byte0		0x1381
#define GPIOControl_u8_FlashGPO_Byte0		0x1382

// page 'g_gpio_control'

#define gpio_control_u32_GPIO_delay_us_Byte0		0x13c0
#define gpio_control_u32_GPIO_delay_us_Byte1		0x13c1
#define gpio_control_u32_GPIO_delay_us_Byte2		0x13c2
#define gpio_control_u32_GPIO_delay_us_Byte3		0x13c3
#define gpio_control_u32_GPIO_channel_src_Byte0		0x13c4
#define gpio_control_u32_GPIO_channel_src_Byte1		0x13c5
#define gpio_control_u32_GPIO_channel_src_Byte2		0x13c6
#define gpio_control_u32_GPIO_channel_src_Byte3		0x13c7
#define gpio_control_u32_GPIO_channel0_ctrl0_Byte0		0x13c8
#define gpio_control_u32_GPIO_channel0_ctrl0_Byte1		0x13c9
#define gpio_control_u32_GPIO_channel0_ctrl0_Byte2		0x13ca
#define gpio_control_u32_GPIO_channel0_ctrl0_Byte3		0x13cb
#define gpio_control_u32_GPIO_channel0_ctrl1_Byte0		0x13cc
#define gpio_control_u32_GPIO_channel0_ctrl1_Byte1		0x13cd
#define gpio_control_u32_GPIO_channel0_ctrl1_Byte2		0x13ce
#define gpio_control_u32_GPIO_channel0_ctrl1_Byte3		0x13cf
#define gpio_control_u32_GPIO_channel0_ctrl0_ext_Byte0		0x13d0
#define gpio_control_u32_GPIO_channel0_ctrl0_ext_Byte1		0x13d1
#define gpio_control_u32_GPIO_channel0_ctrl0_ext_Byte2		0x13d2
#define gpio_control_u32_GPIO_channel0_ctrl0_ext_Byte3		0x13d3
#define gpio_control_u32_GPIO_channel0_ctrl1_ext_Byte0		0x13d4
#define gpio_control_u32_GPIO_channel0_ctrl1_ext_Byte1		0x13d5
#define gpio_control_u32_GPIO_channel0_ctrl1_ext_Byte2		0x13d6
#define gpio_control_u32_GPIO_channel0_ctrl1_ext_Byte3		0x13d7
#define gpio_control_u32_GlobalTimerTimeout_us_Byte0		0x13d8
#define gpio_control_u32_GlobalTimerTimeout_us_Byte1		0x13d9
#define gpio_control_u32_GlobalTimerTimeout_us_Byte2		0x13da
#define gpio_control_u32_GlobalTimerTimeout_us_Byte3		0x13db
#define gpio_control_u16_GPIO_channel0_repeat_Byte0		0x13dc
#define gpio_control_u16_GPIO_channel0_repeat_Byte1		0x13dd
#define gpio_control_u16_GPIO_channel0_ctrl0_Byte0		0x13de
#define gpio_control_u16_GPIO_channel0_ctrl0_Byte1		0x13df
#define gpio_control_u16_GPIO_channel0_ctrl1_Byte0		0x13e0
#define gpio_control_u16_GPIO_channel0_ctrl1_Byte1		0x13e1
#define gpio_control_u8_GPIO_gpd_Byte0		0x13e2
#define gpio_control_u8_GPIO_gpo_Byte0		0x13e3
#define gpio_control_u8_GPIO_config_Byte0		0x13e4
#define gpio_control_e_Flag_Channel_Start_Byte0		0x13e5
#define gpio_control_e_Flag_Channel_Stop_Byte0		0x13e6
#define gpio_control_e_Flag_Channel_Reset_Byte0		0x13e7
#define gpio_control_e_Flag_Channel_Polarity_Byte0		0x13e8
#define gpio_control_u8_GPIO_channel_config_Byte0		0x13e9
#define gpio_control_e_Flag_Debug_IDP_Timer_Byte0		0x13ea

// page 'g_gpio_debug'

#define gpio_debug_f_Counter1_Byte0		0x1400
#define gpio_debug_f_Counter1_Byte1		0x1401
#define gpio_debug_f_Counter1_Byte2		0x1402
#define gpio_debug_f_Counter1_Byte3		0x1403
#define gpio_debug_f_Counter2_Byte0		0x1404
#define gpio_debug_f_Counter2_Byte1		0x1405
#define gpio_debug_f_Counter2_Byte2		0x1406
#define gpio_debug_f_Counter2_Byte3		0x1407
#define gpio_debug_u32_GlobalRefCounter_Byte0		0x1408
#define gpio_debug_u32_GlobalRefCounter_Byte1		0x1409
#define gpio_debug_u32_GlobalRefCounter_Byte2		0x140a
#define gpio_debug_u32_GlobalRefCounter_Byte3		0x140b
#define gpio_debug_u16_Chnl0_Ctrl1_Byte0		0x140c
#define gpio_debug_u16_Chnl0_Ctrl1_Byte1		0x140d
#define gpio_debug_u16_Chnl0_Ctrl0_Byte0		0x140e
#define gpio_debug_u16_Chnl0_Ctrl0_Byte1		0x140f
#define gpio_debug_u16_Chnl0_Repeat_Byte0		0x1410
#define gpio_debug_u16_Chnl0_Repeat_Byte1		0x1411
#define gpio_debug_u16_Chnl1_Ctrl1_Byte0		0x1412
#define gpio_debug_u16_Chnl1_Ctrl1_Byte1		0x1413
#define gpio_debug_u16_Chnl1_Ctrl0_Byte0		0x1414
#define gpio_debug_u16_Chnl1_Ctrl0_Byte1		0x1415
#define gpio_debug_u16_Chnl1_Repeat_Byte0		0x1416
#define gpio_debug_u16_Chnl1_Repeat_Byte1		0x1417
#define gpio_debug_u8_PreScale_Byte0		0x1418

// page 'g_HistStats_Ctrl'

#define HistStats_Ctrl_ptru32_HistRAddr_Byte0		0x1440
#define HistStats_Ctrl_ptru32_HistRAddr_Byte1		0x1441
#define HistStats_Ctrl_ptru32_HistRAddr_Byte2		0x1442
#define HistStats_Ctrl_ptru32_HistRAddr_Byte3		0x1443
#define HistStats_Ctrl_ptru32_HistGAddr_Byte0		0x1444
#define HistStats_Ctrl_ptru32_HistGAddr_Byte1		0x1445
#define HistStats_Ctrl_ptru32_HistGAddr_Byte2		0x1446
#define HistStats_Ctrl_ptru32_HistGAddr_Byte3		0x1447
#define HistStats_Ctrl_ptru32_HistBAddr_Byte0		0x1448
#define HistStats_Ctrl_ptru32_HistBAddr_Byte1		0x1449
#define HistStats_Ctrl_ptru32_HistBAddr_Byte2		0x144a
#define HistStats_Ctrl_ptru32_HistBAddr_Byte3		0x144b
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0		0x144c
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte1		0x144d
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte2		0x144e
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte3		0x144f
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0		0x1450
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte1		0x1451
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte2		0x1452
#define HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte3		0x1453
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0		0x1454
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte1		0x1455
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte2		0x1456
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte3		0x1457
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0		0x1458
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte1		0x1459
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte2		0x145a
#define HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte3		0x145b
#define HistStats_Ctrl_u8_HistPixelInputShift_Byte0		0x145c
#define HistStats_Ctrl_e_HistInputSrc_Byte0		0x145d
#define HistStats_Ctrl_e_Flag_Enable_Byte0		0x145e
#define HistStats_Ctrl_e_Flag_SoftResest_Byte0		0x145f
#define HistStats_Ctrl_e_HistCmd_Byte0		0x1460
#define HistStats_Ctrl_e_CoinCtrl_debug_Byte0		0x1461
#define HistStats_Ctrl_e_HistogramMode_Byte0		0x1462
#define HistStats_Ctrl_e_StatisticsFov_Byte0		0x1463

// page 'g_HistStats_Status'

#define HistStats_Status_u16_HistSizeX_Byte0		0x1480
#define HistStats_Status_u16_HistSizeX_Byte1		0x1481
#define HistStats_Status_u16_HistSizeY_Byte0		0x1482
#define HistStats_Status_u16_HistSizeY_Byte1		0x1483
#define HistStats_Status_u16_HistOffsetX_Byte0		0x1484
#define HistStats_Status_u16_HistOffsetX_Byte1		0x1485
#define HistStats_Status_u16_HistOffsetY_Byte0		0x1486
#define HistStats_Status_u16_HistOffsetY_Byte1		0x1487
#define HistStats_Status_u16_DarkestBin_R_Byte0		0x1488
#define HistStats_Status_u16_DarkestBin_R_Byte1		0x1489
#define HistStats_Status_u16_BrightestBin_R_Byte0		0x148a
#define HistStats_Status_u16_BrightestBin_R_Byte1		0x148b
#define HistStats_Status_u16_HighestBin_R_Byte0		0x148c
#define HistStats_Status_u16_HighestBin_R_Byte1		0x148d
#define HistStats_Status_u16_DarkestBin_G_Byte0		0x148e
#define HistStats_Status_u16_DarkestBin_G_Byte1		0x148f
#define HistStats_Status_u16_BrightestBin_G_Byte0		0x1490
#define HistStats_Status_u16_BrightestBin_G_Byte1		0x1491
#define HistStats_Status_u16_HighestBin_G_Byte0		0x1492
#define HistStats_Status_u16_HighestBin_G_Byte1		0x1493
#define HistStats_Status_u16_DarkestBin_B_Byte0		0x1494
#define HistStats_Status_u16_DarkestBin_B_Byte1		0x1495
#define HistStats_Status_u16_BrightestBin_B_Byte0		0x1496
#define HistStats_Status_u16_BrightestBin_B_Byte1		0x1497
#define HistStats_Status_u16_HighestBin_B_Byte0		0x1498
#define HistStats_Status_u16_HighestBin_B_Byte1		0x1499
#define HistStats_Status_e_CoinStatus_Byte0		0x149a
#define HistStats_Status_e_ExportStatus_Byte0		0x149b
#define HistStats_Status_e_StatisticsFov_Byte0		0x149c

// page 'g_ExpSensor_SensorProperties'

#define ExpSensor_SensorProperties_u16_AnalogGainCodeMin_Byte0		0x14c0
#define ExpSensor_SensorProperties_u16_AnalogGainCodeMin_Byte1		0x14c1
#define ExpSensor_SensorProperties_u16_AnalogGainCodeMax_Byte0		0x14c2
#define ExpSensor_SensorProperties_u16_AnalogGainCodeMax_Byte1		0x14c3
#define ExpSensor_SensorProperties_u16_AnalogGainCodeStep_Byte0		0x14c4
#define ExpSensor_SensorProperties_u16_AnalogGainCodeStep_Byte1		0x14c5
#define ExpSensor_SensorProperties_s16_AnalogGianConstantM0_Byte0		0x14c6
#define ExpSensor_SensorProperties_s16_AnalogGianConstantM0_Byte1		0x14c7
#define ExpSensor_SensorProperties_s16_AnalogGianConstantC0_Byte0		0x14c8
#define ExpSensor_SensorProperties_s16_AnalogGianConstantC0_Byte1		0x14c9
#define ExpSensor_SensorProperties_s16_AnalogGianConstantM1_Byte0		0x14ca
#define ExpSensor_SensorProperties_s16_AnalogGianConstantM1_Byte1		0x14cb
#define ExpSensor_SensorProperties_s16_AnalogGianConstantC1_Byte0		0x14cc
#define ExpSensor_SensorProperties_s16_AnalogGianConstantC1_Byte1		0x14cd
#define ExpSensor_SensorProperties_u16_MinimumCoarseIntegrationLines_Byte0		0x14ce
#define ExpSensor_SensorProperties_u16_MinimumCoarseIntegrationLines_Byte1		0x14cf
#define ExpSensor_SensorProperties_u16_CoarseIntegrationMaxMargin_Byte0		0x14d0
#define ExpSensor_SensorProperties_u16_CoarseIntegrationMaxMargin_Byte1		0x14d1
#define ExpSensor_SensorProperties_u16_MinimumFineIntegrationPixels_Byte0		0x14d2
#define ExpSensor_SensorProperties_u16_MinimumFineIntegrationPixels_Byte1		0x14d3
#define ExpSensor_SensorProperties_u16_FineIntegrationMaxMargin_Byte0		0x14d4
#define ExpSensor_SensorProperties_u16_FineIntegrationMaxMargin_Byte1		0x14d5
#define ExpSensor_SensorProperties_e_IntegrationCapability_Byte0		0x14d6

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Exposure_CompilerStatus'

#define Exposure_CompilerStatus_f_AnalogGainPending_Byte0		0x15c0
#define Exposure_CompilerStatus_f_AnalogGainPending_Byte1		0x15c1
#define Exposure_CompilerStatus_f_AnalogGainPending_Byte2		0x15c2
#define Exposure_CompilerStatus_f_AnalogGainPending_Byte3		0x15c3
#define Exposure_CompilerStatus_f_DigitalGainPending_Byte0		0x15c4
#define Exposure_CompilerStatus_f_DigitalGainPending_Byte1		0x15c5
#define Exposure_CompilerStatus_f_DigitalGainPending_Byte2		0x15c6
#define Exposure_CompilerStatus_f_DigitalGainPending_Byte3		0x15c7
#define Exposure_CompilerStatus_f_CompiledExposureTime_us_Byte0		0x15c8
#define Exposure_CompilerStatus_f_CompiledExposureTime_us_Byte1		0x15c9
#define Exposure_CompilerStatus_f_CompiledExposureTime_us_Byte2		0x15ca
#define Exposure_CompilerStatus_f_CompiledExposureTime_us_Byte3		0x15cb
#define Exposure_CompilerStatus_u32_TotalIntegrationTimePending_us_Byte0		0x15cc
#define Exposure_CompilerStatus_u32_TotalIntegrationTimePending_us_Byte1		0x15cd
#define Exposure_CompilerStatus_u32_TotalIntegrationTimePending_us_Byte2		0x15ce
#define Exposure_CompilerStatus_u32_TotalIntegrationTimePending_us_Byte3		0x15cf
#define Exposure_CompilerStatus_u16_CoarseIntegrationPending_lines_Byte0		0x15d0
#define Exposure_CompilerStatus_u16_CoarseIntegrationPending_lines_Byte1		0x15d1
#define Exposure_CompilerStatus_u16_FineIntegrationPending_pixels_Byte0		0x15d2
#define Exposure_CompilerStatus_u16_FineIntegrationPending_pixels_Byte1		0x15d3
#define Exposure_CompilerStatus_u16_AnalogGainPending_x256_Byte0		0x15d4
#define Exposure_CompilerStatus_u16_AnalogGainPending_x256_Byte1		0x15d5
#define Exposure_CompilerStatus_u16_frameRate_x100_Byte0		0x15d6
#define Exposure_CompilerStatus_u16_frameRate_x100_Byte1		0x15d7

// page 'g_Exposure_ParametersApplied'

#define Exposure_ParametersApplied_f_DigitalGain_Byte0		0x1600
#define Exposure_ParametersApplied_f_DigitalGain_Byte1		0x1601
#define Exposure_ParametersApplied_f_DigitalGain_Byte2		0x1602
#define Exposure_ParametersApplied_f_DigitalGain_Byte3		0x1603
#define Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte0		0x1604
#define Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte1		0x1605
#define Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte2		0x1606
#define Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte3		0x1607
#define Exposure_ParametersApplied_u16_CoarseIntegration_lines_Byte0		0x1608
#define Exposure_ParametersApplied_u16_CoarseIntegration_lines_Byte1		0x1609
#define Exposure_ParametersApplied_u16_FineIntegration_pixels_Byte0		0x160a
#define Exposure_ParametersApplied_u16_FineIntegration_pixels_Byte1		0x160b
#define Exposure_ParametersApplied_u16_AnalogGain_x256_Byte0		0x160c
#define Exposure_ParametersApplied_u16_AnalogGain_x256_Byte1		0x160d

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Exposure_ErrorStatus'

#define Exposure_ErrorStatus_u8_NumberOfForcedInputProcUpdates_Byte0		0x1680
#define Exposure_ErrorStatus_u8_NumberOfConsecutiveDelayedFrames_Byte0		0x1681
#define Exposure_ErrorStatus_u8_ExposureSyncErrorCount_Byte0		0x1682
#define Exposure_ErrorStatus_e_Flag_ForceInputProcUpdation_Byte0		0x1683

// page 'g_Exposure_ErrorControl'

#define Exposure_ErrorControl_u8_MaximumNumberOfFrames_Byte0		0x16c0

// page 'g_Exposure_DriverControls'

#define Exposure_DriverControls_u32_TotalTargetExposureTime_us_Byte0		0x1700
#define Exposure_DriverControls_u32_TotalTargetExposureTime_us_Byte1		0x1701
#define Exposure_DriverControls_u32_TotalTargetExposureTime_us_Byte2		0x1702
#define Exposure_DriverControls_u32_TotalTargetExposureTime_us_Byte3		0x1703
#define Exposure_DriverControls_u32_TargetExposureTime_us_Byte0		0x1704
#define Exposure_DriverControls_u32_TargetExposureTime_us_Byte1		0x1705
#define Exposure_DriverControls_u32_TargetExposureTime_us_Byte2		0x1706
#define Exposure_DriverControls_u32_TargetExposureTime_us_Byte3		0x1707
#define Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0		0x1708
#define Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte1		0x1709
#define Exposure_DriverControls_u16_Aperture_Byte0		0x170a
#define Exposure_DriverControls_u16_Aperture_Byte1		0x170b
#define Exposure_DriverControls_u8_FlashState_Byte0		0x170c
#define Exposure_DriverControls_u8_DistanceFromConvergence_Byte0		0x170d
#define Exposure_DriverControls_e_Flag_NDFilter_Byte0		0x170e
#define Exposure_DriverControls_e_Flag_AECConverged_Byte0		0x170f

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_OffsetCompensationStatus'

#define OffsetCompensationStatus_f_OffsetCompensationGain_GR_Byte0		0x17c0
#define OffsetCompensationStatus_f_OffsetCompensationGain_GR_Byte1		0x17c1
#define OffsetCompensationStatus_f_OffsetCompensationGain_GR_Byte2		0x17c2
#define OffsetCompensationStatus_f_OffsetCompensationGain_GR_Byte3		0x17c3
#define OffsetCompensationStatus_f_OffsetCompensationGain_RR_Byte0		0x17c4
#define OffsetCompensationStatus_f_OffsetCompensationGain_RR_Byte1		0x17c5
#define OffsetCompensationStatus_f_OffsetCompensationGain_RR_Byte2		0x17c6
#define OffsetCompensationStatus_f_OffsetCompensationGain_RR_Byte3		0x17c7
#define OffsetCompensationStatus_f_OffsetCompensationGain_BB_Byte0		0x17c8
#define OffsetCompensationStatus_f_OffsetCompensationGain_BB_Byte1		0x17c9
#define OffsetCompensationStatus_f_OffsetCompensationGain_BB_Byte2		0x17ca
#define OffsetCompensationStatus_f_OffsetCompensationGain_BB_Byte3		0x17cb
#define OffsetCompensationStatus_f_OffsetCompensationGain_GB_Byte0		0x17cc
#define OffsetCompensationStatus_f_OffsetCompensationGain_GB_Byte1		0x17cd
#define OffsetCompensationStatus_f_OffsetCompensationGain_GB_Byte2		0x17ce
#define OffsetCompensationStatus_f_OffsetCompensationGain_GB_Byte3		0x17cf
#define OffsetCompensationStatus_u16_Offset_GR_Byte0		0x17d0
#define OffsetCompensationStatus_u16_Offset_GR_Byte1		0x17d1
#define OffsetCompensationStatus_u16_Offset_RR_Byte0		0x17d2
#define OffsetCompensationStatus_u16_Offset_RR_Byte1		0x17d3
#define OffsetCompensationStatus_u16_Offset_BB_Byte0		0x17d4
#define OffsetCompensationStatus_u16_Offset_BB_Byte1		0x17d5
#define OffsetCompensationStatus_u16_Offset_GB_Byte0		0x17d6
#define OffsetCompensationStatus_u16_Offset_GB_Byte1		0x17d7

// page 'g_SensorSetup'

#define SensorSetup_f_RedTiltGain_Byte0		0x1800
#define SensorSetup_f_RedTiltGain_Byte1		0x1801
#define SensorSetup_f_RedTiltGain_Byte2		0x1802
#define SensorSetup_f_RedTiltGain_Byte3		0x1803
#define SensorSetup_f_GreenTiltGain_Byte0		0x1804
#define SensorSetup_f_GreenTiltGain_Byte1		0x1805
#define SensorSetup_f_GreenTiltGain_Byte2		0x1806
#define SensorSetup_f_GreenTiltGain_Byte3		0x1807
#define SensorSetup_f_BlueTiltGain_Byte0		0x1808
#define SensorSetup_f_BlueTiltGain_Byte1		0x1809
#define SensorSetup_f_BlueTiltGain_Byte2		0x180a
#define SensorSetup_f_BlueTiltGain_Byte3		0x180b
#define SensorSetup_u16_GuaranteedDataSaturationLevel_Byte0		0x180c
#define SensorSetup_u16_GuaranteedDataSaturationLevel_Byte1		0x180d
#define SensorSetup_u16_MinimumSensorRxPixelValue_Byte0		0x180e
#define SensorSetup_u16_MinimumSensorRxPixelValue_Byte1		0x180f
#define SensorSetup_u16_MaximumSensorRxPixelValue_Byte0		0x1810
#define SensorSetup_u16_MaximumSensorRxPixelValue_Byte1		0x1811
#define SensorSetup_u8_BlackCorrectionOffset_Byte0		0x1812

// page 'g_RSO_Control'

#define RSO_Control_e_Flag_EnableRSO_Byte0		0x1840
#define RSO_Control_e_RSO_Mode_Control_Byte0		0x1841

// page 'g_RSO_DataCtrl'

#define RSO_DataCtrl_u32_XCoefGr_Byte0		0x1880
#define RSO_DataCtrl_u32_XCoefGr_Byte1		0x1881
#define RSO_DataCtrl_u32_XCoefGr_Byte2		0x1882
#define RSO_DataCtrl_u32_XCoefGr_Byte3		0x1883
#define RSO_DataCtrl_u32_YCoefGr_Byte0		0x1884
#define RSO_DataCtrl_u32_YCoefGr_Byte1		0x1885
#define RSO_DataCtrl_u32_YCoefGr_Byte2		0x1886
#define RSO_DataCtrl_u32_YCoefGr_Byte3		0x1887
#define RSO_DataCtrl_u32_XCoefR_Byte0		0x1888
#define RSO_DataCtrl_u32_XCoefR_Byte1		0x1889
#define RSO_DataCtrl_u32_XCoefR_Byte2		0x188a
#define RSO_DataCtrl_u32_XCoefR_Byte3		0x188b
#define RSO_DataCtrl_u32_YCoefR_Byte0		0x188c
#define RSO_DataCtrl_u32_YCoefR_Byte1		0x188d
#define RSO_DataCtrl_u32_YCoefR_Byte2		0x188e
#define RSO_DataCtrl_u32_YCoefR_Byte3		0x188f
#define RSO_DataCtrl_u32_XCoefB_Byte0		0x1890
#define RSO_DataCtrl_u32_XCoefB_Byte1		0x1891
#define RSO_DataCtrl_u32_XCoefB_Byte2		0x1892
#define RSO_DataCtrl_u32_XCoefB_Byte3		0x1893
#define RSO_DataCtrl_u32_YCoefB_Byte0		0x1894
#define RSO_DataCtrl_u32_YCoefB_Byte1		0x1895
#define RSO_DataCtrl_u32_YCoefB_Byte2		0x1896
#define RSO_DataCtrl_u32_YCoefB_Byte3		0x1897
#define RSO_DataCtrl_u32_XCoefGb_Byte0		0x1898
#define RSO_DataCtrl_u32_XCoefGb_Byte1		0x1899
#define RSO_DataCtrl_u32_XCoefGb_Byte2		0x189a
#define RSO_DataCtrl_u32_XCoefGb_Byte3		0x189b
#define RSO_DataCtrl_u32_YCoefGb_Byte0		0x189c
#define RSO_DataCtrl_u32_YCoefGb_Byte1		0x189d
#define RSO_DataCtrl_u32_YCoefGb_Byte2		0x189e
#define RSO_DataCtrl_u32_YCoefGb_Byte3		0x189f
#define RSO_DataCtrl_u16_DcTermGr_Byte0		0x18a0
#define RSO_DataCtrl_u16_DcTermGr_Byte1		0x18a1
#define RSO_DataCtrl_u16_DcTermR_Byte0		0x18a2
#define RSO_DataCtrl_u16_DcTermR_Byte1		0x18a3
#define RSO_DataCtrl_u16_DcTermB_Byte0		0x18a4
#define RSO_DataCtrl_u16_DcTermB_Byte1		0x18a5
#define RSO_DataCtrl_u16_DcTermGb_Byte0		0x18a6
#define RSO_DataCtrl_u16_DcTermGb_Byte1		0x18a7
#define RSO_DataCtrl_u16_XSlantOrigin_Byte0		0x18a8
#define RSO_DataCtrl_u16_XSlantOrigin_Byte1		0x18a9
#define RSO_DataCtrl_u16_YSlantOrigin_Byte0		0x18aa
#define RSO_DataCtrl_u16_YSlantOrigin_Byte1		0x18ab

// page 'g_Interrupts_Count'

#define Interrupts_Count_u16_INT23_CRM_Byte0		0x18c0
#define Interrupts_Count_u16_INT23_CRM_Byte1		0x18c1
#define Interrupts_Count_u16_INT20_STAT0_256_bins_histogram_Schedule_Byte0		0x18c2
#define Interrupts_Count_u16_INT20_STAT0_256_bins_histogram_Schedule_Byte1		0x18c3
#define Interrupts_Count_u16_INT17_GPIO_Byte0		0x18c4
#define Interrupts_Count_u16_INT17_GPIO_Byte1		0x18c5
#define Interrupts_Count_u16_INT16_STAT8_Glace_AWB_Byte0		0x18c6
#define Interrupts_Count_u16_INT16_STAT8_Glace_AWB_Byte1		0x18c7
#define Interrupts_Count_u16_INT15_STAT7_Acc_Wg_Zones_White_Balance_Byte0		0x18c8
#define Interrupts_Count_u16_INT15_STAT7_Acc_Wg_Zones_White_Balance_Byte1		0x18c9
#define Interrupts_Count_u16_INT13_STAT5_Visual_Significance_Byte0		0x18ca
#define Interrupts_Count_u16_INT13_STAT5_Visual_Significance_Byte1		0x18cb
#define Interrupts_Count_u16_INT12_STAT4_Skin_Tone_Detection_Byte0		0x18cc
#define Interrupts_Count_u16_INT12_STAT4_Skin_Tone_Detection_Byte1		0x18cd
#define Interrupts_Count_u16_INT11_STAT3_MWWB_White_Balance_Byte0		0x18ce
#define Interrupts_Count_u16_INT11_STAT3_MWWB_White_Balance_Byte1		0x18cf
#define Interrupts_Count_u16_INT10_STAT2_Auto_Focus_Byte0		0x18d0
#define Interrupts_Count_u16_INT10_STAT2_Auto_Focus_Byte1		0x18d1
#define Interrupts_Count_u16_INT09_STAT1_6X8_Exposure_Byte0		0x18d2
#define Interrupts_Count_u16_INT09_STAT1_6X8_Exposure_Byte1		0x18d3
#define Interrupts_Count_u16_INT08_STAT0_256_bins_histogram_Byte0		0x18d4
#define Interrupts_Count_u16_INT08_STAT0_256_bins_histogram_Byte1		0x18d5
#define Interrupts_Count_u16_INT07_VIDEOPIPE1_Byte0		0x18d6
#define Interrupts_Count_u16_INT07_VIDEOPIPE1_Byte1		0x18d7
#define Interrupts_Count_u16_INT06_VIDEOPIPE0_Byte0		0x18d8
#define Interrupts_Count_u16_INT06_VIDEOPIPE0_Byte1		0x18d9
#define Interrupts_Count_u16_INT05_SMIARX_Byte0		0x18da
#define Interrupts_Count_u16_INT05_SMIARX_Byte1		0x18db
#define Interrupts_Count_u16_INT05_SMIARX0_Byte0		0x18dc
#define Interrupts_Count_u16_INT05_SMIARX0_Byte1		0x18dd
#define Interrupts_Count_u16_INT05_SMIARX1_Byte0		0x18de
#define Interrupts_Count_u16_INT05_SMIARX1_Byte1		0x18df
#define Interrupts_Count_u16_INT05_SMIARX2_Byte0		0x18e0
#define Interrupts_Count_u16_INT05_SMIARX2_Byte1		0x18e1
#define Interrupts_Count_u16_INT05_SMIARX3_Byte0		0x18e2
#define Interrupts_Count_u16_INT05_SMIARX3_Byte1		0x18e3
#define Interrupts_Count_u16_INT05_SMIARX4_Byte0		0x18e4
#define Interrupts_Count_u16_INT05_SMIARX4_Byte1		0x18e5
#define Interrupts_Count_u16_INT04_reserved_Byte0		0x18e6
#define Interrupts_Count_u16_INT04_reserved_Byte1		0x18e7
#define Interrupts_Count_u16_INT03_Line_blaning_elimination_Byte0		0x18e8
#define Interrupts_Count_u16_INT03_Line_blaning_elimination_Byte1		0x18e9
#define Interrupts_Count_u16_INT02_MASTERCCI_Byte0		0x18ea
#define Interrupts_Count_u16_INT02_MASTERCCI_Byte1		0x18eb
#define Interrupts_Count_u16_INT01_STAT8_Glace_Schedule_Byte0		0x18ec
#define Interrupts_Count_u16_INT01_STAT8_Glace_Schedule_Byte1		0x18ed
#define Interrupts_Count_u16_INT00_USER_INTERFACE_Byte0		0x18ee
#define Interrupts_Count_u16_INT00_USER_INTERFACE_Byte1		0x18ef
#define Interrupts_Count_u16_INT27_ERROR_EOF_Byte0		0x18f0
#define Interrupts_Count_u16_INT27_ERROR_EOF_Byte1		0x18f1

// page 'g_Profile_BOOT'

#define Profile_BOOT_u32_MinimumCycle_Byte0		0x1900
#define Profile_BOOT_u32_MinimumCycle_Byte1		0x1901
#define Profile_BOOT_u32_MinimumCycle_Byte2		0x1902
#define Profile_BOOT_u32_MinimumCycle_Byte3		0x1903
#define Profile_BOOT_u32_MaximumCycle_Byte0		0x1904
#define Profile_BOOT_u32_MaximumCycle_Byte1		0x1905
#define Profile_BOOT_u32_MaximumCycle_Byte2		0x1906
#define Profile_BOOT_u32_MaximumCycle_Byte3		0x1907
#define Profile_BOOT_u32_AverageCycle_Byte0		0x1908
#define Profile_BOOT_u32_AverageCycle_Byte1		0x1909
#define Profile_BOOT_u32_AverageCycle_Byte2		0x190a
#define Profile_BOOT_u32_AverageCycle_Byte3		0x190b
#define Profile_BOOT_u32_NoOfIterations_Byte0		0x190c
#define Profile_BOOT_u32_NoOfIterations_Byte1		0x190d
#define Profile_BOOT_u32_NoOfIterations_Byte2		0x190e
#define Profile_BOOT_u32_NoOfIterations_Byte3		0x190f
#define Profile_BOOT_u32_CurrentCycles_Byte0		0x1910
#define Profile_BOOT_u32_CurrentCycles_Byte1		0x1911
#define Profile_BOOT_u32_CurrentCycles_Byte2		0x1912
#define Profile_BOOT_u32_CurrentCycles_Byte3		0x1913

// page 'g_Profile_SensorCommit'

#define Profile_SensorCommit_u32_MinimumCycle_Byte0		0x1940
#define Profile_SensorCommit_u32_MinimumCycle_Byte1		0x1941
#define Profile_SensorCommit_u32_MinimumCycle_Byte2		0x1942
#define Profile_SensorCommit_u32_MinimumCycle_Byte3		0x1943
#define Profile_SensorCommit_u32_MaximumCycle_Byte0		0x1944
#define Profile_SensorCommit_u32_MaximumCycle_Byte1		0x1945
#define Profile_SensorCommit_u32_MaximumCycle_Byte2		0x1946
#define Profile_SensorCommit_u32_MaximumCycle_Byte3		0x1947
#define Profile_SensorCommit_u32_AverageCycle_Byte0		0x1948
#define Profile_SensorCommit_u32_AverageCycle_Byte1		0x1949
#define Profile_SensorCommit_u32_AverageCycle_Byte2		0x194a
#define Profile_SensorCommit_u32_AverageCycle_Byte3		0x194b
#define Profile_SensorCommit_u32_NoOfIterations_Byte0		0x194c
#define Profile_SensorCommit_u32_NoOfIterations_Byte1		0x194d
#define Profile_SensorCommit_u32_NoOfIterations_Byte2		0x194e
#define Profile_SensorCommit_u32_NoOfIterations_Byte3		0x194f
#define Profile_SensorCommit_u32_CurrentCycles_Byte0		0x1950
#define Profile_SensorCommit_u32_CurrentCycles_Byte1		0x1951
#define Profile_SensorCommit_u32_CurrentCycles_Byte2		0x1952
#define Profile_SensorCommit_u32_CurrentCycles_Byte3		0x1953

// page 'g_Profile_LCO'

#define Profile_LCO_u32_MinimumCycle_Byte0		0x1980
#define Profile_LCO_u32_MinimumCycle_Byte1		0x1981
#define Profile_LCO_u32_MinimumCycle_Byte2		0x1982
#define Profile_LCO_u32_MinimumCycle_Byte3		0x1983
#define Profile_LCO_u32_MaximumCycle_Byte0		0x1984
#define Profile_LCO_u32_MaximumCycle_Byte1		0x1985
#define Profile_LCO_u32_MaximumCycle_Byte2		0x1986
#define Profile_LCO_u32_MaximumCycle_Byte3		0x1987
#define Profile_LCO_u32_AverageCycle_Byte0		0x1988
#define Profile_LCO_u32_AverageCycle_Byte1		0x1989
#define Profile_LCO_u32_AverageCycle_Byte2		0x198a
#define Profile_LCO_u32_AverageCycle_Byte3		0x198b
#define Profile_LCO_u32_NoOfIterations_Byte0		0x198c
#define Profile_LCO_u32_NoOfIterations_Byte1		0x198d
#define Profile_LCO_u32_NoOfIterations_Byte2		0x198e
#define Profile_LCO_u32_NoOfIterations_Byte3		0x198f
#define Profile_LCO_u32_CurrentCycles_Byte0		0x1990
#define Profile_LCO_u32_CurrentCycles_Byte1		0x1991
#define Profile_LCO_u32_CurrentCycles_Byte2		0x1992
#define Profile_LCO_u32_CurrentCycles_Byte3		0x1993

// page 'g_Profile_ISPUpdate'

#define Profile_ISPUpdate_u32_MinimumCycle_Byte0		0x19c0
#define Profile_ISPUpdate_u32_MinimumCycle_Byte1		0x19c1
#define Profile_ISPUpdate_u32_MinimumCycle_Byte2		0x19c2
#define Profile_ISPUpdate_u32_MinimumCycle_Byte3		0x19c3
#define Profile_ISPUpdate_u32_MaximumCycle_Byte0		0x19c4
#define Profile_ISPUpdate_u32_MaximumCycle_Byte1		0x19c5
#define Profile_ISPUpdate_u32_MaximumCycle_Byte2		0x19c6
#define Profile_ISPUpdate_u32_MaximumCycle_Byte3		0x19c7
#define Profile_ISPUpdate_u32_AverageCycle_Byte0		0x19c8
#define Profile_ISPUpdate_u32_AverageCycle_Byte1		0x19c9
#define Profile_ISPUpdate_u32_AverageCycle_Byte2		0x19ca
#define Profile_ISPUpdate_u32_AverageCycle_Byte3		0x19cb
#define Profile_ISPUpdate_u32_NoOfIterations_Byte0		0x19cc
#define Profile_ISPUpdate_u32_NoOfIterations_Byte1		0x19cd
#define Profile_ISPUpdate_u32_NoOfIterations_Byte2		0x19ce
#define Profile_ISPUpdate_u32_NoOfIterations_Byte3		0x19cf
#define Profile_ISPUpdate_u32_CurrentCycles_Byte0		0x19d0
#define Profile_ISPUpdate_u32_CurrentCycles_Byte1		0x19d1
#define Profile_ISPUpdate_u32_CurrentCycles_Byte2		0x19d2
#define Profile_ISPUpdate_u32_CurrentCycles_Byte3		0x19d3

// page 'g_Profile_AEC_Statistics'

#define Profile_AEC_Statistics_u32_MinimumCycle_Byte0		0x1a00
#define Profile_AEC_Statistics_u32_MinimumCycle_Byte1		0x1a01
#define Profile_AEC_Statistics_u32_MinimumCycle_Byte2		0x1a02
#define Profile_AEC_Statistics_u32_MinimumCycle_Byte3		0x1a03
#define Profile_AEC_Statistics_u32_MaximumCycle_Byte0		0x1a04
#define Profile_AEC_Statistics_u32_MaximumCycle_Byte1		0x1a05
#define Profile_AEC_Statistics_u32_MaximumCycle_Byte2		0x1a06
#define Profile_AEC_Statistics_u32_MaximumCycle_Byte3		0x1a07
#define Profile_AEC_Statistics_u32_AverageCycle_Byte0		0x1a08
#define Profile_AEC_Statistics_u32_AverageCycle_Byte1		0x1a09
#define Profile_AEC_Statistics_u32_AverageCycle_Byte2		0x1a0a
#define Profile_AEC_Statistics_u32_AverageCycle_Byte3		0x1a0b
#define Profile_AEC_Statistics_u32_NoOfIterations_Byte0		0x1a0c
#define Profile_AEC_Statistics_u32_NoOfIterations_Byte1		0x1a0d
#define Profile_AEC_Statistics_u32_NoOfIterations_Byte2		0x1a0e
#define Profile_AEC_Statistics_u32_NoOfIterations_Byte3		0x1a0f
#define Profile_AEC_Statistics_u32_CurrentCycles_Byte0		0x1a10
#define Profile_AEC_Statistics_u32_CurrentCycles_Byte1		0x1a11
#define Profile_AEC_Statistics_u32_CurrentCycles_Byte2		0x1a12
#define Profile_AEC_Statistics_u32_CurrentCycles_Byte3		0x1a13

// page 'g_Profile_AWB_Statistics'

#define Profile_AWB_Statistics_u32_MinimumCycle_Byte0		0x1a40
#define Profile_AWB_Statistics_u32_MinimumCycle_Byte1		0x1a41
#define Profile_AWB_Statistics_u32_MinimumCycle_Byte2		0x1a42
#define Profile_AWB_Statistics_u32_MinimumCycle_Byte3		0x1a43
#define Profile_AWB_Statistics_u32_MaximumCycle_Byte0		0x1a44
#define Profile_AWB_Statistics_u32_MaximumCycle_Byte1		0x1a45
#define Profile_AWB_Statistics_u32_MaximumCycle_Byte2		0x1a46
#define Profile_AWB_Statistics_u32_MaximumCycle_Byte3		0x1a47
#define Profile_AWB_Statistics_u32_AverageCycle_Byte0		0x1a48
#define Profile_AWB_Statistics_u32_AverageCycle_Byte1		0x1a49
#define Profile_AWB_Statistics_u32_AverageCycle_Byte2		0x1a4a
#define Profile_AWB_Statistics_u32_AverageCycle_Byte3		0x1a4b
#define Profile_AWB_Statistics_u32_NoOfIterations_Byte0		0x1a4c
#define Profile_AWB_Statistics_u32_NoOfIterations_Byte1		0x1a4d
#define Profile_AWB_Statistics_u32_NoOfIterations_Byte2		0x1a4e
#define Profile_AWB_Statistics_u32_NoOfIterations_Byte3		0x1a4f
#define Profile_AWB_Statistics_u32_CurrentCycles_Byte0		0x1a50
#define Profile_AWB_Statistics_u32_CurrentCycles_Byte1		0x1a51
#define Profile_AWB_Statistics_u32_CurrentCycles_Byte2		0x1a52
#define Profile_AWB_Statistics_u32_CurrentCycles_Byte3		0x1a53

// page 'g_Profile_VID0'

#define Profile_VID0_u32_MinimumCycle_Byte0		0x1a80
#define Profile_VID0_u32_MinimumCycle_Byte1		0x1a81
#define Profile_VID0_u32_MinimumCycle_Byte2		0x1a82
#define Profile_VID0_u32_MinimumCycle_Byte3		0x1a83
#define Profile_VID0_u32_MaximumCycle_Byte0		0x1a84
#define Profile_VID0_u32_MaximumCycle_Byte1		0x1a85
#define Profile_VID0_u32_MaximumCycle_Byte2		0x1a86
#define Profile_VID0_u32_MaximumCycle_Byte3		0x1a87
#define Profile_VID0_u32_AverageCycle_Byte0		0x1a88
#define Profile_VID0_u32_AverageCycle_Byte1		0x1a89
#define Profile_VID0_u32_AverageCycle_Byte2		0x1a8a
#define Profile_VID0_u32_AverageCycle_Byte3		0x1a8b
#define Profile_VID0_u32_NoOfIterations_Byte0		0x1a8c
#define Profile_VID0_u32_NoOfIterations_Byte1		0x1a8d
#define Profile_VID0_u32_NoOfIterations_Byte2		0x1a8e
#define Profile_VID0_u32_NoOfIterations_Byte3		0x1a8f
#define Profile_VID0_u32_CurrentCycles_Byte0		0x1a90
#define Profile_VID0_u32_CurrentCycles_Byte1		0x1a91
#define Profile_VID0_u32_CurrentCycles_Byte2		0x1a92
#define Profile_VID0_u32_CurrentCycles_Byte3		0x1a93

// page 'g_Profile_VID1'

#define Profile_VID1_u32_MinimumCycle_Byte0		0x1ac0
#define Profile_VID1_u32_MinimumCycle_Byte1		0x1ac1
#define Profile_VID1_u32_MinimumCycle_Byte2		0x1ac2
#define Profile_VID1_u32_MinimumCycle_Byte3		0x1ac3
#define Profile_VID1_u32_MaximumCycle_Byte0		0x1ac4
#define Profile_VID1_u32_MaximumCycle_Byte1		0x1ac5
#define Profile_VID1_u32_MaximumCycle_Byte2		0x1ac6
#define Profile_VID1_u32_MaximumCycle_Byte3		0x1ac7
#define Profile_VID1_u32_AverageCycle_Byte0		0x1ac8
#define Profile_VID1_u32_AverageCycle_Byte1		0x1ac9
#define Profile_VID1_u32_AverageCycle_Byte2		0x1aca
#define Profile_VID1_u32_AverageCycle_Byte3		0x1acb
#define Profile_VID1_u32_NoOfIterations_Byte0		0x1acc
#define Profile_VID1_u32_NoOfIterations_Byte1		0x1acd
#define Profile_VID1_u32_NoOfIterations_Byte2		0x1ace
#define Profile_VID1_u32_NoOfIterations_Byte3		0x1acf
#define Profile_VID1_u32_CurrentCycles_Byte0		0x1ad0
#define Profile_VID1_u32_CurrentCycles_Byte1		0x1ad1
#define Profile_VID1_u32_CurrentCycles_Byte2		0x1ad2
#define Profile_VID1_u32_CurrentCycles_Byte3		0x1ad3

// page 'g_Pipe_Scalar_StripeInputs[0]'

#define Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0		0x1b00
#define Pipe_Scalar_StripeInputs_0_u8_StripeCount_Byte0		0x1b01

// page 'g_Pipe_Scalar_StripeOutputs[0]'

#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHStart_Byte0		0x1b40
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHStart_Byte1		0x1b41
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropVStart_Byte0		0x1b42
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropVStart_Byte1		0x1b43
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0		0x1b44
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte1		0x1b45
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropVSize_Byte0		0x1b46
#define Pipe_Scalar_StripeOutputs_0_s16_StripeInCropVSize_Byte1		0x1b47
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHStart_Byte0		0x1b48
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHStart_Byte1		0x1b49
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropVStart_Byte0		0x1b4a
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropVStart_Byte1		0x1b4b
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHSize_Byte0		0x1b4c
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropHSize_Byte1		0x1b4d
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropVSize_Byte0		0x1b4e
#define Pipe_Scalar_StripeOutputs_0_s16_StripeOutCropVSize_Byte1		0x1b4f
#define Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte0		0x1b50
#define Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte1		0x1b51
#define Pipe_Scalar_StripeOutputs_0_s16_StripeScalarOutputHSize_Byte0		0x1b52
#define Pipe_Scalar_StripeOutputs_0_s16_StripeScalarOutputHSize_Byte1		0x1b53
#define Pipe_Scalar_StripeOutputs_0_s16_StripeScalarOutputVSize_Byte0		0x1b54
#define Pipe_Scalar_StripeOutputs_0_s16_StripeScalarOutputVSize_Byte1		0x1b55
#define Pipe_Scalar_StripeOutputs_0_s16_SScalerInputHSize_Byte0		0x1b56
#define Pipe_Scalar_StripeOutputs_0_s16_SScalerInputHSize_Byte1		0x1b57
#define Pipe_Scalar_StripeOutputs_0_s16_SScalerInputVSize_Byte0		0x1b58
#define Pipe_Scalar_StripeOutputs_0_s16_SScalerInputVSize_Byte1		0x1b59

// page 'g_Pipe_Scalar_StripeInputs[1]'

#define Pipe_Scalar_StripeInputs_1_u8_StripeIndex_Byte0		0x1b80
#define Pipe_Scalar_StripeInputs_1_u8_StripeCount_Byte0		0x1b81

// page 'g_Pipe_Scalar_StripeOutputs[1]'

#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHStart_Byte0		0x1bc0
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHStart_Byte1		0x1bc1
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropVStart_Byte0		0x1bc2
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropVStart_Byte1		0x1bc3
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHSize_Byte0		0x1bc4
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropHSize_Byte1		0x1bc5
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropVSize_Byte0		0x1bc6
#define Pipe_Scalar_StripeOutputs_1_s16_StripeInCropVSize_Byte1		0x1bc7
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHStart_Byte0		0x1bc8
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHStart_Byte1		0x1bc9
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropVStart_Byte0		0x1bca
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropVStart_Byte1		0x1bcb
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHSize_Byte0		0x1bcc
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropHSize_Byte1		0x1bcd
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropVSize_Byte0		0x1bce
#define Pipe_Scalar_StripeOutputs_1_s16_StripeOutCropVSize_Byte1		0x1bcf
#define Pipe_Scalar_StripeOutputs_1_s16_MinLineLength_Byte0		0x1bd0
#define Pipe_Scalar_StripeOutputs_1_s16_MinLineLength_Byte1		0x1bd1
#define Pipe_Scalar_StripeOutputs_1_s16_StripeScalarOutputHSize_Byte0		0x1bd2
#define Pipe_Scalar_StripeOutputs_1_s16_StripeScalarOutputHSize_Byte1		0x1bd3
#define Pipe_Scalar_StripeOutputs_1_s16_StripeScalarOutputVSize_Byte0		0x1bd4
#define Pipe_Scalar_StripeOutputs_1_s16_StripeScalarOutputVSize_Byte1		0x1bd5
#define Pipe_Scalar_StripeOutputs_1_s16_SScalerInputHSize_Byte0		0x1bd6
#define Pipe_Scalar_StripeOutputs_1_s16_SScalerInputHSize_Byte1		0x1bd7
#define Pipe_Scalar_StripeOutputs_1_s16_SScalerInputVSize_Byte0		0x1bd8
#define Pipe_Scalar_StripeOutputs_1_s16_SScalerInputVSize_Byte1		0x1bd9

// page 'g_CSIControl'

#define CSIControl_u16_DataLanesMapCSI2_0_Byte0		0x1c00
#define CSIControl_u16_DataLanesMapCSI2_0_Byte1		0x1c01
#define CSIControl_u16_DataLanesMapCSI2_1_Byte0		0x1c02
#define CSIControl_u16_DataLanesMapCSI2_1_Byte1		0x1c03
#define CSIControl_u8_DPHY0Ctrl_Byte0		0x1c04
#define CSIControl_u8_DPHY1Ctrl_Byte0		0x1c05
#define CSIControl_e_SensorCSI2Version_0_Byte0		0x1c06
#define CSIControl_e_SensorCSI2Version_1_Byte0		0x1c07
#define CSIControl_u8_DPHY0Ctrl_4th_lane_Byte0		0x1c08

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_CE_GammaControl[0]'

#define CE_GammaControl_0_e_GammaCurve_Byte0		0x1c80
#define CE_GammaControl_0_u8_GammaPixelInShift_Sharp_Byte0		0x1c81
#define CE_GammaControl_0_u8_GammaPixelInShift_UnSharp_Byte0		0x1c82
#define CE_GammaControl_0_e_Coin_Ctrl_Byte0		0x1c83

// page 'g_CE_GammaControl[1]'

#define CE_GammaControl_1_e_GammaCurve_Byte0		0x1cc0
#define CE_GammaControl_1_u8_GammaPixelInShift_Sharp_Byte0		0x1cc1
#define CE_GammaControl_1_u8_GammaPixelInShift_UnSharp_Byte0		0x1cc2
#define CE_GammaControl_1_e_Coin_Ctrl_Byte0		0x1cc3

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_CE_ColourMatrixCtrl[0]'

#define CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0		0x1d40
#define CE_ColourMatrixCtrl_0_s16_Offset_R_Byte1		0x1d41
#define CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0		0x1d42
#define CE_ColourMatrixCtrl_0_s16_Offset_G_Byte1		0x1d43
#define CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0		0x1d44
#define CE_ColourMatrixCtrl_0_s16_Offset_B_Byte1		0x1d45

// page 'g_CE_ColourMatrixCtrl[1]'

#define CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0		0x1d80
#define CE_ColourMatrixCtrl_1_s16_Offset_R_Byte1		0x1d81
#define CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0		0x1d82
#define CE_ColourMatrixCtrl_1_s16_Offset_G_Byte1		0x1d83
#define CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0		0x1d84
#define CE_ColourMatrixCtrl_1_s16_Offset_B_Byte1		0x1d85

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_FLADriver_NVMStoredData'

#define FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte0		0x1e00
#define FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte1		0x1e01
#define FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte2		0x1e02
#define FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte3		0x1e03
#define FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte0		0x1e04
#define FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte1		0x1e05
#define FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte2		0x1e06
#define FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte3		0x1e07
#define FLADriver_NVMStoredData_s32_NVMHyperfocalPos_Byte0		0x1e08
#define FLADriver_NVMStoredData_s32_NVMHyperfocalPos_Byte1		0x1e09
#define FLADriver_NVMStoredData_s32_NVMHyperfocalPos_Byte2		0x1e0a
#define FLADriver_NVMStoredData_s32_NVMHyperfocalPos_Byte3		0x1e0b
#define FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte0		0x1e0c
#define FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte1		0x1e0d
#define FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte2		0x1e0e
#define FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte3		0x1e0f
#define FLADriver_NVMStoredData_s32_NVMRestPos_Byte0		0x1e10
#define FLADriver_NVMStoredData_s32_NVMRestPos_Byte1		0x1e11
#define FLADriver_NVMStoredData_s32_NVMRestPos_Byte2		0x1e12
#define FLADriver_NVMStoredData_s32_NVMRestPos_Byte3		0x1e13
#define FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte0		0x1e14
#define FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte1		0x1e15
#define FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte2		0x1e16
#define FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte3		0x1e17
#define FLADriver_NVMStoredData_e_Flag_NVMDataPresent_Byte0		0x1e18
#define FLADriver_NVMStoredData_e_Flag_NVMActuatorLensPresent_Byte0		0x1e19
#define FLADriver_NVMStoredData_e_Flag_NVMPositionSensorPresent_Byte0		0x1e1a

// page 'g_FLADriver_Controls'

#define FLADriver_Controls_u16_Ctrl_TimeLimit_ms_Byte0		0x1e40
#define FLADriver_Controls_u16_Ctrl_TimeLimit_ms_Byte1		0x1e41
#define FLADriver_Controls_e_FLADriver_RangeDef_CtrlRange_Byte0		0x1e42
#define FLADriver_Controls_u8_FrameTolerance_Byte0		0x1e43

// page 'g_FLADriver_LLLCtrlStatusParam'

#define FLADriver_LLLCtrlStatusParam_u16_MinPos_Byte0		0x1e80
#define FLADriver_LLLCtrlStatusParam_u16_MinPos_Byte1		0x1e81
#define FLADriver_LLLCtrlStatusParam_u16_MaxPos_Byte0		0x1e82
#define FLADriver_LLLCtrlStatusParam_u16_MaxPos_Byte1		0x1e83
#define FLADriver_LLLCtrlStatusParam_u16_RestPos_Byte0		0x1e84
#define FLADriver_LLLCtrlStatusParam_u16_RestPos_Byte1		0x1e85
#define FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte0		0x1e86
#define FLADriver_LLLCtrlStatusParam_u16_InfinityFarEndPos_Byte1		0x1e87
#define FLADriver_LLLCtrlStatusParam_u16_InfinityHorPos_Byte0		0x1e88
#define FLADriver_LLLCtrlStatusParam_u16_InfinityHorPos_Byte1		0x1e89
#define FLADriver_LLLCtrlStatusParam_u16_HyperFocalPos_Byte0		0x1e8a
#define FLADriver_LLLCtrlStatusParam_u16_HyperFocalPos_Byte1		0x1e8b
#define FLADriver_LLLCtrlStatusParam_u16_MacroHorPos_Byte0		0x1e8c
#define FLADriver_LLLCtrlStatusParam_u16_MacroHorPos_Byte1		0x1e8d
#define FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte0		0x1e8e
#define FLADriver_LLLCtrlStatusParam_u16_MacroNearEndPos_Byte1		0x1e8f
#define FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte0		0x1e90
#define FLADriver_LLLCtrlStatusParam_s16_ToleranceSize_Byte1		0x1e91
#define FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0		0x1e92
#define FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte1		0x1e93
#define FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0		0x1e94
#define FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte1		0x1e95
#define FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte0		0x1e96
#define FLADriver_LLLCtrlStatusParam_u16_CurrentPos_Byte1		0x1e97
#define FLADriver_LLLCtrlStatusParam_u16_TargetPos_Byte0		0x1e98
#define FLADriver_LLLCtrlStatusParam_u16_TargetPos_Byte1		0x1e99
#define FLADriver_LLLCtrlStatusParam_u16_PreviousPos_Byte0		0x1e9a
#define FLADriver_LLLCtrlStatusParam_u16_PreviousPos_Byte1		0x1e9b
#define FLADriver_LLLCtrlStatusParam_s16_LastStepSizeExecuted_Byte0		0x1e9c
#define FLADriver_LLLCtrlStatusParam_s16_LastStepSizeExecuted_Byte1		0x1e9d
#define FLADriver_LLLCtrlStatusParam_u8_Error_Byte0		0x1e9e

// page 'g_FLADriver_Status'

#define FLADriver_Status_u16_Cycles_Byte0		0x1ec0
#define FLADriver_Status_u16_Cycles_Byte1		0x1ec1
#define FLADriver_Status_e_Flag_LensIsMoving_Byte0		0x1ec2
#define FLADriver_Status_e_Flag_LimitsExceeded_Byte0		0x1ec3
#define FLADriver_Status_e_Flag_LowLevelDriverInitialized_Byte0		0x1ec4
#define FLADriver_Status_e_FLADriver_ActuatorOrientation_Type_Byte0		0x1ec5
#define FLADriver_Status_e_FLADriver_RangeDef_StatusRange_Byte0		0x1ec6

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_FocusControl_Controls'

#define FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0		0x1f80
#define FocusControl_Controls_e_Coin_Control_Byte0		0x1f81
#define FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0		0x1f82

// page 'g_FocusControl_Status'

#define FocusControl_Status_u16_Cycles_Byte0		0x1fc0
#define FocusControl_Status_u16_Cycles_Byte1		0x1fc1
#define FocusControl_Status_e_FocusControl_LensCommand_Status_Byte0		0x1fc2
#define FocusControl_Status_e_FocusControl_FocusMsg_Status_Byte0		0x1fc3
#define FocusControl_Status_e_Coin_Status_Byte0		0x1fc4
#define FocusControl_Status_e_Flag_LensIsMovingAtTheSOF_Byte0		0x1fc5
#define FocusControl_Status_e_Flag_IsStable_Byte0		0x1fc6
#define FocusControl_Status_e_Flag_Error_Byte0		0x1fc7
#define FocusControl_Status_e_Flag_StatsWithLensMove_Status_Byte0		0x1fc8

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_AFStats_Controls'

#define AFStats_Controls_pu32_HostAssignedAddr_Byte0		0x2040
#define AFStats_Controls_pu32_HostAssignedAddr_Byte1		0x2041
#define AFStats_Controls_pu32_HostAssignedAddr_Byte2		0x2042
#define AFStats_Controls_pu32_HostAssignedAddr_Byte3		0x2043
#define AFStats_Controls_u8_CoringValue_Byte0		0x2044
#define AFStats_Controls_u8_HRatioNum_Byte0		0x2045
#define AFStats_Controls_u8_HRatioDen_Byte0		0x2046
#define AFStats_Controls_u8_VRatioNum_Byte0		0x2047
#define AFStats_Controls_u8_VRatioDen_Byte0		0x2048
#define AFStats_Controls_u8_HostActiveZonesCounter_Byte0		0x2049
#define AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0		0x204a
#define AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0		0x204b
#define AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0		0x204c
#define AFStats_Controls_e_Flag_AutoRefresh_Byte0		0x204d
#define AFStats_Controls_e_Flag_AbsSquareEnabled_Byte0		0x204e
#define AFStats_Controls_e_Flag_ReducedZoneSetup_Byte0		0x204f
#define AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0		0x2050
#define AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0		0x2051
#define AFStats_Controls_e_Flag_AFStatsCancel_Byte0		0x2052

// page 'g_AFStats_Status'

#define AFStats_Status_u32_MaxFocusMeasurePerPixel_Byte0		0x2080
#define AFStats_Status_u32_MaxFocusMeasurePerPixel_Byte1		0x2081
#define AFStats_Status_u32_MaxFocusMeasurePerPixel_Byte2		0x2082
#define AFStats_Status_u32_MaxFocusMeasurePerPixel_Byte3		0x2083
#define AFStats_Status_u16_StartingAFZoneLine_Byte0		0x2084
#define AFStats_Status_u16_StartingAFZoneLine_Byte1		0x2085
#define AFStats_Status_u16_WOIWidth_Byte0		0x2086
#define AFStats_Status_u16_WOIWidth_Byte1		0x2087
#define AFStats_Status_u16_WOIHeight_Byte0		0x2088
#define AFStats_Status_u16_WOIHeight_Byte1		0x2089
#define AFStats_Status_u16_AFZonesWidth_Byte0		0x208a
#define AFStats_Status_u16_AFZonesWidth_Byte1		0x208b
#define AFStats_Status_u16_AFZonesHeight_Byte0		0x208c
#define AFStats_Status_u16_AFZonesHeight_Byte1		0x208d
#define AFStats_Status_u8_CoringValue_Byte0		0x208e
#define AFStats_Status_u8_ActiveZonesCounter_Byte0		0x208f
#define AFStats_Status_u8_HRatioNum_Byte0		0x2090
#define AFStats_Status_u8_HRatioDen_Byte0		0x2091
#define AFStats_Status_u8_VRatioNum_Byte0		0x2092
#define AFStats_Status_u8_VRatioDen_Byte0		0x2093
#define AFStats_Status_s8_ZoneIntCycles_Byte0		0x2094
#define AFStats_Status_u8_IndexMax_Byte0		0x2095
#define AFStats_Status_u8_IndexMin_Byte0		0x2096
#define AFStats_Status_e_AFStats_WindowsSystem_Status_Byte0		0x2097
#define AFStats_Status_e_AFStats_Error_Status_Byte0		0x2098
#define AFStats_Status_e_Coin_ZoneConfigStatus_Byte0		0x2099
#define AFStats_Status_e_AFStats_StatusHostCmd_Status_Byte0		0x209a
#define AFStats_Status_e_Flag_ForcedAFStatsIrq_Byte0		0x209b
#define AFStats_Status_e_Flag_AbsSquareEnabled_Byte0		0x209c
#define AFStats_Status_e_Coin_AFStatsExportStatus_Byte0		0x209d
#define AFStats_Status_e_Flag_FW_LensWithStatsStatus_Byte0		0x209e
#define AFStats_Status_u8_focus_stats_Frame_ID_Byte0		0x209f

// page 'g_AFStats_AFZoneInt'

#define AFStats_AFZoneInt_u16_INT00_AUTOFOCUS_Byte0		0x20c0
#define AFStats_AFZoneInt_u16_INT00_AUTOFOCUS_Byte1		0x20c1
#define AFStats_AFZoneInt_u16_INT01_AUTOFOCUS_Byte0		0x20c2
#define AFStats_AFZoneInt_u16_INT01_AUTOFOCUS_Byte1		0x20c3
#define AFStats_AFZoneInt_u16_INT02_AUTOFOCUS_Byte0		0x20c4
#define AFStats_AFZoneInt_u16_INT02_AUTOFOCUS_Byte1		0x20c5
#define AFStats_AFZoneInt_u16_INT03_AUTOFOCUS_Byte0		0x20c6
#define AFStats_AFZoneInt_u16_INT03_AUTOFOCUS_Byte1		0x20c7
#define AFStats_AFZoneInt_u16_INT04_AUTOFOCUS_Byte0		0x20c8
#define AFStats_AFZoneInt_u16_INT04_AUTOFOCUS_Byte1		0x20c9
#define AFStats_AFZoneInt_u16_INT05_AUTOFOCUS_Byte0		0x20ca
#define AFStats_AFZoneInt_u16_INT05_AUTOFOCUS_Byte1		0x20cb
#define AFStats_AFZoneInt_u16_INT06_AUTOFOCUS_Byte0		0x20cc
#define AFStats_AFZoneInt_u16_INT06_AUTOFOCUS_Byte1		0x20cd
#define AFStats_AFZoneInt_u16_INT07_AUTOFOCUS_Byte0		0x20ce
#define AFStats_AFZoneInt_u16_INT07_AUTOFOCUS_Byte1		0x20cf
#define AFStats_AFZoneInt_u16_INT08_AUTOFOCUS_Byte0		0x20d0
#define AFStats_AFZoneInt_u16_INT08_AUTOFOCUS_Byte1		0x20d1
#define AFStats_AFZoneInt_u16_INT09_AUTOFOCUS_Byte0		0x20d2
#define AFStats_AFZoneInt_u16_INT09_AUTOFOCUS_Byte1		0x20d3

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_AFStats_Debug'

#define AFStats_Debug_u32_dummy_Byte0		0x2180
#define AFStats_Debug_u32_dummy_Byte1		0x2181
#define AFStats_Debug_u32_dummy_Byte2		0x2182
#define AFStats_Debug_u32_dummy_Byte3		0x2183

// page 'g_AFStats_HostZoneConfigPercentage[0]'

#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x21c0
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x21c1
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x21c2
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x21c3
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x21c4
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x21c5
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x21c6
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x21c7
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x21c8
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x21c9
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x21ca
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x21cb
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x21cc
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x21cd
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x21ce
#define AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x21cf
#define AFStats_HostZoneConfigPercentage_0_e_Flag_Enabled_Byte0		0x21d0

// page 'g_AFStats_HostZoneConfigPercentage[1]'

#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2200
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2201
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2202
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2203
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2204
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2205
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2206
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2207
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2208
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2209
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x220a
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x220b
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x220c
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x220d
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x220e
#define AFStats_HostZoneConfigPercentage_1_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x220f
#define AFStats_HostZoneConfigPercentage_1_e_Flag_Enabled_Byte0		0x2210

// page 'g_AFStats_HostZoneConfigPercentage[2]'

#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2240
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2241
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2242
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2243
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2244
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2245
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2246
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2247
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2248
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2249
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x224a
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x224b
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x224c
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x224d
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x224e
#define AFStats_HostZoneConfigPercentage_2_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x224f
#define AFStats_HostZoneConfigPercentage_2_e_Flag_Enabled_Byte0		0x2250

// page 'g_AFStats_HostZoneConfigPercentage[3]'

#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2280
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2281
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2282
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2283
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2284
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2285
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2286
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2287
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2288
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2289
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x228a
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x228b
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x228c
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x228d
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x228e
#define AFStats_HostZoneConfigPercentage_3_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x228f
#define AFStats_HostZoneConfigPercentage_3_e_Flag_Enabled_Byte0		0x2290

// page 'g_AFStats_HostZoneConfigPercentage[4]'

#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x22c0
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x22c1
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x22c2
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x22c3
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x22c4
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x22c5
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x22c6
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x22c7
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x22c8
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x22c9
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x22ca
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x22cb
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x22cc
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x22cd
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x22ce
#define AFStats_HostZoneConfigPercentage_4_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x22cf
#define AFStats_HostZoneConfigPercentage_4_e_Flag_Enabled_Byte0		0x22d0

// page 'g_AFStats_HostZoneConfigPercentage[5]'

#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2300
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2301
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2302
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2303
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2304
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2305
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2306
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2307
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2308
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2309
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x230a
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x230b
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x230c
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x230d
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x230e
#define AFStats_HostZoneConfigPercentage_5_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x230f
#define AFStats_HostZoneConfigPercentage_5_e_Flag_Enabled_Byte0		0x2310

// page 'g_AFStats_HostZoneConfigPercentage[6]'

#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2340
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2341
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2342
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2343
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2344
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2345
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2346
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2347
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2348
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2349
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x234a
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x234b
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x234c
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x234d
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x234e
#define AFStats_HostZoneConfigPercentage_6_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x234f
#define AFStats_HostZoneConfigPercentage_6_e_Flag_Enabled_Byte0		0x2350

// page 'g_AFStats_HostZoneConfigPercentage[7]'

#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2380
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2381
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2382
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2383
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2384
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2385
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2386
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2387
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2388
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2389
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x238a
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x238b
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x238c
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x238d
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x238e
#define AFStats_HostZoneConfigPercentage_7_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x238f
#define AFStats_HostZoneConfigPercentage_7_e_Flag_Enabled_Byte0		0x2390

// page 'g_AFStats_HostZoneConfigPercentage[8]'

#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x23c0
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x23c1
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x23c2
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x23c3
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x23c4
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x23c5
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x23c6
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x23c7
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x23c8
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x23c9
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x23ca
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x23cb
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x23cc
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x23cd
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x23ce
#define AFStats_HostZoneConfigPercentage_8_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x23cf
#define AFStats_HostZoneConfigPercentage_8_e_Flag_Enabled_Byte0		0x23d0

// page 'g_AFStats_HostZoneConfigPercentage[9]'

#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0		0x2400
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte1		0x2401
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte2		0x2402
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte3		0x2403
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0		0x2404
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte1		0x2405
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte2		0x2406
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte3		0x2407
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0		0x2408
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte1		0x2409
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte2		0x240a
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte3		0x240b
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0		0x240c
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte1		0x240d
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte2		0x240e
#define AFStats_HostZoneConfigPercentage_9_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte3		0x240f
#define AFStats_HostZoneConfigPercentage_9_e_Flag_Enabled_Byte0		0x2410

// page 'g_AFStats_HostZoneConfig[0]'

#define AFStats_HostZoneConfig_0_u16_HostAFZoneStartX_Byte0		0x2440
#define AFStats_HostZoneConfig_0_u16_HostAFZoneStartX_Byte1		0x2441
#define AFStats_HostZoneConfig_0_u16_HostAFZoneStartY_Byte0		0x2442
#define AFStats_HostZoneConfig_0_u16_HostAFZoneStartY_Byte1		0x2443
#define AFStats_HostZoneConfig_0_u16_HostAFZoneWidth_Byte0		0x2444
#define AFStats_HostZoneConfig_0_u16_HostAFZoneWidth_Byte1		0x2445
#define AFStats_HostZoneConfig_0_u16_HostAFZoneHeight_Byte0		0x2446
#define AFStats_HostZoneConfig_0_u16_HostAFZoneHeight_Byte1		0x2447
#define AFStats_HostZoneConfig_0_e_Flag_Enabled_Byte0		0x2448

// page 'g_AFStats_HostZoneConfig[1]'

#define AFStats_HostZoneConfig_1_u16_HostAFZoneStartX_Byte0		0x2480
#define AFStats_HostZoneConfig_1_u16_HostAFZoneStartX_Byte1		0x2481
#define AFStats_HostZoneConfig_1_u16_HostAFZoneStartY_Byte0		0x2482
#define AFStats_HostZoneConfig_1_u16_HostAFZoneStartY_Byte1		0x2483
#define AFStats_HostZoneConfig_1_u16_HostAFZoneWidth_Byte0		0x2484
#define AFStats_HostZoneConfig_1_u16_HostAFZoneWidth_Byte1		0x2485
#define AFStats_HostZoneConfig_1_u16_HostAFZoneHeight_Byte0		0x2486
#define AFStats_HostZoneConfig_1_u16_HostAFZoneHeight_Byte1		0x2487
#define AFStats_HostZoneConfig_1_e_Flag_Enabled_Byte0		0x2488

// page 'g_AFStats_HostZoneConfig[2]'

#define AFStats_HostZoneConfig_2_u16_HostAFZoneStartX_Byte0		0x24c0
#define AFStats_HostZoneConfig_2_u16_HostAFZoneStartX_Byte1		0x24c1
#define AFStats_HostZoneConfig_2_u16_HostAFZoneStartY_Byte0		0x24c2
#define AFStats_HostZoneConfig_2_u16_HostAFZoneStartY_Byte1		0x24c3
#define AFStats_HostZoneConfig_2_u16_HostAFZoneWidth_Byte0		0x24c4
#define AFStats_HostZoneConfig_2_u16_HostAFZoneWidth_Byte1		0x24c5
#define AFStats_HostZoneConfig_2_u16_HostAFZoneHeight_Byte0		0x24c6
#define AFStats_HostZoneConfig_2_u16_HostAFZoneHeight_Byte1		0x24c7
#define AFStats_HostZoneConfig_2_e_Flag_Enabled_Byte0		0x24c8

// page 'g_AFStats_HostZoneConfig[3]'

#define AFStats_HostZoneConfig_3_u16_HostAFZoneStartX_Byte0		0x2500
#define AFStats_HostZoneConfig_3_u16_HostAFZoneStartX_Byte1		0x2501
#define AFStats_HostZoneConfig_3_u16_HostAFZoneStartY_Byte0		0x2502
#define AFStats_HostZoneConfig_3_u16_HostAFZoneStartY_Byte1		0x2503
#define AFStats_HostZoneConfig_3_u16_HostAFZoneWidth_Byte0		0x2504
#define AFStats_HostZoneConfig_3_u16_HostAFZoneWidth_Byte1		0x2505
#define AFStats_HostZoneConfig_3_u16_HostAFZoneHeight_Byte0		0x2506
#define AFStats_HostZoneConfig_3_u16_HostAFZoneHeight_Byte1		0x2507
#define AFStats_HostZoneConfig_3_e_Flag_Enabled_Byte0		0x2508

// page 'g_AFStats_HostZoneConfig[4]'

#define AFStats_HostZoneConfig_4_u16_HostAFZoneStartX_Byte0		0x2540
#define AFStats_HostZoneConfig_4_u16_HostAFZoneStartX_Byte1		0x2541
#define AFStats_HostZoneConfig_4_u16_HostAFZoneStartY_Byte0		0x2542
#define AFStats_HostZoneConfig_4_u16_HostAFZoneStartY_Byte1		0x2543
#define AFStats_HostZoneConfig_4_u16_HostAFZoneWidth_Byte0		0x2544
#define AFStats_HostZoneConfig_4_u16_HostAFZoneWidth_Byte1		0x2545
#define AFStats_HostZoneConfig_4_u16_HostAFZoneHeight_Byte0		0x2546
#define AFStats_HostZoneConfig_4_u16_HostAFZoneHeight_Byte1		0x2547
#define AFStats_HostZoneConfig_4_e_Flag_Enabled_Byte0		0x2548

// page 'g_AFStats_HostZoneConfig[5]'

#define AFStats_HostZoneConfig_5_u16_HostAFZoneStartX_Byte0		0x2580
#define AFStats_HostZoneConfig_5_u16_HostAFZoneStartX_Byte1		0x2581
#define AFStats_HostZoneConfig_5_u16_HostAFZoneStartY_Byte0		0x2582
#define AFStats_HostZoneConfig_5_u16_HostAFZoneStartY_Byte1		0x2583
#define AFStats_HostZoneConfig_5_u16_HostAFZoneWidth_Byte0		0x2584
#define AFStats_HostZoneConfig_5_u16_HostAFZoneWidth_Byte1		0x2585
#define AFStats_HostZoneConfig_5_u16_HostAFZoneHeight_Byte0		0x2586
#define AFStats_HostZoneConfig_5_u16_HostAFZoneHeight_Byte1		0x2587
#define AFStats_HostZoneConfig_5_e_Flag_Enabled_Byte0		0x2588

// page 'g_AFStats_HostZoneConfig[6]'

#define AFStats_HostZoneConfig_6_u16_HostAFZoneStartX_Byte0		0x25c0
#define AFStats_HostZoneConfig_6_u16_HostAFZoneStartX_Byte1		0x25c1
#define AFStats_HostZoneConfig_6_u16_HostAFZoneStartY_Byte0		0x25c2
#define AFStats_HostZoneConfig_6_u16_HostAFZoneStartY_Byte1		0x25c3
#define AFStats_HostZoneConfig_6_u16_HostAFZoneWidth_Byte0		0x25c4
#define AFStats_HostZoneConfig_6_u16_HostAFZoneWidth_Byte1		0x25c5
#define AFStats_HostZoneConfig_6_u16_HostAFZoneHeight_Byte0		0x25c6
#define AFStats_HostZoneConfig_6_u16_HostAFZoneHeight_Byte1		0x25c7
#define AFStats_HostZoneConfig_6_e_Flag_Enabled_Byte0		0x25c8

// page 'g_AFStats_HostZoneConfig[7]'

#define AFStats_HostZoneConfig_7_u16_HostAFZoneStartX_Byte0		0x2600
#define AFStats_HostZoneConfig_7_u16_HostAFZoneStartX_Byte1		0x2601
#define AFStats_HostZoneConfig_7_u16_HostAFZoneStartY_Byte0		0x2602
#define AFStats_HostZoneConfig_7_u16_HostAFZoneStartY_Byte1		0x2603
#define AFStats_HostZoneConfig_7_u16_HostAFZoneWidth_Byte0		0x2604
#define AFStats_HostZoneConfig_7_u16_HostAFZoneWidth_Byte1		0x2605
#define AFStats_HostZoneConfig_7_u16_HostAFZoneHeight_Byte0		0x2606
#define AFStats_HostZoneConfig_7_u16_HostAFZoneHeight_Byte1		0x2607
#define AFStats_HostZoneConfig_7_e_Flag_Enabled_Byte0		0x2608

// page 'g_AFStats_HostZoneConfig[8]'

#define AFStats_HostZoneConfig_8_u16_HostAFZoneStartX_Byte0		0x2640
#define AFStats_HostZoneConfig_8_u16_HostAFZoneStartX_Byte1		0x2641
#define AFStats_HostZoneConfig_8_u16_HostAFZoneStartY_Byte0		0x2642
#define AFStats_HostZoneConfig_8_u16_HostAFZoneStartY_Byte1		0x2643
#define AFStats_HostZoneConfig_8_u16_HostAFZoneWidth_Byte0		0x2644
#define AFStats_HostZoneConfig_8_u16_HostAFZoneWidth_Byte1		0x2645
#define AFStats_HostZoneConfig_8_u16_HostAFZoneHeight_Byte0		0x2646
#define AFStats_HostZoneConfig_8_u16_HostAFZoneHeight_Byte1		0x2647
#define AFStats_HostZoneConfig_8_e_Flag_Enabled_Byte0		0x2648

// page 'g_AFStats_HostZoneConfig[9]'

#define AFStats_HostZoneConfig_9_u16_HostAFZoneStartX_Byte0		0x2680
#define AFStats_HostZoneConfig_9_u16_HostAFZoneStartX_Byte1		0x2681
#define AFStats_HostZoneConfig_9_u16_HostAFZoneStartY_Byte0		0x2682
#define AFStats_HostZoneConfig_9_u16_HostAFZoneStartY_Byte1		0x2683
#define AFStats_HostZoneConfig_9_u16_HostAFZoneWidth_Byte0		0x2684
#define AFStats_HostZoneConfig_9_u16_HostAFZoneWidth_Byte1		0x2685
#define AFStats_HostZoneConfig_9_u16_HostAFZoneHeight_Byte0		0x2686
#define AFStats_HostZoneConfig_9_u16_HostAFZoneHeight_Byte1		0x2687
#define AFStats_HostZoneConfig_9_e_Flag_Enabled_Byte0		0x2688

// page 'g_AFStats_HostZoneStatus[0]'

#define AFStats_HostZoneStatus_0_u32_Focus_Byte0		0x26c0
#define AFStats_HostZoneStatus_0_u32_Focus_Byte1		0x26c1
#define AFStats_HostZoneStatus_0_u32_Focus_Byte2		0x26c2
#define AFStats_HostZoneStatus_0_u32_Focus_Byte3		0x26c3
#define AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte0		0x26c4
#define AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte1		0x26c5
#define AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte2		0x26c6
#define AFStats_HostZoneStatus_0_u32_AFZoneStartX_Byte3		0x26c7
#define AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte0		0x26c8
#define AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte1		0x26c9
#define AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte2		0x26ca
#define AFStats_HostZoneStatus_0_u32_AFZoneStartY_Byte3		0x26cb
#define AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte0		0x26cc
#define AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte1		0x26cd
#define AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte2		0x26ce
#define AFStats_HostZoneStatus_0_u32_AFZoneEndX_Byte3		0x26cf
#define AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte0		0x26d0
#define AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte1		0x26d1
#define AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte2		0x26d2
#define AFStats_HostZoneStatus_0_u32_AFZoneEndY_Byte3		0x26d3
#define AFStats_HostZoneStatus_0_u32_AFZonesWidth_Byte0		0x26d4
#define AFStats_HostZoneStatus_0_u32_AFZonesWidth_Byte1		0x26d5
#define AFStats_HostZoneStatus_0_u32_AFZonesWidth_Byte2		0x26d6
#define AFStats_HostZoneStatus_0_u32_AFZonesWidth_Byte3		0x26d7
#define AFStats_HostZoneStatus_0_u32_AFZonesHeight_Byte0		0x26d8
#define AFStats_HostZoneStatus_0_u32_AFZonesHeight_Byte1		0x26d9
#define AFStats_HostZoneStatus_0_u32_AFZonesHeight_Byte2		0x26da
#define AFStats_HostZoneStatus_0_u32_AFZonesHeight_Byte3		0x26db
#define AFStats_HostZoneStatus_0_u32_Light_Byte0		0x26dc
#define AFStats_HostZoneStatus_0_u32_Light_Byte1		0x26dd
#define AFStats_HostZoneStatus_0_u32_Light_Byte2		0x26de
#define AFStats_HostZoneStatus_0_u32_Light_Byte3		0x26df
#define AFStats_HostZoneStatus_0_u32_WeightAssigned_Byte0		0x26e0
#define AFStats_HostZoneStatus_0_u32_WeightAssigned_Byte1		0x26e1
#define AFStats_HostZoneStatus_0_u32_WeightAssigned_Byte2		0x26e2
#define AFStats_HostZoneStatus_0_u32_WeightAssigned_Byte3		0x26e3
#define AFStats_HostZoneStatus_0_u32_Enabled_Byte0		0x26e4
#define AFStats_HostZoneStatus_0_u32_Enabled_Byte1		0x26e5
#define AFStats_HostZoneStatus_0_u32_Enabled_Byte2		0x26e6
#define AFStats_HostZoneStatus_0_u32_Enabled_Byte3		0x26e7

// page 'g_AFStats_HostZoneStatus[1]'

#define AFStats_HostZoneStatus_1_u32_Focus_Byte0		0x2700
#define AFStats_HostZoneStatus_1_u32_Focus_Byte1		0x2701
#define AFStats_HostZoneStatus_1_u32_Focus_Byte2		0x2702
#define AFStats_HostZoneStatus_1_u32_Focus_Byte3		0x2703
#define AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte0		0x2704
#define AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte1		0x2705
#define AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte2		0x2706
#define AFStats_HostZoneStatus_1_u32_AFZoneStartX_Byte3		0x2707
#define AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte0		0x2708
#define AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte1		0x2709
#define AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte2		0x270a
#define AFStats_HostZoneStatus_1_u32_AFZoneStartY_Byte3		0x270b
#define AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte0		0x270c
#define AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte1		0x270d
#define AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte2		0x270e
#define AFStats_HostZoneStatus_1_u32_AFZoneEndX_Byte3		0x270f
#define AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte0		0x2710
#define AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte1		0x2711
#define AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte2		0x2712
#define AFStats_HostZoneStatus_1_u32_AFZoneEndY_Byte3		0x2713
#define AFStats_HostZoneStatus_1_u32_AFZonesWidth_Byte0		0x2714
#define AFStats_HostZoneStatus_1_u32_AFZonesWidth_Byte1		0x2715
#define AFStats_HostZoneStatus_1_u32_AFZonesWidth_Byte2		0x2716
#define AFStats_HostZoneStatus_1_u32_AFZonesWidth_Byte3		0x2717
#define AFStats_HostZoneStatus_1_u32_AFZonesHeight_Byte0		0x2718
#define AFStats_HostZoneStatus_1_u32_AFZonesHeight_Byte1		0x2719
#define AFStats_HostZoneStatus_1_u32_AFZonesHeight_Byte2		0x271a
#define AFStats_HostZoneStatus_1_u32_AFZonesHeight_Byte3		0x271b
#define AFStats_HostZoneStatus_1_u32_Light_Byte0		0x271c
#define AFStats_HostZoneStatus_1_u32_Light_Byte1		0x271d
#define AFStats_HostZoneStatus_1_u32_Light_Byte2		0x271e
#define AFStats_HostZoneStatus_1_u32_Light_Byte3		0x271f
#define AFStats_HostZoneStatus_1_u32_WeightAssigned_Byte0		0x2720
#define AFStats_HostZoneStatus_1_u32_WeightAssigned_Byte1		0x2721
#define AFStats_HostZoneStatus_1_u32_WeightAssigned_Byte2		0x2722
#define AFStats_HostZoneStatus_1_u32_WeightAssigned_Byte3		0x2723
#define AFStats_HostZoneStatus_1_u32_Enabled_Byte0		0x2724
#define AFStats_HostZoneStatus_1_u32_Enabled_Byte1		0x2725
#define AFStats_HostZoneStatus_1_u32_Enabled_Byte2		0x2726
#define AFStats_HostZoneStatus_1_u32_Enabled_Byte3		0x2727

// page 'g_AFStats_HostZoneStatus[2]'

#define AFStats_HostZoneStatus_2_u32_Focus_Byte0		0x2740
#define AFStats_HostZoneStatus_2_u32_Focus_Byte1		0x2741
#define AFStats_HostZoneStatus_2_u32_Focus_Byte2		0x2742
#define AFStats_HostZoneStatus_2_u32_Focus_Byte3		0x2743
#define AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte0		0x2744
#define AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte1		0x2745
#define AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte2		0x2746
#define AFStats_HostZoneStatus_2_u32_AFZoneStartX_Byte3		0x2747
#define AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte0		0x2748
#define AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte1		0x2749
#define AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte2		0x274a
#define AFStats_HostZoneStatus_2_u32_AFZoneStartY_Byte3		0x274b
#define AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte0		0x274c
#define AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte1		0x274d
#define AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte2		0x274e
#define AFStats_HostZoneStatus_2_u32_AFZoneEndX_Byte3		0x274f
#define AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte0		0x2750
#define AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte1		0x2751
#define AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte2		0x2752
#define AFStats_HostZoneStatus_2_u32_AFZoneEndY_Byte3		0x2753
#define AFStats_HostZoneStatus_2_u32_AFZonesWidth_Byte0		0x2754
#define AFStats_HostZoneStatus_2_u32_AFZonesWidth_Byte1		0x2755
#define AFStats_HostZoneStatus_2_u32_AFZonesWidth_Byte2		0x2756
#define AFStats_HostZoneStatus_2_u32_AFZonesWidth_Byte3		0x2757
#define AFStats_HostZoneStatus_2_u32_AFZonesHeight_Byte0		0x2758
#define AFStats_HostZoneStatus_2_u32_AFZonesHeight_Byte1		0x2759
#define AFStats_HostZoneStatus_2_u32_AFZonesHeight_Byte2		0x275a
#define AFStats_HostZoneStatus_2_u32_AFZonesHeight_Byte3		0x275b
#define AFStats_HostZoneStatus_2_u32_Light_Byte0		0x275c
#define AFStats_HostZoneStatus_2_u32_Light_Byte1		0x275d
#define AFStats_HostZoneStatus_2_u32_Light_Byte2		0x275e
#define AFStats_HostZoneStatus_2_u32_Light_Byte3		0x275f
#define AFStats_HostZoneStatus_2_u32_WeightAssigned_Byte0		0x2760
#define AFStats_HostZoneStatus_2_u32_WeightAssigned_Byte1		0x2761
#define AFStats_HostZoneStatus_2_u32_WeightAssigned_Byte2		0x2762
#define AFStats_HostZoneStatus_2_u32_WeightAssigned_Byte3		0x2763
#define AFStats_HostZoneStatus_2_u32_Enabled_Byte0		0x2764
#define AFStats_HostZoneStatus_2_u32_Enabled_Byte1		0x2765
#define AFStats_HostZoneStatus_2_u32_Enabled_Byte2		0x2766
#define AFStats_HostZoneStatus_2_u32_Enabled_Byte3		0x2767

// page 'g_AFStats_HostZoneStatus[3]'

#define AFStats_HostZoneStatus_3_u32_Focus_Byte0		0x2780
#define AFStats_HostZoneStatus_3_u32_Focus_Byte1		0x2781
#define AFStats_HostZoneStatus_3_u32_Focus_Byte2		0x2782
#define AFStats_HostZoneStatus_3_u32_Focus_Byte3		0x2783
#define AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte0		0x2784
#define AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte1		0x2785
#define AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte2		0x2786
#define AFStats_HostZoneStatus_3_u32_AFZoneStartX_Byte3		0x2787
#define AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte0		0x2788
#define AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte1		0x2789
#define AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte2		0x278a
#define AFStats_HostZoneStatus_3_u32_AFZoneStartY_Byte3		0x278b
#define AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte0		0x278c
#define AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte1		0x278d
#define AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte2		0x278e
#define AFStats_HostZoneStatus_3_u32_AFZoneEndX_Byte3		0x278f
#define AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte0		0x2790
#define AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte1		0x2791
#define AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte2		0x2792
#define AFStats_HostZoneStatus_3_u32_AFZoneEndY_Byte3		0x2793
#define AFStats_HostZoneStatus_3_u32_AFZonesWidth_Byte0		0x2794
#define AFStats_HostZoneStatus_3_u32_AFZonesWidth_Byte1		0x2795
#define AFStats_HostZoneStatus_3_u32_AFZonesWidth_Byte2		0x2796
#define AFStats_HostZoneStatus_3_u32_AFZonesWidth_Byte3		0x2797
#define AFStats_HostZoneStatus_3_u32_AFZonesHeight_Byte0		0x2798
#define AFStats_HostZoneStatus_3_u32_AFZonesHeight_Byte1		0x2799
#define AFStats_HostZoneStatus_3_u32_AFZonesHeight_Byte2		0x279a
#define AFStats_HostZoneStatus_3_u32_AFZonesHeight_Byte3		0x279b
#define AFStats_HostZoneStatus_3_u32_Light_Byte0		0x279c
#define AFStats_HostZoneStatus_3_u32_Light_Byte1		0x279d
#define AFStats_HostZoneStatus_3_u32_Light_Byte2		0x279e
#define AFStats_HostZoneStatus_3_u32_Light_Byte3		0x279f
#define AFStats_HostZoneStatus_3_u32_WeightAssigned_Byte0		0x27a0
#define AFStats_HostZoneStatus_3_u32_WeightAssigned_Byte1		0x27a1
#define AFStats_HostZoneStatus_3_u32_WeightAssigned_Byte2		0x27a2
#define AFStats_HostZoneStatus_3_u32_WeightAssigned_Byte3		0x27a3
#define AFStats_HostZoneStatus_3_u32_Enabled_Byte0		0x27a4
#define AFStats_HostZoneStatus_3_u32_Enabled_Byte1		0x27a5
#define AFStats_HostZoneStatus_3_u32_Enabled_Byte2		0x27a6
#define AFStats_HostZoneStatus_3_u32_Enabled_Byte3		0x27a7

// page 'g_AFStats_HostZoneStatus[4]'

#define AFStats_HostZoneStatus_4_u32_Focus_Byte0		0x27c0
#define AFStats_HostZoneStatus_4_u32_Focus_Byte1		0x27c1
#define AFStats_HostZoneStatus_4_u32_Focus_Byte2		0x27c2
#define AFStats_HostZoneStatus_4_u32_Focus_Byte3		0x27c3
#define AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte0		0x27c4
#define AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte1		0x27c5
#define AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte2		0x27c6
#define AFStats_HostZoneStatus_4_u32_AFZoneStartX_Byte3		0x27c7
#define AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte0		0x27c8
#define AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte1		0x27c9
#define AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte2		0x27ca
#define AFStats_HostZoneStatus_4_u32_AFZoneStartY_Byte3		0x27cb
#define AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte0		0x27cc
#define AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte1		0x27cd
#define AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte2		0x27ce
#define AFStats_HostZoneStatus_4_u32_AFZoneEndX_Byte3		0x27cf
#define AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte0		0x27d0
#define AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte1		0x27d1
#define AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte2		0x27d2
#define AFStats_HostZoneStatus_4_u32_AFZoneEndY_Byte3		0x27d3
#define AFStats_HostZoneStatus_4_u32_AFZonesWidth_Byte0		0x27d4
#define AFStats_HostZoneStatus_4_u32_AFZonesWidth_Byte1		0x27d5
#define AFStats_HostZoneStatus_4_u32_AFZonesWidth_Byte2		0x27d6
#define AFStats_HostZoneStatus_4_u32_AFZonesWidth_Byte3		0x27d7
#define AFStats_HostZoneStatus_4_u32_AFZonesHeight_Byte0		0x27d8
#define AFStats_HostZoneStatus_4_u32_AFZonesHeight_Byte1		0x27d9
#define AFStats_HostZoneStatus_4_u32_AFZonesHeight_Byte2		0x27da
#define AFStats_HostZoneStatus_4_u32_AFZonesHeight_Byte3		0x27db
#define AFStats_HostZoneStatus_4_u32_Light_Byte0		0x27dc
#define AFStats_HostZoneStatus_4_u32_Light_Byte1		0x27dd
#define AFStats_HostZoneStatus_4_u32_Light_Byte2		0x27de
#define AFStats_HostZoneStatus_4_u32_Light_Byte3		0x27df
#define AFStats_HostZoneStatus_4_u32_WeightAssigned_Byte0		0x27e0
#define AFStats_HostZoneStatus_4_u32_WeightAssigned_Byte1		0x27e1
#define AFStats_HostZoneStatus_4_u32_WeightAssigned_Byte2		0x27e2
#define AFStats_HostZoneStatus_4_u32_WeightAssigned_Byte3		0x27e3
#define AFStats_HostZoneStatus_4_u32_Enabled_Byte0		0x27e4
#define AFStats_HostZoneStatus_4_u32_Enabled_Byte1		0x27e5
#define AFStats_HostZoneStatus_4_u32_Enabled_Byte2		0x27e6
#define AFStats_HostZoneStatus_4_u32_Enabled_Byte3		0x27e7

// page 'g_AFStats_HostZoneStatus[5]'

#define AFStats_HostZoneStatus_5_u32_Focus_Byte0		0x2800
#define AFStats_HostZoneStatus_5_u32_Focus_Byte1		0x2801
#define AFStats_HostZoneStatus_5_u32_Focus_Byte2		0x2802
#define AFStats_HostZoneStatus_5_u32_Focus_Byte3		0x2803
#define AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte0		0x2804
#define AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte1		0x2805
#define AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte2		0x2806
#define AFStats_HostZoneStatus_5_u32_AFZoneStartX_Byte3		0x2807
#define AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte0		0x2808
#define AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte1		0x2809
#define AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte2		0x280a
#define AFStats_HostZoneStatus_5_u32_AFZoneStartY_Byte3		0x280b
#define AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte0		0x280c
#define AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte1		0x280d
#define AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte2		0x280e
#define AFStats_HostZoneStatus_5_u32_AFZoneEndX_Byte3		0x280f
#define AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte0		0x2810
#define AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte1		0x2811
#define AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte2		0x2812
#define AFStats_HostZoneStatus_5_u32_AFZoneEndY_Byte3		0x2813
#define AFStats_HostZoneStatus_5_u32_AFZonesWidth_Byte0		0x2814
#define AFStats_HostZoneStatus_5_u32_AFZonesWidth_Byte1		0x2815
#define AFStats_HostZoneStatus_5_u32_AFZonesWidth_Byte2		0x2816
#define AFStats_HostZoneStatus_5_u32_AFZonesWidth_Byte3		0x2817
#define AFStats_HostZoneStatus_5_u32_AFZonesHeight_Byte0		0x2818
#define AFStats_HostZoneStatus_5_u32_AFZonesHeight_Byte1		0x2819
#define AFStats_HostZoneStatus_5_u32_AFZonesHeight_Byte2		0x281a
#define AFStats_HostZoneStatus_5_u32_AFZonesHeight_Byte3		0x281b
#define AFStats_HostZoneStatus_5_u32_Light_Byte0		0x281c
#define AFStats_HostZoneStatus_5_u32_Light_Byte1		0x281d
#define AFStats_HostZoneStatus_5_u32_Light_Byte2		0x281e
#define AFStats_HostZoneStatus_5_u32_Light_Byte3		0x281f
#define AFStats_HostZoneStatus_5_u32_WeightAssigned_Byte0		0x2820
#define AFStats_HostZoneStatus_5_u32_WeightAssigned_Byte1		0x2821
#define AFStats_HostZoneStatus_5_u32_WeightAssigned_Byte2		0x2822
#define AFStats_HostZoneStatus_5_u32_WeightAssigned_Byte3		0x2823
#define AFStats_HostZoneStatus_5_u32_Enabled_Byte0		0x2824
#define AFStats_HostZoneStatus_5_u32_Enabled_Byte1		0x2825
#define AFStats_HostZoneStatus_5_u32_Enabled_Byte2		0x2826
#define AFStats_HostZoneStatus_5_u32_Enabled_Byte3		0x2827

// page 'g_AFStats_HostZoneStatus[6]'

#define AFStats_HostZoneStatus_6_u32_Focus_Byte0		0x2840
#define AFStats_HostZoneStatus_6_u32_Focus_Byte1		0x2841
#define AFStats_HostZoneStatus_6_u32_Focus_Byte2		0x2842
#define AFStats_HostZoneStatus_6_u32_Focus_Byte3		0x2843
#define AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte0		0x2844
#define AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte1		0x2845
#define AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte2		0x2846
#define AFStats_HostZoneStatus_6_u32_AFZoneStartX_Byte3		0x2847
#define AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte0		0x2848
#define AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte1		0x2849
#define AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte2		0x284a
#define AFStats_HostZoneStatus_6_u32_AFZoneStartY_Byte3		0x284b
#define AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte0		0x284c
#define AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte1		0x284d
#define AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte2		0x284e
#define AFStats_HostZoneStatus_6_u32_AFZoneEndX_Byte3		0x284f
#define AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte0		0x2850
#define AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte1		0x2851
#define AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte2		0x2852
#define AFStats_HostZoneStatus_6_u32_AFZoneEndY_Byte3		0x2853
#define AFStats_HostZoneStatus_6_u32_AFZonesWidth_Byte0		0x2854
#define AFStats_HostZoneStatus_6_u32_AFZonesWidth_Byte1		0x2855
#define AFStats_HostZoneStatus_6_u32_AFZonesWidth_Byte2		0x2856
#define AFStats_HostZoneStatus_6_u32_AFZonesWidth_Byte3		0x2857
#define AFStats_HostZoneStatus_6_u32_AFZonesHeight_Byte0		0x2858
#define AFStats_HostZoneStatus_6_u32_AFZonesHeight_Byte1		0x2859
#define AFStats_HostZoneStatus_6_u32_AFZonesHeight_Byte2		0x285a
#define AFStats_HostZoneStatus_6_u32_AFZonesHeight_Byte3		0x285b
#define AFStats_HostZoneStatus_6_u32_Light_Byte0		0x285c
#define AFStats_HostZoneStatus_6_u32_Light_Byte1		0x285d
#define AFStats_HostZoneStatus_6_u32_Light_Byte2		0x285e
#define AFStats_HostZoneStatus_6_u32_Light_Byte3		0x285f
#define AFStats_HostZoneStatus_6_u32_WeightAssigned_Byte0		0x2860
#define AFStats_HostZoneStatus_6_u32_WeightAssigned_Byte1		0x2861
#define AFStats_HostZoneStatus_6_u32_WeightAssigned_Byte2		0x2862
#define AFStats_HostZoneStatus_6_u32_WeightAssigned_Byte3		0x2863
#define AFStats_HostZoneStatus_6_u32_Enabled_Byte0		0x2864
#define AFStats_HostZoneStatus_6_u32_Enabled_Byte1		0x2865
#define AFStats_HostZoneStatus_6_u32_Enabled_Byte2		0x2866
#define AFStats_HostZoneStatus_6_u32_Enabled_Byte3		0x2867

// page 'g_AFStats_HostZoneStatus[7]'

#define AFStats_HostZoneStatus_7_u32_Focus_Byte0		0x2880
#define AFStats_HostZoneStatus_7_u32_Focus_Byte1		0x2881
#define AFStats_HostZoneStatus_7_u32_Focus_Byte2		0x2882
#define AFStats_HostZoneStatus_7_u32_Focus_Byte3		0x2883
#define AFStats_HostZoneStatus_7_u32_AFZoneStartX_Byte0		0x2884
#define AFStats_HostZoneStatus_7_u32_AFZoneStartX_Byte1		0x2885
#define AFStats_HostZoneStatus_7_u32_AFZoneStartX_Byte2		0x2886
#define AFStats_HostZoneStatus_7_u32_AFZoneStartX_Byte3		0x2887
#define AFStats_HostZoneStatus_7_u32_AFZoneStartY_Byte0		0x2888
#define AFStats_HostZoneStatus_7_u32_AFZoneStartY_Byte1		0x2889
#define AFStats_HostZoneStatus_7_u32_AFZoneStartY_Byte2		0x288a
#define AFStats_HostZoneStatus_7_u32_AFZoneStartY_Byte3		0x288b
#define AFStats_HostZoneStatus_7_u32_AFZoneEndX_Byte0		0x288c
#define AFStats_HostZoneStatus_7_u32_AFZoneEndX_Byte1		0x288d
#define AFStats_HostZoneStatus_7_u32_AFZoneEndX_Byte2		0x288e
#define AFStats_HostZoneStatus_7_u32_AFZoneEndX_Byte3		0x288f
#define AFStats_HostZoneStatus_7_u32_AFZoneEndY_Byte0		0x2890
#define AFStats_HostZoneStatus_7_u32_AFZoneEndY_Byte1		0x2891
#define AFStats_HostZoneStatus_7_u32_AFZoneEndY_Byte2		0x2892
#define AFStats_HostZoneStatus_7_u32_AFZoneEndY_Byte3		0x2893
#define AFStats_HostZoneStatus_7_u32_AFZonesWidth_Byte0		0x2894
#define AFStats_HostZoneStatus_7_u32_AFZonesWidth_Byte1		0x2895
#define AFStats_HostZoneStatus_7_u32_AFZonesWidth_Byte2		0x2896
#define AFStats_HostZoneStatus_7_u32_AFZonesWidth_Byte3		0x2897
#define AFStats_HostZoneStatus_7_u32_AFZonesHeight_Byte0		0x2898
#define AFStats_HostZoneStatus_7_u32_AFZonesHeight_Byte1		0x2899
#define AFStats_HostZoneStatus_7_u32_AFZonesHeight_Byte2		0x289a
#define AFStats_HostZoneStatus_7_u32_AFZonesHeight_Byte3		0x289b
#define AFStats_HostZoneStatus_7_u32_Light_Byte0		0x289c
#define AFStats_HostZoneStatus_7_u32_Light_Byte1		0x289d
#define AFStats_HostZoneStatus_7_u32_Light_Byte2		0x289e
#define AFStats_HostZoneStatus_7_u32_Light_Byte3		0x289f
#define AFStats_HostZoneStatus_7_u32_WeightAssigned_Byte0		0x28a0
#define AFStats_HostZoneStatus_7_u32_WeightAssigned_Byte1		0x28a1
#define AFStats_HostZoneStatus_7_u32_WeightAssigned_Byte2		0x28a2
#define AFStats_HostZoneStatus_7_u32_WeightAssigned_Byte3		0x28a3
#define AFStats_HostZoneStatus_7_u32_Enabled_Byte0		0x28a4
#define AFStats_HostZoneStatus_7_u32_Enabled_Byte1		0x28a5
#define AFStats_HostZoneStatus_7_u32_Enabled_Byte2		0x28a6
#define AFStats_HostZoneStatus_7_u32_Enabled_Byte3		0x28a7

// page 'g_AFStats_HostZoneStatus[8]'

#define AFStats_HostZoneStatus_8_u32_Focus_Byte0		0x28c0
#define AFStats_HostZoneStatus_8_u32_Focus_Byte1		0x28c1
#define AFStats_HostZoneStatus_8_u32_Focus_Byte2		0x28c2
#define AFStats_HostZoneStatus_8_u32_Focus_Byte3		0x28c3
#define AFStats_HostZoneStatus_8_u32_AFZoneStartX_Byte0		0x28c4
#define AFStats_HostZoneStatus_8_u32_AFZoneStartX_Byte1		0x28c5
#define AFStats_HostZoneStatus_8_u32_AFZoneStartX_Byte2		0x28c6
#define AFStats_HostZoneStatus_8_u32_AFZoneStartX_Byte3		0x28c7
#define AFStats_HostZoneStatus_8_u32_AFZoneStartY_Byte0		0x28c8
#define AFStats_HostZoneStatus_8_u32_AFZoneStartY_Byte1		0x28c9
#define AFStats_HostZoneStatus_8_u32_AFZoneStartY_Byte2		0x28ca
#define AFStats_HostZoneStatus_8_u32_AFZoneStartY_Byte3		0x28cb
#define AFStats_HostZoneStatus_8_u32_AFZoneEndX_Byte0		0x28cc
#define AFStats_HostZoneStatus_8_u32_AFZoneEndX_Byte1		0x28cd
#define AFStats_HostZoneStatus_8_u32_AFZoneEndX_Byte2		0x28ce
#define AFStats_HostZoneStatus_8_u32_AFZoneEndX_Byte3		0x28cf
#define AFStats_HostZoneStatus_8_u32_AFZoneEndY_Byte0		0x28d0
#define AFStats_HostZoneStatus_8_u32_AFZoneEndY_Byte1		0x28d1
#define AFStats_HostZoneStatus_8_u32_AFZoneEndY_Byte2		0x28d2
#define AFStats_HostZoneStatus_8_u32_AFZoneEndY_Byte3		0x28d3
#define AFStats_HostZoneStatus_8_u32_AFZonesWidth_Byte0		0x28d4
#define AFStats_HostZoneStatus_8_u32_AFZonesWidth_Byte1		0x28d5
#define AFStats_HostZoneStatus_8_u32_AFZonesWidth_Byte2		0x28d6
#define AFStats_HostZoneStatus_8_u32_AFZonesWidth_Byte3		0x28d7
#define AFStats_HostZoneStatus_8_u32_AFZonesHeight_Byte0		0x28d8
#define AFStats_HostZoneStatus_8_u32_AFZonesHeight_Byte1		0x28d9
#define AFStats_HostZoneStatus_8_u32_AFZonesHeight_Byte2		0x28da
#define AFStats_HostZoneStatus_8_u32_AFZonesHeight_Byte3		0x28db
#define AFStats_HostZoneStatus_8_u32_Light_Byte0		0x28dc
#define AFStats_HostZoneStatus_8_u32_Light_Byte1		0x28dd
#define AFStats_HostZoneStatus_8_u32_Light_Byte2		0x28de
#define AFStats_HostZoneStatus_8_u32_Light_Byte3		0x28df
#define AFStats_HostZoneStatus_8_u32_WeightAssigned_Byte0		0x28e0
#define AFStats_HostZoneStatus_8_u32_WeightAssigned_Byte1		0x28e1
#define AFStats_HostZoneStatus_8_u32_WeightAssigned_Byte2		0x28e2
#define AFStats_HostZoneStatus_8_u32_WeightAssigned_Byte3		0x28e3
#define AFStats_HostZoneStatus_8_u32_Enabled_Byte0		0x28e4
#define AFStats_HostZoneStatus_8_u32_Enabled_Byte1		0x28e5
#define AFStats_HostZoneStatus_8_u32_Enabled_Byte2		0x28e6
#define AFStats_HostZoneStatus_8_u32_Enabled_Byte3		0x28e7

// page 'g_AFStats_HostZoneStatus[9]'

#define AFStats_HostZoneStatus_9_u32_Focus_Byte0		0x2900
#define AFStats_HostZoneStatus_9_u32_Focus_Byte1		0x2901
#define AFStats_HostZoneStatus_9_u32_Focus_Byte2		0x2902
#define AFStats_HostZoneStatus_9_u32_Focus_Byte3		0x2903
#define AFStats_HostZoneStatus_9_u32_AFZoneStartX_Byte0		0x2904
#define AFStats_HostZoneStatus_9_u32_AFZoneStartX_Byte1		0x2905
#define AFStats_HostZoneStatus_9_u32_AFZoneStartX_Byte2		0x2906
#define AFStats_HostZoneStatus_9_u32_AFZoneStartX_Byte3		0x2907
#define AFStats_HostZoneStatus_9_u32_AFZoneStartY_Byte0		0x2908
#define AFStats_HostZoneStatus_9_u32_AFZoneStartY_Byte1		0x2909
#define AFStats_HostZoneStatus_9_u32_AFZoneStartY_Byte2		0x290a
#define AFStats_HostZoneStatus_9_u32_AFZoneStartY_Byte3		0x290b
#define AFStats_HostZoneStatus_9_u32_AFZoneEndX_Byte0		0x290c
#define AFStats_HostZoneStatus_9_u32_AFZoneEndX_Byte1		0x290d
#define AFStats_HostZoneStatus_9_u32_AFZoneEndX_Byte2		0x290e
#define AFStats_HostZoneStatus_9_u32_AFZoneEndX_Byte3		0x290f
#define AFStats_HostZoneStatus_9_u32_AFZoneEndY_Byte0		0x2910
#define AFStats_HostZoneStatus_9_u32_AFZoneEndY_Byte1		0x2911
#define AFStats_HostZoneStatus_9_u32_AFZoneEndY_Byte2		0x2912
#define AFStats_HostZoneStatus_9_u32_AFZoneEndY_Byte3		0x2913
#define AFStats_HostZoneStatus_9_u32_AFZonesWidth_Byte0		0x2914
#define AFStats_HostZoneStatus_9_u32_AFZonesWidth_Byte1		0x2915
#define AFStats_HostZoneStatus_9_u32_AFZonesWidth_Byte2		0x2916
#define AFStats_HostZoneStatus_9_u32_AFZonesWidth_Byte3		0x2917
#define AFStats_HostZoneStatus_9_u32_AFZonesHeight_Byte0		0x2918
#define AFStats_HostZoneStatus_9_u32_AFZonesHeight_Byte1		0x2919
#define AFStats_HostZoneStatus_9_u32_AFZonesHeight_Byte2		0x291a
#define AFStats_HostZoneStatus_9_u32_AFZonesHeight_Byte3		0x291b
#define AFStats_HostZoneStatus_9_u32_Light_Byte0		0x291c
#define AFStats_HostZoneStatus_9_u32_Light_Byte1		0x291d
#define AFStats_HostZoneStatus_9_u32_Light_Byte2		0x291e
#define AFStats_HostZoneStatus_9_u32_Light_Byte3		0x291f
#define AFStats_HostZoneStatus_9_u32_WeightAssigned_Byte0		0x2920
#define AFStats_HostZoneStatus_9_u32_WeightAssigned_Byte1		0x2921
#define AFStats_HostZoneStatus_9_u32_WeightAssigned_Byte2		0x2922
#define AFStats_HostZoneStatus_9_u32_WeightAssigned_Byte3		0x2923
#define AFStats_HostZoneStatus_9_u32_Enabled_Byte0		0x2924
#define AFStats_HostZoneStatus_9_u32_Enabled_Byte1		0x2925
#define AFStats_HostZoneStatus_9_u32_Enabled_Byte2		0x2926
#define AFStats_HostZoneStatus_9_u32_Enabled_Byte3		0x2927

// page 'g_AFStats_ZoneHWStatus[0]'

#define AFStats_ZoneHWStatus_0_u16_AFStartX_Byte0		0x2940
#define AFStats_ZoneHWStatus_0_u16_AFStartX_Byte1		0x2941
#define AFStats_ZoneHWStatus_0_u16_AFStartY_Byte0		0x2942
#define AFStats_ZoneHWStatus_0_u16_AFStartY_Byte1		0x2943
#define AFStats_ZoneHWStatus_0_u16_AFEndX_Byte0		0x2944
#define AFStats_ZoneHWStatus_0_u16_AFEndX_Byte1		0x2945
#define AFStats_ZoneHWStatus_0_u16_AFEndY_Byte0		0x2946
#define AFStats_ZoneHWStatus_0_u16_AFEndY_Byte1		0x2947

// page 'g_AFStats_ZoneHWStatus[1]'

#define AFStats_ZoneHWStatus_1_u16_AFStartX_Byte0		0x2980
#define AFStats_ZoneHWStatus_1_u16_AFStartX_Byte1		0x2981
#define AFStats_ZoneHWStatus_1_u16_AFStartY_Byte0		0x2982
#define AFStats_ZoneHWStatus_1_u16_AFStartY_Byte1		0x2983
#define AFStats_ZoneHWStatus_1_u16_AFEndX_Byte0		0x2984
#define AFStats_ZoneHWStatus_1_u16_AFEndX_Byte1		0x2985
#define AFStats_ZoneHWStatus_1_u16_AFEndY_Byte0		0x2986
#define AFStats_ZoneHWStatus_1_u16_AFEndY_Byte1		0x2987

// page 'g_AFStats_ZoneHWStatus[2]'

#define AFStats_ZoneHWStatus_2_u16_AFStartX_Byte0		0x29c0
#define AFStats_ZoneHWStatus_2_u16_AFStartX_Byte1		0x29c1
#define AFStats_ZoneHWStatus_2_u16_AFStartY_Byte0		0x29c2
#define AFStats_ZoneHWStatus_2_u16_AFStartY_Byte1		0x29c3
#define AFStats_ZoneHWStatus_2_u16_AFEndX_Byte0		0x29c4
#define AFStats_ZoneHWStatus_2_u16_AFEndX_Byte1		0x29c5
#define AFStats_ZoneHWStatus_2_u16_AFEndY_Byte0		0x29c6
#define AFStats_ZoneHWStatus_2_u16_AFEndY_Byte1		0x29c7

// page 'g_AFStats_ZoneHWStatus[3]'

#define AFStats_ZoneHWStatus_3_u16_AFStartX_Byte0		0x2a00
#define AFStats_ZoneHWStatus_3_u16_AFStartX_Byte1		0x2a01
#define AFStats_ZoneHWStatus_3_u16_AFStartY_Byte0		0x2a02
#define AFStats_ZoneHWStatus_3_u16_AFStartY_Byte1		0x2a03
#define AFStats_ZoneHWStatus_3_u16_AFEndX_Byte0		0x2a04
#define AFStats_ZoneHWStatus_3_u16_AFEndX_Byte1		0x2a05
#define AFStats_ZoneHWStatus_3_u16_AFEndY_Byte0		0x2a06
#define AFStats_ZoneHWStatus_3_u16_AFEndY_Byte1		0x2a07

// page 'g_AFStats_ZoneHWStatus[4]'

#define AFStats_ZoneHWStatus_4_u16_AFStartX_Byte0		0x2a40
#define AFStats_ZoneHWStatus_4_u16_AFStartX_Byte1		0x2a41
#define AFStats_ZoneHWStatus_4_u16_AFStartY_Byte0		0x2a42
#define AFStats_ZoneHWStatus_4_u16_AFStartY_Byte1		0x2a43
#define AFStats_ZoneHWStatus_4_u16_AFEndX_Byte0		0x2a44
#define AFStats_ZoneHWStatus_4_u16_AFEndX_Byte1		0x2a45
#define AFStats_ZoneHWStatus_4_u16_AFEndY_Byte0		0x2a46
#define AFStats_ZoneHWStatus_4_u16_AFEndY_Byte1		0x2a47

// page 'g_AFStats_ZoneHWStatus[5]'

#define AFStats_ZoneHWStatus_5_u16_AFStartX_Byte0		0x2a80
#define AFStats_ZoneHWStatus_5_u16_AFStartX_Byte1		0x2a81
#define AFStats_ZoneHWStatus_5_u16_AFStartY_Byte0		0x2a82
#define AFStats_ZoneHWStatus_5_u16_AFStartY_Byte1		0x2a83
#define AFStats_ZoneHWStatus_5_u16_AFEndX_Byte0		0x2a84
#define AFStats_ZoneHWStatus_5_u16_AFEndX_Byte1		0x2a85
#define AFStats_ZoneHWStatus_5_u16_AFEndY_Byte0		0x2a86
#define AFStats_ZoneHWStatus_5_u16_AFEndY_Byte1		0x2a87

// page 'g_AFStats_ZoneHWStatus[6]'

#define AFStats_ZoneHWStatus_6_u16_AFStartX_Byte0		0x2ac0
#define AFStats_ZoneHWStatus_6_u16_AFStartX_Byte1		0x2ac1
#define AFStats_ZoneHWStatus_6_u16_AFStartY_Byte0		0x2ac2
#define AFStats_ZoneHWStatus_6_u16_AFStartY_Byte1		0x2ac3
#define AFStats_ZoneHWStatus_6_u16_AFEndX_Byte0		0x2ac4
#define AFStats_ZoneHWStatus_6_u16_AFEndX_Byte1		0x2ac5
#define AFStats_ZoneHWStatus_6_u16_AFEndY_Byte0		0x2ac6
#define AFStats_ZoneHWStatus_6_u16_AFEndY_Byte1		0x2ac7

// page 'g_AFStats_ZoneHWStatus[7]'

#define AFStats_ZoneHWStatus_7_u16_AFStartX_Byte0		0x2b00
#define AFStats_ZoneHWStatus_7_u16_AFStartX_Byte1		0x2b01
#define AFStats_ZoneHWStatus_7_u16_AFStartY_Byte0		0x2b02
#define AFStats_ZoneHWStatus_7_u16_AFStartY_Byte1		0x2b03
#define AFStats_ZoneHWStatus_7_u16_AFEndX_Byte0		0x2b04
#define AFStats_ZoneHWStatus_7_u16_AFEndX_Byte1		0x2b05
#define AFStats_ZoneHWStatus_7_u16_AFEndY_Byte0		0x2b06
#define AFStats_ZoneHWStatus_7_u16_AFEndY_Byte1		0x2b07

// page 'g_AFStats_ZoneHWStatus[8]'

#define AFStats_ZoneHWStatus_8_u16_AFStartX_Byte0		0x2b40
#define AFStats_ZoneHWStatus_8_u16_AFStartX_Byte1		0x2b41
#define AFStats_ZoneHWStatus_8_u16_AFStartY_Byte0		0x2b42
#define AFStats_ZoneHWStatus_8_u16_AFStartY_Byte1		0x2b43
#define AFStats_ZoneHWStatus_8_u16_AFEndX_Byte0		0x2b44
#define AFStats_ZoneHWStatus_8_u16_AFEndX_Byte1		0x2b45
#define AFStats_ZoneHWStatus_8_u16_AFEndY_Byte0		0x2b46
#define AFStats_ZoneHWStatus_8_u16_AFEndY_Byte1		0x2b47

// page 'g_AFStats_ZoneHWStatus[9]'

#define AFStats_ZoneHWStatus_9_u16_AFStartX_Byte0		0x2b80
#define AFStats_ZoneHWStatus_9_u16_AFStartX_Byte1		0x2b81
#define AFStats_ZoneHWStatus_9_u16_AFStartY_Byte0		0x2b82
#define AFStats_ZoneHWStatus_9_u16_AFStartY_Byte1		0x2b83
#define AFStats_ZoneHWStatus_9_u16_AFEndX_Byte0		0x2b84
#define AFStats_ZoneHWStatus_9_u16_AFEndX_Byte1		0x2b85
#define AFStats_ZoneHWStatus_9_u16_AFEndY_Byte0		0x2b86
#define AFStats_ZoneHWStatus_9_u16_AFEndY_Byte1		0x2b87

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_TestPattern_Ctrl'

#define TestPattern_Ctrl_u16_test_data_red_Byte0		0x2cc0
#define TestPattern_Ctrl_u16_test_data_red_Byte1		0x2cc1
#define TestPattern_Ctrl_u16_test_data_greenR_Byte0		0x2cc2
#define TestPattern_Ctrl_u16_test_data_greenR_Byte1		0x2cc3
#define TestPattern_Ctrl_u16_test_data_blue_Byte0		0x2cc4
#define TestPattern_Ctrl_u16_test_data_blue_Byte1		0x2cc5
#define TestPattern_Ctrl_u16_test_data_greenB_Byte0		0x2cc6
#define TestPattern_Ctrl_u16_test_data_greenB_Byte1		0x2cc7
#define TestPattern_Ctrl_e_TestPattern_Byte0		0x2cc8

// page 'g_TestPattern_Status'

#define TestPattern_Status_u16_test_data_red_Byte0		0x2d00
#define TestPattern_Status_u16_test_data_red_Byte1		0x2d01
#define TestPattern_Status_u16_test_data_greenR_Byte0		0x2d02
#define TestPattern_Status_u16_test_data_greenR_Byte1		0x2d03
#define TestPattern_Status_u16_test_data_blue_Byte0		0x2d04
#define TestPattern_Status_u16_test_data_blue_Byte1		0x2d05
#define TestPattern_Status_u16_test_data_greenB_Byte0		0x2d06
#define TestPattern_Status_u16_test_data_greenB_Byte1		0x2d07
#define TestPattern_Status_e_TestPattern_Byte0		0x2d08

// page 'g_RSO_DataStatus'

#define RSO_DataStatus_u32_XCoefGr_Byte0		0x2d40
#define RSO_DataStatus_u32_XCoefGr_Byte1		0x2d41
#define RSO_DataStatus_u32_XCoefGr_Byte2		0x2d42
#define RSO_DataStatus_u32_XCoefGr_Byte3		0x2d43
#define RSO_DataStatus_u32_YCoefGr_Byte0		0x2d44
#define RSO_DataStatus_u32_YCoefGr_Byte1		0x2d45
#define RSO_DataStatus_u32_YCoefGr_Byte2		0x2d46
#define RSO_DataStatus_u32_YCoefGr_Byte3		0x2d47
#define RSO_DataStatus_u32_XCoefR_Byte0		0x2d48
#define RSO_DataStatus_u32_XCoefR_Byte1		0x2d49
#define RSO_DataStatus_u32_XCoefR_Byte2		0x2d4a
#define RSO_DataStatus_u32_XCoefR_Byte3		0x2d4b
#define RSO_DataStatus_u32_YCoefR_Byte0		0x2d4c
#define RSO_DataStatus_u32_YCoefR_Byte1		0x2d4d
#define RSO_DataStatus_u32_YCoefR_Byte2		0x2d4e
#define RSO_DataStatus_u32_YCoefR_Byte3		0x2d4f
#define RSO_DataStatus_u32_XCoefB_Byte0		0x2d50
#define RSO_DataStatus_u32_XCoefB_Byte1		0x2d51
#define RSO_DataStatus_u32_XCoefB_Byte2		0x2d52
#define RSO_DataStatus_u32_XCoefB_Byte3		0x2d53
#define RSO_DataStatus_u32_YCoefB_Byte0		0x2d54
#define RSO_DataStatus_u32_YCoefB_Byte1		0x2d55
#define RSO_DataStatus_u32_YCoefB_Byte2		0x2d56
#define RSO_DataStatus_u32_YCoefB_Byte3		0x2d57
#define RSO_DataStatus_u32_XCoefGb_Byte0		0x2d58
#define RSO_DataStatus_u32_XCoefGb_Byte1		0x2d59
#define RSO_DataStatus_u32_XCoefGb_Byte2		0x2d5a
#define RSO_DataStatus_u32_XCoefGb_Byte3		0x2d5b
#define RSO_DataStatus_u32_YCoefGb_Byte0		0x2d5c
#define RSO_DataStatus_u32_YCoefGb_Byte1		0x2d5d
#define RSO_DataStatus_u32_YCoefGb_Byte2		0x2d5e
#define RSO_DataStatus_u32_YCoefGb_Byte3		0x2d5f
#define RSO_DataStatus_u16_DcTermGr_Byte0		0x2d60
#define RSO_DataStatus_u16_DcTermGr_Byte1		0x2d61
#define RSO_DataStatus_u16_DcTermR_Byte0		0x2d62
#define RSO_DataStatus_u16_DcTermR_Byte1		0x2d63
#define RSO_DataStatus_u16_DcTermB_Byte0		0x2d64
#define RSO_DataStatus_u16_DcTermB_Byte1		0x2d65
#define RSO_DataStatus_u16_DcTermGb_Byte0		0x2d66
#define RSO_DataStatus_u16_DcTermGb_Byte1		0x2d67
#define RSO_DataStatus_u16_XSlantOrigin_Byte0		0x2d68
#define RSO_DataStatus_u16_XSlantOrigin_Byte1		0x2d69
#define RSO_DataStatus_u16_YSlantOrigin_Byte0		0x2d6a
#define RSO_DataStatus_u16_YSlantOrigin_Byte1		0x2d6b

// page 'g_Babylon_Ctrl'

#define Babylon_Ctrl_e_Flag_BabylonEnable_Byte0		0x2d80
#define Babylon_Ctrl_u8_ZipperKill_Byte0		0x2d81
#define Babylon_Ctrl_u8_Flat_Threshold_Byte0		0x2d82
#define Babylon_Ctrl_u8_Flat_Threshold_Status_Byte0		0x2d83
#define Babylon_Ctrl_e_BabylonMode_Byte0		0x2d84

// page 'g_Scorpio_Ctrl'

#define Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0		0x2dc0
#define Scorpio_Ctrl_e_ScorpioMode_Byte0		0x2dc1
#define Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0		0x2dc2
#define Scorpio_Ctrl_u8_CoringLevel_Status_Byte0		0x2dc3

// page 'g_BinningRepair_Ctrl'

#define BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0		0x2e00
#define BinningRepair_Ctrl_e_Flag_H_Jog_Enable_Byte0		0x2e01
#define BinningRepair_Ctrl_e_Flag_V_Jog_Enable_Byte0		0x2e02
#define BinningRepair_Ctrl_e_BinningRepairMode_Byte0		0x2e03
#define BinningRepair_Ctrl_u8_Coeff_00_Byte0		0x2e04
#define BinningRepair_Ctrl_u8_Coeff_01_Byte0		0x2e05
#define BinningRepair_Ctrl_u8_Coeff_10_Byte0		0x2e06
#define BinningRepair_Ctrl_u8_Coeff_11_Byte0		0x2e07
#define BinningRepair_Ctrl_u8_Coeff_shift_Byte0		0x2e08
#define BinningRepair_Ctrl_u8_BinningRepair_factor_Byte0		0x2e09

// page 'g_DusterControl'

#define DusterControl_u16_FrameSigma_Byte0		0x2e40
#define DusterControl_u16_FrameSigma_Byte1		0x2e41
#define DusterControl_u16_Duster_ByPass_Ctrl_Byte0		0x2e42
#define DusterControl_u16_Duster_ByPass_Ctrl_Byte1		0x2e43
#define DusterControl_e_Flag_DusterEnable_Byte0		0x2e44
#define DusterControl_u8_GaussianWeight_Byte0		0x2e45
#define DusterControl_u8_SigmaWeight_Byte0		0x2e46
#define DusterControl_u8_ScytheControl_hi_Byte0		0x2e47
#define DusterControl_u8_ScytheControl_lo_Byte0		0x2e48
#define DusterControl_u8_CenterCorrectionSigmaFactor_Byte0		0x2e49
#define DusterControl_u8_RingCorrectionNormThr_Byte0		0x2e4a

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_Reserved'

#define Reserved_u8_dummy_Byte0		0x8000

// page 'g_GridironControl'

#define GridironControl_f_LiveCast_Byte0		0x2f80
#define GridironControl_f_LiveCast_Byte1		0x2f81
#define GridironControl_f_LiveCast_Byte2		0x2f82
#define GridironControl_f_LiveCast_Byte3		0x2f83
#define GridironControl_f_CastPosition0_Byte0		0x2f84
#define GridironControl_f_CastPosition0_Byte1		0x2f85
#define GridironControl_f_CastPosition0_Byte2		0x2f86
#define GridironControl_f_CastPosition0_Byte3		0x2f87
#define GridironControl_f_CastPosition1_Byte0		0x2f88
#define GridironControl_f_CastPosition1_Byte1		0x2f89
#define GridironControl_f_CastPosition1_Byte2		0x2f8a
#define GridironControl_f_CastPosition1_Byte3		0x2f8b
#define GridironControl_f_CastPosition2_Byte0		0x2f8c
#define GridironControl_f_CastPosition2_Byte1		0x2f8d
#define GridironControl_f_CastPosition2_Byte2		0x2f8e
#define GridironControl_f_CastPosition2_Byte3		0x2f8f
#define GridironControl_f_CastPosition3_Byte0		0x2f90
#define GridironControl_f_CastPosition3_Byte1		0x2f91
#define GridironControl_f_CastPosition3_Byte2		0x2f92
#define GridironControl_f_CastPosition3_Byte3		0x2f93
#define GridironControl_u16_GridWidth_Byte0		0x2f94
#define GridironControl_u16_GridWidth_Byte1		0x2f95
#define GridironControl_u16_GridHeight_Byte0		0x2f96
#define GridironControl_u16_GridHeight_Byte1		0x2f97
#define GridironControl_e_Flag_Enable_Byte0		0x2f98
#define GridironControl_e_Flag_VerFlip_Byte0		0x2f99
#define GridironControl_e_Flag_HorFlip_Byte0		0x2f9a
#define GridironControl_u8_Active_ReferenceCast_Count_Byte0		0x2f9b
#define GridironControl_e_PixelOrder_Byte0		0x2f9c

// page 'g_SDL_Control'

#define SDL_Control_e_SDLMode_Control_Byte0		0x2fc0

// page 'g_SDL_Status'

#define SDL_Status_e_SDLMode_Status_Byte0		0x3000
#define SDL_Status_e_Flag_SDLUpdatePending_Byte0		0x3001

// page 'g_Adsoc_PK_Ctrl[0]'

#define Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0		0x3040
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Gain_Byte0		0x3041
#define Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0		0x3042
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Coring_Level_Byte0		0x3043
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0		0x3044
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0		0x3045
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0		0x3046
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0		0x3047
#define Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0		0x3048

// page 'g_Adsoc_PK_Ctrl[1]'

#define Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0		0x3080
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0		0x3081
#define Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0		0x3082
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level_Byte0		0x3083
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0		0x3084
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0		0x3085
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0		0x3086
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0		0x3087
#define Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0		0x3088

// page 'g_Adsoc_RP_Ctrl[0]'

#define Adsoc_RP_Ctrl_0_u16_Lens_Centre_HOffset_Byte0		0x30c0
#define Adsoc_RP_Ctrl_0_u16_Lens_Centre_HOffset_Byte1		0x30c1
#define Adsoc_RP_Ctrl_0_u16_Lens_Centre_VOffset_Byte0		0x30c2
#define Adsoc_RP_Ctrl_0_u16_Lens_Centre_VOffset_Byte1		0x30c3
#define Adsoc_RP_Ctrl_0_e_Flag_Adsoc_RP_Enable_Byte0		0x30c4
#define Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef0_Byte0		0x30c5
#define Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef1_Byte0		0x30c6
#define Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_COF_Shift_Byte0		0x30c7
#define Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Out_COF_Shift_Byte0		0x30c8
#define Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Unity_Byte0		0x30c9

// page 'g_Adsoc_RP_Ctrl[1]'

#define Adsoc_RP_Ctrl_1_u16_Lens_Centre_HOffset_Byte0		0x3100
#define Adsoc_RP_Ctrl_1_u16_Lens_Centre_HOffset_Byte1		0x3101
#define Adsoc_RP_Ctrl_1_u16_Lens_Centre_VOffset_Byte0		0x3102
#define Adsoc_RP_Ctrl_1_u16_Lens_Centre_VOffset_Byte1		0x3103
#define Adsoc_RP_Ctrl_1_e_Flag_Adsoc_RP_Enable_Byte0		0x3104
#define Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef0_Byte0		0x3105
#define Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef1_Byte0		0x3106
#define Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_COF_Shift_Byte0		0x3107
#define Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Out_COF_Shift_Byte0		0x3108
#define Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Unity_Byte0		0x3109

// page 'g_Adsoc_RP_Status[0]'

#define Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_X_Byte0		0x3140
#define Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_X_Byte1		0x3141
#define Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_Y_Byte0		0x3142
#define Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_Y_Byte1		0x3143
#define Adsoc_RP_Status_0_s16_Adsoc_RP_HOffset_Byte0		0x3144
#define Adsoc_RP_Status_0_s16_Adsoc_RP_HOffset_Byte1		0x3145
#define Adsoc_RP_Status_0_s16_Adsoc_RP_VOffset_Byte0		0x3146
#define Adsoc_RP_Status_0_s16_Adsoc_RP_VOffset_Byte1		0x3147

// page 'g_Adsoc_RP_Status[1]'

#define Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_X_Byte0		0x3180
#define Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_X_Byte1		0x3181
#define Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_Y_Byte0		0x3182
#define Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_Y_Byte1		0x3183
#define Adsoc_RP_Status_1_s16_Adsoc_RP_HOffset_Byte0		0x3184
#define Adsoc_RP_Status_1_s16_Adsoc_RP_HOffset_Byte1		0x3185
#define Adsoc_RP_Status_1_s16_Adsoc_RP_VOffset_Byte0		0x3186
#define Adsoc_RP_Status_1_s16_Adsoc_RP_VOffset_Byte1		0x3187

// page 'g_DusterStatus'

#define DusterStatus_u16_FrameSigma_Byte0		0x31c0
#define DusterStatus_u16_FrameSigma_Byte1		0x31c1
#define DusterStatus_u16_Gaussian_Th1_Byte0		0x31c2
#define DusterStatus_u16_Gaussian_Th1_Byte1		0x31c3
#define DusterStatus_u16_Gaussian_Th2_Byte0		0x31c4
#define DusterStatus_u16_Gaussian_Th2_Byte1		0x31c5
#define DusterStatus_u16_Gaussian_Th3_Byte0		0x31c6
#define DusterStatus_u16_Gaussian_Th3_Byte1		0x31c7
#define DusterStatus_u16_S0_Byte0		0x31c8
#define DusterStatus_u16_S0_Byte1		0x31c9
#define DusterStatus_u16_S1_Byte0		0x31ca
#define DusterStatus_u16_S1_Byte1		0x31cb
#define DusterStatus_u16_S2_Byte0		0x31cc
#define DusterStatus_u16_S2_Byte1		0x31cd
#define DusterStatus_u8_ScytheRank_hi_Byte0		0x31ce
#define DusterStatus_u8_ScytheRank_lo_Byte0		0x31cf
#define DusterStatus_u8_ScytheSmoothControl_hi_Byte0		0x31d0
#define DusterStatus_u8_ScytheSmoothControl_lo_Byte0		0x31d1
#define DusterStatus_u8_CCLocalSigma_Th_Byte0		0x31d2
#define DusterStatus_u8_RCNorm_Th_Byte0		0x31d3
#define DusterStatus_u8_SigmaGaussian_Byte0		0x31d4
#define DusterStatus_u8_GaussianWeight_Byte0		0x31d5

// page 'g_DMASetup'

#define DMASetup_e_DMADataBitsInMemory_Byte0		0x3200
#define DMASetup_e_DMADataAccessAlignment_Byte0		0x3201

// page 'g_DMAControl'

#define DMAControl_u16_DMACropValueRemainder_0_Byte0		0x3240
#define DMAControl_u16_DMACropValueRemainder_0_Byte1		0x3241
#define DMAControl_u16_DMACropValueRemainder_1_Byte0		0x3242
#define DMAControl_u16_DMACropValueRemainder_1_Byte1		0x3243
#define DMAControl_u16_MinIntegerInDMA_0_Byte0		0x3244
#define DMAControl_u16_MinIntegerInDMA_0_Byte1		0x3245
#define DMAControl_u16_MinIntegerInDMA_1_Byte0		0x3246
#define DMAControl_u16_MinIntegerInDMA_1_Byte1		0x3247
#define DMAControl_u16_HorizontalOffset_Byte0		0x3248
#define DMAControl_u16_HorizontalOffset_Byte1		0x3249
#define DMAControl_s16_DMAStartDiff_Byte0		0x324a
#define DMAControl_s16_DMAStartDiff_Byte1		0x324b
#define DMAControl_u8_MinNoOfPixPickedInOneGoFromDma_Byte0		0x324c

// page 'g_Scalar_StripeInternalParams[0]'

#define Scalar_StripeInternalParams_0_s32_StripeHCropBulk_Byte0		0x3280
#define Scalar_StripeInternalParams_0_s32_StripeHCropBulk_Byte1		0x3281
#define Scalar_StripeInternalParams_0_s32_StripeHCropBulk_Byte2		0x3282
#define Scalar_StripeInternalParams_0_s32_StripeHCropBulk_Byte3		0x3283
#define Scalar_StripeInternalParams_0_s32_StripeHCropFrac_Byte0		0x3284
#define Scalar_StripeInternalParams_0_s32_StripeHCropFrac_Byte1		0x3285
#define Scalar_StripeInternalParams_0_s32_StripeHCropFrac_Byte2		0x3286
#define Scalar_StripeInternalParams_0_s32_StripeHCropFrac_Byte3		0x3287
#define Scalar_StripeInternalParams_0_s32_StripeVCropBulk_Byte0		0x3288
#define Scalar_StripeInternalParams_0_s32_StripeVCropBulk_Byte1		0x3289
#define Scalar_StripeInternalParams_0_s32_StripeVCropBulk_Byte2		0x328a
#define Scalar_StripeInternalParams_0_s32_StripeVCropBulk_Byte3		0x328b
#define Scalar_StripeInternalParams_0_s32_StripeVCropFrac_Byte0		0x328c
#define Scalar_StripeInternalParams_0_s32_StripeVCropFrac_Byte1		0x328d
#define Scalar_StripeInternalParams_0_s32_StripeVCropFrac_Byte2		0x328e
#define Scalar_StripeInternalParams_0_s32_StripeVCropFrac_Byte3		0x328f
#define Scalar_StripeInternalParams_0_s32_StripeGrossHCrop_Byte0		0x3290
#define Scalar_StripeInternalParams_0_s32_StripeGrossHCrop_Byte1		0x3291
#define Scalar_StripeInternalParams_0_s32_StripeGrossHCrop_Byte2		0x3292
#define Scalar_StripeInternalParams_0_s32_StripeGrossHCrop_Byte3		0x3293
#define Scalar_StripeInternalParams_0_s32_StripeGrossVCrop_Byte0		0x3294
#define Scalar_StripeInternalParams_0_s32_StripeGrossVCrop_Byte1		0x3295
#define Scalar_StripeInternalParams_0_s32_StripeGrossVCrop_Byte2		0x3296
#define Scalar_StripeInternalParams_0_s32_StripeGrossVCrop_Byte3		0x3297
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropBulk_Byte0		0x3298
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropBulk_Byte1		0x3299
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropBulk_Byte2		0x329a
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropBulk_Byte3		0x329b
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropFrac_Byte0		0x329c
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropFrac_Byte1		0x329d
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropFrac_Byte2		0x329e
#define Scalar_StripeInternalParams_0_s32_IrStripeHCropFrac_Byte3		0x329f
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropBulk_Byte0		0x32a0
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropBulk_Byte1		0x32a1
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropBulk_Byte2		0x32a2
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropBulk_Byte3		0x32a3
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropFrac_Byte0		0x32a4
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropFrac_Byte1		0x32a5
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropFrac_Byte2		0x32a6
#define Scalar_StripeInternalParams_0_s32_IrStripeVCropFrac_Byte3		0x32a7
#define Scalar_StripeInternalParams_0_s16_SSystemOutputHSize_Byte0		0x32a8
#define Scalar_StripeInternalParams_0_s16_SSystemOutputHSize_Byte1		0x32a9
#define Scalar_StripeInternalParams_0_s16_SSystemOutputVSize_Byte0		0x32aa
#define Scalar_StripeInternalParams_0_s16_SSystemOutputVSize_Byte1		0x32ab
#define Scalar_StripeInternalParams_0_s16_SScalerOutputHSize_Byte0		0x32ac
#define Scalar_StripeInternalParams_0_s16_SScalerOutputHSize_Byte1		0x32ad
#define Scalar_StripeInternalParams_0_s16_SScalerOutputVSize_Byte0		0x32ae
#define Scalar_StripeInternalParams_0_s16_SScalerOutputVSize_Byte1		0x32af
#define Scalar_StripeInternalParams_0_s16_SScalerInputHSize_Byte0		0x32b0
#define Scalar_StripeInternalParams_0_s16_SScalerInputHSize_Byte1		0x32b1
#define Scalar_StripeInternalParams_0_s16_SScalerInputVSize_Byte0		0x32b2
#define Scalar_StripeInternalParams_0_s16_SScalerInputVSize_Byte1		0x32b3
#define Scalar_StripeInternalParams_0_s16_SCropperOutputHSize_Byte0		0x32b4
#define Scalar_StripeInternalParams_0_s16_SCropperOutputHSize_Byte1		0x32b5
#define Scalar_StripeInternalParams_0_s16_SCropperOutputVSize_Byte0		0x32b6
#define Scalar_StripeInternalParams_0_s16_SCropperOutputVSize_Byte1		0x32b7
#define Scalar_StripeInternalParams_0_u8_PreTopBorder_Byte0		0x32b8
#define Scalar_StripeInternalParams_0_u8_PreBottomBorder_Byte0		0x32b9
#define Scalar_StripeInternalParams_0_u8_PreLeftBorder_Byte0		0x32ba
#define Scalar_StripeInternalParams_0_u8_PreRightBorder_Byte0		0x32bb
#define Scalar_StripeInternalParams_0_u8_PostTopBorder_Byte0		0x32bc
#define Scalar_StripeInternalParams_0_u8_PostBottomBorder_Byte0		0x32bd
#define Scalar_StripeInternalParams_0_u8_PostLeftBorder_Byte0		0x32be
#define Scalar_StripeInternalParams_0_u8_PostRightBorder_Byte0		0x32bf

// page 'g_Scalar_StripeInternalParams[1]'

#define Scalar_StripeInternalParams_1_s32_StripeHCropBulk_Byte0		0x32c0
#define Scalar_StripeInternalParams_1_s32_StripeHCropBulk_Byte1		0x32c1
#define Scalar_StripeInternalParams_1_s32_StripeHCropBulk_Byte2		0x32c2
#define Scalar_StripeInternalParams_1_s32_StripeHCropBulk_Byte3		0x32c3
#define Scalar_StripeInternalParams_1_s32_StripeHCropFrac_Byte0		0x32c4
#define Scalar_StripeInternalParams_1_s32_StripeHCropFrac_Byte1		0x32c5
#define Scalar_StripeInternalParams_1_s32_StripeHCropFrac_Byte2		0x32c6
#define Scalar_StripeInternalParams_1_s32_StripeHCropFrac_Byte3		0x32c7
#define Scalar_StripeInternalParams_1_s32_StripeVCropBulk_Byte0		0x32c8
#define Scalar_StripeInternalParams_1_s32_StripeVCropBulk_Byte1		0x32c9
#define Scalar_StripeInternalParams_1_s32_StripeVCropBulk_Byte2		0x32ca
#define Scalar_StripeInternalParams_1_s32_StripeVCropBulk_Byte3		0x32cb
#define Scalar_StripeInternalParams_1_s32_StripeVCropFrac_Byte0		0x32cc
#define Scalar_StripeInternalParams_1_s32_StripeVCropFrac_Byte1		0x32cd
#define Scalar_StripeInternalParams_1_s32_StripeVCropFrac_Byte2		0x32ce
#define Scalar_StripeInternalParams_1_s32_StripeVCropFrac_Byte3		0x32cf
#define Scalar_StripeInternalParams_1_s32_StripeGrossHCrop_Byte0		0x32d0
#define Scalar_StripeInternalParams_1_s32_StripeGrossHCrop_Byte1		0x32d1
#define Scalar_StripeInternalParams_1_s32_StripeGrossHCrop_Byte2		0x32d2
#define Scalar_StripeInternalParams_1_s32_StripeGrossHCrop_Byte3		0x32d3
#define Scalar_StripeInternalParams_1_s32_StripeGrossVCrop_Byte0		0x32d4
#define Scalar_StripeInternalParams_1_s32_StripeGrossVCrop_Byte1		0x32d5
#define Scalar_StripeInternalParams_1_s32_StripeGrossVCrop_Byte2		0x32d6
#define Scalar_StripeInternalParams_1_s32_StripeGrossVCrop_Byte3		0x32d7
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropBulk_Byte0		0x32d8
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropBulk_Byte1		0x32d9
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropBulk_Byte2		0x32da
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropBulk_Byte3		0x32db
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropFrac_Byte0		0x32dc
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropFrac_Byte1		0x32dd
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropFrac_Byte2		0x32de
#define Scalar_StripeInternalParams_1_s32_IrStripeHCropFrac_Byte3		0x32df
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropBulk_Byte0		0x32e0
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropBulk_Byte1		0x32e1
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropBulk_Byte2		0x32e2
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropBulk_Byte3		0x32e3
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropFrac_Byte0		0x32e4
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropFrac_Byte1		0x32e5
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropFrac_Byte2		0x32e6
#define Scalar_StripeInternalParams_1_s32_IrStripeVCropFrac_Byte3		0x32e7
#define Scalar_StripeInternalParams_1_s16_SSystemOutputHSize_Byte0		0x32e8
#define Scalar_StripeInternalParams_1_s16_SSystemOutputHSize_Byte1		0x32e9
#define Scalar_StripeInternalParams_1_s16_SSystemOutputVSize_Byte0		0x32ea
#define Scalar_StripeInternalParams_1_s16_SSystemOutputVSize_Byte1		0x32eb
#define Scalar_StripeInternalParams_1_s16_SScalerOutputHSize_Byte0		0x32ec
#define Scalar_StripeInternalParams_1_s16_SScalerOutputHSize_Byte1		0x32ed
#define Scalar_StripeInternalParams_1_s16_SScalerOutputVSize_Byte0		0x32ee
#define Scalar_StripeInternalParams_1_s16_SScalerOutputVSize_Byte1		0x32ef
#define Scalar_StripeInternalParams_1_s16_SScalerInputHSize_Byte0		0x32f0
#define Scalar_StripeInternalParams_1_s16_SScalerInputHSize_Byte1		0x32f1
#define Scalar_StripeInternalParams_1_s16_SScalerInputVSize_Byte0		0x32f2
#define Scalar_StripeInternalParams_1_s16_SScalerInputVSize_Byte1		0x32f3
#define Scalar_StripeInternalParams_1_s16_SCropperOutputHSize_Byte0		0x32f4
#define Scalar_StripeInternalParams_1_s16_SCropperOutputHSize_Byte1		0x32f5
#define Scalar_StripeInternalParams_1_s16_SCropperOutputVSize_Byte0		0x32f6
#define Scalar_StripeInternalParams_1_s16_SCropperOutputVSize_Byte1		0x32f7
#define Scalar_StripeInternalParams_1_u8_PreTopBorder_Byte0		0x32f8
#define Scalar_StripeInternalParams_1_u8_PreBottomBorder_Byte0		0x32f9
#define Scalar_StripeInternalParams_1_u8_PreLeftBorder_Byte0		0x32fa
#define Scalar_StripeInternalParams_1_u8_PreRightBorder_Byte0		0x32fb
#define Scalar_StripeInternalParams_1_u8_PostTopBorder_Byte0		0x32fc
#define Scalar_StripeInternalParams_1_u8_PostBottomBorder_Byte0		0x32fd
#define Scalar_StripeInternalParams_1_u8_PostLeftBorder_Byte0		0x32fe
#define Scalar_StripeInternalParams_1_u8_PostRightBorder_Byte0		0x32ff

// page 'g_SpecialEffects_Control[0]'

#define SpecialEffects_Control_0_e_SFXSolarisControl_Byte0		0x3300
#define SpecialEffects_Control_0_e_SFXNegativeControl_Byte0		0x3301
#define SpecialEffects_Control_0_e_BlackAndWhiteControl_Byte0		0x3302
#define SpecialEffects_Control_0_e_SepiaControl_Byte0		0x3303

// page 'g_SpecialEffects_Control[1]'

#define SpecialEffects_Control_1_e_SFXSolarisControl_Byte0		0x3340
#define SpecialEffects_Control_1_e_SFXNegativeControl_Byte0		0x3341
#define SpecialEffects_Control_1_e_BlackAndWhiteControl_Byte0		0x3342
#define SpecialEffects_Control_1_e_SepiaControl_Byte0		0x3343

// page 'g_Event0_Count'

#define Event0_Count_u16_EVENT0_0_HOST_COMMS_READY_Byte0		0x3380
#define Event0_Count_u16_EVENT0_0_HOST_COMMS_READY_Byte1		0x3381
#define Event0_Count_u16_EVENT0_1_HOST_COMMS_OPERATION_COMPLETE_Byte0		0x3382
#define Event0_Count_u16_EVENT0_1_HOST_COMMS_OPERATION_COMPLETE_Byte1		0x3383
#define Event0_Count_u16_EVENT0_2_BOOT_COMPLETE_Byte0		0x3384
#define Event0_Count_u16_EVENT0_2_BOOT_COMPLETE_Byte1		0x3385
#define Event0_Count_u16_EVENT0_3_SLEEPING_Byte0		0x3386
#define Event0_Count_u16_EVENT0_3_SLEEPING_Byte1		0x3387
#define Event0_Count_u16_EVENT0_4_WOKEN_UP_Byte0		0x3388
#define Event0_Count_u16_EVENT0_4_WOKEN_UP_Byte1		0x3389
#define Event0_Count_u16_EVENT0_5_ISP_STREAMING_Byte0		0x338a
#define Event0_Count_u16_EVENT0_5_ISP_STREAMING_Byte1		0x338b
#define Event0_Count_u16_EVENT0_6_ISP_STOP_Byte0		0x338c
#define Event0_Count_u16_EVENT0_6_ISP_STOP_Byte1		0x338d
#define Event0_Count_u16_EVENT0_7_SENSOR_STOP_Byte0		0x338e
#define Event0_Count_u16_EVENT0_7_SENSOR_STOP_Byte1		0x338f
#define Event0_Count_u16_EVENT0_8_SENSOR_START_Byte0		0x3390
#define Event0_Count_u16_EVENT0_8_SENSOR_START_Byte1		0x3391
#define Event0_Count_u16_EVENT0_9_ISP_LOAD_READY_Byte0		0x3392
#define Event0_Count_u16_EVENT0_9_ISP_LOAD_READY_Byte1		0x3393
#define Event0_Count_u16_EVENT0_10_ZOOM_CONFIG_REQUEST_DENIED_Byte0		0x3394
#define Event0_Count_u16_EVENT0_10_ZOOM_CONFIG_REQUEST_DENIED_Byte1		0x3395
#define Event0_Count_u16_EVENT0_11_ZOOM_CONFIG_REQUEST_REPROGAM_REQUIRED_Byte0		0x3396
#define Event0_Count_u16_EVENT0_11_ZOOM_CONFIG_REQUEST_REPROGAM_REQUIRED_Byte1		0x3397
#define Event0_Count_u16_EVENT0_12_ZOOM_STEP_COMPLETE_Byte0		0x3398
#define Event0_Count_u16_EVENT0_12_ZOOM_STEP_COMPLETE_Byte1		0x3399
#define Event0_Count_u16_EVENT0_13_ZOOM_SET_OUTOF_RANGE_Byte0		0x339a
#define Event0_Count_u16_EVENT0_13_ZOOM_SET_OUTOF_RANGE_Byte1		0x339b
#define Event0_Count_u16_EVENT0_14_STREAMING_ERROR_Byte0		0x339c
#define Event0_Count_u16_EVENT0_14_STREAMING_ERROR_Byte1		0x339d
#define Event0_Count_u16_EVENT0_15_RESET_ISP_COMPLETE_Byte0		0x339e
#define Event0_Count_u16_EVENT0_15_RESET_ISP_COMPLETE_Byte1		0x339f
#define Event0_Count_u16_EVENT0_16_MASTER_I2C_ACCESS_FAILED_Byte0		0x33a0
#define Event0_Count_u16_EVENT0_16_MASTER_I2C_ACCESS_FAILED_Byte1		0x33a1
#define Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0		0x33a2
#define Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte1		0x33a3
#define Event0_Count_u16_EVENT0_18_HISTOGRAM_STATS_READY_Byte0		0x33a4
#define Event0_Count_u16_EVENT0_18_HISTOGRAM_STATS_READY_Byte1		0x33a5
#define Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0		0x33a6
#define Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte1		0x33a7
#define Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0		0x33a8
#define Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte1		0x33a9
#define Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0		0x33aa
#define Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte1		0x33ab
#define Event0_Count_u16_EVENT0_22_ZOOM_OUTPUT_RESOLUTION_READY_Byte0		0x33ac
#define Event0_Count_u16_EVENT0_22_ZOOM_OUTPUT_RESOLUTION_READY_Byte1		0x33ad
#define Event0_Count_u16_EVENT0_23_reserved01_Byte0		0x33ae
#define Event0_Count_u16_EVENT0_23_reserved01_Byte1		0x33af
#define Event0_Count_u16_EVENT0_24_reserved02_Byte0		0x33b0
#define Event0_Count_u16_EVENT0_24_reserved02_Byte1		0x33b1
#define Event0_Count_u16_EVENT0_25_reserved03_Byte0		0x33b2
#define Event0_Count_u16_EVENT0_25_reserved03_Byte1		0x33b3
#define Event0_Count_u16_EVENT0_26_NVM_Export_Byte0		0x33b4
#define Event0_Count_u16_EVENT0_26_NVM_Export_Byte1		0x33b5
#define Event0_Count_u16_EVENT0_27_SensorTuning_Available_Byte0		0x33b6
#define Event0_Count_u16_EVENT0_27_SensorTuning_Available_Byte1		0x33b7
#define Event0_Count_u16_EVENT0_28_Power_Notification_Byte0		0x33b8
#define Event0_Count_u16_EVENT0_28_Power_Notification_Byte1		0x33b9
#define Event0_Count_u16_EVENT0_29_SMS_DONE_Byte0		0x33ba
#define Event0_Count_u16_EVENT0_29_SMS_DONE_Byte1		0x33bb
#define Event0_Count_u16_EVENT0_30_Sensor_Output_Mode_Export_Notification_Byte0		0x33bc
#define Event0_Count_u16_EVENT0_30_Sensor_Output_Mode_Export_Notification_Byte1		0x33bd
#define Event0_Count_u16_EVENT0_31_Valid_Frame_Notification_Byte0		0x33be
#define Event0_Count_u16_EVENT0_31_Valid_Frame_Notification_Byte1		0x33bf

// page 'g_CE_LumaOffset[0]'

#define CE_LumaOffset_0_s16_LumaOffset_Byte0		0x33c0
#define CE_LumaOffset_0_s16_LumaOffset_Byte1		0x33c1
#define CE_LumaOffset_0_u8_YFloor_Saturated_Byte0		0x33c2
#define CE_LumaOffset_0_u8_CBFloor_Saturated_Byte0		0x33c3
#define CE_LumaOffset_0_u8_CRFloor_Saturated_Byte0		0x33c4

// page 'g_CE_LumaOffset[1]'

#define CE_LumaOffset_1_s16_LumaOffset_Byte0		0x3400
#define CE_LumaOffset_1_s16_LumaOffset_Byte1		0x3401
#define CE_LumaOffset_1_u8_YFloor_Saturated_Byte0		0x3402
#define CE_LumaOffset_1_u8_CBFloor_Saturated_Byte0		0x3403
#define CE_LumaOffset_1_u8_CRFloor_Saturated_Byte0		0x3404

// page 'g_CE_GammaLastPixelValueControl[0]'

#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte0		0x3440
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte1		0x3441
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte0		0x3442
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte1		0x3443
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte0		0x3444
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte1		0x3445
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte0		0x3446
#define CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte1		0x3447
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte0		0x3448
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte1		0x3449
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte0		0x344a
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte1		0x344b
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte0		0x344c
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte1		0x344d
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte0		0x344e
#define CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte1		0x344f

// page 'g_CE_GammaLastPixelValueControl[1]'

#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte0		0x3480
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte1		0x3481
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte0		0x3482
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte1		0x3483
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte0		0x3484
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte1		0x3485
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte0		0x3486
#define CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte1		0x3487
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte0		0x3488
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte1		0x3489
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte0		0x348a
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte1		0x348b
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte0		0x348c
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte1		0x348d
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte0		0x348e
#define CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte1		0x348f

// page 'g_FLADriver_LensLLDParam'

#define FLADriver_LensLLDParam_u32_NVMLensUnitMovementTime_us_Byte0		0x34c0
#define FLADriver_LensLLDParam_u32_NVMLensUnitMovementTime_us_Byte1		0x34c1
#define FLADriver_LensLLDParam_u32_NVMLensUnitMovementTime_us_Byte2		0x34c2
#define FLADriver_LensLLDParam_u32_NVMLensUnitMovementTime_us_Byte3		0x34c3
#define FLADriver_LensLLDParam_u32_TimeTakenByLensAPIs_us_Byte0		0x34c4
#define FLADriver_LensLLDParam_u32_TimeTakenByLensAPIs_us_Byte1		0x34c5
#define FLADriver_LensLLDParam_u32_TimeTakenByLensAPIs_us_Byte2		0x34c6
#define FLADriver_LensLLDParam_u32_TimeTakenByLensAPIs_us_Byte3		0x34c7
#define FLADriver_LensLLDParam_u32_FLADIntTimer2Count_Byte0		0x34c8
#define FLADriver_LensLLDParam_u32_FLADIntTimer2Count_Byte1		0x34c9
#define FLADriver_LensLLDParam_u32_FLADIntTimer2Count_Byte2		0x34ca
#define FLADriver_LensLLDParam_u32_FLADIntTimer2Count_Byte3		0x34cb
#define FLADriver_LensLLDParam_u32_FLADTimer2CallCount_Byte0		0x34cc
#define FLADriver_LensLLDParam_u32_FLADTimer2CallCount_Byte1		0x34cd
#define FLADriver_LensLLDParam_u32_FLADTimer2CallCount_Byte2		0x34ce
#define FLADriver_LensLLDParam_u32_FLADTimer2CallCount_Byte3		0x34cf
#define FLADriver_LensLLDParam_u16_DiffFromTarget_Byte0		0x34d0
#define FLADriver_LensLLDParam_u16_DiffFromTarget_Byte1		0x34d1
#define FLADriver_LensLLDParam_e_FLADriver_APIError_Type_Byte0		0x34d2
#define FLADriver_LensLLDParam_e_FLADriver_Timer2Id_Type_Byte0		0x34d3

// page 'g_SDL_ELT'

#define SDL_ELT_u16_LastElementinLUT_GIR_Byte0		0x3500
#define SDL_ELT_u16_LastElementinLUT_GIR_Byte1		0x3501
#define SDL_ELT_u16_LastElementinLUT_RED_Byte0		0x3502
#define SDL_ELT_u16_LastElementinLUT_RED_Byte1		0x3503
#define SDL_ELT_u16_LastElementinLUT_BLUE_Byte0		0x3504
#define SDL_ELT_u16_LastElementinLUT_BLUE_Byte1		0x3505
#define SDL_ELT_u16_LastElementinLUT_GIB_Byte0		0x3506
#define SDL_ELT_u16_LastElementinLUT_GIB_Byte1		0x3507
#define SDL_ELT_u16_PixelShift_Byte0		0x3508
#define SDL_ELT_u16_PixelShift_Byte1		0x3509

// page 'g_HostComms_PEDump'

#define HostComms_PEDump_u32_Ptr_to_pageDump_Byte0		0x3540
#define HostComms_PEDump_u32_Ptr_to_pageDump_Byte1		0x3541
#define HostComms_PEDump_u32_Ptr_to_pageDump_Byte2		0x3542
#define HostComms_PEDump_u32_Ptr_to_pageDump_Byte3		0x3543

// page 'g_Event1_Count'

#define Event1_Count_u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS_Byte0		0x3580
#define Event1_Count_u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS_Byte1		0x3581
#define Event1_Count_u16_EVENT1_1_SENSOR_COMMIT_Byte0		0x3582
#define Event1_Count_u16_EVENT1_1_SENSOR_COMMIT_Byte1		0x3583
#define Event1_Count_u16_EVENT1_2_ISP_COMMIT_Byte0		0x3584
#define Event1_Count_u16_EVENT1_2_ISP_COMMIT_Byte1		0x3585
#define Event1_Count_u16_EVENT1_3_HR_GAMMA_UPDATE_COMPLETE_Byte0		0x3586
#define Event1_Count_u16_EVENT1_3_HR_GAMMA_UPDATE_COMPLETE_Byte1		0x3587
#define Event1_Count_u16_EVENT1_4_LR_GAMMA_UPDATE_COMPLETE_Byte0		0x3588
#define Event1_Count_u16_EVENT1_4_LR_GAMMA_UPDATE_COMPLETE_Byte1		0x3589

// page 'g_Event2_Count'

#define Event2_Count_u16_Reserved_Byte0		0x35c0
#define Event2_Count_u16_Reserved_Byte1		0x35c1

// page 'g_Event3_Count'

#define Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte0		0x3600
#define Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte1		0x3601
#define Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0		0x3602
#define Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte1		0x3603
#define Event3_Count_u16_EVENT3_1_DMA_GRAB_VideoStab_Byte0		0x3604
#define Event3_Count_u16_EVENT3_1_DMA_GRAB_VideoStab_Byte1		0x3605
#define Event3_Count_u16_EVENT3_2_DMA_GRAB_Abort_Byte0		0x3606
#define Event3_Count_u16_EVENT3_2_DMA_GRAB_Abort_Byte1		0x3607

// page 'g_testpattern_SolidColor_data'

#define testpattern_SolidColor_data_u16_SolidColor_data_red_Byte0		0x3640
#define testpattern_SolidColor_data_u16_SolidColor_data_red_Byte1		0x3641
#define testpattern_SolidColor_data_u16_SolidColor_data_gir_Byte0		0x3642
#define testpattern_SolidColor_data_u16_SolidColor_data_gir_Byte1		0x3643
#define testpattern_SolidColor_data_u16_SolidColor_data_blu_Byte0		0x3644
#define testpattern_SolidColor_data_u16_SolidColor_data_blu_Byte1		0x3645
#define testpattern_SolidColor_data_u16_SolidColor_data_gib_Byte0		0x3646
#define testpattern_SolidColor_data_u16_SolidColor_data_gib_Byte1		0x3647

// page 'g_testpattern_Cursors_values'

#define testpattern_Cursors_values_u8_hcur_posn_per_Byte0		0x3680
#define testpattern_Cursors_values_u8_hcur_width_Byte0		0x3681
#define testpattern_Cursors_values_u8_vcur_posn_per_Byte0		0x3682
#define testpattern_Cursors_values_u8_vcur_width_Byte0		0x3683
#endif	// _BASELINE_H_
