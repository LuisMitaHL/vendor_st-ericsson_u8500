/*----------------------------------------------------------------------------*
 *      Copyright 2005, STMicroelectronics, Incorporated.                     *
 *      All rights reserved.                                                  *
 *                                                                            *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *
 * This software is supplied under the terms of a license agreement or nondis-*
 * closure agreement with STMicroelectronics and may not be copied or disclo- *
 * sed except in accordance with the terms of that agreement.                 *
 *----------------------------------------------------------------------------*
 * System           : STxP70                                         	      *
 * Project Component: System Library                                          *
 * File Name        : STxP70_type.h                                           *
 * Purpose          : Standard definition for the data types                  *
 * History          : 2004/11/18 - First implementation.                      *
 *                  : 2005/05/16 - rename registers + added registers + macros*
 *----------------------------------------------------------------------------*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 

#ifndef __STXP70TYPE_H
#define __STXP70TYPE_H

typedef char			stxP70_int8;            /* signed 8 bits integer  */
typedef unsigned char		stxP70_byte;       	/* unsigned 8 bits integer */
typedef short			stxP70_int16;           /* signed 16 bits integer */
typedef unsigned short		stxP70_word16;      	/* unsigned 16 bits integer */
typedef int			stxP70_int32;           /* signed 32 bits integer */
typedef unsigned int		stxP70_word32;        	/* unsigned 32 bits integer */
typedef long long		stxP70_int64;           /* signed 64 bits integer */
typedef unsigned long long	stxP70_word64;  	/* unsigned 64 bits integer */

typedef enum {false=0,true=1} stxP70_boolean;

#ifndef NULL
#define NULL 0
#endif

/* Define for IO register */
typedef volatile stxP70_word32 stxP70_tRegister;

/* set clear configuration register */ 
typedef struct {
   stxP70_tRegister val;  	/* direct value register  */
   stxP70_tRegister set;      	/* set register      */
   stxP70_tRegister clear;    	/* clear register    */
   stxP70_tRegister toggle; 	/* toggle register */
} Config_Register_st ;

/*----------------*/
/* Generic Macro  */
/*----------------*/

#define STXP70_SET_BIT(reg,mask)        (reg |= (stxP70_tRegister)(mask))
#define STXP70_CLEAR_BIT(reg,mask)      (reg &= (stxP70_tRegister)(~(mask)))
#define STXP70_READ_BIT(reg,mask)       (reg & (stxP70_tRegister)(mask))
#define STXP70_WRITE_BIT(reg,val,mask)  (reg = ((reg & (stxP70_tRegister)(~(mask))) | ((val)&(stxP70_tRegister)(mask))))
#define STXP70_READ_REG(reg)            (reg)
#define STXP70_WRITE_REG(reg,val)       (reg = (val))
#define STXP70_TEST_BIT(reg,mask)       STXP70_READ_BIT(reg,mask)

#define STXP70_BIT_0    0x0001
#define STXP70_BIT_1    0x0002
#define STXP70_BIT_2    0x0004
#define STXP70_BIT_3    0x0008
#define STXP70_BIT_4    0x0010
#define STXP70_BIT_5    0x0020
#define STXP70_BIT_6    0x0040
#define STXP70_BIT_7    0x0080
#define STXP70_BIT_8    0x0100
#define STXP70_BIT_9    0x0200
#define STXP70_BIT_10   0x0400
#define STXP70_BIT_11   0x0800
#define STXP70_BIT_12   0x1000
#define STXP70_BIT_13   0x2000
#define STXP70_BIT_14   0x4000
#define STXP70_BIT_15   0x8000
#define STXP70_BIT_16   0x00010000
#define STXP70_BIT_17   0x00020000
#define STXP70_BIT_18   0x00040000
#define STXP70_BIT_19   0x00080000
#define STXP70_BIT_20   0x00100000
#define STXP70_BIT_21   0x00200000
#define STXP70_BIT_22   0x00400000
#define STXP70_BIT_23   0x00800000
#define STXP70_BIT_24   0x01000000
#define STXP70_BIT_25   0x02000000
#define STXP70_BIT_26   0x04000000
#define STXP70_BIT_27   0x08000000
#define STXP70_BIT_28   0x10000000
#define STXP70_BIT_29   0x20000000
#define STXP70_BIT_30   0x40000000
#define STXP70_BIT_31   0x80000000

#define STXP70_BIT_A    STXP70_BIT_10
#define STXP70_BIT_B    STXP70_BIT_11
#define STXP70_BIT_C    STXP70_BIT_12
#define STXP70_BIT_D    STXP70_BIT_13
#define STXP70_BIT_E    STXP70_BIT_14
#define STXP70_BIT_F    STXP70_BIT_15

#define STXP70_BIT_NONE    0x0
#define STXP70_BIT_LSB	   0xFFFF
#define STXP70_BIT_MSB	   0xFFFF0000
#define STXP70_BIT_ALL	   0xFFFFFFFF
#define STXP70_LREG_ONES      0x0000FFFF

#define STXP70_BIT_LIT_ARG(n) (STXP70_BIT_##n)
#define STXP70_BIT(n)         STXP70_BIT_LIT_ARG(n)

#define STXP70_SYSTEM_LIB_VERSION	"1.0.0"

#endif /* __STXP70TYPE_H */
