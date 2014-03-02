/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/opp_init.c$
| $Revision: 1.10$
| $Date: Fri Sep 14 08:01:58 2012 GMT$
|
| $Source: /sources/opp_init.c$
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
| Description:
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/opp_init.c$
|
|   Revision: 1.10 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|
|   Revision: 1.9 Thu Apr 26 13:45:00 2012 GMT frq06447
|   Update header include and some definition to be aligned with soc-settings module
|
|   Revision: 1.8 Mon Apr 16 11:59:02 2012 GMT frq06447
|   Set SAFe retention mode at 950mV and aligned with PRCMU FW DV 3
|
|   Revision: 1.7 Mon Apr 16 07:56:42 2012 GMT frq06447
|   Implementation of ER425573 : [SocSettings] : Remove Retention voltage for APE
|
|   Revision: 1.6 Thu Apr 05 07:25:31 2012 GMT frq06447
|   Implementation of ER 427888 : : [SocSettings]: Update SAFE OPP2 and Ret voltage value
|
|   Revision: 1.5 Fri Mar 30 12:31:50 2012 GMT frq06447
|   Update the No AVS table with AVS FW DOS 1.4b
|
|   Revision: 1.4 Mon Dec 05 17:42:47 2011 GMT frq02595
|   - Remove frequencies value for VSafe and Vape domain (CR#399746)
|   - Have to be align with update in PRCMU fw
|
|   Revision: 1.3 Mon Nov 28 13:33:01 2011 GMT frq02595
|   Update Opp values: Alignement with Datasheet rev4
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
#include <log.h>
#include <soc_settings.h>

__attribute__((section(".type")))
const u32 type = SECTION_TYPE_OPP;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;

__attribute__((section(".payload_opp")))

/**
 ******************************************************************************
 * @var		const signed short opp_payload[]
 * @author	MLA
 * @brief	Based on the structure sPrcmuApi_InitOppData_t (from prcmuApi.h file)
 *			This table is used to set OPP with the InitOppH service (prcmu fw)
 ******************************************************************************
**/
const signed short opp_payload[] = {
		// -------------------------------------------------------------------------
		// --- ARM OPP Config
		// -------------------------------------------------------------------------
		// --- Retention ---
		0,	0,					// ARM Frequency: 0
		650,					// Varm: ARM power supply (in mV)
		600,                    // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		650,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP5 ---
		0x0F10, 0x0004,			// ARM Frequency: 266MHz
		1025,					// Varm: ARM power supply (in mV)
		906,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1025,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP4 ---
		0x1A80, 0x0006,			// ARM Frequency: 400MHz
		1025,					// Varm: ARM power supply (in mV)
		906,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1025,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP3 ---
		0x3500, 0x000C,			// ARM Frequency: 800MHz
		1025,					// Varm: ARM power supply (in mV)
		906,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1025,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP2 ---
		0x4F80, 0x0012,			// ARM Frequency: 1200MHz
		1100,					// Varm: ARM power supply (in mV)
		983,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1100,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP1 ---
		0xE360, 0x0016,			// ARM Frequency: 1500MHz
		1250,					// Varm: ARM power supply (in mV)
		1067,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1250,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP0 ---
		0x3A90, 0x001C,			// ARM Frequency: 1850MHz
		1337,					// Varm: ARM power supply (in mV)
		1126,                   // Varm transistor ( in mV )
		1,						// OppAllowed: Enable=1/Disable=0
		1337,					// Vbbp: P-MOS Body Bias power supply (in mV)
		0,						// Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP0 BOOST---
		0x8480, 0x001E,         // ARM Frequency: 2000MHz
		1393,                   // Varm: ARM power supply (in mV) // TO UPDATE with DROP compensation !!!!!
		1167,                   // Varm transistor ( in mV )
		0,                      // OppAllowed: Enable=1/Disable=0
		1393,                   // Vbbp: P-MOS Body Bias power supply (in mV)
		0,                      // Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP0 2.3Ghz for 8580 compatibility not used for 9540/8540---
		0x1860, 0x0023,         // ARM Frequency: 2300MHz
		1393,                   // Varm: ARM power supply (in mV)
		1167,                   // Varm transistor ( in mV )
		0,                      // OppAllowed: Enable=1/Disable=0
		1393,                   // Vbbp: P-MOS Body Bias power supply (in mV)
		0,                      // Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --- OPP0 2.5Ghz for 8580 compatibility not used for 9540/8540---
		0x25A0, 0x0026,         // ARM Frequency: 2500MHz
		1393,                   // Varm: ARM power supply (in mV)
		1167,                   // Varm transistor ( in mV )
		0,                      // OppAllowed: Enable=1/Disable=0
		1393,                   // Vbbp: P-MOS Body Bias power supply (in mV)
		0,                      // Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

        // --- OPP0 2.8Ghz for 8580 compatibility not used for 9540/8540---
		0xB980, 0x002A,         // ARM Frequency: 2800MHz
		1393,                   // Varm: ARM power supply (in mV)
		1167,                   // Varm transistor ( in mV )
		0,                      // OppAllowed: Enable=1/Disable=0
		1393,                   // Vbbp: P-MOS Body Bias power supply (in mV)
		0,                      // Vbbn: N-MOS Body Bias power supply (in mV)
		0,                      // Padding

		// --------------------------------------------------------------------------
		// --- APE OPP Config
		// --------------------------------------------------------------------------
		// --- OPP50% ---
		0xD090,0x0003,              // GPU frequency
		950,						// Vape: APE power supply (in mV)
		846,                        // Vape transistor (in mV)
		1,						    // OppAllowed: Enable=1/Disable=0
		0,                          // Padding

		// --- OPP100% ---
		0xA120,0x0007,              // GPU frequency
		1137,						// Vape: APE power supply (in mV)
		995,                       // Vape transistor (in mV)
		1,						    // OppAllowed: Enable=1/Disable=0
		0,                          // Padding

		// ----------------------------------------------------------------------------------------
		// --- VSAFE OPP Config
		// ----------------------------------------------------------------------------------------
		// --- Retention ---
		0,0,                        // DDR Frequency
		950,						// Vsafe: VSAFE power supply (in mV)
		900,                        // Vsafe transistor (in mV)
		1,                          // OppAllowed: Enable=1/Disable=0
		0,                          // Padding

		// --- OPP50% ---
		0x0F10,0x0004,              // DDR Frequency
		1025,						// Vsafe: VSAFE power supply (in mV)
		955,                       // Vsafe transistor (in mV)
		0,                          // OppAllowed: Enable=1/Disable=0
		0,                          // Padding

		// --- OPP100% ---
		0x2208,0x0008,              // DDR Frequency
		1087,						// Vsafe: VSAFE power supply (in mV)
		990,                       // Vsafe transistor (in mV)
		1,                          // OppAllowed: Enable=1/Disable=0
		0,                          // Padding

		// ----------------------------------------------------------------------------------------
		// --- PLM OPP SWITCH
		// ----------------------------------------------------------------------------------------
		0x4F80, 0x0012,         // ARM Frequency: 1200MHz OPP2
		1100,                   // Varm: ARM power supply (in mV)
		1100                   // Vsafe : SAFE power supply (in mV)

		};
