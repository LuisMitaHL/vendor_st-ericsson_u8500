/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/ab8540_init.c$
|  $Revision: 1.3$
|  $Date: Mon Nov 28 13:38:49 2011 GMT$
|
|  $Source: /sources/ab8540_init.c$
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
#include <ab8540_regs.h>
#include <soc_settings.h>

/*  Type = 1 means AB8540 settings */
__attribute__((section(".type")))
const u32 type = SECTION_TYPE_AB8540;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;

__attribute__((section(".payload_pmu")))

/**
 ******************************************************************************
 * @var		const u8 ab8540_init[]
 * @author	MLA
 * @brief
 ******************************************************************************
**/
const u16 ab8540_init[] = {

		//	ACCESS type, AB9540 Register Addr, 			Value,	Mask

			WRITE_ONLY,	AB8540_BANK12ACCESS,			0x01,	0x00,	// Enter PBT Mode
			WRITE_ONLY,	AB8540_DEBUGVARMREGU,			0x31,	0x00,	// Default value for Varm's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVMODREGU,			0x30,	0x00,	// Default value for Vmod's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVAPEREGU,			0x30,	0x00,	// Default value for Vape's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVSMPS1REGU,			0x30,	0x00,	// Default value for Vsmps1's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVSMPS2REGU,			0x30,	0x00,	// Default value for Vsmps2's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVSMPS3REGU,			0x30,	0x00,	// Default value for Vsmps3's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVRF2REGU,			0x30,	0x00,	// Default value for Vrf2's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_DEBUGVRFTXREGU,			0x30,	0x00,	// Default value for Vrftx's PMOS and NMOS switches drivers
			WRITE_ONLY,	AB8540_BANK12ACCESS,			0x00,	0x00,	// Exit PBT Mode
			MAX_REG,	0,								0,		0		// Indicate end of the Table
};
