/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 File generated from file : t1xhv_hamac_top_regs.def
 date : Mon Apr 16 15:35:54 MEST 2007
*/



#ifndef T1XHV_HAMAC_TOP_REGS_H
#define T1XHV_HAMAC_TOP_REGS_H


/* ---------------------------------------------------------------------------- */
/*  */
/*   Purpose  :  This file contains the register address definition and bit definition */
/*               of the Top Video Hamac IP.  */
/*  */
/* ---------------------------------------------------------------------------- */
/*  Modification history : */
/*  26.11.2002 : PV - copyed from /vobs/MMTC_DOC/Hamac/Design/REG_DEFINITION */
/*  04.12.2002 : PV - added CFG_PSA / CFG_PEA */
/*  03.01.2003 : PV - added IDN_RRV & IDN_HRV */
/*  12.02.2003 : PV - added CFG_ICE, remove CCIR_HS / CCIR_VS pins */
/*  10.09.2003 : PR - added CFG_CSC */
/*  11.12.2003 : PR - removed SCD registers */
/*  07.01.2004 : PR - replaced SCD registers by TVD. */
/*  13.02.2004 : PR - Added dependencies registers for task (NTD/CTD/ITD) */
/*  10.03.2004 : PR - Added Registers for TVD (<N/C/I>AD/<N/C/I>TS) */
/*  01.04.2005 :JMP - Registers update for version 4.0.2 + bits definition for ITC */
/* ---------------------------------------------------------------------------- */
/* BASE 	CFG_BASE 	0x40000 */
/* BASE 	TST_BASE 	0x5A040 */
/* BASE 	VEC_BASE 	0x40100 */
/* BASE 	VDC_BASE 	0x40180 */
/* BASE 	GRB_BASE 	0x40200 */
/* BASE 	DPL_BASE 	0x40280 */
/* BASE 	TVD_BASE 	0x40300 */
/* BASE 	IDN_BASE 	0x40380 */
/* -------------------------------------------------- */
/* 	Global Config Registers :  */
/*       0x40000 to 0x40010 and 0x5BC00 to 0x5A004 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define CFG_PSA              0x40000
#define CFG_PEA              0x40004
#define CFG_ICE              0x40008
#define CFG_CSC              0x4000C
#define CFG_CGC              0x40010
/*  to ITC */
#define CFG_TIM              0x5BC00
#define CFG_TIC              0x5BC04
#define CFG_IIS              0x5BC20
#define CFG_ISR              0x5BC24
#define CFG_IMR              0x5BC28
/*  to CKG */
#define CFG_CLK              0x58400
#define CFG_RST              0x58404
/* -------------------------------------------------- */
/* 	Test Registers : 0x5A040 to 0x5A048 */
/* -------------------------------------------------- */
/*  to CKG */
#define TST_TCR              0x5A040
#define TST_ITIP             0x5A044
#define TST_ITOP             0x5A048
/* -------------------------------------------------- */
/* 	Video Encode Task Registers :  */
/*       0x40100 to 0x40130 and 0x5BC80 to 0x5BCA8 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define VEC_NAD              0x40100
#define VEC_NTY              0x40104
#define VEC_NTS              0x40108
#define VEC_NTD              0x4010C
#define VEC_CAD              0x40110
#define VEC_CTY              0x40114
#define VEC_CTS              0x40118
#define VEC_CTD              0x4011C
#define VEC_IAD              0x40120
#define VEC_ITY              0x40124
#define VEC_ITS              0x40128
#define VEC_ITD              0x4012C
#define VEC_CNT              0x40130
/*  to ITC */
#define VEC_CTL              0x5BC80
#define VEC_SEM              0x5BC84
#define VEC_STA              0x5BCA0
#define VEC_ISR              0x5BCA4
#define VEC_IMR              0x5BCA8
/* -------------------------------------------------- */
/* 	Video Decode Task Registers :  */
/*       0x40180 to 0x401B0 and 0x5BCC0 to 0x5BCE8 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define VDC_NAD              0x40180
#define VDC_NTY              0x40184
#define VDC_NTS              0x40188
#define VDC_NTD              0x4018C
#define VDC_CAD              0x40190
#define VDC_CTY              0x40194
#define VDC_CTS              0x40198
#define VDC_CTD              0x4019C
#define VDC_IAD              0x401A0
#define VDC_ITY              0x401A4
#define VDC_ITS              0x401A8
#define VDC_ITD              0x401AC
#define VDC_CNT              0x401B0
/*  to ITC */
#define VDC_CTL              0x5BCC0
#define VDC_SEM              0x5BCC4
#define VDC_STA              0x5BCE0
#define VDC_ISR              0x5BCE4
#define VDC_IMR              0x5BCE8
/* -------------------------------------------------- */
/* 	Grab Task Registers :  */
/*       0x40200 to 0x40230 and 0x5BD00 to 0x5BD28 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define GRB_NAD              0x40200
#define GRB_NTY              0x40204
#define GRB_NTS              0x40208
#define GRB_NTD              0x4020C
#define GRB_CAD              0x40210
#define GRB_CTY              0x40214
#define GRB_CTS              0x40218
#define GRB_CTD              0x4021C
#define GRB_IAD              0x40220
#define GRB_ITY              0x40224
#define GRB_ITS              0x40228
#define GRB_ITD              0x4022C
#define GRB_CNT              0x40230
/*  to ITC */
#define GRB_CTL              0x5BD00
#define GRB_SEM              0x5BD04
#define GRB_STA              0x5BD20
#define GRB_ISR              0x5BD24
#define GRB_IMR              0x5BD28
/* -------------------------------------------------- */
/* 	Display Task Registers :  */
/*       0x40280 to 0x402B0 and 0x5BD40 to 0x5BD68 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define DPL_NAD              0x40280
#define DPL_NTY              0x40284
#define DPL_NTS              0x40288
#define DPL_NTD              0x4028C
#define DPL_CAD              0x40290
#define DPL_CTY              0x40294
#define DPL_CTS              0x40298
#define DPL_CTD              0x4029C
#define DPL_IAD              0x402A0
#define DPL_ITY              0x402A4
#define DPL_ITS              0x402A8
#define DPL_ITD              0x402AC
#define DPL_CNT              0x402B0
/*  to ITC */
#define DPL_CTL              0x5BD40
#define DPL_SEM              0x5BD44
#define DPL_STA              0x5BD60
#define DPL_ISR              0x5BD64
#define DPL_IMR              0x5BD68
/* -------------------------------------------------- */
/* 	TV Display Task Registers :  */
/*       0x40300 to 0x40330 and 0x5BD80 to 0x5BDA8 */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define TVD_NAD              0x40300
#define TVD_NTY              0x40304
#define TVD_NTS              0x40308
#define TVD_NTD              0x4030C
#define TVD_CAD              0x40310
#define TVD_CTY              0x40314
#define TVD_CTS              0x40318
#define TVD_CTD              0x4031C
#define TVD_IAD              0x40320
#define TVD_ITY              0x40324
#define TVD_ITS              0x40328
#define TVD_ITD              0x4032C
#define TVD_CNT              0x40330
/*  to ITC */
#define TVD_CTL              0x5BD80
#define TVD_SEM              0x5BD84
#define TVD_STA              0x5BDA0
#define TVD_ISR              0x5BDA4
#define TVD_IMR              0x5BDA8
/* -------------------------------------------------- */
/* 	Identification Registers :  */
/*       0x40380 to 0x40384 and 0x5A1C0 to 0x5A1FC */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define IDN_FRV              0x40380
#define IDN_RRV              0x40384
/*  to CKG */
#define IDN_HRV              0x585C0
#define IDN_PID0             0x585E0
#define IDN_PID1             0x585E4
#define IDN_PID2             0x585E8
#define IDN_PID3             0x585EC
#define IDN_PCID0            0x585F0
#define IDN_PCID1            0x585F4
#define IDN_PCID2            0x585F8
#define IDN_PCID3            0x585FC
/* -------------------------------------------------- */
/* 	 Bit Definition for ITC */
/* -------------------------------------------------- */
/*  Global Interrupt Status Bits */
#define IT_VEC               (0x1<<0)
#define IT_VDC               (0x1<<1)
#define IT_GRB               (0x1<<2)
#define IT_DPL               (0x1<<3)
#define IT_TVD               (0x1<<4)
#define IT_IRQ1              (0x1<<5)
/*  Interrupt Status Bits */
#define IT_BOT               (0x1<<0)
#define IT_EOT               (0x1<<1)
#define IT_ACK               (0x1<<2)
#define IT_EOW               (0x1<<3)
#define IT_BOF               (0x1<<3)
#define IT_EOF1              (0x1<<3)
#define IT_UBU               (0x1<<4)
#define IT_GS                (0x1<<4)
#define IT_DS                (0x1<<4)
#define IT_EOF2              (0x1<<4)
#define IT_BRC               (0x1<<5)
#define IT_BOW               (0x1<<5)
#define IT_EOF               (0x1<<5)
#define IT_ERR               (0x1<<6)
#define IT_EOK               (0x1<<7)
/*  IRQ1 Interrupt Status Register: CFG_IIS */
#define IT_EOI               (0x1<<0)
#define IT_BE                (0x1<<1)
/* -------------------------------------------------- */
/* 	 Bit Definition for CKG */
/* -------------------------------------------------- */
/*  TST_TCR Bits */
#define ITEN                 (0x1<<0)
/*  TST_ITIP Bits */
#define CCP_CLK              (0x1<<0)
#define CCP_DATA             (0x1<<1)
#define CCIR_CLK             (0x1<<2)
#define CCIR_DATA_0          (0x1<<3)
#define CCIR_DATA_1          (0x1<<4)
#define CCIR_DATA_2          (0x1<<5)
#define CCIR_DATA_3          (0x1<<6)
#define CCIR_DATA_4          (0x1<<7)
#define CCIR_DATA_5          (0x1<<8)
#define CCIR_DATA_6          (0x1<<9)
#define CCIR_DATA_7          (0x1<<10)
#define MMDSP_SCLKI2C        (0x1<<11)
#define MMDSP_SDAIN          (0x1<<12)
#define MMDSP_BKIN           (0x1<<13)
#define DISPLAY_SYNC         (0x1<<14)
#define TIMER_ENABLE         (0x1<<15)
#define TIMER_CLK            (0x1<<16)
/*  TST_ITOP Bits */
#define CCP_EN               (0x1<<0)
#define CCIR_EN              (0x1<<1)
#define MMDSP_SDAOUT         (0x1<<2)
#define MMDSP_BKOUT          (0x1<<3)
#define IRQ0                 (0x1<<4)
#define IRQ1                 (0x1<<5)


#endif	/* T1XHV_HAMAC_TOP_REGS_H */

