/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_MaskInterrupt/Linux/ab8500_MaskInterrupt_types.h
 * 
 *
 * Generated on the 26/02/2010 09:06 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_MASKINTERRUPT_LINUX_AB8500_MASKINTERRUPT_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_MASKINTERRUPT_LINUX_AB8500_MASKINTERRUPT_TYPES_H



#define MASKED_INTERRUPT_1_REG 0xE40

//  
//  Main charger connected is not an allowed Interupt mask.
//  Register MaskedInterrupt1 0x0E40, Bits 0:0, typedef main_charger_allowed_mask_set
//   
//  
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_MASK 0x1
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_OFFSET 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_STOP_BIT 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_START_BIT 0
#define MAIN_CHARGER_ALLOWED_MASKED_INTERRUPT_WIDTH 1


typedef enum main_charger_allowed_mask_set {
    MAIN_CHARGER_ALLOWED_MASK_RESET_SET_E,
    MAIN_CHARGER_ALLOWED_MASK_SET_E
} MAIN_CHARGER_ALLOWED_MASK_SET_T ;

#define MAIN_CHARGER_ALLOWED_MASK_SET_PARAM_MASK  0x1

//  
//  Tvset (75 Ohms) disconnection on CVBS ball detected masked.
//  Register MaskedInterrupt1 0x0E40, Bits 1:1, typedef un_plug_tv_mask_set
//   
//  
#define UN_PLUG_TV_MASKED_INTERRUPT_MASK 0x2
#define UN_PLUG_TV_MASKED_INTERRUPT_OFFSET 1
#define UN_PLUG_TV_MASKED_INTERRUPT_STOP_BIT 1
#define UN_PLUG_TV_MASKED_INTERRUPT_START_BIT 1
#define UN_PLUG_TV_MASKED_INTERRUPT_WIDTH 1


typedef enum un_plug_tv_mask_set {
    UN_PLUG_TV_MASK_RESET_SET_E,
    UN_PLUG_TV_MASK_SET_E
} UN_PLUG_TV_MASK_SET_T ;

#define UN_PLUG_TV_MASK_SET_PARAM_MASK  0x2

//  
//  Tvset (75 Ohms) connection on CVBS ball detected masked.
//  Register MaskedInterrupt1 0x0E40, Bits 2:2, typedef plug_tv_mask_set
//   
//  
#define PLUG_TV_MASKED_INTERRUPT_MASK 0x4
#define PLUG_TV_MASKED_INTERRUPT_OFFSET 2
#define PLUG_TV_MASKED_INTERRUPT_STOP_BIT 2
#define PLUG_TV_MASKED_INTERRUPT_START_BIT 2
#define PLUG_TV_MASKED_INTERRUPT_WIDTH 1


typedef enum plug_tv_mask_set {
    PLUG_TV_MASK_RESET_SET_E,
    PLUG_TV_MASK_SET_E
} PLUG_TV_MASK_SET_T ;

#define PLUG_TV_MASK_SET_PARAM_MASK  0x4

//  
//  Thermal warning masked.
//  Register MaskedInterrupt1 0x0E40, Bits 3:3, typedef thermal_warning_mask_set
//   
//  
#define THERMAL_WARNING_MASKED_INTERRUPT_MASK 0x8
#define THERMAL_WARNING_MASKED_INTERRUPT_OFFSET 3
#define THERMAL_WARNING_MASKED_INTERRUPT_STOP_BIT 3
#define THERMAL_WARNING_MASKED_INTERRUPT_START_BIT 3
#define THERMAL_WARNING_MASKED_INTERRUPT_WIDTH 1


typedef enum thermal_warning_mask_set {
    THERMAL_WARNING_MASK_RESET_SET_E,
    THERMAL_WARNING_MASK_SET_E
} THERMAL_WARNING_MASK_SET_T ;

#define THERMAL_WARNING_MASK_SET_PARAM_MASK  0x8

//  
//  Falling edge on PonKey2 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 4:4, typedef falling_pon_key2_mask_set
//   
//  
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_MASK 0x10
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_OFFSET 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_STOP_BIT 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_START_BIT 4
#define FALLING_PON_KEY_2_MASKED_INTERRUPT_WIDTH 1


typedef enum falling_pon_key2_mask_set {
    FALLING_PON_KEY_2_MASK_RESET_SET_E,
    FALLING_PON_KEY_2_MASK_SET_E
} FALLING_PON_KEY_2_MASK_SET_T ;

#define FALLING_PON_KEY_2_MASK_SET_PARAM_MASK  0x10

//  
//  Rising edge on PonKey2 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 5:5, typedef rising_pon_key2_mask_set
//   
//  
#define RISING_PON_KEY_2_MASKED_INTERRUPT_MASK 0x20
#define RISING_PON_KEY_2_MASKED_INTERRUPT_OFFSET 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_STOP_BIT 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_START_BIT 5
#define RISING_PON_KEY_2_MASKED_INTERRUPT_WIDTH 1


typedef enum rising_pon_key2_mask_set {
    RISING_PON_KEY_2_MASK_RESET_SET_E,
    RISING_PON_KEY_2_MASK_SET_E
} RISING_PON_KEY_2_MASK_SET_T ;

#define RISING_PON_KEY_2_MASK_SET_PARAM_MASK  0x20

//  
//  Falling edge on PonKey1 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 6:6, typedef falling_pon_key1_mask_set
//   
//  
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_MASK 0x40
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_OFFSET 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_STOP_BIT 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_START_BIT 6
#define FALLING_PON_KEY_1_MASKED_INTERRUPT_WIDTH 1


typedef enum falling_pon_key1_mask_set {
    FALLING_PON_KEY_1_MASK_RESET_SET_E,
    FALLING_PON_KEY_1_MASK_SET_E
} FALLING_PON_KEY_1_MASK_SET_T ;

#define FALLING_PON_KEY_1_MASK_SET_PARAM_MASK  0x40

//  
//  Rising edge on PonKey1 ball masked.
//  Register MaskedInterrupt1 0x0E40, Bits 7:7, typedef rising_pon_key1_mask_set
//   
//  
#define RISING_PON_KEY_1_MASKED_INTERRUPT_MASK 0x80
#define RISING_PON_KEY_1_MASKED_INTERRUPT_OFFSET 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_STOP_BIT 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_START_BIT 7
#define RISING_PON_KEY_1_MASKED_INTERRUPT_WIDTH 1


typedef enum rising_pon_key1_mask_set {
    RISING_PON_KEY_1_MASK_RESET_SET_E,
    RISING_PON_KEY_1_MASK_SET_E
} RISING_PON_KEY_1_MASK_SET_T ;

#define RISING_PON_KEY_1_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_2_REG 0xE41

//  
//  TVbatA ball goes upper over voltage threshold masked.
//  Register MaskedInterrupt2 0x0E41, Bits 0:0, typedef battery_over_voltage_mask_set
//   
//  
#define BATTERY_OVER_VOLTAGE_MASK_MASK 0x1
#define BATTERY_OVER_VOLTAGE_MASK_OFFSET 0
#define BATTERY_OVER_VOLTAGE_MASK_STOP_BIT 0
#define BATTERY_OVER_VOLTAGE_MASK_START_BIT 0
#define BATTERY_OVER_VOLTAGE_MASK_WIDTH 1


typedef enum battery_over_voltage_mask_set {
    BATTERY_OVER_VOLTAGE_MASK_RESET_SET_E,
    BATTERY_OVER_VOLTAGE_MASK_SET_E
} BATTERY_OVER_VOLTAGE_MASK_SET_T ;

#define BATTERY_OVER_VOLTAGE_MASK_SET_PARAM_MASK  0x1

//  
//  Main charger unplug masked.
//  Register MaskedInterrupt2 0x0E41, Bits 2:2, typedef main_charger_un_plug_mask_set
//   
//  
#define MAIN_CHARGER_UN_PLUG_MASK_MASK 0x4
#define MAIN_CHARGER_UN_PLUG_MASK_OFFSET 2
#define MAIN_CHARGER_UN_PLUG_MASK_STOP_BIT 2
#define MAIN_CHARGER_UN_PLUG_MASK_START_BIT 2
#define MAIN_CHARGER_UN_PLUG_MASK_WIDTH 1


typedef enum main_charger_un_plug_mask_set {
    MAIN_CHARGER_UN_PLUG_MASK_RESET_SET_E,
    MAIN_CHARGER_UN_PLUG_MASK_SET_E
} MAIN_CHARGER_UN_PLUG_MASK_SET_T ;

#define MAIN_CHARGER_UN_PLUG_MASK_SET_PARAM_MASK  0x4

//  
//  Main charger plug masked.
//  Register MaskedInterrupt2 0x0E41, Bits 3:3, typedef main_charger_plug_mask_set
//   
//  
#define MAIN_CHARGER_PLUG_MASK_MASK 0x8
#define MAIN_CHARGER_PLUG_MASK_OFFSET 3
#define MAIN_CHARGER_PLUG_MASK_STOP_BIT 3
#define MAIN_CHARGER_PLUG_MASK_START_BIT 3
#define MAIN_CHARGER_PLUG_MASK_WIDTH 1


typedef enum main_charger_plug_mask_set {
    MAIN_CHARGER_PLUG_MASK_RESET_SET_E,
    MAIN_CHARGER_PLUG_MASK_SET_E
} MAIN_CHARGER_PLUG_MASK_SET_T ;

#define MAIN_CHARGER_PLUG_MASK_SET_PARAM_MASK  0x8

//  
//  Falling edge on ID ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 4:4, typedef usb_id_falling_edge_mask_set
//   
//  
#define USB_ID_FALLING_EDGE_MASK_MASK 0x10
#define USB_ID_FALLING_EDGE_MASK_OFFSET 4
#define USB_ID_FALLING_EDGE_MASK_STOP_BIT 4
#define USB_ID_FALLING_EDGE_MASK_START_BIT 4
#define USB_ID_FALLING_EDGE_MASK_WIDTH 1


typedef enum usb_id_falling_edge_mask_set {
    USB_ID_FALLING_EDGE_MASK_RESET_SET_E,
    USB_ID_FALLING_EDGE_MASK_SET_E
} USB_ID_FALLING_EDGE_MASK_SET_T ;

#define USB_ID_FALLING_EDGE_MASK_SET_PARAM_MASK  0x10

//  
//  Rising edge on ID ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 5:5, typedef usb_id_rising_edge_mask_set
//   
//  
#define USB_ID_RISING_EDGE_MASK_MASK 0x20
#define USB_ID_RISING_EDGE_MASK_OFFSET 5
#define USB_ID_RISING_EDGE_MASK_STOP_BIT 5
#define USB_ID_RISING_EDGE_MASK_START_BIT 5
#define USB_ID_RISING_EDGE_MASK_WIDTH 1


typedef enum usb_id_rising_edge_mask_set {
    USB_ID_RISING_EDGE_MASK_RESET_SET_E,
    USB_ID_RISING_EDGE_MASK_SET_E
} USB_ID_RISING_EDGE_MASK_SET_T ;

#define USB_ID_RISING_EDGE_MASK_SET_PARAM_MASK  0x20

//  
//  Falling edge on Vbus ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 6:6, typedef vbus_falling_edge_mask_set
//   
//  
#define VBUS_FALLING_EDGE_MASK_MASK 0x40
#define VBUS_FALLING_EDGE_MASK_OFFSET 6
#define VBUS_FALLING_EDGE_MASK_STOP_BIT 6
#define VBUS_FALLING_EDGE_MASK_START_BIT 6
#define VBUS_FALLING_EDGE_MASK_WIDTH 1


typedef enum vbus_falling_edge_mask_set {
    VBUS_FALLING_EDGE_MASK_RESET_SET_E,
    VBUS_FALLING_EDGE_MASK_SET_E
} VBUS_FALLING_EDGE_MASK_SET_T ;

#define VBUS_FALLING_EDGE_MASK_SET_PARAM_MASK  0x40

//  
//  Rising edge on Vbus ball masked.
//  Register MaskedInterrupt2 0x0E41, Bits 7:7, typedef vbus_rising_edge_mask_set
//   
//  
#define VBUS_RISING_EDGE_MASK_MASK 0x80
#define VBUS_RISING_EDGE_MASK_OFFSET 7
#define VBUS_RISING_EDGE_MASK_STOP_BIT 7
#define VBUS_RISING_EDGE_MASK_START_BIT 7
#define VBUS_RISING_EDGE_MASK_WIDTH 1


typedef enum vbus_rising_edge_mask_set {
    VBUS_RISING_EDGE_MASK_RESET_SET_E,
    VBUS_RISING_EDGE_MASK_SET_E
} VBUS_RISING_EDGE_MASK_SET_T ;

#define VBUS_RISING_EDGE_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_3_REG 0xE42

//  
//  Usb charger was in dropout and internal input current loop update current regulation 
//  to avoid usb charger to drop.
//  Register MaskedInterrupt3 0x0E42, Bits 0:0, typedef usb_drop_out_mask_set
//   
//  
#define USB_CHARGER_DROPOUT_MASK_MASK 0x1
#define USB_CHARGER_DROPOUT_MASK_OFFSET 0
#define USB_CHARGER_DROPOUT_MASK_STOP_BIT 0
#define USB_CHARGER_DROPOUT_MASK_START_BIT 0
#define USB_CHARGER_DROPOUT_MASK_WIDTH 1

#define USB_DROP_OUT_MASK_SET_PARAM_MASK  0x1

//  
//  RTC timer reaches a 60s period.
//  Register MaskedInterrupt3 0x0E42, Bits 1:1, typedef period60s_mask_set
//   
//  
#define RTC_6_0S_MASK_MASK 0x2
#define RTC_6_0S_MASK_OFFSET 1
#define RTC_6_0S_MASK_STOP_BIT 1
#define RTC_6_0S_MASK_START_BIT 1
#define RTC_6_0S_MASK_WIDTH 1

#define PERIOD_6_0S_MASK_SET_PARAM_MASK  0x2

//  
//  RTC timer reaches alarm time.
//  Register MaskedInterrupt3 0x0E02, Bits 2:2, typedef reach_alarm_time_mask_set
//   
//  
#define RTC_ALARM_MASK_MASK 0x4
#define RTC_ALARM_MASK_OFFSET 2
#define RTC_ALARM_MASK_STOP_BIT 2
#define RTC_ALARM_MASK_START_BIT 2
#define RTC_ALARM_MASK_WIDTH 1

#define REACH_ALARM_TIME_MASK_SET_PARAM_MASK  0x4

//  
//  Battery removal.
//  Register MaskedInterrupt3 0x0E42, Bits 4:4, typedef battery_removal_mask_set
//   
//  
#define BATTERY_REMOVAL_MASK_MASK 0x10
#define BATTERY_REMOVAL_MASK_OFFSET 4
#define BATTERY_REMOVAL_MASK_STOP_BIT 4
#define BATTERY_REMOVAL_MASK_START_BIT 4
#define BATTERY_REMOVAL_MASK_WIDTH 1


typedef enum battery_removal_mask_set {
    BATTERY_REMOVAL_MASK_RESET_SET_E,
    BATTERY_REMOVAL_MASK_SET_E
} BATTERY_REMOVAL_MASK_SET_T ;

#define BATTERY_REMOVAL_MASK_SET_PARAM_MASK  0x10

//  
//  Watchdog charger expiration detected.
//  Register MaskedInterrupt3 0x0E42, Bits 5:5, typedef 
//  watchdog_charger_expiration_mask_set
//   
//  
#define WATCHDOG_CHARGER_EXPIRATION_MASK_MASK 0x20
#define WATCHDOG_CHARGER_EXPIRATION_MASK_OFFSET 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_STOP_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_START_BIT 5
#define WATCHDOG_CHARGER_EXPIRATION_MASK_WIDTH 1


typedef enum watchdog_charger_expiration_mask_set {
    WATCHDOG_CHARGER_EXPIRATION_MASK_RESET_SET_E,
    WATCHDOG_CHARGER_EXPIRATION_MASK_SET_E
} WATCHDOG_CHARGER_EXPIRATION_MASK_SET_T ;

#define WATCHDOG_CHARGER_EXPIRATION_MASK_SET_PARAM_MASK  0x20

//  
//  Overvoltage on Vbus ball detected.
//  Register MaskedInterrupt3 0x0E42, Bits 6:6, typedef overvoltage_on_vbus_l_mask_set
//   
//  
#define OVERVOLTAGE_ON_VBUS_MASK_MASK 0x40
#define OVERVOLTAGE_ON_VBUS_MASK_OFFSET 6
#define OVERVOLTAGE_ON_VBUS_MASK_STOP_BIT 6
#define OVERVOLTAGE_ON_VBUS_MASK_START_BIT 6
#define OVERVOLTAGE_ON_VBUS_MASK_WIDTH 1


typedef enum overvoltage_on_vbus_l_mask_set {
    OVERVOLTAGE_ON_VBUS_L_MASK_RESET_SET_E,
    OVERVOLTAGE_ON_VBUS_L_MASK_SET_E
} OVERVOLTAGE_ON_VBUS_L_MASK_SET_T ;

#define OVERVOLTAGE_ON_VBUS_L_MASK_SET_PARAM_MASK  0x40

//  
//  Icharge > Icharge max programmed detected.
//  Register MaskedInterrupt3 0x0E42, Bits 7:7, typedef 
//  cain_charger_over_current_mask_set
//   
//  
#define MAIN_CHARGER_OVER_CURRENT_MASK_MASK 0x80
#define MAIN_CHARGER_OVER_CURRENT_MASK_OFFSET 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_STOP_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_START_BIT 7
#define MAIN_CHARGER_OVER_CURRENT_MASK_WIDTH 1


typedef enum cain_charger_over_current_mask_set {
    CAIN_CHARGER_OVER_CURRENT_MASK_RESET_SET_E,
    CAIN_CHARGER_OVER_CURRENT_MASK_SET_E
} CAIN_CHARGER_OVER_CURRENT_MASK_SET_T ;

#define CAIN_CHARGER_OVER_CURRENT_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_4_REG 0xE43

//  
//  Accumulation of N sample conversion is detected.
//  Register MaskedInterrupt4 0x0E43, Bits 0:0, typedef accumulation_conversion_mask_set
//   
//  
#define ACCUMULATION_CONVERSION_MASK_MASK 0x1
#define ACCUMULATION_CONVERSION_MASK_OFFSET 0
#define ACCUMULATION_CONVERSION_MASK_STOP_BIT 0
#define ACCUMULATION_CONVERSION_MASK_START_BIT 0
#define ACCUMULATION_CONVERSION_MASK_WIDTH 1


typedef enum accumulation_conversion_mask_set {
    ACCUMULATION_CONVERSION_MASK_RESET_SET_E,
    ACCUMULATION_CONVERSION_MASK_SET_E
} ACCUMULATION_CONVERSION_MASK_SET_T ;

#define ACCUMULATION_CONVERSION_MASK_SET_PARAM_MASK  0x1

//  
//  Audio interrupt form audio digital part detected.
//  Register MaskedInterrupt4 0x0E43, Bits 1:1, typedef audio_mask_set
//   
//  
#define AUDIO_MASK_MASK 0x2
#define AUDIO_MASK_OFFSET 1
#define AUDIO_MASK_STOP_BIT 1
#define AUDIO_MASK_START_BIT 1
#define AUDIO_MASK_WIDTH 1


typedef enum audio_mask_set {
    AUDIO_MASK_RESET_SET_E,
    AUDIO_MASK_SET_E
} AUDIO_MASK_SET_T ;

#define AUDIO_MASK_SET_PARAM_MASK  0x2

//  
//  Coulomb Counter has ended data convertion.
//  Register MaskedInterrupt4 0x0E43, Bits 2:2, typedef coulomb_counter_ended_mask_set
//   
//  
#define COULOMB_ENDED_MASK_MASK 0x4
#define COULOMB_ENDED_MASK_OFFSET 2
#define COULOMB_ENDED_MASK_STOP_BIT 2
#define COULOMB_ENDED_MASK_START_BIT 2
#define COULOMB_ENDED_MASK_WIDTH 1


typedef enum coulomb_counter_ended_mask_set {
    COULOMB_COUNTER_ENDED_MASK_RESET_SET_E,
    COULOMB_COUNTER_ENDED_MASK_SET_E
} COULOMB_COUNTER_ENDED_MASK_SET_T ;

#define COULOMB_COUNTER_ENDED_MASK_SET_PARAM_MASK  0x4

//  
//  Coulomb Counter has ended its calibration.
//  Register MaskedInterrupt4 0x0E43, Bits 3:3, typedef 
//  coulomb_calibration_endedd_mask_set
//   
//  
#define COULOMB_CALIBRATION_ENDEDD_MASK_MASK 0x8
#define COULOMB_CALIBRATION_ENDEDD_MASK_OFFSET 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_STOP_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_START_BIT 3
#define COULOMB_CALIBRATION_ENDEDD_MASK_WIDTH 1


typedef enum coulomb_calibration_endedd_mask_set {
    COULOMB_CALIBRATION_ENDEDD_MASK_RESET_SET_E,
    COULOMB_CALIBRATION_ENDEDD_MASK_SET_E
} COULOMB_CALIBRATION_ENDEDD_MASK_SET_T ;

#define COULOMB_CALIBRATION_ENDEDD_MASK_SET_PARAM_MASK  0x8

//  
//  VbatA voltage goes below LowBat register programmed threshold.
//  Register MaskedInterrupt4 0x0E43, Bits 4:4, typedef low_battery_mask_set
//   
//  
#define LOW_BATTERY_MASK_MASK 0x10
#define LOW_BATTERY_MASK_OFFSET 4
#define LOW_BATTERY_MASK_STOP_BIT 4
#define LOW_BATTERY_MASK_START_BIT 4
#define LOW_BATTERY_MASK_WIDTH 1


typedef enum low_battery_mask_set {
    LOW_BATTERY_MASK_RESET_SET_E,
    LOW_BATTERY_MASK_SET_E
} LOW_BATTERY_MASK_SET_T ;

#define LOW_BATTERY_MASK_SET_PARAM_MASK  0x10

//  
//  VbatA voltage goes above LowBat register programmed threshold.
//  Register MaskedInterrupt4 0x0E43, Bits 5:5, typedef high_battery_mask_set
//   
//  
#define HIGH_BATTERY_MASK_MASK 0x20
#define HIGH_BATTERY_MASK_OFFSET 5
#define HIGH_BATTERY_MASK_STOP_BIT 5
#define HIGH_BATTERY_MASK_START_BIT 5
#define HIGH_BATTERY_MASK_WIDTH 1


typedef enum high_battery_mask_set {
    LOW_E,
    HIGH_E
} HIGH_BATTERY_MASK_SET_T ;

#define HIGH_BATTERY_MASK_SET_PARAM_MASK  0x20

//  
//  BackUpBat ball voltage goes above RtcBackupChg register.
//  Register MaskedInterrupt4 0x0E43, Bits 7:7, typedef rtc_battery_high_mask_set
//   
//  
#define RTC_BATTERY_HIGH_MASK_MASK 0x80
#define RTC_BATTERY_HIGH_MASK_OFFSET 7
#define RTC_BATTERY_HIGH_MASK_STOP_BIT 7
#define RTC_BATTERY_HIGH_MASK_START_BIT 7
#define RTC_BATTERY_HIGH_MASK_WIDTH 1


typedef enum rtc_battery_high_mask_set {
    RTC_BATTERY_LOW_E,
    RTC_BATTERY_HIGH_E
} RTC_BATTERY_HIGH_MASK_SET_T ;

#define RTC_BATTERY_HIGH_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_5_REG 0xE44

//  
//  GP ADC conversion requested thru Hardware control.
//  Register MaskedInterrupt5 0x0E44, Bits 0:0, typedef adc_hardware_control_mask_set
//   
//  
#define ADC_HARDWARE_CONTROL_MASK_MASK 0x1
#define ADC_HARDWARE_CONTROL_MASK_OFFSET 0
#define ADC_HARDWARE_CONTROL_MASK_STOP_BIT 0
#define ADC_HARDWARE_CONTROL_MASK_START_BIT 0
#define ADC_HARDWARE_CONTROL_MASK_WIDTH 1


typedef enum adc_hardware_control_mask_set {
    ADC_HARDWARE_CONTROL_MASK_RESET_SET_E,
    ADC_HARDWARE_CONTROL_MASK_SET_E
} ADC_HARDWARE_CONTROL_MASK_SET_T ;

#define ADC_HARDWARE_CONTROL_MASK_SET_PARAM_MASK  0x1

//  
//  AccDetect1 ball voltage level goes below AccDetect1Th.
//  Register MaskedInterrupt5 0x0E44, Bits 1:1, typedef accessory1_low_voltage_mask_set
//   
//  
#define ACCESSORY_1_LOW_VOLTAGE_MASK_MASK 0x2
#define ACCESSORY_1_LOW_VOLTAGE_MASK_OFFSET 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_STOP_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_START_BIT 1
#define ACCESSORY_1_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory1_low_voltage_mask_set {
    ACCESSORY_1_LOW_VOLTAGE_MASK_RESET_SET_E,
    ACCESSORY_1_LOW_VOLTAGE_MASK_SET_E
} ACCESSORY_1_LOW_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_1_LOW_VOLTAGE_MASK_SET_PARAM_MASK  0x2

//  
//  AccDetect1 ball voltage level goes above AccDetect1Th.
//  Register MaskedInterrupt5 0x0E44, Bits 2:2, typedef accessory1_high_voltage_mask_set
//   
//  
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_MASK 0x4
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_OFFSET 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_STOP_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_START_BIT 2
#define ACCESSORY_1_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory1_high_voltage_mask_set {
    ACCESSORY_1_LOW_E,
    ACCESSORY_1_HIGH_E
} ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_1_HIGH_VOLTAGE_MASK_SET_PARAM_MASK  0x4

//  
//  AccDetect2 ball voltage level goes below AccDetect22Th.
//  Register MaskedInterrupt5 0x0E44, Bits 3:3, typedef accessory22_low_voltage_mask_set
//   
//  
#define ACCESSORY_22_LOW_VOLTAGE_MASK_MASK 0x8
#define ACCESSORY_22_LOW_VOLTAGE_MASK_OFFSET 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_STOP_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_START_BIT 3
#define ACCESSORY_22_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory22_low_voltage_mask_set {
    ACCESSORY_22_LOW_VOLTAGE_MASK_RESET_SET_E,
    ACCESSORY_22_LOW_VOLTAGE_MASK_SET_E
} ACCESSORY_22_LOW_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_22_LOW_VOLTAGE_MASK_SET_PARAM_MASK  0x8

//  
//  AccDetect2 ball voltage level goes above AccDetect22Th.
//  Register MaskedInterrupt5 0x0E44, Bits 4:4, typedef 
//  accessory22_high_voltage_mask_set
//   
//  
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_MASK 0x10
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_OFFSET 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_STOP_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_START_BIT 4
#define ACCESSORY_22_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory22_high_voltage_mask_set {
    ACCESSORY_22_LOW_E,
    ACCESSORY_22_HIGH_E
} ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_22_HIGH_VOLTAGE_MASK_SET_PARAM_MASK  0x10

//  
//  AccDetect2 ball voltage level goes below AccDetect21Th.
//  Register MaskedInterrupt5 0x0E44, Bits 5:5, typedef accessory21_low_voltage_mask_set
//   
//  
#define ACCESSORY_21_LOW_VOLTAGE_MASK_MASK 0x20
#define ACCESSORY_21_LOW_VOLTAGE_MASK_OFFSET 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_STOP_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_START_BIT 5
#define ACCESSORY_21_LOW_VOLTAGE_MASK_WIDTH 1


typedef enum accessory21_low_voltage_mask_set {
    ACCESSORY_21_LOW_VOLTAGE_MASK_RESET_SET_E,
    ACCESSORY_21_LOW_VOLTAGE_MASK_SET_E
} ACCESSORY_21_LOW_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_21_LOW_VOLTAGE_MASK_SET_PARAM_MASK  0x20

//  
//  AccDetect2 ball voltage level goes above AccDetect21Th.
//  Register MaskedInterrupt5 0x0E44, Bits 6:6, typedef 
//  accessory21_high_voltage_mask_set
//   
//  
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_MASK 0x40
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_OFFSET 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_STOP_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_START_BIT 6
#define ACCESSORY_21_HIGH_VOLTAGE_MASK_WIDTH 1


typedef enum accessory21_high_voltage_mask_set {
    ACCESSORY_21_LOW_E,
    ACCESSORY_21_HIGH_E
} ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_T ;

#define ACCESSORY_21_HIGH_VOLTAGE_MASK_SET_PARAM_MASK  0x40

//  
//  GP ADC conversion requested thru software control is ended (data are 
//  available).
//  Register MaskedInterrupt5 0x0E44, Bits 7:7, typedef adc_software_control_mask_set
//   
//  
#define ADC_SOFTWARE_CONTROL_MASK_MASK 0x80
#define ADC_SOFTWARE_CONTROL_MASK_OFFSET 7
#define ADC_SOFTWARE_CONTROL_MASK_STOP_BIT 7
#define ADC_SOFTWARE_CONTROL_MASK_START_BIT 7
#define ADC_SOFTWARE_CONTROL_MASK_WIDTH 1


typedef enum adc_software_control_mask_set {
    ADC_SOFTWARE_CONTROL_MASK_RESET_SET_E,
    ADC_SOFTWARE_CONTROL_MASK_SET_E
} ADC_SOFTWARE_CONTROL_MASK_SET_T ;

#define ADC_SOFTWARE_CONTROL_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_7_REG 0xE46

//  
//  Rising edge Interrupt masked on GPIO13 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 7:7, typedef gpio13_rising_mask_set
//   
//  
#define GPIO_13_RISING_MASK_MASK 0x80
#define GPIO_13_RISING_MASK_OFFSET 7
#define GPIO_13_RISING_MASK_STOP_BIT 7
#define GPIO_13_RISING_MASK_START_BIT 7
#define GPIO_13_RISING_MASK_WIDTH 1


typedef enum gpio13_rising_mask_set {
    GPIO_13_RISING_MASK_RESET_SET_E,
    GPIO_13_RISING_MASK_SET_E
} GPIO_13_RISING_MASK_SET_T ;

#define GPIO_13_RISING_MASK_SET_PARAM_MASK  0x80

//  
//  Rising edge Interrupt masked on GPIO12 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 6:6, typedef gpio12_rising_mask_set
//   
//  
#define GPIO_12_RISING_MASK_MASK 0x40
#define GPIO_12_RISING_MASK_OFFSET 6
#define GPIO_12_RISING_MASK_STOP_BIT 6
#define GPIO_12_RISING_MASK_START_BIT 6
#define GPIO_12_RISING_MASK_WIDTH 1


typedef enum gpio12_rising_mask_set {
    GPIO_12_RISING_MASK_RESET_SET_E,
    GPIO_12_RISING_MASK_SET_E
} GPIO_12_RISING_MASK_SET_T ;

#define GPIO_12_RISING_MASK_SET_PARAM_MASK  0x40

//  
//  Rising edge Interrupt masked on GPIO11 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 5:8, typedef gpio11_rising_mask_set
//   
//  
#define GPIO_11_RISING_MASK_MASK 0x1E0
#define GPIO_11_RISING_MASK_OFFSET 8
#define GPIO_11_RISING_MASK_STOP_BIT 8
#define GPIO_11_RISING_MASK_START_BIT 5
#define GPIO_11_RISING_MASK_WIDTH 4


typedef enum gpio11_rising_mask_set {
    GPIO_11_RISING_MASK_RESET_SET_E,
    GPIO_11_RISING_MASK_SET_E
} GPIO_11_RISING_MASK_SET_T ;

#define GPIO_11_RISING_MASK_SET_PARAM_MASK  0x1E0

//  
//  Rising edge Interrupt masked on GPIO10 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 4:4, typedef gpio10_rising_mask_set
//   
//  
#define GPIO_10_RISING_MASK_MASK 0x10
#define GPIO_10_RISING_MASK_OFFSET 4
#define GPIO_10_RISING_MASK_STOP_BIT 4
#define GPIO_10_RISING_MASK_START_BIT 4
#define GPIO_10_RISING_MASK_WIDTH 1


typedef enum gpio10_rising_mask_set {
    GPIO_10_RISING_MASK_RESET_SET_E,
    GPIO_10_RISING_MASK_SET_E
} GPIO_10_RISING_MASK_SET_T ;

#define GPIO_10_RISING_MASK_SET_PARAM_MASK  0x10

//  
//  Rising edge Interrupt masked on GPIO9 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 3:3, typedef gpio9_rising_mask_set
//   
//  
#define GPIO_9_RISING_MASK_MASK 0x8
#define GPIO_9_RISING_MASK_OFFSET 3
#define GPIO_9_RISING_MASK_STOP_BIT 3
#define GPIO_9_RISING_MASK_START_BIT 3
#define GPIO_9_RISING_MASK_WIDTH 1


typedef enum gpio9_rising_mask_set {
    GPIO_9_RISING_MASK_RESET_SET_E,
    GPIO_9_RISING_MASK_SET_E
} GPIO_9_RISING_MASK_SET_T ;

#define GPIO_9_RISING_MASK_SET_PARAM_MASK  0x8

//  
//  Rising edge Interrupt masked on GPIO8 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 2:2, typedef gpio8_rising_mask_set
//   
//  
#define GPIO_8_RISING_MASK_MASK 0x4
#define GPIO_8_RISING_MASK_OFFSET 2
#define GPIO_8_RISING_MASK_STOP_BIT 2
#define GPIO_8_RISING_MASK_START_BIT 2
#define GPIO_8_RISING_MASK_WIDTH 1


typedef enum gpio8_rising_mask_set {
    GPIO_8_RISING_MASK_RESET_SET_E,
    GPIO_8_RISING_MASK_SET_E
} GPIO_8_RISING_MASK_SET_T ;

#define GPIO_8_RISING_MASK_SET_PARAM_MASK  0x4

//  
//  Rising edge Interrupt masked on GPIO7 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 1:1, typedef gpio7_rising_mask_set
//   
//  
#define GPIO_7_RISING_MASK_MASK 0x2
#define GPIO_7_RISING_MASK_OFFSET 1
#define GPIO_7_RISING_MASK_STOP_BIT 1
#define GPIO_7_RISING_MASK_START_BIT 1
#define GPIO_7_RISING_MASK_WIDTH 1


typedef enum gpio7_rising_mask_set {
    GPIO_7_RISING_MASK_RESET_SET_E,
    GPIO_7_RISING_MASK_SET_E
} GPIO_7_RISING_MASK_SET_T ;

#define GPIO_7_RISING_MASK_SET_PARAM_MASK  0x2

//  
//  Rising edge Interrupt masked on GPIO6 ball.
//  Register MaskedInterrupt7 0x0E46, Bits 0:0, typedef gpio6_rising_mask_set
//   
//  
#define GPIO_6_RISING_MASK_MASK 0x1
#define GPIO_6_RISING_MASK_OFFSET 0
#define GPIO_6_RISING_MASK_STOP_BIT 0
#define GPIO_6_RISING_MASK_START_BIT 0
#define GPIO_6_RISING_MASK_WIDTH 1


typedef enum gpio6_rising_mask_set {
    GPIO_6_RISING_MASK_RESET_SET_E,
    GPIO_6_RISING_MASK_SET_E
} GPIO_6_RISING_MASK_SET_T ;

#define GPIO_6_RISING_MASK_SET_PARAM_MASK  0x1
#define MASKED_INTERRUPT_8_REG 0xE47

//  
//  Rising edge Interrupt masked on GPIO41 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 7:7, typedef gpio41_rising_mask_set
//   
//  
#define GPIO_41_RISING_MASK_MASK 0x80
#define GPIO_41_RISING_MASK_OFFSET 7
#define GPIO_41_RISING_MASK_STOP_BIT 7
#define GPIO_41_RISING_MASK_START_BIT 7
#define GPIO_41_RISING_MASK_WIDTH 1


typedef enum gpio41_rising_mask_set {
    GPIO_41_RISING_MASK_RESET_SET_E,
    GPIO_41_RISING_MASK_SET_E
} GPIO_41_RISING_MASK_SET_T ;

#define GPIO_41_RISING_MASK_SET_PARAM_MASK  0x80

//  
//  Rising edge Interrupt masked on GPIO40 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 6:6, typedef gpio40_rising_mask_set
//   
//  
#define GPIO_40_RISING_MASK_MASK 0x40
#define GPIO_40_RISING_MASK_OFFSET 6
#define GPIO_40_RISING_MASK_STOP_BIT 6
#define GPIO_40_RISING_MASK_START_BIT 6
#define GPIO_40_RISING_MASK_WIDTH 1


typedef enum gpio40_rising_mask_set {
    GPIO_40_RISING_MASK_RESET_SET_E,
    GPIO_40_RISING_MASK_SET_E
} GPIO_40_RISING_MASK_SET_T ;

#define GPIO_40_RISING_MASK_SET_PARAM_MASK  0x40

//  
//  Rising edge Interrupt masked on GPIO39 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 5:5, typedef gpio39_rising_mask_set
//   
//  
#define GPIO_39_RISING_MASK_MASK 0x20
#define GPIO_39_RISING_MASK_OFFSET 5
#define GPIO_39_RISING_MASK_STOP_BIT 5
#define GPIO_39_RISING_MASK_START_BIT 5
#define GPIO_39_RISING_MASK_WIDTH 1


typedef enum gpio39_rising_mask_set {
    GPIO_39_RISING_MASK_RESET_SET_E,
    GPIO_39_RISING_MASK_SET_E
} GPIO_39_RISING_MASK_SET_T ;

#define GPIO_39_RISING_MASK_SET_PARAM_MASK  0x20

//  
//  Rising edge Interrupt masked on GPIO38 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 4:4, typedef gpio38_rising_mask_set
//   
//  
#define GPIO_38_RISING_MASK_MASK 0x10
#define GPIO_38_RISING_MASK_OFFSET 4
#define GPIO_38_RISING_MASK_STOP_BIT 4
#define GPIO_38_RISING_MASK_START_BIT 4
#define GPIO_38_RISING_MASK_WIDTH 1


typedef enum gpio38_rising_mask_set {
    GPIO_38_RISING_MASK_RESET_SET_E,
    GPIO_38_RISING_MASK_SET_E
} GPIO_38_RISING_MASK_SET_T ;

#define GPIO_38_RISING_MASK_SET_PARAM_MASK  0x10

//  
//  Rising edge Interrupt masked on GPIO37 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 3:3, typedef gpio37_rising_mask_set
//   
//  
#define GPIO_37_RISING_MASK_MASK 0x8
#define GPIO_37_RISING_MASK_OFFSET 3
#define GPIO_37_RISING_MASK_STOP_BIT 3
#define GPIO_37_RISING_MASK_START_BIT 3
#define GPIO_37_RISING_MASK_WIDTH 1


typedef enum gpio37_rising_mask_set {
    GPIO_37_RISING_MASK_RESET_SET_E,
    GPIO_37_RISING_MASK_SET_E
} GPIO_37_RISING_MASK_SET_T ;

#define GPIO_37_RISING_MASK_SET_PARAM_MASK  0x8

//  
//  Rising edge Interrupt masked on GPIO36 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 2:2, typedef gpio36_rising_mask_set
//   
//  
#define GPIO_36_RISING_MASK_MASK 0x4
#define GPIO_36_RISING_MASK_OFFSET 2
#define GPIO_36_RISING_MASK_STOP_BIT 2
#define GPIO_36_RISING_MASK_START_BIT 2
#define GPIO_36_RISING_MASK_WIDTH 1


typedef enum gpio36_rising_mask_set {
    GPIO_36_RISING_MASK_RESET_SET_E,
    GPIO_36_RISING_MASK_SET_E
} GPIO_36_RISING_MASK_SET_T ;

#define GPIO_36_RISING_MASK_SET_PARAM_MASK  0x4

//  
//  Rising edge Interrupt masked on GPIO25 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 1:1, typedef gpio25_rising_mask_set
//   
//  
#define GPIO_25_RISING_MASK_MASK 0x2
#define GPIO_25_RISING_MASK_OFFSET 1
#define GPIO_25_RISING_MASK_STOP_BIT 1
#define GPIO_25_RISING_MASK_START_BIT 1
#define GPIO_25_RISING_MASK_WIDTH 1


typedef enum gpio25_rising_mask_set {
    GPIO_25_RISING_MASK_RESET_SET_E,
    GPIO_25_RISING_MASK_SET_E
} GPIO_25_RISING_MASK_SET_T ;

#define GPIO_25_RISING_MASK_SET_PARAM_MASK  0x2

//  
//  Rising edge Interrupt masked on GPIO24 ball.
//  Register MaskedInterrupt8 0x0E47, Bits 0:0, typedef gpio24_rising_mask_set
//   
//  
#define GPIO_24_RISING_MASK_MASK 0x1
#define GPIO_24_RISING_MASK_OFFSET 0
#define GPIO_24_RISING_MASK_STOP_BIT 0
#define GPIO_24_RISING_MASK_START_BIT 0
#define GPIO_24_RISING_MASK_WIDTH 1


typedef enum gpio24_rising_mask_set {
    GPIO_24_RISING_MASK_RESET_SET_E,
    GPIO_24_RISING_MASK_SET_E
} GPIO_24_RISING_MASK_SET_T ;

#define GPIO_24_RISING_MASK_SET_PARAM_MASK  0x1
#define MASKED_INTERRUPT_9_REG 0xE48

//  
//  Falling edge detected on GPIO13 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 7:7, typedef gpio13_falling_mask_set
//   
//  
#define GPIO_13_FALLING_MASK_MASK 0x80
#define GPIO_13_FALLING_MASK_OFFSET 7
#define GPIO_13_FALLING_MASK_STOP_BIT 7
#define GPIO_13_FALLING_MASK_START_BIT 7
#define GPIO_13_FALLING_MASK_WIDTH 1


typedef enum gpio13_falling_mask_set {
    GPIO_13_FALLING_MASK_RESET_SET_E,
    GPIO_13_FALLING_MASK_SET_E
} GPIO_13_FALLING_MASK_SET_T ;

#define GPIO_13_FALLING_MASK_SET_PARAM_MASK  0x80

//  
//  Falling edge detected on GPIO12 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 6:6, typedef gpio12_falling_mask_set
//   
//  
#define GPIO_12_FALLING_MASK_MASK 0x40
#define GPIO_12_FALLING_MASK_OFFSET 6
#define GPIO_12_FALLING_MASK_STOP_BIT 6
#define GPIO_12_FALLING_MASK_START_BIT 6
#define GPIO_12_FALLING_MASK_WIDTH 1


typedef enum gpio12_falling_mask_set {
    GPIO_12_FALLING_MASK_RESET_SET_E,
    GPIO_12_FALLING_MASK_SET_E
} GPIO_12_FALLING_MASK_SET_T ;

#define GPIO_12_FALLING_MASK_SET_PARAM_MASK  0x40

//  
//  Falling edge detected on GPIO11 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 5:5, typedef gpio11_falling_mask_set
//   
//  
#define GPIO_11_FALLING_MASK_MASK 0x20
#define GPIO_11_FALLING_MASK_OFFSET 5
#define GPIO_11_FALLING_MASK_STOP_BIT 5
#define GPIO_11_FALLING_MASK_START_BIT 5
#define GPIO_11_FALLING_MASK_WIDTH 1


typedef enum gpio11_falling_mask_set {
    GPIO_11_FALLING_MASK_RESET_SET_E,
    GPIO_11_FALLING_MASK_SET_E
} GPIO_11_FALLING_MASK_SET_T ;

#define GPIO_11_FALLING_MASK_SET_PARAM_MASK  0x20

//  
//  Falling edge detected on GPIO10 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 4:4, typedef gpio10_falling_mask_set
//   
//  
#define GPIO_10_FALLING_MASK_MASK 0x10
#define GPIO_10_FALLING_MASK_OFFSET 4
#define GPIO_10_FALLING_MASK_STOP_BIT 4
#define GPIO_10_FALLING_MASK_START_BIT 4
#define GPIO_10_FALLING_MASK_WIDTH 1


typedef enum gpio10_falling_mask_set {
    GPIO_10_FALLING_MASK_RESET_SET_E,
    GPIO_10_FALLING_MASK_SET_E
} GPIO_10_FALLING_MASK_SET_T ;

#define GPIO_10_FALLING_MASK_SET_PARAM_MASK  0x10

//  
//  Falling edge detected on GPIO9 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 3:3, typedef gpio9_falling_mask_set
//   
//  
#define GPIO_9_FALLING_MASK_MASK 0x8
#define GPIO_9_FALLING_MASK_OFFSET 3
#define GPIO_9_FALLING_MASK_STOP_BIT 3
#define GPIO_9_FALLING_MASK_START_BIT 3
#define GPIO_9_FALLING_MASK_WIDTH 1


typedef enum gpio9_falling_mask_set {
    GPIO_9_FALLING_MASK_RESET_SET_E,
    GPIO_9_FALLING_MASK_SET_E
} GPIO_9_FALLING_MASK_SET_T ;

#define GPIO_9_FALLING_MASK_SET_PARAM_MASK  0x8

//  
//  Falling edge detected on GPIO8 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 2:2, typedef gpio8_falling_mask_set
//   
//  
#define GPIO_8_FALLING_MASK_MASK 0x4
#define GPIO_8_FALLING_MASK_OFFSET 2
#define GPIO_8_FALLING_MASK_STOP_BIT 2
#define GPIO_8_FALLING_MASK_START_BIT 2
#define GPIO_8_FALLING_MASK_WIDTH 1


typedef enum gpio8_falling_mask_set {
    GPIO_8_FALLING_MASK_RESET_SET_E,
    GPIO_8_FALLING_MASK_SET_E
} GPIO_8_FALLING_MASK_SET_T ;

#define GPIO_8_FALLING_MASK_SET_PARAM_MASK  0x4

//  
//  Falling edge detected on GPIO7 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 1:1, typedef gpio7_falling_mask_set
//   
//  
#define GPIO_7_FALLING_MASK_MASK 0x2
#define GPIO_7_FALLING_MASK_OFFSET 1
#define GPIO_7_FALLING_MASK_STOP_BIT 1
#define GPIO_7_FALLING_MASK_START_BIT 1
#define GPIO_7_FALLING_MASK_WIDTH 1


typedef enum gpio7_falling_mask_set {
    GPIO_7_FALLING_MASK_RESET_SET_E,
    GPIO_7_FALLING_MASK_SET_E
} GPIO_7_FALLING_MASK_SET_T ;

#define GPIO_7_FALLING_MASK_SET_PARAM_MASK  0x2

//  
//  Falling edge detected on GPIO6 ball.
//  Register MaskedInterrupt9 0x0E48, Bits 0:0, typedef gpio6_falling_mask_set
//   
//  
#define GPIO_6_FALLING_MASK_MASK 0x1
#define GPIO_6_FALLING_MASK_OFFSET 0
#define GPIO_6_FALLING_MASK_STOP_BIT 0
#define GPIO_6_FALLING_MASK_START_BIT 0
#define GPIO_6_FALLING_MASK_WIDTH 1


typedef enum gpio6_falling_mask_set {
    GPIO_6_FALLING_MASK_RESET_SET_E,
    GPIO_6_FALLING_MASK_SET_E
} GPIO_6_FALLING_MASK_SET_T ;

#define GPIO_6_FALLING_MASK_SET_PARAM_MASK  0x1
#define MASKED_INTERRUPT_10_REG 0xE49

//  
//  Falling edge detected on GPIO41 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 7:7, typedef gpio41_falling_mask_set
//   
//  
#define GPIO_41_FALLING_MASK_MASK 0x80
#define GPIO_41_FALLING_MASK_OFFSET 7
#define GPIO_41_FALLING_MASK_STOP_BIT 7
#define GPIO_41_FALLING_MASK_START_BIT 7
#define GPIO_41_FALLING_MASK_WIDTH 1


typedef enum gpio41_falling_mask_set {
    GPIO_41_FALLING_MASK_RESET_SET_E,
    GPIO_41_FALLING_MASK_SET_E
} GPIO_41_FALLING_MASK_SET_T ;

#define GPIO_41_FALLING_MASK_SET_PARAM_MASK  0x80

//  
//  Falling edge detected on GPIO40 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 6:6, typedef gpio40_falling_mask_set
//   
//  
#define GPIO_40_FALLING_MASK_MASK 0x40
#define GPIO_40_FALLING_MASK_OFFSET 6
#define GPIO_40_FALLING_MASK_STOP_BIT 6
#define GPIO_40_FALLING_MASK_START_BIT 6
#define GPIO_40_FALLING_MASK_WIDTH 1


typedef enum gpio40_falling_mask_set {
    GPIO_40_FALLING_MASK_RESET_SET_E,
    GPIO_40_FALLING_MASK_SET_E
} GPIO_40_FALLING_MASK_SET_T ;

#define GPIO_40_FALLING_MASK_SET_PARAM_MASK  0x40

//  
//  Falling edge detected on GPIO39 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 5:5, typedef gpio39_falling_mask_set
//   
//  
#define GPIO_39_FALLING_MASK_MASK 0x20
#define GPIO_39_FALLING_MASK_OFFSET 5
#define GPIO_39_FALLING_MASK_STOP_BIT 5
#define GPIO_39_FALLING_MASK_START_BIT 5
#define GPIO_39_FALLING_MASK_WIDTH 1


typedef enum gpio39_falling_mask_set {
    GPIO_39_FALLING_MASK_RESET_SET_E,
    GPIO_39_FALLING_MASK_SET_E
} GPIO_39_FALLING_MASK_SET_T ;

#define GPIO_39_FALLING_MASK_SET_PARAM_MASK  0x20

//  
//  Falling edge detected on GPIO38 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 4:4, typedef gpio38_falling_mask_set
//   
//  
#define GPIO_38_FALLING_MASK_MASK 0x10
#define GPIO_38_FALLING_MASK_OFFSET 4
#define GPIO_38_FALLING_MASK_STOP_BIT 4
#define GPIO_38_FALLING_MASK_START_BIT 4
#define GPIO_38_FALLING_MASK_WIDTH 1


typedef enum gpio38_falling_mask_set {
    GPIO_38_FALLING_MASK_RESET_SET_E,
    GPIO_38_FALLING_MASK_SET_E
} GPIO_38_FALLING_MASK_SET_T ;

#define GPIO_38_FALLING_MASK_SET_PARAM_MASK  0x10

//  
//  Falling edge detected on GPIO37 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 3:3, typedef gpio37_falling_mask_set
//   
//  
#define GPIO_37_FALLING_MASK_MASK 0x8
#define GPIO_37_FALLING_MASK_OFFSET 3
#define GPIO_37_FALLING_MASK_STOP_BIT 3
#define GPIO_37_FALLING_MASK_START_BIT 3
#define GPIO_37_FALLING_MASK_WIDTH 1


typedef enum gpio37_falling_mask_set {
    GPIO_37_FALLING_MASK_RESET_SET_E,
    GPIO_37_FALLING_MASK_SET_E
} GPIO_37_FALLING_MASK_SET_T ;

#define GPIO_37_FALLING_MASK_SET_PARAM_MASK  0x8

//  
//  Falling edge detected on GPIO36 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 2:2, typedef gpio36_falling_mask_set
//   
//  
#define GPIO_36_FALLING_MASK_MASK 0x4
#define GPIO_36_FALLING_MASK_OFFSET 2
#define GPIO_36_FALLING_MASK_STOP_BIT 2
#define GPIO_36_FALLING_MASK_START_BIT 2
#define GPIO_36_FALLING_MASK_WIDTH 1


typedef enum gpio36_falling_mask_set {
    GPIO_36_FALLING_MASK_RESET_SET_E,
    GPIO_36_FALLING_MASK_SET_E
} GPIO_36_FALLING_MASK_SET_T ;

#define GPIO_36_FALLING_MASK_SET_PARAM_MASK  0x4

//  
//  Falling edge detected on GPIO25 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 1:1, typedef gpio25_falling_mask_set
//   
//  
#define GPIO_25_FALLING_MASK_MASK 0x2
#define GPIO_25_FALLING_MASK_OFFSET 1
#define GPIO_25_FALLING_MASK_STOP_BIT 1
#define GPIO_25_FALLING_MASK_START_BIT 1
#define GPIO_25_FALLING_MASK_WIDTH 1


typedef enum gpio25_falling_mask_set {
    GPIO_25_FALLING_MASK_RESET_SET_E,
    GPIO_25_FALLING_MASK_SET_E
} GPIO_25_FALLING_MASK_SET_T ;

#define GPIO_25_FALLING_MASK_SET_PARAM_MASK  0x2

//  
//  Falling edge detected on GPIO24 ball.
//  Register MaskedInterrupt10 0x0E49, Bits 0:0, typedef gpio24_falling_mask_set
//   
//  
#define GPIO_24_FALLING_MASK_MASK 0x1
#define GPIO_24_FALLING_MASK_OFFSET 0
#define GPIO_24_FALLING_MASK_STOP_BIT 0
#define GPIO_24_FALLING_MASK_START_BIT 0
#define GPIO_24_FALLING_MASK_WIDTH 1


typedef enum gpio24_falling_mask_set {
    GPIO_24_FALLING_MASK_RESET_SET_E,
    GPIO_24_FALLING_MASK_SET_E
} GPIO_24_FALLING_MASK_SET_T ;

#define GPIO_24_FALLING_MASK_SET_PARAM_MASK  0x1
#define MASKED_INTERRUPT_19_REG 0xE52

//  
//  0: Btemp < BtempHigh 
//  1: Btemp > BtempHigh.
//  Register MaskedInterrupt19 0x0E52, Bits 3:3, typedef 
//  battery_temperature_high_mask_set
//   
//  
#define BATTERY_TEMPERATURE_HIGH_MASK_MASK 0x8
#define BATTERY_TEMPERATURE_HIGH_MASK_OFFSET 3
#define BATTERY_TEMPERATURE_HIGH_MASK_STOP_BIT 3
#define BATTERY_TEMPERATURE_HIGH_MASK_START_BIT 3
#define BATTERY_TEMPERATURE_HIGH_MASK_WIDTH 1


typedef enum battery_temperature_high_mask_set {
    BATTERY_TEMPERATURE_LOW_E,
    BATTERY_TEMPERATURE_HIGH_E
} BATTERY_TEMPERATURE_HIGH_MASK_SET_T ;

#define BATTERY_TEMPERATURE_HIGH_MASK_SET_PARAM_MASK  0x8

//  
//  0: Btemp < BtempMedium or Btemp > BtempHigh 
//  1: BtempHigh > Btemp > BtempMedium.
//  Register MaskedInterrupt19 0x0E52, Bits 2:2, typedef 
//  battery_temperature_medium_high_mask_set
//   
//  
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_MASK 0x4
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_OFFSET 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_STOP_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_START_BIT 2
#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_WIDTH 1


typedef enum battery_temperature_medium_high_mask_set {
    BATTERY_TEMPERATURE_MEDIUM_LOW_E,
    BATTERY_TEMPERATURE_MEDIUM_HIGH_E
} BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_T ;

#define BATTERY_TEMPERATURE_MEDIUM_HIGH_MASK_SET_PARAM_MASK  0x4

//  
//  0: Btemp < BtempLow or Btemp > BtempMedium 
//  1: BtempMedium > Btemp > BtempLow.
//  Register MaskedInterrupt19 0x0E52, Bits 1:1, typedef 
//  battery_temperature_low_medium_mask_set
//   
//  
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_MASK 0x2
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_OFFSET 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_STOP_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_START_BIT 1
#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_WIDTH 1


typedef enum battery_temperature_low_medium_mask_set {
    BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_RESET_SET_E,
    BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_E
} BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_T ;

#define BATTERY_TEMPERATURE_LOW_MEDIUM_MASK_SET_PARAM_MASK  0x2

//  
//  0: Btemp > BtempLow 
//  1: BtempLow > Btemp.
//  Register MaskedInterrupt19 0x0E52, Bits 0:0, typedef 
//  battery_temperature_low_mask_set
//   
//  
#define BATTERY_TEMPERATURE_LOW_MASK_MASK 0x1
#define BATTERY_TEMPERATURE_LOW_MASK_OFFSET 0
#define BATTERY_TEMPERATURE_LOW_MASK_STOP_BIT 0
#define BATTERY_TEMPERATURE_LOW_MASK_START_BIT 0
#define BATTERY_TEMPERATURE_LOW_MASK_WIDTH 1


typedef enum battery_temperature_low_mask_set {
    BATTERY_TEMPERATURE_LOW_MASK_RESET_SET_E,
    BATTERY_TEMPERATURE_LOW_MASK_SET_E
} BATTERY_TEMPERATURE_LOW_MASK_SET_T ;

#define BATTERY_TEMPERATURE_LOW_MASK_SET_PARAM_MASK  0x1
#define MASKED_INTERRUPT_20_REG 0xE53

//  
//  Rising edge detected on DP ball comparator.
//  Register MaskedInterrupt22 0x0E53, Bits 0:0, typedef dplus_rising_mask_set
//   
//  
#define DPLUS_COMPARATOR_RISING_MASK_MASK 0x1
#define DPLUS_COMPARATOR_RISING_MASK_OFFSET 0
#define DPLUS_COMPARATOR_RISING_MASK_STOP_BIT 0
#define DPLUS_COMPARATOR_RISING_MASK_START_BIT 0
#define DPLUS_COMPARATOR_RISING_MASK_WIDTH 1

#define MASKED_INTERRUPT_22_REG 0xE53

typedef enum dplus_rising_mask_set {
    DPLUS_RISING_MASK_RESET_SET_E,
    DPLUS_RISING_MASK_SET_E
} DPLUS_RISING_MASK_SET_T ;

#define DPLUS_RISING_MASK_SET_PARAM_MASK  0x1

//  
//  USB charger plug detection.
//  Register MaskedInterrupt20 0x0E53, Bits 1:1, typedef usb_charger_plug_mask_set
//   
//  
#define USB_CHARGER_PLUG_MASK_MASK 0x2
#define USB_CHARGER_PLUG_MASK_OFFSET 1
#define USB_CHARGER_PLUG_MASK_STOP_BIT 1
#define USB_CHARGER_PLUG_MASK_START_BIT 1
#define USB_CHARGER_PLUG_MASK_WIDTH 1


typedef enum usb_charger_plug_mask_set {
    USB_CHARGER_PLUG_MASK_RESET_SET_E,
    USB_CHARGER_PLUG_MASK_SET_E
} USB_CHARGER_PLUG_MASK_SET_T ;

#define USB_CHARGER_PLUG_MASK_SET_PARAM_MASK  0x2

//  
//  Rising edge detected on WakeUp.
//  Register MaskedInterrupt20 0x0E53, Bits 2:2, typedef id_wake_up_rising_mask_set
//   
//  
#define ID_WAKE_UP_RISING_MASK_MASK 0x4
#define ID_WAKE_UP_RISING_MASK_OFFSET 2
#define ID_WAKE_UP_RISING_MASK_STOP_BIT 2
#define ID_WAKE_UP_RISING_MASK_START_BIT 2
#define ID_WAKE_UP_RISING_MASK_WIDTH 1


typedef enum id_wake_up_rising_mask_set {
    ID_WAKE_UP_RISING_MASK_RESET_SET_E,
    ID_WAKE_UP_RISING_MASK_SET_E
} ID_WAKE_UP_RISING_MASK_SET_T ;

#define ID_WAKE_UP_RISING_MASK_SET_PARAM_MASK  0x4

//  
//  ID resistance greater than 37k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 4:4, typedef 
//  id_detection_greater37k_mask_set
//   
//  
#define ID_DETECTION_GREATER_3_7K_MASK_MASK 0x10
#define ID_DETECTION_GREATER_3_7K_MASK_OFFSET 4
#define ID_DETECTION_GREATER_3_7K_MASK_STOP_BIT 4
#define ID_DETECTION_GREATER_3_7K_MASK_START_BIT 4
#define ID_DETECTION_GREATER_3_7K_MASK_WIDTH 1


typedef enum id_detection_greater37k_mask_set {
    ID_DETECTION_GREATER_3_7K_MASK_RESET_SET_E,
    ID_DETECTION_GREATER_3_7K_MASK_SET_E
} ID_DETECTION_GREATER_3_7K_MASK_SET_T ;

#define ID_DETECTION_GREATER_3_7K_MASK_SET_PARAM_MASK  0x10

//  
//  ID resistance greater than 69k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 5:5, typedef 
//  id_detection_greater69k_mask_set
//   
//  
#define ID_DETECTION_GREATER_6_9K_MASK_MASK 0x20
#define ID_DETECTION_GREATER_6_9K_MASK_OFFSET 5
#define ID_DETECTION_GREATER_6_9K_MASK_STOP_BIT 5
#define ID_DETECTION_GREATER_6_9K_MASK_START_BIT 5
#define ID_DETECTION_GREATER_6_9K_MASK_WIDTH 1


typedef enum id_detection_greater69k_mask_set {
    ID_DETECTION_GREATER_6_9K_MASK_RESET_SET_E,
    ID_DETECTION_GREATER_6_9K_MASK_SET_E
} ID_DETECTION_GREATER_6_9K_MASK_SET_T ;

#define ID_DETECTION_GREATER_6_9K_MASK_SET_PARAM_MASK  0x20

//  
//  ID resistance greater than 125k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 6:6, typedef 
//  id_detection_greater125k_mask_set
//   
//  
#define ID_DETECTION_GREATER_15_0K_MASK_MASK 0x40
#define ID_DETECTION_GREATER_15_0K_MASK_OFFSET 6
#define ID_DETECTION_GREATER_15_0K_MASK_STOP_BIT 6
#define ID_DETECTION_GREATER_15_0K_MASK_START_BIT 6
#define ID_DETECTION_GREATER_15_0K_MASK_WIDTH 1


typedef enum id_detection_greater125k_mask_set {
    ID_DETECTION_GREATER_12_5K_MASK_RESET_SET_E,
    ID_DETECTION_GREATER_12_5K_MASK_SET_E
} ID_DETECTION_GREATER_12_5K_MASK_SET_T ;

#define ID_DETECTION_GREATER_12_5K_MASK_SET_PARAM_MASK  0x40

//  
//  ID resistance greater than 220k detected.
//  Register MaskedInterrupt20 0x0E53, Bits 7:7, typedef 
//  id_detection_greater220k_mask_set
//   
//  
#define ID_DETECTION_GREATER_22_0K_MASK_MASK 0x80
#define ID_DETECTION_GREATER_22_0K_MASK_OFFSET 7
#define ID_DETECTION_GREATER_22_0K_MASK_STOP_BIT 7
#define ID_DETECTION_GREATER_22_0K_MASK_START_BIT 7
#define ID_DETECTION_GREATER_22_0K_MASK_WIDTH 1


typedef enum id_detection_greater220k_mask_set {
    ID_DETECTION_GREATER_22_0K_MASK_RESET_SET_E,
    ID_DETECTION_GREATER_22_0K_MASK_SET_E
} ID_DETECTION_GREATER_22_0K_MASK_SET_T ;

#define ID_DETECTION_GREATER_22_0K_MASK_SET_PARAM_MASK  0x80
#define MASKED_INTERRUPT_21_REG 0xE54

//  
//  Falling edge detected on WakeU.
//  Register MaskedInterrupt21 0x0E54, Bits 0:0, typedef id_wake_up_falling_mask_set
//   
//  
#define ID_WAKE_UP_FALLING_MASK_MASK 0x1
#define ID_WAKE_UP_FALLING_MASK_OFFSET 0
#define ID_WAKE_UP_FALLING_MASK_STOP_BIT 0
#define ID_WAKE_UP_FALLING_MASK_START_BIT 0
#define ID_WAKE_UP_FALLING_MASK_WIDTH 1


typedef enum id_wake_up_falling_mask_set {
    ID_WAKE_UP_FALLING_MASK_RESET_SET_E,
    ID_WAKE_UP_FALLING_MASK_SET_E
} ID_WAKE_UP_FALLING_MASK_SET_T ;

#define ID_WAKE_UP_FALLING_MASK_SET_PARAM_MASK  0x1

//  
//  ID resistance lower than 37k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 2:2, typedef id_detection37k_mask_set
//   
//  
#define ID_DETECTION_3_7K_MASK_MASK 0x4
#define ID_DETECTION_3_7K_MASK_OFFSET 2
#define ID_DETECTION_3_7K_MASK_STOP_BIT 2
#define ID_DETECTION_3_7K_MASK_START_BIT 2
#define ID_DETECTION_3_7K_MASK_WIDTH 1


typedef enum id_detection37k_mask_set {
    ID_DETECTION_3_7K_MASK_RESET_SET_E,
    ID_DETECTION_3_7K_MASK_SET_E
} ID_DETECTION_3_7K_MASK_SET_T ;

#define ID_DETECTION_3_7K_MASK_SET_PARAM_MASK  0x4

//  
//  ID resistance lower than 69k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 3:3, typedef id_detection69k_mask_set
//   
//  
#define ID_DETECTION_6_9K_MASK_MASK 0x8
#define ID_DETECTION_6_9K_MASK_OFFSET 3
#define ID_DETECTION_6_9K_MASK_STOP_BIT 3
#define ID_DETECTION_6_9K_MASK_START_BIT 3
#define ID_DETECTION_6_9K_MASK_WIDTH 1


typedef enum id_detection69k_mask_set {
    ID_DETECTION_6_9K_MASK_RESET_SET_E,
    ID_DETECTION_6_9K_MASK_SET_E
} ID_DETECTION_6_9K_MASK_SET_T ;

#define ID_DETECTION_6_9K_MASK_SET_PARAM_MASK  0x8

//  
//  ID resistance lower than 125k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 4:4, typedef id_detection125k_mask_set
//   
//  
#define ID_DETECTION_15_0K_MASK_MASK 0x10
#define ID_DETECTION_15_0K_MASK_OFFSET 4
#define ID_DETECTION_15_0K_MASK_STOP_BIT 4
#define ID_DETECTION_15_0K_MASK_START_BIT 4
#define ID_DETECTION_15_0K_MASK_WIDTH 1


typedef enum id_detection125k_mask_set {
    ID_DETECTION_12_5K_MASK_RESET_SET_E,
    ID_DETECTION_12_5K_MASK_SET_E
} ID_DETECTION_12_5K_MASK_SET_T ;

#define ID_DETECTION_12_5K_MASK_SET_PARAM_MASK  0x10

//  
//  ID resistance lower than 220k detected.
//  Register MaskedInterrupt21 0x0E54, Bits 5:5, typedef id_detection220k_mask_set
//   
//  
#define ID_DETECTION_22_0K_MASK_MASK 0x20
#define ID_DETECTION_22_0K_MASK_OFFSET 5
#define ID_DETECTION_22_0K_MASK_STOP_BIT 5
#define ID_DETECTION_22_0K_MASK_START_BIT 5
#define ID_DETECTION_22_0K_MASK_WIDTH 1


typedef enum id_detection220k_mask_set {
    ID_DETECTION_22_0K_MASK_RESET_SET_E,
    ID_DETECTION_22_0K_MASK_SET_E
} ID_DETECTION_22_0K_MASK_SET_T ;

#define ID_DETECTION_22_0K_MASK_SET_PARAM_MASK  0x20

//  
//  USB charger detected.
//  Register MaskedInterrupt21 0x0E54, Bits 6:6, typedef usb_charger_mask_set
//   
//  
#define USB_CHARGER_MASK_MASK 0x40
#define USB_CHARGER_MASK_OFFSET 6
#define USB_CHARGER_MASK_STOP_BIT 6
#define USB_CHARGER_MASK_START_BIT 6
#define USB_CHARGER_MASK_WIDTH 1


typedef enum usb_charger_mask_set {
    USB_CHARGER_MASK_RESET_SET_E,
    USB_CHARGER_MASK_SET_E
} USB_CHARGER_MASK_SET_T ;

#define USB_CHARGER_MASK_SET_PARAM_MASK  0x40

//  
//  USB charger unplug detected.
//  Register MaskedInterrupt22 0x0E55, Bits 7:7, typedef usb_chargerunplug_mask_set
//   
//  
#define USB_CHARGER_UNPLUG_MASK 0x80
#define USB_CHARGER_UNPLUG_OFFSET 7
#define USB_CHARGER_UNPLUG_STOP_BIT 7
#define USB_CHARGER_UNPLUG_START_BIT 7
#define USB_CHARGER_UNPLUG_WIDTH 1


typedef enum usb_chargerunplug_mask_set {
    USB_CHARGERUNPLUG_MASK_RESET_SET_E,
    USB_CHARGERUNPLUG_MASK_SET_E
} USB_CHARGERUNPLUG_MASK_SET_T ;

#define USB_CHARGERUNPLUG_MASK_SET_PARAM_MASK  0x80

//  
//  0: Die temperature is above than main charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 3:3, typedef 
//  main_charger_thermal_above_mask_set
//   
//  
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_MASK 0x8
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_OFFSET 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_STOP_BIT 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_START_BIT 3
#define MAIN_CHARGER_THERMAL_ABOVE_MASK_WIDTH 1


typedef enum main_charger_thermal_above_mask_set {
    MAIN_CHARGER_THERMAL_ABOVE_MASK_RESET_SET_E,
    MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_E
} MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_T ;

#define MAIN_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK  0x8

//  
//  0: Die temperature is below than main charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than main charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 2:2, typedef 
//  main_charger_thermal_below_mask_set
//   
//  
#define MAIN_CHARGER_THERMAL_BELOW_MASK_MASK 0x4
#define MAIN_CHARGER_THERMAL_BELOW_MASK_OFFSET 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_STOP_BIT 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_START_BIT 2
#define MAIN_CHARGER_THERMAL_BELOW_MASK_WIDTH 1


typedef enum main_charger_thermal_below_mask_set {
    MAIN_CHARGER_THERMAL_BELOW_MASK_RESET_SET_E,
    MAIN_CHARGER_THERMAL_BELOW_MASK_SET_E
} MAIN_CHARGER_THERMAL_BELOW_MASK_SET_T ;

#define MAIN_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK  0x4

//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 1:1, typedef 
//  usb_charger_thermal_above_mask_set
//   
//  
#define USB_CHARGER_THERMAL_ABOVE_MASK_MASK 0x2
#define USB_CHARGER_THERMAL_ABOVE_MASK_OFFSET 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_STOP_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_START_BIT 1
#define USB_CHARGER_THERMAL_ABOVE_MASK_WIDTH 1


typedef enum usb_charger_thermal_above_mask_set {
    USB_CHARGER_THERMAL_ABOVE_MASK_RESET_SET_E,
    USB_CHARGER_THERMAL_ABOVE_MASK_SET_E
} USB_CHARGER_THERMAL_ABOVE_MASK_SET_T ;

#define USB_CHARGER_THERMAL_ABOVE_MASK_SET_PARAM_MASK  0x2

//  
//  0: Die temperature is above than usb charger thermal protection threshold (charge 
//  stopped) 
//  1: Die temperature is upper than usb charger thermal protection threshold (charge 
//  stopped).
//  Register MaskedInterrupt22 0x0E55, Bits 0:0, typedef 
//  usb_charger_thermal_below_mask_set
//   
//  
#define USB_CHARGER_THERMAL_BELOW_MASK_MASK 0x1
#define USB_CHARGER_THERMAL_BELOW_MASK_OFFSET 0
#define USB_CHARGER_THERMAL_BELOW_MASK_STOP_BIT 0
#define USB_CHARGER_THERMAL_BELOW_MASK_START_BIT 0
#define USB_CHARGER_THERMAL_BELOW_MASK_WIDTH 1


typedef enum usb_charger_thermal_below_mask_set {
    USB_CHARGER_THERMAL_BELOW_MASK_RESET_SET_E,
    USB_CHARGER_THERMAL_BELOW_MASK_SET_E
} USB_CHARGER_THERMAL_BELOW_MASK_SET_T ;

#define USB_CHARGER_THERMAL_BELOW_MASK_SET_PARAM_MASK  0x1
#endif
