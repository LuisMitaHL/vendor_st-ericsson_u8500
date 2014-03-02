/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_ADC/Linux/ab8500_ADC.h
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

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_ADC_LINUX_AB8500_ADC_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_ADC_LINUX_AB8500_ADC_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_ADC_types.h"

unsigned char set_charging_current_sense( enum charging_current_sense_enable param );
unsigned char get_charging_current_sense();
unsigned char set_adc_buffer( enum adc_buffer_enable param );
unsigned char get_adc_buffer();
unsigned char set_pull_up_supply( enum pull_up_supply param );
unsigned char get_pull_up_supply();
unsigned char set_adc_trig_edge( enum adc_trig_edge_enable param );
unsigned char get_adc_trig_edge();
unsigned char set_adc_auto_req( enum adc_auto_req_enable param );
unsigned char get_adc_auto_req();
unsigned char set_adc_man_convert( enum adc_man_convert_enable param );
unsigned char get_adc_man_convert();
unsigned char set_adc_trigger( enum adc_trigger_enable param );
unsigned char get_adc_trigger();
unsigned char set_adc_enable( enum adc_enable param );
unsigned char get_adc_enable();
unsigned char set_sample_to_average( enum sample_to_average param );
unsigned char get_sample_to_average();
unsigned char set_adc_select_input( enum adc_input_channel param );
unsigned char get_adc_select_input();
unsigned char set_sample_to_average_trig( enum sample_to_average param );
unsigned char get_sample_to_average_trig();
unsigned char set_adc_select_input_tric( enum adc_input_channel param );
unsigned char get_adc_select_input_tric();
unsigned char set_adc_time_delay( unsigned char param );
unsigned char get_adc_time_delay();
unsigned char get_adc_busy();
unsigned char get_adc_data_low();
unsigned char get_adc_data_high();
unsigned char get_adc_data_low_trig();
unsigned char get_adc_data_high_trig();


#ifdef __cplusplus
}
#endif

#endif
