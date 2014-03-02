/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 \file Adsoc.h
 */
#ifndef ADSOC_H_
#   define ADSOC_H_

#   include "Platform.h"
#   include "Adsoc_ip_interface.h"

typedef struct
{
    /// Flag to enable/disable Adosc Peaking
    /// TRUE : Enable
    /// False: Disable
    uint8_t e_Flag_Adsoc_PK_Enable;

    /// Adsoc Peaking Gain
    uint8_t u8_Adsoc_PK_Gain;

    /// Flag to enable/disable Adaptive sharpening
    /// TRUE : Enable
    /// False: Disable
    uint8_t e_Flag_Adsoc_PK_AdaptiveSharpening_Enable;

    /// Adsoc Peaking Adaptive Coring Level
    uint8_t u8_Adsoc_PK_Coring_Level;

    /// Adsoc Peaking Overshoot Gain Bright Ctrl
    uint8_t u8_Adsoc_PK_OverShoot_Gain_Bright;

    /// Adsoc Peaking Overshoot Gain Dark Ctrl
    uint8_t u8_Adsoc_PK_OverShoot_Gain_Dark;

    /// Adsoc Peaking Emboss Effect Ctrl
    uint8_t u8_Adsoc_PK_Emboss_Effect_Ctrl;

    /// Adsoc Peaking Flipper Ctrl
    uint8_t u8_Adsoc_PK_Flipper_Ctrl;

    /// Adsoc Peaking GreyBack Effect Ctrl
    uint8_t u8_Adsoc_PK_GrayBack_Ctrl;
} Adsoc_PK_Ctrl_ts;

typedef struct
{
    /// horizontal coordinate of lens centre, w.r.t. top-left position in pixel grid
    uint16_t    u16_Lens_Centre_HOffset;

    /// vertical coordinate of lens centre, w.r.t. top-left position in pixel grid
    uint16_t    u16_Lens_Centre_VOffset;

    /// Adsoc Radial Peaking Enable
    uint8_t     e_Flag_Adsoc_RP_Enable;

    /// RP Polynomial coef 0
    uint8_t     u8_Radial_Adsoc_RP_Polycoef0;

    /// RP Polynomial coef 1
    uint8_t     u8_Radial_Adsoc_RP_Polycoef1;

    /// RP Polynomial cof shift
    uint8_t     u8_Radial_Adsoc_RP_COF_Shift;

    /// RP gain shift or out shift
    uint8_t     u8_Radial_Adsoc_RP_Out_COF_Shift;

    /// RP Unity: Radial peaking modulation on lens center
    uint8_t     u8_Radial_Adsoc_RP_Unity;
} Adsoc_RP_Ctrl_ts;

typedef struct
{
    /// RP: radial peaking horizontal scaling factor
    uint16_t    u16_Adsoc_RP_Scale_X;

    /// RP: radial peaking vertical scaling factor
    uint16_t    u16_Adsoc_RP_Scale_Y;

    /// RP HOFFSET: signed relative horizontal coordinate of lens centre
    int16_t     s16_Adsoc_RP_HOffset;

    /// RP VOFFSET: signed relative vertical coordinate of lens centre
    int16_t     s16_Adsoc_RP_VOffset;
} Adsoc_RP_Status_ts;

/// Control Page element for adsoc block (Both PIPE0 and PIPE1)
/// g_Adsoc_PK_Ctrl[0] for programming of PIPE0 parameters
/// g_Adsoc_PK_Ctrl[1] for programming of PIPE1 parameters
extern Adsoc_PK_Ctrl_ts     g_Adsoc_PK_Ctrl[];

/// Control Page element for adsoc radial peaking block (Both PIPE0 and PIPE1)
/// g_Adsoc_RP_Ctrl[0] for programming of PIPE0 parameters
/// g_Adsoc_RP_Ctrl[1] for programming of PIPE1 parameters
extern Adsoc_RP_Ctrl_ts     g_Adsoc_RP_Ctrl[];

/// Control Page element for adsoc radial peaking block (Both PIPE0 and PIPE1)
/// g_Adsoc_RP_Ctrl[0] for programming of PIPE0 parameters
/// g_Adsoc_RP_Ctrl[1] for programming of PIPE1 parameters
extern Adsoc_RP_Status_ts   g_Adsoc_RP_Status[];

// Declare structure for damper pipe 0
//(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, u8_Parameters, damper_name, damper_addr)
//Shared_Damper(ADSOC_DAMPER_DIMENSION, ADSOC_DAMPER_POINTS_ON_BASIS_0, ADSOC_DAMPER_POINTS_ON_BASIS_1, ADSOC_DAMPER_NUM_OF_PARAMETERS_IN_SHARED_DAMPER, ADSOC_0, ADSOC_0_DAMPER_ADDR);
// CoringLevel
//, PK_Gain
// Declare structure for damper pipe 1
//(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, u8_Parameters, damper_name, damper_addr)
//Shared_Damper(ADSOC_DAMPER_DIMENSION, ADSOC_DAMPER_POINTS_ON_BASIS_0, ADSOC_DAMPER_POINTS_ON_BASIS_1, ADSOC_DAMPER_NUM_OF_PARAMETERS_IN_SHARED_DAMPER, ADSOC_1, ADSOC_1_DAMPER_ADDR);
// CoringLevel
//, PK_Gain

/// this function programs the Colour Engine with the appropriate image sharpening parameters
void Adsoc_Commit (uint8_t e_PipeNo) TO_EXT_DDR_PRGM_MEM;
#endif /* ADSOC_H_ */

