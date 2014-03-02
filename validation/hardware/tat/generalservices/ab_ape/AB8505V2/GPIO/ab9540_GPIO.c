/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_GPIO/Linux/ab9540_GPIO.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_GPIO_LINUX_AB9540_GPIO_C
#define C__LISP_PRODUCTION_AB9540_AB9540_GPIO_LINUX_AB9540_GPIO_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_GPIO.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_gpio1_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio1_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_1_REG);

    /* 
     * 0: Pad is used as SysClkReq2 
     * 1: Pad is used as GPIO1 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_1_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_1_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio1_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio1_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq2 
     * 1: Pad is used as GPIO1 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_1_REG) & GPIO_1_PAD_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio2_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio2_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_1_REG);

    /* 
     * 0: Pad is used as SysClkReq3 
     * 1: Pad is used as GPIO2 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_2_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_2_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio2_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio2_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq3 
     * 1: Pad is used as GPIO2 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_1_REG) & GPIO_2_PAD_SELECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio3_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio3_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_1_REG);

    /* 
     * 0: Pad is used as SysClkReq4 
     * 1: Pad is used as GPIO3 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_3_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_3_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio3_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio3_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq4 
     * 1: Pad is used as GPIO3 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_1_REG) & GPIO_3_PAD_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio4_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio4_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_1_REG);

    /* 
     * 0: Pad is used as SysClkReq6 
     * 1: Pad is used as GPIO4 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_4_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_4_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio4_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection1
 *
 * Notes : From register 0x1000, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio4_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq6 
     * 1: Pad is used as GPIO4 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_1_REG) & GPIO_4_PAD_SELECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio10_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as alternate function 
     * 1: Pad is used as GPIO10 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_10_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_10_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio10_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as alternate function 
     * 1: Pad is used as GPIO10 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_10_PAD_SELECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio11_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as alternate function 
     * 1: Pad is used as GPIO11 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_11_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_11_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio11_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as alternate function 
     * 1: Pad is used as GPIO11 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_11_PAD_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio13_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as alternat function 
     * 1: Pad is used as GPIO13 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_13_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_13_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio13_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as alternat function 
     * 1: Pad is used as GPIO13 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_13_PAD_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio14_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio14_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as PWMOut1 
     * 1: Pad is used as GPIO14 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_14_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_14_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio14_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio14_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as PWMOut1 
     * 1: Pad is used as GPIO14 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_14_PAD_SELECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio15_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio15_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as PWMOut2 
     * 1: Pad is used as GPIO15 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_15_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_15_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio15_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio15_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as PWMOut2 
     * 1: Pad is used as GPIO15 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_15_PAD_SELECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio16_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio16_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_2_REG);

    /* 
     * 0: Pad is used as PWMOut3 
     * 1: Pad is used as GPIO16 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_16_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_16_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio16_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection2
 *
 * Notes : From register 0x1001, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio16_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as PWMOut3 
     * 1: Pad is used as GPIO16 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_2_REG) & GPIO_16_PAD_SELECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio17_20_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio17_20_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_3_REG);

    /* 
     * 0: Pad is used as FSync1, BitClk1, DA_DATA1 and AD_Date1 
     * 1: GPIO17_18_19_20 direction is output 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_1720_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_1720_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio17_20_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio17_20_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as FSync1, BitClk1, DA_DATA1 and AD_Date1 
     * 1: GPIO17_18_19_20 direction is output 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_3_REG) & GPIO_1720_PAD_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio21_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio21_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_3_REG);

    /* 
     * 0: Pad is used as UsbUiccDir 
     * 1: Pad is used as GPIO21 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_21_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_21_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio21_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio21_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as UsbUiccDir 
     * 1: Pad is used as GPIO21 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_3_REG) & GPIO_21_PAD_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio22_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio22_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_3_REG);

    /* 
     * 0: Pad is used as UsbUiccData 
     * 1: Pad is used as GPIO22 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_22_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_22_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio22_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio22_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as UsbUiccData 
     * 1: Pad is used as GPIO22 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_3_REG) & GPIO_22_PAD_SELECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio23_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio23_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_3_REG);

    /* 
     * 0: Pad is used as UsbUiccSe0 
     * 1: Pad is used as GPIO23 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_23_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_23_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio23_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio23_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as UsbUiccSe0 
     * 1: Pad is used as GPIO23 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_3_REG) & GPIO_23_PAD_SELECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio24_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_3_REG);

    /* 
     * 0: Pad is used as SysClkReq7 
     * 1: Pad is used as GPIO24 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_24_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_24_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection3
 *
 * Notes : From register 0x1002, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio24_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq7 
     * 1: Pad is used as GPIO24 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_3_REG) & GPIO_24_PAD_SELECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio25_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: Pad is used as SysClkReq8 
     * 1: Pad is used as GPIO25 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_25_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_25_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio25_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq8 
     * 1: Pad is used as GPIO25 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_25_PAD_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio27_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio27_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic12Clk 
     * 1: select GPIO27 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_27_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_27_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio27_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio27_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic12Clk 
     * 1: select GPIO27 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_27_PAD_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio28_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio28_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic12Dat 
     * 1: select GPIO28 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_28_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_28_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio28_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio28_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic12Dat 
     * 1: select GPIO28 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_28_PAD_SELECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio29_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio29_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic34Clk 
     * 1: select GPIO29 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_29_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_29_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio29_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio29_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic34Clk 
     * 1: select GPIO29 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_29_PAD_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio30_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio30_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic34Dat 
     * 1: select GPIO30 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_30_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_30_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio30_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio30_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic34Dat 
     * 1: select GPIO30 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_30_PAD_SELECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio31_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio31_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic56Clk 
     * 1: select GPIO31 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_31_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_31_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio31_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio31_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic56Clk 
     * 1: select GPIO31 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_31_PAD_SELECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio32_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio32_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_4_REG);

    /* 
     * 0: select Dmic56Dat 
     * 1: select GPIO32 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_32_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_32_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio32_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection4
 *
 * Notes : From register 0x1003, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio32_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: select Dmic56Dat 
     * 1: select GPIO32 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_4_REG) & GPIO_32_PAD_SELECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio34_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio34_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ExtCpEna 
     * 1: Pad is used as GPIO34 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_34_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_34_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio34_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio34_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ExtCpEna 
     * 1: Pad is used as GPIO34 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_34_PAD_SELECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio36_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ApeSpiClk 
     * 1: Pad is used as GPIO36 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_36_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_36_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio36_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ApeSpiClk 
     * 1: Pad is used as GPIO36 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_36_PAD_SELECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio37_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ApeSpiCSn 
     * 1: Pad is used as GPIO37 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_37_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_37_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio37_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ApeSpiCSn 
     * 1: Pad is used as GPIO37 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_37_PAD_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio38_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ApeSpiDout 
     * 1: Pad is used as GPIO38 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_38_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_38_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio38_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ApeSpiDout 
     * 1: Pad is used as GPIO38 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_38_PAD_SELECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio39_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ApeSpiDin 
     * 1: Pad is used as GPIO39 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_39_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_39_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio39_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ApeSpiDin 
     * 1: Pad is used as GPIO39 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_39_PAD_SELECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio40_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_5_REG);

    /* 
     * 0: Pad is used as ModScl 
     * 1: Pad is used as GPIO40 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_40_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_40_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection5
 *
 * Notes : From register 0x1004, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio40_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ModScl 
     * 1: Pad is used as GPIO40 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_5_REG) & GPIO_40_PAD_SELECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection6
 *
 * Notes : From register 0x1005, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio41_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_6_REG);

    /* 
     * 0: Pad is used as ModSda 
     * 1: Pad is used asGPIO41 direction is output 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_41_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_41_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection6
 *
 * Notes : From register 0x1005, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio41_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as ModSda 
     * 1: Pad is used asGPIO41 direction is output 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_6_REG) & GPIO_41_PAD_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio42_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection6
 *
 * Notes : From register 0x1005, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio42_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_6_REG);

    /* 
     * 0: Pad is used as SysClkReq5 
     * 1: Pad is used as GPIO42 direction is output 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_42_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_42_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio42_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection6
 *
 * Notes : From register 0x1005, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio42_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SysClkReq5 
     * 1: Pad is used as GPIO42 direction is output 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_6_REG) & GPIO_42_PAD_SELECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio50_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio50_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_7_REG);

    /* 
     * 0: Pad is used as audio_alternate(2)=PWM2 
     * 1: Pad is used as GPIO50 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_50_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_50_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as audio_alternate(2)=PWM2 
     * 1: Pad is used as GPIO50 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_7_REG) & GPIO_50_PAD_SELECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio51_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio51_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_7_REG);

    /* 
     * 0: Pad is used as BATTREM (default) 
     * 1: Pad is used as GPIO51 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_51_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_51_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as BATTREM (default) 
     * 1: Pad is used as GPIO51 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_7_REG) & GPIO_51_PAD_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio52_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio52_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_7_REG);

    /* 
     * 0: Pad is used as RESETHW(default) 
     * 1: Pad is used as GPIO52 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_52_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_52_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as RESETHW(default) 
     * 1: Pad is used as GPIO52 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_7_REG) & GPIO_52_PAD_SELECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio53_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio53_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_7_REG);

    /* 
     * 0: Pad is used as SERVICE (default) 
     * 1: Pad is used as GPIO53 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_53_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_53_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as SERVICE (default) 
     * 1: Pad is used as GPIO53 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_7_REG) & GPIO_53_PAD_SELECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio60_pad_selection
 *
 * IN   : param, a value to write to the regiter GpioPadSelection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio60_pad_selection( enum gpio_pad_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PAD_SELECTION_7_REG);

    /* 
     * 0: Pad is used as USBUICCPD 
     * 1: Pad is used as GPIO60 
     */ 
    switch( param ){
        case USE_PAD_AS_GPIO_E: 
           value = old_value | GPIO_60_PAD_SELECTION_MASK; 
           break;
        case USE_FUNCTION_E: 
           value = old_value & ~ GPIO_60_PAD_SELECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_PAD_SELECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_pad_selection
 *
 * RET  : Return the value of register 0xGpioPadSelection7
 *
 * Notes : From register 0x1006, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_pad_selection()
  {
    unsigned char value;


    /* 
     * 0: Pad is used as USBUICCPD 
     * 1: Pad is used as GPIO60 
     */ 
    value = (I2CRead(GPIO_PAD_SELECTION_7_REG) & GPIO_60_PAD_SELECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio1_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio1_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO1 direction is input 
     * 1: GPIO1 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_1_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_1_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio1_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio1_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO1 direction is input 
     * 1: GPIO1 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_1_DIRECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio2_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio2_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO2 direction is input 
     * 1: GPIO2 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_2_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_2_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio2_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio2_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO2 direction is input 
     * 1: GPIO2 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_2_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio3_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio3_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO3 direction is input 
     * 1: GPIO3 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_3_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_3_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio3_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio3_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO3 direction is input 
     * 1: GPIO3 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_3_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio4_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio4_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO4 direction is input 
     * 1: GPIO4 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_4_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_4_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio4_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio4_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO4 direction is input 
     * 1: GPIO4 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_4_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio6_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO6 direction is input 
     * 1: GPIO6 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_6_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_6_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio6_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO6 direction is input 
     * 1: GPIO6 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_6_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio7_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO7 direction is input 
     * 1: GPIO7 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_7_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_7_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio7_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO7 direction is input 
     * 1: GPIO7 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_7_DIRECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio8_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_1_REG);

    /* 
     * 0: GPIO8 direction is input 
     * 1: GPIO8 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_8_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_8_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_direction
 *
 * RET  : Return the value of register 0xGpioDirection1
 *
 * Notes : From register 0x1010, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio8_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO8 direction is input 
     * 1: GPIO8 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_1_REG) & GPIO_8_DIRECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio9_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO9 direction is input 
     * 1: GPIO9 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_9_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_9_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio9_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO9 direction is input 
     * 1: GPIO9 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_9_DIRECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio10_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO10 direction is input 
     * 1: GPIO10 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_10_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_10_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio10_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO10 direction is input 
     * 1: GPIO10 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_10_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio11_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO11 direction is input 
     * 1: GPIO11 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_11_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_11_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio11_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO11 direction is input 
     * 1: GPIO11 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_11_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio12_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO12 direction is input 
     * 1: GPIO12 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_12_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_12_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio12_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO12 direction is input 
     * 1: GPIO12 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_12_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio13_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO13 direction is input 
     * 1: GPIO13 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_13_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_13_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio13_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO13 direction is input 
     * 1: GPIO13 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_13_DIRECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio14_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio14_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO14 direction is input 
     * 1: GPIO14 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_14_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_14_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio14_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio14_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO14 direction is input 
     * 1: GPIO14 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_14_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio15_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio15_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO15 direction is input 
     * 1: GPIO15 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_15_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_15_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio15_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio15_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO15 direction is input 
     * 1: GPIO15 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_15_DIRECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio16_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio16_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_2_REG);

    /* 
     * 0: GPIO16 direction is input 
     * 1: GPIO16 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_16_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_16_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio16_direction
 *
 * RET  : Return the value of register 0xGpioDirection2
 *
 * Notes : From register 0x1011, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio16_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO16 direction is input 
     * 1: GPIO16 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_2_REG) & GPIO_16_DIRECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio17_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio17_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO17 direction is input 
     * 1: GPIO17 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_17_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_17_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio17_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio17_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO17 direction is input 
     * 1: GPIO17 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_17_DIRECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio18_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio18_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO18 direction is input 
     * 1: GPIO18 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_18_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_18_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio18_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio18_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO18 direction is input 
     * 1: GPIO18 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_18_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio19_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio19_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO19 direction is input 
     * 1: GPIO19 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_19_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_19_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio19_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio19_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO19 direction is input 
     * 1: GPIO19 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_19_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio20_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio20_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO20 direction is input 
     * 1: GPIO20 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_20_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_20_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio20_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio20_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO20 direction is input 
     * 1: GPIO20 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_20_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio21_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio21_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO21 direction is input 
     * 1: GPIO21 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_21_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_21_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio21_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio21_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO21 direction is input 
     * 1: GPIO21 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_21_DIRECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio22_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio22_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO22 direction is input 
     * 1: GPIO22 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_22_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_22_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio22_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio22_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO22 direction is input 
     * 1: GPIO22 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_22_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio23_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio23_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO23 direction is input 
     * 1: GPIO23 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_23_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_23_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio23_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio23_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO23 direction is input 
     * 1: GPIO23 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_23_DIRECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio24_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_3_REG);

    /* 
     * 0: GPIO24 direction is input 
     * 1: GPIO24 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_24_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_24_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_direction
 *
 * RET  : Return the value of register 0xGpioDirection3
 *
 * Notes : From register 0x1012, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio24_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO24 direction is input 
     * 1: GPIO24 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_3_REG) & GPIO_24_DIRECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio25_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO25 direction is input 
     * 1: GPIO25 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_25_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_25_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio25_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO25 direction is input 
     * 1: GPIO25 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_25_DIRECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio26_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio26_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO26 direction is input 
     * 1: GPIO26 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_26_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_26_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio26_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio26_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO26 direction is input 
     * 1: GPIO26 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_26_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio27_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio27_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO27 direction is input 
     * 1: GPIO27 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_27_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_27_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio27_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio27_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO27 direction is input 
     * 1: GPIO27 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_27_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio28_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio28_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO28 direction is input 
     * 1: GPIO28 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_28_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_28_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio28_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio28_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO28 direction is input 
     * 1: GPIO28 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_28_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio29_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio29_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO29 direction is input 
     * 1: GPIO29 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_29_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_29_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio29_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio29_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO29 direction is input 
     * 1: GPIO29 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_29_DIRECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio30_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio30_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO30 direction is input 
     * 1: GPIO30 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_30_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_30_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio30_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio30_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO30 direction is input 
     * 1: GPIO30 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_30_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio31_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio31_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO31 direction is input 
     * 1: GPIO31 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_31_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_31_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio31_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio31_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO31 direction is input 
     * 1: GPIO31 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_31_DIRECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio32_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio32_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_4_REG);

    /* 
     * 0: GPIO32 direction is input 
     * 1: GPIO32 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_32_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_32_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio32_direction
 *
 * RET  : Return the value of register 0xGpioDirection4
 *
 * Notes : From register 0x1013, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio32_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO32 direction is input 
     * 1: GPIO32 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_4_REG) & GPIO_32_DIRECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio34_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio34_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO34 direction is input 
     * 1: GPIO34 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_34_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_34_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio34_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio34_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO34 direction is input 
     * 1: GPIO34 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_34_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio35_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio35_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO35 direction is input 
     * 1: GPIO35 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_35_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_35_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio35_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio35_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO35 direction is input 
     * 1: GPIO35 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_35_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio36_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO36 direction is input 
     * 1: GPIO36 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_36_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_36_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio36_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO36 direction is input 
     * 1: GPIO36 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_36_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio37_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO37 direction is input 
     * 1: GPIO37 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_37_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_37_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio37_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO37 direction is input 
     * 1: GPIO37 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_37_DIRECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio38_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO38 direction is input 
     * 1: GPIO38 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_38_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_38_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio38_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO38 direction is input 
     * 1: GPIO38 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_38_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio39_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO39 direction is input 
     * 1: GPIO39 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_39_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_39_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio39_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO39 direction is input 
     * 1: GPIO39 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_39_DIRECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio40_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_5_REG);

    /* 
     * 0: GPIO40 direction is input 
     * 1: GPIO40 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_40_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_40_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_direction
 *
 * RET  : Return the value of register 0xGpioDirection5
 *
 * Notes : From register 0x1014, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio40_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO40 direction is input 
     * 1: GPIO40 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_5_REG) & GPIO_40_DIRECTION_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection6
 *
 * Notes : From register 0x1015, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio41_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_6_REG);

    /* 
     * 0: GPIO41 direction is input 
     * 1: GPIO41 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_41_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_41_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_direction
 *
 * RET  : Return the value of register 0xGpioDirection6
 *
 * Notes : From register 0x1015, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio41_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO41 direction is input 
     * 1: GPIO41 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_6_REG) & GPIO_41_DIRECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio42_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection6
 *
 * Notes : From register 0x1015, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio42_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_6_REG);

    /* 
     * 0: GPIO42 direction is input 
     * 1: GPIO42 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_42_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_42_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio42_direction
 *
 * RET  : Return the value of register 0xGpioDirection6
 *
 * Notes : From register 0x1015, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio42_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO42 direction is input 
     * 1: GPIO42 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_6_REG) & GPIO_42_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio50_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio50_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_7_REG);

    /* 
     * 0: GPIO50 direction is input 
     * 1: GPIO50 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_50_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_50_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_direction
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO50 direction is input 
     * 1: GPIO50 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_7_REG) & GPIO_50_DIRECTION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio51_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio51_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_7_REG);

    /* 
     * 0: GPIO51 direction is input 
     * 1: GPIO51 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_51_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_51_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_direction
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO51 direction is input 
     * 1: GPIO51 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_7_REG) & GPIO_51_DIRECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio52_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio52_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_7_REG);

    /* 
     * 0: GPIO52 direction is input 
     * 1: GPIO52 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_52_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_52_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_direction
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO52 direction is input 
     * 1: GPIO52 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_7_REG) & GPIO_52_DIRECTION_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio53_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio53_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_7_REG);

    /* 
     * 0: GPIO53 direction is input 
     * 1: GPIO53 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_53_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_53_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_direction
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO53 direction is input 
     * 1: GPIO53 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_7_REG) & GPIO_53_DIRECTION_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio60_direction
 *
 * IN   : param, a value to write to the regiter GpioDirection7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio60_direction( enum gpio_direction param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_DIRECTION_7_REG);

    /* 
     * 0: GPIO60 direction is input 
     * 1: GPIO60 direction is output 
     */ 
    switch( param ){
        case GPIO_OUTPUT_E: 
           value = old_value | GPIO_60_DIRECTION_MASK; 
           break;
        case GPIO_INPUT_E: 
           value = old_value & ~ GPIO_60_DIRECTION_MASK;
           break;
    }
  

    I2CWrite(GPIO_DIRECTION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_direction
 *
 * RET  : Return the value of register 0xGpioDirection7
 *
 * Notes : From register 0x1016, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_direction()
  {
    unsigned char value;


    /* 
     * 0: GPIO60 direction is input 
     * 1: GPIO60 direction is output 
     */ 
    value = (I2CRead(GPIO_DIRECTION_7_REG) & GPIO_60_DIRECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio1_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio1_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO1 output egals '0' 
     * 1: GPIO1 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_1_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_1_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio1_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio1_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO1 output egals '0' 
     * 1: GPIO1 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_1_OUTPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio2_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio2_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO2 output egals '0' 
     * 1: GPIO2 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_2_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_2_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio2_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio2_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO2 output egals '0' 
     * 1: GPIO2 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_2_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio3_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio3_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO3 output egals '0' 
     * 1: GPIO3 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_3_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_3_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio3_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio3_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO3 output egals '0' 
     * 1: GPIO3 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_3_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio4_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio4_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO4 output egals '0' 
     * 1: GPIO4 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_4_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_4_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio4_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio4_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO4 output egals '0' 
     * 1: GPIO4 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_4_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio6_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO6 output egals '0' 
     * 1: GPIO6 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_6_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_6_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio6_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO6 output egals '0' 
     * 1: GPIO6 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_6_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio7_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO7 output egals '0' 
     * 1: GPIO7 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_7_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_7_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio7_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO7 output egals '0' 
     * 1: GPIO7 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_7_OUTPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio8_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_1_REG);

    /* 
     * 0: GPIO8 output egals '0' 
     * 1: GPIO8 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_8_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_8_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_output
 *
 * RET  : Return the value of register 0xGpioOutputControl1
 *
 * Notes : From register 0x1020, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio8_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO8 output egals '0' 
     * 1: GPIO8 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_1_REG) & GPIO_8_OUTPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio9_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO9 output egals '0' 
     * 1: GPIO9 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_9_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_9_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio9_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO9 output egals '0' 
     * 1: GPIO9 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_9_OUTPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio10_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO10 output egals '0' 
     * 1: GPIO10 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_10_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_10_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio10_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO10 output egals '0' 
     * 1: GPIO10 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_10_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio11_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO11 output egals '0' 
     * 1: GPIO11 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_11_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_11_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio11_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO11 output egals '0' 
     * 1: GPIO11 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_11_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio12_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO12 output egals '0' 
     * 1: GPIO12 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_12_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_12_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio12_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO12 output egals '0' 
     * 1: GPIO12 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_12_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio13_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO13 output egals '0' 
     * 1: GPIO13 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_13_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_13_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio13_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO13 output egals '0' 
     * 1: GPIO13 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_13_OUTPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio14_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio14_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO14 output egals '0' 
     * 1: GPIO14 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_14_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_14_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio14_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio14_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO14 output egals '0' 
     * 1: GPIO14 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_14_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio15_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio15_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO15 output egals '0' 
     * 1: GPIO15 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_15_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_15_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio15_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio15_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO15 output egals '0' 
     * 1: GPIO15 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_15_OUTPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio16_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio16_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_2_REG);

    /* 
     * 0: GPIO16 output egals '0' 
     * 1: GPIO16 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_16_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_16_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio16_output
 *
 * RET  : Return the value of register 0xGpioOutputControl2
 *
 * Notes : From register 0x1021, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio16_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO16 output egals '0' 
     * 1: GPIO16 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_2_REG) & GPIO_16_OUTPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio17_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio17_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO17 output egals '0' 
     * 1: GPIO17 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_17_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_17_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio17_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio17_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO17 output egals '0' 
     * 1: GPIO17 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_17_OUTPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio18_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio18_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO18 output egals '0' 
     * 1: GPIO18 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_18_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_18_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio18_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio18_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO18 output egals '0' 
     * 1: GPIO18 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_18_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio19_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio19_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO19 output egals '0' 
     * 1: GPIO19 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_19_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_19_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio19_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio19_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO19 output egals '0' 
     * 1: GPIO19 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_19_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio20_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio20_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO20 output egals '0' 
     * 1: GPIO20 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_20_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_20_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio20_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio20_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO20 output egals '0' 
     * 1: GPIO20 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_20_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio21_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio21_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO21 output egals '0' 
     * 1: GPIO21 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_21_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_21_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio21_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio21_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO21 output egals '0' 
     * 1: GPIO21 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_21_OUTPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio22_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio22_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO22 output egals '0' 
     * 1: GPIO22 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_22_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_22_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio22_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio22_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO22 output egals '0' 
     * 1: GPIO22 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_22_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio23_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio23_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO23 output egals '0' 
     * 1: GPIO23 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_23_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_23_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio23_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio23_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO23 output egals '0' 
     * 1: GPIO23 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_23_OUTPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio24_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_3_REG);

    /* 
     * 0: GPIO24 output egals '0' 
     * 1: GPIO24 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_24_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_24_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_output
 *
 * RET  : Return the value of register 0xGpioOutputControl3
 *
 * Notes : From register 0x1022, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio24_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO24 output egals '0' 
     * 1: GPIO24 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_3_REG) & GPIO_24_OUTPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio25_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO25 output egals '0' 
     * 1: GPIO25 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_25_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_25_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio25_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO25 output egals '0' 
     * 1: GPIO25 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_25_OUTPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio26_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio26_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO26 output egals '0' 
     * 1: GPIO26 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_26_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_26_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio26_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio26_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO26 output egals '0' 
     * 1: GPIO26 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_26_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio27_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio27_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO27 output egals '0' 
     * 1: GPIO27 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_27_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_27_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio27_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio27_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO27 output egals '0' 
     * 1: GPIO27 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_27_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio28_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio28_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO28 output egals '0' 
     * 1: GPIO28 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_28_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_28_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio28_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio28_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO28 output egals '0' 
     * 1: GPIO28 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_28_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio29_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio29_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO29 output egals '0' 
     * 1: GPIO29 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_29_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_29_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio29_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio29_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO29 output egals '0' 
     * 1: GPIO29 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_29_OUTPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio30_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio30_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO30 output egals '0' 
     * 1: GPIO30 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_30_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_30_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio30_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio30_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO30 output egals '0' 
     * 1: GPIO30 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_30_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio31_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio31_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO31 output egals '0' 
     * 1: GPIO31 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_31_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_31_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio31_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio31_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO31 output egals '0' 
     * 1: GPIO31 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_31_OUTPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio32_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio32_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_4_REG);

    /* 
     * 0: GPIO32 output egals '0' 
     * 1: GPIO32 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_32_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_32_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio32_output
 *
 * RET  : Return the value of register 0xGpioOutputControl4
 *
 * Notes : From register 0x1023, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio32_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO32 output egals '0' 
     * 1: GPIO32 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_4_REG) & GPIO_32_OUTPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio34_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio34_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO34 output egals '0' 
     * 1: GPIO34 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_34_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_34_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio34_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio34_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO34 output egals '0' 
     * 1: GPIO34 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_34_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio35_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio35_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO35 output egals '0' 
     * 1: GPIO35 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_35_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_35_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio35_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio35_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO35 output egals '0' 
     * 1: GPIO35 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_35_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio36_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO36 output egals '0' 
     * 1: GPIO36 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_36_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_36_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio36_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO36 output egals '0' 
     * 1: GPIO36 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_36_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio37_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO37 output egals '0' 
     * 1: GPIO37 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_37_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_37_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio37_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO37 output egals '0' 
     * 1: GPIO37 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_37_OUTPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio38_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO38 output egals '0' 
     * 1: GPIO38 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_38_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_38_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio38_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO38 output egals '0' 
     * 1: GPIO38 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_38_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio39_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO39 output egals '0' 
     * 1: GPIO39 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_39_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_39_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio39_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO39 output egals '0' 
     * 1: GPIO39 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_39_OUTPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio40_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_5_REG);

    /* 
     * 0: GPIO40 output egals '0' 
     * 1: GPIO40 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_40_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_40_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_output
 *
 * RET  : Return the value of register 0xGpioOutputControl5
 *
 * Notes : From register 0x1024, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio40_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO40 output egals '0' 
     * 1: GPIO40 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_5_REG) & GPIO_40_OUTPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio41_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_6_REG);

    /* 
     * 0: GPIO41 output egals '0' 
     * 1: GPIO41 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_41_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_41_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_output
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio41_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO41 output egals '0' 
     * 1: GPIO41 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_6_REG) & GPIO_41_OUTPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio42_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio42_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_6_REG);

    /* 
     * 0: GPIO42 output egals '0' 
     * 1: GPIO42 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_42_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_42_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio42_output
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio42_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO42 output egals '0' 
     * 1: GPIO42 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_6_REG) & GPIO_42_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_xtal32_mode
 *
 * IN   : param, a value to write to the regiter GpioOutputControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 7:5
 *
 **************************************************************************/
unsigned char set_xtal32_mode( enum xtal32_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_6_REG);

    /* 
     * [00]: LP mode 
     * [01]: LP mode 
     * [10]: MP mode 
     * [11]: HP mode 
     */ 
  

     value =  old_value & ~XTAL_32_MODE_MASK ;

    switch(  param ){

           case LP_MODE_E:
                value  = value  | (LP_MODE << 0x5);
                break;
           case LP_MODE2_E:
                value  = value  | (LP_MODE2 << 0x5);
                break;
           case MP_MODE_E:
                value  = value  | (MP_MODE << 0x5);
                break;
           case HP_MODE_E:
                value  = value  | (HP_MODE << 0x5);
                break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_xtal32_mode
 *
 * RET  : Return the value of register 0xGpioOutputControl6
 *
 * Notes : From register 0x1025, bits 7:5
 *
 **************************************************************************/
unsigned char get_xtal32_mode()
  {
    unsigned char value;


    /* 
     * [00]: LP mode 
     * [01]: LP mode 
     * [10]: MP mode 
     * [11]: HP mode 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_6_REG) & XTAL_32_MODE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio50_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio50_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_7_REG);

    /* 
     * 0: GPIO50 output egals '0' 
     * 1: GPIO50 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_50_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_50_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_output
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO50 output egals '0' 
     * 1: GPIO50 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_7_REG) & GPIO_50_OUTPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio51_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio51_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_7_REG);

    /* 
     * 0: GPIO51 output egals '0' 
     * 1: GPIO51 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_51_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_51_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_output
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO51 output egals '0' 
     * 1: GPIO51 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_7_REG) & GPIO_51_OUTPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio52_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio52_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_7_REG);

    /* 
     * 0: GPIO52 output egals '0' 
     * 1: GPIO52 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_52_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_52_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_output
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO52 output egals '0' 
     * 1: GPIO52 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_7_REG) & GPIO_52_OUTPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio53_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio53_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_7_REG);

    /* 
     * 0: GPIO53 output egals '0' 
     * 1: GPIO53 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_53_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_53_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_output
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO53 output egals '0' 
     * 1: GPIO53 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_7_REG) & GPIO_53_OUTPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio60_output
 *
 * IN   : param, a value to write to the regiter GpioOutputControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio60_output( enum gpio_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_OUTPUT_CONTROL_7_REG);

    /* 
     * 0: GPIO60 output egals '0' 
     * 1: GPIO60 output egals '1' 
     */ 
    switch( param ){
        case GPIO_SET_1_E: 
           value = old_value | GPIO_60_OUTPUT_MASK; 
           break;
        case GPIO_SET_0_E: 
           value = old_value & ~ GPIO_60_OUTPUT_MASK;
           break;
    }
  

    I2CWrite(GPIO_OUTPUT_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_output
 *
 * RET  : Return the value of register 0xGpioOutputControl7
 *
 * Notes : From register 0x1026, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_output()
  {
    unsigned char value;


    /* 
     * 0: GPIO60 output egals '0' 
     * 1: GPIO60 output egals '1' 
     */ 
    value = (I2CRead(GPIO_OUTPUT_CONTROL_7_REG) & GPIO_60_OUTPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio1_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio1_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO1 pull down is enabled 
     * 1: GPIO1 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_1_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_1_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio1_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio1_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO1 pull down is enabled 
     * 1: GPIO1 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_1_PULLDOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio2_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio2_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO2 pull down is enabled 
     * 1: GPIO2 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_2_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_2_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio2_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio2_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO2 pull down is enabled 
     * 1: GPIO2 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_2_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio3_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio3_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO3 pull down is enabled 
     * 1: GPIO3 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_3_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_3_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio3_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio3_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO3 pull down is enabled 
     * 1: GPIO3 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_3_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio4_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio4_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO4 pull down is enabled 
     * 1: GPIO4 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_4_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_4_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio4_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio4_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO4 pull down is enabled 
     * 1: GPIO4 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_4_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio6_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio6_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO6 pull down is enabled 
     * 1: GPIO6 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_6_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_6_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio6_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO6 pull down is enabled 
     * 1: GPIO6 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_6_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio7_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio7_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO7 pull down is enabled 
     * 1: GPIO7 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_7_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_7_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio7_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO7 pull down is enabled 
     * 1: GPIO7 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_7_PULLDOWN_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio8_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio8_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_1_REG);

    /* 
     * 0: GPIO8 pull down is enabled 
     * 1: GPIO8 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_8_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_8_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl1
 *
 * Notes : From register 0x1030, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio8_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO8 pull down is enabled 
     * 1: GPIO8 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_1_REG) & GPIO_8_PULLDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio9_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio9_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO9 pull down is enabled 
     * 1: GPIO9 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_9_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_9_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio9_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO9 pull down is enabled 
     * 1: GPIO9 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_9_PULLDOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio10_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO10 pull down is enabled 
     * 1: GPIO10 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_10_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_10_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio10_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO10 pull down is enabled 
     * 1: GPIO10 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_10_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio11_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO11 pull down is enabled 
     * 1: GPIO11 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_11_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_11_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio11_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO11 pull down is enabled 
     * 1: GPIO11 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_11_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio12_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO12 pull down is enabled 
     * 1: GPIO12 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_12_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_12_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio12_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO12 pull down is enabled 
     * 1: GPIO12 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_12_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio13_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO13 pull down is enabled 
     * 1: GPIO13 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_13_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_13_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio13_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO13 pull down is enabled 
     * 1: GPIO13 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_13_PULLDOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio14_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio14_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO14 pull down is enabled 
     * 1: GPIO14 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_14_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_14_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio14_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio14_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO14 pull down is enabled 
     * 1: GPIO14 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_14_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio15_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio15_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO15 pull down is enabled 
     * 1: GPIO15 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_15_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_15_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio15_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio15_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO15 pull down is enabled 
     * 1: GPIO15 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_15_PULLDOWN_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio16_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio16_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_2_REG);

    /* 
     * 0: GPIO16 pull down is enabled 
     * 1: GPIO16 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_16_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_16_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio16_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl2
 *
 * Notes : From register 0x1031, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio16_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO16 pull down is enabled 
     * 1: GPIO16 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_2_REG) & GPIO_16_PULLDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio17_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio17_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO17 pull down is enabled 
     * 1: GPIO17 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_17_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_17_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio17_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio17_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO17 pull down is enabled 
     * 1: GPIO17 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_17_PULLDOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio18_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio18_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO18 pull down is enabled 
     * 1: GPIO18 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_18_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_18_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio18_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio18_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO18 pull down is enabled 
     * 1: GPIO18 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_18_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio19_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio19_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO19 pull down is enabled 
     * 1: GPIO19 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_19_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_19_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio19_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio19_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO19 pull down is enabled 
     * 1: GPIO19 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_19_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio20_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio20_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO20 pull down is enabled 
     * 1: GPIO20 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_20_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_20_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio20_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio20_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO20 pull down is enabled 
     * 1: GPIO20 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_20_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio21_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio21_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO21 pull down is enabled 
     * 1: GPIO21 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_21_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_21_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio21_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio21_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO21 pull down is enabled 
     * 1: GPIO21 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_21_PULLDOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio22_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio22_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO22 pull down is enabled 
     * 1: GPIO22 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_22_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_22_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio22_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio22_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO22 pull down is enabled 
     * 1: GPIO22 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_22_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio23_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio23_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO23 pull down is enabled 
     * 1: GPIO23 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_23_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_23_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio23_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio23_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO23 pull down is enabled 
     * 1: GPIO23 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_23_PULLDOWN_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio24_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio24_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_3_REG);

    /* 
     * 0: GPIO24 pull down is enabled 
     * 1: GPIO24 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_24_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_24_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl3
 *
 * Notes : From register 0x1032, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio24_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO24 pull down is enabled 
     * 1: GPIO24 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_3_REG) & GPIO_24_PULLDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio25_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio25_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO25 pull down is enabled 
     * 1: GPIO25 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_25_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_25_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio25_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO25 pull down is enabled 
     * 1: GPIO25 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_25_PULLDOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio26_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio26_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO26 pull down is enabled 
     * 1: GPIO26 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_26_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_26_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio26_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio26_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO26 pull down is enabled 
     * 1: GPIO26 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_26_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio27_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio27_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO27 pull down is enabled 
     * 1: GPIO27 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_27_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_27_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio27_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio27_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO27 pull down is enabled 
     * 1: GPIO27 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_27_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio28_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio28_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO28 pull down is enabled 
     * 1: GPIO28 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_28_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_28_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio28_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio28_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO28 pull down is enabled 
     * 1: GPIO28 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_28_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio29_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio29_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO29 pull down is enabled 
     * 1: GPIO29 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_29_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_29_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio29_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio29_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO29 pull down is enabled 
     * 1: GPIO29 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_29_PULLDOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio30_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio30_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO30 pull down is enabled 
     * 1: GPIO30 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_30_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_30_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio30_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio30_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO30 pull down is enabled 
     * 1: GPIO30 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_30_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio31_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio31_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO31 pull down is enabled 
     * 1: GPIO31 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_31_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_31_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio31_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio31_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO31 pull down is enabled 
     * 1: GPIO31 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_31_PULLDOWN_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio32_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio32_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_4_REG);

    /* 
     * 0: GPIO32 pull down is enabled 
     * 1: GPIO32 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_32_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_32_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio32_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl4
 *
 * Notes : From register 0x1033, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio32_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO32 pull down is enabled 
     * 1: GPIO32 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_4_REG) & GPIO_32_PULLDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio34_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio34_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO34 pull down is enabled 
     * 1: GPIO34 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_34_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_34_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio34_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio34_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO34 pull down is enabled 
     * 1: GPIO34 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_34_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio35_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio35_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO35 pull down is enabled 
     * 1: GPIO35 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_35_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_35_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio35_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio35_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO35 pull down is enabled 
     * 1: GPIO35 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_35_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio36_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio36_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO36 pull down is enabled 
     * 1: GPIO36 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_36_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_36_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio36_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO36 pull down is enabled 
     * 1: GPIO36 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_36_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio37_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio37_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO37 pull down is enabled 
     * 1: GPIO37 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_37_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_37_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio37_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO37 pull down is enabled 
     * 1: GPIO37 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_37_PULLDOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio38_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio38_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO38 pull down is enabled 
     * 1: GPIO38 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_38_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_38_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio38_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO38 pull down is enabled 
     * 1: GPIO38 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_38_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio39_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 6:6
 *
 **************************************************************************/
unsigned char set_gpio39_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO39 pull down is enabled 
     * 1: GPIO39 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_39_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_39_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio39_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO39 pull down is enabled 
     * 1: GPIO39 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_39_PULLDOWN_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio40_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl5
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 7:7
 *
 **************************************************************************/
unsigned char set_gpio40_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_5_REG);

    /* 
     * 0: GPIO40 pull down is enabled 
     * 1: GPIO40 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_40_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_40_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl5
 *
 * Notes : From register 0x1034, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio40_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO40 pull down is enabled 
     * 1: GPIO40 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_5_REG) & GPIO_40_PULLDOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio41_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio41_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_6_REG);

    /* 
     * 0: GPIO41 pull down is enabled 
     * 1: GPIO41 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_41_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_41_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio41_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO41 pull down is enabled 
     * 1: GPIO41 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_6_REG) & GPIO_41_PULLDOWN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio42_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio42_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_6_REG);

    /* 
     * 0: GPIO42 pull down is enabled 
     * 1: GPIO42 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_42_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_42_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio42_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio42_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO42 pull down is enabled 
     * 1: GPIO42 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_6_REG) & GPIO_42_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_batt_ok_sel_pullup
 *
 * IN   : param, a value to write to the regiter GpioPullControl6
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 7:7
 *
 **************************************************************************/
unsigned char set_batt_ok_sel_pullup( enum batt_ok_sel_pullup_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_6_REG);

    /* 
     * 0: pullup is enabled 
     * 1: pullup is disabled 
     */ 
    switch( param ){
        case BATT_OK_SEL_PULLUP_ENABLE_E: 
           value = old_value | BATT_OK_SEL_PULLUP_MASK; 
           break;
        case BATT_OK_SEL_PULLUP_DISABLE_E: 
           value = old_value & ~ BATT_OK_SEL_PULLUP_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_batt_ok_sel_pullup
 *
 * RET  : Return the value of register 0xGpioPullControl6
 *
 * Notes : From register 0x1035, bits 7:7
 *
 **************************************************************************/
unsigned char get_batt_ok_sel_pullup()
  {
    unsigned char value;


    /* 
     * 0: pullup is enabled 
     * 1: pullup is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_6_REG) & BATT_OK_SEL_PULLUP_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio50_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio50_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_7_REG);

    /* 
     * 0: GPIO50 pull down is enabled 
     * 1: GPIO50 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_50_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_50_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO50 pull down is enabled 
     * 1: GPIO50 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_7_REG) & GPIO_50_PULLDOWN_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio51_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio51_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_7_REG);

    /* 
     * 0: GPIO51 pull down is enabled 
     * 1: GPIO51 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_51_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_51_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO51 pull down is enabled 
     * 1: GPIO51 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_7_REG) & GPIO_51_PULLDOWN_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio52_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 3:3
 *
 **************************************************************************/
unsigned char set_gpio52_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_7_REG);

    /* 
     * 0: GPIO52 pull down is enabled 
     * 1: GPIO52 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_52_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_52_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO52 pull down is enabled 
     * 1: GPIO52 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_7_REG) & GPIO_52_PULLDOWN_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio53_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 4:4
 *
 **************************************************************************/
unsigned char set_gpio53_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_7_REG);

    /* 
     * 0: GPIO53 pull down is enabled 
     * 1: GPIO53 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_53_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_53_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO53 pull down is enabled 
     * 1: GPIO53 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_7_REG) & GPIO_53_PULLDOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio60_pulldown
 *
 * IN   : param, a value to write to the regiter GpioPullControl7
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 5:5
 *
 **************************************************************************/
unsigned char set_gpio60_pulldown( enum gpio_pulldown param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_PULL_CONTROL_7_REG);

    /* 
     * 0: GPIO60 pull down is enabled 
     * 1: GPIO60 pull down is disabled 
     */ 
    switch( param ){
        case GPIO_PULLDOWN_DISABLE_E: 
           value = old_value | GPIO_60_PULLDOWN_MASK; 
           break;
        case GPIO_PULLDOWN_ENABLE_E: 
           value = old_value & ~ GPIO_60_PULLDOWN_MASK;
           break;
    }
  

    I2CWrite(GPIO_PULL_CONTROL_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_pulldown
 *
 * RET  : Return the value of register 0xGpioPullControl7
 *
 * Notes : From register 0x1036, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_pulldown()
  {
    unsigned char value;


    /* 
     * 0: GPIO60 pull down is enabled 
     * 1: GPIO60 pull down is disabled 
     */ 
    value = (I2CRead(GPIO_PULL_CONTROL_7_REG) & GPIO_60_PULLDOWN_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio1_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio1_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO1 input egals '0' 
     * 1: GPIO1 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_1_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio2_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio2_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO2 input egals '0' 
     * 1: GPIO2 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_2_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio3_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio3_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO3 input egals '0' 
     * 1: GPIO3 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_3_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio4_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio4_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO4 input egals '0' 
     * 1: GPIO4 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_4_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio6_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio6_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO6 input egals '0' 
     * 1: GPIO6 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_6_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio7_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio7_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO7 input egals '0' 
     * 1: GPIO7 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_7_INPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio8_input
 *
 * RET  : Return the value of register 0xGpioInputControl1
 *
 * Notes : From register 0x1040, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio8_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO8 input egals '0' 
     * 1: GPIO8 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_1_REG) & GPIO_8_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio9_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio9_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO9 input egals '0' 
     * 1: GPIO9 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_9_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio10_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO10 input egals '0' 
     * 1: GPIO10 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_10_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio11_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO11 input egals '0' 
     * 1: GPIO11 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_11_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio12_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO12 input egals '0' 
     * 1: GPIO12 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_12_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio13_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO13 input egals '0' 
     * 1: GPIO13 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_13_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio14_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio14_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO14 input egals '0' 
     * 1: GPIO14 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_14_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio15_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio15_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO15 input egals '0' 
     * 1: GPIO15 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_15_INPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio16_input
 *
 * RET  : Return the value of register 0xGpioInputControl2
 *
 * Notes : From register 0x1041, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio16_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO16 input egals '0' 
     * 1: GPIO16 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_2_REG) & GPIO_16_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio17_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio17_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO17 input egals '0' 
     * 1: GPIO17 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_17_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio18_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio18_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO18 input egals '0' 
     * 1: GPIO18 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_18_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio19_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio19_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO19 input egals '0' 
     * 1: GPIO19 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_19_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio20_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio20_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO20 input egals '0' 
     * 1: GPIO20 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_20_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio21_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio21_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO21 input egals '0' 
     * 1: GPIO21 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_21_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio22_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio22_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO22 input egals '0' 
     * 1: GPIO22 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_22_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio23_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio23_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO23 input egals '0' 
     * 1: GPIO23 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_23_INPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio24_input
 *
 * RET  : Return the value of register 0xGpioInputControl3
 *
 * Notes : From register 0x1042, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio24_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO24 input egals '0' 
     * 1: GPIO24 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_3_REG) & GPIO_24_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio25_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio25_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO25 input egals '0' 
     * 1: GPIO25 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_25_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio26_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio26_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO26 input egals '0' 
     * 1: GPIO26 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_26_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio27_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio27_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO27 input egals '0' 
     * 1: GPIO27 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_27_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio28_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio28_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO28 input egals '0' 
     * 1: GPIO28 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_28_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio29_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio29_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO29 input egals '0' 
     * 1: GPIO29 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_29_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio30_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio30_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO30 input egals '0' 
     * 1: GPIO30 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_30_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio31_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio31_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO31 input egals '0' 
     * 1: GPIO31 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_31_INPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio32_input
 *
 * RET  : Return the value of register 0xGpioInputControl4
 *
 * Notes : From register 0x1043, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio32_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO32 input egals '0' 
     * 1: GPIO32 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_4_REG) & GPIO_32_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio33_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio33_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO33 input egals '0' 
     * 1: GPIO33 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_33_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio34_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio34_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO34 input egals '0' 
     * 1: GPIO34 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_34_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio35_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio35_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO35 input egals '0' 
     * 1: GPIO35 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_35_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio36_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio36_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO36 input egals '0' 
     * 1: GPIO36 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_36_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio37_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio37_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO37 input egals '0' 
     * 1: GPIO37 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_37_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio38_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio38_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO38 input egals '0' 
     * 1: GPIO38 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_38_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio39_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 6:6
 *
 **************************************************************************/
unsigned char get_gpio39_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO39 input egals '0' 
     * 1: GPIO39 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_39_INPUT_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio40_input
 *
 * RET  : Return the value of register 0xGpioInputControl5
 *
 * Notes : From register 0x1044, bits 7:7
 *
 **************************************************************************/
unsigned char get_gpio40_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO40 input egals '0' 
     * 1: GPIO40 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_5_REG) & GPIO_40_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio41_input
 *
 * RET  : Return the value of register 0xGpioInputControl6
 *
 * Notes : From register 0x1045, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio41_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO41 input egals '0' 
     * 1: GPIO41 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_6_REG) & GPIO_41_INPUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio42_input
 *
 * RET  : Return the value of register 0xGpioInputControl6
 *
 * Notes : From register 0x1045, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio42_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO42 input egals '0' 
     * 1: GPIO42 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_6_REG) & GPIO_42_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio50_input
 *
 * RET  : Return the value of register 0xGpioInputControl7
 *
 * Notes : From register 0x1046, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio50_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO50 input egals '0' 
     * 1: GPIO50 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_7_REG) & GPIO_50_INPUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio51_input
 *
 * RET  : Return the value of register 0xGpioInputControl7
 *
 * Notes : From register 0x1046, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio51_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO51 input egals '0' 
     * 1: GPIO51 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_7_REG) & GPIO_51_INPUT_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio52_input
 *
 * RET  : Return the value of register 0xGpioInputControl7
 *
 * Notes : From register 0x1046, bits 3:3
 *
 **************************************************************************/
unsigned char get_gpio52_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO52 input egals '0' 
     * 1: GPIO52 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_7_REG) & GPIO_52_INPUT_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio53_input
 *
 * RET  : Return the value of register 0xGpioInputControl7
 *
 * Notes : From register 0x1046, bits 4:4
 *
 **************************************************************************/
unsigned char get_gpio53_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO53 input egals '0' 
     * 1: GPIO53 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_7_REG) & GPIO_53_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_gpio60_input
 *
 * RET  : Return the value of register 0xGpioInputControl7
 *
 * Notes : From register 0x1046, bits 5:5
 *
 **************************************************************************/
unsigned char get_gpio60_input()
  {
    unsigned char value;


    /* 
     * 0: GPIO60 input egals '0' 
     * 1: GPIO60 input egals '1' 
     */ 
    value = (I2CRead(GPIO_INPUT_CONTROL_7_REG) & GPIO_60_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio10_alternate
 *
 * IN   : param, a value to write to the regiter GpioAlternate1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 0:0
 *
 **************************************************************************/
unsigned char set_gpio10_alternate( enum gpio10_alternate param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_ALTERNATE_1_REG);

    /* 
     * 0: select YcbCr4 function on pad Gpio10 
     * 1: select HiqClkEna function on pad Gpio10 
     */ 
    switch( param ){
        case HIQ_CLOKK_ENABLE_E: 
           value = old_value | GPIO_10_ALTERNATE_MASK; 
           break;
        case YCBCR4_E: 
           value = old_value & ~ GPIO_10_ALTERNATE_MASK;
           break;
    }
  

    I2CWrite(GPIO_ALTERNATE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio10_alternate
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 0:0
 *
 **************************************************************************/
unsigned char get_gpio10_alternate()
  {
    unsigned char value;


    /* 
     * 0: select YcbCr4 function on pad Gpio10 
     * 1: select HiqClkEna function on pad Gpio10 
     */ 
    value = (I2CRead(GPIO_ALTERNATE_1_REG) & GPIO_10_ALTERNATE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio11_alternate
 *
 * IN   : param, a value to write to the regiter GpioAlternate1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 1:1
 *
 **************************************************************************/
unsigned char set_gpio11_alternate( enum gpio11_alternate param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_ALTERNATE_1_REG);

    /* 
     * 0: select YcbCr5 function on pad Gpio11 
     * 1: select UsbUiccPd function on pad Gpio11 
     */ 
    switch( param ){
        case USBUICCPD_E: 
           value = old_value | GPIO_11_ALTERNATE_MASK; 
           break;
        case YCBCR5_E: 
           value = old_value & ~ GPIO_11_ALTERNATE_MASK;
           break;
    }
  

    I2CWrite(GPIO_ALTERNATE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio11_alternate
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 1:1
 *
 **************************************************************************/
unsigned char get_gpio11_alternate()
  {
    unsigned char value;


    /* 
     * 0: select YcbCr5 function on pad Gpio11 
     * 1: select UsbUiccPd function on pad Gpio11 
     */ 
    value = (I2CRead(GPIO_ALTERNATE_1_REG) & GPIO_11_ALTERNATE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio12_alternate
 *
 * IN   : param, a value to write to the regiter GpioAlternate1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 2:2
 *
 **************************************************************************/
unsigned char set_gpio12_alternate( enum gpio12_alternate param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_ALTERNATE_1_REG);

    /* 
     * 0: select YcbCr6 function on pad Gpio12 
     * 1: select I2cTrig1 function on pad Gpio12 
     */ 
    switch( param ){
        case I2C_TRIG1_E: 
           value = old_value | GPIO_12_ALTERNATE_MASK; 
           break;
        case YCBCR6_E: 
           value = old_value & ~ GPIO_12_ALTERNATE_MASK;
           break;
    }
  

    I2CWrite(GPIO_ALTERNATE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio12_alternate
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 2:2
 *
 **************************************************************************/
unsigned char get_gpio12_alternate()
  {
    unsigned char value;


    /* 
     * 0: select YcbCr6 function on pad Gpio12 
     * 1: select I2cTrig1 function on pad Gpio12 
     */ 
    value = (I2CRead(GPIO_ALTERNATE_1_REG) & GPIO_12_ALTERNATE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gpio13_alternate
 *
 * IN   : param, a value to write to the regiter GpioAlternate1
 * OUT  : 
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 4:3
 *
 **************************************************************************/
unsigned char set_gpio13_alternate( enum gpio13_alternate param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GPIO_ALTERNATE_1_REG);

    /* 
     * 00: select YcbCr7 function on pad Gpio13 
     * 01: select I2cTrig2 function on pad Gpio13 
     * 10 or 11: select USBVdat on pad Gpio13 (0: indicates 
     * 100mA, 1: indicates 500mA) 
     */ 
  

     value =  old_value & ~GPIO_13_ALTERNATE_MASK ;

    switch(  param ){

           case YCBCR7_E:
                value  = value  | (YCBCR7 << 0x3);
                break;
           case I2C_TRIG2_E:
                value  = value  | (I2C_TRIG2 << 0x3);
                break;
           case USB_VDAT_100_E:
                value  = value  | (USB_VDAT_100 << 0x3);
                break;
           case USB_VDAT_500_E:
                value  = value  | (USB_VDAT_500 << 0x3);
                break;
    }
  

    I2CWrite(GPIO_ALTERNATE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gpio13_alternate
 *
 * RET  : Return the value of register 0xGpioAlternate1
 *
 * Notes : From register 0x1050, bits 4:3
 *
 **************************************************************************/
unsigned char get_gpio13_alternate()
  {
    unsigned char value;


    /* 
     * 00: select YcbCr7 function on pad Gpio13 
     * 01: select I2cTrig2 function on pad Gpio13 
     * 10 or 11: select USBVdat on pad Gpio13 (0: indicates 
     * 100mA, 1: indicates 500mA) 
     */ 
    value = (I2CRead(GPIO_ALTERNATE_1_REG) & GPIO_13_ALTERNATE_MASK) >> 3;
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
