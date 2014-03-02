/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Scorpio.h
 */
#ifndef SCORPIO_H_
#   define SCORPIO_H_

#   include "Platform.h"
#   include "Scorpio_ip_interface.h"

typedef enum
{
    // manual mode - values given by host are directly written to hw
    ScorpioMode_e_Manual,

    // in this mode, the values provided by host are damped & then applied on to the hw
    ScorpioMode_e_Adaptive
} ScorpioMode_te;

typedef struct
{
    /// enable/disable scorpio block
    uint8_t e_Flag_ScorpioEnable;

    // mode of Scorpio ip
    uint8_t e_ScorpioMode;

    /// coring level ctrl
    uint8_t u8_CoringLevel_Ctrl;

    /// coring status
    uint8_t u8_CoringLevel_Status;
} Scorpio_Ctrl_ts;

/// Ctrl page for scorpio block
extern Scorpio_Ctrl_ts      g_Scorpio_Ctrl;

// Declare structure for damper
//(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, u8_Parameters, damper_name, damper_addr)

// CoringLevel

/// this function programs the recovery engine with appropriate scorpio parameters
void                        Scorpio_Commit (void) TO_EXT_DDR_PRGM_MEM;

/// calculates the values that are to be programmed on to the scorpio hardware block
void                        Scorpio_Update (void) TO_EXT_DDR_PRGM_MEM;

/// specifies whether damper is enabled foe Scorpio hw block
#   define Scorpio_IsDamperEnabled()   (ScorpioMode_e_Adaptive == g_Scorpio_Ctrl.e_ScorpioMode)
#endif /* SCORPIO_H_ */

