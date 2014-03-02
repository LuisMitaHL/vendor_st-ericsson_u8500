/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_ADC/Linux/ab9540_ADC.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_ADC_LINUX_AB9540_ADC_H
#define C__LISP_PRODUCTION_AB9540_AB9540_ADC_LINUX_AB9540_ADC_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_ADC_types.h"

unsigned char get_adc_busy();
unsigned char set_adc_select_input( enum adc_input_channel param );
unsigned char get_adc_select_input();
unsigned char set_sample_to_average( enum sample_to_average param );
unsigned char get_sample_to_average();
unsigned char set_adc_select_input_tric( enum adc_input_channel param );
unsigned char get_adc_select_input_tric();
unsigned char set_sample_to_average_trig( enum sample_to_average param );
unsigned char get_sample_to_average_trig();
unsigned char set_adc_time_delay( unsigned char param );
unsigned char get_adc_time_delay();
unsigned char set_adc( enum adc_enable param );
unsigned char get_adc();
unsigned char set_adc_trigger( enum adc_trigger_enable param );
unsigned char get_adc_trigger();
unsigned char set_adc_man_convert( enum adc_man_convert_enable param );
unsigned char get_adc_man_convert();
unsigned char set_adc_auto_req( enum adc_auto_req_enable param );
unsigned char get_adc_auto_req();
unsigned char set_adc_trig_edge( enum adc_trig_edge_enable param );
unsigned char get_adc_trig_edge();
unsigned char set_pull_up_supply( enum pull_up_supply param );
unsigned char get_pull_up_supply();
unsigned char set_adc_buffer( enum adc_buffer_enable param );
unsigned char get_adc_buffer();
unsigned char set_charging_current_sense( enum charging_current_sense_enable param );
unsigned char get_charging_current_sense();


#ifdef __cplusplus
}
#endif

#endif
