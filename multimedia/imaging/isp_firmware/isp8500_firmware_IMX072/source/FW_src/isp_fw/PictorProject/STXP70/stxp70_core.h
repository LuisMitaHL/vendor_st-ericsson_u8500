/*----------------------------------------------------------------------------*
 *      Copyright 2005, STMicroelectronics, Incorporated.                     *
 *      All rights reserved.                                                  *
 *                                                                            *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *
 * This software is supplied under the terms of a license agreement or nondis-*
 * closure agreement with STMicroelectronics and may not be copied or disclo- *
 * sed except in accordance with the terms of that agreement.                 *
 *----------------------------------------------------------------------------*
 * System           : StxP70 Processor                                        *
 * Project Component: System Library                                          *
 * File Name        : STxP70_core.h                                           *
 * Purpose          : This module contains all macros to inline the core      *
 *                    instructions                                            *
 * History          : 2005/05/16 - First implementation.                      *
 *----------------------------------------------------------------------------*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 


#ifndef _STXP70_CORE_H
#define _STXP70_CORE_H

#include "stxp70_type.h"

/* Definition of System Register SR */

#define SR_IML	(STXP70_BIT_0 | STXP70_BIT_1 | STXP70_BIT_2 | STXP70_BIT_3 | STXP70_BIT_4)
#define SR_IML_MAX	(STXP70_BIT_0 | STXP70_BIT_1 | STXP70_BIT_2 | STXP70_BIT_3 | STXP70_BIT_4)
#define SR_HWLOOP0_ENABLE STXP70_BIT_6
#define SR_HWLOOP1_ENABLE STXP70_BIT_7
#define SR_DIAGNOSTIC	STXP70_BIT_9
#define SR_PRIVILEGE	STXP70_BIT_10
#define SR_CARRY	STXP70_BIT_12
#define SR_PMASK	(STXP70_BIT_16 | STXP70_BIT_17 | STXP70_BIT_18 | STXP70_BIT_19 | STXP70_BIT_20 | \
STXP70_BIT_21 | STXP70_BIT_22 | STXP70_BIT_23 | STXP70_BIT_24 | STXP70_BIT_25 | STXP70_BIT_26 | \
STXP70_BIT_27 | STXP70_BIT_28 | STXP70_BIT_29 | STXP70_BIT_30 | STXP70_BIT_31)
#define SR_IML_SHIFT	0
#define SR_PMASK_SHIFT	16

/* Definition of Processor Control and Status register PCR */

#define PCS_HWTRAP_ENABLE	STXP70_BIT_0 
#define PCS_ARTRAP_ENABLE	STXP70_BIT_1
#define PCS_NONPRE_EXCEP	STXP70_BIT_2
#define PCS_AR_ERR_CODE		(STXP70_BIT_4 | STXP70_BIT_5)
#define PCS_AR_ERR_OVERFLOW	STXP70_BIT_4 
#define PCS_ERR_CODE		(STXP70_BIT_8 | STXP70_BIT_9 | STXP70_BIT_10 | STXP70_BIT_11 | STXP70_BIT_12 | \
STXP70_BIT_13)
#define PCS_ERR_PMISALIGN	STXP70_BIT_8
#define PCS_ERR_PROTECT		STXP70_BIT_9
#define PCS_ERR_OPCODE		(STXP70_BIT_8 | STXP70_BIT_9)
#define PCS_ERR_PEXECUTE	STXP70_BIT_10
#define PCS_ERR_POUTOFMEM	(STXP70_BIT_10 | STXP70_BIT_8)
#define PCS_ERR_PSYSERR		(STXP70_BIT_9 | STXP70_BIT_10)
#define PCS_ERR_DMISALIGN	STXP70_BIT_11
#define PCS_ERR_DOUTOFMEM	(STXP70_BIT_11 | STXP70_BIT_8)
#define PCS_ERR_DREAD		(STXP70_BIT_11 | STXP70_BIT_9)
#define PCS_ERR_DWRITE		(STXP70_BIT_11 | STXP70_BIT_9 | STXP70_BIT_8)
#define PCS_ERR_GPRSIZE		(STXP70_BIT_11 | STXP70_BIT_10)
#define PCS_ERR_DSYSERR		(STXP70_BIT_11 | STXP70_BIT_10 | STXP70_BIT_8) 

#define PCS_ERR_CODE_SHIFT 24

#define STXP70_EXEC_PRECISE STXP70_BIT_NONE
#define STXP70_EXEC_NONPRECISE STXP70_BIT_2


#define MODE0	0x00
#define MODE1	0x01
#define MODE2	0x10
#define MODE3	0x11

/* STxP70 core Functions prototypes */

void STxP70_Init(void);

stxP70_word32 _asm_readsfr_sr(void);
stxP70_word32 _asm_readsfr_pcs(void);
void _asm_writesfr_sr(stxP70_word32 conf);
void _asm_writesfr_pcs(stxP70_word32 conf);
void _asm_idle_mode0();
void _asm_idle_mode1();
void _asm_idle_mode2();
void _asm_idle_mode3();
void _asm_barrier(void);
void _asm_mover2e_psta(stxP70_word32 conf);

void STxP70_Set_Iml(stxP70_word32 new_iml);
stxP70_word32 STxP70_Get_Iml(void);
void STxP70_Set_PrecisionMode(stxP70_word32 PrecisionMode);

#endif /* _STXP70_CORE_H */
