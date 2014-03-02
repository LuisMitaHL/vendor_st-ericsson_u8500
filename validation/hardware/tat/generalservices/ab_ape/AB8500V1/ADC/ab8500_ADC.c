/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_ADC/Linux/ab8500_ADC.c
 * 
 *
 * Generated on the 26/02/2010 08:57 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_ADC.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_charging_current_sense
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 7:7
 *
 **************************************************************************/
unsigned char set_charging_current_sense( enum charging_current_sense_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: disable charging current sense 
     * 1: enable charging current sense 
     */ 
    switch( param )
      {
        case CHARGING_CURRENT_SENSE_ENABLE_E: 
           value = old_value | CHARGING_CURRENT_SENSE_ENABLE_PARAM_MASK; 
           break;
        case CHARGING_CURRENT_SENSE_DISABLE_E: 
           value = old_value & ~ CHARGING_CURRENT_SENSE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charging_current_sense
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 7:7
 *
 **************************************************************************/
unsigned char get_charging_current_sense()
  {
    unsigned char value;


    /* 
     * 0: disable charging current sense 
     * 1: enable charging current sense 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & CHARGING_CURRENT_SENSE_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_buffer
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 6:6
 *
 **************************************************************************/
unsigned char set_adc_buffer( enum adc_buffer_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: disable channel buffer 
     * 1: enable channel buffer 
     */ 
    switch( param )
      {
        case ADC_BUFFER_ENABLE_E: 
           value = old_value | ADC_BUFFER_ENABLE_PARAM_MASK; 
           break;
        case ADC_BUFFER_DISABLE_E: 
           value = old_value & ~ ADC_BUFFER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_buffer
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 6:6
 *
 **************************************************************************/
unsigned char get_adc_buffer()
  {
    unsigned char value;


    /* 
     * 0: disable channel buffer 
     * 1: enable channel buffer 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_BUFFER_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_trig_edge
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 4:4
 *
 **************************************************************************/
unsigned char set_adc_trig_edge( enum adc_trig_edge_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: active 
     */ 
    switch( param )
      {
        case ADC_TRIG_EDGE_ENABLE_E: 
           value = old_value | ADC_TRIG_EDGE_ENABLE_PARAM_MASK; 
           break;
        case ADC_TRIG_EDGE_DISABLE_E: 
           value = old_value & ~ ADC_TRIG_EDGE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_trig_edge
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 4:4
 *
 **************************************************************************/
unsigned char get_adc_trig_edge()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: active 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_TRIG_EDGE_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_auto_req
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 3:3
 *
 **************************************************************************/
unsigned char set_adc_auto_req( enum adc_auto_req_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Auto mode ON 
     */ 
    switch( param )
      {
        case ADC_AUTO_REQ_ENABLE_E: 
           value = old_value | ADC_AUTO_REQ_ENABLE_PARAM_MASK; 
           break;
        case ADC_AUTO_REQ_DISABLE_E: 
           value = old_value & ~ ADC_AUTO_REQ_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_auto_req
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 3:3
 *
 **************************************************************************/
unsigned char get_adc_auto_req()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Auto mode ON 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_AUTO_REQ_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_man_convert
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 2:2
 *
 **************************************************************************/
unsigned char set_adc_man_convert( enum adc_man_convert_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: inactive 
     * 1: Launch an ADC conversion 
     */ 
    switch( param )
      {
        case ADC_MAN_CONVERT_ENABLE_E: 
           value = old_value | ADC_MAN_CONVERT_ENABLE_PARAM_MASK; 
           break;
        case ADC_MAN_CONVERT_DISABLE_E: 
           value = old_value & ~ ADC_MAN_CONVERT_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_man_convert
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 2:2
 *
 **************************************************************************/
unsigned char get_adc_man_convert()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Launch an ADC conversion 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_MAN_CONVERT_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_trigger
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 1:1
 *
 **************************************************************************/
unsigned char set_adc_trigger( enum adc_trigger_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: disable GPADCTrig functionality 
     * 1: enable GPADCTrig functionality 
     */ 
    switch( param )
      {
        case ADC_TRIGGER_ENABLE_E: 
           value = old_value | ADC_TRIGGER_ENABLE_PARAM_MASK; 
           break;
        case ADC_TRIGGER_DISABLE_E: 
           value = old_value & ~ ADC_TRIGGER_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_trigger
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 1:1
 *
 **************************************************************************/
unsigned char get_adc_trigger()
  {
    unsigned char value;


    /* 
     * 0: disable GPADCTrig functionality 
     * 1: enable GPADCTrig functionality 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_TRIGGER_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_enable
 *
 * IN   : param, a value to write to the regiter AdcControl
 * OUT  : 
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 0:0
 *
 **************************************************************************/
unsigned char set_adc_enable( enum adc_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_CONTROL_REG);

    /* 
     * 0: disable ADC 
     * 1: enable ADC 
     */ 
    switch( param )
      {
        case ADC_ENABLE_E: 
           value = old_value | ADC_ENABLE_PARAM_MASK; 
           break;
        case ADC_DISABLE_E: 
           value = old_value & ~ ADC_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(ADC_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_enable
 *
 * RET  : Return the value of register AdcControl
 *
 * Notes : From register 0x0A00, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc_enable()
  {
    unsigned char value;


    /* 
     * 0: disable ADC 
     * 1: enable ADC 
     */ 
    value = (SPIRead(ADC_CONTROL_REG) & ADC_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sample_to_average
 *
 * IN   : param, a value to write to the regiter AdcInputChannel
 * OUT  : 
 *
 * RET  : Return the value of register AdcInputChannel
 *
 * Notes : From register 0x0A01, bits 6:5
 *
 **************************************************************************/
unsigned char set_sample_to_average( enum sample_to_average param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_INPUT_CHANNEL_REG);

    /* 
     * Define number of ADC sample to average 
     */ 
  

     value =  old_value & ~SAMPLE_TO_AVERAGE_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(ADC_INPUT_CHANNEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sample_to_average
 *
 * RET  : Return the value of register AdcInputChannel
 *
 * Notes : From register 0x0A01, bits 6:5
 *
 **************************************************************************/
unsigned char get_sample_to_average()
  {
    unsigned char value;


    /* 
     * Define number of ADC sample to average 
     */ 
    value = (SPIRead(ADC_INPUT_CHANNEL_REG) & SAMPLE_TO_AVERAGE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_select_input
 *
 * IN   : param, a value to write to the regiter AdcInputChannel
 * OUT  : 
 *
 * RET  : Return the value of register AdcInputChannel
 *
 * Notes : From register 0x0A01, bits 4:0
 *
 **************************************************************************/
unsigned char set_adc_select_input( enum adc_input_channel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_INPUT_CHANNEL_REG);

    /* 
     * Select ADC input channel 
     */ 
  

     value =  old_value & ~ADC_SELECT_INPUT_PARAM_MASK ;

    switch(  param )
      {
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
        case DIE_TEMPERATURE_E:
            value =  value | DIE_TEMPERATURE ;
           break;
        case ID_E:
            value =  value | ID ;
           break;
      }
  

    SPIWrite(ADC_INPUT_CHANNEL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_select_input
 *
 * RET  : Return the value of register AdcInputChannel
 *
 * Notes : From register 0x0A01, bits 4:0
 *
 **************************************************************************/
unsigned char get_adc_select_input()
  {
    unsigned char value;


    /* 
     * Select ADC input channel 
     */ 
    value = (SPIRead(ADC_INPUT_CHANNEL_REG) & ADC_INPUT_CHANNEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sample_to_average_trig
 *
 * IN   : param, a value to write to the regiter AdcInputChannelTrig
 * OUT  : 
 *
 * RET  : Return the value of register AdcInputChannelTrig
 *
 * Notes : From register 0x0A02, bits 6:5
 *
 **************************************************************************/
unsigned char set_sample_to_average_trig( enum sample_to_average param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_INPUT_CHANNEL_TRIG_REG);

    /* 
     * Define number of ADC sample to average 
     */ 
  

     value =  old_value & ~SAMPLE_TO_AVERAGE_TRIG_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(ADC_INPUT_CHANNEL_TRIG_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sample_to_average_trig
 *
 * RET  : Return the value of register AdcInputChannelTrig
 *
 * Notes : From register 0x0A02, bits 6:5
 *
 **************************************************************************/
unsigned char get_sample_to_average_trig()
  {
    unsigned char value;


    /* 
     * Define number of ADC sample to average 
     */ 
    value = (SPIRead(ADC_INPUT_CHANNEL_TRIG_REG) & SAMPLE_TO_AVERAGE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_select_input_tric
 *
 * IN   : param, a value to write to the regiter AdcInputChannelTrig
 * OUT  : 
 *
 * RET  : Return the value of register AdcInputChannelTrig
 *
 * Notes : From register 0x0A02, bits 4:0
 *
 **************************************************************************/
unsigned char set_adc_select_input_tric( enum adc_input_channel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_INPUT_CHANNEL_TRIG_REG);

    /* 
     * Select ADC input channel 
     */ 
  

     value =  old_value & ~ADC_SELECT_INPUT_TRIC_PARAM_MASK ;

    switch(  param )
      {
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
        case DIE_TEMPERATURE_E:
            value =  value | DIE_TEMPERATURE ;
           break;
        case ID_E:
            value =  value | ID ;
           break;
      }
  

    SPIWrite(ADC_INPUT_CHANNEL_TRIG_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_select_input_tric
 *
 * RET  : Return the value of register AdcInputChannelTrig
 *
 * Notes : From register 0x0A02, bits 4:0
 *
 **************************************************************************/
unsigned char get_adc_select_input_tric()
  {
    unsigned char value;


    /* 
     * Select ADC input channel 
     */ 
    value = (SPIRead(ADC_INPUT_CHANNEL_TRIG_REG) & ADC_INPUT_CHANNEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc_time_delay
 *
 * IN   : param, a value to write to the regiter AdcTimer
 * OUT  : 
 *
 * RET  : Return the value of register AdcTimer
 *
 * Notes : From register 0x0A03, bits 7:0
 *
 **************************************************************************/
unsigned char set_adc_time_delay( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(ADC_TIMER_REG);

    /* 
     * Defined GPADCTrig timer, from 0 to 255 x period of 32khz 
     */ 
  
    value =  old_value & ~ADC_DELAY_PARAM_MASK;


    value |=  param ;  

    SPIWrite(ADC_TIMER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc_time_delay
 *
 * RET  : Return the value of register AdcTimer
 *
 * Notes : From register 0x0A03, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_time_delay()
  {
    unsigned char value;


    /* 
     * Defined GPADCTrig timer, from 0 to 255 x period of 32khz 
     */ 
    value = (SPIRead(ADC_TIMER_REG) & ADC_DELAY_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_busy
 *
 * RET  : Return the value of register AdcStatus
 *
 * Notes : From register 0x0A04, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc_busy()
  {
    unsigned char value;


    /* 
     * 0: GPADC not busy 
     * 1: GPADC is busy 
     */ 
    value = (SPIRead(ADC_STATUS_REG) & ADC_BUSY_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_low
 *
 * RET  : Return the value of register AdcDataLow
 *
 * Notes : From register 0x0A05, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_low()
  {
    unsigned char value;


    /* 
     * ADC output data in manual mode 
     */ 
    value = SPIRead(ADC_DATA_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_high
 *
 * RET  : Return the value of register AdcDataHigh
 *
 * Notes : From register 0x0A06, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_high()
  {
    unsigned char value;


    /* 
     * ADC output data in manual mode Two signifiant bits 
     */ 
    value = SPIRead(ADC_DATA_HIGH_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_low_trig
 *
 * RET  : Return the value of register AdcDataLowTrig
 *
 * Notes : From register 0x0A07, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_low_trig()
  {
    unsigned char value;


    /* 
     * ADC output data in HW mode 
     */ 
    value = SPIRead(ADC_DATA_LOW_TRIG_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_high_trig
 *
 * RET  : Return the value of register AdcDataHighTrig
 *
 * Notes : From register 0x0A08, bits 7:0
 *
 **************************************************************************/
unsigned char get_adc_data_high_trig()
  {
    unsigned char value;


    /* 
     * ADC output data in HW mode Two signifiant bits 
     */ 
    value = SPIRead(ADC_DATA_HIGH_TRIG_REG) ;
    return value;
  }


