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
| Description: definition of AB9540 registers
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

#ifndef __AB9540_INIT_H__
#define __AB9540_INIT_H__


#define WRITE_ONLY					0
#define READ_WRITE					1
#define MAX_REG						2

// --------------------------------------------------
// --- Register Bank 1: SYSTEM CONTROL 1
// --------------------------------------------------
#define AB9540_SYSTEMCTRL1				0x0180
#define AB9540_SYSTEMCTRL2				0x0181


// --------------------------------------------------
// --- Register Bank 2: SYSTEM CONTROL 2
// --------------------------------------------------
#define AB9540_VSIMSYSCLKCTRL		        	0x0233

#define AB9540_SYSCLKREQ1VALID	        		0x020D


// --------------------------------------------------
// --- Register Bank 3: REGULATOR CONTROL 1
// --------------------------------------------------
/* AB9540 addr 0x03xx */
#define AB9540_REGUSERIALCTRL1		         	0x0300
#define AB9540_REGUREQUESTCTRL1		        	0x0303
#define AB9540_REGUREQUESTCTRL2		        	0x0304
#define AB9540_REGUSYSCLKREQ1HPVALID1           	0x0307
#define AB9540_REGUSYSCLKREQ1HPVALID2            	0x0308
#define AB9540_REGUHWHPREQ1VALID1        		0x0309
#define AB9540_REGUHWHPREQ1VALID2	        	0x030A
#define AB9540_REGUHWHPREQ2VALID1  	        	0X030B
#define AB9540_REGUHWHPREQ2VALID2	        	0X030C
#define AB9540_REGUSWHPREQVALID1            		0X030D
#define AB9540_REGUSWHPREQVALID2         		0X030E

// --------------------------------------------------
// --- Register Bank 4: REGULATOR CONTROL 2
// --------------------------------------------------
#define AB9540_VARMREGU1				0x0400
#define AB9540_VARMREGU2				0x0401
#define AB9540_VAPEREGU					0x0402
#define AB9540_VSMPS1REGU				0x0403
#define AB9540_VSMPS2REGU				0x0404
#define AB9540_VSMPS3REGU				0x0405
#define AB9540_VPLLVANAREGU				0x0406
#define AB9540_VREFDDR					0x0407
#define AB9540_EXTSUPPLYREGU			        0x0408
#define AB9540_VAUX12REGU				0x0409
#define AB9540_VRF1AUX3REGU				0x040A
#define AB9540_VARMSEL1					0x040B
#define AB9540_VARMSEL2					0x040C
#define AB9540_VARMSEL3					0x040D
#define AB9540_VAPESEL1					0x040E
#define AB9540_VAPESEL2					0x040F
#define AB9540_VAPESEL3					0x0410
#define AB9540_VBBSEL1					0x0411
#define AB9540_VBBSEL2					0x0412
#define AB9540_VSMPS1SEL1				0x0413
#define AB9540_VSMPS1SEL2				0x0414
#define AB9540_VSMPS1SEL3				0x0415

/* 0x0416 does not exist */

#define AB9540_VSMPS2SEL1				0x0417
#define AB9540_VSMPS2SEL2				0x0418
#define AB9540_VSMPS2SEL3				0x0419

/* 0x041A does not exist */

#define AB9540_VSMPS3SEL1				0x041B
#define AB9540_VSMPS3SEL2				0x041C
#define AB9540_VSMPS3SEL3				0x041D

/* 0x041E does not exist */

#define AB9540_VAUX1SEL					0x041F
#define AB9540_VAUX2SEL					0x0420
#define AB9540_VRF1VAUX3SEL				0x0421
#define AB9540_REGUCTRLEXTSUP			        0x0422
#define AB9540_VANAVPLLSEL				0x0429

#define AB9540_VMODREGU					0x0440
#define AB9540_VMODSEL1					0x0441
#define AB9540_VMODSEL2					0x0442
#define AB9540_REGUCTRLDISCH			        0x0443
#define AB9540_REGUCTRLDISCH2			        0x0444


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

#endif /* __AB9540_INIT_H__ */

