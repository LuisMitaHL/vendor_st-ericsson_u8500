/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_GPIO/Linux/ab8500_GPIO_types.h
 * 
 *
 * Generated on the 25/02/2010 15:30 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

/****************************************************************************
 * Modifications:
 *  CAP_948_001: 22 March 2010 by Karine Boclaud
 *
 ***************************************************************************/

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_GPIO_LINUX_AB8500_GPIO_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_GPIO_LINUX_AB8500_GPIO_TYPES_H



#define GPIO_PAD_CONTROL_1_REG 0x1000
#define GPIO_PAD_CONTROL_2_REG 0x1001
#define GPIO_PAD_CONTROL_3_REG 0x1002
#define GPIO_PAD_CONTROL_4_REG 0x1003
#define GPIO_PAD_CONTROL_5_REG 0x1004
#define GPIO_PAD_CONTROL_6_REG 0x1005
#define GPIO_DIRECTION_1_REG 0x1010
#define GPIO_DIRECTION_2_REG 0x1011
#define GPIO_DIRECTION_3_REG 0x1012
#define GPIO_DIRECTION_4_REG 0x1013
#define GPIO_DIRECTION_5_REG 0x1014
#define GPIO_DIRECTION_6_REG 0x1015
#define GPIO_SET_OUPUT_1_REG 0x1020
#define GPIO_SET_OUPUT_2_REG 0x1021
#define GPIO_SET_OUPUT_3_REG 0x1022
#define GPIO_SET_OUPUT_4_REG 0x1023
#define GPIO_SET_OUPUT_5_REG 0x1024
#define GPIO_SET_OUPUT_6_REG 0x1025
#define GPIO_PULL_DOWN_1_REG 0x1030
#define GPIO_PULL_DOWN_2_REG 0x1031
#define GPIO_PULL_DOWN_3_REG 0x1032
#define GPIO_PULL_DOWN_4_REG 0x1033
#define GPIO_PULL_DOWN_5_REG 0x1034
#define GPIO_PULL_DOWN_6_REG 0x1035
#define GPIO_GET_INPUT_1_REG 0x1040
#define GPIO_GET_INPUT_2_REG 0x1041
#define GPIO_GET_INPUT_3_REG 0x1042
#define GPIO_GET_INPUT_4_REG 0x1043
#define GPIO_GET_INPUT_5_REG 0x1044
#define GPIO_GET_INPUT_6_REG 0x1045

typedef enum gpio_direction {
    GPIO_INPUT_E,
    GPIO_OUTPUT_E
} GPIO_DIRECTION_T ;



/*+CAP_948_001*/
typedef enum gpio_pullup {
    GPIO_PULLUP_ENABLE_E,
    GPIO_PULLUP_DISABLE_E
} GPIO_PULLUP_T ;
/*-CAP_948_001*/



typedef enum gpio_pulldown {
    GPIO_PULLDOWN_ENABLE_E,
    GPIO_PULLDOWN_DISABLE_E
} GPIO_PULLDOWN_T ;



typedef enum gpio_output {
    GPIO_SET_0_E,
    GPIO_SET_1_E
} GPIO_OUTPUT_T ;



typedef enum gpio_pad_selection {
    USE_FUNCTION_E,
    USE_PAD_AS_GPIO_E
} GPIO_PAD_SELECTION_T ;



//  
//  0: Pad is used as YCbCr2 
//  1: Pad is used as GPIO8.
//  Register GpioPadSelection1 0x1000, Bits 7:7, typedef gpio_pad_selection
//   
//  
#define GPIO_8_PAD_SELECTION_MASK 0x80
#define GPIO_8_PAD_SELECTION_OFFSET 7
#define GPIO_8_PAD_SELECTION_STOP_BIT 7
#define GPIO_8_PAD_SELECTION_START_BIT 7
#define GPIO_8_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_1_REG 0x1000
#define GPIO_8_PAD_SELECTION_PARAM_MASK  0x80
#define GPIO_PAD_SELECTION_PARAM_MASK  0x80

//  
//  0: Pad is used as YCbCr1 
//  1: Pad is used as GPIO7.
//  Register GpioPadSelection1 0x1000, Bits 6:6, typedef gpio_pad_selection
//   
//  
#define GPIO_7_PAD_SELECTION_MASK 0x40
#define GPIO_7_PAD_SELECTION_OFFSET 6
#define GPIO_7_PAD_SELECTION_STOP_BIT 6
#define GPIO_7_PAD_SELECTION_START_BIT 6
#define GPIO_7_PAD_SELECTION_WIDTH 1

#define GPIO_7_PAD_SELECTION_PARAM_MASK  0x40

//  
//  0: Pad is used as YCbCr0 
//  1: Pad is used as GPIO6.
//  Register GpioPadSelection1 0x1000, Bits 5:5, typedef gpio_pad_selection
//   
//  
#define GPIO_6_PAD_SELECTION_MASK 0x20
#define GPIO_6_PAD_SELECTION_OFFSET 5
#define GPIO_6_PAD_SELECTION_STOP_BIT 5
#define GPIO_6_PAD_SELECTION_START_BIT 5
#define GPIO_6_PAD_SELECTION_WIDTH 1

#define GPIO_6_PAD_SELECTION_PARAM_MASK  0x20

//  
//  0: Pad is used as SysClkReq6 
//  1: Pad is used as GPIO4.
//  Register GpioPadSelection1 0x1000, Bits 3:3, typedef gpio_pad_selection
//   
//  
#define GPIO_4_PAD_SELECTION_MASK 0x8
#define GPIO_4_PAD_SELECTION_OFFSET 3
#define GPIO_4_PAD_SELECTION_STOP_BIT 3
#define GPIO_4_PAD_SELECTION_START_BIT 3
#define GPIO_4_PAD_SELECTION_WIDTH 1

#define GPIO_4_PAD_SELECTION_PARAM_MASK  0x8

//  
//  0: Pad is used as SysClkReq4 
//  1: Pad is used as GPIO3.
//  Register GpioPadSelection1 0x1000, Bits 2:2, typedef gpio_pad_selection
//   
//  
#define GPIO_3_PAD_SELECTION_MASK 0x4
#define GPIO_3_PAD_SELECTION_OFFSET 2
#define GPIO_3_PAD_SELECTION_STOP_BIT 2
#define GPIO_3_PAD_SELECTION_START_BIT 2
#define GPIO_3_PAD_SELECTION_WIDTH 1

#define GPIO_3_PAD_SELECTION_PARAM_MASK  0x4

//  
//  0: Pad is used as SysClkReq3 
//  1: Pad is used as GPIO2.
//  Register GpioPadSelection1 0x1000, Bits 1:1, typedef gpio_pad_selection
//   
//  
#define GPIO_2_PAD_SELECTION_MASK 0x2
#define GPIO_2_PAD_SELECTION_OFFSET 1
#define GPIO_2_PAD_SELECTION_STOP_BIT 1
#define GPIO_2_PAD_SELECTION_START_BIT 1
#define GPIO_2_PAD_SELECTION_WIDTH 1

#define GPIO_2_PAD_SELECTION_PARAM_MASK  0x2

//  
//  0: Pad is used as SysClkReq2 
//  1: Pad is used as GPIO1.
//  Register GpioPadSelection1 0x1000, Bits 0:0, typedef gpio_pad_selection
//   
//  
#define GPIO_1_PAD_SELECTION_MASK 0x1
#define GPIO_1_PAD_SELECTION_OFFSET 0
#define GPIO_1_PAD_SELECTION_STOP_BIT 0
#define GPIO_1_PAD_SELECTION_START_BIT 0
#define GPIO_1_PAD_SELECTION_WIDTH 1

#define GPIO_1_PAD_SELECTION_PARAM_MASK  0x1

//  
//  0: Pad is used as PWMOut3 
//  1: Pad is used as GPIO16.
//  Register GpioPadSelection2 0x1001, Bits 7:7, typedef gpio_pad_selection
//   
//  
#define GPIO_16_PAD_SELECTION_MASK 0x80
#define GPIO_16_PAD_SELECTION_OFFSET 7
#define GPIO_16_PAD_SELECTION_STOP_BIT 7
#define GPIO_16_PAD_SELECTION_START_BIT 7
#define GPIO_16_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_2_REG 0x1001
#define GPIO_16_PAD_SELECTION_PARAM_MASK  0x80

//  
//  0: Pad is used as PWMOut2 
//  1: Pad is used as GPIO15.
//  Register GpioPadSelection2 0x1001, Bits 6:6, typedef gpio_pad_selection
//   
//  
#define GPIO_15_PAD_SELECTION_MASK 0x40
#define GPIO_15_PAD_SELECTION_OFFSET 6
#define GPIO_15_PAD_SELECTION_STOP_BIT 6
#define GPIO_15_PAD_SELECTION_START_BIT 6
#define GPIO_15_PAD_SELECTION_WIDTH 1

#define GPIO_15_PAD_SELECTION_PARAM_MASK  0x40

//  
//  0: Pad is used as PWMOut1 
//  1: Pad is used as GPIO14.
//  Register GpioPadSelection2 0x1001, Bits 5:5, typedef gpio_pad_selection
//   
//  
#define GPIO_14_PAD_SELECTION_MASK 0x20
#define GPIO_14_PAD_SELECTION_OFFSET 5
#define GPIO_14_PAD_SELECTION_STOP_BIT 5
#define GPIO_14_PAD_SELECTION_START_BIT 5
#define GPIO_14_PAD_SELECTION_WIDTH 1

#define GPIO_14_PAD_SELECTION_PARAM_MASK  0x20

//  
//  0: Pad is used as YCbCr7 
//  1: Pad is used as GPIO13.
//  Register GpioPadSelection2 0x1001, Bits 4:4, typedef gpio_pad_selection
//   
//  
#define GPIO_13_PAD_SELECTION_MASK 0x10
#define GPIO_13_PAD_SELECTION_OFFSET 4
#define GPIO_13_PAD_SELECTION_STOP_BIT 4
#define GPIO_13_PAD_SELECTION_START_BIT 4
#define GPIO_13_PAD_SELECTION_WIDTH 1

#define GPIO_13_PAD_SELECTION_PARAM_MASK  0x10

//  
//  0: Pad is used as YCbCr6 
//  1: Pad is used as GPIO12.
//  Register GpioPadSelection2 0x1001, Bits 3:3, typedef gpio_pad_selection
//   
//  
#define GPIO_12_PAD_SELECTION_MASK 0x8
#define GPIO_12_PAD_SELECTION_OFFSET 3
#define GPIO_12_PAD_SELECTION_STOP_BIT 3
#define GPIO_12_PAD_SELECTION_START_BIT 3
#define GPIO_12_PAD_SELECTION_WIDTH 1

#define GPIO_12_PAD_SELECTION_PARAM_MASK  0x8

//  
//  0: Pad is used as alternate function 
//  1: Pad is used as GPIO11.
//  Register GpioPadSelection2 0x1001, Bits 2:2, typedef gpio_pad_selection
//   
//  
#define GPIO_11_PAD_SELECTION_MASK 0x4
#define GPIO_11_PAD_SELECTION_OFFSET 2
#define GPIO_11_PAD_SELECTION_STOP_BIT 2
#define GPIO_11_PAD_SELECTION_START_BIT 2
#define GPIO_11_PAD_SELECTION_WIDTH 1

#define GPIO_11_PAD_SELECTION_PARAM_MASK  0x4

//  
//  0: Pad is used as alternate function 
//  1: Pad is used as GPIO10.
//  Register GpioPadSelection2 0x1001, Bits 1:1, typedef gpio_pad_selection
//   
//  
#define GPIO_10_PAD_SELECTION_MASK 0x2
#define GPIO_10_PAD_SELECTION_OFFSET 1
#define GPIO_10_PAD_SELECTION_STOP_BIT 1
#define GPIO_10_PAD_SELECTION_START_BIT 1
#define GPIO_10_PAD_SELECTION_WIDTH 1

#define GPIO_10_PAD_SELECTION_PARAM_MASK  0x2

//  
//  0: Pad is used as YCbCr3 
//  1: Pad is used as GPIO9.
//  Register GpioPadSelection2 0x1001, Bits 0:0, typedef gpio_pad_selection
//   
//  
#define GPIO_9_PAD_SELECTION_MASK 0x1
#define GPIO_9_PAD_SELECTION_OFFSET 0
#define GPIO_9_PAD_SELECTION_STOP_BIT 0
#define GPIO_9_PAD_SELECTION_START_BIT 0
#define GPIO_9_PAD_SELECTION_WIDTH 1

#define GPIO_9_PAD_SELECTION_PARAM_MASK  0x1

//  
//  0: Pad is used as SysClkReq7 
//  1: Pad is used as GPIO24.
//  Register GpioPadSelection3 0x1002, Bits 7:7, typedef gpio_pad_selection
//   
//  
#define GPIO_24_PAD_SELECTION_MASK 0x80
#define GPIO_24_PAD_SELECTION_OFFSET 7
#define GPIO_24_PAD_SELECTION_STOP_BIT 7
#define GPIO_24_PAD_SELECTION_START_BIT 7
#define GPIO_24_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_3_REG 0x1002
#define GPIO_24_PAD_SELECTION_PARAM_MASK  0x80

//  
//  0: Pad is used as UsbUiccSe0 
//  1: Pad is used as GPIO23.
//  Register GpioPadSelection3 0x1002, Bits 6:6, typedef gpio_pad_selection
//   
//  
#define GPIO_23_PAD_SELECTION_MASK 0x40
#define GPIO_23_PAD_SELECTION_OFFSET 6
#define GPIO_23_PAD_SELECTION_STOP_BIT 6
#define GPIO_23_PAD_SELECTION_START_BIT 6
#define GPIO_23_PAD_SELECTION_WIDTH 1

#define GPIO_23_PAD_SELECTION_PARAM_MASK  0x40

//  
//  0: Pad is used as UsbUiccData 
//  1: Pad is used as GPIO22.
//  Register GpioPadSelection3 0x1002, Bits 5:5, typedef gpio_pad_selection
//   
//  
#define GPIO_22_PAD_SELECTION_MASK 0x20
#define GPIO_22_PAD_SELECTION_OFFSET 5
#define GPIO_22_PAD_SELECTION_STOP_BIT 5
#define GPIO_22_PAD_SELECTION_START_BIT 5
#define GPIO_22_PAD_SELECTION_WIDTH 1

#define GPIO_22_PAD_SELECTION_PARAM_MASK  0x20

//  
//  0: Pad is used as UsbUiccDir 
//  1: Pad is used as GPIO21.
//  Register GpioPadSelection3 0x1002, Bits 4:4, typedef gpio_pad_selection
//   
//  
#define GPIO_21_PAD_SELECTION_MASK 0x10
#define GPIO_21_PAD_SELECTION_OFFSET 4
#define GPIO_21_PAD_SELECTION_STOP_BIT 4
#define GPIO_21_PAD_SELECTION_START_BIT 4
#define GPIO_21_PAD_SELECTION_WIDTH 1

#define GPIO_21_PAD_SELECTION_PARAM_MASK  0x10

//  
//  0: Pad is used as FSync1, BitClk1, DA_DATA1 and AD_Date1 
//  1: GPIO17_18_19_20 direction is output.
//  Register GpioPadSelection3 0x1002, Bits 0:0, typedef gpio_pad_selection
//   
//  
#define GPIO_1720_PAD_SELECTION_MASK 0x1
#define GPIO_1720_PAD_SELECTION_OFFSET 0
#define GPIO_1720_PAD_SELECTION_STOP_BIT 0
#define GPIO_1720_PAD_SELECTION_START_BIT 0
#define GPIO_1720_PAD_SELECTION_WIDTH 1

#define GPIO_1720_PAD_SELECTION_PARAM_MASK  0x1

//  
//  0: select Dmic56Clk and Dmic56Dat 
//  1: select GPIO31 and GPIO32.
//  Register GpioPadSelection4 0x1003, Bits 6:6, typedef gpio_pad_selection
//   
//  
#define GPIO_3132_PAD_SELECTION_MASK 0x40
#define GPIO_3132_PAD_SELECTION_OFFSET 6
#define GPIO_3132_PAD_SELECTION_STOP_BIT 6
#define GPIO_3132_PAD_SELECTION_START_BIT 6
#define GPIO_3132_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_4_REG 0x1003
#define GPIO_3132_PAD_SELECTION_PARAM_MASK  0x40

//  
//  0: select Dmic34Clk and Dmic34Dat 
//  1: select GPIO29 and GPIO30.
//  Register GpioPadSelection4 0x1003, Bits 4:4, typedef gpio_pad_selection
//   
//  
#define GPIO_2930_PAD_SELECTION_MASK 0x10
#define GPIO_2930_PAD_SELECTION_OFFSET 4
#define GPIO_2930_PAD_SELECTION_STOP_BIT 4
#define GPIO_2930_PAD_SELECTION_START_BIT 4
#define GPIO_2930_PAD_SELECTION_WIDTH 1

#define GPIO_2930_PAD_SELECTION_PARAM_MASK  0x10

//  
//  0: select Dmic12Clk and Dmic12Dat 
//  1: select GPIO27 and GPIO28.
//  Register GpioPadSelection4 0x1003, Bits 2:2, typedef gpio_pad_selection
//   
//  
#define GPIO_2728_PAD_SELECTION_MASK 0x4
#define GPIO_2728_PAD_SELECTION_OFFSET 2
#define GPIO_2728_PAD_SELECTION_STOP_BIT 2
#define GPIO_2728_PAD_SELECTION_START_BIT 2
#define GPIO_2728_PAD_SELECTION_WIDTH 1

#define GPIO_2728_PAD_SELECTION_PARAM_MASK  0x4

//  
//  0: Pad is used as SysClkReq8 
//  1: Pad is used as GPIO25.
//  Register GpioPadSelection4 0x1003, Bits 0:0, typedef gpio_pad_selection
//   
//  
#define GPIO_25_PAD_SELECTION_MASK 0x1
#define GPIO_25_PAD_SELECTION_OFFSET 0
#define GPIO_25_PAD_SELECTION_STOP_BIT 0
#define GPIO_25_PAD_SELECTION_START_BIT 0
#define GPIO_25_PAD_SELECTION_WIDTH 1

#define GPIO_25_PAD_SELECTION_PARAM_MASK  0x1

//  
//  0: Pad is used as ModScl 
//  1: Pad is used as GPIO40.
//  Register GpioPadSelection5 0x1004, Bits 7:7, typedef gpio_pad_selection
//   
//  
#define GPIO_40_PAD_SELECTION_MASK 0x80
#define GPIO_40_PAD_SELECTION_OFFSET 7
#define GPIO_40_PAD_SELECTION_STOP_BIT 7
#define GPIO_40_PAD_SELECTION_START_BIT 7
#define GPIO_40_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_5_REG 0x1004
#define GPIO_40_PAD_SELECTION_PARAM_MASK  0x80

//  
//  0: Pad is used as ApeSpiDin 
//  1: Pad is used as GPIO39.
//  Register GpioPadSelection5 0x1004, Bits 6:6, typedef gpio_pad_selection
//   
//  
#define GPIO_39_PAD_SELECTION_MASK 0x40
#define GPIO_39_PAD_SELECTION_OFFSET 6
#define GPIO_39_PAD_SELECTION_STOP_BIT 6
#define GPIO_39_PAD_SELECTION_START_BIT 6
#define GPIO_39_PAD_SELECTION_WIDTH 1

#define GPIO_39_PAD_SELECTION_PARAM_MASK  0x40

//  
//  0: Pad is used as ApeSpiDout 
//  1: Pad is used as GPIO38.
//  Register GpioPadSelection5 0x1004, Bits 5:5, typedef gpio_pad_selection
//   
//  
#define GPIO_38_PAD_SELECTION_MASK 0x20
#define GPIO_38_PAD_SELECTION_OFFSET 5
#define GPIO_38_PAD_SELECTION_STOP_BIT 5
#define GPIO_38_PAD_SELECTION_START_BIT 5
#define GPIO_38_PAD_SELECTION_WIDTH 1

#define GPIO_38_PAD_SELECTION_PARAM_MASK  0x20

//  
//  0: Pad is used as ApeSpiCSn 
//  1: Pad is used as GPIO37.
//  Register GpioPadSelection5 0x1004, Bits 4:4, typedef gpio_pad_selection
//   
//  
#define GPIO_37_PAD_SELECTION_MASK 0x10
#define GPIO_37_PAD_SELECTION_OFFSET 4
#define GPIO_37_PAD_SELECTION_STOP_BIT 4
#define GPIO_37_PAD_SELECTION_START_BIT 4
#define GPIO_37_PAD_SELECTION_WIDTH 1

#define GPIO_37_PAD_SELECTION_PARAM_MASK  0x10

//  
//  0: Pad is used as ApeSpiClk 
//  1: Pad is used as GPIO36.
//  Register GpioPadSelection5 0x1004, Bits 3:3, typedef gpio_pad_selection
//   
//  
#define GPIO_36_PAD_SELECTION_MASK 0x8
#define GPIO_36_PAD_SELECTION_OFFSET 3
#define GPIO_36_PAD_SELECTION_STOP_BIT 3
#define GPIO_36_PAD_SELECTION_START_BIT 3
#define GPIO_36_PAD_SELECTION_WIDTH 1

#define GPIO_36_PAD_SELECTION_PARAM_MASK  0x8

//  
//  0: Pad is used as ExtCPEna 
//  1: Pad is used as GPIO35.
//  Register GpioPadSelection5 0x1004, Bits 1:1, typedef gpio_pad_selection
//   
//  
#define GPIO_34_PAD_SELECTION_MASK 0x2
#define GPIO_34_PAD_SELECTION_OFFSET 1
#define GPIO_34_PAD_SELECTION_STOP_BIT 1
#define GPIO_34_PAD_SELECTION_START_BIT 1
#define GPIO_34_PAD_SELECTION_WIDTH 1

#define GPIO_34_PAD_SELECTION_PARAM_MASK  0x2

//  
//  0: Pad is used as SysClkReq5 
//  1: Pad is used as GPIO42 direction is output.
//  Register GpioPadSelection6 0x1005, Bits 1:1, typedef gpio_pad_selection
//   
//  
#define GPIO_42_PAD_SELECTION_MASK 0x2
#define GPIO_42_PAD_SELECTION_OFFSET 1
#define GPIO_42_PAD_SELECTION_STOP_BIT 1
#define GPIO_42_PAD_SELECTION_START_BIT 1
#define GPIO_42_PAD_SELECTION_WIDTH 1

#define GPIO_PAD_SELECTION_6_REG 0x1005
#define GPIO_42_PAD_SELECTION_PARAM_MASK  0x2

//  
//  0: Pad is used as ModSda 
//  1: Pad is used asGPIO41 direction is output.
//  Register GpioPadSelection6 0x1005, Bits 0:0, typedef gpio_pad_selection
//   
//  
#define GPIO_41_PAD_SELECTION_MASK 0x1
#define GPIO_41_PAD_SELECTION_OFFSET 0
#define GPIO_41_PAD_SELECTION_STOP_BIT 0
#define GPIO_41_PAD_SELECTION_START_BIT 0
#define GPIO_41_PAD_SELECTION_WIDTH 1

#define GPIO_41_PAD_SELECTION_PARAM_MASK  0x1

//  
//  0: GPIO8 direction is input 
//  1: GPIO8 direction is output.
//  Register GpioDirection1 0x1010, Bits 7:7, typedef gpio_direction
//   
//  
#define GPIO_8_DIRECTION_MASK 0x80
#define GPIO_8_DIRECTION_OFFSET 7
#define GPIO_8_DIRECTION_STOP_BIT 7
#define GPIO_8_DIRECTION_START_BIT 7
#define GPIO_8_DIRECTION_WIDTH 1

#define GPIO_8_DIRECTION_PARAM_MASK  0x80
#define GPIO_DIRECTION_PARAM_MASK  0x80

//  
//  0: GPIO7 direction is input 
//  1: GPIO7 direction is output.
//  Register GpioDirection1 0x1010, Bits 6:6, typedef gpio_direction
//   
//  
#define GPIO_7_DIRECTION_MASK 0x40
#define GPIO_7_DIRECTION_OFFSET 6
#define GPIO_7_DIRECTION_STOP_BIT 6
#define GPIO_7_DIRECTION_START_BIT 6
#define GPIO_7_DIRECTION_WIDTH 1

#define GPIO_7_DIRECTION_PARAM_MASK  0x40

//  
//  0: GPIO6 direction is input 
//  1: GPIO6 direction is output.
//  Register GpioDirection1 0x1010, Bits 5:5, typedef gpio_direction
//   
//  
#define GPIO_6_DIRECTION_MASK 0x20
#define GPIO_6_DIRECTION_OFFSET 5
#define GPIO_6_DIRECTION_STOP_BIT 5
#define GPIO_6_DIRECTION_START_BIT 5
#define GPIO_6_DIRECTION_WIDTH 1

#define GPIO_6_DIRECTION_PARAM_MASK  0x20

//  
//  0: GPIO4 direction is input 
//  1: GPIO4 direction is output.
//  Register GpioDirection1 0x1010, Bits 3:3, typedef gpio_direction
//   
//  
#define GPIO_4_DIRECTION_MASK 0x8
#define GPIO_4_DIRECTION_OFFSET 3
#define GPIO_4_DIRECTION_STOP_BIT 3
#define GPIO_4_DIRECTION_START_BIT 3
#define GPIO_4_DIRECTION_WIDTH 1

#define GPIO_4_DIRECTION_PARAM_MASK  0x8

//  
//  0: GPIO3 direction is input 
//  1: GPIO3 direction is output.
//  Register GpioDirection1 0x1010, Bits 2:2, typedef gpio_direction
//   
//  
#define GPIO_3_DIRECTION_MASK 0x4
#define GPIO_3_DIRECTION_OFFSET 2
#define GPIO_3_DIRECTION_STOP_BIT 2
#define GPIO_3_DIRECTION_START_BIT 2
#define GPIO_3_DIRECTION_WIDTH 1

#define GPIO_3_DIRECTION_PARAM_MASK  0x4

//  
//  0: GPIO2 direction is input 
//  1: GPIO2 direction is output.
//  Register GpioDirection1 0x1010, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_2_DIRECTION_MASK 0x2
#define GPIO_2_DIRECTION_OFFSET 1
#define GPIO_2_DIRECTION_STOP_BIT 1
#define GPIO_2_DIRECTION_START_BIT 1
#define GPIO_2_DIRECTION_WIDTH 1

#define GPIO_2_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO1 direction is input 
//  1: GPIO1 direction is output.
//  Register GpioDirection1 0x1010, Bits 0:0, typedef gpio_direction
//   
//  
#define GPIO_1_DIRECTION_MASK 0x1
#define GPIO_1_DIRECTION_OFFSET 0
#define GPIO_1_DIRECTION_STOP_BIT 0
#define GPIO_1_DIRECTION_START_BIT 0
#define GPIO_1_DIRECTION_WIDTH 1

#define GPIO_1_DIRECTION_PARAM_MASK  0x1

//  
//  0: GPIO16 direction is input 
//  1: GPIO16 direction is output.
//  Register GpioDirection2 0x1011, Bits 7:7, typedef gpio_direction
//   
//  
#define GPIO_16_DIRECTION_MASK 0x80
#define GPIO_16_DIRECTION_OFFSET 7
#define GPIO_16_DIRECTION_STOP_BIT 7
#define GPIO_16_DIRECTION_START_BIT 7
#define GPIO_16_DIRECTION_WIDTH 1

#define GPIO_16_DIRECTION_PARAM_MASK  0x80

//  
//  0: GPIO15 direction is input 
//  1: GPIO15 direction is output.
//  Register GpioDirection2 0x1011, Bits 6:6, typedef gpio_direction
//   
//  
#define GPIO_15_DIRECTION_MASK 0x40
#define GPIO_15_DIRECTION_OFFSET 6
#define GPIO_15_DIRECTION_STOP_BIT 6
#define GPIO_15_DIRECTION_START_BIT 6
#define GPIO_15_DIRECTION_WIDTH 1

#define GPIO_15_DIRECTION_PARAM_MASK  0x40

//  
//  0: GPIO14 direction is input 
//  1: GPIO14 direction is output.
//  Register GpioDirection2 0x1011, Bits 5:5, typedef gpio_direction
//   
//  
#define GPIO_14_DIRECTION_MASK 0x20
#define GPIO_14_DIRECTION_OFFSET 5
#define GPIO_14_DIRECTION_STOP_BIT 5
#define GPIO_14_DIRECTION_START_BIT 5
#define GPIO_14_DIRECTION_WIDTH 1

#define GPIO_14_DIRECTION_PARAM_MASK  0x20

//  
//  0: GPIO13 direction is input 
//  1: GPIO13 direction is output.
//  Register GpioDirection2 0x1011, Bits 4:4, typedef gpio_direction
//   
//  
#define GPIO_13_DIRECTION_MASK 0x10
#define GPIO_13_DIRECTION_OFFSET 4
#define GPIO_13_DIRECTION_STOP_BIT 4
#define GPIO_13_DIRECTION_START_BIT 4
#define GPIO_13_DIRECTION_WIDTH 1

#define GPIO_13_DIRECTION_PARAM_MASK  0x10

//  
//  0: GPIO12 direction is input 
//  1: GPIO12 direction is output.
//  Register GpioDirection2 0x1011, Bits 3:3, typedef gpio_direction
//   
//  
#define GPIO_12_DIRECTION_MASK 0x8
#define GPIO_12_DIRECTION_OFFSET 3
#define GPIO_12_DIRECTION_STOP_BIT 3
#define GPIO_12_DIRECTION_START_BIT 3
#define GPIO_12_DIRECTION_WIDTH 1

#define GPIO_12_DIRECTION_PARAM_MASK  0x8

//  
//  0: GPIO11 direction is input 
//  1: GPIO11 direction is output.
//  Register GpioDirection2 0x1011, Bits 2:2, typedef gpio_direction
//   
//  
#define GPIO_11_DIRECTION_MASK 0x4
#define GPIO_11_DIRECTION_OFFSET 2
#define GPIO_11_DIRECTION_STOP_BIT 2
#define GPIO_11_DIRECTION_START_BIT 2
#define GPIO_11_DIRECTION_WIDTH 1

#define GPIO_11_DIRECTION_PARAM_MASK  0x4

//  
//  0: GPIO10 direction is input 
//  1: GPIO10 direction is output.
//  Register GpioDirection2 0x1011, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_10_DIRECTION_MASK 0x2
#define GPIO_10_DIRECTION_OFFSET 1
#define GPIO_10_DIRECTION_STOP_BIT 1
#define GPIO_10_DIRECTION_START_BIT 1
#define GPIO_10_DIRECTION_WIDTH 1

#define GPIO_10_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO9 direction is input 
//  1: GPIO9 direction is output.
//  Register GpioDirection2 0x1011, Bits 0:0, typedef gpio_direction
//   
//  
#define GPIO_9_DIRECTION_MASK 0x1
#define GPIO_9_DIRECTION_OFFSET 0
#define GPIO_9_DIRECTION_STOP_BIT 0
#define GPIO_9_DIRECTION_START_BIT 0
#define GPIO_9_DIRECTION_WIDTH 1

#define GPIO_9_DIRECTION_PARAM_MASK  0x1

//  
//  0: GPIO24 direction is input 
//  1: GPIO24 direction is output.
//  Register GpioDirection3 0x1012, Bits 7:7, typedef gpio_direction
//   
//  
#define GPIO_24_DIRECTION_MASK 0x80
#define GPIO_24_DIRECTION_OFFSET 7
#define GPIO_24_DIRECTION_STOP_BIT 7
#define GPIO_24_DIRECTION_START_BIT 7
#define GPIO_24_DIRECTION_WIDTH 1

#define GPIO_24_DIRECTION_PARAM_MASK  0x80

//  
//  0: GPIO23 direction is input 
//  1: GPIO23 direction is output.
//  Register GpioDirection3 0x1012, Bits 6:6, typedef gpio_direction
//   
//  
#define GPIO_23_DIRECTION_MASK 0x40
#define GPIO_23_DIRECTION_OFFSET 6
#define GPIO_23_DIRECTION_STOP_BIT 6
#define GPIO_23_DIRECTION_START_BIT 6
#define GPIO_23_DIRECTION_WIDTH 1

#define GPIO_23_DIRECTION_PARAM_MASK  0x40

//  
//  0: GPIO22 direction is input 
//  1: GPIO22 direction is output.
//  Register GpioDirection3 0x1012, Bits 5:5, typedef gpio_direction
//   
//  
#define GPIO_22_DIRECTION_MASK 0x20
#define GPIO_22_DIRECTION_OFFSET 5
#define GPIO_22_DIRECTION_STOP_BIT 5
#define GPIO_22_DIRECTION_START_BIT 5
#define GPIO_22_DIRECTION_WIDTH 1

#define GPIO_22_DIRECTION_PARAM_MASK  0x20

//  
//  0: GPIO21 direction is input 
//  1: GPIO21 direction is output.
//  Register GpioDirection3 0x1012, Bits 4:4, typedef gpio_direction
//   
//  
#define GPIO_21_DIRECTION_MASK 0x10
#define GPIO_21_DIRECTION_OFFSET 4
#define GPIO_21_DIRECTION_STOP_BIT 4
#define GPIO_21_DIRECTION_START_BIT 4
#define GPIO_21_DIRECTION_WIDTH 1

#define GPIO_21_DIRECTION_PARAM_MASK  0x10

//  
//  0: GPIO20 direction is input 
//  1: GPIO20 direction is output.
//  Register GpioDirection3 0x1012, Bits 3:3, typedef gpio_direction
//   
//  
#define GPIO_20_DIRECTION_MASK 0x8
#define GPIO_20_DIRECTION_OFFSET 3
#define GPIO_20_DIRECTION_STOP_BIT 3
#define GPIO_20_DIRECTION_START_BIT 3
#define GPIO_20_DIRECTION_WIDTH 1

#define GPIO_20_DIRECTION_PARAM_MASK  0x8

//  
//  0: GPIO19 direction is input 
//  1: GPIO19 direction is output.
//  Register GpioDirection3 0x1012, Bits 2:2, typedef gpio_direction
//   
//  
#define GPIO_19_DIRECTION_MASK 0x4
#define GPIO_19_DIRECTION_OFFSET 2
#define GPIO_19_DIRECTION_STOP_BIT 2
#define GPIO_19_DIRECTION_START_BIT 2
#define GPIO_19_DIRECTION_WIDTH 1

#define GPIO_19_DIRECTION_PARAM_MASK  0x4

//  
//  0: GPIO18 direction is input 
//  1: GPIO18 direction is output.
//  Register GpioDirection3 0x1012, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_18_DIRECTION_MASK 0x2
#define GPIO_18_DIRECTION_OFFSET 1
#define GPIO_18_DIRECTION_STOP_BIT 1
#define GPIO_18_DIRECTION_START_BIT 1
#define GPIO_18_DIRECTION_WIDTH 1

#define GPIO_18_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO17 direction is input 
//  1: GPIO17 direction is output.
//  Register GpioDirection3 0x1012, Bits 0:0, typedef gpio_direction
//   
//  
#define GPIO_17_DIRECTION_MASK 0x1
#define GPIO_17_DIRECTION_OFFSET 0
#define GPIO_17_DIRECTION_STOP_BIT 0
#define GPIO_17_DIRECTION_START_BIT 0
#define GPIO_17_DIRECTION_WIDTH 1

#define GPIO_17_DIRECTION_PARAM_MASK  0x1

//  
//  0: GPIO32 direction is input 
//  1: GPIO32 direction is output.
//  Register GpioDirection4 0x1013, Bits 7:7, typedef gpio_direction
//   
//  
#define GPIO_32_DIRECTION_MASK 0x80
#define GPIO_32_DIRECTION_OFFSET 7
#define GPIO_32_DIRECTION_STOP_BIT 7
#define GPIO_32_DIRECTION_START_BIT 7
#define GPIO_32_DIRECTION_WIDTH 1

#define GPIO_32_DIRECTION_PARAM_MASK  0x80

//  
//  0: GPIO31 direction is input 
//  1: GPIO31 direction is output.
//  Register GpioDirection4 0x1013, Bits 6:6, typedef gpio_direction
//   
//  
#define GPIO_31_DIRECTION_MASK 0x40
#define GPIO_31_DIRECTION_OFFSET 6
#define GPIO_31_DIRECTION_STOP_BIT 6
#define GPIO_31_DIRECTION_START_BIT 6
#define GPIO_31_DIRECTION_WIDTH 1

#define GPIO_31_DIRECTION_PARAM_MASK  0x40

//  
//  0: GPIO30 direction is input 
//  1: GPIO30 direction is output.
//  Register GpioDirection4 0x1013, Bits 5:5, typedef gpio_direction
//   
//  
#define GPIO_30_DIRECTION_MASK 0x20
#define GPIO_30_DIRECTION_OFFSET 5
#define GPIO_30_DIRECTION_STOP_BIT 5
#define GPIO_30_DIRECTION_START_BIT 5
#define GPIO_30_DIRECTION_WIDTH 1

#define GPIO_30_DIRECTION_PARAM_MASK  0x20

//  
//  0: GPIO29 direction is input 
//  1: GPIO29 direction is output.
//  Register GpioDirection4 0x1013, Bits 4:4, typedef gpio_direction
//   
//  
#define GPIO_29_DIRECTION_MASK 0x10
#define GPIO_29_DIRECTION_OFFSET 4
#define GPIO_29_DIRECTION_STOP_BIT 4
#define GPIO_29_DIRECTION_START_BIT 4
#define GPIO_29_DIRECTION_WIDTH 1

#define GPIO_29_DIRECTION_PARAM_MASK  0x10

//  
//  0: GPIO28 direction is input 
//  1: GPIO28 direction is output.
//  Register GpioDirection4 0x1013, Bits 3:3, typedef gpio_direction
//   
//  
#define GPIO_28_DIRECTION_MASK 0x8
#define GPIO_28_DIRECTION_OFFSET 3
#define GPIO_28_DIRECTION_STOP_BIT 3
#define GPIO_28_DIRECTION_START_BIT 3
#define GPIO_28_DIRECTION_WIDTH 1

#define GPIO_28_DIRECTION_PARAM_MASK  0x8

//  
//  0: GPIO27 direction is input 
//  1: GPIO27 direction is output.
//  Register GpioDirection4 0x1013, Bits 2:2, typedef gpio_direction
//   
//  
#define GPIO_27_DIRECTION_MASK 0x4
#define GPIO_27_DIRECTION_OFFSET 2
#define GPIO_27_DIRECTION_STOP_BIT 2
#define GPIO_27_DIRECTION_START_BIT 2
#define GPIO_27_DIRECTION_WIDTH 1

#define GPIO_27_DIRECTION_PARAM_MASK  0x4

//  
//  0: GPIO26 direction is input 
//  1: GPIO26 direction is output.
//  Register GpioDirection4 0x1013, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_26_DIRECTION_MASK 0x2
#define GPIO_26_DIRECTION_OFFSET 1
#define GPIO_26_DIRECTION_STOP_BIT 1
#define GPIO_26_DIRECTION_START_BIT 1
#define GPIO_26_DIRECTION_WIDTH 1

#define GPIO_26_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO25 direction is input 
//  1: GPIO25 direction is output.
//  Register GpioDirection4 0x1013, Bits 0:0, typedef gpio_direction
//   
//  
#define GPIO_25_DIRECTION_MASK 0x1
#define GPIO_25_DIRECTION_OFFSET 0
#define GPIO_25_DIRECTION_STOP_BIT 0
#define GPIO_25_DIRECTION_START_BIT 0
#define GPIO_25_DIRECTION_WIDTH 1

#define GPIO_25_DIRECTION_PARAM_MASK  0x1

//  
//  0: GPIO40 direction is input 
//  1: GPIO40 direction is output.
//  Register GpioDirection5 0x1014, Bits 7:7, typedef gpio_direction
//   
//  
#define GPIO_40_DIRECTION_MASK 0x80
#define GPIO_40_DIRECTION_OFFSET 7
#define GPIO_40_DIRECTION_STOP_BIT 7
#define GPIO_40_DIRECTION_START_BIT 7
#define GPIO_40_DIRECTION_WIDTH 1

#define GPIO_40_DIRECTION_PARAM_MASK  0x80

//  
//  0: GPIO39 direction is input 
//  1: GPIO39 direction is output.
//  Register GpioDirection5 0x1014, Bits 6:6, typedef gpio_direction
//   
//  
#define GPIO_39_DIRECTION_MASK 0x40
#define GPIO_39_DIRECTION_OFFSET 6
#define GPIO_39_DIRECTION_STOP_BIT 6
#define GPIO_39_DIRECTION_START_BIT 6
#define GPIO_39_DIRECTION_WIDTH 1

#define GPIO_39_DIRECTION_PARAM_MASK  0x40

//  
//  0: GPIO38 direction is input 
//  1: GPIO38 direction is output.
//  Register GpioDirection5 0x1014, Bits 5:5, typedef gpio_direction
//   
//  
#define GPIO_38_DIRECTION_MASK 0x20
#define GPIO_38_DIRECTION_OFFSET 5
#define GPIO_38_DIRECTION_STOP_BIT 5
#define GPIO_38_DIRECTION_START_BIT 5
#define GPIO_38_DIRECTION_WIDTH 1

#define GPIO_38_DIRECTION_PARAM_MASK  0x20

//  
//  0: GPIO37 direction is input 
//  1: GPIO37 direction is output.
//  Register GpioDirection5 0x1014, Bits 4:4, typedef gpio_direction
//   
//  
#define GPIO_37_DIRECTION_MASK 0x10
#define GPIO_37_DIRECTION_OFFSET 4
#define GPIO_37_DIRECTION_STOP_BIT 4
#define GPIO_37_DIRECTION_START_BIT 4
#define GPIO_37_DIRECTION_WIDTH 1

#define GPIO_37_DIRECTION_PARAM_MASK  0x10

//  
//  0: GPIO36 direction is input 
//  1: GPIO36 direction is output.
//  Register GpioDirection5 0x1014, Bits 3:3, typedef gpio_direction
//   
//  
#define GPIO_36_DIRECTION_MASK 0x8
#define GPIO_36_DIRECTION_OFFSET 3
#define GPIO_36_DIRECTION_STOP_BIT 3
#define GPIO_36_DIRECTION_START_BIT 3
#define GPIO_36_DIRECTION_WIDTH 1

#define GPIO_36_DIRECTION_PARAM_MASK  0x8

//  
//  0: GPIO35 direction is input 
//  1: GPIO35 direction is output.
//  Register GpioDirection5 0x1014, Bits 2:2, typedef gpio_direction
//   
//  
#define GPIO_35_DIRECTION_MASK 0x4
#define GPIO_35_DIRECTION_OFFSET 2
#define GPIO_35_DIRECTION_STOP_BIT 2
#define GPIO_35_DIRECTION_START_BIT 2
#define GPIO_35_DIRECTION_WIDTH 1

#define GPIO_35_DIRECTION_PARAM_MASK  0x4

//  
//  0: GPIO34 direction is input 
//  1: GPIO34 direction is output.
//  Register GpioDirection5 0x1014, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_34_DIRECTION_MASK 0x2
#define GPIO_34_DIRECTION_OFFSET 1
#define GPIO_34_DIRECTION_STOP_BIT 1
#define GPIO_34_DIRECTION_START_BIT 1
#define GPIO_34_DIRECTION_WIDTH 1

#define GPIO_34_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO42 direction is input 
//  1: GPIO42 direction is output.
//  Register GpioDirection6 0x1015, Bits 1:1, typedef gpio_direction
//   
//  
#define GPIO_42_DIRECTION_MASK 0x2
#define GPIO_42_DIRECTION_OFFSET 1
#define GPIO_42_DIRECTION_STOP_BIT 1
#define GPIO_42_DIRECTION_START_BIT 1
#define GPIO_42_DIRECTION_WIDTH 1

#define GPIO_42_DIRECTION_PARAM_MASK  0x2

//  
//  0: GPIO41 direction is input 
//  1: GPIO41 direction is output.
//  Register GpioDirection6 0x1015, Bits 0:0, typedef gpio_direction
//   
//  
#define GPIO_41_DIRECTION_MASK 0x1
#define GPIO_41_DIRECTION_OFFSET 0
#define GPIO_41_DIRECTION_STOP_BIT 0
#define GPIO_41_DIRECTION_START_BIT 0
#define GPIO_41_DIRECTION_WIDTH 1

#define GPIO_41_DIRECTION_PARAM_MASK  0x1

//  
//  0: GPIO8 output egals "0" 
//  1: GPIO8 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 7:7, typedef gpio_output
//   
//  
#define GPIO_8_OUTPUT_MASK 0x80
#define GPIO_8_OUTPUT_OFFSET 7
#define GPIO_8_OUTPUT_STOP_BIT 7
#define GPIO_8_OUTPUT_START_BIT 7
#define GPIO_8_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_1_REG 0x1020
#define GPIO_8_OUTPUT_PARAM_MASK  0x80
#define GPIO_OUTPUT_PARAM_MASK  0x80

//  
//  0: GPIO7 output egals "0" 
//  1: GPIO7 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 6:6, typedef gpio_output
//   
//  
#define GPIO_7_OUTPUT_MASK 0x40
#define GPIO_7_OUTPUT_OFFSET 6
#define GPIO_7_OUTPUT_STOP_BIT 6
#define GPIO_7_OUTPUT_START_BIT 6
#define GPIO_7_OUTPUT_WIDTH 1

#define GPIO_7_OUTPUT_PARAM_MASK  0x40

//  
//  0: GPIO6 output egals "0" 
//  1: GPIO6 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 5:5, typedef gpio_output
//   
//  
#define GPIO_6_OUTPUT_MASK 0x20
#define GPIO_6_OUTPUT_OFFSET 5
#define GPIO_6_OUTPUT_STOP_BIT 5
#define GPIO_6_OUTPUT_START_BIT 5
#define GPIO_6_OUTPUT_WIDTH 1

#define GPIO_6_OUTPUT_PARAM_MASK  0x20

//  
//  0: GPIO4 output egals "0" 
//  1: GPIO4 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 3:3, typedef gpio_output
//   
//  
#define GPIO_4_OUTPUT_MASK 0x8
#define GPIO_4_OUTPUT_OFFSET 3
#define GPIO_4_OUTPUT_STOP_BIT 3
#define GPIO_4_OUTPUT_START_BIT 3
#define GPIO_4_OUTPUT_WIDTH 1

#define GPIO_4_OUTPUT_PARAM_MASK  0x8

//  
//  0: GPIO3 output egals "0" 
//  1: GPIO3 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 2:2, typedef gpio_output
//   
//  
#define GPIO_3_OUTPUT_MASK 0x4
#define GPIO_3_OUTPUT_OFFSET 2
#define GPIO_3_OUTPUT_STOP_BIT 2
#define GPIO_3_OUTPUT_START_BIT 2
#define GPIO_3_OUTPUT_WIDTH 1

#define GPIO_3_OUTPUT_PARAM_MASK  0x4

//  
//  0: GPIO2 output egals "0" 
//  1: GPIO2 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_2_OUTPUT_MASK 0x2
#define GPIO_2_OUTPUT_OFFSET 1
#define GPIO_2_OUTPUT_STOP_BIT 1
#define GPIO_2_OUTPUT_START_BIT 1
#define GPIO_2_OUTPUT_WIDTH 1

#define GPIO_2_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO1 output egals "0" 
//  1: GPIO1 output egals "1".
//  Register GpioOutputControl1 0x1020, Bits 0:0, typedef gpio_output
//   
//  
#define GPIO_1_OUTPUT_MASK 0x1
#define GPIO_1_OUTPUT_OFFSET 0
#define GPIO_1_OUTPUT_STOP_BIT 0
#define GPIO_1_OUTPUT_START_BIT 0
#define GPIO_1_OUTPUT_WIDTH 1

#define GPIO_1_OUTPUT_PARAM_MASK  0x1

//  
//  0: GPIO16 output egals "0" 
//  1: GPIO16 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 7:7, typedef gpio_output
//   
//  
#define GPIO_16_OUTPUT_MASK 0x80
#define GPIO_16_OUTPUT_OFFSET 7
#define GPIO_16_OUTPUT_STOP_BIT 7
#define GPIO_16_OUTPUT_START_BIT 7
#define GPIO_16_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_3_REG 0x1021
#define GPIO_16_OUTPUT_PARAM_MASK  0x80

//  
//  0: GPIO15 output egals "0" 
//  1: GPIO15 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 6:6, typedef gpio_output
//   
//  
#define GPIO_15_OUTPUT_MASK 0x40
#define GPIO_15_OUTPUT_OFFSET 6
#define GPIO_15_OUTPUT_STOP_BIT 6
#define GPIO_15_OUTPUT_START_BIT 6
#define GPIO_15_OUTPUT_WIDTH 1

#define GPIO_15_OUTPUT_PARAM_MASK  0x40

//  
//  0: GPIO14 output egals "0" 
//  1: GPIO14 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 5:5, typedef gpio_output
//   
//  
#define GPIO_14_OUTPUT_MASK 0x20
#define GPIO_14_OUTPUT_OFFSET 5
#define GPIO_14_OUTPUT_STOP_BIT 5
#define GPIO_14_OUTPUT_START_BIT 5
#define GPIO_14_OUTPUT_WIDTH 1

#define GPIO_14_OUTPUT_PARAM_MASK  0x20

//  
//  0: GPIO13 output egals "0" 
//  1: GPIO13 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 4:4, typedef gpio_output
//   
//  
#define GPIO_13_OUTPUT_MASK 0x10
#define GPIO_13_OUTPUT_OFFSET 4
#define GPIO_13_OUTPUT_STOP_BIT 4
#define GPIO_13_OUTPUT_START_BIT 4
#define GPIO_13_OUTPUT_WIDTH 1

#define GPIO_13_OUTPUT_PARAM_MASK  0x10

//  
//  0: GPIO12 output egals "0" 
//  1: GPIO12 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 3:3, typedef gpio_output
//   
//  
#define GPIO_12_OUTPUT_MASK 0x8
#define GPIO_12_OUTPUT_OFFSET 3
#define GPIO_12_OUTPUT_STOP_BIT 3
#define GPIO_12_OUTPUT_START_BIT 3
#define GPIO_12_OUTPUT_WIDTH 1

#define GPIO_12_OUTPUT_PARAM_MASK  0x8

//  
//  0: GPIO11 output egals "0" 
//  1: GPIO11 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 2:2, typedef gpio_output
//   
//  
#define GPIO_11_OUTPUT_MASK 0x4
#define GPIO_11_OUTPUT_OFFSET 2
#define GPIO_11_OUTPUT_STOP_BIT 2
#define GPIO_11_OUTPUT_START_BIT 2
#define GPIO_11_OUTPUT_WIDTH 1

#define GPIO_11_OUTPUT_PARAM_MASK  0x4

//  
//  0: GPIO10 output egals "0" 
//  1: GPIO10 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_10_OUTPUT_MASK 0x2
#define GPIO_10_OUTPUT_OFFSET 1
#define GPIO_10_OUTPUT_STOP_BIT 1
#define GPIO_10_OUTPUT_START_BIT 1
#define GPIO_10_OUTPUT_WIDTH 1

#define GPIO_10_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO9 output egals "0" 
//  1: GPIO9 output egals "1".
//  Register GpioOutputControl3 0x1021, Bits 0:0, typedef gpio_output
//   
//  
#define GPIO_9_OUTPUT_MASK 0x1
#define GPIO_9_OUTPUT_OFFSET 0
#define GPIO_9_OUTPUT_STOP_BIT 0
#define GPIO_9_OUTPUT_START_BIT 0
#define GPIO_9_OUTPUT_WIDTH 1

#define GPIO_9_OUTPUT_PARAM_MASK  0x1

//  
//  0: GPIO24 output egals "0" 
//  1: GPIO24 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 7:7, typedef gpio_output
//   
//  
#define GPIO_24_OUTPUT_MASK 0x80
#define GPIO_24_OUTPUT_OFFSET 7
#define GPIO_24_OUTPUT_STOP_BIT 7
#define GPIO_24_OUTPUT_START_BIT 7
#define GPIO_24_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_4_REG 0x1022
#define GPIO_24_OUTPUT_PARAM_MASK  0x80

//  
//  0: GPIO23 output egals "0" 
//  1: GPIO23 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 6:6, typedef gpio_output
//   
//  
#define GPIO_23_OUTPUT_MASK 0x40
#define GPIO_23_OUTPUT_OFFSET 6
#define GPIO_23_OUTPUT_STOP_BIT 6
#define GPIO_23_OUTPUT_START_BIT 6
#define GPIO_23_OUTPUT_WIDTH 1

#define GPIO_23_OUTPUT_PARAM_MASK  0x40

//  
//  0: GPIO22 output egals "0" 
//  1: GPIO22 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 5:5, typedef gpio_output
//   
//  
#define GPIO_22_OUTPUT_MASK 0x20
#define GPIO_22_OUTPUT_OFFSET 5
#define GPIO_22_OUTPUT_STOP_BIT 5
#define GPIO_22_OUTPUT_START_BIT 5
#define GPIO_22_OUTPUT_WIDTH 1

#define GPIO_22_OUTPUT_PARAM_MASK  0x20

//  
//  0: GPIO21 output egals "0" 
//  1: GPIO21 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 4:4, typedef gpio_output
//   
//  
#define GPIO_21_OUTPUT_MASK 0x10
#define GPIO_21_OUTPUT_OFFSET 4
#define GPIO_21_OUTPUT_STOP_BIT 4
#define GPIO_21_OUTPUT_START_BIT 4
#define GPIO_21_OUTPUT_WIDTH 1

#define GPIO_21_OUTPUT_PARAM_MASK  0x10

//  
//  0: GPIO20 output egals "0" 
//  1: GPIO20 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 3:3, typedef gpio_output
//   
//  
#define GPIO_20_OUTPUT_MASK 0x8
#define GPIO_20_OUTPUT_OFFSET 3
#define GPIO_20_OUTPUT_STOP_BIT 3
#define GPIO_20_OUTPUT_START_BIT 3
#define GPIO_20_OUTPUT_WIDTH 1

#define GPIO_20_OUTPUT_PARAM_MASK  0x8

//  
//  0: GPIO19 output egals "0" 
//  1: GPIO19 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 2:2, typedef gpio_output
//   
//  
#define GPIO_19_OUTPUT_MASK 0x4
#define GPIO_19_OUTPUT_OFFSET 2
#define GPIO_19_OUTPUT_STOP_BIT 2
#define GPIO_19_OUTPUT_START_BIT 2
#define GPIO_19_OUTPUT_WIDTH 1

#define GPIO_19_OUTPUT_PARAM_MASK  0x4

//  
//  0: GPIO18 output egals "0" 
//  1: GPIO18 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_18_OUTPUT_MASK 0x2
#define GPIO_18_OUTPUT_OFFSET 1
#define GPIO_18_OUTPUT_STOP_BIT 1
#define GPIO_18_OUTPUT_START_BIT 1
#define GPIO_18_OUTPUT_WIDTH 1

#define GPIO_18_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO17 output egals "0" 
//  1: GPIO17 output egals "1".
//  Register GpioOutputControl4 0x1022, Bits 0:0, typedef gpio_output
//   
//  
#define GPIO_17_OUTPUT_MASK 0x1
#define GPIO_17_OUTPUT_OFFSET 0
#define GPIO_17_OUTPUT_STOP_BIT 0
#define GPIO_17_OUTPUT_START_BIT 0
#define GPIO_17_OUTPUT_WIDTH 1

#define GPIO_17_OUTPUT_PARAM_MASK  0x1

//  
//  0: GPIO32 output egals "0" 
//  1: GPIO32 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 7:7, typedef gpio_output
//   
//  
#define GPIO_32_OUTPUT_MASK 0x80
#define GPIO_32_OUTPUT_OFFSET 7
#define GPIO_32_OUTPUT_STOP_BIT 7
#define GPIO_32_OUTPUT_START_BIT 7
#define GPIO_32_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_5_REG 0x1023
#define GPIO_32_OUTPUT_PARAM_MASK  0x80

//  
//  0: GPIO31 output egals "0" 
//  1: GPIO31 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 6:6, typedef gpio_output
//   
//  
#define GPIO_31_OUTPUT_MASK 0x40
#define GPIO_31_OUTPUT_OFFSET 6
#define GPIO_31_OUTPUT_STOP_BIT 6
#define GPIO_31_OUTPUT_START_BIT 6
#define GPIO_31_OUTPUT_WIDTH 1

#define GPIO_31_OUTPUT_PARAM_MASK  0x40

//  
//  0: GPIO30 output egals "0" 
//  1: GPIO30 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 5:5, typedef gpio_output
//   
//  
#define GPIO_30_OUTPUT_MASK 0x20
#define GPIO_30_OUTPUT_OFFSET 5
#define GPIO_30_OUTPUT_STOP_BIT 5
#define GPIO_30_OUTPUT_START_BIT 5
#define GPIO_30_OUTPUT_WIDTH 1

#define GPIO_30_OUTPUT_PARAM_MASK  0x20

//  
//  0: GPIO29 output egals "0" 
//  1: GPIO29 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 4:4, typedef gpio_output
//   
//  
#define GPIO_29_OUTPUT_MASK 0x10
#define GPIO_29_OUTPUT_OFFSET 4
#define GPIO_29_OUTPUT_STOP_BIT 4
#define GPIO_29_OUTPUT_START_BIT 4
#define GPIO_29_OUTPUT_WIDTH 1

#define GPIO_29_OUTPUT_PARAM_MASK  0x10

//  
//  0: GPIO28 output egals "0" 
//  1: GPIO28 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 3:3, typedef gpio_output
//   
//  
#define GPIO_28_OUTPUT_MASK 0x8
#define GPIO_28_OUTPUT_OFFSET 3
#define GPIO_28_OUTPUT_STOP_BIT 3
#define GPIO_28_OUTPUT_START_BIT 3
#define GPIO_28_OUTPUT_WIDTH 1

#define GPIO_28_OUTPUT_PARAM_MASK  0x8

//  
//  0: GPIO27 output egals "0" 
//  1: GPIO27 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 2:2, typedef gpio_output
//   
//  
#define GPIO_27_OUTPUT_MASK 0x4
#define GPIO_27_OUTPUT_OFFSET 2
#define GPIO_27_OUTPUT_STOP_BIT 2
#define GPIO_27_OUTPUT_START_BIT 2
#define GPIO_27_OUTPUT_WIDTH 1

#define GPIO_27_OUTPUT_PARAM_MASK  0x4

//  
//  0: GPIO26 output egals "0" 
//  1: GPIO26 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_26_OUTPUT_MASK 0x2
#define GPIO_26_OUTPUT_OFFSET 1
#define GPIO_26_OUTPUT_STOP_BIT 1
#define GPIO_26_OUTPUT_START_BIT 1
#define GPIO_26_OUTPUT_WIDTH 1

#define GPIO_26_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO25 output egals "0" 
//  1: GPIO25 output egals "1".
//  Register GpioOutputControl5 0x1023, Bits 0:0, typedef gpio_output
//   
//  
#define GPIO_25_OUTPUT_MASK 0x1
#define GPIO_25_OUTPUT_OFFSET 0
#define GPIO_25_OUTPUT_STOP_BIT 0
#define GPIO_25_OUTPUT_START_BIT 0
#define GPIO_25_OUTPUT_WIDTH 1

#define GPIO_25_OUTPUT_PARAM_MASK  0x1

//  
//  0: GPIO40 output egals "0" 
//  1: GPIO40 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 7:7, typedef gpio_output
//   
//  
#define GPIO_40_OUTPUT_MASK 0x80
#define GPIO_40_OUTPUT_OFFSET 7
#define GPIO_40_OUTPUT_STOP_BIT 7
#define GPIO_40_OUTPUT_START_BIT 7
#define GPIO_40_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_6_REG 0x1024
#define GPIO_40_OUTPUT_PARAM_MASK  0x80

//  
//  0: GPIO39 output egals "0" 
//  1: GPIO39 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 6:6, typedef gpio_output
//   
//  
#define GPIO_39_OUTPUT_MASK 0x40
#define GPIO_39_OUTPUT_OFFSET 6
#define GPIO_39_OUTPUT_STOP_BIT 6
#define GPIO_39_OUTPUT_START_BIT 6
#define GPIO_39_OUTPUT_WIDTH 1

#define GPIO_39_OUTPUT_PARAM_MASK  0x40

//  
//  0: GPIO38 output egals "0" 
//  1: GPIO38 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 5:5, typedef gpio_output
//   
//  
#define GPIO_38_OUTPUT_MASK 0x20
#define GPIO_38_OUTPUT_OFFSET 5
#define GPIO_38_OUTPUT_STOP_BIT 5
#define GPIO_38_OUTPUT_START_BIT 5
#define GPIO_38_OUTPUT_WIDTH 1

#define GPIO_38_OUTPUT_PARAM_MASK  0x20

//  
//  0: GPIO37 output egals "0" 
//  1: GPIO37 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 4:4, typedef gpio_output
//   
//  
#define GPIO_37_OUTPUT_MASK 0x10
#define GPIO_37_OUTPUT_OFFSET 4
#define GPIO_37_OUTPUT_STOP_BIT 4
#define GPIO_37_OUTPUT_START_BIT 4
#define GPIO_37_OUTPUT_WIDTH 1

#define GPIO_37_OUTPUT_PARAM_MASK  0x10

//  
//  0: GPIO36 output egals "0" 
//  1: GPIO36 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 3:3, typedef gpio_output
//   
//  
#define GPIO_36_OUTPUT_MASK 0x8
#define GPIO_36_OUTPUT_OFFSET 3
#define GPIO_36_OUTPUT_STOP_BIT 3
#define GPIO_36_OUTPUT_START_BIT 3
#define GPIO_36_OUTPUT_WIDTH 1

#define GPIO_36_OUTPUT_PARAM_MASK  0x8

//  
//  0: GPIO35 output egals "0" 
//  1: GPIO35 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 2:2, typedef gpio_output
//   
//  
#define GPIO_35_OUTPUT_MASK 0x4
#define GPIO_35_OUTPUT_OFFSET 2
#define GPIO_35_OUTPUT_STOP_BIT 2
#define GPIO_35_OUTPUT_START_BIT 2
#define GPIO_35_OUTPUT_WIDTH 1

#define GPIO_35_OUTPUT_PARAM_MASK  0x4

//  
//  0: GPIO34 output egals "0" 
//  1: GPIO34 output egals "1".
//  Register GpioOutputControl6 0x1024, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_34_OUTPUT_MASK 0x2
#define GPIO_34_OUTPUT_OFFSET 1
#define GPIO_34_OUTPUT_STOP_BIT 1
#define GPIO_34_OUTPUT_START_BIT 1
#define GPIO_34_OUTPUT_WIDTH 1

#define GPIO_34_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO42 output egals "0" 
//  1: GPIO42 output egals "1".
//  Register GpioOutputControl7 0x1025, Bits 1:1, typedef gpio_output
//   
//  
#define GPIO_42_OUTPUT_MASK 0x2
#define GPIO_42_OUTPUT_OFFSET 1
#define GPIO_42_OUTPUT_STOP_BIT 1
#define GPIO_42_OUTPUT_START_BIT 1
#define GPIO_42_OUTPUT_WIDTH 1

#define GPIO_OUTPUT_CONTROL_7_REG 0x1025
#define GPIO_42_OUTPUT_PARAM_MASK  0x2

//  
//  0: GPIO41 output egals "0" 
//  1: GPIO41 output egals "1".
//  Register GpioOutputControl7 0x1025, Bits 0:0, typedef gpio_output
//   
//  
#define GPIO_41_OUTPUT_MASK 0x1
#define GPIO_41_OUTPUT_OFFSET 0
#define GPIO_41_OUTPUT_STOP_BIT 0
#define GPIO_41_OUTPUT_START_BIT 0
#define GPIO_41_OUTPUT_WIDTH 1

#define GPIO_41_OUTPUT_PARAM_MASK  0x1

//  
//  0: GPIO8 pull down is enabled 
//  1: GPIO8 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 7:7, typedef gpio_pulldown
//   
//  
#define GPIO_8_PULLDOWN_MASK 0x80
#define GPIO_8_PULLDOWN_OFFSET 7
#define GPIO_8_PULLDOWN_STOP_BIT 7
#define GPIO_8_PULLDOWN_START_BIT 7
#define GPIO_8_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_1_REG 0x1030
#define GPIO_8_PULLDOWN_PARAM_MASK  0x80
#define GPIO_PULLDOWN_PARAM_MASK  0x80

//  
//  0: GPIO7 pull down is enabled 
//  1: GPIO7 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 6:6, typedef gpio_pulldown
//   
//  
#define GPIO_7_PULLDOWN_MASK 0x40
#define GPIO_7_PULLDOWN_OFFSET 6
#define GPIO_7_PULLDOWN_STOP_BIT 6
#define GPIO_7_PULLDOWN_START_BIT 6
#define GPIO_7_PULLDOWN_WIDTH 1

#define GPIO_7_PULLDOWN_PARAM_MASK  0x40

//  
//  0: GPIO6 pull down is enabled 
//  1: GPIO6 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_6_PULLDOWN_MASK 0x20
#define GPIO_6_PULLDOWN_OFFSET 5
#define GPIO_6_PULLDOWN_STOP_BIT 5
#define GPIO_6_PULLDOWN_START_BIT 5
#define GPIO_6_PULLDOWN_WIDTH 1

#define GPIO_6_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO4 pull down is enabled 
//  1: GPIO4 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 3:3, typedef gpio_pulldown
//   
//  
#define GPIO_4_PULLDOWN_MASK 0x8
#define GPIO_4_PULLDOWN_OFFSET 3
#define GPIO_4_PULLDOWN_STOP_BIT 3
#define GPIO_4_PULLDOWN_START_BIT 3
#define GPIO_4_PULLDOWN_WIDTH 1

#define GPIO_4_PULLDOWN_PARAM_MASK  0x8

//  
//  0: GPIO3 pull down is enabled 
//  1: GPIO3 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 2:2, typedef gpio_pulldown
//   
//  
#define GPIO_3_PULLDOWN_MASK 0x4
#define GPIO_3_PULLDOWN_OFFSET 2
#define GPIO_3_PULLDOWN_STOP_BIT 2
#define GPIO_3_PULLDOWN_START_BIT 2
#define GPIO_3_PULLDOWN_WIDTH 1

#define GPIO_3_PULLDOWN_PARAM_MASK  0x4

//  
//  0: GPIO2 pull down is enabled 
//  1: GPIO2 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_2_PULLDOWN_MASK 0x2
#define GPIO_2_PULLDOWN_OFFSET 1
#define GPIO_2_PULLDOWN_STOP_BIT 1
#define GPIO_2_PULLDOWN_START_BIT 1
#define GPIO_2_PULLDOWN_WIDTH 1

#define GPIO_2_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO1 pull down is enabled 
//  1: GPIO1 pull down is disabled.
//  Register GpioPullControl1 0x1030, Bits 0:0, typedef gpio_pulldown
//   
//  
#define GPIO_1_PULLDOWN_MASK 0x1
#define GPIO_1_PULLDOWN_OFFSET 0
#define GPIO_1_PULLDOWN_STOP_BIT 0
#define GPIO_1_PULLDOWN_START_BIT 0
#define GPIO_1_PULLDOWN_WIDTH 1

#define GPIO_1_PULLDOWN_PARAM_MASK  0x1

//  
//  0: GPIO16 pull down is enabled 
//  1: GPIO16 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 7:7, typedef gpio_pulldown
//   
//  
#define GPIO_16_PULLDOWN_MASK 0x80
#define GPIO_16_PULLDOWN_OFFSET 7
#define GPIO_16_PULLDOWN_STOP_BIT 7
#define GPIO_16_PULLDOWN_START_BIT 7
#define GPIO_16_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_2_REG 0x1031
#define GPIO_16_PULLDOWN_PARAM_MASK  0x80

//  
//  0: GPIO15 pull down is enabled 
//  1: GPIO15 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 6:6, typedef gpio_pulldown
//   
//  
#define GPIO_15_PULLDOWN_MASK 0x40
#define GPIO_15_PULLDOWN_OFFSET 6
#define GPIO_15_PULLDOWN_STOP_BIT 6
#define GPIO_15_PULLDOWN_START_BIT 6
#define GPIO_15_PULLDOWN_WIDTH 1

#define GPIO_15_PULLDOWN_PARAM_MASK  0x40

//  
//  0: GPIO14 pull down is enabled 
//  1: GPIO14 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_14_PULLDOWN_MASK 0x20
#define GPIO_14_PULLDOWN_OFFSET 5
#define GPIO_14_PULLDOWN_STOP_BIT 5
#define GPIO_14_PULLDOWN_START_BIT 5
#define GPIO_14_PULLDOWN_WIDTH 1

#define GPIO_14_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO13 pull down is enabled 
//  1: GPIO13 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_13_PULLDOWN_MASK 0x20
#define GPIO_13_PULLDOWN_OFFSET 5
#define GPIO_13_PULLDOWN_STOP_BIT 5
#define GPIO_13_PULLDOWN_START_BIT 5
#define GPIO_13_PULLDOWN_WIDTH 1

#define GPIO_13_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO12 pull down is enabled 
//  1: GPIO12 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 3:3, typedef gpio_pulldown
//   
//  
#define GPIO_12_PULLDOWN_MASK 0x8
#define GPIO_12_PULLDOWN_OFFSET 3
#define GPIO_12_PULLDOWN_STOP_BIT 3
#define GPIO_12_PULLDOWN_START_BIT 3
#define GPIO_12_PULLDOWN_WIDTH 1

#define GPIO_12_PULLDOWN_PARAM_MASK  0x8

//  
//  0: GPIO11 pull down is enabled 
//  1: GPIO11 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 2:2, typedef gpio_pulldown
//   
//  
#define GPIO_11_PULLDOWN_MASK 0x4
#define GPIO_11_PULLDOWN_OFFSET 2
#define GPIO_11_PULLDOWN_STOP_BIT 2
#define GPIO_11_PULLDOWN_START_BIT 2
#define GPIO_11_PULLDOWN_WIDTH 1

#define GPIO_11_PULLDOWN_PARAM_MASK  0x4

//  
//  0: GPIO10 pull down is enabled 
//  1: GPIO10 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_10_PULLDOWN_MASK 0x2
#define GPIO_10_PULLDOWN_OFFSET 1
#define GPIO_10_PULLDOWN_STOP_BIT 1
#define GPIO_10_PULLDOWN_START_BIT 1
#define GPIO_10_PULLDOWN_WIDTH 1

#define GPIO_10_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO9 pull down is enabled 
//  1: GPIO9 pull down is disabled.
//  Register GpioPullControl2 0x1031, Bits 0:0, typedef gpio_pulldown
//   
//  
#define GPIO_9_PULLDOWN_MASK 0x1
#define GPIO_9_PULLDOWN_OFFSET 0
#define GPIO_9_PULLDOWN_STOP_BIT 0
#define GPIO_9_PULLDOWN_START_BIT 0
#define GPIO_9_PULLDOWN_WIDTH 1

#define GPIO_9_PULLDOWN_PARAM_MASK  0x1

//  
//  0: GPIO24 pull down is enabled 
//  1: GPIO24 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 7:7, typedef gpio_pulldown
//   
//  
#define GPIO_24_PULLDOWN_MASK 0x80
#define GPIO_24_PULLDOWN_OFFSET 7
#define GPIO_24_PULLDOWN_STOP_BIT 7
#define GPIO_24_PULLDOWN_START_BIT 7
#define GPIO_24_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_3_REG 0x1032
#define GPIO_24_PULLDOWN_PARAM_MASK  0x80

//  
//  0: GPIO23 pull down is enabled 
//  1: GPIO23 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 6:6, typedef gpio_pulldown
//   
//  
#define GPIO_23_PULLDOWN_MASK 0x40
#define GPIO_23_PULLDOWN_OFFSET 6
#define GPIO_23_PULLDOWN_STOP_BIT 6
#define GPIO_23_PULLDOWN_START_BIT 6
#define GPIO_23_PULLDOWN_WIDTH 1

#define GPIO_23_PULLDOWN_PARAM_MASK  0x40

//  
//  0: GPIO22 pull down is enabled 
//  1: GPIO22 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_22_PULLDOWN_MASK 0x20
#define GPIO_22_PULLDOWN_OFFSET 5
#define GPIO_22_PULLDOWN_STOP_BIT 5
#define GPIO_22_PULLDOWN_START_BIT 5
#define GPIO_22_PULLDOWN_WIDTH 1

#define GPIO_22_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO21 pull down is enabled 
//  1: GPIO21 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_21_PULLDOWN_MASK 0x20
#define GPIO_21_PULLDOWN_OFFSET 5
#define GPIO_21_PULLDOWN_STOP_BIT 5
#define GPIO_21_PULLDOWN_START_BIT 5
#define GPIO_21_PULLDOWN_WIDTH 1

#define GPIO_21_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO20 pull down is enabled 
//  1: GPIO20 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 3:3, typedef gpio_pulldown
//   
//  
#define GPIO_20_PULLDOWN_MASK 0x8
#define GPIO_20_PULLDOWN_OFFSET 3
#define GPIO_20_PULLDOWN_STOP_BIT 3
#define GPIO_20_PULLDOWN_START_BIT 3
#define GPIO_20_PULLDOWN_WIDTH 1

#define GPIO_20_PULLDOWN_PARAM_MASK  0x8

//  
//  0: GPIO19 pull down is enabled 
//  1: GPIO19 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 2:2, typedef gpio_pulldown
//   
//  
#define GPIO_19_PULLDOWN_MASK 0x4
#define GPIO_19_PULLDOWN_OFFSET 2
#define GPIO_19_PULLDOWN_STOP_BIT 2
#define GPIO_19_PULLDOWN_START_BIT 2
#define GPIO_19_PULLDOWN_WIDTH 1

#define GPIO_19_PULLDOWN_PARAM_MASK  0x4

//  
//  0: GPIO18 pull down is enabled 
//  1: GPIO18 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_18_PULLDOWN_MASK 0x2
#define GPIO_18_PULLDOWN_OFFSET 1
#define GPIO_18_PULLDOWN_STOP_BIT 1
#define GPIO_18_PULLDOWN_START_BIT 1
#define GPIO_18_PULLDOWN_WIDTH 1

#define GPIO_18_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO17 pull down is enabled 
//  1: GPIO17 pull down is disabled.
//  Register GpioPullControl3 0x1032, Bits 0:0, typedef gpio_pulldown
//   
//  
#define GPIO_17_PULLDOWN_MASK 0x1
#define GPIO_17_PULLDOWN_OFFSET 0
#define GPIO_17_PULLDOWN_STOP_BIT 0
#define GPIO_17_PULLDOWN_START_BIT 0
#define GPIO_17_PULLDOWN_WIDTH 1

#define GPIO_17_PULLDOWN_PARAM_MASK  0x1

//  
//  0: GPIO32 pull down is enabled 
//  1: GPIO32 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 7:7, typedef gpio_pulldown
//   
//  
#define GPIO_32_PULLDOWN_MASK 0x80
#define GPIO_32_PULLDOWN_OFFSET 7
#define GPIO_32_PULLDOWN_STOP_BIT 7
#define GPIO_32_PULLDOWN_START_BIT 7
#define GPIO_32_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_4_REG 0x1033
#define GPIO_32_PULLDOWN_PARAM_MASK  0x80

//  
//  0: GPIO31 pull down is enabled 
//  1: GPIO31 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 6:6, typedef gpio_pulldown
//   
//  
#define GPIO_31_PULLDOWN_MASK 0x40
#define GPIO_31_PULLDOWN_OFFSET 6
#define GPIO_31_PULLDOWN_STOP_BIT 6
#define GPIO_31_PULLDOWN_START_BIT 6
#define GPIO_31_PULLDOWN_WIDTH 1

#define GPIO_31_PULLDOWN_PARAM_MASK  0x40

//  
//  0: GPIO30 pull down is enabled 
//  1: GPIO30 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_30_PULLDOWN_MASK 0x20
#define GPIO_30_PULLDOWN_OFFSET 5
#define GPIO_30_PULLDOWN_STOP_BIT 5
#define GPIO_30_PULLDOWN_START_BIT 5
#define GPIO_30_PULLDOWN_WIDTH 1

#define GPIO_30_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO29 pull down is enabled 
//  1: GPIO29 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_29_PULLDOWN_MASK 0x20
#define GPIO_29_PULLDOWN_OFFSET 5
#define GPIO_29_PULLDOWN_STOP_BIT 5
#define GPIO_29_PULLDOWN_START_BIT 5
#define GPIO_29_PULLDOWN_WIDTH 1

#define GPIO_29_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO28 pull down is enabled 
//  1: GPIO28 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 3:3, typedef gpio_pulldown
//   
//  
#define GPIO_28_PULLDOWN_MASK 0x8
#define GPIO_28_PULLDOWN_OFFSET 3
#define GPIO_28_PULLDOWN_STOP_BIT 3
#define GPIO_28_PULLDOWN_START_BIT 3
#define GPIO_28_PULLDOWN_WIDTH 1

#define GPIO_28_PULLDOWN_PARAM_MASK  0x8

//  
//  0: GPIO27 pull down is enabled 
//  1: GPIO27 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 2:2, typedef gpio_pulldown
//   
//  
#define GPIO_27_PULLDOWN_MASK 0x4
#define GPIO_27_PULLDOWN_OFFSET 2
#define GPIO_27_PULLDOWN_STOP_BIT 2
#define GPIO_27_PULLDOWN_START_BIT 2
#define GPIO_27_PULLDOWN_WIDTH 1

#define GPIO_27_PULLDOWN_PARAM_MASK  0x4

//  
//  0: GPIO26 pull down is enabled 
//  1: GPIO26 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_26_PULLDOWN_MASK 0x2
#define GPIO_26_PULLDOWN_OFFSET 1
#define GPIO_26_PULLDOWN_STOP_BIT 1
#define GPIO_26_PULLDOWN_START_BIT 1
#define GPIO_26_PULLDOWN_WIDTH 1

#define GPIO_26_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO25 pull down is enabled 
//  1: GPIO25 pull down is disabled.
//  Register GpioPullControl4 0x1033, Bits 0:0, typedef gpio_pulldown
//   
//  
#define GPIO_25_PULLDOWN_MASK 0x1
#define GPIO_25_PULLDOWN_OFFSET 0
#define GPIO_25_PULLDOWN_STOP_BIT 0
#define GPIO_25_PULLDOWN_START_BIT 0
#define GPIO_25_PULLDOWN_WIDTH 1

#define GPIO_25_PULLDOWN_PARAM_MASK  0x1

//  
//  0: GPIO40 pull down is enabled 
//  1: GPIO40 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 7:7, typedef gpio_pulldown
//   
//  
#define GPIO_40_PULLDOWN_MASK 0x80
#define GPIO_40_PULLDOWN_OFFSET 7
#define GPIO_40_PULLDOWN_STOP_BIT 7
#define GPIO_40_PULLDOWN_START_BIT 7
#define GPIO_40_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_5_REG 0x1034
#define GPIO_40_PULLDOWN_PARAM_MASK  0x80

//  
//  0: GPIO39 pull down is enabled 
//  1: GPIO39 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 6:6, typedef gpio_pulldown
//   
//  
#define GPIO_39_PULLDOWN_MASK 0x40
#define GPIO_39_PULLDOWN_OFFSET 6
#define GPIO_39_PULLDOWN_STOP_BIT 6
#define GPIO_39_PULLDOWN_START_BIT 6
#define GPIO_39_PULLDOWN_WIDTH 1

#define GPIO_39_PULLDOWN_PARAM_MASK  0x40

//  
//  0: GPIO38 pull down is enabled 
//  1: GPIO38 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_38_PULLDOWN_MASK 0x20
#define GPIO_38_PULLDOWN_OFFSET 5
#define GPIO_38_PULLDOWN_STOP_BIT 5
#define GPIO_38_PULLDOWN_START_BIT 5
#define GPIO_38_PULLDOWN_WIDTH 1

#define GPIO_38_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO37 pull down is enabled 
//  1: GPIO37 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 5:5, typedef gpio_pulldown
//   
//  
#define GPIO_37_PULLDOWN_MASK 0x20
#define GPIO_37_PULLDOWN_OFFSET 5
#define GPIO_37_PULLDOWN_STOP_BIT 5
#define GPIO_37_PULLDOWN_START_BIT 5
#define GPIO_37_PULLDOWN_WIDTH 1

#define GPIO_37_PULLDOWN_PARAM_MASK  0x20

//  
//  0: GPIO36 pull down is enabled 
//  1: GPIO36 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 3:3, typedef gpio_pulldown
//   
//  
#define GPIO_36_PULLDOWN_MASK 0x8
#define GPIO_36_PULLDOWN_OFFSET 3
#define GPIO_36_PULLDOWN_STOP_BIT 3
#define GPIO_36_PULLDOWN_START_BIT 3
#define GPIO_36_PULLDOWN_WIDTH 1

#define GPIO_36_PULLDOWN_PARAM_MASK  0x8

//  
//  0: GPIO35 pull down is enabled 
//  1: GPIO35 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 2:2, typedef gpio_pulldown
//   
//  
#define GPIO_35_PULLDOWN_MASK 0x4
#define GPIO_35_PULLDOWN_OFFSET 2
#define GPIO_35_PULLDOWN_STOP_BIT 2
#define GPIO_35_PULLDOWN_START_BIT 2
#define GPIO_35_PULLDOWN_WIDTH 1

#define GPIO_35_PULLDOWN_PARAM_MASK  0x4

//  
//  0: GPIO34 pull down is enabled 
//  1: GPIO34 pull down is disabled.
//  Register GpioPullControl5 0x1034, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_34_PULLDOWN_MASK 0x2
#define GPIO_34_PULLDOWN_OFFSET 1
#define GPIO_34_PULLDOWN_STOP_BIT 1
#define GPIO_34_PULLDOWN_START_BIT 1
#define GPIO_34_PULLDOWN_WIDTH 1

#define GPIO_34_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO42 pull down is enabled 
//  1: GPIO42 pull down is disabled.
//  Register GpioPullControl6 0x1035, Bits 1:1, typedef gpio_pulldown
//   
//  
#define GPIO_42_PULLDOWN_MASK 0x2
#define GPIO_42_PULLDOWN_OFFSET 1
#define GPIO_42_PULLDOWN_STOP_BIT 1
#define GPIO_42_PULLDOWN_START_BIT 1
#define GPIO_42_PULLDOWN_WIDTH 1

#define GPIO_PULL_CONTROL_6_REG 0x1035
#define GPIO_42_PULLDOWN_PARAM_MASK  0x2

//  
//  0: GPIO41 pull down is enabled 
//  1: GPIO41 pull down is disabled.
//  Register GpioPullControl6 0x1035, Bits 0:0, typedef gpio_pulldown
//   
//  
#define GPIO_41_PULLDOWN_MASK 0x1
#define GPIO_41_PULLDOWN_OFFSET 0
#define GPIO_41_PULLDOWN_STOP_BIT 0
#define GPIO_41_PULLDOWN_START_BIT 0
#define GPIO_41_PULLDOWN_WIDTH 1

#define GPIO_41_PULLDOWN_PARAM_MASK  0x1

//  
//  0: GPIO8 input egals "0" 
//  1: GPIO8 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 7:7, typedef gpio_input
//   
//  
#define GPIO_8_INPUT_MASK 0x80
#define GPIO_8_INPUT_OFFSET 7
#define GPIO_8_INPUT_STOP_BIT 7
#define GPIO_8_INPUT_START_BIT 7
#define GPIO_8_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_1_REG 0x1040
#define GPIO_8_INPUT_PARAM_MASK  0x80
#define GPIO_INPUT_PARAM_MASK  0x80

//  
//  0: GPIO7 input egals "0" 
//  1: GPIO7 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 6:6, typedef gpio_input
//   
//  
#define GPIO_7_INPUT_MASK 0x40
#define GPIO_7_INPUT_OFFSET 6
#define GPIO_7_INPUT_STOP_BIT 6
#define GPIO_7_INPUT_START_BIT 6
#define GPIO_7_INPUT_WIDTH 1

#define GPIO_7_INPUT_PARAM_MASK  0x40

//  
//  0: GPIO6 input egals "0" 
//  1: GPIO6 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 5:5, typedef gpio_input
//   
//  
#define GPIO_6_INPUT_MASK 0x20
#define GPIO_6_INPUT_OFFSET 5
#define GPIO_6_INPUT_STOP_BIT 5
#define GPIO_6_INPUT_START_BIT 5
#define GPIO_6_INPUT_WIDTH 1

#define GPIO_6_INPUT_PARAM_MASK  0x20

//  
//  0: GPIO4 input egals "0" 
//  1: GPIO4 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 3:3, typedef gpio_input
//   
//  
#define GPIO_4_INPUT_MASK 0x8
#define GPIO_4_INPUT_OFFSET 3
#define GPIO_4_INPUT_STOP_BIT 3
#define GPIO_4_INPUT_START_BIT 3
#define GPIO_4_INPUT_WIDTH 1

#define GPIO_4_INPUT_PARAM_MASK  0x8

//  
//  0: GPIO3 input egals "0" 
//  1: GPIO3 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 2:2, typedef gpio_input
//   
//  
#define GPIO_3_INPUT_MASK 0x4
#define GPIO_3_INPUT_OFFSET 2
#define GPIO_3_INPUT_STOP_BIT 2
#define GPIO_3_INPUT_START_BIT 2
#define GPIO_3_INPUT_WIDTH 1

#define GPIO_3_INPUT_PARAM_MASK  0x4

//  
//  0: GPIO2 input egals "0" 
//  1: GPIO2 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_2_INPUT_MASK 0x2
#define GPIO_2_INPUT_OFFSET 1
#define GPIO_2_INPUT_STOP_BIT 1
#define GPIO_2_INPUT_START_BIT 1
#define GPIO_2_INPUT_WIDTH 1

#define GPIO_2_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO1 input egals "0" 
//  1: GPIO1 input egals "1".
//  Register GpioInputControl1 0x1040, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_1_INPUT_MASK 0x1
#define GPIO_1_INPUT_OFFSET 0
#define GPIO_1_INPUT_STOP_BIT 0
#define GPIO_1_INPUT_START_BIT 0
#define GPIO_1_INPUT_WIDTH 1

#define GPIO_1_INPUT_PARAM_MASK  0x1

//  
//  0: GPIO16 input egals "0" 
//  1: GPIO16 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 7:7, typedef gpio_input
//   
//  
#define GPIO_16_INPUT_MASK 0x80
#define GPIO_16_INPUT_OFFSET 7
#define GPIO_16_INPUT_STOP_BIT 7
#define GPIO_16_INPUT_START_BIT 7
#define GPIO_16_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_2_REG 0x1041
#define GPIO_16_INPUT_PARAM_MASK  0x80

//  
//  0: GPIO15 input egals "0" 
//  1: GPIO15 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 6:6, typedef gpio_input
//   
//  
#define GPIO_15_INPUT_MASK 0x40
#define GPIO_15_INPUT_OFFSET 6
#define GPIO_15_INPUT_STOP_BIT 6
#define GPIO_15_INPUT_START_BIT 6
#define GPIO_15_INPUT_WIDTH 1

#define GPIO_15_INPUT_PARAM_MASK  0x40

//  
//  0: GPIO14 input egals "0" 
//  1: GPIO14 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 5:5, typedef gpio_input
//   
//  
#define GPIO_14_INPUT_MASK 0x20
#define GPIO_14_INPUT_OFFSET 5
#define GPIO_14_INPUT_STOP_BIT 5
#define GPIO_14_INPUT_START_BIT 5
#define GPIO_14_INPUT_WIDTH 1

#define GPIO_14_INPUT_PARAM_MASK  0x20

//  
//  0: GPIO13 input egals "0" 
//  1: GPIO13 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 4:4, typedef gpio_input
//   
//  
#define GPIO_13_INPUT_MASK 0x10
#define GPIO_13_INPUT_OFFSET 4
#define GPIO_13_INPUT_STOP_BIT 4
#define GPIO_13_INPUT_START_BIT 4
#define GPIO_13_INPUT_WIDTH 1

#define GPIO_13_INPUT_PARAM_MASK  0x10

//  
//  0: GPIO12 input egals "0" 
//  1: GPIO12 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 3:3, typedef gpio_input
//   
//  
#define GPIO_12_INPUT_MASK 0x8
#define GPIO_12_INPUT_OFFSET 3
#define GPIO_12_INPUT_STOP_BIT 3
#define GPIO_12_INPUT_START_BIT 3
#define GPIO_12_INPUT_WIDTH 1

#define GPIO_12_INPUT_PARAM_MASK  0x8

//  
//  0: GPIO11 input egals "0" 
//  1: GPIO11 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 2:2, typedef gpio_input
//   
//  
#define GPIO_11_INPUT_MASK 0x4
#define GPIO_11_INPUT_OFFSET 2
#define GPIO_11_INPUT_STOP_BIT 2
#define GPIO_11_INPUT_START_BIT 2
#define GPIO_11_INPUT_WIDTH 1

#define GPIO_11_INPUT_PARAM_MASK  0x4

//  
//  0: GPIO10 input egals "0" 
//  1: GPIO10 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_10_INPUT_MASK 0x2
#define GPIO_10_INPUT_OFFSET 1
#define GPIO_10_INPUT_STOP_BIT 1
#define GPIO_10_INPUT_START_BIT 1
#define GPIO_10_INPUT_WIDTH 1

#define GPIO_10_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO9 input egals "0" 
//  1: GPIO9 input egals "1".
//  Register GpioInputControl2 0x1041, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_9_INPUT_MASK 0x1
#define GPIO_9_INPUT_OFFSET 0
#define GPIO_9_INPUT_STOP_BIT 0
#define GPIO_9_INPUT_START_BIT 0
#define GPIO_9_INPUT_WIDTH 1

#define GPIO_9_INPUT_PARAM_MASK  0x1

//  
//  0: GPIO24 input egals "0" 
//  1: GPIO24 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 7:7, typedef gpio_input
//   
//  
#define GPIO_24_INPUT_MASK 0x80
#define GPIO_24_INPUT_OFFSET 7
#define GPIO_24_INPUT_STOP_BIT 7
#define GPIO_24_INPUT_START_BIT 7
#define GPIO_24_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_3_REG 0x1042
#define GPIO_24_INPUT_PARAM_MASK  0x80

//  
//  0: GPIO23 input egals "0" 
//  1: GPIO23 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 6:6, typedef gpio_input
//   
//  
#define GPIO_23_INPUT_MASK 0x40
#define GPIO_23_INPUT_OFFSET 6
#define GPIO_23_INPUT_STOP_BIT 6
#define GPIO_23_INPUT_START_BIT 6
#define GPIO_23_INPUT_WIDTH 1

#define GPIO_23_INPUT_PARAM_MASK  0x40

//  
//  0: GPIO22 input egals "0" 
//  1: GPIO22 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 5:5, typedef gpio_input
//   
//  
#define GPIO_22_INPUT_MASK 0x20
#define GPIO_22_INPUT_OFFSET 5
#define GPIO_22_INPUT_STOP_BIT 5
#define GPIO_22_INPUT_START_BIT 5
#define GPIO_22_INPUT_WIDTH 1

#define GPIO_22_INPUT_PARAM_MASK  0x20

//  
//  0: GPIO21 input egals "0" 
//  1: GPIO21 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 4:4, typedef gpio_input
//   
//  
#define GPIO_21_INPUT_MASK 0x10
#define GPIO_21_INPUT_OFFSET 4
#define GPIO_21_INPUT_STOP_BIT 4
#define GPIO_21_INPUT_START_BIT 4
#define GPIO_21_INPUT_WIDTH 1

#define GPIO_21_INPUT_PARAM_MASK  0x10

//  
//  0: GPIO20 input egals "0" 
//  1: GPIO20 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 3:3, typedef gpio_input
//   
//  
#define GPIO_20_INPUT_MASK 0x8
#define GPIO_20_INPUT_OFFSET 3
#define GPIO_20_INPUT_STOP_BIT 3
#define GPIO_20_INPUT_START_BIT 3
#define GPIO_20_INPUT_WIDTH 1

#define GPIO_20_INPUT_PARAM_MASK  0x8

//  
//  0: GPIO19 input egals "0" 
//  1: GPIO19 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 2:2, typedef gpio_input
//   
//  
#define GPIO_19_INPUT_MASK 0x4
#define GPIO_19_INPUT_OFFSET 2
#define GPIO_19_INPUT_STOP_BIT 2
#define GPIO_19_INPUT_START_BIT 2
#define GPIO_19_INPUT_WIDTH 1

#define GPIO_19_INPUT_PARAM_MASK  0x4

//  
//  0: GPIO18 input egals "0" 
//  1: GPIO18 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_18_INPUT_MASK 0x2
#define GPIO_18_INPUT_OFFSET 1
#define GPIO_18_INPUT_STOP_BIT 1
#define GPIO_18_INPUT_START_BIT 1
#define GPIO_18_INPUT_WIDTH 1

#define GPIO_18_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO17 input egals "0" 
//  1: GPIO17 input egals "1".
//  Register GpioInputControl3 0x1042, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_17_INPUT_MASK 0x1
#define GPIO_17_INPUT_OFFSET 0
#define GPIO_17_INPUT_STOP_BIT 0
#define GPIO_17_INPUT_START_BIT 0
#define GPIO_17_INPUT_WIDTH 1

#define GPIO_17_INPUT_PARAM_MASK  0x1

//  
//  0: GPIO32 input egals "0" 
//  1: GPIO32 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 7:7, typedef gpio_input
//   
//  
#define GPIO_32_INPUT_MASK 0x80
#define GPIO_32_INPUT_OFFSET 7
#define GPIO_32_INPUT_STOP_BIT 7
#define GPIO_32_INPUT_START_BIT 7
#define GPIO_32_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_4_REG 0x1043
#define GPIO_32_INPUT_PARAM_MASK  0x80

//  
//  0: GPIO31 input egals "0" 
//  1: GPIO31 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 6:6, typedef gpio_input
//   
//  
#define GPIO_31_INPUT_MASK 0x40
#define GPIO_31_INPUT_OFFSET 6
#define GPIO_31_INPUT_STOP_BIT 6
#define GPIO_31_INPUT_START_BIT 6
#define GPIO_31_INPUT_WIDTH 1

#define GPIO_31_INPUT_PARAM_MASK  0x40

//  
//  0: GPIO30 input egals "0" 
//  1: GPIO30 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 5:5, typedef gpio_input
//   
//  
#define GPIO_30_INPUT_MASK 0x20
#define GPIO_30_INPUT_OFFSET 5
#define GPIO_30_INPUT_STOP_BIT 5
#define GPIO_30_INPUT_START_BIT 5
#define GPIO_30_INPUT_WIDTH 1

#define GPIO_30_INPUT_PARAM_MASK  0x20

//  
//  0: GPIO29 input egals "0" 
//  1: GPIO29 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 4:4, typedef gpio_input
//   
//  
#define GPIO_29_INPUT_MASK 0x10
#define GPIO_29_INPUT_OFFSET 4
#define GPIO_29_INPUT_STOP_BIT 4
#define GPIO_29_INPUT_START_BIT 4
#define GPIO_29_INPUT_WIDTH 1

#define GPIO_29_INPUT_PARAM_MASK  0x10

//  
//  0: GPIO28 input egals "0" 
//  1: GPIO28 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 3:3, typedef gpio_input
//   
//  
#define GPIO_28_INPUT_MASK 0x8
#define GPIO_28_INPUT_OFFSET 3
#define GPIO_28_INPUT_STOP_BIT 3
#define GPIO_28_INPUT_START_BIT 3
#define GPIO_28_INPUT_WIDTH 1

#define GPIO_28_INPUT_PARAM_MASK  0x8

//  
//  0: GPIO27 input egals "0" 
//  1: GPIO27 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 2:2, typedef gpio_input
//   
//  
#define GPIO_27_INPUT_MASK 0x4
#define GPIO_27_INPUT_OFFSET 2
#define GPIO_27_INPUT_STOP_BIT 2
#define GPIO_27_INPUT_START_BIT 2
#define GPIO_27_INPUT_WIDTH 1

#define GPIO_27_INPUT_PARAM_MASK  0x4

//  
//  0: GPIO26 input egals "0" 
//  1: GPIO26 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_26_INPUT_MASK 0x2
#define GPIO_26_INPUT_OFFSET 1
#define GPIO_26_INPUT_STOP_BIT 1
#define GPIO_26_INPUT_START_BIT 1
#define GPIO_26_INPUT_WIDTH 1

#define GPIO_26_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO25 input egals "0" 
//  1: GPIO25 input egals "1".
//  Register GpioInputControl4 0x1043, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_25_INPUT_MASK 0x1
#define GPIO_25_INPUT_OFFSET 0
#define GPIO_25_INPUT_STOP_BIT 0
#define GPIO_25_INPUT_START_BIT 0
#define GPIO_25_INPUT_WIDTH 1

#define GPIO_25_INPUT_PARAM_MASK  0x1

//  
//  0: GPIO40 input egals "0" 
//  1: GPIO40 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 7:7, typedef gpio_input
//   
//  
#define GPIO_40_INPUT_MASK 0x80
#define GPIO_40_INPUT_OFFSET 7
#define GPIO_40_INPUT_STOP_BIT 7
#define GPIO_40_INPUT_START_BIT 7
#define GPIO_40_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_5_REG 0x1044
#define GPIO_40_INPUT_PARAM_MASK  0x80

//  
//  0: GPIO39 input egals "0" 
//  1: GPIO39 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 6:6, typedef gpio_input
//   
//  
#define GPIO_39_INPUT_MASK 0x40
#define GPIO_39_INPUT_OFFSET 6
#define GPIO_39_INPUT_STOP_BIT 6
#define GPIO_39_INPUT_START_BIT 6
#define GPIO_39_INPUT_WIDTH 1

#define GPIO_39_INPUT_PARAM_MASK  0x40

//  
//  0: GPIO38 input egals "0" 
//  1: GPIO38 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 5:5, typedef gpio_input
//   
//  
#define GPIO_38_INPUT_MASK 0x20
#define GPIO_38_INPUT_OFFSET 5
#define GPIO_38_INPUT_STOP_BIT 5
#define GPIO_38_INPUT_START_BIT 5
#define GPIO_38_INPUT_WIDTH 1

#define GPIO_38_INPUT_PARAM_MASK  0x20

//  
//  0: GPIO37 input egals "0" 
//  1: GPIO37 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 4:4, typedef gpio_input
//   
//  
#define GPIO_37_INPUT_MASK 0x10
#define GPIO_37_INPUT_OFFSET 4
#define GPIO_37_INPUT_STOP_BIT 4
#define GPIO_37_INPUT_START_BIT 4
#define GPIO_37_INPUT_WIDTH 1

#define GPIO_37_INPUT_PARAM_MASK  0x10

//  
//  0: GPIO36 input egals "0" 
//  1: GPIO36 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 3:3, typedef gpio_input
//   
//  
#define GPIO_36_INPUT_MASK 0x8
#define GPIO_36_INPUT_OFFSET 3
#define GPIO_36_INPUT_STOP_BIT 3
#define GPIO_36_INPUT_START_BIT 3
#define GPIO_36_INPUT_WIDTH 1

#define GPIO_36_INPUT_PARAM_MASK  0x8

//  
//  0: GPIO35 input egals "0" 
//  1: GPIO35 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 2:2, typedef gpio_input
//   
//  
#define GPIO_35_INPUT_MASK 0x4
#define GPIO_35_INPUT_OFFSET 2
#define GPIO_35_INPUT_STOP_BIT 2
#define GPIO_35_INPUT_START_BIT 2
#define GPIO_35_INPUT_WIDTH 1

#define GPIO_35_INPUT_PARAM_MASK  0x4

//  
//  0: GPIO34 input egals "0" 
//  1: GPIO34 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_34_INPUT_MASK 0x2
#define GPIO_34_INPUT_OFFSET 1
#define GPIO_34_INPUT_STOP_BIT 1
#define GPIO_34_INPUT_START_BIT 1
#define GPIO_34_INPUT_WIDTH 1

#define GPIO_34_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO33 input egals "0" 
//  1: GPIO33 input egals "1".
//  Register GpioInputControl5 0x1044, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_33_INPUT_MASK 0x1
#define GPIO_33_INPUT_OFFSET 0
#define GPIO_33_INPUT_STOP_BIT 0
#define GPIO_33_INPUT_START_BIT 0
#define GPIO_33_INPUT_WIDTH 1

#define GPIO_33_INPUT_PARAM_MASK  0x1

//  
//  0: GPIO42 input egals "0" 
//  1: GPIO42 input egals "1".
//  Register GpioInputControl6 0x1045, Bits 1:1, typedef gpio_input
//   
//  
#define GPIO_42_INPUT_MASK 0x2
#define GPIO_42_INPUT_OFFSET 1
#define GPIO_42_INPUT_STOP_BIT 1
#define GPIO_42_INPUT_START_BIT 1
#define GPIO_42_INPUT_WIDTH 1

#define GPIO_INPUT_CONTROL_6_REG 0x1045
#define GPIO_42_INPUT_PARAM_MASK  0x2

//  
//  0: GPIO41 input egals "0" 
//  1: GPIO41 input egals "1".
//  Register GpioInputControl6 0x1045, Bits 0:0, typedef gpio_input
//   
//  
#define GPIO_41_INPUT_MASK 0x1
#define GPIO_41_INPUT_OFFSET 0
#define GPIO_41_INPUT_STOP_BIT 0
#define GPIO_41_INPUT_START_BIT 0
#define GPIO_41_INPUT_WIDTH 1

#define GPIO_41_INPUT_PARAM_MASK  0x1

//  
//  00: select YcbCr7 function on pad Gpio13 
//  01: select I2cTrig2 function on pad Gpio13 
//  10 or 11: select USBVdat on pad Gpio13 (0: indicates 100mA, 1: indicates 
//  500mA).
//  Register GpioAlternate1 0x1050, Bits 3:4, typedef gpio13_alternate
//   
//  
#define GPIO_13_ALTERNATE_MASK 0x18
#define GPIO_13_ALTERNATE_OFFSET 4
#define GPIO_13_ALTERNATE_STOP_BIT 4
#define GPIO_13_ALTERNATE_START_BIT 3
#define GPIO_13_ALTERNATE_WIDTH 2

#define GPIO_ALTERNATE_1_REG 0x1050
#define  YCBCR7 0
#define  I2C_TRIG2 1
#define  USB_VDAT_100 2
#define  USB_VDAT_500 3
#define GPIO_13_ALTERNATE_PARAM_MASK  0x18



typedef enum gpio13_alternate {
    YCBCR7_E,
    I2C_TRIG2_E,
    USB_VDAT_100_E,
    USB_VDAT_500_E
} GPIO_13_ALTERNATE_T ;




//  
//  0: select YcbCr6 function on pad Gpio12 
//  1: select I2cTrig1 function on pad Gpio12.
//  Register GpioAlternate1 0x1050, Bits 2:2, typedef gpio12_alternate
//   
//  
#define GPIO_12_ALTERNATE_MASK 0x4
#define GPIO_12_ALTERNATE_OFFSET 2
#define GPIO_12_ALTERNATE_STOP_BIT 2
#define GPIO_12_ALTERNATE_START_BIT 2
#define GPIO_12_ALTERNATE_WIDTH 1

#define  GPIO_12_ALTERNATE 4

/*+CAP_948_001*/
typedef enum gpio12_alternate {
    YCBCR6_E,
    I2C_TRIG1_E
} GPIO_12_ALTERNATE_T ;
/*-CAP_948_001*/
#define GPIO_12_ALTERNATE_PARAM_MASK  0x4



//  
//  0: select YcbCr5 function on pad Gpio11 
//  1: select UsbUiccPd function on pad Gpio11.
//  Register GpioAlternate1 0x1050, Bits 1:1, typedef gpio11_alternate
//   
//  
#define GPIO_11_ALTERNATE_MASK 0x2
#define GPIO_11_ALTERNATE_OFFSET 1
#define GPIO_11_ALTERNATE_STOP_BIT 1
#define GPIO_11_ALTERNATE_START_BIT 1
#define GPIO_11_ALTERNATE_WIDTH 1

#define  GPIO_11_ALTERNATE 2

/*+CAP_948_001*/
typedef enum gpio11_alternate {
    YCBCR5_E,
	USBUICCPD_E,
} GPIO_11_ALTERNATE_T ;
/*-CAP_948_001*/
#define GPIO_11_ALTERNATE_PARAM_MASK  0x2



//  
//  0: select YcbCr4 function on pad Gpio10 
//  1: select HiqClkEna function on pad Gpio10.
//  Register GpioAlternate1 0x1050, Bits 0:0, typedef gpio10_alternate
//   
//  
#define GPIO_10_ALTERNATE_MASK 0x1
#define GPIO_10_ALTERNATE_OFFSET 0
#define GPIO_10_ALTERNATE_STOP_BIT 0
#define GPIO_10_ALTERNATE_START_BIT 0
#define GPIO_10_ALTERNATE_WIDTH 1

#define  GPIO_10_ALTERNATE 1

/*+CAP_948_001*/
typedef enum gpio10_alternate {
    YCBCR4_E,
    HIQ_CLOKK_ENABLE_E
} GPIO_10_ALTERNATE_T ;
/*-CAP_948_001*/
#define GPIO_10_ALTERNATE_PARAM_MASK  0x1


#endif
