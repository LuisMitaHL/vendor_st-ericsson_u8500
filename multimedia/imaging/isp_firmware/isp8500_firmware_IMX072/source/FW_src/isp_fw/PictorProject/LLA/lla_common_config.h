/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/

/*
lla_common_config.h - common header file applicable for both smia and non-smia sensors
*/
#ifndef __LLA_COMMON_CONFIG_H__
#   define __LLA_COMMON_CONFIG_H__

#   include "cam_drv.h"                         /*For LLA specific inclusion*/
#   include "Platform.h"                        /*For Platform specific defines*/
#   include "DeviceParameter.h"

/*----Module inclusion defines----*/
#define LLA_INCLUDE_IMX072      1

/* I2C Address of PMIC- IMX072 */
#define LLA_I2C_DEVICE_ADDRESS_PMIC                               0x94
#define IMX072_ENABLE_PMIC_WRITES                                 (0)

// Count for number of sets of AG & EXP which will be saved in array
#define MAX_FRAME_COUNT                                           4

/*Frame Count after which requested values of AG/EXP will be absorbed*/
#define LLA_DELAY_MODEL_FRAME_COUNT                               2

/*SMIA Versions supported*/
/*SMIA version 0.9*/
#define LLA_SMIA_VERSION_V0_9                                     9

/*SMIA version 1.0*/
#define LLA_SMIA_VERSION_V1_0                                     10

/*SMIA version 1.1*/
#define LLA_SMIA_VERSION_V1_1                                     11

/**
    [NON_ISL_SUPPORT]:  SENSOR_ISL_DISABLE  and SENSOR_ISL_SMIA_COMPLIANT -
    These flag should be defined during the compilation in command line argument.
    Use combination of these flags to compile ISPFw for
 * Without ISL Lines enabled
 * With ISL Lines but not SMIA Compliant
 * With SMIA Compliant ISL
    during compilation, run this command "amm SENSOR_ISL_DISABLE=true  SENSOR_ISL_SMIA_COMPLIANT=true"
    Default configuration would be ISL Enabled but NON-Smia Compliant.
    */

// Select if Sensor can output ISL lines or not
#ifdef SENSOR_ISL_DISABLE

    // In this case, IMX072 is configured NOT to output ISL; hence NO_OF_ISL_FROM_SENSOR is set to 0.
    #define NO_OF_ISL_FROM_SENSOR                                 0
    #define LLA_IS_NON_SMIA                                       Flag_e_TRUE
    #define SENSOR_VERSION                                        SENSOR_VERSION_NON_SMIA

#else   //SENSOR_ISL_DISABLE

/* Only valid if SENSOR_ISL_ENABLE is defined*/
#ifdef SENSOR_ISL_SMIA_COMPLIANT

    // In this case, IMX072 is configured to output ISL; hence NO_OF_ISL_FROM_SENSOR is set to 2.
    #define NO_OF_ISL_FROM_SENSOR                                 2
    #define LLA_IS_NON_SMIA                                       Flag_e_FALSE

    // Sensor version for SMIA sensors will be read from the sensor itself. Defining here to avoid compilation issue.
    #define SENSOR_VERSION                                        LLA_SMIA_VERSION_V1_0

#else //SENSOR_ISL_SMIA_COMPLIANT

    // In this case, IMX072 is configured to output ISL; hence NO_OF_ISL_FROM_SENSOR is set to 2.
    #define NO_OF_ISL_FROM_SENSOR                                 2
    #define LLA_IS_NON_SMIA                                       Flag_e_TRUE
    #define SENSOR_VERSION                                        SENSOR_VERSION_NON_SMIA

#endif //SENSOR_ISL_SMIA_COMPLIANT

#endif //SENSOR_ISL_DISABLE

/*Change integer to hex char*/
#define IntegerToHexChar(int)                                     ((int>=0 && int<=9) ? int + '0' : ((int>=10 && int<=15) ? int +'A' -10 : 0))

/*Allow LLD to genrate flicker free timings*/
#define LLA_FLICKER_COMPATIABLE_TIMING                            Flag_e_FALSE


/*Maximum and minimum external input frequencyx100*/
/*6MHz*/
#define LLA_MIN_IN_EXT_FREQx100                                   600

/*27MHz*/
#define LLA_MAX_IN_EXT_FREQx100                                   2700

/*Data-format*/
/*RAW-10 Format*/
#define LLA_DATA_FORMAT_RAW10                                     0x0A0A

/*RAW-8 Format*/
#define LLA_DATA_FORMAT_RAW8                                      0x0808

/*10 to 8 DPCM*/
#define LLA_DATA_FORMAT_DPCM10TO8                                 0x0A08

/*MAX output speed in different output modes*/

/*CCP2 Data/Clock mode*/
#define MAX_SPEED_CCP2_DATA_CLK_MBPS                              (208)

/*CCP2 Data/Strobe mode*/
#define MAX_SPEED_CCP2_DATA_STB_MBPS                              (650)

/*CSI 1 Lane*/
#define MAX_SPEED_CSI_L1_MBPS                                     (680)

/*CSI 2 Lane*/
#define MAX_SPEED_CSI_L2_MBPS                                     (680 * 2)

/*Lens caliberation param*/

//Maximum number of times, lens movment is checked inside the FW
#define MAX_LENS_TIMER_RETRIES                                    (10)

//Time between each iteration - 100 usec
#define LENS_RETRIES_STEP_TIME                                    (LLA_IMX072_AF_ONE_STEP_TIME)

/*Mapped LLA assert to platform specific assert*/
#define LLA_ASSERT_XP70()                                         ASSERT_XP70()

/*Maximum number of bytes that can be read from EEPROM at one time*/
#define NVM_MAX_READ_BYTE                                         (12)

/*
Enum defining current state of selected camera module
*/
typedef enum
{
    CamDrvState_e_UnInitialize  = 0,            /*Un-initialized state*/
    CamDrvState_e_Initialize,
    CamDrvState_e_Off           = 1,            /* PM: Changed to satisy script 8500DocumentParameters.py */
    CamDrvState_e_Idle,
    CamDrvState_e_ViewFinder,
    CamDrvState_e_Capture                       /*Camera in capture mode*/
} CamDrvState_te;

/*
Enum defining current state of Flash
*/
typedef enum
{
    Flash_e_NotRequested        = 0,            /*Un-initialized state*/
    Flash_e_Requested,
    Flash_e_Fired,
    Flash_e_ReportFrameLit
} FlashState_te;

/*Enum defining current lens state*/
typedef enum
{
    CamLensState_e_UnInitalize  = 0,            /*Camera lens is in un-initializes state*/
    CamLensState_e_Idle,                        /*Camera lens is in idle state, not moving*/
    CamLensState_e_Moving,                      /*Camera lens is moving currently*/
    CamLensState_e_off,                         /*Camera Lens is in Off state*/
    CamLensState_e_Error                        /*Camera Lens is in error state, error occur either
                                                while moving or measuring lens position*/
} CamLensState_te;

typedef struct
{
    /// This is to be used to store the value of frame for which new update request has come
    uint32_t                                                      u32_FrameFSC_ID[MAX_FRAME_COUNT];

    ///counter to count the number of frames coming. Will be reset to zero when streaming is stopped.
    uint32_t                                                      u32_FSCCount;

    ///Frame number which will be having exposure sync for flash
    uint32_t                                                      u32_ExposureSyncFrame;

    ///This is to store the value of the current configuration which is to be send in the ISL of each Nth
    ///frame for which there is no update request in N-2 frame
    CAM_DRV_SENS_SETTINGS_T                                       FrameCurrentConfig;

    ///This is to store the feedback of the update (done in N th frame) which we have to send in the ISL of N+2 frame
    CAM_DRV_SENS_SETTINGS_T                                       FrameAppliedConfig[MAX_FRAME_COUNT];
}TimeModelConfig_ts;


/*Structure for camera driver current configuration*/
typedef struct
{
    /*Current sensor configuration*/
    CAM_DRV_SENS_RESOLUTION_T                                     camCurrWoiRes;
    CAM_DRV_SENS_RESOLUTION_T                                     camCurrOutputRes;
    CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T                       camCurrExpGain;
    uint16_t                                                      camCurrLineLenPck;
    uint16_t                                                      camCurrFrameLenLine;
} CamDrvCurrConfig_ts;

/*Common Macro defines*/

/** Structure used to configure/map sensor specific functions in the initialisation. */
typedef struct
{
    CAM_DRV_FN_RETVAL_E                                           (*FnApplyMiscSettings ) (CAM_DRV_SENS_ORIENTATION_T orientation) ;
    CAM_DRV_RETVAL_E                                              (*FnSensorInitSequence) (void);
    CAM_DRV_FN_RETVAL_E                                           (*FnModulePowerOn ) (const CAM_DRV_CONFIG_ON_T   *p_config_on) ;
    CAM_DRV_FN_RETVAL_E                                           (*FnModulePowerOff ) ( void ) ;
    CAM_DRV_FN_RETVAL_E                                           (*FnStartStopSensor) (uint8_t u8_Value);
    CAM_DRV_FN_RETVAL_E                                           (*FnGetCameraTypeDetail ) (CAM_DRV_CAMERA_TYPE_T * p_camera_type) ;
    CAM_DRV_RETVAL_E                                              (*FnGetCameraDetail) (CAM_DRV_CAMERA_DETAILS_T    *p_camera_details);
    uint8                                                         (*FnInterpretSensorSettings) (const uint8 *p_ancillary_lines, CAM_DRV_SENS_SETTINGS_T * p_sensor_settings);
    uint32_t                                                      (*FnComputeFrameTimeForExposure) (uint32_t exposure_time_us) ;
    CAM_DRV_RETVAL_E                                              (*FnUpdateTestPicture ) (CAM_DRV_SENS_CONFIG_TEST_PICTURE_T * p_config_test_picture) ;
    CAM_DRV_RETVAL_E                                              (*FnUpdateFlash) (CAM_DRV_CONFIG_FLASH_STROBES_T  *p_config_flash_strobe);
    CAM_DRV_RETVAL_E                                              (*FnTriggerFlash)(FlashState_te   *p_e_FlashState);
    CAM_DRV_FN_RETVAL_E                                           (*FnUpdateFrameRate) (uint16_t frameRate, uint8 trial_only) ;
    CAM_DRV_FN_RETVAL_E                                           (*FnUpdateGainAndExposure) (CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T * p_config_gain_and_exp_time, uint8 trial_only) ;
    CAM_DRV_FN_RETVAL_E                                           (*FnUpdateFrameDimensionParameters) (CAM_DRV_SENS_CONFIG_WOI_T   *p_config_woi, uint32 u32_MinReqLineLenPck, uint8 trial_only);
    CAM_DRV_RETVAL_E                                              (*FnUpdateFeedback) (CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback, uint8 trial_only) ;
    void                                                          (*FnGetCurrSensorConfig) (CamDrvCurrConfig_ts * pcurrConfig) ;
    CAM_DRV_FN_RETVAL_E                                           (*FnApplyFrameRateSettings) ( void ) ;
    CAM_DRV_RETVAL_E                                              (*FnApplyFrameDimensionSettings ) ( void ) ;
    CAM_DRV_RETVAL_E                                              (*FnApplyExposureAndGainSettings) ( void ) ;
    CAM_DRV_RETVAL_E                                              (*FnCheckModeConfigurations) (const CAM_DRV_CONFIG_ON_T   *p_config_on);
    CAM_DRV_RETVAL_E                                              (*FnGroupHoldRegSet) (void);
    CAM_DRV_RETVAL_E                                              (*FnGroupHoldRegReset) (void);
}
CAM_DRV_SENSOR_SPECIFIC_FN_INIT_ts;

/** Structure used to configure/map sensor specific functions in the initialisation. */
typedef struct
{
    //Pointer to hold AF Driver functiosn
    CAM_DRV_RETVAL_E                                              (*GetLensDetail) (CAM_DRV_LENS_DETAILS_T * p_lens_details);
    CAM_DRV_FN_RETVAL_E                                           (*AFDriver_Init) ( void );
    CAM_DRV_FN_RETVAL_E                                           (*AFDriver_MeasurePos) (uint16_t * u16_CurrentDac);
    CAM_DRV_RETVAL_E                                              (*LensMovToPos) (int32 position, uint32 * pu32_MoveTime_us);
}
CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts;

/** Structure used to store variable for flash status*/
typedef struct
{
    /// Minimum requested line length pck for current configuration
    uint8_t                                                       u8_FlashFrameCount;

    /// Status of the flash
    uint8_t                                                       u8_FrameLit;

    /// Status flag to store if flash request is to be delayed or not
    uint8_t                                                       u8_FlashWaitForExposure;

    /// Flash State Variable
    FlashState_te                                                 e_FlashState;
}FlashState_ts;

/*
structure holding the common (for both smia and non-smia sensor) global configuration
*/
typedef struct
{
    ///Callback APIs passed from client in init
    CAM_DRV_CONFIG_INIT_T                                         camDrvCallbackApis;
    CAM_DRV_SENSOR_SPECIFIC_FN_INIT_ts                            SensorCallbackFnApis;
    CAM_DRV_AF_DRIVER_SPECIFIC_FN_INIT_ts                         AFDriverCallbackFnApis;

    ///Current selected camera state
    CamDrvState_te                                                camDrvState;

    ///Current state of lens
    CamLensState_te                                               camLensState;

    ///TBD:<CN>:no-need to store
    CAM_DRV_SENS_OPERATING_VOLTAGES_T                             operatingVoltages;

    ///Stroing current DCC registers setings
    CAM_DRV_CAMERA_CONFIGURATION_DATA_T                           camDrvDccConfigData;

    ///Current Camera raw format - passed in VF and Capture API
    CAM_DRV_SENS_FORMAT_E                                         csiRawFormat;

    /// Struct to hold flash status
    FlashState_ts                                                 CamDrvFlashState;

    ///Passed in VF api
    uint8_t                                                       u8_Acfrequency;

    ///Selected camera lens detail
    CAM_DRV_LENS_DETAILS_T                                        camLensDetail;

    /*Few status variables*/
    /// To keep track whether sensor is streaming or not
    Flag_te                                                       flagIsStreaming;

    /// To check if streaming stopped by client using mode_control API
    Flag_te                                                       flagIsClientStoppedStreaming;

    /// To keep track, if the lens move timer is running
    Flag_te                                                       flagIsLensTimerRunning;

    ///Count to keep track of number of times lens timer started, after initial expiry
    uint8_t                                                       u8_LensRetries;
} CamDrvCommonGlbConfig_ts;


// TimeModel Struct Object
extern volatile TimeModelConfig_ts                                g_TimeModelConfig;

/*Extern declaration for global variables*/
extern CamDrvCommonGlbConfig_ts                                   g_CamDrvConfig;

/*Get the camra driver state*/
#define LLA_GetCamDrvState()                                      (g_CamDrvConfig.camDrvState)

/*sensor device I2C address*/
#define LLA_I2C_DEVICE_ADDRESS_SENSOR                             (0x34)

/*Sensor register address size*/
#define LLA_I2C_REG_ADDRESS_SIZE                                  CAM_DRV_I2C_REG_SIZE_16BIT

/*Byte order to be used for I2C read/write transaction*/
#define LLA_I2C_BYTE_ORDER                                        CAM_DRV_I2C_BYTE_ORDER_NORMAL

/*LLA I2C read interface*/
#define LLA_READ_I2C_BYTES_16BitRegSize(address, NoOfBytes, ptrBuffer)\
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn( \
            LLA_I2C_DEVICE_ADDRESS_SENSOR,                                      \
            address,                                                     \
            LLA_I2C_REG_ADDRESS_SIZE,                                    \
            NoOfBytes,                                                   \
            LLA_I2C_BYTE_ORDER,                                          \
            ptrBuffer)

/*LLA I2C write interface*/
#define LLA_WRITE_I2C_BYTES_16BitRegSize(address, NoOfBytes, ptrBuffer)\
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_write_fn( \
            LLA_I2C_DEVICE_ADDRESS_SENSOR,                                       \
            address,                                                      \
            LLA_I2C_REG_ADDRESS_SIZE,                                     \
            NoOfBytes,                                                    \
            LLA_I2C_BYTE_ORDER,                                           \
            ptrBuffer)

/*NVM Read interface*/
#define LLA_READ_NVM_BYTES(devAddr, regAddr, NoOfBytes, ptrBuffer)   \
        g_CamDrvConfig.camDrvCallbackApis.cam_drv_common_i2c_read_fn(   \
            devAddr,                                                    \
            regAddr,                                                    \
            CAM_DRV_I2C_REG_SIZE_8BIT,                                  \
            NoOfBytes,                                                  \
            CAM_DRV_I2C_BYTE_ORDER_NORMAL,                              \
            ptrBuffer)

void    LLA_blocking_delay (uint32 time_in_us);
#endif /*__LLA_COMMON_CONFIG_H__*/

