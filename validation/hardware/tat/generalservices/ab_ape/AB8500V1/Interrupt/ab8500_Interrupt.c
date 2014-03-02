/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Interrupt/Linux/ab8500_Interrupt.c
 * 
 *
 * Generated on the 23/09/2009 15:21 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_Interrupt.h"

extern unsigned char SPIRead( unsigned short  register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_interrupt_source1
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source1()
  {
    unsigned char value;


    /* 
     * IntSource1 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_1_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_allowed
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 0:0
 *
 **************************************************************************/
unsigned char get_main_charger_allowed()
  {
    unsigned char value;


    /* 
     * Main charger connected is not an allowed one 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_1_REG) & MAIN_CHARGER_ALLOWED_INTERRUPT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_plug_tv_interrupt
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 2:2
 *
 **************************************************************************/
unsigned char get_plug_tv_interrupt()
  {
    unsigned char value;


    /* 
     * Tvset (75 Ohms) connection/disconnection on CVBS ball 
     * detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_1_REG) & PLUG_TV_INTERRUPT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_thermal_warning_interrupt
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 3:3
 *
 **************************************************************************/
unsigned char get_thermal_warning_interrupt()
  {
    unsigned char value;


    /* 
     * Thermal warning 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_1_REG) & THERMAL_WARNING_INTERRUPT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_level_change_pon_key2_interrupt
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 5:5
 *
 **************************************************************************/
unsigned char get_level_change_pon_key2_interrupt()
  {
    unsigned char value;


    /* 
     * LevelChange on PonKey2 ball detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_1_REG) & RISING_PON_KEY_2_INTERRUPT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_level_change_pon_key1_interrupt
 *
 * RET  : Return the value of register InterruptSource1
 *
 * Notes : From register 0x0E00, bits 7:7
 *
 **************************************************************************/
unsigned char get_level_change_pon_key1_interrupt()
  {
    unsigned char value;


    /* 
     * LevelChange on PonKey1 ball detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_1_REG) & RISING_PON_KEY_1_INTERRUPT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source2
 *
 * RET  : Return the value of register InterruptSource2
 *
 * Notes : From register 0x0E01, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source2()
  {
    unsigned char value;


    /* 
     * IntSource2 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_2_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_over_voltage_interrupt
 *
 * RET  : Return the value of register InterruptSource2
 *
 * Notes : From register 0x0E01, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_over_voltage_interrupt()
  {
    unsigned char value;


    /* 
     * VbatA ball goes upper over voltage threshold (charge 
     * stopped) 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_2_REG) & BATTERY_OVER_VOLTAGE_INTERRUPT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_plug_interrupt
 *
 * RET  : Return the value of register InterruptSource2
 *
 * Notes : From register 0x0E01, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_charger_plug_interrupt()
  {
    unsigned char value;


    /* 
     * Main charger plug/unplug detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_2_REG) & MAIN_CHARGER_PLUG_INTERRUPT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_id_level_interrupt
 *
 * RET  : Return the value of register InterruptSource2
 *
 * Notes : From register 0x0E01, bits 5:5
 *
 **************************************************************************/
unsigned char get_usb_id_level_interrupt()
  {
    unsigned char value;


    /* 
     * ID ball level change detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_2_REG) & USB_ID_LEVEL_INTERRUPT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_vbus_level_interrupt
 *
 * RET  : Return the value of register InterruptSource2
 *
 * Notes : From register 0x0E01, bits 7:7
 *
 **************************************************************************/
unsigned char get_vbus_level_interrupt()
  {
    unsigned char value;


    /* 
     * Vbus ball level change detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_2_REG) & VBUS_EDGE_INTERRUPT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source3
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source3()
  {
    unsigned char value;


    /* 
     * IntSource3 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_3_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_dropout
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_charger_dropout()
  {
    unsigned char value;


    /* 
     * Usb charger was in dropout and internal input current 
     * loop update current regulation to avoid usb charger to 
     * drop 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & USB_DROP_OUT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc60s
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 1:1
 *
 **************************************************************************/
unsigned char get_rtc60s()
  {
    unsigned char value;


    /* 
     * RTC timer reaches a 60s period 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & PERIOD_6_0S_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc_alarm
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 2:2
 *
 **************************************************************************/
unsigned char get_rtc_alarm()
  {
    unsigned char value;


    /* 
     * RTC timer reaches alarm time 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & REACH_ALARM_TIME_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_removal_interrupt
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 4:4
 *
 **************************************************************************/
unsigned char get_battery_removal_interrupt()
  {
    unsigned char value;


    /* 
     * Battery removal 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & BATTERY_REMOVAL_INTERRUPT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_watchdog_charger_expiration_interrupt
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 5:5
 *
 **************************************************************************/
unsigned char get_watchdog_charger_expiration_interrupt()
  {
    unsigned char value;


    /* 
     * Watchdog charger expiration detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & WATCHDOG_CHARGER_EXPIRATION_INTERRUPT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_overvoltage_on_vbus_interrupt
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 6:6
 *
 **************************************************************************/
unsigned char get_overvoltage_on_vbus_interrupt()
  {
    unsigned char value;


    /* 
     * Overvoltage on Vbus ball detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & OVERVOLTAGE_ON_VBUS_INTERRUPT_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_over_current_interrupt
 *
 * RET  : Return the value of register InterruptSource3
 *
 * Notes : From register 0x0E02, bits 7:7
 *
 **************************************************************************/
unsigned char get_main_charger_over_current_interrupt()
  {
    unsigned char value;


    /* 
     * Icharge > Icharge max programmed detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_3_REG) & MAIN_CHARGER_OVER_CURRENT_INTERRUPT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source4
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source4()
  {
    unsigned char value;


    /* 
     * IntSource4 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_4_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_accumulation_sample_conversion
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 0:0
 *
 **************************************************************************/
unsigned char get_accumulation_sample_conversion()
  {
    unsigned char value;


    /* 
     * Accumulation of N sample conversion is reached 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & ACCUMULATION_SAMPLE_CONVERSION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_audio_interrupt
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_interrupt()
  {
    unsigned char value;


    /* 
     * Audio interrupt form audio digital part detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & AUDIO_INTERRUPT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_ended_interrupt
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 2:2
 *
 **************************************************************************/
unsigned char get_coulomb_ended_interrupt()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended data convertion 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & COULOMB_COUNTER_ENDED_INTERRUPT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_coulomb_calibration_endedd_interrupt
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 3:3
 *
 **************************************************************************/
unsigned char get_coulomb_calibration_endedd_interrupt()
  {
    unsigned char value;


    /* 
     * Coulomb Counter has ended its calibration 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & COULOMB_CALIBRATION_ENDEDD_INTERRUPT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_high_battery_interrupt
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 5:5
 *
 **************************************************************************/
unsigned char get_high_battery_interrupt()
  {
    unsigned char value;


    /* 
     * VbatA voltage reaches LowBat register programmed 
     * threshold 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & HIGH_BATTERY_INTERRUPT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_rtc_battery_high_interrupt
 *
 * RET  : Return the value of register InterruptSource4
 *
 * Notes : From register 0x0E03, bits 7:7
 *
 **************************************************************************/
unsigned char get_rtc_battery_high_interrupt()
  {
    unsigned char value;


    /* 
     * BackUpBat ball voltage goes above RtcBackupChg register 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_4_REG) & RTC_BATTERY_HIGH_INTERRUPT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source5
 *
 * RET  : Return the value of register InterruptSource5
 *
 * Notes : From register 0x0E04, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source5()
  {
    unsigned char value;


    /* 
     * IntSource5 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_5_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_accessory1_low_voltage
 *
 * RET  : Return the value of register InterruptSource5
 *
 * Notes : From register 0x0E04, bits 1:1
 *
 **************************************************************************/
unsigned char get_accessory1_low_voltage()
  {
    unsigned char value;


    /* 
     * AccDetect1 ball voltage level goes below AccDetect1Th 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_5_REG) & ACCESSORY_1_LOW_VOLTAGE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_accessory22_high_voltage
 *
 * RET  : Return the value of register InterruptSource5
 *
 * Notes : From register 0x0E04, bits 4:4
 *
 **************************************************************************/
unsigned char get_accessory22_high_voltage()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level reaches AccDetect22Th 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_5_REG) & ACCESSORY_22_HIGH_VOLTAGE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_accessory21_high_voltage
 *
 * RET  : Return the value of register InterruptSource5
 *
 * Notes : From register 0x0E04, bits 6:6
 *
 **************************************************************************/
unsigned char get_accessory21_high_voltage()
  {
    unsigned char value;


    /* 
     * AccDetect2 ball voltage level reaches AccDetect21Th 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_5_REG) & ACCESSORY_21_HIGH_VOLTAGE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_adc_data_available
 *
 * RET  : Return the value of register InterruptSource5
 *
 * Notes : From register 0x0E04, bits 7:7
 *
 **************************************************************************/
unsigned char get_adc_data_available()
  {
    unsigned char value;


    /* 
     * ADC DAta Available 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_5_REG) & ADC_DATA_AVAILABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source7
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source7()
  {
    unsigned char value;


    /* 
     * IntSource7 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_7_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio13_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO13 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_13_RISING_EVENT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio12_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO12 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_12_RISING_EVENT_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio11_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO11 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_11_RISING_EVENT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio10_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO10 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_10_RISING_EVENT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio9_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO9 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_9_RISING_EVENT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio8_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO8 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_8_RISING_EVENT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio7_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO7 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_7_RISING_EVENT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E06, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio6_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO6 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_6_RISING_EVENT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source8
 *
 * RET  : Return the value of register InterruptSource8
 *
 * Notes : From register 0x0E07, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source8()
  {
    unsigned char value;


    /* 
     * IntSource8 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_8_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio41_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO41 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_41_RISING_EVENT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio40_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO40 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_40_RISING_EVENT_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio39_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO39 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_39_RISING_EVENT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio38_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO38 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_38_RISING_EVENT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio37_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO37 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_37_RISING_EVENT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio36_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO36 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_36_RISING_EVENT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio25_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO25 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_25_RISING_EVENT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_rising
 *
 * RET  : Return the value of register InterruptSource7
 *
 * Notes : From register 0x0E07, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio24_rising()
  {
    unsigned char value;


    /* 
     * Edge detected on GPIO24 ball 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_7_REG) & GPIO_24_RISING_EVENT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source19
 *
 * RET  : Return the value of register InterruptSource19
 *
 * Notes : From register 0x0E12, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source19()
  {
    unsigned char value;


    /* 
     * IntSource19 Status 
     */ 
    value = SPIRead(INTERRUPT_SOURCE_19_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_high
 *
 * RET  : Return the value of register InterruptSource19
 *
 * Notes : From register 0x0E12, bits 3:3
 *
 **************************************************************************/
unsigned char get_battery_temperature_high()
  {
    unsigned char value;


    /* 
     * 0: Btemp < BtempHigh 
     * 1: Btemp > BtempHigh 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_19_REG) & BATTERY_TEMPERATURE_HIGH_INTERRUPT_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_medium_high
 *
 * RET  : Return the value of register InterruptSource19
 *
 * Notes : From register 0x0E12, bits 2:2
 *
 **************************************************************************/
unsigned char get_battery_temperature_medium_high()
  {
    unsigned char value;


    /* 
     * 0: Btemp < BtempMedium or Btemp > BtempHigh 
     * 1: BtempHigh > Btemp > BtempMedium 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_19_REG) & BATTERY_TEMPERATURE_MEDIUM_HIGH_INTERRUPT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low_medium
 *
 * RET  : Return the value of register InterruptSource19
 *
 * Notes : From register 0x0E12, bits 1:1
 *
 **************************************************************************/
unsigned char get_battery_temperature_low_medium()
  {
    unsigned char value;


    /* 
     * 0: Btemp < BtempLow or Btemp > BtempMedium 
     * 1: BtempMedium > Btemp > BtempLow 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_19_REG) & BATTERY_TEMPERATURE_LOW_MEDIUM_INTERRUPT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_battery_temperature_low
 *
 * RET  : Return the value of register InterruptSource19
 *
 * Notes : From register 0x0E12, bits 0:0
 *
 **************************************************************************/
unsigned char get_battery_temperature_low()
  {
    unsigned char value;


    /* 
     * 0: Btemp > BtempLow 
     * 1: BtempLow > Btemp 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_19_REG) & BATTERY_TEMPERATURE_LOW_INTERRUPT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source20
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source20()
  {
    unsigned char value;


    /* 
     * IntSource20 Status 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & INTERRUPT_SOURCE_20_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_bad_charger_plug
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_bad_charger_plug()
  {
    unsigned char value;


    /* 
     * Not allowed USB charger detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & USB_BAD_CHARGER_PLUG_EVENT_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_id_wake_up_rising
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_wake_up_rising()
  {
    unsigned char value;


    /* 
     * Rising edge detected on WakeUp 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & ID_WAKE_UP_RISING_EVENT_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_r1
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_detection_r1()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 37k detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & ID_DETECTION_GREATER_8_0K_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_r2
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 5:5
 *
 **************************************************************************/
unsigned char get_id_detection_r2()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 69k detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & ID_DETECTION_GREATER_14_0K_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_r3
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 6:6
 *
 **************************************************************************/
unsigned char get_id_detection_r3()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 125k detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & ID_DETECTION_GREATER_22_0K_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_id_detection_r4
 *
 * RET  : Return the value of register InterruptSource20
 *
 * Notes : From register 0x0E13, bits 7:7
 *
 **************************************************************************/
unsigned char get_id_detection_r4()
  {
    unsigned char value;


    /* 
     * ID resistance greater than 220k detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_20_REG) & ID_DETECTION_GREATER_33_0K_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source21
 *
 * RET  : Return the value of register InterruptSource21
 *
 * Notes : From register 0x0E14, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source21()
  {
    unsigned char value;


    /* 
     * IntSource21 Status 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_21_REG) & INTERRUPT_SOURCE_21_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_plug
 *
 * RET  : Return the value of register InterruptSource21
 *
 * Notes : From register 0x0E14, bits 6:6
 *
 **************************************************************************/
unsigned char get_usb_charger_plug()
  {
    unsigned char value;


    /* 
     * USB charger detected 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_21_REG) & USB_CHARGER_PLUG_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_source22
 *
 * RET  : Return the value of register InterruptSource22
 *
 * Notes : From register 0x0E15, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_source22()
  {
    unsigned char value;


    /* 
     * IntSource22 Status 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_22_REG) & INTERRUPT_SOURCE_22_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_main_charger_thermal_protection
 *
 * RET  : Return the value of register InterruptSource22
 *
 * Notes : From register 0x0E15, bits 3:3
 *
 **************************************************************************/
unsigned char get_main_charger_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is not upper than main charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than main charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_22_REG) & MAIN_CHARGER_THERMAL_PROTECTION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_thermal_protection
 *
 * RET  : Return the value of register InterruptSource22
 *
 * Notes : From register 0x0E15, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_charger_thermal_protection()
  {
    unsigned char value;


    /* 
     * 0: Die temperature is not upper than usb charger thermal 
     * protection threshold (charge stopped) 
     * 1: Die temperature is upper than usb charger thermal 
     * protection threshold (charge stopped) 
     */ 
    value = (SPIRead(INTERRUPT_SOURCE_22_REG) & USB_CHARGER_THERMAL_PROTECTION_PARAM_MASK) >> 1;
    return value;
  }


