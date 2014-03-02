/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: 
 * 
 */

/**
 \defgroup  PageElement Debug interface

 \details   This module provides an debug interface for reading/writing page elements.
 \n         This used fixed address struct for communication between Host and FW
            
*/

#ifndef _PEDEBUG_H_
#define _PEDEBUG_H_

#include "Platform.h"

#ifdef PEDEBUG_ENABLE

/**
 * \if        INCLUDE_IN_HTML_ONLY
 * \struct    DEBUG_PEInterface_ts
 * \brief     Used to access page elements directly from host environment.
 * \details   Struct DEBUG_PEInterface is assigned a fix address.
 *            It is placed at the end of IDM before grab_notify struct.
 *            With this fixed address, this stuct can be accessed by HOST
 *            and can be used to send command.
 * \ingroup   PEDebug
 * \endif
 */
typedef struct DEBUG_PEInterface
{
    /// PE address to be access
    uint32_t    u32_PEAddress;

    /// PE Value
    uint32_t    u32_PEVal;

    /// Size of PE
    uint32_t    u32_PESize;

    /// Read/Write
    uint32_t    u32_PEAccess;

    /// Control/Status - for sending the command
    uint32_t    u32_PEControl;

    /// Control/Status - for sending the command
    uint32_t    u32_PEStatus;

    /// Dummy1
    uint32_t    u32_dummy_1;

    /// Dummy2
    uint32_t    u32_dummy_2;
}DEBUG_PEInterface_ts;

//Fix address is alloted to this struct
extern volatile DEBUG_PEInterface_ts g_DebugPEInterface TO_FIXED_EXT_DATA_2_ADDR_MEM;

// fwd decl
void DebugPE_ProcessUserInterfaceReq(void);

//OP interface for State machine
#define PEDEBUG_CHECKREQ() DebugPE_ProcessUserInterfaceReq() 

#endif // PEDEBUG_ENABLE

#endif //ifndef _PEDEBUG_H_
