/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Charger/Linux/ab9540_Charger.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_CHARGER_LINUX_AB9540_CHARGER_H
#define C__LISP_PRODUCTION_AB9540_AB9540_CHARGER_LINUX_AB9540_CHARGER_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Charger_types.h"

unsigned char get_voltage_below_threshold();
unsigned char get_battery_temperature();
unsigned char get_charging_stopped();
unsigned char get_vbus_protection();
unsigned char get_usb_thermal_protection();
unsigned char get_usb_device_illegal();
unsigned char get_automatic_usb_charger_current();
unsigned char get_vbus_detection_debouce1_ms();
unsigned char get_vbus_detection_debouce100_ms();
unsigned char get_usb_charger_on();
unsigned char get_voltage_regulation();
unsigned char get_voltage_close_battery_voltage();
unsigned char set_kick_watchdog_timer( enum kick_watchdog_timer_enable param );
unsigned char get_kick_watchdog_timer();
unsigned char set_charger_watch_dog_timer9( unsigned char param );
unsigned char get_charger_watch_dog_timer9();
unsigned char set_reset_charger( enum reset_charger_enable param );
unsigned char get_reset_charger();
unsigned char set_charger_temperature_high( enum temperature_high_threshold param );
unsigned char get_charger_temperature_high();
unsigned char get_voltage_status();
unsigned char get_thermal_protection();
unsigned char get_main_charger_input_low_voltage();
unsigned char get_main_charger_input_high_voltage();
unsigned char get_main_charger_detected();
unsigned char get_voltage_drop();
unsigned char set_maximum_charging_voltage_level( unsigned char param );
unsigned char get_maximum_charging_voltage_level();
unsigned char set_thermal_threshold( enum thermal_threshold param );
unsigned char get_thermal_threshold();
unsigned char set_main_channel_max( enum max_current param );
unsigned char get_main_channel_max();
unsigned char set_blink_frequency( enum blink_frequency param );
unsigned char get_blink_frequency();
unsigned char set_blink_char_led( enum blink_char_led_enable param );
unsigned char get_blink_char_led();
unsigned char set_blink_duty_charger_led( unsigned char param );
unsigned char get_blink_duty_charger_led();
unsigned char set_ch_volt_level( enum volt_level param );
unsigned char get_ch_volt_level();
unsigned char set_channel_output_current_level( enum charger_current_level param );
unsigned char get_channel_output_current_level();
unsigned char set_ubs_charger_current_limit( enum usb_input_current_charge param );
unsigned char get_ubs_charger_current_limit();
unsigned char set_ubs_charger_thermal_threshold( enum thermal_threshold param );
unsigned char get_ubs_charger_thermal_threshold();
unsigned char set_ubs_charger_max_current( enum max_current param );
unsigned char get_ubs_charger_max_current();
unsigned char set_ubs_voltage_stop( enum vbus_voltage_stop param );
unsigned char get_ubs_voltage_stop();
unsigned char set_vbus_monitor( enum vbus_monitor_enable param );
unsigned char get_vbus_monitor();
unsigned char set_charger_dithering( enum clock_charger_dithering_enabled param );
unsigned char get_charger_dithering();
unsigned char set_charging_current_behavor( enum charging_current param );
unsigned char get_charging_current_behavor();
unsigned char set_bat_ctrl8u( enum bat_ctrl8u_enable param );
unsigned char get_bat_ctrl8u();
unsigned char set_bat_ctrl16u( enum bat_ctrl16u_enable param );
unsigned char get_bat_ctrl16u();
unsigned char set_bat_ctrl_cmp( enum bat_ctrl_cmp_enable param );
unsigned char get_bat_ctrl_cmp();
unsigned char set_battery_pullup( enum pullup_enable param );
unsigned char get_battery_pullup();
unsigned char set_battery_crontrol4uu_ena( enum battery_crontrol4u_enable param );
unsigned char get_battery_crontrol4uu_ena();
unsigned char set_main_charger( enum main_charger_enable param );
unsigned char get_main_charger();
unsigned char set_vbat_overshoot_control( enum vbat_overshoot_enable param );
unsigned char get_vbat_overshoot_control();
unsigned char get_main_charger_level();
unsigned char get_main_charge_on();
unsigned char get_main_voltage_regulation();
unsigned char get_main_current_regulation();
unsigned char get_main_current_limitation();
unsigned char set_maximum_charging_current( unsigned char param );
unsigned char get_maximum_charging_current();
unsigned char set_led_indicator( enum led_indicator_enable param );
unsigned char get_led_indicator();
unsigned char set_charger_led_current( enum led_current param );
unsigned char get_charger_led_current();
unsigned char set_ubs_charger( enum ubs_charger_enable param );
unsigned char get_ubs_charger();
unsigned char set_usb_chg_overshoot_control( enum vbat_overshoot_control_enable param );
unsigned char get_usb_chg_overshoot_control();
unsigned char set_mask_usb_chirp( enum mask_usb_chirp_enable param );
unsigned char get_mask_usb_chirp();
unsigned char set_ubs_current_limit_disable( enum current_limit_disable param );
unsigned char get_ubs_current_limit_disable();
unsigned char set_usb_overshoot_control( enum usb_overshoot_control_enable param );
unsigned char get_usb_overshoot_control();


#ifdef __cplusplus
}
#endif

#endif
