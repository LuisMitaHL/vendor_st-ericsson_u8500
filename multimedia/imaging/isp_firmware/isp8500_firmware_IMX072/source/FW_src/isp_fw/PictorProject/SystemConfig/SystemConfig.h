/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup SystemConfig System configuration
 \brief    System configuration specify no of active sensor in system, their frequencies and xshutdelays etc.
*/

/**
 \file SystemConfig.h
 \brief  This file is a part of the Pictor release code and provide
         information about no of sensors connected to system.
 \ingroup SystemConfig
*/
#ifndef _SYSTEM_CONFIG_H_
#   define _SYSTEM_CONFIG_H_

#   include "PictorhwReg.h"
#   include "smia_sensor_memorymap_defs.h"
#   include "MasterI2C_op_interface.h"
#   include "videotiming_op_interface.h"
#   include "RgbToYuvCoder_op_interface.h"
#   include "ColourMatrix_op_interface.h"
#   include "weighted_statistics_processor_op_interface.h"
#   include "channel_gains_op_interface.h"
#   include "exposure_statistics_op_interface.h"
#   include "EventManager.h"
#   include "DeviceParameter.h"
#   include "Duster.h"

typedef enum
{
    // Metering Mode is CONTINUOUS
    SystemMeteringMode_e_CONTINUOUS,

    // Metering Mode is SINGLE_STEP
    SystemMeteringMode_e_SINGLE_STEP
} SystemMeteringMode_te;

typedef enum
{
    // Input image source is sensor0
    InputImageSource_e_Sensor0,

    // Input image source is sensor1
    InputImageSource_e_Sensor1,

    // input image source is Rx test pattern
    InputImageSource_e_Rx,

    // Input image source is bayer load1
    InputImageSource_e_BayerLoad1,

    // Input image source is bayer load2
    InputImageSource_e_BayerLoad2,

    // Input image source is RGB load
    InputImageSource_e_RGBLoad
} InputImageSource_te;

typedef enum
{
    // Input image interface is CSI2_0
    InputImageInterface_CSI2_0,

    // Input image interface is CSI2_1
    InputImageInterface_CSI2_1,

    // Input image interface is CCP
    InputImageInterface_CCP,
} InputImageInterface_te;

typedef enum
{
    RxTestPattern_e_Normal = 0,

    // Colour Bars
    RxTestPattern_e_ColourBars          = 1,

    // Graduated colour bars
    RxTestPattern_e_GraduatedColourBars = 2,

    // Diagonal grey scale
    RxTestPattern_e_DiagonalGreyscale   = 3,

    // Psuedo random
    RxTestPattern_e_PsuedoRandom        = 4,

    // Horizontal grey scale
    RxTestPattern_e_HorizontalGreyscale = 5,

    // Vertical grey scale
    RxTestPattern_e_VerticalGreyscale   = 6,

    // Solid colour bars
    RxTestPattern_e_SolidColour         = 7
} RxTestPattern_te;

typedef enum
{
    // Sensor is CSI2 v0.81 complaint
    SensorCSI2Version_0_81              = 0,

    // Sensor is CSI2 v0.90 complaint
    SensorCSI2Version_0_90              = 1
} SensorCSI2Version_te;

typedef enum
{
    /// XP70 Idle Mode 0
    IdleMode_e_IDLE_MODE_0              = 0,

    /// XP70 Idle Mode 1
    IdleMode_e_IDLE_MODE_1              = 1,

    /// XP70 Idle Mode 2
    IdleMode_e_IDLE_MODE_2              = 2,

    /// XP70 Idle Mode 3
    IdleMode_e_IDLE_MODE_3              = 3,

    /// Normal Mode
    IdleMode_e_IDLE_MODE_NONE           = 0xFF
} IdleMode_te;


typedef enum
{
    /// NORMAL Mode: Grab OK/NOK will be notified based on absorption of AEC/flash params.
    GrabMode_e_NORMAL              = 0,

    /// Grab OK will be sent forcefully for every bms frame.
    GrabMode_e_FORCE_OK            = 1,

    /// Grab NOTOK will be sent forcefully for every bms frame.
    GrabMode_e_FORCE_NOK           = 2
} GrabMode_te;


/**
 \struct    SystemSetup_ts
 \brief     Holds system parameters that remain static for a given system input image source.
            They must be programmed before issuing a streaming command and not changed while streaming.
 \ingroup   SystemConfig
*/
typedef struct
{
    /// Frequency of input clock to the sensor
    float_t     f_SensorInputClockFreq_Mhz;

    /// Frequency of the input clock to the MCU
    float_t     f_MCUClockFreq_Mhz;

    /// Value of clk_host_ipp in Mhz
    /// It is the clock from which
    /// the STXP70 is clocked and the
    /// emulated sensor clocks are generated
    float_t     f_ClockHost_Mhz;

    /// Number of microseconds after which the sensor will output good
    /// clocks after given the start sensor command
    /// i.e., after how many us later does the sensor start(good clocks).\n
    float_t     f_SensorStartDelay_us;

    /// Sensor Device ID
    uint16_t    u16_SensorDeviceID;

    /// Number of microseconds the device must wait before attempting any Read/Write operation to Sensor0
    /// after asserting the XSHUTDOWN \n
    uint16_t    u16_SensorXshutdownDelay_us;

    /// Sensor device address type
    uint8_t     e_DeviceAddress_Type;

    /// Sensor device index type
    uint8_t     e_DeviceIndex_Type;

    /// Source of input image to the ISP
    uint8_t     e_InputImageSource;

    /// Specifies the input image interface (i.e. CSI0, CS1 or CCP)
    /// Only valid if e_InputImageSource is sensor0 or sensor1
    uint8_t     e_InputImageInterface;

    /// Specifies the number of CSI2 data lines
    /// Valid only if input image source is sensor and input
    /// image interface is CSI
    uint8_t     u8_NumberOfCSI2DataLines;

    /// Specifies the Rx test pattern type \n
    /// Valid only when e_InputImageSource ==  e_Rx
    uint8_t     e_RxTestPattern;

    /// Specifies if the IPP setup has to be done by the ISP firmware
    uint8_t     e_Flag_PerformIPPSetup;

    /// Specifies the number of frames to stream for the current operation. \n
    /// A value of 0 mean infinite streaming
    uint8_t     u8_FrameCount;

    /// Specifies the number of lines to wait after Rx STOP command to allow the ISP pipe to be flushed.
    uint8_t     u8_LinesToWaitAtRxStop;

    /// Specifies the ype of Metering the host want to use.CONTINUOUS or SINGLE_STEP.
    /// Default value is CONTINUOUS
    uint8_t     e_SystemMeteringMode_Control;

    /// Specifies whether Exposure Module is to be used in the metering process.
    /// Default value is FALSE
    uint8_t     e_Flag_InhibitExpMetering;

    /// Specifies whether Exposure Module is to be used in the metering process.
    /// Default value is FALSE
    uint8_t     e_Flag_InhibitWbMetering;

    /// Control coin to available to HOST for controlling the Exp & Wb functionality.
    /// Default value = Tails
    uint8_t     e_Coin_Ctrl;

    /// Image will be horizontally mirrored if set to Flag_e_True \n
    /// [DEFAULT]: Flag_e_False
    uint8_t     e_Flag_mirror;

    /// Image will be vertically flipped if set to Flag_e_True \n
    /// [DEFAULT]: Flag_e_False
    uint8_t     e_Flag_flip;

    /// Control flag to link glace and histogram statistics blocks.
    /// Default value = Tails
    uint8_t     e_Coin_Glace_Histogram_Ctrl_debug;

    /// XP70 Processor Idle Mode Control.
    /// Default value = IdleMode_e_IDLE_MODE_NONE
    uint8_t     e_IdleMode_Ctrl;

    /// u8_crm_clk_pip_in_div specify the value of divider to be used in BML phase.
    /// The value should be provided before BOOT command. Based on value explained below, ISP FW will generate BML clock from input clock.
    /// For example, if value = 0x8 is set and input clock is 200MHz, ISP FW will use 200 MHz as BML clock.
    /// [4:0] CLK_PIPE_IN_DIV:
    /// 5’b01_00_0 : 1           : 0x8
    /// 5’b01_00_1: 7/6 (1.16...): 0x9
    /// 5’b01_01_0: 8/6 (1.33...): 0xA
    /// 5’b01_10_0: 6/4 (1.5)    : 0xC
    /// 5’b01_10_1: 10/6 (1.66   : 0xD
    /// 5’b01_11_0: 7/4 (1.75)   : 0xE
    /// 5’b10_00_0: 2            : 0x10
    /// 5’b10_10_0: 10/4 (2.5)   : 0x14
    /// 5’b11_10_0: 14/4 (3.5)   : 0x1C
    uint8_t     u8_crm_clk_pip_in_div;

    /// smiaRx will be aborted on STOP command
    /// [DEFAULT]: Flag_e_FALSE
    /// Flag_e_TRUE  : smia-rx should abort when HOST sends STOP command
    /// Flag_e_FALSE : smia-rx should do stop when HOST sends STOP command
    uint8_t     e_Flag_abortRx_OnStop;

    /// Significant when there is a pending stats request, and the Host issues a STOP command. \n
    /// If TRUE, pending stats request will be cancelled before FW moves to STOPPED state. \n
    /// If FALSE, pending stats request will be completed, but notification will send to the Host in STOPPED state. \n
    /// [DEFAULT]: Flag_e_TRUE
    uint8_t     e_Flag_AecStatsCancel;

    /// start_grab_dly setting, i.e., num of frames to be skipped in still capture
    /// Default value = 0
    uint8_t     u8_NumOfFramesTobeSkipped;

    /// Send DMA Grab OK or NOTOK forcefully based on the value of this flag, otherwise
    /// based on the whether stats are valid for the frame.
    /// [Default] GrabMode_e_NORMAL
    uint8_t     e_GrabMode_Ctrl;

    /// This flag indicates if stats are collected on requested FOV without 
    /// actually applying zoom in ISP. This facilitates in doing zoom outside ISP
    /// and still collecting 3A stats on zoom FOV
    uint8_t     e_Flag_ZoomdStatsOnly;

    /// This flag indicates if BML is done for full BMS frame or for cropped BMS frame to reduce BML time
    uint8_t     e_Flag_FastBML;

    /// If this flag is set to true, it means host want"Valid Frame Event Notification" from firmware.
    /// If false, then host doesn't want the notification..
    /// The reason for adding this flag is to avoid unnecessary notifications sent by firmware to host for each frame
    /// which can lead to overflow of event queues.
    /// Ideally the value will be changed by HOST before streaming, but firmware will be able
    /// to manage the chnage while streaming also.
    /// [DEFAULT]: Flag_e_TRUE
    uint8_t     e_Flag_Request_ValidFrameNotification_Ctrl;

} SystemSetup_ts;


/**
\Struct
\brief This Strcuture is need to be configured in the Solid Color Test Pattern else not required
\Defalut Value is 0, 0, 0, 0
\in group SystemConfig

**/
typedef struct
{
    /// u16_SolidColor_data_red specify the Test Data Value for Red Pixel
    uint16_t  u16_SolidColor_data_red;

    /// u16_SolidColor_data_gir specify the Test Data Value for GREEN/RED Pixel (GIR)
    uint16_t  u16_SolidColor_data_gir;

    /// u16_SolidColor_data_blu specify the Test Data Value for Blue Pixel
    uint16_t  u16_SolidColor_data_blu;

    /// u16_SolidColor_data_gib specify the Test Data Value for GREEN/Blue Pixel (GIB)
    uint16_t  u16_SolidColor_data_gib;

}TestPattern_SolidColor_ts;

/**

\Struct
\brief This Strcuture helps in configuring the cursor height, width and positions on top of the image.
\Default Value 0, 0, 0, 0
\in group SystemConfig

**/

typedef struct
{
    /// Horizontal Cursor postion percentage is calculated with Width of the Frame to get the Horizontal pixel Position.
    /// The Value Should be in Between 0 ~ 100%
    /// Calculation :- Horizontal Pixel Position = (Width of the Frame *  u8_hcur_posn_per)/100
    uint8_t  u8_hcur_posn_per;

    /// Horizontal Cursor Width in Pixels
    uint8_t  u8_hcur_width; // Horizontal Cursor width in pixels

    /// Vertical Cursor postion percentage is calculated with Height of the Frame to get the Vertical pixel Position.
    /// The Value Should be in Between 0 ~ 100%
    /// Calculation :- Vertical Pixel Position = (Height of the Frame *  u8_vcur_posn_per)/100
    uint8_t  u8_vcur_posn_per;

    /// Vertical Cursor width in pixels
    uint8_t  u8_vcur_width;

}TestPattern_Cursors_ts;

/**
 \struct    CSIControl_ts
 \brief     Interface to allow the host to specify and override specific CSI2 parameters.
            They should be programmed before issuing a streaming command.
 \ingroup   SystemConfig
*/
typedef struct
{
    /// Specifies the data lanes mapping for CSI2_0 interface
    /// Needed only when the input image interface is InputImageInterface_CSI2_0
    uint16_t    u16_DataLanesMapCSI2_0;

    /// Specifies the data lanes mapping for CSI2_1 interface
    /// Needed only when the input image interface is InputImageInterface_CSI2_1
    uint16_t    u16_DataLanesMapCSI2_1;

    /// Specifies the DPHY0 clk control. Needed only when
    /// input image interface is InputImageInterface_CSI2_0
    /// Consists of bit fields to be interpreted as follows:
    /// u8_DPHY0Ctrl[0] : CSI0_SWAP_PIN_CL
    /// u8_DPHY0Ctrl[1] : CSI0_HS_INVERT_CL
    /// u8_DPHY0Ctrl[2] : CSI0_SWAP_PIN_DL1
    /// u8_DPHY0Ctrl[3] : CSI0_HS_INVERT_DL1
    /// u8_DPHY0Ctrl[4] : CSI0_SWAP_PIN_DL2
    /// u8_DPHY0Ctrl[5] : CSI0_HS_INVERT_DL2
    /// u8_DPHY0Ctrl[6] : CSI0_SWAP_PIN_DL3
    /// u8_DPHY0Ctrl[7] : CSI0_HS_INVERT_DL2
    uint8_t     u8_DPHY0Ctrl;

    /// Specifies the DPHY1 clk control. Needed only when
    /// input image interface is InputImageInterface_CSI2_1
    /// Consists of bit fields to be interpreted as follows:
    /// u8_DPHY1Ctrl[0] : CSI1_SWAP_PIN_CL
    /// u8_DPHY1Ctrl[1] : CSI1_HS_INVERT_CL
    /// u8_DPHY1Ctrl[2] : CSI1_SWAP_PIN_DL1
    /// u8_DPHY1Ctrl[3] : CSI1_HS_INVERT_DL1
    /// u8_DPHY1Ctrl[7:4] : reserved
    uint8_t     u8_DPHY1Ctrl;

    /// Specifies the sensor CSI2 version of sensor mounted on interface 0
    uint8_t     e_SensorCSI2Version_0;

    /// Specifies the sensor CSI2 version of sensor mounted on interface 1
    uint8_t     e_SensorCSI2Version_1;

    //Specifies the DPHY0 clk control only for DL4 (4TH DATALANE)
    /// u8_DPHY0Ctrl_4th_lane[0] : CSI0_SWAP_PIN_DL4
    /// u8_DPHY0Ctrl_4th_lane[1] : CSI0_HS_INVERT_DL4
    /// u8_DPHY1Ctrl[7:2] : reserved
    uint8_t u8_DPHY0Ctrl_4th_lane ;

} CSIControl_ts;

/**
 \struct    ColdStart_ts
 \brief     Used to start algorithm from cold start values
            They should be programmed only in stop state, The algorithm will start from cold start values after idle state.
 \ingroup   SystemConfig
*/
typedef struct
{
    /// Programme Control != Status, After white balance cold start is complete, Status will become equal to command
    uint8_t e_Coin_WhiteBalance;

    /// Programme Control != Status, After exposure cold start is complete, Status will become equal to command
    uint8_t e_Coin_Exposure;

    /// Programme Control != Status, After Frame rate cold start is complete, Status will become equal to command
    uint8_t e_Coin_FrameRate;
} ColdStart_ts;

typedef enum
{
    // Bayer store 2 soruce is duster output
    BayerStore2Source_e_DusterOutput,

    // Bayer store 2 source is bayer crop output
    BayerStore2Source_e_BayerCrop
} BayerStore2Source_te;

typedef enum
{
    /// RGB store source is babylon output
    RGBStoreSource_e_BabylonOutput,

/// RGB store source is channel-merge output
    RGBStoreSource_e_ChannelMergeOutput,

 /// RGB store source is Mozart output
     RGBStoreSource_e_MozartOutput
} RGBStoreSource_te;

/**
 \struct    DataPathSetup_ts
 \brief     Holds parameters that specify the output image data paths. Can be modified while streaming
 \ingroup   SystemConfig
*/
typedef struct
{
    /// Pipe0 output is to be enabled
    uint8_t e_Flag_Pipe0Enable;

    /// Pipe1 output is to be enabled
    uint8_t e_Flag_Pipe1Enable;

    /// Bayer store0 output is to be enabled
    uint8_t e_Flag_BayerStore0Enable;

    /// Bayer store1 output is to be enabled
    uint8_t e_Flag_BayerStore1Enable;

    /// Bayer store2 output is to be enabled
    uint8_t e_Flag_BayerStore2Enable;

    /// Bayer store2 source \n
    /// Valid only when e_Flag_BayerStore2Enable == Flag_e_TRUE
    uint8_t e_BayerStore2Source;

    /// RGB store output is to be enabled
    uint8_t e_Flag_RGBStoreEnable;

    /// RGB store source \n
    /// Valid only when e_Flag_RGBStoreEnable == Flag_e_TRUE
    uint8_t e_RGBStoreSource;

    /// Pipe0 output is to be enabled
    uint8_t e_Flag_PipeRAWEnable;

    /// Coin to update pipe dynamically
    uint8_t e_Coin_PipeEnable;
} DataPathSetup_ts;

typedef enum
{
    /// Peaked Red: 10, Green: 10, Blue: 10
    OutputFormat_e_RGB101010_PEAKED         = 0x10,

    /// UnPeaked Red: 10, Green: 10, Blue: 10
    OutputFormat_e_RGB101010_UNPEAKED       = 0x00,

    /// Peaked Red: 8, Green: 8, Blue: 8
    OutputFormat_e_RGB888_PEAKED            = 0x11,

    /// UnPeaked Red: 8, Green: 8, Blue: 8
    OutputFormat_e_RGB888_UNPEAKED          = 0x01,

    /// Peaked Red: 5, Green: 6, Blue: 5
    OutputFormat_e_RGB565_PEAKED            = 0x12,

    /// UnPeaked Red: 5, Green: 6, Blue: 5
    OutputFormat_e_RGB565_UNPEAKED          = 0x02,

    /// Peaked Red: 5, Green: 5, Blue: 5
    OutputFormat_e_RGB555_PEAKED            = 0x13,

    /// UnPeaked Red: 5, Green: 5, Blue: 5
    OutputFormat_e_RGB555_UNPEAKED          = 0x03,

    /// Unpacked, Peaked Red: 4, Green: 4, Blue: 4
    OutputFormat_e_RGB444_UNPACKED_PEAKED   = 0x14,

    /// Unpacked, UnPeaked Red: 4, Green: 4, Blue: 4
    OutputFormat_e_RGB444_UNPACKED_UNPEAKED = 0x04,

    /// Packed, Peaked Red: 4, Green: 4, Blue: 4
    OutputFormat_e_RGB444_PACKED_PEAKED     = 0x34,

    /// Packed, UnPeaked Red: 4, Green: 4, Blue: 4
    OutputFormat_e_RGB444_PACKED_UNPEAKED   = 0x24,

    /// YUV format
    OutputFormat_e_YUV                      = 0x05
} OutputFormat_te;

typedef enum
{
    //DMA Data Alignment in Memory
    //0 - DMAData 2Byte Aligned
    DMADataAccessAlignment_e_2Byte          = 0x0,

    //1 - DMAData 4Byte Aligned
    DMADataAccessAlignment_e_4Byte          = 0x1,

    //2-  DMAData 8Byte Aligned
    DMADataAccessAlignment_e_8Byte          = 0x2
} DMADataAccessAlignment_te;

typedef enum
{
    //DMA Data Bits in One Memory Word
    //0 - DMAData Each Pixel is taking 10 bits in Memory
    DMADataBitsInMemory_10                  = 0x0,

    //1 - DMAData Each Pixel is taking 12 bits in Memory
    DMADataBitsInMemory_12                  = 0x1,

    //2 - DMAData Each Pixel is taking 16 bits in Memory
    DMADataBitsInMemory_16                  = 0x2,
    //3 - DMAData Each Pixel is taking 16 bits in Memory	
	DMADataBitsInMemory_8                 = 0x3
} DMADataBitsInMemory_te;

/****************** DEFAULT PARAMETERS *********************/
#   define DEFAULT_EXTERNAL_CLOCK_FREQUENCY    12.0
#   define DEFAULT_SENSOR_ADDRESS              0x20
#   define DEFAULT_SENSOR_X_SHUTDOWN_DELAY_US  1000

/***********************************************************/

/* TODO: <AG>: Read H/W datasheet and put comments */
#   define DEFAULT_X_SIZE_PIPE             640
#   define DEFAULT_Y_SIZE_PIPE             480

#   define DEFAULT_PIXEL_VALID_LINE_TYPES  0x20

/**********************************************************/

/**
 \struct PipeSetup_ts
 \brief Control page elements for Pipe. All the page element are mode static in nature.
        All the page elements must be programmed before RUN command
 \ingroup SystemConfig
*/
typedef struct
{
    /// Output size X \n
    /// [Program before RUN] \n
    uint16_t    u16_X_size;

    /// Output size Y \n
    /// [Program before RUN] \n
    uint16_t    u16_Y_size;

    /// Output image format from Pipe
    uint8_t     e_OutputFormat_Pipe;

    /// Set to e_TRUE if toggle_pix_valid required \n
    /// [Program before RUN] \n
    /// [DEFAULT]:e_TRUE
    uint8_t     e_Flag_TogglePixValid;

    /// decides for which pixels, the pixvalid signal would be generated\n
    /// [Program before RUN] \n
    /// [6] eof_blank_only_en: Used to qualify blank lines at the end of frame (no other blank lines in frame are qualified).
    /// Bit2 of pixvalid_linetypes MUST be set if this register is to be used.\n\n
    /// [5:0] pixvalid_linetypes: Select line types for which pixvalid will be generated :\n
    /// bit0 - SOF \n
    /// bit1 - EOF \n
    /// bit2 - BLANK \n
    /// bit3 - BLACK \n
    /// bit4 - DARK \n
    /// bit5 - ACTIVE \n
    /// [DEFAULT]: 0x20 \n
    /// [NOTE]: ISP_OPFx_PIXVALID_LTYPES, Output format, p359, Pictor datasheet \n
    uint8_t     u8_PixValidLineTypes;

    /// Set to e_TRUE if Cb and Cr of the pipe have to be flipped.
    /// [NOTE]: Figure.32 Output format, p74, Pictor datasheet \n
    /// [DEFAULT]: e_FALSE
    uint8_t     e_Flag_Flip_Cb_Cr;

    /// Set to e_TRUE if  Y and CbCr of the pipe have to be flipped.
    /// [NOTE]: Figure.32 Output format, p74, Pictor datasheet \n
    /// [DEFAULT]: e_FALSE
    uint8_t     e_Flag_Flip_Y_CbCr;

    /// Set to e_TRUE if  above settings are valid.
    /// [NOTE]: Figure.32 Output format, p74, Pictor datasheet \n
    /// [DEFAULT]: e_FALSE
    uint8_t     e_Flag_Valid;
} PipeSetup_ts;

/**
 \struct PipeSetup_ts
 \brief Control page elements for Pipe. All the page element are mode static in nature.
        All the page elements must be programmed before RUN command
 \ingroup SystemConfig
*/
typedef struct
{
    /// Output size X \n
    /// The value strictly must be from one of the modes exported by LLCD
    /// [Program before RUN] \n
    uint16_t    u16_output_res_X_size;

    /// Output size Y \n
    /// The value strictly must be from one of the modes exported by LLCD
    /// [Program before RUN] \n
    uint16_t    u16_output_res_Y_size;

   /// Window of interest in X direction
   /// The value strictly must be from one of the modes exported by LLCD
    /// [Program before RUN] \n
   uint16_t    u16_woi_res_X;

   /// Window of interest in Y direction
   /// The value strictly must be from one of the modes exported by LLCD
    /// [Program before RUN] \n
   uint16_t    u16_woi_res_Y;

} PipeSetup_RAW_ts;

/**
 \struct DMASetup_ts
 \brief Control page elements for DMA. All the page element are mode static in nature.
        All the page elements must be programmed before RUN command
 \ingroup SystemConfig
*/
typedef struct
{
    /// DMA Data how many bits taken in one word of memory
    /// [Program before RUN] \n
    uint8_t e_DMADataBitsInMemory;

    /// Access to memory for DMA Data is 4Byte Aligned or 8Byte Aligned \n
    /// [Program before RUN] \n
    uint8_t e_DMADataAccessAlignment;
} DMASetup_ts;

/**
 \struct DMASetup_ts
 \brief Control page elements for DMA. All the page element are mode static in nature.
        All the page elements must be programmed before RUN command
 \ingroup SystemConfig
*/
typedef struct
{
    uint16_t    u16_DMACropValueRemainder_0;
    uint16_t    u16_DMACropValueRemainder_1;
    uint16_t    u16_MinIntegerInDMA_0;
    uint16_t    u16_MinIntegerInDMA_1;
    uint16_t    u16_HorizontalOffset;
    int16_t     s16_DMAStartDiff;
    uint8_t     u8_MinNoOfPixPickedInOneGoFromDma;
} DMAControl_ts;

/**
 \struct PipeStatus_ts
 \brief Status page element for the Pipe.
 \ingroup SystemConfig
*/
typedef struct
{
    /// Shows the number of frames streamed into the Pipe since last streaming command \n
    uint8_t u8_FramesStreamedInPipeLastRun;

    /// Shows the number of frames streamed by Pipe since last streaming command \n
    /// [NOTE]: Valid if Pipe is enabled
    // TODO: <AG>: check if we get VID if pipe is disabled
    uint8_t u8_FramesStreamedOutOfPipeLastRun;

    /// e_FALSE: If VID complete is not pending\n
    /// e_TRUE:  If VID complete is pending after status line interrupt
    uint8_t e_Flag_VideoCompleteInterruptPending;
    ///e_TRUE:If pipe resolution exceeds HW limit \n
    ///e_FALSE:If pipe resolution is within HW limit \n
    uint8_t e_Flag_Exceed_HW_Limitation;
} PipeStatus_ts;

typedef enum
{
    //GR BG
    PixelOrder_e_GrR_BGb                    = 0x00,

    //RG GB
    PixelOrder_e_RGr_GbB                    = 0x01,

    //BG GR
    PixelOrder_e_BGb_GrR                    = 0x02,

    //GB RG
    PixelOrder_e_GbB_RGr                    = 0x03,
} PixelOrder_te;

typedef enum
{
    //SFX Solarise Enable
    SFXSolariseControl_Enable,

    //SFX Solarise Disable
    SFXSolariseControl_Disable
} SFXSolarisControl_te;

typedef enum
{
    //SFX Negative Enable
    SFXNegativeControl_Enable,

    //SFX Negative Disable
    SFXNegativeControl_Disable
} SFXNegativeControl_te;

/// contains swapping status of three colour channels
typedef enum
{
    //BlackAndWhite Enable
    BlackAndWhiteControl_Enable,

    //BlackAndWhite Disable
    BlackAndWhiteControl_Disable
} BlackAndWhiteControl_te;

typedef enum
{
    //Sepia Enable
    SepiaControl_Enable,

    //Sepia Disable
    SepiaControl_Disable
} SepiaControl_te;

typedef struct
{
    //SFX Solaris Control
    uint8_t e_SFXSolarisControl;

    //SFX Negative Control
    uint8_t e_SFXNegativeControl;

    //BlackAndWhite Control
    // Not supported
    uint8_t e_BlackAndWhiteControl;

    //Sepia Control
    // Not supported
    uint8_t e_SepiaControl;
} SpecialEffects_Control_ts;

typedef struct
{
    /// Shows the status of Input Pipe Updation.
    /// e_FALSE: Not Updated.
    /// e_TRUE:  Updated.
    uint8_t e_Flag_InputPipeUpdated;

    /// This page element specifies whether the exposure parameters applied
    /// and the sensor sending the frame have the same parameters applied.
    /// When TRUE, it indicates that Exposure is in sync.
    uint8_t e_Flag_ExposureOutOfSync;

    /// Status coin for fw to know, when to apply the parameters programmed by HOST.
    /// Default value = Tails
    uint8_t e_Coin_Status;

    /// Flag to indicate that a zoom step was completed
    /// This information will be used by the SOF interrupt
    /// to setup its stats accordingly
    uint8_t e_Flag_ZoomUpdateDone;

    /// Image is horizontally mirrored if set to Flag_e_True \n
    /// [DEFAULT]: Flag_e_False
    uint8_t e_Flag_mirror;

    /// Image is vertically flipped if set to Flag_e_True \n
    /// [DEFAULT]: Flag_e_False
    uint8_t e_Flag_flip;

    ///  Flag to indicate that the notification of the update is pending to the Host
    uint8_t e_Flag_UpdateNotificationPending;

    /// Update status for the exposure cycle
    uint8_t e_UpdateStatus_Exposure;

    /// Update status of teh Frame Rate cycle
    uint8_t e_UpdateStatus_FrameRate;

    /// Update status of Flash
    uint8_t e_UpdateStatus_Flash;

    /// status coin for glace-histogram control
    uint8_t e_Coin_Glace_Histogram_Status;

    /// Update status of pixel order from Sensor Bayer pattern
    uint8_t e_PixelOrder;

    /// This flag is used to indicate to zoom manager that
    /// Zoom_Run() is to be called when any pipe is enabled on the fly
    uint8_t e_Flag_RunBestSensormode;

    uint8_t e_Flag_FireDummyVidComplete0;

    uint8_t e_Flag_FireDummyVidComplete1;

    // Update Status value of RxTest Pattern
    uint8_t e_RxTestPattern_Status;

    /// This flag indicates if stats are collected on requested FOV without 
    /// actually applying zoom in ISP. This facilitates in doing zoom outside ISP
    /// and still collecting 3A stats on zoom FOV
    uint8_t     e_Flag_ZoomdStatsOnly;

    /// This flag indicates if BML is done for full BMS frame or for cropped BMS frame to reduce BML time
    uint8_t     e_Flag_FastBML;

    /// Status coin for fw to know, what value of GrabMode is to be send to HOST for each frame.
    /// [Default] GrabMode_e_NORMAL
    uint8_t     e_GrabMode_Status;

    /// The flag will be used as a status flag for e_Flag_Request_ValidFrameNotification_Ctrl.
    /// The value of e_Flag_Request_ValidFrameNotification_Ctrl will be copied to e_Flag_Request_ValidFrameNotification_Status
    /// whenever System coin is toggled.
    /// [Default] Flag_e_TRUE
    uint8_t     e_Flag_Request_ValidFrameNotification_Status;

} SystemConfig_Status_ts;

typedef struct
{
    /// SMIARx_0 Interrupt Status
    uint8_t e_Flag_SMIARx_0;

    /// SMIARx_1 Interrupt Status
    uint8_t e_Flag_SMIARx_1;

    /// SMIARx_2 Interrupt Status
    uint8_t e_Flag_SMIARx_2;

    /// SMIARx_3 Interrupt Status
    uint8_t e_Flag_SMIARx_3;

    /// Video Complete Pipe0 Interrupt Status
    uint8_t e_Flag_VidComplete_Pipe0;

    /// Video Complete Pipe1 Interrupt Status
    uint8_t e_Flag_VidComplete_Pipe1;

    /// Glace Interrupt Status
    uint8_t e_Flag_Stats_Glace;

    /// Histo Interrupt Status
    uint8_t e_Flag_Stats_Histo;
} FrameInterrupts_Status_ts;

typedef enum
{
    /// No configuratino done
    GlaceHistogramStatus_None               = 0,

    ///  Glace configured
    GlaceHistogramStatus_GlaceDone          = 0x1,

    // Histogram configured
    GlaceHistogramStatus_HistogramDone      = 0x2
} GlaceHistogramStatus_te;

typedef enum
{
    /// Idle state
    UpdateStatus_e_Idle,

    /// Update Waiting
    UpdateStatus_e_Updating,

    /// Waiting for the Params to be applied on to the sensor
    UpdateStatus_e_WaitingForParams,

    /// attribute to indicate that the sensor is streaming with the values sent
    UpdateStatus_e_ParamsReceived,
} UpdateStatus_te;


typedef struct
{
    /// Coin to apply and verify Sensor Settings
    uint8_t     e_Coin_SensorSettings;

    /// Coin to apply and verify ISP Settings
    uint8_t     e_Coin_ISPSettings;

} Sensor_Pipe_Settings_ts;


// exported defines
#   define SystemConfig_SetExposureOutOfSync() (g_SystemConfig_Status.e_Flag_ExposureOutOfSync = Flag_e_TRUE)
#   define SystemConfig_SetExposureInSync()    (g_SystemConfig_Status.e_Flag_ExposureOutOfSync = Flag_e_FALSE)
#   define SystemConfig_SetZoomUpdateDone()    (g_SystemConfig_Status.e_Flag_ZoomUpdateDone = Flag_e_TRUE)
#   define SystemConfig_IsZoomUpdateDone()     (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_ZoomUpdateDone)
#   define SystemConfig_ResetZoomUpdateDone()  (g_SystemConfig_Status.e_Flag_ZoomUpdateDone = Flag_e_FALSE)

//#   define SystemConfig_DEFAULT_EXPOSURE_OUT_OF_SYNC    (Flag_e_TRUE)
// exported functions
extern void                                 RBM_SetupSensorStartOfFrameLineCount (void);

/************************ Exported Page elements *********************/
extern SystemSetup_ts                       g_SystemSetup;
extern CSIControl_ts                        g_CSIControl;
extern DataPathSetup_ts                     g_DataPathControl;
extern DataPathSetup_ts                     g_DataPathStatus;
extern PipeSetup_ts                         g_Pipe[];
extern volatile PipeStatus_ts               g_PipeStatus[];
extern SystemConfig_Status_ts               g_SystemConfig_Status;
extern DMASetup_ts                          g_DMASetup;
extern DMAControl_ts                        g_DMAControl;
extern SpecialEffects_Control_ts            g_SpecialEffects_Control[];
extern volatile FrameInterrupts_Status_ts   g_FrameInterrupts_Status;

extern PipeSetup_RAW_ts                     g_Pipe_RAW;

extern Sensor_Pipe_Settings_ts         g_SensorPipeSettings_Control;
extern Sensor_Pipe_Settings_ts         g_SensorPipeSettings_Status;

// Rx Test Pattern Page Elements.

extern TestPattern_SolidColor_ts g_testpattern_SolidColor_data;
extern TestPattern_Cursors_ts g_testpattern_Cursors_values;



typedef struct
{
    uint32_t    u32_DMA_GRAB_Indicator;                 // Variable
    uint32_t    u32_DMA_GRAB_Frame_Id;                  // Frame Id
    uint32_t    u32_DMA_GRAB_Indicator_For_VideoStab;   // video stab
    uint32_t    u32_DMA_GRAB_Abort;                      // Grab abort. // 0: Correct 1: Abort triggered UMANG
} DMA_GRAB_Indicator_Params_ts;

extern volatile DMA_GRAB_Indicator_Params_ts g_GrabNotify   TO_FIXED_DATA_ADDR_MEM;

extern uint8_t                                              SystemConfig_IsPipeIdle (uint8_t u8_PipeNumber);
extern void                                                 SystemConfig_EnablePipes (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 SMIA_LC0_ISR (void);
extern void                                                 SMIA_LC1_ISR (void);
extern void                                                 SMIA_ISPUpdate_ISR (void);
extern void                                                 SMIA_STOP_RX_ISR (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 SMIA_RX_ISR_FRAME_END (void);
extern void                                                 VideoComplete_Pipe0 (void);
extern void                                                 VideoComplete_Pipe1 (void);
extern void                                                 SystemConfig_StaticDataPathSetup (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 SystemConfig_StaticDataPathCommit ( void ) TO_EXT_DDR_PRGM_MEM;
extern void                                                 SystemConfig_RunTimeDataPathUpdate (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 SystemConfig_RunTimeDataPathCommit (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 SystemConfig_RunTimePipeEnable (void)TO_EXT_DDR_PRGM_MEM;
extern void                                                 DisableMemoryBasedHardware (void)TO_EXT_DDR_PRGM_MEM;
extern void                         SystemConfig_UpdateRxPattern (void)TO_EXT_DDR_PRGM_MEM;

/// Control page element for cold start
extern ColdStart_ts             g_ColdStartControl;

/// Status page element for cold start
extern ColdStart_ts             g_ColdStartStatus;

/**
    Exported globals
*/

/// Indicates whether glace/histo stats are frozen
extern volatile uint8_t         g_GlaceHistogramStatsFrozen;
/// AEC target state machine variables
extern volatile UpdateStatus_te g_SensorSettingStatus;
extern volatile Flag_te         g_isAecStatsCancelled;
/// SMIA Rx Frame Counter
extern volatile uint8_t         g_RxFrameCounter;


/**
    Input Image source defines
*/

/// Input image source is Sensor 0
#   define SystemConfig_IsInputImageSourceSensor0()    (InputImageSource_e_Sensor0 == g_SystemSetup.e_InputImageSource)

/// Input image source is Sensor 0
#   define SystemConfig_IsInputImageSourceSensor1()    (InputImageSource_e_Sensor1 == g_SystemSetup.e_InputImageSource)

/// Input image source is Sensor 0/1
#   define SystemConfig_IsInputImageSourceSensor() \
        (SystemConfig_IsInputImageSourceSensor0() || SystemConfig_IsInputImageSourceSensor1())

/// Input image source is Rx
#   define SystemConfig_IsInputImageSourceRx() (InputImageSource_e_Rx == g_SystemSetup.e_InputImageSource)

/// Input image source is in ISP control
#   define SystemConfig_IsInputImageSourceUnderISPControl() \
        (SystemConfig_IsInputImageSourceSensor() || SystemConfig_IsInputImageSourceRx())

/// Input image source is Bayer load 1
#   define SystemConfig_IsInputImageSourceBayerLoad1() (InputImageSource_e_BayerLoad1 == g_SystemSetup.e_InputImageSource)

/// Input image source is Bayer load 2
#   define SystemConfig_IsInputImageSourceBayerLoad2() (InputImageSource_e_BayerLoad2 == g_SystemSetup.e_InputImageSource)

/// Input image source is RGB load
#   define SystemConfig_IsInputImageSourceRGBLoad()    ((InputImageSource_e_RGBLoad == g_SystemSetup.e_InputImageSource))

/**
    Data store defines
*/
#   define SystemConfig_IsPipe0Active()            (Flag_e_TRUE == g_DataPathStatus.e_Flag_Pipe0Enable)
#   define SystemConfig_IsPipe1Active()            (Flag_e_TRUE == g_DataPathStatus.e_Flag_Pipe1Enable)
#   define SystemConfig_IsPipeRAWActive()          (Flag_e_TRUE == g_DataPathStatus.e_Flag_PipeRAWEnable)
#   define SystemConfig_IsAnyPipeActive()          (SystemConfig_IsPipe0Active() || SystemConfig_IsPipe1Active())
#   define SystemConfig_IsRGBStoreActive()         (Flag_e_TRUE == g_DataPathStatus.e_Flag_RGBStoreEnable)
#   define SystemConfig_IsRGBStoreSourceBabylon()  (RGBStoreSource_e_BabylonOutput == g_DataPathStatus.e_RGBStoreSource)
#   define SystemConfig_IsRGBStoreSourceMozart()  (RGBStoreSource_e_MozartOutput == g_DataPathStatus.e_RGBStoreSource)
#   define SystemConfig_IsRGBStoreSourceChannelMerge() (RGBStoreSource_e_ChannelMergeOutput == g_DataPathStatus.e_RGBStoreSource)
#   define SystemConfig_IsBayerStore2Active()  (Flag_e_TRUE == g_DataPathStatus.e_Flag_BayerStore2Enable)
#   define SystemConfig_IsBayerStore2SourceDuster() \
        (BayerStore2Source_e_DusterOutput == g_DataPathStatus.e_BayerStore2Source)
#   define SystemConfig_IsBayerStore1Active()  (Flag_e_TRUE == g_DataPathStatus.e_Flag_BayerStore1Enable)
#   define SystemConfig_IsBayerStore0Active()  (Flag_e_TRUE == g_DataPathStatus.e_Flag_BayerStore0Enable)
#   define SystemConfig_IsDMCE_Active()        (SystemConfig_IsAnyPipeActive() || SystemConfig_IsRGBStoreActive())
#   define SystemConfig_IsRE_Active()                                                                         \
        (                                                                                                     \
            SystemConfig_IsBayerStore2Active()                                                                \
        ||  (                                                                                                 \
                SystemConfig_IsDMCE_Active() && !                                                             \
                    (SystemConfig_IsInputImageSourceBayerLoad2() || SystemConfig_IsInputImageSourceRGBLoad()) \
            )                                                                                                 \
        )

#   define SystemConfig_IsRAWPipeActive()             ( SystemConfig_IsBayerStore0Active() || SystemConfig_IsBayerStore1Active() || SystemConfig_IsBayerStore2Active() || SystemConfig_IsRGBStoreActive())
#   define SystemConfig_IsSDPipeActive()              ((SystemConfig_IsInputImageSourceSensor() || SystemConfig_IsInputImageSourceRx()) && (!SystemConfig_IsBayerStore0Active()))
#   define SystemConfig_GetSensorInputClockFreq_Mhz()  READONLY(g_SystemSetup.f_SensorInputClockFreq_Mhz)
#   define SystemConfig_GetSensorDeviceID()            READONLY(g_SystemSetup.u16_SensorDeviceID)
#   define SystemConfig_GetSensorDeviceAddressType()   READONLY(g_SystemSetup.e_DeviceAddress_Type)
#   define SystemConfig_GetSensorDeviceIndexType()     READONLY(g_SystemSetup.e_DeviceIndex_Type)
#   define SystemConfig_GetPipe0OpSizeX()              READONLY(g_Pipe[0].u16_X_size)
#   define SystemConfig_GetPipe0OpSizeY()              READONLY(g_Pipe[0].u16_Y_size)
#   define SystemConfig_GetPipe1OpSizeX()              READONLY(g_Pipe[1].u16_X_size)
#   define SystemConfig_GetPipe1OpSizeY()              READONLY(g_Pipe[1].u16_Y_size)
#   define SystemConfig_GetPipeRAWOpSizeX()            READONLY(g_Pipe_RAW.u16_X_size)
#   define SystemConfig_GetPipeRAWOpSizeY()            READONLY(g_Pipe_RAW.u16_Y_size)
#   define SystemConfig_IsPipe0Idle()                  (Flag_e_TRUE == SystemConfig_IsPipeIdle(0))
#   define SystemConfig_IsPipe1Idle()                  (Flag_e_TRUE == SystemConfig_IsPipeIdle(1))
#   define SystemConfig_IsPipe0TogglePixValid()        (Flag_e_TRUE == g_Pipe[0].e_Flag_TogglePixValid)
#   define SystemConfig_IsPipe1TogglePixValid()        (Flag_e_TRUE == g_Pipe[1].e_Flag_TogglePixValid)
#   define SystemConfig_GetInputPipeStatus()           (g_SystemConfig_Status.e_Flag_InputPipeUpdated)
#   define SystemConfig_GetPipe0OutputFormat()         (g_Pipe[0].e_OutputFormat_Pipe & 0x0F)
#   define SystemConfig_GetPipe1OutputFormat()         (g_Pipe[1].e_OutputFormat_Pipe & 0x0F)
#   define SystemConfig_GetPipe0PixValidLineTypes()    (g_Pipe[0].u8_PixValidLineTypes & 0xFF)
#   define SystemConfig_GetPipe1PixValidLineTypes()    (g_Pipe[1].u8_PixValidLineTypes & 0xFF)
#   define SystemConfig_IsPipe0CbCrFlip()              (Flag_e_TRUE == g_Pipe[0].e_Flag_Flip_Cb_Cr)
#   define SystemConfig_IsPipe1CbCrFlip()              (Flag_e_TRUE == g_Pipe[1].e_Flag_Flip_Cb_Cr)
#   define SystemConfig_IsPipe0YCbCrFlip()             (Flag_e_TRUE == g_Pipe[0].e_Flag_Flip_Y_CbCr)
#   define SystemConfig_IsPipe1YCbCrFlip()             (Flag_e_TRUE == g_Pipe[1].e_Flag_Flip_Y_CbCr)
#   define SystemConfig_GetPipe0UpScaling()            (Flag_e_TRUE != g_Pipe_Scalar_Output[0].e_Flag_DownScaling)
#   define SystemConfig_GetPipe1UpScaling()            (Flag_e_TRUE != g_Pipe_Scalar_Output[1].e_Flag_DownScaling)
#   define SystemConfig_GetPipe0TopBorders()           (g_Pipe_Scalar_Output[0].u16_TopBorder == 0x3ff)
#   define SystemConfig_GetPipe1TopBorders()           (g_Pipe_Scalar_Output[1].u16_TopBorder == 0x3ff)
#   define SystemConfig_IsPipeOutputYUV(x)             (OutputFormat_e_YUV == g_Pipe[x].e_OutputFormat_Pipe)
#   define SystemConfig_IsPipeOutputPeaked(x)          (g_Pipe[x].e_OutputFormat_Pipe & 0x10)
#   define SystemConfig_IsPipeOutputUnPeaked(x)        (!SystemConfig_IsPipeOutputPeaked(x))
#   define SystemConfig_IsPipe0OuptutPacked()          (g_Pipe[0].e_OutputFormat_Pipe & 0x20)
#   define SystemConfig_IsPipe1OuptutPacked()          (g_Pipe[1].e_OutputFormat_Pipe & 0x20)
#   define SystemConfig_FramesStreamedOutOfPipe0()     (g_PipeStatus[0].u8_FramesStreamedOutOfPipeLastRun)
#   define SystemConfig_FramesStreamedOutOfPipe1()     (g_PipeStatus[1].u8_FramesStreamedOutOfPipeLastRun)
#   define SystemConfig_IsPipe0Valid()                 (SystemConfig_IsPipe0Active())
#   define SystemConfig_IsPipe1Valid()                 (SystemConfig_IsPipe1Active())
#   define SystemConfig_IsPipeRAWValid()               (SystemConfig_IsPipeRAWActive())
#   define Is_SOF_Arrived()                            (Flag_e_TRUE == g_FrameInterrupts_Status.e_Flag_SMIARx_0)
#   define Set_SOF_Arrived(x)                          g_FrameInterrupts_Status.e_Flag_SMIARx_0 = x
#   define IsBMS2Enabled()                             (Flag_e_TRUE == g_DataPathStatus.e_Flag_BayerStore2Enable)

/// Interface to specify if host wants the ISP firmware to perform the IPP setup or not
#   define SystemConfig_IPPSetupRequired() (Flag_e_TRUE == g_SystemSetup.e_Flag_PerformIPPSetup)

/// Interface to specify if the input image interface is CSI2 (interface0)
#   define SystemConfig_IsInputInterfaceCSI2_0()   (InputImageInterface_CSI2_0 == g_SystemSetup.e_InputImageInterface)

/// Interface to specify if the input image interface is CSI2 (interface1)
#   define SystemConfig_IsInputInterfaceCSI2_1()   (InputImageInterface_CSI2_1 == g_SystemSetup.e_InputImageInterface)

/// Interface to specify if the input image interface is CCP
#   define SystemConfig_IsInputInterfaceCCP()  (InputImageInterface_CCP == g_SystemSetup.e_InputImageInterface)

/// Interface to query the number of data lines on the CSI2 interface
/// Valid only when input image source is sensor and input image interface is
/// CSI2 interface0 (for CSI2 interface1, the number of data lines is 1 always)
#   define SystemConfig_GetNumberOfCSI2DataLines() READONLY(g_SystemSetup.u8_NumberOfCSI2DataLines)

/// Interface to query if the sensor is CSI2_0 is v0.90 compatible
#   define SystemConfig_IsSensorCSI2_0_0_90()  (SensorCSI2Version_0_90 == g_CSIControl.e_SensorCSI2Version_0)

/// Interface to query if the sensor is CSI2_1 is v0.90 compatible
#   define SystemConfig_IsSensorCSI2_1_0_90()  (SensorCSI2Version_0_90 == g_CSIControl.e_SensorCSI2Version_1)

/// Interface to query the data lanes map for CSI2_0 interface
#   define SystemConfig_GetDataLanesMapCSI2_0()    READONLY(g_CSIControl.u16_DataLanesMapCSI2_0)

/// Interface to query the data lanes map for CSI2_0 interface
#   define SystemConfig_GetDataLanesMapCSI2_1()    READONLY(g_CSIControl.u16_DataLanesMapCSI2_1)

/// Interface to query CSI0_SWAP_PIN_CL
#   define SystemConfig_GetSwapPinClCSI2_0()   ((g_CSIControl.u8_DPHY0Ctrl & 0x01)>>0)

/// Interface to query CSI0_HS_INVERT_CL
#   define SystemConfig_GetInvertClCSI2_0()    ((g_CSIControl.u8_DPHY0Ctrl & 0x02)>>1)

/// Interface to query CSI0_SWAP_PIN_DL1
#   define SystemConfig_GetSwapPinDl1CSI2_0()  ((g_CSIControl.u8_DPHY0Ctrl & 0x04)>>2)

/// Interface to query CSI0_HS_INVERT_DL1
#   define SystemConfig_GetInvertDl1CSI2_0()   ((g_CSIControl.u8_DPHY0Ctrl & 0x08)>>3)

/// Interface to query CSI0_SWAP_PIN_DL2
#   define SystemConfig_GetSwapPinDl2CSI2_0()  ((g_CSIControl.u8_DPHY0Ctrl & 0x10)>>4)

/// Interface to query CSI0_HS_INVERT_DL2
#   define SystemConfig_GetInvertDl2CSI2_0()   ((g_CSIControl.u8_DPHY0Ctrl & 0x20)>>5)

/// Interface to query CSI0_SWAP_PIN_DL3
#   define SystemConfig_GetSwapPinDl3CSI2_0()  ((g_CSIControl.u8_DPHY0Ctrl & 0x40)>>6)

/// Interface to query CSI0_HS_INVERT_DL2
#   define SystemConfig_GetInvertDl3CSI2_0()   ((g_CSIControl.u8_DPHY0Ctrl & 0x80)>>7)

/// Interface to query CSI1_SWAP_PIN_CL
#   define SystemConfig_GetSwapPinClCSI2_1()   ((g_CSIControl.u8_DPHY1Ctrl & 0x01)>>0)

/// Interface to query CSI1_HS_INVERT_CL
#   define SystemConfig_GetInvertClCSI2_1()    ((g_CSIControl.u8_DPHY1Ctrl & 0x02)>>1)

/// Interface to query CSI1_SWAP_PIN_DL1
#   define SystemConfig_GetSwapPinDl1CSI2_1()  ((g_CSIControl.u8_DPHY1Ctrl & 0x04)>>2)

/// Interface to query CSI1_HS_INVERT_DL1
#   define SystemConfig_GetInvertDl1CSI2_1()   ((g_CSIControl.u8_DPHY1Ctrl & 0x08)>>3)

//   For 4 Datalane Support
/// Interface to query CSI0_SWAP_PIN_DL4
#   define SystemConfig_GetSwapPinDl4CSI2_0()   ((g_CSIControl.u8_DPHY0Ctrl_4th_lane & 0x01)>>0)

/// Interface to query CSI0_HS_INVERT_DL3
#   define SystemConfig_GetInvertDl4CSI2_0()    ((g_CSIControl.u8_DPHY0Ctrl_4th_lane & 0x02)>>1)



/// Return true if White balance cold start is required
#   define SystemConfig_IsWhiteBalanceColdStartRequired() \
        (g_ColdStartStatus.e_Coin_WhiteBalance != g_ColdStartControl.e_Coin_WhiteBalance)

/// Set White balance cold start complete
#   define SystemConfig_SetWhiteBalanceColdStartComplete() \
        (g_ColdStartStatus.e_Coin_WhiteBalance = g_ColdStartControl.e_Coin_WhiteBalance)

/// Return true if Exposure cold start is required
#   define SystemConfig_IsExposureColdStartRequired() \
        (g_ColdStartStatus.e_Coin_Exposure != g_ColdStartControl.e_Coin_Exposure)

/// Set exposure cold start complete
#   define SystemConfig_SetExposureColdStartComplete() \
        (g_ColdStartStatus.e_Coin_Exposure = g_ColdStartControl.e_Coin_Exposure)

/// Return true if Frame rate cold start is required
#   define SystemConfig_IsFrameRateColdStartRequired() \
        (g_ColdStartStatus.e_Coin_FrameRate != g_ColdStartControl.e_Coin_FrameRate)

/// Set Frame rate cold start complete
#   define SystemConfig_SetFrameRateColdStartComplete() \
        (g_ColdStartStatus.e_Coin_FrameRate = g_ColdStartControl.e_Coin_FrameRate)

#   define SystemConfig_IsSensorSettingStatusIdle() \
        (UpdateStatus_e_Idle == g_SensorSettingStatus)

#   define SystemConfig_IsSensorSettingStatusUpdating() \
        (UpdateStatus_e_Updating == g_SensorSettingStatus)

#   define SystemConfig_IsSensorSettingStatusWaitingForParams() \
        (UpdateStatus_e_WaitingForParams == g_SensorSettingStatus)

#   define SystemConfig_IsSensorSettingStatusParamsReceived() \
        (UpdateStatus_e_ParamsReceived == g_SensorSettingStatus)

#   define SystemConfig_SetSensorSettingStatus(value) \
    (g_SensorSettingStatus = (value))

#   define SystemConfig_IsSensorSettingStatusToBeUpdated() \
        ((g_SystemConfig_Status.e_Coin_Status != g_SystemSetup.e_Coin_Ctrl) \
        && SystemConfig_IsSensorSettingStatusIdle())

#   define SystemConfig_DMADataAccessAlignment()     (g_DMASetup.e_DMADataAccessAlignment)

#   define SystemConfig_DMADataBitsInOneMemoryWord()   (g_DMASetup.e_DMADataBitsInMemory)

#   define SystemConfig_IsAgExposureOrFrameRateChanged()  ((g_Exposure_DriverControls.u16_TargetAnalogGain_x256 != g_Exposure_CompilerStatus.u16_AnalogGainPending_x256)\
                                                          ||(g_Exposure_DriverControls.u32_TargetExposureTime_us != g_Exposure_CompilerStatus.u32_TotalIntegrationTimePending_us)\
                                                          ||(GET_ABS_DIFF(g_FrameDimensionStatus.f_CurrentFrameRate ,  FrameRate_GetAskedFrameRate()) > 0.1) )

#   define SystemConfig_SetStatsCancelStatus(status)      (g_isAecStatsCancelled = (status))

#   define SystemConfig_IsStatsRequestCancelled()         (Flag_e_TRUE == g_isAecStatsCancelled)

#endif // _SYSTEM_CONFIG_H_

