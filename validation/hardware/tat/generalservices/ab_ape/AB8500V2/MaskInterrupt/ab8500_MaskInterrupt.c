/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_MaskInterrupt/Linux/ab8500_MaskInterrupt.c
 * 
 *
 * Generated on the 31/05/2011 12:28 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : UM0836 User manual, AB8500_V2 Mixed signal multimedia and power management Rev 1
 *
 ***************************************************************************/
#include "ab8500_MaskInterrupt.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_masked_interrupt1
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt1( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_1_REG);

    /* 
     * IntSource1 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_1_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt1
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt1()
  {
    unsigned char value;


    /* 
     * IntSource1 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_1_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_allowed_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 0:0
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
        case MAIN_CHARGER_ALLOWED_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_ALLOWED_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_ALLOWED_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_ALLOWED_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_allowed_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_allowed_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Main charger connected is not an allowed Interupt mask 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & MAIN_CHARGER_ALLOWED_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_un_plug_tv_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 1:1
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
        case UN_PLUG_TV_MASK_SET_E: 
           value = old_value | UN_PLUG_TV_MASK_SET_PARAM_MASK; 
           break;
        case UN_PLUG_TV_MASK_RESET_SET_E: 
           value = old_value & ~ UN_PLUG_TV_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_un_plug_tv_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 1:1
 *
 **************************************************************************/
unsigned char get_un_plug_tv_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Tvset (75 Ohms) disconnection on CVBS ball detected 
     * masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & UN_PLUG_TV_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_plug_tv_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 2:2
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
        case PLUG_TV_MASK_SET_E: 
           value = old_value | PLUG_TV_MASK_SET_PARAM_MASK; 
           break;
        case PLUG_TV_MASK_RESET_SET_E: 
           value = old_value & ~ PLUG_TV_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_plug_tv_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 2:2
 *
 **************************************************************************/
unsigned char get_plug_tv_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Tvset (75 Ohms) connection on CVBS ball detected masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & PLUG_TV_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_thermal_warning_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 3:3
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
        case THERMAL_WARNING_MASK_SET_E: 
           value = old_value | THERMAL_WARNING_MASK_SET_PARAM_MASK; 
           break;
        case THERMAL_WARNING_MASK_RESET_SET_E: 
           value = old_value & ~ THERMAL_WARNING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_thermal_warning_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 3:3
 *
 **************************************************************************/
unsigned char get_thermal_warning_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Thermal warning masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & THERMAL_WARNING_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_falling_pon_key2_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 4:4
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
        case FALLING_PON_KEY_2_MASK_SET_E: 
           value = old_value | FALLING_PON_KEY_2_MASK_SET_PARAM_MASK; 
           break;
        case FALLING_PON_KEY_2_MASK_RESET_SET_E: 
           value = old_value & ~ FALLING_PON_KEY_2_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_falling_pon_key2_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 4:4
 *
 **************************************************************************/
unsigned char get_falling_pon_key2_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Falling edge on PonKey2 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & FALLING_PON_KEY_2_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rising_pon_key2_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 5:5
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
        case RISING_PON_KEY_2_MASK_SET_E: 
           value = old_value | RISING_PON_KEY_2_MASK_SET_PARAM_MASK; 
           break;
        case RISING_PON_KEY_2_MASK_RESET_SET_E: 
           value = old_value & ~ RISING_PON_KEY_2_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rising_pon_key2_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 5:5
 *
 **************************************************************************/
unsigned char get_rising_pon_key2_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Rising edge on PonKey2 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & RISING_PON_KEY_2_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_falling_pon_key1_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 6:6
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
        case FALLING_PON_KEY_1_MASK_SET_E: 
           value = old_value | FALLING_PON_KEY_1_MASK_SET_PARAM_MASK; 
           break;
        case FALLING_PON_KEY_1_MASK_RESET_SET_E: 
           value = old_value & ~ FALLING_PON_KEY_1_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_falling_pon_key1_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 6:6
 *
 **************************************************************************/
unsigned char get_falling_pon_key1_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Falling edge on PonKey1 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & FALLING_PON_KEY_1_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rising_pon_key1_masked_interrupt
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt1
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 7:7
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
        case RISING_PON_KEY_1_MASK_SET_E: 
           value = old_value | RISING_PON_KEY_1_MASK_SET_PARAM_MASK; 
           break;
        case RISING_PON_KEY_1_MASK_RESET_SET_E: 
           value = old_value & ~ RISING_PON_KEY_1_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rising_pon_key1_masked_interrupt
 *
 * RET  : Return the value of register MaskedInterrupt1
 *
 * Notes : From register 0x0E40, bits 7:7
 *
 **************************************************************************/
unsigned char get_rising_pon_key1_masked_interrupt()
  {
    unsigned char value;


    /* 
     * Rising edge on PonKey1 ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_1_REG) & RISING_PON_KEY_1_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt2
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt2( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * IntSource2 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_2_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt2
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt2()
  {
    unsigned char value;


    /* 
     * IntSource2 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_over_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 0:0
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
        case BATTERY_OVER_VOLTAGE_MASK_SET_E: 
           value = old_value | BATTERY_OVER_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_OVER_VOLTAGE_MASK_RESET_SET_E: 
           value = old_value & ~ BATTERY_OVER_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_over_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_over_voltage_mask()
  {
    unsigned char value;


    /* 
     * TVbatA ball goes upper over voltage threshold masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & BATTERY_OVER_VOLTAGE_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_un_plug_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 2:2
 *
 **************************************************************************/
unsigned char set_main_charger_un_plug_mask( enum main_charger_un_plug_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * Main charger unplug masked 
     */ 
    switch( param ){
        case MAIN_CHARGER_UN_PLUG_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_UN_PLUG_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_UN_PLUG_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_UN_PLUG_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_un_plug_mask
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 2:2
 *
 **************************************************************************/
unsigned char get_main_charger_un_plug_mask()
  {
    unsigned char value;


    /* 
     * Main charger unplug masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & MAIN_CHARGER_UN_PLUG_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_plug_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 3:3
 *
 **************************************************************************/
unsigned char set_main_charger_plug_mask( enum main_charger_plug_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_2_REG);

    /* 
     * Main charger plug masked 
     */ 
    switch( param ){
        case MAIN_CHARGER_PLUG_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_PLUG_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_PLUG_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_PLUG_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_plug_mask
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_charger_plug_mask()
  {
    unsigned char value;


    /* 
     * Main charger plug masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & MAIN_CHARGER_PLUG_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_falling_edge_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 6:6
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
        case VBUS_FALLING_EDGE_MASK_SET_E: 
           value = old_value | VBUS_FALLING_EDGE_MASK_SET_PARAM_MASK; 
           break;
        case VBUS_FALLING_EDGE_MASK_RESET_SET_E: 
           value = old_value & ~ VBUS_FALLING_EDGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_falling_edge_mask
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 6:6
 *
 **************************************************************************/
unsigned char get_vbus_falling_edge_mask()
  {
    unsigned char value;


    /* 
     * Falling edge on Vbus ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & VBUS_FALLING_EDGE_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_rising_edge_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt2
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 7:7
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
        case VBUS_RISING_EDGE_MASK_SET_E: 
           value = old_value | VBUS_RISING_EDGE_MASK_SET_PARAM_MASK; 
           break;
        case VBUS_RISING_EDGE_MASK_RESET_SET_E: 
           value = old_value & ~ VBUS_RISING_EDGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_rising_edge_mask
 *
 * RET  : Return the value of register MaskedInterrupt2
 *
 * Notes : From register 0x0E41, bits 7:7
 *
 **************************************************************************/
unsigned char get_vbus_rising_edge_mask()
  {
    unsigned char value;


    /* 
     * Rising edge on Vbus ball masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_2_REG) & VBUS_RISING_EDGE_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt3
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt3( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * IntSource3 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_3_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt3
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt3()
  {
    unsigned char value;


    /* 
     * IntSource3 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_3_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_dropout_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_charger_dropout_mask( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Usb charger was in dropout and internal input current 
     * loop update current regulation to avoid usb charger to 
     * drop 
     */ 
    value =  old_value & ~USB_DROP_OUT_MASK_SET_PARAM_MASK;


     value |= param;  
    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_dropout_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_charger_dropout_mask()
  {
    unsigned char value;


    /* 
     * Usb charger was in dropout and internal input current 
     * loop update current regulation to avoid usb charger to 
     * drop 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & USB_DROP_OUT_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc60s_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 1:1
 *
 **************************************************************************/
unsigned char set_rtc60s_mask( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * RTC timer reaches a 60s period 
     */ 
    value =  old_value & ~PERIOD_6_0S_MASK_SET_PARAM_MASK;


     param |= (value << 0x1);  
    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc60s_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 1:1
 *
 **************************************************************************/
unsigned char get_rtc60s_mask()
  {
    unsigned char value;


    /* 
     * RTC timer reaches a 60s period 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & PERIOD_6_0S_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_alarm_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 2:2
 *
 **************************************************************************/
unsigned char set_rtc_alarm_mask( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * RTC timer reaches alarm time 
     */ 
    value =  old_value & ~REACH_ALARM_TIME_MASK_SET_PARAM_MASK;


     param |= (value << 0x2);  
    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 2:2
 *
 **************************************************************************/
unsigned char get_rtc_alarm_mask()
  {
    unsigned char value;


    /* 
     * RTC timer reaches alarm time 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & REACH_ALARM_TIME_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_removal_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 4:4
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
        case BATTERY_REMOVAL_MASK_SET_E: 
           value = old_value | BATTERY_REMOVAL_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_REMOVAL_MASK_RESET_SET_E: 
           value = old_value & ~ BATTERY_REMOVAL_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_removal_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 4:4
 *
 **************************************************************************/
unsigned char get_battery_removal_mask()
  {
    unsigned char value;


    /* 
     * Battery removal 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & BATTERY_REMOVAL_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_watchdog_charger_expiration_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 5:5
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
        case WATCHDOG_CHARGER_EXPIRATION_MASK_SET_E: 
           value = old_value | WATCHDOG_CHARGER_EXPIRATION_MASK_SET_PARAM_MASK; 
           break;
        case WATCHDOG_CHARGER_EXPIRATION_MASK_RESET_SET_E: 
           value = old_value & ~ WATCHDOG_CHARGER_EXPIRATION_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_watchdog_charger_expiration_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 5:5
 *
 **************************************************************************/
unsigned char get_watchdog_charger_expiration_mask()
  {
    unsigned char value;


    /* 
     * Watchdog charger expiration detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & WATCHDOG_CHARGER_EXPIRATION_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_overvoltage_on_vbus_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 6:6
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
        case OVERVOLTAGE_ON_VBUS_L_MASK_SET_E: 
           value = old_value | OVERVOLTAGE_ON_VBUS_L_MASK_SET_PARAM_MASK; 
           break;
        case OVERVOLTAGE_ON_VBUS_L_MASK_RESET_SET_E: 
           value = old_value & ~ OVERVOLTAGE_ON_VBUS_L_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_overvoltage_on_vbus_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 6:6
 *
 **************************************************************************/
unsigned char get_overvoltage_on_vbus_mask()
  {
    unsigned char value;


    /* 
     * Overvoltage on Vbus ball detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & OVERVOLTAGE_ON_VBUS_L_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_over_current_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt3
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 7:7
 *
 **************************************************************************/
unsigned char set_main_charger_over_current_mask( enum main_charger_over_current_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_3_REG);

    /* 
     * Icharge > Icharge max programmed detected 
     */ 
    switch( param ){
        case MAIN_CHARGER_OVER_CURRENT_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_OVER_CURRENT_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_OVER_CURRENT_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_OVER_CURRENT_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_over_current_mask
 *
 * RET  : Return the value of register MaskedInterrupt3
 *
 * Notes : From register 0x0E42, bits 7:7
 *
 **************************************************************************/
unsigned char get_main_charger_over_current_mask()
  {
    unsigned char value;


    /* 
     * Icharge > Icharge max programmed detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_3_REG) & MAIN_CHARGER_OVER_CURRENT_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt4
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt4( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_4_REG);

    /* 
     * IntSource4 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_4_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt4
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt4()
  {
    unsigned char value;


    /* 
     * IntSource4 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_4_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accumulation_conversion_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 0:0
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
        case ACCUMULATION_CONVERSION_MASK_SET_E: 
           value = old_value | ACCUMULATION_CONVERSION_MASK_SET_PARAM_MASK; 
           break;
        case ACCUMULATION_CONVERSION_MASK_RESET_SET_E: 
           value = old_value & ~ ACCUMULATION_CONVERSION_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accumulation_conversion_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 0:0
 *
 **************************************************************************/
unsigned char get_accumulation_conversion_mask()
  {
    unsigned char value;


    /* 
     * Accumulation of N sample conversion is detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & ACCUMULATION_CONVERSION_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 1:1
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
        case AUDIO_MASK_SET_E: 
           value = old_value | AUDIO_MASK_SET_PARAM_MASK; 
           break;
        case AUDIO_MASK_RESET_SET_E: 
           value = old_value & ~ AUDIO_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_mask()
  {
    unsigned char value;


    /* 
     * Audio interrupt form audio digital part detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & AUDIO_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_ended_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 2:2
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
        case COULOMB_COUNTER_ENDED_MASK_SET_E: 
           value = old_value | COULOMB_COUNTER_ENDED_MASK_SET_PARAM_MASK; 
           break;
        case COULOMB_COUNTER_ENDED_MASK_RESET_SET_E: 
           value = old_value & ~ COULOMB_COUNTER_ENDED_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_ended_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 2:2
 *
 **************************************************************************/
unsigned char get_coulomb_ended_mask()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended data convertion 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & COULOMB_COUNTER_ENDED_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_coulomb_calibration_endedd_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 3:3
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
        case COULOMB_CALIBRATION_ENDEDD_MASK_SET_E: 
           value = old_value | COULOMB_CALIBRATION_ENDEDD_MASK_SET_PARAM_MASK; 
           break;
        case COULOMB_CALIBRATION_ENDEDD_MASK_RESET_SET_E: 
           value = old_value & ~ COULOMB_CALIBRATION_ENDEDD_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_calibration_endedd_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 3:3
 *
 **************************************************************************/
unsigned char get_coulomb_calibration_endedd_mask()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended its calibration 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & COULOMB_CALIBRATION_ENDEDD_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_battery_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 4:4
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
        case LOW_BATTERY_MASK_SET_E: 
           value = old_value | LOW_BATTERY_MASK_SET_PARAM_MASK; 
           break;
        case LOW_BATTERY_MASK_RESET_SET_E: 
           value = old_value & ~ LOW_BATTERY_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_battery_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 4:4
 *
 **************************************************************************/
unsigned char get_low_battery_mask()
  {
    unsigned char value;


    /* 
     * VbatA voltage goes below LowBat register programmed 
     * threshold 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & LOW_BATTERY_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_battery_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 5:5
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
        case HIGH_E: 
           value = old_value | HIGH_BATTERY_MASK_SET_PARAM_MASK; 
           break;
        case LOW_E: 
           value = old_value & ~ HIGH_BATTERY_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_battery_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 5:5
 *
 **************************************************************************/
unsigned char get_high_battery_mask()
  {
    unsigned char value;


    /* 
     * VbatA voltage goes above LowBat register programmed 
     * threshold 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & HIGH_BATTERY_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_battery_low_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 6:6
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
        case RTC_BATTERY_LOW_MASK_SET_E: 
           value = old_value | RTC_BATTERY_LOW_MASK_SET_PARAM_MASK; 
           break;
        case RTC_BATTERY_LOW_MASK_RESET_SET_E: 
           value = old_value & ~ RTC_BATTERY_LOW_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_battery_low_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 6:6
 *
 **************************************************************************/
unsigned char get_rtc_battery_low_mask()
  {
    unsigned char value;


    /* 
     * BackUpBat ball voltage goes below RtcBackupChg register 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & RTC_BATTERY_LOW_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_rtc_battery_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt4
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 7:7
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
        case RTC_BATTERY_HIGH_E: 
           value = old_value | RTC_BATTERY_HIGH_MASK_SET_PARAM_MASK; 
           break;
        case RTC_BATTERY_LOW_E: 
           value = old_value & ~ RTC_BATTERY_HIGH_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_rtc_battery_high_mask
 *
 * RET  : Return the value of register MaskedInterrupt4
 *
 * Notes : From register 0x0E43, bits 7:7
 *
 **************************************************************************/
unsigned char get_rtc_battery_high_mask()
  {
    unsigned char value;


    /* 
     * BackUpBat ball voltage goes above RtcBackupChg register 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_4_REG) & RTC_BATTERY_HIGH_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt5
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt5( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_5_REG);

    /* 
     * IntSource5 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_5_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt5
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt5()
  {
    unsigned char value;


    /* 
     * IntSource5 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_5_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_software_control_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 0:0
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
        case ADC_SOFTWARE_CONTROL_MASK_SET_E: 
           value = old_value | ADC_SOFTWARE_CONTROL_MASK_SET_PARAM_MASK; 
           break;
        case ADC_SOFTWARE_CONTROL_MASK_RESET_SET_E: 
           value = old_value & ~ ADC_SOFTWARE_CONTROL_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_software_control_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc_software_control_mask()
  {
    unsigned char value;


    /* 
     * GP ADC conversion requested thru software control is 
     * ended (data are available) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ADC_SOFTWARE_CONTROL_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory1_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 1:1
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
        case ACCESSORY_1_LOW_VOLTAGE_MASK_SET_E: 
           value = old_value | ACCESSORY_1_LOW_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_1_LOW_VOLTAGE_MASK_RESET_SET_E: 
           value = old_value & ~ ACCESSORY_1_LOW_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory1_low_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 1:1
 *
 **************************************************************************/
unsigned char get_accessory1_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect1 ball voltage level goes below AccDetect1Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_1_LOW_VOLTAGE_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory1_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 2:2
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
        case ACCESSORY_1_HIGH_E: 
           value = old_value | ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_1_LOW_E: 
           value = old_value & ~ ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory1_high_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 2:2
 *
 **************************************************************************/
unsigned char get_accessory1_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect1 ball voltage level goes above AccDetect1Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory22_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 3:3
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
        case ACCESSORY_22_LOW_VOLTAGE_MASK_SET_E: 
           value = old_value | ACCESSORY_22_LOW_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_22_LOW_VOLTAGE_MASK_RESET_SET_E: 
           value = old_value & ~ ACCESSORY_22_LOW_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory22_low_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 3:3
 *
 **************************************************************************/
unsigned char get_accessory22_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes below AccDetect22Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_22_LOW_VOLTAGE_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory22_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 4:4
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
        case ACCESSORY_22_HIGH_E: 
           value = old_value | ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_22_LOW_E: 
           value = old_value & ~ ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory22_high_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 4:4
 *
 **************************************************************************/
unsigned char get_accessory22_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes above AccDetect22Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory21_low_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 5:5
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
        case ACCESSORY_21_LOW_VOLTAGE_MASK_SET_E: 
           value = old_value | ACCESSORY_21_LOW_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_21_LOW_VOLTAGE_MASK_RESET_SET_E: 
           value = old_value & ~ ACCESSORY_21_LOW_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory21_low_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 5:5
 *
 **************************************************************************/
unsigned char get_accessory21_low_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes below AccDetect21Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_21_LOW_VOLTAGE_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_accessory21_high_voltage_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 6:6
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
        case ACCESSORY_21_HIGH_E: 
           value = old_value | ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_PARAM_MASK; 
           break;
        case ACCESSORY_21_LOW_E: 
           value = old_value & ~ ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_accessory21_high_voltage_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 6:6
 *
 **************************************************************************/
unsigned char get_accessory21_high_voltage_mask()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level goes above AccDetect21Th 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_hardware_control_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt5
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 7:7
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
        case ADC_HARDWARE_CONTROL_MASK_SET_E: 
           value = old_value | ADC_HARDWARE_CONTROL_MASK_SET_PARAM_MASK; 
           break;
        case ADC_HARDWARE_CONTROL_MASK_RESET_SET_E: 
           value = old_value & ~ ADC_HARDWARE_CONTROL_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_hardware_control_mask
 *
 * RET  : Return the value of register MaskedInterrupt5
 *
 * Notes : From register 0x0E44, bits 7:7
 *
 **************************************************************************/
unsigned char get_adc_hardware_control_mask()
  {
    unsigned char value;


    /* 
     * GP ADC conversion requested thru Hardware control 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_5_REG) & ADC_HARDWARE_CONTROL_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt7
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt7( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_7_REG);

    /* 
     * IntSource7 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_7_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt7
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt7()
  {
    unsigned char value;


    /* 
     * IntSource7 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_7_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 7:7
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
        case GPIO_13_RISING_MASK_SET_E: 
           value = old_value | GPIO_13_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_13_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_13_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio13_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO13 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_13_RISING_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 6:6
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
        case GPIO_12_RISING_MASK_SET_E: 
           value = old_value | GPIO_12_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_12_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_12_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio12_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO12 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_12_RISING_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 5:5
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
        case GPIO_11_RISING_MASK_SET_E: 
           value = old_value | GPIO_11_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_11_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_11_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio11_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO11 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_11_RISING_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 4:4
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
        case GPIO_10_RISING_MASK_SET_E: 
           value = old_value | GPIO_10_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_10_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_10_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio10_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO10 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_10_RISING_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 3:3
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
        case GPIO_9_RISING_MASK_SET_E: 
           value = old_value | GPIO_9_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_9_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_9_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio9_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO9 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_9_RISING_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 2:2
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
        case GPIO_8_RISING_MASK_SET_E: 
           value = old_value | GPIO_8_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_8_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_8_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio8_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO8 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_8_RISING_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 1:1
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
        case GPIO_7_RISING_MASK_SET_E: 
           value = old_value | GPIO_7_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_7_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_7_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio7_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO7 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_7_RISING_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt7
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 0:0
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
        case GPIO_6_RISING_MASK_SET_E: 
           value = old_value | GPIO_6_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_6_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_6_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt7
 *
 * Notes : From register 0x0E46, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio6_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO6 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_7_REG) & GPIO_6_RISING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt8
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt8( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_8_REG);

    /* 
     * IntSource8 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_8_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt8
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt8()
  {
    unsigned char value;


    /* 
     * IntSource8 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_8_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 7:7
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
        case GPIO_41_RISING_MASK_SET_E: 
           value = old_value | GPIO_41_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_41_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_41_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio41_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO41 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_41_RISING_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 6:6
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
        case GPIO_40_RISING_MASK_SET_E: 
           value = old_value | GPIO_40_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_40_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_40_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio40_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO40 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_40_RISING_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 5:5
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
        case GPIO_39_RISING_MASK_SET_E: 
           value = old_value | GPIO_39_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_39_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_39_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio39_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO39 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_39_RISING_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 4:4
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
        case GPIO_38_RISING_MASK_SET_E: 
           value = old_value | GPIO_38_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_38_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_38_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio38_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO38 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_38_RISING_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 3:3
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
        case GPIO_37_RISING_MASK_SET_E: 
           value = old_value | GPIO_37_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_37_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_37_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio37_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO37 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_37_RISING_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 2:2
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
        case GPIO_36_RISING_MASK_SET_E: 
           value = old_value | GPIO_36_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_36_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_36_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio36_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO36 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_36_RISING_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 1:1
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
        case GPIO_25_RISING_MASK_SET_E: 
           value = old_value | GPIO_25_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_25_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_25_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio25_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO25 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_25_RISING_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_rising_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt8
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 0:0
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
        case GPIO_24_RISING_MASK_SET_E: 
           value = old_value | GPIO_24_RISING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_24_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_24_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_rising_mask
 *
 * RET  : Return the value of register MaskedInterrupt8
 *
 * Notes : From register 0x0E47, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio24_rising_mask()
  {
    unsigned char value;


    /* 
     * Rising edge Interrupt masked on GPIO24 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_8_REG) & GPIO_24_RISING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt9
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt9( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * IntSource9 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_9_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt9
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt9()
  {
    unsigned char value;


    /* 
     * IntSource9 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_9_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio13_falling_mask( enum gpio13_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO13 ball 
     */ 
    switch( param ){
        case GPIO_13_FALLING_MASK_SET_E: 
           value = old_value | GPIO_13_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_13_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_13_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio13_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO13 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_13_FALLING_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio12_falling_mask( enum gpio12_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO12 ball 
     */ 
    switch( param ){
        case GPIO_12_FALLING_MASK_SET_E: 
           value = old_value | GPIO_12_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_12_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_12_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio12_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO12 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_12_FALLING_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio11_falling_mask( enum gpio11_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO11 ball 
     */ 
    switch( param ){
        case GPIO_11_FALLING_MASK_SET_E: 
           value = old_value | GPIO_11_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_11_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_11_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio11_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO11 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_11_FALLING_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio10_falling_mask( enum gpio10_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO10 ball 
     */ 
    switch( param ){
        case GPIO_10_FALLING_MASK_SET_E: 
           value = old_value | GPIO_10_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_10_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_10_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio10_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO10 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_10_FALLING_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio9_falling_mask( enum gpio9_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO9 ball 
     */ 
    switch( param ){
        case GPIO_9_FALLING_MASK_SET_E: 
           value = old_value | GPIO_9_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_9_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_9_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio9_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO9 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_9_FALLING_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio8_falling_mask( enum gpio8_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO8 ball 
     */ 
    switch( param ){
        case GPIO_8_FALLING_MASK_SET_E: 
           value = old_value | GPIO_8_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_8_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_8_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio8_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO8 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_8_FALLING_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio7_falling_mask( enum gpio7_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO7 ball 
     */ 
    switch( param ){
        case GPIO_7_FALLING_MASK_SET_E: 
           value = old_value | GPIO_7_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_7_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_7_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio7_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO7 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_7_FALLING_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt9
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio6_falling_mask( enum gpio6_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_9_REG);

    /* 
     * Falling edge detected on GPIO6 ball 
     */ 
    switch( param ){
        case GPIO_6_FALLING_MASK_SET_E: 
           value = old_value | GPIO_6_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_6_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_6_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt9
 *
 * Notes : From register 0x0E48, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio6_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO6 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_9_REG) & GPIO_6_FALLING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt10
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt10( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * IntSource10 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_10_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt10
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt10()
  {
    unsigned char value;


    /* 
     * IntSource10 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_10_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio41_falling_mask( enum gpio41_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO41 ball 
     */ 
    switch( param ){
        case GPIO_41_FALLING_MASK_SET_E: 
           value = old_value | GPIO_41_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_41_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_41_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio41_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO41 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_41_FALLING_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio40_falling_mask( enum gpio40_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO40 ball 
     */ 
    switch( param ){
        case GPIO_40_FALLING_MASK_SET_E: 
           value = old_value | GPIO_40_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_40_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_40_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio40_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO40 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_40_FALLING_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio39_falling_mask( enum gpio39_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO39 ball 
     */ 
    switch( param ){
        case GPIO_39_FALLING_MASK_SET_E: 
           value = old_value | GPIO_39_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_39_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_39_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio39_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO39 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_39_FALLING_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio38_falling_mask( enum gpio38_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO38 ball 
     */ 
    switch( param ){
        case GPIO_38_FALLING_MASK_SET_E: 
           value = old_value | GPIO_38_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_38_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_38_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio38_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO38 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_38_FALLING_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio37_falling_mask( enum gpio37_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO37 ball 
     */ 
    switch( param ){
        case GPIO_37_FALLING_MASK_SET_E: 
           value = old_value | GPIO_37_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_37_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_37_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio37_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO37 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_37_FALLING_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio36_falling_mask( enum gpio36_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO36 ball 
     */ 
    switch( param ){
        case GPIO_36_FALLING_MASK_SET_E: 
           value = old_value | GPIO_36_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_36_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_36_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio36_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO36 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_36_FALLING_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio25_falling_mask( enum gpio25_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO25 ball 
     */ 
    switch( param ){
        case GPIO_25_FALLING_MASK_SET_E: 
           value = old_value | GPIO_25_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_25_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_25_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio25_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO25 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_25_FALLING_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt10
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio24_falling_mask( enum gpio24_falling_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_10_REG);

    /* 
     * Falling edge detected on GPIO24 ball 
     */ 
    switch( param ){
        case GPIO_24_FALLING_MASK_SET_E: 
           value = old_value | GPIO_24_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case GPIO_24_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ GPIO_24_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt10
 *
 * Notes : From register 0x0E49, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio24_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge detected on GPIO24 ball 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_10_REG) & GPIO_24_FALLING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_masked_interrupt12
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt12()
  {
    unsigned char value;


    /* 
     * MaskedInterrupt12 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_12_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_link_status_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 7:7
 *
 **************************************************************************/
unsigned char get_usb_link_status_mask()
  {
    unsigned char value;


    /* 
     * 0: UsbLink status unchanged 
     * 1: UsbLink status changed 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & USB_LINK_STATUS_CHANGED_MASK_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_phy_error_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_phy_error_mask()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: USB PHY has been reset due to a power loss 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & USB_PHY_ERROR_LATCH_MASK_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adp_sense_off_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 4:4
 *
 **************************************************************************/
unsigned char get_adp_sense_off_mask()
  {
    unsigned char value;


    /* 
     * 0: Current source Off 
     * 1: Current source On 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & CURRENT_SOURCE_ON_MASK_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adp_probe_un_plug_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 3:3
 *
 **************************************************************************/
unsigned char get_adp_probe_un_plug_mask()
  {
    unsigned char value;


    /* 
     * 0: previous state 
     * 1: accessory unplug on Vbus 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & ADP_PROBE_UN_PLUG_MASK_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adp_probe_plug_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 2:2
 *
 **************************************************************************/
unsigned char get_adp_probe_plug_mask()
  {
    unsigned char value;


    /* 
     * 0: previous state 
     * 1: accessory plug on Vbus 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & ADP_PROBE_PLUG_MASK_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adp_sink_error_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 1:1
 *
 **************************************************************************/
unsigned char get_adp_sink_error_mask()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: no detection after 2046*32Khzclock cycles 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & ADP_SINK_ERROR_MASK_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adp_source_error_mask
 *
 * RET  : Return the value of register MaskedInterrupt12
 *
 * Notes : From register 0x0E4B, bits 0:0
 *
 **************************************************************************/
unsigned char get_adp_source_error_mask()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: no detection after inactive no detection after 
     * 2046*32Khzclock cycles 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_12_REG) & ADP_SOURCE_ERROR_MASK_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt19
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt19( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * IntSource19 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_19_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt19
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt19()
  {
    unsigned char value;


    /* 
     * IntSource19 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_19_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 3:3
 *
 **************************************************************************/
unsigned char set_battery_temperature_high_mask( enum battery_temperature_high_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'Btemp > BtempHigh' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_HIGH_E: 
           value = old_value | BATTERY_TEMPERATURE_HIGH_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_TEMPERATURE_LOW_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_HIGH_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_high_mask
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 3:3
 *
 **************************************************************************/
unsigned char get_battery_temperature_high_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'Btemp > BtempHigh' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_HIGH_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_medium_high_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 2:2
 *
 **************************************************************************/
unsigned char set_battery_temperature_medium_high_mask( enum battery_temperature_medium_high_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempHigh > Btemp > BtempMedium' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_MEDIUM_HIGH_E: 
           value = old_value | BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_TEMPERATURE_MEDIUM_LOW_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_medium_high_mask
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 2:2
 *
 **************************************************************************/
unsigned char get_battery_temperature_medium_high_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempHigh > Btemp > BtempMedium' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_low_medium_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 1:1
 *
 **************************************************************************/
unsigned char set_battery_temperature_low_medium_mask( enum battery_temperature_low_medium_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempMedium > Btemp > BtempLow' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_E: 
           value = old_value | BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_RESET_SET_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low_medium_mask
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 1:1
 *
 **************************************************************************/
unsigned char get_battery_temperature_low_medium_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: Interrupt 'BtempMedium > Btemp > BtempLow' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_battery_temperature_low_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt19
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 0:0
 *
 **************************************************************************/
unsigned char set_battery_temperature_low_mask( enum battery_temperature_low_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_19_REG);

    /* 
     * 0: Inactive 
     * 1:Interrupt 'Btemp <BtempLow' masked 
     */ 
    switch( param ){
        case BATTERY_TEMPERATURE_LOW_MASK_SET_E: 
           value = old_value | BATTERY_TEMPERATURE_LOW_MASK_SET_PARAM_MASK; 
           break;
        case BATTERY_TEMPERATURE_LOW_MASK_RESET_SET_E: 
           value = old_value & ~ BATTERY_TEMPERATURE_LOW_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low_mask
 *
 * RET  : Return the value of register MaskedInterrupt19
 *
 * Notes : From register 0x0E52, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_temperature_low_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1:Interrupt 'Btemp <BtempLow' masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_19_REG) & BATTERY_TEMPERATURE_LOW_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt20
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt20( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_20_REG);

    /* 
     * IntSource20 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_20_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt20
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt20()
  {
    unsigned char value;


    /* 
     * IntSource20 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_20_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_srp_dectect_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 0:0
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
        case DPLUS_RISING_MASK_SET_E: 
           value = old_value | DPLUS_RISING_MASK_SET_PARAM_MASK; 
           break;
        case DPLUS_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ DPLUS_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_srp_dectect_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 0:0
 *
 **************************************************************************/
unsigned char get_srp_dectect_mask()
  {
    unsigned char value;


    /* 
     * 0: Inactive 
     * 1: SRP detect Interrupt masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & DPLUS_RISING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_plug_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 1:1
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
        case USB_CHARGER_PLUG_MASK_SET_E: 
           value = old_value | USB_CHARGER_PLUG_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGER_PLUG_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGER_PLUG_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_plug_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 1:1
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
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & USB_CHARGER_PLUG_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wake_up_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 2:2
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
        case ID_WAKE_UP_RISING_MASK_SET_E: 
           value = old_value | ID_WAKE_UP_RISING_MASK_SET_PARAM_MASK; 
           break;
        case ID_WAKE_UP_RISING_MASK_RESET_SET_E: 
           value = old_value & ~ ID_WAKE_UP_RISING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wake_up_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_wake_up_mask()
  {
    unsigned char value;


    /* 
     * Rising edge detected on WakeUp 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_WAKE_UP_RISING_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater37k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 4:4
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
        case ID_DETECTION_GREATER_3_7K_MASK_SET_E: 
           value = old_value | ID_DETECTION_GREATER_3_7K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_GREATER_3_7K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_GREATER_3_7K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater37k_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_detection_greater37k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 37k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_3_7K_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater69k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 5:5
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
        case ID_DETECTION_GREATER_6_9K_MASK_SET_E: 
           value = old_value | ID_DETECTION_GREATER_6_9K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_GREATER_6_9K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_GREATER_6_9K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater69k_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 5:5
 *
 **************************************************************************/
unsigned char get_id_detection_greater69k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 69k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_6_9K_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater150k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 6:6
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
        case ID_DETECTION_GREATER_12_5K_MASK_SET_E: 
           value = old_value | ID_DETECTION_GREATER_12_5K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_GREATER_12_5K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_GREATER_12_5K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater150k_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 6:6
 *
 **************************************************************************/
unsigned char get_id_detection_greater150k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 125k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_12_5K_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection_greater220k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt20
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 7:7
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
        case ID_DETECTION_GREATER_22_0K_MASK_SET_E: 
           value = old_value | ID_DETECTION_GREATER_22_0K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_GREATER_22_0K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_GREATER_22_0K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_greater220k_mask
 *
 * RET  : Return the value of register MaskedInterrupt20
 *
 * Notes : From register 0x0E53, bits 7:7
 *
 **************************************************************************/
unsigned char get_id_detection_greater220k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 220k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_20_REG) & ID_DETECTION_GREATER_22_0K_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt21
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt21( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_21_REG);

    /* 
     * IntSource21 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_21_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt21
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt21()
  {
    unsigned char value;


    /* 
     * IntSource21 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_21_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wake_up_falling_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 0:0
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
        case ID_WAKE_UP_FALLING_MASK_SET_E: 
           value = old_value | ID_WAKE_UP_FALLING_MASK_SET_PARAM_MASK; 
           break;
        case ID_WAKE_UP_FALLING_MASK_RESET_SET_E: 
           value = old_value & ~ ID_WAKE_UP_FALLING_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wake_up_falling_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 0:0
 *
 **************************************************************************/
unsigned char get_id_wake_up_falling_mask()
  {
    unsigned char value;


    /* 
     * Falling edge on ID Interrupt masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_WAKE_UP_FALLING_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection37k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 2:2
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
        case ID_DETECTION_3_7K_MASK_SET_E: 
           value = old_value | ID_DETECTION_3_7K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_3_7K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_3_7K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection37k_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_detection37k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 37k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_3_7K_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection69k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 3:3
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
        case ID_DETECTION_6_9K_MASK_SET_E: 
           value = old_value | ID_DETECTION_6_9K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_6_9K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_6_9K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection69k_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 3:3
 *
 **************************************************************************/
unsigned char get_id_detection69k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 69k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_6_9K_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection150k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 4:4
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
        case ID_DETECTION_12_5K_MASK_SET_E: 
           value = old_value | ID_DETECTION_12_5K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_12_5K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_12_5K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection150k_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_detection150k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 125k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_12_5K_MASK_SET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_detection220k_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 5:5
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
        case ID_DETECTION_22_0K_MASK_SET_E: 
           value = old_value | ID_DETECTION_22_0K_MASK_SET_PARAM_MASK; 
           break;
        case ID_DETECTION_22_0K_MASK_RESET_SET_E: 
           value = old_value & ~ ID_DETECTION_22_0K_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_detection220k_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 5:5
 *
 **************************************************************************/
unsigned char get_id_detection220k_mask()
  {
    unsigned char value;


    /* 
     * ID resistance lower than 220k detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & ID_DETECTION_22_0K_MASK_SET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt21
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 6:6
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
        case USB_CHARGER_MASK_SET_E: 
           value = old_value | USB_CHARGER_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGER_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGER_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_mask
 *
 * RET  : Return the value of register MaskedInterrupt21
 *
 * Notes : From register 0x0E54, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_charger_mask()
  {
    unsigned char value;


    /* 
     * USB charger detected 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_21_REG) & USB_CHARGER_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_masked_interrupt22
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 7:0
 *
 **************************************************************************/
unsigned char set_masked_interrupt22( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * IntSource22 Status 
     */ 
    I2CWrite(MASKED_INTERRUPT_22_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_masked_interrupt22
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 7:0
 *
 **************************************************************************/
unsigned char get_masked_interrupt22()
  {
    unsigned char value;


    /* 
     * IntSource22 Status 
     */ 
   value = I2CRead(MASKED_INTERRUPT_22_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_unplug
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 7:7
 *
 **************************************************************************/
unsigned char set_usb_charger_unplug( enum usb_chargerunplug_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * USB charger compliant with charging specification 
     * interrupt masked 
     */ 
    switch( param ){
        case USB_CHARGERUNPLUG_MASK_SET_E: 
           value = old_value | USB_CHARGERUNPLUG_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGERUNPLUG_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGERUNPLUG_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_unplug
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 7:7
 *
 **************************************************************************/
unsigned char get_usb_charger_unplug()
  {
    unsigned char value;


    /* 
     * USB charger compliant with charging specification 
     * interrupt masked 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGERUNPLUG_MASK_SET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_compiliant_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 6:6
 *
 **************************************************************************/
unsigned char set_usb_charger_compiliant_mask( enum usb_charger_compiliant_mask_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(MASKED_INTERRUPT_22_REG);

    /* 
     * USB charger is compliant with charger 
     * specifications(charging restart) 
     */ 
    switch( param ){
        case USB_CHARGER_COMPILIANT_MASK_SET_E: 
           value = old_value | USB_CHARGER_COMPILIANT_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGER_COMPILIANT_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGER_COMPILIANT_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_compiliant_mask
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_charger_compiliant_mask()
  {
    unsigned char value;


    /* 
     * USB charger is compliant with charger 
     * specifications(charging restart) 
     */ 
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGER_COMPILIANT_MASK_SET_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_thermal_above_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 3:3
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
        case MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_THERMAL_ABOVE_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_thermal_above_mask
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 3:3
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
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_main_charger_thermal_below_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 2:2
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
        case MAIN_CHARGER_THERMAL_BELOW_MASK_SET_E: 
           value = old_value | MAIN_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK; 
           break;
        case MAIN_CHARGER_THERMAL_BELOW_MASK_RESET_SET_E: 
           value = old_value & ~ MAIN_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_thermal_below_mask
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 2:2
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
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & MAIN_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_thermal_above_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 1:1
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
        case USB_CHARGER_THERMAL_ABOVE_MASK_SET_E: 
           value = old_value | USB_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGER_THERMAL_ABOVE_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_thermal_above_mask
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 1:1
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
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_thermal_below_mask
 *
 * IN   : param, a value to write to the regiter MaskedInterrupt22
 * OUT  : 
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 0:0
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
        case USB_CHARGER_THERMAL_BELOW_MASK_SET_E: 
           value = old_value | USB_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK; 
           break;
        case USB_CHARGER_THERMAL_BELOW_MASK_RESET_SET_E: 
           value = old_value & ~ USB_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK;
           break;
    }
  

    I2CWrite(MASKED_INTERRUPT_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_thermal_below_mask
 *
 * RET  : Return the value of register MaskedInterrupt22
 *
 * Notes : From register 0x0E55, bits 0:0
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
    value = (I2CRead(MASKED_INTERRUPT_22_REG) & USB_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_it_latch_hier1
 *
 * IN   : param, a value to write to the regiter ITLatchHier1
 * OUT  : 
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 7:0
 *
 **************************************************************************/
unsigned char set_it_latch_hier1( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(IT_LATCH_HIER_1_REG);

    /* 
     * ITLatchHier1 
     */ 
    I2CWrite(IT_LATCH_HIER_1_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_it_latch_hier1
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 7:0
 *
 **************************************************************************/
unsigned char get_it_latch_hier1()
  {
    unsigned char value;


    /* 
     * ITLatchHier1 
     */ 
   value = I2CRead(IT_LATCH_HIER_1_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch8
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 7:7
 *
 **************************************************************************/
unsigned char get_it_from_latch8()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch8 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_8_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch7
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 5:5
 *
 **************************************************************************/
unsigned char get_it_from_latch7()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch7 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_7_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch5
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 4:4
 *
 **************************************************************************/
unsigned char get_it_from_latch5()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch5 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_5_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch4
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 3:3
 *
 **************************************************************************/
unsigned char get_it_from_latch4()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch4 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_4_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch3
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 2:2
 *
 **************************************************************************/
unsigned char get_it_from_latch3()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch3 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_3_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch2
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 1:1
 *
 **************************************************************************/
unsigned char get_it_from_latch2()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch2 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_2_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch1
 *
 * RET  : Return the value of register ITLatchHier1
 *
 * Notes : From register 0x0E60, bits 0:0
 *
 **************************************************************************/
unsigned char get_it_from_latch1()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch1 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_1_REG) & IT_FROM_LATCH_1_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_it_latch_hier2
 *
 * IN   : param, a value to write to the regiter ITLatchHier2
 * OUT  : 
 *
 * RET  : Return the value of register ITLatchHier2
 *
 * Notes : From register 0x0E61, bits 7:0
 *
 **************************************************************************/
unsigned char set_it_latch_hier2( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(IT_LATCH_HIER_2_REG);

    /* 
     * ITLatchHier2 
     */ 
    I2CWrite(IT_LATCH_HIER_2_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_it_latch_hier2
 *
 * RET  : Return the value of register ITLatchHier2
 *
 * Notes : From register 0x0E61, bits 7:0
 *
 **************************************************************************/
unsigned char get_it_latch_hier2()
  {
    unsigned char value;


    /* 
     * ITLatchHier2 
     */ 
   value = I2CRead(IT_LATCH_HIER_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch12
 *
 * RET  : Return the value of register ITLatchHier2
 *
 * Notes : From register 0x0E61, bits 3:3
 *
 **************************************************************************/
unsigned char get_it_from_latch12()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch12 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_2_REG) & IT_FROM_LATCH_12_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch10
 *
 * RET  : Return the value of register ITLatchHier2
 *
 * Notes : From register 0x0E61, bits 1:1
 *
 **************************************************************************/
unsigned char get_it_from_latch10()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch10 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_2_REG) & IT_FROM_LATCH_10_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch9
 *
 * RET  : Return the value of register ITLatchHier2
 *
 * Notes : From register 0x0E61, bits 0:0
 *
 **************************************************************************/
unsigned char get_it_from_latch9()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch9 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_2_REG) & IT_FROM_LATCH_9_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_latch_hier3
 *
 * RET  : Return the value of register ITLatchHier3
 *
 * Notes : From register 0x0E62, bits 7:0
 *
 **************************************************************************/
unsigned char get_it_latch_hier3()
  {
    unsigned char value;


    /* 
     * ITLatchHier3 
     */ 
   value = I2CRead(IT_LATCH_HIER_3_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch22
 *
 * RET  : Return the value of register ITLatchHier3
 *
 * Notes : From register 0x0E62, bits 5:5
 *
 **************************************************************************/
unsigned char get_it_from_latch22()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch22 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_3_REG) & IT_FROM_LATCH_22_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch21
 *
 * RET  : Return the value of register ITLatchHier3
 *
 * Notes : From register 0x0E62, bits 4:4
 *
 **************************************************************************/
unsigned char get_it_from_latch21()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch21 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_3_REG) & IT_FROM_LATCH_21_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch20
 *
 * RET  : Return the value of register ITLatchHier3
 *
 * Notes : From register 0x0E62, bits 3:3
 *
 **************************************************************************/
unsigned char get_it_from_latch20()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch20 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_3_REG) & IT_FROM_LATCH_20_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_it_from_latch19
 *
 * RET  : Return the value of register ITLatchHier3
 *
 * Notes : From register 0x0E62, bits 2:2
 *
 **************************************************************************/
unsigned char get_it_from_latch19()
  {
    unsigned char value;


    /* 
     * At least one interrupt appends in ITLatch19 register 
     */ 
    value = (I2CRead(IT_LATCH_HIER_3_REG) & IT_FROM_LATCH_19_PARAM_MASK) >> 2;
    return value;
  }


