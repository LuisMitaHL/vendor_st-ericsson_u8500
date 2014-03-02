/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 /*
\file Mozart.h
 */
#ifndef MOZART_H_
#define MOZART_H_

#include "Platform.h"

typedef struct
{
    /// Sharpens the color boundaries at middle frequencies
    uint16_t u16_Mozart_M_SBPeaker;

    /// Controls the level of the whole coring  to obtain the sharper image
    uint16_t  u16_Mozart_SBCoring_min;

    /// Sharpens the color boundaries  at High frequencies
    uint16_t u16_Mozart_H_SBPeaker;

    /// Enable/disable the hw block
    uint8_t  e_Flag_MozartEnable;

    /// Sharpens/smoothens out the polygon edges
    uint8_t  u8_Mozart_Chroma_AntiAliasing;

    /// Controls the level of coring on the middle frequency to obtain the sharper image
    uint8_t  u8_Mozart_M_SBCoring_gain;

    /// Controls the level of coring on the high frequency to obtain the sharper image
    uint8_t  u8_Mozart_H_SBCoring_gain;

    /// Controls the level brightness
    uint8_t  u8_Mozart_LumaGain;

    /// Controls the level color saturation
    uint8_t  u8_Mozart_ColorGain;
} Mozart_Ctrl_ts;

// Control page of Mozart
extern Mozart_Ctrl_ts  g_Mozart_Ctrl;

// Status page of Mozart
extern Mozart_Ctrl_ts  g_Mozart_Status;

// Function to commit Mozart parameters
void Mozart_Commit (void);

// Function to store the status  page elements of mozart
void Mozart_Update (void);

#endif /* MOZART_H_ */
