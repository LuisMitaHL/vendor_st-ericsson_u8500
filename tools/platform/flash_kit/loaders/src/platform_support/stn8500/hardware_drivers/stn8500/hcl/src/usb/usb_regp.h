/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief This file holds the register data structures used by HCL
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _USB_REG_P_H_
#define _USB_REG_P_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "usb.h"

#define OTG_FIFOSIZE_MASK   0x3FF

#define OTG_TESTMODE_J_MASK                 MASK_BIT1
#define OTG_TESTMODE_K_MASK                 MASK_BIT2
#define OTG_TESTMODE_SE0NAK_MASK            MASK_BIT0
#define OTG_TESTMODE_FORCE_ENABLE_HOST_MASK MASK_BIT7
#define OTG_TESTMODE_PACKET_TEST_MASK       MASK_BIT3

#define GET_0_ENDPNUM_SHORT(n)              (((t_uint8) n) % 16)
#define USB_DEV_0_GET_FIFO_OFFSET_VALUE(n)  ((t_uint32) n / 8)

#ifdef ST_8500ED
typedef volatile struct
{
    /*
      Function Address Register
    */
    t_uint8     OTG_FADDR;

    /*
      OTG Power register
    */
    t_uint8     OTG_PWR;

    /*
      OTG Tx Interrupt Status Register
    */
    t_uint16    OTG_INTTX;

    /*
      OTG Rx Interrupt Status Register
    */
    t_uint16    OTG_INTRX;

    /*
      OTG Tx Interrupt Enable Register
    */
    t_uint16    OTG_INTTXEN;

    /*
      OTG Rx Interrupt Enable Register
    */
    t_uint16    OTG_INTRXEN;

    /*
      OTG USB System Interrupt Status Register
    */
    t_uint8     OTG_INTUSB;

    /*
      OTG USB System Interrupt Enable Register
    */
    t_uint8     OTG_INTUSBEN;

    /*
      Frame Number register
    */
    t_uint16    OTG_FMNO;

    /*
      Endpoint Index Register
    */
    t_uint8     OTG_INDX;

    /*
      Test Mode Register
    */
    t_uint8     OTG_TM;

    /*
      Tx Max Packet Register
    */
    t_uint16    OTG_TXMAXP;

    /*
      Tx Control and Status Register
    */
    t_uint16    OTG_CSR0_TXCSR;

    /*
      Rx Max Packet Register
    */
    t_uint16    OTG_RXMAXP;

    /*
      Rx Control and Status Register
    */
    t_uint16    OTG_RXCSR;

    /*
      Rx Count Register
    */
    t_uint16    OTG_CNT0_RXCNT;

    /*
      Transaction Control Register (Tx)
    */
    t_uint8     OTG_TYPE0_TXTYPE;

    /*
      Polling Interval Register (Tx)
    */
    t_uint8     OTG_NAKLMT0_TXINTV;

    /*
       Transaction Control Register (Rx)
    */
    t_uint8     OTG_RXTYPE;

    /*
      Polling Interval Register (Rx)
    */
    t_uint8     OTG_RXINTV;
    t_uint8     FILL0;
    t_uint8     OTG_CFD_FSIZE;

    /*
      FIFO registers
    */
    t_uint32    OTG_FIFO0;
    t_uint32    OTG_FIFO1;
    t_uint32    OTG_FIFO2;
    t_uint32    OTG_FIFO3;
    t_uint32    OTG_FIFO4;
    t_uint32    OTG_FIFO5;
    t_uint32    OTG_FIFO6;
    t_uint32    OTG_FIFO7;
    t_uint32    OTG_FIFO8;
    t_uint32    OTG_FIFO9;
    t_uint32    OTG_FIFO10;
    t_uint32    OTG_FIFO11;
    t_uint32    OTG_FIFO12;
    t_uint32    OTG_FIFO13;
    t_uint32    OTG_FIFO14;
    t_uint32    OTG_FIFO15;
    t_uint8     OTG_DEVCTL;
    t_uint8     FILL1;
    t_uint8     OTG_TXFSZ;
    t_uint8     OTG_RXFSZ;
    t_uint16    OTG_TXFA;
    t_uint16    OTG_RXFA;
    t_uint32    OTG_VCNTL;
    t_uint16    OTG_HWVER;
    t_uint16    FILL2;
    t_uint8     OTG_UVBCTRL;
    t_uint8     OTG_UCKIT;
    t_uint8     OTG_UINTMASK;
    t_uint8     OTG_UINTSRC;
    t_uint8     OTG_UREGDATA;
    t_uint8     OTG_UREGADDR;
    t_uint8     OTG_UREGCTRL;
    t_uint8     OTG_URAWDATA;
    t_uint8     OTG_EPINFO;
    t_uint8     OTG_RAMINFO;
    t_uint8     OTG_LINKINFO;
    t_uint8     OTG_VPLEN;
    t_uint8     OTG_HSEOF1;
    t_uint8     OTG_FSEOF1;
    t_uint8     OTG_LSEOF1;
    t_uint8     OTG_SOFTRST;

    /*
      Target Endpoint 0 control registers
    */
    t_uint8     OTG_TX0FAD;
    t_uint8     FILL4;
    t_uint8     OTG_TX0HAD;
    t_uint8     OTG_TX0HP;
    t_uint8     OTG_RX0FAD;
    t_uint8     FILL5;
    t_uint8     OTG_RX0HAD;
    t_uint8     OTG_RX0HP;

    /*
      Target Endpoint 1 control registers
    */
    t_uint8     OTG_TX1FAD;
    t_uint8     FILL6;
    t_uint8     OTG_TX1HAD;
    t_uint8     OTG_TX1HP;
    t_uint8     OTG_RX1FAD;
    t_uint8     FILL7;
    t_uint8     OTG_RX1HAD;
    t_uint8     OTG_RX1HP;

    /*
      Target Endpoint 2 control registers
    */
    t_uint8     OTG_TX2FAD;
    t_uint8     FILL8;
    t_uint8     OTG_TX2HAD;
    t_uint8     OTG_TX2HP;
    t_uint8     OTG_RX2FAD;
    t_uint8     FILL9;
    t_uint8     OTG_RX2HAD;
    t_uint8     OTG_RX2HP;

    /*
      Target Endpoint 3 control registers
    */
    t_uint8     OTG_TX3FAD;
    t_uint8     FILL10;
    t_uint8     OTG_TX3HAD;
    t_uint8     OTG_TX3HP;
    t_uint8     OTG_RX3FAD;
    t_uint8     FILL11;
    t_uint8     OTG_RX3HAD;
    t_uint8     OTG_RX3HP;

    /*
      Target Endpoint 4 control registers
    */
    t_uint8     OTG_TX4FAD;
    t_uint8     FILL12;
    t_uint8     OTG_TX4HAD;
    t_uint8     OTG_TX4HP;
    t_uint8     OTG_RX4FAD;
    t_uint8     FILL13;
    t_uint8     OTG_RX4HAD;
    t_uint8     OTG_RX4HP;

    /*
      Target Endpoint 5 control registers
    */
    t_uint8     OTG_TX5FAD;
    t_uint8     FILL14;
    t_uint8     OTG_TX5HAD;
    t_uint8     OTG_TX5HP;
    t_uint8     OTG_RX5FAD;
    t_uint8     FILL15;
    t_uint8     OTG_RX5HAD;
    t_uint8     OTG_RX5HP;

    /*
      Target Endpoint 6 control registers
    */
    t_uint8     OTG_TX6FAD;
    t_uint8     FILL16;
    t_uint8     OTG_TX6HAD;
    t_uint8     OTG_TX6HP;
    t_uint8     OTG_RX6FAD;
    t_uint8     FILL17;
    t_uint8     OTG_RX6HAD;
    t_uint8     OTG_RX6HP;

    /*
      Target Endpoint 7 control registers
    */
    t_uint8     OTG_TX7FAD;
    t_uint8     FILL18;
    t_uint8     OTG_TX7HAD;
    t_uint8     OTG_TX7HP;
    t_uint8     OTG_RX7FAD;
    t_uint8     FILL19;
    t_uint8     OTG_RX7HAD;
    t_uint8     OTG_RX7HP;

    /*
      Target Endpoint 8 control registers
    */
    t_uint8     OTG_TX8FAD;
    t_uint8     FILL20;
    t_uint8     OTG_TX8HAD;
    t_uint8     OTG_TX8HP;
    t_uint8     OTG_RX8FAD;
    t_uint8     FILL21;
    t_uint8     OTG_RX8HAD;
    t_uint8     OTG_RX8HP;

    /*
      Target Endpoint 9 control registers
    */
    t_uint8     OTG_TX9FAD;
    t_uint8     FILL22;
    t_uint8     OTG_TX9HAD;
    t_uint8     OTG_TX9HP;
    t_uint8     OTG_RX9FAD;
    t_uint8     FILL23;
    t_uint8     OTG_RX9HAD;
    t_uint8     OTG_RX9HP;

    /*
      Target Endpoint 10 control registers
    */
    t_uint8     OTG_TX10FAD;
    t_uint8     FILL24;
    t_uint8     OTG_TX10HAD;
    t_uint8     OTG_TX10HP;
    t_uint8     OTG_RX10FAD;
    t_uint8     FILL25;
    t_uint8     OTG_RX10HAD;
    t_uint8     OTG_RX10HP;

    /*
      Target Endpoint 11 control registers
    */
    t_uint8     OTG_TX11FAD;
    t_uint8     FILL26;
    t_uint8     OTG_TX11HAD;
    t_uint8     OTG_TX11HP;
    t_uint8     OTG_RX11FAD;
    t_uint8     FILL27;
    t_uint8     OTG_RX11HAD;
    t_uint8     OTG_RX11HP;

    /*
      Target Endpoint 12 control registers
    */
    t_uint8     OTG_TX12FAD;
    t_uint8     FILL28;
    t_uint8     OTG_TX12HAD;
    t_uint8     OTG_TX12HP;
    t_uint8     OTG_RX12FAD;
    t_uint8     FILL29;
    t_uint8     OTG_RX12HAD;
    t_uint8     OTG_RX12HP;

    /*
      Target Endpoint 13 control registers
    */
    t_uint8     OTG_TX13FAD;
    t_uint8     FILL30;
    t_uint8     OTG_TX13HAD;
    t_uint8     OTG_TX13HP;
    t_uint8     OTG_RX13FAD;
    t_uint8     FILL31;
    t_uint8     OTG_RX13HAD;
    t_uint8     OTG_RX13HP;

    /*
      Target Endpoint 14 control registers
    */
    t_uint8     OTG_TX14FAD;
    t_uint8     FILL32;
    t_uint8     OTG_TX14HAD;
    t_uint8     OTG_TX14HP;
    t_uint8     OTG_RX14FAD;
    t_uint8     FILL33;
    t_uint8     OTG_RX14HAD;
    t_uint8     OTG_RX14HP;

    /*
      Target Endpoint 15 control registers
    */
    t_uint8     OTG_TX15FAD;
    t_uint8     FILL34;
    t_uint8     OTG_TX15HAD;
    t_uint8     OTG_TX15HP;
    t_uint8     OTG_RX15FAD;
    t_uint8     FILL35;
    t_uint8     OTG_RX15HAD;
    t_uint8     OTG_RX15HP;

    /*
      Control Status register for Endpoint 0
    */
    t_uint16    OTG_TX0MAXP;
    t_uint16    OTG_CSR0;
    t_uint16    OTG_RX0MAXP;
    t_uint16    OTG_RX0CSR;
    t_uint16    OTG_CNT0;
    t_uint8     OTG_TYPE0;
    t_uint8     OTG_NAKLMT0;
    t_uint8     OTG_RX0TYPE;
    t_uint8     OTG_RX0INTV;
    t_uint8     FILL36;
    t_uint8     OTG_CFD;

    /*
      Control Status register for Endpoint 1
    */
    t_uint16    OTG_TX1MAXP;
    t_uint16    OTG_TX1CSR;
    t_uint16    OTG_RX1MAXP;
    t_uint16    OTG_RX1CSR;
    t_uint16    OTG_RX1CNT;
    t_uint8     OTG_TX1TYPE;
    t_uint8     OTG_TX1INTV;
    t_uint8     OTG_RX1TYPE;
    t_uint8     OTG_RX1INTV;
    t_uint8     FILL37;
    t_uint8     OTG_FSIZE1;

    /*
      Control Status register for Endpoint 2
    */
    t_uint16    OTG_TX2MAXP;
    t_uint16    OTG_TX2CSR;
    t_uint16    OTG_RX2MAXP;
    t_uint16    OTG_RX2CSR;
    t_uint16    OTG_RX2CNT;
    t_uint8     OTG_TX2TYPE;
    t_uint8     OTG_TX2INTV;
    t_uint8     OTG_RX2TYPE;
    t_uint8     OTG_RX2INTV;
    t_uint8     FILL38;
    t_uint8     OTG_FSIZE2;

    /*
      Control Status register for Endpoint 3
    */
    t_uint16    OTG_TX3MAXP;
    t_uint16    OTG_TX3CSR;
    t_uint16    OTG_RX3MAXP;
    t_uint16    OTG_RX3CSR;
    t_uint16    OTG_RX3CNT;
    t_uint8     OTG_TX3TYPE;
    t_uint8     OTG_TX3INTV;
    t_uint8     OTG_RX3TYPE;
    t_uint8     OTG_RX3INTV;
    t_uint8     FILL39;
    t_uint8     OTG_FSIZE3;

    /*
      Control Status register for Endpoint 4
    */
    t_uint16    OTG_TX4MAXP;
    t_uint16    OTG_TX4CSR;
    t_uint16    OTG_RX4MAXP;
    t_uint16    OTG_RX4CSR;
    t_uint16    OTG_RX4CNT;
    t_uint8     OTG_TX4TYPE;
    t_uint8     OTG_TX4INTV;
    t_uint8     OTG_RX4TYPE;
    t_uint8     OTG_RX4INTV;
    t_uint8     FILL40;
    t_uint8     OTG_FSIZE4;

    /* Control Status register for Endpoint 5 */
    t_uint16    OTG_TX5MAXP;
    t_uint16    OTG_TX5CSR;
    t_uint16    OTG_RX5MAXP;
    t_uint16    OTG_RX5CSR;
    t_uint16    OTG_RX5CNT;
    t_uint8     OTG_TX5TYPE;
    t_uint8     OTG_TX5INTV;
    t_uint8     OTG_RX5TYPE;
    t_uint8     OTG_RX5INTV;
    t_uint8     FILL41;
    t_uint8     OTG_FSIZE5;

    /*
      Control Status register for Endpoint 6
    */
    t_uint16    OTG_TX6MAXP;
    t_uint16    OTG_TX6CSR;
    t_uint16    OTG_RX6MAXP;
    t_uint16    OTG_RX6CSR;
    t_uint16    OTG_RX6CNT;
    t_uint8     OTG_TX6TYPE;
    t_uint8     OTG_TX6INTV;
    t_uint8     OTG_RX6TYPE;
    t_uint8     OTG_RX6INTV;
    t_uint8     FILL42;
    t_uint8     OTG_FSIZE6;

    /*
      Control Status register for Endpoint 7
    */
    t_uint16    OTG_TX7MAXP;
    t_uint16    OTG_TX7CSR;
    t_uint16    OTG_RX7MAXP;
    t_uint16    OTG_RX7CSR;
    t_uint16    OTG_RX7CNT;
    t_uint8     OTG_TX7TYPE;
    t_uint8     OTG_TX7INTV;
    t_uint8     OTG_RX7TYPE;
    t_uint8     OTG_RX7INTV;
    t_uint8     FILL43;
    t_uint8     OTG_FSIZE7;

    /*
      Control Status register for Endpoint 8
    */
    t_uint16    OTG_TX8MAXP;
    t_uint16    OTG_TX8CSR;
    t_uint16    OTG_RX8MAXP;
    t_uint16    OTG_RX8CSR;
    t_uint16    OTG_RX8CNT;
    t_uint8     OTG_TX8TYPE;
    t_uint8     OTG_TX8INTV;
    t_uint8     OTG_RX8TYPE;
    t_uint8     OTG_RX8INTV;
    t_uint8     FILL44;
    t_uint8     OTG_FSIZE8;

    /*
      Control Status register for Endpoint 9
    */
    t_uint16    OTG_TX9MAXP;
    t_uint16    OTG_TX9CSR;
    t_uint16    OTG_RX9MAXP;
    t_uint16    OTG_RX9CSR;
    t_uint16    OTG_RX9CNT;
    t_uint8     OTG_TX9TYPE;
    t_uint8     OTG_TX9INTV;
    t_uint8     OTG_RX9TYPE;
    t_uint8     OTG_RX9INTV;
    t_uint8     FILL45;
    t_uint8     OTG_FSIZE9;

    /*
      Control Status register for Endpoint 10
    */
    t_uint16    OTG_TX10MAXP;
    t_uint16    OTG_TX10CSR;
    t_uint16    OTG_RX10MAXP;
    t_uint16    OTG_RX10CSR;
    t_uint16    OTG_RX10CNT;
    t_uint8     OTG_TX10TYPE;
    t_uint8     OTG_TX10INTV;
    t_uint8     OTG_RX10TYPE;
    t_uint8     OTG_RX10INTV;
    t_uint8     FILL46;
    t_uint8     OTG_FSIZE10;

    /*
      Control Status register for Endpoint 11
    */
    t_uint16    OTG_TX11MAXP;
    t_uint16    OTG_TX11CSR;
    t_uint16    OTG_RX11MAXP;
    t_uint16    OTG_RX11CSR;
    t_uint16    OTG_RX11CNT;
    t_uint8     OTG_TX11TYPE;
    t_uint8     OTG_TX11INTV;
    t_uint8     OTG_RX11TYPE;
    t_uint8     OTG_RX11INTV;
    t_uint8     FILL47;
    t_uint8     OTG_FSIZE11;

    /*
      Control Status register for Endpoint 12
    */
    t_uint16    OTG_TX12MAXP;
    t_uint16    OTG_TX12CSR;
    t_uint16    OTG_RX12MAXP;
    t_uint16    OTG_RX12CSR;
    t_uint16    OTG_RX12CNT;
    t_uint8     OTG_TX12TYPE;
    t_uint8     OTG_TX12INTV;
    t_uint8     OTG_RX12TYPE;
    t_uint8     OTG_RX12INTV;
    t_uint8     FILL48;
    t_uint8     OTG_FSIZE12;

    /*
      Control Status register for Endpoint 13
    */
    t_uint16    OTG_TX13MAXP;
    t_uint16    OTG_TX13CSR;
    t_uint16    OTG_RX13MAXP;
    t_uint16    OTG_RX13CSR;
    t_uint16    OTG_RX13CNT;
    t_uint8     OTG_TX13TYPE;
    t_uint8     OTG_TX13INTV;
    t_uint8     OTG_RX13TYPE;
    t_uint8     OTG_RX13INTV;
    t_uint8     FILL49;
    t_uint8     OTG_FSIZE13;

    /*
      Control Status register for Endpoint 14
    */
    t_uint16    OTG_TX14MAXP;
    t_uint16    OTG_TX14CSR;
    t_uint16    OTG_RX14MAXP;
    t_uint16    OTG_RX14CSR;
    t_uint16    OTG_RX14CNT;
    t_uint8     OTG_TX14TYPE;
    t_uint8     OTG_TX14INTV;
    t_uint8     OTG_RX14TYPE;
    t_uint8     OTG_RX14INTV;
    t_uint8     FILL50;
    t_uint8     OTG_FSIZE14;

    /*
      Control Status register for Endpoint 15
    */
    t_uint16    OTG_TX15MAXP;
    t_uint16    OTG_TX15CSR;
    t_uint16    OTG_RX15MAXP;
    t_uint16    OTG_RX15CSR;
    t_uint16    OTG_RX15CNT;
    t_uint8     OTG_TX15TYPE;
    t_uint8     OTG_TX15INTV;
    t_uint8     OTG_RX15TYPE;
    t_uint8     OTG_RX15INTV;
    t_uint8     FILL51;
    t_uint8     OTG_FSIZE15;

    t_uint32    OTG_DMASEL;
    t_uint8     OTG_TOPCTRL;
} t_usb_0_register;

#else

typedef volatile struct
{
    t_uint8     OTG_FADDR;            /* 0x000 */
    t_uint8     OTG_PWR;              /* 0x001 */
    t_uint16    OTG_INTTX;            /* 0x002 */
    t_uint16    OTG_INTRX;            /* 0x004 */
    t_uint16    OTG_INTTXEN;          /* 0x006 */
    t_uint16    OTG_INTRXEN;          /* 0x008 */
    t_uint8     OTG_INTUSB;           /* 0x00A */
    t_uint8     OTG_INTUSBEN;         /* 0x00B */
    t_uint16    OTG_FMNO;             /* 0x00C */
    t_uint8     OTG_INDX;             /* 0x00E */
    t_uint8     OTG_TM;               /* 0x00F */

    /* Selected through Index */
    t_uint16    OTG_TXMAXP;           /* 0x010 */
    t_uint16    OTG_CSR0_TXCSR;       /* 0x012 */
    t_uint16    OTG_RXMAXP;           /* 0x014 */
    t_uint16    OTG_RXCSR;            /* 0x016 */
    t_uint16    OTG_CNT0_RXCNT;       /* 0x018 */

    t_uint8     OTG_TYPE0_TXTYPE;     /* 0x01A (only Host) */
    t_uint8     OTG_NAKLMT0_TXINTV;   /* 0x01B (only Host) */
    t_uint8     OTG_RXTYPE;           /* 0x01C (only Host) */
    t_uint8     OTG_RXINTV;           /* 0x01D (only Host) */
    t_uint8     FILL0;                /* 0x01E (only Host) */
    t_uint8     OTG_CFD_FSIZE;        /* 0x01F (Only when Index is 0) */

    /* FIFO registers */
    t_uint32    OTG_FIFO0;            /* 0x020 */
    t_uint32    OTG_FIFO1;            /* 0x024 */
    t_uint32    OTG_FIFO2;            /* 0x028 */
    t_uint32    OTG_FIFO3;            /* 0x02C */
    t_uint32    OTG_FIFO4;            /* 0x030 */
    t_uint32    OTG_FIFO5;            /* 0x034 */
    t_uint32    OTG_FIFO6;            /* 0x038 */
    t_uint32    OTG_FIFO7;            /* 0x03C */
    t_uint32    OTG_FIFO8;            /* 0x040 */
    t_uint32    OTG_FIFO9;            /* 0x044 */
    t_uint32    OTG_FIFO10;           /* 0x048 */
    t_uint32    OTG_FIFO11;           /* 0x04C */
    t_uint32    OTG_FIFO12;           /* 0x050 */
    t_uint32    OTG_FIFO13;           /* 0x054 */
    t_uint32    OTG_FIFO14;           /* 0x058 */
    t_uint32    OTG_FIFO15;           /* 0x05C */
    t_uint8     OTG_DEVCTL;           /* 0x060 */
    t_uint8     OTG_MISC_EDMA;        /* 0x061 */
    t_uint8     OTG_TXFSZ;            /* 0x062 */
    t_uint8     OTG_RXFSZ;            /* 0x063 */
    t_uint16    OTG_TXFA;             /* 0x064 */
    t_uint16    OTG_RXFA;             /* 0x066 */
    t_uint32    OTG_VCNTL;            /* 0x068 */
    t_uint16    OTG_HWVER;            /* 0x06C */
    t_uint16    FILL1;                /* 0x06E */
    t_uint8     OTG_UVBCTRL;          /* 0x070 */
    t_uint8     OTG_UCKIT;            /* 0x071 */
    t_uint8     OTG_UINTMASK;         /* 0x072 */
    t_uint8     OTG_UINTSRC;          /* 0x073 */
    t_uint8     OTG_UREGDATA;         /* 0x074 */
    t_uint8     OTG_UREGADDR;         /* 0x075 */
    t_uint8     OTG_UREGCTRL;         /* 0x076 */
    t_uint8     OTG_URAWDATA;         /* 0x077 */
    t_uint8     OTG_EPINFO;           /* 0x078 */
    t_uint8     OTG_RAMINFO;          /* 0x079 */
    t_uint8     OTG_LINKINFO;         /* 0x07A */
    t_uint8     OTG_VPLEN;            /* 0x07B */
    t_uint8     OTG_HSEOF1;           /* 0x07C */
    t_uint8     OTG_FSEOF1;           /* 0x07D */
    t_uint8     OTG_LSEOF1;           /* 0x07E */
    t_uint8     OTG_SOFTRST;          /* 0x07F */

    /*
      Target Endpoint 0 control registers
    */
    t_uint8     OTG_TX0FAD;           /* 0x080 */
    t_uint8     FILL2;
    t_uint8     OTG_TX0HAD;
    t_uint8     OTG_TX0HP;
    t_uint8     OTG_RX0FAD;
    t_uint8     FILL3;
    t_uint8     OTG_RX0HAD;
    t_uint8     OTG_RX0HP;

    /*
      Target Endpoint 1 control registers
    */
    t_uint8     OTG_TX1FAD;
    t_uint8     FILL4;
    t_uint8     OTG_TX1HAD;
    t_uint8     OTG_TX1HP;
    t_uint8     OTG_RX1FAD;
    t_uint8     FILL5;
    t_uint8     OTG_RX1HAD;
    t_uint8     OTG_RX1HP;

    /*
      Target Endpoint 2 control registers
    */
    t_uint8     OTG_TX2FAD;           /* 0x090 */
    t_uint8     FILL6;
    t_uint8     OTG_TX2HAD;
    t_uint8     OTG_TX2HP;
    t_uint8     OTG_RX2FAD;
    t_uint8     FILL7;
    t_uint8     OTG_RX2HAD;
    t_uint8     OTG_RX2HP;

    /*
      Target Endpoint 3 control registers
    */
    t_uint8     OTG_TX3FAD;
    t_uint8     FILL8;
    t_uint8     OTG_TX3HAD;
    t_uint8     OTG_TX3HP;
    t_uint8     OTG_RX3FAD;
    t_uint8     FILL9;
    t_uint8     OTG_RX3HAD;
    t_uint8     OTG_RX3HP;

    /*
      Target Endpoint 4 control registers
    */
    t_uint8     OTG_TX4FAD;           /* 0x0A0 */
    t_uint8     FILL10;
    t_uint8     OTG_TX4HAD;
    t_uint8     OTG_TX4HP;
    t_uint8     OTG_RX4FAD;
    t_uint8     FILL11;
    t_uint8     OTG_RX4HAD;
    t_uint8     OTG_RX4HP;

    /*
      Target Endpoint 5 control registers
    */
    t_uint8     OTG_TX5FAD;
    t_uint8     FILL12;
    t_uint8     OTG_TX5HAD;
    t_uint8     OTG_TX5HP;
    t_uint8     OTG_RX5FAD;
    t_uint8     FILL13;
    t_uint8     OTG_RX5HAD;
    t_uint8     OTG_RX5HP;

    /*
      Target Endpoint 6 control registers
    */
    t_uint8     OTG_TX6FAD;           /* 0x0B0 */
    t_uint8     FILL14;
    t_uint8     OTG_TX6HAD;
    t_uint8     OTG_TX6HP;
    t_uint8     OTG_RX6FAD;
    t_uint8     FILL15;
    t_uint8     OTG_RX6HAD;
    t_uint8     OTG_RX6HP;

    /*
      Target Endpoint 7 control registers
    */
    t_uint8     OTG_TX7FAD;
    t_uint8     FILL16;
    t_uint8     OTG_TX7HAD;
    t_uint8     OTG_TX7HP;
    t_uint8     OTG_RX7FAD;
    t_uint8     FILL17;
    t_uint8     OTG_RX7HAD;
    t_uint8     OTG_RX7HP;

    /*
      Target Endpoint 8 control registers
    */
    t_uint8     OTG_TX8FAD;           /* 0x0C0 */
    t_uint8     FILL18;
    t_uint8     OTG_TX8HAD;
    t_uint8     OTG_TX8HP;
    t_uint8     OTG_RX8FAD;
    t_uint8     FILL19;
    t_uint8     OTG_RX8HAD;
    t_uint8     OTG_RX8HP;

    /*
      Target Endpoint 9 control registers
    */
    t_uint8     OTG_TX9FAD;
    t_uint8     FILL20;
    t_uint8     OTG_TX9HAD;
    t_uint8     OTG_TX9HP;
    t_uint8     OTG_RX9FAD;
    t_uint8     FILL21;
    t_uint8     OTG_RX9HAD;
    t_uint8     OTG_RX9HP;

    /*
      Target Endpoint 10 control registers
    */
    t_uint8     OTG_TX10FAD;           /* 0x0D0 */
    t_uint8     FILL22;
    t_uint8     OTG_TX10HAD;
    t_uint8     OTG_TX10HP;
    t_uint8     OTG_RX10FAD;
    t_uint8     FILL23;
    t_uint8     OTG_RX10HAD;
    t_uint8     OTG_RX10HP;

    /*
      Target Endpoint 11 control registers
    */
    t_uint8     OTG_TX11FAD;
    t_uint8     FILL24;
    t_uint8     OTG_TX11HAD;
    t_uint8     OTG_TX11HP;
    t_uint8     OTG_RX11FAD;
    t_uint8     FILL25;
    t_uint8     OTG_RX11HAD;
    t_uint8     OTG_RX11HP;

    /*
      Target Endpoint 12 control registers
    */
    t_uint8     OTG_TX12FAD;           /* 0x0E0 */
    t_uint8     FILL26;
    t_uint8     OTG_TX12HAD;
    t_uint8     OTG_TX12HP;
    t_uint8     OTG_RX12FAD;
    t_uint8     FILL27;
    t_uint8     OTG_RX12HAD;
    t_uint8     OTG_RX12HP;

    /*
      Target Endpoint 13 control registers
    */
    t_uint8     OTG_TX13FAD;
    t_uint8     FILL28;
    t_uint8     OTG_TX13HAD;
    t_uint8     OTG_TX13HP;
    t_uint8     OTG_RX13FAD;
    t_uint8     FILL29;
    t_uint8     OTG_RX13HAD;
    t_uint8     OTG_RX13HP;

    /*
      Target Endpoint 14 control registers
    */
    t_uint8     OTG_TX14FAD;           /* 0x0F0 */
    t_uint8     FILL30;
    t_uint8     OTG_TX14HAD;
    t_uint8     OTG_TX14HP;
    t_uint8     OTG_RX14FAD;
    t_uint8     FILL31;
    t_uint8     OTG_RX14HAD;
    t_uint8     OTG_RX14HP;

    /*
      Target Endpoint 15 control registers
    */
    t_uint8     OTG_TX15FAD;
    t_uint8     FILL32;
    t_uint8     OTG_TX15HAD;
    t_uint8     OTG_TX15HP;
    t_uint8     OTG_RX15FAD;
    t_uint8     FILL33;
    t_uint8     OTG_RX15HAD;
    t_uint8     OTG_RX15HP;

    /*
      Control Status register for Endpoint 0
    */
    t_uint16    FILL34;                /* 0x100 */
    t_uint16    OTG_CSR0;              /* 0x102 */
    t_uint16    FILL35[(0x108-0x104)/2];
    t_uint16    OTG_CNT0;              /* 0x108 */
    t_uint8     FILL36[(0x10F-0x10A)];
    t_uint8     OTG_CFD;               /* 0x10F */

    /*
      Control Status register for Endpoint 1
    */
    t_uint16    OTG_TX1MAXP;           /* 0x110 */
    t_uint16    OTG_TX1CSR;
    t_uint16    OTG_RX1MAXP;
    t_uint16    OTG_RX1CSR;
    t_uint16    OTG_RX1CNT;
    t_uint8     OTG_TX1TYPE;
    t_uint8     OTG_TX1INTV;
    t_uint8     OTG_RX1TYPE;
    t_uint8     OTG_RX1INTV;
    t_uint8     FILL37;
    t_uint8     OTG_FSIZE1;

    /*
      Control Status register for Endpoint 2
    */
    t_uint16    OTG_TX2MAXP;           /* 0x120 */
    t_uint16    OTG_TX2CSR;
    t_uint16    OTG_RX2MAXP;
    t_uint16    OTG_RX2CSR;
    t_uint16    OTG_RX2CNT;
    t_uint8     OTG_TX2TYPE;
    t_uint8     OTG_TX2INTV;
    t_uint8     OTG_RX2TYPE;
    t_uint8     OTG_RX2INTV;
    t_uint8     FILL38;
    t_uint8     OTG_FSIZE2;

    /*
      Control Status register for Endpoint 3
    */
    t_uint16    OTG_TX3MAXP;           /* 0x130 */
    t_uint16    OTG_TX3CSR;
    t_uint16    OTG_RX3MAXP;
    t_uint16    OTG_RX3CSR;
    t_uint16    OTG_RX3CNT;
    t_uint8     OTG_TX3TYPE;
    t_uint8     OTG_TX3INTV;
    t_uint8     OTG_RX3TYPE;
    t_uint8     OTG_RX3INTV;
    t_uint8     FILL39;
    t_uint8     OTG_FSIZE3;

    /*
      Control Status register for Endpoint 4
    */
    t_uint16    OTG_TX4MAXP;           /* 0x140 */
    t_uint16    OTG_TX4CSR;
    t_uint16    OTG_RX4MAXP;
    t_uint16    OTG_RX4CSR;
    t_uint16    OTG_RX4CNT;
    t_uint8     OTG_TX4TYPE;
    t_uint8     OTG_TX4INTV;
    t_uint8     OTG_RX4TYPE;
    t_uint8     OTG_RX4INTV;
    t_uint8     FILL40;
    t_uint8     OTG_FSIZE4;

    /* Control Status register for Endpoint 5 */
    t_uint16    OTG_TX5MAXP;           /* 0x150 */
    t_uint16    OTG_TX5CSR;
    t_uint16    OTG_RX5MAXP;
    t_uint16    OTG_RX5CSR;
    t_uint16    OTG_RX5CNT;
    t_uint8     OTG_TX5TYPE;
    t_uint8     OTG_TX5INTV;
    t_uint8     OTG_RX5TYPE;
    t_uint8     OTG_RX5INTV;
    t_uint8     FILL41;
    t_uint8     OTG_FSIZE5;

    /*
      Control Status register for Endpoint 6
    */
    t_uint16    OTG_TX6MAXP;           /* 0x160 */
    t_uint16    OTG_TX6CSR;
    t_uint16    OTG_RX6MAXP;
    t_uint16    OTG_RX6CSR;
    t_uint16    OTG_RX6CNT;
    t_uint8     OTG_TX6TYPE;
    t_uint8     OTG_TX6INTV;
    t_uint8     OTG_RX6TYPE;
    t_uint8     OTG_RX6INTV;
    t_uint8     FILL42;
    t_uint8     OTG_FSIZE6;

    /*
      Control Status register for Endpoint 7
    */
    t_uint16    OTG_TX7MAXP;           /* 0x170 */
    t_uint16    OTG_TX7CSR;
    t_uint16    OTG_RX7MAXP;
    t_uint16    OTG_RX7CSR;
    t_uint16    OTG_RX7CNT;
    t_uint8     OTG_TX7TYPE;
    t_uint8     OTG_TX7INTV;
    t_uint8     OTG_RX7TYPE;
    t_uint8     OTG_RX7INTV;
    t_uint8     FILL43;
    t_uint8     OTG_FSIZE7;

    /*
      Control Status register for Endpoint 8
    */
    t_uint16    OTG_TX8MAXP;           /* 0x180 */
    t_uint16    OTG_TX8CSR;
    t_uint16    OTG_RX8MAXP;
    t_uint16    OTG_RX8CSR;
    t_uint16    OTG_RX8CNT;
    t_uint8     OTG_TX8TYPE;
    t_uint8     OTG_TX8INTV;
    t_uint8     OTG_RX8TYPE;
    t_uint8     OTG_RX8INTV;
    t_uint8     FILL44;
    t_uint8     OTG_FSIZE8;

    /*
      Control Status register for Endpoint 9
    */
    t_uint16    OTG_TX9MAXP;           /* 0x190 */
    t_uint16    OTG_TX9CSR;
    t_uint16    OTG_RX9MAXP;
    t_uint16    OTG_RX9CSR;
    t_uint16    OTG_RX9CNT;
    t_uint8     OTG_TX9TYPE;
    t_uint8     OTG_TX9INTV;
    t_uint8     OTG_RX9TYPE;
    t_uint8     OTG_RX9INTV;
    t_uint8     FILL45;
    t_uint8     OTG_FSIZE9;

    /*
      Control Status register for Endpoint 10
    */
    t_uint16    OTG_TX10MAXP;           /* 0x1A0 */
    t_uint16    OTG_TX10CSR;
    t_uint16    OTG_RX10MAXP;
    t_uint16    OTG_RX10CSR;
    t_uint16    OTG_RX10CNT;
    t_uint8     OTG_TX10TYPE;
    t_uint8     OTG_TX10INTV;
    t_uint8     OTG_RX10TYPE;
    t_uint8     OTG_RX10INTV;
    t_uint8     FILL46;
    t_uint8     OTG_FSIZE10;

    /*
      Control Status register for Endpoint 11
    */
    t_uint16    OTG_TX11MAXP;           /* 0x1B0 */
    t_uint16    OTG_TX11CSR;
    t_uint16    OTG_RX11MAXP;
    t_uint16    OTG_RX11CSR;
    t_uint16    OTG_RX11CNT;
    t_uint8     OTG_TX11TYPE;
    t_uint8     OTG_TX11INTV;
    t_uint8     OTG_RX11TYPE;
    t_uint8     OTG_RX11INTV;
    t_uint8     FILL47;
    t_uint8     OTG_FSIZE11;

    /*
      Control Status register for Endpoint 12
    */
    t_uint16    OTG_TX12MAXP;           /* 0x1C0 */
    t_uint16    OTG_TX12CSR;
    t_uint16    OTG_RX12MAXP;
    t_uint16    OTG_RX12CSR;
    t_uint16    OTG_RX12CNT;
    t_uint8     OTG_TX12TYPE;
    t_uint8     OTG_TX12INTV;
    t_uint8     OTG_RX12TYPE;
    t_uint8     OTG_RX12INTV;
    t_uint8     FILL48;
    t_uint8     OTG_FSIZE12;

    /*
      Control Status register for Endpoint 13
    */
    t_uint16    OTG_TX13MAXP;           /* 0x1D0 */
    t_uint16    OTG_TX13CSR;
    t_uint16    OTG_RX13MAXP;
    t_uint16    OTG_RX13CSR;
    t_uint16    OTG_RX13CNT;
    t_uint8     OTG_TX13TYPE;
    t_uint8     OTG_TX13INTV;
    t_uint8     OTG_RX13TYPE;
    t_uint8     OTG_RX13INTV;
    t_uint8     FILL49;
    t_uint8     OTG_FSIZE13;

    /*
      Control Status register for Endpoint 14
    */
    t_uint16    OTG_TX14MAXP;           /* 0x1E0 */
    t_uint16    OTG_TX14CSR;
    t_uint16    OTG_RX14MAXP;
    t_uint16    OTG_RX14CSR;
    t_uint16    OTG_RX14CNT;
    t_uint8     OTG_TX14TYPE;
    t_uint8     OTG_TX14INTV;
    t_uint8     OTG_RX14TYPE;
    t_uint8     OTG_RX14INTV;
    t_uint8     FILL50;
    t_uint8     OTG_FSIZE14;

    /*
      Control Status register for Endpoint 15
    */
    t_uint16    OTG_TX15MAXP;          /* 0x1F0 */
    t_uint16    OTG_TX15CSR;
    t_uint16    OTG_RX15MAXP;
    t_uint16    OTG_RX15CSR;
    t_uint16    OTG_RX15CNT;
    t_uint8     OTG_TX15TYPE;
    t_uint8     OTG_TX15INTV;
    t_uint8     OTG_RX15TYPE;
    t_uint8     OTG_RX15INTV;
    t_uint8     FILL51;
    t_uint8     OTG_FSIZE15;

    t_uint32    OTG_FILL52[(0x300-0x200)/4]; /* 0x200-0x2FF */

    t_uint16    OTG_RX0PKTCNT;         /* 0x300 (Only Host)*/
    t_uint16    OTG_FILL53;            /* 0x302 */
    t_uint16    OTG_RX1PKTCNT;         /* 0x304 (Only Host)*/
    t_uint16    OTG_FILL54;            /* 0x306 */
    t_uint16    OTG_RX2PKTCNT;         /* 0x308 (Only Host)*/
    t_uint16    OTG_FILL55;            /* 0x30A */
    t_uint16    OTG_RX3PKTCNT;         /* 0x30C (Only Host)*/
    t_uint16    OTG_FILL56;            /* 0x30E */
    t_uint16    OTG_RX4PKTCNT;         /* 0x310 (Only Host)*/
    t_uint16    OTG_FILL57;            /* 0x312 */
    t_uint16    OTG_RX5PKTCNT;         /* 0x314 (Only Host)*/
    t_uint16    OTG_FILL58;            /* 0x316 */
    t_uint16    OTG_RX6PKTCNT;         /* 0x318 (Only Host)*/
    t_uint16    OTG_FILL59;            /* 0x31A */
    t_uint16    OTG_RX7PKTCNT;         /* 0x31C (Only Host)*/
    t_uint16    OTG_FILL60;            /* 0x31E */
    t_uint16    OTG_RX8PKTCNT;         /* 0x320 (Only Host)*/
    t_uint16    OTG_FILL61;            /* 0x322 */
    t_uint16    OTG_RX9PKTCNT;         /* 0x324 (Only Host)*/
    t_uint16    OTG_FILL62;            /* 0x326 */
    t_uint16    OTG_RX10PKTCNT;        /* 0x328 (Only Host)*/
    t_uint16    OTG_FILL63;            /* 0x32A */
    t_uint16    OTG_RX11PKTCNT;        /* 0x32C (Only Host)*/
    t_uint16    OTG_FILL64;            /* 0x32E */
    t_uint16    OTG_RX12PKTCNT;        /* 0x330 (Only Host)*/
    t_uint16    OTG_FILL65;            /* 0x332 */
    t_uint16    OTG_RX13PKTCNT;        /* 0x334 (Only Host)*/
    t_uint16    OTG_FILL66;            /* 0x336 */
    t_uint16    OTG_RX14PKTCNT;        /* 0x338 (Only Host)*/
    t_uint16    OTG_FILL67;            /* 0x33A */
    t_uint16    OTG_RX15PKTCNT;        /* 0x33C (Only Host)*/
    t_uint16    OTG_FILL68;            /* 0x33E */

    t_uint16    OTG_RXDBDIS;           /* 0x340 */
    t_uint16    OTG_TXDBDIS;           /* 0x342 */
    t_uint16    OTG_UCH;               /* 0x344 */
    t_uint16    OTG_HSRTN;             /* 0x346 */

    t_uint16    OTG_FILL69[(0x360-0x348)/4];   /* 0x348-0x360 */

    t_uint16    OTG_LPMATTR;           /* 0x360 */
    t_uint8     OTG_LPMCNTRL;          /* 0x362 */
    t_uint8     OTG_LPMINTREN;         /* 0x363 */
    t_uint8     OTG_LPMINTR;           /* 0x364 */
    t_uint8     OTG_LPMFADDR;          /* 0x365 (Only Host) */
} t_usb_0_register;
#endif


/*
  Rx FIFO Base Address.
*/
#define USB_DEV_RXFIFOREG(i)

/*
  Tx FIFO Base Address.
*/
#define USB_DEV_TXFIFOREG(i)

#define OTG_FADDR_MASK              0x7F
#define OTG_FADDR_SHIFT             0x00

#define OTG_PWR_ISOUP_MASK          MASK_BIT7
#define OTG_PWR_SFCN_MASK           MASK_BIT6
#define OTG_PWR_HSEN_MASK           MASK_BIT5
#define OTG_PWR_HSMODE_MASK         MASK_BIT4
#define OTG_PWR_RST_MASK            MASK_BIT3
#define OTG_PWR_RSM_MASK            MASK_BIT2
#define OTG_PWR_SUSP_MASK           MASK_BIT1
#define OTG_PWR_ENSUSPM_MASK        MASK_BIT0

#define OTG_INTEP0_MASK             MASK_BIT0
#define OTG_INTEP1_MASK             MASK_BIT1
#define OTG_INTEP2_MASK             MASK_BIT2
#define OTG_INTEP3_MASK             MASK_BIT3
#define OTG_INTEP4_MASK             MASK_BIT4
#define OTG_INTEP5_MASK             MASK_BIT5
#define OTG_INTEP6_MASK             MASK_BIT6
#define OTG_INTEP7_MASK             MASK_BIT7
#define OTG_INTEP8_MASK             MASK_BIT8
#define OTG_INTEP9_MASK             MASK_BIT9
#define OTG_INTEP10_MASK            MASK_BIT10
#define OTG_INTEP11_MASK            MASK_BIT11
#define OTG_INTEP12_MASK            MASK_BIT12
#define OTG_INTEP13_MASK            MASK_BIT13
#define OTG_INTEP14_MASK            MASK_BIT14
#define OTG_INTEP15_MASK            MASK_BIT15

#define OTG_INTUSB_VBE_MASK         MASK_BIT7
#define OTG_INTUSB_SREQ_MASK        MASK_BIT6
#define OTG_INTUSB_DISCON_MASK      MASK_BIT5
#define OTG_INTUSB_CONN_MASK        MASK_BIT4
#define OTG_INTUSB_SOF_MASK         MASK_BIT3
#define OTG_INTUSB_RST_MASK         MASK_BIT2
#define OTG_INTUSB_RES_MASK         MASK_BIT1
#define OTG_INTUSB_SUSP_MASK        MASK_BIT0

#define OTG_FMNO_MASK               0x7FF

#define OTG_ENDPNO_INDX_MASK        0x0F

#define OTG_TM_FRH_MASK             MASK_BIT7
#define OTG_TM_FIFOA_MASK           MASK_BIT6
#define OTG_TM_FFS_MASK             MASK_BIT5
#define OTG_TM_FHS_MASK             MASK_BIT4
#define OTG_TM_TESTPKT_MASK         MASK_BIT3
#define OTG_TM_TSTK_MASK            MASK_BIT2
#define OTG_TM_TSTJ_MASK            MASK_BIT1
#define OTG_TM_TSTNAK_MASK          MASK_BIT0

#define OTG_MAXP_MASK               0x7FF
#define OTG_MAXP_MULT_MASK          0xF800
#define OTG_MAXP_MULT_SHIFT         11

#define OTG_DTXCSR_ASET_MASK        MASK_BIT15
#define OTG_DTXCSR_ISO_MASK         MASK_BIT14
#define OTG_DTXCSR_MD_MASK          MASK_BIT13
#define OTG_DTXCSR_DMAR_MASK        MASK_BIT12
#define OTG_DTXCSR_FDT_MASK         MASK_BIT11
#define OTG_DTXCSR_DRM_MASK         MASK_BIT10
#define OTG_DTXCSR_ITX_MASK         MASK_BIT7
#define OTG_DTXCSR_CLR_MASK         MASK_BIT6
#define OTG_DTXCSR_SNT_MASK         MASK_BIT5
#define OTG_DTXCSR_SND_MASK         MASK_BIT4
#define OTG_DTXCSR_FFLU_MASK        MASK_BIT3
#define OTG_DTXCSR_UR_MASK          MASK_BIT2
#define OTG_DTXCSR_FNE_MASK         MASK_BIT1
#define OTG_DTXCSR_TRDY_MASK        MASK_BIT0

#define OTG_HTXCSR_ASET_MASK        MASK_BIT15
#define OTG_HTXCSR_MD_MASK          MASK_BIT13
#define OTG_HTXCSR_DMAR_MASK        MASK_BIT12
#define OTG_HTXCSR_FDT_MASK         MASK_BIT11
#define OTG_HTXCSR_DRM_MASK         MASK_BIT10
#define OTG_HTXCSR_DWE_MASK         MASK_BIT9
#define OTG_HTXCSR_DT_MASK          MASK_BIT8
#define OTG_HTXCSR_NAK_MASK         MASK_BIT7
#define OTG_HTXCSR_CLR_MASK         MASK_BIT6
#define OTG_HTXCSR_RXS_MASK         MASK_BIT5
#define OTG_HTXCSR_STP_MASK         MASK_BIT4
#define OTG_HTXCSR_FFLU_MASK        MASK_BIT3
#define OTG_HTXCSR_ERR_MASK         MASK_BIT2
#define OTG_HTXCSR_FNE_MASK         MASK_BIT1
#define OTG_HTXCSR_TRDY_MASK        MASK_BIT0

#define OTG_DRXCSR_ACLR_MASK        MASK_BIT15
#define OTG_DRXCSR_ISO_MASK         MASK_BIT14
#define OTG_DRXCSR_DMAR_MASK        MASK_BIT13
#define OTG_DRXCSR_DNY_MASK         MASK_BIT12
#define OTG_DRXCSR_DRM_MASK         MASK_BIT11
#define OTG_DRXCSR_IRX_MASK         MASK_BIT8
#define OTG_DRXCSR_CLR_MASK         MASK_BIT7
#define OTG_DRXCSR_SNT_MASK         MASK_BIT6
#define OTG_DRXCSR_SND_MASK         MASK_BIT5
#define OTG_DRXCSR_FFLU_MASK        MASK_BIT4
#define OTG_DRXCSR_ERR_MASK         MASK_BIT3
#define OTG_DRXCSR_OR_MASK          MASK_BIT2
#define OTG_DRXCSR_FFULL_MASK       MASK_BIT1
#define OTG_DRXCSR_RRDY_MASK        MASK_BIT0

#define OTG_HRXCSR_ASET_MASK        MASK_BIT15
#define OTG_HRXCSR_ARQ_MASK         MASK_BIT14
#define OTG_HRXCSR_DMAR_MASK        MASK_BIT13
#define OTG_HRXCSR_DNY_MASK         MASK_BIT12
#define OTG_HRXCSR_DRM_MASK         MASK_BIT11
#define OTG_HRXCSR_DWE_MASK         MASK_BIT10
#define OTG_HRXCSR_DT_MASK          MASK_BIT9
#define OTG_HRXCSR_IRX_MASK         MASK_BIT8
#define OTG_HRXCSR_CLR_MASK         MASK_BIT7
#define OTG_HRXCSR_RXSTALL_MASK     MASK_BIT6
#define OTG_HRXCSR_RPK_MASK         MASK_BIT5
#define OTG_HRXCSR_FFLU_MASK        MASK_BIT4
#define OTG_HRXCSR_DERR_MASK        MASK_BIT3
#define OTG_HRXCSR_ERR_MASK         MASK_BIT2
#define OTG_HRXCSR_FFULL_MASK       MASK_BIT1
#define OTG_HRXCSR_RRDY_MASK        MASK_BIT0

#define OTG_DCSR0_FFLUSH_MASK       MASK_BIT8
#define OTG_DCSR0_SERSETEND_MASK    MASK_BIT7
#define OTG_DCSR0_SERRDY_MASK       MASK_BIT6
#define OTG_DCSR0_SNDSTALL_MASK     MASK_BIT5
#define OTG_DCSR0_SETUPEND_MASK     MASK_BIT4
#define OTG_DCSR0_DATAEND_MASK      MASK_BIT3
#define OTG_DCSR0_SNTSTALL_MASK     MASK_BIT2
#define OTG_DCSR0_TRDY_MASK         MASK_BIT1
#define OTG_DCSR0_RRDY_MASK         MASK_BIT0

#define OTG_HCSR0_DISPING           MASK_BIT11
#define OTG_HCSR0_DTWEN_MASK        MASK_BIT10
#define OTG_HCSR0_DT_MASK           MASK_BIT9
#define OTG_HCSR0_FFLUSH_MASK       MASK_BIT8
#define OTG_HCSR0_NAKTO_MASK        MASK_BIT7
#define OTG_HCSR0_STATUSPKT_MASK    MASK_BIT6
#define OTG_HCSR0_REQPKT_MASK       MASK_BIT5
#define OTG_HCSR0_ERROR_MASK        MASK_BIT4
#define OTG_HCSR0_SETUPPKT_MASK     MASK_BIT3
#define OTG_HCSR0_RXSTALL_MASK      MASK_BIT2
#define OTG_HCSR0_TRDY_MASK         MASK_BIT1
#define OTG_HCSR0_RRDY_MASK         MASK_BIT0

#define OTG_RXCNT_MASK              0x1FFF

#define OTG_TXRXTYPE_SPEED_MASK     0xC0
#define OTG_TXRXTYPE_PROT_MASK      0x30
#define OTG_TXRXTYPE_EP_MASK        0x0F

#define OTG_TXRXTYPE_SPEED_SHIFT    6
#define OTG_TXRXTYPE_PROT_SHIFT     4
#define OTG_TXRXTYPE_EP_SHIFT       0

#define OTG_TXRXINTV_MASK           MASK_ALL8

#define OTG_CFD_MPRXE_MASK          MASK_BIT7
#define OTG_CFD_MPTXE_MASK          MASK_BIT6
#define OTG_CFD_BE_MASK             MASK_BIT5
#define OTG_CFD_HBRXE_MASK          MASK_BIT4
#define OTG_CFD_HBTXE_MASK          MASK_BIT3
#define OTG_CFD_DNYF_MASK           MASK_BIT2
#define OTG_CFD_SC_MASK             MASK_BIT1
#define OTG_CFD_UI_MASK             MASK_BIT0

#define OTG_DEVCTL_BDEV_MASK        MASK_BIT7
#define OTG_DEVCTL_FSDEV_MASK       MASK_BIT6
#define OTG_DEVCTL_LSDEV_MASK       MASK_BIT5
#define OTG_DEVCTL_VBUS_MASK        (MASK_BIT4 | MASK_BIT3)
#define OTG_DEVCTL_HOST_MASK        MASK_BIT2
#define OTG_DEVCTL_HREQ_MASK        MASK_BIT1
#define OTG_DEVCTL_SESS_MASK        MASK_BIT0

#define OTG_DEVCTL_VBUS_SHIFT       3

#define OTG_FIFOSZ_DPB_MASK         MASK_BIT4

#define OTG_FIFOA_MASK              0x1FFF

#define OTG_FAD_MASK                0x7F

#define OTG_HAD_MASK                0x7F
#define OTG_HAD_MLT_MASK            MASK_BIT7

#define OTG_HP_MASK                 0x7F

#ifdef ST_8500ED
#define OTG_TOPCTRL_VBUSLO_MASK     MASK_BIT6
#define OTG_TOPCTRL_HDEV_MASK       MASK_BIT5
#define OTG_TOPCTRL_I2COFF_MASK     MASK_BIT4
#define OTG_TOPCTRL_XGATE_MASK      MASK_BIT3
#define OTG_TOPCTRL_SRST_MASK       MASK_BIT2
#define OTG_TOPCTRL_UDDR_MASK       MASK_BIT1
#define OTG_TOPCTRL_ULPI_MASK       MASK_BIT0
#endif

#ifdef __cplusplus
}
#endif /*   __cplusplus     */
#endif /*   _USB_REG_P_H_   */
