/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_ADC/Linux/ab8500_ADC_types.h
 * 
 *
 * Generated on the 20/04/2011 10:26 by the 'gen-I2C' code generator 
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

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_ADC_LINUX_AB8500_ADC_TYPES_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_ADC_LINUX_AB8500_ADC_TYPES_H




//  
//  0: disable charging current sense 
//  1: enable charging current sense.
//  Register AdcControl 0x0A00, Bits 7:7, typedef charging_current_sense_enable
//   
//  
#define CHARGING_CURRENT_SENSE_MASK 0x80
#define CHARGING_CURRENT_SENSE_OFFSET 7
#define CHARGING_CURRENT_SENSE_STOP_BIT 7
#define CHARGING_CURRENT_SENSE_START_BIT 7
#define CHARGING_CURRENT_SENSE_WIDTH 1

#define ADC_CONTROL_REG 0xA00

typedef enum charging_current_sense_enable {
    CHARGING_CURRENT_SENSE_DISABLE_E,
    CHARGING_CURRENT_SENSE_ENABLE_E
} CHARGING_CURRENT_SENSE_ENABLE_T ;

#define CHARGING_CURRENT_SENSE_ENABLE_PARAM_MASK  0x80

//  
//  0: disable channel buffer 
//  1: enable channel buffer.
//  Register AdcControl 0x0A00, Bits 6:6, typedef adc_buffer_enable
//   
//  
#define ADC_BUFFER_MASK 0x40
#define ADC_BUFFER_OFFSET 6
#define ADC_BUFFER_STOP_BIT 6
#define ADC_BUFFER_START_BIT 6
#define ADC_BUFFER_WIDTH 1


typedef enum adc_buffer_enable {
    ADC_BUFFER_DISABLE_E,
    ADC_BUFFER_ENABLE_E
} ADC_BUFFER_ENABLE_T ;

#define ADC_BUFFER_ENABLE_PARAM_MASK  0x40

//  
//  0: Select Vtvout as Pull-Up supply 
//  1: Select Vrtc as Pull-Up supply.
//  Register AdcControl 0x0A00, Bits 5:5, typedef pull_up_supply
//   
//  
#define PULL_UP_SUPPLY_MASK 0x20
#define PULL_UP_SUPPLY_OFFSET 5
#define PULL_UP_SUPPLY_STOP_BIT 5
#define PULL_UP_SUPPLY_START_BIT 5
#define PULL_UP_SUPPLY_WIDTH 1

#define  PULL_UP_SUPPLY 32

typedef enum pull_up_supply {
    V_TV_OUT_SUPPLY_E,
    V_RTC_SUPPLY_E
} PULL_UP_SUPPLY_T ;
#define PULL_UP_SUPPLY_PARAM_MASK  0x20



//  
//  0: inactive 
//  1: active.
//  Register AdcControl 0x0A00, Bits 4:4, typedef adc_trig_edge_enable
//   
//  
#define ADC_TRIG_EDGE_MASK 0x10
#define ADC_TRIG_EDGE_OFFSET 4
#define ADC_TRIG_EDGE_STOP_BIT 4
#define ADC_TRIG_EDGE_START_BIT 4
#define ADC_TRIG_EDGE_WIDTH 1


typedef enum adc_trig_edge_enable {
    ADC_TRIG_EDGE_DISABLE_E,
    ADC_TRIG_EDGE_ENABLE_E
} ADC_TRIG_EDGE_ENABLE_T ;

#define ADC_TRIG_EDGE_ENABLE_PARAM_MASK  0x10

//  
//  0: inactive 
//  1: Auto mode ON.
//  Register AdcControl 0x0A00, Bits 3:3, typedef adc_auto_req_enable
//   
//  
#define ADC_AUTO_REQ_MASK 0x8
#define ADC_AUTO_REQ_OFFSET 3
#define ADC_AUTO_REQ_STOP_BIT 3
#define ADC_AUTO_REQ_START_BIT 3
#define ADC_AUTO_REQ_WIDTH 1


typedef enum adc_auto_req_enable {
    ADC_AUTO_REQ_DISABLE_E,
    ADC_AUTO_REQ_ENABLE_E
} ADC_AUTO_REQ_ENABLE_T ;

#define ADC_AUTO_REQ_ENABLE_PARAM_MASK  0x8

//  
//  0: inactive 
//  1: Launch an ADC conversion.
//  Register AdcControl 0x0A00, Bits 2:2, typedef adc_man_convert_enable
//   
//  
#define ADC_MAN_CONVERT_MASK 0x4
#define ADC_MAN_CONVERT_OFFSET 2
#define ADC_MAN_CONVERT_STOP_BIT 2
#define ADC_MAN_CONVERT_START_BIT 2
#define ADC_MAN_CONVERT_WIDTH 1


typedef enum adc_man_convert_enable {
    ADC_MAN_CONVERT_DISABLE_E,
    ADC_MAN_CONVERT_ENABLE_E
} ADC_MAN_CONVERT_ENABLE_T ;

#define ADC_MAN_CONVERT_ENABLE_PARAM_MASK  0x4

//  
//  0: disable GPADCTrig functionality 
//  1: enable GPADCTrig functionality.
//  Register AdcControl 0x0A00, Bits 1:1, typedef adc_trigger_enable
//   
//  
#define ADC_TRIGGER_MASK 0x2
#define ADC_TRIGGER_OFFSET 1
#define ADC_TRIGGER_STOP_BIT 1
#define ADC_TRIGGER_START_BIT 1
#define ADC_TRIGGER_WIDTH 1


typedef enum adc_trigger_enable {
    ADC_TRIGGER_DISABLE_E,
    ADC_TRIGGER_ENABLE_E
} ADC_TRIGGER_ENABLE_T ;

#define ADC_TRIGGER_ENABLE_PARAM_MASK  0x2

//  
//  0: disable ADC 
//  1: enable ADC.
//  Register AdcControl 0x0A00, Bits 0:0, typedef adc_enable
//   
//  
#define ADC_ENABLE_MASK 0x1
#define ADC_ENABLE_OFFSET 0
#define ADC_ENABLE_STOP_BIT 0
#define ADC_ENABLE_START_BIT 0
#define ADC_ENABLE_WIDTH 1


typedef enum adc_enable {
    ADC_DISABLE_E,
    ADC_ENABLE_E
} ADC_ENABLE_T ;

#define ADC_ENABLE_PARAM_MASK  0x1
#define  NOT_USED 0
#define  BAT_CTRL 1
#define  BAT_TEMP 2
#define  MAIN_CHARGER_VOLTAGE 3
#define  ACC_DETECT1 4
#define  ACC_DETECT2 5
#define  ADC_AUX1 6
#define  ADC_AUX2 7
#define  VBATA 8
#define  VBUS 9
#define  MAIN_CHARGER_CURRENT 10
#define  USB_CHARGER_CURRENT 11
#define  BACKUP_BAT 12
#define  RESERVED 13
#define  ID_BALL 14
#define  INTERNAL_TEST1 15
#define  INTERNAL_TEST2 16
#define  INTERNAL_TEST3 17



typedef enum adc_input_channel {
    NOT_USED_E,
    BAT_CTRL_E,
    BAT_TEMP_E,
    MAIN_CHARGER_VOLTAGE_E,
    ACC_DETECT1_E,
    ACC_DETECT2_E,
    ADC_AUX1_E,
    ADC_AUX2_E,
    VBATA_E,
    VBUS_E,
    MAIN_CHARGER_CURRENT_E,
    USB_CHARGER_CURRENT_E,
    BACKUP_BAT_E,
    RESERVED_E,
    ID_BALL_E,
    INTERNAL_TEST1_E,
    INTERNAL_TEST2_E,
    INTERNAL_TEST3_E
} ADC_INPUT_CHANNEL_T ;



#define  AVARAGE_1_SAMPLE 0
#define  AVARAGE_4_SAMPLES 1
#define  AVARAGE_8_SAMPLES 2
#define  AVARAGE_16_SAMPLES 3



typedef enum sample_to_average {
    AVARAGE_1_SAMPLE_E,
    AVARAGE_4_SAMPLES_E,
    AVARAGE_8_SAMPLES_E,
    AVARAGE_16_SAMPLES_E
} SAMPLE_TO_AVERAGE_T ;




//  
//  Define number of ADC sample to average.
//  Register AdcInputChannel 0x0A01, Bits 5:6, typedef sample_to_average
//   
//  
#define SAMPLE_TO_AVERAGE_MASK 0x60
#define SAMPLE_TO_AVERAGE_OFFSET 6
#define SAMPLE_TO_AVERAGE_STOP_BIT 6
#define SAMPLE_TO_AVERAGE_START_BIT 5
#define SAMPLE_TO_AVERAGE_WIDTH 2

#define ADC_INPUT_CHANNEL_REG 0xA01
#define SAMPLE_TO_AVERAGE_PARAM_MASK  0x60

//  
//  Select ADC input channel.
//  Register AdcInputChannel 0x0A01, Bits 0:4, typedef adc_input_channel
//   
//  
#define ADC_SELECT_INPUT_MASK 0x1F
#define ADC_SELECT_INPUT_OFFSET 4
#define ADC_SELECT_INPUT_STOP_BIT 4
#define ADC_SELECT_INPUT_START_BIT 0
#define ADC_SELECT_INPUT_WIDTH 5

#define ADC_SELECT_INPUT_PARAM_MASK  0x1F
#define ADC_INPUT_CHANNEL_PARAM_MASK  0x1F

//  
//  Define number of ADC sample to average.
//  Register AdcInputChannelTrig 0x0A02, Bits 5:6, typedef sample_to_average
//   
//  
#define SAMPLE_TO_AVERAGE_TRIG_MASK 0x60
#define SAMPLE_TO_AVERAGE_TRIG_OFFSET 6
#define SAMPLE_TO_AVERAGE_TRIG_STOP_BIT 6
#define SAMPLE_TO_AVERAGE_TRIG_START_BIT 5
#define SAMPLE_TO_AVERAGE_TRIG_WIDTH 2

#define ADC_INPUT_CHANNEL_TRIG_REG 0xA02
#define SAMPLE_TO_AVERAGE_TRIG_PARAM_MASK  0x60

//  
//  Select ADC input channel.
//  Register AdcInputChannelTrig 0x0A02, Bits 0:4, typedef adc_input_channel
//   
//  
#define ADC_SELECT_INPUT_TRIC_MASK 0x1F
#define ADC_SELECT_INPUT_TRIC_OFFSET 4
#define ADC_SELECT_INPUT_TRIC_STOP_BIT 4
#define ADC_SELECT_INPUT_TRIC_START_BIT 0
#define ADC_SELECT_INPUT_TRIC_WIDTH 5

#define ADC_SELECT_INPUT_TRIC_PARAM_MASK  0x1F

//  
//  Defined GPADCTrig timer, from 0 to 255 x period of 32khz.
//  Register AdcTimer 0x0A03, Bits 0:7, typedef adc_delay
//   
//  
#define ADC_TIME_DELAY_MASK 0xFF
#define ADC_TIME_DELAY_OFFSET 7
#define ADC_TIME_DELAY_STOP_BIT 7
#define ADC_TIME_DELAY_START_BIT 0
#define ADC_TIME_DELAY_WIDTH 8

#define ADC_TIMER_REG 0xA03
#define ADC_DELAY_PARAM_MASK  0xFFUL

//  
//  0: GPADC not busy 
//  1: GPADC is busy.
//  Register AdcStatus 0x0A04, Bits 0:0, typedef adc_busy
//   
//  
#define ADC_BUSY_MASK 0x1
#define ADC_BUSY_OFFSET 0
#define ADC_BUSY_STOP_BIT 0
#define ADC_BUSY_START_BIT 0
#define ADC_BUSY_WIDTH 1

#define ADC_STATUS_REG 0xA04
#define ADC_BUSY_PARAM_MASK  0x1
#define ADC_DATA_LOW_REG 0xA05
#define ADC_DATA_HIGH_REG 0xA06
#define ADC_DATA_LOW_TRIG_REG 0xA07
#define ADC_DATA_HIGH_TRIG_REG 0xA08
#endif
