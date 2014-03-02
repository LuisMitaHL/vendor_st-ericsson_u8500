/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Regul/Linux/ab9540_Regul.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_REGUL_LINUX_AB9540_REGUL_H
#define C__LISP_PRODUCTION_AB9540_AB9540_REGUL_LINUX_AB9540_REGUL_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Regul_types.h"

unsigned char set_vaux4_sw_h_p_req_valid( enum sys_clock_request param );
unsigned char get_vaux4_sw_h_p_req_valid();
unsigned char set_vaux4_hw_h_p_req2_valid( enum sys_clock_request param );
unsigned char get_vaux4_hw_h_p_req2_valid();
unsigned char set_vaux4_hw_h_p_req1_valid( enum sys_clock_request param );
unsigned char get_vaux4_hw_h_p_req1_valid();
unsigned char set_vaux4_sys_clk_req1_h_p_valid( enum sys_clock_request param );
unsigned char get_vaux4_sys_clk_req1_h_p_valid();
unsigned char set_vamic1_polarity( enum impedance param );
unsigned char get_vamic1_polarity();
unsigned char set_vamic2_polarity( enum impedance param );
unsigned char get_vamic2_polarity();
unsigned char set_vsim_ldo_polarity( enum impedance param );
unsigned char get_vsim_ldo_polarity();
unsigned char set_vamic2_impedance( enum impedance param );
unsigned char get_vamic2_impedance();
unsigned char set_vusb( enum usb_supply_enable param );
unsigned char get_vusb();
unsigned char set_vbus_low_power_mode( enum vbus_low_power_mode param );
unsigned char get_vbus_low_power_mode();
unsigned char set_v_usb_turn_off( enum v_usb_turn_off param );
unsigned char get_v_usb_turn_off();
unsigned char set_sys_clk_req2_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req2_valid2();
unsigned char set_sys_clk_req3_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req3_valid2();
unsigned char set_sys_clk_req4_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req4_valid2();
unsigned char set_sys_clk_req5_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req5_valid2();
unsigned char set_sys_clk_req6_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req6_valid2();
unsigned char set_sys_clk_req7_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req7_valid2();
unsigned char set_sys_clk_req8_valid2( enum sys_clock_request param );
unsigned char get_sys_clk_req8_valid2();
unsigned char set_vmod_sel_ctrl_sel( enum vmod_sel_ctrl_sel param );
unsigned char get_vmod_sel_ctrl_sel();
unsigned char set_serial_control_interface( enum serial_control_by param );
unsigned char get_serial_control_interface();
unsigned char set_sys_clk_req2_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req2_valid1();
unsigned char set_sys_clk_req3_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req3_valid1();
unsigned char set_sys_clk_req4_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req4_valid1();
unsigned char set_sys_clk_req5_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req5_valid1();
unsigned char set_sys_clk_req6_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req6_valid1();
unsigned char set_sys_clk_req7_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req7_valid1();
unsigned char set_sys_clk_req8_valid1( enum sys_clock_request param );
unsigned char get_sys_clk_req8_valid1();
unsigned char set_otg_supply( enum otg_supply_enable param );
unsigned char get_otg_supply();
unsigned char set_otg_supply_force_l_p( enum otg_supply_force_l_p_enable param );
unsigned char get_otg_supply_force_l_p();
unsigned char set_vbus_bis( enum vbus_bis_enable param );
unsigned char get_vbus_bis();
unsigned char set_vape_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vape_sys_clk_req1_h_p_valid();
unsigned char set_varm_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_varm_sys_clk_req1_h_p_valid();
unsigned char set_vbb_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vbb_sys_clk_req1_h_p_valid();
unsigned char set_vmod_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vmod_sys_clk_req1_h_p_valid();
unsigned char set_vext_supply1_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vext_supply1_sys_clk_req1_h_p_valid();
unsigned char set_vext_supply2_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vext_supply2_sys_clk_req1_h_p_valid();
unsigned char set_vext_supply3_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vext_supply3_sys_clk_req1_h_p_valid();
unsigned char set_audio_supply( enum audio1_enable param );
unsigned char get_audio_supply();
unsigned char set_digital_mic_supply( enum digital_mic_enable param );
unsigned char get_digital_mic_supply();
unsigned char set_analog_mic1_supply( enum analog_mic1_enable param );
unsigned char get_analog_mic1_supply();
unsigned char set_vnegative_mic2_supply( enum vnegative_mic2_enable param );
unsigned char get_vnegative_mic2_supply();
unsigned char set_vsmps1_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vsmps1_hw_h_p_req2_valid();
unsigned char set_vsmps2_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vsmps2_hw_h_p_req2_valid();
unsigned char set_vsmps3_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vsmps3_hw_h_p_req2_valid();
unsigned char set_vana_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vana_hw_h_p_req2_valid();
unsigned char set_vpll_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vpll_hw_h_p_req2_valid();
unsigned char set_vaux1_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vaux1_hw_h_p_req2_valid();
unsigned char set_vaux2_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vaux2_hw_h_p_req2_valid();
unsigned char set_vaux3_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vaux3_hw_h_p_req2_valid();
unsigned char set_vaux3_request_control( enum request_control param );
unsigned char get_vaux3_request_control();
unsigned char set_software_supply( enum software_supply_enable param );
unsigned char get_software_supply();
unsigned char set_vext_supply1_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vext_supply1_hw_h_p_req1_valid();
unsigned char set_vext_supply2_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vext_supply2_hw_h_p_req1_valid();
unsigned char set_vext_supply3_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vext_supply3_hw_h_p_req1_valid();
unsigned char set_vmod_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vmod_hw_h_p_req1_valid();
unsigned char set_vext_supply2_request_control( enum request_control param );
unsigned char get_vext_supply2_request_control();
unsigned char set_vext_supply3_request_control( enum request_control param );
unsigned char get_vext_supply3_request_control();
unsigned char set_vaux1_request_control( enum request_control param );
unsigned char get_vaux1_request_control();
unsigned char set_vaux2_request_control( enum request_control param );
unsigned char get_vaux2_request_control();
unsigned char set_vsmps3_request_control( enum request_control param );
unsigned char get_vsmps3_request_control();
unsigned char set_vpll_request_control( enum request_control param );
unsigned char get_vpll_request_control();
unsigned char set_vana_request_control( enum request_control param );
unsigned char get_vana_request_control();
unsigned char set_vext_supply1_request_control( enum request_control param );
unsigned char get_vext_supply1_request_control();
unsigned char set_vext_supply1_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vext_supply1_hw_h_p_req2_valid();
unsigned char set_vext_supply2_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vext_supply2_hw_h_p_req2_valid();
unsigned char set_vext_supply3_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vext_supply3_hw_h_p_req2_valid();
unsigned char set_vmod_hw_h_p_req2_valid( enum hardware_validation param );
unsigned char get_vmod_hw_h_p_req2_valid();
unsigned char set_vsmps1_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vsmps1_sys_clk_req1_h_p_valid();
unsigned char set_vsmps2_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vsmps2_sys_clk_req1_h_p_valid();
unsigned char set_vsmps3_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vsmps3_sys_clk_req1_h_p_valid();
unsigned char set_vana_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vana_sys_clk_req1_h_p_valid();
unsigned char set_vppl_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vppl_sys_clk_req1_h_p_valid();
unsigned char set_vaux1_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vaux1_sys_clk_req1_h_p_valid();
unsigned char set_vaux2_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vaux2_sys_clk_req1_h_p_valid();
unsigned char set_vaux3_sys_clk_req1_h_p_valid( enum request_valid param );
unsigned char get_vaux3_sys_clk_req1_h_p_valid();
unsigned char set_vaux2_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vaux2_sw_h_p_req_valid();
unsigned char set_vaux3_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vaux3_sw_h_p_req_valid();
unsigned char set_vext_supply1_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vext_supply1_sw_h_p_req_valid();
unsigned char set_vext_supply2_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vext_supply2_sw_h_p_req_valid();
unsigned char set_vext_supply3_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vext_supply3_sw_h_p_req_valid();
unsigned char set_vmod_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vmod_sw_h_p_req_valid();
unsigned char set_vape_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vape_sw_h_p_req_valid();
unsigned char set_varm_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_varm_sw_h_p_req_valid();
unsigned char set_vsmps1_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vsmps1_sw_h_p_req_valid();
unsigned char set_vsmps2_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vsmps2_sw_h_p_req_valid();
unsigned char set_vsmps3_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vsmps3_sw_h_p_req_valid();
unsigned char set_vana_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vana_sw_h_p_req_valid();
unsigned char set_vpll_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vpll_sw_h_p_req_valid();
unsigned char set_vaux1_sw_h_p_req_valid( enum software_signal_validation param );
unsigned char get_vaux1_sw_h_p_req_valid();
unsigned char set_varm_request_control( enum request_control param );
unsigned char get_varm_request_control();
unsigned char set_vape_request_control( enum request_control param );
unsigned char get_vape_request_control();
unsigned char set_vsmps1_request_control( enum request_control param );
unsigned char get_vsmps1_request_control();
unsigned char set_vsmps2_request_control( enum request_control param );
unsigned char get_vsmps2_request_control();
unsigned char set_vsmps1_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vsmps1_hw_h_p_req1_valid();
unsigned char set_vsmps2_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vsmps2_hw_h_p_req1_valid();
unsigned char set_vsmps3_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vsmps3_hw_h_p_req1_valid();
unsigned char set_vana_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vana_hw_h_p_req1_valid();
unsigned char set_vpll_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vpll_hw_h_p_req1_valid();
unsigned char set_vaux1_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vaux1_hw_h_p_req1_valid();
unsigned char set_vaux2_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vaux2_hw_h_p_req1_valid();
unsigned char set_vaux3_hw_h_p_req1_valid( enum hardware_validation param );
unsigned char get_vaux3_hw_h_p_req1_valid();
unsigned char set_vt_vout( enum low_power_enable param );
unsigned char get_vt_vout();
unsigned char set_vint_core12_ena( enum low_power_enable param );
unsigned char get_vint_core12_ena();
unsigned char set_vint_core12_sel( enum vout_core param );
unsigned char get_vint_core12_sel();
unsigned char set_low_power_mode( enum low_power_enable param );
unsigned char get_low_power_mode();
unsigned char set_vt_vout_low_power( enum low_power_enable param );
unsigned char get_vt_vout_low_power();


#ifdef __cplusplus
}
#endif

#endif
