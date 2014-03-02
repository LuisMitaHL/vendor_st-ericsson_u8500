/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup ClockManager Clock Manager

 \detail This module provides a driver layer for the CRM (Clock Reset Manager)
*/

/**
 \file CRM.c
 \brief It implements a driver layer for the CRM.
 \ingroup ClockManager
*/
#include "CRM.h"
#include "CRMhwReg.h"
#include "ITM.h"
#include "videotiming_op_interface.h"
#include "SystemConfig.h"
#include "EventManager.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_ClockManager_CRMTraces.h"
#endif
volatile CRM_Status_ts  g_CRM_Status =
{
    DEFAULT_WINDOW_SIZE,
    0,
    0,
    0,
    0,
    0,
    0,
    Flag_e_FALSE,
    Flag_e_FALSE,
    DEFAULT_SENSOR_CLOCKS_AVAILABLE
};

void
CRM_Initialise(void)
{
    // Initialise CRM
    // switch to emulated sensor clocks
    CRM_SwitchToEmulatedSensorClocks();

    Set_CRM_CRM_EN_CLK_BML(0);                      // CRM_EN_CLK_BML
    Set_CRM_CRM_EN_CLK_BMS(0);                      // CRM_EN_CLK_BMS
    Set_CRM_CRM_EN_CLK_PIPE_IN(0);                  // CRM_EN_CLK_PIPE_IN

    // Enable All Clocks
    CRM_SetClocks(Flag_e_TRUE);

    // set the CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL to be divided clock host ipp
    //Till  16.0 release we wree using different clock freq for GPIO's before boot and after boot. Now we no longer needs this kind of implementation. So fixing the GPIO clock source as HOST_CLOCK.
    Set_CRM_CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL(STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_B_0x2);

    // set the CRM_CLK_HOST_IPP_DIV (it remains the same always)
    Set_CRM_CRM_CLK_HOST_IPP_DIV(DEFAULT_CLK_HOST_IPP_DIV);

    // set the CRM_CLK_PIPE_IN_DIV (it remains the same always)
    Set_CRM_CRM_CLK_PIPE_IN_DIV(g_SystemSetup.u8_crm_clk_pip_in_div);

    // Set the CRM_CLK_PICTOR_GPIO_DIV (for the time being it will remain the same)
    Set_CRM_CRM_CLK_PICTOR_GPIO_DIV(DEFAULT_CLK_PICTOR_GPIO_DIV);

    return;
}


void
CRM_SwitchToEmulatedSensorClocks(void)
{
    Set_CRM_CRM_EMUL_SENSOR_CLOCK(1);
}


void
CRM_SwitchToRealSensorClocks(void)
{
    Set_CRM_CRM_EMUL_SENSOR_CLOCK(0);
}


void
CRM_SetClocks(
uint8_t e_Flag_ClocksEnable)
{
    if (Flag_e_TRUE == e_Flag_ClocksEnable)
    {
        // enable all the clocks
        Set_CRM_CRM_EN_CLK_SD_HISPEED(1, 1);        // CRM_EN_CLK_SD_HISPEED
        Set_CRM_CRM_EN_CLK_SD_LOSPEED(1, 1);        // CRM_EN_CLK_SD_LOSPEED
        if(Is_8540v1())
         {
           Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 1);  // CRM_EN_CLK_PIPE  //EN_CLK_PIPE_DXO 
         }
        else
         {
          Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 0);  // CRM_EN_CLK_PIPE //EN_CLK_PIPE_DXO  NOT PRESENT
         }
        Set_CRM_CRM_EN_CLK_PICTOR_GPIO(1);          // CRM_EN_CLK_PICTOR_GPIO
    }
    else
    {
        // disable all the clocks
        Set_CRM_CRM_EN_CLK_SD_HISPEED(0, 0);        // CRM_EN_CLK_SD_HISPEED
        Set_CRM_CRM_EN_CLK_SD_LOSPEED(0, 0);        // CRM_EN_CLK_SD_LOSPEED
        Set_CRM_CRM_EN_CLK_PIPE(0, 0, 0, 0, 0, 0, 0);  // CRM_EN_CLK_PIPE  //EN_CLK_PIPE_DXO  NOT PRESENT
        Set_CRM_CRM_EN_CLK_PICTOR_GPIO(0);          // CRM_EN_CLK_PICTOR_GPIO
    }
}


void
CRM_PreRunUpdate(void)
{   // Hem : please find out this information
    uint16_t    u16_Window_max = 0;
    uint16_t    u16_Window_min = 0;

    //uint32_t    u32_Count = 0;
    //uint32_t    u32_TimeOut = 0xFFFF;
    // program the CRM checker detect window
    // from STn8500 SIA hardware architecture specification page 282 v1.6:
    // the constraints governing the programming of CRM_CHECKER_DETECT_WDW is:
    // 1.>
    // Fin > 2Fhost / (CRM_CHECKER_DETECT_WDW +1)
    // ==> CRM_CHECKER_DETECT_WDW > (2Fhost/Fin) - 1
    // ==> CRM_CHECKER_DETECT_WDW >= (2Fhost/Fin)   ... since CRM_CHECKER_DETECT_WDW is integer
    // 2.>
    // Fin < /(CRM_CHECKER_DETECT_WDW +1)
    // ==> CRM_CHECKER_DETECT_WDW < (DFhost/Fin) - 1
    //
    // where D = 255,
    // Fhost = clk_host_ipp and
    // Fin = clock coming from the sensor
    // f_SensorClockFrequency_Mhz should be considered in Pixels per second as opposed to Mhz
    // see Bug #71604 on Pictor ISP codex tracker for details.
    // TODO: replace VideoTiming_GetCsiRawFormat with an actual op if for the streaming mode wrt number of
    // bits per pixel
    //  Fin < DFhost/(CRM_CHECKER_DETECT_WDW +1).
    // and D = 255
    u16_Window_max = (255 * g_SystemSetup.f_ClockHost_Mhz / g_VideoTimingOutput.f_VTPixelClockFrequency_Mhz) - 1;

    // Fin > 4Fhost / (CRM_CHECKER_DETECT_WDW +1)
    u16_Window_min = (4 * g_SystemSetup.f_ClockHost_Mhz / g_VideoTimingOutput.f_VTPixelClockFrequency_Mhz) - 1;
    if (u16_Window_max < u16_Window_min)
    {
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!CRM_PreRunUpdate() -  inconsistent values of u16_Window_max and u16_Window_min encountered!!");
        ASSERT_XP70();
    }


    g_CRM_Status.u16_Window = (u16_Window_max + u16_Window_min) / 2;

    // program this value into the register
    Set_CRM_CRM_CHECKER_DETECT_WDW(g_CRM_Status.u16_Window);
}


void
CRM_ISR(void)
{
    if (Get_CRM_CRM_CHECKER_ITM_CKC_OK_ITM() && Get_CRM_CRM_CHECKER_ITS_CKC_OK_ITS())
    {
        g_CRM_Status.u16_interrupt_0_CKC_OK_ITS++;

        OstTraceInt1(TRACE_FLOW, "->CRM_ISR: CKC_OK: %d ", g_CRM_Status.u16_interrupt_0_CKC_OK_ITS);

        if (Flag_e_TRUE == g_CRM_Status.e_Flag_StartEnabled)    // Start sensor sequence initiated
        {
            // clocks are available, switch to sensor clocks
            CRM_SwitchToRealSensorClocks();

            g_CRM_Status.e_Flag_StartEnabled = Flag_e_FALSE;
            g_CRM_Status.e_Flag_SensorClocksAvailable = Flag_e_TRUE;
        }
        else    // ok interrupt coming without valid sequence, !!! Warning !!!
        {
            g_CRM_Status.u16_interrupt_0_CKC_OK_ITS_warning++;

            // increment warning counter
            OstTraceInt1(TRACE_WARNING, "<Warning>!!CRM_ISR() - Clock ok interrupt recieved without starting sensor !!: %d",g_CRM_Status.u16_interrupt_0_CKC_OK_ITS_warning);
        }


        Set_CRM_CRM_CHECKER_ITS_BCLR(1, 0, 0, 0);
    }
    else if (Get_CRM_CRM_CHECKER_ITM_CKC_LOST_ITM() && Get_CRM_CRM_CHECKER_ITS_CKC_LOST_ITS())
    {
        g_CRM_Status.u16_interrupt_0_CKC_LOST_ITS++;
        OstTraceInt1(TRACE_FLOW, "->CRM_ISR: CKC_LOST: %d ", g_CRM_Status.u16_interrupt_0_CKC_LOST_ITS);

        if (Flag_e_TRUE == g_CRM_Status.e_Flag_StopEnabled) // Stop sequence initiated
        {
            CRM_SwitchToEmulatedSensorClocks();
            g_CRM_Status.e_Flag_StopEnabled = Flag_e_FALSE;
            g_CRM_Status.e_Flag_SensorClocksAvailable = Flag_e_FALSE;

            // send msg to LLCD that sensor is stopped and break delay loop
             cam_drv_signal(CAM_DRV_SIGNAL_FEC);
        }
        else    //  loss interrupt coming without valid sequence, !!! Warning !!!
        {
            g_CRM_Status.u16_interrupt_0_CKC_LOST_ITS_warning++;

            // increment warning counter
            OstTraceInt1(TRACE_WARNING, "<Warning>!!CRM_ISR() - Clock loss interrupt recieved without stopping sensor !!: %d", g_CRM_Status.u16_interrupt_0_CKC_LOST_ITS_warning);
        }


        Set_CRM_CRM_CHECKER_ITS_BCLR(0, 1, 0, 0);
    }
    else if (Get_CRM_CRM_CHECKER_ITS_CKC_OK_3D_ITS())
    {
        g_CRM_Status.u16_interrupt_0_CKC_OK_3D_ITS++;
        OstTraceInt1(TRACE_FLOW, "->CRM_ISR: CKC_OK_3D: %d ", g_CRM_Status.u16_interrupt_0_CKC_OK_3D_ITS);
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!CRM_ISR() - Interrupt recieved for 3D !!");
        ASSERT_XP70();
    }
    else
    {
        g_CRM_Status.u16_interrupt_0_CKC_LOST_3D_ITS++;
        OstTraceInt1(TRACE_FLOW, "->CRM_ISR: CKC_LOST_3D: %d ", g_CRM_Status.u16_interrupt_0_CKC_LOST_3D_ITS);
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!CRM_ISR() - Interrupt recieved for 3D !!");
        ASSERT_XP70();
    }


    return;
}

