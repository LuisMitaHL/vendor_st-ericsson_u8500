/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   prcc_registers.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _PRCC_REGISTERS_H_
#define  _PRCC_REGISTERS_H_

#define PRCC_PERIPH1_MMDSP_BASE_ADDR  0xF97800UL

typedef struct { 
    long PCKEN;
    long PCKDIS;
    long KCKEN;
    long KCKDIS;
    long PCKSR;
    long PKCKSR;
    long SOFTRST_SET;
    long SOFTRST_CLR;
} PRCCConfig;

static volatile PRCCConfig EXTMMIO * prcc1Reg = (volatile EXTMMIO PRCCConfig *) PRCC_PERIPH1_MMDSP_BASE_ADDR;

#endif   // _PRCC_REGISTERS_H_
