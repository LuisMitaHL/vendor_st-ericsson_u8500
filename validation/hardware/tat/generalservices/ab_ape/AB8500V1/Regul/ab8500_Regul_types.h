/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Regul/Linux/ab8500_Regul_types.h
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

/****************************************************************************
 * Modifications:
 *  CAP_948_001: 22 March 2010 by Karine Boclaud
 *
 ***************************************************************************/

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_REGUL_LINUX_AB8500_REGUL_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_REGUL_LINUX_AB8500_REGUL_TYPES_H




typedef enum controled_by {
    CONTROLLED_THRU_SPI_E,
    CONTROLLED_THRU_APE_I2C_E
} CONTROLED_BY_T ;


#define  REG_CONTROLLED_THRU_SPI 0
#define  REG_CONTROLLED_THRU_APE_I2C 1
#define  REG_CONTROLLED_THRU_APE_MODEM 2



typedef enum reg_controled_by {
    REG_CONTROLLED_THRU_SPI_E,
    REG_CONTROLLED_THRU_APE_I2C_E,
    REG_CONTROLLED_THRU_APE_MODEM_E
} REG_CONTROLED_BY_T ;




//  
//  Serial Control Interface (I2C or SPI).
//  Register ProtocolControl 0x0300, Bits 0:3, typedef serial_control_by
//   
//  
#define SERIAL_CONTROL_INTERFACE_MASK 0xF
#define SERIAL_CONTROL_INTERFACE_OFFSET 3
#define SERIAL_CONTROL_INTERFACE_STOP_BIT 3
#define SERIAL_CONTROL_INTERFACE_START_BIT 0
#define SERIAL_CONTROL_INTERFACE_WIDTH 4

#define PROTOCOL_CONTROL_REG 0x300
#define  CONFIGURATION_1 0
#define  CONFIGURATION_2 1
#define  CONFIGURATION_3 2
#define  CONFIGURATION_4 3
#define SERIAL_CONTROL_BY_PARAM_MASK  0xF



typedef enum serial_control_by {
    CONFIGURATION_1_E,
    CONFIGURATION_2_E,
    CONFIGURATION_3_E,
    CONFIGURATION_4_E
} SERIAL_CONTROL_BY_T ;



#define  ALLOWS_HP_LP 0
#define  ALLOWS_HP_OFF 1
#define  FORCE_HP 2



typedef enum request_control {
    ALLOWS_HP_LP_E,
    ALLOWS_HP_OFF_E,
    FORCE_HP_E
} REQUEST_CONTROL_T ;




//  
//  00: allows to set Vsmps2 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vsmps2 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vsmps2 in HP whatever Vsmps2Request signal value.
//  Register ReguRequestControl 0x0303, Bits 6:7, typedef request_control
//   
//  
#define VSMPS_2_REQUEST_CONTROL_MASK 0xC0
#define VSMPS_2_REQUEST_CONTROL_OFFSET 7
#define VSMPS_2_REQUEST_CONTROL_STOP_BIT 7
#define VSMPS_2_REQUEST_CONTROL_START_BIT 6
#define VSMPS_2_REQUEST_CONTROL_WIDTH 2

#define REGU_REQUEST_CONTROL_REG 0x303
#define VSMPS_2_REQUEST_CONTROL_PARAM_MASK  0xC0
#define REQUEST_CONTROL_PARAM_MASK  0xC0

//  
//  00: allows to set Vsmps1 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vsmps1 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vsmps1 in HP whatever Vsmps1Request signal value.
//  Register ReguRequestControl 0x0303, Bits 4:5, typedef request_control
//   
//  
#define VSMPS_1_REQUEST_CONTROL_MASK 0x30
#define VSMPS_1_REQUEST_CONTROL_OFFSET 5
#define VSMPS_1_REQUEST_CONTROL_STOP_BIT 5
#define VSMPS_1_REQUEST_CONTROL_START_BIT 4
#define VSMPS_1_REQUEST_CONTROL_WIDTH 2

#define VSMPS_1_REQUEST_CONTROL_PARAM_MASK  0x30

//  
//  00: allows to set Vape in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vape in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vape in HP whatever VapeRequest signal value.
//  Register ReguRequestControl 0x0303, Bits 2:3, typedef request_control
//   
//  
#define VAPE_REQUEST_CONTROL_MASK 0xC
#define VAPE_REQUEST_CONTROL_OFFSET 3
#define VAPE_REQUEST_CONTROL_STOP_BIT 3
#define VAPE_REQUEST_CONTROL_START_BIT 2
#define VAPE_REQUEST_CONTROL_WIDTH 2

#define VAPE_REQUEST_CONTROL_PARAM_MASK  0xC

//  
//  00: allows to set Varm in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Varm in HP/OFF modes depending on VxRequest signal. 
//  1x: force Varm in HP whatever VarmRequest signal value.
//  Register ReguRequestControl 0x0303, Bits 0:1, typedef request_control
//   
//  
#define VARM_REQUEST_CONTROL_MASK 0x3
#define VARM_REQUEST_CONTROL_OFFSET 1
#define VARM_REQUEST_CONTROL_STOP_BIT 1
#define VARM_REQUEST_CONTROL_START_BIT 0
#define VARM_REQUEST_CONTROL_WIDTH 2

#define VARM_REQUEST_CONTROL_PARAM_MASK  0x3

//  
//  00: allows to set VextSupply1 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set VextSupply1 in HP/OFF modes depending on VxRequest signal. 
//  1x: force VextSupply1 in HP whatever VextSupply1Request signal value.
//  Register ReguRequestControl2 0x0304, Bits 6:7, typedef request_control
//   
//  
#define VEXT_SUPPLY_1_REQUEST_CONTROL_MASK 0xC0
#define VEXT_SUPPLY_1_REQUEST_CONTROL_OFFSET 7
#define VEXT_SUPPLY_1_REQUEST_CONTROL_STOP_BIT 7
#define VEXT_SUPPLY_1_REQUEST_CONTROL_START_BIT 6
#define VEXT_SUPPLY_1_REQUEST_CONTROL_WIDTH 2

#define REGU_REQUEST_CONTROL_2_REG 0x304
#define VEXT_SUPPLY_1_REQUEST_CONTROL_PARAM_MASK  0xC0

//  
//  00: allows to set Vana in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vana in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vana in HP whatever VanaRequest signal value.
//  Register ReguRequestControl2 0x0304, Bits 4:5, typedef request_control
//   
//  
#define VANA_REQUEST_CONTROL_MASK 0x30
#define VANA_REQUEST_CONTROL_OFFSET 5
#define VANA_REQUEST_CONTROL_STOP_BIT 5
#define VANA_REQUEST_CONTROL_START_BIT 4
#define VANA_REQUEST_CONTROL_WIDTH 2

#define VANA_REQUEST_CONTROL_PARAM_MASK  0x30

//  
//  00: allows to set Vpll in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vpll in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vpll in HP whatever VpllRequest signal value.
//  Register ReguRequestControl2 0x0304, Bits 2:3, typedef request_control
//   
//  
#define VPLL_REQUEST_CONTROL_MASK 0xC
#define VPLL_REQUEST_CONTROL_OFFSET 3
#define VPLL_REQUEST_CONTROL_STOP_BIT 3
#define VPLL_REQUEST_CONTROL_START_BIT 2
#define VPLL_REQUEST_CONTROL_WIDTH 2

#define VPLL_REQUEST_CONTROL_PARAM_MASK  0xC

//  
//  00: allows to set Vsmps3 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vsmps3 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vsmps3 in HP whatever Vsmps3Request signal value.
//  Register ReguRequestControl2 0x0304, Bits 0:1, typedef request_control
//   
//  
#define VSMPS_3_REQUEST_CONTROL_MASK 0x3
#define VSMPS_3_REQUEST_CONTROL_OFFSET 1
#define VSMPS_3_REQUEST_CONTROL_STOP_BIT 1
#define VSMPS_3_REQUEST_CONTROL_START_BIT 0
#define VSMPS_3_REQUEST_CONTROL_WIDTH 2

#define VSMPS_3_REQUEST_CONTROL_PARAM_MASK  0x3

//  
//  00: allows to set Vaux2 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vaux2 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vaux2 in HP whatever Vaux2Request signal value.
//  Register ReguRequestControl3 0x0305, Bits 6:7, typedef request_control
//   
//  
#define VAUX_2_REQUEST_CONTROL_MASK 0xC0
#define VAUX_2_REQUEST_CONTROL_OFFSET 7
#define VAUX_2_REQUEST_CONTROL_STOP_BIT 7
#define VAUX_2_REQUEST_CONTROL_START_BIT 6
#define VAUX_2_REQUEST_CONTROL_WIDTH 2

#define REGU_REQUEST_CONTROL_3_REG 0x305
#define VAUX_2_REQUEST_CONTROL_PARAM_MASK  0xC0

//  
//  00: allows to set Vaux1 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vaux1 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vaux1 in HP whatever Vaux1Request signal value.
//  Register ReguRequestControl3 0x0305, Bits 4:5, typedef request_control
//   
//  
#define VAUX_1_REQUEST_CONTROL_MASK 0x30
#define VAUX_1_REQUEST_CONTROL_OFFSET 5
#define VAUX_1_REQUEST_CONTROL_STOP_BIT 5
#define VAUX_1_REQUEST_CONTROL_START_BIT 4
#define VAUX_1_REQUEST_CONTROL_WIDTH 2

#define VAUX_1_REQUEST_CONTROL_PARAM_MASK  0x30

//  
//  00: allows to set VextSupply3 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set VextSupply3 in HP/OFF modes depending on VxRequest signal. 
//  1x: force VextSupply3 in HP whatever VextSupply3Request signal value.
//  Register ReguRequestControl3 0x0305, Bits 2:3, typedef request_control
//   
//  
#define VEXT_SUPPLY_3_REQUEST_CONTROL_MASK 0xC
#define VEXT_SUPPLY_3_REQUEST_CONTROL_OFFSET 3
#define VEXT_SUPPLY_3_REQUEST_CONTROL_STOP_BIT 3
#define VEXT_SUPPLY_3_REQUEST_CONTROL_START_BIT 2
#define VEXT_SUPPLY_3_REQUEST_CONTROL_WIDTH 2

#define VEXT_SUPPLY_3_REQUEST_CONTROL_PARAM_MASK  0xC

//  
//  00: allows to set VextSupply2 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set VextSupply2 in HP/OFF modes depending on VxRequest signal. 
//  1x: force VextSupply2 in HP whatever VextSupply2Request signal value.
//  Register ReguRequestControl3 0x0305, Bits 0:1, typedef request_control
//   
//  
#define VEXT_SUPPLY_2_REQUEST_CONTROL_MASK 0x3
#define VEXT_SUPPLY_2_REQUEST_CONTROL_OFFSET 1
#define VEXT_SUPPLY_2_REQUEST_CONTROL_STOP_BIT 1
#define VEXT_SUPPLY_2_REQUEST_CONTROL_START_BIT 0
#define VEXT_SUPPLY_2_REQUEST_CONTROL_WIDTH 2

#define VEXT_SUPPLY_2_REQUEST_CONTROL_PARAM_MASK  0x3

//  
//  0: Sw doesn't request supply 
//  1: Sw request supply validated by 'VxSwHPReqValid' bits.
//  Register ReguRequestControl4 0x0306, Bits 2:2, typedef software_supply_enable
//   
//  
#define SOFTWARE_SUPPLY_ENABLE_MASK 0x4
#define SOFTWARE_SUPPLY_ENABLE_OFFSET 2
#define SOFTWARE_SUPPLY_ENABLE_STOP_BIT 2
#define SOFTWARE_SUPPLY_ENABLE_START_BIT 2
#define SOFTWARE_SUPPLY_ENABLE_WIDTH 1

#define REGU_REQUEST_CONTROL_4_REG 0x306

typedef enum software_supply_enable {
    SOFTWARE_SUPPLY_DISABLE_E,
    SOFTWARE_SUPPLY_ENABLE_E
} SOFTWARE_SUPPLY_ENABLE_T ;

#define SOFTWARE_SUPPLY_ENABLE_PARAM_MASK  0x4

//  
//  00: allows to set Vaux3 in HP/LP modes depending on VxRequest signal. 
//  01: allows to set Vaux3 in HP/OFF modes depending on VxRequest signal. 
//  1x: force Vaux3 in HP whatever Vaux3Request signal value.
//  Register ReguRequestControl4 0x0306, Bits 0:1, typedef request_control
//   
//  
#define VAUX_3_REQUEST_CONTROL_MASK 0x3
#define VAUX_3_REQUEST_CONTROL_OFFSET 1
#define VAUX_3_REQUEST_CONTROL_STOP_BIT 1
#define VAUX_3_REQUEST_CONTROL_START_BIT 0
#define VAUX_3_REQUEST_CONTROL_WIDTH 2

#define VAUX_3_REQUEST_CONTROL_PARAM_MASK  0x3

/*+CAP_948_001*/
typedef enum request_valid {
    DONT_VALID_INPUT_TO_REQUEST_E,
    VALID_INPUT_TO_REQUEST_E
} REQUEST_VALID_T ;
/*-CAP_948_001*/


//  
//  0: doesn't valid SysClkReq1 input to request Vaux3 supply 
//  1: valid SysClkReq1 input to request Vaux3 supply.
//  Register SupplyControlValid 0x0307, Bits 7:7, typedef request_valid
//   
//  
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_MASK 0x80
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_OFFSET 7
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_STOP_BIT 7
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_START_BIT 7
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define SUPPLY_CONTROL_VALID_REG 0x307
#define VAUX_3_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x80
#define REQUEST_VALID_PARAM_MASK  0x80

//  
//  0: doesn't valid SysClkReq1 input to request Vaux2 supply 
//  1: valid SysClkReq1 input to request Vaux2 supply.
//  Register SupplyControlValid 0x0307, Bits 6:6, typedef request_valid
//   
//  
#define VAUX_2_SYS_CLK_REQ_1HP_VALID_MASK 0x40
#define VAUX_2_SYS_CLK_REQ_1HP_VALID_OFFSET 6
#define VAUX_2_SYS_CLK_REQ_1HP_VALID_STOP_BIT 6
#define VAUX_2_SYS_CLK_REQ_1HP_VALID_START_BIT 6
#define VAUX_2_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VAUX_2_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq1 input to request Vaux1 supply 
//  1: valid SysClkReq1 input to request Vaux1 supply.
//  Register SupplyControlValid 0x0307, Bits 5:5, typedef request_valid
//   
//  
#define VAUX_1_SYS_CLK_REQ_1HP_VALID_MASK 0x20
#define VAUX_1_SYS_CLK_REQ_1HP_VALID_OFFSET 5
#define VAUX_1_SYS_CLK_REQ_1HP_VALID_STOP_BIT 5
#define VAUX_1_SYS_CLK_REQ_1HP_VALID_START_BIT 5
#define VAUX_1_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VAUX_1_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq1 input to request Vppl supply 
//  1: valid SysClkReq1 input to request Vppl supply.
//  Register SupplyControlValid 0x0307, Bits 4:4, typedef request_valid
//   
//  
#define VPPL_SYS_CLK_REQ_1HP_VALID_MASK 0x10
#define VPPL_SYS_CLK_REQ_1HP_VALID_OFFSET 4
#define VPPL_SYS_CLK_REQ_1HP_VALID_STOP_BIT 4
#define VPPL_SYS_CLK_REQ_1HP_VALID_START_BIT 4
#define VPPL_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VPPL_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq1 input to request Vana supply 
//  1: valid SysClkReq1 input to request Vana supply.
//  Register SupplyControlValid 0x0307, Bits 3:3, typedef request_valid
//   
//  
#define VANA_SYS_CLK_REQ_1HP_VALID_MASK 0x8
#define VANA_SYS_CLK_REQ_1HP_VALID_OFFSET 3
#define VANA_SYS_CLK_REQ_1HP_VALID_STOP_BIT 3
#define VANA_SYS_CLK_REQ_1HP_VALID_START_BIT 3
#define VANA_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VANA_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq1 input to request Vsmps3 supply 
//  1: valid SysClkReq1 input to request Vsmps3 supply.
//  Register SupplyControlValid 0x0307, Bits 2:2, typedef request_valid
//   
//  
#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_MASK 0x4
#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_OFFSET 2
#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_STOP_BIT 2
#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_START_BIT 2
#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VSMPS_3_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq1 input to request Vsmps2 supply 
//  1: valid SysClkReq1 input to request Vsmps2 supply.
//  Register SupplyControlValid 0x0307, Bits 1:1, typedef request_valid
//   
//  
#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_MASK 0x2
#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_OFFSET 1
#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_STOP_BIT 1
#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_START_BIT 1
#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VSMPS_2_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid SysClkReq1 input to request Vsmps1 supply 
//  1: valid SysClkReq1 input to request Vsmps1 supply.
//  Register SupplyControlValid 0x0307, Bits 0:0, typedef request_valid
//   
//  
#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_MASK 0x1
#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_OFFSET 0
#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_STOP_BIT 0
#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_START_BIT 0
#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VSMPS_1_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x1

//  
//  0: doesn't valid SysClkReq1 input to request VextSupply3 supply 
//  1: valid SysClkReq1 input to request VextSupply3 supply.
//  Register SupplyControlValid 0x0308, Bits 6:6, typedef request_valid
//   
//  
#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_MASK 0x40
#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_OFFSET 6
#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_STOP_BIT 6
#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_START_BIT 6
#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VEXT_SUPPLY_3_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq1 input to request VextSupply2 supply 
//  1: valid SysClkReq1 input to request VextSupply2 supply.
//  Register SupplyControlValid 0x0308, Bits 5:5, typedef request_valid
//   
//  
#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_MASK 0x20
#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_OFFSET 5
#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_STOP_BIT 5
#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_START_BIT 5
#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VEXT_SUPPLY_2_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq1 input to request VextSupply1 supply 
//  1: valid SysClkReq1 input to request VextSupply1 supply.
//  Register SupplyControlValid 0x0308, Bits 4:4, typedef request_valid
//   
//  
#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_MASK 0x10
#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_OFFSET 4
#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_STOP_BIT 4
#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_START_BIT 4
#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VEXT_SUPPLY_1_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq1 input to request Vmod supply 
//  1: valid SysClkReq1 input to request Vmod supply.
//  Register SupplyControlValid 0x0308, Bits 3:3, typedef request_valid
//   
//  
#define VMOD_SYS_CLK_REQ_1HP_VALID_MASK 0x8
#define VMOD_SYS_CLK_REQ_1HP_VALID_OFFSET 3
#define VMOD_SYS_CLK_REQ_1HP_VALID_STOP_BIT 3
#define VMOD_SYS_CLK_REQ_1HP_VALID_START_BIT 3
#define VMOD_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VMOD_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq1 input to request Vbb supply 
//  1: valid SysClkReq1 input to request Vbb supply.
//  Register SupplyControlValid 0x0308, Bits 2:2, typedef request_valid
//   
//  
#define VBB_SYS_CLK_REQ_1HP_VALID_MASK 0x4
#define VBB_SYS_CLK_REQ_1HP_VALID_OFFSET 2
#define VBB_SYS_CLK_REQ_1HP_VALID_STOP_BIT 2
#define VBB_SYS_CLK_REQ_1HP_VALID_START_BIT 2
#define VBB_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VBB_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq1 input to request Varm supply 
//  1: valid SysClkReq1 input to request Varm supply.
//  Register SupplyControlValid 0x0308, Bits 1:1, typedef request_valid
//   
//  
#define VARM_SYS_CLK_REQ_1HP_VALID_MASK 0x2
#define VARM_SYS_CLK_REQ_1HP_VALID_OFFSET 1
#define VARM_SYS_CLK_REQ_1HP_VALID_STOP_BIT 1
#define VARM_SYS_CLK_REQ_1HP_VALID_START_BIT 1
#define VARM_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VARM_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid SysClkReq1 input to request Vape supply 
//  1: valid SysClkReq1 input to request Vape supply.
//  Register SupplyControlValid 0x0308, Bits 0:0, typedef request_valid
//   
//  
#define VAPE_SYS_CLK_REQ_1HP_VALID_MASK 0x1
#define VAPE_SYS_CLK_REQ_1HP_VALID_OFFSET 0
#define VAPE_SYS_CLK_REQ_1HP_VALID_STOP_BIT 0
#define VAPE_SYS_CLK_REQ_1HP_VALID_START_BIT 0
#define VAPE_SYS_CLK_REQ_1HP_VALID_WIDTH 1

#define VAPE_SYS_CLK_REQ_1HP_VALID_PARAM_MASK  0x1

/*+CAP_948_001*/
typedef enum hardware_validation {
    DONT_VALID_HARDWARE_CONFIGURATION_E,
    VALID_HARDWARE_CONFIGURATION_E
} HARDWARE_VALIDATION_T ;
/*-CAP_948_001*/



//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux3 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux3 supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 7:7, typedef hardware_validation
//   
//  
#define VAUX_3_HW_HP_REQ_1_VALID_MASK 0x80
#define VAUX_3_HW_HP_REQ_1_VALID_OFFSET 7
#define VAUX_3_HW_HP_REQ_1_VALID_STOP_BIT 7
#define VAUX_3_HW_HP_REQ_1_VALID_START_BIT 7
#define VAUX_3_HW_HP_REQ_1_VALID_WIDTH 1

#define SUPPLY_CONTROL_HARDWARE_VALID_REG 0x309
#define VAUX_3_HW_HP_REQ_1_VALID_PARAM_MASK  0x80
#define HARDWARE_VALIDATION_PARAM_MASK  0x80

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux2 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux2 supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 6:6, typedef hardware_validation
//   
//  
#define VAUX_2_HW_HP_REQ_1_VALID_MASK 0x40
#define VAUX_2_HW_HP_REQ_1_VALID_OFFSET 6
#define VAUX_2_HW_HP_REQ_1_VALID_STOP_BIT 6
#define VAUX_2_HW_HP_REQ_1_VALID_START_BIT 6
#define VAUX_2_HW_HP_REQ_1_VALID_WIDTH 1

#define VAUX_2_HW_HP_REQ_1_VALID_PARAM_MASK  0x40

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux1 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vaux1 supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 5:5, typedef hardware_validation
//   
//  
#define VAUX_1_HW_HP_REQ_1_VALID_MASK 0x20
#define VAUX_1_HW_HP_REQ_1_VALID_OFFSET 5
#define VAUX_1_HW_HP_REQ_1_VALID_STOP_BIT 5
#define VAUX_1_HW_HP_REQ_1_VALID_START_BIT 5
#define VAUX_1_HW_HP_REQ_1_VALID_WIDTH 1

#define VAUX_1_HW_HP_REQ_1_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vpll supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vpll supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 4:4, typedef hardware_validation
//   
//  
#define VPLL_HW_HP_REQ_1_VALID_MASK 0x10
#define VPLL_HW_HP_REQ_1_VALID_OFFSET 4
#define VPLL_HW_HP_REQ_1_VALID_STOP_BIT 4
#define VPLL_HW_HP_REQ_1_VALID_START_BIT 4
#define VPLL_HW_HP_REQ_1_VALID_WIDTH 1

#define VPLL_HW_HP_REQ_1_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vana supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vana supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 3:3, typedef hardware_validation
//   
//  
#define VANA_HW_HP_REQ_1_VALID_MASK 0x8
#define VANA_HW_HP_REQ_1_VALID_OFFSET 3
#define VANA_HW_HP_REQ_1_VALID_STOP_BIT 3
#define VANA_HW_HP_REQ_1_VALID_START_BIT 3
#define VANA_HW_HP_REQ_1_VALID_WIDTH 1

#define VANA_HW_HP_REQ_1_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps3 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps3 
//  supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 2:2, typedef hardware_validation
//   
//  
#define VSMPS_3_HW_HP_REQ_1_VALID_MASK 0x4
#define VSMPS_3_HW_HP_REQ_1_VALID_OFFSET 2
#define VSMPS_3_HW_HP_REQ_1_VALID_STOP_BIT 2
#define VSMPS_3_HW_HP_REQ_1_VALID_START_BIT 2
#define VSMPS_3_HW_HP_REQ_1_VALID_WIDTH 1

#define VSMPS_3_HW_HP_REQ_1_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps2 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps2 
//  supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 1:1, typedef hardware_validation
//   
//  
#define VSMPS_2_HW_HP_REQ_1_VALID_MASK 0x2
#define VSMPS_2_HW_HP_REQ_1_VALID_OFFSET 1
#define VSMPS_2_HW_HP_REQ_1_VALID_STOP_BIT 1
#define VSMPS_2_HW_HP_REQ_1_VALID_START_BIT 1
#define VSMPS_2_HW_HP_REQ_1_VALID_WIDTH 1

#define VSMPS_2_HW_HP_REQ_1_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps1 supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vsmps1 
//  supply.
//  Register SupplyControlHardwareValid 0x0309, Bits 0:1, typedef hardware_validation
//   
//  
#define VSMPS_1_HW_HP_REQ_1_VALID_MASK 0x3
#define VSMPS_1_HW_HP_REQ_1_VALID_OFFSET 1
#define VSMPS_1_HW_HP_REQ_1_VALID_STOP_BIT 1
#define VSMPS_1_HW_HP_REQ_1_VALID_START_BIT 0
#define VSMPS_1_HW_HP_REQ_1_VALID_WIDTH 2

#define VSMPS_1_HW_HP_REQ_1_VALID_PARAM_MASK  0x3

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vmod supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request Vmod supply.
//  Register SupplyControlHardwareValid2 0x030A, Bits 3:3, typedef hardware_validation
//   
//  
#define VMOD_HW_HP_REQ_1_VALID_MASK 0x8
#define VMOD_HW_HP_REQ_1_VALID_OFFSET 3
#define VMOD_HW_HP_REQ_1_VALID_STOP_BIT 3
#define VMOD_HW_HP_REQ_1_VALID_START_BIT 3
#define VMOD_HW_HP_REQ_1_VALID_WIDTH 1

#define SUPPLY_CONTROL_HARDWARE_VALID_2_REG 0x30A
#define VMOD_HW_HP_REQ_1_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply3 
//  supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply3 
//  supply.
//  Register SupplyControlHardwareValid2 0x030A, Bits 2:2, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_MASK 0x4
#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_OFFSET 2
#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_STOP_BIT 2
#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_START_BIT 2
#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_WIDTH 1

#define VEXT_SUPPLY_3_HW_HP_REQ_1_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply2 
//  supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply2 
//  supply.
//  Register SupplyControlHardwareValid2 0x030A, Bits 1:1, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_MASK 0x2
#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_OFFSET 1
#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_STOP_BIT 1
#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_START_BIT 1
#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_WIDTH 1

#define VEXT_SUPPLY_2_HW_HP_REQ_1_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply1 
//  supply 
//  1: valid HW configuration 1 (SysClkReq[8:1] inputs) to request VextSupply1 
//  supply.
//  Register SupplyControlHardwareValid2 0x030A, Bits 0:1, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_MASK 0x3
#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_OFFSET 1
#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_STOP_BIT 1
#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_START_BIT 0
#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_WIDTH 2

#define VEXT_SUPPLY_1_HW_HP_REQ_1_VALID_PARAM_MASK  0x3

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux3 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux3 supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 7:7, typedef hardware_validation
//   
//  
#define VAUX_3_HW_HP_REQ_2_VALID_MASK 0x80
#define VAUX_3_HW_HP_REQ_2_VALID_OFFSET 7
#define VAUX_3_HW_HP_REQ_2_VALID_STOP_BIT 7
#define VAUX_3_HW_HP_REQ_2_VALID_START_BIT 7
#define VAUX_3_HW_HP_REQ_2_VALID_WIDTH 1

#define VAUX_3_HW_HP_REQ_2_VALID_PARAM_MASK  0x80

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux2 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux2 supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 6:6, typedef hardware_validation
//   
//  
#define VAUX_2_HW_HP_REQ_2_VALID_MASK 0x40
#define VAUX_2_HW_HP_REQ_2_VALID_OFFSET 6
#define VAUX_2_HW_HP_REQ_2_VALID_STOP_BIT 6
#define VAUX_2_HW_HP_REQ_2_VALID_START_BIT 6
#define VAUX_2_HW_HP_REQ_2_VALID_WIDTH 1

#define VAUX_2_HW_HP_REQ_2_VALID_PARAM_MASK  0x40

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux1 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vaux1 supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 5:5, typedef hardware_validation
//   
//  
#define VAUX_1_HW_HP_REQ_2_VALID_MASK 0x20
#define VAUX_1_HW_HP_REQ_2_VALID_OFFSET 5
#define VAUX_1_HW_HP_REQ_2_VALID_STOP_BIT 5
#define VAUX_1_HW_HP_REQ_2_VALID_START_BIT 5
#define VAUX_1_HW_HP_REQ_2_VALID_WIDTH 1

#define VAUX_1_HW_HP_REQ_2_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vpll supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vpll supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 4:4, typedef hardware_validation
//   
//  
#define VPLL_HW_HP_REQ_2_VALID_MASK 0x10
#define VPLL_HW_HP_REQ_2_VALID_OFFSET 4
#define VPLL_HW_HP_REQ_2_VALID_STOP_BIT 4
#define VPLL_HW_HP_REQ_2_VALID_START_BIT 4
#define VPLL_HW_HP_REQ_2_VALID_WIDTH 1

#define VPLL_HW_HP_REQ_2_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vana supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vana supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 3:3, typedef hardware_validation
//   
//  
#define VANA_HW_HP_REQ_2_VALID_MASK 0x8
#define VANA_HW_HP_REQ_2_VALID_OFFSET 3
#define VANA_HW_HP_REQ_2_VALID_STOP_BIT 3
#define VANA_HW_HP_REQ_2_VALID_START_BIT 3
#define VANA_HW_HP_REQ_2_VALID_WIDTH 1

#define VANA_HW_HP_REQ_2_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps3 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps3 
//  supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 2:2, typedef hardware_validation
//   
//  
#define VSMPS_3_HW_HP_REQ_2_VALID_MASK 0x4
#define VSMPS_3_HW_HP_REQ_2_VALID_OFFSET 2
#define VSMPS_3_HW_HP_REQ_2_VALID_STOP_BIT 2
#define VSMPS_3_HW_HP_REQ_2_VALID_START_BIT 2
#define VSMPS_3_HW_HP_REQ_2_VALID_WIDTH 1

#define VSMPS_3_HW_HP_REQ_2_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps2 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps2 
//  supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 1:1, typedef hardware_validation
//   
//  
#define VSMPS_2_HW_HP_REQ_2_VALID_MASK 0x2
#define VSMPS_2_HW_HP_REQ_2_VALID_OFFSET 1
#define VSMPS_2_HW_HP_REQ_2_VALID_STOP_BIT 1
#define VSMPS_2_HW_HP_REQ_2_VALID_START_BIT 1
#define VSMPS_2_HW_HP_REQ_2_VALID_WIDTH 1

#define VSMPS_2_HW_HP_REQ_2_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps1 supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vsmps1 
//  supply.
//  Register SupplyControlHardwareValid 0x030B, Bits 0:1, typedef hardware_validation
//   
//  
#define VSMPS_1_HW_HP_REQ_2_VALID_MASK 0x3
#define VSMPS_1_HW_HP_REQ_2_VALID_OFFSET 1
#define VSMPS_1_HW_HP_REQ_2_VALID_STOP_BIT 1
#define VSMPS_1_HW_HP_REQ_2_VALID_START_BIT 0
#define VSMPS_1_HW_HP_REQ_2_VALID_WIDTH 2

#define VSMPS_1_HW_HP_REQ_2_VALID_PARAM_MASK  0x3

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vmod supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request Vmod supply.
//  Register SupplyControlHardwareValid2 0x030C, Bits 3:3, typedef hardware_validation
//   
//  
#define VMOD_HW_HP_REQ_2_VALID_MASK 0x8
#define VMOD_HW_HP_REQ_2_VALID_OFFSET 3
#define VMOD_HW_HP_REQ_2_VALID_STOP_BIT 3
#define VMOD_HW_HP_REQ_2_VALID_START_BIT 3
#define VMOD_HW_HP_REQ_2_VALID_WIDTH 1

#define VMOD_HW_HP_REQ_2_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply3 
//  supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply3 
//  supply.
//  Register SupplyControlHardwareValid2 0x030C, Bits 2:2, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_MASK 0x4
#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_OFFSET 2
#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_STOP_BIT 2
#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_START_BIT 2
#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_WIDTH 1

#define VEXT_SUPPLY_3_HW_HP_REQ_2_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply2 
//  supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply2 
//  supply.
//  Register SupplyControlHardwareValid2 0x030C, Bits 1:1, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_MASK 0x2
#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_OFFSET 1
#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_STOP_BIT 1
#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_START_BIT 1
#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_WIDTH 1

#define VEXT_SUPPLY_2_HW_HP_REQ_2_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply1 
//  supply 
//  1: valid HW configuration 2 (SysClkReq[8:1] inputs) to request VextSupply1 
//  supply.
//  Register SupplyControlHardwareValid2 0x030C, Bits 0:1, typedef hardware_validation
//   
//  
#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_MASK 0x3
#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_OFFSET 1
#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_STOP_BIT 1
#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_START_BIT 0
#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_WIDTH 2

#define VEXT_SUPPLY_1_HW_HP_REQ_2_VALID_PARAM_MASK  0x3

/*+CAP_948_001*/
typedef enum software_signal_validation {
    DONT_VALID_SOFTWARE_SIGNAL_E,
    VALID_SOFTWARE_SIGNAL_E
} SOFTWARE_SIGNAL_VALIDATION_T ;
/*-CAP_948_001*/



//  
//  0: doesn't valid SW to request Vaux1 supply 
//  1: valid SW to request Vaux1 supply.
//  Register SoftwareSignalValidation 0x030D, Bits 7:7, typedef 
//  software_signal_validation
//   
//  
#define VAUX_1_SW_HP_REQ_VALID_MASK 0x80
#define VAUX_1_SW_HP_REQ_VALID_OFFSET 7
#define VAUX_1_SW_HP_REQ_VALID_STOP_BIT 7
#define VAUX_1_SW_HP_REQ_VALID_START_BIT 7
#define VAUX_1_SW_HP_REQ_VALID_WIDTH 1

#define SOFTWARE_SIGNAL_VALIDATION_REG 0x30D
#define VAUX_1_SW_HP_REQ_VALID_PARAM_MASK  0x80
#define SOFTWARE_SIGNAL_VALIDATION_PARAM_MASK  0x80

//  
//  0: doesn't valid SW to request Vpll supply 
//  1: valid SW to request Vpll supply.
//  Register SoftwareSignalValidation 0x030D, Bits 6:6, typedef 
//  software_signal_validation
//   
//  
#define VPLL_SW_HP_REQ_VALID_MASK 0x40
#define VPLL_SW_HP_REQ_VALID_OFFSET 6
#define VPLL_SW_HP_REQ_VALID_STOP_BIT 6
#define VPLL_SW_HP_REQ_VALID_START_BIT 6
#define VPLL_SW_HP_REQ_VALID_WIDTH 1

#define VPLL_SW_HP_REQ_VALID_PARAM_MASK  0x40

//  
//  0: doesn't valid SW to request Vana supply 
//  1: valid SW to request Vana supply.
//  Register SoftwareSignalValidation 0x030D, Bits 5:5, typedef 
//  software_signal_validation
//   
//  
#define VANA_SW_HP_REQ_VALID_MASK 0x20
#define VANA_SW_HP_REQ_VALID_OFFSET 5
#define VANA_SW_HP_REQ_VALID_STOP_BIT 5
#define VANA_SW_HP_REQ_VALID_START_BIT 5
#define VANA_SW_HP_REQ_VALID_WIDTH 1

#define VANA_SW_HP_REQ_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid SW to request Vsmps3 supply 
//  1: valid SW to request Vsmps3 supply.
//  Register SoftwareSignalValidation 0x030D, Bits 4:4, typedef 
//  software_signal_validation
//   
//  
#define VSMPS_3_SW_HP_REQ_VALID_MASK 0x10
#define VSMPS_3_SW_HP_REQ_VALID_OFFSET 4
#define VSMPS_3_SW_HP_REQ_VALID_STOP_BIT 4
#define VSMPS_3_SW_HP_REQ_VALID_START_BIT 4
#define VSMPS_3_SW_HP_REQ_VALID_WIDTH 1

#define VSMPS_3_SW_HP_REQ_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid SW to request Vsmps2 supply 
//  1: valid SW to request Vsmps2 supply.
//  Register SoftwareSignalValidation 0x030D, Bits 3:3, typedef 
//  software_signal_validation
//   
//  
#define VSMPS_2_SW_HP_REQ_VALID_MASK 0x8
#define VSMPS_2_SW_HP_REQ_VALID_OFFSET 3
#define VSMPS_2_SW_HP_REQ_VALID_STOP_BIT 3
#define VSMPS_2_SW_HP_REQ_VALID_START_BIT 3
#define VSMPS_2_SW_HP_REQ_VALID_WIDTH 1

#define VSMPS_2_SW_HP_REQ_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid SW to request Vsmps1 supply 
//  1: valid SW to request Vsmps1 supply.
//  Register SoftwareSignalValidation 0x030D, Bits 2:2, typedef 
//  software_signal_validation
//   
//  
#define VSMPS_1_SW_HP_REQ_VALID_MASK 0x4
#define VSMPS_1_SW_HP_REQ_VALID_OFFSET 2
#define VSMPS_1_SW_HP_REQ_VALID_STOP_BIT 2
#define VSMPS_1_SW_HP_REQ_VALID_START_BIT 2
#define VSMPS_1_SW_HP_REQ_VALID_WIDTH 1

#define VSMPS_1_SW_HP_REQ_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid SW to request Varm supply 
//  1: valid SW to request Varm supply.
//  Register SoftwareSignalValidation 0x030D, Bits 1:1, typedef 
//  software_signal_validation
//   
//  
#define VARM_SW_HP_REQ_VALID_MASK 0x2
#define VARM_SW_HP_REQ_VALID_OFFSET 1
#define VARM_SW_HP_REQ_VALID_STOP_BIT 1
#define VARM_SW_HP_REQ_VALID_START_BIT 1
#define VARM_SW_HP_REQ_VALID_WIDTH 1

#define VARM_SW_HP_REQ_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid SW to request Vape supply 
//  1: valid SW to request Vape supply.
//  Register SoftwareSignalValidation 0x030D, Bits 0:0, typedef 
//  software_signal_validation
//   
//  
#define VAPE_SW_HP_REQ_VALID_MASK 0x1
#define VAPE_SW_HP_REQ_VALID_OFFSET 0
#define VAPE_SW_HP_REQ_VALID_STOP_BIT 0
#define VAPE_SW_HP_REQ_VALID_START_BIT 0
#define VAPE_SW_HP_REQ_VALID_WIDTH 1

#define VAPE_SW_HP_REQ_VALID_PARAM_MASK  0x1

//  
//  0: doesn't valid SW to request Vmod supply 
//  1: valid SW to request Vmod supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 5:5, typedef 
//  software_signal_validation
//   
//  
#define VMOD_SW_HP_REQ_VALID_MASK 0x20
#define VMOD_SW_HP_REQ_VALID_OFFSET 5
#define VMOD_SW_HP_REQ_VALID_STOP_BIT 5
#define VMOD_SW_HP_REQ_VALID_START_BIT 5
#define VMOD_SW_HP_REQ_VALID_WIDTH 1

#define SOFTWARE_SIGNAL_VALIDATION_2_REG 0x30E
#define VMOD_SW_HP_REQ_VALID_PARAM_MASK  0x20

//  
//  0: doesn't valid SW to request VextSupply3 supply 
//  1: valid SW to request VextSupply3 supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 4:4, typedef 
//  software_signal_validation
//   
//  
#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_MASK 0x10
#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_OFFSET 4
#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_STOP_BIT 4
#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_START_BIT 4
#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_WIDTH 1

#define VEXT_SUPPLY_3_SW_HP_REQ_VALID_PARAM_MASK  0x10

//  
//  0: doesn't valid SW to request VextSupply2 supply 
//  1: valid SW to request VextSupply2 supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 3:3, typedef 
//  software_signal_validation
//   
//  
#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_MASK 0x8
#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_OFFSET 3
#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_STOP_BIT 3
#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_START_BIT 3
#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_WIDTH 1

#define VEXT_SUPPLY_2_SW_HP_REQ_VALID_PARAM_MASK  0x8

//  
//  0: doesn't valid SW to request VextSupply1 supply 
//  1: valid SW to request VextSupply1 supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 2:2, typedef 
//  software_signal_validation
//   
//  
#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_MASK 0x4
#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_OFFSET 2
#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_STOP_BIT 2
#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_START_BIT 2
#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_WIDTH 1

#define VEXT_SUPPLY_1_SW_HP_REQ_VALID_PARAM_MASK  0x4

//  
//  0: doesn't valid SW to request Vaux3 supply 
//  1: valid SW to request Vaux3 supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 1:1, typedef 
//  software_signal_validation
//   
//  
#define VAUX_3_SW_HP_REQ_VALID_MASK 0x2
#define VAUX_3_SW_HP_REQ_VALID_OFFSET 1
#define VAUX_3_SW_HP_REQ_VALID_STOP_BIT 1
#define VAUX_3_SW_HP_REQ_VALID_START_BIT 1
#define VAUX_3_SW_HP_REQ_VALID_WIDTH 1

#define VAUX_3_SW_HP_REQ_VALID_PARAM_MASK  0x2

//  
//  0: doesn't valid SW to request Vaux2 supply 
//  1: valid SW to request Vaux2 supply.
//  Register SoftwareSignalValidation2 0x030E, Bits 0:0, typedef 
//  software_signal_validation
//   
//  
#define VAUX_2_SW_HP_REQ_VALID_MASK 0x1
#define VAUX_2_SW_HP_REQ_VALID_OFFSET 0
#define VAUX_2_SW_HP_REQ_VALID_STOP_BIT 0
#define VAUX_2_SW_HP_REQ_VALID_START_BIT 0
#define VAUX_2_SW_HP_REQ_VALID_WIDTH 1

#define VAUX_2_SW_HP_REQ_VALID_PARAM_MASK  0x1

/*+CAP_948_001*/
typedef enum sys_clock_request {
    DONT_VALID_SYS_CLCK_CONFIGURATION_E,
    VALID_SYS_CLCK_CONFIGURATION_E
} SYS_CLOCK_REQUEST_T ;
/*-CAP_948_001*/



//  
//  0: doesn't valid SysClkReq8 configuration 1 to request Vx supply 
//  1: valid SysClkReq8 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 7:7, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_8_VALID_1_MASK 0x80
#define SYS_CLK_REQ_8_VALID_1_OFFSET 7
#define SYS_CLK_REQ_8_VALID_1_STOP_BIT 7
#define SYS_CLK_REQ_8_VALID_1_START_BIT 7
#define SYS_CLK_REQ_8_VALID_1_WIDTH 1

#define SUPPLY_CONTROL_SYS_CLK_REQ_REG 0x30F
#define SYS_CLK_REQ_8_VALID_1_PARAM_MASK  0x80
#define SYS_CLOCK_REQUEST_PARAM_MASK  0x80

//  
//  0: doesn't valid SysClkReq7 configuration 1 to request Vx supply 
//  1: valid SysClkReq7 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 6:6, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_7_VALID_1_MASK 0x40
#define SYS_CLK_REQ_7_VALID_1_OFFSET 6
#define SYS_CLK_REQ_7_VALID_1_STOP_BIT 6
#define SYS_CLK_REQ_7_VALID_1_START_BIT 6
#define SYS_CLK_REQ_7_VALID_1_WIDTH 1

#define SYS_CLK_REQ_7_VALID_1_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq6 configuration 1 to request Vx supply 
//  1: valid SysClkReq6 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 5:5, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_6_VALID_1_MASK 0x20
#define SYS_CLK_REQ_6_VALID_1_OFFSET 5
#define SYS_CLK_REQ_6_VALID_1_STOP_BIT 5
#define SYS_CLK_REQ_6_VALID_1_START_BIT 5
#define SYS_CLK_REQ_6_VALID_1_WIDTH 1

#define SYS_CLK_REQ_6_VALID_1_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq5 configuration 1 to request Vx supply 
//  1: valid SysClkReq5 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 4:4, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_5_VALID_1_MASK 0x10
#define SYS_CLK_REQ_5_VALID_1_OFFSET 4
#define SYS_CLK_REQ_5_VALID_1_STOP_BIT 4
#define SYS_CLK_REQ_5_VALID_1_START_BIT 4
#define SYS_CLK_REQ_5_VALID_1_WIDTH 1

#define SYS_CLK_REQ_5_VALID_1_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq4 configuration 1 to request Vx supply 
//  1: valid SysClkReq4 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 3:3, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_4_VALID_1_MASK 0x8
#define SYS_CLK_REQ_4_VALID_1_OFFSET 3
#define SYS_CLK_REQ_4_VALID_1_STOP_BIT 3
#define SYS_CLK_REQ_4_VALID_1_START_BIT 3
#define SYS_CLK_REQ_4_VALID_1_WIDTH 1

#define SYS_CLK_REQ_4_VALID_1_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq3 configuration 1 to request Vx supply 
//  1: valid SysClkReq3 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 2:2, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_3_VALID_1_MASK 0x4
#define SYS_CLK_REQ_3_VALID_1_OFFSET 2
#define SYS_CLK_REQ_3_VALID_1_STOP_BIT 2
#define SYS_CLK_REQ_3_VALID_1_START_BIT 2
#define SYS_CLK_REQ_3_VALID_1_WIDTH 1

#define SYS_CLK_REQ_3_VALID_1_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq2 configuration 1 to request Vx supply 
//  1: valid SysClkReq2 configuration 1 to request Vx supply.
//  Register SupplyControlSysClkReq 0x030F, Bits 1:1, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_2_VALID_1_MASK 0x2
#define SYS_CLK_REQ_2_VALID_1_OFFSET 1
#define SYS_CLK_REQ_2_VALID_1_STOP_BIT 1
#define SYS_CLK_REQ_2_VALID_1_START_BIT 1
#define SYS_CLK_REQ_2_VALID_1_WIDTH 1

#define SYS_CLK_REQ_2_VALID_1_PARAM_MASK  0x2

/*+CAP_948_001*/
typedef enum sys_clock_request2 {
   DONT_VALID_SYS_CLCK_CONFIGURATION2_E,
    VALID_SYS_CLCK_CONFIGURATION2_E
} SYS_CLOCK_REQUEST_2_T ;
/*-CAP_948_001*/



//  
//  0: doesn't valid SysClkReq8 configuration 2 to request Vx supply 
//  1: valid SysClkReq8 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 7:7, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_8_VALID_2_MASK 0x80
#define SYS_CLK_REQ_8_VALID_2_OFFSET 7
#define SYS_CLK_REQ_8_VALID_2_STOP_BIT 7
#define SYS_CLK_REQ_8_VALID_2_START_BIT 7
#define SYS_CLK_REQ_8_VALID_2_WIDTH 1

#define SUPPLY_CONTROL_SYS_CLK_REQ_2_REG 0x310
#define SYS_CLK_REQ_8_VALID_2_PARAM_MASK  0x80

//  
//  0: doesn't valid SysClkReq7 configuration 2 to request Vx supply 
//  1: valid SysClkReq7 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 6:6, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_7_VALID_2_MASK 0x40
#define SYS_CLK_REQ_7_VALID_2_OFFSET 6
#define SYS_CLK_REQ_7_VALID_2_STOP_BIT 6
#define SYS_CLK_REQ_7_VALID_2_START_BIT 6
#define SYS_CLK_REQ_7_VALID_2_WIDTH 1

#define SYS_CLK_REQ_7_VALID_2_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq6 configuration 2 to request Vx supply 
//  1: valid SysClkReq6 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 5:5, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_6_VALID_2_MASK 0x20
#define SYS_CLK_REQ_6_VALID_2_OFFSET 5
#define SYS_CLK_REQ_6_VALID_2_STOP_BIT 5
#define SYS_CLK_REQ_6_VALID_2_START_BIT 5
#define SYS_CLK_REQ_6_VALID_2_WIDTH 1

#define SYS_CLK_REQ_6_VALID_2_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq5 configuration 2 to request Vx supply 
//  1: valid SysClkReq5 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 4:4, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_5_VALID_2_MASK 0x10
#define SYS_CLK_REQ_5_VALID_2_OFFSET 4
#define SYS_CLK_REQ_5_VALID_2_STOP_BIT 4
#define SYS_CLK_REQ_5_VALID_2_START_BIT 4
#define SYS_CLK_REQ_5_VALID_2_WIDTH 1

#define SYS_CLK_REQ_5_VALID_2_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq4 configuration 2 to request Vx supply 
//  1: valid SysClkReq4 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 3:3, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_4_VALID_2_MASK 0x8
#define SYS_CLK_REQ_4_VALID_2_OFFSET 3
#define SYS_CLK_REQ_4_VALID_2_STOP_BIT 3
#define SYS_CLK_REQ_4_VALID_2_START_BIT 3
#define SYS_CLK_REQ_4_VALID_2_WIDTH 1

#define SYS_CLK_REQ_4_VALID_2_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq3 configuration 2 to request Vx supply 
//  1: valid SysClkReq3 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 2:2, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_3_VALID_2_MASK 0x4
#define SYS_CLK_REQ_3_VALID_2_OFFSET 2
#define SYS_CLK_REQ_3_VALID_2_STOP_BIT 2
#define SYS_CLK_REQ_3_VALID_2_START_BIT 2
#define SYS_CLK_REQ_3_VALID_2_WIDTH 1

#define SYS_CLK_REQ_3_VALID_2_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq2 configuration 2 to request Vx supply 
//  1: valid SysClkReq2 configuration 2 to request Vx supply.
//  Register SupplyControlSysClkReq2 0x0310, Bits 1:1, typedef sys_clock_request
//   
//  
#define SYS_CLK_REQ_2_VALID_2_MASK 0x2
#define SYS_CLK_REQ_2_VALID_2_OFFSET 1
#define SYS_CLK_REQ_2_VALID_2_STOP_BIT 1
#define SYS_CLK_REQ_2_VALID_2_START_BIT 1
#define SYS_CLK_REQ_2_VALID_2_WIDTH 1

#define SYS_CLK_REQ_2_VALID_2_PARAM_MASK  0x2


typedef enum vout_core {
    V_CORE_1200_MV = 0x0,
    V_CORE_1225_MV = 0x1,
    V_CORE_1250_MV = 0x2,
    V_CORE_1275_MV = 0x3,
    V_CORE_1300_MV = 0x4,
    V_CORE_1325_MV = 0x5,
    V_CORE_1350_MV = 0x6
} VOUT_CORE_T ;


/*+CAP_948_001*/
typedef enum low_power_enable {
    LOW_POWER_DISABLE_E,
	LOW_POWER_ENABLE_E
} LOW_POWER_ENABLE_T ;
/*-CAP_948_001*/


//  
//  0: inactive 
//  1: enable LP mode.
//  Register ReguMiscellanous 0x0380, Bits 7:7, typedef low_power_enable
//   
//  
#define VT_VOUT_LOW_POWER_MASK 0x80
#define VT_VOUT_LOW_POWER_OFFSET 7
#define VT_VOUT_LOW_POWER_STOP_BIT 7
#define VT_VOUT_LOW_POWER_START_BIT 7
#define VT_VOUT_LOW_POWER_WIDTH 1

#define REGU_MISCELLANOUS_REG 0x380
#define VT_VOUT_LOW_POWER_PARAM_MASK  0x80
#define LOW_POWER_ENABLE_PARAM_MASK  0x80

//  
//  VintCore12LP 
//  0: inactive 
//  1: enable LP mode.
//  Register ReguMiscellanous 0x0380, Bits 6:6, typedef low_power_enable
//   
//  
#define LOW_POWER_MODE_MASK 0x40
#define LOW_POWER_MODE_OFFSET 6
#define LOW_POWER_MODE_STOP_BIT 6
#define LOW_POWER_MODE_START_BIT 6
#define LOW_POWER_MODE_WIDTH 1

#define LOW_POWER_MODE_PARAM_MASK  0x40

//  
//  select output voltage.
//  Register ReguMiscellanous 0x0380, Bits 3:5, typedef vout_core
//   
//  
#define VINT_CORE_12_SEL_MASK 0x38
#define VINT_CORE_12_SEL_OFFSET 5
#define VINT_CORE_12_SEL_STOP_BIT 5
#define VINT_CORE_12_SEL_START_BIT 3
#define VINT_CORE_12_SEL_WIDTH 3

#define VINT_CORE_12_SEL_PARAM_MASK  0x38
#define VOUT_CORE_PARAM_MASK  0x38

//  
//  VintCore12Ena 
//  0: inactive 
//  1: enable LP mode.
//  Register ReguMiscellanous 0x0380, Bits 2:2, typedef low_power_enable
//   
//  
#define VINT_CORE_12_ENA_MASK 0x4
#define VINT_CORE_12_ENA_OFFSET 2
#define VINT_CORE_12_ENA_STOP_BIT 2
#define VINT_CORE_12_ENA_START_BIT 2
#define VINT_CORE_12_ENA_WIDTH 1

#define VINT_CORE_12_ENA_PARAM_MASK  0x4

//  
//  VTVoutEna 
//  0: disable VTVout supply 
//  1: enable VTVout supply.
//  Register ReguMiscellanous 0x0380, Bits 1:1, typedef low_power_enable
//   
//  
#define VT_VOUT_ENABLE_MASK 0x2
#define VT_VOUT_ENABLE_OFFSET 1
#define VT_VOUT_ENABLE_STOP_BIT 1
#define VT_VOUT_ENABLE_START_BIT 1
#define VT_VOUT_ENABLE_WIDTH 1

#define VT_VOUT_ENABLE_PARAM_MASK  0x2

//  
//  0: disable VbusBis (5v stepup) supply 
//  1: enable Vbusbis (5v stepup) supply.
//  Register OtgSupplyControl 0x0381, Bits 3:3, typedef vbus_bis_enable
//   
//  
#define VBUS_BIS_ENABLE_MASK 0x8
#define VBUS_BIS_ENABLE_OFFSET 3
#define VBUS_BIS_ENABLE_STOP_BIT 3
#define VBUS_BIS_ENABLE_START_BIT 3
#define VBUS_BIS_ENABLE_WIDTH 1

#define OTG_SUPPLY_CONTROL_REG 0x381

typedef enum vbus_bis_enable {
    VBUS_BIS_DISABLE_E,
    VBUS_BIS_ENABLE_E
} VBUS_BIS_ENABLE_T ;

#define VBUS_BIS_ENABLE_PARAM_MASK  0x8

//  
//  RVbusPDDis 
//  0: inactive 
//  1: disable Vbus pull-down.
//  Register OtgSupplyControl 0x0381, Bits 1:1, typedef otg_supply_force_l_p_enable
//   
//  
#define OTG_SUPPLY_FORCE_LP_MASK 0x2
#define OTG_SUPPLY_FORCE_LP_OFFSET 1
#define OTG_SUPPLY_FORCE_LP_STOP_BIT 1
#define OTG_SUPPLY_FORCE_LP_START_BIT 1
#define OTG_SUPPLY_FORCE_LP_WIDTH 1


typedef enum otg_supply_force_l_p_enable {
    OTG_SUPPLY_FORCE_LP_DISABLE_E,
    OTG_SUPPLY_FORCE_LP_ENABLE_E
} OTG_SUPPLY_FORCE_LP_ENABLE_T ;

#define OTG_SUPPLY_FORCE_LP_ENABLE_PARAM_MASK  0x2

//  
//  RVbusPDDis 
//  0: inactive 
//  1: disable Vbus pull-down.
//  Register OtgSupplyControl 0x0381, Bits 0:0, typedef otg_supply_enable
//   
//  
#define OTG_SUPPLY_ENABLE_MASK 0x1
#define OTG_SUPPLY_ENABLE_OFFSET 0
#define OTG_SUPPLY_ENABLE_STOP_BIT 0
#define OTG_SUPPLY_ENABLE_START_BIT 0
#define OTG_SUPPLY_ENABLE_WIDTH 1


typedef enum otg_supply_enable {
    OTG_SUPPLY_DISABLE_E,
    OTG_SUPPLY_ENABLE_E
} OTG_SUPPLY_ENABLE_T ;

#define OTG_SUPPLY_ENABLE_PARAM_MASK  0x1

//  
//  0: Vusb regulator is not in LP mode 
//  1: Vusb regulator is set LP mode.
//  Register UsbSupplyControl 0x0382, Bits 1:1, typedef vbus_low_power_mode
//   
//  
#define VBUS_LOW_POWER_MODE_MASK 0x2
#define VBUS_LOW_POWER_MODE_OFFSET 1
#define VBUS_LOW_POWER_MODE_STOP_BIT 1
#define VBUS_LOW_POWER_MODE_START_BIT 1
#define VBUS_LOW_POWER_MODE_WIDTH 1

#define USB_SUPPLY_CONTROL_REG 0x382
#define  VBUS_LOW_POWER_MODE 2

/*+CAP_948_001*/
typedef enum vbus_low_power_mode {
     VBUS_LOWPOWER_NOT_IN_MODE_E,
	 VBUS_LOWPOWER_IN_MODE_E
} VBUS_LOW_POWER_MODE_T ;
/*-CAP_948_001*/
#define VBUS_LOW_POWER_MODE_PARAM_MASK  0x2



//  
//  VusbEna (Vusb can be enabled thru 'usb logic' or VusbEna bit) 
//  0: disable Vusb supply 
//  1: enable Vusb supply.
//  Register UsbSupplyControl 0x0382, Bits 0:0, typedef usb_supply_enable
//   
//  
#define VUSB_ENABLE_MASK 0x1
#define VUSB_ENABLE_OFFSET 0
#define VUSB_ENABLE_STOP_BIT 0
#define VUSB_ENABLE_START_BIT 0
#define VUSB_ENABLE_WIDTH 1


typedef enum usb_supply_enable {
    USB_SUPPLY_DISABLE_E,
    USB_SUPPLY_ENABLE_E
} USB_SUPPLY_ENABLE_T ;

#define USB_SUPPLY_ENABLE_PARAM_MASK  0x1

//  
//  0: disable Vamic2 supply 
//  1: enable Vamic2 supply.
//  Register AudioSupplyControl 0x0383, Bits 4:4, typedef vnegative_mic2_enable
//   
//  
#define VNEGATIVE_MIC_2_SUPPLY_MASK 0x10
#define VNEGATIVE_MIC_2_SUPPLY_OFFSET 4
#define VNEGATIVE_MIC_2_SUPPLY_STOP_BIT 4
#define VNEGATIVE_MIC_2_SUPPLY_START_BIT 4
#define VNEGATIVE_MIC_2_SUPPLY_WIDTH 1

#define AUDIO_SUPPLY_CONTROL_REG 0x383

typedef enum vnegative_mic2_enable {
    VNEGATIVE_MIC_2_DISABLE_E,
    VNEGATIVE_MIC_2_ENABLE_E
} VNEGATIVE_MIC_2_ENABLE_T ;

#define VNEGATIVE_MIC_2_ENABLE_PARAM_MASK  0x10

//  
//  0: disable Vamic1 supply 
//  1: enable Vamic1 supply.
//  Register AudioSupplyControl 0x0383, Bits 3:3, typedef analog_mic1_enable
//   
//  
#define ANALOG_MIC_1_SUPPLY_MASK 0x8
#define ANALOG_MIC_1_SUPPLY_OFFSET 3
#define ANALOG_MIC_1_SUPPLY_STOP_BIT 3
#define ANALOG_MIC_1_SUPPLY_START_BIT 3
#define ANALOG_MIC_1_SUPPLY_WIDTH 1


typedef enum analog_mic1_enable {
    ANALOG_MIC_1_DISABLE_E,
    ANALOG_MIC_1_ENABLE_E
} ANALOG_MIC_1_ENABLE_T ;

#define ANALOG_MIC_1_ENABLE_PARAM_MASK  0x8

//  
//  0: disable Vamic1 supply 
//  1: enable Vamic1 supply.
//  Register AudioSupplyControl 0x0383, Bits 2:2, typedef digital_mic_enable
//   
//  
#define DIGITAL_MIC_SUPPLY_MASK 0x4
#define DIGITAL_MIC_SUPPLY_OFFSET 2
#define DIGITAL_MIC_SUPPLY_STOP_BIT 2
#define DIGITAL_MIC_SUPPLY_START_BIT 2
#define DIGITAL_MIC_SUPPLY_WIDTH 1


typedef enum digital_mic_enable {
    DIGITAL_MIC_DISABLE_E,
    DIGITAL_MIC_ENABLE_E
} DIGITAL_MIC_ENABLE_T ;

#define DIGITAL_MIC_ENABLE_PARAM_MASK  0x4

//  
//  0: disable Vaudio supply 
//  1: enable Vaudio supply.
//  Register AudioSupplyControl 0x0383, Bits 1:1, typedef audio1_enable
//   
//  
#define AUDIO_SUPPLY_MASK 0x2
#define AUDIO_SUPPLY_OFFSET 1
#define AUDIO_SUPPLY_STOP_BIT 1
#define AUDIO_SUPPLY_START_BIT 1
#define AUDIO_SUPPLY_WIDTH 1


typedef enum audio1_enable {
    AUDIO_1_DISABLE_E,
    AUDIO_1_ENABLE_E
} AUDIO_1_ENABLE_T ;

#define AUDIO_1_ENABLE_PARAM_MASK  0x2

/*+CAP_948_001*/
typedef enum polarity {
	OUTPUT_HIGH_IMPEDANCE_E,
    OUPPUT_GROUNDED_E
} POLARITY_T ;
/*-CAP_948_001*/



//  
//  0: output in high impedance when Vamic2 is disabled 
//  1: output is grounded when Vamic2 is disabled.
//  Register RegulatorControl1 0x0384, Bits 1:1, typedef polarity
//   
//  
#define VAMIC_2_POLARITY_MASK 0x2
#define VAMIC_2_POLARITY_OFFSET 1
#define VAMIC_2_POLARITY_STOP_BIT 1
#define VAMIC_2_POLARITY_START_BIT 1
#define VAMIC_2_POLARITY_WIDTH 1

#define REGULATOR_CONTROL_1_REG 0x384
#define VAMIC_2_POLARITY_PARAM_MASK  0x2
#define POLARITY_PARAM_MASK  0x2

//  
//  0: output in high impedance when Vamic1 is disabled 
//  1: output is grounded when Vamic1 is disabled.
//  Register RegulatorControl1 0x0384, Bits 0:0, typedef polarity
//   
//  
#define VAMIC_1_POLARITY_MASK 0x1
#define VAMIC_1_POLARITY_OFFSET 0
#define VAMIC_1_POLARITY_STOP_BIT 0
#define VAMIC_1_POLARITY_START_BIT 0
#define VAMIC_1_POLARITY_WIDTH 1

#define VAMIC_1_POLARITY_PARAM_MASK  0x1
#endif
