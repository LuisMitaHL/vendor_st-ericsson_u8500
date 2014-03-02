/*----------------------------------------------------------------------------*
 *      Copyright 2005, STMicroelectronics, Incorporated.                     *
 *      All rights reserved.                                                  *
 *                                                                            *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *
 * This software is supplied under the terms of a license agreement or nondis-*
 * closure agreement with STMicroelectronics and may not be copied or disclo- *
 * sed except in accordance with the terms of that agreement.                 *
 *----------------------------------------------------------------------------*
 * System           : STxP70                                                  *
 * Project component: System Library                                          *
 * File name        : STxP70_itc.h                                            *
 * Purpose : Interrupt controller driver                                      *
 * History          : 2004/12/8 - First implementation.                       *
 *		              2005/05/16 - Modify names, add new structures and new   *
 *				      functions	     			                              *
 *				      2007/05/01 - Optimization for code size                 *
 *----------------------------------------------------------------------------*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 


#ifndef _STXP70_ITC_H_
#define _STXP70_ITC_H_

#include "stxp70_config.h"
#include "stxp70_core.h"
#include "stxp70_type.h"


/* ITC registers defined through a structure */

#define IT_0        0
#define IT_1        1
#define IT_2        2
#define IT_3        3
#define IT_4        4
#define IT_5        5
#define IT_6        6
#define IT_7        7
#define IT_8        8
#define IT_9        9
#define IT_10       10
#define IT_11       11
#define IT_12       12
#define IT_13       13
#define IT_14       14
#define IT_15       15
#define IT_16       16
#define IT_17       17
#define IT_18       18
#define IT_19       19
#define IT_20       20
#define IT_21       21
#define IT_22       22
#define IT_23       23
#define IT_24       24
#define IT_25       25
#define IT_26       26
#define IT_27       27
#define IT_28       28
#define IT_29       29
#define IT_30       30


#define ITC_IL       (STXP70_BIT_0|STXP70_BIT_1|STXP70_BIT_2|STXP70_BIT_3|STXP70_BIT_4)
#define ITC_ISY     STXP70_BIT_13
#define ITC_IE      STXP70_BIT_14
#define ITC_IST     STXP70_BIT_15

/* To enable or disable all interrupt */
#define ITC_GLOBAL 0xFFFFFFFF

/* Low-level Functions prototypes for setting/getting configuration of interrupt, enable/disable interrupts */

void _asm_set_it(stxP70_tRegister ID, stxP70_tRegister conf);
void _asm_set_nmi(stxP70_tRegister conf);
void _asm_set_it_enable(stxP70_tRegister IDmask);
stxP70_tRegister _asm_get_it(stxP70_tRegister ID);
stxP70_tRegister _asm_get_nmi(void);
stxP70_tRegister _asm_get_it_enable(void);


stxP70_int32 ITC_Set_Level(stxP70_word32 ID, stxP70_word32 Level);


/* High level functions prototypes for ITC and Handler initialization */
void ITC_Disable_Enable(stxP70_word32 IDmask, stxP70_boolean Enable);
#define ITC_ENABLE(a)      ITC_Disable_Enable(a,1)
#define ITC_DISABLE(a)     ITC_Disable_Enable(a,0)

stxP70_int32 ITC_Pending(stxP70_word32 ID);
stxP70_int32 ITC_Clear_Set_Pending(stxP70_word32 ID,stxP70_boolean Set);
#define ITC_SET_PENDING(a)	ITC_Clear_Set_Pending(a,1)
#define ITC_CLEAR_PENDING(a)	ITC_Clear_Set_Pending(a,0)

stxP70_int32 ITC_Clear_Set_Synchro(stxP70_word32 ID, stxP70_boolean Set);
#define ITC_SET_SYNCHRO(a)       ITC_Clear_Set_Synchro(a,1)
#define ITC_CLEAR_SYNCHRO(a)     ITC_Clear_Set_Synchro(a,0)

void ITC_Init_StaticIrq(stxP70_tRegister ID, stxP70_tRegister Level, stxP70_boolean Synchro, stxP70_boolean Enable, stxP70_boolean Trigger);

#endif //_STXP70_ITC_H_
