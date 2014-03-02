/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_MaskInterrupt/Linux/ab9540_MaskInterrupt.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_C
#define C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_MaskInterrupt.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_rising_pon_key1_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 7:7
 *
 **************************************************************************/
unsigned char set_rising_pon_key1_masked_interrupt( enum rising_pon_key1_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Rising edge on PonKey1 ball masked 
     */ 
    switch( param ){
        case RISING_PON_KEY_1_MASK_ENABLE_E: 
           value = old_value | RISING_PON_KEY_1_MASKED_INTERRUPT_MASK; 
           break;
        case RISING_PON_KEY_1_MASK_NO_ACTION_E: 
           value = old_value & ~ RISING_PON_KEY_1_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rising_pon_key1_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 7:7
 *
 **************************************************************************/
unsigned char get_rising_pon_key1_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Rising edge on PonKey1 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & RISING_PON_KEY_1_MASKED_INTERRUPT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_falling_pon_key1_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 6:6
 *
 **************************************************************************/
unsigned char set_falling_pon_key1_masked_interrupt( enum falling_pon_key1_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Falling edge on PonKey1 ball masked 
     */ 
    switch( param ){
        case FALLING_PON_KEY_1_MASK_ENABLE_E: 
           value = old_value | FALLING_PON_KEY_1_MASKED_INTERRUPT_MASK; 
           break;
        case FALLING_PON_KEY_1_MASK_NO_ACTION_E: 
           value = old_value & ~ FALLING_PON_KEY_1_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_falling_pon_key1_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 6:6
 *
 **************************************************************************/
unsigned char get_falling_pon_key1_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Falling edge on PonKey1 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & FALLING_PON_KEY_1_MASKED_INTERRUPT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rising_pon_key2_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 5:5
 *
 **************************************************************************/
unsigned char set_rising_pon_key2_masked_interrupt( enum rising_pon_key2_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Rising edge on PonKey2 ball masked 
     */ 
    switch( param ){
        case RISING_PON_KEY_2_MASK_ENABLE_E: 
           value = old_value | RISING_PON_KEY_2_MASKED_INTERRUPT_MASK; 
           break;
        case RISING_PON_KEY_2_MASK_NO_ACTION_E: 
           value = old_value & ~ RISING_PON_KEY_2_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rising_pon_key2_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 5:5
 *
 **************************************************************************/
unsigned char get_rising_pon_key2_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Rising edge on PonKey2 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & RISING_PON_KEY_2_MASKED_INTERRUPT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_falling_pon_key2_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 4:4
 *
 **************************************************************************/
unsigned char set_falling_pon_key2_masked_interrupt( enum falling_pon_key2_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Falling edge on PonKey2 ball masked 
     */ 
    switch( param ){
        case FALLING_PON_KEY_2_MASK_ENABLE_E: 
           value = old_value | FALLING_PON_KEY_2_MASKED_INTERRUPT_MASK; 
           break;
        case FALLING_PON_KEY_2_MASK_NO_ACTION_E: 
           value = old_value & ~ FALLING_PON_KEY_2_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_falling_pon_key2_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 4:4
 *
 **************************************************************************/
unsigned char get_falling_pon_key2_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Falling edge on PonKey2 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & FALLING_PON_KEY_2_MASKED_INTERRUPT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_warning_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 3:3
 *
 **************************************************************************/
unsigned char set_thermal_warning_masked_interrupt( enum thermal_warning_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Thermal warning masked 
     */ 
    switch( param ){
        case THERMAL_WARNING_MASK_ENABLE_E: 
           value = old_value | THERMAL_WARNING_MASKED_INTERRUPT_MASK; 
           break;
        case THERMAL_WARNING_MASK_NO_ACTION_E: 
           value = old_value & ~ THERMAL_WARNING_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_warning_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 3:3
 *
 **************************************************************************/
unsigned char get_thermal_warning_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Thermal warning masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & THERMAL_WARNING_MASKED_INTERRUPT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_plug_tv_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 2:2
 *
 **************************************************************************/
unsigned char set_plug_tv_masked_interrupt( enum plug_tv_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Tvset (75 Ohms) connection on CVBS ball detected masked 
     */ 
    switch( param ){
        case PLUG_TV_MASK_ENABLE_E: 
           value = old_value | PLUG_TV_MASKED_INTERRUPT_MASK; 
           break;
        case PLUG_TV_MASK_NO_ACTION_E: 
           value = old_value & ~ PLUG_TV_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_plug_tv_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 2:2
 *
 **************************************************************************/
unsigned char get_plug_tv_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Tvset (75 Ohms) connection on CVBS ball detected masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & PLUG_TV_MASKED_INTERRUPT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_un_plug_tv_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 1:1
 *
 **************************************************************************/
unsigned char set_un_plug_tv_masked_interrupt( enum un_plug_tv_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Tvset (75 Ohms) disconnection on CVBS ball detected 
     * masked 
     */ 
    switch( param ){
        case UN_PLUG_TV_MASK_ENABLE_E: 
           value = old_value | UN_PLUG_TV_MASKED_INTERRUPT_MASK; 
           break;
        case UN_PLUG_TV_MASK_NO_ACTION_E: 
           value = old_value & ~ UN_PLUG_TV_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_un_plug_tv_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 1:1
 *
 **************************************************************************/
unsigned char get_un_plug_tv_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Tvset (75 Ohms) disconnection on CVBS ball detected 
     * masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & UN_PLUG_TV_MASKED_INTERRUPT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_allowed_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 0:0
 *
 **************************************************************************/
unsigned char set_main_charger_allowed_masked_interrupt( enum main_charger_allowed_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * Main charger connected is not an allowed Interupt mask 
     */ 
    switch( param ){
        case MAIN_CHARGER_ALLOWED_MASK_ENABLE_E: 
           value = old_value | MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_MASK; 
           break;
        case MAIN_CHARGER_ALLOWED_MASK_NO_ACTION_E: 
           value = old_value & ~ MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_allowed_masked_interrupt
 *
 * RET  : Return the value of register 0xMaskedInterrupt1
 *
 * Notes : From register 0xE40, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_allowed_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Main charger connected is not an allowed Interupt mask 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_rising_edge_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 7:7
 *
 **************************************************************************/
unsigned char set_vbus_rising_edge_mask( enum vbus_rising_edge_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * Rising edge on Vbus ball masked 
     */ 
    switch( param ){
        case VBUS_RISING_EDGE_MASK_ENABLE_E: 
           value = old_value | VBUS_RISING_EDGE_MASK_MASK; 
           break;
        case VBUS_RISING_EDGE_MASK_NO_ACTION_E: 
           value = old_value & ~ VBUS_RISING_EDGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_rising_edge_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 7:7
 *
 **************************************************************************/
unsigned char get_vbus_rising_edge_mask()
  {
    unsigned char value;


    /* 
     * Rising edge on Vbus ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & VBUS_RISING_EDGE_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_falling_edge_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 6:6
 *
 **************************************************************************/
unsigned char set_vbus_falling_edge_mask( enum vbus_falling_edge_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * Falling edge on Vbus ball masked 
     */ 
    switch( param ){
        case VBUS_FALLING_EDGE_MASK_ENABLE_E: 
           value = old_value | VBUS_FALLING_EDGE_MASK_MASK; 
           break;
        case VBUS_FALLING_EDGE_MASK_NO_ACTION_E: 
           value = old_value & ~ VBUS_FALLING_EDGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_falling_edge_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 6:6
 *
 **************************************************************************/
unsigned char get_vbus_falling_edge_mask()
  {
    unsigned char value;


    /* 
     * Falling edge on Vbus ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & VBUS_FALLING_EDGE_MASK_MASK) >> 6;
    return value;
  }



/***************************************************************************
 *
 * Function : set_battery_over_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_over_voltage_mask( enum battery_over_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * TVbatA ball goes upper over voltage threshold masked 
     */ 
    switch( param ){
        case BATTERY_OVER_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | BATTERY_OVER_VOLTAGE_MASK_MASK; 
           break;
        case BATTERY_OVER_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_OVER_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_over_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt2
 *
 * Notes : From register 0xE41, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_over_voltage_mask()
  {
    unsigned char value;


    /* 
     * TVbatA ball goes upper over voltage threshold masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & BATTERY_OVER_VOLTAGE_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_over_current_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 7:7
 *
 **************************************************************************/
unsigned char set_main_charger_over_current_mask( enum cain_charger_over_current_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Icharge greater Icharge max programmed detected 
     */ 
    switch( param ){
        case CAIN_CHARGER_OVER_CURRENT_MASK_ENABLE_E: 
           value = old_value | MAIN_CHARGER_OVER_CURRENT_MASK_MASK; 
           break;
        case CAIN_CHARGER_OVER_CURRENT_MASK_NO_ACTION_E: 
           value = old_value & ~ MAIN_CHARGER_OVER_CURRENT_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_over_current_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 7:7
 *
 **************************************************************************/
unsigned char get_main_charger_over_current_mask()
  {
    unsigned char value;


    /* 
     * Icharge greater Icharge max programmed detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & MAIN_CHARGER_OVER_CURRENT_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_overvoltage_on_vbus_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 6:6
 *
 **************************************************************************/
unsigned char set_overvoltage_on_vbus_mask( enum overvoltage_on_vbus_l_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Overvoltage on Vbus ball detected 
     */ 
    switch( param ){
        case OVERVOLTAGE_ON_E: 
           value = old_value | OVERVOLTAGE_ON_VBUS_MASK_MASK; 
           break;
        case OVERVOLTAGE_OFF_E: 
           value = old_value & ~ OVERVOLTAGE_ON_VBUS_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_overvoltage_on_vbus_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 6:6
 *
 **************************************************************************/
unsigned char get_overvoltage_on_vbus_mask()
  {
    unsigned char value;


    /* 
     * Overvoltage on Vbus ball detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & OVERVOLTAGE_ON_VBUS_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watchdog_charger_expiration_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 5:5
 *
 **************************************************************************/
unsigned char set_watchdog_charger_expiration_mask( enum watchdog_charger_expiration_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Watchdog charger expiration detected 
     */ 
    switch( param ){
        case WATCHDOG_CHARGER_EXPIRATION_MASK_ENABLE_E: 
           value = old_value | WATCHDOG_CHARGER_EXPIRATION_MASK_MASK; 
           break;
        case WATCHDOG_CHARGER_EXPIRATION_MASK_NO_ACTION_E: 
           value = old_value & ~ WATCHDOG_CHARGER_EXPIRATION_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_watchdog_charger_expiration_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 5:5
 *
 **************************************************************************/
unsigned char get_watchdog_charger_expiration_mask()
  {
    unsigned char value;


    /* 
     * Watchdog charger expiration detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & WATCHDOG_CHARGER_EXPIRATION_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_removal_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 4:4
 *
 **************************************************************************/
unsigned char set_battery_removal_mask( enum battery_removal_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Battery removal 
     */ 
    switch( param ){
        case BATTERY_REMOVAL_MASK_ENABLE_E: 
           value = old_value | BATTERY_REMOVAL_MASK_MASK; 
           break;
        case BATTERY_REMOVAL_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_REMOVAL_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_removal_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt3
 *
 * Notes : From register 0xE42, bits 4:4
 *
 **************************************************************************/
unsigned char get_battery_removal_mask()
  {
    unsigned char value;


    /* 
     * Battery removal 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & BATTERY_REMOVAL_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_battery_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 7:7
 *
 **************************************************************************/
unsigned char set_rtc_battery_high_mask( enum rtc_battery_high_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * BackUpBat ball voltage goes above RtcBackupChg register 
     */ 
    switch( param ){
        case RTC_BATTERY_HIGH_MASK_ENABLE_E: 
           value = old_value | RTC_BATTERY_HIGH_MASK_MASK; 
           break;
        case RTC_BATTERY_HIGH_MASK_NO_ACTION_E: 
           value = old_value & ~ RTC_BATTERY_HIGH_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_battery_high_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 7:7
 *
 **************************************************************************/
unsigned char get_rtc_battery_high_mask()
  {
    unsigned char value;


    /* 
     * BackUpBat ball voltage goes above RtcBackupChg register 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & RTC_BATTERY_HIGH_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_battery_low_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 6:6
 *
 **************************************************************************/
unsigned char set_rtc_battery_low_mask( enum rtc_battery_low_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * BackUpBat ball voltage goes below RtcBackupChg register 
     */ 
    switch( param ){
        case RTC_BATTERY_LOW_MASK_ENABLE_E: 
           value = old_value | RTC_BATTERY_LOW_MASK_MASK; 
           break;
        case RTC_BATTERY_LOW_MASK_NO_ACTION_E: 
           value = old_value & ~ RTC_BATTERY_LOW_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_battery_low_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 6:6
 *
 **************************************************************************/
unsigned char get_rtc_battery_low_mask()
  {
    unsigned char value;


    /* 
     * BackUpBat ball voltage goes below RtcBackupChg register 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & RTC_BATTERY_LOW_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_battery_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 5:5
 *
 **************************************************************************/
unsigned char set_high_battery_mask( enum high_battery_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * VbatA voltage goes above LowBat register programmed 
     * threshold 
     */ 
    switch( param ){
        case HIGH_BATTERY_MASK_ENABLE_E: 
           value = old_value | HIGH_BATTERY_MASK_MASK; 
           break;
        case HIGH_BATTERY_MASK_NO_ACTION_E: 
           value = old_value & ~ HIGH_BATTERY_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_battery_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 5:5
 *
 **************************************************************************/
unsigned char get_high_battery_mask()
  {
    unsigned char value;


    /* 
     * VbatA voltage goes above LowBat register programmed 
     * threshold 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & HIGH_BATTERY_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_battery_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 4:4
 *
 **************************************************************************/
unsigned char set_low_battery_mask( enum low_battery_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * VbatA voltage goes below LowBat register programmed 
     * threshold 
     */ 
    switch( param ){
        case LOW_BATTERY_MASK_ENABLE_E: 
           value = old_value | LOW_BATTERY_MASK_MASK; 
           break;
        case LOW_BATTERY_MASK_NO_ACTION_E: 
           value = old_value & ~ LOW_BATTERY_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_battery_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 4:4
 *
 **************************************************************************/
unsigned char get_low_battery_mask()
  {
    unsigned char value;


    /* 
     * VbatA voltage goes below LowBat register programmed 
     * threshold 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & LOW_BATTERY_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_calibration_endedd_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 3:3
 *
 **************************************************************************/
unsigned char set_coulomb_calibration_endedd_mask( enum coulomb_calibration_endedd_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * Coulomb Counter has ended its calibration 
     */ 
    switch( param ){
        case COULOMB_CALIBRATION_ENDEDD_MASK_ENABLE_E: 
           value = old_value | COULOMB_CALIBRATION_ENDEDD_MASK_MASK; 
           break;
        case COULOMB_CALIBRATION_ENDEDD_MASK_NO_ACTION_E: 
           value = old_value & ~ COULOMB_CALIBRATION_ENDEDD_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_calibration_endedd_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 3:3
 *
 **************************************************************************/
unsigned char get_coulomb_calibration_endedd_mask()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended its calibration 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & COULOMB_CALIBRATION_ENDEDD_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_ended_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 2:2
 *
 **************************************************************************/
unsigned char set_coulomb_ended_mask( enum coulomb_counter_ended_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * Coulomb Counter has ended data convertion 
     */ 
    switch( param ){
        case COULOMB_COUNTER_ENDED_MASK_ENABLE_E: 
           value = old_value | COULOMB_ENDED_MASK_MASK; 
           break;
        case COULOMB_COUNTER_ENDED_MASK_NO_ACTION_E: 
           value = old_value & ~ COULOMB_ENDED_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_ended_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 2:2
 *
 **************************************************************************/
unsigned char get_coulomb_ended_mask()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended data convertion 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & COULOMB_ENDED_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 1:1
 *
 **************************************************************************/
unsigned char set_audio_mask( enum audio_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * Audio interrupt form audio digital part detected 
     */ 
    switch( param ){
        case AUDIO_MASK_ENABLE_E: 
           value = old_value | AUDIO_MASK_MASK; 
           break;
        case AUDIO_MASK_NO_ACTION_E: 
           value = old_value & ~ AUDIO_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_mask()
  {
    unsigned char value;


    /* 
     * Audio interrupt form audio digital part detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & AUDIO_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accumulation_conversion_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 0:0
 *
 **************************************************************************/
unsigned char set_accumulation_conversion_mask( enum accumulation_conversion_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * Accumulation of N sample conversion is detected 
     */ 
    switch( param ){
        case ACCUMULATION_CONVERSION_MASK_ENABLE_E: 
           value = old_value | ACCUMULATION_CONVERSION_MASK_MASK; 
           break;
        case ACCUMULATION_CONVERSION_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCUMULATION_CONVERSION_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accumulation_conversion_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt4
 *
 * Notes : From register 0xE43, bits 0:0
 *
 **************************************************************************/
unsigned char get_accumulation_conversion_mask()
  {
    unsigned char value;


    /* 
     * Accumulation of N sample conversion is detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & ACCUMULATION_CONVERSION_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_hardware_control_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 7:7
 *
 **************************************************************************/
unsigned char set_adc_hardware_control_mask( enum adc_hardware_control_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * GP ADC conversion requested thru Hardware control 
     */ 
    switch( param ){
        case ADC_HARDWARE_CONTROL_MASK_ENABLE_E: 
           value = old_value | ADC_HARDWARE_CONTROL_MASK_MASK; 
           break;
        case ADC_HARDWARE_CONTROL_MASK_NO_ACTION_E: 
           value = old_value & ~ ADC_HARDWARE_CONTROL_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_hardware_control_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 7:7
 *
 **************************************************************************/
unsigned char get_adc_hardware_control_mask()
  {
    unsigned char value;


    /* 
     * GP ADC conversion requested thru Hardware control 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ADC_HARDWARE_CONTROL_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory21_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 6:6
 *
 **************************************************************************/
unsigned char set_accessory21_high_voltage_mask( enum accessory21_high_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect2 ball voltage level goes above AccDetect21Th 
     */ 
    switch( param ){
        case ACCESSORY_21_HIGH_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_21_HIGH_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_21_HIGH_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_21_HIGH_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory21_high_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 6:6
 *
 **************************************************************************/
unsigned char get_accessory21_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes above AccDetect21Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_21_HIGH_VOLTAGE_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory21_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 5:5
 *
 **************************************************************************/
unsigned char set_accessory21_low_voltage_mask( enum accessory21_low_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect2 ball voltage level goes below AccDetect21Th 
     */ 
    switch( param ){
        case ACCESSORY_21_LOW_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_21_LOW_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_21_LOW_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_21_LOW_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory21_low_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 5:5
 *
 **************************************************************************/
unsigned char get_accessory21_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes below AccDetect21Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_21_LOW_VOLTAGE_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory22_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 4:4
 *
 **************************************************************************/
unsigned char set_accessory22_high_voltage_mask( enum accessory22_high_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect2 ball voltage level goes above AccDetect22Th 
     */ 
    switch( param ){
        case ACCESSORY_22_HIGH_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_22_HIGH_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_22_HIGH_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_22_HIGH_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory22_high_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 4:4
 *
 **************************************************************************/
unsigned char get_accessory22_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes above AccDetect22Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_22_HIGH_VOLTAGE_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory22_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 3:3
 *
 **************************************************************************/
unsigned char set_accessory22_low_voltage_mask( enum accessory22_low_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect2 ball voltage level goes below AccDetect22Th 
     */ 
    switch( param ){
        case ACCESSORY_22_LOW_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_22_LOW_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_22_LOW_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_22_LOW_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory22_low_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 3:3
 *
 **************************************************************************/
unsigned char get_accessory22_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes below AccDetect22Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_22_LOW_VOLTAGE_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory1_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 2:2
 *
 **************************************************************************/
unsigned char set_accessory1_high_voltage_mask( enum accessory1_high_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect1 ball voltage level goes above AccDetect1Th 
     */ 
    switch( param ){
        case ACCESSORY_1_HIGH_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_1_HIGH_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_1_HIGH_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_1_HIGH_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory1_high_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 2:2
 *
 **************************************************************************/
unsigned char get_accessory1_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect1 ball voltage level goes above AccDetect1Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_1_HIGH_VOLTAGE_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory1_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 1:1
 *
 **************************************************************************/
unsigned char set_accessory1_low_voltage_mask( enum accessory1_low_voltage_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * AccDetect1 ball voltage level goes below AccDetect1Th 
     */ 
    switch( param ){
        case ACCESSORY_1_LOW_VOLTAGE_MASK_ENABLE_E: 
           value = old_value | ACCESSORY_1_LOW_VOLTAGE_MASK_MASK; 
           break;
        case ACCESSORY_1_LOW_VOLTAGE_MASK_NO_ACTION_E: 
           value = old_value & ~ ACCESSORY_1_LOW_VOLTAGE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory1_low_voltage_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 1:1
 *
 **************************************************************************/
unsigned char get_accessory1_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect1 ball voltage level goes below AccDetect1Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_1_LOW_VOLTAGE_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_software_control_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 0:0
 *
 **************************************************************************/
unsigned char set_adc_software_control_mask( enum adc_software_control_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * GP ADC conversion requested thru software control is 
     * ended (data are available) 
     */ 
    switch( param ){
        case ADC_SOFTWARE_CONTROL_MASK_ENABLE_E: 
           value = old_value | ADC_SOFTWARE_CONTROL_MASK_MASK; 
           break;
        case ADC_SOFTWARE_CONTROL_MASK_NO_ACTION_E: 
           value = old_value & ~ ADC_SOFTWARE_CONTROL_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_software_control_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt5
 *
 * Notes : From register 0xE44, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc_software_control_mask()
  {
    unsigned char value;


    /* 
     * GP ADC conversion requested thru software control is 
     * ended (data are available) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ADC_SOFTWARE_CONTROL_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio6_rising_mask( enum gpio6_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO6 ball 
     */ 
    switch( param ){
        case GPIO_6_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_6_RISING_MASK_MASK; 
           break;
        case GPIO_6_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_6_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio6_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO6 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_6_RISING_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio7_rising_mask( enum gpio7_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO7 ball 
     */ 
    switch( param ){
        case GPIO_7_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_7_RISING_MASK_MASK; 
           break;
        case GPIO_7_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_7_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio7_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO7 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_7_RISING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio8_rising_mask( enum gpio8_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO8 ball 
     */ 
    switch( param ){
        case GPIO_8_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_8_RISING_MASK_MASK; 
           break;
        case GPIO_8_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_8_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio8_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO8 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_8_RISING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio9_rising_mask( enum gpio9_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO9 ball 
     */ 
    switch( param ){
        case GPIO_9_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_9_RISING_MASK_MASK; 
           break;
        case GPIO_9_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_9_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio9_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO9 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_9_RISING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio10_rising_mask( enum gpio10_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO10 ball 
     */ 
    switch( param ){
        case GPIO_10_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_10_RISING_MASK_MASK; 
           break;
        case GPIO_10_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_10_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio10_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO10 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_10_RISING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio11_rising_mask( enum gpio11_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO11 ball 
     */ 
    switch( param ){
        case GPIO_11_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_11_RISING_MASK_MASK; 
           break;
        case GPIO_11_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_11_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio11_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO11 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_11_RISING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio12_rising_mask( enum gpio12_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO12 ball 
     */ 
    switch( param ){
        case GPIO_12_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_12_RISING_MASK_MASK; 
           break;
        case GPIO_12_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_12_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio12_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO12 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_12_RISING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio13_rising_mask( enum gpio13_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * Rising edge Interrupt masked on GPIO13 ball 
     */ 
    switch( param ){
        case GPIO_13_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_13_RISING_MASK_MASK; 
           break;
        case GPIO_13_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_13_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt7
 *
 * Notes : From register 0xE46, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio13_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO13 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_13_RISING_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio24_rising_mask( enum gpio24_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO24 ball 
     */ 
    switch( param ){
        case GPIO_24_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_24_RISING_MASK_MASK; 
           break;
        case GPIO_24_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_24_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio24_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO24 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_24_RISING_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio25_rising_mask( enum gpio25_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO25 ball 
     */ 
    switch( param ){
        case GPIO_25_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_25_RISING_MASK_MASK; 
           break;
        case GPIO_25_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_25_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio25_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO25 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_25_RISING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio36_rising_mask( enum gpio36_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO36 ball 
     */ 
    switch( param ){
        case GPIO_36_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_36_RISING_MASK_MASK; 
           break;
        case GPIO_36_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_36_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio36_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO36 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_36_RISING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio37_rising_mask( enum gpio37_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO37 ball 
     */ 
    switch( param ){
        case GPIO_37_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_37_RISING_MASK_MASK; 
           break;
        case GPIO_37_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_37_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio37_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO37 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_37_RISING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio38_rising_mask( enum gpio38_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO38 ball 
     */ 
    switch( param ){
        case GPIO_38_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_38_RISING_MASK_MASK; 
           break;
        case GPIO_38_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_38_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio38_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO38 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_38_RISING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio39_rising_mask( enum gpio39_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO39 ball 
     */ 
    switch( param ){
        case GPIO_39_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_39_RISING_MASK_MASK; 
           break;
        case GPIO_39_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_39_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio39_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO39 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_39_RISING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio40_rising_mask( enum gpio40_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO40 ball 
     */ 
    switch( param ){
        case GPIO_40_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_40_RISING_MASK_MASK; 
           break;
        case GPIO_40_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_40_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio40_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO40 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_40_RISING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio41_rising_mask( enum gpio41_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * Rising edge Interrupt masked on GPIO41 ball 
     */ 
    switch( param ){
        case GPIO_41_RISING_MASK_ENABLE_E: 
           value = old_value | GPIO_41_RISING_MASK_MASK; 
           break;
        case GPIO_41_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_41_RISING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt8
 *
 * Notes : From register 0xE47, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio41_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO41 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_41_RISING_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio6_falling_mask( enum gpio6_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO6 ball 
     */ 
    switch( param ){
        case GPIO_6_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_6_FALLING_MASK_MASK; 
           break;
        case GPIO_6_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_6_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio6_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO6 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_6_FALLING_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio7_falling_mask( enum gpio7_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO7 ball 
     */ 
    switch( param ){
        case GPIO_7_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_7_FALLING_MASK_MASK; 
           break;
        case GPIO_7_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_7_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio7_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO7 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_7_FALLING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio8_falling_mask( enum gpio8_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO8 ball 
     */ 
    switch( param ){
        case GPIO_8_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_8_FALLING_MASK_MASK; 
           break;
        case GPIO_8_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_8_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio8_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO8 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_8_FALLING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio9_falling_mask( enum gpio9_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO9 ball 
     */ 
    switch( param ){
        case GPIO_9_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_9_FALLING_MASK_MASK; 
           break;
        case GPIO_9_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_9_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio9_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO9 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_9_FALLING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio10_falling_mask( enum gpio10_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO10 ball 
     */ 
    switch( param ){
        case GPIO_10_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_10_FALLING_MASK_MASK; 
           break;
        case GPIO_10_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_10_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio10_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO10 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_10_FALLING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio11_falling_mask( enum gpio11_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO11 ball 
     */ 
    switch( param ){
        case GPIO_11_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_11_FALLING_MASK_MASK; 
           break;
        case GPIO_11_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_11_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio11_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO11 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_11_FALLING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio12_falling_mask( enum gpio12_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO12 ball 
     */ 
    switch( param ){
        case GPIO_12_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_12_FALLING_MASK_MASK; 
           break;
        case GPIO_12_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_12_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio12_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO12 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_12_FALLING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio13_falling_mask( enum gpio13_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge Masked on GPIO13 ball 
     */ 
    switch( param ){
        case GPIO_13_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_13_FALLING_MASK_MASK; 
           break;
        case GPIO_13_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_13_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt9
 *
 * Notes : From register 0xE48, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio13_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO13 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_13_FALLING_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio24_falling_mask( enum gpio24_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO24 ball 
     */ 
    switch( param ){
        case GPIO_24_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_24_FALLING_MASK_MASK; 
           break;
        case GPIO_24_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_24_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio24_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO24 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_24_FALLING_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio25_falling_mask( enum gpio25_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO25 ball 
     */ 
    switch( param ){
        case GPIO_25_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_25_FALLING_MASK_MASK; 
           break;
        case GPIO_25_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_25_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio25_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO25 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_25_FALLING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio36_falling_mask( enum gpio36_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO36 ball 
     */ 
    switch( param ){
        case GPIO_36_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_36_FALLING_MASK_MASK; 
           break;
        case GPIO_36_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_36_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio36_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO36 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_36_FALLING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio37_falling_mask( enum gpio37_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO37 ball 
     */ 
    switch( param ){
        case GPIO_37_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_37_FALLING_MASK_MASK; 
           break;
        case GPIO_37_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_37_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio37_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO37 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_37_FALLING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio38_falling_mask( enum gpio38_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO38 ball 
     */ 
    switch( param ){
        case GPIO_38_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_38_FALLING_MASK_MASK; 
           break;
        case GPIO_38_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_38_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio38_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO38 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_38_FALLING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio39_falling_mask( enum gpio39_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO39 ball 
     */ 
    switch( param ){
        case GPIO_39_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_39_FALLING_MASK_MASK; 
           break;
        case GPIO_39_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_39_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio39_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO39 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_39_FALLING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio40_falling_mask( enum gpio40_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO40 ball 
     */ 
    switch( param ){
        case GPIO_40_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_40_FALLING_MASK_MASK; 
           break;
        case GPIO_40_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_40_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio40_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO40 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_40_FALLING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio41_falling_mask( enum gpio41_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge Masked on GPIO41 ball 
     */ 
    switch( param ){
        case GPIO_41_FALLING_MASK_ENABLE_E: 
           value = old_value | GPIO_41_FALLING_MASK_MASK; 
           break;
        case GPIO_41_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ GPIO_41_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt10
 *
 * Notes : From register 0xE49, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio41_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge Masked on GPIO41 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_41_FALLING_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on GPIO50 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & GPIO_50_RISING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on GPIO51 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & GPIO_51_RISING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on GPIO52 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & GPIO_52_RISING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on GPIO53 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & GPIO_53_RISING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on GPIO60 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & GPIO_60_RISING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_external_charger_rising_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt13
 *
 * Notes : From register 0xE4C, bits 6:6
 *
 **************************************************************************/
unsigned char get_external_charger_rising_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Rising edge Masked on IEXTCHRFBN ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_13_REG) & EXTERNAL_CHARGER_RISING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on GPIO50 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & GPIO_50_FALLING_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on GPIO51 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & GPIO_51_FALLING_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on GPIO52 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & GPIO_52_FALLING_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on GPIO53 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & GPIO_53_FALLING_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on GPIO60 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & GPIO_60_FALLING_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_external_charger_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt14
 *
 * Notes : From register 0xE4D, bits 6:6
 *
 **************************************************************************/
unsigned char get_external_charger_falling_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Falling edge Masked on IEXTCHRFBN ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_14_REG) & EXTERNAL_CHARGER_FALLING_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_low_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_temperature_low_mask( enum battery_temperature_low_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1:Interrupt 'Btemp lower BtempLow' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_LOW_MASK_ENABLE_E: 
           value = old_value | BATTERY_TEMPERATURE_LOW_MASK_MASK; 
           break;
        case BATTERY_TEMPERATURE_LOW_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_LOW_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_temperature_low_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1:Interrupt 'Btemp lower BtempLow' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_LOW_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_low_medium_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 1:1
 *
 **************************************************************************/
unsigned char set_battery_temperature_low_medium_mask( enum battery_temperature_low_medium_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempMedium greater Btemp greater BtempLow' 
     * masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_ENABLE_E: 
           value = old_value | BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_MASK; 
           break;
        case BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low_medium_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 1:1
 *
 **************************************************************************/
unsigned char get_battery_temperature_low_medium_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempMedium greater Btemp greater BtempLow' 
     * masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_medium_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 2:2
 *
 **************************************************************************/
unsigned char set_battery_temperature_medium_high_mask( enum battery_temperature_medium_high_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempHigh greater Btemp greater 
     * BtempMedium' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_ENABLE_E: 
           value = old_value | BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_MASK; 
           break;
        case BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_medium_high_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 2:2
 *
 **************************************************************************/
unsigned char get_battery_temperature_medium_high_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempHigh greater Btemp greater 
     * BtempMedium' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 3:3
 *
 **************************************************************************/
unsigned char set_battery_temperature_high_mask( enum battery_temperature_high_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'Btemp greater BtempHigh' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_HIGH_MASK_ENABLE_E: 
           value = old_value | BATTERY_TEMPERATURE_HIGH_MASK_MASK; 
           break;
        case BATTERY_TEMPERATURE_HIGH_MASK_NO_ACTION_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_HIGH_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_high_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt19
 *
 * Notes : From register 0xE52, bits 3:3
 *
 **************************************************************************/
unsigned char get_battery_temperature_high_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'Btemp greater BtempHigh' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_HIGH_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater220k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 7:7
 *
 **************************************************************************/
unsigned char set_id_detection_greater220k_mask( enum id_detection_greater220k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * ID resistance greater than 220k detected 
     */ 
    switch( param ){
        case ID_DETECTION_GREATER_22_0K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_GREATER_22_0K_MASK_MASK; 
           break;
        case ID_DETECTION_GREATER_22_0K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_GREATER_22_0K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater220k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 7:7
 *
 **************************************************************************/
unsigned char get_id_detection_greater220k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 220k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_22_0K_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater150k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 6:6
 *
 **************************************************************************/
unsigned char set_id_detection_greater150k_mask( enum id_detection_greater125k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * ID resistance greater than 125k detected 
     */ 
    switch( param ){
        case ID_DETECTION_GREATER_12_5K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_GREATER_15_0K_MASK_MASK; 
           break;
        case ID_DETECTION_GREATER_12_5K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_GREATER_15_0K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater150k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 6:6
 *
 **************************************************************************/
unsigned char get_id_detection_greater150k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 125k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_15_0K_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater69k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 5:5
 *
 **************************************************************************/
unsigned char set_id_detection_greater69k_mask( enum id_detection_greater69k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * ID resistance greater than 69k detected 
     */ 
    switch( param ){
        case ID_DETECTION_GREATER_6_9K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_GREATER_6_9K_MASK_MASK; 
           break;
        case ID_DETECTION_GREATER_6_9K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_GREATER_6_9K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater69k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 5:5
 *
 **************************************************************************/
unsigned char get_id_detection_greater69k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 69k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_6_9K_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater37k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 4:4
 *
 **************************************************************************/
unsigned char set_id_detection_greater37k_mask( enum id_detection_greater37k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * ID resistance greater than 37k detected 
     */ 
    switch( param ){
        case ID_DETECTION_GREATER_3_7K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_GREATER_3_7K_MASK_MASK; 
           break;
        case ID_DETECTION_GREATER_3_7K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_GREATER_3_7K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater37k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_detection_greater37k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 37k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_3_7K_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wake_up_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 2:2
 *
 **************************************************************************/
unsigned char set_id_wake_up_mask( enum id_wake_up_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * Rising edge detected on WakeUp 
     */ 
    switch( param ){
        case ID_WAKE_UP_RISING_MASK_ENABLE_E: 
           value = old_value | ID_WAKE_UP_MASK_MASK; 
           break;
        case ID_WAKE_UP_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_WAKE_UP_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wake_up_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_wake_up_mask()
  {
    unsigned char value;


    /* 
     * Rising edge detected on WakeUp 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_WAKE_UP_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_plug_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_charger_plug_mask( enum usb_charger_plug_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * 0: Inactive 
     * 1: not allowed USB charger detection on Vbus interrupt 
     * masked 
     */ 
    switch( param ){
        case USB_CHARGER_PLUG_MASK_ENABLE_E: 
           value = old_value | USB_CHARGER_PLUG_MASK_MASK; 
           break;
        case USB_CHARGER_PLUG_MASK_NO_ACTION_E: 
           value = old_value & ~ USB_CHARGER_PLUG_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_plug_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_charger_plug_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: not allowed USB charger detection on Vbus interrupt 
     * masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & USB_CHARGER_PLUG_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_srp_dectect_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 0:0
 *
 **************************************************************************/
unsigned char set_srp_dectect_mask( enum dplus_rising_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * 0: Inactive 
     * 1: SRP detect Interrupt masked 
     */ 
    switch( param ){
        case DPLUS_RISING_MASK_ENABLE_E: 
           value = old_value | SRP_DECTECT_MASK_MASK; 
           break;
        case DPLUS_RISING_MASK_NO_ACTION_E: 
           value = old_value & ~ SRP_DECTECT_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_srp_dectect_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt20
 *
 * Notes : From register 0xE53, bits 0:0
 *
 **************************************************************************/
unsigned char get_srp_dectect_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: SRP detect Interrupt masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & SRP_DECTECT_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ch_stop_by_sec
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 7:7
 *
 **************************************************************************/
unsigned char set_ch_stop_by_sec( enum charger_sttoped_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * 0: Inactive 
     * 1: Charging is stopped by security reason interrupt 
     * masked 
     */ 
    switch( param ){
        case CHARGER_STTOPED_MASK_ENABLE_E: 
           value = old_value | CH_STOP_BY_SEC_MASK; 
           break;
        case CHARGER_STTOPED_MASK_NO_ACTION_E: 
           value = old_value & ~ CH_STOP_BY_SEC_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ch_stop_by_sec
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 7:7
 *
 **************************************************************************/
unsigned char get_ch_stop_by_sec()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Charging is stopped by security reason interrupt 
     * masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & CH_STOP_BY_SEC_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 6:6
 *
 **************************************************************************/
unsigned char set_usb_charger_mask( enum usb_charger_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * USB charger detected 
     */ 
    switch( param ){
        case USB_CHARGER_MASK_ENABLE_E: 
           value = old_value | USB_CHARGER_MASK_MASK; 
           break;
        case USB_CHARGER_MASK_NO_ACTION_E: 
           value = old_value & ~ USB_CHARGER_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_charger_mask()
  {
    unsigned char value;


    /* 
     * USB charger detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & USB_CHARGER_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection220k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 5:5
 *
 **************************************************************************/
unsigned char set_id_detection220k_mask( enum id_detection220k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * ID resistance lower than 220k detected 
     */ 
    switch( param ){
        case ID_DETECTION_22_0K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_22_0K_MASK_MASK; 
           break;
        case ID_DETECTION_22_0K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_22_0K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection220k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 5:5
 *
 **************************************************************************/
unsigned char get_id_detection220k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 220k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_22_0K_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection150k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 4:4
 *
 **************************************************************************/
unsigned char set_id_detection150k_mask( enum id_detection125k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * ID resistance lower than 125k detected 
     */ 
    switch( param ){
        case ID_DETECTION_12_5K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_15_0K_MASK_MASK; 
           break;
        case ID_DETECTION_12_5K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_15_0K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection150k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_detection150k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 125k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_15_0K_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection69k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 3:3
 *
 **************************************************************************/
unsigned char set_id_detection69k_mask( enum id_detection69k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * ID resistance lower than 69k detected 
     */ 
    switch( param ){
        case ID_DETECTION_6_9K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_6_9K_MASK_MASK; 
           break;
        case ID_DETECTION_6_9K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_6_9K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection69k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 3:3
 *
 **************************************************************************/
unsigned char get_id_detection69k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 69k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_6_9K_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection37k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 2:2
 *
 **************************************************************************/
unsigned char set_id_detection37k_mask( enum id_detection37k_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * ID resistance lower than 37k detected 
     */ 
    switch( param ){
        case ID_DETECTION_3_7K_MASK_ENABLE_E: 
           value = old_value | ID_DETECTION_3_7K_MASK_MASK; 
           break;
        case ID_DETECTION_3_7K_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_DETECTION_3_7K_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection37k_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_detection37k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 37k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_3_7K_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wake_up_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 0:0
 *
 **************************************************************************/
unsigned char set_id_wake_up_falling_mask( enum id_wake_up_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * Falling edge on ID Interrupt masked 
     */ 
    switch( param ){
        case ID_WAKE_UP_FALLING_MASK_ENABLE_E: 
           value = old_value | ID_WAKE_UP_FALLING_MASK_MASK; 
           break;
        case ID_WAKE_UP_FALLING_MASK_NO_ACTION_E: 
           value = old_value & ~ ID_WAKE_UP_FALLING_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wake_up_falling_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt21
 *
 * Notes : From register 0xE54, bits 0:0
 *
 **************************************************************************/
unsigned char get_id_wake_up_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge on ID Interrupt masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_WAKE_UP_FALLING_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_thermal_below_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_charger_thermal_below_mask( enum usb_charger_thermal_below_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: Die temperature is above than usb charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than usb charger thermal 
     * protection threshold (charge stopped) 
     */ 
    switch( param ){
        case USB_CHARGER_THERMAL_BELOW_MASK_ENABLE_E: 
           value = old_value | USB_CHARGER_THERMAL_BELOW_MASK_MASK; 
           break;
        case USB_CHARGER_THERMAL_BELOW_MASK_NO_ACTION_E: 
           value = old_value & ~ USB_CHARGER_THERMAL_BELOW_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_thermal_below_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_charger_thermal_below_mask()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is above than usb charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than usb charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGER_THERMAL_BELOW_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_thermal_above_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_charger_thermal_above_mask( enum usb_charger_thermal_above_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: Die temperature is above than usb charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than usb charger thermal 
     * protection threshold (charge stopped) 
     */ 
    switch( param ){
        case USB_CHARGER_THERMAL_ABOVE_MASK_ENABLE_E: 
           value = old_value | USB_CHARGER_THERMAL_ABOVE_MASK_MASK; 
           break;
        case USB_CHARGER_THERMAL_ABOVE_MASK_NO_ACTION_E: 
           value = old_value & ~ USB_CHARGER_THERMAL_ABOVE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_thermal_above_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_charger_thermal_above_mask()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is above than usb charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than usb charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGER_THERMAL_ABOVE_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_thermal_below_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 2:2
 *
 **************************************************************************/
unsigned char set_main_charger_thermal_below_mask( enum main_charger_thermal_below_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: Die temperature is below than main charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than main charger thermal 
     * protection threshold (charge stopped) 
     */ 
    switch( param ){
        case MAIN_CHARGER_THERMAL_BELOW_MASK_ENABLE_E: 
           value = old_value | MAIN_CHARGER_THERMAL_BELOW_MASK_MASK; 
           break;
        case MAIN_CHARGER_THERMAL_BELOW_MASK_NO_ACTION_E: 
           value = old_value & ~ MAIN_CHARGER_THERMAL_BELOW_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_thermal_below_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_charger_thermal_below_mask()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is below than main charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than main charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & MAIN_CHARGER_THERMAL_BELOW_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_thermal_above_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 3:3
 *
 **************************************************************************/
unsigned char set_main_charger_thermal_above_mask( enum main_charger_thermal_above_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: Die temperature is above than main charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than main charger thermal 
     * protection threshold (charge stopped) 
     */ 
    switch( param ){
        case MAIN_CHARGER_THERMAL_ABOVE_MASK_ENABLE_E: 
           value = old_value | MAIN_CHARGER_THERMAL_ABOVE_MASK_MASK; 
           break;
        case MAIN_CHARGER_THERMAL_ABOVE_MASK_NO_ACTION_E: 
           value = old_value & ~ MAIN_CHARGER_THERMAL_ABOVE_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_thermal_above_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_charger_thermal_above_mask()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is above than main charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than main charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & MAIN_CHARGER_THERMAL_ABOVE_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charging_current_no_more_limited_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 5:5
 *
 **************************************************************************/
unsigned char set_charging_current_no_more_limited_mask( enum charging_current_no_more_limited_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: IT unmasked 
     * 1: IT_ChargingCurrentNoMoreLimited masked 
     */ 
    switch( param ){
        case CHARGING_CURRENT_NO_MORE_LIMITED_MASK_ENABLE_E: 
           value = old_value | CHARGING_CURRENT_NO_MORE_LIMITED_MASK_MASK; 
           break;
        case CHARGING_CURRENT_NO_MORE_LIMITED_MASK_NO_ACTION_E: 
           value = old_value & ~ CHARGING_CURRENT_NO_MORE_LIMITED_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_no_more_limited_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 5:5
 *
 **************************************************************************/
unsigned char get_charging_current_no_more_limited_mask()
  {
    unsigned char value;


    /* 
     * 0: IT unmasked 
     * 1: IT_ChargingCurrentNoMoreLimited masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & CHARGING_CURRENT_NO_MORE_LIMITED_MASK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charging_current_limited_h_s_chirp_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 6:6
 *
 **************************************************************************/
unsigned char set_charging_current_limited_h_s_chirp_mask( enum charging_current_limited_h_s_chirp_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: IT unmasked 
     * 1: IT_ChargingCurrentLimitedHSChirp masked 
     */ 
    switch( param ){
        case CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_ENABLE_E: 
           value = old_value | CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_MASK; 
           break;
        case CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_NO_ACTION_E: 
           value = old_value & ~ CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_limited_h_s_chirp_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 6:6
 *
 **************************************************************************/
unsigned char get_charging_current_limited_h_s_chirp_mask()
  {
    unsigned char value;


    /* 
     * 0: IT unmasked 
     * 1: IT_ChargingCurrentLimitedHSChirp masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & CHARGING_CURRENT_LIMITED_HS_CHIRP_MASK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_xtal32_ok_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 7:7
 *
 **************************************************************************/
unsigned char set_xtal32_ok_mask( enum xtal32_ok_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * 0: IT unmasked 
     * 1: XTAL startup time failure masked 
     */ 
    switch( param ){
        case XTAL_32_OK_MASK_ENABLE_E: 
           value = old_value | XTAL_32_OK_MASK_MASK; 
           break;
        case XTAL_32_OK_MASK_NO_ACTION_E: 
           value = old_value & ~ XTAL_32_OK_MASK_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_xtal32_ok_mask
 *
 * RET  : Return the value of register 0xMaskedInterrupt22
 *
 * Notes : From register 0xE55, bits 7:7
 *
 **************************************************************************/
unsigned char get_xtal32_ok_mask()
  {
    unsigned char value;


    /* 
     * 0: IT unmasked 
     * 1: XTAL startup time failure masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & XTAL_32_OK_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_key_stuck_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 0:0
 *
 **************************************************************************/
unsigned char get_key_stuck_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & KEY_STUCK_MASK_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_lpr_vdddig_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 1:1
 *
 **************************************************************************/
unsigned char get_lpr_vdddig_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & LPR_VDDDIG_MASK_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_l_kp_vdddig_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 2:2
 *
 **************************************************************************/
unsigned char get_l_kp_vdddig_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & L_KP_VDDDIG_MASK_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_kp_vdddig_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 3:3
 *
 **************************************************************************/
unsigned char get_kp_vdddig_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & KP_VDDDIG_MASK_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_key_deglitch_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 4:4
 *
 **************************************************************************/
unsigned char get_key_deglitch_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & KEY_DEGLITCH_MASK_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_modem_power_status_mask
 *
 * RET  : Return the value of register 0xMaskInterrupt25
 *
 * Notes : From register 0xE58, bits 7:6
 *
 **************************************************************************/
unsigned char get_modem_power_status_mask()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(MASK_INTERRUPT_25_REG) & MODEM_POWER_STATUS_MASK_MASK) >> 6;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
