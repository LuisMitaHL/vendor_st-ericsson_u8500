/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/ab9540_regs.h$
| $Revision: 1.0$
| $Date: Wed Nov 09 14:15:00 2011 GMT$
|
| $Source: /sources/ab9540_regs.h$
|
| Copyright Statement:
| -------------------
| The confidential and proprietary information contained in this file may
| only be used by a person authorized under and to the extent permitted
| by a subsisting licensing agreement from ST-Ericsson S.A.
|
| Copyright (C) ST-Ericsson S.A. 2011. All rights reserved.
|
| This entire notice must be reproduced on all copies of this file
| and copies of this file may only be made by a person if such person is
| permitted to do so under the terms of a subsisting license agreement
| from ST-Ericsson S.A..
|
|
| Project :  AP9540
| -------
|
| Description: definition of AB8540 registers
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/ab9540_init.h$
|
|
|  $Aliases: $
|
|
|  $KeysEnd $
|
|
|===========================================================================*/

#ifndef __AB8540_INIT_H__
#define __AB8540_INIT_H__


#define WRITE_ONLY					0
#define READ_WRITE					1
#define MAX_REG						2


// --------------------------------------------------
// --- Register Bank 3: REGULATOR CONTROL 1
// --------------------------------------------------
/* AB9540 addr 0x03xx */
#define AB9540_REGUSERIALCTRL1		         	0x0300

// --------------------------------------------------
// --- Register Bank 0x11 - Debug registers
// --------------------------------------------------
#define AB9540_BANK12ACCESS				0x1100

// --------------------------------------------------
// --- Register Bank 0x12 - Debug registers
// --------------------------------------------------
#define AB9540_DEBUGVARMREGU			0x120C
#define AB9540_DEBUGVMODREGU			0x120D
#define AB9540_DEBUGVAPEREGU			0x120E
#define AB9540_DEBUGVSMPS1REGU			0x120F
#define AB9540_DEBUGVSMPS2REGU			0x1210
#define AB9540_DEBUGVSMPS3REGU			0x1211

// --------------------------------------------------
// --- Register Bank 0x11 - Debug registers
// --------------------------------------------------
#define AB8540_BANK12ACCESS				0x1100

// --------------------------------------------------
// --- Register Bank 0x12 - Debug registers
// --------------------------------------------------
#define AB8540_DEBUGVARMREGU			0x120C
#define AB8540_DEBUGVMODREGU			0x120D
#define AB8540_DEBUGVAPEREGU			0x120E
#define AB8540_DEBUGVSMPS1REGU			0x120F
#define AB8540_DEBUGVSMPS2REGU			0x1210
#define AB8540_DEBUGVSMPS3REGU			0x1211
#define AB8540_DEBUGVRF2REGU			0x1212
#define AB8540_DEBUGVRFTXREGU			0x1213

#endif /* __AB8540_INIT_H__ */

