/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Miscellanous/Linux/ab8500_Miscellanous_types.h
 * 
 *
 * Generated on the 26/02/2010 09:06 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_MISCELLANOUS_LINUX_AB8500_MISCELLANOUS_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_MISCELLANOUS_LINUX_AB8500_MISCELLANOUS_TYPES_H




//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 3:3, typedef ape_i2c_data_pullup_enable
//   
//  
#define APE_I_2C_DATA_PULLUP_MASK 0x8
#define APE_I_2C_DATA_PULLUP_OFFSET 3
#define APE_I_2C_DATA_PULLUP_STOP_BIT 3
#define APE_I_2C_DATA_PULLUP_START_BIT 3
#define APE_I_2C_DATA_PULLUP_WIDTH 1

#define I_2C_PAD_CONTROL_REG 0x1067

typedef enum ape_i2c_data_pullup_enable {
    APE_I_2C_DATA_PULLUP_ENABLE_E,
    APE_I_2C_DATA_PULLUP_DISABLE_E
} APE_I_2C_DATA_PULLUP_ENABLE_T ;

#define APE_I_2C_DATA_PULLUP_ENABLE_PARAM_MASK  0x8

//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 2:2, typedef ape_i2c_clock_pullup_enable
//   
//  
#define APE_I_2C_CLOCK_PULLUP_MASK 0x4
#define APE_I_2C_CLOCK_PULLUP_OFFSET 2
#define APE_I_2C_CLOCK_PULLUP_STOP_BIT 2
#define APE_I_2C_CLOCK_PULLUP_START_BIT 2
#define APE_I_2C_CLOCK_PULLUP_WIDTH 1


typedef enum ape_i2c_clock_pullup_enable {
    APE_I_2C_CLOCK_PULLUP_ENABLE_E,
    APE_I_2C_CLOCK_PULLUP_DISABLE_E
} APE_I_2C_CLOCK_PULLUP_ENABLE_T ;

#define APE_I_2C_CLOCK_PULLUP_ENABLE_PARAM_MASK  0x4

//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 1:1, typedef modem_i2c_data_pullup_enable
//   
//  
#define MODEM_I_2C_DATA_PULLUP_MASK 0x2
#define MODEM_I_2C_DATA_PULLUP_OFFSET 1
#define MODEM_I_2C_DATA_PULLUP_STOP_BIT 1
#define MODEM_I_2C_DATA_PULLUP_START_BIT 1
#define MODEM_I_2C_DATA_PULLUP_WIDTH 1


typedef enum modem_i2c_data_pullup_enable {
    MODEM_I_2C_DATA_PULLUP_ENABLE_E,
    MODEM_I_2C_DATA_PULLUP_DISABLE_E
} MODEM_I_2C_DATA_PULLUP_ENABLE_T ;

#define MODEM_I_2C_DATA_PULLUP_ENABLE_PARAM_MASK  0x2

//  
//  0: enable internal pull-up 
//  1: Disable internal pull-up.
//  Register I2cPadControl 0x1067, Bits 0:0, typedef modem_i2c_pullup_enable
//   
//  
#define MODEM_I_2C_CLOCK_PULLUP_MASK 0x1
#define MODEM_I_2C_CLOCK_PULLUP_OFFSET 0
#define MODEM_I_2C_CLOCK_PULLUP_STOP_BIT 0
#define MODEM_I_2C_CLOCK_PULLUP_START_BIT 0
#define MODEM_I_2C_CLOCK_PULLUP_WIDTH 1


typedef enum modem_i2c_pullup_enable {
    MODEM_I_2C_PULLUP_ENABLE_E,
    MODEM_I_2C_PULLUP_DISABLE_E
} MODEM_I_2C_PULLUP_ENABLE_T ;

#define MODEM_I_2C_PULLUP_ENABLE_PARAM_MASK  0x1

//  
//  Full mask set revision.
//  Register stw4500Revision 0x1080, Bits 4:7, typedef full_mask_revision
//   
//  
#define FULL_MASK_SET_REVISION_MASK 0xF0
#define FULL_MASK_SET_REVISION_OFFSET 7
#define FULL_MASK_SET_REVISION_STOP_BIT 7
#define FULL_MASK_SET_REVISION_START_BIT 4
#define FULL_MASK_SET_REVISION_WIDTH 4

#define STW_4500_REVISION_REG 0x1080
#define FULL_MASK_REVISION_PARAM_MASK  0xF0

//  
//  Full mask set revision.
//  Register stw4500Revision 0x1080, Bits 0:3, typedef metal_revision
//   
//  
#define METAL_FIX_REVISION_MASK 0xF
#define METAL_FIX_REVISION_OFFSET 3
#define METAL_FIX_REVISION_STOP_BIT 3
#define METAL_FIX_REVISION_START_BIT 0
#define METAL_FIX_REVISION_WIDTH 4

#define METAL_REVISION_PARAM_MASK  0xF
#endif
