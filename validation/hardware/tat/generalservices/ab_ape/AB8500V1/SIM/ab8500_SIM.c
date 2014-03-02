/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_SIM/Linux/ab8500_SIM.c
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
#include "ab8500_SIM.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_power_down_procedure
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 7:7
 *
 **************************************************************************/
unsigned char set_power_down_procedure( enum powe_down_procedure_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: If ATRErrInt is generated, power down procedure is 
     * activated (card powered down) 
     * 1: If ATRErrInt is generated, power down procedure is NOT 
     * activated 
     */ 
    switch( param )
      {
        case POWE_DOWN_PROCEDURE_DISABLE_E: 
           value = old_value | POWE_DOWN_PROCEDURE_ENABLE_PARAM_MASK; 
           break;
        case POWE_DOWN_PROCEDURE_ENABLE_E: 
           value = old_value & ~ POWE_DOWN_PROCEDURE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_power_down_procedure
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 7:7
 *
 **************************************************************************/
unsigned char get_power_down_procedure()
  {
    unsigned char value;


    /* 
     * 0: If ATRErrInt is generated, power down procedure is 
     * activated (card powered down) 
     * 1: If ATRErrInt is generated, power down procedure is NOT 
     * activated 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & POWE_DOWN_PROCEDURE_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_reset_duration
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 6:6
 *
 **************************************************************************/
unsigned char set_reset_duration( enum reset_duration param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: SIM reset active 400 card clock cycles after the clock 
     * is applied 
     * 1: SIM reset active 40000 card clock cycles after the 
     * clock is applied 
     */ 
    switch( param )
      {
        case ACTIVE_400_CARD_CLOCK_E: 
           value = old_value | RESET_DURATION_PARAM_MASK; 
           break;
        case ACTIVE_40000_CARD_CLOCK_E: 
           value = old_value & ~ RESET_DURATION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_reset_duration
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 6:6
 *
 **************************************************************************/
unsigned char get_reset_duration()
  {
    unsigned char value;


    /* 
     * 0: SIM reset active 400 card clock cycles after the clock 
     * is applied 
     * 1: SIM reset active 40000 card clock cycles after the 
     * clock is applied 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & RESET_DURATION_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_warm_reset
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 5:5
 *
 **************************************************************************/
unsigned char set_warm_reset( enum warm_reset_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: Warm reset of the card is NOT active 
     * 1: Warm reset of the card active 
     */ 
    switch( param )
      {
        case WARM_RESET_E: 
           value = old_value | WARM_RESET_ENABLE_PARAM_MASK; 
           break;
        case WARM_UN_RESET_E: 
           value = old_value & ~ WARM_RESET_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_warm_reset
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 5:5
 *
 **************************************************************************/
unsigned char get_warm_reset()
  {
    unsigned char value;


    /* 
     * 0: Warm reset of the card is NOT active 
     * 1: Warm reset of the card active 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & WARM_RESET_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_internally_reset
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 4:4
 *
 **************************************************************************/
unsigned char set_internally_reset( enum internally_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * An externally reset card 
     * 1: An internally reset card 
     */ 
    switch( param )
      {
        case EXTERNALLY_RESET_CARD_E: 
           value = old_value | INTERNALLY_RESET_PARAM_MASK; 
           break;
        case INTERNALLY_RESET_CARD_E: 
           value = old_value & ~ INTERNALLY_RESET_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_internally_reset
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 4:4
 *
 **************************************************************************/
unsigned char get_internally_reset()
  {
    unsigned char value;


    /* 
     * An externally reset card 
     * 1: An internally reset card 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & INTERNALLY_RESET_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_power_on
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 3:3
 *
 **************************************************************************/
unsigned char set_sim_power_on( enum sim_power_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: SIM power off by MCU 
     * 1: SIM power on by MCU 
     */ 
    switch( param )
      {
        case SIM_POWER_UP_E: 
           value = old_value | SIM_POWER_ON_PARAM_MASK; 
           break;
        case SIM_POWER_DOWN_E: 
           value = old_value & ~ SIM_POWER_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_power_on
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 3:3
 *
 **************************************************************************/
unsigned char get_sim_power_on()
  {
    unsigned char value;


    /* 
     * 0: SIM power off by MCU 
     * 1: SIM power on by MCU 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & SIM_POWER_ON_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_clock32_selected
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 2:2
 *
 **************************************************************************/
unsigned char set_clock32_selected( enum clock32_selected param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: Clk32k not selected to counter 
     * 1: Clk32k selected to counter 
     */ 
    switch( param )
      {
        case CLOCK_32_SELECT_E: 
           value = old_value | CLOCK_32_SELECTED_PARAM_MASK; 
           break;
        case CLOCK_32_UN_SELECT_E: 
           value = old_value & ~ CLOCK_32_SELECTED_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_clock32_selected
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 2:2
 *
 **************************************************************************/
unsigned char get_clock32_selected()
  {
    unsigned char value;


    /* 
     * 0: Clk32k not selected to counter 
     * 1: Clk32k selected to counter 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & CLOCK_32_SELECTED_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_card_clock_selected
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 1:1
 *
 **************************************************************************/
unsigned char set_card_clock_selected( enum card_clock_selected param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: SIMIfClk/2 not selected to counter 
     * 1: SIMIfClk/2 selected to counter 
     */ 
    switch( param )
      {
        case CARD_CLOCK_SELECT_E: 
           value = old_value | CARD_CLOCK_SELECTED_PARAM_MASK; 
           break;
        case CARD_CLOCK_UN_SELECT_E: 
           value = old_value & ~ CARD_CLOCK_SELECTED_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_card_clock_selected
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 1:1
 *
 **************************************************************************/
unsigned char get_card_clock_selected()
  {
    unsigned char value;


    /* 
     * 0: SIMIfClk/2 not selected to counter 
     * 1: SIMIfClk/2 selected to counter 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & CARD_CLOCK_SELECTED_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_counter_reset
 *
 * IN   : param, a value to write to the regiter SimInterfaceControl
 * OUT  : 
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 0:0
 *
 **************************************************************************/
unsigned char set_counter_reset( enum counter_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_INTERFACE_CONTROL_REG);

    /* 
     * 0: Counter reset inactive 
     * 1: Counter reset active 
     */ 
    switch( param )
      {
        case COUNTER_RESET_E: 
           value = old_value | COUNTER_RESET_PARAM_MASK; 
           break;
        case COUNTER_UN_RESET_E: 
           value = old_value & ~ COUNTER_RESET_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_INTERFACE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_counter_reset
 *
 * RET  : Return the value of register SimInterfaceControl
 *
 * Notes : From register 0x0480, bits 0:0
 *
 **************************************************************************/
unsigned char get_counter_reset()
  {
    unsigned char value;


    /* 
     * 0: Counter reset inactive 
     * 1: Counter reset active 
     */ 
    value = (SPIRead(SIM_INTERFACE_CONTROL_REG) & COUNTER_RESET_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_card_detection_timeout
 *
 * IN   : param, a value to write to the regiter CardDetectionTimeout
 * OUT  : 
 *
 * RET  : Return the value of register CardDetectionTimeout
 *
 * Notes : From register 0x0481, bits 7:0
 *
 **************************************************************************/
unsigned char set_card_detection_timeout( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(CARD_DETECTION_TIMEOUT_REG);

    /* 
     * If the card detection counter reaches this compare value, 
     * the SIM card absence information is updated based on the 
     * value of SIMCardDet signal. So this counter filters 
     * glitches away from SIMCardDet signal 
     */ 
    SPIWrite(CARD_DETECTION_TIMEOUT_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_card_detection_timeout
 *
 * RET  : Return the value of register CardDetectionTimeout
 *
 * Notes : From register 0x0481, bits 7:0
 *
 **************************************************************************/
unsigned char get_card_detection_timeout()
  {
    unsigned char value;


    /* 
     * If the card detection counter reaches this compare value, 
     * the SIM card absence information is updated based on the 
     * value of SIMCardDet signal. So this counter filters 
     * glitches away from SIMCardDet signal 
     */ 
    value = SPIRead(CARD_DETECTION_TIMEOUT_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_comp_value_low_byte
 *
 * IN   : param, a value to write to the regiter CompValueLowByte
 * OUT  : 
 *
 * RET  : Return the value of register CompValueLowByte
 *
 * Notes : From register 0x0482, bits 7:0
 *
 **************************************************************************/
unsigned char set_comp_value_low_byte( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(COMP_VALUE_LOW_BYTE_REG);

    /* 
     * ATR error is generated, if the counter reaches this 
     */ 
    SPIWrite(COMP_VALUE_LOW_BYTE_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_comp_value_low_byte
 *
 * RET  : Return the value of register CompValueLowByte
 *
 * Notes : From register 0x0482, bits 7:0
 *
 **************************************************************************/
unsigned char get_comp_value_low_byte()
  {
    unsigned char value;


    /* 
     * ATR error is generated, if the counter reaches this 
     */ 
    value = SPIRead(COMP_VALUE_LOW_BYTE_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_comp_value_high_byte
 *
 * IN   : param, a value to write to the regiter CompValueHighByte
 * OUT  : 
 *
 * RET  : Return the value of register CompValueHighByte
 *
 * Notes : From register 0x0483, bits 7:0
 *
 **************************************************************************/
unsigned char set_comp_value_high_byte( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(COMP_VALUE_HIGH_BYTE_REG);

    /* 
     * ATR error is generated, if the counter reaches this 
     */ 
    SPIWrite(COMP_VALUE_HIGH_BYTE_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_comp_value_high_byte
 *
 * RET  : Return the value of register CompValueHighByte
 *
 * Notes : From register 0x0483, bits 7:0
 *
 **************************************************************************/
unsigned char get_comp_value_high_byte()
  {
    unsigned char value;


    /* 
     * ATR error is generated, if the counter reaches this 
     */ 
    value = SPIRead(COMP_VALUE_HIGH_BYTE_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_active_mode
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 7:7
 *
 **************************************************************************/
unsigned char set_vsim_active_mode( enum active_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: VSIM controlled by SleepX (if VSIM is on) 
     * 1: VSIM always in active mode (if VSIM is on) 
     */ 
    switch( param )
      {
        case VSIM_CONTROLLED_BY_SLEEPX_E: 
           value = old_value | ACTIVE_MODE_PARAM_MASK; 
           break;
        case VSIM_ALWAYS_ACTIVE_MODE_E: 
           value = old_value & ~ ACTIVE_MODE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_active_mode
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 7:7
 *
 **************************************************************************/
unsigned char get_vsim_active_mode()
  {
    unsigned char value;


    /* 
     * 0: VSIM controlled by SleepX (if VSIM is on) 
     * 1: VSIM always in active mode (if VSIM is on) 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & ACTIVE_MODE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_da_pull_up
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 6:6
 *
 **************************************************************************/
unsigned char set_sim_da_pull_up( enum pull_up param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: SIMDa pull up is R1 only 
     * 1: SIMDa pull up is R1 + R2 
     */ 
    switch( param )
      {
        case BY_R1_ONLY_E: 
           value = old_value | PULL_UP_PARAM_MASK; 
           break;
        case BY_R1_PLUS_R2_E: 
           value = old_value & ~ PULL_UP_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_da_pull_up
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 6:6
 *
 **************************************************************************/
unsigned char get_sim_da_pull_up()
  {
    unsigned char value;


    /* 
     * 0: SIMDa pull up is R1 only 
     * 1: SIMDa pull up is R1 + R2 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & PULL_UP_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_comp_detection
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 5:5
 *
 **************************************************************************/
unsigned char set_sim_comp_detection( enum comparator_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: SIMCardDetX comparator disabled 
     * 1: SIMCardDetX comparator enabled 
     */ 
    switch( param )
      {
        case COMPARATOR_ENABLE_E: 
           value = old_value | COMPARATOR_ENABLED_PARAM_MASK; 
           break;
        case COMPARATOR_DISABLE_E: 
           value = old_value & ~ COMPARATOR_ENABLED_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_comp_detection
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 5:5
 *
 **************************************************************************/
unsigned char get_sim_comp_detection()
  {
    unsigned char value;


    /* 
     * 0: SIMCardDetX comparator disabled 
     * 1: SIMCardDetX comparator enabled 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & COMPARATOR_ENABLED_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_removal_detection
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 4:4
 *
 **************************************************************************/
unsigned char set_removal_detection( enum removal_detection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: SIMIf detects removal from BatCtrl only 
     * 1: SIMIf detects removal from SIMCarDetX also 
     */ 
    switch( param )
      {
        case REMOVAL_FROM_BATCTRL_ONLY_E: 
           value = old_value | REMOVAL_DETECTION_PARAM_MASK; 
           break;
        case REMOVAL_FROM_SIMCARDETX_ALSO_E: 
           value = old_value & ~ REMOVAL_DETECTION_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_removal_detection
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 4:4
 *
 **************************************************************************/
unsigned char get_removal_detection()
  {
    unsigned char value;


    /* 
     * 0: SIMIf detects removal from BatCtrl only 
     * 1: SIMIf detects removal from SIMCarDetX also 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & REMOVAL_DETECTION_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_regulator
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 3:3
 *
 **************************************************************************/
unsigned char set_vsim_regulator( enum vsim_regulator param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: VSIM regulator follows start-up order 
     * 1: VSIM regulator always on 
     */ 
    switch( param )
      {
        case REGULATOR_FOLLOWS_START_UP_ORDER_E: 
           value = old_value | VSIM_REGULATOR_PARAM_MASK; 
           break;
        case REGULATOR_ALWAYS_ON_E: 
           value = old_value & ~ VSIM_REGULATOR_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_regulator
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 3:3
 *
 **************************************************************************/
unsigned char get_vsim_regulator()
  {
    unsigned char value;


    /* 
     * 0: VSIM regulator follows start-up order 
     * 1: VSIM regulator always on 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & VSIM_REGULATOR_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vsim_voltage
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 2:1
 *
 **************************************************************************/
unsigned char set_vsim_voltage( enum vsim_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 00: VSIM1 voltage level 1.2V 
     * 01: VSIM1 voltage level 1.8V 
     * 10: VSIM1 voltage level 3.0V 
     */ 
  

     value =  old_value & ~VSIM_VOLTAGE_PARAM_MASK ;

    switch(  param )
      {
        case LEVEL_1_POINT_2V_E:
            value  = value  | (LEVEL_1_POINT_2V << 0x1);
           break;
        case LEVEL_1_POINT_8V_E:
            value  = value  | (LEVEL_1_POINT_8V << 0x1);
           break;
        case LEVEL_3V_E:
            value  = value  | (LEVEL_3V << 0x1);
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vsim_voltage
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 2:1
 *
 **************************************************************************/
unsigned char get_vsim_voltage()
  {
    unsigned char value;


    /* 
     * 00: VSIM1 voltage level 1.2V 
     * 01: VSIM1 voltage level 1.8V 
     * 10: VSIM1 voltage level 3.0V 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & VSIM_VOLTAGE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_io_normal
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 0:0
 *
 **************************************************************************/
unsigned char set_sim_io_normal( enum sim_io_normal param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_REG);

    /* 
     * 0: SIM interface in normal use 
     * 1: ISO-UICC disabled, SIM IOs gen. purpose outputs 
     */ 
    switch( param )
      {
        case SIM_INTERFACE_NORMAL_USE_E: 
           value = old_value | SIM_IO_NORMAL_PARAM_MASK; 
           break;
        case GENERAL_PURPOSE_OUTPUT_E: 
           value = old_value & ~ SIM_IO_NORMAL_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_io_normal
 *
 * RET  : Return the value of register SimSupplyManagement
 *
 * Notes : From register 0x0484, bits 0:0
 *
 **************************************************************************/
unsigned char get_sim_io_normal()
  {
    unsigned char value;


    /* 
     * 0: SIM interface in normal use 
     * 1: ISO-UICC disabled, SIM IOs gen. purpose outputs 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_REG) & SIM_IO_NORMAL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_select_vsim_power
 *
 * IN   : param, a value to write to the regiter SimSupplyManagement2
 * OUT  : 
 *
 * RET  : Return the value of register SimSupplyManagement2
 *
 * Notes : From register 0x0485, bits 0:0
 *
 **************************************************************************/
unsigned char set_select_vsim_power( enum select_vsim_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SUPPLY_MANAGEMENT_2_REG);

    /* 
     * 0: Select VbatVsim ball to supply Vsim LDO 
     * 1: Select VinVsim ball to supply Vsim LDO 
     */ 
    switch( param )
      {
        case SELECT_VBAT_E: 
           value = old_value | SELECT_VSIM_POWER_PARAM_MASK; 
           break;
        case SELECT_VIN_E: 
           value = old_value & ~ SELECT_VSIM_POWER_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_SUPPLY_MANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_select_vsim_power
 *
 * RET  : Return the value of register SimSupplyManagement2
 *
 * Notes : From register 0x0485, bits 0:0
 *
 **************************************************************************/
unsigned char get_select_vsim_power()
  {
    unsigned char value;


    /* 
     * 0: Select VbatVsim ball to supply Vsim LDO 
     * 1: Select VinVsim ball to supply Vsim LDO 
     */ 
    value = (SPIRead(SIM_SUPPLY_MANAGEMENT_2_REG) & SELECT_VSIM_POWER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_source_interrupts
 *
 * IN   : param, a value to write to the regiter SimSourceInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 7:0
 *
 **************************************************************************/
unsigned char set_sim_source_interrupts( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(SIM_SOURCE_INTERRUPTS_REG);

    /* 
     * SimSourceInterrupts 
     */ 
    SPIWrite(SIM_SOURCE_INTERRUPTS_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sim_source_interrupts
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 7:0
 *
 **************************************************************************/
unsigned char get_sim_source_interrupts()
  {
    unsigned char value;


    /* 
     * SimSourceInterrupts 
     */ 
    value = SPIRead(SIM_SOURCE_INTERRUPTS_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_intr_tx_error
 *
 * IN   : param, a value to write to the regiter SimSourceInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 3:3
 *
 **************************************************************************/
unsigned char set_intr_tx_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SOURCE_INTERRUPTS_REG);

    /* 
     * Tx Error Interrupt 
     */ 
    value =  old_value & ~INTR_TX_ERROR_PARAM_MASK;


     param |= (value << 0x3);  
    SPIWrite(SIM_SOURCE_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_intr_tx_error
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 3:3
 *
 **************************************************************************/
unsigned char get_intr_tx_error()
  {
    unsigned char value;


    /* 
     * Tx Error Interrupt 
     */ 
    value = (SPIRead(SIM_SOURCE_INTERRUPTS_REG) & INTR_TX_ERROR_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_intr_atr_error
 *
 * IN   : param, a value to write to the regiter SimSourceInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 2:2
 *
 **************************************************************************/
unsigned char set_intr_atr_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SOURCE_INTERRUPTS_REG);

    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value =  old_value & ~INTR_ATR_ERROR_PARAM_MASK;


     param |= (value << 0x2);  
    SPIWrite(SIM_SOURCE_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_intr_atr_error
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 2:2
 *
 **************************************************************************/
unsigned char get_intr_atr_error()
  {
    unsigned char value;


    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value = (SPIRead(SIM_SOURCE_INTERRUPTS_REG) & INTR_ATR_ERROR_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_intr_card_detected
 *
 * IN   : param, a value to write to the regiter SimSourceInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 1:1
 *
 **************************************************************************/
unsigned char set_intr_card_detected( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_SOURCE_INTERRUPTS_REG);

    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value =  old_value & ~INTR_CARD_DETECTED_PARAM_MASK;


     param |= (value << 0x1);  
    SPIWrite(SIM_SOURCE_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_intr_card_detected
 *
 * RET  : Return the value of register SimSourceInterrupts
 *
 * Notes : From register 0x0486, bits 1:1
 *
 **************************************************************************/
unsigned char get_intr_card_detected()
  {
    unsigned char value;


    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value = (SPIRead(SIM_SOURCE_INTERRUPTS_REG) & INTR_CARD_DETECTED_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_latch_interrupts
 *
 * IN   : param, a value to write to the regiter SimLatchInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 7:0
 *
 **************************************************************************/
unsigned char set_sim_latch_interrupts( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(SIM_LATCH_INTERRUPTS_REG);

    /* 
     * SimLatchInterrupts 
     */ 
    SPIWrite(SIM_LATCH_INTERRUPTS_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sim_latch_interrupts
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 7:0
 *
 **************************************************************************/
unsigned char get_sim_latch_interrupts()
  {
    unsigned char value;


    /* 
     * SimLatchInterrupts 
     */ 
    value = SPIRead(SIM_LATCH_INTERRUPTS_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_latch_tx_error
 *
 * IN   : param, a value to write to the regiter SimLatchInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 3:3
 *
 **************************************************************************/
unsigned char set_latch_tx_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_LATCH_INTERRUPTS_REG);

    /* 
     * Tx Error Interrupt 
     */ 
    value =  old_value & ~LATCH_TX_ERROR_PARAM_MASK;


     param |= (value << 0x3);  
    SPIWrite(SIM_LATCH_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_latch_tx_error
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 3:3
 *
 **************************************************************************/
unsigned char get_latch_tx_error()
  {
    unsigned char value;


    /* 
     * Tx Error Interrupt 
     */ 
    value = (SPIRead(SIM_LATCH_INTERRUPTS_REG) & LATCH_TX_ERROR_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_latch_atr_error
 *
 * IN   : param, a value to write to the regiter SimLatchInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 2:2
 *
 **************************************************************************/
unsigned char set_latch_atr_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_LATCH_INTERRUPTS_REG);

    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value =  old_value & ~LATCH_ATR_ERROR_PARAM_MASK;


     param |= (value << 0x2);  
    SPIWrite(SIM_LATCH_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_latch_atr_error
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 2:2
 *
 **************************************************************************/
unsigned char get_latch_atr_error()
  {
    unsigned char value;


    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value = (SPIRead(SIM_LATCH_INTERRUPTS_REG) & LATCH_ATR_ERROR_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_latch_card_detected
 *
 * IN   : param, a value to write to the regiter SimLatchInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 1:1
 *
 **************************************************************************/
unsigned char set_latch_card_detected( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_LATCH_INTERRUPTS_REG);

    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value =  old_value & ~LATCH_CARD_DETECTED_PARAM_MASK;


     param |= (value << 0x1);  
    SPIWrite(SIM_LATCH_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_latch_card_detected
 *
 * RET  : Return the value of register SimLatchInterrupts
 *
 * Notes : From register 0x0487, bits 1:1
 *
 **************************************************************************/
unsigned char get_latch_card_detected()
  {
    unsigned char value;


    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value = (SPIRead(SIM_LATCH_INTERRUPTS_REG) & LATCH_CARD_DETECTED_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_mask_interrupts
 *
 * IN   : param, a value to write to the regiter SimMaskInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 7:0
 *
 **************************************************************************/
unsigned char set_sim_mask_interrupts( unsigned char param )
  {
    unsigned char value;

    value = SPIRead(SIM_MASK_INTERRUPTS_REG);

    /* 
     * SimMaskInterrupts 
     */ 
    SPIWrite(SIM_MASK_INTERRUPTS_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_sim_mask_interrupts
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 7:0
 *
 **************************************************************************/
unsigned char get_sim_mask_interrupts()
  {
    unsigned char value;


    /* 
     * SimMaskInterrupts 
     */ 
    value = SPIRead(SIM_MASK_INTERRUPTS_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mask_tx_error
 *
 * IN   : param, a value to write to the regiter SimMaskInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 3:3
 *
 **************************************************************************/
unsigned char set_mask_tx_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_MASK_INTERRUPTS_REG);

    /* 
     * Tx Error Interrupt 
     */ 
    value =  old_value & ~MASK_TX_ERROR_PARAM_MASK;


     param |= (value << 0x3);  
    SPIWrite(SIM_MASK_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mask_tx_error
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 3:3
 *
 **************************************************************************/
unsigned char get_mask_tx_error()
  {
    unsigned char value;


    /* 
     * Tx Error Interrupt 
     */ 
    value = (SPIRead(SIM_MASK_INTERRUPTS_REG) & MASK_TX_ERROR_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mask_atr_error
 *
 * IN   : param, a value to write to the regiter SimMaskInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 2:2
 *
 **************************************************************************/
unsigned char set_mask_atr_error( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_MASK_INTERRUPTS_REG);

    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value =  old_value & ~MASK_ATR_ERROR_PARAM_MASK;


     param |= (value << 0x2);  
    SPIWrite(SIM_MASK_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mask_atr_error
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 2:2
 *
 **************************************************************************/
unsigned char get_mask_atr_error()
  {
    unsigned char value;


    /* 
     * 1: ISO-UICC SIM ATR(Answer to Reset) Error Interrupt 
     * pending (Activates if no ATR received 
     */ 
    value = (SPIRead(SIM_MASK_INTERRUPTS_REG) & MASK_ATR_ERROR_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mask_card_detected
 *
 * IN   : param, a value to write to the regiter SimMaskInterrupts
 * OUT  : 
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 1:1
 *
 **************************************************************************/
unsigned char set_mask_card_detected( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_MASK_INTERRUPTS_REG);

    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value =  old_value & ~MASK_CARD_DETECTED_PARAM_MASK;


     param |= (value << 0x1);  
    SPIWrite(SIM_MASK_INTERRUPTS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mask_card_detected
 *
 * RET  : Return the value of register SimMaskInterrupts
 *
 * Notes : From register 0x0488, bits 1:1
 *
 **************************************************************************/
unsigned char get_mask_card_detected()
  {
    unsigned char value;


    /* 
     * 1: SIM Card Detected Interrupt pending (Activates at both 
     * edges) 
     */ 
    value = (SPIRead(SIM_MASK_INTERRUPTS_REG) & MASK_CARD_DETECTED_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_weak_pull_down
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 5:5
 *
 **************************************************************************/
unsigned char set_weak_pull_down( enum weak_pull_down_active param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: USB-UICC data lines weak pull down active 
     * 1: USB-UICC data lines weak pull down not active 
     */ 
    switch( param )
      {
        case WEAK_PULL_DOWN_ACTIVE_E: 
           value = old_value | WEAK_PULL_DOWN_ACTIVE_PARAM_MASK; 
           break;
        case WEAK_PULL_DOWN_NOT_ACTIVE_E: 
           value = old_value & ~ WEAK_PULL_DOWN_ACTIVE_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_weak_pull_down
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 5:5
 *
 **************************************************************************/
unsigned char get_weak_pull_down()
  {
    unsigned char value;


    /* 
     * 0: USB-UICC data lines weak pull down active 
     * 1: USB-UICC data lines weak pull down not active 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & WEAK_PULL_DOWN_ACTIVE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_reset_value
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 4:4
 *
 **************************************************************************/
unsigned char set_sim_reset_value( enum pad_driven param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: CIsoReset pad driven low if Simrst_force=1 
     * 1: CIsoReset pad driven high if Simrst_force 
     */ 
    switch( param )
      {
        case RESET_DRIVEN_LOW_E: 
           value = old_value | PAD_DRIVEN_PARAM_MASK; 
           break;
        case RESET_DRIVEN_HIGH_E: 
           value = old_value & ~ PAD_DRIVEN_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_reset_value
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 4:4
 *
 **************************************************************************/
unsigned char get_sim_reset_value()
  {
    unsigned char value;


    /* 
     * 0: CIsoReset pad driven low if Simrst_force=1 
     * 1: CIsoReset pad driven high if Simrst_force 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & PAD_DRIVEN_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usim_by_pass
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 3:3
 *
 **************************************************************************/
unsigned char set_usim_by_pass( enum usim_by_pass param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: USB SIM in normal use 
     * 1: All decoding logic and single-ended zero filter 
     * bypassed. Only data drivers in use. 
     */ 
    switch( param )
      {
        case ALL_DECODING_LOGIG_BYPASSED_E: 
           value = old_value | USIM_BY_PASS_PARAM_MASK; 
           break;
        case NORMAL_USE_E: 
           value = old_value & ~ USIM_BY_PASS_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usim_by_pass
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 3:3
 *
 **************************************************************************/
unsigned char get_usim_by_pass()
  {
    unsigned char value;


    /* 
     * 0: USB SIM in normal use 
     * 1: All decoding logic and single-ended zero filter 
     * bypassed. Only data drivers in use. 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & USIM_BY_PASS_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_reset_control
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 2:2
 *
 **************************************************************************/
unsigned char set_sim_reset_control( enum sim_reset_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: CSIMRST controlled by traditional SIM IF 
     * 1: CSIMRST pad driven high 
     */ 
    switch( param )
      {
        case CONTROLLED_BY_SIM_IF_E: 
           value = old_value | SIM_RESET_CONTROL_PARAM_MASK; 
           break;
        case DRIVEN_HIGH_E: 
           value = old_value & ~ SIM_RESET_CONTROL_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_reset_control
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 2:2
 *
 **************************************************************************/
unsigned char get_sim_reset_control()
  {
    unsigned char value;


    /* 
     * 0: CSIMRST controlled by traditional SIM IF 
     * 1: CSIMRST pad driven high 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & SIM_RESET_CONTROL_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usim_load
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 1:1
 *
 **************************************************************************/
unsigned char set_usim_load( enum usim_load param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: Data line load < 21pF 
     * 1: Data line load 21-30pF 
     */ 
    switch( param )
      {
        case UNDER_21_PF_E: 
           value = old_value | USIM_LOAD_PARAM_MASK; 
           break;
        case ABOVE_21_PF_E: 
           value = old_value & ~ USIM_LOAD_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usim_load
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 1:1
 *
 **************************************************************************/
unsigned char get_usim_load()
  {
    unsigned char value;


    /* 
     * 0: Data line load < 21pF 
     * 1: Data line load 21-30pF 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & USIM_LOAD_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usim_power
 *
 * IN   : param, a value to write to the regiter SimIfContol
 * OUT  : 
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 0:0
 *
 **************************************************************************/
unsigned char set_usim_power( enum usim_power_on param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_IF_CONTOL_REG);

    /* 
     * 0: USB SIM not in use. (VSIM powered down, weak pull-down 
     * activated and strong pull-down 
     * activated when VSIM voltage low) 
     * 1: USB SIM powered up (VSIM Powered up, all put-downs 
     * released 
     */ 
    switch( param )
      {
        case USIM_POWER_UP_E: 
           value = old_value | USIM_POWER_ON_PARAM_MASK; 
           break;
        case USIM_POWER_DOWN_E: 
           value = old_value & ~ USIM_POWER_ON_PARAM_MASK;
           break;
      }
  

    SPIWrite(SIM_IF_CONTOL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usim_power
 *
 * RET  : Return the value of register SimIfContol
 *
 * Notes : From register 0x0489, bits 0:0
 *
 **************************************************************************/
unsigned char get_usim_power()
  {
    unsigned char value;


    /* 
     * 0: USB SIM not in use. (VSIM powered down, weak pull-down 
     * activated and strong pull-down 
     * activated when VSIM voltage low) 
     * 1: USB SIM powered up (VSIM Powered up, all put-downs 
     * released 
     */ 
    value = (SPIRead(SIM_IF_CONTOL_REG) & USIM_POWER_ON_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sim_delay
 *
 * IN   : param, a value to write to the regiter SimDelay
 * OUT  : 
 *
 * RET  : Return the value of register SimDelay
 *
 * Notes : From register 0x048A, bits 2:0
 *
 **************************************************************************/
unsigned char set_sim_delay( enum sim_delay param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(SIM_DELAY_REG);

    /* 
     * Sim Ad Delay 
     */ 
  
    value =  old_value & ~SIM_DELAY_PARAM_MASK;


     value |=  param ;  
    SPIWrite(SIM_DELAY_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sim_delay
 *
 * RET  : Return the value of register SimDelay
 *
 * Notes : From register 0x048A, bits 2:0
 *
 **************************************************************************/
unsigned char get_sim_delay()
  {
    unsigned char value;


    /* 
     * Sim Ad Delay 
     */ 
    value = (SPIRead(SIM_DELAY_REG) & SIM_DELAY_PARAM_MASK);
    return value;
  }


