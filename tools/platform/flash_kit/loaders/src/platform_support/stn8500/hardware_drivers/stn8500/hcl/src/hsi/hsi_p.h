/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of High Speed Serial Interface (HSI) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HSI_P_H_
#define _HSI_P_H_

/*---------------------------------------------------------
	Includes
---------------------------------------------------------*/
#include "hsi.h"
#include "hsi_irqp.h"

/*-----------------------------------------------------------------------------
	Define
---------------------------------------------------------------------------*/
#define HSI_BUILD_VERSION   2
#define HSI_MAJOR_VERSION   1
#define HSI_MINOR_VERSION   6

#define HSI_TX_PERIPHID0    0x2B
#define HSI_TX_PERIPHID1    0x00
#define HSI_TX_PERIPHID2    0x08
#define HSI_TX_PERIPHID3    0x18
#define HSI_TX_PCELLID0     0x0D
#define HSI_TX_PCELLID1     0xF0
#define HSI_TX_PCELLID2     0x05
#define HSI_TX_PCELLID3     0xB1

#define HSI_RX_PERIPHID0    0x2C
#define HSI_RX_PERIPHID1    0x00
#define HSI_RX_PERIPHID2    0x08
#define HSI_RX_PERIPHID3    0x18
#define HSI_RX_PCELLID0     0x0D
#define HSI_RX_PCELLID1     0xF0
#define HSI_RX_PCELLID2     0x05
#define HSI_RX_PCELLID3     0xB1

#define HSI_DEFAULT         0x1F
/*---------------------------------------------------------
	Defines
---------------------------------------------------------*/
#define HSI_RX_FIFO_SIZE    0x20
#define HSI_TX_FIFO_SIZE    0x20

#ifdef ERROR
#undef ERROR
#endif
#define ERROR(msg)  DBGPRINT(DBGL_ERROR, msg)

/*---------------------------------------------------------
	Transmitter/ Reciever registers
---------------------------------------------------------*/
/*Registers common to both reciever and transmitter*/
/******************Configuration register******************/

#define HSI_CONFIG_MASK_TX_MODE      0x03
#define HSI_CONFIG_SHIFT_TX_MODE     0x00

#define HSI_CONFIG_MASK_RX_MODE      0x07
#define HSI_CONFIG_SHIFT_RX_MODE     0x00

#define HSI_CONFIG_MASK_FRMSZ        0x1F
#define HSI_CONFIG_SHIFT_FRMSZ       0x00

#define HSI_CONFIG_MASK_NOC          0x0F
#define HSI_CONFIG_SHIFT_NOC         0x00

/******************Status Registers*************************/
#define HSI_MASK_RX_STATE            0x03
#define HSI_SHIFT_RX_STATE           0x00

#define HSI_MASK_TX_STATE            0x07
#define HSI_SHIFT_TX_STATE           0x00

/*****************Buffer status register********************/
#define HSI_BUFSTATUS_MASK_Cx(ch_no)  ((MASK_BIT0) << (ch_no))
#define HSI_BUFSTATUS_SHIFT_Cx(ch_no) ch_no
/***************** Transmitter I\O Status Register *********/
#define HSI_MASK_TX_IO               0x01

#define HSI_SHIFT_TX_IO_DSATE        0x00
#define HSI_SHIFT_TX_IO_FSTATE       0x01
#define HSI_SHIFT_TX_IO_RSTATE       0x02

/****************** Reciever Threshold Register ************/
#define HSI_MASK_RX_THRESHOLD        0x3F
#define HSI_SHIFT_RX_THRESHOLD       0x00

/***************Break Count Register*************************/
#define HSI_TX_MASK_BREAK            MASK_BIT0

/************** Exception Register **************************/
#define HSI_RX_ESR_MASK              MASK_BIT0
#define HSI_RX_ESR_MASK_ALL          (MASK_BIT5 - 0x01)
#define HSI_RX_ESR_SHIFT_ALL         0x00

/*******************Timeout Exception*******************/
#define HSI_RX_ESR_SHIFT_TIMEOUT     0x00
#define HSI_RX_ESR_MASK_TIMEOUT      MASK_BIT0

/*******************OverRun Exception*******************/
#define HSI_RX_ESR_SHIFT_OVR         0x01
#define HSI_RX_ESR_MASK_OVR          MASK_BIT1

/*******************Break Exception*********************/
#define HSI_RX_ESR_SHIFT_BREAK       0x02
#define HSI_RX_ESR_MASK_BREAK        MASK_BIT2

/*******************Parity Exception********************/
#define HSI_RX_ESR_SHIFT_PARITY      0x03
#define HSI_RX_ESR_MASK_PARITY       MASK_BIT3

/***************Bit Rate Divisor register***************/
#define HSI_TX_DIVISOR_MASK_BRDIV  0xFFFFFF
#define HSI_TX_DIVISOR_SHIFT_BRDIV 0x000000

#define HSI_MASK_WATERMARK         0x1F
#define HSI_MASK_EXCEPTION_ALL     0x0F
/************** Real Time Register**********************/
#define HSI_RX_MASK_REALTIME_CHx(ch_no)  ((MASK_BIT0) << (ch_no))

/************* OverRun Register ************************/
#define HSI_RX_MASK_OVERRUN_CHx(ch_no)  ((MASK_BIT0) << (ch_no))

/************ Preamble Register ************************/
#define HSI_MASK_PREAMBLE           0xFFFFFF
#define HSI_SHIFT_PREAMBLE          0x00

/********************DMA Control Register*****************************/
#define HSI_DMACONTROL_MASK_DMAEx(ch_no)    ((MASK_BIT0) << ch_no)
#define HSI_DMACONTROL_SHIFT_DMAEx(ch_no)   (ch_no)

/*	Masked Interrupt Register*/
#define HSI_INTR_MASK_CxIM(intr_no)     ((MASK_BIT0) << intr_no)
#define HSI_INTR_SHIFT_CxIM(intr_no)    (intr_no)

#define HSI_INTR_MASK_CxIM_ALL          (MASK_BIT8 - 1)
#define HSI_INTR_SHIFT_CxIM_ALL          0x0
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
#define HSI_RX_MASK_EXP_ALL             (MASK_BIT4 - 1)
#else
#define HSI_RX_MASK_EXP_ALL				 0x07
#endif
#define HSI_RX_SHIFT_EXP_ALL             0x00

#define HSI_RX_INTR_MASK_CxIM_ALL       (MASK_BIT18 - 1)
#define HSI_RX_INTR_SHIFT_CxIM_ALL      0x0
/****************DataSwap Register ******************************/
#define HSI_TX_DATASWAP_MASK            0x03
#define HSI_TX_DATASWAP_SHIFT           0x00

/***************PipeGauge Register*******************************/
#define HSI_RX_PIPEGAUGE_MASK           0x03
#define HSI_RX_PIPEGAUGE_SHIFT          0x00

/***************Rx Timeout Register******************************/
#define HSI_RX_TIMEOUT_MASK             0xFFFFFF
#define HSI_RX_TIMEOUT_SHIFT            0x00
/***************Tx Burstlen Register*****************************/
#define HSI_TX_MASK_BURSTLEN            0xFFFFFF
#define HSI_TX_SHIFT_BURSTLEN           0x00
/***************Peripheral Identification Registers**************/
/* HSIxPeriphID0 Register*/
#define HSI_PERPHID0_MASK_PART_NO_0     (MASK_BIT8 - 0x1)
#define HSI_PERPHID0_SHIFT_PART_NO_0    (0x0)

/* HSIxPeriphID1 Register*/
#define HSI_PERPHID1_MASK_PART_NO_1     (MASK_BIT4 - 0x1)
#define HSI_PERPHID1_SHIFT_PART_NO_1    (0x0)
#define HSI_PERPHID1_MASK_DESIGNER0     ((MASK_BIT4 - 0x1) << (HSI_PERPHID1_SHIFT_DESIGNER0))
#define HSI_PERPHID1_SHIFT_DESIGNER0    (0x4)

/* HSIxPeriphID2 Register*/
#define HSI_PERPHID2_MASK_DESIGNER1     (MASK_BIT4 - 0x1)
#define HSI_PERPHID2_SHIFT_DESIGNER1    (0x0)
#define HSI_PERPHID2_MASK_REVISION      ((MASK_BIT4 - 0x1) << (HSI_PERPHID2_SHIFT_REVISION))
#define HSI_PERPHID2_SHIFT_REVISION     (0x4)

/* HSIxPeriphID3 Register*/
#define HSI_PERPHID3_MASK_CONFIGURATION     (MASK_BIT8 - 0x1)
#define HSI_PERPHID3_SHIFT_CONFIGURATION    (0x0)

/******************** PCell Identification Registers*************/
#define HSI_PCELL_MASK_IDx  (MASK_BIT8 - 0x1)
#define HSI_PCELL_SHIFT_IDx 0x0

/****************Integration Test Registers****************/
/********************Integration Test Control Register********/
#define HSI_ITCR_MASK_ITEN      MASK_BIT0

#define HSI_ITCR_MASK_TESTFIFO  MASK_BIT1

#define HSI_ITCR_SHIFT_ITEN     0x0

#define HSI_ITCR_SHIFT_TESTFIFO 0x1

/********************Integration Test Input Register**********/
#define HSI_ITIP_MASK_DAMx(ch_no)   ((MASK_BIT0) << (ch_no))
#define HSI_ITIP_SHIFT_DMAx(ch_no)  (ch_no)
#define HSI_ITIP_MASK_DAM0          HSI_ITIP_MASK_DAMx(0x0)
#define HSI_ITIP_MASK_DAM1          HSI_ITIP_MASK_DAMx(0x1)
#define HSI_ITIP_MASK_DAM2          HSI_ITIP_MASK_DAMx(0x2)
#define HSI_ITIP_MASK_DAM3          HSI_ITIP_MASK_DAMx(0x3)
#define HSI_ITIP_MASK_DAM4          HSI_ITIP_MASK_DAMx(0x4)
#define HSI_ITIP_MASK_DAM5          HSI_ITIP_MASK_DAMx(0x5)
#define HSI_ITIP_MASK_DAM6          HSI_ITIP_MASK_DAMx(0x6)
#define HSI_ITIP_MASK_DAM7          HSI_ITIP_MASK_DAMx(0x7)
#define HSI_ITIP_SHIFT_DAM0         HSI_ITIP_SHIFT_DAMx(0x0)
#define HSI_ITIP_SHIFT_DAM1         HSI_ITIP_SHIFT_DAMx(0x1)
#define HSI_ITIP_SHIFT_DAM2         HSI_ITIP_SHIFT_DAMx(0x2)
#define HSI_ITIP_SHIFT_DAM3         HSI_ITIP_SHIFT_DAMx(0x3)
#define HSI_ITIP_SHIFT_DAM4         HSI_ITIP_SHIFT_DAMx(0x4)
#define HSI_ITIP_SHIFT_DAM5         HSI_ITIP_SHIFT_DAMx(0x5)
#define HSI_ITIP_SHIFT_DAM6         HSI_ITIP_SHIFT_DAMx(0x6)
#define HSI_ITIP_SHIFT_DAM7         HSI_ITIP_SHIFT_DAMx(0x7)

/*********************Transmitter Specific*******************/
#define HSI_TX_ITIP_MASK_RDYI   MASK_BIT16

#define HSI_TX_ITIP_SHIFT_RDYI  16

/*********************Reciever Specific**********************/
#define HSI_RX_ITIP_MASK_DATI   MASK_BIT16

#define HSI_RX_ITIP_MASK_FLAI   MASK_BIT17

#define HSI_RX_ITIP_SHIFT_DATI  16

#define HSI_RX_ITIP_SHIFT_FLAI  17

/*************Integration Test Output 1 Register************/
#define HSI_ITOP1_MASK_DMABREQX(ch_no)  ((MASK_BIT0) << (ch_no))
#define HSI_ITOP1_SHIFT_DMABREQX(ch_no) (ch_no)
#define HSI_ITOP1_MASK_DMABREQ0         HSI_ITOP1_MASK_DMABREQX(0x0)
#define HSI_ITOP1_MASK_DMABREQ1         HSI_ITOP1_MASK_DMABREQX(0x1)
#define HSI_ITOP1_MASK_DMABREQ2         HSI_ITOP1_MASK_DMABREQX(0x2)
#define HSI_ITOP1_MASK_DMABREQ3         HSI_ITOP1_MASK_DMABREQX(0x3)
#define HSI_ITOP1_MASK_DMABREQ4         HSI_ITOP1_MASK_DMABREQX(0x4)
#define HSI_ITOP1_MASK_DMABREQ5         HSI_ITOP1_MASK_DMABREQX(0x5)
#define HSI_ITOP1_MASK_DMABREQ6         HSI_ITOP1_MASK_DMABREQX(0x6)
#define HSI_ITOP1_MASK_DMABREQ7         HSI_ITOP1_MASK_DMABREQX(0x7)
#define HSI_ITOP1_MASK_DMASREQX(ch_no)  ((MASK_BIT16) << (ch_no))
#define HSI_ITOP1_SHIFT_DMASREQX(ch_no) ((ch_no) + 16)
#define HSI_ITOP1_MASK_DMASREQ0         HSI_ITOP1_MASK_DMASREQX(0x0)
#define HSI_ITOP1_MASK_DMASREQ1         HSI_ITOP1_MASK_DMASREQX(0x1)
#define HSI_ITOP1_MASK_DMASREQ2         HSI_ITOP1_MASK_DMASREQX(0x2)
#define HSI_ITOP1_MASK_DMASREQ3         HSI_ITOP1_MASK_DMASREQX(0x3)
#define HSI_ITOP1_MASK_DMASREQ4         HSI_ITOP1_MASK_DMASREQX(0x4)
#define HSI_ITOP1_MASK_DMASREQ5         HSI_ITOP1_MASK_DMASREQX(0x5)
#define HSI_ITOP1_MASK_DMASREQ6         HSI_ITOP1_MASK_DMASREQX(0x6)
#define HSI_ITOP1_MASK_DMASREQ7         HSI_ITOP1_MASK_DMASREQX(0x7)
#define HSI_ITOP1_SHIFT_DMASREQ0        HSI_ITOP1_SHIFT_DMASREQX(0x0)
#define HSI_ITOP1_SHIFT_DMASREQ1        HSI_ITOP1_SHIFT_DMASREQX(0x1)
#define HSI_ITOP1_SHIFT_DMASREQ2        HSI_ITOP1_SHIFT_DMASREQX(0x2)
#define HSI_ITOP1_SHIFT_DMASREQ3        HSI_ITOP1_SHIFT_DMASREQX(0x3)
#define HSI_ITOP1_SHIFT_DMASREQ4        HSI_ITOP1_SHIFT_DMASREQX(0x4)
#define HSI_ITOP1_SHIFT_DMASREQ5        HSI_ITOP1_SHIFT_DMASREQX(0x5)
#define HSI_ITOP1_SHIFT_DMASREQ6        HSI_ITOP1_SHIFT_DMASREQX(0x6)
#define HSI_ITOP1_SHIFT_DMASREQ7        HSI_ITOP1_SHIFT_DMASREQX(0x7)

/***************Integration Test Output 2 Register****************/
#define HSI_ITOP2_MASK_CINTRx(ch_no)    (MASK_BIT0 << (ch_no))
#define HSI_ITOP2_SHIFT_CINTRx(ch_no)   (ch_no)
#define HSI_ITOP2_MASK_CINTR0           HSI_ITOP2_MASK_CINTRx(0x0)
#define HSI_ITOP2_MASK_CINTR1           HSI_ITOP2_MASK_CINTRx(0x1)
#define HSI_ITOP2_MASK_CINTR2           HSI_ITOP2_MASK_CINTRx(0x2)
#define HSI_ITOP2_MASK_CINTR3           HSI_ITOP2_MASK_CINTRx(0x3)
#define HSI_ITOP2_MASK_CINTR4           HSI_ITOP2_MASK_CINTRx(0x4)
#define HSI_ITOP2_MASK_CINTR5           HSI_ITOP2_MASK_CINTRx(0x5)
#define HSI_ITOP2_MASK_CINTR6           HSI_ITOP2_MASK_CINTRx(0x6)
#define HSI_ITOP2_MASK_CINTR7           HSI_ITOP2_MASK_CINTRx(0x7)
#define HSI_ITOP2_SHIFT_CINTR0          HSI_ITOP2_SHIFT_CINTRx(0x0)
#define HSI_ITOP2_SHIFT_CINTR1          HSI_ITOP2_SHIFT_CINTRx(0x1)
#define HSI_ITOP2_SHIFT_CINTR2          HSI_ITOP2_SHIFT_CINTRx(0x2)
#define HSI_ITOP2_SHIFT_CINTR3          HSI_ITOP2_SHIFT_CINTRx(0x3)
#define HSI_ITOP2_SHIFT_CINTR4          HSI_ITOP2_SHIFT_CINTRx(0x4)
#define HSI_ITOP2_SHIFT_CINTR5          HSI_ITOP2_SHIFT_CINTRx(0x5)
#define HSI_ITOP2_SHIFT_CINTR6          HSI_ITOP2_SHIFT_CINTRx(0x6)
#define HSI_ITOP2_SHIFT_CINTR7          HSI_ITOP2_SHIFT_CINTRx(0x7)

/* Transmitter Specific macros*/
#define HSI_TX_ITOP2_MASK_TDAT  MASK_BIT16

#define HSI_TX_ITOP2_MASK_TFLA  MASK_BIT17

#define HSI_TX_ITOP2_MASK_TOEn  MASK_BIT18

#define HSI_TX_ITOP2_SHIFT_TDAT 16

#define HSI_TX_ITOP2_SHIFT_TFLA 17

#define HSI_TX_ITOP2_SHIFT_TOEn 18

/* Reciever Specfic Macros*/
#define HSI_RX_ITOP2_MASK_RRDY      MASK_BIT16

#define HSI_RX_ITOP2_MASK_EXINTR    MASK_BIT17

#define HSI_RX_ITOP2_SHIFT_RRDY     16

#define HSI_RX_ITOP2_SHIFT_EXINTR   17

/*-----------------------------------------------------------------------------

 * Generic Macros                                                       

 *---------------------------------------------------------------------------*/
#define HSI_SET_BIT(reg_name, mask)             HCL_SET_BITS(reg_name, mask)
#define HSI_CLR_BIT(reg_name, mask)             HCL_CLEAR_BITS(reg_name, mask)
#define HSI_WRITE_BIT(reg_name, val, mask, sb)  HCL_WRITE_BITS(reg_name, ((val) << sb), mask)
#define HSI_TEST_BIT(reg_name, mask, sb)        ((HCL_READ_BITS(reg_name, (mask))) >> sb)
#define HSI_WRITE_REG(reg_name, val)            HCL_WRITE_REG(reg_name, val)
#define HSI_READ_REG(reg_name)                  HCL_READ_REG(reg_name)

/*------------------------------------------------------------------------------------------------------------------

 * Specific Macros                                                      

 *------------------------------------------------------------------------------------------------------------------*/
/******************** setting/clearing/reading bits of Configutaion Register register ***************************/
/**************************** HSI SET & GET Configuration Register ************************************************/
#define HSI_SET_TX_MODE(con, a)             HSI_WRITE_BIT(con, a, HSI_CONFIG_MASK_TX_MODE, HSI_CONFIG_SHIFT_TX_MODE)
#define HSI_GET_TX_MODE(con)                HSI_TEST_BIT(con,     HSI_CONFIG_MASK_TX_MODE, HSI_CONFIG_SHIFT_TX_MODE)

#define HSI_SET_RX_MODE(con, a)             HSI_WRITE_BIT(con, a, HSI_CONFIG_MASK_RX_MODE, HSI_CONFIG_SHIFT_RX_MODE)
#define HSI_GET_RX_MODE(con)                HSI_TEST_BIT(con,     HSI_CONFIG_MASK_RX_MODE, HSI_CONFIG_SHIFT_RX_MODE)

#define HSI_SET_FRMSZ(con, a)               HSI_WRITE_BIT(con, a, HSI_CONFIG_MASK_FRMSZ, HSI_CONFIG_SHIFT_FRMSZ)
#define HSI_GET_FRMSZ(con)                  HSI_TEST_BIT(con,     HSI_CONFIG_MASK_FRMSZ, HSI_CONFIG_SHIFT_FRMSZ)

#define HSI_SET_NOC(con, a)                 HCL_WRITE_REG(con,a)
#define HSI_GET_NOC(con)                    HSI_TEST_BIT(con,HSI_CONFIG_MASK_NOC, HSI_CONFIG_SHIFT_NOC)

/**************************** HSI Priority Registers **************************************************************/
#define HSI_SET_TX_PRIORITY(con,a)          HSI_SET_BIT(con, (MASK_BIT0 << a))
#define HSI_CLR_TX_PRIORITY(con,a)          HSI_CLR_BIT(con, (MASK_BIT0 << a))
#define HSI_GET_TX_PRIORITY(con,a)          HSI_TEST_BIT(con, MASK_BIT0,a)

/************************** Reading bits of Tx/ Rx State Register **************************************************/
#define HSI_GET_RX_STATE(con)               HSI_TEST_BIT(con, HSI_MASK_RX_STATE, HSI_SHIFT_RX_STATE)
#define HSI_GET_TX_STATE(con)               HSI_TEST_BIT(con, HSI_MASK_TX_STATE, HSI_SHIFT_TX_STATE)

/************************* Writing bits of Tx/ Rx State Register ***************************************************/
#define HSI_SET_RX_STATE(con,a)             HSI_WRITE_BIT(con,a, HSI_MASK_RX_STATE, HSI_SHIFT_RX_STATE)
#define HSI_SET_TX_STATE(con,a)             HSI_WRITE_BIT(con,a, HSI_MASK_TX_STATE, HSI_SHIFT_TX_STATE)

/************************ Reading and Writing bits of Buffer Status Register ***************************************/
#define HSI_GET_FS(con, ch_no)              HSI_TEST_BIT(con, HSI_BUFSTATUS_MASK_Cx(ch_no), HSI_BUFSTATUS_SHIFT_Cx(ch_no))
#define HSI_SET_FS(con, ch_no)              HSI_WRITE_BIT(con, HSI_BUFSTATUS_MASK_Cx(ch_no), HSI_BUFSTATUS_SHIFT_Cx(ch_no))
#define HSI_CLR_FS(con, ch_no)              HSI_CLR_BIT(con,HSI_BUFSTATUS_MASK_Cx(ch_no))
/************************* HSI Reciever Threshold Bits *************************************************************/
#define HSI_SET_RX_THRESHOLD(con,a)         HSI_WRITE_BIT(con,a,HSI_MASK_RX_THRESHOLD,HSI_SHIFT_RX_THRESHOLD)  

/************************* Setting/ reading bits of Bit Rate Divisor Regiter ***************************************/
#define HSI_SET_TX_BRDEV(con, a)            HSI_WRITE_BIT(con, a, HSI_TX_DIVISOR_MASK_BRDIV, HSI_TX_DIVISOR_SHIFT_BRDIV)
#define HSI_GET_TX_BRDEV(con)               HSI_TEST_BIT(con, HSI_TX_DIVISOR_MASK_BRDIV, HSI_TX_DIVISOR_SHIFT_BRDIV)

/************************** Setting and Reading the HSI TX and RX Parity *******************************************/

#define HSI_SET_PARITY(con,a)               HSI_WRITE_REG(con,a)
#define HSI_GET_PARITY(con)                 HSI_READ_REG(con)

/************************* Setting/Reading bits of the Break Register **********************************************/
#define HSI_SET_TX_BREAK(con)               HSI_SET_BIT(con,HSI_TX_MASK_BREAK)
#define HSI_GET_TX_BREAK(con)               HSI_READ_REG(con)

/************************** Setting the flush bits *****************************************************************/
#define HSI_SET_TX_FLUSHBITS(con,a)         HSI_WRITE_REG(con,a)
#define HSI_GET_TX_FLUSHBITS(con,a)         HSI_READ_REG(con)

/*************************** Setting/Reading the Detector Register *************************************************/
#define HSI_SET_RX_DETECTOR(con)            HSI_SET_BIT(con,MASK_BIT0)
#define HSI_CLR_RX_DETECTOR(con)            HSI_CLR_BIT(con,MASK_BIT0) 

/*********************** Setting/Clearing/Reading the bits of the Timout Register **********************************/
#define HSI_SET_RX_EX_TIMEOUT(con)          HSI_SET_BIT(con,HSI_RX_ESR_MASK_TIMEOUT)
#define HSI_RX_GET_EX_TIMEOUT(con)          HSI_TEST_BIT(con, HSI_RX_ESR_MASK_TIMEOUT, HSI_RX_ESR_SHIFT_TIMEOUT)
#define HSI_RX_CLR_EX_TIMEOUT(con)          HSI_SET_BIT(con,HSI_RX_ESR_MASK_TIMEOUT)

/*********************** Setting/Clearing/Reading the bits of the OverRun Register **********************************/
#define HSI_SET_RX_EX_OVR(con)              HSI_SET_BIT(con,HSI_RX_ESR_MASK_OVR)
#define HSI_GET_RX_EX_OVR(con)              HSI_TEST_BIT(con,HSI_RX_ESR_MASK_OVR,HSI_RX_ESR_SHIFT_OVR)
#define HSI_CLR_RX_EX_OVR(con)              HSI_SET_BIT(con,HSI_RX_ESR_MASK_OVR)

/*********************** Setting/Clearing/Reading the bits of the Break Register ************************************/
#define HSI_SET_RX_EX_BREAK(con)            HSI_SET_BIT(con,HSI_RX_ESR_MASK_BREAK)
#define HSI_GET_RX_EX_BREAK(con)            HSI_TEST_BIT(con, HSI_RX_ESR_MASK_BREAK, HSI_RX_ESR_SHIFT_BREAK)
#define HSI_CLR_RX_EX_BREAK(con)            HSI_SET_BIT(con,HSI_RX_ESR_MASK_BREAK)

/*********************** Setting/Clearing/Reading the bits of the Parity Register ***********************************/

#define HSI_SET_RX_EX_PARITY(con)           HSI_SET_BIT(con,HSI_RX_ESR_MASK_PARITY)
#define HSI_GET_RX_EX_PARITY(con)           HSI_TEST_BIT(con, HSI_RX_ESR_MASK_PARITY, HSI_RX_ESR_SHIFT_PARITY)
#define HSI_CLR_RX_EX_PARITY(con)           HSI_SET_BIT(con,HSI_RX_ESR_MASK_PARITY)


#define HSI_SET_RX_EX_ALL(con)              HSI_SET_BIT(con, HSI_RX_ESR_MASK_ALL)
#define HSI_GET_RX_EX_ALL(con)              HSI_TEST_BIT(con, HSI_RX_ESR_MASK_ALL, HSI_RX_ESR_SHIFT_ALL)
#define HSI_CLR_RX_EX_ALL(con)              HSI_CLR_BIT(con, HSI_RX_ESR_MASK_ALL)

/************************** Setting/Clearing/Reading the bits of the DataSwap Register *******************************/
#define HSI_SET_TX_DATASWAP(con,a)          HSI_WRITE_BIT(con,a,HSI_TX_DATASWAP_MASK,HSI_TX_DATASWAP_SHIFT)
#define HSI_GET_TX_DATASWAP(con,a)          HSI_TEST_BIT(con,HSI_TX_DATASWAP_MASK,HSI_TX_DATASWAP_SHIFT)
#define HSI_CLR_TX_DATASWAP(con)            HSI_CLR_BIT(con,HSI_TX_DATASWAP_MASK)
/************************** Setting/Clearing/Reading the bits of the PipeGauge Register ******************************/
#define HSI_SET_RX_PIPEGAUGE(con,a)         HSI_WRITE_BIT(con,a,HSI_RX_PIPEGAUGE_MASK,HSI_RX_PIPEGAUGE_SHIFT)
#define HSI_GET_RX_PIPEGAUGE(con,a)         HSI_TEST_BIT(con,a,HSI_RX_PIPEGAUGE_MASK,HSI_RX_PIPEGAUGE_SHIFT)

/************************** Setting/Clearing/Reading the bits of the Timeout Register *******************************/
#define HSI_SET_RX_TIMEOUT(con,a)           HSI_WRITE_BIT(con,a,HSI_RX_TIMEOUT_MASK,HSI_RX_TIMEOUT_SHIFT)
#define HSI_GET_RX_TIMEOUT(con)             HSI_TEST_BIT(con,HSI_RX_TIMEOUT_MASK,HSI_RX_TIMEOUT_SHIFT)

/*********************** Setting/Clearing/Reading the bits of the Real Time Register *********************************/
#define HSI_SET_RX_REALTIME_CHx(con,a)      HSI_SET_BIT(con,HSI_RX_MASK_REALTIME_CHx(a))
#define HSI_GET_RX_REALTIME_CHx(con,a)      HSI_TEST_BIT(con,HSI_RX_MASK_REALTIME_CHx(a),a)
#define HSI_CLEAR_RX_REALTIME_CHx(con,a)    HSI_CLR_BIT(con,HSI_RX_MASK_REALTIME_CHx(a))

/************************* Setting\Reading OverRun Register **********************************************************/
#define HSI_SET_RX_OVERRUN_CHx(con,a)      HSI_SET_BIT(con,HSI_RX_MASK_OVERRUN_CHx(a))
#define HSI_GET_RX_OVERRUN_CHx(con,a)      HSI_TEST_BIT(con,HSI_RX_MASK_OVERRUN_CHx(a),a)
#define HSI_CLEAR_RX_OVERRUN_CHx(con,a)    HSI_CLR_BIT(con,HSI_RX_MASK_OVERRUN_CHx(a))

/************************* Setting\Reading OverRun Acknowladge Register **********************************************/
#define HSI_SET_RX_OVERRUNACK_CHx(con,a)   HSI_SET_BIT(con,HSI_RX_MASK_OVERRUN_CHx(a))

/************************* Setting Burstlen Register *****************************************************************/
#define HSI_SET_TX_BURSTLEN(con,a)         HSI_WRITE_BIT(con,a,HSI_TX_MASK_BURSTLEN,HSI_TX_SHIFT_BURSTLEN)
#define HSI_GET_TX_BURSTLEN(con)           HSI_TEST_BIT(con,HSI_TX_MASK_BURSTLEN,HSI_TX_SHIFT_BURSTLEN)  

/************************* Setting\Reading the Preamble Register *****************************************************/
#define HSI_TX_SET_PREAMBLE(con, a)        HSI_WRITE_BIT(con, a, HSI_MASK_PREAMBLE, HSI_SHIFT_PREAMBLE)
#define HSI_TX_GET_PREAMBLE(con)           HSI_TEST_BIT(con, HSI_MASK_PREAMBLE, HSI_SHIFT_PREAMBLE)

#define HSI_RX_SET_PREAMBLE(con, a)        HSI_WRITE_BIT(con, a, HSI_MASK_PREAMBLE, HSI_SHIFT_PREAMBLE)
#define HSI_RX_GET_PREAMBLE(con)           HSI_TEST_BIT(con, HSI_MASK_PREAMBLE, HSI_SHIFT_PREAMBLE)
   
/************************** Setting/clearing/reading bits of DMA Control Register ************************************/
#define HSI_SET_DMAE(con, ch_no)           HSI_SET_BIT(con, HSI_DMACONTROL_MASK_DMAEx(ch_no))
#define HSI_CLR_DMAE(con, ch_no)           HSI_CLR_BIT(con, HSI_DMACONTROL_MASK_DMAEx(ch_no))
#define HSI_GET_DMAE(con, ch_no)           HSI_TEST_BIT(con, HSI_DMACONTROL_MASK_DMAEx(ch_no), HSI_DMACONTROL_SHIFT_DMAEx(ch_no))

/*************************** Setting/clearing/reading the bits of Watermakx Register *********************************/
#define HSI_SET_WATERMARK(con,a)           HSI_WRITE_BIT(con,a,HSI_MASK_WATERMARK,SHIFT_BYTE0)
#define HSI_GET_WATERMARK(con)             HSI_TEST_BIT(con,HSI_MASK_WATERMARK,SHIFT_BYTE0)
/*************************** Setting the chhanel buffer **************************************************************/
#define HSI_SET_BUFFER(con,a)              HSI_WRITE_REG(con,a)
#define HSI_GET_BUFFER(con)                HSI_READ_REG(con)

/*************************** Setting/Clearing/Reading Span Register **************************************************/
#define HSI_SET_SPANx(con,a)               HSI_WRITE_REG(con,a)
#define HSI_GET_SPANx(con)                 HSI_READ_REG(con)

/*************************** Setting/Clearing/Reading Base Register **************************************************/
#define HSI_SET_BASEx(con,a)               HSI_WRITE_REG(con,a)
#define HSI_GET_BASEx(con)                 HSI_READ_REG(con)

/*************************** Setting/Clearing/Reading Gauge Register **************************************************/
#define HSI_SET_GAUGEx(con,ch_no,a)        HSI_WRITE_REG(con+ch_no,a)
#define HSI_GET_GAUGEx(con,ch_no)          HSI_READ_REG(con+ch_no)

#if ((defined ST_8500V1) || (defined ST_HREFV1) || (defined __PEPS_8500_V1)||(defined __PEPS_8500_V2))
#define HSI_SET_RX_FRAMEBURSTCNT(con,a)    HSI_WRITE_REG(con,a)
#define HSI_GET_RX_FRAMEBURSTCNT(con)      HSI_READ_REG(con)
#endif
/*************************** Setting/Clearing/Reading Watermark Registers ********************************************/
       /**************************** Setting the bits of Interrupt Destination Selection Register **********/
#define HSI_SET_WMARKIDx(con, ch_no)       HSI_SET_BIT(con, HSI_INTR_MASK_CxIM(ch_no))
#define HSI_CLR_WMARKIDx(con, ch_no)       HSI_CLR_BIT(con, HSI_INTR_MASK_CxIM(ch_no))

      /**************************** Setting the bits of Interrupt Status Register **************************/
#define HSI_GET_WMARKMISx(con,ch_no)       HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM(ch_no),ch_no)
#define HSI_GET_WMARKMISx_ALL(con)         HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL)

     /**************************** Setting the bits of Interrupt Mask Register *****************************/
#define HSI_SET_WMARKIMx(con,ch_no)        HSI_SET_BIT(con,HSI_INTR_MASK_CxIM(ch_no))
#define HSI_SET_WMARKIMx_ALL(con)          HSI_WRITE_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL)

#define HSI_GET_WMARKIMx(con,ch_no)        HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM(ch_no),ch_no) 
#define HSI_GET_WMARKIMx_ALL(con)          HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL) 

#define HSI_CLR_WMARKIMx(con,ch_no)        HSI_CLR_BIT(con,HSI_INTR_MASK_CxIM(ch_no))
#define HSI_CLR_WMARKIMx_ALL(con,ch_no)    HSI_CLR_BIT(con,HSI_INTR_MASK_CxIM_ALL)

#define HSI_SET_WMARKICx(con,ch_no)        HSI_SET_BIT(con,HSI_INTR_MASK_CxIM(ch_no))

/********************************** Setting/Reading/Clearing the OverRun Register **************************************/
     /******************************** OverRun Interrupt Staus Register *************************************/
#define HSI_GET_RX_OVERRUNMISx(con,ch_no)  HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM(ch_no),ch_no)
#define HSI_GET_RX_OVERRUNMISx_ALL(con)    HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL)

     /**************************** Setting the bits of Interrupt Mask Register *****************************/
#define HSI_SET_RX_OVERRUNIMx(con,ch_no)   HSI_SET_BIT(con,HSI_INTR_MASK_CxIM(ch_no))
#define HSI_SET_RX_OVERRUNIMx_ALL(con)     HSI_WRITE_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL)

#define HSI_GET_RX_OVERRUNIMx(con,ch_no)   HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM(ch_no),ch_no) 
#define HSI_GET_RX_OVERRUNIMx_ALL(con)     HSI_TEST_BIT(con,HSI_INTR_MASK_CxIM_ALL,HSI_INTR_SHIFT_CxIM_ALL) 

#define HSI_CLR_RX_OVERRUNIMx(con,ch_no)   HSI_CLR_BIT(con,HSI_INTR_MASK_CxIM(ch_no))
#define HSI_CLR_RX_OVERRUNIMx_ALL(con,ch_no)  HSI_CLR_BIT(con,HSI_INTR_MASK_CxIM_ALL)

/********************************** Setting/Reading/Clearing the Exception Register **************************************/
/******************************** Setting the Interrupt Staus Register *************************************/
/**************************** Setting the bits of Interrupt Mask Register *****************************/
#define HSI_SET_RX_EXCEPIM_ALL(con,a)      HSI_WRITE_BIT(con,a,HSI_RX_MASK_EXP_ALL,HSI_RX_SHIFT_EXP_ALL)
#define HSI_GET_RX_EXCEPIM_ALL(con)        HSI_TEST_BIT(con,HSI_RX_MASK_EXP_ALL,HSI_RX_SHIFT_EXP_ALL) 
#define HSI_CLR_RX_EXCEPIM_ALL(con)        HSI_CLR_BIT(con,HSI_RX_MASK_EXP_ALL)


/******** reading bits of peripheral identification registers***************************************/
#define HSI_GET_PART_NO_0(con)  HSI_TEST_BIT(con, HSI_PERPHID0_MASK_PART_NO_0, HSI_PERPHID0_SHIFT_PART_NO_0)
#define HSI_GET_PART_NO_1(con)  HSI_TEST_BIT(con, HSI_PERPHID1_MASK_PART_NO_1, HSI_PERPHID1_SHIFT_PART_NO_1)
#define HSI_GET_DESIGNER0(con)  HSI_TEST_BIT(con, HSI_PERPHID1_MASK_DESIGNER0, HSI_PERPHID1_SHIFT_DESIGNER0)
#define HSI_GET_DESIGNER1(con)  HSI_TEST_BIT(con, HSI_PERPHID2_MASK_DESIGNER1, HSI_PERPHID2_SHIFT_DESIGNER1)
#define HSI_GET_REVISION(con)   HSI_TEST_BIT(con, HSI_PERPHID2_MASK_REVISION, HSI_PERPHID2_SHIFT_REVISION)
#define HSI_GET_CONFIG(con)     HSI_TEST_BIT(con, HSI_PERPHID3_MASK_CONFIGURATION, HSI_PERPHID3_SHIFT_CONFIGURATION)

/******** reading bits of peripheral identification registers***************************************/
#define HSI_GET_PCELL_IDx(con)  HSI_TEST_BIT(con, HSI_PCELL_MASK_IDX, HSI_PCELL_SHIFT_IDX)





/****************************************************************************************************/

/*--------------------------------------------

Private Structures

----------------------------------------------*/
typedef struct
{
    t_uint32            *data;
    t_uint32            size;
    t_hsi_channel_num   ch_no;
    t_uint8             status;                     /*t_hsi_channel_status*/
    t_uint8             reserved_1[2];
} t_hsi_data_buffer;

typedef struct
{                                                   /*Tx register*/
    t_uint32    hsi_tx_id;
    t_uint32    hsi_tx_mode;
    t_uint32    hsi_tx_txstate;
    t_uint32    hsi_tx_iostate;
    t_uint32    hsi_tx_parity;
    t_uint32    hsi_tx_span[8];
    t_uint32    hsi_tx_gauge[8];
    t_uint32    hsi_tx_base[8];
    t_uint32    hsi_tx_dmaen;

    t_uint32    hsi_tx_wmarkmis;
    t_uint32    hsi_tx_wmarkid;

    /*Rx register*/
    t_uint32    hsi_rx_id;
    t_uint32    hsi_rx_mode;
    t_uint32    hsi_rx_rxstate;
    t_uint32    hsi_rx_parity;
    t_uint32    hsi_rx_dmaen;
    
    t_uint32    hsi_rx_wmarkmis;
    t_uint32    hsi_rx_wmarkid;
} t_hsi_device_context;

typedef struct
{
    t_hsi_tx_register           *p_hsi_tx;                                  /* TRANSMITTER REGISTER POINTER */
    t_hsi_tx_mode               hsi_tx0_prev_mode;                          /* TRANSMITTER_0 MODE */
    t_uint32                    hsi_tx_frm_size[HSI_MAX_CHANNEL_NUM];       /* TRANSMITTER CHANNEL FRAME SIZE */
    
    volatile t_hsi_data_buffer  hsi_tx_data_buffer[HSI_MAX_CHANNEL_NUM];    /* TRANSMITTER CHANNEL DATA BUFFER */
    t_uint32                    hsi_tx_span[HSI_MAX_CHANNEL_NUM];           /* TRANSMITTER CHANNEL NUMBER OF ENTRIES */        
    t_uint32                    hsi_tx_base[HSI_MAX_CHANNEL_NUM];           /* TRANSMITTER CHANNEL BASE ADDRESS*/

    t_hsi_rx_register           *p_hsi_rx;                                  /* RECEIVER REGISTER POINTER */
    t_hsi_rx_mode               hsi_rx0_prev_mode;                          /* RECEIVER MODE */
    t_uint32                    hsi_rx_frm_size[HSI_MAX_CHANNEL_NUM];       /* RECEIVER CHANNEL FRAME SIZE */
    
    volatile t_hsi_data_buffer  hsi_rx_data_buffer[HSI_MAX_CHANNEL_NUM];    /* RECEIVER CHANNEL DATA BUFFER */
    t_uint32                    hsi_rx_span[HSI_MAX_CHANNEL_NUM];           /* RECEIVER CHANNEL NUMBER OF ENTRIES */      
    t_uint32                    hsi_rx_base[HSI_MAX_CHANNEL_NUM];           /* RECEIVER CHANNEL BASE ADDRESS*/

    t_hsi_event                 hsi_event;
    t_hsi_device_context        device_context;                             /*Device context*/
} t_hsi_system_context;
#endif /*_HSIP_H_*/

