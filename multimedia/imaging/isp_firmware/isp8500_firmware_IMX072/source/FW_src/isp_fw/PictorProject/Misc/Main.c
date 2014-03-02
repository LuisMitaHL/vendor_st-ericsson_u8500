/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  Main.c

 \brief This file hosts the main() function. It contains the C entry point into the firmware.

 \ingroup Miscellaneous
 \endif
*/

// TODO: Remove the include of PictorhwReg.h when The user if enable is moved out
#include "PictorhwReg.h"
#include "InterruptManager.h"
#include "EventManager.h"
#include "HostInterface.h"
#include "ZoomTop.h"
#include "HostComms.h"
#include "DeviceParameter.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Misc_MainTraces.h"
#endif

//void    PowerUpInit (void)TO_EXT_DDR_PRGM_MEM;
void    PowerUpInit (void);
void    ReadHWRevision (void);

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn            void main(void)
 \brief         This function is the high level entry point to the
                device firmware. It must perform all the power up
                initialisation and finally invoke the device
                state machine in a loop.
 \return        void
 \callgraph
 \callergraph
 \ingroup       Miscellaneous
 \endif
*/
int
main(void)
{
    // Initialise the low level blocks
    PowerUpInit();

    _asm_mover2e_psta(0); //Enabling the pcache

    // Power up initialisation done
    // Invoke the state machine in a loop
    while (1)
    {
        HostInterface_Run();
        Stream_StateMachine();
    }
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn            void PowerUpInit(void)
 \brief         This function performs initialisation of IPs required at power on time.
 \return        void
 \callgraph
 \callergraph
 \ingroup       Miscellaneous
 \endif
*/
void
PowerUpInit(void)
{
    // Read the Hardware Revision
    ReadHWRevision();

    // we will use this function to temporarily do setup of
    // things that will eventually move to other modules
    // Initialise the STXP70 interrupt manager
    InterruptManager_Initialise();

    ZoomTop_Initialise();

#if HOST_COMMS_DEBUG
    g_HostComms_PEDump.u32_Ptr_to_pageDump = (uint16_t)(&g_PageRegDump[0]);
#endif
    // enable the Host Comms IP
    Set_USER_IF_USER_IF_ENABLE_word(1);


     // Function to map register of STM
    #if ((USE_OST_TRACES == PROTOCOL_ID_ASCII) || (USE_OST_TRACES == PROTOCOL_ID_BINARY))
    Map_STM_Registers();
    #endif

    //Print FW version g_DeviceParameters
    OstTraceInt3(TRACE_DEBUG, "FW Version     : %d.%d.%d", g_DeviceParameters.u32_FirmwareVersionMajor, g_DeviceParameters.u32_FirmwareVersionMinor, g_DeviceParameters.u32_FirmwareVersionMicro);
    OstTraceInt1(TRACE_DEBUG, "Sensor         : %d",g_DeviceParameters.u32_LLA_Sensor);
    OstTraceInt2(TRACE_DEBUG, "LLA Version    : %d.%d", g_DeviceParameters.u32_LLA_MajorVersion, g_DeviceParameters.u32_LLA_MinorVersion);
    OstTraceInt2(TRACE_DEBUG, "LLCD Version   : %d.%d", g_DeviceParameters.u32_LLCD_MajorVersion, g_DeviceParameters.u32_LLCD_MinorVersion);

    // Raise an event notification for HOST_COMMS_READY
    EventManager_HostCommsReady_Notify();
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn            void ReadHWRevision(void)
 \brief         This function reads the Hardware Revision Register SIA_IDN_HRV.
 \return        void
 \callgraph
 \callergraph
 \ingroup       Miscellaneous
 \endif
*/
void
ReadHWRevision(void)
{
    // read SIA_IDN_HRV, bits [11:8]
    uint16_t  hw_revision = (*(uint16_t*)(0xF0002020) & 0x0F00) >> 8;

    //Switch case added for hardware selection
     switch (hw_revision) 
    {

     case 4: g_DeviceParameters.e_SiliconVersion = SiliconVersion_e_8500v1; break;
     case 5: g_DeviceParameters.e_SiliconVersion = SiliconVersion_e_8500v2; break;
     case 8: g_DeviceParameters.e_SiliconVersion = SiliconVersion_e_9540v1; break;
     case 0xC: g_DeviceParameters.e_SiliconVersion = SiliconVersion_e_8540v1; break;
     default: ASSERT_XP70(); break;
    
    } 
    return;
}

