/*
* Copyright (C) ST-Ericsson 2010
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/

/*
lla_drv_imx072.h - header file for IMX072 5MP camera module
*/
#ifndef __LLA_DRV_IMX072_H__
#define __LLA_DRV_IMX072_H__

#   include "cam_drv.h"
#   include "lla_common_config.h" //to be removed
#   include "GenericFunctions.h"

#if (1 == LLA_INCLUDE_IMX072)

#   define LLA_ASSERT_LOG_EN 0

/*Idle register settings required for  IMX072 camera modure*/
extern uint8_t  g_IMX072_0x3015_regVal[];
extern uint16_t g_IMX072_MinFrameBlankingLine[];
extern uint8_t  g_u8GetRawNVMData;

/*XSHUTDOWN delay in number of EXTCLK cycle*/
#   define XshutdownDelay_extclk_start (2400)
#   define XshutdownDelay_extclk_stop  (512)

#      define LLA_SUPPORTED_OUTPUT_MODES 2             /**/

#      define EXT_SENSOR_CLOCK               960

/* resolution*/
#      define LLA_SENSOR_IMX072_FFOV_X   (2608)
#      define LLA_SENSOR_IMX072_FFOV_Y   (1960)

/* default values*/
#      define DEFAULT_PLL_STABLE_WAIT    (200)         /*in ns*/

#      define DEFAULT_VT_SYS_CLK_DIV     (1)

/*Register definitions*/
#   define SENSOR_VTIMING_LIMIT__MAX_X_OUTPUT_SIZE_HI  0x118C


// Default value requested by customer is 40 mm. Focal length in x100 units will be 40x100 : 4000
// Default value changed to 3.6 mm .  Focal length in x100 units will be 3.6x100 : 360 as suggested by (Nilsson, Rene )
#define FOCAL_LENGTH_DEFAULT_VALUE_IMX072_X100      (360)
// Request Fnumber is 2.6 i.e. in x100 units it is 260
#define FNUMBER_IMX072                              (260)

/* Value is 2 for RGPLTD register for its value as 1 Pg 90 - IMX072PQH5-C(E).pdf */
#      define DEFAULT_POST_PLL_CLK_DIV_REG_VAL   (2)
#      define DEFAULT_POST_PLL_CLK_DIV           (1)
#      define DEFAULT_PRE_PLL_CLK_DIV            (1)
#      define DEFAULT_ANC_LINES_TOP              (2)
#      define DEFAULT_DUMMY_PIXEL_TOP                 (0)   //Cnanda temp. change to 18, orig is 4
#      define DEFAULT_BLACK_PIXELS_TOP                (0)
#      define DEFAULT_BLACK_PIXELS_BOTTOM             (0)
#      define DEFAULT_DUMMY_PIXEL_BOTTOM              (0)
#      define DEFAULT_ANC_LINES_BOTTOM           (0)

/*minimum line length pck for IMX072 is different from that is read from sensor*/
#      define IMX072_MIN_LINE_LEN_PCK (2784)

//[CR - 445315]
//Mode specific minimum line blanking pck
#define     MIN_LINE_BLANKING_PCK_MODE0     168
#define     MIN_LINE_BLANKING_PCK_MODE1     80
#define     MIN_LINE_BLANKING_PCK_MODE2     168
#define     MIN_LINE_BLANKING_PCK_MODE3     168
#define     MIN_LINE_BLANKING_PCK_MODE4     168
#define     MIN_LINE_BLANKING_PCK_MODE5     168
#define     MIN_LINE_BLANKING_PCK_MODE6     168
#define     MIN_LINE_BLANKING_PCK_MODE7     168
#define     MIN_LINE_BLANKING_PCK_MODE8     168
#define     MIN_LINE_BLANKING_PCK_MODE9     168
#define     MIN_LINE_BLANKING_PCK_MODE10    168



//[CR - 445315]
//Mode specific minimum frame blanking lines
#define     MIN_FRAME_BLANKING_LINE_MODE0   46
#define     MIN_FRAME_BLANKING_LINE_MODE1   23
#define     MIN_FRAME_BLANKING_LINE_MODE2   46
#define     MIN_FRAME_BLANKING_LINE_MODE3   46
#define     MIN_FRAME_BLANKING_LINE_MODE4   46
#define     MIN_FRAME_BLANKING_LINE_MODE5   46
#define     MIN_FRAME_BLANKING_LINE_MODE6   46
#define     MIN_FRAME_BLANKING_LINE_MODE7   46
#define     MIN_FRAME_BLANKING_LINE_MODE8   46
#define     MIN_FRAME_BLANKING_LINE_MODE9   46
#define     MIN_FRAME_BLANKING_LINE_MODE10  46


//[CR - 445315]
//Mode specific extra lines
#define     EXTRA_LINES_MODE0       20   /*In All scan mode, extra lines  = 2 (Embedded line) + 18 (Dummy and OPB lines)*/
#define     EXTRA_LINES_MODE1       12   /*       2 (embedded lines) + 10 (Dummy and OPB lins)                                        */
#define     EXTRA_LINES_MODE2       20
#define     EXTRA_LINES_MODE3       20
#define     EXTRA_LINES_MODE4       20
#define     EXTRA_LINES_MODE5       20
#define     EXTRA_LINES_MODE6       20
#define     EXTRA_LINES_MODE7       20
#define     EXTRA_LINES_MODE8       20
#define     EXTRA_LINES_MODE9       20
#define     EXTRA_LINES_MODE10      20


/* Maximum recomended speed for CCP*/
/* We use max frequency per lane as 648 when pixl clock is set at 129.6 for 2 datalanes from datasheet pg.no :91*/
#      define MAX_CSI_PLL_OUT_FREQ_IMX072_RAW10   (648.0)
#      define MAX_CSI_PLL_SDL_FREQ_IMX072_RAW10   (999.0)
#      define MAX_CSI_PLL_OUT_FREQ_IMX072_RAW8    (520.0)
#      define MIN_CSI_PLL_OUT_FREQ_IMX072         (432.0)

#      define IMX072_EXP_OFFSET_WITH_VA   (0.13)
#      define IMX072_EXP_OFFSET_WO_VA     (0.27)
#      define IMX072_CSI_PKT_HDR_BYTE     (4)
#      define IMX072_PKT_FTR_BYTE         (4)
#      define IMX072_MIN_COARSE_INT_LINE  (600)


//# define LLA_IMX072_NVM_SIZE 0x7FF /* NVM size in bytes */
# define LLA_IMX072_NVM_SIZE 0x800 /* NVM size in bytes */

/* NVM Data Addresses */
#      define LLA_IMX072_ADDR_WB_TEMP_COUNT       0x03B
#      define LLA_IMX072_ADDR_LSC_TEMP_COUNT      0x03C
#      define LLA_IMX072_ADDR_DEFECT_PIXEL_COUNT  0x03D
#      define LLA_IMX072_ADDR_GbGr_CALIBRATION    0x03E
#      define LLA_IMX072_DATA_ADDR_DEFECT_PIXEL   0x404

/*NVM data addr for AF*/
#      define LLA_IMX072_AF_DATA_ADDR_INF     0x0D0 /*NVM location for Focus data at INF - 2byte*/
#      define LLA_IMX072_AF_DATA_ADDR_1M      0x0D2 /*NVM location for Focus data at 1m - 2byte*/
#      define LLA_IMX072_AF_DATA_ADDR_MACRO   0x0D4 /*NVM location for Focus data at Macro - 2byte*/
#      define LLA_IMX072_AF_RANG_ADDR_INF     0x0D8 /*NVM location for focus scan range of INF - 2byte*/
#      define LLA_IMX072_AF_RANG_ADDR_MACRO   0x0DC /*NVM location for focus scan range of Macro - 2byte*/

#      define LLA_IMX072_AF_STEP_WIDTH        0x10  /*Step width*/
#      define LLA_IMX072_AF_STEP_INTVAL       0x14  /*Step Interval*/
#      define LLA_IMX072_AF_ONE_STEP_TIME     (LLA_IMX072_AF_STEP_INTVAL * 50)  /*Time interval in one step time*/
#      define LLA_IMX072_AF_STEP_MUL          (1)                               /*Number of steps required will be multiplied by this factor*/
#      define LLA_IMX072_AF_DATA_POSN_COUNT   (3)

#      define LLA_IMX072_AF_SlewRate_Threshold    0x0038  /*SlewRate Threshold value(DAC)*/
#      define LLA_IMX072_AF_SlewRate_Low_TIME     0x0C80  /*Low speed value*/
#      define LLA_IMX072_AF_SlewRate_High_TIME    0x1900  /*High speed value*/

/* Is Gb/Gr Calibration present in the NVM */
#      define LLA_IMX072_GbGr_NOT_CALIBRATED  0x0
#      define LLA_IMX072_GbGr_CALIBRATED      0x1

/* Number of channels calibrated */
#      define LLA_IMX072_RGBGrGb_CALIBRATED               0x4                   /* Number of channels calibrated is 4 (R, G, B, Gb/Gr) */
#      define LLA_IMX072_RGB_CALIBRATED                   0x3                   /* Number of channels calibrated is 3 (R, G, B) */
#      define LLA_IMX072_LSC_LENS_POSN_COUNT              0x1                   /* Number of lens positions for which data is present */
#      define LLA_IMX072_INVALID_CHANNEL_COUNT_CALIBRATED 0x0                   /* Invalid channel count in NVM */

/* Number of Color Temperatures Calibrated */
#      define LLA_IMX072_MIN_COLOR_TEMP_CALIBRATED        0x1                   /* High Temperatures only */
#      define LLA_IMX072_MAX_COLOR_TEMP_CALIBRATED        0x3                   /* High, Low and Flouroscent Temperatures Calibrated */
#      define LLA_IMX072_INVALID_COLOR_TEMP_CALIBRATED    0x0

/* Defect data Counts */
#      define LLA_IMX072_MIN_DEFECT_PIXEL_COUNT       0x0
#      define LLA_IMX072_MAX_DEFECT_PIXEL_COUNT       0xFF
#      define LLA_IMX072_INVALID_DEFECT_PIXEL_COUNT   0x0

/*NVM data addr for WB/Sensitivity data */
#      define LLA_IMX072_WB_DATA_ADDR_HIGH_RED_GREEN          0x070
#      define LLA_IMX072_WB_DATA_ADDR_HIGH_BLUE_GREEN         0x074
#      define LLA_IMX072_WB_DATA_ADDR_HIGH_Gb_Gr              0x0BC

#      define LLA_IMX072_WB_DATA_ADDR_LOW_RED_GREEN           0x078
#      define LLA_IMX072_WB_DATA_ADDR_LOW_BLUE_GREEN          0x07C
#      define LLA_IMX072_WB_DATA_ADDR_LOW_Gb_Gr               0x0C0

#      define LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_RED_GREEN   0x098
#      define LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_BLUE_GREEN  0x09C
#      define LLA_IMX072_WB_DATA_ADDR_FlUORESCENT_Gb_Gr       0x0C4

/*NVM data addr for LSC*/

/* High Color */
#      define LLA_IMX072_LSC_DATA_ADDR_HIGH_RED   0x100
#      define LLA_IMX072_LSC_DATA_ADDR_HIGH_GREEN 0x140
#      define LLA_IMX072_LSC_DATA_ADDR_HIGH_BLUE  0x180
#      define LLA_IMX072_LSC_DATA_ADDR_HIGH_GbGr  0x340

/* Low Color */
#      define LLA_IMX072_LSC_DATA_ADDR_LOW_RED    0x1C0
#      define LLA_IMX072_LSC_DATA_ADDR_LOW_GREEN  0x200
#      define LLA_IMX072_LSC_DATA_ADDR_LOW_BLUE   0x240
#      define LLA_IMX072_LSC_DATA_ADDR_LOW_GbGr   0x380

/* Low Color */
#      define LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_RED    0x280
#      define LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_GREEN  0x2C0
#      define LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_BLUE   0x300
#      define LLA_IMX072_LSC_DATA_ADDR_FlUORESCENT_GbGr   0x3C0

/* CIE 1931 X,Y Co-ordinate values for High Temperature (Approximately 5000K)*/
#      define LLA_IMX072_CIE1931_X_HIGH_TEMP  0.3457
#      define LLA_IMX072_CIE1931_Y_HIGH_TEMP  0.3586

/* CIE 1931 X,Y Co-ordinate values for Low Temperature (Approximately 2800K)*/
#      define LLA_IMX072_CIE1931_X_LOW_TEMP   0.4709
#      define LLA_IMX072_CIE1931_Y_LOW_TEMP   0.4463

/* CIE 1931 X,Y Co-ordinate values for Fluorescent Temperature (Approximately 3500K)*/
#      define LLA_IMX072_CIE1931_X_FlUORESCENT_TEMP   0.4190
#      define LLA_IMX072_CIE1931_Y_FlUORESCENT_TEMP   0.4250

/* LSC related Information */
#      define LLA_IMX072_LSC_GRID_HORIZ_COUNT     9                             //EEPROM Map document Page 18
#      define LLA_IMX072_LSC_GRID_VERTI_COUNT     7                             //EEPROM Map document Page 18
#      define LLA_IMX072_LSC_SRC_DECIMAL_DIGITS   7
#      define LLA_IMX072_LSC_SRC_UNITY_I          (1 << LLA_IMX072_LSC_SRC_DECIMAL_DIGITS)
#      define LLA_IMX072_LSC_SRC_UNITY_F          ((float_t) LLA_IMX072_LSC_SRC_UNITY_I)
#      define LLA_IMX072_LSC_DST_ABS_BIT_DEPTH    16                            // no real need for these specific values, they just need to be different in order to  tell the SMIA++ NVM data user that
#      define LLA_IMX072_LSC_DST_REL_BIT_DEPTH    14                            // "relative method" is used for LSC data; other_bit_depth will anyway be used in order to correctly intrerpret the relative values
#      define LLA_IMX072_LSC_DST_REL_UNITY_I      (1 << LLA_IMX072_LSC_DST_REL_BIT_DEPTH)

/* sensitivity related Information */
#      define LLA_IMX072_SENS_SRC_DECIMAL_DIGITS  24
#      define LLA_IMX072_SENS_SRC_UNITY_I         (1 << LLA_IMX072_SENS_SRC_DECIMAL_DIGITS)
#      define LLA_IMX072_SENS_SRC_UNITY_F         ((float_t) LLA_IMX072_SENS_SRC_UNITY_I)
#      define LLA_IMX072_SENS_DST_REFERENCE       (1 << 14)

/* Zoom Related */
#      define LLA_IMX072_DEFAULT_ZOOM_FACTOR      1

#      define LLA_IMX072_DEFAULT_OBJECT_DISTANCE  128

/*Model ID info for supported camera*/
#   define LLA_CAM_IMX072 0x0045

/*definitions*/
#   define LLA_PIXEL_CODE_EMBEDDED 1
#   define LLA_PIXEL_CODE_DUMMY    2
#   define LLA_PIXEL_CODE_BLACK    3
#   define LLA_PIXEL_CODE_DARK     4
#   define LLA_PIXEL_CODE_VISIBLE  5

/*SMIA standard, MIN X and MIN Y OP size*/
#   define LLA_MIN_X_SIZE              256
#   define LLA_MIN_Y_SIZE              192

#   define LLA_SYNC_CODE_WIDTH_BITS    32
#   define LLA_CHECKSUM_WIDTH_BITS     32

/*CSI*/
#   define LLA_SOL_SYNC_CODE_WIDTH_BITS    32
#   define LLA_EOL_SYNC_CODE_WIDTH_BITS    32


#   define LLA_DEF_GAIN_x1000           1000

/*Embedded data format*/
#   define LLA_EDL_FORMAT_CODE          0x0A
#   define LLA_EDL_CCI_MSB_TAG          0xAA
#   define LLA_EDL_CCI_LSB_TAG          0xA5
#   define LLA_EDL_INC_IDX_VLD_TAG      0x5A
#   define LLA_EDL_INC_IDX_INVLD_TAG    0x55
#   define LLA_EDL_END_DATA             0x07

/*Default values for solid color*/
#   define LLA_TEST_SOLID_COLOR_RED     0xA5
#   define LLA_TEST_SOLID_COLOR_GrR     0xA5
#   define LLA_TEST_SOLID_COLOR_BLU     0xA5
#   define LLA_TEST_SOLID_COLOR_GrB     0xA5



#   define LLA_FrameDimension_GetSensorConstantCols()                           \
        (                                                                       \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_left +  \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_right + \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_left +  \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_right   \
        )
#   define LLA_FrameDimension_GetSensorConstantRows()                            \
        (                                                                        \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_top +    \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.black_pixels_bottom + \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_top +    \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.dummy_pixels_bottom + \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_top +       \
            g_IMX072_CamGlbConfig.camDrvSensorDetails.camSensorDetail.frame_format.anc_lines_bottom      \
        )
/*IMX072 module does not support CCP2 as data outout interface, so compiling out support for CCP*/
#      undef LLA_INCLUDE_IMX072_CCP_SUPPORT



typedef struct
{
    float_t f_post_pll_clk_div_val;
    uint8_t u8_post_pll_clk_div_reg_val;
} PostPllClkDivMap_ts;

typedef struct
{
    /// post pll clock divider value
    float_t f_PostPllClkDiv;
} SpecificCamGlbConfig_ts;




typedef struct
{
    uint8_t u8_MakerCode[3];
    uint8_t u8_PixelNumber[2];
    uint8_t u8_OutputDataFmt;
    uint8_t u8_OutputIntf;
    uint8_t u8_ModuleNum;
    uint8_t u8_ModulePrdSerial[8];
} sModuleInfoRegion_ts;

/*Structure for holding registers parsed from ISL dump*/
typedef struct
{
    uint8_t     frameCount; /*Frame count register parsed from EDL*/
    uint16_t    fineIntgPixel;
    uint16_t    coarseIntgLines;
    uint16_t    analogGainCode;
    uint16_t      digitalGainGreenR;
    uint16_t    frameLenLines;
    uint16_t    frameLineLenPck;
}CamISLReg_ts;

/*structure for holding analog gain constraints (capability) of sensor*/
typedef struct
{
    uint16_t    analogue_gain_type;
    int16_t     analogue_gain_m0;
    int16_t     analogue_gain_c0;
    int16_t     analogue_gain_m1;   /*signed integer*/
    int16_t     analogue_gain_c1;
    uint16_t    analogue_gain_min;
    uint16_t    analogue_gain_max;
    uint16_t    analogue_gain_step;
} CamAnalogGainCap_ts;

/*structure for holding gigital gain constraints (capability) of sensor*/
typedef struct
{
    uint16_t    digital_gain_cap;
    uint16_t    digital_gain_min;
    uint16_t    digital_gain_max;
    uint16_t    digital_gain_step;
} CamDigitalGainCap_ts;

/*Exposure capability od sensor*/
typedef struct
{
    uint16_t    u16_MinimumCoarseIntegrationLines;  /* Specifies the minimum coarse integration lines for the active sensor*/
    uint16_t    u16_CoarseIntegrationMaxMargin;     /* Specifies the SMIA coarse integration max margin for the active sensor */
    uint16_t    u16_MinimumFineIntegrationPixels;   /*Specifies the minimum fine integration pixels for the active sensor*/
    uint16_t    u16_FineIntegrationMaxMargin;       /*Specifies the fine integration max margin for the active sensor */
    uint8_t     e_IntegrationCapability;            /*Specifies the integration capability for the active sensor sensor*/
} CamExposureCap_ts;



/* Frame dimension constraints structure
*/
typedef struct
{
    /// Minimum value of X coordinate of the top left
        /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTXAddrMin;

    /// Minimum value of Y coordinate of the top left
        /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTYAddrMin;

    /// Maximum value of X coordinate of the bottom right
        /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTXAddrMax;

    /// Maximum value of Y coordinate of the bottom right
        /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTYAddrMax;

    /// Minimum OP X output size.
    uint16_t    u16_MinOPXOutputSize;

    /// Minimum OP Y output size.
    uint16_t    u16_MinOPYOutputSize;

    /// Maximum OP X output size. It is also interpreted as the
        /// X size of the input image for bayer memory load operations.
    uint16_t    u16_MaxOPXOutputSize;

    /// Maximum OP Y output size. It is also interpreted as the
        /// Y size of the input image for bayer memory load operations.
    uint16_t    u16_MaxOPYOutputSize;


    /// Minimum value of video timing frame length (in lines).
    uint16_t    u16_MinVTFrameLengthLines;

    /// Maximum value of video timing frame length (in lines).
    uint16_t    u16_MaxVTFrameLengthLines;
    /// Minimum value of video timing line length (in pixel clocks).
    uint16_t    u16_MinVTLineLengthPck;
    /// Maximum value of video timing line length (in pixel clocks).
    uint16_t    u16_MaxVTLineLengthPck;
    /// Minimum value of video timing line blanking (in pixel clocks).
    uint16_t    u16_MinVTLineBlankingPck;

    /// Minimum value of video timing frame blanking (in lines).
    uint16_t    u16_MinVTFrameBlanking;

    /// Minimum value of scaler_m register.
    uint16_t    u16_ScalerMMin;

    /// Maximum value of scaler_m register.
    uint16_t    u16_ScalerMMax;

    /// Maximum value of odd inc value.
    uint16_t    u16_MaxOddInc;

    /// Maximum value of even inc value.
    uint16_t    u16_MaxEvenInc;

    /// Specifies the sensor scaling mode (None, Horizonatal
        /// only and Full).
    uint8_t     e_SensorProfile;

} CamFrameDimensionConstraints_ts;

/**
\struct LLA_FrameDimension_ts
\brief Specifies the various parameters of the frame dimension.
\ingroup Frame Dimension
*/
typedef struct
{
   /// Videotiming frame length in lines.
    uint16_t    u16_VTFrameLengthLines;

    /// Videotiming line length in pixel clocks.
    uint16_t    u16_VTLineLengthPck;

    /// X co-ordinate of the top left corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTXAddrStart;

    /// Y co-ordinate of the top left corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTYAddrStart;

    /// X co-ordinate of the bottom right corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTXAddrEnd;

    /// Y co-ordinate of the bottom right corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTYAddrEnd;

    /// Horizontal size of Window Of Interest in output timing domain.
    uint16_t    u16_OPXSize;

    /// Vertical size of Window Of Interest in output timing domain.
    uint16_t    u16_OPYSize;

    /// Horizontal size of Window Of Interest in video timing domain.
    uint16_t    u16_WOIXSize;

    /// Vertical size of Window Of Interest in video timing domain.
    uint16_t    u16_WOIYSize;

    /// x_odd_inc value corresponding to the horizontal subsampling factor.
        /// x_even_inc == 1 always.
    uint16_t    u16_XOddInc ;

    /// y_odd_inc value corresponding to the vertical subsampling factor.
        /// y_even_inc == 1 always.\n
    uint16_t    u16_YOddInc;

     ///Max Line length in current Configuration
    uint16_t    u16_MaxLineLenPckInCurrConf;

    /// Specifies the scale_m factor with respect to the current f_ScaleFactor.
    uint16_t    u16_Scaler_M;

    /// f_XScaleFactor for given configuration
    float_t     f_XScaleFactor;

    /// f_YScaleFactor for given configuration
    float_t     f_YScaleFactor;

    /// frame_length_us
    float_t     f_FrameLength_us;

    ///flicker_period_us
    float_t     f_FlickerPeriod_us;

} CamFrameDimension_ts;




/*Current Exposure and Gain control parameters*/
typedef struct
{
    uint16_t    u16_CurrAnalogGainCode;
    uint16_t    u16_CurrCoarseIntegrationLines;
    uint16_t    u16_CurrFineIntegrationPixels;
    uint16_t    u16_CurrDigitalGainCode;
} CamSensorGainControl_ts;

typedef struct
{
    CAM_DRV_SENS_DETAILS_T                                     camSensorDetail;                /*Selected camera sensor detail*/
    CAM_DRV_SHUTTER_DETAILS_T                                camShutterDetail;               /*Selected camera shutter detail*/
    CAM_DRV_ND_DETAILS_T                                         camNDFilterDetail;              /*Selected camera ND filter detail*/
    CAM_DRV_NVM_DETAILS_T                                       camNVMDetail;                   /*Selected camera NVM Detail*/
    CAM_DRV_APERTURE_DETAILS_T                  camDrvApertureDetail;           /*Selected camera aperture detail*/
    CAM_DRV_FLASH_STROBE_DETAILS_T              camDrvFlashDetail;              /*Selected camera flash detail*/
}CamDrvSensorDetails_ts;



typedef struct
{
    /*Reg parsed from ISL*/
    CamISLReg_ts                                                  g_CamISLReg;

    CAM_DRV_SENS_FORMAT_E                                         csiRawFormat;

    uint16_t                                                      u16_CCP2SpeedMaxMbps;           /**< Maximum CCP2 speed supported by the client in Mbps */

    uint8_t                                                                         u8_NoCSILane;                   /**< Number of lanes used in case of CSI-2 image data interface */

    uint8_t                                                       u8_ChannelIdentifier;           /*Channel identifier*/

    /* Var to store FD Mode selected */
    uint8                                                         u8_FDMModeSelected;

   /*Var to store FD Output Mode selected */	
    uint8										                  u8_FDMOutputModeSelected;

   

    /*if we need to add more PLL settings, add similar structures*/
    uint8                                                         u8_ProfileSelected;

    uint8_t                                                       u8_Acfrequency;                 /*Passed in VF api*/

    CAM_DRV_SELECT_CAMERA_E                                       selectedCamera;                 /*Selected camera module*/
}CamDrvMiscDetails_ts;



/*Global structure holding sensor configurations*/
typedef struct
{
    /*Sensor capability*/
    CamAnalogGainCap_ts                                                 analogGainCap;      /*Structure holding analog gain capability of the sensor*/
    CamDigitalGainCap_ts                                                  digitalGainCap;     /*Structure holding digital gain capability of the sensor*/
    CamFrameDimensionConstraints_ts                                    frameDimensionCap;  /*Structure holding frame dimension capability/constraints of SMIA sensor*/
    CamExposureCap_ts                                                    exposureCap;        /*Exposure/Integration capability of SMIa sensor*/

    /*Sensor current configurations*/
    CamFrameDimension_ts                                                currFrameDimensionParam;    /*Current FD configurations*/
    CamSensorGainControl_ts                                             currGainConfigParam;        /*Current gain configuration param*/

    /*Local structs to hold data temp for Trial Mode calculations*/
    CamFrameDimension_ts                                                 FrameDimensionParam_TrialMode;   //Structs to hold parameter values incase of Trial Mode
   // CamVideoTimingOutput_ts                                              currVideoTimingParam;            /*Current VT configurations*/
    CamSensorGainControl_ts                                              SensorGainControl_TrialMode;     //Structs to hold parameter values incase of Trial Mode

    /*Sensor Specific API's*/
    CamDrvSensorDetails_ts                                                camDrvSensorDetails;
    CamDrvMiscDetails_ts                                                  camDrvMiscDetails;
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_OPTIONS_E                           dataInterfaceMode;              /*CCP2 or CSI2 image data interface*/
} CamGlbConfig_ts;


/*Global Structure Sensor Configurations (PLL and FDM)*/
typedef struct
{
    CAM_DRV_SENS_RESOLUTION_T   woi_res;                    /**< WOI resolution of the mode */
    CAM_DRV_SENS_RESOLUTION_T   output_res;                 /**< Output resolution of the mode */
	
      /// vt_pix_clk_freq_mhz
    float_t     f_VTPixelClockFrequency_Mhz;

    /// op_pix_clk_freq_mhz
    float_t     f_OPPixelClockFrequency_Mhz;

    /// op_sys_clk_freq_mhz: Output system clock Frequency. It is same or less than the host specified data rate in video timing inputs
    float_t     f_OPSystemClockFrequency_Mhz;

    /// vt_pix_clk_period_us
    float_t     f_VTPixelClockPeriod_us;

    /// Mode specific value for minimum line lenght. Initialize it with 0 incase if sensor does not imposes such condition.
    uint16     u16_MiniLineLengthModeSpecific;

    /// PLL Mode. If in some PLL depends on Mode selected, then PLL will be applied depending on mode selected.
    const CAM_DRV_CAMERA_CONFIGS_T    *   p_PllModes;

    /// Sensor Mode configurations
    const CAM_DRV_CAMERA_CONFIGS_T    *   p_FDModes;

    //[CR - 445315]
    uint16    u16_MinLineBlankingPck;         //value of minimum line blanking pck for each mode
    uint16    u16_MinFrameBlankingLines;       //value of minimum frame blanking line for each mode
    uint16    u16_ExtraLines;                  //value of extra lines for each mode.

} SensorModeSettings_ts;


typedef struct
{
    /// External clock frequency for selected camera
    uint32                                               u32_extClkFreqx100;

    /// Requested CSI2 link bit rate
    uint32                                               u32_CSI2LinkBitRate;

    ///CSI2 Data Lane Select
   CAM_DRV_SENS_CSI2_LANE_MODE_SELECT_E                  e_CSI2_lane_select ;

    /// Global PLL settings array.
    const CAM_DRV_CAMERA_CONFIGS_T                            *  p_GlobalPllModes;

    /// Pointer to configuration of 1 sensor mode
    const SensorModeSettings_ts                               **  p_SensorModeSetting;

} SensorSettings_ts;


/*Vertical sub-sampling mode*/
#      define IMX072_VMODE    e_IMX072_Vertical_Elimination

//SpecificCamGlbConfig_ts g_CamConfig;

/*Module information region map*/
#      define IMX072_NVM_MODULE_INFO_ADDR 0x000
#      define IMX072_NVM_MODULE_INFO_SIZE 0x10              /*Bytes*/

/*ModelId for different Manufact*/
#      define LLA_IMX072_MODEL_ID_STW '1'
#      define LLA_IMX072_MODEL_ID_KMO '2'
#      define LLA_IMX072_MODEL_ID_OTH '3'

/*Register default values*/
#      define MANU_OUTCHSINGLE_1CHANNEL   0xC0
#      define MANU_OUTCHSINGLE_2CHANNEL   0x40

/*Lane select*/
#      define MANU_1LANE_SECOND_LANE  0x2
#      define MANU_1LANE_FIRST_LANE   0x1
#      define MANU_2LANE_BOTH_LANE    0x0

/*Clock operation mode*/
#      define MANU_CLOCK_NORMAL_MODE          0x0
#      define MANU_CLOCK_CONST_OUTPUT_MODE    0x1

/*Sensor specific register definitions*/
#      define SENSOR_SETUP__MANU_OUTPUT_MODES 0x3000
#      define SENSOR_SETUP__MANU_OUTCHSINGLE  0x3017
#      define SENSOR_SETUP__MANU_PLSTATIM     0x302B
#      define SENSOR_SETUP__MANU_3015         0x3015
#      define SENSOR_SETUP__MANU_VMODEADD     0x3016
#      define SENSOR_SETUP__MANU_HMODEADD     0x30E8
#      define SENSOR_SETUP__MANU_RGPLTD       0x3022
#      define SENSOR_SETUP__MANU_RGCP         0x3025
#      define SENSOR_SETUP__MANU_RGLANESEL    0x3301

#      define REG_FLASH_CONTROL               0x301f
#      define REG_FLASH_PL_STEP               0x3020
#      define REG_TOUTSEL                     0x3090

#      define SDO_XVS_MONITOR_OUTPUT_ENABLE   1
#      define SDO_XVS_MONITOR_OUTPUT_DISABLE  0

// Choose between LED and Xenon FLash, zero value will mean Xenon Flash
#      define LLA_FLASH_LED           1

#      define ENABLE_FLASH_LED        5                     /* BIT 0 : LED_FLASH_EN
                                       BIT 2 : XVSFLASHSEL (Selects if (XVS) OR (Flash Strobe) is being output on XVS pin. 1 Selects Flash Strobe 0 selects XVS
                                       Enable BIT 0 and BIT 2 = (4 +1 )
                                    */
#      define ENABLE_FLASH_XENON      164                   /* Enable
                                       BIT 7 : FLASH_EN       enables flash strobe pulse generation
                                       BIT 5 : FLASH_REP      controls the flash strobe pulse repeat
                                       BIT 2 : XVSFLASHSEL    Controls the output to output pin, 1 selects flash. 0 selects xvs
                                       2pow7 + 2pow5 + 2pow2 = 164
                                    */

#      define DISABLE_FLASH_REG       4                     /* Leave the Flash register at default value
                                       Disable BIT 0,1,3,4,5,6,7 and enable BIT 2
                                       BIT 2 : XVSFLASHSEL (Selects if (XVS) OR (Flash Strobe) is being output on XVS pin. 1 Selects Flash Strobe, 0 selects XVS
                                    */

#      define DISABLE_FLASH_PULSE_REG 0

// IMX072 has sensor driven flash control
#         define FLASH_TYPE  (FLASHTYPE_SENSORDRIVEN)


/*Function prototype*/


void                    LLA_IMX072_InitializeCallbackFn(CAM_DRV_SENSOR_SPECIFIC_FN_INIT_ts  * p_CallbackFn) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_ApplyMiscSettings (CAM_DRV_SENS_ORIENTATION_T orientation) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_SensorInitSequence(void)TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_ModulePowerOn (const CAM_DRV_CONFIG_ON_T   *p_config_on) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_ModulePowerOff ( void ) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_StartStopSensor(uint8_t u8_Value)TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_GetCameraTypeDetail (CAM_DRV_CAMERA_TYPE_T * p_camera_type) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_GetCameraDetail(CAM_DRV_CAMERA_DETAILS_T    *p_camera_details)TO_EXT_PRGM_MEM;
uint8                   LLA_IMX072_InterpretSensorSettings(const uint8 *p_ancillary_lines, CAM_DRV_SENS_SETTINGS_T * p_sensor_settings)TO_EXT_PRGM_MEM;
uint32_t                LLA_IMX072_ComputeFrameTimeForExposure(uint32_t exposure_time_us) TO_EXT_PRGM_MEM;
void                    LLA_IMX072_GetCurrSensorConfig(CamDrvCurrConfig_ts * pcurrConfig) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_UpdateTestPicture (CAM_DRV_SENS_CONFIG_TEST_PICTURE_T * p_config_test_picture) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_UpdateFrameRate(uint16_t frameRate, uint8 trial_only) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_UpdateGainAndExposure(CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T * p_config_gain_and_exp_time,uint8 trial_only) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E     LLA_IMX072_UpdateFrameDimensionParameters(CAM_DRV_SENS_CONFIG_WOI_T   *p_config_woi, uint32 u32_MinReqLineLenPck, uint8 trial_only)TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_UpdateFeedback(CAM_DRV_CONFIG_FEEDBACK_T   *p_config_feedback, uint8 trial_only) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_UpdateFlash(CAM_DRV_CONFIG_FLASH_STROBES_T  *p_config_flash_strobe)TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_TriggerFlash(FlashState_te * p_e_FlashState) TO_EXT_PRGM_MEM;
CAM_DRV_FN_RETVAL_E        LLA_IMX072_ApplyFrameRateSettings( void ) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_ApplyFrameDimensionSettings ( void ) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_ApplyExposureAndGainSettings( void ) TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_IMX072_CheckModeConfigurations(const CAM_DRV_CONFIG_ON_T   *p_config_on)TO_EXT_PRGM_MEM;
CAM_DRV_RETVAL_E        LLA_NVMMAP_Read( uint16_t    addr,uint8_t     size,uint8_t     *u8_ptr);
CAM_DRV_RETVAL_E        LLA_IMX072_GetParsedNVM(CAM_DRV_NVM_T   *p_nvm);
CAM_DRV_RETVAL_E        LLA_IMX072_GetRawNVM(uint8   *p_nvm);
CAM_DRV_RETVAL_E        LLA_IMX072_GroupHoldRegSet(void);
CAM_DRV_RETVAL_E        LLA_IMX072_GroupHoldRegReset(void);


/* =================
    AF driver - APIs
    =================
*/
typedef struct
{
    uint16_t    u16_RngInf;
    uint16_t    u16_RngMacro;
} LLA_IMX072_AF_ScanRangs_ts;

/*IMX072 specific eeprom map for RAW export*/

/*

"Module Information Region"
0x0    - 0x7    : Module Name
0x8    - 0xF    : Module Product Serial
0x10  - 0x1F  : Version information
0x20  - 0x37  : Name information
0x38  - 0x39  : Lens driving method
0x3A  - 0x3A  : Mechanical shutter
0x3B  - 0x3B  : WB Calibration
0x3C  - 0x3C  : Shading Calibration
0x3D  - 0x3D  : Defect Pixel
0x3E  - 0x3E  : Gb/Gr
0x40  - 0x47  : Module production date
0x60  - 0x6B  : Checksum

"White Balance Calibration Region"
0x70  - 0x7F  : White balance calibration data
0x98  - 0x9F
0xBC  - 0xC7

"Mechatronics Region"
NO ADD: Calibrating condition
0xD0  - 0xD5  : Focus data
0xD8  - 0xD9  : AF scan range
0xDC  - 0xDD
0xE0  - 0xE5 : Optical zoom position
0xF0  - 0xF1  : Mechanical shutter delay time

"Lens Shading Correction Region"
0x100 - 0x3FF : Lens Shading Correction (Version 1)

"Defect Pixel Region"
0x400 - 0x400  : Defect Pixel (No of defect pixels)
0x404  - 0x7FF :                    (depends on No of defect pixels)

"Appendix"
0x48  - 0x4F : Module adjustment date

*/

#endif /*LLA_INCLUDE_IMX072*/
#endif /*!__LLA_DRV_IMX072_H__*/

