/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      Glace_OPInterface.h
  \brief    This file is a part of the Glace module release.
            It provides an interface to allow other modules to access
            the exported Glace interfaces and functions.
 \ingroup   Glace
*/
#ifndef GLACE_OPINTERFACE_H_
#   define GLACE_OPINTERFACE_H_

#include "Platform.h"
#include "Glace_PlatformSpecific.h"
#define ENABLE_GLACE_TRACES (0)

/**
 \enum GlaceDataSource_te
 \brief Data sources applicable to Glace input
 \ingroup Glace
*/
typedef enum
{
    /// Source of input data to Glace is Gridiron output
    GlaceDataSource_e_PostLensShading,

    /// Source of input data to Glace is Channel Gain output
    GlaceDataSource_e_PostChannelGain
} GlaceDataSource_te;

/**
 \enum GlaceOperationMode_te
 \brief Mode of operation for Glace
 \ingroup Glace
*/
typedef enum
{
    /// Glace to be disabled
    GlaceOperationMode_e_Disable,

    /// Glace accumulation to be done continuously over frames
    GlaceOperationMode_e_Continuous,

    /// Glace accumulation done only once
    GlaceOperationMode_e_Once
} GlaceOperationMode_te;

/**
 \enum GlaceExportStatus_te
 \brief Status of the Glace IP
 \ingroup Glace
*/
typedef enum
{
    /// Glace statistics are not being exported
    GlaceExportStatus_e_Idle,

    /// Glace statistics are being exported
    GlaceExportStatus_e_Busy
} GlaceExportStatus_te;

/**
 \struct    Glace_Statistics_ts
 \brief     Status page for Glace.
 \ingroup   Glace
*/
typedef struct
{
    /// Memory address where the device will dump the mean pixel value per block for
    /// Red channel.
    uint32_t    u32_TargetRedAddress;

    /// Memory address where the device will dump the mean pixel value per block for
    /// Green channel.
    uint32_t    u32_TargetGreenAddress;

    /// Memory address where the device will dump the mean pixel value per block for
    /// Blue channel.
    uint32_t    u32_TargetBlueAddress;

    /// Memory address where the device will dump the number of saturated pixels per block.
    uint32_t    u32_TargetSaturationAddress;

    /// The horizontal grid size applicable to the current statistics
    uint32_t    u32_HGridSize;

    /// The horizontal grid size applicable to the current statistics
    uint32_t    u32_VGridSize;
} Glace_Statistics_ts;

/**
 \struct    Glace_Control_ts
 \brief     Main control page for Glace.
 \ingroup   Glace
*/
typedef struct
{
    /// Horizontal size of the Glace window of accumulation specified as a fraction
    /// of the current sensor output size. This value must be less than or equal to 1.
    /// e.g. if f_HBlockSizeFraction == 0.8, then the device will only consider 80%
    /// of the current sensor output while computing the horizontal
    /// Glace block size. Must be programmed once only before start of streaming.
    float_t             f_HBlockSizeFraction;

    /// Vertical size of the Glace window of accumulation specified as a fraction
    /// of the current sensor output size. This value must be less than or equal to 1.
    /// e.g. if f_VBlockSizeFraction == 0.8, then the device will only consider 80%
    /// of the current sensor output size while computing the vertical
    /// Glace block size. Must be programmed once only before start of streaming.
    float_t             f_VBlockSizeFraction;

    /// Specifies the horizontal offset for the Glace window of accumulation
    /// expressed as a fraction of the current sensor output size. This value must
    /// be less than or equal to 1. e.g. if f_HROIStartFraction == 0.05, then
    /// the horizontal offset for window of accumulation is considered to be 5%
    /// of the current sensor output size. Must be programmed once only before start of streaming.
    float_t             f_HROIStartFraction;

    /// Specifies the vertical offset for the Glace window of accumulation
    /// expressed as a fraction of the current sensor output size. This value must
    /// be less than or equal to 1. e.g. if f_VROIStartFraction == 0.05, then
    /// the vertical offset for window of accumulation is considered to be 5%
    /// of the current sensor output size. Must be programmed once only before start of streaming.
    float_t             f_VROIStartFraction;

    /// Address of external memory where the device will extract the locations where
    /// the Red, Green, Blue channel and the saturation statistics will be dumped.
    /// The device assumes the memory pointer to be of type Glace_Statistics_ts.
    /// No copy will be done if ptrGlace_Statistics == 0
    Glace_Statistics_ts *ptrGlace_Statistics;

    /// Specifies the saturation level to be considered for the Red channel.
    /// Must be programmed once only before start of streaming.
    uint8_t             u8_RedSaturationLevel;

    /// Specifies the saturation level to be considered for the Green channel.
    /// Must be programmed once only before start of streaming.
    uint8_t             u8_GreenSaturationLevel;

    /// Specifies the saturation level to be considered for the Blue channel
    /// Must be programmed once only before start of streaming.
    uint8_t             u8_BlueSaturationLevel;

    /// Number of blocks in the horizontal cross section of the Glace grid.
    /// Must be programmed once only before start of streaming.
    uint8_t             u8_HGridSize;

    /// Number of blocks in the vertical cross section of the Glace grid.
    /// Must be programmed once only before start of streaming.
    uint8_t             u8_VGridSize;

    /// Control to manage the operation mode.
    /// In continuous mode, the device continuously performs
    /// Glace accumulation over successive frames.
    /// In Once mode, the device performs Glace accumulation over
    /// one frame only.
    /// When disabled, no accumulation is performed.
    uint8_t             e_GlaceOperationMode_Control;

    /// Control to specify the source of data at Glace input.
    /// Must be programmed once only before start of streaming.
    uint8_t             e_GlaceDataSource;

    /// Control count to update the Glace parameters
    /// together with the Glace_Status.u8_ParamUpdateCount, it provides
    /// an atomic way for the host to update the Glace parameters.
    /// When Glace_Control.u8_ParamUpdateCount != Glace_Control.u8_ParamUpdateCount
    /// the device will absorb the new Glace parameters
    uint8_t             u8_ParamUpdateCount;

    /// Control count to update the Glace operation mode control
    /// together with the Glace_Status.u8_ControlUpdateCount, it provides
    /// an atomic way for the host to update the Glace operation mode control.
    /// When Glace_Control.u8_ControlUpdateCount != Glace_Control.u8_ControlUpdateCount
    /// it will act as a trigger for the device to absorb Glace_Control.e_GlaceOperationMode_Control
    uint8_t             u8_ControlUpdateCount_debug;

    /// Specifies whether histogram geometry is calculated on the basis of master pipe FOV or on the basis of sensor FOV
    /// StatisticsFov_e_Sensor
    /// StatisticsFov_e_Master_Pipe
    uint8_t     e_StatisticsFov;
} Glace_Control_ts;

/**
 \struct    Glace_Status_ts
 \brief     Status page for Glace.
 \ingroup   Glace
*/
typedef struct
{
    /// Specifies the value of multiplier to be used for computation of pixel mean
    uint32_t    u32_GlaceMultiplier;

    /// Specifies the absolute value of start of horizontal window of accumulation in
    /// terms of number of pixels
    uint16_t    u16_HROIStart;

    /// Specifies the absolute value of start of vertical window of accumulation in
    /// terms of number of pixels
    uint16_t    u16_VROIStart;

    /// Specifies the horizontal size of the grid in terms of number of blocks
    uint8_t     u8_HGridSize;

    /// Specifies the vertical size of the grid in terms of number of blocks
    uint8_t     u8_VGridSize;

    /// Specifies the absolute horizontal size of the block in terms of number of pixels
    uint8_t     u8_HBlockSize;

    /// Specifies the absolute vertical size of the block in terms of number of pixels
    uint8_t     u8_VBlockSize;

    /// Specifies the value of shift to be used to computation of pixel mean
    uint8_t     u8_GlaceShift;

    /// Status of the glace operation
    uint8_t     e_GlaceOperationMode_Status;

    /// Status count to update the Glace parameters
    /// together with the Glace_Control.u8_ParamUpdateCount, it provides
    /// an atomic way for the host to update the Glace parameters.
    /// When Glace_Control.u8_ParamUpdateCount != Glace_Control.u8_ParamUpdateCount
    /// the device will absorb the new Glace parameters
    uint8_t     u8_ParamUpdateCount;

    /// Specifies the status of the exporting activity
    uint8_t     e_GlaceExportStatus;

    /// Status count to update the Glace operation mode control
    /// together with the Glace_Control.u8_ControlUpdateCount_debug, it provides
    /// an atomic way for the host to update the Glace operation mode control.
    /// When Glace_Control.u8_ControlUpdateCount_debug != Glace_Control.u8_ControlUpdateCount
    /// it will act as a trigger for the device to absorb Glace_Control.e_GlaceOperationMode_Control
    uint8_t     u8_ControlUpdateCount;

    /// Specifies whether the glace enable is pending from a previous enable attempt
    uint8_t     e_Flag_GlaceEnablePending;

    /// Specifies whether glace geometry is calculated on the basis of master pipe FOV or on the basis of
    /// sensor output size
    /// StatisticsFov_e_Sensor
    /// StatisticsFov_e_Master_Pipe
    uint8_t     e_StatisticsFov;
} Glace_Status_ts;

extern void Glace_Commit ( void ) TO_EXT_PRGM_MEM;
extern void Glace_ISR (void);
extern void Glace_ExportISR (void);
extern void EnableGlaceInternally ( void ) TO_EXT_PRGM_MEM;

extern Glace_Control_ts g_Glace_Control;
extern Glace_Status_ts  g_Glace_Status;
#endif /*GLACE_OPINTERFACE_H_*/

