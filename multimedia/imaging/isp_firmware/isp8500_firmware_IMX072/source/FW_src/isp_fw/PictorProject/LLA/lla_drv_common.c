/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/

/*
lla_drv_common.c - common file for both SMIA and non-smia sensors
*/
#include "lla_common_config.h"
#include "smia_sensor_memorymap_defs.h"
#include "lla_drv_bh6476.h"
#include "lla_drv_imx072.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_LLA_lla_drv_commonTraces.h"
#endif

/*Global variables*/
// Object definition for time model struct
volatile TimeModelConfig_ts                                       g_TimeModelConfig;

uint8_t                     g_LensMoving_AtStopRequest = Flag_e_FALSE;

/*Structure holding global configuration for selected camera module*/
CamDrvCommonGlbConfig_ts                                          g_CamDrvConfig;

// Create a structure and initilize the pointer to lens details with address of this struct. Else AF wont work because driver will receive NULL pointer
CAM_DRV_LENS_DETAILS_T                                            g_LensDetails;

/*Local Functions*/
void cam_drv_initialize_dcc_data ( void ) TO_EXT_PRGM_MEM;
void cam_drv_update_awb_data (CAM_DRV_AWB_DATA_T * p_config_awb_data) TO_EXT_PRGM_MEM;
void cam_drv_update_zoom_roi (CAM_DRV_SENS_CONFIG_ZOOM_ROI_T * p_config_zoom_roi) TO_EXT_PRGM_MEM;
void cam_drv_apply_config_settings (CAM_DRV_CONFIG_CONTENTS_T config_contents) TO_EXT_PRGM_MEM;
void cam_drv_apply_vf_settings ( const CAM_DRV_CONFIG_VF_T *p_vf_config, Flag_te fdParamUpdated ) TO_EXT_PRGM_MEM;
void cam_drv_apply_capture_settings
    ( const CAM_DRV_CONFIG_CAPTURE_T *p_capture_config, Flag_te fdParamUpdated ) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E cam_drv_is_lens_moving (Flag_te * isLensMoving) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E cam_drv_get_curr_lens_pos (uint16_t * currPos) TO_EXT_PRGM_MEM;

void AF_DummyInitializeCallbackFn (CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts * p_CallbackFn) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E AF_GetLensDetail (CAM_DRV_LENS_DETAILS_T * p_lens_details) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E AF_AFDriver_Init ( void ) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E AF_AFDriver_MeasurePos (uint16_t * u16_CurrentDac) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E AF_LensMovToPos (int32 position, uint32_t * pu32_MoveTime_us) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E cam_drv_handle_flash_before_streaming(void);
/***
LLA API definitions --
*/

/* cam_drv_init:
    LLA API for initializing camera driver
*/
CAM_DRV_RETVAL_E
cam_drv_init(
const CAM_DRV_CONFIG_INIT_T *p_init_config)
{
    /*Parameter validation*/
    if (NULL == p_init_config)
    {
        //OstTraceInt0(TRACE_DEBUG, "STXP70 IN ASSERT cam_drv_init");
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /*Also validate the state, if camera driver is already initialized then this API
            should not have been called again*/
    if (CamDrvState_e_UnInitialize != LLA_GetCamDrvState())
    {
        //OstTraceInt0(TRACE_DEBUG, "CamDrvState_e_UnInitialize IN cam_drv_init");
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Store all the function pointers (Callback API) passed into LLA global config datastructure
            TBD:<CN>:All of the API must be implemented by client. If client not supporting any API
            then should alteast implement dummy function (returning success) for that API
            */
    if
    (
        NULL == p_init_config->cam_drv_block_alloc_no_wait_fn
    ||  NULL == p_init_config->cam_drv_block_dealloc_fn
    ||  NULL == p_init_config->cam_drv_event_fn
    ||  NULL == p_init_config->cam_drv_msg_send_fn
    ||  NULL == p_init_config->cam_drv_timer_start_fn
    ||  NULL == p_init_config->cam_drv_timer_stop_fn
    ||  NULL == p_init_config->cam_drv_control_physical_pins_fn
    ||  NULL == p_init_config->cam_drv_common_i2c_read_fn
    ||  NULL == p_init_config->cam_drv_common_i2c_write_fn
    ||  NULL == p_init_config->cam_drv_sensor_about_to_stop_fn
    ||  NULL == p_init_config->cam_drv_sensor_started_fn
    ||  NULL == p_init_config->cam_drv_int_timer_fn
    ||  NULL == p_init_config->cam_drv_blocking_timer_fn
    ||  NULL == p_init_config->cam_drv_control_additional_physical_pins_fn
    )
    {
        /*No Call back API should be NULL, there should be atleast dummy implementation at client*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /*Store all function pointers in global configuration structure*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_block_alloc_no_wait_fn = p_init_config->cam_drv_block_alloc_no_wait_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_block_dealloc_fn = p_init_config->cam_drv_block_dealloc_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn = p_init_config->cam_drv_event_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_msg_send_fn = p_init_config->cam_drv_msg_send_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_timer_start_fn = p_init_config->cam_drv_timer_start_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_timer_stop_fn = p_init_config->cam_drv_timer_stop_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_physical_pins_fn = p_init_config->cam_drv_control_physical_pins_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn = p_init_config->cam_drv_common_i2c_read_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn = p_init_config->cam_drv_common_i2c_write_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_about_to_stop_fn = p_init_config->cam_drv_sensor_about_to_stop_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_started_fn = p_init_config->cam_drv_sensor_started_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_int_timer_fn = p_init_config->cam_drv_int_timer_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_blocking_timer_fn = p_init_config->cam_drv_blocking_timer_fn;
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_control_additional_physical_pins_fn = p_init_config->cam_drv_control_additional_physical_pins_fn;

    /*Change camera driver state*/
    g_CamDrvConfig.camDrvState = CamDrvState_e_Initialize;
    return (CAM_DRV_OK);
}


void
LLA_blocking_delay(
uint32  time_in_us)
{
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_blocking_timer_fn(time_in_us / 10);
}


/* cam_drv_on: LLA camera driver API.
Camera will be first queried to identify whether its a SMIA compliant or Non-SMIA sensor
*/
CAM_DRV_RETVAL_E
cam_drv_on(
const CAM_DRV_CONFIG_ON_T   *p_config_on,
CAM_DRV_CAMERA_TYPE_T       *p_camera_type)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;

    /*This API must be called only when driver is in off state*/
    if ((CamDrvState_e_Off != LLA_GetCamDrvState()) || (NULL == p_config_on) || (NULL == p_camera_type))
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*parameter validation*/
    if
    (
        p_config_on->ext_clock_freq_x100 < LLA_MIN_IN_EXT_FREQx100
    ||  p_config_on->ext_clock_freq_x100 > LLA_MAX_IN_EXT_FREQx100
    )
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    // Initilize AF functions with dummy values to avoid crash if not inialized with correct parameter and when called later on.
    AF_DummyInitializeCallbackFn(&g_CamDrvConfig.AFDriverCallbackFnApis);

    //Initialise the function pointer with sensor's function
    LLA_IMX072_InitializeCallbackFn(&g_CamDrvConfig.SensorCallbackFnApis);

    // Initilize AF functions
    BH6476_InitializeCallbackFn(&g_CamDrvConfig.AFDriverCallbackFnApis);

    /*Bit of initialization*/
    g_CamDrvConfig.flagIsStreaming = Flag_e_FALSE;

    /*output format defaulted to RAW10to8*/
    g_CamDrvConfig.csiRawFormat = CAM_DRV_SENS_FORMAT_RAW10;
    g_CamDrvConfig.flagIsClientStoppedStreaming = Flag_e_FALSE;
    g_CamDrvConfig.camLensState = CamLensState_e_UnInitalize;
    g_CamDrvConfig.flagIsLensTimerRunning = Flag_e_FALSE;

    /*Number of times lens timer restarted after inital expiry*/
    g_CamDrvConfig.u8_LensRetries = 0;

    /* DM -> Initialization of the FSC count value*/
    g_TimeModelConfig.u32_FSCCount = 0;

    /*Module specific power on, if required*/
    retVal = g_CamDrvConfig.SensorCallbackFnApis.FnModulePowerOn(p_config_on);

    /*parameter validation: MUST BE CALLED AFTER "ModulePowerOn", as some var will be initilized there*/
    if (CAM_DRV_OK != g_CamDrvConfig.SensorCallbackFnApis.FnCheckModeConfigurations(p_config_on))
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - FnCheckModeConfigurations CAM_DRV_ERROR_PARAM!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    if (CAM_DRV_FN_OK != retVal)
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - CAM_DRV_ERROR_HW!!");
#endif

        /*Module specific power on failed,*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    /*TBD:<CN>:Current LLA do not make use of voltages*/
    g_CamDrvConfig.operatingVoltages.Vana_x100 = p_config_on->operating_voltages.Vana_x100;
    g_CamDrvConfig.operatingVoltages.Vdig_x100 = p_config_on->operating_voltages.Vdig_x100;
    g_CamDrvConfig.operatingVoltages.Vio_x100 = p_config_on->operating_voltages.Vio_x100;

    /*Read camera type detail from sensor directly*/
    if (CAM_DRV_FN_OK != g_CamDrvConfig.SensorCallbackFnApis.FnGetCameraTypeDetail(p_camera_type))
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - CAM_DRV_ERROR_MISC!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MISC);
    }


    /*Initialize the DCC ConfigData, atleast initialize the idle settings if any*/
    cam_drv_initialize_dcc_data();

    /*Do sensor specific init sequence if required*/
    if (CAM_DRV_FN_OK != g_CamDrvConfig.SensorCallbackFnApis.FnSensorInitSequence())
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    /*Register with camera driver client for frame start and frame end events*/

    /*Register for frame start event*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_REGISTER_FOR_FSC, NULL, FALSE);

    /*register for frame end event*/
    g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_REGISTER_FOR_FEC, NULL, FALSE);

    /*Change driver state*/
    g_CamDrvConfig.camDrvState = CamDrvState_e_Idle;

    if (CAM_DRV_OK != g_CamDrvConfig.AFDriverCallbackFnApis.AFDriver_Init())
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_on - AFDriverCallbackFnApis CAM_DRV_ERROR_PARAM!!");
#endif
        LLA_ASSERT_XP70();
    }
    else
    {
        /*Lens init*/
        g_CamDrvConfig.camLensState = CamLensState_e_Idle;
    }


    return (CAM_DRV_OK);
}


/*cam_drv_off
LLA API to turn off the previously selected camera module
*/
CAM_DRV_RETVAL_E
cam_drv_off(void)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_FN_OK;
    CamDrvState_te      state = LLA_GetCamDrvState();

    /*Client should call this API, only if client is on state*/
    if (CamDrvState_e_Off == state || CamDrvState_e_Initialize == state)
    {
        return (CAM_DRV_ERROR_MODE);
    }


    /*Check if camera is streaming*/
    if (g_CamDrvConfig.flagIsStreaming)
    {
        /*Inform client about sensor stopping*/
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_about_to_stop_fn();
        retVal = g_CamDrvConfig.SensorCallbackFnApis.FnStartStopSensor(Flag_e_FALSE);

        /*Mark sensor not streaming*/
        g_CamDrvConfig.flagIsStreaming = Flag_e_FALSE;

        //DM -> Whenever streaming will be false, the count should be set to zero
        g_TimeModelConfig.u32_FSCCount = 0;

        if (CAM_DRV_FN_OK != retVal)
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_off - CAM_DRV_ERROR_HW!!");
#endif
            LLA_ASSERT_XP70();
            return (CAM_DRV_ERROR_HW);
        }


        /*Wait for minimum number of EXTCLK after last I2C*/

        /*Second argument is the number of EXTCLK to be wait after last I2C*/
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_timer_start_fn(CAM_DRV_OS_TIMER_GENERAL, XshutdownDelay_extclk_stop);
    }


    /*Module specific power on, if required*/
    retVal = g_CamDrvConfig.SensorCallbackFnApis.FnModulePowerOff();

    if (CAM_DRV_FN_OK != retVal)
    {
#if LLA_ASSERT_LOG_EN
        /*Module specific power on failed,*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_off - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_HW);
    }


    /*Moving camera state to off state*/
    g_CamDrvConfig.camDrvState = CamDrvState_e_Off;

    return (CAM_DRV_OK);
}


/*cam_drv_get_camera_details
 * This API will export selected camera capabilities to client
 * */
CAM_DRV_RETVAL_E
cam_drv_get_camera_details(
CAM_DRV_CAMERA_DETAILS_T    *p_camera_details)
{
    CamDrvState_te      camState = LLA_GetCamDrvState();
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    /*parameter validation*/
    if (NULL == p_camera_details)
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_camera_details - CAM_DRV_ERROR_PARAM!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /*Camera driver must be in on state*/
    if
    (
        (CamDrvState_e_Idle == camState)
    ||  (CamDrvState_e_ViewFinder == camState)
    ||  (CamDrvState_e_Capture == camState)
    )
    {
        //initilize the pointer to lens details of GetDetals to address of this struct.
        // Otherwise AF wont work because driver will receive NULL pointer.
        p_camera_details->p_lens_details = &g_LensDetails;

        /*Update camera camera modules detail - nvm, ndfilter, shutter, flash etc*/
        retVal = (CAM_DRV_RETVAL_E) ((uint8_t) retVal | (uint8_t) g_CamDrvConfig.SensorCallbackFnApis.FnGetCameraDetail(p_camera_details));

        /*Update camera driver lens detail*/
        retVal = (CAM_DRV_RETVAL_E)
            (
                (uint8_t) retVal | (uint8_t) g_CamDrvConfig.AFDriverCallbackFnApis.GetLensDetail(
                    p_camera_details->p_lens_details)
            );

        if (CAM_DRV_OK != retVal)
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_camera_details - CAM_DRV_ERROR_HW!!");
#endif
            LLA_ASSERT_XP70();
        }
    }
    else
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_camera_details - CAM_DRV_ERROR_MODE!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    return (CAM_DRV_OK);
}


/* cam_drv_config_set
LLA API to modify camera configuration like WOI, frame rate, DCC register settings etc...
*/
CAM_DRV_RETVAL_E
cam_drv_config_set(
const CAM_DRV_CONFIG_T      *p_config,
CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback)
{
    uint16              u16_VTFrameLengthLinesOlder,
                        u16_VTLineLengthPckOlder;
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    CamDrvCurrConfig_ts sCamCurrConfig;
    Flag_te             isStreaming = g_CamDrvConfig.flagIsStreaming;

    /*Camera driver must be on before this API being called*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Off == LLA_GetCamDrvState()))
    {
        /*Adding assert*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Parameter validations*/
    if (p_config == NULL || p_config_feedback == NULL)
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /*Camera driver must be on before this API being called*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Off == LLA_GetCamDrvState()))
    {
#if LLA_ASSERT_LOG_EN
        //OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_config_set - CAM_DRV_ERROR_MODE!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Trial mode is supported only for WOI, Framerate*/
    if (Flag_e_TRUE == p_config->trial_only)
    {
        if
        (
            1 == p_config->config_contents.config_awb_data
        ||  1 == p_config->config_contents.config_camera_data
        ||  1 == p_config->config_contents.config_gain_and_exp_time
        ||  1 == p_config->config_contents.config_test_picture
        ||  1 == p_config->config_contents.config_zoom_roi
        )
        {
            /*Trial mode is not supported for these configs*/
            LLA_ASSERT_XP70();
            return (CAM_DRV_ERROR_PARAM);
        }
    }


    /*Get the current sensor configuration i.e. Current WOI, OUTSIZE, programmed frame rate,programmed exposure */
    g_CamDrvConfig.SensorCallbackFnApis.FnGetCurrSensorConfig(&sCamCurrConfig);

    /*save the current frame length and line lenght in variables*/
    u16_VTLineLengthPckOlder = sCamCurrConfig.camCurrLineLenPck;
    u16_VTFrameLengthLinesOlder = sCamCurrConfig.camCurrFrameLenLine;

    /*Store DCC register settings*/

    /*TBD:<CN>: It is assumed here that the pointers to various configuration data will be kept in LLA.
            **Client should not free the information
            */
    if (1 == p_config->config_contents.config_camera_data)
    {
        /*updating the pointers with the new register list. Its a responsibility of client to free the previous
                        *pointer
                        */

        /*Idle register settings*/
        if (NULL != p_config->p_config_camera_data->p_config_idle)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_idle = p_config->p_config_camera_data->p_config_idle;
        }


        /*Still VF settings*/
        if (NULL != p_config->p_config_camera_data->p_config_still_vf)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_still_vf = p_config->p_config_camera_data->p_config_still_vf;
        }


        /*Video VF settings*/
        if (NULL != p_config->p_config_camera_data->p_config_video_vf)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_video_vf = p_config->p_config_camera_data->p_config_video_vf;
        }


        /*Video rec settings*/
        if (NULL != p_config->p_config_camera_data->p_config_video_rec)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_video_rec = p_config->p_config_camera_data->p_config_video_rec;
        }


        /*Capture*/
        if (NULL != p_config->p_config_camera_data->p_config_capture)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_capture = p_config->p_config_camera_data->p_config_capture;
        }


        /*Override settings*/
        if (NULL != p_config->p_config_camera_data->p_config_override)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_override = p_config->p_config_camera_data->p_config_override;
        }


        /*VF Night mode*/
        if (NULL != p_config->p_config_camera_data->p_config_vf_night_mode)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_vf_night_mode = p_config->p_config_camera_data->p_config_vf_night_mode;
        }


        /*Capture night mode*/
        if (NULL != p_config->p_config_camera_data->p_config_capture_night_mode)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_capture_night_mode = p_config->p_config_camera_data->p_config_capture_night_mode;
        }


        /*Prod test mode settings*/
        if (NULL != p_config->p_config_camera_data->p_config_prod_test_mode)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_prod_test_mode = p_config->p_config_camera_data->p_config_prod_test_mode;
        }


        /*Seq shot mode*/
        if (NULL != p_config->p_config_camera_data->p_config_seq_shot_mode)
        {
            g_CamDrvConfig.camDrvDccConfigData.p_config_seq_shot_mode = p_config->p_config_camera_data->p_config_seq_shot_mode;
        }
    }


    /*WOI or output resolution is changed, this can be done either in idel state or when sensor is streaming
            **As of now any change in the Framedimension will result in stopping of sensor (if streaming) */
    if (1 == p_config->config_contents.config_woi)
    {
        /*Client may ask for min_line_length required for up-scaling in the pipe so  pass asked line length p_config->min_line_length_pck also*/
        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameDimensionParameters(
            p_config->p_config_woi,
            p_config->min_line_length_pck,
            p_config->trial_only);
    }


    /*Requested frame rate is modified, frame rate is given priority over exposure*/
    if (1 == p_config->config_contents.config_frame_rate)
    {
        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameRate(
            p_config->p_config_frame_rate->frame_rate_x100,
            p_config->trial_only);
    }


    /*Gain and exposure time parameters has been modified, or change in frame rate may cause change in
                exposure related parameters
            */
    if (1 == p_config->config_contents.config_gain_and_exp_time)
    {
        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateGainAndExposure(
            p_config->p_config_gain_and_exp_time,
            p_config->trial_only);

        if (1 == p_config->config_contents.config_flash_strobe)
        {
            g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure = 1;
            if (Flag_e_TRUE == LLA_IS_NON_SMIA)
            {
                g_TimeModelConfig.u32_ExposureSyncFrame = g_TimeModelConfig.u32_FSCCount + LLA_DELAY_MODEL_FRAME_COUNT;
            }
        }
    }
    else
    {
        /*If there is change in frame len or line len because of woi/frame rate. Then exposure need to be adjusted*/

        /*Get the current sensor configuration i.e. Current WOI, OUTSIZE, programmed frame rate,programmed exposure */
        g_CamDrvConfig.SensorCallbackFnApis.FnGetCurrSensorConfig(&sCamCurrConfig);

        if
        (
            (sCamCurrConfig.camCurrFrameLenLine != u16_VTFrameLengthLinesOlder)
        ||  (sCamCurrConfig.camCurrLineLenPck != u16_VTLineLengthPckOlder)
        )
        {
            g_CamDrvConfig.SensorCallbackFnApis.FnUpdateGainAndExposure(
                &(sCamCurrConfig.camCurrExpGain),
                p_config->trial_only);
        }
    }


    /*Populate the feedback*/
    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFeedback(p_config_feedback, p_config->trial_only);

    /*Config test picture modified*/
    if (1 == p_config->config_contents.config_test_picture)
    {
        if (CAM_DRV_OK != g_CamDrvConfig.SensorCallbackFnApis.FnUpdateTestPicture(p_config->p_config_test_picture))
        {
#if LLA_ASSERT_LOG_EN
            /*Error has occured while enabling test pattern mode*/
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_config_set - CAM_DRV_ERROR_HW!!");
#endif
            LLA_ASSERT_XP70();
        }
    }


    /*AWB data modified*/
    if (1 == p_config->config_contents.config_awb_data)
    {
        cam_drv_update_awb_data(p_config->p_config_awb_data);
    }


    /*Zoom ROI data modified*/
    if (1 == p_config->config_contents.config_zoom_roi)
    {
        cam_drv_update_zoom_roi(p_config->p_config_zoom_roi);
    }


    /*Flash Data Modified*/
    if (1 == p_config->config_contents.config_flash_strobe)
    {
        // This is just to protect a case where a flash is requested even before the previous exposure and flash request was fulfilled.
        if (0 == p_config->config_contents.config_gain_and_exp_time)
        {
            g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure = 0;
        }


        // adding one frame as when we switch on flash in streaming mode, the first frame will not be fully exposed to flash as flash goes high
        // at readout time whereas exposure has started much before that, so we need to ignore the first frame and consider the second
        // frame as valid lit frame.
        if (Flag_e_TRUE == isStreaming)
        {
            g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount = p_config->p_config_flash_strobe->frame_count + 1;
        }
        else
        {
            //[PM]now flash can be configured even before start.
            g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount = p_config->p_config_flash_strobe->frame_count;
        }

        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFlash(p_config->p_config_flash_strobe);

        //Flash State Variable
        g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_Requested;
    }


    if (Flag_e_TRUE == p_config->trial_only)
    {
        return (CAM_DRV_OK);
    }
    else
    {
        /*Not in trial mode, so apply settings to sensor*/
        cam_drv_apply_config_settings(p_config->config_contents);
    }


    return (retVal);
}


/*cam_drv_start_vf
LLA implementation for 'cam_drv_start_vf' API
This will configure sensor in viewfinder mode and start streaming
*/
CAM_DRV_RETVAL_E
cam_drv_start_vf(
const CAM_DRV_CONFIG_VF_T   *p_vf_config,
CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback)
{
    CAM_DRV_RETVAL_E        retVal = CAM_DRV_OK;
    CamDrvCurrConfig_ts     sCamCurrConfig;
    Flag_te                 Flag_FDParametersUpdated = Flag_e_FALSE;

    /*Camera must not be in off state*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Initialize == LLA_GetCamDrvState()))
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Parameter validation*/
    if (NULL == p_vf_config || NULL == p_config_feedback)
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /* TODO Check if trial mode is requested and if it is requested, is it requested for the right parameters*/

    /*Update the frame format, ac_frequency*/
    g_CamDrvConfig.csiRawFormat = p_vf_config->format;

    //g_CamDrvConfig.u8_Acfrequency = p_vf_config->ac_frequency;

    /*Get the current configuration for the sensor*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGetCurrSensorConfig(&sCamCurrConfig);

    /*Update Frame dimension and video timing parameters*/

    /*Sensor is streaming then we should not touch any VideoTiming and FrameDimension parameters*/
    if ((Flag_e_FALSE == g_CamDrvConfig.flagIsStreaming) || ((Flag_e_TRUE == p_vf_config->trial_only)))
    {
        CAM_DRV_SENS_CONFIG_WOI_T   config_woi;
        config_woi.output_res = p_vf_config->output_res;
        config_woi.woi_res = p_vf_config->woi_res;

        /*There is change in frame dimension parameters, re-calculate the effective parameters*/
        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameDimensionParameters(
            &config_woi,
            p_vf_config->min_line_length_pck,
            p_vf_config->trial_only);

        /*Mark FD parameters has been updated*/
        Flag_FDParametersUpdated = Flag_e_TRUE;
    }


    /*Update frame length as per requested frame rate*/
    if (0 != p_vf_config->frame_rate_x100)
    {
        g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameRate(p_vf_config->frame_rate_x100, p_vf_config->trial_only);
    }


    /*Update the camera driver state to be in streaming*/
    g_CamDrvConfig.camDrvState = CamDrvState_e_ViewFinder;

    /*Update gain control and exposure time parameter*/
    CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T config_gain_and_exp_time;
    config_gain_and_exp_time.analog_gain_x1000 = p_vf_config->analog_gain_x1000;
    config_gain_and_exp_time.digital_gain_x1000 = p_vf_config->digital_gain_x1000;
    config_gain_and_exp_time.exposure_time_us = p_vf_config->exposure_time_us;

    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateGainAndExposure(&config_gain_and_exp_time, p_vf_config->trial_only);

    /*Populate feedbak structure to be returned to client*/
    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFeedback(p_config_feedback, p_vf_config->trial_only);

    if (Flag_e_TRUE == p_vf_config->trial_only)
    {
        return (retVal);
    }
    else
    {
        /* Commit settings to sensor only if it is NOT Trial Mode */
        cam_drv_apply_vf_settings(p_vf_config, Flag_FDParametersUpdated);
    }


    return (retVal);
}


/* LLA API - cam_drv_capture
*/
CAM_DRV_RETVAL_E
cam_drv_capture(
const CAM_DRV_CONFIG_CAPTURE_T  *p_capture_config,
CAM_DRV_CONFIG_FEEDBACK_T       *p_config_feedback)
{
    CAM_DRV_RETVAL_E        retVal = CAM_DRV_OK;
    CAM_DRV_SENS_FORMAT_E   oldFormat = g_CamDrvConfig.csiRawFormat;
    CamDrvCurrConfig_ts     sCamCurrConfig;
    Flag_te                 Flag_FDParametersUpdated = Flag_e_FALSE;

    /*Camera must not be in off state*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Initialize == LLA_GetCamDrvState()))
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Parameter validation*/
    if (NULL == p_capture_config || NULL == p_config_feedback)
    {
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }


    /*Camera must not be in off state*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Initialize == LLA_GetCamDrvState()))
    {
#if LLA_ASSERT_LOG_EN
        //OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_capture - CAM_DRV_ERROR_MODE!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Update the frame format, ac_frequency*/
    g_CamDrvConfig.csiRawFormat = p_capture_config->format;

    //*Get the current configuration for the sensor*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGetCurrSensorConfig(&sCamCurrConfig);

    /*Update Frame dimension and video timing parameters*/
    if
    (
        sCamCurrConfig.camCurrWoiRes.width != p_capture_config->woi_res.width
    ||  sCamCurrConfig.camCurrWoiRes.height != p_capture_config->woi_res.height
    ||  sCamCurrConfig.camCurrOutputRes.width != p_capture_config->output_res.width
    ||  sCamCurrConfig.camCurrOutputRes.height != p_capture_config->output_res.height
    ||  oldFormat != p_capture_config->format
    )
    {
        /*Sensor is streaming then we should not touch any VideoTiming and FrameDimension parameters*/
        if (Flag_e_FALSE == g_CamDrvConfig.flagIsStreaming)
        {
            CAM_DRV_SENS_CONFIG_WOI_T   config_woi;
            config_woi.output_res = p_capture_config->output_res;
            config_woi.woi_res = p_capture_config->woi_res;

            /*Call capture function to update global structs. We dont need Trial/NonTrial modes for this <SS>*/

            /*Pass minimum line length as 0 in this function for capture usecase*/
            g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameDimensionParameters(&config_woi, 0, Flag_e_FALSE);

            /*Mark FD parameters has been updated*/
            Flag_FDParametersUpdated = Flag_e_TRUE;
        }
    }


    /*Update frame rate, In capture mode integration time has more importance than frame rate. So set the frame rate so that requested integration time
    is achieved while keeping the frame rate as maximum
    */
    uint32_t    u32_TotalFrameTime,
                u32_FrameRate_x100;

    /*Calculate frame time required for exposure*/
    u32_TotalFrameTime = g_CamDrvConfig.SensorCallbackFnApis.FnComputeFrameTimeForExposure(p_capture_config->exposure_time_us);

    /*CalculateFrameRate*/
    u32_FrameRate_x100 = (uint32_t) ((float_t) (1000000.0 * 100) / u32_TotalFrameTime);

    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFrameRate(u32_FrameRate_x100, Flag_e_FALSE);

    /*Update gain control and exposure time parameter*/
    CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T config_gain_and_exp_time;
    config_gain_and_exp_time.analog_gain_x1000 = p_capture_config->analog_gain_x1000;
    config_gain_and_exp_time.digital_gain_x1000 = p_capture_config->digital_gain_x1000;
    config_gain_and_exp_time.exposure_time_us = p_capture_config->exposure_time_us;

    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateGainAndExposure(&config_gain_and_exp_time, Flag_e_FALSE);

    /*
            ######## TO DO ############
            #### HANDLE SHUTTER #########
            ### TOBE DONE LATER #########

            p_capture_config->mechanical_shutter_ctrl
            p_capture_config->use_mechanical_shutter

        */

    /*Populate feedbak structure to be returned to client*/
    g_CamDrvConfig.SensorCallbackFnApis.FnUpdateFeedback(p_config_feedback, Flag_e_FALSE);

    /*Commit settings to sensor*/
    cam_drv_apply_capture_settings(p_capture_config, Flag_FDParametersUpdated);

    /*Update the camera driver state to be in streaming*/
    g_CamDrvConfig.camDrvState = CamDrvState_e_Capture;

    return (retVal);
}


/*LLA API - cam_drv_interpret_sensor_settings
Parse embedded data lines provided by the sensor to determine whether IT/AG has been absorbed or not
*/
CAM_DRV_RETVAL_E
cam_drv_interpret_sensor_settings(
const uint8             *p_ancillary_lines,
CAM_DRV_SENS_SETTINGS_T *p_sensor_settings)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    uint8 u8_RetVal = Flag_e_FALSE;

    /*In the current implementation of LLD, it is assumed that client will provide the ISL lines*/
    if (NULL == p_sensor_settings)
    {
#if LLA_ASSERT_LOG_EN
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_interpret_sensor_settings - CAM_DRV_ERROR_PARAM!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_PARAM);
    }

    if(Flag_e_FALSE == LLA_IS_NON_SMIA)
    {
        // Pointer to ancillary data should not be NULL for SMIA sensor only. For NON-SMIA sensors, ancillary data will be NULL
        if (NULL == p_ancillary_lines)
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_interpret_sensor_settings - CAM_DRV_ERROR_PARAM!!");
#endif
            LLA_ASSERT_XP70();
            return (CAM_DRV_ERROR_PARAM);
        }

        u8_RetVal = g_CamDrvConfig.SensorCallbackFnApis.FnInterpretSensorSettings(p_ancillary_lines, p_sensor_settings);

        /* Check the return value from ISL for SMIA Sensor. This will be used when flash request is delayed because of change in EXP
        InterpretSensorSettings fn will return true if the requested value of EXP was absorbed. If so, we can clear the flag
        g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure. when flash request is pending
        */
        if((Flag_e_TRUE == u8_RetVal) && (1 == g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure))
        {
            g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure = 0;
        }
    }
    else
    {
        u8_RetVal = g_CamDrvConfig.SensorCallbackFnApis.FnInterpretSensorSettings(p_ancillary_lines, p_sensor_settings);
        if(
            (1 == g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure)
            && (g_TimeModelConfig.u32_ExposureSyncFrame == g_TimeModelConfig.u32_FSCCount)
            )
        {
            g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure = 0;
            g_TimeModelConfig.u32_ExposureSyncFrame = 0;
        }
    }

    // Communicate flash status to Firmware
    if (1 == g_CamDrvConfig.CamDrvFlashState.u8_FrameLit)
    {
        p_sensor_settings->flash_fired = Flag_e_TRUE;

        // Clear the status of flash-lit flag after its been used to report back in ISL.
        g_CamDrvConfig.CamDrvFlashState.u8_FrameLit = 0;
    }
    else
    {
        p_sensor_settings->flash_fired = Flag_e_FALSE;
    }

    return (retVal);
}


/*LLA API: cam_drv_mode_control
Interface to start/stop streaming
or to get streaming status from low level driver
*/
CAM_DRV_RETVAL_E
cam_drv_mode_control(
const CAM_DRV_MODE_CONTROL_E    action,
CAM_DRV_MODES_E                 *mode_val)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    Flag_te             flag_current_streaming_status = g_CamDrvConfig.flagIsStreaming;

    /*Parameters validations -mode_val should not be null*/
    if (NULL == mode_val)
    {
        retVal = CAM_DRV_ERROR_PARAM;
        return (retVal);
    }


    if (CAM_DRV_MODE_CONTROL_GET == action)
    {
        /*Provide streaming status to client*/
        if (Flag_e_TRUE == flag_current_streaming_status)
        {
            *mode_val = CAM_DRV_MODE_STREAMING;
        }
        else
        {
            *mode_val = CAM_DRV_MODE_SW_STANDBY_NO_TRUNCATION;
        }
    }
    else if (CAM_DRV_MODE_CONTROL_SET == action)
    {
        /*set the streaming status*/
        if (CAM_DRV_MODE_STREAMING == *mode_val)
        {
            if (Flag_e_FALSE == flag_current_streaming_status)
            {
                // Update flash state variables if flash is requested before start streaming
                if (CAM_DRV_OK  != cam_drv_handle_flash_before_streaming())
                {
#if LLA_ASSERT_LOG_EN
                    OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_mode_control - CAM_DRV_ERROR_HW!!");
#endif
                }

                g_CamDrvConfig.SensorCallbackFnApis.FnStartStopSensor(Flag_e_TRUE);

                /*Mark sensor streaming*/
                g_CamDrvConfig.flagIsStreaming = Flag_e_TRUE;

                /* Inform client about streaming */
                g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_started_fn();

                /*set flag streamingStopped by client to false */
                g_CamDrvConfig.flagIsClientStoppedStreaming = Flag_e_FALSE;
            }
        }
        else if ((CAM_DRV_MODE_SW_STANDBY == *mode_val) || (CAM_DRV_MODE_SW_STANDBY_NO_TRUNCATION == *mode_val))
        {
            /*
                IMX072 do not support Fast s/w standby.
                Following behaviour should be followed:
                CAM_DRV_MODE_SW_STANDBY: Do fast s/w standby
                CAM_DRV_MODE_SW_STANDBY_NO_TRUNCATION: Stop sensor so that full frame is given by sensor, it should not be truncated in b/w
            */

            /*Inform client about sensor stopping*/
            g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_about_to_stop_fn();
            if (CAM_DRV_FN_OK != g_CamDrvConfig.SensorCallbackFnApis.FnStartStopSensor(Flag_e_FALSE))
            {
                retVal = CAM_DRV_ERROR_HW;
            }


            /*Mark sensor not streaming*/
            g_CamDrvConfig.flagIsStreaming = Flag_e_FALSE;

            //DM -> Whenever streaming will be false, the count should be set to zero
            g_TimeModelConfig.u32_FSCCount = 0;
            g_TimeModelConfig.u32_ExposureSyncFrame = 0;

            /*set flag streamingStopped by client to false */
            g_CamDrvConfig.flagIsClientStoppedStreaming = Flag_e_TRUE;
        }
        else
        {
            /*return error*/
            retVal = CAM_DRV_ERROR_PARAM;
        }
    }
    else
    {
        /*return error*/
        retVal = CAM_DRV_ERROR_PARAM;
    }


    return (retVal);
}


CAM_DRV_RETVAL_E
cam_drv_signal(
const CAM_DRV_SIGNAL_E  signal_id)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    switch (signal_id)
    {
        case CAM_DRV_SIGNAL_FSC:
            //TBD:<CN>Currently we are specifying 'msg_info' and 'msg_size' NULL,
            // as in current LLD these are passed as NULL
            //
            // TODO : SM : First if statement is a workaround for a bug where a dummy FSC is sent, this should be removed at some later stage.
            if (g_CamDrvConfig.flagIsStreaming == Flag_e_TRUE)
            {
                //DM -> here for each FSC we are increment the FSC count, to count the number of frames
                g_TimeModelConfig.u32_FSCCount++;

                if (0 == g_CamDrvConfig.CamDrvFlashState.u8_FlashWaitForExposure)
                {
                    if ((0 == g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount) && (Flash_e_Fired == g_CamDrvConfig.CamDrvFlashState.e_FlashState))
                    {
                        g_CamDrvConfig.CamDrvFlashState.u8_FrameLit = 1;
                        g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_ReportFrameLit;
                    }


                    if ((Flash_e_Requested == g_CamDrvConfig.CamDrvFlashState.e_FlashState) || (Flash_e_Fired == g_CamDrvConfig.CamDrvFlashState.e_FlashState) || (Flash_e_ReportFrameLit == g_CamDrvConfig.CamDrvFlashState.e_FlashState))
                    {
                        g_CamDrvConfig.camDrvCallbackApis.cam_drv_msg_send_fn(
                            CAM_DRV_MSG_FSC,
                            NULL /*void * p_msg_info */,
                            0 /*uint16 msg_size*/ );
                    }
                }
            }


            break;

        case CAM_DRV_SIGNAL_TIMER_1_EXPIRED:
            retVal = g_CamDrvConfig.SensorCallbackFnApis.FnTriggerFlash(&(g_CamDrvConfig.CamDrvFlashState.e_FlashState));
            break;

        case CAM_DRV_SIGNAL_TIMER_2_EXPIRED:
            {
                CAM_DRV_EVENT_INFO_U    eventInfo;

                /*TIMER_2 is dedicated for lens movement related function*/

                /*This is the interrupt context*/

                /*Set lens timer flag to false*/
                g_CamDrvConfig.flagIsLensTimerRunning = Flag_e_FALSE;

                /*if there was any error while moving lens, then send that error to FW as a event*/
                if (CamLensState_e_Error == g_CamDrvConfig.camLensState)
                {
                    /*Send error event to FW*/
                    eventInfo.error_id = CAM_DRV_EVENT_ERROR_LENS;
                    g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_EVENT_ERROR, &eventInfo, TRUE);
                }


                /*If no error, and lens state is moving, then this indicate that lens has been moved*/
                if (CamLensState_e_Moving == g_CamDrvConfig.camLensState)
                {
                    /*This means that lens has been moved, need to send message to client to be in non-intrrupt case*/
                    g_CamDrvConfig.camDrvCallbackApis.cam_drv_msg_send_fn(CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED, NULL, 0);
                }
            }


            break;

        case CAM_DRV_SIGNAL_FEC:
#if 0
            if (Flash_e_ReportFrameLit == g_CamDrvConfig.CamDrvFlashState.e_FlashState)
            {
                g_CamDrvConfig.CamDrvFlashState.u8_FrameLit = 0;
                g_CamDrvConfig.camDrvCallbackApis.cam_drv_msg_send_fn(
                    CAM_DRV_MSG_FEC,
                    NULL /*void * p_msg_info */,
                    0 /*uint16 msg_size*/ );
            }

#endif
            break;

        case CAM_DRV_SIGNAL_OTHER:
        default:
    #if LLA_ASSERT_LOG_EN
            /*This should not be the case, so asserting here to identify any potential bug*/

            //OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_signal - CAM_DRV_ERROR_PARAM!!");
    #endif

            /*This should not be the case, so asserting here to identify any potential bug*/
            LLA_ASSERT_XP70();
            retVal = CAM_DRV_ERROR_PARAM;
            break;
    }


    return (retVal);
}


/* LLA API - cam_drv_handle_msg
Handle messages corresponding to events raised by driver in interrupt context.
Client will move to normal context and called this API for each raised event
*/
CAM_DRV_RETVAL_E
cam_drv_handle_msg(
CAM_DRV_MSG_E   msg_id,
const void      *p_msg)
{
    CAM_DRV_RETVAL_E        retVal = CAM_DRV_OK;
    Flag_te                 Flag_isLensMoving = Flag_e_FALSE;

    CAM_DRV_EVENT_INFO_U    eventInfo;
    uint16_t                u16_CurrLensPos = 0;

    /*Handle messages posted by LLD to move some task from interrupt context to non-interrupt context*/
    switch (msg_id)
    {
        case CAM_DRV_MSG_FSC:
            // We need to write registers in this frame to get Flash in the next frame
            if (g_CamDrvConfig.CamDrvFlashState.e_FlashState == Flash_e_Requested)
            {
                retVal = g_CamDrvConfig.SensorCallbackFnApis.FnTriggerFlash(&(g_CamDrvConfig.CamDrvFlashState.e_FlashState));

                //Registers programmed
                g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_Fired;
                g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount--;
            }
            else if (g_CamDrvConfig.CamDrvFlashState.e_FlashState == Flash_e_Fired)
            {
                g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount--;
            }
            else if(g_CamDrvConfig.CamDrvFlashState.e_FlashState == Flash_e_ReportFrameLit)
            {
                // Do Not clear the flash lit status flag (g_CamDrvConfig.CamDrvFlashState.u8_FrameLit) from here. This flag should be cleared from Interpret
                // sensor settings of the next frame. If cleared from here, there is possibility that we set it from CAM_DRV_SIGNAL but clear it from FEC HANDLE MSG
                // before reporting back flash status to ISP FW (in ISL from MAIN LOOP)
                retVal = g_CamDrvConfig.SensorCallbackFnApis.FnTriggerFlash(&(g_CamDrvConfig.CamDrvFlashState.e_FlashState));
                g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_NotRequested;
            }

            break;

        case CAM_DRV_MSG_FEC:
#if 0
            if (g_CamDrvConfig.CamDrvFlashState.e_FlashState == Flash_e_ReportFrameLit)
            {
                retVal = g_CamDrvConfig.SensorCallbackFnApis.FnTriggerFlash(&(g_CamDrvConfig.CamDrvFlashState.e_FlashState));
                g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_NotRequested;
            }
#endif

            break;

        case CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED:
            {
                //OstTraceInt1(TRACE_DEBUG, "CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED event came on frame no.: %d", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());

                /*This message is posted when lens has been moved, need to send an even for completeion of lens movement*/
                if (CamLensState_e_Moving == g_CamDrvConfig.camLensState)
                {
                    /*Check if lens has indeed stop moving*/
                    retVal = cam_drv_is_lens_moving(&Flag_isLensMoving);
                    if (CAM_DRV_OK != retVal)
                    {
                        /*error has occured*/
                        eventInfo.error_id = CAM_DRV_EVENT_ERROR_LENS;
                        g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_EVENT_ERROR, &eventInfo, FALSE);

                        /*Set the lens state to error*/
                        g_CamDrvConfig.camLensState = CamLensState_e_Error;
                    }


                    if ( /*if lens is still moving*/ (Flag_e_TRUE == Flag_isLensMoving))
                    {
                        /*Get the current lens position*/
                        retVal = cam_drv_get_curr_lens_pos(&u16_CurrLensPos);

                        /*Lens is still moving, either restart the timer or report error if alredy passed max number of retries*/
                        if (MAX_LENS_TIMER_RETRIES == g_CamDrvConfig.u8_LensRetries)
                        {
                            /*lens is still moving and max number of retries has done*/
                            eventInfo.error_id = CAM_DRV_EVENT_ERROR_LENS;
                            g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_EVENT_ERROR, &eventInfo, FALSE);

                            /*Set the lens state to error*/
                            g_CamDrvConfig.camLensState = CamLensState_e_Error;
                        }
                        else
                        {
                            /*restart the lens timer*/

                            /*Start the timer, in granularity of 10us*/

                            //OstTraceInt1(TRACE_DEBUG, "Restart the timer on frame no.: %d", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());
                            g_CamDrvConfig.camDrvCallbackApis.cam_drv_int_timer_fn(
                                CAM_DRV_INT_TIMER_2,
                                LENS_RETRIES_STEP_TIME / 10);

                            /*Set lens state to moving*/
                            g_CamDrvConfig.camLensState = CamLensState_e_Moving;

                            /*Set the timer flag to true*/
                            g_CamDrvConfig.flagIsLensTimerRunning = Flag_e_TRUE;

                            /*inc the lens retries counter*/
                            g_CamDrvConfig.u8_LensRetries++;
                        }
                    }
                    else
                    {
                        /*lens has been moved to requested position*/

                        /*Get the current lens position*/
                        retVal = cam_drv_get_curr_lens_pos(&u16_CurrLensPos);
                        if (CAM_DRV_OK != retVal)
                        {
                            /*error has occured while measuring the pos*/
                            eventInfo.error_id = CAM_DRV_EVENT_ERROR_LENS;
                            g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_EVENT_ERROR, &eventInfo, FALSE);

                            /*Set the lens state to error*/
                            g_CamDrvConfig.camLensState = CamLensState_e_Error;
                        }
                        else
                        {
                            /*Send the lens moved even to FW*/
                            eventInfo.lens_position = u16_CurrLensPos;
			    if (Flag_e_TRUE == g_LensMoving_AtStopRequest)
			    {
				g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_AF_LENS_MOVED, &eventInfo, FALSE);
				g_LensMoving_AtStopRequest = Flag_e_FALSE;
			    }
			    else
			    {
                            g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_AF_LENS_MOVED, &eventInfo, FALSE);
			    }

                            /*Set the lens state to idle*/
                            g_CamDrvConfig.camLensState = CamLensState_e_Idle;

                            /*reset the lens retries counter*/
                            g_CamDrvConfig.u8_LensRetries = 0;
                        }
                    }
                }
            }


            break;

        default:
    #if LLA_ASSERT_LOG_EN
            //OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_handle_msg - CAM_DRV_ERROR_PARAM!!");
    #endif
            LLA_ASSERT_XP70();
            retVal = CAM_DRV_ERROR_PARAM;
            break;
    }


    return (retVal);
}


/*LLA API - cam_drv_handle_flash_before_streaming
*/
CAM_DRV_RETVAL_E
cam_drv_handle_flash_before_streaming(void)
{
   CAM_DRV_RETVAL_E retVal = CAM_DRV_OK;

    //[PM] Applying flash settings just before Streaming
    if (g_CamDrvConfig.CamDrvFlashState.e_FlashState == Flash_e_Requested)
    {
        retVal = g_CamDrvConfig.SensorCallbackFnApis.FnTriggerFlash(&(g_CamDrvConfig.CamDrvFlashState.e_FlashState));
        if (CAM_DRV_OK == retVal)
        {
            //Registers programmed
            g_CamDrvConfig.CamDrvFlashState.e_FlashState = Flash_e_Fired;
            g_CamDrvConfig.CamDrvFlashState.u8_FlashFrameCount--;
        }
    }

    return (retVal);
}


/*LLA API - cam_drv_aperture_control
Aperture Control API
*/
CAM_DRV_RETVAL_E
cam_drv_aperture_control(
const CAM_DRV_APERTURE_CONTROL_E    action,
uint16 *const                       p_f_number_x_100)
{
    /*IMX072 module do not have aperture, for other modules this API may need to be extended*/
    return (CAM_DRV_ERROR_HW);
}


/* LLA API: cam_drv_nd_control
     Used to set/get ND Filter state.
*/
CAM_DRV_RETVAL_E
cam_drv_nd_control(
const CAM_DRV_ND_CONTROL_E  action,
bool8 *const                state)
{
    /*Currently supported sensors do not have ND Filter, so return error from here*/
    return (CAM_DRV_ERROR_MODE);
}


/*LLA API: cam_drv_get_config_data_identifiers
    Provides a list of configuration sub-block IDs known to this driver.
*/
CAM_DRV_RETVAL_E
cam_drv_get_config_data_identifiers(
CAM_DRV_CONFIG_DATA_IDENTIFIERS_T   *p_config_ids)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    /*Parameter validation*/
    if (NULL == p_config_ids)
    {
        //OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_config_data_identifiers - CAM_DRV_ERROR_PARAM!!");
#if LLA_ASSERT_LOG_EN
        /*Null parameter passed, hang in debug mode*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_config_data_identifiers - CAM_DRV_ERROR_PARAM!!");
#endif

        /*Null parameter passed, hang in debug mode*/
        LLA_ASSERT_XP70();

        return (CAM_DRV_ERROR_PARAM);
    }


    /*Currently for STE LLD, Sensor Tuning parser is not required, so returning 0 as LLD do not require any CDCC settings*/
    p_config_ids->identifiers = NULL;   /*No Identifiers required*/
    p_config_ids->number_of_identifiers = 0;

    return (retVal);
}


/*LLA API: cam_drv_cache_nvm
    set a global variable for accesing RAW NVM data
*/
CAM_DRV_RETVAL_E
cam_drv_cache_nvm(void)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    g_u8GetRawNVMData = Flag_e_TRUE;

    return (retVal);
}


/*LLA API: cam_drv_get_parsed_nvm
    Returns the filled CAM_DRV_NVM_T structure with NVM data from camera module
*/
CAM_DRV_RETVAL_E
cam_drv_get_parsed_nvm(
CAM_DRV_NVM_T   *p_nvm)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    if (NULL == p_nvm)
    {
#if LLA_ASSERT_LOG_EN
        /*Null parameter passed, hang in debug mode*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_get_parsed_nvm - CAM_DRV_ERROR_PARAM!!");
#endif

        /*Null parameter passed, hang in debug mode*/
        LLA_ASSERT_XP70();

        return (CAM_DRV_ERROR_PARAM);
    }


    if (Flag_e_FALSE == g_u8GetRawNVMData)
    {
        //OstTraceInt0(TRACE_DEBUG, "<NVM>cam_drv_get_parsed_nvm()-> LLA_GetParsedNVM()");
        retVal = LLA_IMX072_GetParsedNVM(p_nvm);
    }
    else
    {
        // OstTraceInt0(TRACE_DEBUG, "<NVM>cam_drv_get_parsed_nvm()-> LLA_GetRawNVM()");
        retVal = LLA_IMX072_GetRawNVM(( uint8 * ) p_nvm);
    }


    return (retVal);
}


/* LLA_InitializeDCCData:
Initialize DCC register settings configurations
*/
void
cam_drv_initialize_dcc_data(void)
{
    /*Initialize the flags*/
    g_CamDrvConfig.camDrvDccConfigData.configuration_flags.sens_disable_aec = 0;
    g_CamDrvConfig.camDrvDccConfigData.configuration_flags.sens_hscale = 0;
    g_CamDrvConfig.camDrvDccConfigData.configuration_flags.sens_idle_stream = 0;
    g_CamDrvConfig.camDrvDccConfigData.configuration_flags.sens_invert = 0;

    /*Initialize DCC registers list for different modes*/
    g_CamDrvConfig.camDrvDccConfigData.p_config_capture = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_capture_night_mode = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_idle = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_override = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_prod_test_mode = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_seq_shot_mode = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_still_vf = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_vf_night_mode = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_video_rec = NULL;
    g_CamDrvConfig.camDrvDccConfigData.p_config_video_vf = NULL;

    return;
}


void
cam_drv_update_awb_data(
CAM_DRV_AWB_DATA_T  *p_config_awb_data)
{
    /*TODO:<CN>:Need to implement AWB data usage*/
}


void
cam_drv_update_zoom_roi(
CAM_DRV_SENS_CONFIG_ZOOM_ROI_T  *p_config_zoom_roi)
{
    /*TODO:<CN>:Need to implement Zoom support*/
}


/*cam_drv_apply_config_settings
Apply config_set settings to sensor
*/
void
cam_drv_apply_config_settings(
CAM_DRV_CONFIG_CONTENTS_T   config_contents)
{
#if 1
    /*Commit FrameDimension and VideoTiming parameters to sensor*/
    if (1 == config_contents.config_woi)
    {
        /*Its a design decision to not update FD and VD parameters while sensor is streaming. And its a resposibility of client
         to explicitly stop streaming before changing FD parameters*/

        /*Comit frame dimension parameters*/
        g_CamDrvConfig.SensorCallbackFnApis.FnApplyFrameDimensionSettings();
    }

    if (((1 != config_contents.config_woi) && (1 == config_contents.config_frame_rate))|| (1 == config_contents.config_gain_and_exp_time))
     {
       /*Accquire Group Hold Register*/
        g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegSet();
     }

    /*If FD parameters has been applied then no need to do setting for frame rate, as FD settings also include settings required for frame rate*/
    if ((1 != config_contents.config_woi) && (1 == config_contents.config_frame_rate))
    {
        g_CamDrvConfig.SensorCallbackFnApis.FnApplyFrameRateSettings();
    }


    if (1 == config_contents.config_gain_and_exp_time)
    {
        /*As per SMIA functional - consumption of AnalogGain and ExposureParameters must be grouped together*/

        /*Apply exposure and gain settings to sensor*/
        g_CamDrvConfig.SensorCallbackFnApis.FnApplyExposureAndGainSettings();
    }

   if (((1 != config_contents.config_woi) && (1 == config_contents.config_frame_rate))|| (1 == config_contents.config_gain_and_exp_time))
   {
        /*Relase Group Hold Register*/
        g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegReset();
    }
#endif
    return;
}


void
cam_drv_apply_vf_settings(
const CAM_DRV_CONFIG_VF_T   *p_vf_config,
Flag_te                     fdParamUpdated)
{
    Flag_te isStreaming = g_CamDrvConfig.flagIsStreaming;

    /*Comit frame dimension parameters*/
    g_CamDrvConfig.SensorCallbackFnApis.FnApplyFrameDimensionSettings();

    /*Commit information passed in camera_on api into sensor and other misc settings*/
    g_CamDrvConfig.SensorCallbackFnApis.FnApplyMiscSettings(p_vf_config->orientation);

    /*Accquire Group Hold Register*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegSet();

    /*Commit gain related parameters*/
    g_CamDrvConfig.SensorCallbackFnApis.FnApplyExposureAndGainSettings();

    /*Relase Group Hold Register*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegReset();

    /*start streaming*/
    if (!(Flag_e_TRUE == isStreaming || Flag_e_TRUE == g_CamDrvConfig.flagIsClientStoppedStreaming))
    {
        // Update flash state variables if flash is requested before start streaming
        if (CAM_DRV_OK  != cam_drv_handle_flash_before_streaming())
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_mode_control - CAM_DRV_ERROR_HW!!");
#endif
        }

        /* If sensor is already streaming, or streaming has been stopped by client via mode_control API, then driver should not attempt to
        start the streaming again */
        g_CamDrvConfig.SensorCallbackFnApis.FnStartStopSensor(Flag_e_TRUE); /*Mark sensor streaming*/
        g_CamDrvConfig.flagIsStreaming = Flag_e_TRUE;

        /* Inform client about streaming */
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_started_fn();
    }


    return;
}


void
cam_drv_apply_capture_settings(
const CAM_DRV_CONFIG_CAPTURE_T  *p_capture_config,
Flag_te                         fdParamUpdated)
{
    Flag_te isStreaming = g_CamDrvConfig.flagIsStreaming;
    if (fdParamUpdated)
    {
        /*Comit frame dimension parameters*/
        g_CamDrvConfig.SensorCallbackFnApis.FnApplyFrameDimensionSettings();
    }


    /*Commit information passed in camera_on api into sensor and other misc settings*/
    g_CamDrvConfig.SensorCallbackFnApis.FnApplyMiscSettings(p_capture_config->orientation);

    /*Accquire Group Hold Register*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegSet();

    /*Commit gain related parameters*/
    g_CamDrvConfig.SensorCallbackFnApis.FnApplyExposureAndGainSettings();

    /*Relase Group Hold Register*/
    g_CamDrvConfig.SensorCallbackFnApis.FnGroupHoldRegReset();

    /*start streaming*/
    if (!(Flag_e_TRUE == isStreaming || Flag_e_TRUE == g_CamDrvConfig.flagIsClientStoppedStreaming))
    {
        // Update flash state variables if flash is requested before start streaming
        if (CAM_DRV_OK != cam_drv_handle_flash_before_streaming())
        {
#if LLA_ASSERT_LOG_EN
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_mode_control - CAM_DRV_ERROR_HW!!");
#endif
        }

        /* If sensor is already streaming, or streaming has been stopped by client via mode_control API, then driver should not attempt to
        start the streaming again */
        g_CamDrvConfig.SensorCallbackFnApis.FnStartStopSensor(Flag_e_TRUE);

        /*Mark sensor streaming*/
        g_CamDrvConfig.flagIsStreaming = Flag_e_TRUE;

        /* Inform client about streaming */
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_sensor_started_fn();
    }


    return;
}


/*LLA_IsLensMoving
Check low level AF driver to find is lens moving or not
*/
CAM_DRV_RETVAL_E
cam_drv_is_lens_moving(
Flag_te *isLensMoving)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    if (CAM_DRV_FN_OK != BH6476_AFDriver_IsLensMoving(isLensMoving))
    {
        /*there is an error in measuring lens status*/
        retVal = CAM_DRV_ERROR_HW;
    }


    return (retVal);
}


/*cam_drv_get_curr_lens_pos
 - Measure current lens position
*/
CAM_DRV_RETVAL_E
cam_drv_get_curr_lens_pos(
uint16_t    *currPos)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    if (CAM_DRV_FN_OK != g_CamDrvConfig.AFDriverCallbackFnApis.AFDriver_MeasurePos(currPos))
    {
        /*there is an error in measuring lens status*/
        retVal = CAM_DRV_ERROR_HW;
    }


    return (retVal);
}


/*cam_drv_lens_move_to_pos:-
    Move lens to position, intermidiate fn actual implementation depends on the AF driver used by the
    module,

*/
CAM_DRV_RETVAL_E
cam_drv_lens_move_to_pos(
int32   position)
{
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;
    uint32_t            u32_MoveTime_us = 0;    /*Time required for moving lens*/

    //    OstTraceInt1(TRACE_DEBUG, "Called cam_drv_lens_move_to_pos() in frame no.: %d", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());

    /*Camera must be init state*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Off == LLA_GetCamDrvState()))
    {
#if LLA_ASSERT_LOG_EN
        /*Adding assert*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_lens_move_to_pos - CAM_DRV_ERROR_MODE!!");
#endif
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    /*Move lens to a particular position - */
    retVal = g_CamDrvConfig.AFDriverCallbackFnApis.LensMovToPos(position, &u32_MoveTime_us);

    if (CAM_DRV_OK != retVal)
    {
#if LLA_ASSERT_LOG_EN
        /*AFDriver API return error, failed to move lens*/
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_lens_mov_to_pos - CAM_DRV_ERROR_HW!!");
#endif
        LLA_ASSERT_XP70();

        /*For non-debug release, set the lens state to error*/
        g_CamDrvConfig.camLensState = CamLensState_e_Error;
    }


    /*Check if required to start the timer*/
    if (NULL != u32_MoveTime_us)
    {
        /*Start the timer, in granularity of 10us*/
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_int_timer_fn(CAM_DRV_INT_TIMER_2, u32_MoveTime_us / 10);

        /*Set lens state to moving*/
        g_CamDrvConfig.camLensState = CamLensState_e_Moving;

        /*Set the timer flag to true*/
        g_CamDrvConfig.flagIsLensTimerRunning = Flag_e_TRUE;
    }
    else
    {
        CAM_DRV_EVENT_INFO_U    eventInfo;

        // Driver specific api return 0 as movement time,
        // i.e. either error occured or lens is already at that position
        // error case has already been taken careof, so lens must be at the same position already
        // send the lens movement complete event to client
        eventInfo.lens_position = position;
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_AF_LENS_MOVED, &eventInfo, FALSE);

        //Set lens state to idle
        g_CamDrvConfig.camLensState = CamLensState_e_Idle;
    }


    if (CAM_DRV_OK != retVal)
    {
        return (CAM_DRV_ERROR_MISC);
    }


    return (CAM_DRV_OK);
}


/* LLA API - cam_drv_lens_measure_pos
Starts measuring the current position of the lens.
*/
CAM_DRV_RETVAL_E
cam_drv_lens_measure_pos(void)
{
    CAM_DRV_RETVAL_E        retVal = CAM_DRV_OK;
    CAM_DRV_EVENT_INFO_U    eventInfo;
    uint16_t                u16_CurrLensPos = 0;

    /*Camera must be init state*/
    if ((CamDrvState_e_UnInitialize == LLA_GetCamDrvState()) || (CamDrvState_e_Off == LLA_GetCamDrvState()))
    {
        /*Adding assert*/
        LLA_ASSERT_XP70();
        return (CAM_DRV_ERROR_MODE);
    }


    retVal = cam_drv_get_curr_lens_pos(&u16_CurrLensPos);
    if (CAM_DRV_OK == retVal)
    {
        /*Send the lens pos measured event to FW*/
        eventInfo.lens_position = u16_CurrLensPos;
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_AF_LENS_POSITION_MEASURED, &eventInfo, FALSE);
    }


    return (CAM_DRV_OK);
}


/* LLA API - cam_drv_lens_get_status
Returns the status of the lens driver.
*/
CAM_DRV_RETVAL_E
cam_drv_lens_get_status(
CAM_DRV_LENS_STATUS_E   *p_status)
{
#if 0
    /*Lens is moving then return the status as active*/
    if (CamLensState_e_Moving)
    {
        *p_status = CAM_DRV_LENS_ACTIVE;
    }
    else
    {
        *p_status = CAM_DRV_LENS_IDLE;
    }


#endif
    return (CAM_DRV_OK);
}


/* LLA API - cam_drv_lens_stop
Stops anything that the lens driver is doing (moving lenses or measuring position of the lenses).
*/
CAM_DRV_RETVAL_E
cam_drv_lens_stop(void)
{
    Flag_te isLensMoving = Flag_e_FALSE;
    uint8_t isLensAbortSupported = Flag_e_FALSE;
    CAM_DRV_EVENT_INFO_U    eventInfo;

    isLensAbortSupported = BH6476_SupportLensAbort();

    cam_drv_is_lens_moving (&isLensMoving);
    if (Flag_e_FALSE == isLensMoving )
    {
	    // error case, lens is not moving, nothing to stop..
	    return CAM_DRV_ERROR_MODE;

    }
    else if (Flag_e_TRUE == isLensAbortSupported)
    {
	    uint16_t u16_CurrLensPos = 0xFFFF;
	    //lens movement can be aborted, program the actuator accordingly
	    // write regisers to abort lens movement

	    // Measure current position

	    // Store current position

            // return current lens position and stopped event
            eventInfo.lens_position = u16_CurrLensPos;
            g_CamDrvConfig.camDrvCallbackApis.cam_drv_event_fn(CAM_DRV_LENS_STOPPED, &eventInfo, FALSE);

    }
    else
    {
	    // Lens is moving and lens movement cannot be aborted, wait for lens movement to complete, do nothing here.
	    g_LensMoving_AtStopRequest = Flag_e_TRUE;
    }

    return (CAM_DRV_OK);
}


//////// Null Func Definition For AF Driver
CAM_DRV_RETVAL_E
AF_GetLensDetail(
CAM_DRV_LENS_DETAILS_T  *p_lens_details)
{
    return (CAM_DRV_OK);
}


CAM_DRV_FN_RETVAL_E
AF_AFDriver_Init(void)
{
    return (CAM_DRV_FN_OK);
}


CAM_DRV_FN_RETVAL_E
AF_AFDriver_MeasurePos(
uint16_t    *u16_CurrentDac)
{
    return (CAM_DRV_FN_OK);
}


CAM_DRV_RETVAL_E
AF_LensMovToPos(
int32       position,
uint32_t    *pu32_MoveTime_us)
{
    return (CAM_DRV_OK);
}


void
AF_DummyInitializeCallbackFn(
CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts    *p_CallbackFn)
{
    p_CallbackFn->GetLensDetail = AF_GetLensDetail;
    p_CallbackFn->AFDriver_Init = AF_AFDriver_Init;
    p_CallbackFn->AFDriver_MeasurePos = AF_AFDriver_MeasurePos;
    p_CallbackFn->LensMovToPos = AF_LensMovToPos;
}

