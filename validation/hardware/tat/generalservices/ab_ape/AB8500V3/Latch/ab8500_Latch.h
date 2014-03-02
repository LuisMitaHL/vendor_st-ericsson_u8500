/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Latch/Linux/ab8500_Latch.h
 * 
 *
 * Generated on the 25/05/2011 17:59 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : STw4500 Registers Specification Rev 2.61 5 January 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_LATCH_LINUX_AB8500_LATCH_H
#define C__LISP_PRODUCTION_AB8500_AB8500_LATCH_LINUX_AB8500_LATCH_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Latch_types.h"

unsigned char get_latched_interrupt1();
unsigned char get_main_charger_allowed_latched_interrupt();
unsigned char get_un_plug_tv_latched_interrupt();
unsigned char get_plug_tv_latched_interrupt();
unsigned char get_thermal_warning_latched_interrupt();
unsigned char get_falling_pon_key2_latched_interrupt();
unsigned char get_rising_pon_key2_latched_interrupt();
unsigned char get_falling_pon_key1_latched_interrupt();
unsigned char get_rising_pon_key1_latched_interrupt();
unsigned char get_latched_interrupt2();
unsigned char get_battery_over_voltage_latch();
unsigned char get_main_charger_un_plug_latch();
unsigned char get_main_charger_plug_latch();
unsigned char get_vbus_falling_edge_latch();
unsigned char get_vbus_rising_edge_latch();
unsigned char get_latched_interrupt3();
unsigned char get_usb_charger_dropout_latch();
unsigned char get_rtc60s_latch();
unsigned char get_rtc_alarm_latch();
unsigned char get_battery_removal_latch();
unsigned char get_watchdog_charger_expiration_latch();
unsigned char get_overvoltage_on_vbus_latch();
unsigned char get_main_charger_over_current_latch();
unsigned char get_latched_interrupt4();
unsigned char get_accumulation_conversion_latch();
unsigned char get_audio_latch();
unsigned char get_coulomb_ended_latch();
unsigned char get_coulomb_calibration_endedd_latch();
unsigned char get_low_battery_latch();
unsigned char get_high_battery_latch();
unsigned char get_rtc_battery_low_latch();
unsigned char get_rtc_battery_high_latch();
unsigned char get_latched_interrupt5();
unsigned char get_adc_software_control_latch();
unsigned char get_accessory1_low_voltage_latch();
unsigned char get_accessory1_high_voltage_latch();
unsigned char get_accessory22_low_voltage_latch();
unsigned char get_accessory22_high_voltage_latch();
unsigned char get_accessory21_low_voltage_latch();
unsigned char get_accessory21_high_voltage_latch();
unsigned char get_adc_hardware_control_latch();
unsigned char get_latched_interrupt7();
unsigned char get_gpio13_rising_latch();
unsigned char get_gpio12_rising_latch();
unsigned char get_gpio11_rising_latch();
unsigned char get_gpio10_rising_latch();
unsigned char get_gpio9_rising_latch();
unsigned char get_gpio8_rising_latch();
unsigned char get_gpio7_rising_latch();
unsigned char get_gpio6_rising_latch();
unsigned char get_latched_interrupt8();
unsigned char get_gpio41_rising_latch();
unsigned char get_gpio40_rising_latch();
unsigned char get_gpio39_rising_latch();
unsigned char get_gpio38_rising_latch();
unsigned char get_gpio37_rising_latch();
unsigned char get_gpio36_rising_latch();
unsigned char get_gpio25_rising_latch();
unsigned char get_gpio24_rising_latch();
unsigned char get_latched_interrupt9();
unsigned char get_gpio13_falling_latch();
unsigned char get_gpio12_falling_latch();
unsigned char get_gpio11_falling_latch();
unsigned char get_gpio10_falling_latch();
unsigned char get_gpio9_falling_latch();
unsigned char get_gpio8_falling_latch();
unsigned char get_gpio7_falling_latch();
unsigned char get_gpio6_falling_latch();
unsigned char get_latched_interrupt10();
unsigned char get_gpio41_falling_latch();
unsigned char get_gpio40_falling_latch();
unsigned char get_gpio39_falling_latch();
unsigned char get_gpio38_falling_latch();
unsigned char get_gpio37_falling_latch();
unsigned char get_gpio36_falling_latch();
unsigned char get_gpio25_falling_latch();
unsigned char get_gpio24_falling_latch();
unsigned char get_latched_interrupt12();
unsigned char get_usb_link_status_latch();
unsigned char get_usb_phy_power_error();
unsigned char get_adp_sense_off_latch();
unsigned char get_adp_probe_un_plug_latch();
unsigned char get_adp_probe_plug_latch();
unsigned char get_adp_sink_error_latch();
unsigned char get_adp_source_error_latch();
unsigned char get_latched_source19();
unsigned char get_battery_temperature_high_latch();
unsigned char get_battery_temperature_medium_high_latch();
unsigned char get_battery_temperature_low_medium_latch();
unsigned char get_battery_temperature_low_latch();
unsigned char get_latched_interrupt20();
unsigned char get_srpd_dected();
unsigned char get_usb_charger_plug_latch();
unsigned char get_id_up_rising_event_latch();
unsigned char get_id_detection_greater37k_latch();
unsigned char get_id_detection_greater69k_latch();
unsigned char get_id_detection_greater125k_latch();
unsigned char get_id_detection_greater220k_latch();
unsigned char get_latched_interrupt21();
unsigned char get_id_wake_up_falling_latch();
unsigned char get_id_falling_event_latch();
unsigned char get_id_detection37k_latch();
unsigned char get_id_detection69k_latch();
unsigned char get_id_detection125k_latch();
unsigned char get_id_detection220k_latch();
unsigned char get_latched_interrupt22();
unsigned char get_charging_current_limited_h_s_chirp_latch();
unsigned char get_charging_current_no_more_limited_latch();
unsigned char get_main_charger_thermal_above_latch();
unsigned char get_main_charger_thermal_below_latch();
unsigned char get_usb_charger_thermal_above_latch();
unsigned char get_usb_charger_thermal_below_latch();


#ifdef __cplusplus
}
#endif

#endif
