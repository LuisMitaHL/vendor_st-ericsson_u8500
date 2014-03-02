/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup HStats Histogram statistics Module
 \brief Hist module copies the stats given by hw to the host memory.
*/

/**
\file    histogram_op_interface.h
\brief   Provides an interface for control of the histogram block.
\ingroup HStats
*/
#ifndef _HISTOGRAM_OP_INTERFACE_H_
#   define _HISTOGRAM_OP_INTERFACE_H_

#   include "Platform.h"
#   include "histogram_ip_interface.h"
#   include "histogram_platform_specific.h"
#   include "InterruptManager.h"
#   include "STXP70_OPInterface.h"

#define ENABLE_HISTOGRAM_TRACES (0)

typedef enum
{
    /// no action performed.
    HistCmd_e_IDLE,

    /// Continuous histogram computation. At the beginning of each frame, histogram values are reset.
    /// MCU/HOST must retrieve the histogram element values before the beginning of the next frame.
    HistCmd_e_GRAB,

    /// Single Histogram Computation. This command is only active during a single frame.
    /// The reset happens at the beginning of the frame. The histogram issued is kept unmodified
    /// till the next cmd_rwp is ordered. The MCU can thus retrieve the histogram at any time.
    HistCmd_e_STILL
} HistCmd_te;

typedef enum
{
    /// Block is idle and no data are gathered
    HistStatus_e_IDLE,

    /// Block is waiting for data to start gathering histogram
    HistStatus_e_READY,

    /// Histogram is being gathered
    HistStatus_e_ON_PROGRESS,

    /// Histogram has been gathered and MCU/Host can retrieve the data
    HistStatus_e_COMPLETED
} HistStatus_te;

/**

 * \enum 	  ExportStatus_te
 * \brief	  Enum defined for the reporting of status of exporting of statistics to the host.
 * \ingroup   HStats

*/
typedef enum
{
    /// export complete
    ExportStatus_e_COMPLETE,

    /// export incomplete
    ExportStatus_e_INCOMPLETE
} ExportStatus_te;

/**

 * \enum 	  HistogramMode_te
 * \brief	  Enum defined for selection of mode for Histogram
 * \ingroup   HStats

*/
typedef enum
{
    /// No operation
    HistogramMode_e_IDLE,

    /// Histogram to run in every frame continuously
    HistogramMode_e_CONTINUOUS,

    /// Hostogram to run only once(used for single step operation)
    HistogramMode_e_ONCE
} HistogramMode_te;

/**

 * \enum 	  HistInputSrc_te
 * \brief	  Enum defined for selection of source for Histogram
 * \ingroup   HStats

*/
typedef enum
{
    /// After Lens Shading Correction
    HistInputSrc_e_PostLensShading,

    /// After Channel gains processing
    HistInputSrc_e_PostChannelGains
} HistInputSrc_te;

/// Page containing the Histogram Statistics Controls.
typedef struct
{
    /// Address of the R Memory to which the XP70 copies
    /// the stats so that they can be accessed by the HOST
    uint32_t    *ptru32_HistRAddr;

    /// Address of the G Memory to which the XP70 copies
    /// the stats so that they can be accessed by the HOST
    uint32_t    *ptru32_HistGAddr;

    /// Address of the B Memory to which the XP70 copies
    /// the stats so that they can be accessed by the HOST
    uint32_t    *ptru32_HistBAddr;

    /// X Size of the Region Of Interest : 12
    /// A value of 1.0 means 100 %
    float_t     f_HistSizeRelativeToFOV_X;

    /// Y Size of the Region Of Interest : 12
    /// A value of 1.0 means 100 %
    float_t     f_HistSizeRelativeToFOV_Y;

    /// X Offset of the Region Of Interest : 12
    /// A value of 1.0 means 100 %
    float_t     f_HistOffsetRelativeToFOV_X;

    /// Y Offset of the Region Of Interest : 12
    /// A value of 1.0 means 100 %
    float_t     f_HistOffsetRelativeToFOV_Y;

    /// Shift of the Input pixel           :  3
    /// [NOTE]: It should not exceed the pixel width - 1, in this case it should not exceed 7.
    uint8_t     u8_HistPixelInputShift;

    /// Select the iNput for the Hostogram Block
    uint8_t     e_HistInputSrc;

    /// Enable/Disable the IP
    uint8_t     e_Flag_Enable;

    /// Soft reset of the IP
    uint8_t     e_Flag_SoftResest;

    /// Cmd to the IP
    uint8_t     e_HistCmd;

    /// coin mechanism for the control of the Ip to the host
    uint8_t     e_CoinCtrl_debug;

    /// the mode of Histogram whether, it will be ONCE/CONTINUOUS
    uint8_t     e_HistogramMode;

    /// Specifies whether glace geometry is calculated on the basis of master pipe FOV or on the basis of sensor FOV
    /// StatisticsFov_e_Sensor
    /// StatisticsFov_e_Master_Pipe 
    uint8_t     e_StatisticsFov;         
} HistStats_Ctrl_ts;

typedef struct
{
    /// X Size of the Region Of Interest : 12
    uint16_t    u16_HistSizeX;

    /// Y Size of the Region Of Interest : 12
    uint16_t    u16_HistSizeY;

    /// X Offset of the Region Of Interest : 12
    uint16_t    u16_HistOffsetX;

    /// Y Offset of the Region Of Interest : 12
    uint16_t    u16_HistOffsetY;

    /// r_dark_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel R) :  12
    uint16_t    u16_DarkestBin_R;

    /// r_bright_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel R) :  12
    uint16_t    u16_BrightestBin_R;

    /// r_highest_bin: Lowest bin index with non-zero value in Green channel
    /// Bin index corresponding to the maximum value in the array (channel R) :  12
    uint16_t    u16_HighestBin_R;

    /// g_dark_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel G) :  12
    uint16_t    u16_DarkestBin_G;

    /// g_bright_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel G) :  12
    uint16_t    u16_BrightestBin_G;

    /// g_highest_bin: Lowest bin index with non-zero value in Green channel
    /// Bin index corresponding to the maximum value in the array (channel G) :  12
    uint16_t    u16_HighestBin_G;

    /// b_dark_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel B) :  12
    uint16_t    u16_DarkestBin_B;

    /// b_bright_bin: Lowest bin index with non-zero value in Green channel
    /// Lowest bin index for which value in the array is not zero (channel B) :  12
    uint16_t    u16_BrightestBin_B;

    /// b_highest_bin: Lowest bin index with non-zero value in Green channel
    /// Bin index corresponding to the maximum value in the array (channel B) :  12
    uint16_t    u16_HighestBin_B;

    /// coin mechanism for the control of the Ip to the host
    uint8_t     e_CoinStatus;

    /// for the reporting of status of exporting of statistics to the host
    uint8_t     e_ExportStatus;

    /// Specifies whether histogram geometry is calculated on the basis of master pipe FOV or on the basis of sensor FOV
    /// StatisticsFov_e_Sensor
    /// StatisticsFov_e_Master_Pipe 
    uint8_t     e_StatisticsFov;         
} HistStats_Status_ts;

/// global pages
extern HistStats_Ctrl_ts    g_HistStats_Ctrl;
extern HistStats_Status_ts  g_HistStats_Status;

/// global functions
//extern void Histogram_SetupStatistics(void);
//extern void Histogram_UpdateHostMemories(void);
extern void                 Histogram_Commit (void)TO_EXT_DDR_PRGM_MEM;
extern void                 Histogram_ISR (void);
extern void                 Histogram_ExportISR (void);

extern void                 EnableHistogramInternally (void)TO_EXT_DDR_PRGM_MEM;
#endif // _HISTOGRAM_OP_INTERFACE_H_

