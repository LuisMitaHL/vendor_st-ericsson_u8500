/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef HOSTCOMMS_
#define HOSTCOMMS_
/**
 \defgroup  HostComms Host Comms
 \details   The host comms module manages the communication interface between
            the device and the host. The current implementation uses a mailbox
            based commincation mechanism.
*/

/**
 \if    INCLUDE_IN_HTML_ONLY
 \file  HostComms.h

 \brief This file is a part of the release code. It contains all the data
        structures, macros and function declarations used by the module.
        The entities defined in the file are not supposed to be accessed
        directly. The relevant interface exposed through the HostComms_OPInterface.h
        file should be used instead.

 \ingroup HostComms
 \endif
*/

#include "Platform.h"


typedef enum
{
    /// Specifies the status of the last host comms operation.
    /// Signifies that the host comms module is in its default state
    /// and no read and write operation has happened till now.
    HostComms_Status_e_None,

    /// Specifies the status of the last host comms operation.
    /// Signifies that the last host comms operation was a
    /// read operation and it was completed successfully.
    HostComms_Status_e_ReadComplete,

    /// Specifies the status of the last host comms operation.
    /// Signifies that the last host comms operation was a
    /// write operation and it was completed successfully.
    HostComms_Status_e_WriteComplete,

    /// Specifies the status of the last host comms operation.
    /// Signifies that the last host comms operation was a
    /// write operation on a read only page and it was
    /// denied by the module.
    HostComms_Status_e_ReadOnlyWriteDenied,

    /// Specifies the status of the last host comms operation.
    /// Not in use currently.
    HostComms_Status_e_InvalidAccess
} HostComms_Status_te;


/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    HostComms_Status_ts
 \brief     Status page of the Host Comms Module.
            Contains information about the last
            operation attempted by the host.
 \ingroup   HostComms
 \endif
*/
typedef struct
{
    /// Specifies the page number and the byte offset accessed
    /// in the last read or write operation.
    /// It is in encoded form, the top 16 bits denote the page
    /// number, the bottom 16 bits denote the page offset.
    uint32_t u32_PageNumberAndOffset;

    /// Specifies the status of the last read/write operation.
    uint8_t e_HostComms_Status;

    /// Set to TRUE if a mode static page is written onto.
    bool_t  bo_ModeStaticSetupChanged;

} HostComms_Status_ts;

typedef struct
{

    uint32_t u32_Ptr_to_pageDump;

} HostComms_PEDump_ts;

typedef struct PageReg{
    uint32_t page;
    uint32_t opc;
    uint32_t data;
    uint32_t ack;
}PageReg_ts;

extern HostComms_PEDump_ts g_HostComms_PEDump;

#if HOST_COMMS_DEBUG
extern PageReg_ts g_PageRegDump[];
#endif

#endif// HOSTCOMMS_


