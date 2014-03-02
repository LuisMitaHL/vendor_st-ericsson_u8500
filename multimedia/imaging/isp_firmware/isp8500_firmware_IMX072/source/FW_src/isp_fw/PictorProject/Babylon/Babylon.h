/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Babylon.h
 */
#ifndef BABYLON_H_
#   define BABYLON_H_

#   include "Platform.h"
#   include "Babylon_ip_interface.h"

typedef enum
{
    // manual mode - values given by host are directly written to hw
    BabylonMode_e_Manual,

    // in this mode, the values provided by host are damped & then applied on to the hw
    BabylonMode_e_Adaptive
} BabylonMode_te;

typedef struct
{
    /// enable/disable the hw block
    uint8_t e_Flag_BabylonEnable;

    /// zipper vs text trade-off control
    uint8_t u8_ZipperKill;

    /// flat region detect threshold
    uint8_t u8_Flat_Threshold;

    /// flat region detect threshold status, contains damped value
    uint8_t u8_Flat_Threshold_Status;

    /// Babylon operation mode
    uint8_t e_BabylonMode;
} Babylon_Ctrl_ts;

/// control page of Babylon
extern Babylon_Ctrl_ts  g_Babylon_Ctrl;

// Declare structure for damper
//(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, u8_Parameters, damper_name, damper_addr)

// Flat_threshold

/// function to commit babylon parameters
void                    Babylon_Commit (void);

/// function to commit babylon parameters
void                    Babylon_Update (void);

/// specifies whether damper is enabled for Babylon hw block
#   define Babylon_IsDamperEnabled()   (BabylonMode_e_Adaptive == g_Babylon_Ctrl.e_BabylonMode)
#endif /* BABYLON_H_ */

