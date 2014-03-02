/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_MaskInterrupt/Linux/ab9540_MaskInterrupt.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_H
#define C__LISP_PRODUCTION_AB9540_AB9540_MASKINTERRUPT_LINUX_AB9540_MASKINTERRUPT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_MaskInterrupt_types.h"

unsigned char set_battery_temperature_low_mask( enum battery_temperature_low_mask_set param );
unsigned char get_battery_temperature_low_mask();
unsigned char set_battery_temperature_low_medium_mask( enum battery_temperature_low_medium_mask_set param );
unsigned char get_battery_temperature_low_medium_mask();
unsigned char set_battery_temperature_medium_high_mask( enum battery_temperature_medium_high_mask_set param );
unsigned char get_battery_temperature_medium_high_mask();
unsigned char set_battery_temperature_high_mask( enum battery_temperature_high_mask_set param );
unsigned char get_battery_temperature_high_mask();
unsigned char get_gpio50_falling_mask();
unsigned char get_gpio51_falling_mask();
unsigned char get_gpio52_falling_mask();
unsigned char get_gpio53_falling_mask();
unsigned char get_gpio60_falling_mask();
unsigned char get_external_charger_falling_mask();
unsigned char get_gpio50_rising_mask();
unsigned char get_gpio51_rising_mask();
unsigned char get_gpio52_rising_mask();
unsigned char get_gpio53_rising_mask();
unsigned char get_gpio60_rising_mask();
unsigned char get_external_charger_rising_mask();
unsigned char get_adp_source_error_mask();
unsigned char get_adp_sink_error_mask();
unsigned char get_adp_probe_plug_mask();
unsigned char get_adp_probe_un_plug_mask();
unsigned char get_adp_sense_off_mask();
unsigned char get_usb_phy_error_mask();
unsigned char get_usb_link_status_mask();
unsigned char set_gpio24_falling_mask( enum gpio24_falling_mask_set param );
unsigned char get_gpio24_falling_mask();
unsigned char set_gpio25_falling_mask( enum gpio25_falling_mask_set param );
unsigned char get_gpio25_falling_mask();
unsigned char set_gpio36_falling_mask( enum gpio36_falling_mask_set param );
unsigned char get_gpio36_falling_mask();
unsigned char set_gpio37_falling_mask( enum gpio37_falling_mask_set param );
unsigned char get_gpio37_falling_mask();
unsigned char set_gpio38_falling_mask( enum gpio38_falling_mask_set param );
unsigned char get_gpio38_falling_mask();
unsigned char set_gpio39_falling_mask( enum gpio39_falling_mask_set param );
unsigned char get_gpio39_falling_mask();
unsigned char set_gpio40_falling_mask( enum gpio40_falling_mask_set param );
unsigned char get_gpio40_falling_mask();
unsigned char set_gpio41_falling_mask( enum gpio41_falling_mask_set param );
unsigned char get_gpio41_falling_mask();
unsigned char set_rising_pon_key1_masked_interrupt( enum rising_pon_key1_mask_set param );
unsigned char get_rising_pon_key1_masked_interrupt();
unsigned char set_falling_pon_key1_masked_interrupt( enum falling_pon_key1_mask_set param );
unsigned char get_falling_pon_key1_masked_interrupt();
unsigned char set_rising_pon_key2_masked_interrupt( enum rising_pon_key2_mask_set param );
unsigned char get_rising_pon_key2_masked_interrupt();
unsigned char set_falling_pon_key2_masked_interrupt( enum falling_pon_key2_mask_set param );
unsigned char get_falling_pon_key2_masked_interrupt();
unsigned char set_thermal_warning_masked_interrupt( enum thermal_warning_mask_set param );
unsigned char get_thermal_warning_masked_interrupt();
unsigned char set_plug_tv_masked_interrupt( enum plug_tv_mask_set param );
unsigned char get_plug_tv_masked_interrupt();
unsigned char set_un_plug_tv_masked_interrupt( enum un_plug_tv_mask_set param );
unsigned char get_un_plug_tv_masked_interrupt();
unsigned char set_main_charger_allowed_masked_interrupt( enum main_charger_allowed_mask_set param );
unsigned char get_main_charger_allowed_masked_interrupt();
unsigned char set_gpio6_falling_mask( enum gpio6_falling_mask_set param );
unsigned char get_gpio6_falling_mask();
unsigned char set_gpio7_falling_mask( enum gpio7_falling_mask_set param );
unsigned char get_gpio7_falling_mask();
unsigned char set_gpio8_falling_mask( enum gpio8_falling_mask_set param );
unsigned char get_gpio8_falling_mask();
unsigned char set_gpio9_falling_mask( enum gpio9_falling_mask_set param );
unsigned char get_gpio9_falling_mask();
unsigned char set_gpio10_falling_mask( enum gpio10_falling_mask_set param );
unsigned char get_gpio10_falling_mask();
unsigned char set_gpio11_falling_mask( enum gpio11_falling_mask_set param );
unsigned char get_gpio11_falling_mask();
unsigned char set_gpio12_falling_mask( enum gpio12_falling_mask_set param );
unsigned char get_gpio12_falling_mask();
unsigned char set_gpio13_falling_mask( enum gpio13_falling_mask_set param );
unsigned char get_gpio13_falling_mask();
unsigned char get_it_from_latch19();
unsigned char get_it_from_latch20();
unsigned char get_it_from_latch21();
unsigned char get_it_from_latch22();
unsigned char set_gpio24_rising_mask( enum gpio24_rising_mask_set param );
unsigned char get_gpio24_rising_mask();
unsigned char set_gpio25_rising_mask( enum gpio25_rising_mask_set param );
unsigned char get_gpio25_rising_mask();
unsigned char set_gpio36_rising_mask( enum gpio36_rising_mask_set param );
unsigned char get_gpio36_rising_mask();
unsigned char set_gpio37_rising_mask( enum gpio37_rising_mask_set param );
unsigned char get_gpio37_rising_mask();
unsigned char set_gpio38_rising_mask( enum gpio38_rising_mask_set param );
unsigned char get_gpio38_rising_mask();
unsigned char set_gpio39_rising_mask( enum gpio39_rising_mask_set param );
unsigned char get_gpio39_rising_mask();
unsigned char set_gpio40_rising_mask( enum gpio40_rising_mask_set param );
unsigned char get_gpio40_rising_mask();
unsigned char set_gpio41_rising_mask( enum gpio41_rising_mask_set param );
unsigned char get_gpio41_rising_mask();
unsigned char set_gpio6_rising_mask( enum gpio6_rising_mask_set param );
unsigned char get_gpio6_rising_mask();
unsigned char set_gpio7_rising_mask( enum gpio7_rising_mask_set param );
unsigned char get_gpio7_rising_mask();
unsigned char set_gpio8_rising_mask( enum gpio8_rising_mask_set param );
unsigned char get_gpio8_rising_mask();
unsigned char set_gpio9_rising_mask( enum gpio9_rising_mask_set param );
unsigned char get_gpio9_rising_mask();
unsigned char set_gpio10_rising_mask( enum gpio10_rising_mask_set param );
unsigned char get_gpio10_rising_mask();
unsigned char set_gpio11_rising_mask( enum gpio11_rising_mask_set param );
unsigned char get_gpio11_rising_mask();
unsigned char set_gpio12_rising_mask( enum gpio12_rising_mask_set param );
unsigned char get_gpio12_rising_mask();
unsigned char set_gpio13_rising_mask( enum gpio13_rising_mask_set param );
unsigned char get_gpio13_rising_mask();
unsigned char get_it_from_latch9();
unsigned char get_it_from_latch10();
unsigned char get_it_from_latch12();
unsigned char get_it_from_latch1();
unsigned char get_it_from_latch2();
unsigned char get_it_from_latch3();
unsigned char get_it_from_latch4();
unsigned char get_it_from_latch5();
unsigned char get_it_from_latch7();
unsigned char get_it_from_latch8();
unsigned char set_adc_hardware_control_mask( enum adc_hardware_control_mask_set param );
unsigned char get_adc_hardware_control_mask();
unsigned char set_accessory21_high_voltage_mask( enum accessory21_high_voltage_mask_set param );
unsigned char get_accessory21_high_voltage_mask();
unsigned char set_accessory21_low_voltage_mask( enum accessory21_low_voltage_mask_set param );
unsigned char get_accessory21_low_voltage_mask();
unsigned char set_accessory22_high_voltage_mask( enum accessory22_high_voltage_mask_set param );
unsigned char get_accessory22_high_voltage_mask();
unsigned char set_accessory22_low_voltage_mask( enum accessory22_low_voltage_mask_set param );
unsigned char get_accessory22_low_voltage_mask();
unsigned char set_accessory1_high_voltage_mask( enum accessory1_high_voltage_mask_set param );
unsigned char get_accessory1_high_voltage_mask();
unsigned char set_accessory1_low_voltage_mask( enum accessory1_low_voltage_mask_set param );
unsigned char get_accessory1_low_voltage_mask();
unsigned char set_adc_software_control_mask( enum adc_software_control_mask_set param );
unsigned char get_adc_software_control_mask();
unsigned char set_rtc_battery_high_mask( enum rtc_battery_high_mask_set param );
unsigned char get_rtc_battery_high_mask();
unsigned char set_rtc_battery_low_mask( enum rtc_battery_low_mask_set param );
unsigned char get_rtc_battery_low_mask();
unsigned char set_high_battery_mask( enum high_battery_mask_set param );
unsigned char get_high_battery_mask();
unsigned char set_low_battery_mask( enum low_battery_mask_set param );
unsigned char get_low_battery_mask();
unsigned char set_coulomb_calibration_endedd_mask( enum coulomb_calibration_endedd_mask_set param );
unsigned char get_coulomb_calibration_endedd_mask();
unsigned char set_coulomb_ended_mask( enum coulomb_counter_ended_mask_set param );
unsigned char get_coulomb_ended_mask();
unsigned char set_audio_mask( enum audio_mask_set param );
unsigned char get_audio_mask();
unsigned char set_accumulation_conversion_mask( enum accumulation_conversion_mask_set param );
unsigned char get_accumulation_conversion_mask();
unsigned char set_main_charger_over_current_mask( enum cain_charger_over_current_mask_set param );
unsigned char get_main_charger_over_current_mask();
unsigned char set_overvoltage_on_vbus_mask( enum overvoltage_on_vbus_l_mask_set param );
unsigned char get_overvoltage_on_vbus_mask();
unsigned char set_watchdog_charger_expiration_mask( enum watchdog_charger_expiration_mask_set param );
unsigned char get_watchdog_charger_expiration_mask();
unsigned char set_battery_removal_mask( enum battery_removal_mask_set param );
unsigned char get_battery_removal_mask();
unsigned char set_rtc_alarm_mask( unsigned char param );
unsigned char get_rtc_alarm_mask();
unsigned char set_rtc60s_mask( unsigned char param );
unsigned char get_rtc60s_mask();
unsigned char set_usb_charger_dropout_mask( unsigned char param );
unsigned char get_usb_charger_dropout_mask();
unsigned char set_usb_charger_thermal_below_mask( enum usb_charger_thermal_below_mask_set param );
unsigned char get_usb_charger_thermal_below_mask();
unsigned char set_usb_charger_thermal_above_mask( enum usb_charger_thermal_above_mask_set param );
unsigned char get_usb_charger_thermal_above_mask();
unsigned char set_main_charger_thermal_below_mask( enum main_charger_thermal_below_mask_set param );
unsigned char get_main_charger_thermal_below_mask();
unsigned char set_main_charger_thermal_above_mask( enum main_charger_thermal_above_mask_set param );
unsigned char get_main_charger_thermal_above_mask();
unsigned char set_charging_current_no_more_limited_mask( enum charging_current_no_more_limited_mask_set param );
unsigned char get_charging_current_no_more_limited_mask();
unsigned char set_charging_current_limited_h_s_chirp_mask( enum charging_current_limited_h_s_chirp_mask_set param );
unsigned char get_charging_current_limited_h_s_chirp_mask();
unsigned char set_xtal32_ok_mask( enum xtal32_ok_mask_set param );
unsigned char get_xtal32_ok_mask();
unsigned char set_ch_stop_by_sec( enum charger_sttoped_mask_set param );
unsigned char get_ch_stop_by_sec();
unsigned char set_usb_charger_mask( enum usb_charger_mask_set param );
unsigned char get_usb_charger_mask();
unsigned char set_id_detection220k_mask( enum id_detection220k_mask_set param );
unsigned char get_id_detection220k_mask();
unsigned char set_id_detection150k_mask( enum id_detection125k_mask_set param );
unsigned char get_id_detection150k_mask();
unsigned char set_id_detection69k_mask( enum id_detection69k_mask_set param );
unsigned char get_id_detection69k_mask();
unsigned char set_id_detection37k_mask( enum id_detection37k_mask_set param );
unsigned char get_id_detection37k_mask();
unsigned char set_id_wake_up_falling_mask( enum id_wake_up_falling_mask_set param );
unsigned char get_id_wake_up_falling_mask();
unsigned char set_id_detection_greater220k_mask( enum id_detection_greater220k_mask_set param );
unsigned char get_id_detection_greater220k_mask();
unsigned char set_id_detection_greater150k_mask( enum id_detection_greater125k_mask_set param );
unsigned char get_id_detection_greater150k_mask();
unsigned char set_id_detection_greater69k_mask( enum id_detection_greater69k_mask_set param );
unsigned char get_id_detection_greater69k_mask();
unsigned char set_id_detection_greater37k_mask( enum id_detection_greater37k_mask_set param );
unsigned char get_id_detection_greater37k_mask();
unsigned char set_id_wake_up_mask( enum id_wake_up_rising_mask_set param );
unsigned char get_id_wake_up_mask();
unsigned char set_usb_charger_plug_mask( enum usb_charger_plug_mask_set param );
unsigned char get_usb_charger_plug_mask();
unsigned char set_srp_dectect_mask( enum dplus_rising_mask_set param );
unsigned char get_srp_dectect_mask();
unsigned char get_key_stuck_mask();
unsigned char get_lpr_vdddig_mask();
unsigned char get_l_kp_vdddig_mask();
unsigned char get_kp_vdddig_mask();
unsigned char get_key_deglitch_mask();
unsigned char get_modem_power_status_mask();
unsigned char set_vbus_rising_edge_mask( enum vbus_rising_edge_mask_set param );
unsigned char get_vbus_rising_edge_mask();
unsigned char set_vbus_falling_edge_mask( enum vbus_falling_edge_mask_set param );
unsigned char get_vbus_falling_edge_mask();
unsigned char set_battery_over_voltage_mask( enum battery_over_voltage_mask_set param );
unsigned char get_battery_over_voltage_mask();


#ifdef __cplusplus
}
#endif

#endif
