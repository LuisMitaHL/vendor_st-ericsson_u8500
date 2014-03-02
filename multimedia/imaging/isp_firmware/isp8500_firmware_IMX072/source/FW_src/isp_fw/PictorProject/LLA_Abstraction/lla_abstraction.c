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
#include "lla_abstraction.h"
#include "callbacks.h"

#include "Exposure_OPInterface.h"
#include "SensorManager.h"
#include "HostInterface.h"
#include "run_mode_ctrl.h"
#include "Aperture.h"
#include "FLADriver.h"
#include "Sensor_Tuning.h"
#include "Flash.h"
#include "HDR.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_LLA_Abstraction_lla_abstractionTraces.h"
#endif
#include "FrameRate_op_interface.h"

#if SEND_ISL_DUMP


//ISL Defines
#   define ISL_RAW8_FRAME_COUNT_OFFSET                 6
#   define ISL_RAW8_FINE_INTEGRATION_OFFSET_HIGH       12
#   define ISL_RAW8_FINE_INTEGRATION_OFFSET_LOW        14
#   define ISL_RAW8_COARSE_INTEGRATION_OFFSET_HIGH     16
#   define ISL_RAW8_COARSE_INTEGRATION_OFFSET_LOW      18
#   define ISL_RAW8_ANALOGUE_GAIN_OFFSET_HIGH          20
#   define ISL_RAW8_ANALOGUE_GAIN_OFFSET_LOW           22
#   define ISL_RAW8_FRAME_LENGTH_LINES_OFFSET_HIGH     28
#   define ISL_RAW8_FRAME_LENGTH_LINES_OFFSET_LOW      30
#   define ISL_RAW8_LINE_LENGTH_PCK_OFFSET_HIGH        32
#   define ISL_RAW8_LINE_LENGTH_PCK_OFFSET_LOW         34
#   define ISL_RAW8_DIGITAL_GAIN_GREENR_HIGH           40
#   define ISL_RAW8_DIGITAL_GAIN_GREENR_LOW            42
#   define ISL_RAW8_FLASH_STATUS_OFFSET_LOW            48

#   define ISL_RAW10_FRAME_COUNT_OFFSET                7
#   define ISL_RAW10_FINE_INTEGRATION_OFFSET_HIGH      15
#   define ISL_RAW10_FINE_INTEGRATION_OFFSET_LOW       17
#   define ISL_RAW10_COARSE_INTEGRATION_OFFSET_HIGH    20
#   define ISL_RAW10_COARSE_INTEGRATION_OFFSET_LOW     22
#   define ISL_RAW10_ANALOGUE_GAIN_OFFSET_HIGH         25
#   define ISL_RAW10_ANALOGUE_GAIN_OFFSET_LOW          27
#   define ISL_RAW10_FRAME_LENGTH_LINES_OFFSET_HIGH    35
#   define ISL_RAW10_FRAME_LENGTH_LINES_OFFSET_LOW     37
#   define ISL_RAW10_LINE_LENGTH_PCK_OFFSET_HIGH       40
#   define ISL_RAW10_LINE_LENGTH_PCK_OFFSET_LOW        42
#   define ISL_RAW10_DIGITAL_GAIN_GREENR_HIGH          50
#   define ISL_RAW10_DIGITAL_GAIN_GREENR_LOW           52
#   define ISL_RAW10_FLASH_STATUS_OFFSET_LOW           60

uint8_t g_pu8_ISL_Raw8[] =
{
    0x0A,   // Data Format Code
    0xAA,
    0x00,
    0xA5,
    0x05,
    0x5A,
    0xFF,   // FRAME_COUNT
    0xAA,
    0x02,
    0xA5,
    0x00,
    0x5A,
    0xFF,
    0x5A,
    0xFF,   // Fine integration
    0x5A,
    0xFF,
    0x5A,
    0xFF,   // Coarse integration
    0x5A,
    0xFF,
    0x5A,
    0xFF,   // Analogue Gain
    0xAA,
    0x03,
    0xA5,
    0x40,
    0x5A,
    0xFF,
    0x5A,
    0xFF,   // FRAME_LENGTH_LINES_HI
    0x5A,
    0xFF,
    0x5A,
    0xFF,   // LINE_LENGTH_PCK_HI
    0xAA,
    0x02,
    0xA5,
    0x0E,
    0x5A,
    0xFF, //DIGITAL_GAIN_GREENR_HI
    0x5A,
    0xFF, //DIGITAL_GAIN_GREENR_LOW
    0xAA,
    (FLASH_STATUS_REGSTER & 0xFF00) >> 8,   //0x0C,
    0xA5,
    (FLASH_STATUS_REGSTER & 0x00FF),        //0x1C,
    0x5A,
    0xFF,   //FLASH_STATUS_REGSTER
    0x07    // End Codes
};

uint8_t g_pu8_ISL_Raw10[] =
{
    0x0A,   // Data Format Code
    0xAA,
    0x00,
    0xA5,
    0x55,
    0x05,
    0x5A,
    0xFF,   // FRAME_COUNT
    0xAA,
    0x55,
    0x02,
    0xA5,
    0x00,
    0x5A,
    0x55,
    0xFF,
    0x5A,
    0xFF,   // Fine integration
    0x5A,
    0x55,
    0xFF,
    0x5A,
    0xFF,   // Coarse integration
    0x5A,
    0x55,
    0xFF,
    0x5A,
    0xFF,   // Analogue Gain
    0xAA,
    0x55,
    0x03,
    0xA5,
    0x40,
    0x5A,
    0x55,
    0xFF,
    0x5A,
    0xFF,   // FRAME_LENGTH_LINES_HI
    0x5A,
    0x55,
    0xFF,
    0x5A,
    0xFF,   // LINE_LENGTH_PCK_HI
    0xAA,
    0x55,
    0x02,
    0xA5,
    0x0E,
    0x5A,
    0x55,
    0xFF, //DIGITAL_GAIN_GREENR_HI:
    0x5A,
    0xFF, //DIGITAL_GAIN_GREENR_LOW
    0xAA,
    0x55,
    (FLASH_STATUS_REGSTER & 0xFF00) >> 8,   //0x0C,
    0xA5,
    (FLASH_STATUS_REGSTER & 0x00FF),        //0x1C,
    0x5A,
    0X55,
    0xFF,   //FLASH_STATUS_REGSTER
    0x07    // End Codes
};
#endif //SEND_ISL_DUMP

/**
    Callback structure is defined in callback.c/h
*/

/**
    Details Structure
*/
CAM_DRV_CAMERA_DETAILS_T                g_camera_details = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/**
    Configuration Structure
*/
CAM_DRV_CONFIG_T                        g_cam_drv_config;
CAM_DRV_SENS_CONFIG_WOI_T               g_config_woi;
CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T g_config_gain_and_exp_time = { 15000, 1000, 1000 }; //cnanda - defaukt values for exp and gain
CAM_DRV_SENS_CONFIG_FRAME_RATE_T        g_config_frame_rate;
CAM_DRV_SENS_CONFIG_TEST_PICTURE_T      g_config_test_picture;
CAM_DRV_CAMERA_CONFIGURATION_DATA_T     g_config_camera_data;
CAM_DRV_AWB_DATA_T                      g_config_awb_data;
CAM_DRV_SENS_CONFIG_ZOOM_ROI_T          g_config_zoom_roi;
CAM_DRV_CONFIG_FLASH_STROBES_T          g_config_flash_strobes;
CAM_DRV_CONFIG_FLASH_STROBE_T           g_config_each_strobe;
CAM_DRV_CONFIG_DATA_IDENTIFIERS_T       g_config_ids = { NULL, 0 };

/**
    Configuration feedback Structure
*/
CAM_DRV_CONFIG_FEEDBACK_T               g_config_feedback;

/**
configuration structures to be used by exposure and framerate for configuring sensor
**/
CAM_DRV_SENS_CONFIG_FRAME_RATE_T        g_ConfigFrameRate;
CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T g_ExpGainSetting;

/**
Structure obtained when reinterpret sensor settings is done
**/
CAM_DRV_SENS_SETTINGS_T                 g_SensorSettings;

/**
    VF configuration
*/
CAM_DRV_CONFIG_VF_T                     g_vf_config;

/**
    Capture configuration
*/
CAM_DRV_CONFIG_CAPTURE_T                g_capture_config;

///Low level camera driver state
LLA_Abstraction_CamDrvState_te             g_LLA_Abstraction_CamDrvState;

///Status of events register to FW
LLA_EventRegisterStatus_ts              g_event_status;

// Last mode selected: to be used if Changeover is not allowed.
uint16_t    g_u16Lastmode_Selected = 0xFFFF;

///Status variable to keep track of pending messages sent by low level driver
///There is a bit in this variable for each possible message, in current implementation it is assumed that no two
///messages would be pending at same time
volatile uint16_t                       g_u16DrvPendMsg = 0;                                /*Initially no message is pending */

CAM_DRV_RETVAL_E camera_on ( void ) TO_EXT_DDR_PRGM_MEM;
static void LLA_Setup_FrameFormat (CAM_DRV_CAMERA_DETAILS_T * p_camera_details) TO_EXT_DDR_PRGM_MEM;
static void LLA_Update_FlashSupportDetails ( void ) TO_EXT_DDR_PRGM_MEM;

void lla_abstraction_set_orientation (CAM_DRV_SENS_ORIENTATION_T * ptr_orientation) TO_EXT_DDR_PRGM_MEM;
static void lla_abstraction_update_frame_format (CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback) TO_EXT_DDR_PRGM_MEM;
static void lla_abstraction_update_exposure_limits (CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback) TO_EXT_DDR_PRGM_MEM;
static void lla_abstraction_update_analog_limits ( void ) TO_EXT_DDR_PRGM_MEM;
void lla_abstraction_ConvertFeedbackToFrameParamStatus (CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback) TO_EXT_DDR_PRGM_MEM;
void lla_abstraction_ConvertFeedbackToPageElemets (CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback, Flag_te e_Flag_trial_only) TO_EXT_DDR_PRGM_MEM;
void lla_abstraction_ConvertFeedbackToTestPatternStatus ( void ) TO_EXT_DDR_PRGM_MEM;
void lla_abstraction_Configure_camera_data(void)TO_EXT_DDR_PRGM_MEM;

CAM_DRV_SENS_MODE_E lla_abstraction_FindSensorModeFromUserRestrictionBitmask(uint8_t)TO_EXT_DDR_PRGM_MEM;
/**
 \if INCLUDE_IN_HTML_ONLY
 \fn  CAM_DRV_RETVAL_E  camera_on(void)
 \brief
 \details
 \param void
 \return CAM_DRV_RETVAL_E
 \ingroup SensorManager
 \callgraph
 \callergraph
 \endif
*/
CAM_DRV_RETVAL_E
camera_on(void)
{
    CAM_DRV_RETVAL_E        ReturnVal;
    CAM_DRV_CONFIG_ON_T     cam_drv_config_on;
    CAM_DRV_CAMERA_TYPE_T   cam_drv_camera_type;

    cam_drv_config_on.ext_clock_freq_x100 = (uint32) (SystemConfig_GetSensorInputClockFreq_Mhz() * 100);

    cam_drv_config_on.selected_cam = SystemConfig_IsInputImageSourceSensor0() ? CAM_DRV_CAMERA_PRIMARY : CAM_DRV_CAMERA_SECONDARY;

    //  cam_drv_config_on.image_data_interface_settings.channel_identifier == DMA/VC;
    //  cam_drv_config_on.operating_voltages = 0;
    if (SystemConfig_IsInputInterfaceCSI2_0() || SystemConfig_IsInputInterfaceCSI2_1())
    {
        cam_drv_config_on.requested_link_bit_rate_mbps = g_VideoTimingHostInputs.f_HostRxMaxDataRate_Mbps;
        cam_drv_config_on.image_data_interface_settings.CCP2_speed_max_Mbps = (uint16) g_VideoTimingHostInputs.f_HostRxMaxDataRate_Mbps;
        cam_drv_config_on.image_data_interface_settings.mode = CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CSI2;

        cam_drv_config_on.image_data_interface_settings.channel_identifier = 0;

        if ((1 == SystemConfig_GetNumberOfCSI2DataLines()))         // && (1 == g_camera_details.p_sensor_details->csi_signalling_options.csi2_1_lane_supported))
        {
            cam_drv_config_on.image_data_interface_settings.CSI2_lane_mode = CAM_DRV_SENS_CSI2_LANE_MODE_1;
        }
        else if ((2 == SystemConfig_GetNumberOfCSI2DataLines()))    // && (1 == g_camera_details.p_sensor_details->csi_signalling_options.csi2_2_lane_supported) )
        {
            cam_drv_config_on.image_data_interface_settings.CSI2_lane_mode = CAM_DRV_SENS_CSI2_LANE_MODE_2;
        }
        else if ((3 == SystemConfig_GetNumberOfCSI2DataLines()))    //&& (1 == g_camera_details.p_sensor_details->csi_signalling_options.csi2_3_lane_supported))
        {
            cam_drv_config_on.image_data_interface_settings.CSI2_lane_mode = CAM_DRV_SENS_CSI2_LANE_MODE_3;
        }
        else if ((4 == SystemConfig_GetNumberOfCSI2DataLines()))    // && (1 == g_camera_details.p_sensor_details->csi_signalling_options.csi2_4_lane_supported))
        {
            cam_drv_config_on.image_data_interface_settings.CSI2_lane_mode = CAM_DRV_SENS_CSI2_LANE_MODE_4;
        }
    }
    else if (SystemConfig_IsInputInterfaceCCP())
    {
        cam_drv_config_on.image_data_interface_settings.channel_identifier = 0;
        cam_drv_config_on.image_data_interface_settings.mode = (SensorBitsPerSystemClock_e_DATA_STROBE == VideoTiming_GetSensorBitsPerSystemClock()) ? CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_STROBE : CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_CLOCK;
        cam_drv_config_on.image_data_interface_settings.CCP2_speed_max_Mbps = (uint16) g_VideoTimingHostInputs.f_HostRxMaxDataRate_Mbps;
    }


    ReturnVal = cam_drv_on(&cam_drv_config_on, &cam_drv_camera_type);

    if (CAM_DRV_OK == ReturnVal)
    {
#if 0
        g_SensorInformation.u16_model_id = (uint16_t) (cam_drv_camera_type.id_string[2] << 8) | cam_drv_camera_type.id_string[3];
        g_SensorInformation.u8_revision_number = cam_drv_camera_type.id_string[6];

        //cam_drv_camera_type.camera_model;
        g_SensorInformation.u8_manufacturer_id = cam_drv_camera_type.id_string[3];
#else
        g_SensorInformation.u16_model_id =
            (
                HexChartoInt(cam_drv_camera_type.id_string[2]) *
                GenericFunctions_PowerOfInt(HEX_BASE, 3)
            ) +
            (HexChartoInt(cam_drv_camera_type.id_string[3]) * GenericFunctions_PowerOfInt(HEX_BASE, 2)) +
            (HexChartoInt(cam_drv_camera_type.id_string[4]) * HEX_BASE) +
            (HexChartoInt(cam_drv_camera_type.id_string[5]));
        g_SensorInformation.u8_revision_number = (HexChartoInt(cam_drv_camera_type.id_string[6]) * HEX_BASE) + (HexChartoInt(cam_drv_camera_type.id_string[7]));

        g_SensorInformation.u8_manufacturer_id = (HexChartoInt(cam_drv_camera_type.id_string[0]) * HEX_BASE) + (HexChartoInt(cam_drv_camera_type.id_string[1]));
#endif

        g_SensorInformation.u8_smia_version = cam_drv_camera_type.camera_register_map_version;
        g_SensorInformation.e_Flag_Available = (CAM_DRV_FN_OK == ReturnVal ? Flag_e_TRUE : Flag_e_FALSE);

        //updating the page elements with serial number
        //serial_id is totally six bytes

        /*cam_drv_camera_type.camera_serial_number[] is a 16 bytes char array in this
                firsrt 8 bytes contains the serial_id
                camera_serial_number[]: 0 byte serial_id_0
                                        1 byte serial_id_1
                                        2 byte serial_id_2
                                        3 byte Reserved
                                        4 byte serial_id_3
                                        5 byte serial_id_4
                                        6 byte serial_id_5
                                        7 byte Reserved  */
        g_SensorInformation.u8_serial_id_0 = (uint8_t) cam_drv_camera_type.camera_serial_number[0];
        g_SensorInformation.u8_serial_id_1 = (uint8_t) cam_drv_camera_type.camera_serial_number[1];
        g_SensorInformation.u8_serial_id_2 = (uint8_t) cam_drv_camera_type.camera_serial_number[2];
        g_SensorInformation.u8_serial_id_3 = (uint8_t) cam_drv_camera_type.camera_serial_number[4];
        g_SensorInformation.u8_serial_id_4 = (uint8_t) cam_drv_camera_type.camera_serial_number[5];
        g_SensorInformation.u8_serial_id_5 = (uint8_t) cam_drv_camera_type.camera_serial_number[6];
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!camera_on() - cam_drv_on() returned failure!!");
#endif
        ASSERT_XP70();
    }


    //Set camera driver to in on state
    SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_Idle);

    return (ReturnVal);
}


void
lla_abstractionInit_Configuration(void)
{
    g_cam_drv_config.p_config_awb_data = &g_config_awb_data;
    g_cam_drv_config.p_config_camera_data = &g_config_camera_data;
    g_cam_drv_config.p_config_frame_rate = &g_config_frame_rate;
    g_cam_drv_config.p_config_gain_and_exp_time = &g_config_gain_and_exp_time;
    g_cam_drv_config.p_config_woi = &g_config_woi;
    g_cam_drv_config.p_config_test_picture = &g_config_test_picture;
    g_cam_drv_config.p_config_zoom_roi = &g_config_zoom_roi;

    g_config_flash_strobes.p_each_strobe_config = &g_config_each_strobe;
    g_cam_drv_config.p_config_flash_strobe = &g_config_flash_strobes;

    lla_abstraction_ResetConfigContents();

    g_camera_details.p_sensor_details = NULL;
    g_camera_details.p_lens_details = NULL;
    g_camera_details.p_shutter_details = NULL;
    g_camera_details.p_nd_details = NULL;
    g_camera_details.p_nvm_details = NULL;
    g_camera_details.p_flash_strobe_details = NULL;
}


//Check whether Flash is supported. If yes, updates the status page.
static void
LLA_Update_FlashSupportDetails(void)
{
    g_FlashStatus.u8_MaxStrobesPerFrame = g_camera_details.p_flash_strobe_details->number_of_strobe_per_frame;

    if (g_FlashStatus.u8_MaxStrobesPerFrame > 0)
    {
        g_FlashStatus.u32_MinStrobeLength_us = g_camera_details.p_flash_strobe_details->min_strobe_length;
        g_FlashStatus.u32_MaxStrobeLength_us = g_camera_details.p_flash_strobe_details->max_strobe_length;
        g_FlashStatus.u32_StrobeLengthStep = g_camera_details.p_flash_strobe_details->strobe_length_step;
        g_FlashStatus.e_Flag_StrobeModulationSupported = g_camera_details.p_flash_strobe_details->strobe_modulation_support;
    }


    return;
}


static void lla_abstraction_update_frame_format(CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback)
{
    g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge = 0;
    g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge = 0;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge = 0;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge = 0;

    //[NON_ISL_SUPPORT] -- when EDL is zero, in that case non_smia_correction_factor will be set to 2.
    //when EDL is not zero but not smia complaint in that case non_smia_correction_factor will be set to 0
    //in smia complaint cases,it will always be 0
    if(p_config_feedback->frame_format.anc_lines_top == 0)
        non_smia_correction_factor = 2;
    else
        non_smia_correction_factor = 0;

    g_CurrentFrameDimension.u8_NumberOfStatusLines = p_config_feedback->frame_format.anc_lines_top + non_smia_correction_factor;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge = p_config_feedback->frame_format.anc_lines_top + non_smia_correction_factor;

    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge = p_config_feedback->frame_format.anc_lines_bottom;

    if (CAM_DRV_SENS_NO_BLACK_PIXELS != p_config_feedback->frame_format.black_pixels_capability)
    {
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge += p_config_feedback->frame_format.black_pixels_left;
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge += p_config_feedback->frame_format.black_pixels_right;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge += p_config_feedback->frame_format.black_pixels_top;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge += p_config_feedback->frame_format.black_pixels_bottom;
    }


    if (CAM_DRV_SENS_DUMMY_PIXELS_NONE != p_config_feedback->frame_format.dummy_pixel_position)
    {
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge += p_config_feedback->frame_format.dummy_pixels_left;
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge += p_config_feedback->frame_format.dummy_pixels_right;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge += p_config_feedback->frame_format.dummy_pixels_top;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge += p_config_feedback->frame_format.dummy_pixels_bottom;
    }
}


static void
LLA_Setup_FrameFormat(
CAM_DRV_CAMERA_DETAILS_T    *p_camera_details)
{
    g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge = 0;
    g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge = 0;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge = 0;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge = 0;

    //[NON_ISL_SUPPORT] -- when EDL is zero, in that case non_smia_correction_factor will be set to 2.
    //when EDL is not zero but not smia complaint in that case non_smia_correction_factor will be set to 0
    //in smia complaint cases,it will always be 0
    if(p_camera_details->p_sensor_details->frame_format.anc_lines_top == 0)
        non_smia_correction_factor = 2;
    else
        non_smia_correction_factor = 0;

    g_CurrentFrameDimension.u8_NumberOfStatusLines = p_camera_details->p_sensor_details->frame_format.anc_lines_top + non_smia_correction_factor;
    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge = p_camera_details->p_sensor_details->frame_format.anc_lines_top + non_smia_correction_factor;

    g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge = p_camera_details->p_sensor_details->frame_format.anc_lines_bottom;

    if (CAM_DRV_SENS_NO_BLACK_PIXELS != p_camera_details->p_sensor_details->frame_format.black_pixels_capability)
    {
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge += p_camera_details->p_sensor_details->frame_format.black_pixels_left;
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge += p_camera_details->p_sensor_details->frame_format.black_pixels_right;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge += p_camera_details->p_sensor_details->frame_format.black_pixels_top;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge += p_camera_details->p_sensor_details->frame_format.black_pixels_bottom;
    }


    if (CAM_DRV_SENS_DUMMY_PIXELS_NONE != p_camera_details->p_sensor_details->frame_format.dummy_pixel_position)
    {
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge += p_camera_details->p_sensor_details->frame_format.dummy_pixels_left;
        g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge += p_camera_details->p_sensor_details->frame_format.dummy_pixels_right;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge += p_camera_details->p_sensor_details->frame_format.dummy_pixels_top;
        g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge += p_camera_details->p_sensor_details->frame_format.dummy_pixels_bottom;
    }
}


static void
lla_abstraction_update_exposure_limits(
CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback)
{
    // update max exposure time
    g_FrameParamStatus.u32_SensorExposureTimeMin_us = p_config_feedback->current_et_capability.min_exp_time_us;

    // update max exposure time
    g_FrameParamStatus.u32_SensorExposureTimeMax_us = GetSensorExposureTimeMax();

    g_FrameParamStatus.u32_ExposureQuantizationStep_us = p_config_feedback->current_et_capability.exp_time_step;
}


static void
lla_abstraction_update_analog_limits(void)
{
    g_FrameParamStatus_Extn.u32_SensorParametersAnalogGainMin_x256 =
        (
            ((uint32_t) g_camera_details.p_sensor_details->analog_gain_capability.min_gain_x1000 * 256) /
            1000
        );
    g_FrameParamStatus_Extn.u32_SensorParametersAnalogGainMax_x256 =
        (
            ((uint32_t) g_camera_details.p_sensor_details->analog_gain_capability.max_gain_x1000 * 256) /
            1000
        );
    g_FrameParamStatus_Extn.u32_SensorParametersAnalogGainStep_x256 =
        (
            ((uint32_t) g_camera_details.p_sensor_details->analog_gain_capability.gain_step_x1000 * 256) /
            1000
        );
}


CAM_DRV_SENS_FORMAT_E
lla_HostRawFormatToLLARawFormat(
uint16_t    u16_HostRawFormat)
{
    CAM_DRV_SENS_FORMAT_E   llaRawFormat = CAM_DRV_SENS_FORMAT_RAW10;   //default
    switch (u16_HostRawFormat)
    {
        case 0x0A0A:    llaRawFormat = CAM_DRV_SENS_FORMAT_RAW10; break;
        case 0x0808:    llaRawFormat = CAM_DRV_SENS_FORMAT_RAW8; break;
        case 0x0A08:    llaRawFormat = CAM_DRV_SENS_FORMAT_RAW8_DPCM; break;
        case 0x0A06:    llaRawFormat = CAM_DRV_SENS_FORMAT_RAW10TO6_DPCM; break;
        default:
                OstTraceInt1(TRACE_DEBUG, "lla_HostRawFormatToLLARawFormat(): %d data format not supported ", u16_HostRawFormat);
                ASSERT_XP70();
            break;
    }

    return (llaRawFormat);
}


uint32_t
ConvertLLCDRawFormatToCSIRawFormat(
uint32_t    llcd_raw_format)
{
    switch (llcd_raw_format)
    {
        case CAM_DRV_SENS_FORMAT_RAW10:         return (0x0A0A); break;
        case CAM_DRV_SENS_FORMAT_RAW8:          return (0x0808); break;
        case CAM_DRV_SENS_FORMAT_RAW8_DPCM:     return (0x0A08); break;
        case CAM_DRV_SENS_FORMAT_RAW10TO6_DPCM: return (0x0A06); break;
        default:                                break;
    }


    return (0);
}


void
lla_abstraction_ConvertFeedbackToFrameParamStatus(
CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback)
{
    g_FrameParamStatus.u32_ExposureTime_us = p_config_feedback->exposure_time_us;
    g_FrameParamStatus.u32_AnalogGain_x256 = ((uint32_t) p_config_feedback->analog_gain_x1000 * 256) / 1000;
    g_FrameParamStatus.u32_frameRate_x100 = p_config_feedback->frame_rate_x100;

    g_FrameParamStatus.u32_ActiveData_ReadoutTime_us = CURRENT_SENSOR_ACTIVEDATA_READOUT_TIME_US();

    g_FrameParamStatus_Extn.u32_focal_length_x100 = p_config_feedback->focal_length_x100;

    lla_abstraction_update_exposure_limits(p_config_feedback);
}


void
lla_abstraction_ConvertFeedbackToPageElemets(
CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback,
Flag_te                      e_Flag_trial_only)
{
    if(Flag_e_FALSE == e_Flag_trial_only)
    {
        g_Exposure_ParametersApplied.u16_AnalogGain_x256 = (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
        g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us = g_config_feedback.exposure_time_us;

        //[PM]Update FrameRate patch for non-trial mode from Hem
        g_FrameDimensionStatus.f_CurrentFrameRate = ((float_t) p_config_feedback->frame_rate_x100) / 100;
        g_VariableFrameRateStatus.f_CurrentFrameRate_Hz = g_FrameDimensionStatus.f_CurrentFrameRate;
    }

    g_FrameDimensionStatus.f_VTLineLength_us = CURRENT_SENSOR_LINE_READOUT_TIME_US();
    g_FrameDimensionStatus.f_VTFrameLength_us = (1 / (((float_t) p_config_feedback->frame_rate_x100) / 100)) * 1000000;

    g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines = (uint16) (g_FrameDimensionStatus.f_VTFrameLength_us / g_FrameDimensionStatus.f_VTLineLength_us);

    /* <AG> todo: put right pre-scaling here? */
    g_CurrentFrameDimension.f_PreScaleFactor = p_config_feedback->woi_res.width / p_config_feedback->output_res.width;

    g_CurrentFrameDimension.u16_VTFrameLengthLines = g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines;

    g_CurrentFrameDimension.u16_OPXOutputSize = p_config_feedback->output_res.width;
    g_CurrentFrameDimension.u16_OPYOutputSize = p_config_feedback->output_res.height;

    g_CurrentFrameDimension.u16_VTLineLengthPck = p_config_feedback->line_length_pck;

    //Fill the VideoTiming page-elements
    g_VideoTimingOutput.f_VTPixelClockFrequency_Mhz = p_config_feedback->vt_pix_clk_freq;
    g_VideoTimingOutput.f_OPPixelClockFrequency_Mhz = p_config_feedback->op_pix_clk_freq;

    g_VideoTimingOutput.f_OutputTimingClockDerating = g_VideoTimingOutput.f_OPPixelClockFrequency_Mhz / g_VideoTimingOutput.f_VTPixelClockFrequency_Mhz;

    // <Todo: BG> Check this...in VT domain, relation between pix_frequency and sys_frequency
    g_VideoTimingOutput.f_VTSystemClockFrequency_Mhz = g_VideoTimingOutput.f_VTPixelClockFrequency_Mhz * (VideoTiming_GetCsiRawFormat() & 0x00FF);

    lla_abstraction_update_frame_format(p_config_feedback);
}


void
lla_abstractionInit(void)
{
    CAM_DRV_RETVAL_E    ReturnVal;
    Flag_te             state;
    OstTraceInt0(TRACE_DEBUG, "<Sensor_Tuning> >> lla_abstractionInit");

    lla_abstractionInit_Configuration();

    // Initialize callback functions
    ReturnVal = cam_drv_init(&g_callback_fn);

    if (CAM_DRV_OK == ReturnVal)
    {
        ReturnVal = camera_on();
        if (CAM_DRV_OK == ReturnVal)
        {
            //query sensor_tuning sub-block IDs
            ReturnVal = cam_drv_get_config_data_identifiers(&g_config_ids);
            if (CAM_DRV_OK == ReturnVal)
            {
                if ((NULL != g_config_ids.identifiers) && (g_config_ids.number_of_identifiers > 0))
                {
                    g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount = g_config_ids.number_of_identifiers;
                    EventManager_sensor_tuning_Available_Notify();
                }
                else
                {
                    g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount = 0;
                }
            }
            else
            {
            OstTraceInt0(TRACE_ERROR, "<Sensor_Tuning> cam_drv_get_config_data_identifiers() returned failure");
                ASSERT_XP70();
            }
        }
        else
        {
        OstTraceInt0(TRACE_ERROR, "<Sensor_Tuning> camera_on() returned failure");
            ASSERT_XP70();
        }
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<Sensor_Tuning> cam_drv_init() returned failure");
        ASSERT_XP70();
    }

    if(Flag_e_TRUE == g_Sensor_Tuning_Control.e_Flag_ReadConfigBeforeBoot)
    {
        OstTraceInt0(TRACE_DEBUG, "<Sensor_Tuning> g_Sensor_Tuning_Control.e_Flag_ReadConfigBeforeBoot == Flag_e_TRUE ");
        while(Flag_e_FALSE == g_Sensor_Tuning_Control.e_Flag_ConfigurationDone)
        {
            OstTraceInt0(TRACE_DEBUG, "<Sensor_Tuning> sensor_tuning_Readsensor_tuningConfig");
            sensor_tuning_Read_sensor_tuningConfig();
        }

        lla_abstraction_ResetConfigContents();
        OstTraceInt0(TRACE_FLOW, "lla_abstractionInit()-> Congigure preboot CDCC");
        lla_abstraction_Configure_camera_data();

        ReturnVal = cam_drv_config_set(&g_cam_drv_config, &g_config_feedback);
        if (CAM_DRV_OK != ReturnVal)
        {
            OstTraceInt0(TRACE_ERROR, "<Sensor_Tuning> failed to write sensor tuning data");
            ASSERT_XP70();
        }
        //[NON_ISL_SUPPORT]
        //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
        g_config_feedback.output_res.height -= non_smia_correction_factor;
        g_config_feedback.woi_res.height -= non_smia_correction_factor;
    }

      /*Check the value of this PE here for exporting NVM data. This is just a workaround to inform LLCD that whether "RAW" or "PARSED" NVM data
          is expected from it. write value OF this PE to some global varibale through this function call. Which will be used inside LLCD to take decision while returning buffer size and
          filling NVM data. Default value of this PE g_Sensor_Tuning_Control.e_TypeNVMExport= TypeNVMExport_e_Parsed..
          NOTE: Once asked for a particular type of NVM data (e.g. RAW), we will be exporting that (RAW) data on all subsequent exports. */

     if (TypeNVMExport_e_Raw== g_Sensor_Tuning_Control.e_TypeNVMExport)
     {
        OstTraceInt0(TRACE_ERROR, "<NVM> do cam_drv_cache_nvm()");
        cam_drv_cache_nvm();
     }

    ReturnVal = cam_drv_get_camera_details(&g_camera_details);
    if (CAM_DRV_OK != ReturnVal)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstractionInit() - cam_drv_get_camera_details() returned failure!!");
        ASSERT_XP70();
    }


    lla_abstraction_update_analog_limits();
    g_ReadLLAConfig_Status.u32_sensor_Output_Mode_data_size = g_camera_details.p_sensor_details->
            output_mode_capability.number_of_modes *
        sizeof(Sensor_Output_Mode_ts);

    if (NULL != g_camera_details.p_nvm_details)
    {
        if (TypeNVMExport_e_Raw == g_Sensor_Tuning_Control.e_TypeNVMExport)
        {
            g_Sensor_Tuning_Status.u16_NVM_Data_Size = g_camera_details.p_nvm_details->size;
        }
        else
        {
            g_Sensor_Tuning_Status.u16_NVM_Data_Size = g_camera_details.p_nvm_details->parsed_nvm_size;
        }
    }


    LLA_Setup_FrameFormat(&g_camera_details);
    g_ReadLLAConfig_Status.u16_number_of_modes = g_camera_details.p_sensor_details->output_mode_capability.number_of_modes;

    if (LLA_Abstraction_Is_Aperture_Supported())
    {
        g_ApertureConfig_Status.u16_number_of_apertures = g_camera_details.p_aperture_details->number_of_supported_apertures;
        g_FrameParamStatus.u32_applied_f_number_x100 = g_camera_details.p_aperture_details->p_f_number_x_100[0];
    }
    else
    {
        g_ApertureConfig_Status.u16_number_of_apertures = 0;
        g_FrameParamStatus.u32_applied_f_number_x100 = DEFAULT_APERTURE_VALUE_X100;
    }


    if (NULL != g_camera_details.p_nd_details)
    {
        g_FrameParamStatus.u32_NDFilter_Transparency_x100 = g_camera_details.p_nd_details->nd_filter_transparency_x_100;

        ReturnVal = cam_drv_nd_control(CAM_DRV_ND_STATE_GET, ( bool8 * ) &state);
        if (CAM_DRV_OK == ReturnVal)
        {
            g_FrameParamStatus.u32_Flag_NDFilter = state;
        }
        else
        {
            g_FrameParamStatus.u32_Flag_NDFilter = 0;
        }
    }
    else        //ND Filter not supported
    {
        g_FrameParamStatus.u32_NDFilter_Transparency_x100 = 0;
        g_FrameParamStatus.u32_Flag_NDFilter = 0;
    }


    LLA_Update_FlashSupportDetails();
}


void
lla_abstractionSensorInit(void)
{
    CAM_DRV_RETVAL_E    ReturnVal;
    Flag_te             state;

    ReturnVal = camera_on();

    lla_abstraction_ResetConfigContents();
    OstTraceInt0(TRACE_FLOW, "lla_abstractionSensorInit()-> Congigure preboot CDCC in wakeup");
    lla_abstraction_Configure_camera_data();
    ReturnVal = cam_drv_config_set(&g_cam_drv_config, &g_config_feedback);
    if (CAM_DRV_OK != ReturnVal)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstractionSensorInit() - cam_drv_config_set() returned failure while applying CDCC!!");
        ASSERT_XP70();
    }
    //[NON_ISL_SUPPORT]
    //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
    g_config_feedback.output_res.height -= non_smia_correction_factor;
    g_config_feedback.woi_res.height -= non_smia_correction_factor;

    lla_abstraction_ResetConfigContents();

    if (CAM_DRV_OK == ReturnVal)
    {
        ReturnVal = cam_drv_get_camera_details(&g_camera_details);
        if (CAM_DRV_OK == ReturnVal)
        {
            g_ReadLLAConfig_Status.u16_number_of_modes = g_camera_details.p_sensor_details->output_mode_capability.number_of_modes;
            OstTraceInt1(TRACE_FLOW, "No of modes after wakeup: %d ", g_ReadLLAConfig_Status.u16_number_of_modes);

            if (LLA_Abstraction_Is_Aperture_Supported())
            {
                g_ApertureConfig_Status.u16_number_of_apertures = g_camera_details.p_aperture_details->number_of_supported_apertures;
                g_FrameParamStatus.u32_applied_f_number_x100 = g_camera_details.p_aperture_details->p_f_number_x_100[0];
            }
            else
            {
                g_ApertureConfig_Status.u16_number_of_apertures = 0;
                g_FrameParamStatus.u32_applied_f_number_x100 = DEFAULT_APERTURE_VALUE_X100;
            }

            if (NULL != g_camera_details.p_nd_details)
            {
                g_FrameParamStatus.u32_NDFilter_Transparency_x100 = g_camera_details.p_nd_details->nd_filter_transparency_x_100;
                ReturnVal = cam_drv_nd_control(CAM_DRV_ND_STATE_GET, ( bool8 * ) &state);
                if (CAM_DRV_OK == ReturnVal)
                {
                    g_FrameParamStatus.u32_Flag_NDFilter = state;
                }
                else
                {
                    g_FrameParamStatus.u32_Flag_NDFilter = 0;
                }
            }
            else    //ND Filter not supported
            {
                g_FrameParamStatus.u32_NDFilter_Transparency_x100 = 0;
                g_FrameParamStatus.u32_Flag_NDFilter = 0;
            }


            LLA_Setup_FrameFormat(&g_camera_details);
            LLA_Update_FlashSupportDetails();
        }
        else
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstractionSensorInit() - cam_drv_get_camera_details() returned failure!!");
            ASSERT_XP70();
        }
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstractionSensorInit() - camera_on() returned failure!!");
        ASSERT_XP70();
    }
}


//lla_abstraction_ConfigureFlash

//TODO: [Flash]MULTIPLE STROBE PER FRAME SUPPORT NOT ADDED YET.
void
lla_abstraction_ConfigureFlash(void)
{
    //Initialise Flash Status Page
    g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_NONE;
    if (Flag_e_FALSE == Flash_IsFlashSupported())
    {
        g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_CONFIGURATION;
        return;
    }


    g_FrameParamStatus.u32_flash_fired = 0;
    g_FlashStatus.u8_FlashFiredFrameCount = 0;
    g_FlashStatus.u8_FlashSyncErrorCount = 0;

    //Update g_config_flash_strobes and g_config_each_strobe
    g_config_each_strobe.strobe_start_point = g_FlashControl.e_StrobeStartPoint_Frame;
    g_config_each_strobe.delay_from_start_point = g_FlashControl.s32_DelayFromStartPoint_lines;

    g_config_each_strobe.strobe_length = clip(
        g_FlashControl.u32_StrobeLength_us,
        g_FlashStatus.u32_MinStrobeLength_us,
        g_FlashStatus.u32_MaxStrobeLength_us);
    g_config_flash_strobes.strobes_per_frame = clip(
        g_FlashControl.u8_StrobesPerFrame,
        1,
        g_FlashStatus.u8_MaxStrobesPerFrame);
    g_config_flash_strobes.frame_count = clip(g_FlashControl.u8_FrameCount, 1, 0xFF);

    g_config_flash_strobes.global_reset_frame_only = g_FlashControl.e_Flag_GlobalResetFrameOnly;
    g_config_flash_strobes.do_strobe_modulation = g_FlashControl.e_Flag_DoStrobeModulation;

    g_cam_drv_config.config_contents.config_flash_strobe = 1;

    return;
}


void
lla_abstraction_ConvertFeedbackToTestPatternStatus(void)
{
    g_TestPattern_Status.u16_test_data_red = 0;
    g_TestPattern_Status.u16_test_data_greenR = 0;
    g_TestPattern_Status.u16_test_data_blue = 0;
    g_TestPattern_Status.u16_test_data_greenB = 0;

    if (TestPattern_e_NotSupported != g_TestPattern_Status.e_TestPattern)
    {
        g_TestPattern_Status.e_TestPattern = g_config_feedback.test_pic_mode;
    }


    if (CAM_DRV_SENS_TEST_PICTURE_MODE_SOLID_COLOUR == g_config_feedback.test_pic_mode)
    {
        g_TestPattern_Status.u16_test_data_red = g_config_feedback.test_picture_colour.red;
        g_TestPattern_Status.u16_test_data_greenR = g_config_feedback.test_picture_colour.greenR;
        g_TestPattern_Status.u16_test_data_blue = g_config_feedback.test_picture_colour.blue;
        g_TestPattern_Status.u16_test_data_greenB = g_config_feedback.test_picture_colour.greenB;
    }
}


void
lla_abstraction_Configure_test_picture(void)
{
    g_cam_drv_config.config_contents.config_test_picture = 1;

    switch (g_TestPattern_Ctrl.e_TestPattern)
    {
        case TestPattern_e_Normal:
            g_cam_drv_config.p_config_test_picture->test_picture_mode = CAM_DRV_SENS_TEST_PICTURE_MODE_NONE;
            break;

        case TestPattern_e_SolidColour:
            if (g_camera_details.p_sensor_details->test_picture_capability.solid_colour_mode)
            {
                g_cam_drv_config.p_config_test_picture->test_picture_mode = CAM_DRV_SENS_TEST_PICTURE_MODE_SOLID_COLOUR;
                g_cam_drv_config.p_config_test_picture->test_picture_colour.red = g_TestPattern_Ctrl.u16_test_data_red;
                g_cam_drv_config.p_config_test_picture->test_picture_colour.greenR = g_TestPattern_Ctrl.u16_test_data_greenR;
                g_cam_drv_config.p_config_test_picture->test_picture_colour.blue = g_TestPattern_Ctrl.u16_test_data_blue;
                g_cam_drv_config.p_config_test_picture->test_picture_colour.greenB = g_TestPattern_Ctrl.u16_test_data_greenB;
            }


            break;

        case TestPattern_e_SolidColourBars:
            if (g_camera_details.p_sensor_details->test_picture_capability.colour_bar_solid_mode)
            {
                g_cam_drv_config.p_config_test_picture->test_picture_mode = CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_SOLID;
            }


            break;

        case TestPattern_e_SolidColourBarsFade:
            if (g_camera_details.p_sensor_details->test_picture_capability.colour_bar_fade_mode)
            {
                g_cam_drv_config.p_config_test_picture->test_picture_mode = CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_FADE;
            }


            break;

        case TestPattern_e_PN9:
            if (g_camera_details.p_sensor_details->test_picture_capability.pseudo_random_mode)
            {
                g_cam_drv_config.p_config_test_picture->test_picture_mode = CAM_DRV_SENS_TEST_PICTURE_MODE_PSEUDO_RANDOM;
            }


            break;

        default:
            g_cam_drv_config.config_contents.config_test_picture = 0;
            g_TestPattern_Status.e_TestPattern = TestPattern_e_NotSupported;
            break;
    }
}


void
lla_abstraction_Configure_camera_data(void)
{
    if
    (
        (0 < g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount)
    &&  (NULL != g_Sensor_Tuning_Control.u32_SubBlock_Data_Address)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_capture)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_idle)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_still_vf)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_video_vf)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_video_rec)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_capture)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_override)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_vf_night_mode)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_capture_night_mode)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_prod_test_mode)
    &&  (NULL == g_cam_drv_config.p_config_camera_data->p_config_seq_shot_mode)
    )
    {
            //copy sensor_tuning buffer's address
            g_cam_drv_config.p_config_camera_data->p_config_data_all = ( uint32_t * ) g_Sensor_Tuning_Control.u32_SubBlock_Data_Address;
            g_cam_drv_config.config_contents.config_camera_data = 1;
            OstTraceInt2(TRACE_DEBUG, "<Sensor_Tuning> Tuning data write successfull. Total sub block count =  %d, Source memory address = %x", g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount, g_Sensor_Tuning_Control.u32_SubBlock_Data_Address);
    }
    else
    {
        g_cam_drv_config.config_contents.config_camera_data = 0;
    }
}



CAM_DRV_SENS_MODE_E lla_abstraction_FindSensorModeFromUserRestrictionBitmask(uint8_t e_LLD_USAGE_MODE_usagemode)
{
    CAM_DRV_SENS_MODE_E sensor_mode;

    switch(e_LLD_USAGE_MODE_usagemode)
    {
        case LLD_USAGE_MODE_VF:
            sensor_mode = CAM_DRV_SENS_MODE_STILL;
            break;

        case LLD_USAGE_MODE_AF:
            sensor_mode = CAM_DRV_SENS_MODE_AF;
            break;

        case LLD_USAGE_MODE_STILL_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_STILL_CAPTURE;
            break;

        case LLD_USAGE_MODE_NIGHT_STILL_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_STILL_CAPTURE;
            break;

        case LLD_USAGE_MODE_STILL_SEQ_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_STILL_SEQ;
            break;

        case LLD_USAGE_MODE_VIDEO_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_VIDEO;
            break;

        case LLD_USAGE_MODE_NIGHT_VIDEO_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_VIDEO;
            break;

        case LLD_USAGE_MODE_HQ_VIDEO_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_HQ_VIDEO;
            break;

        case LLD_USAGE_MODE_HS_VIDEO_CAPTURE:
            sensor_mode = CAM_DRV_SENS_MODE_HS_VIDEO;
            break;

        case CAM_DRV_USAGE_MODE_UNDEFINED:
            sensor_mode = CAM_DRV_SENS_MODE_STILL;
            break;

        default:
            sensor_mode = CAM_DRV_SENS_MODE_STILL;
            break;
    }

       OstTraceInt2(TRACE_FLOW, "UserMode: %d, SensorMode: %x",e_LLD_USAGE_MODE_usagemode,sensor_mode);

    return(sensor_mode);
}


/// Configure sensor for Flash and sensor_tuning
/// Rest start_VF() in Trial mode
void
lla_abstraction_ConfigureSensor(void)
{
    CAM_DRV_RETVAL_E    ReturnVal;
    OstTraceInt0(TRACE_DEBUG, "<Sensor_Tuning> >> lla_abstraction_ConfigureSensor");


    lla_abstraction_ResetConfigContents();

    g_cam_drv_config.trial_only = Flag_e_FALSE;

    // Added to support  STOP_ISP--> RUN  sequence
    // In case of normal start, there will be multiple
    // sensor configurations, once here and another in startsensor
    LLA_Abstraction_ApplyAecAndFreameRate(&g_Exposure_DriverControls);

    lla_abstraction_Configure_test_picture();

    if(Flag_e_FALSE == g_Sensor_Tuning_Control.e_Flag_ReadConfigBeforeBoot)
    {
       OstTraceInt2(TRACE_DEBUG, "<Sensor_Tuning> Tuning data write successfull. Total sub block count =  %d, Source memory address = %x", g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount, g_Sensor_Tuning_Control.u32_SubBlock_Data_Address);
       lla_abstraction_Configure_camera_data();
    }

    if (Flash_IsFlashToBeTriggered())
    {
        Flash_SetFlashStatus(UpdateStatus_e_Updating);
        lla_abstraction_ConfigureFlash();

        //check for flash config error
        if (FlashError_e_FLASH_ERROR_NONE != g_FlashStatus.e_FlashError_Info)
        {
            Flash_HandleFlashErrors();
        }
    }
    else if (Flash_IsFlashModeActive())  //Flash Settings for first frame
    {
        //By default, trigger flash for first frame (if flashmode TRUE).
        lla_abstraction_ConfigureFlash();
    }


    ReturnVal = cam_drv_config_set(&g_cam_drv_config, &g_config_feedback);
    if (CAM_DRV_OK == ReturnVal)
    {
        //[NON_ISL_SUPPORT]
        //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
        g_config_feedback.output_res.height -= non_smia_correction_factor;
        g_config_feedback.woi_res.height -= non_smia_correction_factor;

        // Update other PEs related to exposure & frame rate
        g_Exposure_ParametersApplied.u16_AnalogGain_x256 = (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
        g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us = g_config_feedback.exposure_time_us;
        g_FrameDimensionStatus.f_VTFrameLength_us = (1 / (((float_t) g_config_feedback.frame_rate_x100) / 100)) * 1000000;
        g_FrameDimensionStatus.f_CurrentFrameRate = ((float_t) g_config_feedback.frame_rate_x100) / 100;
        g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines = (uint16) (g_FrameDimensionStatus.f_VTFrameLength_us / g_FrameDimensionStatus.f_VTLineLength_us);
        g_CurrentFrameDimension.u16_VTFrameLengthLines = g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines;
        g_VariableFrameRateStatus.f_CurrentFrameRate_Hz = g_FrameDimensionStatus.f_CurrentFrameRate;

        // Update exposure limits
        lla_abstraction_update_exposure_limits(&g_config_feedback);

        // Update test pattern status PEs
        lla_abstraction_ConvertFeedbackToTestPatternStatus();

        lla_abstraction_ResetConfigContents();
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_ConfigureSensor() - cam_drv_config_set returned failure!!");
        ASSERT_XP70();
    }


    // Following fields are common to VF/capture and therefore set in trial mode
    g_vf_config.trial_only = Flag_e_TRUE;

    g_vf_config.output_res.width = g_Zoom_Status_LLA.u16_Ex_output_resX;

    g_vf_config.output_res.height = g_Zoom_Status_LLA.u16_Ex_output_resY + non_smia_correction_factor;

    g_vf_config.woi_res.height = g_Zoom_Status_LLA.u16_Ex_woi_resY + non_smia_correction_factor;

    g_vf_config.woi_res.width = g_Zoom_Status_LLA.u16_Ex_woi_resX;

    g_vf_config.frame_rate_x100 = (uint16) (FrameRate_GetMaximumFrameRate() * 100);

    g_vf_config.min_line_length_pck = g_RequestedFrameDimension.u16_VTLineLengthPck;

    g_vf_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());

    g_vf_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);

    g_vf_config.ac_frequency = 0;

    lla_abstraction_set_orientation(&g_vf_config.orientation);

    // Apply actual settings
    ReturnVal = cam_drv_start_vf(&g_vf_config, &g_config_feedback);

    if (CAM_DRV_OK == ReturnVal)
    {
        //[NON_ISL_SUPPORT]
        //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
        g_config_feedback.output_res.height -= non_smia_correction_factor;
        g_config_feedback.woi_res.height -= non_smia_correction_factor;
        lla_abstraction_ConvertFeedbackToPageElemets(&g_config_feedback, g_vf_config.trial_only);
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_ConfigureSensor() - cam_drv_start_vf(TRIAL) returned failure!!");
        ASSERT_XP70();
    }
}


void
lla_abstraction_ResetConfigContents(void)
{
    g_cam_drv_config.config_contents.config_woi = 0;
    g_cam_drv_config.config_contents.config_gain_and_exp_time = 0;
    g_cam_drv_config.config_contents.config_frame_rate = 0;
    g_cam_drv_config.config_contents.config_test_picture = 0;
    g_cam_drv_config.config_contents.config_camera_data = 0;
    g_cam_drv_config.config_contents.config_awb_data = 0;
    g_cam_drv_config.config_contents.config_zoom_roi = 0;
    g_cam_drv_config.config_contents.config_flash_strobe = 0;
    g_cam_drv_config.config_contents.config_sensor_mode = 0;
    g_cam_drv_config.trial_only = Flag_e_FALSE;
}


void
lla_abstraction_ResetInterpretSensorSettingsContents(void)
{
    g_SensorSettings.exposure_time_us = 0;
    g_SensorSettings.analog_gain_x1000 = 0;
    g_SensorSettings.digital_gain_x1000 = 0;
    g_SensorSettings.ms_used = Flag_e_FALSE;
    g_SensorSettings.flash_fired = Flag_e_FALSE;
}


void lla_abstraction_set_orientation(CAM_DRV_SENS_ORIENTATION_T  *ptr_orientation)
{
    //  Set mirror capability
    if
    (
        (1 == g_camera_details.p_sensor_details->orientation_capability.mirror)
    &&  (Flag_e_TRUE == g_SystemSetup.e_Flag_mirror)
    )
    {
        ptr_orientation->mirror = g_SystemConfig_Status.e_Flag_mirror = Flag_e_TRUE;
    }
    else
    {
        ptr_orientation->mirror = g_SystemConfig_Status.e_Flag_mirror = Flag_e_FALSE;
    }


    //  Set flip capability
    if
    (
        (1 == g_camera_details.p_sensor_details->orientation_capability.flip)
    &&  (Flag_e_TRUE == g_SystemSetup.e_Flag_flip)
    )
    {
        ptr_orientation->flip = g_SystemConfig_Status.e_Flag_flip = Flag_e_TRUE;
    }
    else
    {
        ptr_orientation->flip = g_SystemConfig_Status.e_Flag_flip = Flag_e_FALSE;
    }
}


void
lla_abstraction_ReprogramSensorForZoom(
uint8_t e_Flag_trial_only)
{
    CAM_DRV_RETVAL_E    ReturnVal;
    lla_abstraction_ResetConfigContents();
    g_vf_config.trial_only = e_Flag_trial_only;

    g_vf_config.output_res.width = g_Zoom_Status_LLA.u16_Ex_output_resX;

    g_vf_config.output_res.height = g_Zoom_Status_LLA.u16_Ex_output_resY + non_smia_correction_factor;

    g_vf_config.woi_res.height = g_Zoom_Status_LLA.u16_Ex_woi_resY + non_smia_correction_factor;
    g_vf_config.woi_res.width = g_Zoom_Status_LLA.u16_Ex_woi_resX;

    if(Flag_e_TRUE == e_Flag_trial_only)
    {
        g_vf_config.frame_rate_x100 = (uint16) (FrameRate_GetMaximumFrameRate() * 100);
    }
    else
    {
        g_vf_config.frame_rate_x100 = (uint16) (FrameRate_GetAskedFrameRate() * 100);
    }

    g_vf_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());

    g_vf_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);
    g_vf_config.ac_frequency = 0;

    lla_abstraction_set_orientation(&g_vf_config.orientation);
    ReturnVal = cam_drv_start_vf(&g_vf_config, &g_config_feedback);

    if (CAM_DRV_OK == ReturnVal)
    {
        //[NON_ISL_SUPPORT]
        //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
        g_config_feedback.output_res.height -= non_smia_correction_factor;
        g_config_feedback.woi_res.height -= non_smia_correction_factor;
        lla_abstraction_ConvertFeedbackToPageElemets(&g_config_feedback, e_Flag_trial_only);
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_ReprogramSensorForZoom() - cam_drv_start_vf() returned failure!!");
        ASSERT_XP70();
    }
}


void
lla_abstraction_StartVFInTrial(void)
{
    CAM_DRV_CONFIG_VF_T         vf_config_trial = { { 0 } };
    CAM_DRV_CONFIG_FEEDBACK_T   vf_config_trial_feedback = { { 0 } };
    CAM_DRV_RETVAL_E            start_vf_return_val = CAM_DRV_ERROR_MISC; //initialize it to some random error
    float_t                     f_VTLineLength_us = 0,
                                f_VTFrameLength_us = 0;

    //Take SMS_Control Inputs
    vf_config_trial.output_res.width = g_SMS_Control.u16_X_size;
    vf_config_trial.output_res.height = g_SMS_Control.u16_Y_size + non_smia_correction_factor;

    vf_config_trial.woi_res.width = g_SMS_Control.u16_WOI_X_size;
    vf_config_trial.woi_res.height = g_SMS_Control.u16_WOI_Y_size + non_smia_correction_factor;
    vf_config_trial.frame_rate_x100 = g_SMS_Control.u32_FrameRate_x100;

    vf_config_trial.format = lla_HostRawFormatToLLARawFormat(SMS_GetCsiRawFormat());

    //Take remaining inputs from existing global System State
    //[TBC] Partha Not sure how many of these are used currently by LLCD 4.0 in trial mode
    //Currently assigning some dummy values.
    vf_config_trial.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);
    vf_config_trial.orientation.mirror = 0;
    vf_config_trial.orientation.flip = 0;
    vf_config_trial.min_line_length_pck = 0;
    vf_config_trial.exposure_time_us = 100;
    vf_config_trial.analog_gain_x1000 = 1000;
    vf_config_trial.digital_gain_x1000 = 1000;
    vf_config_trial.ac_frequency = 0;

    //set Trial mode to TRUE
    vf_config_trial.trial_only = Flag_e_TRUE;
/*
    OstTraceInt0(TRACE_DEBUG, "[TRIAL MODE] Requested Params:");
//    OstTraceInt6("u16_X_size: %d u16_X_size: %d u16_WOI_X_size: %d u16_WOI_Y_size: %d u32_FrameRate_x100: %d CsiRawFormat: %d",
                  vf_config_trial.output_res.width, vf_config_trial.output_res.height,
                  vf_config_trial.woi_res.width, vf_config_trial.woi_res.height,
                  vf_config_trial.frame_rate_x100, vf_config_trial.format);
*/
    //call Trial Mode LLCD API
    start_vf_return_val = cam_drv_start_vf(&vf_config_trial, &vf_config_trial_feedback);

    if (CAM_DRV_OK == start_vf_return_val)
    {
        f_VTLineLength_us = vf_config_trial_feedback.line_readout_time_ns / 1000;
        f_VTFrameLength_us = (1 / (((float_t) vf_config_trial_feedback.frame_rate_x100) / 100)) * 1000000;

        g_SMS_Status.u32_LineLength_pck = vf_config_trial_feedback.line_length_pck;
        g_SMS_Status.u32_FrameLength_lines = (uint32_t) (f_VTFrameLength_us / f_VTLineLength_us);
        g_SMS_Status.u32_Min_ExposureTime_us = vf_config_trial_feedback.current_et_capability.min_exp_time_us;
        g_SMS_Status.u32_Max_ExposureTime_us = vf_config_trial_feedback.current_et_capability.max_exp_time_us;
        g_SMS_Status.u32_ExposureQuantizationStep_us = vf_config_trial_feedback.current_et_capability.exp_time_step;
        g_SMS_Status.u32_ActiveData_ReadoutTime_us = (uint32_t) (vf_config_trial_feedback.output_res.height * f_VTLineLength_us);

        //set status coin
        g_SMS_Status.e_Coin_Status = g_SMS_Control.e_Coin_Ctrl;

        //send notification
        EventManager_SMS_Notification_Notify();
/*
        OstTraceInt0(TRACE_DEBUG, "[TRIAL MODE] Feedback:");
        //OstTraceInt6("LineLength_pck: %d FrameLength_lines: %d u3Min_ExposureTime_us: %d Max_ExposureTime_us: %d ExposureQuantizationStep_us: %d ActiveData_ReadoutTime_us: %d",
                      g_SMS_Status.u32_LineLength_pck, g_SMS_Status.u32_FrameLength_lines,
                      g_SMS_Status.u32_Min_ExposureTime_us, g_SMS_Status.u32_Max_ExposureTime_us,
                      g_SMS_Status.u32_ExposureQuantizationStep_us, g_SMS_Status.u32_ActiveData_ReadoutTime_us);
*/
    }
    else
    {
           OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_StartVFInTrial() - cam_drv_start_vf() returned failure in [TRIAL_MODE]!!");
           ASSERT_XP70();
    }


    return;
}


void
lla_abstraction_StartSensor(void)
{
    CAM_DRV_RETVAL_E            ReturnVal;

    //Get the camera driver state
    LLA_Abstraction_CamDrvState_te camDrvState = GET_LLA_CAM_DRV_STATE();

    if (g_RunMode_Control.e_StreamMode == StreamMode_e_VF)                  // VF
    {
        g_vf_config.output_res.width = g_Zoom_Status_LLA.u16_output_resX;
        g_vf_config.output_res.height = g_Zoom_Status_LLA.u16_output_resY + non_smia_correction_factor;

        g_vf_config.woi_res.width = g_Zoom_Status_LLA.u16_woi_resX;
        g_vf_config.woi_res.height = g_Zoom_Status_LLA.u16_woi_resY + non_smia_correction_factor;

        g_vf_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());

        g_vf_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);

        g_vf_config.min_line_length_pck = g_RequestedFrameDimension.u16_VTLineLengthPck;
        g_vf_config.analog_gain_x1000 = (uint16) (((uint32_t) g_Exposure_DriverControls.u16_TargetAnalogGain_x256 * 1000) / 256);

        g_vf_config.digital_gain_x1000 = U16_DIGITAL_GAIN_x1000;

        g_vf_config.exposure_time_us = g_Exposure_DriverControls.u32_TargetExposureTime_us;

        g_vf_config.frame_rate_x100 = (uint16) (FrameRate_GetAskedFrameRate() * 100);

        // <AG> Antiflicker is part of ARM s/w. LLCD should not be used for it.
        g_vf_config.ac_frequency = 0;

        lla_abstraction_set_orientation(&g_vf_config.orientation);

        g_vf_config.trial_only = Flag_e_FALSE;

        ReturnVal = cam_drv_start_vf(&g_vf_config, &g_config_feedback);

        if (CAM_DRV_OK == ReturnVal)
        {
            //[NON_ISL_SUPPORT]
            //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
            g_config_feedback.output_res.height -= non_smia_correction_factor;
            g_config_feedback.woi_res.height -= non_smia_correction_factor;
            if (e_LLA_Abstraction_CamDrvState_Idle != camDrvState)
            {
                LLA_Abstraction_SetStreamStatus(Flag_e_TRUE);
            }


            g_RunMode_Status.e_StreamMode = StreamMode_e_VF;

            //Set camera driver to in VF state
            SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_ViewFinder);
        }
        else
        {
            SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_Error);     //error case
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_StartSensor() - cam_drv_start_vf() returned failure!!");
            ASSERT_XP70();
        }
    }
    else
    {
        ///  Still capture
        ///  Flash condition not supported
        ///  For preflash metering several VF frames need to be streamed
        ///  For normal mechanical shutter first frame is always ES frame; Need to disable pipe output;
        g_capture_config.output_res.width = g_Zoom_Status_LLA.u16_output_resX;
        g_capture_config.output_res.height = g_Zoom_Status_LLA.u16_output_resY+ non_smia_correction_factor;

        g_capture_config.woi_res.width = g_Zoom_Status_LLA.u16_woi_resX;
        g_capture_config.woi_res.height = g_Zoom_Status_LLA.u16_woi_resY + non_smia_correction_factor;

        g_capture_config.analog_gain_x1000 = (uint16_t) (((uint32_t) g_Exposure_DriverControls.u16_TargetAnalogGain_x256 * 1000) / 256);
        g_capture_config.digital_gain_x1000 = U16_DIGITAL_GAIN_x1000;
        g_capture_config.exposure_time_us = g_Exposure_DriverControls.u32_TargetExposureTime_us;

        g_capture_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());

        /// <To Do: BG> need to take care for other modes e.g flash
        g_capture_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);

        lla_abstraction_set_orientation(&g_capture_config.orientation);

        // AWB data
        //g_capture_config.awb_data.green_gain_x1000 = 1000;
        //g_capture_config.awb_data.red_gain_x1000 = 1000;
        //g_capture_config.awb_data.blue_gain_x1000 = 1000;
        //g_capture_config.awb_data.color_temperature = ;
        // Mechanical shutter control
        if
        (
            (g_RunMode_Control.e_Flag_MechanicalShutterUsed == Flag_e_TRUE)
        &&  (1 == g_camera_details.p_shutter_details->ms_single_capture_supported)
        )
        {
            g_capture_config.use_mechanical_shutter = 1;
            g_capture_config.mechanical_shutter_ctrl.capture_with_ms_closed = 0;
            g_capture_config.mechanical_shutter_ctrl.capture_continuous = 1;
        }
        else
        {
            g_capture_config.use_mechanical_shutter = 0;
            g_capture_config.mechanical_shutter_ctrl.capture_with_ms_closed = 0;
            g_capture_config.mechanical_shutter_ctrl.capture_continuous = 0;
        }


        cam_drv_capture(&g_capture_config, &g_config_feedback);
        //[NON_ISL_SUPPORT]
        //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
        g_config_feedback.output_res.height -= non_smia_correction_factor;
        g_config_feedback.woi_res.height -= non_smia_correction_factor;
        if (e_LLA_Abstraction_CamDrvState_Idle != camDrvState)
        {
            LLA_Abstraction_SetStreamStatus(Flag_e_TRUE);
        }


        g_RunMode_Status.e_StreamMode = StreamMode_e_Capture;

        //Set camera driver to in VF state
        SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_Capture);
    }

    lla_abstraction_ConvertFeedbackToPageElemets(&g_config_feedback, Flag_e_FALSE); //[PM]trial mode will always be false for this function
    lla_abstraction_ConvertFeedbackToFrameParamStatus(&g_config_feedback);
}


void
lla_abstractionSensorOff(void)
{
    cam_drv_off();

    g_camera_details.p_sensor_details->output_mode_capability.p_modes = ( void * ) NULL;
    g_camera_details.p_sensor_details = ( void * ) NULL;


    //Set camera driver to in on state
    SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_Off);
}


void
lla_abstractionSetMaximumUsableSensorFOV(void)
{
    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.width;
    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.height;
}


void
lla_ConfigureWOIandFrameRate(
uint16  u16_Pipe0OpSizeX,
uint16  u16_Pipe0OpSizeY,
uint16  u16_DesiredFOVX,
uint16  u16_DesiredFOVY,
float   f_FrameRate_Hz)
{
    g_cam_drv_config.config_contents.config_woi = 1;

    g_cam_drv_config.p_config_woi->output_res.width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.width;
    g_cam_drv_config.p_config_woi->output_res.height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.height - non_smia_correction_factor;

    g_cam_drv_config.p_config_woi->woi_res.width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.width;
    g_cam_drv_config.p_config_woi->woi_res.height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[0].woi_res.height - non_smia_correction_factor;

    g_cam_drv_config.config_contents.config_frame_rate = 1;
    g_cam_drv_config.p_config_frame_rate->frame_rate_x100 = (uint16) (f_FrameRate_Hz * 100);
}


uint8_t
lla_abstractionFrameDimension_RequestFrameConfig(
uint16_t    u16_DesiredFOVX,
uint16_t    u16_DesiredFOVY,
uint16_t    u16_RequiredLineLength,
uint16_t    u16_MaxLineLength,
int16_t     s16_CenterXOffset,
int16_t     s16_CenterYOffset,
uint8_t     e_Flag_ScaleLineLengthForDerating,
uint8_t     e_Flag_EnableModeSelection)
{
    float_t     f_Value;
    float_t     f_MaximumSystemFrameRate;

    int16_t     i16_iter;
    uint16_t    u16_MaximumUsableSensorFOVX;
    uint16_t    u16_MaximumUsableSensorFOVY;
    uint16_t    u16_number_of_modes;
    uint16_t    u16_Selected = 0xFFFF;
    uint16_t    u16_woi_resX;
    uint16_t    u16_woi_resY;

    uint16_t    u16_output_resX;
    uint16_t    u16_output_resY;

    int16_t     s16_XOffsetSign;
    int16_t     s16_YOffsetSign;
    uint16_t    u16_CenterOffsetX;
    uint16_t    u16_CenterOffsetY;

    uint16_t    u16_MaxLineLengthForMaxDZ;
    uint8_t     e_Flag_CurrentFDMSufficient;
    uint8_t     e_FrameDimensionRequestStatus;

    uint16_t    u16_SelectedModeLineLen = 0;
    uint8_t     u8_CurrModePscVal = 0;
    uint8_t     u8_SelectedModePscVal = 255;

    f_MaximumSystemFrameRate = FrameRate_GetMaximumFrameRate();
    e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Denied;
    u16_number_of_modes = g_camera_details.p_sensor_details->output_mode_capability.number_of_modes;
    OstTraceInt4(TRACE_FLOW, "Desired FOV X: %d, Y: %d, offset: X: %d, Y: %d",u16_DesiredFOVX, u16_DesiredFOVY,s16_CenterXOffset,s16_CenterYOffset);

    // The aim is to find:
    // The minimum sensor sub sampling that will satisfy
    // the f_MaximumSystemFrameRate requirement @ FOV f_DesiredFOVX x f_DesiredFOVY
    // and line length of MAX(sensor_min_line_length, f_RequiredLineLength)
    //
    // The following requests must be denied:
    // 1.> f_DesiredFOVX > g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX
    // 2.> f_DesiredFOVY > g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY
    // 3.> max_frame_rate @ f_DesiredFOVX x f_DesiredFOVY and f_RequiredLineLength < f_MaximumSystemFrameRate
    //
    // The following overheads must be added to the requested FOV
    // 1.> a margin to ensure that the border requirements of the scalar are met
    // 2.> the loss in FOV that will result in the ISP @ the maximum pre scale factor
    // point 2 will account for the extra offset that is applied to negate the shift in
    // center due to a change in sub sampling factor.
    // e.g. @ pre scale = 3, location 0,0 of the user view = 6,6 of the array
    //      @ pre scale = 2, location 0,0 of the user view = 4,4 of the array
    //      @ pre scale = 1, location 0,0 of the user view = 2,2 of the array
    // hence an additional offset is applied to compensate for this differential loss
    // when there is a change in the pre scale factor

    /* TODO: <LLA> <AG>: RU? What is need to FOV margin? */

    // some margin must be added to the field of view
    f_Value = (float_t) ((float_t) u16_DesiredFOVX * g_HostFrameConstraints.f_FOVMargin);
    u16_DesiredFOVX = FrameDimension_Ceiling(f_Value);

    f_Value = (float_t) ((float_t) u16_DesiredFOVY * g_HostFrameConstraints.f_FOVMargin);
    u16_DesiredFOVY = FrameDimension_Ceiling(f_Value);

    if( s16_CenterXOffset < 0)
        s16_XOffsetSign = -1;
    else
        s16_XOffsetSign = 1;

    if( s16_CenterYOffset < 0)
        s16_YOffsetSign = -1;
    else
        s16_YOffsetSign = 1;

    u16_CenterOffsetX = s16_CenterXOffset *s16_XOffsetSign;
    u16_CenterOffsetY = s16_CenterYOffset *s16_YOffsetSign;

    /// margin for center change should be taken care of to calculate FOV
    u16_DesiredFOVX += 2*u16_CenterOffsetX;
    u16_DesiredFOVY += 2*u16_CenterOffsetY;

    u16_DesiredFOVX += FrameDimension_GetISPColumnLoss() * g_HostFrameConstraints.u8_MaximumPreScale;
    u16_DesiredFOVY += FrameDimension_GetISPRowLoss() * g_HostFrameConstraints.u8_MaximumPreScale;

    // compute the maximum FOV possible for the requested center
    // FOV loss = abs(offset)*2
    u16_MaximumUsableSensorFOVX = g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX;
    u16_MaximumUsableSensorFOVY = g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY;

    u16_DesiredFOVX = FrameDimension_Min(u16_DesiredFOVX, u16_MaximumUsableSensorFOVX);
    u16_DesiredFOVY = FrameDimension_Min(u16_DesiredFOVY, u16_MaximumUsableSensorFOVY);
    OstTraceInt3(TRACE_FLOW, "Desired FOV X: %d, Y: %d, Requested line len: %d", u16_DesiredFOVX, u16_DesiredFOVY, u16_RequiredLineLength);

    // if FDM is in manual mode, then there is no concept of meeting frame rate requirements
    if (FrameDimensionProgMode_e_Auto == g_HostFrameConstraints.e_FrameDimensionProgMode)
    {
        if(e_Flag_EnableModeSelection)
        {
            CAM_DRV_RETVAL_E            e_Flag_ret_val;
            CAM_DRV_CONFIG_VF_T         vf_config;
            CAM_DRV_CONFIG_FEEDBACK_T   config_feedback;
            Flag_te                     mode_valid = Flag_e_TRUE;

            // Initialize the index variable for traversing sensor modes array
            i16_iter = 0;
            vf_config.trial_only = Flag_e_TRUE;
            vf_config.min_line_length_pck = 0;
            vf_config.ac_frequency = 0;
            vf_config.frame_rate_x100 = (uint16_t) (f_MaximumSystemFrameRate * 100);
            vf_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);
            vf_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());
            lla_abstraction_set_orientation(&vf_config.orientation);
            while (i16_iter < u16_number_of_modes)
            {
                u16_woi_resX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].woi_res.width;
                u16_woi_resY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].woi_res.height;
                u16_output_resX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].output_res.width;
                u16_output_resY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].output_res.height;

                if ((CAM_DRV_USAGE_MODE_UNDEFINED == g_RunMode_Control.e_LLD_USAGE_MODE_usagemode) ||
                ! ((g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].usage_restriction_bitmask) & (1 << g_RunMode_Control.e_LLD_USAGE_MODE_usagemode)))
                {
                    mode_valid = Flag_e_TRUE;
                    OstTraceInt1(TRACE_DEBUG, "Valid Mode : mode_no %d", i16_iter);
                }
                else
                {
                    mode_valid = Flag_e_FALSE;
                    OstTraceInt1(TRACE_DEBUG, "Invalid mode : mode_no %d", i16_iter);
                }

                if ((u16_woi_resX >= u16_DesiredFOVX) &&
                (u16_woi_resY >= u16_DesiredFOVY) &&
                (u16_output_resX < MAX_OUTPUT_SIZEX_PIPE0) &&
                (u16_output_resY < MAX_OUTPUT_SIZEY)&&
                (g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].data_format == vf_config.format)&&
                (Flag_e_TRUE == mode_valid)&&
                ((f_MaximumSystemFrameRate*100) <= g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].max_frame_rate_x100)
                )
                {
                    // Following fields are common to VF/capture and therefore set in trial mode
                    vf_config.output_res.width = u16_output_resX;
                    vf_config.output_res.height = u16_output_resY;
                    vf_config.woi_res.height = u16_woi_resY;
                    vf_config.woi_res.width = u16_woi_resX;

                    // Apply actual settings
                    e_Flag_ret_val = cam_drv_start_vf(&vf_config, &config_feedback);

                    if (CAM_DRV_OK != e_Flag_ret_val)
                    {
                        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstractionFrameDimension_RequestFrameConfig() - cam_drv_start_vf() returned failure!!");
                        ASSERT_XP70();
                    }


                    if((config_feedback.frame_rate_x100 >= (uint16_t) (f_MaximumSystemFrameRate * 100))
                    &&  (config_feedback.max_line_length_pck >= u16_RequiredLineLength)
                    )
                    {
                        u8_CurrModePscVal = g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].woi_res.width /g_camera_details.p_sensor_details->output_mode_capability.p_modes[i16_iter].output_res.width;
                        e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Accepted;

                        // Among all the allowed modes (depending upon value of usage restriction bitmask,) which are atleast able to fulfill minimum
                        // requiremen of WOI_X, WOI_Y, OP_X, OP_Y, FPS.
                        // a) Select the modes with minimum sensor pre-scaling.
                        // b) If there are 2 modes with the same sensor pre-scale, then mode max_line_length will be selected.
                        if (u8_CurrModePscVal < u8_SelectedModePscVal)
                        {
                            //Current mode fulfills all the basic requirements of fps, woi etc and has lower prescaling value then selected mode.
                            u16_Selected = i16_iter;
                            u8_SelectedModePscVal = u8_CurrModePscVal;
                            u16_SelectedModeLineLen = config_feedback.max_line_length_pck;
                        }
                        // Current mode and selected mode both has same prescaling values. Mode which offers more line length will be selected from here.
                        else if (u8_CurrModePscVal == u8_SelectedModePscVal)
                        {
                            // Current mode is offering more line length than selected mode. Switch to current mode.
                            if(u16_SelectedModeLineLen < config_feedback.max_line_length_pck)
                            {
                                // In the first run of this loop, default value of "u8_SelectedModePscVal" is 1 and that of "u16_SelectedModeLineLen" is 0,
                                // Hence following code will be executed.
                                u16_Selected = i16_iter;
                                u8_SelectedModePscVal = u8_CurrModePscVal;
                                u16_SelectedModeLineLen = config_feedback.max_line_length_pck;
                            }
                            else
                            {
                                //Selected mode can provide more line lenght then current mode. Dont change the value of selected mode.
                            }
                        }
                        else //i.e. case (u8_CurrModePscVal > u8_SelectedModePscVal)
                        {
                            // Current mode has more prescaling value then selected mode. Dont change the value of selected mode.
                        }
                    }
                }
                else
                {
                    OstTraceInt1(TRACE_FLOW, "Mode skipped: %d ", i16_iter);
                }

                //[CN] Under ZSL mode (i.e. Sensor change over is not allowed). Sensor mode selection should always select previous selected mode only (that would be FFOV)
                if((e_FrameDimensionRequestStatus == FrameDimensionRequestStatus_e_Accepted) &&
                (u16_Selected != 0xFFFF) &&
                (g_u16Lastmode_Selected != 0xFFFF) &&
                (Flag_e_FALSE == g_HostFrameConstraints.e_Flag_AllowChangeOver) &&
                (u16_Selected == g_u16Lastmode_Selected))
                {
                    break;
                }

                i16_iter++;
            }

            // Update the value of max line lenght for max dz here after mode selection is complete
            u16_MaxLineLengthForMaxDZ = Min(u16_SelectedModeLineLen, Max(u16_MaxLineLength, u16_RequiredLineLength));

            if (e_FrameDimensionRequestStatus == FrameDimensionRequestStatus_e_Denied)
            {
                OstTraceInt0(TRACE_ERROR, "No Mode found");
                return (FrameDimensionRequestStatus_e_Denied);
            }
            else
            {
                g_u16Lastmode_Selected = u16_Selected;
                if
                (
                    (
                    g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].woi_res.width ==
                    g_Zoom_Status_LLA.u16_woi_resX
                    )
                &&
                    (
                    g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].output_res.width ==
                    g_Zoom_Status_LLA.u16_output_resX
                    )
                &&
                    (
                    g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].woi_res.height ==
                    g_Zoom_Status_LLA.u16_woi_resY
                    )
                &&
                    (
                    g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].output_res.height ==
                    g_Zoom_Status_LLA.u16_output_resY
                    )
                &&
                    (
                    ((uint16_t)(g_CurrentFrameDimension.u16_VTLineLengthPck * VT_32SF_OUTPUT_TIMING_DERATING)) >=
                    u16_RequiredLineLength
                    )
                && (g_Zoom_Status.e_ZoomRequestLLDStatus != ZoomRequestLLDStatus_e_ReProgeamRequired)
                )
                {
                    e_Flag_CurrentFDMSufficient = Flag_e_TRUE;
                }
                else
                {
                    e_Flag_CurrentFDMSufficient = Flag_e_FALSE;
                }

                g_Zoom_Status_LLA.u16_Ex_output_resX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].output_res.width;
                g_Zoom_Status_LLA.u16_Ex_output_resY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].output_res.height - non_smia_correction_factor;
                g_Zoom_Status_LLA.u16_Ex_woi_resX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].woi_res.width;
                g_Zoom_Status_LLA.u16_Ex_woi_resY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_Selected].woi_res.height - non_smia_correction_factor;
                g_RequestedFrameDimension.u16_VTLineLengthPck = u16_MaxLineLengthForMaxDZ;
                OstTraceInt4(TRACE_FLOW,"Selected mode: WOI: X:%d,Y:%d,OP: X:%d,Y:%d",g_Zoom_Status_LLA.u16_Ex_woi_resX,g_Zoom_Status_LLA.u16_Ex_woi_resY,g_Zoom_Status_LLA.u16_Ex_output_resX,g_Zoom_Status_LLA.u16_Ex_output_resY);
            }
        }
        else
        {
            e_Flag_CurrentFDMSufficient = Flag_e_TRUE;
        }

        if (FD_IS_SENSOR_STREAMING())
        {
            // in auto mode while streaming
            // if the current frame dimension can fit the request, accept it
            if (Flag_e_TRUE == e_Flag_CurrentFDMSufficient)
            {
                e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Accepted;
            }
            else
            {
                // current frame dimension cannot meet the request
                // if a sensor change over is allowed, go for it
                if (Flag_e_TRUE == g_HostFrameConstraints.e_Flag_AllowChangeOver)
                {
                    // a sensor reprogram will be required
                    e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_ReProgeamRequired;
                }
                else
                {
                    // a change over is not allowed, deny the request
                    e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Denied;
                }
            }
        }
    }
    else
    {
        // BML case
        g_Zoom_Status_LLA.u16_output_resX = g_SensorFrameConstraints.u16_MaxOPXOutputSize;
        g_Zoom_Status_LLA.u16_output_resY = g_SensorFrameConstraints.u16_MaxOPYOutputSize;
        g_Zoom_Status_LLA.u16_woi_resX = g_SensorFrameConstraints.u16_MaxOPXOutputSize * g_CurrentFrameDimension.f_PreScaleFactor;
        g_Zoom_Status_LLA.u16_woi_resY = g_SensorFrameConstraints.u16_MaxOPYOutputSize * g_CurrentFrameDimension.f_PreScaleFactor;

        OstTraceInt0(TRACE_FLOW, "lla_abstractionFrameDimension_RequestFrameConfig()-> BML case");
        if ((g_Zoom_Status_LLA.u16_woi_resX >= u16_DesiredFOVX) && (g_Zoom_Status_LLA.u16_woi_resY >= u16_DesiredFOVY))
        {
            g_RequestedFrameDimension.u16_VTLineLengthPck = u16_RequiredLineLength;
            e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Accepted;
        }
    }

    return (e_FrameDimensionRequestStatus);
}


///Interrupt timer ISR,
///interrupt from timer2 of GPIO block
void
LLA_InterruptTimer_ISR(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    //Send signal to LLD about timer expiration
    retVal = cam_drv_signal(CAM_DRV_SIGNAL_TIMER_1_EXPIRED);
    if (CAM_DRV_OK != retVal)
    {
        //Failed to send signal to low level camera driver
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_InterruptTimer_ISR() - cam_drv_signal() returned failure for timer_1 expiry!!");
        ASSERT_XP70();
        return;
    }


    return;
}


// include Auto Focus 4 Modules (AutoFocus+AFstats+FocusControl+FLADriver)
#if INCLUDE_FOCUS_MODULES

///Interrupt timer ISR,
///interrupt from timer2 of GPIO block
void
LLA_InterruptTimer2_ISR(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    g_FLADriver_LensLLDParam.u32_FLADIntTimer2Count++;
    retVal = cam_drv_signal(CAM_DRV_SIGNAL_TIMER_2_EXPIRED);
    if (CAM_DRV_OK != retVal)
    {
        //Failed to send signal to low level camera driver
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_InterruptTimer2_ISR() - cam_drv_signal() returned failure for timer_2 expiry!!");
        ASSERT_XP70();
        return;
    }


    return;
}

void
LLA_Abstraction_Lens_Stop(void)
{
    CAM_DRV_RETVAL_E        ReturnVal;

    ReturnVal = cam_drv_lens_stop();

    if (CAM_DRV_OK != ReturnVal)
    {
        /* handle the case*/
    }

    return;
}

#endif //INCLUDE_FOCUS_MODULES

void
LLA_AbstractionRegisterReq(
CAM_DRV_EVENT_E event_code)
{
    //Store the event registration information in lla_abstraction layer
    if
    (
        CAM_DRV_REGISTER_FOR_NEXT_FSC == event_code
    ||  CAM_DRV_REGISTER_FOR_FSC == event_code
    ||  CAM_DRV_UNREGISTER_FOR_FSC == event_code
    )
    {
        g_event_status.u8_EventStatus_FSC = event_code;
    }


    if
    (
        CAM_DRV_REGISTER_FOR_NEXT_FEC == event_code
    ||  CAM_DRV_REGISTER_FOR_FEC == event_code
    ||  CAM_DRV_UNREGISTER_FOR_FEC == event_code
    )
    {
        g_event_status.u8_EventStatus_FEC = event_code;
    }


    return;
}


void
LLA_Abstraction_SendSignal_FSC(void)
{
    Flag_te             Flag_SendSignal = Flag_e_FALSE;
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    if (CAM_DRV_REGISTER_FOR_FSC == g_event_status.u8_EventStatus_FSC)
    {
        //Driver has register for ALL FSC events so send this signal to low level driver
        Flag_SendSignal = Flag_e_TRUE;
    }
    else if (CAM_DRV_REGISTER_FOR_NEXT_FSC == g_event_status.u8_EventStatus_FSC)
    {
        //Driver has requested for next FSC only i.e. driver is probably not interested in further event of this kind
        //so unregister this event
        g_event_status.u8_EventStatus_FSC = CAM_DRV_UNREGISTER_FOR_FSC;

        //Also need to send this event to low level driver
        Flag_SendSignal = Flag_e_TRUE;
        OstTraceInt0(TRACE_DEBUG, "NxtFSC");
    }
    else
    {
        //Do nothing
        //In this case no event sent to driver
    }


    if (Flag_e_TRUE == Flag_SendSignal)
    {
        //send signal to low level driver
        retVal = cam_drv_signal(CAM_DRV_SIGNAL_FSC);
        if (CAM_DRV_OK != retVal)
        {
            //Failed to sent signal to low level driver
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_SendSignal_FSC() - cam_drv_signal() returned failure for SIGNAL FSC!!");
            ASSERT_XP70();
        }
    }


    return;
}


void
LLA_Abstraction_SendSignal_FEC(void)
{
    Flag_te             Flag_SendSignal = Flag_e_FALSE;
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    if (CAM_DRV_REGISTER_FOR_FEC == g_event_status.u8_EventStatus_FEC)
    {
        //Driver has register for ALL FEC events so send this signal to low level driver
        Flag_SendSignal = Flag_e_TRUE;
    }
    else if (CAM_DRV_REGISTER_FOR_NEXT_FEC == g_event_status.u8_EventStatus_FEC)
    {
        //Driver has requested for next FEC only i.e. driver is probably not interested in further event of this kind
        //so unregister this event
        g_event_status.u8_EventStatus_FEC = CAM_DRV_UNREGISTER_FOR_FEC;

        //Also need to send this event to low level driver
        Flag_SendSignal = Flag_e_TRUE;
        OstTraceInt0(TRACE_DEBUG, "Nxt FEC");
    }
    else
    {
        //Do nothing
        //In this case no event sent to driver
    }


    if (Flag_e_TRUE == Flag_SendSignal)
    {
        //send signal to low level driver
        retVal = cam_drv_signal(CAM_DRV_SIGNAL_FEC);
        if (CAM_DRV_OK != retVal)
        {
            //Failed to sent signal to low level driver
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_SendSignal_FEC() - cam_drv_signal() returned failure for SIGNAL FEC!!");
            ASSERT_XP70();
        }
    }


    return;
}


void
LLA_Abstraction_InterpretSensorSettings(void)
{
    uint32_t    retVal = CAM_DRV_OK;
    uint8_t     *pu8_AncillaryLine = NULL;

    // Parse ISL data for senors with SMIA compatible ISL lines.
    if(SENSOR_VERSION_NON_SMIA != g_SensorInformation.u8_smia_version)
    {
        /*
          // ISL Data to be sent to the LLD
          ===================================================================================================
          Register Name         : Address                : Embedded Line                        : No of bytes
          ===================================================================================================
          Data Format Code      : [0x0A]                 : [0A]                                 : 1
          FRAME_COUNT           : [0x0005]               : [AA,00, A5,05, 5A,XX]                : 6
          Fine integration      : [0x0200, 0x0201]       : [AA,02, A5,00], [5A,XX],[5A,XX]      : 8
          Coarse integration    : [0x0202, 0x0203]       : [5A,XX], [5A,XX]                     : 4
          Analogue Gain         : [0x0204, 0x0205]       : [5A,XX], [5A,XX]                     : 4
          FRAME_LENGTH_LINES_HI : [0x340, 0x341]         : [AA,03, A5,40], [5A,XX],[5A,XX]      : 8
          LINE_LENGTH_PCK_HI    : [0x342, 0x343]         : [5A,XX], [5A,XX]                     : 4
          DIGITAL_GAIN_GREENR   : [0x020E, 0X020F]       : [AA,02, A5,0E], [5A,XX],[5A,XX]      : 8
          FLASH_STATUS          : [0x0C1C]               : [AA,0C, A5,1C, 5A,XX]                : 6
          End Codes             : [0x07]                 : [07]                                 : 1
          ===================================================================================================
          Total                                                                                 : 50 bytes
          ===================================================================================================

          For 8bit data format (No 0x55 bytes)
          0x0A,
          0xAA, 0x00, 0xA5, 0x05, 0x5A, 0x**,
          0xAA, 0x02, 0xA5, 0x00, 0x5A, 0x**, 0x5A, 0x**,
          0x5A, 0x**, 0x5A, 0x**,
          0x5A, 0x**, 0x5A, 0x**,
          0xAA, 0x03, 0xA5, 0x40, 0x5A, 0x**, 0x5A, 0x**,
          0x5A, 0x**, 0x5A, 0x**,
          0xAA, 0x02, 0xA5, 0x0E, 0x5A, 0x**, 0x5A, 0x**,
          0xAA, 0x0C, 0xA5, 0x1C, 0x5A, 0x**,
          0x07

          For 10bit data format (a 0x55 added after every 4 bytes)
          0x0A,
          0xAA, 0x00, 0xA5, {0x55}, 0x05, 0x5A, 0x**,
          0xAA, {0x55}, 0x02, 0xA5, 0x00, 0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0xAA, {0x55}, 0x03, 0xA5, 0x40, 0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0xAA, {0x55}, 0x02, 0xA5, 0x0E, 0x5A, {0x55}, 0x**, 0x5A, 0x**,
          0xAA, {0x55}, 0x0C, 0xA5, 0x1C, 0x5A,{0x55}, 0x**,
          0x07
        */
        if
        (
            (
                (StreamMode_e_VF == g_RunMode_Control.e_StreamMode)
            &&  ((CAM_DRV_SENS_FORMAT_RAW8 == g_vf_config.format) || (CAM_DRV_SENS_FORMAT_RAW8_DPCM == g_vf_config.format))
            )
        ||  (
                (StreamMode_e_Capture == g_RunMode_Control.e_StreamMode)
            &&  (
                    (CAM_DRV_SENS_FORMAT_RAW8 == g_capture_config.format)
                ||  (CAM_DRV_SENS_FORMAT_RAW8_DPCM == g_capture_config.format)
                )
            )
        )
        {
            pu8_AncillaryLine = g_pu8_ISL_Raw8;
            pu8_AncillaryLine[ISL_RAW8_FRAME_COUNT_OFFSET] = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();
            pu8_AncillaryLine[ISL_RAW8_FINE_INTEGRATION_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_FINE_EXPOSURE_fine_exposure()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_FINE_INTEGRATION_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_FINE_EXPOSURE_fine_exposure()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_COARSE_INTEGRATION_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_COARSE_EXPOSURE_coarse_exposure()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_COARSE_INTEGRATION_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_COARSE_EXPOSURE_coarse_exposure()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_ANALOGUE_GAIN_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_ANALOG_GAIN_analog_gain()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_ANALOGUE_GAIN_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_ANALOG_GAIN_analog_gain()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_FRAME_LENGTH_LINES_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_FRAME_LENGTH_frame_length()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_FRAME_LENGTH_LINES_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_FRAME_LENGTH_frame_length()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_LINE_LENGTH_PCK_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_LINE_LENGTH_line_length()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_LINE_LENGTH_PCK_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_LINE_LENGTH_line_length()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW8_DIGITAL_GAIN_GREENR_HIGH] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_1_hk_req_data()));
            pu8_AncillaryLine[ISL_RAW8_DIGITAL_GAIN_GREENR_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_2_hk_req_data()));
            pu8_AncillaryLine[ISL_RAW8_FLASH_STATUS_OFFSET_LOW] = (uint8_t) (Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_0_hk_req_data());
        }
        else if
            (
                (
                (StreamMode_e_VF == g_RunMode_Control.e_StreamMode)
            &&  (CAM_DRV_SENS_FORMAT_RAW10 == g_vf_config.format)
                )
                ||  (
                        (StreamMode_e_Capture == g_RunMode_Control.e_StreamMode)
                    &&  (CAM_DRV_SENS_FORMAT_RAW10 == g_capture_config.format)
                    )
            )
        {
            pu8_AncillaryLine = g_pu8_ISL_Raw10;
            pu8_AncillaryLine[ISL_RAW10_FRAME_COUNT_OFFSET] = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();
            pu8_AncillaryLine[ISL_RAW10_FINE_INTEGRATION_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_FINE_EXPOSURE_fine_exposure()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_FINE_INTEGRATION_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_FINE_EXPOSURE_fine_exposure()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_COARSE_INTEGRATION_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_COARSE_EXPOSURE_coarse_exposure()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_COARSE_INTEGRATION_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_COARSE_EXPOSURE_coarse_exposure()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_ANALOGUE_GAIN_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_ANALOG_GAIN_analog_gain()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_ANALOGUE_GAIN_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_ANALOG_GAIN_analog_gain()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_FRAME_LENGTH_LINES_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_FRAME_LENGTH_frame_length()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_FRAME_LENGTH_LINES_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_FRAME_LENGTH_frame_length()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_LINE_LENGTH_PCK_OFFSET_HIGH] = (uint8_t) (((Get_ISP_SMIARX_ISP_SMIARX_LINE_LENGTH_line_length()) >> 8) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_LINE_LENGTH_PCK_OFFSET_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_LINE_LENGTH_line_length()) & 0xFF);
            pu8_AncillaryLine[ISL_RAW10_DIGITAL_GAIN_GREENR_HIGH] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_1_hk_req_data()));
            pu8_AncillaryLine[ISL_RAW10_DIGITAL_GAIN_GREENR_LOW] = (uint8_t) ((Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_2_hk_req_data()));
            pu8_AncillaryLine[ISL_RAW10_FLASH_STATUS_OFFSET_LOW] = (uint8_t) (Get_ISP_SMIARX_ISP_SMIARX_HK_REQ_DATA_0_hk_req_data());
        }
    }

    /*Call low level driver API to find the current exposure and gain settings*/
    retVal = cam_drv_interpret_sensor_settings(pu8_AncillaryLine, &g_SensorSettings);

    //Debug print to check data in ISL
    OstTraceInt3(TRACE_DEBUG, "ISL: EXP: %d, AG: %d, FlashStatus: %d", g_SensorSettings.exposure_time_us,  g_SensorSettings.analog_gain_x1000 *256/1000, g_SensorSettings.flash_fired);

    if (CAM_DRV_OK != retVal)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_InterpretSensorSettings() - cam_drv_interpret_sensor_settings() returned failure!!");
        ASSERT_XP70();
    }

    return;
}


//Check whether Exposure and gain settings applied to sensors has been absorbed or not
Flag_te
LLA_Abstraction_IsExpGainSettingsAbsorbed(void)
{
    Flag_te Flag_SettingsAbsorbed = Flag_e_FALSE;

    //compare if current exposure and gain are same as given in the last feedback
    if(IS_HDR_COIN_TOGGLED())
    {
        Flag_SettingsAbsorbed = HDR_IsAEC_Absorbed();
        if(Flag_SettingsAbsorbed)
        {
            OstTraceInt0(TRACE_USER7, "<EXP> <HDR> setting absorbed");
        }
        else
        {
            OstTraceInt0(TRACE_USER7, "<EXP> <HDR> setting not absorbed");
        }
    }
    else
    {
        if(
            (g_SensorSettings.exposure_time_us == g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us)
              && ((((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000) == g_Exposure_ParametersApplied.u16_AnalogGain_x256)
           )
        {
            //last gain and exposure settings has been absorbed
            g_FrameParamStatus.u32_ExposureTime_us = g_SensorSettings.exposure_time_us;
            g_FrameParamStatus.u32_AnalogGain_x256 = ((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000;
            g_FrameParamStatus.u32_frameRate_x100 = g_FrameDimensionStatus.f_CurrentFrameRate * 100;    // as framerate is not being returned, so for the time-being do this
            g_FrameParamStatus.u32_SensorExposureTimeMax_us = GetSensorExposureTimeMax();
            Flag_SettingsAbsorbed = Flag_e_TRUE;
            OstTraceInt0(TRACE_DEBUG, "<EXP> setting absorbed");
        }
        else
        {
            //last gain and exposure settings yet to be absorbed
            Flag_SettingsAbsorbed = Flag_e_FALSE;
            OstTraceInt0(TRACE_DEBUG, "<EXP> setting not absorbed");
        }
    }

    return (Flag_SettingsAbsorbed);
}


void
LLA_Abstraction_ApplyAecAndFreameRate(
Exposure_DriverControls_ts  *ptr_Exposure_DriverControls)
{
    //memset((void*)&g_cam_drv_config, NULL, sizeof(CAM_DRV_CONFIG_T));
    g_cam_drv_config.p_config_frame_rate = &g_ConfigFrameRate;
    g_cam_drv_config.p_config_gain_and_exp_time = &g_ExpGainSetting;

    //Initialize the local config structure for applying gain and exposure setting to sensor
    g_cam_drv_config.config_contents.config_gain_and_exp_time = 1;

    g_cam_drv_config.p_config_gain_and_exp_time->exposure_time_us = ptr_Exposure_DriverControls->u32_TargetExposureTime_us;

    g_cam_drv_config.p_config_gain_and_exp_time->analog_gain_x1000 = (uint16_t) (((uint32_t) ptr_Exposure_DriverControls->u16_TargetAnalogGain_x256 * 1000) / 256);

    // Sensor digital gain is not used, All digital domain gains are applied in pipe "Channel gains block"
    g_cam_drv_config.p_config_gain_and_exp_time->digital_gain_x1000 = U16_DIGITAL_GAIN_x1000;

    g_cam_drv_config.config_contents.config_frame_rate = 1;

    // Sensor digital gain is not used, All digital domain gains are applied in pipe "Channel gains block"

    g_cam_drv_config.p_config_frame_rate->frame_rate_x100 = FrameRate_GetAskedFrameRate() * 100;

    return;
}


void
LLA_Abstraction_ApplyAec(
float_t f_FrameRate_Hz,
uint32_t u32_TargetExposureTime_us,
uint16_t u16_TargetAnalogGain_x256
)
{
    g_cam_drv_config.p_config_frame_rate = &g_ConfigFrameRate;
    g_cam_drv_config.p_config_gain_and_exp_time = &g_ExpGainSetting;

    //Initialize the local config structure for applying gain and exposure setting to sensor
    g_cam_drv_config.config_contents.config_gain_and_exp_time = 1;
    g_cam_drv_config.p_config_gain_and_exp_time->exposure_time_us = u32_TargetExposureTime_us;
    g_cam_drv_config.p_config_gain_and_exp_time->analog_gain_x1000 = (uint16_t) (((uint32_t) u16_TargetAnalogGain_x256 * 1000) / 256);

    // Sensor digital gain is not used, All digital domain gains are applied in pipe "Channel gains block"
    g_cam_drv_config.p_config_gain_and_exp_time->digital_gain_x1000 = U16_DIGITAL_GAIN_x1000;

    if(g_HDR_StatusVariable.u8_ParamsReqFrameCount <= g_HDR_Control.u8_FPSConfigCount)
    {
        g_cam_drv_config.config_contents.config_frame_rate = 1;
        g_cam_drv_config.p_config_frame_rate->frame_rate_x100 = (uint16_t) f_FrameRate_Hz * 100;
    }

    return;
}


//This is to set configuration only for Exposure, Framerate and Flash only when running
void
LLA_Abstraction_ConfigSet_streaming(void)
{
    uint32_t    retVal = CAM_DRV_OK;

    //Call LLA API to apply gain and exposure settings to sensor
    retVal = cam_drv_config_set(&g_cam_drv_config, &g_config_feedback);
    if (CAM_DRV_OK != retVal)
    {
        if (1 == g_cam_drv_config.config_contents.config_flash_strobe)
        {
            g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_CONFIGURATION;
        }

        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_ConfigSet_streaming() - cam_drv_config_set() returned failure while config flash!!");
        ASSERT_XP70();
    }
    //[NON_ISL_SUPPORT]
    //when we are getting feedback, we are subtracting non_smia_correction_factor from height of output and woi sizes
    g_config_feedback.output_res.height -= non_smia_correction_factor;
    g_config_feedback.woi_res.height -= non_smia_correction_factor;

    if(IS_HDR_COIN_TOGGLED())
    {
        switch (g_HDR_StatusVariable.u8_ParamsReqFrameCount)
        {
            // TODO : make it configurable so that it can be applied to any sensor
            case 1:
                g_HDR_Status.u16_TargetAnalogGain_1_x256 =
                        (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
                g_HDR_Status.u32_TargetExposureTime_1_us = g_config_feedback.exposure_time_us;
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Requested- Exp: %d, AG_x256: %d", g_HDR_Control.u32_TargetExposureTime_1_us, g_HDR_GainControl.u16_TargetAnalogGain_1_x256);
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Feedback - Exp: %d, AG_x256: %d", g_HDR_Status.u32_TargetExposureTime_1_us, g_HDR_Status.u16_TargetAnalogGain_1_x256);
                break;

            case 2:
                g_HDR_Status.u16_TargetAnalogGain_2_x256 =
                        (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
                g_HDR_Status.u32_TargetExposureTime_2_us = g_config_feedback.exposure_time_us;
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Requested- Exp: %d, AG_x256: %d", g_HDR_Control.u32_TargetExposureTime_2_us, g_HDR_GainControl.u16_TargetAnalogGain_2_x256);
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Feedback - Exp: %d, AG_x256: %d", g_HDR_Status.u32_TargetExposureTime_2_us, g_HDR_Status.u16_TargetAnalogGain_2_x256);
                break;

            case 3:
                g_HDR_Status.u16_TargetAnalogGain_3_x256 =
                        (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
                g_HDR_Status.u32_TargetExposureTime_3_us = g_config_feedback.exposure_time_us;
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Requested- Exp: %d, AG_x256: %d", g_HDR_Control.u32_TargetExposureTime_3_us, g_HDR_GainControl.u16_TargetAnalogGain_3_x256);
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Feedback - Exp: %d, AG_x256: %d", g_HDR_Status.u32_TargetExposureTime_3_us, g_HDR_Status.u16_TargetAnalogGain_3_x256);
                break;

            case 4:
                g_HDR_Status.u16_TargetAnalogGain_4_x256 =
                        (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
                g_HDR_Status.u32_TargetExposureTime_4_us = g_config_feedback.exposure_time_us;
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Requested- Exp: %d, AG_x256: %d", g_HDR_Control.u32_TargetExposureTime_4_us, g_HDR_GainControl.u16_TargetAnalogGain_4_x256);
                OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming:HDR- Feedback - Exp: %d, AG_x256: %d", g_HDR_Status.u32_TargetExposureTime_4_us, g_HDR_Status.u16_TargetAnalogGain_4_x256);
                break;

            default:
                OstTraceInt1(TRACE_USER7, "LLA_Abstraction_ConfigSet_streaming: HDR- Unhandled state: Exp Req FC: %d", g_HDR_StatusVariable.u8_ParamsReqFrameCount);
            break;
        }
    }
    else
    {
        g_Exposure_ParametersApplied.u16_AnalogGain_x256 = (uint16_t) ((uint32_t) g_config_feedback.analog_gain_x1000 * 256 / 1000);
        g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us = g_config_feedback.exposure_time_us;
        OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming: Requested- Exp: %d, AG_x256: %d", g_Exposure_DriverControls.u32_TargetExposureTime_us, g_Exposure_DriverControls.u16_TargetAnalogGain_x256);
        OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming: Feedback - Exp: %d, AG_x256: %d", g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us, g_Exposure_ParametersApplied.u16_AnalogGain_x256);
    }

    OstTraceInt2(TRACE_DEBUG, "LLA_Abstraction_ConfigSet_streaming: Applied  - Exp: %d, AG_x256: %d", g_FrameParamStatus.u32_ExposureTime_us, g_FrameParamStatus.u32_AnalogGain_x256);
    g_FrameDimensionStatus.f_VTFrameLength_us = (1 / (((float_t) g_config_feedback.frame_rate_x100) / 100)) * 1000000;
    g_FrameDimensionStatus.f_CurrentFrameRate = ((float_t) g_config_feedback.frame_rate_x100) / 100;
    g_VariableFrameRateStatus.f_CurrentFrameRate_Hz = g_FrameDimensionStatus.f_CurrentFrameRate;
    g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines = (uint16) (g_FrameDimensionStatus.f_VTFrameLength_us / g_FrameDimensionStatus.f_VTLineLength_us);
    g_CurrentFrameDimension.u16_VTFrameLengthLines = g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines;

    lla_abstraction_update_exposure_limits(&g_config_feedback);

    //check for flash config error
    if (FlashError_e_FLASH_ERROR_NONE != g_FlashStatus.e_FlashError_Info)
    {
        Flash_HandleFlashErrors();
    }


    //Clear all config bits
    lla_abstraction_ResetConfigContents();
}


//This configures ND Filter to ON/OFF state. To be called when user toggles ND Filter coin.
void
LLA_Abstraction_Set_NDFilter(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    Flag_te             state;

    if (g_FrameParamStatus.u32_Flag_NDFilter != g_Exposure_DriverControls.e_Flag_NDFilter)
    {
        //if ND Filter is not supported, return
        if (NULL == g_camera_details.p_nd_details)
        {
            return;
        }


        state = g_Exposure_DriverControls.e_Flag_NDFilter;

        retVal = cam_drv_nd_control(CAM_DRV_ND_STATE_SET, ( bool8 * ) &state);
        if (CAM_DRV_OK != retVal)
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_Set_NDFilter() - cam_drv_nd_control() returned failure!!");
            ASSERT_XP70();
        }
        else
        {
            g_FrameParamStatus.u32_Flag_NDFilter = state;
        }
    }
}


//Get actual stream status of sensor from low level driver
Flag_te
LLA_Abstraction_GetStreamingStatus(void)
{
    Flag_te             Flag_StreamingState;
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    CAM_DRV_MODES_E     modeVal;

    //Get sensor current streaming status
    retVal = cam_drv_mode_control(CAM_DRV_MODE_CONTROL_GET, ( CAM_DRV_MODES_E * ) &modeVal);
    if (CAM_DRV_OK != retVal)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_GetStreamingStatus() - cam_drv_mode_control() returned failure!!");
        ASSERT_XP70();
    }


    if (CAM_DRV_MODE_STREAMING == modeVal)
    {
        //Sensor is in streaming mode
        Flag_StreamingState = Flag_e_TRUE;
    }
    else
    {
        //Sensor is not streaming, either is in HW/SW standby mode
        Flag_StreamingState = Flag_e_FALSE;
    }


    return (Flag_StreamingState);
}


//Modify stream status of sensor using low level driver API

//No need to call cam_drv_off to just stop sensor streaming
void
LLA_Abstraction_SetStreamStatus(
Flag_te Flag_StreamingState)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    CAM_DRV_MODES_E     modeVal;

    if (Flag_e_TRUE == Flag_StreamingState)
    {
        //Start sensor streaming
        modeVal = CAM_DRV_MODE_STREAMING;
    }
    else
    {
        //Stop sensor streaming
        modeVal = CAM_DRV_MODE_SW_STANDBY;
    }


    //Set sensor current streaming status
    retVal = cam_drv_mode_control(CAM_DRV_MODE_CONTROL_SET, ( CAM_DRV_MODES_E * ) &modeVal);

    if (CAM_DRV_OK != retVal)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_SetStreamStatus() - cam_drv_mode_control() returned failure!!");
        ASSERT_XP70();
    }


    return;
}


//Store any messages recieved from camera driver

//Also this function would be called from interrupt context
CAM_DRV_FN_RETVAL_E
LLA_Abstracetion_SendDrvMsg(
CAM_DRV_MSG_E   msg_id,
void            *p_msg_info,
uint16          msg_size)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_OK;

    //TBD:<CN>Currently we are not storing 'msg_info' and 'msg_size', as in current LLD these are passed as NULL
    //May be in future we need to modify this func and store extra information
    //in current implementation it is assumed that no two messages would be pending at same time
    //set the bit corresponding to message id
    g_u16DrvPendMsg = (1 << msg_id);

    return (retVal);
}


//Handle any pending camera driver message

//This function is called from state machine i.e. in non-interrupt context
void
LLA_Abstraction_CamDrvHandleMsg(void)
{
    uint16_t    u16_PendingMsg;

    /* To make the following operations atomic, disable all interrupts,
     * process, and then re-enable them.
     */
    // Disable all interrupts that are being used
    STXP70_DisableInterrupts(ITM_INTERRUPT_MASK);

    u16_PendingMsg = g_u16DrvPendMsg;
    g_u16DrvPendMsg = 0;    //clear shared variable

    // Enable all interrupts that are being used
    STXP70_EnableInterrupts(ITM_INTERRUPT_MASK);

    //handle any pending message one by one
    if (u16_PendingMsg & (1 << CAM_DRV_MSG_FSC))
    {
        //handle FSC message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_FSC, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for FSC!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_FEC))
    {
        //handle FEC message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_FEC, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for FEC!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_AF_LENS_MOVED))
    {
        //handle Lens moved message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_AF_LENS_MOVED, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for AF_LENS_MOVED!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_AF_LENS_POSITION_MEASURED))
    {
        //handle positioned measured message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_AF_LENS_POSITION_MEASURED, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for LENS_POSITION_MEASURED!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED))
    {
        //handle lens timer expired message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for AF_LENS_TIMER_EXPIRED!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_LENS_STOPPED))
    {
        //handle lens stopped message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_LENS_STOPPED, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for LENS_STOPPED!!");
            ASSERT_XP70();
        }
    }


    if (u16_PendingMsg & (1 << CAM_DRV_MSG_DUMP_REGISTERS))
    {
        //Dump register message
        if (CAM_DRV_OK != cam_drv_handle_msg(CAM_DRV_MSG_DUMP_REGISTERS, NULL))
        {
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!LLA_Abstraction_CamDrvHandleMsg() - cam_drv_handle_msg() returned failure for MSG_DUMP_REGISTERS!!");
            ASSERT_XP70();
        }
    }


    return;
}


uint8_t
lla_abstraction_user_BMS_input_valid(void)
{
    CAM_DRV_CONFIG_VF_T         vf_config;
    CAM_DRV_CONFIG_FEEDBACK_T   config_feedback;
    CAM_DRV_RETVAL_E            ReturnVal;
    uint8_t                     e_Flag_valid = Flag_e_FALSE;

    vf_config.trial_only = Flag_e_TRUE;

    vf_config.output_res.width = g_Zoom_Status_LLA.u16_Ex_output_resX;

    vf_config.output_res.height = g_Zoom_Status_LLA.u16_Ex_output_resY + non_smia_correction_factor; //correct bms

    vf_config.woi_res.height = g_Zoom_Status_LLA.u16_Ex_woi_resY + non_smia_correction_factor;     //correct bms

    vf_config.woi_res.width = g_Zoom_Status_LLA.u16_Ex_woi_resX;

    vf_config.frame_rate_x100 = (uint16) (FrameRate_GetMaximumFrameRate() * 100);

    // Program 0 line length as ISP FW do not need any line length
    vf_config.min_line_length_pck = 0;

    vf_config.format = lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat());

    vf_config.analog_gain_x1000 = (uint16) (((uint32_t) g_Exposure_DriverControls.u16_TargetAnalogGain_x256 * 1000) / 256);

    vf_config.digital_gain_x1000 = U16_DIGITAL_GAIN_x1000;

    vf_config.exposure_time_us = g_Exposure_DriverControls.u32_TargetExposureTime_us;

    vf_config.sensor_mode = lla_abstraction_FindSensorModeFromUserRestrictionBitmask(g_RunMode_Control.e_LLD_USAGE_MODE_usagemode);

    vf_config.ac_frequency = 0;

    lla_abstraction_set_orientation(&vf_config.orientation);

    // Apply actual settings
    ReturnVal = cam_drv_start_vf(&vf_config, &config_feedback);

    if (CAM_DRV_OK == ReturnVal)
    {
        if((config_feedback.woi_res.height == vf_config.woi_res.height)
        &&  (config_feedback.woi_res.width == vf_config.woi_res.width)
        &&  (config_feedback.output_res.width == vf_config.output_res.width)
        &&  (config_feedback.output_res.height == vf_config.output_res.height)
        &&  (config_feedback.frame_rate_x100 >= vf_config.frame_rate_x100)
        )
        {
            e_Flag_valid = Flag_e_TRUE;
        }
        else
        {
            OstTraceInt0(TRACE_WARNING, "lla_abstraction_user_BMS_input_valid: Some parameters failed for BMS");
            OstTraceInt4(TRACE_WARNING, "Feedback W:X: %d Y: %d O:X: %d Y: %d", config_feedback.woi_res.width, config_feedback.woi_res.height, config_feedback.output_res.width, config_feedback.output_res.height);
            OstTraceInt1(TRACE_WARNING, "Feedback Frame rate %d",config_feedback.frame_rate_x100);
            /// Some parameters are not correct in VF trial mode
            e_Flag_valid = Flag_e_FALSE;
        }
    }
    else
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!lla_abstraction_user_BMS_input_valid() - cam_drv_start_vf() returned failure!!");
        ASSERT_XP70();
    }

    return (e_Flag_valid);
}

