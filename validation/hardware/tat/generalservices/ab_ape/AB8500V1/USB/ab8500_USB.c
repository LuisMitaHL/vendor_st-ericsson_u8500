/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_USB/Linux/ab8500_USB.c
 * 
 *
 * Generated on the 26/02/2010 08:57 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyrigtht ST Microelectronics  2009
 ***************************************************************************/
#include "ab8500_USB.h"

extern unsigned char SPIRead( unsigned short register_address);
extern void SPIWrite( unsigned short register_address, unsigned char data );


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
    value = (SPIRead(USB_LINE_STATUS_REG) & CHARGER_TYPE_PARAM_MASK) >> 3;
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
    value = (SPIRead(USB_LINE_STATUS_REG) & HOST_CHARGER_DETECTED_PARAM_MASK) >> 2;
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
    value = (SPIRead(USB_LINE_STATUS_REG) & DMINUS_RECEIVER_HIGH_PARAM_MASK) >> 1;
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
    value = (SPIRead(USB_LINE_STATUS_REG) & DPLUS_RECEIVER_HIGH_PARAM_MASK);
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param )
      {
        case DM_PULL_UP_ENABLE_E: 
           value = old_value | DM_PULL_UP_ENABLE_PARAM_MASK; 
           break;
        case DM_PULL_UP_DISABLE_E: 
           value = old_value & ~ DM_PULL_UP_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_UP_ENABLE_PARAM_MASK) >> 6;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpMPull Down disable 
     * 1: DpM Pulldown enable 
     */ 
    switch( param )
      {
        case DM_PULLDOWN_DISABLE_E: 
           value = old_value | DM_PULL_PARAM_MASK; 
           break;
        case DM_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DM_PULL_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_PARAM_MASK) >> 5;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DMPull Up disable 
     * 1: DM Pull down enable 
     */ 
    switch( param )
      {
        case DM_PULL_DOWN_ENABLE_E: 
           value = old_value | DM_PULL_DOWN_ENABLE_PARAM_MASK; 
           break;
        case DM_PULL_DOWN_DISABLE_E: 
           value = old_value & ~ DM_PULL_DOWN_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & DM_PULL_DOWN_ENABLE_PARAM_MASK) >> 4;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: DpPull Up disable 
     * 1: DM Pull Up enable 
     */ 
    switch( param )
      {
        case DP_PULL_UP_ENABLE_E: 
           value = old_value | DP_PULL_UP_ENABLE_PARAM_MASK; 
           break;
        case DP_PULL_UP_DISABLE_E: 
           value = old_value & ~ DP_PULL_UP_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & DP_PULL_UP_ENABLE_PARAM_MASK) >> 3;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by SPI register 
     * 1: Pull-up and Pull-down switches (including ENC 
     * switches) are controlled by ULPI register 
     */ 
    switch( param )
      {
        case CONTROLED_BY_SPI_E: 
           value = old_value | USB_SWITCH_PARAM_MASK; 
           break;
        case CONTROLED_BY_ULPI_E: 
           value = old_value & ~ USB_SWITCH_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & USB_SWITCH_PARAM_MASK) >> 1;
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
unsigned char set_charger_pull_up( enum charger_pull_up_close param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(USB_PHY_MMANAGEMENT_REG);

    /* 
     * 0: S4 switch is open (disables Charger pull up on Dplus 
     * line) 
     * 1: S4 switch is closed (enables Charger pull up on Dplus 
     * line) 
     */ 
    switch( param )
      {
        case CHARGER_PULLUP_OPEN_E: 
           value = old_value | CHARGER_PULL_UP_CLOSE_PARAM_MASK; 
           break;
        case CHARGER_PULLUP_CLOSED_E: 
           value = old_value & ~ CHARGER_PULL_UP_CLOSE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_REG, value);

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
     * 0: S4 switch is open (disables Charger pull up on Dplus 
     * line) 
     * 1: S4 switch is closed (enables Charger pull up on Dplus 
     * line) 
     */ 
    value = (SPIRead(USB_PHY_MMANAGEMENT_REG) & CHARGER_PULL_UP_CLOSE_PARAM_MASK);
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: Disable DM 19.25KOhms pull down 
     * 1: Enable DM 19.25kOhms pull down 
     */ 
    switch( param )
      {
        case DM_PULLDOWN_DISABLE_E: 
           value = old_value | DM_PULL_PARAM_MASK; 
           break;
        case DM_PULLDOWN_ENABLE_E: 
           value = old_value & ~ DM_PULL_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_2_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_2_REG) & DM_PULL_PARAM_MASK) >> 7;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by ULPI 
     * 1: DM 19.25kOhms pull down and Single Ended Receiver 
     * controlled by SPI for charger 
     */ 
    switch( param )
      {
        case UPLI_E: 
           value = old_value | CHARGER_MUX_CTRL_PARAM_MASK; 
           break;
        case SPI_E: 
           value = old_value & ~ CHARGER_MUX_CTRL_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_2_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_2_REG) & CHARGER_MUX_CTRL_PARAM_MASK) >> 6;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_2_REG);

    /* 
     * 0: disable USB charger detection 
     * 1: enable USB charger detection 
     */ 
    switch( param )
      {
        case USB_CHARGER_DETECTION_ENABLE_E: 
           value = old_value | USB_CHARGER_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case USB_CHARGER_DETECTION_DISABLE_E: 
           value = old_value & ~ USB_CHARGER_DETECTION_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_2_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_2_REG) & USB_CHARGER_DETECTION_ENABLE_PARAM_MASK);
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: inactive 
     * 1: force 60 Mhz PLL on 
     */ 
    switch( param )
      {
        case DATA_CONNECT_DETECTION_ENABLE_E: 
           value = old_value | DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case DATA_CONNECT_DETECTION_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_DETECTION_ENABLE_PARAM_MASK) >> 7;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: Data Connect source is disabled 
     * 1:Data Connect source is enable 
     */ 
    switch( param )
      {
        case DATA_CONNECT_SOURCE_ENABLE_E: 
           value = old_value | DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK; 
           break;
        case DATA_CONNECT_SOURCE_DISABLE_E: 
           value = old_value & ~ DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & DATA_CONNECT_SOURCE_ENABLE_PARAM_MASK) >> 6;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SNK USB switch is opened 
     * 1: SDM_SNK USB switch is closed 
     */ 
    switch( param )
      {
        case SDM_SNK_SWITCH_OPEN_E: 
           value = old_value | SDM_SNK_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDM_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SNK_SWITCH_OPENED_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SNK_SWITCH_OPENED_PARAM_MASK) >> 5;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SNK USB switch is opened 
     * 1: SDP_SNK USB switch is closed 
     */ 
    switch( param )
      {
        case SDP_SNK_SWITCH_OPEN_E: 
           value = old_value | SDP_SNK_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDP_SNK_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SNK_SWITCH_OPENED_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SNK_SWITCH_OPENED_PARAM_MASK) >> 4;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDM_SRC USB switch is opened 
     * 1: SDM_SRC USB switch is closed 
     */ 
    switch( param )
      {
        case SDM_SRC_SWITCH_OPEN_E: 
           value = old_value | SDM_SRC_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDM_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDM_SRC_SWITCH_OPENED_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & SDM_SRC_SWITCH_OPENED_PARAM_MASK) >> 3;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: SDP_SRC USB switch is opened 
     * 1: SDP_SRC USB switch is closed 
     */ 
    switch( param )
      {
        case SDP_SRC_SWITCH_OPEN_E: 
           value = old_value | SDP_SRC_SWITCH_OPENED_PARAM_MASK; 
           break;
        case SDP_SRC_SWITCH_CLOSE_E: 
           value = old_value & ~ SDP_SRC_SWITCH_OPENED_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & SDP_SRC_SWITCH_OPENED_PARAM_MASK) >> 2;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference are disabled 
     * 1: USB VDAT SRC comparator, VDAT DET comparator, VDAT_REF 
     * reference comparators are enabled 
     */ 
    switch( param )
      {
        case VDAT_SOURCE_ENABLE_E: 
           value = old_value | VDAT_SOURCE_ENABLE_PARAM_MASK; 
           break;
        case VDAT_SOURCE_DISABLE_E: 
           value = old_value & ~ VDAT_SOURCE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & VDAT_SOURCE_ENABLE_PARAM_MASK) >> 1;
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

    old_value = SPIRead(USB_PHY_MMANAGEMENT_3_REG);

    /* 
     * 0: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are disabled 
     * 1: USB IDAT SIN current source, VDAT DET comparator, 
     * VDAT_REF reference are enabled 
     */ 
    switch( param )
      {
        case IDAT_SINK_ENABLE_E: 
           value = old_value | IDAT_SINK_ENABLE_PARAM_MASK; 
           break;
        case IDAT_SINK_DISABLE_E: 
           value = old_value & ~ IDAT_SINK_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_MMANAGEMENT_3_REG, value);

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
    value = (SPIRead(USB_PHY_MMANAGEMENT_3_REG) & IDAT_SINK_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vbus_comp
 *
 * IN   : param, a value to write to the regiter UsbPhyAdaptation
 * OUT  : 
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 6:6
 *
 **************************************************************************/
unsigned char set_vbus_comp( enum compatrator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable VBUS comparators 
     * 1: Enable VBUS comparator 
     */ 
    switch( param )
      {
        case COMPATRATOR_ENABLE_E: 
           value = old_value | COMPATRATOR_ENABLE_PARAM_MASK; 
           break;
        case COMPATRATOR_DISABLE_E: 
           value = old_value & ~ COMPATRATOR_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_ADAPTATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vbus_comp
 *
 * RET  : Return the value of register UsbPhyAdaptation
 *
 * Notes : From register 0x0587, bits 6:6
 *
 **************************************************************************/
unsigned char get_vbus_comp()
  {
    unsigned char value;


    /* 
     * 0: Disable VBUS comparators 
     * 1: Enable VBUS comparator 
     */ 
    value = (SPIRead(USB_PHY_ADAPTATION_REG) & COMPATRATOR_ENABLE_PARAM_MASK) >> 6;
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

    old_value = SPIRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable VBUS Valid comparator 
     * 1: Enable VBUS Valid comparator 
     */ 
    switch( param )
      {
        case VALID_COMPATRATOR_ENABLE_E: 
           value = old_value | VALID_COMPATRATOR_ENABLE_PARAM_MASK; 
           break;
        case VALID_COMPATRATOR_DISABLE_E: 
           value = old_value & ~ VALID_COMPATRATOR_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_ADAPTATION_REG, value);

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
    value = (SPIRead(USB_PHY_ADAPTATION_REG) & VALID_COMPATRATOR_ENABLE_PARAM_MASK) >> 5;
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

    old_value = SPIRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Host detection 
     * 1: Enable ID Host detection 
     */ 
    switch( param )
      {
        case DISABLE_DETECTION_E: 
           value = old_value | DETECTION_PARAM_MASK; 
           break;
        case ENABLE_DETECTION_E: 
           value = old_value & ~ DETECTION_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_ADAPTATION_REG, value);

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
    value = (SPIRead(USB_PHY_ADAPTATION_REG) & DETECTION_PARAM_MASK) >> 1;
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

    old_value = SPIRead(USB_PHY_ADAPTATION_REG);

    /* 
     * 0: Disable ID Device detection 
     * 1: Enable ID Device detection 
     */ 
    switch( param )
      {
        case DISABLE_DETECTION_E: 
           value = old_value | DETECTION_PARAM_MASK; 
           break;
        case ENABLE_DETECTION_E: 
           value = old_value & ~ DETECTION_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_ADAPTATION_REG, value);

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
    value = (SPIRead(USB_PHY_ADAPTATION_REG) & DETECTION_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det330k
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 4:4
 *
 **************************************************************************/
unsigned char set_id_det330k( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below 330k 
     * 1: ID resistance is above 330k 
     */ 
    switch( param )
      {
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det330k
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 4:4
 *
 **************************************************************************/
unsigned char get_id_det330k()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below 330k 
     * 1: ID resistance is above 330k 
     */ 
    value = (SPIRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det220k
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 3:3
 *
 **************************************************************************/
unsigned char set_id_det220k( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below 220k 
     * 1: ID resistance is above 220k 
     */ 
    switch( param )
      {
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det220k
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 3:3
 *
 **************************************************************************/
unsigned char get_id_det220k()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below 220k 
     * 1: ID resistance is above 220k 
     */ 
    value = (SPIRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det140k
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 2:2
 *
 **************************************************************************/
unsigned char set_id_det140k( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below 140k 
     * 1: ID resistance is above 140k 
     */ 
    switch( param )
      {
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det140k
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 2:2
 *
 **************************************************************************/
unsigned char get_id_det140k()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below 140k 
     * 1: ID resistance is above 140k 
     */ 
    value = (SPIRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_id_det80k
 *
 * IN   : param, a value to write to the regiter OtgIdResistance
 * OUT  : 
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 1:1
 *
 **************************************************************************/
unsigned char set_id_det80k( enum id_resitance param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = SPIRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is below 80k 
     * 1: ID resistance is above 80k 
     */ 
    switch( param )
      {
        case BELOW_E: 
           value = old_value | ID_RESITANCE_PARAM_MASK; 
           break;
        case ABOVE_E: 
           value = old_value & ~ ID_RESITANCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_ID_RESISTANCE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_id_det80k
 *
 * RET  : Return the value of register OtgIdResistance
 *
 * Notes : From register 0x0588, bits 1:1
 *
 **************************************************************************/
unsigned char get_id_det80k()
  {
    unsigned char value;


    /* 
     * 0: ID resistance is below 80k 
     * 1: ID resistance is above 80k 
     */ 
    value = (SPIRead(OTG_ID_RESISTANCE_REG) & ID_RESITANCE_PARAM_MASK) >> 1;
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

    old_value = SPIRead(OTG_ID_RESISTANCE_REG);

    /* 
     * 0: ID resistance is floating (no micro-A plug detected) 
     * 1: ID resistance is grounded (micro-A plug detected)k 
     */ 
    switch( param )
      {
        case FLOATING_E: 
           value = old_value | RESITANCE_PARAM_MASK; 
           break;
        case GROUNDED_E: 
           value = old_value & ~ RESITANCE_PARAM_MASK;
           break;
      }
  

    SPIWrite(OTG_ID_RESISTANCE_REG, value);

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
    value = (SPIRead(OTG_ID_RESISTANCE_REG) & RESITANCE_PARAM_MASK);
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
    value = (SPIRead(USB_PHY_STATUS_REG) & HIGH_SPEED_MODE_PARAM_MASK) >> 2;
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
    value = (SPIRead(USB_PHY_STATUS_REG) & CHIRP_MODE_PARAM_MASK) >> 1;
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
    value = (SPIRead(USB_PHY_STATUS_REG) & SUSPEND_MODE_PARAM_MASK);
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

    old_value = SPIRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in Device mode (turn on sequence of 
     * USB PHY with no Vbus generation) 
     */ 
    switch( param )
      {
        case USB_DEVICE_MODE_ENABLE_E: 
           value = old_value | USB_DEVICE_MODE_ENABLE_PARAM_MASK; 
           break;
        case USB_DEVICE_MODE_DISABLE_E: 
           value = old_value & ~ USB_DEVICE_MODE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_CONTROL_REG, value);

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
    value = (SPIRead(USB_PHY_CONTROL_REG) & USB_DEVICE_MODE_ENABLE_PARAM_MASK) >> 1;
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

    old_value = SPIRead(USB_PHY_CONTROL_REG);

    /* 
     * 0: doesn’t turn on USB PHY 
     * 1: turn on USB PHY in host mode 
     */ 
    switch( param )
      {
        case USB_HOST_MODE_ENABLE_E: 
           value = old_value | USB_HOST_MODE_ENABLE_PARAM_MASK; 
           break;
        case USB_HOST_MODE_DISABLE_E: 
           value = old_value & ~ USB_HOST_MODE_ENABLE_PARAM_MASK;
           break;
      }
  

    SPIWrite(USB_PHY_CONTROL_REG, value);

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
    value = (SPIRead(USB_PHY_CONTROL_REG) & USB_HOST_MODE_ENABLE_PARAM_MASK);
    return value;
  }


