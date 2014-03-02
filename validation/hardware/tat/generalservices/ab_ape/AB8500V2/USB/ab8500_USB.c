/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_USB/Linux/ab8500_USB.c
 * 
 *
 * Generated on the 20/04/2011 10:26 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : UM0836 User manual, AB8500_V2 Mixed signal multimedia and power management Rev 1
 *
 ***************************************************************************/
#include "ab8500_USB.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : get_uart_mode
 *
 * RET  : Return the value of register UsbLineStatus
 *
 * Notes : From register 0x0580, bits 7:7
 *
 **************************************************************************/
unsigned char get_uart_mode()
  {
    unsigned char value;


    /* 
     * 0: Uart mode is not active 
     * 1: Uart mode is active 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & UART_MMODE_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_charger_type
 *
 * RET  : Return the value of register UsbLineStatus
 *
 * Notes : From register 0x0580, bits 6:3
 *
 **************************************************************************/
unsigned char get_charger_type()
  {
    unsigned char value;


    /* 
     * 0000: Not configured 
     * 0001: Standard Host, not charging 
     * 0010: Standard Host, charging, not suspended 
     * 0011: Standard Host, charging, suspended 
     * 0100: Host charger, normal mode 
     * 0101: Host charger, HS mode 
     * 0110: Host charger, HS Chirp mode 
     * 0111: Dedicated USx charger 
     * 1000: ACA RID_A configuration 
     * 1001: ACA RID_B configuration 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & CHARGER_TYPE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_host_charger_detected
 *
 * RET  : Return the value of register UsbLineStatus
 *
 * Notes : From register 0x0580, bits 2:2
 *
 **************************************************************************/
unsigned char get_host_charger_detected()
  {
    unsigned char value;


    /* 
     * 0: No Host charger detected 
     * 1: Host Charger detected 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & HOST_CHARGER_DETECTED_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_dminus_cmp_out
 *
 * RET  : Return the value of register UsbLineStatus
 *
 * Notes : From register 0x0580, bits 1:1
 *
 **************************************************************************/
unsigned char get_dminus_cmp_out()
  {
    unsigned char value;


    /* 
     * 0: DM Single Ended Receiver output is low 
     * 1: DMSingle Ended Receiver output is high 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & DMINUS_RECEIVER_HIGH_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_dplus_cmp_out
 *
 * RET  : Return the value of register UsbLineStatus
 *
 * Notes : From register 0x0580, bits 0:0
 *
 **************************************************************************/
unsigned char get_dplus_cmp_out()
  {
    unsigned char value;


    /* 
     * 0: DM Single Ended Receiver output is low 
     * 1: DMSingle Ended Receiver output is high 
     */ 
    value = (I2CRead(USB_LINE_STATUS_REG) & DPLUS_RECEIVER_HIGH_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 6:6
 *
 **************************************************************************/
unsigned char set_dm_pull_up( enum dm_pull_up_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param ){
        case DM_PULL_UP_ENABLE_E: 
           value = old_value | DM_PULL_UP_ENABLE_PARAM_MASK; 
           break;
        case DM_PULL_UP_DISABLE_E: 
           value = old_value & ~ DM_PULL_UP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_up
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 6:6
 *
 **************************************************************************/
unsigned char get_dm_pull_up()
  {
    unsigned char value;


    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_UP_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dp_pull_down
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 5:5
 *
 **************************************************************************/
unsigned char set_dp_pull_down( enum dm_pull param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpMPull Down disable 
     * 1: DpM Pulldown enable 
     */ 
    switch( param ){
        case DM_PULLDOWN_DISABLE_E: 
           value = old_value | DM_PULL_PARAM_MASK; 
           break;
        case DM_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DM_PULL_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dp_pull_down
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 5:5
 *
 **************************************************************************/
unsigned char get_dp_pull_down()
  {
    unsigned char value;


    /* 
     * 0: DpMPull Down disable 
     * 1: DpM Pulldown enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_down
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 4:4
 *
 **************************************************************************/
unsigned char set_dm_pull_down( enum dm_pull_down_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull down enable 
     */ 
    switch( param ){
        case DM_PULL_DOWN_ENABLE_E: 
           value = old_value | DM_PULL_DOWN_ENABLE_PARAM_MASK; 
           break;
        case DM_PULL_DOWN_DISABLE_E: 
           value = old_value & ~ DM_PULL_DOWN_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_down
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 4:4
 *
 **************************************************************************/
unsigned char get_dm_pull_down()
  {
    unsigned char value;


    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull down enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_DOWN_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dp_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 3:3
 *
 **************************************************************************/
unsigned char set_dp_pull_up( enum dp_pull_up_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param ){
        case DP_PULL_UP_ENABLE_E: 
           value = old_value | DP_PULL_UP_ENABLE_PARAM_MASK; 
           break;
        case DP_PULL_UP_DISABLE_E: 
           value = old_value & ~ DP_PULL_UP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dp_pull_up
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 3:3
 *
 **************************************************************************/
unsigned char get_dp_pull_up()
  {
    unsigned char value;


    /* 
     * 0: DpPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & DP_PULL_UP_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_switch_ctrl
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_switch_ctrl( enum usb_switch param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by SPI register 
     * 1: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by ULPI register 
     */ 
    switch( param ){
        case CONTROLED_BY_ULPI_E: 
           value = old_value | USB_SWITCH_PARAM_MASK; 
           break;
        case CONTROLED_BY_SPI_E: 
           value = old_value & ~ USB_SWITCH_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_switch_ctrl
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_switch_ctrl()
  {
    unsigned char value;


    /* 
     * 0: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by SPI register 
     * 1: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by ULPI register 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & USB_SWITCH_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_pull_up
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 0:0
 *
 **************************************************************************/
unsigned char set_charger_pull_up( enum id_detection_thru_adc_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: disable Id detection thru ADC 
     * 1: enable Id detection thru ADC 
     */ 
    switch( param ){
        case ID_DETECTION_THRU_ADC_ENABLE_E: 
           value = old_value | ID_DETECTION_THRU_ADC_ENABLE_PARAM_MASK; 
           break;
        case ID_DETECTION_THRU_ADC_DISABLE_E: 
           value = old_value & ~ ID_DETECTION_THRU_ADC_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_pull_up
 *
 * RET  : Return the value of register UsbPhyMmanagement
 *
 * Notes : From register 0x0581, bits 0:0
 *
 **************************************************************************/
unsigned char get_charger_pull_up()
  {
    unsigned char value;


    /* 
     * 0: disable Id detection thru ADC 
     * 1: enable Id detection thru ADC 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_REG) & ID_DETECTION_THRU_ADC_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_dm_pull_down_disable
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 7:7
 *
 **************************************************************************/
unsigned char set_dm_pull_down_disable( enum dm_pull param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: Disable DM 19.25KOhms pull down 
     * 1: Enable DM 19.25kOhms pull down 
     */ 
    switch( param ){
        case DM_PULLDOWN_DISABLE_E: 
           value = old_value | DM_PULL_PARAM_MASK; 
           break;
        case DM_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DM_PULL_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dm_pull_down_disable
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 7:7
 *
 **************************************************************************/
unsigned char get_dm_pull_down_disable()
  {
    unsigned char value;


    /* 
     * 0: Disable DM 19.25KOhms pull down 
     * 1: Enable DM 19.25kOhms pull down 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & DM_PULL_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charger_mux_ctrl
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 6:6
 *
 **************************************************************************/
unsigned char set_charger_mux_ctrl( enum charger_mux_ctrl param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by ULPI 
     * 1: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by SPI for charger 
     */ 
    switch( param ){
        case SPI_E: 
           value = old_value | CHARGER_MUX_CTRL_PARAM_MASK; 
           break;
        case UPLI_E: 
           value = old_value & ~ CHARGER_MUX_CTRL_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charger_mux_ctrl
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 6:6
 *
 **************************************************************************/
unsigned char get_charger_mux_ctrl()
  {
    unsigned char value;


    /* 
     * 0: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by ULPI 
     * 1: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by SPI for charger 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & CHARGER_MUX_CTRL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_uart_low_power_mode
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 5:5
 *
 **************************************************************************/
unsigned char set_uart_low_power_mode( enum uart_low_power_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: inactive 
     * 1: Uart low power mode is enabled 
     */ 
    switch( param ){
        case UART_LOW_POWER_UP_E: 
           value = old_value | UART_LOW_POWER_MODE_ENABLE_PARAM_MASK; 
           break;
        case UART_LOW_POWER_DOWN_E: 
           value = old_value & ~ UART_LOW_POWER_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_uart_low_power_mode
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 5:5
 *
 **************************************************************************/
unsigned char get_uart_low_power_mode()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: Uart low power mode is enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & UART_LOW_POWER_MODE_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_gate_sys_ulp_clock
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 4:4
 *
 **************************************************************************/
unsigned char set_gate_sys_ulp_clock( enum gate_ulp_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: clock not gated 
     * 1: clock gated, sent only to audio digital 
     */ 
    switch( param ){
        case ULPCLOCK_GATED_E: 
           value = old_value | GATE_ULP_CLOCK_PARAM_MASK; 
           break;
        case ULPCLOCK_NOT_GATED_E: 
           value = old_value & ~ GATE_ULP_CLOCK_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_gate_sys_ulp_clock
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 4:4
 *
 **************************************************************************/
unsigned char get_gate_sys_ulp_clock()
  {
    unsigned char value;


    /* 
     * 0: clock not gated 
     * 1: clock gated, sent only to audio digital 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & GATE_ULP_CLOCK_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_charger_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement2
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_charger_detection( enum usb_charger_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: disable USB charger detection 
     * 1: enable USB charger detection 
     */ 
    switch( param ){
        case USB_CHARGER_DETECTION_ENABLE_E: 
           value = old_value | USB_CHARGER_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case USB_CHARGER_DETECTION_DISABLE_E: 
           value = old_value & ~ USB_CHARGER_DETECTION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_charger_detection
 *
 * RET  : Return the value of register UsbPhyMmanagement2
 *
 * Notes : From register 0x0582, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_charger_detection()
  {
    unsigned char value;


    /* 
     * 0: disable USB charger detection 
     * 1: enable USB charger detection 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_2_REG) & USB_CHARGER_DETECTION_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_data_connect_detection_enable
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 7:7
 *
 **************************************************************************/
unsigned char set_data_connect_detection_enable( enum data_connect_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: inactive 
     * 1: force 60 Mhz PLL on 
     */ 
    switch( param ){
        case DATA_CONNECT_DETECTION_ENABLE_E: 
           value = old_value | DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case DATA_CONNECT_DETECTION_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_data_connect_detection_enable
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 7:7
 *
 **************************************************************************/
unsigned char get_data_connect_detection_enable()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: force 60 Mhz PLL on 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_data_connect_source_enable
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 6:6
 *
 **************************************************************************/
unsigned char set_data_connect_source_enable( enum data_connect_source_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: Data Connect source is disabled 
     * 1:Data Connect source is enable 
     */ 
    switch( param ){
        case DATA_CONNECT_SOURCE_ENABLE_E: 
           value = old_value | DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK; 
           break;
        case DATA_CONNECT_SOURCE_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_data_connect_source_enable
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 6:6
 *
 **************************************************************************/
unsigned char get_data_connect_source_enable()
  {
    unsigned char value;


    /* 
     * 0: Data Connect source is disabled 
     * 1:Data Connect source is enable 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdm_snk_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 5:5
 *
 **************************************************************************/
unsigned char set_sdm_snk_switch( enum sdm_snk_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SNK USB switch is opened 
     * 1: SDM_SNK USB switch is closed 
     */ 
    switch( param ){
        case SDM_SNK_SWITCH_OPEN_E: 
           value = old_value | SDM_SNK_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDM_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SNK_SWITCH_OPENED_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdm_snk_switch
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 5:5
 *
 **************************************************************************/
unsigned char get_sdm_snk_switch()
  {
    unsigned char value;


    /* 
     * 0: SDM_SNK USB switch is opened 
     * 1: SDM_SNK USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SNK_SWITCH_OPENED_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdp_snk_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 4:4
 *
 **************************************************************************/
unsigned char set_sdp_snk_switch( enum sdp_snk_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SNK USB switch is opened 
     * 1: SDP_SNK USB switch is closed 
     */ 
    switch( param ){
        case SDP_SNK_SWITCH_OPEN_E: 
           value = old_value | SDP_SNK_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDP_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SNK_SWITCH_OPENED_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdp_snk_switch
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 4:4
 *
 **************************************************************************/
unsigned char get_sdp_snk_switch()
  {
    unsigned char value;


    /* 
     * 0: SDP_SNK USB switch is opened 
     * 1: SDP_SNK USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SNK_SWITCH_OPENED_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdm_src_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 3:3
 *
 **************************************************************************/
unsigned char set_sdm_src_switch( enum sdm_src_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SRC USB switch is opened 
     * 1: SDM_SRC USB switch is closed 
     */ 
    switch( param ){
        case SDM_SRC_SWITCH_OPEN_E: 
           value = old_value | SDM_SRC_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDM_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SRC_SWITCH_OPENED_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdm_src_switch
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 3:3
 *
 **************************************************************************/
unsigned char get_sdm_src_switch()
  {
    unsigned char value;


    /* 
     * 0: SDM_SRC USB switch is opened 
     * 1: SDM_SRC USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SRC_SWITCH_OPENED_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sdp_src_switch
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 2:2
 *
 **************************************************************************/
unsigned char set_sdp_src_switch( enum sdp_src_switch_opened param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SRC USB switch is opened 
     * 1: SDP_SRC USB switch is closed 
     */ 
    switch( param ){
        case SDP_SRC_SWITCH_OPEN_E: 
           value = old_value | SDP_SRC_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDP_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SRC_SWITCH_OPENED_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sdp_src_switch
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 2:2
 *
 **************************************************************************/
unsigned char get_sdp_src_switch()
  {
    unsigned char value;


    /* 
     * 0: SDP_SRC USB switch is opened 
     * 1: SDP_SRC USB switch is closed 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SRC_SWITCH_OPENED_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vdat_source
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 1:1
 *
 **************************************************************************/
unsigned char set_vdat_source( enum vdat_source_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference are disabled 
     * 1: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference comparators are enabled 
     */ 
    switch( param ){
        case VDAT_SOURCE_ENABLE_E: 
           value = old_value | VDAT_SOURCE_ENABLE_PARAM_MASK; 
           break;
        case VDAT_SOURCE_DISABLE_E: 
           value = old_value & ~ VDAT_SOURCE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vdat_source
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 1:1
 *
 **************************************************************************/
unsigned char get_vdat_source()
  {
    unsigned char value;


    /* 
     * 0: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference are disabled 
     * 1: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference comparators are enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & VDAT_SOURCE_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_idat_sink
 *
 * IN   : param, a value to write to the regiter UsbPhyMmanagement3
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 0:0
 *
 **************************************************************************/
unsigned char set_idat_sink( enum idat_sink_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are disabled 
     * 1: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are enabled 
     */ 
    switch( param ){
        case IDAT_SINK_ENABLE_E: 
           value = old_value | IDAT_SINK_ENABLE_PARAM_MASK; 
           break;
        case IDAT_SINK_DISABLE_E: 
           value = old_value & ~ IDAT_SINK_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_MMANAGEMENT_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_idat_sink
 *
 * RET  : Return the value of register UsbPhyMmanagement3
 *
 * Notes : From register 0x0583, bits 0:0
 *
 **************************************************************************/
unsigned char get_idat_sink()
  {
    unsigned char value;


    /* 
     * 0: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are disabled 
     * 1: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are enabled 
     */ 
    value = (I2CRead(USB_PHY_MMANAGEMENT_3_REG) & IDAT_SINK_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_valid
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 5:5
 *
 **************************************************************************/
unsigned char set_vbus_valid( enum valid_compatrator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable VBUS Valid comparator 
     * 1: Enable VBUS Valid comparator 
     */ 
    switch( param ){
        case VALID_COMPATRATOR_ENABLE_E: 
           value = old_value | VALID_COMPATRATOR_ENABLE_PARAM_MASK; 
           break;
        case VALID_COMPATRATOR_DISABLE_E: 
           value = old_value & ~ VALID_COMPATRATOR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_valid
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 5:5
 *
 **************************************************************************/
unsigned char get_vbus_valid()
  {
    unsigned char value;


    /* 
     * 0: Disable VBUS Valid comparator 
     * 1: Enable VBUS Valid comparator 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & VALID_COMPATRATOR_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_host_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 1:1
 *
 **************************************************************************/
unsigned char set_id_host_detection( enum detection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Host detection 
     * 1: Enable ID Host detection 
     */ 
    switch( param ){
        case DISABLE_DETECTION_E: 
           value = old_value | DETECTION_PARAM_MASK; 
           break;
        case ENABLE_DETECTION_E: 
           value = old_value & ~ DETECTION_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_host_detection
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 1:1
 *
 **************************************************************************/
unsigned char get_id_host_detection()
  {
    unsigned char value;


    /* 
     * 0: Disable ID Host detection 
     * 1: Enable ID Host detection 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & DETECTION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_device_detection
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 0:0
 *
 **************************************************************************/
unsigned char set_device_detection( enum detection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Device detection 
     * 1: Enable ID Device detection 
     */ 
    switch( param ){
        case DISABLE_DETECTION_E: 
           value = old_value | DETECTION_PARAM_MASK; 
           break;
        case ENABLE_DETECTION_E: 
           value = old_value & ~ DETECTION_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_device_detection
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 0:0
 *
 **************************************************************************/
unsigned char get_device_detection()
  {
    unsigned char value;


    /* 
     * 0: Disable ID Device detection 
     * 1: Enable ID Device detection 
     */ 
    value = (I2CRead(USB_PHY_ADAPTATION_REG) & DETECTION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_drive_vbus
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 5:5
 *
 **************************************************************************/
unsigned char set_drive_vbus( enum vbus_generation_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: 5V generation is disabled 
     * 1: 5V generation is enabled 
     */ 
    switch( param ){
        case VBUS_GENERATION_ENABLE_E: 
           value = old_value | VBUS_GENERATION_ENABLE_PARAM_MASK; 
           break;
        case VBUS_GENERATION_DISABLE_E: 
           value = old_value & ~ VBUS_GENERATION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_drive_vbus
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 5:5
 *
 **************************************************************************/
unsigned char get_drive_vbus()
  {
    unsigned char value;


    /* 
     * 0: 5V generation is disabled 
     * 1: 5V generation is enabled 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & VBUS_GENERATION_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r4
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 4:4
 *
 **************************************************************************/
unsigned char set_id_det_r4( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below R4 
     * 1: ID resistance is above R4 
     */ 
    switch( param ){
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r4
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_det_r4()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below R4 
     * 1: ID resistance is above R4 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r3
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 3:3
 *
 **************************************************************************/
unsigned char set_id_det_r3( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below R3 
     * 1: ID resistance is above R3 
     */ 
    switch( param ){
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r3
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 3:3
 *
 **************************************************************************/
unsigned char get_id_det_r3()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below R3 
     * 1: ID resistance is above R3 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r2
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 2:2
 *
 **************************************************************************/
unsigned char set_id_det_r2( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below R2 
     * 1: ID resistance is above R2 
     */ 
    switch( param ){
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r2
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_det_r2()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below R2 
     * 1: ID resistance is above R2 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det_r1
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 1:1
 *
 **************************************************************************/
unsigned char set_id_det_r1( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below R1 
     * 1: ID resistance is above R1 
     */ 
    switch( param ){
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det_r1
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 1:1
 *
 **************************************************************************/
unsigned char get_id_det_r1()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below R1 
     * 1: ID resistance is above R1 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_wakeup
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 0:0
 *
 **************************************************************************/
unsigned char set_id_wakeup( enum resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is floating (no micro-A plug detected) 
     * 1: ID resistance is grounded (micro-A plug detected)k 
     */ 
    switch( param ){
        case GROUNDED_E: 
           value = old_value | RESITANCE_PARAM_MASK; 
           break;
        case FLOATING_E: 
           value = old_value & ~ RESITANCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_wakeup
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 0:0
 *
 **************************************************************************/
unsigned char get_id_wakeup()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is floating (no micro-A plug detected) 
     * 1: ID resistance is grounded (micro-A plug detected)k 
     */ 
    value = (I2CRead(OTG_ID_RESISTANCE_REG) & RESITANCE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_high_speed_mode
 *
 * RET  : Return the value of register UsbPhyStatus
 *
 * Notes : From register 0x0589, bits 2:2
 *
 **************************************************************************/
unsigned char get_high_speed_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & HIGH_SPEED_MODE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_chirp_mode
 *
 * RET  : Return the value of register UsbPhyStatus
 *
 * Notes : From register 0x0589, bits 1:1
 *
 **************************************************************************/
unsigned char get_chirp_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & CHIRP_MODE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_suspend_mode
 *
 * RET  : Return the value of register UsbPhyStatus
 *
 * Notes : From register 0x0589, bits 0:0
 *
 **************************************************************************/
unsigned char get_suspend_mode()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(USB_PHY_STATUS_REG) & SUSPEND_MODE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_device_mode_enable
 *
 * IN   : param, a value to write to the regiter UsbPhyControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyControl
 *
 * Notes : From register 0x058A, bits 1:1
 *
 **************************************************************************/
unsigned char set_usb_device_mode_enable( enum usb_device_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in Device mode (turn on sequence of 
     * USB PHY with no Vbus generation) 
     */ 
    switch( param ){
        case USB_DEVICE_MODE_ENABLE_E: 
           value = old_value | USB_DEVICE_MODE_ENABLE_PARAM_MASK; 
           break;
        case USB_DEVICE_MODE_DISABLE_E: 
           value = old_value & ~ USB_DEVICE_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_device_mode_enable
 *
 * RET  : Return the value of register UsbPhyControl
 *
 * Notes : From register 0x058A, bits 1:1
 *
 **************************************************************************/
unsigned char get_usb_device_mode_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in Device mode (turn on sequence of 
     * USB PHY with no Vbus generation) 
     */ 
    value = (I2CRead(USB_PHY_CONTROL_REG) & USB_DEVICE_MODE_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_usb_host_mode_enable
 *
 * IN   : param, a value to write to the regiter UsbPhyControl
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyControl
 *
 * Notes : From register 0x058A, bits 0:0
 *
 **************************************************************************/
unsigned char set_usb_host_mode_enable( enum usb_host_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in host mode 
     */ 
    switch( param ){
        case USB_HOST_MODE_ENABLE_E: 
           value = old_value | USB_HOST_MODE_ENABLE_PARAM_MASK; 
           break;
        case USB_HOST_MODE_DISABLE_E: 
           value = old_value & ~ USB_HOST_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_PHY_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_usb_host_mode_enable
 *
 * RET  : Return the value of register UsbPhyControl
 *
 * Notes : From register 0x058A, bits 0:0
 *
 **************************************************************************/
unsigned char get_usb_host_mode_enable()
  {
    unsigned char value;


    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in host mode 
     */ 
    value = (I2CRead(USB_PHY_CONTROL_REG) & USB_HOST_MODE_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_time_two_thresholds_lsb
 *
 * IN   : param, a value to write to the regiter TimeTwoThresholdsLsb
 * OUT  : 
 *
 * RET  : Return the value of register TimeTwoThresholdsLsb
 *
 * Notes : From register 0x0591, bits 7:0
 *
 **************************************************************************/
unsigned char set_time_two_thresholds_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(TIME_TWO_THRESHOLDS_LSB_REG);

    /* 
     * Time Between Two thresholds LSB. 
     */ 
    I2CWrite(TIME_TWO_THRESHOLDS_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_time_two_thresholds_lsb
 *
 * RET  : Return the value of register TimeTwoThresholdsLsb
 *
 * Notes : From register 0x0591, bits 7:0
 *
 **************************************************************************/
unsigned char get_time_two_thresholds_lsb()
  {
    unsigned char value;


    /* 
     * Time Between Two thresholds LSB. 
     */ 
   value = I2CRead(TIME_TWO_THRESHOLDS_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_time_two_thresholds_msb
 *
 * IN   : param, a value to write to the regiter TimeTwoThresholdsMsb
 * OUT  : 
 *
 * RET  : Return the value of register TimeTwoThresholdsMsb
 *
 * Notes : From register 0x0592, bits 7:0
 *
 **************************************************************************/
unsigned char set_time_two_thresholds_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(TIME_TWO_THRESHOLDS_MSB_REG);

    /* 
     * Time Between Two thresholds MSB. 
     */ 
    I2CWrite(TIME_TWO_THRESHOLDS_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_time_two_thresholds_msb
 *
 * RET  : Return the value of register TimeTwoThresholdsMsb
 *
 * Notes : From register 0x0592, bits 7:0
 *
 **************************************************************************/
unsigned char get_time_two_thresholds_msb()
  {
    unsigned char value;


    /* 
     * Time Between Two thresholds MSB. 
     */ 
   value = I2CRead(TIME_TWO_THRESHOLDS_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_time_two_thresholds
 *
 * IN   : param, a value to write to the regiter TimeTwoThresholds
 * OUT  : 
 *
 * RET  : Return the value of register TimeTwoThresholds
 *
 * Notes : NIL
 *
 **************************************************************************/
void set_time_two_thresholds(unsigned short value)
{
  set_time_two_thresholds_msb(value >> 8);
  set_time_two_thresholds_lsb(value &0xFF);
   
}

/***************************************************************************
 *
 * Function : get_time_two_thresholds
 *
 * RET  : Return the value of register TimeTwoThresholds
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_time_two_thresholds()
{
  unsigned char p0, p1;
  p0 = get_time_two_thresholds_lsb();
  p1 = get_time_two_thresholds_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : set_adp_enabled
 *
 * IN   : param, a value to write to the regiter UsbAdpProbe
 * OUT  : 
 *
 * RET  : Return the value of register UsbAdpProbe
 *
 * Notes : From register 0x0593, bits 0:0
 *
 **************************************************************************/
unsigned char set_adp_enabled( enum adp_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_ADP_PROBE_REG);

    /* 
     * 0: inactive 
     * 1: ADP is enabled 
     */ 
    switch( param ){
        case ADP_ENABLE_E: 
           value = old_value | ADP_ENABLED_PARAM_MASK; 
           break;
        case ADP_DISABLE_E: 
           value = old_value & ~ ADP_ENABLED_PARAM_MASK;
           break;
    }
  

    I2CWrite(USB_ADP_PROBE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adp_enabled
 *
 * RET  : Return the value of register UsbAdpProbe
 *
 * Notes : From register 0x0593, bits 0:0
 *
 **************************************************************************/
unsigned char get_adp_enabled()
  {
    unsigned char value;


    /* 
     * 0: inactive 
     * 1: ADP is enabled 
     */ 
    value = (I2CRead(USB_ADP_PROBE_REG) & ADP_ENABLED_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adp_probe_time
 *
 * IN   : param, a value to write to the regiter UsbAdpProbe
 * OUT  : 
 *
 * RET  : Return the value of register UsbAdpProbe
 *
 * Notes : From register 0x0593, bits 3:1
 *
 **************************************************************************/
unsigned char set_adp_probe_time( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(USB_ADP_PROBE_REG);

    /* 
     * Threshold tolerance of AdpProbeTimeCurrent[10:0] 
     * measurement 
     */ 
  
    value =  old_value & ~TOLERANCE_PARAM_MASK;


    value |= ( param << 0x1);  

    I2CWrite(USB_ADP_PROBE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adp_probe_time
 *
 * RET  : Return the value of register UsbAdpProbe
 *
 * Notes : From register 0x0593, bits 3:1
 *
 **************************************************************************/
unsigned char get_adp_probe_time()
  {
    unsigned char value;


    /* 
     * Threshold tolerance of AdpProbeTimeCurrent[10:0] 
     * measurement 
     */ 
    value = (I2CRead(USB_ADP_PROBE_REG) & TOLERANCE_PARAM_MASK) >> 1;
    return value;
  }


