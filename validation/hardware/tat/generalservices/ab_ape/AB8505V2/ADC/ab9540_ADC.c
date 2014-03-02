/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_ADC/Linux/ab9540_ADC.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_ADC_LINUX_AB9540_ADC_C
#define C__LISP_PRODUCTION_AB9540_AB9540_ADC_LINUX_AB9540_ADC_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_ADC.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_adc
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 0:0
 *
 **************************************************************************/
unsigned char set_adc( enum adc_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: disable ADC 
     * 1: enable ADC 
     */ 
    switch( param ){
        case ADC_ENABLE_E: 
           value = old_value | ADC_MASK; 
           break;
        case ADC_DISABLE_E: 
           value = old_value & ~ ADC_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc()
  {
    unsigned char value;


    /* 
     * 0: disable ADC 
     * 1: enable ADC 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_trigger
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 1:1
 *
 **************************************************************************/
unsigned char set_adc_trigger( enum adc_trigger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: disable GPADCTrig functionality 
     * 1: enable GPADCTrig functionality 
     */ 
    switch( param ){
        case ADC_TRIGGER_ENABLE_E: 
           value = old_value | ADC_TRIGGER_MASK; 
           break;
        case ADC_TRIGGER_DISABLE_E: 
           value = old_value & ~ ADC_TRIGGER_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_trigger
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 1:1
 *
 **************************************************************************/
unsigned char get_adc_trigger()
  {
    unsigned char value;


    /* 
     * 0: disable GPADCTrig functionality 
     * 1: enable GPADCTrig functionality 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_TRIGGER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_man_convert
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 2:2
 *
 **************************************************************************/
unsigned char set_adc_man_convert( enum adc_man_convert_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Launch an ADC conversion(this bit is auto-cleared, 
     * once the conversion is launched) 
     */ 
    switch( param ){
        case ADC_MAN_CONVERT_ENABLE_E: 
           value = old_value | ADC_MAN_CONVERT_MASK; 
           break;
        case ADC_MAN_CONVERT_DISABLE_E: 
           value = old_value & ~ ADC_MAN_CONVERT_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_man_convert
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 2:2
 *
 **************************************************************************/
unsigned char get_adc_man_convert()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Launch an ADC conversion(this bit is auto-cleared, 
     * once the conversion is launched) 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_MAN_CONVERT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_auto_req
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 3:3
 *
 **************************************************************************/
unsigned char set_adc_auto_req( enum adc_auto_req_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Auto mode ON 
     */ 
    switch( param ){
        case ADC_AUTO_REQ_ENABLE_E: 
           value = old_value | ADC_AUTO_REQ_MASK; 
           break;
        case ADC_AUTO_REQ_DISABLE_E: 
           value = old_value & ~ ADC_AUTO_REQ_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_auto_req
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 3:3
 *
 **************************************************************************/
unsigned char get_adc_auto_req()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Auto mode ON 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_AUTO_REQ_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_trig_edge
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 4:4
 *
 **************************************************************************/
unsigned char set_adc_trig_edge( enum adc_trig_edge_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: active 
     */ 
    switch( param ){
        case ADC_TRIG_EDGE_ENABLE_E: 
           value = old_value | ADC_TRIG_EDGE_MASK; 
           break;
        case ADC_TRIG_EDGE_DISABLE_E: 
           value = old_value & ~ ADC_TRIG_EDGE_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_trig_edge
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 4:4
 *
 **************************************************************************/
unsigned char get_adc_trig_edge()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: active 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_TRIG_EDGE_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pull_up_supply
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 5:5
 *
 **************************************************************************/
unsigned char set_pull_up_supply( enum pull_up_supply param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: Select Vtvout as Pull-Up supply 
     * 1: Select Vrtc as Pull-Up supply 
     */ 
    switch( param ){
        case V_RTC_SUPPLY_E: 
           value = old_value | PULL_UP_SUPPLY_MASK; 
           break;
        case V_TV_OUT_SUPPLY_E: 
           value = old_value & ~ PULL_UP_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pull_up_supply
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 5:5
 *
 **************************************************************************/
unsigned char get_pull_up_supply()
  {
    unsigned char value;


    /* 
     * 0: Select Vtvout as Pull-Up supply 
     * 1: Select Vrtc as Pull-Up supply 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & PULL_UP_SUPPLY_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_buffer
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 6:6
 *
 **************************************************************************/
unsigned char set_adc_buffer( enum adc_buffer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: disable channel buffer 
     * 1: enable channel buffer 
     */ 
    switch( param ){
        case ADC_BUFFER_ENABLE_E: 
           value = old_value | ADC_BUFFER_MASK; 
           break;
        case ADC_BUFFER_DISABLE_E: 
           value = old_value & ~ ADC_BUFFER_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_buffer
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 6:6
 *
 **************************************************************************/
unsigned char get_adc_buffer()
  {
    unsigned char value;


    /* 
     * 0: disable channel buffer 
     * 1: enable channel buffer 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & ADC_BUFFER_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charging_current_sense
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 7:7
 *
 **************************************************************************/
unsigned char set_charging_current_sense( enum charging_current_sense_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_CONTROL_REG);

    /* 
     * 0: disable charging current sense 
     * 1: enable charging current sense 
     */ 
    switch( param ){
        case CHARGING_CURRENT_SENSE_ENABLE_E: 
           value = old_value | CHARGING_CURRENT_SENSE_MASK; 
           break;
        case CHARGING_CURRENT_SENSE_DISABLE_E: 
           value = old_value & ~ CHARGING_CURRENT_SENSE_MASK;
           break;
    }
  

    I2CWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_sense
 *
 * RET  : Return the value of register 0xAdcControl
 *
 * Notes : From register 0xA00, bits 7:7
 *
 **************************************************************************/
unsigned char get_charging_current_sense()
  {
    unsigned char value;


    /* 
     * 0: disable charging current sense 
     * 1: enable charging current sense 
     */ 
    value = (I2CRead(ADC_CONTROL_REG) & CHARGING_CURRENT_SENSE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_select_input
 *
 * IN   : param, a value to write to the regiter AdcInputChannel
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcInputChannel
 *
 * Notes : From register 0xA01, bits 4:0
 *
 **************************************************************************/
unsigned char set_adc_select_input( enum adc_input_channel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_INPUT_CHANNEL_REG);

    /* 
     * Select ADC input channel 
     */ 
  

     value =  old_value & ~ADC_SELECT_INPUT_MASK ;

    switch(  param ){

           case NOT_USED_E:
                value =  value | NOT_USED ;
                break;
           case BAT_CTRL_E:
                value =  value | BAT_CTRL ;
                break;
           case BAT_TEMP_E:
                value =  value | BAT_TEMP ;
                break;
           case MAIN_CHARGER_VOLTAGE_E:
                value =  value | MAIN_CHARGER_VOLTAGE ;
                break;
           case ACC_DETECT1_E:
                value =  value | ACC_DETECT1 ;
                break;
           case ACC_DETECT2_E:
                value =  value | ACC_DETECT2 ;
                break;
           case ADC_AUX1_E:
                value =  value | ADC_AUX1 ;
                break;
           case ADC_AUX2_E:
                value =  value | ADC_AUX2 ;
                break;
           case VBATA_E:
                value =  value | VBATA ;
                break;
           case VBUS_E:
                value =  value | VBUS ;
                break;
           case MAIN_CHARGER_CURRENT_E:
                value =  value | MAIN_CHARGER_CURRENT ;
                break;
           case USB_CHARGER_CURRENT_E:
                value =  value | USB_CHARGER_CURRENT ;
                break;
           case BACKUP_BAT_E:
                value =  value | BACKUP_BAT ;
                break;
           case RESERVED_E:
                value =  value | RESERVED ;
                break;
           case ID_BALL_E:
                value =  value | ID_BALL ;
                break;
           case INTERNAL_TEST1_E:
                value =  value | INTERNAL_TEST1 ;
                break;
           case INTERNAL_TEST2_E:
                value =  value | INTERNAL_TEST2 ;
                break;
           case INTERNAL_TEST3_E:
                value =  value | INTERNAL_TEST3 ;
                break;
    }
  

    I2CWrite(ADC_INPUT_CHANNEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_select_input
 *
 * RET  : Return the value of register 0xAdcInputChannel
 *
 * Notes : From register 0xA01, bits 4:0
 *
 **************************************************************************/
unsigned char get_adc_select_input()
  {
    unsigned char value;


    /* 
     * Select ADC input channel 
     */ 
    value = (I2CRead(ADC_INPUT_CHANNEL_REG) & ADC_SELECT_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sample_to_average
 *
 * IN   : param, a value to write to the regiter AdcInputChannel
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcInputChannel
 *
 * Notes : From register 0xA01, bits 6:5
 *
 **************************************************************************/
unsigned char set_sample_to_average( enum sample_to_average param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_INPUT_CHANNEL_REG);

    /* 
     * Define number of ADC sample to average 
     */ 
  

     value =  old_value & ~SAMPLE_TO_AVERAGE_MASK ;

    switch(  param ){

           case AVARAGE_1_SAMPLE_E:
                value  = value  | (AVARAGE_1_SAMPLE << 0x5);
                break;
           case AVARAGE_4_SAMPLES_E:
                value  = value  | (AVARAGE_4_SAMPLES << 0x5);
                break;
           case AVARAGE_8_SAMPLES_E:
                value  = value  | (AVARAGE_8_SAMPLES << 0x5);
                break;
           case AVARAGE_16_SAMPLES_E:
                value  = value  | (AVARAGE_16_SAMPLES << 0x5);
                break;
    }
  

    I2CWrite(ADC_INPUT_CHANNEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sample_to_average
 *
 * RET  : Return the value of register 0xAdcInputChannel
 *
 * Notes : From register 0xA01, bits 6:5
 *
 **************************************************************************/
unsigned char get_sample_to_average()
  {
    unsigned char value;


    /* 
     * Define number of ADC sample to average 
     */ 
    value = (I2CRead(ADC_INPUT_CHANNEL_REG) & SAMPLE_TO_AVERAGE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_select_input_tric
 *
 * IN   : param, a value to write to the regiter AdcInputChannelTrig
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcInputChannelTrig
 *
 * Notes : From register 0xA02, bits 4:0
 *
 **************************************************************************/
unsigned char set_adc_select_input_tric( enum adc_input_channel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_INPUT_CHANNEL_TRIG_REG);

    /* 
     * Select ADC input channel 
     */ 
  

     value =  old_value & ~ADC_SELECT_INPUT_TRIC_MASK ;

    switch(  param ){

           case NOT_USED_E:
                value =  value | NOT_USED ;
                break;
           case BAT_CTRL_E:
                value =  value | BAT_CTRL ;
                break;
           case BAT_TEMP_E:
                value =  value | BAT_TEMP ;
                break;
           case MAIN_CHARGER_VOLTAGE_E:
                value =  value | MAIN_CHARGER_VOLTAGE ;
                break;
           case ACC_DETECT1_E:
                value =  value | ACC_DETECT1 ;
                break;
           case ACC_DETECT2_E:
                value =  value | ACC_DETECT2 ;
                break;
           case ADC_AUX1_E:
                value =  value | ADC_AUX1 ;
                break;
           case ADC_AUX2_E:
                value =  value | ADC_AUX2 ;
                break;
           case VBATA_E:
                value =  value | VBATA ;
                break;
           case VBUS_E:
                value =  value | VBUS ;
                break;
           case MAIN_CHARGER_CURRENT_E:
                value =  value | MAIN_CHARGER_CURRENT ;
                break;
           case USB_CHARGER_CURRENT_E:
                value =  value | USB_CHARGER_CURRENT ;
                break;
           case BACKUP_BAT_E:
                value =  value | BACKUP_BAT ;
                break;
           case RESERVED_E:
                value =  value | RESERVED ;
                break;
           case ID_BALL_E:
                value =  value | ID_BALL ;
                break;
           case INTERNAL_TEST1_E:
                value =  value | INTERNAL_TEST1 ;
                break;
           case INTERNAL_TEST2_E:
                value =  value | INTERNAL_TEST2 ;
                break;
           case INTERNAL_TEST3_E:
                value =  value | INTERNAL_TEST3 ;
                break;
    }
  

    I2CWrite(ADC_INPUT_CHANNEL_TRIG_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_select_input_tric
 *
 * RET  : Return the value of register 0xAdcInputChannelTrig
 *
 * Notes : From register 0xA02, bits 4:0
 *
 **************************************************************************/
unsigned char get_adc_select_input_tric()
  {
    unsigned char value;


    /* 
     * Select ADC input channel 
     */ 
    value = (I2CRead(ADC_INPUT_CHANNEL_TRIG_REG) & ADC_SELECT_INPUT_TRIC_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sample_to_average_trig
 *
 * IN   : param, a value to write to the regiter AdcInputChannelTrig
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcInputChannelTrig
 *
 * Notes : From register 0xA02, bits 6:5
 *
 **************************************************************************/
unsigned char set_sample_to_average_trig( enum sample_to_average param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_INPUT_CHANNEL_TRIG_REG);

    /* 
     * Define number of ADC sample to average 
     */ 
  

     value =  old_value & ~SAMPLE_TO_AVERAGE_TRIG_MASK ;

    switch(  param ){

           case AVARAGE_1_SAMPLE_E:
                value  = value  | (AVARAGE_1_SAMPLE << 0x5);
                break;
           case AVARAGE_4_SAMPLES_E:
                value  = value  | (AVARAGE_4_SAMPLES << 0x5);
                break;
           case AVARAGE_8_SAMPLES_E:
                value  = value  | (AVARAGE_8_SAMPLES << 0x5);
                break;
           case AVARAGE_16_SAMPLES_E:
                value  = value  | (AVARAGE_16_SAMPLES << 0x5);
                break;
    }
  

    I2CWrite(ADC_INPUT_CHANNEL_TRIG_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sample_to_average_trig
 *
 * RET  : Return the value of register 0xAdcInputChannelTrig
 *
 * Notes : From register 0xA02, bits 6:5
 *
 **************************************************************************/
unsigned char get_sample_to_average_trig()
  {
    unsigned char value;


    /* 
     * Define number of ADC sample to average 
     */ 
    value = (I2CRead(ADC_INPUT_CHANNEL_TRIG_REG) & SAMPLE_TO_AVERAGE_TRIG_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_time_delay
 *
 * IN   : param, a value to write to the regiter AdcTimer
 * OUT  : 
 *
 * RET  : Return the value of register 0xAdcTimer
 *
 * Notes : From register 0xA03, bits 7:0
 *
 **************************************************************************/
unsigned char set_adc_time_delay( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ADC_TIMER_REG);

    /* 
     * Defined GPADCTrig timer, from 0 to 255 x period of 32khz 
     */ 
  
    value =  old_value & ~ADC_TIME_DELAY_MASK;


    value |=  param ;  

    I2CWrite(ADC_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_time_delay
 *
 * RET  : Return the value of register 0xAdcTimer
 *
 * Notes : From register 0xA03, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_time_delay()
  {
    unsigned char value;


    /* 
     * Defined GPADCTrig timer, from 0 to 255 x period of 32khz 
     */ 
    value = (I2CRead(ADC_TIMER_REG) & ADC_TIME_DELAY_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_low
 *
 * RET  : Return the value of register 0xAdcDataLow
 *
 * Notes : From register 0xA05, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ADC_DATA_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_high
 *
 * RET  : Return the value of register 0xAdcDataHigh
 *
 * Notes : From register 0xA06, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_high()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ADC_DATA_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_low_trig
 *
 * RET  : Return the value of register 0xAdcDataLowTrig
 *
 * Notes : From register 0xA07, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_low_trig()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ADC_DATA_LOW_TRIG_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_high_trig
 *
 * RET  : Return the value of register 0xAdcDataHighTrig
 *
 * Notes : From register 0xA08, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_high_trig()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ADC_DATA_HIGH_TRIG_REG) ;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
