/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/ab9540_init.c$
|  $Revision: 1.3$
|  $Date: Mon Nov 28 13:38:49 2011 GMT$
|
|  $Source: /sources/ab9540_init.c$
|
|  Copyright Statement:
|  -------------------
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
| Description:
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/ab9540_init.c$
|
|   Revision: 1.3 Mon Nov 28 13:38:49 2011 GMT frq02595
|   Following to wakeUp feedback information, Remove some AB registers setting.
|
|   Revision: 1.2 Wed Nov 09 14:15:00 2011 GMT frq02595
|   Wake-Up: First delivery for AB9540
|
|   Revision: 1.1 Wed Nov 09 10:54:13 2011 GMT frq02595
|
|  $Aliases: $
|
|
|  $KeysEnd $
|
|
|===========================================================================*/
#include <config.h>
#include <types.h>
#include <ab9540_regs.h>
#include <soc_settings.h>

/*  Type = 1 means AB9540 settings */
__attribute__((section(".type")))
const u32 type = SECTION_TYPE_AB9540;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;

__attribute__((section(".payload_pmu")))

/**
 ******************************************************************************
 * @var		const u8 ab9540_init[]
 * @author	MLA
 * @brief
 ******************************************************************************
**/
const u16 ab9540_init[] = {

		//	ACCESS type, AB9540 Register Addr, 			Value,	Mask
			WRITE_ONLY,	AB9540_REGUSERIALCTRL1, 		0x02,	0x00,	// write 0x2 in @0x300 (ReguSerialCtrl1 register)

			WRITE_ONLY,	AB9540_BANK12ACCESS,			0x01,	0x00,	// Enter PBT Mode
			WRITE_ONLY,	AB9540_DEBUGVARMREGU,			0x31,	0x00,	// Default value for Varm's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_DEBUGVMODREGU,			0x30,	0x00,	// Default value for Vmod's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_DEBUGVAPEREGU,			0x30,	0x00,	// Default value for Vape's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_DEBUGVSMPS1REGU,			0x30,	0x00,	// Default value for Vsmps1's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_DEBUGVSMPS2REGU,			0x30,	0x00,	// Default value for Vsmps2's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_DEBUGVSMPS3REGU,			0x30,	0x00,	// Default value for Vsmps3's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB9540_BANK12ACCESS,			0x00,	0x00,	// Exit PBT Mode
			READ_WRITE,	AB9540_VANAVPLLSEL,				0x00,	0x07,	// Vana = 1.2V
/*
			WRITE_ONLY,	AB9540_SYSTEMCTRL2, 			0xF7,	0x00,	// SystemCtrl2 @0x181
//			READ_WRITE, AB9540_SYSCLKREQ1VALID,			0x00, 	0x01,	// SysClkReq1Valid @0x20D (bit0 = 0)
			READ_WRITE,	AB9540_VSIMSYSCLKCTRL, 			0x01,	0x00,	// VsimSysClkCtrl @0x233 (bit0 = 0)
			WRITE_ONLY,	AB9540_REGUREQUESTCTRL1, 		0x00,	0x00,	// ReguRequestCtrl1 @0x303 (bit0-7 = 0x0)
			READ_WRITE,	AB9540_REGUREQUESTCTRL2, 		0x00,	0x0F,  	// ReguRequestCtrl2 @0x304 (bit0-3 = 0x0)
			READ_WRITE,	AB9540_REGUSYSCLKREQ1HPVALID1,	0x17,	0x17,  	// ReguSysClkReq1HPValid1 @0x307 (bit0-2 bit4 = 0x17)
			READ_WRITE,	AB9540_REGUSYSCLKREQ1HPVALID2,	0x00,	0x0F,  	// ReguSysClkReq1HPValid2 @0x308 (bit0-3 = 0x0)
			READ_WRITE, AB9540_REGUHWHPREQ1VALID1, 		0x02,	0x17, 	// ReguHwHPReq1Valid1 @0x309 (bit0-2 bit4 = 0x2)
			READ_WRITE, AB9540_REGUHWHPREQ1VALID2,		0x00,	0x08,	// ReguHwHPReq1Valid2 @0x30A (bit3 = 0x0)
			READ_WRITE, AB9540_REGUHWHPREQ2VALID1, 		0x00,	0x17,	// ReguHwHPReq2Valid1 @0x30B (bit0-2 bit4 = 0x0)
			READ_WRITE, AB9540_REGUHWHPREQ2VALID2,		0x00,	0x08,	// ReguHwHPReq2Valid2 @0x30C (bit8 = 0x0)
			READ_WRITE, AB9540_REGUSWHPREQVALID1,		0x00,	0x5F,	// ReguSwHPReqValid1 @0x30D (bit0-3 bit4 bit6 = 0x0)
			READ_WRITE,	AB9540_REGUSWHPREQVALID2, 		0x00,	0x20,	// ReguSwHPReqValid2 @0x30E (bit5 = 0x0)
			WRITE_ONLY, AB9540_VARMREGU1,				0x01,	0x00,	// Varmregu1 @0x400
			WRITE_ONLY, AB9540_VARMREGU2,				0x45,	0x00,	// Varmregu2 @0x401
			WRITE_ONLY,	AB9540_VAPEREGU,				0x01,	0x00,	// Vaperegu	@0x402
			READ_WRITE, AB9540_VSMPS1REGU,				0x06,	0x3F,	// Vsmps1regu @0x403 (bit0-5 = 0x6)
			READ_WRITE, AB9540_VSMPS2REGU,				0x06,	0x3F,	// Vsmps2regu @0x404 (bit0-5 = 0x6)
			READ_WRITE, AB9540_VSMPS3REGU,				0x02,	0x3F,	// Vsmps3regu  @0x405 (bit0-5 = 0x2)
			READ_WRITE, AB9540_VPLLVANAREGU,			0x02,	0x03,	// VpllVanaregu @0x406 (bit0-1 = 0x2)
			READ_WRITE, AB9540_VRF1AUX3REGU,			0x08,	0x0C,	// VRF1Vaux3regu @0x40A (bit2-3 = 0x8)
			WRITE_ONLY, AB9540_VAPESEL3,				0x28,	0x00,	// Vapesel3 @0x410
			WRITE_ONLY, AB9540_VSMPS1SEL1,				0x24,	0x00,	// Vsmps1Sel1 @0x413
			WRITE_ONLY, AB9540_VSMPS1SEL2,				0x28,	0x00,	// Vsmps1Sel2 @0x0x414
			WRITE_ONLY, AB9540_VSMPS1SEL3,				0x28,	0x00,	// Vsmps1Sel3 @0x415
			WRITE_ONLY, AB9540_VSMPS2SEL1,				0x39,	0x00,	// Vsmps2Sel1 @0x417
			WRITE_ONLY, AB9540_VSMPS2SEL2,				0x39,	0x00,	// Vsmps2Sel2 @0x418
			WRITE_ONLY, AB9540_VSMPS2SEL3,				0x39,	0x00,	// Vsmps2Sel3 @0x419
			READ_WRITE, AB9540_REGUCTRLDISCH,			0x00,	0x03,	// ReguCtrlDisch @0x443 (bit0-1 = 0x0)
			READ_WRITE, AB9540_REGUCTRLDISCH2,			0x00,	0x09,	// ReguCtrlDisch2 @0x444 (bit0 bit3)
*/
			MAX_REG,	0,								0,		0		// Indicate end of the Table
};

