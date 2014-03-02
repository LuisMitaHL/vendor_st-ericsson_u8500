/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Charger/Linux/ab9540_Charger.c
 * 
 *
 * Generated on the 13/01/2012 09:19 by the 'super-cool' code generator 
 *
 * Generator written in Common Lisp, created by  Remi PRUD'HOMME - STEricsson 
 * with the help of : 
 *
 ***************************************************************************
 *  Copyright STEricsson  2012
 *
 *  Reference document : User Manual ref : CD00291561 Rev 3, May 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_CHARGER_LINUX_AB9540_CHARGER_C
#define C__LISP_PRODUCTION_AB9540_AB9540_CHARGER_LINUX_AB9540_CHARGER_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Charger.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_main_charger_level
 *
 * RET  : Return the value of register 0xMainChargerStatus
 *
 * Notes : From register 0xB00, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_level()
  {
    unsigned char value;


    /* 
     * 0: MainCharger voltage below MainChDet threshold 
     * 1: MainCharger voltage above MainChDet threshold 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_CHARGER_LEVEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charge_on
 *
 * RET  : Return the value of register 0xMainChargerStatus
 *
 * Notes : From register 0xB00, bits 1:1
 *
 **************************************************************************/
unsigned char get_main_charge_on()
  {
    unsigned char value;


    /* 
     * 0: Main charger is off 
     * 1: Main charger is on 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_CHARGE_ON_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_voltage_regulation
 *
 * RET  : Return the value of register 0xMainChargerStatus
 *
 * Notes : From register 0xB00, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_voltage_regulation()
  {
    unsigned char value;


    /* 
     * 0: charger is not in voltage regulation 
     * 1: charger is in voltage regulation 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_VOLTAGE_REGULATION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_current_regulation
 *
 * RET  : Return the value of register 0xMainChargerStatus
 *
 * Notes : From register 0xB00, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_current_regulation()
  {
    unsigned char value;


    /* 
     * 0: Charger is not in input current regulation 
     * 1: Charger is in input current regulation 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_CURRENT_REGULATION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_current_limitation
 *
 * RET  : Return the value of register 0xMainChargerStatus
 *
 * Notes : From register 0xB00, bits 7:4
 *
 **************************************************************************/
unsigned char get_main_current_limitation()
  {
    unsigned char value;


    /* 
     * Result of automatic input current limitation (to avoid 
     * Main voltage to drop). Input current limitation can be 
     * from 90mA to 1.3A by about 200mA steps. 
     */ 
    value = (I2CRead(MAIN_CHARGER_STATUS_REG) & MAIN_CURRENT_LIMITATION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_thermal_protection
 *
 * RET  : Return the value of register 0xMainChargerEvent
 *
 * Notes : From register 0xB01, bits 1:1
 *
 **************************************************************************/
unsigned char get_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature below thermal threshold. 
     * 1: Die temperature above thermal threshold. Charging is 
     * stopped. 
     */ 
    value = (I2CRead(MAIN_CHARGER_EVENT_REG) & THERMAL_PROTECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_protection
 *
 * RET  : Return the value of register 0xUsbChargerStatus2
 *
 * Notes : From register 0xB03, bits 0:0
 *
 **************************************************************************/
unsigned char get_vbus_protection()
  {
    unsigned char value;


    /* 
     * 0: Vbus voltage below VbusOVV threshold (6.3v). 
     * 1: Vbus voltage above VbusOVV threshold (6.3v). Charger 
     * is disabled 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & VBUS_PROTECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_thermal_protection
 *
 * RET  : Return the value of register 0xUsbChargerStatus2
 *
 * Notes : From register 0xB03, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is below thermal threshold. 
     * 1: Die temperature is above thermal threshold. Charger is 
     * disabled. 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & USB_THERMAL_PROTECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_device_illegal
 *
 * RET  : Return the value of register 0xUsbChargerStatus2
 *
 * Notes : From register 0xB03, bits 3:3
 *
 **************************************************************************/
unsigned char get_usb_device_illegal()
  {
    unsigned char value;


    /* 
     * 0: USB device connected to STw4500 is not 'illegal' (does 
     * not include VbusOVV) 
     * 1: USB device connected to STw4500 is 'illegal' (does not 
     * include VbusOVV) 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & USB_DEVICE_ILLEGAL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_automatic_usb_charger_current
 *
 * RET  : Return the value of register 0xUsbChargerStatus2
 *
 * Notes : From register 0xB03, bits 7:4
 *
 **************************************************************************/
unsigned char get_automatic_usb_charger_current()
  {
    unsigned char value;


    /* 
     * Result of automatic input current limitation (to avoid to 
     * Vbus voltage to drop). Input current limitation can be 
     * from 90mA to 1.3A by about 200mA steps 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_2_REG) & AUTOMATIC_USB_CHARGER_CURRENT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_voltage_below_threshold
 *
 * RET  : Return the value of register 0xUsbChargerStatus3
 *
 * Notes : From register 0xB05, bits 0:0
 *
 **************************************************************************/
unsigned char get_voltage_below_threshold()
  {
    unsigned char value;


    /* 
     * 0: Battery voltage is below BatOVV threshold. 
     * 1: Battery voltage is above BatOVV threshold. Charger 
     * (Main or USB) is disabled when BattOVV occurs. BattOVV 
     * default threshold is 3.7v. 4.75v threshold is selected at 
     * first primary watchdog kick 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_3_REG) & VOLTAGE_BELOW_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature
 *
 * RET  : Return the value of register 0xUsbChargerStatus3
 *
 * Notes : From register 0xB05, bits 2:1
 *
 **************************************************************************/
unsigned char get_battery_temperature()
  {
    unsigned char value;


    /* 
     * Btemp comparator status (BtempLow threshold = typical -7 
     * C, BtempMedium 
     * typical 3 C, BtempHigh threshold = typical 52 C or 57 C 
     * or 62 C) 
     */ 
    value = (I2CRead(USB_CHARGER_STATUS_3_REG) & BATTERY_TEMPERATURE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ch_volt_level
 *
 * IN   : param, a value to write to the regiter ChVoltLevel
 * OUT  : 
 *
 * RET  : Return the value of register 0xChVoltLevel
 *
 * Notes : From register 0xB40, bits 6:0
 *
 **************************************************************************/
unsigned char set_ch_volt_level( enum volt_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CH_VOLT_LEVEL_REG);

    /* 
     * Common to Main and USB charger 
     */ 
  
    value =  old_value & ~CH_VOLT_LEVEL_MASK;


    value |=  param ;  
    I2CWrite(CH_VOLT_LEVEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ch_volt_level
 *
 * RET  : Return the value of register 0xChVoltLevel
 *
 * Notes : From register 0xB40, bits 6:0
 *
 **************************************************************************/
unsigned char get_ch_volt_level()
  {
    unsigned char value;


    /* 
     * Common to Main and USB charger 
     */ 
    value = (I2CRead(CH_VOLT_LEVEL_REG) & CH_VOLT_LEVEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_maximum_charging_voltage_level
 *
 * IN   : param, a value to write to the regiter ChargerVoltLevelMax
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerVoltLevelMax
 *
 * Notes : From register 0xB41, bits 6:0
 *
 **************************************************************************/
unsigned char set_maximum_charging_voltage_level( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_VOLT_LEVEL_MAX_REG);

    /* 
     * Defines maximum charging voltage level. Can be written 
     * only once 
     */ 
  
    value =  old_value & ~MAXIMUM_CHARGING_VOLTAGE_LEVEL_MASK;


    value |=  param ;  

    I2CWrite(CHARGER_VOLT_LEVEL_MAX_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_maximum_charging_voltage_level
 *
 * RET  : Return the value of register 0xChargerVoltLevelMax
 *
 * Notes : From register 0xB41, bits 6:0
 *
 **************************************************************************/
unsigned char get_maximum_charging_voltage_level()
  {
    unsigned char value;


    /* 
     * Defines maximum charging voltage level. Can be written 
     * only once 
     */ 
    value = (I2CRead(CHARGER_VOLT_LEVEL_MAX_REG) & MAXIMUM_CHARGING_VOLTAGE_LEVEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_channel_output_current_level
 *
 * IN   : param, a value to write to the regiter ChannelOutputCurrentLevel
 * OUT  : 
 *
 * RET  : Return the value of register 0xChannelOutputCurrentLevel
 *
 * Notes : From register 0xB42, bits 3:0
 *
 **************************************************************************/
unsigned char set_channel_output_current_level( enum charger_current_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_REG);

    /* 
     * MainChCurrLevel 
     */ 
  
    value =  old_value & ~CHANNEL_OUTPUT_CURRENT_LEVEL_MASK;


    value |=  param ;  
    I2CWrite(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_channel_output_current_level
 *
 * RET  : Return the value of register 0xChannelOutputCurrentLevel
 *
 * Notes : From register 0xB42, bits 3:0
 *
 **************************************************************************/
unsigned char get_channel_output_current_level()
  {
    unsigned char value;


    /* 
     * MainChCurrLevel 
     */ 
    value = (I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_REG) & CHANNEL_OUTPUT_CURRENT_LEVEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_maximum_charging_current
 *
 * IN   : param, a value to write to the regiter MaximumChargingCurrent
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaximumChargingCurrent
 *
 * Notes : From register 0xB43, bits 3:0
 *
 **************************************************************************/
unsigned char set_maximum_charging_current( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAXIMUM_CHARGING_CURRENT_REG);

    /* 
     * Defines maximum charging current level. Can be written 
     * only once 
     */ 
  
    value =  old_value & ~MAXIMUM_CHARGING_CURRENT_MASK;


    value |=  param ;  

    I2CWrite(MAXIMUM_CHARGING_CURRENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_maximum_charging_current
 *
 * RET  : Return the value of register 0xMaximumChargingCurrent
 *
 * Notes : From register 0xB43, bits 3:0
 *
 **************************************************************************/
unsigned char get_maximum_charging_current()
  {
    unsigned char value;


    /* 
     * Defines maximum charging current level. Can be written 
     * only once 
     */ 
    value = (I2CRead(MAXIMUM_CHARGING_CURRENT_REG) & MAXIMUM_CHARGING_CURRENT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charging_current_behavor
 *
 * IN   : param, a value to write to the regiter ChannelOutputCurrentLevelBehavor
 * OUT  : 
 *
 * RET  : Return the value of register 0xChannelOutputCurrentLevelBehavor
 *
 * Notes : From register 0xB44, bits 0:0
 *
 **************************************************************************/
unsigned char set_charging_current_behavor( enum charging_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG);

    /* 
     * When Btemp is above Low and below Medium temperature 
     * threshold: 
     * 0: Charging current is defined by OTP bits. 
     * 1: Charging current is set at 300mA (typ) regardless OTP 
     * bits 
     */ 
    switch( param ){
        case CHARGING_CURRENT_300_MA_E: 
           value = old_value | CHARGING_CURRENT_BEHAVOR_MASK; 
           break;
        case CHARGING_CURRENT_DEFINED_BY_OTP_E: 
           value = old_value & ~ CHARGING_CURRENT_BEHAVOR_MASK;
           break;
    }
  

    I2CWrite(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_behavor
 *
 * RET  : Return the value of register 0xChannelOutputCurrentLevelBehavor
 *
 * Notes : From register 0xB44, bits 0:0
 *
 **************************************************************************/
unsigned char get_charging_current_behavor()
  {
    unsigned char value;


    /* 
     * When Btemp is above Low and below Medium temperature 
     * threshold: 
     * 0: Charging current is defined by OTP bits. 
     * 1: Charging current is set at 300mA (typ) regardless OTP 
     * bits 
     */ 
    value = (I2CRead(CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG) & CHARGING_CURRENT_BEHAVOR_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_charger_watch_dog_timer
 *
 * RET  : Return the value of register 0xChargerWatchDogTimer
 *
 * Notes : From register 0xB50, bits 7:0
 *
 **************************************************************************/
unsigned char get_charger_watch_dog_timer()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(CHARGER_WATCH_DOG_TIMER_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_watch_dog_timer
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogTimer
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerWatchDogTimer
 *
 * Notes : From register 0xB50, bits 7:0
 *
 **************************************************************************/
unsigned char set_charger_watch_dog_timer( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(CHARGER_WATCH_DOG_TIMER_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(CHARGER_WATCH_DOG_TIMER_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_kick_watchdog_timer
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerWatchDogControl
 *
 * Notes : From register 0xB51, bits 0:0
 *
 **************************************************************************/
unsigned char set_kick_watchdog_timer( enum kick_watchdog_timer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_WATCH_DOG_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Kick Watchdog timer (re-start timer). Auto-cleared 
     * (cleared once timer setting done). 
     */ 
    switch( param ){
        case KICK_WATCHDOG_TIMER_ENABLE_E: 
           value = old_value | KICK_WATCHDOG_TIMER_MASK; 
           break;
        case KICK_WATCHDOG_TIMER_DISABLE_E: 
           value = old_value & ~ KICK_WATCHDOG_TIMER_MASK;
           break;
    }
  

    I2CWrite(CHARGER_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_kick_watchdog_timer
 *
 * RET  : Return the value of register 0xChargerWatchDogControl
 *
 * Notes : From register 0xB51, bits 0:0
 *
 **************************************************************************/
unsigned char get_kick_watchdog_timer()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Kick Watchdog timer (re-start timer). Auto-cleared 
     * (cleared once timer setting done). 
     */ 
    value = (I2CRead(CHARGER_WATCH_DOG_CONTROL_REG) & KICK_WATCHDOG_TIMER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_watch_dog_timer9
 *
 * IN   : param, a value to write to the regiter ChargerWatchDogControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerWatchDogControl
 *
 * Notes : From register 0xB51, bits 1:1
 *
 **************************************************************************/
unsigned char set_charger_watch_dog_timer9( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_WATCH_DOG_CONTROL_REG);

    /* 
     * bit 9 of ChargerWatchDogTimer 0-7 at 0xB50 
     */ 
  
    value =  old_value & ~CHARGER_WATCH_DOG_TIMER_9_MASK;


    value |= ( param << 0x1);  

    I2CWrite(CHARGER_WATCH_DOG_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_watch_dog_timer9
 *
 * RET  : Return the value of register 0xChargerWatchDogControl
 *
 * Notes : From register 0xB51, bits 1:1
 *
 **************************************************************************/
unsigned char get_charger_watch_dog_timer9()
  {
    unsigned char value;


    /* 
     * bit 9 of ChargerWatchDogTimer 0-7 at 0xB50 
     */ 
    value = (I2CRead(CHARGER_WATCH_DOG_CONTROL_REG) & CHARGER_WATCH_DOG_TIMER_9_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_temperature_high
 *
 * IN   : param, a value to write to the regiter ChargerTemperatureHigh
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerTemperatureHigh
 *
 * Notes : From register 0xB52, bits 1:0
 *
 **************************************************************************/
unsigned char set_charger_temperature_high( enum temperature_high_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_TEMPERATURE_HIGH_REG);

    /* 
     * Defined Btemp high threshold: 
     * 00: 57 degree C (typ) 
     * 01: 52 degree (typ) 
     * 10: 57 degree c(typ) 
     * 11: 62 degree c (typ) 
     */ 
  

     value =  old_value & ~CHARGER_TEMPERATURE_HIGH_MASK ;

    switch(  param ){

           case TEMPERATURE_HIGH_57_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_57_CELSIUS ;
                break;
           case TEMPERATURE_HIGH_52_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_52_CELSIUS ;
                break;
           case TEMPERATURE_HIGH_57_CELSIUS_1_E:
                value =  value | TEMPERATURE_HIGH_57_CELSIUS_1 ;
                break;
           case TEMPERATURE_HIGH_62_CELSIUS_E:
                value =  value | TEMPERATURE_HIGH_62_CELSIUS ;
                break;
    }
  

    I2CWrite(CHARGER_TEMPERATURE_HIGH_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_temperature_high
 *
 * RET  : Return the value of register 0xChargerTemperatureHigh
 *
 * Notes : From register 0xB52, bits 1:0
 *
 **************************************************************************/
unsigned char get_charger_temperature_high()
  {
    unsigned char value;


    /* 
     * Defined Btemp high threshold: 
     * 00: 57 degree C (typ) 
     * 01: 52 degree (typ) 
     * 10: 57 degree c(typ) 
     * 11: 62 degree c (typ) 
     */ 
    value = (I2CRead(CHARGER_TEMPERATURE_HIGH_REG) & CHARGER_TEMPERATURE_HIGH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_led_indicator
 *
 * IN   : param, a value to write to the regiter LedIndicatorPwmControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xLedIndicatorPwmControl
 *
 * Notes : From register 0xB53, bits 0:0
 *
 **************************************************************************/
unsigned char set_led_indicator( enum led_indicator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LED_INDICATOR_PWM_CONTROL_REG);

    /* 
     * 0: disable LedIndicator PWM generator 
     * 1: enable LedIndicator PWM generator 
     */ 
    switch( param ){
        case LED_INDICATOR_ENABLE_E: 
           value = old_value | LED_INDICATOR_MASK; 
           break;
        case LED_INDICATOR_DISABLE_E: 
           value = old_value & ~ LED_INDICATOR_MASK;
           break;
    }
  

    I2CWrite(LED_INDICATOR_PWM_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_led_indicator
 *
 * RET  : Return the value of register 0xLedIndicatorPwmControl
 *
 * Notes : From register 0xB53, bits 0:0
 *
 **************************************************************************/
unsigned char get_led_indicator()
  {
    unsigned char value;


    /* 
     * 0: disable LedIndicator PWM generator 
     * 1: enable LedIndicator PWM generator 
     */ 
    value = (I2CRead(LED_INDICATOR_PWM_CONTROL_REG) & LED_INDICATOR_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_led_current
 *
 * IN   : param, a value to write to the regiter LedIndicatorPwmControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xLedIndicatorPwmControl
 *
 * Notes : From register 0xB53, bits 2:1
 *
 **************************************************************************/
unsigned char set_charger_led_current( enum led_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LED_INDICATOR_PWM_CONTROL_REG);

    /* 
     */ 
  

     value =  old_value & ~CHARGER_LED_CURRENT_MASK ;

    switch(  param ){

           case LED_CURRENT_2_5_MA_E:
                value  = value  | (LED_CURRENT_2_5_MA << 0x1);
                break;
           case LED_CURRENT_1_MA_E:
                value  = value  | (LED_CURRENT_1_MA << 0x1);
                break;
           case LED_CURRENT_5_MA_E:
                value  = value  | (LED_CURRENT_5_MA << 0x1);
                break;
           case LED_CURRENT_10_MA_E:
                value  = value  | (LED_CURRENT_10_MA << 0x1);
                break;
    }
  

    I2CWrite(LED_INDICATOR_PWM_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_led_current
 *
 * RET  : Return the value of register 0xLedIndicatorPwmControl
 *
 * Notes : From register 0xB53, bits 2:1
 *
 **************************************************************************/
unsigned char get_charger_led_current()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(LED_INDICATOR_PWM_CONTROL_REG) & CHARGER_LED_CURRENT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_led_indicator_duty
 *
 * RET  : Return the value of register 0xLedIndicatorDuty
 *
 * Notes : From register 0xB54, bits 7:0
 *
 **************************************************************************/
unsigned char get_led_indicator_duty()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(LED_INDICATOR_DUTY_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_led_indicator_duty
 *
 * IN   : param, a value to write to the regiter LedIndicatorDuty
 * OUT  : 
 *
 * RET  : Return the value of register 0xLedIndicatorDuty
 *
 * Notes : From register 0xB54, bits 7:0
 *
 **************************************************************************/
unsigned char set_led_indicator_duty( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(LED_INDICATOR_DUTY_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(LED_INDICATOR_DUTY_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_battery_threshold
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_threshold( enum battery_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * 0: BattOVV threshold = 3.7v (typ) 
     * 1: BattOVV threshold = 4.75v (typ) 
     */ 
    switch( param ){
        case BATTERY_THRESHOLD_4_75_E: 
           value = old_value | BATTERY_THRESHOLD_MASK; 
           break;
        case BATTERY_THRESHOLD_3_70_E: 
           value = old_value & ~ BATTERY_THRESHOLD_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_threshold
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_threshold()
  {
    unsigned char value;


    /* 
     * 0: BattOVV threshold = 3.7v (typ) 
     * 1: BattOVV threshold = 4.75v (typ) 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & BATTERY_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_o_v_v
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 1:1
 *
 **************************************************************************/
unsigned char set_battery_o_v_v( enum battery_o_v_v_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * can be enabled by charger state machine or by Sw, default 
     * value depends if a charger is plug in 
     */ 
    switch( param ){
        case BATTERY_OVV_ENABLE_E: 
           value = old_value | BATTERY_OVV_MASK; 
           break;
        case BATTERY_OVV_DISABLE_E: 
           value = old_value & ~ BATTERY_OVV_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_o_v_v
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 1:1
 *
 **************************************************************************/
unsigned char get_battery_o_v_v()
  {
    unsigned char value;


    /* 
     * can be enabled by charger state machine or by Sw, default 
     * value depends if a charger is plug in 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & BATTERY_OVV_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_dithering
 *
 * IN   : param, a value to write to the regiter BatteryThresholdControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 2:2
 *
 **************************************************************************/
unsigned char set_charger_dithering( enum clock_charger_dithering_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BATTERY_THRESHOLD_CONTROL_REG);

    /* 
     * 0: clock charger dithering disabled 
     * 1: clock charger dithering enabled 
     */ 
    switch( param ){
        case CLOCK_CHARGER_DITHERING_ENABLE_E: 
           value = old_value | CHARGER_DITHERING_MASK; 
           break;
        case CLOCK_CHARGER_DITHERING_DISABLE_E: 
           value = old_value & ~ CHARGER_DITHERING_MASK;
           break;
    }
  

    I2CWrite(BATTERY_THRESHOLD_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_dithering
 *
 * RET  : Return the value of register 0xBatteryThresholdControl
 *
 * Notes : From register 0xB55, bits 2:2
 *
 **************************************************************************/
unsigned char get_charger_dithering()
  {
    unsigned char value;


    /* 
     * 0: clock charger dithering disabled 
     * 1: clock charger dithering enabled 
     */ 
    value = (I2CRead(BATTERY_THRESHOLD_CONTROL_REG) & CHARGER_DITHERING_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_charger
 *
 * IN   : param, a value to write to the regiter ChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerControl
 *
 * Notes : From register 0xB56, bits 0:0
 *
 **************************************************************************/
unsigned char set_reset_charger( enum reset_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_CONTROL_REG);

    /* 
     * 0: reset following registers (#) 
     * 1: inactive 
     */ 
    switch( param ){
        case RESET_CHARGER_ENABLE_E: 
           value = old_value | RESET_CHARGER_MASK; 
           break;
        case RESET_CHARGER_DISABLE_E: 
           value = old_value & ~ RESET_CHARGER_MASK;
           break;
    }
  

    I2CWrite(CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_charger
 *
 * RET  : Return the value of register 0xChargerControl
 *
 * Notes : From register 0xB56, bits 0:0
 *
 **************************************************************************/
unsigned char get_reset_charger()
  {
    unsigned char value;


    /* 
     * 0: reset following registers (#) 
     * 1: inactive 
     */ 
    value = (I2CRead(CHARGER_CONTROL_REG) & RESET_CHARGER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_blink_frequency
 *
 * IN   : param, a value to write to the regiter ChargerLedBlinkControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 2:0
 *
 **************************************************************************/
unsigned char set_blink_frequency( enum blink_frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_LED_BLINK_CONTROL_REG);

    /* 
     * Blink Frequency 
     */ 
  

     value =  old_value & ~BLINK_FREQUENCY_MASK ;

    switch(  param ){

           case BLINKING_PERIOD_4S_E:
                value =  value | BLINKING_PERIOD_4S ;
                break;
           case BLINKING_PERIOD_3S_E:
                value =  value | BLINKING_PERIOD_3S ;
                break;
           case BLINKING_PERIOD_2S_E:
                value =  value | BLINKING_PERIOD_2S ;
                break;
           case BLINKING_PERIOD_1S_E:
                value =  value | BLINKING_PERIOD_1S ;
                break;
           case BLINKING_PERIOD_05S_E:
                value =  value | BLINKING_PERIOD_05S ;
                break;
    }
  

    I2CWrite(CHARGER_LED_BLINK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_blink_frequency
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 2:0
 *
 **************************************************************************/
unsigned char get_blink_frequency()
  {
    unsigned char value;


    /* 
     * Blink Frequency 
     */ 
    value = (I2CRead(CHARGER_LED_BLINK_CONTROL_REG) & BLINK_FREQUENCY_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_blink_char_led
 *
 * IN   : param, a value to write to the regiter ChargerLedBlinkControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 3:3
 *
 **************************************************************************/
unsigned char set_blink_char_led( enum blink_char_led_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_LED_BLINK_CONTROL_REG);

    /* 
     * 0: Blinking on CharLed Indicator is desabled 
     * 1: Blinking on CharLed Indicator is enabled 
     */ 
    switch( param ){
        case BLINK_CHAR_LED_ENABLE_E: 
           value = old_value | BLINK_CHAR_LED_MASK; 
           break;
        case BLINK_CHAR_LED_DISABLE_E: 
           value = old_value & ~ BLINK_CHAR_LED_MASK;
           break;
    }
  

    I2CWrite(CHARGER_LED_BLINK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_blink_char_led
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 3:3
 *
 **************************************************************************/
unsigned char get_blink_char_led()
  {
    unsigned char value;


    /* 
     * 0: Blinking on CharLed Indicator is desabled 
     * 1: Blinking on CharLed Indicator is enabled 
     */ 
    value = (I2CRead(CHARGER_LED_BLINK_CONTROL_REG) & BLINK_CHAR_LED_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_blink_duty_charger_led
 *
 * IN   : param, a value to write to the regiter ChargerLedBlinkControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 7:4
 *
 **************************************************************************/
unsigned char set_blink_duty_charger_led( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CHARGER_LED_BLINK_CONTROL_REG);

    /* 
     * BlinkDutyCharLed 
     * From 1/16 to 16/16 (default '0011' is 4/16) 
     */ 
  
    value =  old_value & ~BLINK_DUTY_CHARGER_LED_MASK;


    value |= ( param << 0x4);  

    I2CWrite(CHARGER_LED_BLINK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_blink_duty_charger_led
 *
 * RET  : Return the value of register 0xChargerLedBlinkControl
 *
 * Notes : From register 0xB57, bits 7:4
 *
 **************************************************************************/
unsigned char get_blink_duty_charger_led()
  {
    unsigned char value;


    /* 
     * BlinkDutyCharLed 
     * From 1/16 to 16/16 (default '0011' is 4/16) 
     */ 
    value = (I2CRead(CHARGER_LED_BLINK_CONTROL_REG) & BLINK_DUTY_CHARGER_LED_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bat_ctrl8u
 *
 * IN   : param, a value to write to the regiter BatCtrlCurrentSource
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 0:0
 *
 **************************************************************************/
unsigned char set_bat_ctrl8u( enum bat_ctrl8u_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BAT_CTRL_CURRENT_SOURCE_REG);

    /* 
     * 0: inactive 
     * 1: enable 8uA current source 
     */ 
    switch( param ){
        case BAT_CTRL_8U_ENABLE_E: 
           value = old_value | BAT_CTRL_8U_MASK; 
           break;
        case BAT_CTRL_8U_DISABLE_E: 
           value = old_value & ~ BAT_CTRL_8U_MASK;
           break;
    }
  

    I2CWrite(BAT_CTRL_CURRENT_SOURCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bat_ctrl8u
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 0:0
 *
 **************************************************************************/
unsigned char get_bat_ctrl8u()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: enable 8uA current source 
     */ 
    value = (I2CRead(BAT_CTRL_CURRENT_SOURCE_REG) & BAT_CTRL_8U_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_bat_ctrl16u
 *
 * IN   : param, a value to write to the regiter BatCtrlCurrentSource
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 1:1
 *
 **************************************************************************/
unsigned char set_bat_ctrl16u( enum bat_ctrl16u_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BAT_CTRL_CURRENT_SOURCE_REG);

    /* 
     * 0: inactive 
     * 1: enable 16uA current source 
     */ 
    switch( param ){
        case BAT_CTRL_1_6U_ENABLE_E: 
           value = old_value | BAT_CTRL_1_6U_MASK; 
           break;
        case BAT_CTRL_1_6U_DISABLE_E: 
           value = old_value & ~ BAT_CTRL_1_6U_MASK;
           break;
    }
  

    I2CWrite(BAT_CTRL_CURRENT_SOURCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bat_ctrl16u
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 1:1
 *
 **************************************************************************/
unsigned char get_bat_ctrl16u()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: enable 16uA current source 
     */ 
    value = (I2CRead(BAT_CTRL_CURRENT_SOURCE_REG) & BAT_CTRL_1_6U_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bat_ctrl_cmp
 *
 * IN   : param, a value to write to the regiter BatCtrlCurrentSource
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 2:2
 *
 **************************************************************************/
unsigned char set_bat_ctrl_cmp( enum bat_ctrl_cmp_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BAT_CTRL_CURRENT_SOURCE_REG);

    /* 
     * 0: inactive 
     * 1: enable BatCtrl comparator 
     */ 
    switch( param ){
        case BAT_CTRL_CMP_ENABLE_E: 
           value = old_value | BAT_CTRL_CMP_MASK; 
           break;
        case BAT_CTRL_CMP_DISABLE_E: 
           value = old_value & ~ BAT_CTRL_CMP_MASK;
           break;
    }
  

    I2CWrite(BAT_CTRL_CURRENT_SOURCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bat_ctrl_cmp
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 2:2
 *
 **************************************************************************/
unsigned char get_bat_ctrl_cmp()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: enable BatCtrl comparator 
     */ 
    value = (I2CRead(BAT_CTRL_CURRENT_SOURCE_REG) & BAT_CTRL_CMP_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_pullup
 *
 * IN   : param, a value to write to the regiter BatCtrlCurrentSource
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 4:4
 *
 **************************************************************************/
unsigned char set_battery_pullup( enum pullup_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BAT_CTRL_CURRENT_SOURCE_REG);

    /* 
     * 0: disable BatCtrlPullUp resistor 
     * 1: enable BatCtrlPullUp resistor 
     */ 
    switch( param ){
        case PULLUP_ENABLE_E: 
           value = old_value | BATTERY_PULLUP_MASK; 
           break;
        case PULLUP_DISABLE_E: 
           value = old_value & ~ BATTERY_PULLUP_MASK;
           break;
    }
  

    I2CWrite(BAT_CTRL_CURRENT_SOURCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_pullup
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 4:4
 *
 **************************************************************************/
unsigned char get_battery_pullup()
  {
    unsigned char value;


    /* 
     * 0: disable BatCtrlPullUp resistor 
     * 1: enable BatCtrlPullUp resistor 
     */ 
    value = (I2CRead(BAT_CTRL_CURRENT_SOURCE_REG) & BATTERY_PULLUP_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_crontrol4uu_ena
 *
 * IN   : param, a value to write to the regiter BatCtrlCurrentSource
 * OUT  : 
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 7:7
 *
 **************************************************************************/
unsigned char set_battery_crontrol4uu_ena( enum battery_crontrol4u_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(BAT_CTRL_CURRENT_SOURCE_REG);

    /* 
     * 0: disable current source 
     * 1: enable current source 
     */ 
    switch( param ){
        case BATTERY_CRONTROL_4U_ENABLE_E: 
           value = old_value | BATTERY_CRONTROL_4UU_ENA_MASK; 
           break;
        case BATTERY_CRONTROL_4U_DISABLE_E: 
           value = old_value & ~ BATTERY_CRONTROL_4UU_ENA_MASK;
           break;
    }
  

    I2CWrite(BAT_CTRL_CURRENT_SOURCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_crontrol4uu_ena
 *
 * RET  : Return the value of register 0xBatCtrlCurrentSource
 *
 * Notes : From register 0xB60, bits 7:7
 *
 **************************************************************************/
unsigned char get_battery_crontrol4uu_ena()
  {
    unsigned char value;


    /* 
     * 0: disable current source 
     * 1: enable current source 
     */ 
    value = (I2CRead(BAT_CTRL_CURRENT_SOURCE_REG) & BATTERY_CRONTROL_4UU_ENA_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger
 *
 * IN   : param, a value to write to the regiter MainChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainChargerControl
 *
 * Notes : From register 0xB80, bits 0:0
 *
 **************************************************************************/
unsigned char set_main_charger( enum main_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_REG);

    /* 
     * 0: Disable MainCharger 
     * 1: Enable MainCharger 
     */ 
    switch( param ){
        case MAIN_CHARGER_ENABLE_E: 
           value = old_value | MAIN_CHARGER_MASK; 
           break;
        case MAIN_CHARGER_DISABLE_E: 
           value = old_value & ~ MAIN_CHARGER_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger
 *
 * RET  : Return the value of register 0xMainChargerControl
 *
 * Notes : From register 0xB80, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger()
  {
    unsigned char value;


    /* 
     * 0: Disable MainCharger 
     * 1: Enable MainCharger 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_REG) & MAIN_CHARGER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbat_overshoot_control
 *
 * IN   : param, a value to write to the regiter MainChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainChargerControl
 *
 * Notes : From register 0xB80, bits 1:1
 *
 **************************************************************************/
unsigned char set_vbat_overshoot_control( enum vbat_overshoot_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_REG);

    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    switch( param ){
        case VBAT_OVERSHOOT_ENABLE_E: 
           value = old_value | VBAT_OVERSHOOT_CONTROL_MASK; 
           break;
        case VBAT_OVERSHOOT_DISABLE_E: 
           value = old_value & ~ VBAT_OVERSHOOT_CONTROL_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbat_overshoot_control
 *
 * RET  : Return the value of register 0xMainChargerControl
 *
 * Notes : From register 0xB80, bits 1:1
 *
 **************************************************************************/
unsigned char get_vbat_overshoot_control()
  {
    unsigned char value;


    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_REG) & VBAT_OVERSHOOT_CONTROL_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_threshold
 *
 * IN   : param, a value to write to the regiter MainChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainChargerControl2
 *
 * Notes : From register 0xB81, bits 1:0
 *
 **************************************************************************/
unsigned char set_thermal_threshold( enum thermal_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_2_REG);

    /* 
     * 00: Positive thermal threshold = 150 degree C 
     * 01: Positive thermal threshold = 159 degree C 
     * 10: Positive thermal threshold = 168 degree C 
     * 11: Positive thermal threshold = 140 degree C 
     */ 
  

     value =  old_value & ~THERMAL_THRESHOLD_MASK ;

    switch(  param ){

           case THERMAL_THRESHOLD_150_E:
                value =  value | THERMAL_THRESHOLD_150 ;
                break;
           case THERMAL_THRESHOLD_159_E:
                value =  value | THERMAL_THRESHOLD_159 ;
                break;
           case THERMAL_THRESHOLD_168_E:
                value =  value | THERMAL_THRESHOLD_168 ;
                break;
           case THERMAL_THRESHOLD_140_E:
                value =  value | THERMAL_THRESHOLD_140 ;
                break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_threshold
 *
 * RET  : Return the value of register 0xMainChargerControl2
 *
 * Notes : From register 0xB81, bits 1:0
 *
 **************************************************************************/
unsigned char get_thermal_threshold()
  {
    unsigned char value;


    /* 
     * 00: Positive thermal threshold = 150 degree C 
     * 01: Positive thermal threshold = 159 degree C 
     * 10: Positive thermal threshold = 168 degree C 
     * 11: Positive thermal threshold = 140 degree C 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_2_REG) & THERMAL_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_channel_max
 *
 * IN   : param, a value to write to the regiter MainChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xMainChargerControl2
 *
 * Notes : From register 0xB81, bits 2:2
 *
 **************************************************************************/
unsigned char set_main_channel_max( enum max_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MAIN_CHARGER_CONTROL_2_REG);

    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    switch( param ){
        case CHARGE_MAX_900MA_E: 
           value = old_value | MAIN_CHANNEL_MAX_MASK; 
           break;
        case CHARGE_MAX_1A5_E: 
           value = old_value & ~ MAIN_CHANNEL_MAX_MASK;
           break;
    }
  

    I2CWrite(MAIN_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_channel_max
 *
 * RET  : Return the value of register 0xMainChargerControl2
 *
 * Notes : From register 0xB81, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_channel_max()
  {
    unsigned char value;


    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    value = (I2CRead(MAIN_CHARGER_CONTROL_2_REG) & MAIN_CHANNEL_MAX_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 0:0
 *
 **************************************************************************/
unsigned char set_ubs_charger( enum ubs_charger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: Disable USB charge 
     * 1: Enable USB charger 
     */ 
    switch( param ){
        case UBS_CHARGER_ENABLE_E: 
           value = old_value | UBS_CHARGER_MASK; 
           break;
        case UBS_CHARGER_DISABLE_E: 
           value = old_value & ~ UBS_CHARGER_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 0:0
 *
 **************************************************************************/
unsigned char get_ubs_charger()
  {
    unsigned char value;


    /* 
     * 0: Disable USB charge 
     * 1: Enable USB charger 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & UBS_CHARGER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_chg_overshoot_control
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_chg_overshoot_control( enum vbat_overshoot_control_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * [0]: enable Vbat overshoot control 
     * [1]: disable Vbat overshoot control (default) 
     */ 
    switch( param ){
        case VBAT_OVERSHOOT_CONTROL_ENABLE_E: 
           value = old_value | USB_CHG_OVERSHOOT_CONTROL_MASK; 
           break;
        case VBAT_OVERSHOOT_CONTROL_DISABLE_E: 
           value = old_value & ~ USB_CHG_OVERSHOOT_CONTROL_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_chg_overshoot_control
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_chg_overshoot_control()
  {
    unsigned char value;


    /* 
     * [0]: enable Vbat overshoot control 
     * [1]: disable Vbat overshoot control (default) 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & USB_CHG_OVERSHOOT_CONTROL_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mask_usb_chirp
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 2:2
 *
 **************************************************************************/
unsigned char set_mask_usb_chirp( enum mask_usb_chirp_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * [0]: no effect 
     * [1]: mask the signal chirp (forcing it to 0) to disable 
     * current limitation when in USB Chirp signaling ` 
     */ 
    switch( param ){
        case MASK_USB_CHIRP_ENABLE_E: 
           value = old_value | MASK_USB_CHIRP_MASK; 
           break;
        case MASK_USB_CHIRP_DISABLE_E: 
           value = old_value & ~ MASK_USB_CHIRP_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mask_usb_chirp
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 2:2
 *
 **************************************************************************/
unsigned char get_mask_usb_chirp()
  {
    unsigned char value;


    /* 
     * [0]: no effect 
     * [1]: mask the signal chirp (forcing it to 0) to disable 
     * current limitation when in USB Chirp signaling ` 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & MASK_USB_CHIRP_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_current_limit_disable
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 3:3
 *
 **************************************************************************/
unsigned char set_ubs_current_limit_disable( enum current_limit_disable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: inactive (current limitation: 900mA in HS mode, 500mA 
     * in Chirp mode) 
     * 1: disable current limitation (current limitation from 
     * ULPI path disable, current max can be up to 1.5A) 
     */ 
    switch( param ){
        case CURRENT_LIMIT_DISABLE_DISABLE_E: 
           value = old_value | UBS_CURRENT_LIMIT_DISABLE_MASK; 
           break;
        case CURRENT_LIMIT_DISABLE_INACTIVE_E: 
           value = old_value & ~ UBS_CURRENT_LIMIT_DISABLE_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_current_limit_disable
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 3:3
 *
 **************************************************************************/
unsigned char get_ubs_current_limit_disable()
  {
    unsigned char value;


    /* 
     * 0: inactive (current limitation: 900mA in HS mode, 500mA 
     * in Chirp mode) 
     * 1: disable current limitation (current limitation from 
     * ULPI path disable, current max can be up to 1.5A) 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & UBS_CURRENT_LIMIT_DISABLE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_overshoot_control
 *
 * IN   : param, a value to write to the regiter UsbChargerControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 4:4
 *
 **************************************************************************/
unsigned char set_usb_overshoot_control( enum usb_overshoot_control_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_REG);

    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    switch( param ){
        case USB_OVERSHOOT_CONTROL_ENABLE_E: 
           value = old_value | USB_OVERSHOOT_CONTROL_MASK; 
           break;
        case USB_OVERSHOOT_CONTROL_DISABLE_E: 
           value = old_value & ~ USB_OVERSHOOT_CONTROL_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_overshoot_control
 *
 * RET  : Return the value of register 0xUsbChargerControl
 *
 * Notes : From register 0xBC0, bits 4:4
 *
 **************************************************************************/
unsigned char get_usb_overshoot_control()
  {
    unsigned char value;


    /* 
     * 0: enable Vbat overshoot control 
     * 1: disable Vbat overshoot control 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_REG) & USB_OVERSHOOT_CONTROL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_thermal_threshold
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 1:0
 *
 **************************************************************************/
unsigned char set_ubs_charger_thermal_threshold( enum thermal_threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 00: Positive thermal threshold = 150 degree C 
     * 01: Positive thermal threshold = 159 degree C 
     * 10: Positive thermal threshold = 168 degree C 
     * 11: Positive thermal threshold = 140 degree C 
     */ 
  

     value =  old_value & ~UBS_CHARGER_THERMAL_THRESHOLD_MASK ;

    switch(  param ){

           case THERMAL_THRESHOLD_150_E:
                value =  value | THERMAL_THRESHOLD_150 ;
                break;
           case THERMAL_THRESHOLD_159_E:
                value =  value | THERMAL_THRESHOLD_159 ;
                break;
           case THERMAL_THRESHOLD_168_E:
                value =  value | THERMAL_THRESHOLD_168 ;
                break;
           case THERMAL_THRESHOLD_140_E:
                value =  value | THERMAL_THRESHOLD_140 ;
                break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_thermal_threshold
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 1:0
 *
 **************************************************************************/
unsigned char get_ubs_charger_thermal_threshold()
  {
    unsigned char value;


    /* 
     * 00: Positive thermal threshold = 150 degree C 
     * 01: Positive thermal threshold = 159 degree C 
     * 10: Positive thermal threshold = 168 degree C 
     * 11: Positive thermal threshold = 140 degree C 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & UBS_CHARGER_THERMAL_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_max_current
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 2:2
 *
 **************************************************************************/
unsigned char set_ubs_charger_max_current( enum max_current param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    switch( param ){
        case CHARGE_MAX_900MA_E: 
           value = old_value | UBS_CHARGER_MAX_CURRENT_MASK; 
           break;
        case CHARGE_MAX_1A5_E: 
           value = old_value & ~ UBS_CHARGER_MAX_CURRENT_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_max_current
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 2:2
 *
 **************************************************************************/
unsigned char get_ubs_charger_max_current()
  {
    unsigned char value;


    /* 
     * 0: I charge Max = 900 mA 
     * 1: I charge Max = 1.5 A 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & UBS_CHARGER_MAX_CURRENT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_voltage_stop
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 6:3
 *
 **************************************************************************/
unsigned char set_ubs_voltage_stop( enum vbus_voltage_stop param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * Charging stops is Vbus voltage is higher than 
     */ 
  
    value =  old_value & ~UBS_VOLTAGE_STOP_MASK;


    value |= ( param << 0x3);  
    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_voltage_stop
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 6:3
 *
 **************************************************************************/
unsigned char get_ubs_voltage_stop()
  {
    unsigned char value;


    /* 
     * Charging stops is Vbus voltage is higher than 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & UBS_VOLTAGE_STOP_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_monitor
 *
 * IN   : param, a value to write to the regiter UsbChargerControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 7:7
 *
 **************************************************************************/
unsigned char set_vbus_monitor( enum vbus_monitor_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_2_REG);

    /* 
     * 0: Inactive 
     * 1: when Vbus drops below VbusValid (4.4V), Usb charging 
     * will stop. SW needs to restart charger by setting 
     * 'UsbChEna' bit low then high 
     */ 
    switch( param ){
        case VBUS_MONITOR_ENABLE_E: 
           value = old_value | VBUS_MONITOR_MASK; 
           break;
        case VBUS_MONITOR_DISABLE_E: 
           value = old_value & ~ VBUS_MONITOR_MASK;
           break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_monitor
 *
 * RET  : Return the value of register 0xUsbChargerControl2
 *
 * Notes : From register 0xBC1, bits 7:7
 *
 **************************************************************************/
unsigned char get_vbus_monitor()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: when Vbus drops below VbusValid (4.4V), Usb charging 
     * will stop. SW needs to restart charger by setting 
     * 'UsbChEna' bit low then high 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_2_REG) & VBUS_MONITOR_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ubs_charger_current_limit
 *
 * IN   : param, a value to write to the regiter UsbChargerControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbChargerControl3
 *
 * Notes : From register 0xBC2, bits 6:4
 *
 **************************************************************************/
unsigned char set_ubs_charger_current_limit( enum usb_input_current_charge param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_CHARGER_CONTROL_3_REG);

    /* 
     */ 
  

     value =  old_value & ~UBS_CHARGER_CURRENT_LIMIT_MASK ;

    switch(  param ){

           case USB_CURRENT_CHARGE_50MA_E:
                value  = value  | (USB_CURRENT_CHARGE_50MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_98MA_E:
                value  = value  | (USB_CURRENT_CHARGE_98MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_193MA_E:
                value  = value  | (USB_CURRENT_CHARGE_193MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_290MA_E:
                value  = value  | (USB_CURRENT_CHARGE_290MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_380MA_E:
                value  = value  | (USB_CURRENT_CHARGE_380MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_450MA_E:
                value  = value  | (USB_CURRENT_CHARGE_450MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_500MA_E:
                value  = value  | (USB_CURRENT_CHARGE_500MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_600MA_E:
                value  = value  | (USB_CURRENT_CHARGE_600MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_700MA_E:
                value  = value  | (USB_CURRENT_CHARGE_700MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_800MA_E:
                value  = value  | (USB_CURRENT_CHARGE_800MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_900MA_E:
                value  = value  | (USB_CURRENT_CHARGE_900MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1000MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1000MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1100MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1100MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1300MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1300MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1400MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1400MA << 0x4);
                break;
           case USB_CURRENT_CHARGE_1500MA_E:
                value  = value  | (USB_CURRENT_CHARGE_1500MA << 0x4);
                break;
    }
  

    I2CWrite(USB_CHARGER_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ubs_charger_current_limit
 *
 * RET  : Return the value of register 0xUsbChargerControl3
 *
 * Notes : From register 0xBC2, bits 6:4
 *
 **************************************************************************/
unsigned char get_ubs_charger_current_limit()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_CHARGER_CONTROL_3_REG) & UBS_CHARGER_CURRENT_LIMIT_MASK) >> 4;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
