/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file GPIOManager_Op_interface.h
 \brief Export GPIO Manager Functions and macros to other module
 \details  The file is part of release code and exports functions and macros which may be required
           by other modules.The parameters exported by the module are read only i.e. no other module
           should change the GPIO Manager parameters and external modules should only use the
           exported macros in this file.
 \ingroup GPIO Manager
*/
#ifndef GPIOMANAGER_OPINTERFACE_H_
#define GPIOMANAGER_OPINTERFACE_H_

#   include "Platform.h"
#   include "GPIOManager_PlatformSpecific.h"

/**
 \struct GPIOControl_ts
 \brief This page facilitates flexible GPIO mapping.The host can program the
        GPIO pins before Boot.
 \ingroup GPIO Manager
*/
typedef struct
{
    /// Sensor 0 XShutDown GPO to be programmed by the host.
    uint8_t u8_Sensor0XShutdownGPO;

    /// Sensor 1 XShutDown GPO to be programmed by the host.
    uint8_t u8_Sensor1XShutdownGPO;

    /// Main flash GPIO to be programmed by the host.
    uint8_t u8_FlashGPO;
} GPIOControl_ts;

/**
 \struct gpio_debug_ts
 \brief This page provides page elements for debugging of GPIO block.
 \ingroup GPIO Manager
*/
typedef struct
{
    float_t     f_Counter1;

    float_t     f_Counter2;

    /// [PM]For Debug: global reference counter
    uint32_t    u32_GlobalRefCounter;
 
    uint16_t    u16_Chnl0_Ctrl1;

    uint16_t    u16_Chnl0_Ctrl0;

    uint16_t    u16_Chnl0_Repeat;

    uint16_t    u16_Chnl1_Ctrl1;

    uint16_t    u16_Chnl1_Ctrl0;

    uint16_t    u16_Chnl1_Repeat;

    uint8_t     u8_PreScale;
} gpio_debug_ts;

/**
 \struct gpio_control_ts
 \brief This page provides page elements for GPIO hw reg.
 \ingroup GPIO Manager
*/
typedef struct
{
    uint32_t    u32_GPIO_delay_us;

    uint32_t    u32_GPIO_channel_src;

    uint32_t    u32_GPIO_channel0_ctrl0;

    uint32_t    u32_GPIO_channel0_ctrl1;

    uint32_t    u32_GPIO_channel0_ctrl0_ext;

    uint32_t    u32_GPIO_channel0_ctrl1_ext;

    /// Timeout (expiry) of global timer
    /// Default: 1000
    uint32_t    u32_GlobalTimerTimeout_us;
 
    uint16_t    u16_GPIO_channel0_repeat;

    uint16_t    u16_GPIO_channel0_ctrl0;

    uint16_t    u16_GPIO_channel0_ctrl1;

    uint8_t     u8_GPIO_gpd;

    uint8_t     u8_GPIO_gpo;

    uint8_t     u8_GPIO_config;

    uint8_t     e_Flag_Channel_Start;

    uint8_t     e_Flag_Channel_Stop;

    uint8_t     e_Flag_Channel_Reset;

    uint8_t     e_Flag_Channel_Polarity;

    uint8_t     u8_GPIO_channel_config;

    uint8_t     e_Flag_Debug_IDP_Timer;
} gpio_control_ts;

/*************** Page elements to be exported to user *********************/

/// GPIO Control Page
///  g_GPIOControl  NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
extern GPIOControl_ts   g_GPIOControl;
extern gpio_control_ts  g_gpio_control;
extern gpio_debug_ts    g_gpio_debug;

/*************** Procedures exported by module *********************/
extern void GPIOManager_Initialize ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_AssertSensor1Gpio ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_DeAssertSensor1Gpio ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_AssertSensor0Gpio ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_DeAssertSensor0Gpio ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_TriggerUpFlashOnStreaming (void);

extern void GPIOManager_TriggerDownFlashOnStreaming (void);

extern void GPIOManager_ISR (void);

extern void GPIOManager_ProgramGPIOForFlash ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_debug ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_Toggle_GPIO_Pin ( void ) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_Toggle_GPIO_Pin_Num (uint8_t u8_gpio_num) TO_EXT_DDR_PRGM_MEM;

extern void GPIOManager_Delay_us (float_t);
#   if INCLUDE_FOCUS_MODULES
extern void GPIOManager_StartIntTimerFLAD_us (uint32 u32_DelayUsec) TO_EXT_DDR_PRGM_MEM;
#   endif

/// Function added for implementing interrupt timer required for LLD
extern void GPIOManager_StartIntTimer_us (uint32 delay_usec);

extern void GPIOManager_StartGlobalTimer(uint32 u32_timeout_us) TO_EXT_DDR_PRGM_MEM;

/*************** Exported Preprocessing Macros *********************/

/// Get Sensor 0 XShutdown GPIO number
#   define GPIOManager_Sensor0XShutDownGPIONum (g_GPIOControl.u8_Sensor0XShutdownGPO)

/// Get Sensor 1 XShutdown GPIO number
#   define GPIOManager_Sensor1XShutDownGPIONum (g_GPIOControl.u8_Sensor1XShutdownGPO)

/// Get Main flash GPO
#   define GPIOManager_MainFlashGPO    (g_GPIOControl.u8_FlashGPO)

/// Main flash GPIO Mask
#   define GPIOManager_MainFlashGPOMask    (0x1 << GPIOManager_MainFlashGPO)

/// Pre Flash GPIO
#   define GPIOManager_PreFlashGPO (GPIOManager_MainFlashGPO + 1)

/// Pre flash GPO Mask
#   define GPIOManager_PreFlashGPOMask (0x1 << GPIOManager_PreFlashGPO)

/// Sensor 0 XShutDown Mask
#   define GPIOManager_Sensor0XShutdownMask    (0x1 << GPIOManager_Sensor0XShutDownGPIONum)

/// Sensor 1 XShutDown Mask
#   define GPIOManager_Sensor1XShutdownMask    (0x1 << GPIOManager_Sensor1XShutDownGPIONum)

/// Channel reset for Main Flash
#   define GPIOManager_FlashMainChannelAbort() Set_GPIO_GPIO_CHANNEL_RESET_word(GPIOManager_MainFlashGPOMask)

/// Channel reset for Pre Flash
#   define GPIOManager_FlashPreChannelAbort()  Set_GPIO_GPIO_CHANNEL_RESET_word(GPIOManager_PreFlashGPOMask)

/// Global Timer Timeout Duration
#define TOP_LEVEL_GLOBAL_TIMER_TIMEOUT_DURATION_US  1000

/// Global Counter Min Value
#define GLOBAL_COUNTER_MIN_VALUE                    0x0

/// Global Counter Max Value
#define GLOBAL_COUNTER_MAX_VALUE                    0xFFFFFFFFu

/// Global Timer Error Margin in Milliseconds
#define GLOBAL_TIMER_ERROR_MARGIN_MS                1

/// Whether XShutDown is present for Primary Sensor
#   define GPIOManager_IsSensor0XShutDownSupported()   (0xFF != g_GPIOControl.u8_Sensor0XShutdownGPO)

/// Whether XShutDown is present for Secondary Sensor
#   define GPIOManager_IsSensor1XShutDownSupported()   (0xFF != g_GPIOControl.u8_Sensor1XShutdownGPO)
#endif // GPIOMANAGER_OPINTERFACE_H_

