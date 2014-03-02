/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_Charger/Linux/ab8500_Charger.h
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

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_CHARGER_LINUX_AB8500_CHARGER_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_CHARGER_LINUX_AB8500_CHARGER_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Charger_types.h"

unsigned char get_main_current_limitation();
unsigned char get_main_current_regulation();
unsigned char get_main_voltage_regulation();
unsigned char get_main_charge_on();
unsigned char get_main_charger_level();
unsigned char get_main_charger_event();
unsigned char get_voltage_drop();
unsigned char get_main_charger_detected();
unsigned char get_main_charger_input_high_voltage();
unsigned char get_main_charger_input_low_voltage();
unsigned char get_thermal_protection();
unsigned char get_voltage_status();
unsigned char get_usb_charger_status();
unsigned char get_voltage_close_battery_voltage();
unsigned char get_voltage_regulation();
unsigned char get_usb_charger_on();
unsigned char get_vbus_detection_debouce100_ms();
unsigned char get_vbus_detection_debouce1_ms();
unsigned char get_usb_charger_status2();
unsigned char get_automatic_usb_charger_current();
unsigned char get_usb_device_illegal();
unsigned char get_usb_thermal_protection();
unsigned char get_vbus_protection();
unsigned char get_usb_charger_status3();
unsigned char get_charging_stopped();
unsigned char get_battery_temperature();
unsigned char get_voltage_below_threshold();
unsigned char set_ch_volt_level( enum volt_level param );
unsigned char get_ch_volt_level();
unsigned char set_maximum_charging_voltage_level( unsigned char param );
unsigned char get_maximum_charging_voltage_level();
unsigned char set_channel_output_current_level( enum charger_current_level param );
unsigned char get_channel_output_current_level();
unsigned char set_maximum_charging_current( unsigned char param );
unsigned char get_maximum_charging_current();
unsigned char set_charging_current_behavor( enum charging_current param );
unsigned char get_charging_current_behavor();
unsigned char set_charger_watch_dog_timer( unsigned char param );
unsigned char get_charger_watch_dog_timer();
unsigned char set_charger_watch_dog_timer9( unsigned char param );
unsigned char get_charger_watch_dog_timer9();
unsigned char set_kick_watchdog_timer_enable( enum kick_watchdog_timer_enable param );
unsigned char get_kick_watchdog_timer_enable();
unsigned char set_charger_temperature_high( enum temperature_high_threshold param );
unsigned char get_charger_temperature_high();
unsigned char set_charger_led_current( enum led_current param );
unsigned char get_charger_led_current();
unsigned char set_led_indicator_enable( enum led_indicator_enable param );
unsigned char get_led_indicator_enable();
unsigned char set_led_indicator_duty( unsigned char param );
unsigned char get_led_indicator_duty();
unsigned char set_charger_dithering_enable( enum clock_charger_dithering_enabled param );
unsigned char get_charger_dithering_enable();
unsigned char set_battery_o_v_v_enable( enum battery_o_v_v_enable param );
unsigned char get_battery_o_v_v_enable();
unsigned char set_battery_threshold( enum battery_threshold param );
unsigned char get_battery_threshold();
unsigned char set_reset_charger( enum reset_charger_enable param );
unsigned char get_reset_charger();
unsigned char set_vbat_overshoot_control( enum vbat_overshoot_enable param );
unsigned char get_vbat_overshoot_control();
unsigned char set_main_charger_enable( enum main_charger_enable param );
unsigned char get_main_charger_enable();
unsigned char set_main_channel_max( enum max_current param );
unsigned char get_main_channel_max();
unsigned char set_thermal_threshold( enum thermal_threshold param );
unsigned char get_thermal_threshold();
unsigned char set_main_ch_input_curr( enum charger_current_level param );
unsigned char get_main_ch_input_curr();
unsigned char set_usb_overshoot_control( enum usb_overshoot_control_enable param );
unsigned char get_usb_overshoot_control();
unsigned char set_ubs_current_limit_disable( enum current_limit_disable param );
unsigned char get_ubs_current_limit_disable();
unsigned char set_ubs_charger_enable( enum ubs_charger_enable param );
unsigned char get_ubs_charger_enable();
unsigned char set_vbus_monitor( enum vbus_monitor_enable param );
unsigned char get_vbus_monitor();
unsigned char set_ubs_voltage_stop( enum vbus_voltage_stop param );
unsigned char get_ubs_voltage_stop();
unsigned char set_ubs_charger_max_current( enum max_current param );
unsigned char get_ubs_charger_max_current();
unsigned char set_ubs_charger_thermal_threshold( enum thermal_threshold param );
unsigned char get_ubs_charger_thermal_threshold();
unsigned char set_ubs_charger_current_limit( enum usb_input_current_charge param );
unsigned char get_ubs_charger_current_limit();


#ifdef __cplusplus
}
#endif

#endif
