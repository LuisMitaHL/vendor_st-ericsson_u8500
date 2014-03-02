/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      SleepManager.c

 \brief     This file implements the main Sleep Manager interface function
            which is invoked in context of SLEEP and WAKE commands to the device.

 \ingroup   SleepManager
 \endif
*/
#include "SleepManager.h"
#include "EventManager.h"
#include "SystemConfig.h"
#include "GPIOManager_OpInterface.h"

#include "CRM.h"

extern void lla_abstractionSensorOff ( void );
extern void lla_abstractionSensorInit ( void ) ;

/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void SleepManager_Sleep( void )
  \brief        This is the main function that is to be invoked in context of a SLEEP command
                to the device. At the end of the function, a SLEEPING event notification is
                generated to the host.
  \return       void
  \callgraph
  \callergraph
  \ingroup      SleepManager
  \endif
*/
void
SleepManager_Sleep(void)
{
    lla_abstractionSensorOff();

    // disable all CRM clocks
    CRM_SetClocks(Flag_e_FALSE);

    // Checking for processor idle modes now
    // This will flush all pending operations
    _asm_barrier();

    // Each of these modes encapsulates the previous mode features
    // Therefore, each of these modes saves more energy than its previous one.

    //IDLE 0 : Stop fetching instructions
    if (IdleMode_e_IDLE_MODE_0 == g_SystemSetup.e_IdleMode_Ctrl)
    {
      _asm_idle_mode0();
    }
    //IDLE 1 : Stop clock on core logic (DMA access possible)
    else if (IdleMode_e_IDLE_MODE_1 == g_SystemSetup.e_IdleMode_Ctrl)
    {
      _asm_idle_mode1();
    }
    //IDLE 2 : Stop clock on core logic and memories
    else if (IdleMode_e_IDLE_MODE_2 == g_SystemSetup.e_IdleMode_Ctrl)
    {
      _asm_idle_mode2();
    }
    //IDLE 3 : Stop clock on core logic and memories and Interrupt & Event controllers
    else if (IdleMode_e_IDLE_MODE_3 == g_SystemSetup.e_IdleMode_Ctrl)
    {
      _asm_idle_mode3();
    }

#ifdef TEST_IDLE_MODES
      //g_GrabNotify.u32_TEST_Idle_Mode = 0x1A2B3C4D;
#endif

    EventManager_Sleeping_Notify();

    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void SleepManager_Wakeup( void )
  \brief        This is the main function that is to be invoked in context of a WAKE command
                to the device. At the end of the function, a WOKEN_UP event notification is
                generated to the host.
  \return       void
  \callgraph
  \callergraph
  \ingroup      SleepManager
  \endif
*/
void
SleepManager_Wakeup(void)
{
    // enable all the clocks
    CRM_SetClocks(Flag_e_TRUE);
    lla_abstractionSensorInit();

    //Set camera driver to in on state
    SET_LLA_CAM_DRV_STATE(e_LLA_Abstraction_CamDrvState_WakeUP);
    EventManager_WokenUp_Notify();
    return;
}

