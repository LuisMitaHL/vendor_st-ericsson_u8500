/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Interrupt/Linux/ab9540_Interrupt.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_H
#define C__LISP_PRODUCTION_AB9540_AB9540_INTERRUPT_LINUX_AB9540_INTERRUPT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Interrupt_types.h"

unsigned char get_gpio6_rising();
unsigned char get_gpio7_rising();
unsigned char get_gpio8_rising();
unsigned char get_gpio9_rising();
unsigned char get_gpio10_rising();
unsigned char get_gpio11_rising();
unsigned char get_gpio12_rising();
unsigned char get_gpio13_rising();
unsigned char get_gpio24_rising();
unsigned char get_gpio25_rising();
unsigned char get_gpio36_rising();
unsigned char get_gpio37_rising();
unsigned char get_gpio38_rising();
unsigned char get_gpio39_rising();
unsigned char get_gpio40_rising();
unsigned char get_gpio41_rising();
unsigned char get_adp_source_error();
unsigned char get_adp_sink_error();
unsigned char get_adp_probe_plug();
unsigned char get_adp_probe_un_plug();
unsigned char get_adp_sense_off();
unsigned char get_usb_link_status_changed();
unsigned char get_kp_vdddig();
unsigned char get_gpio50_high_level();
unsigned char get_gpio51_high_level();
unsigned char get_gpio52_high_level();
unsigned char get_gpio53_high_level();
unsigned char get_gpio60_high_level();
unsigned char get_high_level_external_charger();
unsigned char get_battery_temperature_low();
unsigned char get_battery_temperature_low_medium();
unsigned char get_battery_temperature_medium_high();
unsigned char get_battery_temperature_high();
unsigned char get_level_change_pon_key1_interrupt();
unsigned char get_level_change_pon_key2_interrupt();
unsigned char get_thermal_warning_interrupt();
unsigned char get_usb_charger_thermal_protection();
unsigned char get_main_charger_thermal_protection();
unsigned char get_charging_current_limited_hs_chirp();
unsigned char get_xtal32_ok();
unsigned char get_id_detection_r4();
unsigned char get_id_detection_r3();
unsigned char get_id_detection_r2();
unsigned char get_id_detection_r1();
unsigned char get_id_wake_up_rising();
unsigned char get_usb_bad_charger_plug();
unsigned char get_srp_p_detect();
unsigned char get_usb_charger_plug();
unsigned char get_key_stuck();
unsigned char get_lpr_vdddig();
unsigned char get_l_kp_vdddig();
unsigned char get_key_deglitch();
unsigned char get_modem_power_status();
unsigned char get_vbus_level_interrupt();
unsigned char get_main_charger_plug_interrupt();
unsigned char get_battery_over_voltage_interrupt();
unsigned char get_main_charger_over_current_interrupt();
unsigned char get_overvoltage_on_vbus_interrupt();
unsigned char get_watchdog_charger_expiration_interrupt();
unsigned char get_battery_removal_interrupt();
unsigned char get_rtc_alarm();
unsigned char get_rtc60s();
unsigned char get_usb_charger_dropout();
unsigned char get_rtc_battery_high_interrupt();
unsigned char get_high_battery_interrupt();
unsigned char get_coulomb_calibration_endedd_interrupt();
unsigned char get_coulomb_ended_interrupt();
unsigned char get_audio_interrupt();
unsigned char get_accumulation_sample_conversion();
unsigned char get_adc_data_hardware_int();
unsigned char get_accessory21_high_voltage();
unsigned char get_accessory22_high_voltage();
unsigned char get_accessory1_low_voltage();
unsigned char get_adc_data_software_int();


#ifdef __cplusplus
}
#endif

#endif
