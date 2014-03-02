/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_PwmOut/Linux/ab8500_PwmOut.h
 * 
 *
 * Generated on the 04/04/2011 14:09 by the 'gen-I2C' code generator 
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

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_PWMOUT_LINUX_AB8500_PWMOUT_H
#define C__LISP_PRODUCTION_AB8500_AB8500_PWMOUT_LINUX_AB8500_PWMOUT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab8500_PwmOut_types.h"

unsigned char set_pwm_out1_duty_cycle_low( unsigned char param );
unsigned char get_pwm_out1_duty_cycle_low();
unsigned char set_pwm1_frequency( enum frequency param );
unsigned char get_pwm1_frequency();
unsigned char set_pwm_out1_duty_high( unsigned char param );
unsigned char get_pwm_out1_duty_high();
unsigned char set_pwm_out2_duty_cycle_low( unsigned char param );
unsigned char get_pwm_out2_duty_cycle_low();
unsigned char set_pwm2_frequency( enum frequency param );
unsigned char get_pwm2_frequency();
unsigned char set_pwm_out2_duty_high( unsigned char param );
unsigned char get_pwm_out2_duty_high();
unsigned char set_pwm_out3_duty_cycle_low( unsigned char param );
unsigned char get_pwm_out3_duty_cycle_low();
unsigned char set_pwm3_frequency( enum frequency param );
unsigned char get_pwm3_frequency();
unsigned char set_pwm_out3_duty_high( unsigned char param );
unsigned char get_pwm_out3_duty_high();
unsigned char set_pwm_out3_enable( enum pwm_out3_enable param );
unsigned char get_pwm_out3_enable();
unsigned char set_pwm_out2_enable( enum pwm_out2_enable param );
unsigned char get_pwm_out2_enable();
unsigned char set_pwm_out1_enable( enum pwm_out1_enable param );
unsigned char get_pwm_out1_enable();


#ifdef __cplusplus
}
#endif

#endif
