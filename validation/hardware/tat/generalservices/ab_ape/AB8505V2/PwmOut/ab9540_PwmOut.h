/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_PwmOut/Linux/ab9540_PwmOut.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_PWMOUT_LINUX_AB9540_PWMOUT_H
#define C__LISP_PRODUCTION_AB9540_AB9540_PWMOUT_LINUX_AB9540_PWMOUT_H


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_PwmOut_types.h"

unsigned char set_pwm_out1( enum pwm_out1_enable param );
unsigned char get_pwm_out1();
unsigned char set_pwm_out2( enum pwm_out2_enable param );
unsigned char get_pwm_out2();
unsigned char set_pwm_out3( enum pwm_out3_enable param );
unsigned char get_pwm_out3();
unsigned char set_pwm_out3_duty_high( unsigned char param );
unsigned char get_pwm_out3_duty_high();
unsigned char set_pwm3_frequency( enum frequency param );
unsigned char get_pwm3_frequency();
unsigned char set_pwm_out2_duty_high( unsigned char param );
unsigned char get_pwm_out2_duty_high();
unsigned char set_pwm2_frequency( enum frequency param );
unsigned char get_pwm2_frequency();
unsigned char set_pwm_out1_duty_high( unsigned char param );
unsigned char get_pwm_out1_duty_high();
unsigned char set_pwm1_frequency( enum frequency param );
unsigned char get_pwm1_frequency();


#ifdef __cplusplus
}
#endif

#endif
