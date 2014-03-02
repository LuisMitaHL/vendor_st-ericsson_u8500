/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      SDL_op_interface.h
 \brief     Implements the driver for Sensor Data Linearization (SDL) FlexTF module

 \ingroup   SDL
 \endif
*/
#ifndef SDL_OP_INTERFACE_H_
#   define SDL_OP_INTERFACE_H_

#   include "Platform.h"
#   include "SDL_ip_interface.h"

/**
 \enum SDLMode_te
 \brief Command set the sensor data linearization mode
 \ingroup SDL
*/
typedef enum
{
    /// The sensor data linearization FlexTF module will be disabled
    SDL_Disable,

    /// The sensor data linearization FlexTF module will be enabled
    /// and the device will program the FlexTF with a linear ramp
    SDL_Linear,

    /// The sensor data linearization FlexTF module will be enabled
    /// however, the device will not program the FlexTF. It will be the
    /// host responsibility to program the FlexTF memories before starting
    /// to stream
    SDL_Custom
} SDLMode_te;

/**
 \struct    SDL_Control_ts
 \brief     Main control page for the sensor data linearization block
 \ingroup   SDL
*/
typedef struct
{
    /// Control to set the sensor data linearization working mode. The fiel is valid only in Custom mode.
    /// After programming LUT in Flextf memory, host must toggle this coin.
    uint8_t e_SDLMode_Control;
} SDL_Control_ts;

/**
 \struct    SDL_Status_ts
 \brief     Status page for the sensor data linearization block
 \ingroup   SDL
*/
typedef struct
{
    /// Status of the sensor data linearization working mode
    uint8_t e_SDLMode_Status;

    /// Flag to indicate that SDL update is pending
    /// The IP has been updated and the host must be notified
    /// about Update completion at the start of new frame
    uint8_t e_Flag_SDLUpdatePending;
} SDL_Status_ts;

/**
 \struct    SDL_Last_ELT_ts
 \brief     Last look up table entry of the sensor data linearization block. The structure is valid only in custom mode.
            i.e. g_SDL_Control.e_SDLMode_Control = SDL_Custom
 \ingroup   SDL
*/
typedef struct
{
    /// ISP_FLEXTF_LINEAR_LAST_ELT_GREEN_GIR: Last Element for Green in Red channel
    uint16_t    u16_LastElementinLUT_GIR;

    /// ISP_FLEXTF_LINEAR_LAST_ELT_RED: Last Element for Red channel
    uint16_t    u16_LastElementinLUT_RED;

    /// ISP_FLEXTF_LINEAR_LAST_ELT_BLUE: Last Element for Blue channel
    uint16_t    u16_LastElementinLUT_BLUE;

    /// ISP_FLEXTF_LINEAR_LAST_ELT_GIB: Last Element for Green in Blue channel
    uint16_t    u16_LastElementinLUT_GIB;

    /// ISP_FLEXTF_LINEAR_PIXELIN_SHIFT: Pixel Input Shift Index
    /// [2:0] pixel_shift: index value of the right shift
    /// 0: 256 LUT entries to be used
    /// 1: 128 LUT entries to be used
    /// 2: 64 LUT entries to be used
    /// 3: 32 LUT entries to be used
    /// 4: 16 LUT entries to be used
    /// 5: 8 LUT entries to be used
    /// 6: 4 LUT entries to be used
    /// 7: 2 LUT entries to be used
    /// [DEFAULT]: 0, 256 LUT to be used.
    uint16_t    u16_PixelShift;
} SDL_Last_ELT_ts;

extern SDL_Control_ts   g_SDL_Control TO_EXT_DATA_MEM;
extern SDL_Status_ts    g_SDL_Status TO_EXT_DATA_MEM;
extern SDL_Last_ELT_ts  g_SDL_ELT TO_EXT_DATA_MEM;

extern void             SDL_Commit (void);
#endif /* SDL_OP_INTERFACE_H_ */

