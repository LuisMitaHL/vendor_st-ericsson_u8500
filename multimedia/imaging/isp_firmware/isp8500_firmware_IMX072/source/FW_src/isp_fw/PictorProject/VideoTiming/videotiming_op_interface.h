/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup VideoTiming Video Timing Module
 \details Video Timing module calculate multipliers and dividers to be
        programmed in the sensor to achieve host specified data rate.
        If any module require services of Video Timing, it should call
        clock setup function first. clock setup function will update all
        the parameters exported by Video Timing based on external module's inputs
        and sensor contraints.
*/

/**
 \file videotiming_op_interface.h
 \brief Export Video Timing Functions and macros to other module
 \details  The file is part of release code and export functions and macros which may be required
           by other modules to program sensor. Video timing module should be invoked after finalizing
           frame dimensions. The parameters exported by module is read only i.e. no other module should
           change the Video timing parameters and external modules should only use the exported macros in this file.
 \ingroup VideoTiming
*/
#ifndef _VT_OP_INTERFACE_H_
#   define _VT_OP_INTERFACE_H_

#   include "Platform.h"
#   include "videotiming_PlatformSpecific.h"
/**
 \enum SensorScalingMode_te
 \brief Pre scaling mode being used in the sensor
 \ingroup VideoTiming
*/
typedef enum
{
    /// No pre scaling being used in the sensor
    VTSensorScalingMode_e_VIDEOTIMING_SENSOR_SCALING_NONE,

    /// Pre scaling type is "Scaling"
    VTSensorScalingMode_e_VIDEOTIMING_SENSOR_SCALING,

    /// Pre scaling type is "Sub Sampling"
    VTSensorScalingMode_e_VIDEOTIMING_SENSOR_SUB_SAMPLING
} VTSensorScalingMode_te;


/**
 \enum DeratingRoundingMode_te
 \brief Specifies the output clock derating rounding mode
 \ingroup VideoTiming
*/
typedef enum
{
    /// If derating rounding mode == ROUND_UP then FIFO requirements are low since
    /// the output clock is faster then the average scaler output data rate
    DeratingRoundingMode_e_ROUND_UP,

    /// If derating rounding mode == ROUND_CLOSEST then the error between the target
    /// derating factor and actual derating factor is the minimum
    DeratingRoundingMode_e_ROUND_CLOSEST
} DeratingRoundingMode_te;

/**
 \enum VideoTimingMode_te
 \brief Video Timing mode. Only 2 modes are supported: Manual and automatic
 \ingroup VideoTiming
*/
typedef enum
{
    /// The device does not perform any video timing computations i.e. Muplipliers and Dividers to be programmed in the sensor.
    /// Host should provide all the dividers and multipliers to be programmed in the sensor
    VideoTimingMode_e_VideoTimingMode_Manual,

    /// The device will calculate all the Video timing values
    VideoTimingMode_e_VideoTimingMode_Automatic
} VideoTimingMode_te;

/**
 \enum Polarity_te
 \brief Polarity: Used only for parallel interface sensors
 \ingroup VideoTiming
*/
typedef enum
{
    /// Active low
    Polarity_e_Polarity_ActiveLow,

    /// Active High
    Polarity_e_Polarity_ActiveHigh
} Polarity_te;

/**
 \enum SensorBitsPerSystemClock_te
 \brief CCP clock settings
        Host data rate > 208 Mbps, user differential clock \n
        It transfer 2 bits of data per system clock and is called as Data Strobe.
 \ingroup VideoTiming
*/
typedef enum
{
    /// Host Data Rate <= 208Mbps Data/Clock
    SensorBitsPerSystemClock_e_DATA_CLOCK                                        = 1,

    /// Host Data Rate >  208Mbps Data/Strobe
    SensorBitsPerSystemClock_e_DATA_STROBE                                       = 2
} SensorBitsPerSystemClock_te;

/**
 \struct VideoTimingHostInputs_ts
 \brief Input paramters from Host to Video Timing module.
        Host should program the values of following page elements
 \ingroup VideoTiming
*/
typedef struct
{
    /// Maximum host data rate (in Mbps)\n
    /// \n
    ///    [DEFAULT]: 640 Mbps
    float_t     f_HostRxMaxDataRate_Mbps;

    /// Specifies the target derating factor as a fraction of the maximum possible derating
    /// i.e. target_derating_factor = maximum_possible_derating_factor * f_OutputClockDeratingFraction \n
    /// Range [0.0 to Max possible] \n
    ///     => 0.0 will disable de-rating \n
    ///     => 1.0 will de-rate output clock by factor of equal or less than scaling factor \n
    ///     => De-rating factor > 1.0, will de-rate the output clock more than scaling factor \n
    ///        In this case, User want Video timing to run at higher frequency and de-rate the output clock,
    ///        so user has to increase the frame length by certain factor. Please refer f_reserve for line length calculations \n
    /// \n
    ///    [DEFAULT]: 1.0
    float_t     e_Flag_reserve01;

    /// csi_raw_format \n
    /// for example RAW8: 0x0808, RAW10: 0x0A0A etc.\n
    /// For more details on data format, Please refer SMIA_Functional_specification_1.0.pdf\n\n
    /// [DEFAULT]: RAW8: 0x0808
    uint16_t    u16_CsiRawFormat;

    /// Video Timing Mode: Manual or Automatic \n
    /// The host has to provide a few inputs to the video timing block for it to automatically (automatic mode) compute the video timing parameters.
    /// These values may be programmed at any point before issuing a streaming command. \n\n
    /// [DEFAULT]: e_VideoTimingMode_Automatic
    uint8_t     e_Flag_reserve02;

    /// Number of data bits per sensor clock \n\n
    /// [DEFAULT]: e_DATA_STROBE(2)
    uint8_t     e_SensorBitsPerSystemClock_DataType;

    /// Specifies the output clock derating rounding mode.\n
    /// e_ROUND_UP: FIFO requirements are low since theoutput clock is faster then the average scaler output data rate.\n
    /// e_ROUND_CLOSEST: The error between the target derating factor and actual derating factor is the minimum.\n\n
    /// [DEFAULT]: e_ROUND_UP
    uint8_t     e_Flag_reserve03;

    /// Specifies if the video timing clock derating is required for profile 0 sensor
    /// Any sub sampling can then be exploited to slow down the video timing clock to reduce power consumption. \n\n
    /// [DEFAULT]: e_FALSE
    uint8_t     e_Flag_reserve04;

    /// Specifies the vsync polarity.
    /// Input only valid for parallel sensor, Don't care for serial sensor.
    uint8_t     e_Flag_reserve05;

    /// Specifies the hsync polarity.
    /// Input only valid for parallel sensor, Don't care for serial sensor.
    uint8_t     e_Flag_reserve06;
} VideoTimingHostInputs_ts;

/**
 \struct VideoTimingOutput_ts
 \brief Output generated by the video timing module is stored in following page elements.
 The page elements hold values of multipliers, dividers to be programmed in the sensor as well as
 frequencies calculated at different blocks, Derating factor etc.
 \ingroup VideoTiming
*/
typedef struct
{
    /// pll_ip_freq_mhz: Input frequency into PLL multiplier
    float_t     f_reserve01;

    /// pll_op_freq_mhz: Output frequency from PLL multiplier
    float_t     f_reserve02;

    /// vt_sys_clk_freq_mhz: VT system clock frequency is the readout frequency from sensor array
    float_t     f_VTSystemClockFrequency_Mhz;

    /// vt_pix_clk_freq_mhz
    float_t     f_VTPixelClockFrequency_Mhz;

    /// vt_pix_clk_period_us
    float_t     f_reserve03;

    /// op_sys_clk_freq_mhz: Output system clock Frequency. It is same or less than the host specified data rate in video timing inputs
    float_t     f_reserve04;

    /// op_pix_clk_freq_mhz
    float_t     f_OPPixelClockFrequency_Mhz;

    /// Specifies the clock derating in the output timing domain
    float_t     f_OutputTimingClockDerating;

    /// Specifies the data reduction done in output timing domain, VT takes care of data reduction
    /// User want to run VT at heigher clock and de-rate output, in that case VT line length has to be increased
    /// Following should be used to calculate increase in line length \n
    /// Increase in line length: fpOutputTimingClockDerating / fpTotalDataReductionFactor
    ///
    float_t     f_reserve05;

    /// pre_pll_clk_div: Pre Pll Clock divider
    uint16_t    u16_reserve06;

    /// pll_multiplier: Pll Multiplier connected after Pre Pll clock divider
    uint16_t    u16_reserve07;

    /// vt_sys_clk_div: Video timing system clock divider connected after Pll multiplier in VT domain
    uint16_t    u16_reserve08;

    /// vt_pix_clk_div: Video timing pixel clock divider connected after VT system clock divider in VT domain
    uint16_t    u16_reserve09;

    /// op_sys_clk_div: Output system clock divider connected after Pll multiplier in output domain, this is parallel to VT system clock divider
    uint16_t    u16_reserve10;

    /// op_pix_clk_div: Output pixel clock divider connected after output system clock divider output domain. The value of this register is Target data format
    uint16_t    u16_reserve11;
} VideoTimingOutput_ts;


/*************** Page elements to be exported to user *********************/

/// Host Inputs e.g.
///  g_VideoTimingHostInputs                MODE_STATIC_PAGE         READ_WRITE_PAGE
extern VideoTimingHostInputs_ts         g_VideoTimingHostInputs TO_EXT_DATA_MEM;

//// video timing output
///  g_VideoTimingOutput                    MODE_STATIC_PAGE         READ_WRITE_PAGE
extern VideoTimingOutput_ts             g_VideoTimingOutput TO_EXT_DATA_MEM;

/// De-rating calculated by Video Timing
#   define VT_32SF_OUTPUT_TIMING_DERATING  READONLY(g_VideoTimingOutput.f_OutputTimingClockDerating)

/// Data format programmed for currently active sensor i.e. RAW8 : 0x0808, RAW10: 0x0A0A, 10to08: 0x0A08
#   define VideoTiming_GetCsiRawFormat()    READONLY(g_VideoTimingHostInputs.u16_CsiRawFormat)

/// Interface to specify if the sensor is streaming in data strobe mode
#define VideoTiming_IsDataStrobeMode()      (SensorBitsPerSystemClock_e_DATA_STROBE == g_VideoTimingHostInputs.e_SensorBitsPerSystemClock_DataType)

/// Interface to specify if the sensor is streaming in data clock mode
#define VideoTiming_IsDataClockMode()       (SensorBitsPerSystemClock_e_DATA_CLOCK == g_VideoTimingHostInputs.e_SensorBitsPerSystemClock_DataType)

/// No of bits per system clock: 2 for data strobe and 1 for data clock
#define VideoTiming_GetSensorBitsPerSystemClock() (VideoTiming_IsDataStrobeMode() ? 2 : 1)

/// Interface to expose the sensor data rate
#define VideoTiming_GetSensorDataRate()     READONLY(g_VideoTimingOutput.f_VTSystemClockFrequency_Mhz)

/// Interface to define the pixel width prior to any scaling in the sensor i.e.
/// if csi_raw_format = 0x0A08, then the pre scaled pixel width = 0x0A
#define VideoTiming_GetSensorInputPixelWidth()  (uint8_t)(g_VideoTimingHostInputs.u16_CsiRawFormat >> 0x08)

/// Interface to define the pixel width post any scaling in the sensor i.e.
/// if csi_raw_format = 0x0A08, then the post scaled pixel width = 0x08
#define VideoTiming_GetSensorOutputPixelWidth() (uint8_t)(g_VideoTimingHostInputs.u16_CsiRawFormat & 0x0F)

#endif // _VT_OP_INTERFACE_H_

