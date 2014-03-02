/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file CRM.h
 \brief It provides an interface to the CRM driver.
 \ingroup ClockManager
*/
#ifndef CRM_H_
#   define CRM_H_

#   include "Platform.h"

#define DEFAULT_HOST_CLOCK              (48.0)
#define DEFAULT_WINDOW_SIZE             (12)

#define DEFAULT_SENSOR_CLOCKS_AVAILABLE (Flag_e_FALSE)

#define DEFAULT_CLK_HOST_IPP_DIV        (4)

// Divider value = 8 means dividing input clock by 1
#define DEFAULT_CLK_PIPE_IN_DIV         (8)

#define DEFAULT_CLK_PICTOR_GPIO_DIV     (4)

#define Is_CRM_CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_B_0x2() (p_CRM_IP->CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL.CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_ts.STATIC_CLK_PICTOR_GPIO_SOURCE_SEL == STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_B_0x2)
#define Set_CRM_CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL__B_0x2() (p_CRM_IP->CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL.CRM_STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_ts.STATIC_CLK_PICTOR_GPIO_SOURCE_SEL = STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_B_0x2)
#define STATIC_CLK_PICTOR_GPIO_SOURCE_SEL_B_0x2 0x02    //divided clk_host_ipp


typedef struct
{
    /// Specifies the value of the CRM_CHECKER_DETECT_WDW register
    /// computed and applied
    uint16_t    u16_Window;

    // Count of occurrences of clock_ok interrupt
    uint16_t    u16_interrupt_0_CKC_OK_ITS;

    // Count of occurrences of clock_loss interrupt
    uint16_t    u16_interrupt_0_CKC_LOST_ITS;

    // Count of occurrences of clock_ok_3d interrupt
    uint16_t    u16_interrupt_0_CKC_OK_3D_ITS;

    // Count of occurrences of clock_loss_3d interrupt
    uint16_t    u16_interrupt_0_CKC_LOST_3D_ITS;

    // Count of invalid occurrences of clock_ok interrupt.
    uint16_t    u16_interrupt_0_CKC_OK_ITS_warning;

    // Count of invalid occurrences of clock_loss interrupt.
    uint16_t    u16_interrupt_0_CKC_LOST_ITS_warning;

   // This flag specifies whether a start sequence is enabled or not.
    uint8_t     e_Flag_StartEnabled;

    // This flag specifies whether a start sequence is enabled or not.
    uint8_t     e_Flag_StopEnabled;

    /// Specifies if the sensor clocks are available
    /// Set to Flag_e_TRUE if clocks are available
    /// Set to Flag_e_FALSE if clocks are not available
    uint8_t     e_Flag_SensorClocksAvailable;
} CRM_Status_ts;

extern volatile CRM_Status_ts   g_CRM_Status;

extern void CRM_Initialise ( void ) TO_EXT_DDR_PRGM_MEM;
extern void CRM_SwitchToEmulatedSensorClocks ( void ) TO_EXT_DDR_PRGM_MEM;
extern void CRM_SwitchToRealSensorClocks ( void ) TO_EXT_DDR_PRGM_MEM;
extern void CRM_PreRunUpdate ( void ) TO_EXT_DDR_PRGM_MEM;
extern void CRM_ISR ( void ) TO_EXT_DDR_PRGM_MEM;
extern void CRM_SetClocks (uint8_t e_Flag_ClocksEnable) TO_EXT_DDR_PRGM_MEM;

#   define CRM_GetIsSensorClockAvailable()     (Flag_e_TRUE == g_CRM_Status.e_Flag_SensorClocksAvailable)
#   define CRM_GetIsSensorClockUnavailable()   (Flag_e_FALSE == g_CRM_Status.e_Flag_SensorClocksAvailable)
#   define CRM_SetSensorClockAvailable()       (g_CRM_Status.e_Flag_SensorClocksAvailable = Flag_e_TRUE)
#   define CRM_SetSensorClockUnavailable()     (g_CRM_Status.e_Flag_SensorClocksAvailable = Flag_e_FALSE)

#endif /*CRM_H_*/

