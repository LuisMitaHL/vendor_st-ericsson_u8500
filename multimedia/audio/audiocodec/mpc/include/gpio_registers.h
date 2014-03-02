/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   gpio_registers.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _GPIO_REGISTERS_H_
#define  _GPIO_REGISTERS_H_
//ARMside #define GPIO1B_32_REG_BASE_ADDR 0x8012E080UL
//#define GPIO1B_32_MMDSP_BASE_ADDR (0xF80000UL + ((0x8012E080UL - 0x80100000UL)>>1))
#define GPIO1B_32_MMDSP_BASE_ADDR (0xF80000UL + ((0x8012E080UL - 0x80100000UL)>>1))

typedef struct { // Warning: comment is offset in bytes not MAU to get MAU divide offset by 2
    long DAT;    // 0x00  
    long DATS;  
    long DATC;  
    long PDIS;  
    long DIR;    // 0x10
    long DIRS;  
    long DIRC;  
    long SLPM;  
    long AFSLA;  // 0x20
    long AFSLB;
    long LOWEMI;
    long fake[5]; 
    long RIMSC;  // 0x40
    long FIMSC;
    long IS;
    long IC;
    long RWMSC;  // 0x50
    long FWMSC;
    long WKS; 
} GPIOconfig;
static volatile GPIOconfig EXTMMIO * gpioReg = ( volatile GPIOconfig EXTMMIO *)GPIO1B_32_MMDSP_BASE_ADDR;


#endif   // _GPIO_REGISTERS_H_
