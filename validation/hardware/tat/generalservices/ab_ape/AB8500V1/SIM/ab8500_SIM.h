/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_SIM/Linux/ab8500_SIM.h
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

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_SIM_LINUX_AB8500_SIM_H
#define C__LISP_PRODUCTION_AB8500_AB8500_SIM_LINUX_AB8500_SIM_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_SIM_types.h"

unsigned char set_power_down_procedure( enum powe_down_procedure_enable param );
unsigned char get_power_down_procedure();
unsigned char set_reset_duration( enum reset_duration param );
unsigned char get_reset_duration();
unsigned char set_warm_reset( enum warm_reset_enable param );
unsigned char get_warm_reset();
unsigned char set_internally_reset( enum internally_reset param );
unsigned char get_internally_reset();
unsigned char set_sim_power_on( enum sim_power_on param );
unsigned char get_sim_power_on();
unsigned char set_clock32_selected( enum clock32_selected param );
unsigned char get_clock32_selected();
unsigned char set_card_clock_selected( enum card_clock_selected param );
unsigned char get_card_clock_selected();
unsigned char set_counter_reset( enum counter_reset param );
unsigned char get_counter_reset();
unsigned char set_card_detection_timeout( unsigned char param );
unsigned char get_card_detection_timeout();
unsigned char set_comp_value_low_byte( unsigned char param );
unsigned char get_comp_value_low_byte();
unsigned char set_comp_value_high_byte( unsigned char param );
unsigned char get_comp_value_high_byte();
unsigned char set_vsim_active_mode( enum active_mode param );
unsigned char get_vsim_active_mode();
unsigned char set_sim_da_pull_up( enum pull_up param );
unsigned char get_sim_da_pull_up();
unsigned char set_sim_comp_detection( enum comparator_enabled param );
unsigned char get_sim_comp_detection();
unsigned char set_removal_detection( enum removal_detection param );
unsigned char get_removal_detection();
unsigned char set_vsim_regulator( enum vsim_regulator param );
unsigned char get_vsim_regulator();
unsigned char set_vsim_voltage( enum vsim_voltage param );
unsigned char get_vsim_voltage();
unsigned char set_sim_io_normal( enum sim_io_normal param );
unsigned char get_sim_io_normal();
unsigned char set_select_vsim_power( enum select_vsim_power param );
unsigned char get_select_vsim_power();
unsigned char set_sim_source_interrupts( unsigned char param );
unsigned char get_sim_source_interrupts();
unsigned char set_intr_tx_error( unsigned char param );
unsigned char get_intr_tx_error();
unsigned char set_intr_atr_error( unsigned char param );
unsigned char get_intr_atr_error();
unsigned char set_intr_card_detected( unsigned char param );
unsigned char get_intr_card_detected();
unsigned char set_sim_latch_interrupts( unsigned char param );
unsigned char get_sim_latch_interrupts();
unsigned char set_latch_tx_error( unsigned char param );
unsigned char get_latch_tx_error();
unsigned char set_latch_atr_error( unsigned char param );
unsigned char get_latch_atr_error();
unsigned char set_latch_card_detected( unsigned char param );
unsigned char get_latch_card_detected();
unsigned char set_sim_mask_interrupts( unsigned char param );
unsigned char get_sim_mask_interrupts();
unsigned char set_mask_tx_error( unsigned char param );
unsigned char get_mask_tx_error();
unsigned char set_mask_atr_error( unsigned char param );
unsigned char get_mask_atr_error();
unsigned char set_mask_card_detected( unsigned char param );
unsigned char get_mask_card_detected();
unsigned char set_weak_pull_down( enum weak_pull_down_active param );
unsigned char get_weak_pull_down();
unsigned char set_sim_reset_value( enum pad_driven param );
unsigned char get_sim_reset_value();
unsigned char set_usim_by_pass( enum usim_by_pass param );
unsigned char get_usim_by_pass();
unsigned char set_sim_reset_control( enum sim_reset_control param );
unsigned char get_sim_reset_control();
unsigned char set_usim_load( enum usim_load param );
unsigned char get_usim_load();
unsigned char set_usim_power( enum usim_power_on param );
unsigned char get_usim_power();
unsigned char set_sim_delay( enum sim_delay param );
unsigned char get_sim_delay();


#ifdef __cplusplus
}
#endif

#endif
