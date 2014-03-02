/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Regul/Linux/ab9540_Regul.c
 * 
 *
 * Generated on the 29/02/2012 09:14 by the 'super-cool' code generator 
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_REGUL_LINUX_AB9540_REGUL_C
#define C__LISP_PRODUCTION_AB9540_AB9540_REGUL_LINUX_AB9540_REGUL_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Regul.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_vmod_sel_ctrl_sel
 *
 * IN   : param, a value to write to the regiter ProtocolControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xProtocolControl
 *
 * Notes : From register 0x300, bits 4:4
 *
 **************************************************************************/
unsigned char set_vmod_sel_ctrl_sel( enum vmod_sel_ctrl_sel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PROTOCOL_CONTROL_REG);

    /* 
     * 0: allows 'VmodSelCtrl' to select Vmod output voltage 
     * 1:allows 'VmodSelCtrlI2cModem' to select Vmod output 
     * voltage 
     */ 
    switch( param ){
        case VMOD_SELECTED_BY_I2C_MODEM_E: 
           value = old_value | VMOD_SEL_CTRL_SEL_MASK; 
           break;
        case VMOD_SLECTED_BY_VMOD_CONTROL_E: 
           value = old_value & ~ VMOD_SEL_CTRL_SEL_MASK;
           break;
    }
  

    I2CWrite(PROTOCOL_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_sel_ctrl_sel
 *
 * RET  : Return the value of register 0xProtocolControl
 *
 * Notes : From register 0x300, bits 4:4
 *
 **************************************************************************/
unsigned char get_vmod_sel_ctrl_sel()
  {
    unsigned char value;


    /* 
     * 0: allows 'VmodSelCtrl' to select Vmod output voltage 
     * 1:allows 'VmodSelCtrlI2cModem' to select Vmod output 
     * voltage 
     */ 
    value = (I2CRead(PROTOCOL_CONTROL_REG) & VMOD_SEL_CTRL_SEL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_serial_control_interface
 *
 * IN   : param, a value to write to the regiter ProtocolControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xProtocolControl
 *
 * Notes : From register 0x300, bits 3:0
 *
 **************************************************************************/
unsigned char set_serial_control_interface( enum serial_control_by param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(PROTOCOL_CONTROL_REG);

    /* 
     * Serial Control Interface (I2C or SPI) 
     */ 
  

     value =  old_value & ~SERIAL_CONTROL_INTERFACE_MASK ;

    switch(  param ){

           case CONFIGURATION_1_E:
                value =  value | CONFIGURATION_1 ;
                break;
           case CONFIGURATION_2_E:
                value =  value | CONFIGURATION_2 ;
                break;
    }
  

    I2CWrite(PROTOCOL_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_serial_control_interface
 *
 * RET  : Return the value of register 0xProtocolControl
 *
 * Notes : From register 0x300, bits 3:0
 *
 **************************************************************************/
unsigned char get_serial_control_interface()
  {
    unsigned char value;


    /* 
     * Serial Control Interface (I2C or SPI) 
     */ 
    value = (I2CRead(PROTOCOL_CONTROL_REG) & SERIAL_CONTROL_INTERFACE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 1:0
 *
 **************************************************************************/
unsigned char set_varm_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Varm in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Varm in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Varm in HP whatever VarmRequest signal value 
     */ 
  

     value =  old_value & ~VARM_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value =  value | ALLOWS_HP_LP ;
                break;
           case ALLOWS_HP_OFF_E:
                value =  value | ALLOWS_HP_OFF ;
                break;
           case FORCE_HP_E:
                value =  value | FORCE_HP ;
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 1:0
 *
 **************************************************************************/
unsigned char get_varm_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Varm in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Varm in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Varm in HP whatever VarmRequest signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_REG) & VARM_REQUEST_CONTROL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 3:2
 *
 **************************************************************************/
unsigned char set_vape_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vape in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vape in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vape in HP whatever VapeRequest signal value 
     */ 
  

     value =  old_value & ~VAPE_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x2);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x2);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x2);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 3:2
 *
 **************************************************************************/
unsigned char get_vape_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vape in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vape in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vape in HP whatever VapeRequest signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_REG) & VAPE_REQUEST_CONTROL_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 5:4
 *
 **************************************************************************/
unsigned char set_vsmps1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vsmps1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps1 in HP whatever Vsmps1Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_1_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x4);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x4);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x4);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 5:4
 *
 **************************************************************************/
unsigned char get_vsmps1_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vsmps1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps1 in HP whatever Vsmps1Request signal 
     * value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_REG) & VSMPS_1_REQUEST_CONTROL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 7:6
 *
 **************************************************************************/
unsigned char set_vsmps2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vsmps2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps2 in HP whatever Vsmps2Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_2_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x6);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x6);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x6);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl
 *
 * Notes : From register 0x303, bits 7:6
 *
 **************************************************************************/
unsigned char get_vsmps2_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vsmps2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps2 in HP whatever Vsmps2Request signal 
     * value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_REG) & VSMPS_2_REQUEST_CONTROL_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 1:0
 *
 **************************************************************************/
unsigned char set_vsmps3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vsmps3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps3 in HP whatever Vsmps3Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_3_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value =  value | ALLOWS_HP_LP ;
                break;
           case ALLOWS_HP_OFF_E:
                value =  value | ALLOWS_HP_OFF ;
                break;
           case FORCE_HP_E:
                value =  value | FORCE_HP ;
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 1:0
 *
 **************************************************************************/
unsigned char get_vsmps3_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vsmps3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps3 in HP whatever Vsmps3Request signal 
     * value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_2_REG) & VSMPS_3_REQUEST_CONTROL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 3:2
 *
 **************************************************************************/
unsigned char set_vpll_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vpll in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vpll in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vpll in HP whatever VpllRequest signal value 
     */ 
  

     value =  old_value & ~VPLL_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x2);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x2);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x2);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 3:2
 *
 **************************************************************************/
unsigned char get_vpll_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vpll in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vpll in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vpll in HP whatever VpllRequest signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_2_REG) & VPLL_REQUEST_CONTROL_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 5:4
 *
 **************************************************************************/
unsigned char set_vana_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vana in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vana in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vana in HP whatever VanaRequest signal value 
     */ 
  

     value =  old_value & ~VANA_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x4);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x4);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x4);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 5:4
 *
 **************************************************************************/
unsigned char get_vana_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vana in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vana in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vana in HP whatever VanaRequest signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_2_REG) & VANA_REQUEST_CONTROL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 7:6
 *
 **************************************************************************/
unsigned char set_vext_supply1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set VextSupply1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply1 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply1 in HP whatever VextSupply1Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_1_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x6);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x6);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x6);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl2
 *
 * Notes : From register 0x304, bits 7:6
 *
 **************************************************************************/
unsigned char get_vext_supply1_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set VextSupply1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply1 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply1 in HP whatever VextSupply1Request 
     * signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_2_REG) & VEXT_SUPPLY_1_REQUEST_CONTROL_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 1:0
 *
 **************************************************************************/
unsigned char set_vext_supply2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set VextSupply2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply2 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply2 in HP whatever VextSupply2Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_2_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value =  value | ALLOWS_HP_LP ;
                break;
           case ALLOWS_HP_OFF_E:
                value =  value | ALLOWS_HP_OFF ;
                break;
           case FORCE_HP_E:
                value =  value | FORCE_HP ;
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 1:0
 *
 **************************************************************************/
unsigned char get_vext_supply2_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set VextSupply2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply2 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply2 in HP whatever VextSupply2Request 
     * signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_3_REG) & VEXT_SUPPLY_2_REQUEST_CONTROL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 3:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set VextSupply3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply3 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply3 in HP whatever VextSupply3Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_3_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x2);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x2);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x2);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 3:2
 *
 **************************************************************************/
unsigned char get_vext_supply3_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set VextSupply3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply3 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply3 in HP whatever VextSupply3Request 
     * signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_3_REG) & VEXT_SUPPLY_3_REQUEST_CONTROL_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 5:4
 *
 **************************************************************************/
unsigned char set_vaux1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set Vaux1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux1 in HP whatever Vaux1Request signal value 
     */ 
  

     value =  old_value & ~VAUX_1_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x4);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x4);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x4);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 5:4
 *
 **************************************************************************/
unsigned char get_vaux1_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vaux1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux1 in HP whatever Vaux1Request signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_3_REG) & VAUX_1_REQUEST_CONTROL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 7:6
 *
 **************************************************************************/
unsigned char set_vaux2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set Vaux2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux2 in HP whatever Vaux2Request signal value 
     */ 
  

     value =  old_value & ~VAUX_2_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value  = value  | (ALLOWS_HP_LP << 0x6);
                break;
           case ALLOWS_HP_OFF_E:
                value  = value  | (ALLOWS_HP_OFF << 0x6);
                break;
           case FORCE_HP_E:
                value  = value  | (FORCE_HP << 0x6);
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl3
 *
 * Notes : From register 0x305, bits 7:6
 *
 **************************************************************************/
unsigned char get_vaux2_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vaux2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux2 in HP whatever Vaux2Request signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_3_REG) & VAUX_2_REQUEST_CONTROL_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl4
 *
 * Notes : From register 0x306, bits 1:0
 *
 **************************************************************************/
unsigned char set_vaux3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_4_REG);

    /* 
     * 00: allows to set Vaux3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux3 in HP whatever Vaux3Request signal value 
     */ 
  

     value =  old_value & ~VAUX_3_REQUEST_CONTROL_MASK ;

    switch(  param ){

           case ALLOWS_HP_LP_E:
                value =  value | ALLOWS_HP_LP ;
                break;
           case ALLOWS_HP_OFF_E:
                value =  value | ALLOWS_HP_OFF ;
                break;
           case FORCE_HP_E:
                value =  value | FORCE_HP ;
                break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_request_control
 *
 * RET  : Return the value of register 0xReguRequestControl4
 *
 * Notes : From register 0x306, bits 1:0
 *
 **************************************************************************/
unsigned char get_vaux3_request_control()
  {
    unsigned char value;


    /* 
     * 00: allows to set Vaux3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux3 in HP whatever Vaux3Request signal value 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_4_REG) & VAUX_3_REQUEST_CONTROL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_supply
 *
 * IN   : param, a value to write to the regiter ReguRequestControl4
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguRequestControl4
 *
 * Notes : From register 0x306, bits 2:2
 *
 **************************************************************************/
unsigned char set_software_supply( enum software_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_REQUEST_CONTROL_4_REG);

    /* 
     * 0: Sw doesn't request supply 
     * 1: Sw request supply validated by 'VxSwHPReqValid' bits 
     */ 
    switch( param ){
        case SOFTWARE_SUPPLY_ENABLE_E: 
           value = old_value | SOFTWARE_SUPPLY_MASK; 
           break;
        case SOFTWARE_SUPPLY_DISABLE_E: 
           value = old_value & ~ SOFTWARE_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(REGU_REQUEST_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_supply
 *
 * RET  : Return the value of register 0xReguRequestControl4
 *
 * Notes : From register 0x306, bits 2:2
 *
 **************************************************************************/
unsigned char get_software_supply()
  {
    unsigned char value;


    /* 
     * 0: Sw doesn't request supply 
     * 1: Sw request supply validated by 'VxSwHPReqValid' bits 
     */ 
    value = (I2CRead(REGU_REQUEST_CONTROL_4_REG) & SOFTWARE_SUPPLY_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsmps1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps1 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps1 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VSMPS_1_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VSMPS_1_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsmps1_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps1 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VSMPS_1_SYS_CLK_REQ_1HP_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps2 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps2 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VSMPS_2_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VSMPS_2_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsmps2_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps2 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VSMPS_2_SYS_CLK_REQ_1HP_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps3 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps3 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VSMPS_3_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VSMPS_3_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps3_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps3 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VSMPS_3_SYS_CLK_REQ_1HP_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vana supply 
     * 1: valid SysClkReq1 input to request Vana supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VANA_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VANA_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 3:3
 *
 **************************************************************************/
unsigned char get_vana_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vana supply 
     * 1: valid SysClkReq1 input to request Vana supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VANA_SYS_CLK_REQ_1HP_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vppl_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 4:4
 *
 **************************************************************************/
unsigned char set_vppl_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vppl supply 
     * 1: valid SysClkReq1 input to request Vppl supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VPPL_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VPPL_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vppl_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 4:4
 *
 **************************************************************************/
unsigned char get_vppl_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vppl supply 
     * 1: valid SysClkReq1 input to request Vppl supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VPPL_SYS_CLK_REQ_1HP_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux1 supply 
     * 1: valid SysClkReq1 input to request Vaux1 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VAUX_1_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VAUX_1_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 5:5
 *
 **************************************************************************/
unsigned char get_vaux1_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux1 supply 
     * 1: valid SysClkReq1 input to request Vaux1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VAUX_1_SYS_CLK_REQ_1HP_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux2 supply 
     * 1: valid SysClkReq1 input to request Vaux2 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VAUX_2_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VAUX_2_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 6:6
 *
 **************************************************************************/
unsigned char get_vaux2_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux2 supply 
     * 1: valid SysClkReq1 input to request Vaux2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VAUX_2_SYS_CLK_REQ_1HP_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux3 supply 
     * 1: valid SysClkReq1 input to request Vaux3 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VAUX_3_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VAUX_3_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid
 *
 * Notes : From register 0x307, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux3_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux3 supply 
     * 1: valid SysClkReq1 input to request Vaux3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_REG) & VAUX_3_SYS_CLK_REQ_1HP_VALID_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 0:0
 *
 **************************************************************************/
unsigned char set_vape_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vape supply 
     * 1: valid SysClkReq1 input to request Vape supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VAPE_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VAPE_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 0:0
 *
 **************************************************************************/
unsigned char get_vape_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vape supply 
     * 1: valid SysClkReq1 input to request Vape supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VAPE_SYS_CLK_REQ_1HP_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 1:1
 *
 **************************************************************************/
unsigned char set_varm_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Varm supply 
     * 1: valid SysClkReq1 input to request Varm supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VARM_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VARM_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 1:1
 *
 **************************************************************************/
unsigned char get_varm_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Varm supply 
     * 1: valid SysClkReq1 input to request Varm supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VARM_SYS_CLK_REQ_1HP_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbb_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 2:2
 *
 **************************************************************************/
unsigned char set_vbb_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vbb supply 
     * 1: valid SysClkReq1 input to request Vbb supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VBB_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VBB_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbb_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 2:2
 *
 **************************************************************************/
unsigned char get_vbb_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vbb supply 
     * 1: valid SysClkReq1 input to request Vbb supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VBB_SYS_CLK_REQ_1HP_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vmod supply 
     * 1: valid SysClkReq1 input to request Vmod supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VMOD_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VMOD_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 3:3
 *
 **************************************************************************/
unsigned char get_vmod_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vmod supply 
     * 1: valid SysClkReq1 input to request Vmod supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VMOD_SYS_CLK_REQ_1HP_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 4:4
 *
 **************************************************************************/
unsigned char set_vext_supply1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply1 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply1 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 4:4
 *
 **************************************************************************/
unsigned char get_vext_supply1_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply1 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 5:5
 *
 **************************************************************************/
unsigned char set_vext_supply2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply2 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply2 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 5:5
 *
 **************************************************************************/
unsigned char get_vext_supply2_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply2 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 6:6
 *
 **************************************************************************/
unsigned char set_vext_supply3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_VALID_1_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply3 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply3 supply 
     */ 
    switch( param ){
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_VALID_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xSupplyControlValid1
 *
 * Notes : From register 0x308, bits 6:6
 *
 **************************************************************************/
unsigned char get_vext_supply3_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply3 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_VALID_1_REG) & VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsmps1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_1_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_1_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsmps1_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VSMPS_1_HW_HP_REQ_1_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_2_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_2_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsmps2_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VSMPS_2_HW_HP_REQ_1_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_3_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_3_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps3_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VSMPS_3_HW_HP_REQ_1_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VANA_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VANA_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 3:3
 *
 **************************************************************************/
unsigned char get_vana_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VANA_HW_HP_REQ_1_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 4:4
 *
 **************************************************************************/
unsigned char set_vpll_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VPLL_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VPLL_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 4:4
 *
 **************************************************************************/
unsigned char get_vpll_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VPLL_HW_HP_REQ_1_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_1_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_1_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 5:5
 *
 **************************************************************************/
unsigned char get_vaux1_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VAUX_1_HW_HP_REQ_1_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_2_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_2_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 6:6
 *
 **************************************************************************/
unsigned char get_vaux2_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VAUX_2_HW_HP_REQ_1_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_3_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_3_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux3_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & VAUX_3_HW_HP_REQ_1_VALID_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 0:0
 *
 **************************************************************************/
unsigned char set_vext_supply1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 0:0
 *
 **************************************************************************/
unsigned char get_vext_supply1_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 1:1
 *
 **************************************************************************/
unsigned char set_vext_supply2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 1:1
 *
 **************************************************************************/
unsigned char get_vext_supply2_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 2:2
 *
 **************************************************************************/
unsigned char get_vext_supply3_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VMOD_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VMOD_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 3:3
 *
 **************************************************************************/
unsigned char get_vmod_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & VMOD_HW_HP_REQ_1_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsmps1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_1_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_1_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsmps1_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VSMPS_1_HW_HP_REQ_2_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_2_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_2_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 1:1
 *
 **************************************************************************/
unsigned char get_vsmps2_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VSMPS_2_HW_HP_REQ_2_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VSMPS_3_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VSMPS_3_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps3_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VSMPS_3_HW_HP_REQ_2_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VANA_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VANA_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 3:3
 *
 **************************************************************************/
unsigned char get_vana_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VANA_HW_HP_REQ_2_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 4:4
 *
 **************************************************************************/
unsigned char set_vpll_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VPLL_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VPLL_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 4:4
 *
 **************************************************************************/
unsigned char get_vpll_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VPLL_HW_HP_REQ_2_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_1_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_1_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 5:5
 *
 **************************************************************************/
unsigned char get_vaux1_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VAUX_1_HW_HP_REQ_2_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_2_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_2_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 6:6
 *
 **************************************************************************/
unsigned char get_vaux2_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VAUX_2_HW_HP_REQ_2_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VAUX_3_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VAUX_3_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid3
 *
 * Notes : From register 0x30B, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux3_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_3_REG) & VAUX_3_HW_HP_REQ_2_VALID_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 0:0
 *
 **************************************************************************/
unsigned char set_vext_supply1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 0:0
 *
 **************************************************************************/
unsigned char get_vext_supply1_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG) & VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 1:1
 *
 **************************************************************************/
unsigned char set_vext_supply2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 1:1
 *
 **************************************************************************/
unsigned char get_vext_supply2_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG) & VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 2:2
 *
 **************************************************************************/
unsigned char get_vext_supply3_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG) & VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    switch( param ){
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | VMOD_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ VMOD_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_HARDWARE_VALID_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xSupplyControlHardwareValid4
 *
 * Notes : From register 0x30C, bits 3:3
 *
 **************************************************************************/
unsigned char get_vmod_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_HARDWARE_VALID_4_REG) & VMOD_HW_HP_REQ_2_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 0:0
 *
 **************************************************************************/
unsigned char set_vape_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vape supply 
     * 1: valid SW to request Vape supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VAPE_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VAPE_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 0:0
 *
 **************************************************************************/
unsigned char get_vape_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vape supply 
     * 1: valid SW to request Vape supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VAPE_SW_HP_REQ_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 1:1
 *
 **************************************************************************/
unsigned char set_varm_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Varm supply 
     * 1: valid SW to request Varm supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VARM_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VARM_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 1:1
 *
 **************************************************************************/
unsigned char get_varm_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Varm supply 
     * 1: valid SW to request Varm supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VARM_SW_HP_REQ_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps1 supply 
     * 1: valid SW to request Vsmps1 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VSMPS_1_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VSMPS_1_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 2:2
 *
 **************************************************************************/
unsigned char get_vsmps1_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vsmps1 supply 
     * 1: valid SW to request Vsmps1 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VSMPS_1_SW_HP_REQ_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsmps2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps2 supply 
     * 1: valid SW to request Vsmps2 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VSMPS_2_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VSMPS_2_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsmps2_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vsmps2 supply 
     * 1: valid SW to request Vsmps2 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VSMPS_2_SW_HP_REQ_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 4:4
 *
 **************************************************************************/
unsigned char set_vsmps3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps3 supply 
     * 1: valid SW to request Vsmps3 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VSMPS_3_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VSMPS_3_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 4:4
 *
 **************************************************************************/
unsigned char get_vsmps3_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vsmps3 supply 
     * 1: valid SW to request Vsmps3 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VSMPS_3_SW_HP_REQ_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 5:5
 *
 **************************************************************************/
unsigned char set_vana_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vana supply 
     * 1: valid SW to request Vana supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VANA_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VANA_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 5:5
 *
 **************************************************************************/
unsigned char get_vana_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vana supply 
     * 1: valid SW to request Vana supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VANA_SW_HP_REQ_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 6:6
 *
 **************************************************************************/
unsigned char set_vpll_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vpll supply 
     * 1: valid SW to request Vpll supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VPLL_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VPLL_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 6:6
 *
 **************************************************************************/
unsigned char get_vpll_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vpll supply 
     * 1: valid SW to request Vpll supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VPLL_SW_HP_REQ_VALID_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vaux1 supply 
     * 1: valid SW to request Vaux1 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VAUX_1_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VAUX_1_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 7:7
 *
 **************************************************************************/
unsigned char get_vaux1_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vaux1 supply 
     * 1: valid SW to request Vaux1 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_REG) & VAUX_1_SW_HP_REQ_VALID_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 0:0
 *
 **************************************************************************/
unsigned char set_vaux2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vaux2 supply 
     * 1: valid SW to request Vaux2 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VAUX_2_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VAUX_2_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 0:0
 *
 **************************************************************************/
unsigned char get_vaux2_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vaux2 supply 
     * 1: valid SW to request Vaux2 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VAUX_2_SW_HP_REQ_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 1:1
 *
 **************************************************************************/
unsigned char set_vaux3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vaux3 supply 
     * 1: valid SW to request Vaux3 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VAUX_3_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VAUX_3_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 1:1
 *
 **************************************************************************/
unsigned char get_vaux3_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vaux3 supply 
     * 1: valid SW to request Vaux3 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VAUX_3_SW_HP_REQ_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply1 supply 
     * 1: valid SW to request VextSupply1 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VEXT_SUPPLY_1_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VEXT_SUPPLY_1_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 2:2
 *
 **************************************************************************/
unsigned char get_vext_supply1_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request VextSupply1 supply 
     * 1: valid SW to request VextSupply1 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VEXT_SUPPLY_1_SW_HP_REQ_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 3:3
 *
 **************************************************************************/
unsigned char set_vext_supply2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply2 supply 
     * 1: valid SW to request VextSupply2 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VEXT_SUPPLY_2_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VEXT_SUPPLY_2_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 3:3
 *
 **************************************************************************/
unsigned char get_vext_supply2_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request VextSupply2 supply 
     * 1: valid SW to request VextSupply2 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VEXT_SUPPLY_2_SW_HP_REQ_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 4:4
 *
 **************************************************************************/
unsigned char set_vext_supply3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply3 supply 
     * 1: valid SW to request VextSupply3 supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VEXT_SUPPLY_3_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VEXT_SUPPLY_3_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 4:4
 *
 **************************************************************************/
unsigned char get_vext_supply3_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request VextSupply3 supply 
     * 1: valid SW to request VextSupply3 supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VEXT_SUPPLY_3_SW_HP_REQ_VALID_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 5:5
 *
 **************************************************************************/
unsigned char set_vmod_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vmod supply 
     * 1: valid SW to request Vmod supply 
     */ 
    switch( param ){
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | VMOD_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ VMOD_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xSoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 5:5
 *
 **************************************************************************/
unsigned char get_vmod_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vmod supply 
     * 1: valid SW to request Vmod supply 
     */ 
    value = (I2CRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & VMOD_SW_HP_REQ_VALID_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq2 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_2_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_2_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 1:1
 *
 **************************************************************************/
unsigned char get_sys_clk_req2_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_2_VALID_1_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq3 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_3_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_3_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clk_req3_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_3_VALID_1_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq4 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_4_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_4_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clk_req4_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_4_VALID_1_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq5 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_5_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_5_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clk_req5_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_5_VALID_1_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq6 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_6_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_6_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_req6_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_6_VALID_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq7 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_7_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_7_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_req7_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_7_VALID_1_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq8 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 1 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_8_VALID_1_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_8_VALID_1_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_valid1
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_req8_valid1()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 1 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLK_REQ_8_VALID_1_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq2 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_2_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_2_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 1:1
 *
 **************************************************************************/
unsigned char get_sys_clk_req2_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq2 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_2_VALID_2_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq3 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_3_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_3_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 2:2
 *
 **************************************************************************/
unsigned char get_sys_clk_req3_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq3 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_3_VALID_2_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq4 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_4_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_4_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 3:3
 *
 **************************************************************************/
unsigned char get_sys_clk_req4_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq4 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_4_VALID_2_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq5 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_5_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_5_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 4:4
 *
 **************************************************************************/
unsigned char get_sys_clk_req5_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq5 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_5_VALID_2_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq6 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_6_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_6_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 5:5
 *
 **************************************************************************/
unsigned char get_sys_clk_req6_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq6 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_6_VALID_2_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq7 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_7_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_7_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 6:6
 *
 **************************************************************************/
unsigned char get_sys_clk_req7_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq7 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_7_VALID_2_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq8 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 2 to request Vx supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLK_REQ_8_VALID_2_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLK_REQ_8_VALID_2_MASK;
           break;
    }
  

    I2CWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_valid2
 *
 * RET  : Return the value of register 0xSupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 7:7
 *
 **************************************************************************/
unsigned char get_sys_clk_req8_valid2()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq8 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 2 to request Vx supply 
     */ 
    value = (I2CRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLK_REQ_8_VALID_2_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux4_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter ReguVaux4ReqValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 0:0
 *
 **************************************************************************/
unsigned char set_vaux4_sw_h_p_req_valid( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_VAUX_4_REQ_VALID_REG);

    /* 
     * 0: doesn't valid SW to request Vape supply 
     * 1: valid SW to request Vape supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | VAUX_4_SW_HP_REQ_VALID_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ VAUX_4_SW_HP_REQ_VALID_MASK;
           break;
    }
  

    I2CWrite(REGU_VAUX_4_REQ_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux4_sw_h_p_req_valid
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 0:0
 *
 **************************************************************************/
unsigned char get_vaux4_sw_h_p_req_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SW to request Vape supply 
     * 1: valid SW to request Vape supply 
     */ 
    value = (I2CRead(REGU_VAUX_4_REQ_VALID_REG) & VAUX_4_SW_HP_REQ_VALID_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux4_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter ReguVaux4ReqValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 1:1
 *
 **************************************************************************/
unsigned char set_vaux4_hw_h_p_req2_valid( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_VAUX_4_REQ_VALID_REG);

    /* 
     * 0: doesn't valid configuration 2 to request Vaux4 supply 
     * 1: valid HW configuration 2 to request Vaux4 supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | VAUX_4_HW_HP_REQ_2_VALID_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ VAUX_4_HW_HP_REQ_2_VALID_MASK;
           break;
    }
  

    I2CWrite(REGU_VAUX_4_REQ_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux4_hw_h_p_req2_valid
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 1:1
 *
 **************************************************************************/
unsigned char get_vaux4_hw_h_p_req2_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid configuration 2 to request Vaux4 supply 
     * 1: valid HW configuration 2 to request Vaux4 supply 
     */ 
    value = (I2CRead(REGU_VAUX_4_REQ_VALID_REG) & VAUX_4_HW_HP_REQ_2_VALID_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux4_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter ReguVaux4ReqValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 2:2
 *
 **************************************************************************/
unsigned char set_vaux4_hw_h_p_req1_valid( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_VAUX_4_REQ_VALID_REG);

    /* 
     * 0: doesn't valid configuration 1 to request Vaux4 supply 
     * 1: valid HW configuration 1 to request Vaux4 supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | VAUX_4_HW_HP_REQ_1_VALID_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ VAUX_4_HW_HP_REQ_1_VALID_MASK;
           break;
    }
  

    I2CWrite(REGU_VAUX_4_REQ_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux4_hw_h_p_req1_valid
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 2:2
 *
 **************************************************************************/
unsigned char get_vaux4_hw_h_p_req1_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid configuration 1 to request Vaux4 supply 
     * 1: valid HW configuration 1 to request Vaux4 supply 
     */ 
    value = (I2CRead(REGU_VAUX_4_REQ_VALID_REG) & VAUX_4_HW_HP_REQ_1_VALID_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux4_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter ReguVaux4ReqValid
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 3:3
 *
 **************************************************************************/
unsigned char set_vaux4_sys_clk_req1_h_p_valid( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_VAUX_4_REQ_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux4 supply 
     * 1: valid SysClkReq1 input to request Vaux4 supply 
     */ 
    switch( param ){
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | VAUX_4_SYS_CLK_REQ_1HP_VALID_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ VAUX_4_SYS_CLK_REQ_1HP_VALID_MASK;
           break;
    }
  

    I2CWrite(REGU_VAUX_4_REQ_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux4_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register 0xReguVaux4ReqValid
 *
 * Notes : From register 0x311, bits 3:3
 *
 **************************************************************************/
unsigned char get_vaux4_sys_clk_req1_h_p_valid()
  {
    unsigned char value;


    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux4 supply 
     * 1: valid SysClkReq1 input to request Vaux4 supply 
     */ 
    value = (I2CRead(REGU_VAUX_4_REQ_VALID_REG) & VAUX_4_SYS_CLK_REQ_1HP_VALID_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vt_vout
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 1:1
 *
 **************************************************************************/
unsigned char set_vt_vout( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_MISCELLANOUS_REG);

    /* 
     * VTVoutEna 
     * 0: disable VTVout supply 
     * 1: enable VTVout supply 
     */ 
    switch( param ){
        case LOW_POWER_DISABLE_E: 
           value = old_value | VT_VOUT_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VT_VOUT_MASK;
           break;
    }
  

    I2CWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vt_vout
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 1:1
 *
 **************************************************************************/
unsigned char get_vt_vout()
  {
    unsigned char value;


    /* 
     * VTVoutEna 
     * 0: disable VTVout supply 
     * 1: enable VTVout supply 
     */ 
    value = (I2CRead(REGU_MISCELLANOUS_REG) & VT_VOUT_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vint_core12_ena
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 2:2
 *
 **************************************************************************/
unsigned char set_vint_core12_ena( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_MISCELLANOUS_REG);

    /* 
     * VintCore12Ena 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param ){
        case LOW_POWER_DISABLE_E: 
           value = old_value | VINT_CORE_12_ENA_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VINT_CORE_12_ENA_MASK;
           break;
    }
  

    I2CWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vint_core12_ena
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 2:2
 *
 **************************************************************************/
unsigned char get_vint_core12_ena()
  {
    unsigned char value;


    /* 
     * VintCore12Ena 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    value = (I2CRead(REGU_MISCELLANOUS_REG) & VINT_CORE_12_ENA_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vint_core12_sel
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 5:3
 *
 **************************************************************************/
unsigned char set_vint_core12_sel( enum vout_core param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_MISCELLANOUS_REG);

    /* 
     * select output voltage 
     */ 
  
    value =  old_value & ~VINT_CORE_12_SEL_MASK;


    value |= ( param << 0x3);  
    I2CWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vint_core12_sel
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 5:3
 *
 **************************************************************************/
unsigned char get_vint_core12_sel()
  {
    unsigned char value;


    /* 
     * select output voltage 
     */ 
    value = (I2CRead(REGU_MISCELLANOUS_REG) & VINT_CORE_12_SEL_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_mode
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_mode( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_MISCELLANOUS_REG);

    /* 
     * VintCore12LP 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param ){
        case LOW_POWER_DISABLE_E: 
           value = old_value | LOW_POWER_MODE_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ LOW_POWER_MODE_MASK;
           break;
    }
  

    I2CWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_mode
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 6:6
 *
 **************************************************************************/
unsigned char get_low_power_mode()
  {
    unsigned char value;


    /* 
     * VintCore12LP 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    value = (I2CRead(REGU_MISCELLANOUS_REG) & LOW_POWER_MODE_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vt_vout_low_power
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 7:7
 *
 **************************************************************************/
unsigned char set_vt_vout_low_power( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGU_MISCELLANOUS_REG);

    /* 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param ){
        case LOW_POWER_DISABLE_E: 
           value = old_value | VT_VOUT_LOW_POWER_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VT_VOUT_LOW_POWER_MASK;
           break;
    }
  

    I2CWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vt_vout_low_power
 *
 * RET  : Return the value of register 0xReguMiscellanous
 *
 * Notes : From register 0x380, bits 7:7
 *
 **************************************************************************/
unsigned char get_vt_vout_low_power()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    value = (I2CRead(REGU_MISCELLANOUS_REG) & VT_VOUT_LOW_POWER_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_otg_supply
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 0:0
 *
 **************************************************************************/
unsigned char set_otg_supply( enum otg_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    switch( param ){
        case OTG_SUPPLY_ENABLE_E: 
           value = old_value | OTG_SUPPLY_MASK; 
           break;
        case OTG_SUPPLY_DISABLE_E: 
           value = old_value & ~ OTG_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_otg_supply
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 0:0
 *
 **************************************************************************/
unsigned char get_otg_supply()
  {
    unsigned char value;


    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    value = (I2CRead(OTG_SUPPLY_CONTROL_REG) & OTG_SUPPLY_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_otg_supply_force_l_p
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 1:1
 *
 **************************************************************************/
unsigned char set_otg_supply_force_l_p( enum otg_supply_force_l_p_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    switch( param ){
        case OTG_SUPPLY_FORCE_LP_ENABLE_E: 
           value = old_value | OTG_SUPPLY_FORCE_LP_MASK; 
           break;
        case OTG_SUPPLY_FORCE_LP_DISABLE_E: 
           value = old_value & ~ OTG_SUPPLY_FORCE_LP_MASK;
           break;
    }
  

    I2CWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_otg_supply_force_l_p
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 1:1
 *
 **************************************************************************/
unsigned char get_otg_supply_force_l_p()
  {
    unsigned char value;


    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    value = (I2CRead(OTG_SUPPLY_CONTROL_REG) & OTG_SUPPLY_FORCE_LP_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_bis
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 3:3
 *
 **************************************************************************/
unsigned char set_vbus_bis( enum vbus_bis_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable VbusBis (5v stepup) supply 
     * 1: enable Vbusbis (5v stepup) supply 
     */ 
    switch( param ){
        case VBUS_BIS_ENABLE_E: 
           value = old_value | VBUS_BIS_MASK; 
           break;
        case VBUS_BIS_DISABLE_E: 
           value = old_value & ~ VBUS_BIS_MASK;
           break;
    }
  

    I2CWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_bis
 *
 * RET  : Return the value of register 0xOtgSupplyControl
 *
 * Notes : From register 0x381, bits 3:3
 *
 **************************************************************************/
unsigned char get_vbus_bis()
  {
    unsigned char value;


    /* 
     * 0: disable VbusBis (5v stepup) supply 
     * 1: enable Vbusbis (5v stepup) supply 
     */ 
    value = (I2CRead(OTG_SUPPLY_CONTROL_REG) & VBUS_BIS_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vusb
 *
 * IN   : param, a value to write to the regiter UsbSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 0:0
 *
 **************************************************************************/
unsigned char set_vusb( enum usb_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_SUPPLY_CONTROL_REG);

    /* 
     * VusbEna (Vusb can be enabled thru 'usb logic' or VusbEna 
     * bit) 
     * 0: disable Vusb supply 
     * 1: enable Vusb supply 
     */ 
    switch( param ){
        case USB_SUPPLY_ENABLE_E: 
           value = old_value | VUSB_MASK; 
           break;
        case USB_SUPPLY_DISABLE_E: 
           value = old_value & ~ VUSB_MASK;
           break;
    }
  

    I2CWrite(USB_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vusb
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 0:0
 *
 **************************************************************************/
unsigned char get_vusb()
  {
    unsigned char value;


    /* 
     * VusbEna (Vusb can be enabled thru 'usb logic' or VusbEna 
     * bit) 
     * 0: disable Vusb supply 
     * 1: enable Vusb supply 
     */ 
    value = (I2CRead(USB_SUPPLY_CONTROL_REG) & VUSB_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_low_power_mode
 *
 * IN   : param, a value to write to the regiter UsbSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 1:1
 *
 **************************************************************************/
unsigned char set_vbus_low_power_mode( enum vbus_low_power_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_SUPPLY_CONTROL_REG);

    /* 
     * 0: Vusb regulator is not in LP mode 
     * 1: Vusb regulator is set LP mode 
     */ 
    switch( param ){
        case VBUS_LOWPOWER_IN_MODE_E: 
           value = old_value | VBUS_LOW_POWER_MODE_MASK; 
           break;
        case VBUS_LOWPOWER_NOT_IN_MODE_E: 
           value = old_value & ~ VBUS_LOW_POWER_MODE_MASK;
           break;
    }
  

    I2CWrite(USB_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_low_power_mode
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 1:1
 *
 **************************************************************************/
unsigned char get_vbus_low_power_mode()
  {
    unsigned char value;


    /* 
     * 0: Vusb regulator is not in LP mode 
     * 1: Vusb regulator is set LP mode 
     */ 
    value = (I2CRead(USB_SUPPLY_CONTROL_REG) & VBUS_LOW_POWER_MODE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_v_usb_turn_off
 *
 * IN   : param, a value to write to the regiter UsbSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 4:4
 *
 **************************************************************************/
unsigned char set_v_usb_turn_off( enum v_usb_turn_off param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_SUPPLY_CONTROL_REG);

    /* 
     * 0 : Doesn't mask Vusb turn off when VbusOVV occurs 
     * 1: Mask Vusb turn off when VbusOVV occurs 
     */ 
    switch( param ){
        case MASK_VBUS_OVER_VOLATGE_E: 
           value = old_value | V_USB_TURN_OFF_MASK; 
           break;
        case UNMASK_VBUS_OVER_VOLATGE_E: 
           value = old_value & ~ V_USB_TURN_OFF_MASK;
           break;
    }
  

    I2CWrite(USB_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_v_usb_turn_off
 *
 * RET  : Return the value of register 0xUsbSupplyControl
 *
 * Notes : From register 0x382, bits 4:4
 *
 **************************************************************************/
unsigned char get_v_usb_turn_off()
  {
    unsigned char value;


    /* 
     * 0 : Doesn't mask Vusb turn off when VbusOVV occurs 
     * 1: Mask Vusb turn off when VbusOVV occurs 
     */ 
    value = (I2CRead(USB_SUPPLY_CONTROL_REG) & V_USB_TURN_OFF_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 1:1
 *
 **************************************************************************/
unsigned char set_audio_supply( enum audio1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vaudio supply 
     * 1: enable Vaudio supply 
     */ 
    switch( param ){
        case AUDIO_1_ENABLE_E: 
           value = old_value | AUDIO_SUPPLY_MASK; 
           break;
        case AUDIO_1_DISABLE_E: 
           value = old_value & ~ AUDIO_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_supply
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 1:1
 *
 **************************************************************************/
unsigned char get_audio_supply()
  {
    unsigned char value;


    /* 
     * 0: disable Vaudio supply 
     * 1: enable Vaudio supply 
     */ 
    value = (I2CRead(AUDIO_SUPPLY_CONTROL_REG) & AUDIO_SUPPLY_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_mic_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 2:2
 *
 **************************************************************************/
unsigned char set_digital_mic_supply( enum digital_mic_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    switch( param ){
        case DIGITAL_MIC_ENABLE_E: 
           value = old_value | DIGITAL_MIC_SUPPLY_MASK; 
           break;
        case DIGITAL_MIC_DISABLE_E: 
           value = old_value & ~ DIGITAL_MIC_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_mic_supply
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 2:2
 *
 **************************************************************************/
unsigned char get_digital_mic_supply()
  {
    unsigned char value;


    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    value = (I2CRead(AUDIO_SUPPLY_CONTROL_REG) & DIGITAL_MIC_SUPPLY_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_mic1_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 3:3
 *
 **************************************************************************/
unsigned char set_analog_mic1_supply( enum analog_mic1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    switch( param ){
        case ANALOG_MIC_1_ENABLE_E: 
           value = old_value | ANALOG_MIC_1_SUPPLY_MASK; 
           break;
        case ANALOG_MIC_1_DISABLE_E: 
           value = old_value & ~ ANALOG_MIC_1_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_mic1_supply
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 3:3
 *
 **************************************************************************/
unsigned char get_analog_mic1_supply()
  {
    unsigned char value;


    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    value = (I2CRead(AUDIO_SUPPLY_CONTROL_REG) & ANALOG_MIC_1_SUPPLY_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vnegative_mic2_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 4:4
 *
 **************************************************************************/
unsigned char set_vnegative_mic2_supply( enum vnegative_mic2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic2 supply 
     * 1: enable Vamic2 supply 
     */ 
    switch( param ){
        case VNEGATIVE_MIC_2_ENABLE_E: 
           value = old_value | VNEGATIVE_MIC_2_SUPPLY_MASK; 
           break;
        case VNEGATIVE_MIC_2_DISABLE_E: 
           value = old_value & ~ VNEGATIVE_MIC_2_SUPPLY_MASK;
           break;
    }
  

    I2CWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vnegative_mic2_supply
 *
 * RET  : Return the value of register 0xAudioSupplyControl
 *
 * Notes : From register 0x383, bits 4:4
 *
 **************************************************************************/
unsigned char get_vnegative_mic2_supply()
  {
    unsigned char value;


    /* 
     * 0: disable Vamic2 supply 
     * 1: enable Vamic2 supply 
     */ 
    value = (I2CRead(AUDIO_SUPPLY_CONTROL_REG) & VNEGATIVE_MIC_2_SUPPLY_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vamic1_polarity
 *
 * IN   : param, a value to write to the regiter RegulatorControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xRegulatorControl1
 *
 * Notes : From register 0x384, bits 0:0
 *
 **************************************************************************/
unsigned char set_vamic1_polarity( enum impedance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGULATOR_CONTROL_1_REG);

    /* 
     * 0: output in high impedance when Vamic1 is disabled 
     * 1: output is grounded when Vamic1 is disabled 
     */ 
  

     value =  old_value & ~VAMIC_1_POLARITY_MASK ;

    switch(  param ){

           case OUPPUT_GROUNDED_E:
                value =  value | OUPPUT_GROUNDED ;
                break;
           case OUTPUT_HIGH_IMPEDANCE_E:
                value =  value | OUTPUT_HIGH_IMPEDANCE ;
                break;
    }
  

    I2CWrite(REGULATOR_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vamic1_polarity
 *
 * RET  : Return the value of register 0xRegulatorControl1
 *
 * Notes : From register 0x384, bits 0:0
 *
 **************************************************************************/
unsigned char get_vamic1_polarity()
  {
    unsigned char value;


    /* 
     * 0: output in high impedance when Vamic1 is disabled 
     * 1: output is grounded when Vamic1 is disabled 
     */ 
    value = (I2CRead(REGULATOR_CONTROL_1_REG) & VAMIC_1_POLARITY_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vamic2_polarity
 *
 * IN   : param, a value to write to the regiter RegulatorControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xRegulatorControl1
 *
 * Notes : From register 0x384, bits 1:1
 *
 **************************************************************************/
unsigned char set_vamic2_polarity( enum impedance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGULATOR_CONTROL_1_REG);

    /* 
     * 0: output in high impedance when Vamic2 is disabled 
     * 1: output is grounded when Vamic2 is disabled 
     */ 
  

     value =  old_value & ~VAMIC_2_POLARITY_MASK ;

    switch(  param ){

           case OUPPUT_GROUNDED_E:
                value  = value  | (OUPPUT_GROUNDED << 0x1);
                break;
           case OUTPUT_HIGH_IMPEDANCE_E:
                value  = value  | (OUTPUT_HIGH_IMPEDANCE << 0x1);
                break;
    }
  

    I2CWrite(REGULATOR_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vamic2_polarity
 *
 * RET  : Return the value of register 0xRegulatorControl1
 *
 * Notes : From register 0x384, bits 1:1
 *
 **************************************************************************/
unsigned char get_vamic2_polarity()
  {
    unsigned char value;


    /* 
     * 0: output in high impedance when Vamic2 is disabled 
     * 1: output is grounded when Vamic2 is disabled 
     */ 
    value = (I2CRead(REGULATOR_CONTROL_1_REG) & VAMIC_2_POLARITY_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vamic2_impedance
 *
 * IN   : param, a value to write to the regiter RegulatorControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xRegulatorControl2
 *
 * Notes : From register 0x385, bits 0:0
 *
 **************************************************************************/
unsigned char set_vamic2_impedance( enum impedance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGULATOR_CONTROL_2_REG);

    /* 
     * 0: output in high impedance when Vamic1 is disabled 
     * 1: output is grounded when Vamic1 is disabled 
     */ 
  

     value =  old_value & ~VAMIC_2_IMPEDANCE_MASK ;

    switch(  param ){

           case OUPPUT_GROUNDED_E:
                value =  value | OUPPUT_GROUNDED ;
                break;
           case OUTPUT_HIGH_IMPEDANCE_E:
                value =  value | OUTPUT_HIGH_IMPEDANCE ;
                break;
    }
  

    I2CWrite(REGULATOR_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vamic2_impedance
 *
 * RET  : Return the value of register 0xRegulatorControl2
 *
 * Notes : From register 0x385, bits 0:0
 *
 **************************************************************************/
unsigned char get_vamic2_impedance()
  {
    unsigned char value;


    /* 
     * 0: output in high impedance when Vamic1 is disabled 
     * 1: output is grounded when Vamic1 is disabled 
     */ 
    value = (I2CRead(REGULATOR_CONTROL_2_REG) & VAMIC_2_IMPEDANCE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_ldo_polarity
 *
 * IN   : param, a value to write to the regiter RegulatorControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xRegulatorControl3
 *
 * Notes : From register 0x386, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsim_ldo_polarity( enum impedance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(REGULATOR_CONTROL_3_REG);

    /* 
     * 0: disable 
     * 1: allow to active the command setting the output of the 
     * VsimLDO on a High Impedance state. 
     */ 
  

     value =  old_value & ~VSIM_LDO_POLARITY_MASK ;

    switch(  param ){

           case OUPPUT_GROUNDED_E:
                value =  value | OUPPUT_GROUNDED ;
                break;
           case OUTPUT_HIGH_IMPEDANCE_E:
                value =  value | OUTPUT_HIGH_IMPEDANCE ;
                break;
    }
  

    I2CWrite(REGULATOR_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_ldo_polarity
 *
 * RET  : Return the value of register 0xRegulatorControl3
 *
 * Notes : From register 0x386, bits 0:0
 *
 **************************************************************************/
unsigned char get_vsim_ldo_polarity()
  {
    unsigned char value;


    /* 
     * 0: disable 
     * 1: allow to active the command setting the output of the 
     * VsimLDO on a High Impedance state. 
     */ 
    value = (I2CRead(REGULATOR_CONTROL_3_REG) & VSIM_LDO_POLARITY_MASK);
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
