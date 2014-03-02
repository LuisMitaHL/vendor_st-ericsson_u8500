/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  GPIOManager.c

 \brief This file is a part of the release code of the GPIO manager module.
        It is the main file which implements the GPIOs.

 \ingroup GPIOManger
 \endif
*/
#include "GPIOManager_OpInterface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_GPIOManager_GPIOManagerTraces.h"
#endif
GPIOControl_ts  g_GPIOControl =
{
    GPIO_MANAGER_SENSOR_0_XSHUTDOWN_GPO,
    GPIO_MANAGER_SENSOR_1_XSHUTDOWN_GPO,
    GPIO_MANAGER_MAIN_FLASH_GPO,
};

gpio_control_ts g_gpio_control = { 0, 500, 500, 0, 0, 0, TOP_LEVEL_GLOBAL_TIMER_TIMEOUT_DURATION_US,
                                   0, 0, 0, 0, 0, 0,
                                   Flag_e_FALSE, Flag_e_FALSE, Flag_e_FALSE, Flag_e_FALSE,
                                   0, 0};

gpio_debug_ts g_gpio_debug = {0.0, 0.0, GLOBAL_COUNTER_MIN_VALUE, 0, 0, 0, 0, 0, 0, 0};

void            ProgramPWM (
                uint8_t     u8_gpio_num,
                uint8_t     u8_channel_num,
                uint8_t     u8_timer_num,
                uint16_t    u16_count_lo,
                uint16_t    u16_count_hi,
                uint16_t    u16_repeat);
void            ProgramIDPTimer (void);

/******************************************************
 \fn void   GPIOManager_Initialize(void)
 \brief This function initializes the GPIOs.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/

/*
void
GPIOManager_Initialize(void)
{
    /// Configure the directon (input/output) for the Gpio.
    GM_SET_GPIO_GPD_DIR_INPUT();


    /// reset gpio interrupt control regs
    /// to allow proper operation even after a warm BOOT
    ///////////////////////////////// TODO:
    ///GpioIntEn.gpio_int_en   = 0;
    ///GpioIntCtrl0.bByte      = 0x7f;
    ///GpioIntCtrl1.bByte      = 0x3f;
    GM_SET_GPIO_INT_SRC_WRD(0);
}
*/
void
GPIOManager_Initialize(void)
{
    uint32_t    u32_GPD = 0;        //initialized to zero
    uint32_t    u32_GPOConfig = 0;  //initialized to zero

    // configure the direction (input/output) for the GPIO.
    GM_SET_GPIO_GPD_DIR_INPUT();

    // enable the GPIO IP
    Set_GPIO_GPIO_ENABLE(gpio_enable_ENABLE, soft_reset_DISABLE);   // gpio_enable,soft_reset

    // set the output of all the GPIOs to low
    Set_GPIO_GPIO_GPO(0);

    // set the direction of the Flash GPIO as output, rest as input;
    u32_GPD |= (uint32_t) ((uint32_t) gpd_DIR_OUTPUT << g_GPIOControl.u8_FlashGPO);

    // set the Flash GPIO to be driven by the GPO register
    u32_GPOConfig |= (uint32_t)((uint32_t) gpo0_config_GPO_REGISTER << g_GPIOControl.u8_FlashGPO);

    // check if XSHUTDOWN is supported in primary sensor
    if (GPIOManager_IsSensor0XShutDownSupported())
    {
        // set the direction of the XSHUTDOWN GPIOs as output, rest as input
        u32_GPD |= (uint32_t) ((uint32_t) gpd_DIR_OUTPUT << g_GPIOControl.u8_Sensor0XShutdownGPO);

        // set the XSHUTDOWN GPIOs to be driven by the GPO register
        u32_GPOConfig |= (uint32_t)((uint32_t) gpo0_config_GPO_REGISTER << g_GPIOControl.u8_Sensor0XShutdownGPO);
    }


    // check if XSHUTDOWN is supported in secondary sensor
    if (GPIOManager_IsSensor1XShutDownSupported())
    {
        // set the direction of the XSHUTDOWN GPIOs as output, rest as input
        u32_GPD |= (uint32_t) ((uint32_t) gpd_DIR_OUTPUT << g_GPIOControl.u8_Sensor1XShutdownGPO);

        // set the XSHUTDOWN GPIOs to be driven by the GPO register
        u32_GPOConfig |= (uint32_t)((uint32_t) gpo0_config_GPO_REGISTER << g_GPIOControl.u8_Sensor1XShutdownGPO);
    }


    // finally set the direction register
    Set_GPIO_GPIO_GPD(u32_GPD);

    // set the config register
    Set_GPIO_GPIO_OUTPUT_CONFIG_word(u32_GPOConfig);

    return;
}


/*
************************************************************************************************
 \fn void   GPIOManager_AssertSensor1Gpio(void)
 \brief This function asserts Sensor1 Gpio.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_AssertSensor1Gpio(void)
{
    uint16_t    u16_GPIO_Gpo;

    /// Get the current value of GP output register.
    u16_GPIO_Gpo = GM_GET_GPIO_GPO_gpo();

    /// Set the value of GP output register for asserting Sensor1XShutdown
    GM_SET_GPIO_GPO_WRD(u16_GPIO_Gpo | GPIOManager_Sensor1XShutdownMask);
}


/*
************************************************************************************************
 \fn void   GPIOManager_DeAssertSensor1Gpio(void)
 \brief This function deasserts Sensor1 Gpio.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_DeAssertSensor1Gpio(void)
{
    uint16_t    u16_GPIO_Gpo;

    /// Get the current value of GP output register.
    u16_GPIO_Gpo = GM_GET_GPIO_GPO_gpo();

    /// Set the value of GP output register for deasserting Sensor1XShutdown
    GM_SET_GPIO_GPO_WRD(u16_GPIO_Gpo &~(GPIOManager_Sensor1XShutdownMask));
}


/*
************************************************************************************************
 \fn void   GPIOManager_AssertSensor0Gpio(void)
 \brief This function asserts Sensor0 Gpio.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_AssertSensor0Gpio(void)
{
    uint16_t    u16_GPIO_Gpo;

    /// Get the current value of GP output register.
    u16_GPIO_Gpo = GM_GET_GPIO_GPO_gpo();

    /// Set the value of GP output register for asserting Sensor1XShutdown
    GM_SET_GPIO_GPO_WRD(u16_GPIO_Gpo | GPIOManager_Sensor0XShutdownMask);
}


/*
************************************************************************************************
 \fn void   GPIOManager_DeAssertSensor0Gpio(void)
 \brief This function deasserts Sensor0 Gpio.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_DeAssertSensor0Gpio(void)
{
    uint16_t    u16_GPIO_Gpo;

    /// Get the current value of GP output register.
    u16_GPIO_Gpo = GM_GET_GPIO_GPO_gpo();

    /// Set the value of GP output register for deasserting Sensor1XShutdown
    GM_SET_GPIO_GPO_WRD(u16_GPIO_Gpo &~(GPIOManager_Sensor0XShutdownMask));
}


/*
************************************************************************************************
 \fn void   GPIOManager_TriggerUpFlashOnStreaming(void)
 \brief Service routine used to fire flash in intelligent flash.Called when
        "e_Flag_TriggerFlashOnStreaming" flag is e_Flag_TRUE.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_TriggerUpFlashOnStreaming(void)
{
    uint16_t    u16_GPIOGpo;

    //u32_GPOConfig = GM_GET_GPIO_CHANNEL_CONFIG();
    //GM_SET_GPIO_CHANNEL_CONFIG_WRD(u32_GPOConfig | GPIOManager_MainFlashGPOMask);
    u16_GPIOGpo = GM_GET_GPIO_GPO_gpo();
    GM_SET_GPIO_GPO_WRD(u16_GPIOGpo | GPIOManager_MainFlashGPOMask);
}


/*
************************************************************************************************
 \fn void   GPIOManager_TriggerDownFlashOnStreaming(void)
 \brief Service routine used to fire flash in intelligent flash.Called when
        "e_Flag_TriggerFlashOnStreaming" flag is e_Flag_TRUE.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_TriggerDownFlashOnStreaming(void)
{
    uint16_t    u16_GPIOGpo;

    u16_GPIOGpo = GM_GET_GPIO_GPO_gpo();
    GM_SET_GPIO_GPO_WRD(u16_GPIOGpo &~(GPIOManager_MainFlashGPOMask));

    //u32_GPOConfig = GM_GET_GPIO_CHANNEL_CONFIG();
    //GM_SET_GPIO_CHANNEL_CONFIG_WRD(u32_GPOConfig & ~(GPIOManager_MainFlashGPOMask));
}


/*
************************************************************************************************
 \fn void   GPIOManager_ISR(void)
 \brief ISR routine to handle the GPIO interupt and program GPIO for next flash.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void
GPIOManager_ISR(void)
{
    /// TODO:INT_DISABLE_GPIO();
    //Set_ITM_ITM_GPIO_EN_BCLR_word(0x3fffff);
    //Set_GPIO_GPIO_ENABLE(gpio_enable_DISABLE,soft_reset_DISABLE);
    uint8_t u8_flag;
    u8_flag = Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_1_EN_STATUS();
    Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_1_EN_BCLR(0x1);

    /// TODO:FlashGunManager_IDPReferenceTimerInterruptAsserted()
    //if (Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_1_EN_STATUS())
    if (u8_flag)
    {
        /// Call flash manager ISR
        // Commented to remove compiler warning
        //GM_FlashManager_ISR();
    }


    ///TODO:INT_ENABLE_GPIO();
    //Set_ITM_ITM_GPIO_EN_BSET_word(0x3fffff);
    //Set_GPIO_GPIO_ENABLE(gpio_enable_ENABLE,soft_reset_DISABLE);
    Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_1_EN_BSET(0x1);
}


/******************************************************
 \fn void   GPIOManager_ProgramGPIOForFlash(void)
 \brief This function programs the GPIO necessary for firing the flash.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
GPIOManager_ProgramGPIOForFlash(void)
{
    float_t f_Counter,
            f_Maximum;
    uint8_t u8_PreScale,
            u8_channel_pairing0,
            u8_channel_pairing1,
            u8_channel_pairing2,
            u8_channel_pairing3;
    u8_channel_pairing0 = u8_channel_pairing1 = u8_channel_pairing2 = u8_channel_pairing3 = 0;

    /// First disable the flashgun IDP reference GPIO timer to ensure that it forgets its previous state.
    GM_GPIO_TIMER_CTRL_IDPTIMER_DISABLE();

    // gpio interrut 1 is used as idp refernce timer interrupt.
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_1_STATUS_BCLR(0x1);

    Set_GPIO_GPIO_GPD_word(Get_GPIO_GPIO_GPD() | (0x3 << GPIOManager_MainFlashGPO));
    Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() &~(0x3 << GPIOManager_MainFlashGPO));
    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() | (0x3 << GPIOManager_MainFlashGPO));

    Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_1_EN_BSET(0x1);

    /// Now enable the timer for a fresh operation.
    //GM_SET_GPIO_TIMER0_ENABLE();

        /// Check is ORing of main and pre flash pulses is required.
    if (GM_OR_MAIN_AND_PRE_FLASH_PULSE())
    {
        /// Pair channels.
        if ((GPIOManager_MainFlashGPO / 2) == 0)
        {
            u8_channel_pairing0 = 0x3;
        }
        else if ((GPIOManager_MainFlashGPO / 2) == 1)
        {
            u8_channel_pairing1 = 0x3;
        }
        else if ((GPIOManager_MainFlashGPO / 2) == 2)
        {
            u8_channel_pairing2 = 0x3;
        }
        else if ((GPIOManager_MainFlashGPO / 2) == 3)
        {
            u8_channel_pairing3 = 0x3;
        }


        GM_SET_GPIO_CHANNEL_PAIRING(u8_channel_pairing0, u8_channel_pairing1, u8_channel_pairing2, u8_channel_pairing3);
    }
    else
    {
        GM_SET_GPIO_CHANNEL_PAIRING(u8_channel_pairing0, u8_channel_pairing1, u8_channel_pairing2, u8_channel_pairing3);
    }


    /// Set the timer source for both channels.
    //GM_SetFlashChnlSource(GPIO_MANAGER_FLASH_TIMERSELECT, GPIOManager_MainFlashGPO);
    //GM_SetFlashChnlSource(GPIO_MANAGER_FLASH_TIMERSELECT, GPIOManager_PreFlashGPO);

    /// We must ensure that the counter value does not overflow 16 bits.
        /// For this we must ensure that the pre scale value is calculated for the maximum of the following values
        ///  pre flash high time, inter pre flash distance and main flash high time.

    /*
        if(GM_GET_INTER_PRE_FLASH_DISTANCE() > GM_GET_PRE_FLASH_PULSE_WIDTH())
        {
            f_Maximum = GM_GET_INTER_PRE_FLASH_DISTANCE();
        }
        else
        {
            f_Maximum = GM_GET_PRE_FLASH_PULSE_WIDTH();
        }

        if(GM_GET_MAIN_FLASH_PULSE_WIDTH() > f_Maximum)
        {
            f_Maximum = GM_GET_MAIN_FLASH_PULSE_WIDTH();
        }
        */
    f_Maximum = FMAX(
        GM_GET_MAIN_FLASH_PULSE_WIDTH(),
        FMAX(GM_GET_PRE_FLASH_PULSE_WIDTH(), GM_GET_MAIN_FLASH_PULSE_WIDTH()));

    /// Find the pre scale value.
    u8_PreScale = 0;
    f_Counter = (f_Maximum * GM_GET_EXT_CLK_FREQ());

    g_gpio_debug.f_Counter1 = f_Counter;

    /// Ensure that the counter value fits within 16 bits.
    while (f_Counter > 65535)
    {
        u8_PreScale++;
        f_Counter = ((uint32_t) (f_Counter) >> 1);
    }


    g_gpio_debug.f_Counter2 = f_Counter;
    g_gpio_debug.u8_PreScale = u8_PreScale;

    /// Program the pre scale value.
    GM_SET_GPIO_TIMER_PRESCALE(u8_PreScale, 0, 0, 0);

    /// Program the counter value for the main flash.
        /// channel 0 <TODO: MS> generalize it
    //GM_SET_COUNTER_VALUE_CTRL1((uint16_t)((uint32_t)(GM_GET_MAIN_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale));
    //GM_SET_COUNTER_VALUE_CTRL1((uint16_t) f_Counter);
    Set_GPIO_GPIO_CHANNEL0_CTRL1(
    (uint16_t) ((uint32_t) (GM_GET_MAIN_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale),
    0);
    g_gpio_debug.u16_Chnl0_Ctrl1 = (uint16_t) ((uint32_t) (GM_GET_MAIN_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale);
    g_gpio_debug.u16_Chnl0_Ctrl0 = 0;

    /// Program the main flash repeat count to 1 (2 high/low or low/high transitions)
    //GM_SET_REPEAT_COUNT(1);
    Set_GPIO_GPIO_CHANNEL0_REPEAT(1, 0, 0);
    g_gpio_debug.u16_Chnl0_Repeat = 2;

    if (GM_GET_NUM_OF_PRE_FLASHES() > 0)
    {
        /// We must ensure that the timer prescale value is taken into account.
        //GM_SET_COUNTER_VALUE_CTRL1((uint16_t)((uint32_t)(GM_GET_PRE_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale));
        Set_GPIO_GPIO_CHANNEL1_CTRL1(
        (uint16_t) ((uint32_t) (GM_GET_PRE_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale),
        0);
        g_gpio_debug.u16_Chnl1_Ctrl1 = (uint16_t) ((uint32_t) (GM_GET_PRE_FLASH_PULSE_WIDTH() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale);

        //GM_SET_COUNTER_VALUE_CTRL0((uint16_t)((uint32_t)(GM_GET_INTER_PRE_FLASH_DISTANCE() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale));
        Set_GPIO_GPIO_CHANNEL1_CTRL0(
        (uint16_t) ((uint32_t) (GM_GET_INTER_PRE_FLASH_DISTANCE() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale),
        0);
        g_gpio_debug.u16_Chnl1_Ctrl0 = (uint16_t) ((uint32_t) (GM_GET_INTER_PRE_FLASH_DISTANCE() * GM_GET_EXT_CLK_FREQ()) >> u8_PreScale);

        /// repeat
        //GM_SET_REPEAT_COUNT((GM_GET_NUM_OF_PRE_FLASHES() << 1) - 1);
        Set_GPIO_GPIO_CHANNEL1_REPEAT((GM_GET_NUM_OF_PRE_FLASHES() << 1), 0, 0);
        g_gpio_debug.u16_Chnl1_Repeat = (GM_GET_NUM_OF_PRE_FLASHES() << 1);
    }


    /// We have programmed the Flashgun Pulse GPIO for their rise and fall times.
        /// However we must tie the pulse on the GPIO with a reference IDP timer.
        /// We want the flash pulse (either pre or main) to rise at only specified frame/line/pixel counts.
        /// For this we use a GPIO timer interrupt.

        /// Select the source of the flashgun IDP GPIO timer interrupt to IDP timer
    GM_SET_GPIO_INT_SRC(0, GPIO_MANAGER_FLASH_IDP_TIMER, 0, 0);

    /// The main flash is always to be fired...
    // Commented to remove compiler warning
    //GM_SET_MAIN_FLASH_PENDING_FLAG(Flag_e_TRUE);

    /// TODO: FlashType_e_FLASH_TYPE_WHITELED
    if (GM_GET_NUM_OF_PRE_FLASHES() && (GM_GET_FLASH_TYPE() != 0))
    {
        GM_SET_FRAME_COUNT(GM_START_PRE_FLASH_FRAME());
        GM_SET_LINE_COUNT(GM_START_PRE_FLASH_LINE());
        GM_SET_PIXEL_COUNT(GM_START_PRE_FLASH_PIXEL());

        // Commented to remove compiler warning
        // GM_SET_PRE_FLASH_PENDING_FLAG(Flag_e_TRUE);
    }
    else
    {
        GM_SET_FRAME_COUNT(GM_START_MAIN_FLASH_FRAME());
        GM_SET_LINE_COUNT(GM_START_MAIN_FLASH_LINE());
        GM_SET_PIXEL_COUNT(GM_START_MAIN_FLASH_PIXEL());

        // Commented to remove compiler warning
        //GM_SET_PRE_FLASH_PENDING_FLAG(Flag_e_FALSE);
    }


    ///TODO: enable the Flashgun IDP reference timer interrupt.
    //Set_GPIO_GPIO_TIMER_CTRL_timer1_enable__ENABLE();
    Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_0_EN_BSET(0x1);

    //GM_GPIO_TIMER_CTRL_IDPTIMER_ENABLE();
}


/******************************************************
 \fn void GPIOManager_Toggle_GPIO_Pin(void)
 \brief This function toggles the GPIO Pin 2 for debugging the flash.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
GPIOManager_Toggle_GPIO_Pin(void)
{
    uint16_t    u16_gpo;

    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() | 0x4);
    Set_GPIO_GPIO_GPD_word(Get_GPIO_GPIO_GPD() | 0x4);

    u16_gpo = ((Get_GPIO_GPIO_GPO() & 0x4) >> 2);
    if (u16_gpo)
    {
        Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() &~(0x4));
    }
    else
    {
        Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() | (0x4));
    }
}


/******************************************************
 \fn void GPIOManager_Toggle_GPIO_Pin_Num(uint8_t u8_gpio_num)
 \brief This function toggles the GPIO Pin for debugging the flash.
 \param u8_gpio_num  The GPIO pin that is to be toggled.
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
GPIOManager_Toggle_GPIO_Pin_Num(
uint8_t u8_gpio_num)
{
    uint16_t    u16_gpo;

    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() | (0x1 << u8_gpio_num));
    Set_GPIO_GPIO_GPD_word(Get_GPIO_GPIO_GPD() | (0x1 << u8_gpio_num));

    u16_gpo = ((Get_GPIO_GPIO_GPO() & (0x1 << u8_gpio_num)) >> u8_gpio_num);
    if (u16_gpo)
    {
        Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() &~(0x1 << u8_gpio_num));
    }
    else
    {
        Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() | (0x1 << u8_gpio_num));
    }
}


/******************************************************
 \fn void    GPIOManager_debug(void)
 \brief This function is used for debugging the flash, for delay, triggering of IDP Timer, GpIO Triggering.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
GPIOManager_debug(void)
{
    /*
        // check dumb gpio working
        Set_GPIO_GPIO_OUTPUT_CONFIG_word(g_gpio_control.u8_GPIO_config);
        Set_GPIO_GPIO_GPO_word(g_gpio_control.u8_GPIO_gpo);
        Set_GPIO_GPIO_GPD_word(g_gpio_control.u8_GPIO_gpd);
        */
    g_gpio_control.u8_GPIO_config = Get_GPIO_GPIO_OUTPUT_CONFIG();
    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() | 0x1);
    g_gpio_control.u8_GPIO_gpd = Get_GPIO_GPIO_GPD();
    Set_GPIO_GPIO_GPD_word(Get_GPIO_GPIO_GPD() | 0x1);
    g_gpio_control.u8_GPIO_gpo = Get_GPIO_GPIO_GPO();
    Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() | 0x1);

    GPIOManager_Delay_us(g_gpio_control.u32_GPIO_delay_us);

    //ProgramIDPTimer();
    Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() & 0xfe);

    // pwm mode
    // to test pwm mode, following sequence is to be followed for output at gpio[0]
    // 1.) enable gpio
    // 2.) gpd = 0xff
    // 3.) gpo = 0x0
    // 4.) output_config = 1
    // 5.) channel config = 0
    // 6.) channel_src = 0
    // 7.) channel_ctrl0 = 0x0a;
    // 8.) channel_ctrl1 = 0x05
    // 9.) channel_repeat = 0xff
    // 10.) timer_ctrl = 0x1
    // 11.) channel_start = 0x1
    // 12.) channel_start = 0x0
    // 13.) channel_start = 0x1
    // 14.) output_config = 0x0
    // 15.) channel_start = 0x0
    //ProgramPWM(0, 0, 0, g_gpio_control.u16_GPIO_channel0_ctrl0, g_gpio_control.u16_GPIO_channel0_ctrl1, g_gpio_control.u16_GPIO_channel0_repeat);
}


/******************************************************
 \fn void ProgramPWM(uint8_t u8_gpio_num, uint8_t u8_channel_num, uint8_t u8_timer_num, uint16_t u16_count_lo, uint16_t u16_count_hi, uint16_t u16_repeat)
 \brief This function programs GPIO for PWM.
 \param u8_gpio_num The GPIO pin at which PWM is required.
 \param u8_channel_num The GPIO channel from which PWm is required.
 \param u8_timer_num  The GPIO timer used as a source for GPIO & Channel.
 \param u16_count_lo  The count for which the PWM sequence has to stay low.
 \param u16_count_hi  The count for which the PWM sequence has to stay high.
 \param u16_repeat    The count for which the PWM sequence has to be repeated.
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
ProgramPWM(
uint8_t     u8_gpio_num,
uint8_t     u8_channel_num,
uint8_t     u8_timer_num,
uint16_t    u16_count_lo,
uint16_t    u16_count_hi,
uint16_t    u16_repeat)
{
    Set_GPIO_GPIO_GPD_word(Get_GPIO_GPIO_GPD() | (0x1 << u8_gpio_num));
    Set_GPIO_GPIO_GPO_word(Get_GPIO_GPIO_GPO() &~(0x1 << u8_gpio_num));
    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() | (0x1 << u8_gpio_num));
    Set_GPIO_GPIO_CHANNEL_CONFIG_word(Get_GPIO_GPIO_CHANNEL_CONFIG() &~(0x1 << u8_channel_num));

    //  Set_GPIO_GPIO_CHANNEL_SRC_word(Get_GPIO_GPIO_CHANNEL_SRC() | ((0x4) << (u8_channel_num * 3)));
    Set_GPIO_GPIO_CHANNEL0_CTRL0(u16_count_lo, 0);
    Set_GPIO_GPIO_CHANNEL0_CTRL1(u16_count_hi, 0);
    Set_GPIO_GPIO_CHANNEL0_REPEAT(u16_repeat, 0x0, 0x0);
    Set_GPIO_GPIO_TIMER_CTRL_word(Get_GPIO_GPIO_TIMER_CTRL() | (0x1 << u8_timer_num));
    Set_GPIO_GPIO_CHANNEL_START_channel0_start__CHANNEL_START();
    Set_GPIO_GPIO_CHANNEL_START_channel0_start__NO_EFFECT();
    Set_GPIO_GPIO_CHANNEL_START_channel0_start__CHANNEL_START();
    Set_GPIO_GPIO_OUTPUT_CONFIG_word(Get_GPIO_GPIO_OUTPUT_CONFIG() &~(0x1 << u8_gpio_num));
    Set_GPIO_GPIO_CHANNEL_START_channel0_start__NO_EFFECT();
}


/******************************************************
 \fn void ProgramIDPTimer(void)
 \brief This function programs IDP Timer which is used for triggering of flash.
 \param None
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
ProgramIDPTimer(void)
{
    //sequence to follow
    //1.) enable idp timer
    //2.) set channel 0 src to be idp timer
    //3.) program channel 0 low & high counters
    //4.) set gpio_int_src to be idptimer
    Set_GPIO_GPIO_TIMER_CTRL_idptimer_enable__DISABLE();

    /// clear GPIO timer 0 interrupt status
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_1_STATUS_BCLR(0x1);

    Set_GPIO_GPIO_INT_SRC_word(Get_GPIO_GPIO_INT_SRC() | (0x4 << 3));
    Set_GPIO_GPIO_INT1_TRIGGER(500, 500);
    Set_GPIO_GPIO_INT1_TRIGGER_EX(10, 0);

    // <TODO:MS>before enabling idp timer.
    // the source of gpio must be set to pixel clock
    Set_GPIO_GPIO_TIMER_CTRL_idptimer_enable__ENABLE();

    /// wait for interrupt to occur in the ITM
    while (!Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_1_STATUS())
        ;
}


/******************************************************
 \fn void GPIOManager_Delay_us(float_t f_delay_usec)
 \brief This function provides delay for the given number of usecs.
 \param f_delay_usec  The delay in usec
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
******************************************************/
void
GPIOManager_Delay_us(
float_t f_delay_usec)
{
    // a delay of f_delay_sec
    // the counter in the GPIO timer 3 is needed to be programmed.
    // no. of counts required will be = (GPIO_clk_freq * Delay in seconds)
    float_t f_Counter;
    uint8_t u8_PreScale;

    OstTraceInt1(TRACE_DEBUG, "->GPIOManager_Delay_us: %d", (uint32_t) f_delay_usec);

    /// disable timer 3
    GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xf7);

    ///    disable timer interrupt for GPIO timer 3 in ITM
    Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_3_EN_BCLR(0x1);

    /// clear GPIO timer 3 interrupt status
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_3_STATUS_BCLR(0x1);

    /// Find the pre scale value.
    u8_PreScale = 0;
    f_Counter = (f_delay_usec * GM_GET_EXT_CLK_FREQ());

    /// Ensure that the counter value fits within 16 bits.
    while (f_Counter > 65535)
    {
        u8_PreScale++;
        f_Counter = ((uint32_t) (f_Counter) >> 1);
    }


    /// Program the pre scale value.
    GM_SET_GPIO_TIMER_PRESCALE(0, 0, 0, u8_PreScale);

    /// Program the counter value for the delay in timer 3
    Set_GPIO_GPIO_INT3_TRIGGER((uint16_t) f_Counter, 0);

    GM_SET_GPIO_INT_SRC_WRD(GM_GET_GPIO_INT_SRC() | 0x600);

    /// enable timer 3
    GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() | 0x08);

    /// Interrupt is not enabled for this timer, so wait here
        /// wait for interrupt to occur in the ITM
    while (!Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_3_STATUS())
        ;

    OstTraceInt0(TRACE_DEBUG, "<-GPIOManager_Delay_us");
}


//Function added for implementing interrupt timer

//interrupt timer will use timer2 from GPIO block
void
GPIOManager_StartIntTimer_us(
uint32  u32_DelayUsec)
{
    // a delay of u32_DelayUsec
    // the counter in the GPIO timer 2 is needed to be programmed.
    // no. of counts required will be = (GPIO_clk_freq * Delay in seconds)
    float_t f_Counter;
    uint8_t u8_PreScale;

    /// Disable Timer 2 interrupt in ITM
    Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_2_EN_BCLR(0x1);

    /// disable timer 2
    GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfb);

    /// clear GPIO timer 2 interrupt status
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_2_STATUS_BCLR(0x1);

    if (NULL != u32_DelayUsec)
    {
        /// disable timer 2
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfb);

        /// clear GPIO timer 2 interrupt status
        //        Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_2_STATUS_BCLR(0x1);

                /// Find the pre scale value.
        //TODO:CN:Currently implementing interrupt timer based on
        u8_PreScale = 0;
        f_Counter = (u32_DelayUsec * GM_GET_EXT_CLK_FREQ());

        /// Ensure that the counter value fits within 16 bits.
        while (f_Counter > 65535)
        {
            u8_PreScale++;
            f_Counter = ((uint32_t) (f_Counter) >> 1);
        }


        /// Program the pre scale value for timer2.
        GM_SET_GPIO_TIMER_PRESCALE(0, 0, u8_PreScale, 0);

        /// Program the counter value for the delay in timer 2
        Set_GPIO_GPIO_INT2_TRIGGER((uint16_t) f_Counter, 0);

        //Set the interrupt source for GPIO interrupt
        GM_SET_GPIO_INT_SRC_WRD(GM_GET_GPIO_INT_SRC() | 0x080);

        /// enable timer 2
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() | 0x04);

        //Enable timer interrupt for timer2 in ITM block
        //GPIO interrupt for xP70 has already been enabled during boot-up
        //        Set_ITM_ITM_GPIO_EN_BSET_word(Get_ITM_ITM_GPIO_EN_BSET() | (0x1 << 18));
        Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_2_EN_BSET(0x1);
    }


    return;
}


#if INCLUDE_FOCUS_MODULES

//Function added for implementing interrupt timer for FLAD

//interrupt timer will use timer1 from GPIO block
void
GPIOManager_StartIntTimerFLAD_us(
uint32  u32_DelayUsec)
{
    // a delay of u32_DelayUsec
    // the counter in the GPIO timer 1 is needed to be programmed.
    // no. of counts required will be = (GPIO_clk_freq * Delay in seconds)
    float_t f_Counter;
    uint8_t u8_PreScale;

    /// Disable Timer 1 interrupt in ITM
    Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_1_EN_BCLR(0x1);

    /// disable timer 1
    GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfd);

    /// clear GPIO timer 1 interrupt status
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_1_STATUS_BCLR(0x1);

    if (0 != u32_DelayUsec)
    {
        /// disable timer 1
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfd);

        /// clear GPIO timer 1 interrupt status
        /// Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_1_STATUS_BCLR(0x1);

        /// Find the pre scale value.
        //TODO:CN:Currently implementing interrupt timer based on
        u8_PreScale = 0;
        f_Counter = (u32_DelayUsec * GM_GET_EXT_CLK_FREQ());

        /// Ensure that the counter value fits within 16 bits.
        while (f_Counter > 65535)
        {
            u8_PreScale++;
            f_Counter = ((uint32_t) (f_Counter) >> 1);
        }


        /// Program the pre scale value for timer1.
        GM_SET_GPIO_TIMER_PRESCALE(0, u8_PreScale, 0, 0);

        /// Program the counter value for the delay in timer 1
        Set_GPIO_GPIO_INT1_TRIGGER((uint16_t) f_Counter, 0);

        //Set the interrupt source for GPIO interrupt
        GM_SET_GPIO_INT_SRC_WRD(GM_GET_GPIO_INT_SRC() | 0x08);

        /// enable timer 1
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() | 0x02);

        //Enable timer interrupt for timer1 in ITM block
        Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_1_EN_BSET(0x1);
    }


    return;
}
#endif /*INCLUDE_FOCUS_MODULES*/


/*
************************************************************************************************
 \fn void GPIOManager_StartGlobalTimer(uint32 u32_timeout_us)
 \brief This function uses GPIO timer 0 to start a global reference timeer.
 \param u32_DelayUsec : timeout value
 \return void
 \ingroup GPIOManager
 \callgraph
 \callergraph
************************************************************************************************/
void GPIOManager_StartGlobalTimer(uint32 u32_timeout_us)
{
    // no. of counts required will be = (GPIO_clk_freq * Delay in seconds)
    float_t f_Counter;
    uint8_t u8_PreScale;


    // Disable Timer 0 interrupt in ITM
    Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_0_EN_BCLR(0x1) ;

    // Disable timer 0
    GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfe);

    // Clear GPIO timer 0 interrupt status
    Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_0_STATUS_BCLR(0x1);

    if(NULL != u32_timeout_us)
    {
        // disable timer 0
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfe);

        // clear GPIO timer 0 interrupt status
        // [PM]not sure why this is commented for each timer
        // Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_0_STATUS_BCLR(0x1);

        // Find the pre scale value.
        //TODO:CN:Currently implementing interrupt timer based on
        // [PM] Meaning of above comment not clear.
        u8_PreScale = 0;
        f_Counter = (u32_timeout_us * GM_GET_EXT_CLK_FREQ());

        // Ensure that the counter value fits within 16 bits.
        while(f_Counter > 65535)
        {
            u8_PreScale++;
            f_Counter = ((uint32_t)(f_Counter) >> 1);
        }

        // Program the pre scale value for timer0.
        GM_SET_GPIO_TIMER_PRESCALE(u8_PreScale,0,0,0);

        // Program the counter value for the delay in timer 0
        Set_GPIO_GPIO_INT0_TRIGGER((uint16_t)f_Counter,0);

        //Set the interrupt source for GPIO interrupt
        //[PM] Actually bitwise-OR with 0 not required!
        GM_SET_GPIO_INT_SRC_WRD(GM_GET_GPIO_INT_SRC() | 0x0);

        // enable timer 0
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() | 0x01);

        //Enable timer interrupt for timer0 in ITM block
        Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_0_EN_BSET(0x1);
    }

    return;
}

