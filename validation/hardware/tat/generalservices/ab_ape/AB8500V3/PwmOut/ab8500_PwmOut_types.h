/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_PwmOut/Linux/ab8500_PwmOut_types.h
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

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_PWMOUT_LINUX_AB8500_PWMOUT_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_PWMOUT_LINUX_AB8500_PWMOUT_TYPES_H



#define PWM_OUT_1_DUTY_CYCLE_LOW_REG 0x1060
#define  FREQ_293_HZ 0
#define  FREQ_302_HZ 1
#define  FREQ_312_HZ 2
#define  FREQ_323_HZ 3
#define  FREQ_335_HZ 4
#define  FREQ_347_HZ 5
#define  FREQ_361_HZ 6
#define  FREQ_375_HZ 7
#define  FREQ_391_HZ 8
#define  FREQ_408_HZ 9
#define  FREQ_426_HZ 10
#define  FREQ_446_HZ 11
#define  FREQ_469_HZ 12
#define  FREQ_493_HZ 13
#define  FREQ_521_HZ 14
#define  FREQ_551_HZ 15



typedef enum frequency {
    FREQ_293_HZ_E,
    FREQ_302_HZ_E,
    FREQ_312_HZ_E,
    FREQ_323_HZ_E,
    FREQ_335_HZ_E,
    FREQ_347_HZ_E,
    FREQ_361_HZ_E,
    FREQ_375_HZ_E,
    FREQ_391_HZ_E,
    FREQ_408_HZ_E,
    FREQ_426_HZ_E,
    FREQ_446_HZ_E,
    FREQ_469_HZ_E,
    FREQ_493_HZ_E,
    FREQ_521_HZ_E,
    FREQ_551_HZ_E
} FREQUENCY_T ;




//  
//  Frequency 1(from 293 Hz to 551 Hz).
//  Register PwmOutManagement2 0x1061, Bits 4:7, typedef frequency
//   
//  
#define PWM_1_FREQUENCY_MASK 0xF0
#define PWM_1_FREQUENCY_OFFSET 7
#define PWM_1_FREQUENCY_STOP_BIT 7
#define PWM_1_FREQUENCY_START_BIT 4
#define PWM_1_FREQUENCY_WIDTH 4

#define PWM_OUT_MANAGEMENT_2_REG 0x1061
#define PWM_1_FREQUENCY_PARAM_MASK  0xF0
#define FREQUENCY_PARAM_MASK  0xF0

//  
//  Defined duty clycle out 1 [9:0].
//  Register PwmOutManagement2 0x1061, Bits 0:1, typedef duty1
//   
//  
#define PWM_OUT_1_DUTY_HIGH_MASK 0x3
#define PWM_OUT_1_DUTY_HIGH_OFFSET 1
#define PWM_OUT_1_DUTY_HIGH_STOP_BIT 1
#define PWM_OUT_1_DUTY_HIGH_START_BIT 0
#define PWM_OUT_1_DUTY_HIGH_WIDTH 2

#define DUTY_1_PARAM_MASK  0x3
#define PWM_OUT_2_DUTY_CYCLE_LOW_REG 0x1062

//  
//  Frequency 1(from 293 Hz to 551 Hz).
//  Register PwmOutManagement3 0x1063, Bits 4:7, typedef frequency
//   
//  
#define PWM_2_FREQUENCY_MASK 0xF0
#define PWM_2_FREQUENCY_OFFSET 7
#define PWM_2_FREQUENCY_STOP_BIT 7
#define PWM_2_FREQUENCY_START_BIT 4
#define PWM_2_FREQUENCY_WIDTH 4

#define PWM_OUT_MANAGEMENT_3_REG 0x1063
#define PWM_2_FREQUENCY_PARAM_MASK  0xF0

//  
//  Defined duty clycle out 2 [9:0].
//  Register PwmOutManagement3 0x1063, Bits 0:1, typedef duty2
//   
//  
#define PWM_OUT_2_DUTY_HIGH_MASK 0x3
#define PWM_OUT_2_DUTY_HIGH_OFFSET 1
#define PWM_OUT_2_DUTY_HIGH_STOP_BIT 1
#define PWM_OUT_2_DUTY_HIGH_START_BIT 0
#define PWM_OUT_2_DUTY_HIGH_WIDTH 2

#define DUTY_2_PARAM_MASK  0x3
#define PWM_OUT_3_DUTY_CYCLE_LOW_REG 0x1064

//  
//  Frequency 1(from 293 Hz to 551 Hz).
//  Register PwmOutManagement5 0x1065, Bits 4:7, typedef frequency
//   
//  
#define PWM_3_FREQUENCY_MASK 0xF0
#define PWM_3_FREQUENCY_OFFSET 7
#define PWM_3_FREQUENCY_STOP_BIT 7
#define PWM_3_FREQUENCY_START_BIT 4
#define PWM_3_FREQUENCY_WIDTH 4

#define PWM_OUT_MANAGEMENT_5_REG 0x1065
#define PWM_3_FREQUENCY_PARAM_MASK  0xF0

//  
//  Defined duty clycle out 2 [9:0].
//  Register PwmOutManagement5 0x1065, Bits 0:1, typedef duty3
//   
//  
#define PWM_OUT_3_DUTY_HIGH_MASK 0x3
#define PWM_OUT_3_DUTY_HIGH_OFFSET 1
#define PWM_OUT_3_DUTY_HIGH_STOP_BIT 1
#define PWM_OUT_3_DUTY_HIGH_START_BIT 0
#define PWM_OUT_3_DUTY_HIGH_WIDTH 2

#define DUTY_3_PARAM_MASK  0x3

//  
//  0: Disable PWMOut3 generator 
//  1: Enable PWMOut3 generator.
//  Register PwmOutManagement6 0x1066, Bits 2:2, typedef pwm_out3_enable
//   
//  
#define PWM_OUT_3_ENABLE_MASK 0x4
#define PWM_OUT_3_ENABLE_OFFSET 2
#define PWM_OUT_3_ENABLE_STOP_BIT 2
#define PWM_OUT_3_ENABLE_START_BIT 2
#define PWM_OUT_3_ENABLE_WIDTH 1

#define PWM_OUT_MANAGEMENT_6_REG 0x1066

typedef enum pwm_out3_enable {
    PWM_OUT_3_DISABLE_E,
    PWM_OUT_3_ENABLE_E
} PWM_OUT_3_ENABLE_T ;

#define PWM_OUT_3_ENABLE_PARAM_MASK  0x4

//  
//  0: Disable PWMOut2 generator 
//  1: Enable PWMOut2 generator.
//  Register PwmOutManagement6 0x1066, Bits 1:1, typedef pwm_out2_enable
//   
//  
#define PWM_OUT_2_ENABLE_MASK 0x2
#define PWM_OUT_2_ENABLE_OFFSET 1
#define PWM_OUT_2_ENABLE_STOP_BIT 1
#define PWM_OUT_2_ENABLE_START_BIT 1
#define PWM_OUT_2_ENABLE_WIDTH 1


typedef enum pwm_out2_enable {
    PWM_OUT_2_DISABLE_E,
    PWM_OUT_2_ENABLE_E
} PWM_OUT_2_ENABLE_T ;

#define PWM_OUT_2_ENABLE_PARAM_MASK  0x2

//  
//  0: Disable PWMOut1 generator 
//  1: Enable PWMOut1 generator.
//  Register PwmOutManagement6 0x1066, Bits 0:0, typedef pwm_out1_enable
//   
//  
#define PWM_OUT_1_ENABLE_MASK 0x1
#define PWM_OUT_1_ENABLE_OFFSET 0
#define PWM_OUT_1_ENABLE_STOP_BIT 0
#define PWM_OUT_1_ENABLE_START_BIT 0
#define PWM_OUT_1_ENABLE_WIDTH 1


typedef enum pwm_out1_enable {
    PWM_OUT_1_DISABLE_E,
    PWM_OUT_1_ENABLE_E
} PWM_OUT_1_ENABLE_T ;

#define PWM_OUT_1_ENABLE_PARAM_MASK  0x1
#endif
