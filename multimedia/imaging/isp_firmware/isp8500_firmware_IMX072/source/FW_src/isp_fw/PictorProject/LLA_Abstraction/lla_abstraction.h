/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/
#ifndef _LLA_ABSTRACTION_H_
#   define _LLA_ABSTRACTION_H_

#   include "cam_drv.h"
#   include "cam_drv_platform_hardcoding.h"
#   include "Exposure.h"
#   include "FrameRate_op_interface.h"

#define GET_ABS_DIFF(a, b)  ((a) > (b) ? ((a) - (b)) : ((b) - (a)))  //Both a and b are positive
extern void lla_abstractionInit ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstractionSensorInit ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_ConfigureSensor ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_ReprogramSensorForZoom (uint8_t) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_StartVFInTrial ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_StartSensor ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstractionSensorOff ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstractionInit_Configuration ( void ) TO_EXT_DDR_PRGM_MEM;
extern uint8_t lla_abstractionFrameDimension_RequestFrameConfig
    (
        uint16_t u16_DesiredFOVX, uint16_t u16_DesiredFOVY, uint16_t u16_RequiredLineLength, uint16_t u16_MaxLineLength,
            int16_t s16_CenterXOffset, int16_t s16_CenterYOffset, uint8_t e_Flag_ScaleLineLengthForDerating, uint8_t     e_Flag_EnableModeSelection
    ) TO_EXT_DDR_PRGM_MEM;
extern void lla_ConfigureWOIandFrameRate
    (
        uint16 u16_Pipe0OpSizeX, uint16 u16_Pipe0OpSizeY, uint16 u16_DesiredFOVX, uint16 u16_DesiredFOVY, float
            f_FrameRate_Hz
    ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_ConfigureFlash ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_ResetConfigContents ( void ) TO_EXT_DDR_PRGM_MEM;
extern void lla_abstraction_ResetInterpretSensorSettingsContents (void) TO_EXT_DDR_PRGM_MEM;
extern void LLA_AbstractionRegisterReq (CAM_DRV_EVENT_E event_code) TO_EXT_DDR_PRGM_MEM;
extern void     LLA_InterruptTimer_ISR (void);
extern void     LLA_InterruptTimer2_ISR (void);
extern void     LLA_Abstraction_SendSignal_FSC (void);
extern void     LLA_Abstraction_SendSignal_FEC (void);
extern Flag_te  LLA_Abstraction_IsExpGainSettingsAbsorbed (void)TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_SetStreamStatus (Flag_te Flag_StreamingState) TO_EXT_DDR_PRGM_MEM;
extern CAM_DRV_FN_RETVAL_E LLA_Abstracetion_SendDrvMsg
    (CAM_DRV_MSG_E msg_id, void *p_msg_info, uint16 msg_size) TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_CamDrvHandleMsg (void)  TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_ApplyAec(float_t f_FrameRate_Hz, uint32_t u32_TargetExposureTime_us, uint16_t u16_TargetAnalogGain_x256);
extern void LLA_Abstraction_ApplyAecAndFreameRate (Exposure_DriverControls_ts  *ptr_Exposure_DriverControls) TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_Set_NDFilter ( void ) TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_ConfigSet_streaming ( void ) TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_InterpretSensorSettings (void);
extern void lla_abstraction_Configure_VF ( void ) TO_EXT_DDR_PRGM_MEM;
extern void LLA_Abstraction_Lens_Stop(void) TO_EXT_DDR_PRGM_MEM;

/**
    Details Structure
*/
extern CAM_DRV_CAMERA_DETAILS_T g_camera_details                            TO_EXT_DATA_MEM;

extern CAM_DRV_CONFIG_T                                                     g_cam_drv_config TO_EXT_DATA_MEM;
extern CAM_DRV_SENS_CONFIG_WOI_T g_config_woi                               TO_EXT_DATA_MEM;
extern CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T g_config_gain_and_exp_time   TO_EXT_DATA_MEM;
extern CAM_DRV_SENS_CONFIG_FRAME_RATE_T g_config_frame_rate                 TO_EXT_DATA_MEM;
extern CAM_DRV_SENS_CONFIG_TEST_PICTURE_T g_config_test_picture             TO_EXT_DATA_MEM;
extern CAM_DRV_CAMERA_CONFIGURATION_DATA_T g_config_camera_data             TO_EXT_DATA_MEM;
extern CAM_DRV_AWB_DATA_T g_config_awb_data                                 TO_EXT_DATA_MEM;
extern CAM_DRV_SENS_CONFIG_ZOOM_ROI_T g_config_zoom_roi                     TO_EXT_DATA_MEM;
extern CAM_DRV_CONFIG_DATA_IDENTIFIERS_T g_config_ids                       TO_EXT_DATA_MEM;
extern CAM_DRV_CONFIG_FEEDBACK_T                                            g_config_feedback TO_EXT_DATA_MEM;

#   define CURRENT_SENSOR_WOI_X()                  (g_config_feedback.woi_res.width)
#   define CURRENT_SENSOR_WOI_Y()                  (g_config_feedback.woi_res.height)
#   define CURRENT_SENSOR_OUTPUT_X()               (g_config_feedback.output_res.width)
#   define CURRENT_SENSOR_OUTPUT_Y()               (g_config_feedback.output_res.height)
#   define CURRENT_SENSOR_LINE_READOUT_TIME_US()   ((float_t)(g_config_feedback.line_readout_time_ns / 1000.0))
#   define CURRENT_SENSOR_ACTIVEDATA_READOUT_TIME_US()       ((uint32_t) CURRENT_SENSOR_OUTPUT_Y() * CURRENT_SENSOR_LINE_READOUT_TIME_US())
#   define LLD_ZoomTop_GetSensorArraySizeX()   (g_cam_drv_config.p_config_woi->output_res.width)
#   define LLD_ZoomTop_GetSensorArraySizeY()   (g_cam_drv_config.p_config_woi->output_res.height)
#   define MAX_SENSOR_X()                      (g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.width)
#   define MAX_SENSOR_Y()                      (g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.height)
#   define LLA_BINNING_FACTOR()                (g_config_feedback.binning_factor)
#   define GetSensorExposureTimeMax()          (g_config_feedback.current_et_capability.max_exp_time_us)

/**
    Exporting details structure
*/
#   define LLA_GET_NO_OF_STATUS_LINES()    READONLY(g_camera_details.p_sensor_details->frame_format.anc_lines_top)

/*Structure to store event register status*/
typedef struct
{
    uint8_t u8_EventStatus_FSC;
    uint8_t u8_EventStatus_FEC;
} LLA_EventRegisterStatus_ts;
extern LLA_EventRegisterStatus_ts   g_event_status;

/*States of low level camera driver*/
typedef enum
{
    e_LLA_Abstraction_CamDrvState_UnInitialize,
    e_LLA_Abstraction_CamDrvState_Off,
    e_LLA_Abstraction_CamDrvState_Idle,
    e_LLA_Abstraction_CamDrvState_ViewFinder,
    e_LLA_Abstraction_CamDrvState_Capture,
    e_LLA_Abstraction_CamDrvState_WakeUP,
    e_LLA_Abstraction_CamDrvState_Error
} LLA_Abstraction_CamDrvState_te;

///Low level camera driver states
extern LLA_Abstraction_CamDrvState_te g_LLA_Abstraction_CamDrvState;
extern CAM_DRV_SENS_SETTINGS_T g_SensorSettings;
#   define GET_LLA_CAM_DRV_STATE()     (g_LLA_Abstraction_CamDrvState)
#   define SET_LLA_CAM_DRV_STATE(x)    (g_LLA_Abstraction_CamDrvState = x)
#   define LLA_IS_CAM_DRV_ON()                                                            \
        (                                                                                 \
            (e_LLA_Abstraction_CamDrvState_UnInitialize != g_LLA_Abstraction_CamDrvState) \
        &&  (e_LLA_Abstraction_CamDrvState_Off != g_LLA_Abstraction_CamDrvState)          \
        )
#   define LLA_IS_LIT_FRAME()  (Flag_e_TRUE == g_SensorSettings.flash_fired)

//Pending Cam drv messages
extern volatile uint16_t    g_u16DrvPendMsg;
#   define LLA_IS_CAM_DRV_MSG_PENDING()    (0x0 != g_u16DrvPendMsg)

void lla_abstraction_Configure_test_picture ( void ) TO_EXT_DDR_PRGM_MEM;

CAM_DRV_SENS_FORMAT_E lla_HostRawFormatToLLARawFormat (uint16_t u16_HostRawFormat) TO_EXT_DDR_PRGM_MEM;
uint32_t ConvertLLCDRawFormatToCSIRawFormat (uint32_t llcd_raw_format) TO_EXT_DDR_PRGM_MEM;
uint8_t lla_abstraction_user_BMS_input_valid ( void ) TO_EXT_DDR_PRGM_MEM;
#endif /* _LLA_ABSTRACTION_H_ */

