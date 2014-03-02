/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup Run Mode Control runmode
 \brief
*/

/**
 \file run_mode_ctrl.h
 \brief
 \ingroup runmode
*/
#ifndef _RUN_MODE_CTRL_H_
#   define _RUN_MODE_CTRL_H_

#   include "PictorhwReg.h"

typedef enum
{
    /// VF
    StreamMode_e_VF     = 0,

    /// Capture
    StreamMode_e_Capture
} StreamMode_te;

typedef enum
{
    /// MODE_STILL
    SensorModeVF_e_STILL        = 0,

    /// MODE_VIDEO
    SensorModeVF_e_VIDEO        = 1,

    /// MODE_NIGHT
    SensorModeVF_e_STILL_NIGHT  = 2,

    /// MODE_SEQ
    SensorModeVF_e_STILL_SEQ    = 3,

    /// MODE_AF
    SensorModeVF_e_AF           = 5
} SensorModeVF_te;

typedef enum
{
    /// MODE_STILL
    SensorModeCapture_e_STILL       = 0,

    /// MODE_NIGHT
    SensorModeCapture_e_STILL_NIGHT = 2,

    /// MODE_SEQ
    SensorModeCapture_e_STILL_SEQ   = 3,

    /// MODE_FLASH
    SensorModeCapture_e_FLASH       = 4
} SensorModeCapture_te;

typedef enum
{
    /// MODE_STILL
    Usecase_e_STILL       = 0,

    /// MODE_NIGHT
    Usecase_e_VIDEO = 1,

} Usecase_te;

typedef enum
{
    /// MODE_NORMAL
    Orientation_e_NORMAL       = 0,

    /// MODE_ROTATED
    Orientation_e_ROTATED = 1,

} Orientation_te;


typedef enum {

  /// Restriction bitmask for VF
  LLD_USAGE_MODE_VF ,

  /// Restriction bitmask for AF
  LLD_USAGE_MODE_AF ,

  /// Restriction bitmask for STILL_CAPTURE
  LLD_USAGE_MODE_STILL_CAPTURE,

  /// Restriction bitmask for NIGHT_STILL_CAPTURE
  LLD_USAGE_MODE_NIGHT_STILL_CAPTURE ,

  /// Restriction bitmask for NIGHT_STILL_CAPTURE
  LLD_USAGE_MODE_STILL_SEQ_CAPTURE ,

  /// Restriction bitmask for VIDEO_CAPTURE
  LLD_USAGE_MODE_VIDEO_CAPTURE ,

  /// Restriction bitmask for NIGHT_VIDEO_CAPTURE
  LLD_USAGE_MODE_NIGHT_VIDEO_CAPTURE ,

  /// Restriction bitmask for HQ_VIDEO_CAPTURE
  LLD_USAGE_MODE_HQ_VIDEO_CAPTURE ,

  /// Restriction bitmask for HS_VIDEO_CAPTURE
  LLD_USAGE_MODE_HS_VIDEO_CAPTURE , // high frame rate video

  ///This value ndicates that restriction bitmask is not not used by SMS
  CAM_DRV_USAGE_MODE_UNDEFINED = 255,
} LLD_USAGE_MODE_te;

/**
\struct  RunMode_Control_ts
\brief   Control parameters VF or CAPTURE
\ingroup
*/
typedef struct
{
    /// Streaming mode either VF or Capture
    uint8_t e_StreamMode;

    /// not used
    uint8_t e_SensorModeVF;

    /// not used
    uint8_t e_SensorModeCapture;

    /// If Flag_e_TRUE, Mechanical shutter is to be used
    uint8_t e_Flag_MechanicalShutterUsed;

    /// Control coin to available to HOST for controlling mode change.
    /// Default value = Tails
    uint8_t e_Coin_Ctrl;

    /// Indicates usage mode
    uint8_t e_LLD_USAGE_MODE_usagemode;

} RunMode_Control_ts;

/**
\struct  RunMode_Control_ts
\brief   Control parameters VF or CAPTURE
\ingroup
*/
typedef struct
{
    /// Streaming mode either VF or Capture
    uint8_t e_StreamMode;

    /// Status coin for fw to know, when to apply the parameters programmed by HOST.
    /// Default value = Tails
    uint8_t e_Coin_Status;
} RunMode_Status_ts;

extern RunMode_Control_ts   g_RunMode_Control;
extern RunMode_Status_ts    g_RunMode_Status;
#endif //_RUN_MODE_CTRL_H_

