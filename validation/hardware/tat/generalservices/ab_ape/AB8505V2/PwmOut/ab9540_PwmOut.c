/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_PwmOut/Linux/ab9540_PwmOut.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_PWMOUT_LINUX_AB9540_PWMOUT_C
#define C__LISP_PRODUCTION_AB9540_AB9540_PWMOUT_LINUX_AB9540_PWMOUT_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_PwmOut.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_pwm_out1_duty_cycle_low
 *
 * RET  : Return the value of register 0xPwmOut1DutyCycleLow
 *
 * Notes : From register 0x1060, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out1_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(PWM_OUT_1_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out1_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut1DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOut1DutyCycleLow
 *
 * Notes : From register 0x1060, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out1_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(PWM_OUT_1_DUTY_CYCLE_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(PWM_OUT_1_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_pwm_out1_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement2
 *
 * Notes : From register 0x1061, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out1_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_2_REG);

    /* 
     * Defined duty clycle out 1 [9:0] 
     */ 
  
    value =  old_value & ~PWM_OUT_1_DUTY_HIGH_MASK;


    value |=  param ;  

    I2CWrite(PWM_OUT_MANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out1_duty_high
 *
 * RET  : Return the value of register 0xPwmOutManagement2
 *
 * Notes : From register 0x1061, bits 1:0
 *
 **************************************************************************/
unsigned char get_pwm_out1_duty_high()
  {
    unsigned char value;


    /* 
     * Defined duty clycle out 1 [9:0] 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_2_REG) & PWM_OUT_1_DUTY_HIGH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm1_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement2
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement2
 *
 * Notes : From register 0x1061, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm1_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_2_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_1_FREQUENCY_MASK ;

    switch(  param ){

           case FREQ_293_HZ_E:
                value  = value  | (FREQ_293_HZ << 0x4);
                break;
           case FREQ_302_HZ_E:
                value  = value  | (FREQ_302_HZ << 0x4);
                break;
           case FREQ_312_HZ_E:
                value  = value  | (FREQ_312_HZ << 0x4);
                break;
           case FREQ_323_HZ_E:
                value  = value  | (FREQ_323_HZ << 0x4);
                break;
           case FREQ_335_HZ_E:
                value  = value  | (FREQ_335_HZ << 0x4);
                break;
           case FREQ_347_HZ_E:
                value  = value  | (FREQ_347_HZ << 0x4);
                break;
           case FREQ_361_HZ_E:
                value  = value  | (FREQ_361_HZ << 0x4);
                break;
           case FREQ_375_HZ_E:
                value  = value  | (FREQ_375_HZ << 0x4);
                break;
           case FREQ_391_HZ_E:
                value  = value  | (FREQ_391_HZ << 0x4);
                break;
           case FREQ_408_HZ_E:
                value  = value  | (FREQ_408_HZ << 0x4);
                break;
           case FREQ_426_HZ_E:
                value  = value  | (FREQ_426_HZ << 0x4);
                break;
           case FREQ_446_HZ_E:
                value  = value  | (FREQ_446_HZ << 0x4);
                break;
           case FREQ_469_HZ_E:
                value  = value  | (FREQ_469_HZ << 0x4);
                break;
           case FREQ_493_HZ_E:
                value  = value  | (FREQ_493_HZ << 0x4);
                break;
           case FREQ_521_HZ_E:
                value  = value  | (FREQ_521_HZ << 0x4);
                break;
           case FREQ_551_HZ_E:
                value  = value  | (FREQ_551_HZ << 0x4);
                break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm1_frequency
 *
 * RET  : Return the value of register 0xPwmOutManagement2
 *
 * Notes : From register 0x1061, bits 7:4
 *
 **************************************************************************/
unsigned char get_pwm1_frequency()
  {
    unsigned char value;


    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_2_REG) & PWM_1_FREQUENCY_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out2_duty_cycle_low
 *
 * RET  : Return the value of register 0xPwmOut2DutyCycleLow
 *
 * Notes : From register 0x1062, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out2_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(PWM_OUT_2_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out2_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut2DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOut2DutyCycleLow
 *
 * Notes : From register 0x1062, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out2_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(PWM_OUT_2_DUTY_CYCLE_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(PWM_OUT_2_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_pwm_out2_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement4
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement4
 *
 * Notes : From register 0x1063, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out2_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_4_REG);

    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
  
    value =  old_value & ~PWM_OUT_2_DUTY_HIGH_MASK;


    value |=  param ;  

    I2CWrite(PWM_OUT_MANAGEMENT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out2_duty_high
 *
 * RET  : Return the value of register 0xPwmOutManagement4
 *
 * Notes : From register 0x1063, bits 1:0
 *
 **************************************************************************/
unsigned char get_pwm_out2_duty_high()
  {
    unsigned char value;


    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_4_REG) & PWM_OUT_2_DUTY_HIGH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm2_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement4
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement4
 *
 * Notes : From register 0x1063, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm2_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_4_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_2_FREQUENCY_MASK ;

    switch(  param ){

           case FREQ_293_HZ_E:
                value  = value  | (FREQ_293_HZ << 0x4);
                break;
           case FREQ_302_HZ_E:
                value  = value  | (FREQ_302_HZ << 0x4);
                break;
           case FREQ_312_HZ_E:
                value  = value  | (FREQ_312_HZ << 0x4);
                break;
           case FREQ_323_HZ_E:
                value  = value  | (FREQ_323_HZ << 0x4);
                break;
           case FREQ_335_HZ_E:
                value  = value  | (FREQ_335_HZ << 0x4);
                break;
           case FREQ_347_HZ_E:
                value  = value  | (FREQ_347_HZ << 0x4);
                break;
           case FREQ_361_HZ_E:
                value  = value  | (FREQ_361_HZ << 0x4);
                break;
           case FREQ_375_HZ_E:
                value  = value  | (FREQ_375_HZ << 0x4);
                break;
           case FREQ_391_HZ_E:
                value  = value  | (FREQ_391_HZ << 0x4);
                break;
           case FREQ_408_HZ_E:
                value  = value  | (FREQ_408_HZ << 0x4);
                break;
           case FREQ_426_HZ_E:
                value  = value  | (FREQ_426_HZ << 0x4);
                break;
           case FREQ_446_HZ_E:
                value  = value  | (FREQ_446_HZ << 0x4);
                break;
           case FREQ_469_HZ_E:
                value  = value  | (FREQ_469_HZ << 0x4);
                break;
           case FREQ_493_HZ_E:
                value  = value  | (FREQ_493_HZ << 0x4);
                break;
           case FREQ_521_HZ_E:
                value  = value  | (FREQ_521_HZ << 0x4);
                break;
           case FREQ_551_HZ_E:
                value  = value  | (FREQ_551_HZ << 0x4);
                break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm2_frequency
 *
 * RET  : Return the value of register 0xPwmOutManagement4
 *
 * Notes : From register 0x1063, bits 7:4
 *
 **************************************************************************/
unsigned char get_pwm2_frequency()
  {
    unsigned char value;


    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_4_REG) & PWM_2_FREQUENCY_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out3_duty_cycle_low
 *
 * RET  : Return the value of register 0xPwmOut3DutyCycleLow
 *
 * Notes : From register 0x1064, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out3_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(PWM_OUT_3_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out3_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut3DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOut3DutyCycleLow
 *
 * Notes : From register 0x1064, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out3_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(PWM_OUT_3_DUTY_CYCLE_LOW_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(PWM_OUT_3_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_pwm_out3_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement6
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement6
 *
 * Notes : From register 0x1065, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out3_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_6_REG);

    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
  
    value =  old_value & ~PWM_OUT_3_DUTY_HIGH_MASK;


    value |=  param ;  

    I2CWrite(PWM_OUT_MANAGEMENT_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out3_duty_high
 *
 * RET  : Return the value of register 0xPwmOutManagement6
 *
 * Notes : From register 0x1065, bits 1:0
 *
 **************************************************************************/
unsigned char get_pwm_out3_duty_high()
  {
    unsigned char value;


    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_6_REG) & PWM_OUT_3_DUTY_HIGH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm3_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement6
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement6
 *
 * Notes : From register 0x1065, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm3_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_6_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_3_FREQUENCY_MASK ;

    switch(  param ){

           case FREQ_293_HZ_E:
                value  = value  | (FREQ_293_HZ << 0x4);
                break;
           case FREQ_302_HZ_E:
                value  = value  | (FREQ_302_HZ << 0x4);
                break;
           case FREQ_312_HZ_E:
                value  = value  | (FREQ_312_HZ << 0x4);
                break;
           case FREQ_323_HZ_E:
                value  = value  | (FREQ_323_HZ << 0x4);
                break;
           case FREQ_335_HZ_E:
                value  = value  | (FREQ_335_HZ << 0x4);
                break;
           case FREQ_347_HZ_E:
                value  = value  | (FREQ_347_HZ << 0x4);
                break;
           case FREQ_361_HZ_E:
                value  = value  | (FREQ_361_HZ << 0x4);
                break;
           case FREQ_375_HZ_E:
                value  = value  | (FREQ_375_HZ << 0x4);
                break;
           case FREQ_391_HZ_E:
                value  = value  | (FREQ_391_HZ << 0x4);
                break;
           case FREQ_408_HZ_E:
                value  = value  | (FREQ_408_HZ << 0x4);
                break;
           case FREQ_426_HZ_E:
                value  = value  | (FREQ_426_HZ << 0x4);
                break;
           case FREQ_446_HZ_E:
                value  = value  | (FREQ_446_HZ << 0x4);
                break;
           case FREQ_469_HZ_E:
                value  = value  | (FREQ_469_HZ << 0x4);
                break;
           case FREQ_493_HZ_E:
                value  = value  | (FREQ_493_HZ << 0x4);
                break;
           case FREQ_521_HZ_E:
                value  = value  | (FREQ_521_HZ << 0x4);
                break;
           case FREQ_551_HZ_E:
                value  = value  | (FREQ_551_HZ << 0x4);
                break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm3_frequency
 *
 * RET  : Return the value of register 0xPwmOutManagement6
 *
 * Notes : From register 0x1065, bits 7:4
 *
 **************************************************************************/
unsigned char get_pwm3_frequency()
  {
    unsigned char value;


    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_6_REG) & PWM_3_FREQUENCY_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out1
 *
 * IN   : param, a value to write to the regiter PwmOutManagement7
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 0:0
 *
 **************************************************************************/
unsigned char set_pwm_out1( enum pwm_out1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_7_REG);

    /* 
     * 0: Disable PWMOut1 generator 
     * 1: Enable PWMOut1 generator 
     */ 
    switch( param ){
        case PWM_OUT_1_ENABLE_E: 
           value = old_value | PWM_OUT_1_MASK; 
           break;
        case PWM_OUT_1_DISABLE_E: 
           value = old_value & ~ PWM_OUT_1_MASK;
           break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out1
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 0:0
 *
 **************************************************************************/
unsigned char get_pwm_out1()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut1 generator 
     * 1: Enable PWMOut1 generator 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_7_REG) & PWM_OUT_1_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out2
 *
 * IN   : param, a value to write to the regiter PwmOutManagement7
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 1:1
 *
 **************************************************************************/
unsigned char set_pwm_out2( enum pwm_out2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_7_REG);

    /* 
     * 0: Disable PWMOut2 generator 
     * 1: Enable PWMOut2 generator 
     */ 
    switch( param ){
        case PWM_OUT_2_ENABLE_E: 
           value = old_value | PWM_OUT_2_MASK; 
           break;
        case PWM_OUT_2_DISABLE_E: 
           value = old_value & ~ PWM_OUT_2_MASK;
           break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out2
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 1:1
 *
 **************************************************************************/
unsigned char get_pwm_out2()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut2 generator 
     * 1: Enable PWMOut2 generator 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_7_REG) & PWM_OUT_2_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out3
 *
 * IN   : param, a value to write to the regiter PwmOutManagement7
 * OUT  : 
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 2:2
 *
 **************************************************************************/
unsigned char set_pwm_out3( enum pwm_out3_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PWM_OUT_MANAGEMENT_7_REG);

    /* 
     * 0: Disable PWMOut3 generator 
     * 1: Enable PWMOut3 generator 
     */ 
    switch( param ){
        case PWM_OUT_3_ENABLE_E: 
           value = old_value | PWM_OUT_3_MASK; 
           break;
        case PWM_OUT_3_DISABLE_E: 
           value = old_value & ~ PWM_OUT_3_MASK;
           break;
    }
  

    I2CWrite(PWM_OUT_MANAGEMENT_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out3
 *
 * RET  : Return the value of register 0xPwmOutManagement7
 *
 * Notes : From register 0x1066, bits 2:2
 *
 **************************************************************************/
unsigned char get_pwm_out3()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut3 generator 
     * 1: Enable PWMOut3 generator 
     */ 
    value = (I2CRead(PWM_OUT_MANAGEMENT_7_REG) & PWM_OUT_3_MASK) >> 2;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
