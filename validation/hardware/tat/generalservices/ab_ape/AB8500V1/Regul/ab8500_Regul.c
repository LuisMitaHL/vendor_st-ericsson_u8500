/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Regul/Linux/ab8500_Regul.c
 * 
 *
 * Generated on the 26/02/2010 11:33 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_Regul.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_serial_control_interface
 *
 * IN   : param, a value to write to the regiter ProtocolControl
 * OUT  : 
 *
 * RET  : Return the value of register ProtocolControl
 *
 * Notes : From register 0x300, bits 3:0
 *
 **************************************************************************/
unsigned char set_serial_control_interface( enum serial_control_by param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(PROTOCOL_CONTROL_REG);

    /* 
     * Serial Control Interface (I2C or SPI) 
     */ 
  

     value =  old_value & ~SERIAL_CONTROL_BY_PARAM_MASK ;

    switch(  param )
      {
        case CONFIGURATION_1_E:
            value =  value | CONFIGURATION_1 ;
           break;
        case CONFIGURATION_2_E:
            value =  value | CONFIGURATION_2 ;
           break;
        case CONFIGURATION_3_E:
            value =  value | CONFIGURATION_3 ;
           break;
        case CONFIGURATION_4_E:
            value =  value | CONFIGURATION_4 ;
           break;
      }
  

    SPIWrite(PROTOCOL_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_serial_control_interface
 *
 * RET  : Return the value of register ProtocolControl
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
    value = (SPIRead(PROTOCOL_CONTROL_REG) & SERIAL_CONTROL_BY_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl
 *
 * Notes : From register 0x303, bits 7:6
 *
 **************************************************************************/
unsigned char set_vsmps2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vsmps2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps2 in HP whatever Vsmps2Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_2_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_request_control
 *
 * RET  : Return the value of register ReguRequestControl
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
    value = (SPIRead(REGU_REQUEST_CONTROL_REG) & REQUEST_CONTROL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl
 *
 * Notes : From register 0x303, bits 5:4
 *
 **************************************************************************/
unsigned char set_vsmps1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vsmps1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps1 in HP whatever Vsmps1Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_1_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_request_control
 *
 * RET  : Return the value of register ReguRequestControl
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
    value = (SPIRead(REGU_REQUEST_CONTROL_REG) & REQUEST_CONTROL_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl
 *
 * Notes : From register 0x303, bits 3:2
 *
 **************************************************************************/
unsigned char set_vape_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Vape in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vape in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vape in HP whatever VapeRequest signal value 
     */ 
  

     value =  old_value & ~VAPE_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_request_control
 *
 * RET  : Return the value of register ReguRequestControl
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
    value = (SPIRead(REGU_REQUEST_CONTROL_REG) & REQUEST_CONTROL_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl
 *
 * Notes : From register 0x303, bits 1:0
 *
 **************************************************************************/
unsigned char set_varm_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_REG);

    /* 
     * 00: allows to set Varm in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Varm in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Varm in HP whatever VarmRequest signal value 
     */ 
  

     value =  old_value & ~VARM_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_request_control
 *
 * RET  : Return the value of register ReguRequestControl
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
    value = (SPIRead(REGU_REQUEST_CONTROL_REG) & REQUEST_CONTROL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl2
 *
 * Notes : From register 0x304, bits 7:6
 *
 **************************************************************************/
unsigned char set_vext_supply1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set VextSupply1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply1 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply1 in HP whatever VextSupply1Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_1_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_request_control
 *
 * RET  : Return the value of register ReguRequestControl2
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
    value = (SPIRead(REGU_REQUEST_CONTROL_2_REG) & REQUEST_CONTROL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl2
 *
 * Notes : From register 0x304, bits 5:4
 *
 **************************************************************************/
unsigned char set_vana_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vana in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vana in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vana in HP whatever VanaRequest signal value 
     */ 
  

     value =  old_value & ~VANA_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_request_control
 *
 * RET  : Return the value of register ReguRequestControl2
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
    value = (SPIRead(REGU_REQUEST_CONTROL_2_REG) & REQUEST_CONTROL_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl2
 *
 * Notes : From register 0x304, bits 3:2
 *
 **************************************************************************/
unsigned char set_vpll_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vpll in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vpll in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vpll in HP whatever VpllRequest signal value 
     */ 
  

     value =  old_value & ~VPLL_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_request_control
 *
 * RET  : Return the value of register ReguRequestControl2
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
    value = (SPIRead(REGU_REQUEST_CONTROL_2_REG) & REQUEST_CONTROL_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl2
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl2
 *
 * Notes : From register 0x304, bits 1:0
 *
 **************************************************************************/
unsigned char set_vsmps3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_2_REG);

    /* 
     * 00: allows to set Vsmps3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vsmps3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vsmps3 in HP whatever Vsmps3Request signal 
     * value 
     */ 
  

     value =  old_value & ~VSMPS_3_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_request_control
 *
 * RET  : Return the value of register ReguRequestControl2
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
    value = (SPIRead(REGU_REQUEST_CONTROL_2_REG) & REQUEST_CONTROL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl3
 *
 * Notes : From register 0x305, bits 7:6
 *
 **************************************************************************/
unsigned char set_vaux2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set Vaux2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux2 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux2 in HP whatever Vaux2Request signal value 
     */ 
  

     value =  old_value & ~VAUX_2_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_request_control
 *
 * RET  : Return the value of register ReguRequestControl3
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
    value = (SPIRead(REGU_REQUEST_CONTROL_3_REG) & REQUEST_CONTROL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl3
 *
 * Notes : From register 0x305, bits 5:4
 *
 **************************************************************************/
unsigned char set_vaux1_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set Vaux1 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux1 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux1 in HP whatever Vaux1Request signal value 
     */ 
  

     value =  old_value & ~VAUX_1_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_request_control
 *
 * RET  : Return the value of register ReguRequestControl3
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
    value = (SPIRead(REGU_REQUEST_CONTROL_3_REG) & REQUEST_CONTROL_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl3
 *
 * Notes : From register 0x305, bits 3:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set VextSupply3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply3 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply3 in HP whatever VextSupply3Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_3_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_request_control
 *
 * RET  : Return the value of register ReguRequestControl3
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
    value = (SPIRead(REGU_REQUEST_CONTROL_3_REG) & REQUEST_CONTROL_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl3
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl3
 *
 * Notes : From register 0x305, bits 1:0
 *
 **************************************************************************/
unsigned char set_vext_supply2_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_3_REG);

    /* 
     * 00: allows to set VextSupply2 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set VextSupply2 in HP/OFF modes depending 
     * on VxRequest signal. 
     * 1x: force VextSupply2 in HP whatever VextSupply2Request 
     * signal value 
     */ 
  

     value =  old_value & ~VEXT_SUPPLY_2_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_request_control
 *
 * RET  : Return the value of register ReguRequestControl3
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
    value = (SPIRead(REGU_REQUEST_CONTROL_3_REG) & REQUEST_CONTROL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_software_supply_enable
 *
 * IN   : param, a value to write to the regiter ReguRequestControl4
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl4
 *
 * Notes : From register 0x306, bits 2:2
 *
 **************************************************************************/
unsigned char set_software_supply_enable( enum software_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_4_REG);

    /* 
     * 0: Sw doesn't request supply 
     * 1: Sw request supply validated by 'VxSwHPReqValid' bits 
     */ 
    switch( param )
      {
        case SOFTWARE_SUPPLY_ENABLE_E: 
           value = old_value | SOFTWARE_SUPPLY_ENABLE_PARAM_MASK; 
           break;
        case SOFTWARE_SUPPLY_DISABLE_E: 
           value = old_value & ~ SOFTWARE_SUPPLY_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGU_REQUEST_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_software_supply_enable
 *
 * RET  : Return the value of register ReguRequestControl4
 *
 * Notes : From register 0x306, bits 2:2
 *
 **************************************************************************/
unsigned char get_software_supply_enable()
  {
    unsigned char value;


    /* 
     * 0: Sw doesn't request supply 
     * 1: Sw request supply validated by 'VxSwHPReqValid' bits 
     */ 
    value = (SPIRead(REGU_REQUEST_CONTROL_4_REG) & SOFTWARE_SUPPLY_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_request_control
 *
 * IN   : param, a value to write to the regiter ReguRequestControl4
 * OUT  : 
 *
 * RET  : Return the value of register ReguRequestControl4
 *
 * Notes : From register 0x306, bits 1:0
 *
 **************************************************************************/
unsigned char set_vaux3_request_control( enum request_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_REQUEST_CONTROL_4_REG);

    /* 
     * 00: allows to set Vaux3 in HP/LP modes depending on 
     * VxRequest signal. 
     * 01: allows to set Vaux3 in HP/OFF modes depending on 
     * VxRequest signal. 
     * 1x: force Vaux3 in HP whatever Vaux3Request signal value 
     */ 
  

     value =  old_value & ~VAUX_3_REQUEST_CONTROL_PARAM_MASK ;

    switch(  param )
      {
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
  

    SPIWrite(REGU_REQUEST_CONTROL_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_request_control
 *
 * RET  : Return the value of register ReguRequestControl4
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
    value = (SPIRead(REGU_REQUEST_CONTROL_4_REG) & REQUEST_CONTROL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux3 supply 
     * 1: valid SysClkReq1 input to request Vaux3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux2 supply 
     * 1: valid SysClkReq1 input to request Vaux2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vaux1 supply 
     * 1: valid SysClkReq1 input to request Vaux1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vppl_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 4:4
 *
 **************************************************************************/
unsigned char set_vppl_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vppl supply 
     * 1: valid SysClkReq1 input to request Vppl supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vppl_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vana supply 
     * 1: valid SysClkReq1 input to request Vana supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps3 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps2 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x307, bits 0:0
 *
 **************************************************************************/
unsigned char set_vsmps1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vsmps1 
     * supply 
     * 1: valid SysClkReq1 input to request Vsmps1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 6:6
 *
 **************************************************************************/
unsigned char set_vext_supply3_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply3 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 5:5
 *
 **************************************************************************/
unsigned char set_vext_supply2_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply2 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 4:4
 *
 **************************************************************************/
unsigned char set_vext_supply1_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request VextSupply1 
     * supply 
     * 1: valid SysClkReq1 input to request VextSupply1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vmod supply 
     * 1: valid SysClkReq1 input to request Vmod supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbb_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 2:2
 *
 **************************************************************************/
unsigned char set_vbb_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vbb supply 
     * 1: valid SysClkReq1 input to request Vbb supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbb_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 1:1
 *
 **************************************************************************/
unsigned char set_varm_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Varm supply 
     * 1: valid SysClkReq1 input to request Varm supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_sys_clk_req1_h_p_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlValid
 *
 * Notes : From register 0x308, bits 0:0
 *
 **************************************************************************/
unsigned char set_vape_sys_clk_req1_h_p_valid( enum request_valid param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_VALID_REG);

    /* 
     * 0: doesn't valid SysClkReq1 input to request Vape supply 
     * 1: valid SysClkReq1 input to request Vape supply 
     */ 
    switch( param )
      {
        case DONT_VALID_INPUT_TO_REQUEST_E: 
           value = old_value | REQUEST_VALID_PARAM_MASK; 
           break;
        case VALID_INPUT_TO_REQUEST_E: 
           value = old_value & ~ REQUEST_VALID_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_sys_clk_req1_h_p_valid
 *
 * RET  : Return the value of register SupplyControlValid
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
    value = (SPIRead(SUPPLY_CONTROL_VALID_REG) & REQUEST_VALID_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 4:4
 *
 **************************************************************************/
unsigned char set_vpll_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 1:0
 *
 **************************************************************************/
unsigned char set_vsmps1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x309, bits 1:0
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 1:1
 *
 **************************************************************************/
unsigned char set_vext_supply2_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_hw_h_p_req1_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 1:0
 *
 **************************************************************************/
unsigned char set_vext_supply1_hw_h_p_req1_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 1 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 1 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_hw_h_p_req1_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30A, bits 1:0
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 6:6
 *
 **************************************************************************/
unsigned char set_vaux2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 5:5
 *
 **************************************************************************/
unsigned char set_vaux1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vaux1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vaux1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 4:4
 *
 **************************************************************************/
unsigned char set_vpll_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vpll supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vpll supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 3:3
 *
 **************************************************************************/
unsigned char set_vana_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vana supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vana supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 1:1
 *
 **************************************************************************/
unsigned char set_vsmps2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 1:0
 *
 **************************************************************************/
unsigned char set_vsmps1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vsmps1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vsmps1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid
 *
 * Notes : From register 0x30B, bits 1:0
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_REG) & HARDWARE_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30C, bits 3:3
 *
 **************************************************************************/
unsigned char set_vmod_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request Vmod supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request Vmod supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30C, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply3_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply3 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30C, bits 1:1
 *
 **************************************************************************/
unsigned char set_vext_supply2_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply2 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_hw_h_p_req2_valid
 *
 * IN   : param, a value to write to the regiter SupplyControlHardwareValid2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30C, bits 1:0
 *
 **************************************************************************/
unsigned char set_vext_supply1_hw_h_p_req2_valid( enum hardware_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG);

    /* 
     * 0: doesn't valid HW configuration 2 (SysClkReq[8:1] 
     * inputs) to request VextSupply1 supply 
     * 1: valid HW configuration 2 (SysClkReq[8:1] inputs) to 
     * request VextSupply1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value | HARDWARE_VALIDATION_PARAM_MASK; 
           break;
        case VALID_HARDWARE_CONFIGURATION_E: 
           value = old_value & ~ HARDWARE_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_HARDWARE_VALID_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_hw_h_p_req2_valid
 *
 * RET  : Return the value of register SupplyControlHardwareValid2
 *
 * Notes : From register 0x30C, bits 1:0
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
    value = (SPIRead(SUPPLY_CONTROL_HARDWARE_VALID_2_REG) & HARDWARE_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 7:7
 *
 **************************************************************************/
unsigned char set_vaux1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vaux1 supply 
     * 1: valid SW to request Vaux1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux1_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vpll_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 6:6
 *
 **************************************************************************/
unsigned char set_vpll_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vpll supply 
     * 1: valid SW to request Vpll supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vpll_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vana_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 5:5
 *
 **************************************************************************/
unsigned char set_vana_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vana supply 
     * 1: valid SW to request Vana supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vana_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 4:4
 *
 **************************************************************************/
unsigned char set_vsmps3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps3 supply 
     * 1: valid SW to request Vsmps3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps3_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsmps2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps2 supply 
     * 1: valid SW to request Vsmps2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps2_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsmps1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 2:2
 *
 **************************************************************************/
unsigned char set_vsmps1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vsmps1 supply 
     * 1: valid SW to request Vsmps1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsmps1_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_varm_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 1:1
 *
 **************************************************************************/
unsigned char set_varm_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Varm supply 
     * 1: valid SW to request Varm supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_varm_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vape_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation
 *
 * Notes : From register 0x30D, bits 0:0
 *
 **************************************************************************/
unsigned char set_vape_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG);

    /* 
     * 0: doesn't valid SW to request Vape supply 
     * 1: valid SW to request Vape supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vape_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vmod_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 5:5
 *
 **************************************************************************/
unsigned char set_vmod_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vmod supply 
     * 1: valid SW to request Vmod supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vmod_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 4:4
 *
 **************************************************************************/
unsigned char set_vext_supply3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply3 supply 
     * 1: valid SW to request VextSupply3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply3_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 3:3
 *
 **************************************************************************/
unsigned char set_vext_supply2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply2 supply 
     * 1: valid SW to request VextSupply2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply2_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vext_supply1_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 2:2
 *
 **************************************************************************/
unsigned char set_vext_supply1_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request VextSupply1 supply 
     * 1: valid SW to request VextSupply1 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vext_supply1_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux3_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 1:1
 *
 **************************************************************************/
unsigned char set_vaux3_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vaux3 supply 
     * 1: valid SW to request Vaux3 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux3_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vaux2_sw_h_p_req_valid
 *
 * IN   : param, a value to write to the regiter SoftwareSignalValidation2
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareSignalValidation2
 *
 * Notes : From register 0x30E, bits 0:0
 *
 **************************************************************************/
unsigned char set_vaux2_sw_h_p_req_valid( enum software_signal_validation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG);

    /* 
     * 0: doesn't valid SW to request Vaux2 supply 
     * 1: valid SW to request Vaux2 supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SOFTWARE_SIGNAL_E: 
           value = old_value | SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK; 
           break;
        case VALID_SOFTWARE_SIGNAL_E: 
           value = old_value & ~ SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SOFTWARE_SIGNAL_VALIDATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vaux2_sw_h_p_req_valid
 *
 * RET  : Return the value of register SoftwareSignalValidation2
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
    value = (SPIRead(SOFTWARE_SIGNAL_VALIDATION_2_REG) & SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq8 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq7 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq6 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq5 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq4 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq3 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_valid1
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq
 *
 * Notes : From register 0x30F, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_valid1( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG);

    /* 
     * 0: doesn't valid SysClkReq2 configuration 1 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 1 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_valid1
 *
 * RET  : Return the value of register SupplyControlSysClkReq
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req8_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 7:7
 *
 **************************************************************************/
unsigned char set_sys_clk_req8_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq8 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq8 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req8_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req7_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 6:6
 *
 **************************************************************************/
unsigned char set_sys_clk_req7_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq7 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq7 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req7_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req6_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 5:5
 *
 **************************************************************************/
unsigned char set_sys_clk_req6_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq6 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq6 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req6_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req5_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 4:4
 *
 **************************************************************************/
unsigned char set_sys_clk_req5_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq5 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq5 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req5_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req4_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 3:3
 *
 **************************************************************************/
unsigned char set_sys_clk_req4_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq4 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq4 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req4_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req3_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 2:2
 *
 **************************************************************************/
unsigned char set_sys_clk_req3_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq3 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq3 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req3_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sys_clk_req2_valid2
 *
 * IN   : param, a value to write to the regiter SupplyControlSysClkReq2
 * OUT  : 
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
 *
 * Notes : From register 0x310, bits 1:1
 *
 **************************************************************************/
unsigned char set_sys_clk_req2_valid2( enum sys_clock_request param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG);

    /* 
     * 0: doesn't valid SysClkReq2 configuration 2 to request Vx 
     * supply 
     * 1: valid SysClkReq2 configuration 2 to request Vx supply 
     */ 
    switch( param )
      {
        case DONT_VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value | SYS_CLOCK_REQUEST_PARAM_MASK; 
           break;
        case VALID_SYS_CLCK_CONFIGURATION_E: 
           value = old_value & ~ SYS_CLOCK_REQUEST_PARAM_MASK;
           break;
      }
  

    SPIWrite(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sys_clk_req2_valid2
 *
 * RET  : Return the value of register SupplyControlSysClkReq2
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
    value = (SPIRead(SUPPLY_CONTROL_SYS_CLK_REQ_2_REG) & SYS_CLOCK_REQUEST_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_v_t_vout_low_power
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 7:7
 *
 **************************************************************************/
unsigned char set_v_t_vout_low_power( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_MISCELLANOUS_REG);

    /* 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param )
      {
        case LOW_POWER_DISABLE_E: 
           value = old_value | VT_VOUT_LOW_POWER_PARAM_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VT_VOUT_LOW_POWER_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vt_vout_low_power
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 7:7
 *
 **************************************************************************/
unsigned char get_v_t_vout_low_power()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    value = (SPIRead(REGU_MISCELLANOUS_REG) & VT_VOUT_LOW_POWER_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_mode
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_mode( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_MISCELLANOUS_REG);

    /* 
     * VintCore12LP 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param )
      {
        case LOW_POWER_DISABLE_E: 
           value = old_value | LOW_POWER_MODE_PARAM_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ LOW_POWER_MODE_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_mode
 *
 * RET  : Return the value of register ReguMiscellanous
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
    value = (SPIRead(REGU_MISCELLANOUS_REG) & LOW_POWER_MODE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vint_core12_sel
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 5:3
 *
 **************************************************************************/
unsigned char set_vint_core12_sel( enum vout_core param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_MISCELLANOUS_REG);

    /* 
     * select output voltage 
     */ 
  
    value =  old_value & ~VINT_CORE_12_SEL_PARAM_MASK;


     value |= ( param << 0x3);  
    SPIWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vint_core12_sel
 *
 * RET  : Return the value of register ReguMiscellanous
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
    value = (SPIRead(REGU_MISCELLANOUS_REG) & VINT_CORE_12_SEL_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vint_core12_ena
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 2:2
 *
 **************************************************************************/
unsigned char set_vint_core12_ena( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_MISCELLANOUS_REG);

    /* 
     * VintCore12Ena 
     * 0: inactive 
     * 1: enable LP mode 
     */ 
    switch( param )
      {
        case LOW_POWER_DISABLE_E: 
           value = old_value | VINT_CORE_12_ENA_PARAM_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VINT_CORE_12_ENA_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vint_core12_ena
 *
 * RET  : Return the value of register ReguMiscellanous
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
    value = (SPIRead(REGU_MISCELLANOUS_REG) & VINT_CORE_12_ENA_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_v_t_vout_enable
 *
 * IN   : param, a value to write to the regiter ReguMiscellanous
 * OUT  : 
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 1:1
 *
 **************************************************************************/
unsigned char set_v_t_vout_enable( enum low_power_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGU_MISCELLANOUS_REG);

    /* 
     * VTVoutEna 
     * 0: disable VTVout supply 
     * 1: enable VTVout supply 
     */ 
    switch( param )
      {
        case LOW_POWER_DISABLE_E: 
           value = old_value | VT_VOUT_ENABLE_PARAM_MASK; 
           break;
        case LOW_POWER_ENABLE_E: 
           value = old_value & ~ VT_VOUT_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGU_MISCELLANOUS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vt_vout_enable
 *
 * RET  : Return the value of register ReguMiscellanous
 *
 * Notes : From register 0x380, bits 1:1
 *
 **************************************************************************/
unsigned char get_v_t_vout_enable()
  {
    unsigned char value;


    /* 
     * VTVoutEna 
     * 0: disable VTVout supply 
     * 1: enable VTVout supply 
     */ 
    value = (SPIRead(REGU_MISCELLANOUS_REG) & VT_VOUT_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_bis_enable
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register OtgSupplyControl
 *
 * Notes : From register 0x381, bits 3:3
 *
 **************************************************************************/
unsigned char set_vbus_bis_enable( enum vbus_bis_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable VbusBis (5v stepup) supply 
     * 1: enable Vbusbis (5v stepup) supply 
     */ 
    switch( param )
      {
        case VBUS_BIS_ENABLE_E: 
           value = old_value | VBUS_BIS_ENABLE_PARAM_MASK; 
           break;
        case VBUS_BIS_DISABLE_E: 
           value = old_value & ~ VBUS_BIS_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_bis_enable
 *
 * RET  : Return the value of register OtgSupplyControl
 *
 * Notes : From register 0x381, bits 3:3
 *
 **************************************************************************/
unsigned char get_vbus_bis_enable()
  {
    unsigned char value;


    /* 
     * 0: disable VbusBis (5v stepup) supply 
     * 1: enable Vbusbis (5v stepup) supply 
     */ 
    value = (SPIRead(OTG_SUPPLY_CONTROL_REG) & VBUS_BIS_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_otg_supply_force_l_p
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register OtgSupplyControl
 *
 * Notes : From register 0x381, bits 1:1
 *
 **************************************************************************/
unsigned char set_otg_supply_force_l_p( enum otg_supply_force_l_p_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    switch( param )
      {
        case OTG_SUPPLY_FORCE_LP_ENABLE_E: 
           value = old_value | OTG_SUPPLY_FORCE_LP_ENABLE_PARAM_MASK; 
           break;
        case OTG_SUPPLY_FORCE_LP_DISABLE_E: 
           value = old_value & ~ OTG_SUPPLY_FORCE_LP_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_otg_supply_force_l_p
 *
 * RET  : Return the value of register OtgSupplyControl
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
    value = (SPIRead(OTG_SUPPLY_CONTROL_REG) & OTG_SUPPLY_FORCE_LP_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_otg_supply_enable
 *
 * IN   : param, a value to write to the regiter OtgSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register OtgSupplyControl
 *
 * Notes : From register 0x381, bits 0:0
 *
 **************************************************************************/
unsigned char set_otg_supply_enable( enum otg_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_SUPPLY_CONTROL_REG);

    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    switch( param )
      {
        case OTG_SUPPLY_ENABLE_E: 
           value = old_value | OTG_SUPPLY_ENABLE_PARAM_MASK; 
           break;
        case OTG_SUPPLY_DISABLE_E: 
           value = old_value & ~ OTG_SUPPLY_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_otg_supply_enable
 *
 * RET  : Return the value of register OtgSupplyControl
 *
 * Notes : From register 0x381, bits 0:0
 *
 **************************************************************************/
unsigned char get_otg_supply_enable()
  {
    unsigned char value;


    /* 
     * RVbusPDDis 
     * 0: inactive 
     * 1: disable Vbus pull-down 
     */ 
    value = (SPIRead(OTG_SUPPLY_CONTROL_REG) & OTG_SUPPLY_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_low_power_mode
 *
 * IN   : param, a value to write to the regiter UsbSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbSupplyControl
 *
 * Notes : From register 0x382, bits 1:1
 *
 **************************************************************************/
unsigned char set_vbus_low_power_mode( enum vbus_low_power_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(USB_SUPPLY_CONTROL_REG);

    /* 
     * 0: Vusb regulator is not in LP mode 
     * 1: Vusb regulator is set LP mode 
     */ 
    switch( param )
      {
        case VBUS_LOWPOWER_NOT_IN_MODE_E: 
           value = old_value | VBUS_LOW_POWER_MODE_PARAM_MASK; 
           break;
        case VBUS_LOWPOWER_IN_MODE_E: 
           value = old_value & ~ VBUS_LOW_POWER_MODE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_low_power_mode
 *
 * RET  : Return the value of register UsbSupplyControl
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
    value = (SPIRead(USB_SUPPLY_CONTROL_REG) & VBUS_LOW_POWER_MODE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vusb_enable
 *
 * IN   : param, a value to write to the regiter UsbSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbSupplyControl
 *
 * Notes : From register 0x382, bits 0:0
 *
 **************************************************************************/
unsigned char set_vusb_enable( enum usb_supply_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(USB_SUPPLY_CONTROL_REG);

    /* 
     * VusbEna (Vusb can be enabled thru 'usb logic' or VusbEna 
     * bit) 
     * 0: disable Vusb supply 
     * 1: enable Vusb supply 
     */ 
    switch( param )
      {
        case USB_SUPPLY_ENABLE_E: 
           value = old_value | USB_SUPPLY_ENABLE_PARAM_MASK; 
           break;
        case USB_SUPPLY_DISABLE_E: 
           value = old_value & ~ USB_SUPPLY_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vusb_enable
 *
 * RET  : Return the value of register UsbSupplyControl
 *
 * Notes : From register 0x382, bits 0:0
 *
 **************************************************************************/
unsigned char get_vusb_enable()
  {
    unsigned char value;


    /* 
     * VusbEna (Vusb can be enabled thru 'usb logic' or VusbEna 
     * bit) 
     * 0: disable Vusb supply 
     * 1: enable Vusb supply 
     */ 
    value = (SPIRead(USB_SUPPLY_CONTROL_REG) & USB_SUPPLY_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vnegative_mic2_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register AudioSupplyControl
 *
 * Notes : From register 0x383, bits 4:4
 *
 **************************************************************************/
unsigned char set_vnegative_mic2_supply( enum vnegative_mic2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic2 supply 
     * 1: enable Vamic2 supply 
     */ 
    switch( param )
      {
        case VNEGATIVE_MIC_2_ENABLE_E: 
           value = old_value | VNEGATIVE_MIC_2_ENABLE_PARAM_MASK; 
           break;
        case VNEGATIVE_MIC_2_DISABLE_E: 
           value = old_value & ~ VNEGATIVE_MIC_2_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vnegative_mic2_supply
 *
 * RET  : Return the value of register AudioSupplyControl
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
    value = (SPIRead(AUDIO_SUPPLY_CONTROL_REG) & VNEGATIVE_MIC_2_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_mic1_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register AudioSupplyControl
 *
 * Notes : From register 0x383, bits 3:3
 *
 **************************************************************************/
unsigned char set_analog_mic1_supply( enum analog_mic1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    switch( param )
      {
        case ANALOG_MIC_1_ENABLE_E: 
           value = old_value | ANALOG_MIC_1_ENABLE_PARAM_MASK; 
           break;
        case ANALOG_MIC_1_DISABLE_E: 
           value = old_value & ~ ANALOG_MIC_1_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_mic1_supply
 *
 * RET  : Return the value of register AudioSupplyControl
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
    value = (SPIRead(AUDIO_SUPPLY_CONTROL_REG) & ANALOG_MIC_1_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_mic_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register AudioSupplyControl
 *
 * Notes : From register 0x383, bits 2:2
 *
 **************************************************************************/
unsigned char set_digital_mic_supply( enum digital_mic_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vamic1 supply 
     * 1: enable Vamic1 supply 
     */ 
    switch( param )
      {
        case DIGITAL_MIC_ENABLE_E: 
           value = old_value | DIGITAL_MIC_ENABLE_PARAM_MASK; 
           break;
        case DIGITAL_MIC_DISABLE_E: 
           value = old_value & ~ DIGITAL_MIC_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_mic_supply
 *
 * RET  : Return the value of register AudioSupplyControl
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
    value = (SPIRead(AUDIO_SUPPLY_CONTROL_REG) & DIGITAL_MIC_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_supply
 *
 * IN   : param, a value to write to the regiter AudioSupplyControl
 * OUT  : 
 *
 * RET  : Return the value of register AudioSupplyControl
 *
 * Notes : From register 0x383, bits 1:1
 *
 **************************************************************************/
unsigned char set_audio_supply( enum audio1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(AUDIO_SUPPLY_CONTROL_REG);

    /* 
     * 0: disable Vaudio supply 
     * 1: enable Vaudio supply 
     */ 
    switch( param )
      {
        case AUDIO_1_ENABLE_E: 
           value = old_value | AUDIO_1_ENABLE_PARAM_MASK; 
           break;
        case AUDIO_1_DISABLE_E: 
           value = old_value & ~ AUDIO_1_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(AUDIO_SUPPLY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_supply
 *
 * RET  : Return the value of register AudioSupplyControl
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
    value = (SPIRead(AUDIO_SUPPLY_CONTROL_REG) & AUDIO_1_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vamic2_polarity
 *
 * IN   : param, a value to write to the regiter RegulatorControl1
 * OUT  : 
 *
 * RET  : Return the value of register RegulatorControl1
 *
 * Notes : From register 0x384, bits 1:1
 *
 **************************************************************************/
unsigned char set_vamic2_polarity( enum polarity param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGULATOR_CONTROL_1_REG);

    /* 
     * 0: output in high impedance when Vamic2 is disabled 
     * 1: output is grounded when Vamic2 is disabled 
     */ 
    switch( param )
      {
        case OUTPUT_HIGH_IMPEDANCE_E: 
           value = old_value | VAMIC_2_POLARITY_PARAM_MASK; 
           break;
        case OUPPUT_GROUNDED_E: 
           value = old_value & ~ VAMIC_2_POLARITY_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGULATOR_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vamic2_polarity
 *
 * RET  : Return the value of register RegulatorControl1
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
    value = (SPIRead(REGULATOR_CONTROL_1_REG) & VAMIC_2_POLARITY_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vamic1_polarity
 *
 * IN   : param, a value to write to the regiter RegulatorControl1
 * OUT  : 
 *
 * RET  : Return the value of register RegulatorControl1
 *
 * Notes : From register 0x384, bits 0:0
 *
 **************************************************************************/
unsigned char set_vamic1_polarity( enum polarity param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(REGULATOR_CONTROL_1_REG);

    /* 
     * 0: output in high impedance when Vamic1 is disabled 
     * 1: output is grounded when Vamic1 is disabled 
     */ 
    switch( param )
      {
        case OUTPUT_HIGH_IMPEDANCE_E: 
           value = old_value | VAMIC_1_POLARITY_PARAM_MASK; 
           break;
        case OUPPUT_GROUNDED_E: 
           value = old_value & ~ VAMIC_1_POLARITY_PARAM_MASK;
           break;
      }
  

    SPIWrite(REGULATOR_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vamic1_polarity
 *
 * RET  : Return the value of register RegulatorControl1
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
    value = (SPIRead(REGULATOR_CONTROL_1_REG) & VAMIC_1_POLARITY_PARAM_MASK);
    return value;
  }


