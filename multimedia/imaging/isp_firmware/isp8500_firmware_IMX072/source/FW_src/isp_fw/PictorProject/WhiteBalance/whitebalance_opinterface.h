/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\defgroup WhiteBalance White Balance Module
\brief This Module implements the White Balance Algorithm.
\details Based on the statistics provided by the Stats Processor, this module
         calculates gains to be applied. Host can use manual gains or the gains
         calculated automatically by White Balance Algorithm.
*/

/**
\file whitebalance_opinterface.h
\brief The file exports all the structures and functions needed by any external module to run white balance.
        The module will need to include this file. The file is part of release code.
        This file contains the data structures, macros, enums and function declarations used by the module.
\ingroup WhiteBalance
*/
#ifndef WHITEBALANCE_OPINTERFACE_H_
#   define WHITEBALANCE_OPINTERFACE_H_
#   include "Platform.h"

/**
\enum WhiteBalance_Status_te
\brief enum for current Status of WhiteBalance Module Gains.
\ingroup WhiteBalance
*/
typedef enum
{
    ///White Balance is Not Stable
    WhiteBalance_Status_e_NotStable,

    ///White Balance is Stable
    WhiteBalance_Status_e_Stable,
} WhiteBalance_Status_te;

/**
\struct   WhiteBalanceControl_ts
\brief    Control parameters for different modes of White Balance Module.
\ingroup  WhiteBalance
*/
typedef struct
{
    /// Manual Red Gain. Values programmed by the user.\n
    float_t f_RedManualGain;

    /// Manual Green Gain. Values programmed by the user.\n
    float_t f_GreenManualGain;

    /// Manual  Blue. Values programmed by the user.\n
    float_t f_BlueManualGain;
} WhiteBalanceControl_ts;

/**
 \struct      WhiteBalanceStatus_ts
 \brief   Status Page for the WhiteBalance Module.
 \ingroup   WhiteBalance
*/
typedef struct
{
    /// Current red channel gain
    float_t f_RedGain;

    /// Current Green  channel gain
    float_t f_GreenGain;

    /// Current Blue channel gain
    float_t f_BlueGain;

} WhiteBalanceStatus_ts;

#define WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR PLATFORM_WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR


/************************ Exported Page elements *********************/
extern WhiteBalanceControl_ts   g_WhiteBalanceControl;
extern WhiteBalanceStatus_ts    g_WhiteBalanceStatus;


/// Macros to be used by other module

/// Red Gain calculated by White balance module
#   define WhiteBalance_GetRedGain()   READONLY(g_WhiteBalanceStatus.f_RedGain)

/// Green Gain calculated by White balance module
#   define WhiteBalance_GetGreenGain() READONLY(g_WhiteBalanceStatus.f_GreenGain)

/// Blue Gain calculated by White balance module
#   define WhiteBalance_GetBlueGain()  READONLY(g_WhiteBalanceStatus.f_BlueGain)

#endif /*WHITEBALANCE_OPINTERFACE_H_*/

