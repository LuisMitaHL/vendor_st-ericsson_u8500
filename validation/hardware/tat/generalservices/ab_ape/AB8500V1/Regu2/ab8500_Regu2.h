/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Regu2/Linux/ab8500_Regu2.h
 * 
 *
 * Generated on the 11/02/2010 09:50 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_REGU2_LINUX_AB8500_REGU2_H
#define C__LISP_PRODUCTION_AB8500_AB8500_REGU2_LINUX_AB8500_REGU2_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_Regu2_types.h"

unsigned char set_arm_voltage_pwm_mode( enum forced param );
unsigned char get_arm_voltage_pwm_mode();
unsigned char set_arm_voltage_auto_mode( enum auto_mode_enable param );
unsigned char get_arm_voltage_auto_mode();
unsigned char set_vbbn_selection_control( enum selection1 param );
unsigned char get_vbbn_selection_control();
unsigned char set_vbbp_selection_control( enum selection1 param );
unsigned char get_vbbp_selection_control();
unsigned char set_arm_ouput_volage_select( enum selection param );
unsigned char get_arm_ouput_volage_select();
unsigned char set_arm_ouput_regulation( enum regulation param );
unsigned char get_arm_ouput_regulation();
unsigned char set_force_no_step( enum force_no_step param );
unsigned char get_force_no_step();
unsigned char set_vbbp_pulldonw_enable( enum vbbp_pulldonw_enable param );
unsigned char get_vbbp_pulldonw_enable();
unsigned char set_switch_varm_and_vbbp( enum switch_enable param );
unsigned char get_switch_varm_and_vbbp();
unsigned char set_vbbn_regulation( enum regulation param );
unsigned char get_vbbn_regulation();
unsigned char set_vbbp_regulation( enum regulation param );
unsigned char get_vbbp_regulation();
unsigned char set_vape_pwm_mode_forced( enum forced param );
unsigned char get_vape_pwm_mode_forced();
unsigned char set_vape_auto_mode( enum auto_mode param );
unsigned char get_vape_auto_mode();
unsigned char set_vape_selection_control( enum ape_selection param );
unsigned char get_vape_selection_control();
unsigned char set_vape_regulation( enum regulation param );
unsigned char get_vape_regulation();
unsigned char set_vsmps1_voltage_pwm_mode( enum forced param );
unsigned char get_vsmps1_voltage_pwm_mode();
unsigned char set_vsmps1_auto_mode( enum auto_mode param );
unsigned char get_vsmps1_auto_mode();
unsigned char set_vsmps1_ouput_volage_select( enum selection param );
unsigned char get_vsmps1_ouput_volage_select();
unsigned char set_vsmps1_regulation( enum regulation param );
unsigned char get_vsmps1_regulation();
unsigned char set_vsmps2_voltage_pwm_mode( enum forced param );
unsigned char get_vsmps2_voltage_pwm_mode();
unsigned char set_vsmps2_auto_mode( enum auto_mode param );
unsigned char get_vsmps2_auto_mode();
unsigned char set_vsmps2_ouput_volage_select( enum selection param );
unsigned char get_vsmps2_ouput_volage_select();
unsigned char set_vsmps2_regulation( enum regulation param );
unsigned char get_vsmps2_regulation();
unsigned char set_vsmps3_voltage_pwm_mode( enum forced param );
unsigned char get_vsmps3_voltage_pwm_mode();
unsigned char set_vsmps3_auto_mode( enum auto_mode param );
unsigned char get_vsmps3_auto_mode();
unsigned char set_vsmps3_ouput_volage_select( enum selection param );
unsigned char get_vsmps3_ouput_volage_select();
unsigned char set_vsmps3_regulation( enum regulation param );
unsigned char get_vsmps3_regulation();
unsigned char set_vana_regulation( enum regulation param );
unsigned char get_vana_regulation();
unsigned char set_vpll_regulation( enum regulation param );
unsigned char get_vpll_regulation();
unsigned char set_vref_ddr_sleep_mode( enum sleep_mode param );
unsigned char get_vref_ddr_sleep_mode();
unsigned char set_vref_ddr_enable( enum vref_ddr_enable param );
unsigned char get_vref_ddr_enable();
unsigned char set_vext_suppply3_regulation( enum regulation param );
unsigned char get_vext_suppply3_regulation();
unsigned char set_vext_suppply2_regulation( enum regulation param );
unsigned char get_vext_suppply2_regulation();
unsigned char set_vext_suppply1_regulation( enum regulation param );
unsigned char get_vext_suppply1_regulation();
unsigned char set_vaux2_regulation( enum regulation param );
unsigned char get_vaux2_regulation();
unsigned char set_vaux1_regulation( enum regulation param );
unsigned char get_vaux1_regulation();
unsigned char set_vrf1_regulation( enum regulation param );
unsigned char get_vrf1_regulation();
unsigned char set_vaux3_regulation( enum regulation param );
unsigned char get_vaux3_regulation();
unsigned char set_varm_output_sel1( enum output_supply param );
unsigned char get_varm_output_sel1();
unsigned char set_varm_output_sel2( enum output_supply param );
unsigned char get_varm_output_sel2();
unsigned char set_varm_output_sel3( enum output_supply param );
unsigned char get_varm_output_sel3();
unsigned char set_vape_output_sel1( enum output_supply param );
unsigned char get_vape_output_sel1();
unsigned char set_vape_output_sel2( enum output_supply param );
unsigned char get_vape_output_sel2();
unsigned char set_vape_output_sel3( enum output_supply param );
unsigned char get_vape_output_sel3();
unsigned char set_vbb_positive_selection1( enum positivie_voltage_selection param );
unsigned char get_vbb_positive_selection1();
unsigned char set_vbb_negative_selection1( enum negative_voltage_selection param );
unsigned char get_vbb_negative_selection1();
unsigned char set_vbb_positive_selection2( enum positivie_voltage_selection param );
unsigned char get_vbb_positive_selection2();
unsigned char set_vbb_negative_selection2( enum negative_voltage_selection param );
unsigned char get_vbb_negative_selection2();
unsigned char set_vsmps1_sel1_output( enum output_supply param );
unsigned char get_vsmps1_sel1_output();
unsigned char set_vsmps1_sel2_output( enum output_supply param );
unsigned char get_vsmps1_sel2_output();
unsigned char set_vsmps1_sel3_output( enum output_supply param );
unsigned char get_vsmps1_sel3_output();
unsigned char set_vsmps2_sel1_output( enum output_supply param );
unsigned char get_vsmps2_sel1_output();
unsigned char set_vsmps2_sel2_output( enum output_supply param );
unsigned char get_vsmps2_sel2_output();
unsigned char set_vsmps2_sel3_output( enum output_supply param );
unsigned char get_vsmps2_sel3_output();
unsigned char set_vsmps3_sel1_output( enum output_supply2 param );
unsigned char get_vsmps3_sel1_output();
unsigned char set_vsmps3_sel2_output( enum output_supply2 param );
unsigned char get_vsmps3_sel2_output();
unsigned char set_vsmps3_sel3_output( enum output_supply2 param );
unsigned char get_vsmps3_sel3_output();
unsigned char set_vaux1_sel_output( enum vaux_output param );
unsigned char get_vaux1_sel_output();
unsigned char set_vaux2_sel_output( enum vaux_output param );
unsigned char get_vaux2_sel_output();
unsigned char set_vaux3_sel_output( enum vaux_output param );
unsigned char get_vaux3_sel_output();
unsigned char set_vrf1_sel_output( enum vrf1_output param );
unsigned char get_vrf1_sel_output();
unsigned char set_ext_supply_force_low( enum ext_supply_force_low param );
unsigned char get_ext_supply_force_low();
unsigned char set_vmod_resquet_control( enum vmod_resquet_control param );
unsigned char get_vmod_resquet_control();
unsigned char set_mod_sys_clk_req1_valid( enum modem_system_clock_request_valid param );
unsigned char get_mod_sys_clk_req1_valid();
unsigned char set_vmod_p_w_m_mode( enum vmod_forced_enable param );
unsigned char get_vmod_p_w_m_mode();
unsigned char set_vmod_auto_mode( enum vmod_auto_mode_enable param );
unsigned char get_vmod_auto_mode();
unsigned char set_vmod_selection_control( enum vmod_selection param );
unsigned char get_vmod_selection_control();
unsigned char set_vmod_regulation( enum vmod_regulation param );
unsigned char get_vmod_regulation();
unsigned char set_vmod_sel1_output( enum output_supply param );
unsigned char get_vmod_sel1_output();
unsigned char set_vmod_sel2_output( enum output_supply param );
unsigned char get_vmod_sel2_output();
unsigned char set_vaudio_discharge_time( enum supply_discharge_time param );
unsigned char get_vaudio_discharge_time();
unsigned char set_vtv_out_discharge_time( enum supply_discharge_time param );
unsigned char get_vtv_out_discharge_time();
unsigned char set_vint_core12_discharge_time( enum supply_discharge_time param );
unsigned char get_vint_core12_discharge_time();
unsigned char set_vaux3_discharge_time( enum supply_discharge_time param );
unsigned char get_vaux3_discharge_time();
unsigned char set_vaux2_discharge_time( enum supply_discharge_time param );
unsigned char get_vaux2_discharge_time();
unsigned char set_vaux1_discharge_time( enum supply_discharge_time param );
unsigned char get_vaux1_discharge_time();
unsigned char set_vrf1_discharge_time( enum supply_discharge_time param );
unsigned char get_vrf1_discharge_time();
unsigned char set_vpll_discharge_time( enum supply_discharge_time param );
unsigned char get_vpll_discharge_time();
unsigned char set_vana_discharge_time( enum supply_discharge_time param );
unsigned char get_vana_discharge_time();
unsigned char set_vsim_discharge_time( enum supply_discharge_time param );
unsigned char get_vsim_discharge_time();


#ifdef __cplusplus
}
#endif

#endif
