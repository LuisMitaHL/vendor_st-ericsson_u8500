/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file GPIOManager_ip_interface.h
 \brief This file is NOT a part of the module release code.
            All inputs needed by the GPIO Manager module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup GPIO Manager
*/
#ifndef _GPIOMANAGER_IP_INTERFACE_H_
#   define _GPIOMANAGER_IP_INTERFACE_H_

#   include "GPIOManager.h"
//#   include "FlashManager_OpInterface.h"
#   include "CRM.h"
#   include "videotiming_op_interface.h"
#   include "CRMhwReg.h"


/// Set the GPIO direction for input
#define GM_SET_GPIO_GPD_DIR_INPUT() Set_GPIO_GPIO_GPD_gpd__DIR_INPUT()

/// Set GPIO INT source
#define GM_SET_GPIO_INT_SRC_WRD(x)  Set_GPIO_GPIO_INT_SRC_word(x)

/// Get GPIO INT source
#define GM_GET_GPIO_INT_SRC()   Get_GPIO_GPIO_INT_SRC()

/// Get GPIO GPO
#define GM_GET_GPIO_GPO_gpo()   Get_GPIO_GPIO_GPO_gpo()

/// Set GPIO GPO word
#define GM_SET_GPIO_GPO_WRD(x)  Set_GPIO_GPIO_GPO_word(x)



/// Disable idp timer
#define GM_GPIO_TIMER_CTRL_IDPTIMER_DISABLE()   Set_GPIO_GPIO_TIMER_CTRL_idptimer_enable__DISABLE()





#define GM_SET_GPIO_TIMER_CTRL(timer0_enable,timer1_enable,timer2_enable,timer3_enable,idptimer_enable)    Set_GPIO_GPIO_TIMER_CTRL(timer0_enable,timer1_enable,timer2_enable,timer3_enable,idptimer_enable)

#define GM_SET_GPIO_TIMER_CTRL_WRD(x)       Set_GPIO_GPIO_TIMER_CTRL_word(x)

#define GM_GET_GPIO_TIMER_CTRL()            Get_GPIO_GPIO_TIMER_CTRL()

/// Set GPIO channel pairing
#define GM_SET_GPIO_CHANNEL_PAIRING(channel_pairing0,channel_pairing1,channel_pairing2,channel_pairing3)    Set_GPIO_GPIO_CHANNEL_PAIRING(channel_pairing0,channel_pairing1,channel_pairing2,channel_pairing3)




/// Set GPIO timer prescale
#define GM_SET_GPIO_TIMER_PRESCALE(x,y,z,w) Set_GPIO_GPIO_TIMER_PRESCALE(x,y,z,w)







/// Select the source of the flashgun IDP GPIO timer interrupt to IDP timer
#define GM_SET_GPIO_INT_SRC(x,y,z,w)    Set_GPIO_GPIO_INT_SRC(x,y,z,w)

/// Program frame count
#define GM_SET_FRAME_COUNT(x)   Set_GPIO_GPIO_INT1_TRIGGER_EX_count_ext(x)

/// Program line count
#define GM_SET_LINE_COUNT(x)    Set_GPIO_GPIO_INT1_TRIGGER_count_high(x)

/// Program pixel count
#define GM_SET_PIXEL_COUNT(x)   Set_GPIO_GPIO_INT1_TRIGGER_count_low(x)

/// Whether main and pre flash pulses are to be ored
#define GM_OR_MAIN_AND_PRE_FLASH_PULSE()    (Flag_e_TRUE)//(g_FlashManagerControl.e_Flag_OrMainAndPreFlashPulse)

/// Get inter pre flash distance
#define GM_GET_INTER_PRE_FLASH_DISTANCE()   (1000)//(g_FlashManagerControl.f_TimeBetweenTwoPreFlashes_us)

/// Get pre flash pulse width
#define GM_GET_PRE_FLASH_PULSE_WIDTH()  (1000)//(g_FlashManagerControl.f_PreFlashPulseWidth_us)

/// Get main flash pulse width
#define GM_GET_MAIN_FLASH_PULSE_WIDTH() (2000)//(g_FlashManagerControl.f_MainFlashPulseWidth_us)

/// Get external clock frequency.
/// TODO: <MS>
//#define GM_GET_EXT_CLK_FREQ() 3  //3MHz


//Till  16.0 release we wree using different clock freq for GPIO's before boot and after boot. Now we no longer needs this kind of implementation. So fixing the GPIO clock source as HOST_CLOCK.
// #define GM_GET_EXT_CLK_FREQ()   (CRM_GetIsSensorClockAvailable()? ( 2 * g_VideoTimingOutput.f_OPPixelClockFrequency_Mhz/Get_CRM_CRM_CLK_PICTOR_GPIO_DIV()):(g_SystemSetup.f_ClockHost_Mhz/(Get_CRM_CRM_CLK_PICTOR_GPIO_DIV() * Get_CRM_CRM_CLK_HOST_IPP_DIV())))//DEFAULT_CLK_HOST_IPP_DIV
#define GM_GET_EXT_CLK_FREQ()   g_SystemSetup.f_ClockHost_Mhz/(Get_CRM_CRM_CLK_PICTOR_GPIO_DIV() * Get_CRM_CRM_CLK_HOST_IPP_DIV())


/// Get number of pre flashes
#define GM_GET_NUM_OF_PRE_FLASHES() (2)//(g_FlashManagerControl.u8_NumberOfPreFlashes)

/// Get flash type
#define GM_GET_FLASH_TYPE()           (1)//(g_FlashManagerControl.e_FlashType)

/// Get Pre flash start frame
#define GM_START_PRE_FLASH_FRAME()  (1)//(g_FlashManagerStatus.u8_StartPreFlashFrame)

/// Get Pre flash start line
#define GM_START_PRE_FLASH_LINE()   (1)//(g_FlashManagerStatus.u16_StartPreFlashLine)

/// Get Pre flash start pixel
#define GM_START_PRE_FLASH_PIXEL()  (1)//(g_FlashManagerStatus.u16_StartPreFlashPixel)

/// Get Main flash start frame
#define GM_START_MAIN_FLASH_FRAME() (2)//(g_FlashManagerStatus.u8_StartFlashFrame)

/// Get main flash start line
#define GM_START_MAIN_FLASH_LINE()  (2)//(g_FlashManagerStatus.u16_StartFlashLine)

/// Get main flash start pixel
#define GM_START_MAIN_FLASH_PIXEL() (2)//(g_FlashManagerStatus.u16_StartFlashPixel)

// [PM]commented to remove compiler warning, kept for future reference.
/*
/// Set main flash pending flag.
#define GM_SET_MAIN_FLASH_PENDING_FLAG(x)   (Flag_e_FALSE)//(g_FlashManagerStatus.e_Flag_MainFlashPending = (x))

/// Set Pre flash pending flag
#define GM_SET_PRE_FLASH_PENDING_FLAG(x)     (Flag_e_FALSE)//(g_FlashManagerStatus.e_Flag_PreFlashPending = (x))

/// call the flash isr function here.
#define GM_FlashManager_ISR()       (Flag_e_TRUE)
*/

#endif // _GPIOMANAGER_IP_INTERFACE_H_

