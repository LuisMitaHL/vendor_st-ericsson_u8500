/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Miscellanous/Linux/ab9540_Miscellanous_types.h
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_MISCELLANOUS_LINUX_AB9540_MISCELLANOUS_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_MISCELLANOUS_LINUX_AB9540_MISCELLANOUS_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 0:0, typedef modem_i2c_pullup_enable
//   
//  
#define MODEM_I_2C_CLOCK_PULLUP_MASK 0x1U
#define MODEM_I_2C_CLOCK_PULLUP_OFFSET 0
#define MODEM_I_2C_CLOCK_PULLUP_STOP_BIT 0
#define MODEM_I_2C_CLOCK_PULLUP_START_BIT 0
#define MODEM_I_2C_CLOCK_PULLUP_WIDTH 1

#define I_2C_PAD_CONTROL_REG 0x1067

typedef enum modem_i2c_pullup_enable {
    MODEM_I_2C_PULLUP_ENABLE_E,
    MODEM_I_2C_PULLUP_DISABLE_E
} MODEM_I_2C_PULLUP_ENABLE_T ;
#define MODEM_I_2C_CLOCK_PULLUP_PARAM_MASK  0x1



//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 1:1, typedef modem_i2c_data_pullup_enable
//   
//  
#define MODEM_I_2C_DATA_PULLUP_MASK 0x2U
#define MODEM_I_2C_DATA_PULLUP_OFFSET 1
#define MODEM_I_2C_DATA_PULLUP_STOP_BIT 1
#define MODEM_I_2C_DATA_PULLUP_START_BIT 1
#define MODEM_I_2C_DATA_PULLUP_WIDTH 1


typedef enum modem_i2c_data_pullup_enable {
    MODEM_I_2C_DATA_PULLUP_ENABLE_E,
    MODEM_I_2C_DATA_PULLUP_DISABLE_E
} MODEM_I_2C_DATA_PULLUP_ENABLE_T ;
#define MODEM_I_2C_DATA_PULLUP_PARAM_MASK  0x2



//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 2:2, typedef ape_i2c_clock_pullup_enable
//   
//  
#define APE_I_2C_CLOCK_PULLUP_MASK 0x4U
#define APE_I_2C_CLOCK_PULLUP_OFFSET 2
#define APE_I_2C_CLOCK_PULLUP_STOP_BIT 2
#define APE_I_2C_CLOCK_PULLUP_START_BIT 2
#define APE_I_2C_CLOCK_PULLUP_WIDTH 1


typedef enum ape_i2c_clock_pullup_enable {
    APE_I_2C_CLOCK_PULLUP_ENABLE_E,
    APE_I_2C_CLOCK_PULLUP_DISABLE_E
} APE_I_2C_CLOCK_PULLUP_ENABLE_T ;
#define APE_I_2C_CLOCK_PULLUP_PARAM_MASK  0x4



//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 3:3, typedef ape_i2c_data_pullup_enable
//   
//  
#define APE_I_2C_DATA_PULLUP_MASK 0x8U
#define APE_I_2C_DATA_PULLUP_OFFSET 3
#define APE_I_2C_DATA_PULLUP_STOP_BIT 3
#define APE_I_2C_DATA_PULLUP_START_BIT 3
#define APE_I_2C_DATA_PULLUP_WIDTH 1


typedef enum ape_i2c_data_pullup_enable {
    APE_I_2C_DATA_PULLUP_ENABLE_E,
    APE_I_2C_DATA_PULLUP_DISABLE_E
} APE_I_2C_DATA_PULLUP_ENABLE_T ;
#define APE_I_2C_DATA_PULLUP_PARAM_MASK  0x8



//  
//  Full mask set revision.
//  Register Stw4500Revision 0x1080, Bits 0:3, typedef metal_revision
//   
//  
#define METAL_FIX_REVISION_MASK 0xFU
#define METAL_FIX_REVISION_OFFSET 3
#define METAL_FIX_REVISION_STOP_BIT 3
#define METAL_FIX_REVISION_START_BIT 0
#define METAL_FIX_REVISION_WIDTH 4

#define STW_4500_REVISION_REG 0x1080



//  
//  Full mask set revision.
//  Register Stw4500Revision 0x1080, Bits 4:7, typedef full_mask_revision
//   
//  
#define FULL_MASK_SET_REVISION_MASK 0xF0U
#define FULL_MASK_SET_REVISION_OFFSET 7
#define FULL_MASK_SET_REVISION_STOP_BIT 7
#define FULL_MASK_SET_REVISION_START_BIT 4
#define FULL_MASK_SET_REVISION_WIDTH 4



#endif
