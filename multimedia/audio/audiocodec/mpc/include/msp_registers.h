/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   msp_registers.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _MSP_REGISTERS_H_
#define  _MSP_REGISTERS_H_
#include "usefull.h"

//---------------------------------------------------------------
#define MSP_0_REG_BASE_ADDR 0x80123000UL
#define MSP_1_REG_BASE_ADDR 0x80124000UL
#define MSP_3_REG_BASE_ADDR 0x80125000UL
#define MSP_0_REG_END_ADDR  0x80123FFFUL
#define MSP_1_REG_END_ADDR  0x80124FFFUL
#define MSP_3_REG_END_ADDR  0x80125FFFUL
//  with ext_io_base set on STM (0x80100000)
#define MSP0_MMDSP_BASE_ADDR 0xF91800UL
#define MSP1_MMDSP_BASE_ADDR 0xF92000UL
#define MSP3_MMDSP_BASE_ADDR 0xF92800UL

#define MSP_AB8500_TX MSP1
#define MSP_AB8500_TX_REG_BASE_ADDR MSP_1_REG_BASE_ADDR

#define MSP_AB8500_RX MSP3
#define MSP_AB8500_RX_REG_BASE_ADDR MSP_3_REG_BASE_ADDR

//---------------------------------------------------------------

typedef struct {
    unsigned long DR       ;
    unsigned long GCR      ;
    unsigned long TCF      ;
    unsigned long RCF      ;
    unsigned long SRG      ;
    unsigned long FLR      ;
    unsigned long DMACR    ;
    unsigned long WMRK     ;
    unsigned long IMSC     ;
    unsigned long RIS      ;
    unsigned long MIS      ;
    unsigned long ICR      ;
    unsigned long MCR      ;
    unsigned long RCV      ;
    unsigned long RCM      ;
    unsigned long fake_1   ;
    unsigned long TCE0     ;
    unsigned long TCE1     ;
    unsigned long TCE2     ;
    unsigned long TCE3     ;
    unsigned long fake_2[4];
    unsigned long RCE0     ;
    unsigned long RCE1     ;
    unsigned long RCE2     ;
    unsigned long RCE3     ;
    unsigned long IODLY    ;
    unsigned long fake_3[3];
    unsigned long ITCR     ;
    unsigned long ITIP     ;
    unsigned long ITOP     ;
    unsigned long TSTDR    ;
} MspConfig;

enum MspChannel {
    MSP0 = 0,
    MSP1 = 1,
    MSP3 = 3
};
typedef enum MspChannel MspChannel;

enum MspDirection {
    MSP_TX = 0,
    MSP_RX = 1
};
typedef enum MspDirection MspDirection;

#endif   // _MSP_REGISTERS_H_

