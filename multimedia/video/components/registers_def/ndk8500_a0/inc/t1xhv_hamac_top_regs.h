/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 File generated from file : t1xhv_hamac_top_regs.def
 date : Mon Oct 06 10:37:03 MEST 2008
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
/*  26.09.2009 : AB - Registers update for the 8500-v1 version  */
/* ---------------------------------------------------------------------------- */
/* -------------------------------------------------- */
/* 	Top level Registers :  */
/* -------------------------------------------------- */
/* -------------------------------------------------- */
/*  to be removed when 8500V1 programming model available */
/*  to MMDSP Memory */
#define CFG_PSA              0x40000
#define CFG_PEA              0x40004
#define CFG_ICE              0x40008
#define CFG_CSC              0x4000C
#define CFG_CGC              0x40010
/* -------------------------------------------------- */
/*  to ITC */
#define CFG_TIM              0x5BC00
#define CFG_TIC              0x5BC04
#define CFG_IIS              0x5BC20
#define CFG_ISR              0x5BC24
#define CFG_IMR              0x5BC28
/* -------------------------------------------------- */
/*  to CKG */
#define CFG_RST              0x58800
#define CKG_CKEN             0x58804
#define IDN_HRV              0x58808
/* -------------------------------------------------- */
/*  to DMA */
#define DMA_MAX_OPC_SZ0      0x58600
#define DMA_MAX_OPC_SZ1      0x58602
#define DMA_MAX_OPC_SZ2      0x58604
#define DMA_MAX_OPC_SZ3      0x58606
#define DMA_MAX_OPC_SZ4      0x58608
#define DMA_MAX_OPC_SZ5      0x5860A
#define DMA_MAX_OPC_SZ6      0x5860C
#define DMA_MAX_OPC_SZ7      0x5860E
#define DMA_MAX_OPC_SZ8      0x58610
#define DMA_MAX_OPC_SZ9      0x58612
#define DMA_MAX_OPC_SZ10     0x58614
#define DMA_MAX_OPC_SZ11     0x58616
#define DMA_MAX_OPC_SZ12     0x58618
#define DMA_MAX_OPC_SZ13     0x5861A
#define DMA_MAX_OPC_SZ14     0x5861C
#define DMA_MAX_OPC_SZ15     0x5861E
#define DMA_MAX_OPC_SZ16     0x58620
#define DMA_MAX_OPC_SZ17     0x58622
#define DMA_MAX_OPC_SZ18     0x58624
#define DMA_MAX_OPC_SZ19     0x58626
#define DMA_MAX_OPC_SZ20     0x58628
#define DMA_MAX_OPC_SZ21     0x5862A
#define DMA_MAX_OPC_SZ22     0x5862C
#define DMA_MAX_OPC_SZ23     0x5862E
#define DMA_MAX_OPC_SZ24     0x58630
#define DMA_PRIO_0           0x58640
#define DMA_PRIO_1           0x58642
#define DMA_PRIO_2           0x58644
#define DMA_PRIO_3           0x58646
#define DMA_PRIO_4           0x58648
#define DMA_PRIO_5           0x5864A
#define DMA_PRIO_6           0x5864C
#define DMA_PRIO_7           0x5864E
#define DMA_PRIO_8           0x58650
#define DMA_PRIO_9           0x58652
#define DMA_PRIO_10          0x58654
#define DMA_PRIO_11          0x58656
#define DMA_PRIO_12          0x58658
#define DMA_PRIO_13          0x5865A
#define DMA_PRIO_14          0x5865C
#define DMA_PRIO_15          0x5865E
#define DMA_PRIO_16          0x58660
#define DMA_PRIO_17          0x58662
#define DMA_PRIO_18          0x58664
#define DMA_PRIO_19          0x58666
#define DMA_PRIO_20          0x58668
#define DMA_PRIO_21          0x5866A
#define DMA_PRIO_22          0x5866C
#define DMA_PRIO_23          0x5866E
#define DMA_PRIO_24          0x58670
#define DMA_MAX_MSSG_SZ0     0x58680
#define DMA_MAX_MSSG_SZ1     0x58682
#define DMA_MAX_MSSG_SZ2     0x58684
#define DMA_MAX_MSSG_SZ3     0x58686
#define DMA_MAX_MSSG_SZ4     0x58688
#define DMA_MAX_MSSG_SZ5     0x5868A
#define DMA_MAX_MSSG_SZ6     0x5868C
#define DMA_MAX_MSSG_SZ7     0x5868E
#define DMA_MAX_MSSG_SZ8     0x58690
#define DMA_MAX_MSSG_SZ9     0x58692
#define DMA_MAX_MSSG_SZ10    0x58694
#define DMA_MAX_MSSG_SZ11    0x58696
#define DMA_MAX_MSSG_SZ12    0x58698
#define DMA_MAX_MSSG_SZ13    0x5869A
#define DMA_MAX_MSSG_SZ14    0x5869C
#define DMA_MAX_MSSG_SZ15    0x5869E
#define DMA_MAX_MSSG_SZ16    0x586A0
#define DMA_MAX_MSSG_SZ17    0x586A2
#define DMA_MAX_MSSG_SZ18    0x586A4
#define DMA_MAX_MSSG_SZ19    0x586A6
#define DMA_MAX_MSSG_SZ20    0x586A8
#define DMA_MAX_MSSG_SZ21    0x586AA
#define DMA_MAX_MSSG_SZ22    0x586AC
#define DMA_MAX_MSSG_SZ23    0x586AE
#define DMA_MAX_MSSG_SZ24    0x586B0
#define DMA_MAX_CHCK_SZ0     0x586C0
#define DMA_MAX_CHCK_SZ1     0x586C2
#define DMA_MAX_CHCK_SZ2     0x586C4
#define DMA_MAX_CHCK_SZ3     0x586C6
#define DMA_MAX_CHCK_SZ4     0x586C8
#define DMA_MAX_CHCK_SZ5     0x586CA
#define DMA_MAX_CHCK_SZ6     0x586CC
#define DMA_MAX_CHCK_SZ7     0x586CE
#define DMA_MAX_CHCK_SZ8     0x586D0
#define DMA_MAX_CHCK_SZ9     0x586D2
#define DMA_MAX_CHCK_SZ10    0x586D4
#define DMA_MAX_CHCK_SZ11    0x586D6
#define DMA_MAX_CHCK_SZ12    0x586D8
#define DMA_MAX_CHCK_SZ13    0x586DA
#define DMA_MAX_CHCK_SZ14    0x586DC
#define DMA_MAX_CHCK_SZ15    0x586DE
#define DMA_MAX_CHCK_SZ16    0x586E0
#define DMA_MAX_CHCK_SZ17    0x586E2
#define DMA_MAX_CHCK_SZ18    0x586E4
#define DMA_MAX_CHCK_SZ19    0x586E6
#define DMA_MAX_CHCK_SZ20    0x586E8
#define DMA_MAX_CHCK_SZ21    0x586EA
#define DMA_MAX_CHCK_SZ22    0x586EC
#define DMA_MAX_CHCK_SZ23    0x586EE
#define DMA_MAX_CHCK_SZ24    0x586F0
#define DMA_MIN_OPC_SZ       0x586F4
#define DMA_PAGE_SZ_SRAM     0x586F6
#define DMA_PAGE_SZ_DRAM     0x586F8
#define DMA_PORT_CTRL_L      0x586FA
#define DMA_PORT_CTRL_H      0x586FC
#define DMA_MAX_PRI          0x586FE

/* -------------------------------------------------- */
/*  to STBNODE */
#define NODE_INIT_1_PRIORITY_REG 0x58700
#define NODE_INIT_2_PRIORITY_REG 0x58704
#define NODE_INIT_3_PRIORITY_REG 0x58708
#define NODE_INIT_1_LATENCY_REG 0x5870C
#define NODE_INIT_2_LATENCY_REG 0x58710
#define NODE_INIT_3_LATENCY_REG 0x58714
#define NODE_INIT_1_LIMIT_REG 0x58724
#define NODE_INIT_2_LIMIT_REG 0x58728
#define NODE_INIT_3_LIMIT_REG 0x5872C
/* -------------------------------------------------- */
/* 	Video Encode Task Registers :  */
/*  to be removed when 8500V1 programming model available */
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
/*  to be removed when 8500V1 programming model available */
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
/*  to be removed when 8500V1 programming model available */
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
/*  to be removed when 8500V1 programming model available */
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
/*  to be removed when 8500V1 programming model available */
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
/*  to be removed when 8500V1 programming model available */
/* -------------------------------------------------- */
/*  to MMDSP Memory */
#define IDN_FRV              0x40380
#define IDN_RRV              0x40384
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


#endif	/* T1XHV_HAMAC_TOP_REGS_H */

