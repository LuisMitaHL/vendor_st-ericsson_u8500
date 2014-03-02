/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_PwmOut/Linux/ab8500_PwmOut.c
 * 
 *
 * Generated on the 26/02/2010 08:58 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_PwmOut.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_pwm_out1_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut1DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register PwmOut1DutyCycleLow
 *
 * Notes : From register 0x1060, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out1_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(PWM_OUT_1_DUTY_CYCLE_LOW_REG);

    /* 
     * Defined duty clycle Out 2 ( from 1 to 1024 ) 
     */ 
    SPIWrite(PWM_OUT_1_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_pwm_out1_duty_cycle_low
 *
 * RET  : Return the value of register PwmOut1DutyCycleLow
 *
 * Notes : From register 0x1060, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out1_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Defined duty clycle Out 2 ( from 1 to 1024 ) 
     */ 
    value = SPIRead(PWM_OUT_1_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm1_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement2
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement2
 *
 * Notes : From register 0x1061, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm1_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_2_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_1_FREQUENCY_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(PWM_OUT_MANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm1_frequency
 *
 * RET  : Return the value of register PwmOutManagement2
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_2_REG) & FREQUENCY_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out1_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement2
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement2
 *
 * Notes : From register 0x1061, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out1_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_2_REG);

    /* 
     * Defined duty clycle out 1 [9:0] 
     */ 
  
    value =  old_value & ~DUTY_1_PARAM_MASK;


    value |=  param ;  

    SPIWrite(PWM_OUT_MANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out1_duty_high
 *
 * RET  : Return the value of register PwmOutManagement2
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_2_REG) & DUTY_1_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out2_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut2DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register PwmOut2DutyCycleLow
 *
 * Notes : From register 0x1062, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out2_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(PWM_OUT_2_DUTY_CYCLE_LOW_REG);

    /* 
     * Defined duty clycle Out 2 ( from 1 to 1024 ) 
     */ 
    SPIWrite(PWM_OUT_2_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_pwm_out2_duty_cycle_low
 *
 * RET  : Return the value of register PwmOut2DutyCycleLow
 *
 * Notes : From register 0x1062, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out2_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Defined duty clycle Out 2 ( from 1 to 1024 ) 
     */ 
    value = SPIRead(PWM_OUT_2_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm2_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement3
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement3
 *
 * Notes : From register 0x1063, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm2_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_3_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_2_FREQUENCY_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(PWM_OUT_MANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm2_frequency
 *
 * RET  : Return the value of register PwmOutManagement3
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_3_REG) & FREQUENCY_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out2_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement3
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement3
 *
 * Notes : From register 0x1063, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out2_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_3_REG);

    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
  
    value =  old_value & ~DUTY_2_PARAM_MASK;


    value |=  param ;  

    SPIWrite(PWM_OUT_MANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out2_duty_high
 *
 * RET  : Return the value of register PwmOutManagement3
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_3_REG) & DUTY_2_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out3_duty_cycle_low
 *
 * IN   : param, a value to write to the regiter PwmOut3DutyCycleLow
 * OUT  : 
 *
 * RET  : Return the value of register PwmOut3DutyCycleLow
 *
 * Notes : From register 0x1064, bits 7:0
 *
 **************************************************************************/
unsigned char set_pwm_out3_duty_cycle_low( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(PWM_OUT_3_DUTY_CYCLE_LOW_REG);

    /* 
     * Defined duty clycle Out 3 ( from 1 to 1024 ) 
     */ 
    SPIWrite(PWM_OUT_3_DUTY_CYCLE_LOW_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_pwm_out3_duty_cycle_low
 *
 * RET  : Return the value of register PwmOut3DutyCycleLow
 *
 * Notes : From register 0x1064, bits 7:0
 *
 **************************************************************************/
unsigned char get_pwm_out3_duty_cycle_low()
  {
    unsigned char value;


    /* 
     * Defined duty clycle Out 3 ( from 1 to 1024 ) 
     */ 
    value = SPIRead(PWM_OUT_3_DUTY_CYCLE_LOW_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm3_frequency
 *
 * IN   : param, a value to write to the regiter PwmOutManagement5
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1065, bits 7:4
 *
 **************************************************************************/
unsigned char set_pwm3_frequency( enum frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_5_REG);

    /* 
     * Frequency 1(from 293 Hz to 551 Hz) 
     */ 
  

     value =  old_value & ~PWM_3_FREQUENCY_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(PWM_OUT_MANAGEMENT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm3_frequency
 *
 * RET  : Return the value of register PwmOutManagement5
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_5_REG) & FREQUENCY_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out3_duty_high
 *
 * IN   : param, a value to write to the regiter PwmOutManagement5
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1065, bits 1:0
 *
 **************************************************************************/
unsigned char set_pwm_out3_duty_high( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_5_REG);

    /* 
     * Defined duty clycle out 2 [9:0] 
     */ 
  
    value =  old_value & ~DUTY_3_PARAM_MASK;


    value |=  param ;  

    SPIWrite(PWM_OUT_MANAGEMENT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out3_duty_high
 *
 * RET  : Return the value of register PwmOutManagement5
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
    value = (SPIRead(PWM_OUT_MANAGEMENT_5_REG) & DUTY_3_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out3_enable
 *
 * IN   : param, a value to write to the regiter PwmOutManagement5
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 2:2
 *
 **************************************************************************/
unsigned char set_pwm_out3_enable( enum pwm_out3_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_5_REG);

    /* 
     * 0: Disable PWMOut3 generator 
     * 1: Enable PWMOut3 generator 
     */ 
    switch( param )
      {
        case PWM_OUT_3_ENABLE_E: 
           value = old_value | PWM_OUT_3_ENABLE_PARAM_MASK; 
           break;
        case PWM_OUT_3_DISABLE_E: 
           value = old_value & ~ PWM_OUT_3_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(PWM_OUT_MANAGEMENT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out3_enable
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 2:2
 *
 **************************************************************************/
unsigned char get_pwm_out3_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut3 generator 
     * 1: Enable PWMOut3 generator 
     */ 
    value = (SPIRead(PWM_OUT_MANAGEMENT_5_REG) & PWM_OUT_3_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out2_enable
 *
 * IN   : param, a value to write to the regiter PwmOutManagement5
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 1:1
 *
 **************************************************************************/
unsigned char set_pwm_out2_enable( enum pwm_out2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_5_REG);

    /* 
     * 0: Disable PWMOut2 generator 
     * 1: Enable PWMOut2 generator 
     */ 
    switch( param )
      {
        case PWM_OUT_2_ENABLE_E: 
           value = old_value | PWM_OUT_2_ENABLE_PARAM_MASK; 
           break;
        case PWM_OUT_2_DISABLE_E: 
           value = old_value & ~ PWM_OUT_2_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(PWM_OUT_MANAGEMENT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out2_enable
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 1:1
 *
 **************************************************************************/
unsigned char get_pwm_out2_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut2 generator 
     * 1: Enable PWMOut2 generator 
     */ 
    value = (SPIRead(PWM_OUT_MANAGEMENT_5_REG) & PWM_OUT_2_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_out1_enable
 *
 * IN   : param, a value to write to the regiter PwmOutManagement5
 * OUT  : 
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 0:0
 *
 **************************************************************************/
unsigned char set_pwm_out1_enable( enum pwm_out1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PWM_OUT_MANAGEMENT_5_REG);

    /* 
     * 0: Disable PWMOut1 generator 
     * 1: Enable PWMOut1 generator 
     */ 
    switch( param )
      {
        case PWM_OUT_1_ENABLE_E: 
           value = old_value | PWM_OUT_1_ENABLE_PARAM_MASK; 
           break;
        case PWM_OUT_1_DISABLE_E: 
           value = old_value & ~ PWM_OUT_1_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(PWM_OUT_MANAGEMENT_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_out1_enable
 *
 * RET  : Return the value of register PwmOutManagement5
 *
 * Notes : From register 0x1066, bits 0:0
 *
 **************************************************************************/
unsigned char get_pwm_out1_enable()
  {
    unsigned char value;


    /* 
     * 0: Disable PWMOut1 generator 
     * 1: Enable PWMOut1 generator 
     */ 
    value = (SPIRead(PWM_OUT_MANAGEMENT_5_REG) & PWM_OUT_1_ENABLE_PARAM_MASK);
    return value;
  }


