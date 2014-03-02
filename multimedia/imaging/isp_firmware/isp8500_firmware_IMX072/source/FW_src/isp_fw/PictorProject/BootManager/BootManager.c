/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      BootManager.c

 \brief     This file implements the main Boot Manager interface function
            which is invoked in context of a BOOT command to the device.

 \ingroup   BootManager
 \endif
*/
#include "BootManager.h"
#include "EventManager.h"
#include "CRM.h"
#include "Platform.h"
#include "GPIOManager_OpInterface.h"
#include "IPPhwReg.h"
#include "SystemConfig.h"
#include "DeviceParameter.h"

// include Auto Focus 4 Modules (AutoFocus+AFstats+FocusControl+FLADriver)
#if INCLUDE_FOCUS_MODULES
#   include "FocusControl_OPInterface.h"
#endif
#include "lla_abstraction.h"
#include "Sensor_Tuning.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_BootManager_BootManagerTraces.h"
#endif




/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void BootManager_Boot( void )
  \brief        This is the main function that is to be invoked in context of a BOOT command
                to the device. Care must be taken by the host to ensure that the inputs to
                all the modules that are initialised by the Boot Manager are valid before
                the BOOT command is issued. At the end of the function, a BOOT_COMPLETE
                notification is generated.
  \return       void
  \callgraph
  \callergraph
  \ingroup      BootManager
  \endif
*/
void
BootManager_Boot(void)
{
    /// \par Implementation
    /// The following operations are performed in sequence:
    /// - Raise the XSHUTDOWN signal of the active sensor
    /// - Perform the master i2c initialisation
    /// - Read the sensor parameters like model id, revision number, manufacturer id, SMIA revision, video timing constraints and frame dimension constraints.
    /// - Raise a BOOT_COMPLETE event notification
    //uint16_t    u16_GPIOEnableMask;
            CRM_Initialise();
            //GPIO_Init();
            GPIOManager_Initialize();

            //Start Global reference timer
            GPIOManager_StartGlobalTimer(g_gpio_control.u32_GlobalTimerTimeout_us);

            //Trace log initialization
#if TRACE_LOG
            TraceLog_Init();
#endif
            // Disable all those IP blocks in ISP that access ISP memories,
            // These memories are accessed by HOST in stopped state
            DisableMemoryBasedHardware();

            // MasterI2C initialization can be done only after providing external clock and voltages to the sensor.
            // So, in case of smia++ power-up sequence is enabled by HOST, post-pone MasterI2C_ModeStaticInitialise
            // till external clocks are supplied
            if(Flag_e_FALSE == g_SensorPowerManagement_Control.e_Flag_EnableSMIAPP_PowerUpSequence)
            {
                MasterI2C_ModeStaticInitialise();
            }

            g_FrameParamStatus_Extn.u32_SizeOfFrameParamStatus = sizeof(FrameParamStatus_ts) + sizeof(FrameParamStatus_Extn_ts);
            g_FrameParamStatus_Af.u32_SizeOfFrameParamStatus = sizeof(FrameParamStatus_Af_ts);

            lla_abstractionInit();

        // perform the DPHY compensation
        if (SystemConfig_IPPSetupRequired())
        {
            // Enable compensation clock
            Set_IPP_IPP_DPHY_COMPENSATION_CTRL_CLK_1_5M_COMPENSATION_EN(1);

            // Start the compensation
            Set_IPP_IPP_DPHY_COMPENSATION_CTRL_DPHY_COMPENSATION_START(1);

            //  Poll on IPP_DPHY_COMPENSATION_STATUS register to detect the end of the compensation
            while (1 != Get_IPP_IPP_DPHY_COMPENSATION_STATUS_DPHY_COMPENSATION_OK())
                ;

            // Disable compensation clock
            Set_IPP_IPP_DPHY_COMPENSATION_CTRL_CLK_1_5M_COMPENSATION_EN(0);
        }

        // Handle zoom commands if any
#if INCLUDE_FOCUS_MODULES
        // we allow direct actuator control only if
        //  - the actuator is present and
        //  - the system has booted
        FocusControl_BootInitialize();
#endif //INCLUDE_FOCUS_MODULES

        OstTraceInt0(TRACE_DEBUG, "Sensor is present, The details =>");
        OstTraceInt1(TRACE_DEBUG, "ModelID               : IIII => 0x%X\n", g_SensorInformation.u16_model_id);
        OstTraceInt1(TRACE_DEBUG, "Module Revision       : RR   => 0x%X\n", g_SensorInformation.u8_revision_number);
        OstTraceInt1(TRACE_DEBUG, "Manufacturer ID       : MM   => 0x%X\n", g_SensorInformation.u8_manufacturer_id);
        OstTraceInt1(TRACE_DEBUG, "SMIA Version          : SM   => 0x%X\n", g_SensorInformation.u8_smia_version);
    
        EventManager_BootComplete_Notify();
        g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;
    return;
}

