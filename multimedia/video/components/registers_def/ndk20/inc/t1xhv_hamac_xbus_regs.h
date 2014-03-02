/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 File generated from file : t1xhv_hamac_xbus_regs.def
 date : Tue Sep 11 14:22:19 MEST 2007
*/



#ifndef T1XHV_HAMAC_XBUS_REGS_H
#define T1XHV_HAMAC_XBUS_REGS_H


/* ----------------------------------------------------------------------------- */
/*  */
/*   Purpose  :  This file contains the register address definition */
/*               of the Hamac internal Registers connected on MMDSP Xbus */
/*  */
/* ---------------------------------------------------------------------------- */
/*  Modification history : */
/*  26.11.2002 : PV - copyed from /vobs/MMTC_DOC/Hamac/Design/REG_DEFINITION */
/*  26.11.2002 : PV - added Software registers and CFG_BASE */
/*  04.12.2002 : PV - fixed software registers with _L / _H */
/*  08.01.2003 : PV - update of DMA TST registers */
/*  13.01.2003 : PV - added bit GRAB_RAW_W for DMA */
/*  20.01.2003 : PV - added DMA priority registers, added MECC MBX / MBY registers */
/*  29.01.2003 : PV - added PIF regs for CCP */
/*  30.01.2003 ; PV - added DMA Grab RAW end address */
/*  30.01.2003 ; PV - added identification version HV_HW_REL = 2.3.2 */
/*  03.02.2003 ; PV - added identification version IDN_RRV & IDN_HRV */
/*  30.01.2003 ; YM - renamed PPP regs SFH/SFW/DFH/DFW into SWW/SWH/CWW/CWH to comply */
/*                    with HamacV spec params naming conventions */
/*                  - added PPP_CON/PPP_BRI for contrast/brightness control */
/*  12.02.2003 ; PV - added CFG_ICE */
/*  17.02.2003 ; PV - updated version HV_HW_REL = 2.4.0 */
/*  13.03.2003 ; PV - updated version HV_HW_REL = 2.4.1 */
/*  15.04.2003 ; PYM / PR - added regs for PIF full */
/*  15.04.2003 ; PV / PR - removed regs for PIF full. added HV_HW_REL = 2.4.2 */
/*  26.05.2003 : PV / PR - readded regs for PIF full. added HV_HW_REL = 2.5.0 */
/*  18.06.2003 : PYM - Grab cache regs removed for mupoc full - HV_HW_REL = 3.0.0 */
/*  03.09.2003 : PYM - DMA reg size modif for XVGA - HV_HW_REL = 3.0.2 */
/*  09.09.2003 : PR  - added CSC for Grab. Modified XXX_CNT => XXX_CNT_L, XXX_CNT_H */
/*  13.10.2003 : PYM - PIF_CPACK and PIF_LPACK merged in PIF_PACK - HV_HW_REL = 3.0.3 */
/*  05.11.2003 : PYM - PIF_CCIR_EXT_SYNC reg added - HV_HW_REL = 3.0.4 */
/*  12.11.2003 : PR  - DMA for TV_OUT added - HV_HW_REL = 3.0.4 */
/*  02.12.2003 : PR  - PPP for Color Boosting added - HV_HW_REL_ 3.1.0 */
/*  09.12.2003 : PR  - Add registers for TVO (PIF and DMA) - HV_HW_REL_ 3.1.0 */
/*  10.12.2003 : PR  - Commentaries on all SCD registers - HV_HW_REL_ 3.1.0 */
/*  19.12.2003 : PR  - Add registers ACE for PPP and PIF - HV_HW_REL_ 3.2.0 */
/*  06.01.2004 : PR  - Update/Add registers for PPP (Resize/CSC), and for TVO (Background) - HV_HW_REL_ 3.2.1 */
/*  06.01.2004 : PR  - Add registers for PPP (IT Sync Row) - HV_HW_REL_ 3.2.1 */
/*  06.01.2004 : PR  - Add registers for PXP (IDCT_OUT) - HV_HW_REL_ 3.2.1 */
/*  06.01.2004 : PR  - Updated TVO (rename background) - HV_HW_REL_ 3.2.1 */
/*  07.01.2004 : PR  - Add registers for ITC(TVO)  - HV_HW_REL_ 3.2.1 */
/*  15.01.2004 : PR  - Replaced registers for ITC(TVO -> TVD)  - HV_HW_REL_ 3.2.2 */
/*  15.01.2004 : PR  - Add registers for TVO (for CLK)  - HV_HW_REL_ 3.2.2 */
/*  20.01.2004 : PR  - Add registers for PIF (for ACE_EN)  - HV_HW_REL_ 3.2.2 */
/*  28.01.2004 : PR  - (D-3) Updated Release nb/PPP nb of bits for LUT DATA - HV_HW_REL_4.0.0 */
/*  28.01.2004 : PR  - (D-3) Updated Release nb (heavy negotiation) - HV_HW_REL_3.3.0 */
/*  04.02.2004 : PR  - Bugfix on PIF (ACE_RANGE was in PPP) - HV_HW_REL_3.3.1 */
/*  20.02.2004 : PR  - Add reg in PIF (cpack range) - HV_HW_REL_3.3.2 */
/*  10.03.2004 : PR  - Release 3.3.3 only - HV_HW_REL_3.3.3 */
/*  05.04.2004 : PR  - Release 3.3.4 only - HV_HW_REL_3.3.4 */
/*  03.05.2004 : PR  - Release 3.3.5 only - HV_HW_REL_3.3.5 */
/*  13.05.2004 : PR  - Add reg in BDU (CBR) - HV_HW_REL_3.3.6 */
/*  27.05.2004 : PR  - HV_HW_REL_3.3.7 only */
/*  29.07.2004 : PR  - HV_HW_REL_3.4.0 only */
/*  26.10.2004 : EC  - update register for PIF 8810cut2 (CCP2, interlaced,CCIR10b) */
/*                   - & update reg for DMA (PIF interlaced modif) */
/*  29.10.2004 : EC  - Added BDU_CBP_H, replace BDU_CBP by BDU_CBP_L */
/*  10.11.2004 : EC  - modify version register to v3.4.2 */
/*  13.12.2004 : EC  - modify version register to v3.4.3 */
/*  17.01.2005 : EC  - modify version register to v3.4.4 */
/*  29.07.2004 : PR  - Update PXP Register for bypass - HV_HW_REL_4.0.0 */
/*  10.09.2004 : PR  - Update MECC for new registers - HV_HW_REL_4.0.0 */
/*  17.01.2005 : PR  - Update MECC for MBX and MBY registers - HV_HW_REL_4.0.0 */
/*  20.01.2005 : EC  - added CFG_CKDIS register in CKG module to have block disable clk (CFG_CKDIS_L , CFG_CKDIS_H) */
/*  09.02.2005 : EC  - added H4D register description */
/*  21.02.2005 : EC  - added DMA register description + version 4.0.1 */
/*  01.04.2005 : JMP - Base address definition update + regs update for version 4.0.2 */
/*                     + added missing regs: CFG_CGC, XXX_NTD_L/H, XXX_CTD_L/H, XXX_ITD_L/H, TVD_NTS_L/H, TVD_CTS_L/H */
/*                       TVD_ITS_L/H, TVD_CNT_L/H */
/*  30.05.2005 : PYM - PPP_RASTER, DMA_DISP_SCS2, DMA_DISP_RASTER registers added for version 4.0.4 */
/*  */
/*  11.05.2005 : AB - added H4D register description for DMA. */
/*  13.05.2005 : EB - added MECC register description for SAD_BIAS for version 4.0.4 */
/*  24.05.2005 : EB - added MECC register description for MPEG2 for version 4.0.5 */
/*  30.05.2005 : FC - removed PIF registers, added IPP ones. */
/*  16.06.2005 : FC - added IPP_CRM_xxx registers (HV_HW_REL 4.0.7). */
/*  21.06.2005 : FC - added IPP_CRM_xxx registers (HV_HW_REL 4.0.7). */
/*  29.06.2005 : EC - added BDU_HERR registers (HV_HW_REL 4.0.8). */
/*  05.07.2005 : EC - added IPP_ITCR, IPP_ITIP, IPP_ITOP. */
/*  07.07.2005 : EC -.release version 4.0.9 */
/*  18.07.2005 : EB -.updated pxp mask for registers PA_CMD, DEC_FRAME ENC_FRAME */
/*                    Now 128 addresses in QP_TABLE. */
/*  19.07.2005 : EB -.added PXP_DEC_IQTZ_PARAM_xxx registers. */
/*                    removed PXP_DEC_IQTZ_PARAM register */
/*  20.07.2005 : EB -.extended PXP_REG_ENC_FRAME_CFG mask */
/*  20.07.2005 : EB -.removed last modification */
/*  21.07.2005 : EB - modifications since 18.07.2005 removed for retrocompatibility */
/*               EB - added at the end of the list the new register PXP_DEC_IQTZ_PARAM_MP2 */
/*  28.07.2005 : EC - version = 4.1.1  */
/*  28.07.2005 : EC - version = 4.1.2 MECC 1.18, PPP 1.5 , DMA 1.16 IPP 2.4 */
/*  01.08.2005 : EC - version = 4.1.3 DMA 1.17 */
/*  12.08.2005 : EB - version = 4.1.4 PXP 5.10 modified masks */
/*  18.08.2005 : EB - version = 4.1.5 updated IDN */
/*  18.08.2005 : EC - version = 4.1.6 updated IDN */
/*  24.08.2005 : EC - version = 4.1.7 updated IDN + DFT tst_bistpll */
/*  26.08.2005 : EC - version = 4.1.8 updated IDN + fix resize */
/*  06.09.2005 : EC - version = 4.1.9 updated IDN + fix resize + fix  DMA + fix CKG */
/*  06.09.2005 : EC - version = 4.2.0 updated IDN + add h264 cavlc registers */
/*  06.09.2005 : EC - version = 4.2.0 updated IDN + add h264 cavlc registers */
/*  30.11.2005 : FC - added IPP_IFP_DCE_xx for downsize bugfix */
/*  09.12.2005 : FC - added IPP_BML_LINE_TIME_xx. */
/*  23.12.2005 : FC - modified IPP_CCP_TOP_IF and IPP_ITIP_H width. */
/*  05.01.2006 : FC - retrieved previous IPP_CCP_TOP_IF width. */
/*  19.01.2006 : EC - added register for total zero table. */
/*  27.01.2006 : FC - increased width for IPP_BML_WW */
/*  30.01.2006 : EC - version = 4.2.1 new PPP registers integrated */
/*  02.02.2006 : EC - added register in BDU to ease FW  */
/*  20.02.2006 : EC - version = 4.2.3  */
/*  21.02.2006 : EB - MECC_REG_RR and MECC_REG_IC added for range reduction and */
/*                    intensity compensation respectively */
/*  27.02.2006 : EB - MECC_REG_IC split into MECC_REG_IC, MECC_REG_LUMSCALE and */
/*                    MECC_REG_ISHIFT */
/*  15.03.2006 : EC - version = 4.2.4  */
/*  22.03.2006 : EC - version = 4.2.5 + added register in IPP (IT GS STATUS REG + AHB_DETECTION WINDOW REG)  */
/*  28.03.2006 : EB - version = 4.2.5 MECC IC and RR registers renamed */
/*  28.03.2006 : EB - version = 4.2.5 MECC ISCALE-> LUMSCALE register renamed */
/*  04.04.2006 : EC - version = 4.2.6 + added BDU_H264_RUN_BEF_ZL16 + BDU_H264_CT_NC16 */
/*  13.04.2006 : EC - version = 4.2.7  */
/*  20.04.2006 : EC - version = 4.2.8  */
/*  21.04.2006 : EB - version = 4.2.9 MECC_REG_STABX & MECC_REG_STABY added */
/*  26.04.2006 : EC - version = 4.2.10  */
/*  26.04.2006 : EC - version = 4.2.11 XOR tree added in MECC for testability  */
/*  26.04.2006 : EC - version = 4.2.12 BDU fix  */
/*  16.05.2006 : EC - version = 4.3.0 BDU fix + last MECC */
/*  07.06.2006 : JMP- IPA registers added in dedicated branch to start 8820 IPA verif  */
/*  08.06.2006 : JMP- version = 4.3.1 DMA fix */
/*  08.06.2006 : JMP- version = 4.3.2 IPP fix of soft reset */
/*  20.06.2006 : EC - version = 5.0.0 first 8820 reg list (stop added in BDU) */
/*  04.07.2006 : EB - version = 5.0.1 DFI added */
/*  02.08.2006 : JMP- IPA registers now all between addr E000 and E0FF */
/*  07.08.2006 : EB - improvements for BASE generation for .h and .e */
/*  08.08.2006 : EB - H264IT registers added */
/*  23.08.2006 : EB - PA, REC, DCT, IDCT, SCN registers added */
/*  06.09.2006 : JMP- IPA reg_store_res size increased from 1 bit to 2 bits */
/*  06.09.2006 : EC - CAVLC reg addition */
/*  07.09.2006 : EB - PXP UNITs lacking registers added */
/*  08.09.2006 : JCC- registers MTF modified and added */
/*  14.09.2006 : EC - all clock enable / mmio mapping */
/*  25.09.2006 : EB - version = 5.0.5 BDU_BASE moved */
/*                    corrections on units addresses */
/*  26.09.2006 : EB - version = 5.0.5 BDU_BASE back to its previous value */
/*                    DFI_<UNIT>_ID shifted : >> 1 */
/*  10.10.2006 : EB - version = 5.0.5 MECC registers added */
/*  18.10.2006 : EB - version = 5.0.6 + DFI_REG_MACRO_DEF + DFI_REG_MACRO_DEF */
/*  23.10.2006 : JCC- modif register for deblocking */
/*  24.10.2006 : EB - VC1 IT registers added */
/*  30.10.2006 : EB - version = 5.0.7 SCN_FIFO_CMD corrected */
/*  13.11.2006 : EC - remapped BPU for Aziz */
/*  14.11.2006 : EB - Remved null width registers */
/*  20.11.2006 : EC - version 5.0.8 */
/*  04.12.2006 : EC - BPU reg update */
/*  14.12.2006 : EB - FIFO reg added */
/*  15.12.2006 : JCC- Modif reg deb Source&Frame height */
/*  18.12.2006 : EB - MECC reg update */
/*  20.12.2006 : JCC- DMA REC Inter Field removed */
/*  02.01.2007 : EC - 5.1.2 release  + added clk_dma+ahb_enable */
/*  05.01.2007 : EB - DFI_REG_MACRO_CFG added */
/*  05.01.2007 : EB - extra units: VBF and FIFO2 */
/*  08.01.2007 : EC - 5.1.3 release */
/*  11.01.2007 : CM - Add mask, value, access, type, xiordy for each reg */
/*  15.01.2007 : EC - 5.1.4 release */
/*  17.01.2007 : EC - 5.1.5 release */
/*  18.01.2007 : JCC- Correction mask DMA */
/*  26.01.2007 : EC - 5.1.6 release */
/*  02.02.2007 : EC - 5.1.7 release */
/*  07.02.2007 : EC - 5.1.8 release */
/*  08.02.2007 : EC - 5.1.9 release */
/*  15.02.2007 : EC - 5.2.0 release */
/*  26.02.2007 : EC - 5.2.1 release  2 bit added in CKG_CKEN2 reg for vbf and fifo2 */
/*  02.03.2007 : EC - 5.2.2 release */
/*  02.03.2007 : EC - 5.2.3 release */
/*  06.03.2007 : EC - 5.2.4 release removed itip itop register */
/*  08.03.2007 : JCC- 5.2.5 release */
/*  19.03.2007 : EC- 5.2.6  release modified BPU FLU1, FLU2, FLU3 address */
/*  23.03.2007 : JCC- 5.2.7 release */
/*  04.04.2007 : JCC- 5.2.8 release */
/*  16.04.2007 : JMP- IPA registers udpated to match with IPA spec 1.3 */
/*  16.04.2007 : EC 5.2.9 release */
/*  18.04.2007 : EC 5.3.0 release */
/*  20.04.2007 : JCC 5.3.1 release */
/*  26.04.2007 : JCC 5.3.3 release */
/*  30.04.2007 : JCC 5.3.4 release */
/*  30.04.2007 : JCC 5.3.5 release */
/*  04.05.2007 : JCC 5.3.6 release */
/*  11.05.2007 : JCC 5.3.7 release */
/*  14.05.2007 : JCC 5.3.8 release */
/*  15.05.2007 : JCC 5.3.9 release */
/*  15.05.2007 : JCC 5.4.0 release */
/*  23.05.2007 : JCC 5.4.1 release */
/*  28.05.2007 : JCC 5.4.2 release */
/*  08.06.2007 : JCC 5.4.3 release equal to 5.4.2 */
/*  27.07.2007 : JCC 5.5.0 release first for cutB */
/*  30.07.2007 : JPA correction of BDU registers */
/*  01.08.2007 : EC BDU_CAVLC_START addition */
/*  07.08.2007 : 5.5.1 release */
/*  10.09.2007 : 5.5.2 release */
/* ---------------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/*  Base address definition */
/* -------------------------------------------------------------------- */
#define CFG_BASE             0x0000U
#define CKG_BASE             0xC200U
#define DMA_BASE             0xC300U
#define MTF_BASE             0xC400U
#define VPP_BASE             0xC500U
#define DFI_BASE             0xC600U
#define BDU_BASE             0xC700U
#define BPU_BASE             0xC800U
#define MECC_BASE            0xC900U
#define IPA_BASE             0xCA00U
#define H264VLC_BASE         0xCB00U
#define H264Q_BASE           0xCB40U
#define H264IQ_BASE          0xCB80U
#define H264T_BASE           0xCBC0U
#define H264IT_BASE          0xCC00U
#define DCT_BASE             0xCC40U
#define IDCT_BASE            0xCC80U
#define Q_BASE               0xCCC0U
#define IQ_BASE              0xCD00U
#define SCN_BASE             0xCD40U
#define RLC_BASE             0xCD80U
#define IPRD_BASE            0xCDC0U
#define PA_BASE              0xCE00U
#define REC_BASE             0xCE40U
#define FIFO_BASE            0xCE80U
#define QTAB_BASE            0xCEC0U
#define VC1IT_BASE           0xCF00U
#define FIFO2_BASE           0xCF80U
#define VBF_BASE             0xD000U
#define VLC_BASE             0xD400U
#define ITC_BASE             0xDE00U
#define DFI_BDU_ID           0x0008U
#define DFI_BPU_ID           0x0010U
#define DFI_MECC_ID          0x0018U
#define DFI_IPA_ID           0x0020U
#define DFI_DCT_ID           0x0028U
#define DFI_IDCT_ID          0x0030U
#define DFI_H264T_ID         0x0038U
#define DFI_H264IT_ID        0x0040U
#define DFI_Q_ID             0x0048U
#define DFI_IQ_ID            0x0050U
#define DFI_H264Q_ID         0x0058U
#define DFI_H264IQ_ID        0x0060U
#define DFI_SCN_ID           0x0068U
#define DFI_RLC_ID           0x0070U
#define DFI_H264RLC_ID       0x0078U
#define DFI_IPRD_ID          0x0080U
#define DFI_PA_ID            0x0088U
#define DFI_REC_ID           0x0090U
#define DFI_FIFO_ID          0x0098U
#define DFI_QTAB_ID          0x00A0U
#define DFI_VC1IT_ID         0x00A8U
#define DFI_VBF_ID           0x00B0U
#define DFI_FIFO2_ID         0x00B8U
#define DFI_VLC_ID           0x00C0U
/* -------------------------------------------------------------------- */
/*  Hardware identification version (must match with VisualElite / DMDGui release datebase id !!!) */
/* -------------------------------------------------------------------- */
#define HV_HW_REL            0x0552U
/* -------------------------------------------------------------------- */
/*  Software registers (from 0x0000U to 0x01FFU, mmdsp data memory area) */
/* -------------------------------------------------------------------- */

#ifndef T1XHV_HOST

#define CFG_PSA              0x0000U
#define CFG_PEA              0x0002U
#define CFG_ICE              0x0004U
#define CFG_CSC_L            0x0006U
#define CFG_CSC_H            0x0007U
#define CFG_CGC              0x0008U
#define VEC_NAD_L            0x0080U
#define VEC_NAD_H            0x0081U
#define VEC_NTY_L            0x0082U
#define VEC_NTY_H            0x0083U
#define VEC_NTS_L            0x0084U
#define VEC_NTS_H            0x0085U
#define VEC_NTD_L            0x0086U
#define VEC_NTD_H            0x0087U
#define VEC_CAD_L            0x0088U
#define VEC_CAD_H            0x0089U
#define VEC_CTY_L            0x008AU
#define VEC_CTY_H            0x008BU
#define VEC_CTS_L            0x008CU
#define VEC_CTS_H            0x008DU
#define VEC_CTD_L            0x008EU
#define VEC_CTD_H            0x008FU
#define VEC_IAD_L            0x0090U
#define VEC_IAD_H            0x0091U
#define VEC_ITY_L            0x0092U
#define VEC_ITY_H            0x0093U
#define VEC_ITS_L            0x0094U
#define VEC_ITS_H            0x0095U
#define VEC_ITD_L            0x0096U
#define VEC_ITD_H            0x0097U
#define VEC_CNT_L            0x0098U
#define VEC_CNT_H            0x0099U
#define VDC_NAD_L            0x00C0U
#define VDC_NAD_H            0x00C1U
#define VDC_NTY_L            0x00C2U
#define VDC_NTY_H            0x00C3U
#define VDC_NTS_L            0x00C4U
#define VDC_NTS_H            0x00C5U
#define VDC_NTD_L            0x00C6U
#define VDC_NTD_H            0x00C7U
#define VDC_CAD_L            0x00C8U
#define VDC_CAD_H            0x00C9U
#define VDC_CTY_L            0x00CAU
#define VDC_CTY_H            0x00CBU
#define VDC_CTS_L            0x00CCU
#define VDC_CTS_H            0x00CDU
#define VDC_CTD_L            0x00CEU
#define VDC_CTD_H            0x00CFU
#define VDC_IAD_L            0x00D0U
#define VDC_IAD_H            0x00D1U
#define VDC_ITY_L            0x00D2U
#define VDC_ITY_H            0x00D3U
#define VDC_ITS_L            0x00D4U
#define VDC_ITS_H            0x00D5U
#define VDC_ITD_L            0x00D6U
#define VDC_ITD_H            0x00D7U
#define VDC_CNT_L            0x00D8U
#define VDC_CNT_H            0x00D9U
#define GRB_NAD_L            0x0100U
#define GRB_NAD_H            0x0101U
#define GRB_NTY_L            0x0102U
#define GRB_NTY_H            0x0103U
#define GRB_NTS_L            0x0104U
#define GRB_NTS_H            0x0105U
#define GRB_NTD_L            0x0106U
#define GRB_NTD_H            0x0107U
#define GRB_CAD_L            0x0108U
#define GRB_CAD_H            0x0109U
#define GRB_CTY_L            0x010AU
#define GRB_CTY_H            0x010BU
#define GRB_CTS_L            0x010CU
#define GRB_CTS_H            0x010DU
#define GRB_CTD_L            0x010EU
#define GRB_CTD_H            0x010FU
#define GRB_IAD_L            0x0110U
#define GRB_IAD_H            0x0111U
#define GRB_ITY_L            0x0112U
#define GRB_ITY_H            0x0113U
#define GRB_ITS_L            0x0114U
#define GRB_ITS_H            0x0115U
#define GRB_ITD_L            0x0116U
#define GRB_ITD_H            0x0117U
#define GRB_CNT_L            0x0118U
#define GRB_CNT_H            0x0119U
#define DPL_NAD_L            0x0140U
#define DPL_NAD_H            0x0141U
#define DPL_NTY_L            0x0142U
#define DPL_NTY_H            0x0143U
#define DPL_NTS_L            0x0144U
#define DPL_NTS_H            0x0145U
#define DPL_NTD_L            0x0146U
#define DPL_NTD_H            0x0147U
#define DPL_CAD_L            0x0148U
#define DPL_CAD_H            0x0149U
#define DPL_CTY_L            0x014AU
#define DPL_CTY_H            0x014BU
#define DPL_CTS_L            0x014CU
#define DPL_CTS_H            0x014DU
#define DPL_CTD_L            0x014EU
#define DPL_CTD_H            0x014FU
#define DPL_IAD_L            0x0150U
#define DPL_IAD_H            0x0151U
#define DPL_ITY_L            0x0152U
#define DPL_ITY_H            0x0153U
#define DPL_ITS_L            0x0154U
#define DPL_ITS_H            0x0155U
#define DPL_ITD_L            0x0156U
#define DPL_ITD_H            0x0157U
#define DPL_CNT_L            0x0158U
#define DPL_CNT_H            0x0159U
#define TVD_NAD_L            0x0180U
#define TVD_NAD_H            0x0181U
#define TVD_NTY_L            0x0182U
#define TVD_NTY_H            0x0183U
#define TVD_NTS_L            0x0184U
#define TVD_NTS_H            0x0185U
#define TVD_NTD_L            0x0186U
#define TVD_NTD_H            0x0187U
#define TVD_CAD_L            0x0188U
#define TVD_CAD_H            0x0189U
#define TVD_CTY_L            0x018AU
#define TVD_CTY_H            0x018BU
#define TVD_CTS_L            0x018CU
#define TVD_CTS_H            0x018DU
#define TVD_CTD_L            0x018EU
#define TVD_CTD_H            0x018FU
#define TVD_IAD_L            0x0190U
#define TVD_IAD_H            0x0191U
#define TVD_ITY_L            0x0192U
#define TVD_ITY_H            0x0193U
#define TVD_ITS_L            0x0194U
#define TVD_ITS_H            0x0195U
#define TVD_ITD_L            0x0196U
#define TVD_ITD_H            0x0197U
#define TVD_CNT_L            0x0198U
#define TVD_CNT_H            0x0199U
#define IDN_FRV_L            0x01C0U
#define IDN_FRV_H            0x01C1U
#define IDN_RRV              0x01C2U
#define IDN_HRV              0x00E0U

#endif

/* -------------------------------------------------------------------- */
/*  Motion Estimation sub-addresses */
/* -------------------------------------------------------------------- */
#define MECC_REG_SOFT_RESET  0x0000U
#define MECC_REG_ENR         0x0001U
#define MECC_REG_DIR         0x0002U
#define MECC_REG_STA         0x0003U
#define MECC_REG_ISR         0x0004U
#define MECC_REG_ME_ORDER    0x0005U
#define MECC_REG_FILTER_MODE 0x0006U
#define MECC_REG_RND         0x0007U
#define MECC_REG_IMAGE_WIDTH 0x0008U
#define MECC_REG_BIAS        0x0009U
#define MECC_REG_BYPASS_CACHE 0x000AU
#define MECC_REG_RANGE_REDUCTION 0x000BU
#define MECC_REG_INTENSITY_COMP 0x000CU
#define MECC_REG_IC_FWD      0x000DU
#define MECC_REG_IC_BWD      0x000EU
#define MECC_REG_DEBUG_CACHE 0x000FU
#define MECC_FIFO_CMD        0x0010U
#define MECC_FIFO_MPX        0x0011U
#define MECC_FIFO_MPY        0x0012U
#define MECC_FIFO_REF_ID     0x0013U
#define MECC_FIFO_SIZE       0x0014U
#define MECC_FIFO_DST_BUF    0x0015U
#define MECC_FIFO_SAD_BIAS   0x0016U
#define MECC_REG_MEAN        0x0020U
#define MECC_REG_MAD         0x0021U
#define MECC_REG_SAD         0x0022U
#define MECC_REG_SAD_BIAS    0x0023U
#define MECC_REG_MPX         0x0024U
#define MECC_REG_MPY         0x0025U
#define MECC_REG_REF_ID      0x0026U
#define MECC_REG_SAD_DELAYED 0x0027U
#define MECC_REG_SAD_BIAS_DELAYED 0x0028U
#define MECC_REG_MPX_DELAYED 0x0029U
#define MECC_REG_MPY_DELAYED 0x002AU
#define MECC_REG_REF_ID_DELAYED 0x002BU
#define MECC_REG_LAST_SAD    0x002CU
#define MECC_REG_MBX         0x0030U
#define MECC_REG_MBY         0x0031U
#define MECC_REG_STABX       0x0032U
#define MECC_REG_STABY       0x0033U
/* -------------------------------------------------------------------- */
/*  DFI (Data Flow Interconnect) sub-addresses */
/* -------------------------------------------------------------------- */
#define DFI_REG_SOFT_RESET   0x0000U
#define DFI_REG_MACRO_DEF    0x0001U
#define DFI_REG_MACRO_CFG    0x0002U
#define DFI_REG_MACRO_CMD    0x0003U
#define DFI_FIFO_MACRO_CMD   0x0004U
/*  For each Unit: */
#define DFI_REG_DEF          0x0000U
#define DFI_REG_CMD          0x0001U
#define DFI_FIFO_CMD         0x0002U
#define DFI_FIFO_SRC_LSB     0x0003U
#define DFI_FIFO_SRC_MSB     0x0004U
#define DFI_FIFO_DST_LSB     0x0005U
#define DFI_FIFO_DST_MSB     0x0006U
#define DFI_REG_STA_LAST     0x0007U
/*  BDU */
#define DFI_REG_BDU_DEF      0x0008U
#define DFI_REG_BDU_CMD      0x0009U
#define DFI_FIFO_BDU_CMD     0x000AU
#define DFI_FIFO_BDU_SRC_LSB 0x000BU
#define DFI_FIFO_BDU_SRC_MSB 0x000CU
#define DFI_FIFO_BDU_DST_LSB 0x000DU
#define DFI_FIFO_BDU_DST_MSB 0x000EU
#define DFI_REG_BDU_STA_LAST 0x000FU
/*  BPU */
#define DFI_REG_BPU_DEF      0x0010U
#define DFI_REG_BPU_CMD      0x0011U
#define DFI_FIFO_BPU_CMD     0x0012U
#define DFI_FIFO_BPU_SRC_LSB 0x0013U
#define DFI_FIFO_BPU_SRC_MSB 0x0014U
#define DFI_FIFO_BPU_DST_LSB 0x0015U
#define DFI_FIFO_BPU_DST_MSB 0x0016U
#define DFI_REG_BPU_STA_LAST 0x0017U
/*  MECC */
#define DFI_REG_MECC_DEF     0x0018U
#define DFI_REG_MECC_CMD     0x0019U
#define DFI_FIFO_MECC_CMD    0x001AU
#define DFI_FIFO_MECC_SRC_LSB 0x001BU
#define DFI_FIFO_MECC_SRC_MSB 0x001CU
#define DFI_FIFO_MECC_DST_LSB 0x001DU
#define DFI_FIFO_MECC_DST_MSB 0x001EU
#define DFI_REG_MECC_STA_LAST 0x001FU
/*  IPA */
#define DFI_REG_IPA_DEF      0x0020U
#define DFI_REG_IPA_CMD      0x0021U
#define DFI_FIFO_IPA_CMD     0x0022U
#define DFI_FIFO_IPA_SRC_LSB 0x0023U
#define DFI_FIFO_IPA_SRC_MSB 0x0024U
#define DFI_FIFO_IPA_DST_LSB 0x0025U
#define DFI_FIFO_IPA_DST_MSB 0x0026U
#define DFI_REG_IPA_STA_LAST 0x0027U
/*  DCT */
#define DFI_REG_DCT_DEF      0x0028U
#define DFI_REG_DCT_CMD      0x0029U
#define DFI_FIFO_DCT_CMD     0x002AU
#define DFI_FIFO_DCT_SRC_LSB 0x002BU
#define DFI_FIFO_DCT_SRC_MSB 0x002CU
#define DFI_FIFO_DCT_DST_LSB 0x002DU
#define DFI_FIFO_DCT_DST_MSB 0x002EU
#define DFI_REG_DCT_STA_LAST 0x002FU
/*  IDCT */
#define DFI_REG_IDCT_DEF     0x0030U
#define DFI_REG_IDCT_CMD     0x0031U
#define DFI_FIFO_IDCT_CMD    0x0032U
#define DFI_FIFO_IDCT_SRC_LSB 0x0033U
#define DFI_FIFO_IDCT_SRC_MSB 0x0034U
#define DFI_FIFO_IDCT_DST_LSB 0x0035U
#define DFI_FIFO_IDCT_DST_MSB 0x0036U
#define DFI_REG_IDCT_STA_LAST 0x0037U
/*  H264T */
#define DFI_REG_H264T_DEF    0x0038U
#define DFI_REG_H264T_CMD    0x0039U
#define DFI_FIFO_H264T_CMD   0x003AU
#define DFI_FIFO_H264T_SRC_LSB 0x003BU
#define DFI_FIFO_H264T_SRC_MSB 0x003CU
#define DFI_FIFO_H264T_DST_LSB 0x003DU
#define DFI_FIFO_H264T_DST_MSB 0x003EU
#define DFI_REG_H264T_STA_LAST 0x003FU
/*  H264IT */
#define DFI_REG_H264IT_DEF   0x0040U
#define DFI_REG_H264IT_CMD   0x0041U
#define DFI_FIFO_H264IT_CMD  0x0042U
#define DFI_FIFO_H264IT_SRC_LSB 0x0043U
#define DFI_FIFO_H264IT_SRC_MSB 0x0044U
#define DFI_FIFO_H264IT_DST_LSB 0x0045U
#define DFI_FIFO_H264IT_DST_MSB 0x0046U
#define DFI_REG_H264IT_STA_LAST 0x0047U
/*  Q */
#define DFI_REG_Q_DEF        0x0048U
#define DFI_REG_Q_CMD        0x0049U
#define DFI_FIFO_Q_CMD       0x004AU
#define DFI_FIFO_Q_SRC_LSB   0x004BU
#define DFI_FIFO_Q_SRC_MSB   0x004CU
#define DFI_FIFO_Q_DST_LSB   0x004DU
#define DFI_FIFO_Q_DST_MSB   0x004EU
#define DFI_REG_Q_STA_LAST   0x004FU
/*  IQ */
#define DFI_REG_IQ_DEF       0x0050U
#define DFI_REG_IQ_CMD       0x0051U
#define DFI_FIFO_IQ_CMD      0x0052U
#define DFI_FIFO_IQ_SRC_LSB  0x0053U
#define DFI_FIFO_IQ_SRC_MSB  0x0054U
#define DFI_FIFO_IQ_DST_LSB  0x0055U
#define DFI_FIFO_IQ_DST_MSB  0x0056U
#define DFI_REG_IQ_STA_LAST  0x0057U
/*  H264Q */
#define DFI_REG_H264Q_DEF    0x0058U
#define DFI_REG_H264Q_CMD    0x0059U
#define DFI_FIFO_H264Q_CMD   0x005AU
#define DFI_FIFO_H264Q_SRC_LSB 0x005BU
#define DFI_FIFO_H264Q_SRC_MSB 0x005CU
#define DFI_FIFO_H264Q_DST_LSB 0x005DU
#define DFI_FIFO_H264Q_DST_MSB 0x005EU
#define DFI_REG_H264Q_STA_LAST 0x005FU
/*  H264IQ */
#define DFI_REG_H264IQ_DEF   0x0060U
#define DFI_REG_H264IQ_CMD   0x0061U
#define DFI_FIFO_H264IQ_CMD  0x0062U
#define DFI_FIFO_H264IQ_SRC_LSB 0x0063U
#define DFI_FIFO_H264IQ_SRC_MSB 0x0064U
#define DFI_FIFO_H264IQ_DST_LSB 0x0065U
#define DFI_FIFO_H264IQ_DST_MSB 0x0066U
#define DFI_REG_H264IQ_STA_LAST 0x0067U
/*  SCN */
#define DFI_REG_SCN_DEF      0x0068U
#define DFI_REG_SCN_CMD      0x0069U
#define DFI_FIFO_SCN_CMD     0x006AU
#define DFI_FIFO_SCN_SRC_LSB 0x006BU
#define DFI_FIFO_SCN_SRC_MSB 0x006CU
#define DFI_FIFO_SCN_DST_LSB 0x006DU
#define DFI_FIFO_SCN_DST_MSB 0x006EU
#define DFI_REG_SCN_STA_LAST 0x006FU
/*  RLC */
#define DFI_REG_RLC_DEF      0x0070U
#define DFI_REG_RLC_CMD      0x0071U
#define DFI_FIFO_RLC_CMD     0x0072U
#define DFI_FIFO_RLC_SRC_LSB 0x0073U
#define DFI_FIFO_RLC_SRC_MSB 0x0074U
#define DFI_FIFO_RLC_DST_LSB 0x0075U
#define DFI_FIFO_RLC_DST_MSB 0x0076U
#define DFI_REG_RLC_STA_LAST 0x0077U
/*  H264RLC */
#define DFI_REG_H264RLC_DEF  0x0078U
#define DFI_REG_H264RLC_CMD  0x0079U
#define DFI_FIFO_H264RLC_CMD 0x007AU
#define DFI_FIFO_H264RLC_SRC_LSB 0x007BU
#define DFI_FIFO_H264RLC_SRC_MSB 0x007CU
#define DFI_FIFO_H264RLC_DST_LSB 0x007DU
#define DFI_FIFO_H264RLC_DST_MSB 0x007EU
#define DFI_REG_H264RLC_STA_LAST 0x007FU
/*  IPRD */
#define DFI_REG_IPRD_DEF     0x0080U
#define DFI_REG_IPRD_CMD     0x0081U
#define DFI_FIFO_IPRD_CMD    0x0082U
#define DFI_FIFO_IPRD_SRC_LSB 0x0083U
#define DFI_FIFO_IPRD_SRC_MSB 0x0084U
#define DFI_FIFO_IPRD_DST_LSB 0x0085U
#define DFI_FIFO_IPRD_DST_MSB 0x0086U
#define DFI_REG_IPRD_STA_LAST 0x0087U
/*  PA */
#define DFI_REG_PA_DEF       0x0088U
#define DFI_REG_PA_CMD       0x0089U
#define DFI_FIFO_PA_CMD      0x008AU
#define DFI_FIFO_PA_SRC_LSB  0x008BU
#define DFI_FIFO_PA_SRC_MSB  0x008CU
#define DFI_FIFO_PA_DST_LSB  0x008DU
#define DFI_FIFO_PA_DST_MSB  0x008EU
#define DFI_REG_PA_STA_LAST  0x008FU
/*  REC */
#define DFI_REG_REC_DEF      0x0090U
#define DFI_REG_REC_CMD      0x0091U
#define DFI_FIFO_REC_CMD     0x0092U
#define DFI_FIFO_REC_SRC_LSB 0x0093U
#define DFI_FIFO_REC_SRC_MSB 0x0094U
#define DFI_FIFO_REC_DST_LSB 0x0095U
#define DFI_FIFO_REC_DST_MSB 0x0096U
#define DFI_REG_REC_STA_LAST 0x0097U
/*  FIFO */
#define DFI_REG_FIFO_DEF     0x0098U
#define DFI_REG_FIFO_CMD     0x0099U
#define DFI_FIFO_FIFO_CMD    0x009AU
#define DFI_FIFO_FIFO_SRC_LSB 0x009BU
#define DFI_FIFO_FIFO_SRC_MSB 0x009CU
#define DFI_FIFO_FIFO_DST_LSB 0x009DU
#define DFI_FIFO_FIFO_DST_MSB 0x009EU
#define DFI_REG_FIFO_STA_LAST 0x009FU
/*  QTAB */
#define DFI_REG_QTAB_DEF     0x00A0U
#define DFI_REG_QTAB_CMD     0x00A1U
#define DFI_FIFO_QTAB_CMD    0x00A2U
#define DFI_FIFO_QTAB_SRC_LSB 0x00A3U
#define DFI_FIFO_QTAB_SRC_MSB 0x00A4U
#define DFI_FIFO_QTAB_DST_LSB 0x00A5U
#define DFI_FIFO_QTAB_DST_MSB 0x00A6U
#define DFI_REG_QTAB_STA_LAST 0x00A7U
/*  VC1IT */
#define DFI_REG_VC1IT_DEF    0x00A8U
#define DFI_REG_VC1IT_CMD    0x00A9U
#define DFI_FIFO_VC1IT_CMD   0x00AAU
#define DFI_FIFO_VC1IT_SRC_LSB 0x00ABU
#define DFI_FIFO_VC1IT_SRC_MSB 0x00ACU
#define DFI_FIFO_VC1IT_DST_LSB 0x00ADU
#define DFI_FIFO_VC1IT_DST_MSB 0x00AEU
#define DFI_REG_VC1IT_STA_LAST 0x00AFU
/*  VBF */
#define DFI_REG_VBF_DEF      0x00B0U
#define DFI_REG_VBF_CMD      0x00B1U
#define DFI_FIFO_VBF_CMD     0x00B2U
#define DFI_FIFO_VBF_SRC_LSB 0x00B3U
#define DFI_FIFO_VBF_SRC_MSB 0x00B4U
#define DFI_FIFO_VBF_DST_LSB 0x00B5U
#define DFI_FIFO_VBF_DST_MSB 0x00B6U
#define DFI_REG_VBF_STA_LAST 0x00B7U
/*  FIFO2 */
#define DFI_REG_FIFO2_DEF    0x00B8U
#define DFI_REG_FIFO2_CMD    0x00B9U
#define DFI_FIFO_FIFO2_CMD   0x00BAU
#define DFI_FIFO_FIFO2_SRC_LSB 0x00BBU
#define DFI_FIFO_FIFO2_SRC_MSB 0x00BCU
#define DFI_FIFO_FIFO2_DST_LSB 0x00BDU
#define DFI_FIFO_FIFO2_DST_MSB 0x00BEU
#define DFI_REG_FIFO2_STA_LAST 0x00BFU
/*  VLC */
#define DFI_REG_VLC_DEF      0x00C0U
#define DFI_REG_VLC_CMD      0x00C1U
#define DFI_FIFO_VLC_CMD     0x00C2U
#define DFI_FIFO_VLC_SRC_LSB 0x00C3U
#define DFI_FIFO_VLC_SRC_MSB 0x00C4U
#define DFI_FIFO_VLC_DST_LSB 0x00C5U
#define DFI_FIFO_VLC_DST_MSB 0x00C6U
#define DFI_REG_VLC_STA_LAST 0x00C7U
/* -------------------------------------------------------------------- */
/*  DMA sub-addresses */
/* -------------------------------------------------------------------- */
#define DMA_ENR              0x0000U
#define DMA_DIR              0x0001U
#define DMA_IMR              0x0002U
#define DMA_ISR              0x0003U
#define DMA_BSM              0x0004U
#define DMA_SOFT_RST         0x0005U
#define DMA_MTF_R_TFL        0x0010U
#define DMA_MTF_R_INCR       0x0011U
#define DMA_MTF_R_BSA_L      0x0012U
#define DMA_MTF_R_BSA_H      0x0013U
#define DMA_MTF_W_TFL        0x0020U
#define DMA_MTF_W_INCR       0x0021U
#define DMA_MTF_W_BSA_L      0x0022U
#define DMA_MTF_W_BSA_H      0x0023U
#define DMA_CDR_BWS_L        0x0030U
#define DMA_CDR_BWS_H        0x0031U
#define DMA_CDR_BWE_L        0x0032U
#define DMA_CDR_BWE_H        0x0033U
#define DMA_CDW_BWS_L        0x0040U
#define DMA_CDW_BWS_H        0x0041U
#define DMA_CDW_BWE_L        0x0042U
#define DMA_CDW_BWE_H        0x0043U
#define DMA_REC_DLS_L        0x0050U
#define DMA_REC_DLS_H        0x0051U
#define DMA_REC_DCS_L        0x0052U
#define DMA_REC_DCS_H        0x0053U
#define DMA_REC_DCS2_L       0x0054U
#define DMA_REC_DCS2_H       0x0055U
#define DMA_REC_DFW          0x0056U
#define DMA_REC_DFH          0x0057U
#define DMA_REC_DMC          0x0058U
#define DMA_REC_MODE         0x0059U
#define DMA_SWF_DLS_L        0x005BU
#define DMA_SWF_DLS_H        0x005CU
#define DMA_SWF_DFH          0x005DU
#define DMA_REF0_FLS_L       0x0060U
#define DMA_REF0_FLS_H       0x0061U
#define DMA_REF0_FCS_L       0x0062U
#define DMA_REF0_FCS_H       0x0063U
#define DMA_REF1_FLS_L       0x0064U
#define DMA_REF1_FLS_H       0x0065U
#define DMA_REF1_FCS_L       0x0066U
#define DMA_REF1_FCS_H       0x0067U
#define DMA_REF2_FLS_L       0x0068U
#define DMA_REF2_FLS_H       0x0069U
#define DMA_REF2_FCS_L       0x006AU
#define DMA_REF2_FCS_H       0x006BU
#define DMA_REF3_FLS_L       0x006CU
#define DMA_REF3_FLS_H       0x006DU
#define DMA_REF3_FCS_L       0x006EU
#define DMA_REF3_FCS_H       0x006FU
#define DMA_REF4_FLS_L       0x0070U
#define DMA_REF4_FLS_H       0x0071U
#define DMA_REF4_FCS_L       0x0072U
#define DMA_REF4_FCS_H       0x0073U
#define DMA_REF0_FCS2_L      0x0074U
#define DMA_REF0_FCS2_H      0x0075U
#define DMA_REF1_FCS2_L      0x0076U
#define DMA_REF1_FCS2_H      0x0077U
#define DMA_REF2_FCS2_L      0x0078U
#define DMA_REF2_FCS2_H      0x0079U
#define DMA_REF3_FCS2_L      0x007AU
#define DMA_REF3_FCS2_H      0x007BU
#define DMA_REF4_FCS2_L      0x007CU
#define DMA_REF4_FCS2_H      0x007DU
#define DMA_MECC_ORG_SFW     0x007EU
#define DMA_MECC_ORG_SFH     0x007FU
#define DMA_MECC_REF_SFW     0x0080U
#define DMA_MECC_REF_SFH     0x0081U
#define DMA_MECC_MZY0        0x0082U
#define DMA_MECC_MODE        0x0083U
#define DMA_DEB_SLS_L        0x0084U
#define DMA_DEB_SLS_H        0x0085U
#define DMA_DEB_SCS_L        0x0086U
#define DMA_DEB_SCS_H        0x0087U
#define DMA_DEB_SCS2_L       0x0088U
#define DMA_DEB_SCS2_H       0x0089U
#define DMA_DEB_SWH          0x008AU
#define DMA_DEB_SFW          0x008BU
#define DMA_DEB_RASTER_IN    0x008CU
#define DMA_DEB_RASTER_OUT   0x008DU
#define DMA_DEB_DLS_L        0x008EU
#define DMA_DEB_DLS_H        0x008FU
#define DMA_DEB_DCS_L        0x0090U
#define DMA_DEB_DCS_H        0x0091U
#define DMA_DEB_DCS2_L       0x0092U
#define DMA_DEB_DCS2_H       0x0093U
#define DMA_DEB_DWH          0x0094U
#define DMA_DEB_DFW          0x0095U
#define DMA_DEB_PS_L         0x0096U
#define DMA_DEB_PS_H         0x0097U
#define DMA_DEB_PE_L         0x0098U
#define DMA_DEB_PE_H         0x0099U
#define DMA_DEB_DUMMYS_L     0x009AU
#define DMA_DEB_DUMMYS_H     0x009BU
#define DMA_DEB_DUMMYE_L     0x009CU
#define DMA_DEB_DUMMYE_H     0x009DU
#define DMA_MAX_OPC_SZ1      0x00A0U
#define DMA_MAX_OPC_SZ2      0x00A1U
#define DMA_MAX_OPC_SZ3      0x00A2U
#define DMA_MAX_OPC_SZ4      0x00A3U
#define DMA_MAX_OPC_SZ5      0x00A4U
#define DMA_MAX_OPC_SZ6      0x00A5U
#define DMA_MAX_OPC_SZ7      0x00A6U
#define DMA_MAX_OPC_SZ8      0x00A7U
#define DMA_MAX_OPC_SZ9      0x00A8U
#define DMA_MAX_OPC_SZ10     0x00A9U
#define DMA_MAX_OPC_SZ11     0x00AAU
#define DMA_MAX_OPC_SZ12     0x00ABU
#define DMA_MAX_OPC_SZ13     0x00ACU
#define DMA_PRIO_1           0x00B0U
#define DMA_PRIO_2           0x00B1U
#define DMA_PRIO_3           0x00B2U
#define DMA_PRIO_4           0x00B3U
#define DMA_PRIO_5           0x00B4U
#define DMA_PRIO_6           0x00B5U
#define DMA_PRIO_7           0x00B6U
#define DMA_PRIO_8           0x00B7U
#define DMA_PRIO_9           0x00B8U
#define DMA_PRIO_10          0x00B9U
#define DMA_PRIO_11          0x00BAU
#define DMA_PRIO_12          0x00BBU
#define DMA_PRIO_13          0x00BCU
#define DMA_MAX_MSSG_SZ1     0x00C0U
#define DMA_MAX_MSSG_SZ2     0x00C1U
#define DMA_MAX_MSSG_SZ3     0x00C2U
#define DMA_MAX_MSSG_SZ4     0x00C3U
#define DMA_MAX_MSSG_SZ5     0x00C4U
#define DMA_MAX_MSSG_SZ6     0x00C5U
#define DMA_MAX_MSSG_SZ7     0x00C6U
#define DMA_MAX_MSSG_SZ8     0x00C7U
#define DMA_MAX_MSSG_SZ9     0x00C8U
#define DMA_MAX_MSSG_SZ10    0x00C9U
#define DMA_MAX_MSSG_SZ11    0x00CAU
#define DMA_MAX_MSSG_SZ12    0x00CBU
#define DMA_MAX_MSSG_SZ13    0x00CCU
#define DMA_MAX_CHCK_SZ1     0x00D0U
#define DMA_MAX_CHCK_SZ2     0x00D1U
#define DMA_MAX_CHCK_SZ3     0x00D2U
#define DMA_MAX_CHCK_SZ4     0x00D3U
#define DMA_MAX_CHCK_SZ5     0x00D4U
#define DMA_MAX_CHCK_SZ6     0x00D5U
#define DMA_MAX_CHCK_SZ7     0x00D6U
#define DMA_MAX_CHCK_SZ8     0x00D7U
#define DMA_MAX_CHCK_SZ9     0x00D8U
#define DMA_MAX_CHCK_SZ10    0x00D9U
#define DMA_MAX_CHCK_SZ11    0x00DAU
#define DMA_MAX_CHCK_SZ12    0x00DBU
#define DMA_MAX_CHCK_SZ13    0x00DCU
#define DMA_MAX_ATTR         0x00DDU
#define DMA_MAX_PRI          0x00DEU
#define DMA_MIN_OPC_SZ       0x00E0U
#define DMA_PAGE_SZ          0x00E1U
/* -------------------------------------------------------------------- */
/*  Bit Stream De-Packer Unit sub-addresses */
/* -------------------------------------------------------------------- */
#define BDU_RST              0x0000U
#define BDU_CFR              0x0001U
#define BDU_ENA              0x0002U
#define BDU_STOP             0x0003U
#define BDU_CBP_L            0x000AU
#define BDU_CBP_H            0x0009U
#define BDU_SFT              0x000BU
#define BDU_GTB1             0x0011U
#define BDU_GTB2             0x0012U
#define BDU_GTB3             0x0013U
#define BDU_GTB4             0x0014U
#define BDU_GTB5             0x0015U
#define BDU_GTB6             0x0016U
#define BDU_GTB7             0x0017U
#define BDU_GTB8             0x0018U
#define BDU_GTB9             0x0019U
#define BDU_GTB10            0x001AU
#define BDU_GTB11            0x001BU
#define BDU_GTB12            0x001CU
#define BDU_GTB13            0x001DU
#define BDU_GTB14            0x001EU
#define BDU_GTB15            0x001FU
#define BDU_GTB16            0x0020U
#define BDU_SHB1             0x0031U
#define BDU_SHB2             0x0032U
#define BDU_SHB3             0x0033U
#define BDU_SHB4             0x0034U
#define BDU_SHB5             0x0035U
#define BDU_SHB6             0x0036U
#define BDU_SHB7             0x0037U
#define BDU_SHB8             0x0038U
#define BDU_SHB9             0x0039U
#define BDU_SHB10            0x003AU
#define BDU_SHB11            0x003BU
#define BDU_SHB12            0x003CU
#define BDU_SHB13            0x003DU
#define BDU_SHB14            0x003EU
#define BDU_SHB15            0x003FU
#define BDU_SHB16            0x0040U
#define BDU_SHB17            0x0041U
#define BDU_SHB18            0x0042U
#define BDU_SHB19            0x0043U
#define BDU_SHB20            0x0044U
#define BDU_SHB21            0x0045U
#define BDU_SHB22            0x0046U
#define BDU_SHB23            0x0047U
#define BDU_SHB24            0x0048U
#define BDU_SHB25            0x0049U
#define BDU_SHB26            0x004AU
#define BDU_SHB27            0x004BU
#define BDU_SHB28            0x004CU
#define BDU_SHB29            0x004DU
#define BDU_SHB30            0x004EU
#define BDU_SHB31            0x004FU
#define BDU_SHB32            0x0050U
#define BDU_SCS              0x0051U
#define BDU_INTER            0x0052U
#define BDU_INTRA            0x0053U
#define BDU_INTRA64          0x0054U
#define BDU_PXP0             0x0056U
#define BDU_PXP1             0x0057U
#define BDU_REV              0x0059U
#define BDU_FLU              0x005AU
#define BDU_RINTER           0x005CU
#define BDU_RINTRA           0x005DU
#define BDU_RINTRA64         0x005EU
#define BDU_CBR              0x0060U
#define BDU_HERR             0x0061U
#define BDU_H264_PREFIX      0x0062U
#define BDU_H264_CT_NCM1     0x006FU
#define BDU_H264_CT_NC0      0x0070U
#define BDU_H264_CT_NC1      0x0071U
#define BDU_H264_CT_NC2      0x0072U
#define BDU_H264_CT_NC3      0x0073U
#define BDU_H264_CT_NC4      0x0074U
#define BDU_H264_CT_NC5      0x0075U
#define BDU_H264_CT_NC6      0x0076U
#define BDU_H264_CT_NC7      0x0077U
#define BDU_H264_CT_NC8      0x0078U
#define BDU_H264_CT_NC9      0x0079U
#define BDU_H264_CT_NC10     0x007AU
#define BDU_H264_CT_NC11     0x007BU
#define BDU_H264_CT_NC12     0x007CU
#define BDU_H264_CT_NC13     0x007DU
#define BDU_H264_CT_NC14     0x007EU
#define BDU_H264_CT_NC15     0x007FU
#define BDU_H264_CT_NC16     0x0080U
#define BDU_H264_TZ_TC1      0x0081U
#define BDU_H264_TZ_TC2      0x0082U
#define BDU_H264_TZ_TC3      0x0083U
#define BDU_H264_TZ_TC4      0x0084U
#define BDU_H264_TZ_TC5      0x0085U
#define BDU_H264_TZ_TC6      0x0086U
#define BDU_H264_TZ_TC7      0x0087U
#define BDU_H264_TZ_TC8      0x0088U
#define BDU_H264_TZ_TC9      0x0089U
#define BDU_H264_TZ_TC10     0x008AU
#define BDU_H264_TZ_TC11     0x008BU
#define BDU_H264_TZ_TC12     0x008CU
#define BDU_H264_TZ_TC13     0x008DU
#define BDU_H264_TZ_TC14     0x008EU
#define BDU_H264_TZ_TC15     0x008FU
#define BDU_H264_TZC_TC1     0x0091U
#define BDU_H264_TZC_TC2     0x0092U
#define BDU_H264_TZC_TC3     0x0093U
#define BDU_H264_RUN_BEF_ZL0 0x00A0U
#define BDU_H264_RUN_BEF_ZL1 0x00A1U
#define BDU_H264_RUN_BEF_ZL2 0x00A2U
#define BDU_H264_RUN_BEF_ZL3 0x00A3U
#define BDU_H264_RUN_BEF_ZL4 0x00A4U
#define BDU_H264_RUN_BEF_ZL5 0x00A5U
#define BDU_H264_RUN_BEF_ZL6 0x00A6U
#define BDU_H264_RUN_BEF_ZL7 0x00A7U
#define BDU_H264_RUN_BEF_ZL8 0x00A8U
#define BDU_H264_RUN_BEF_ZL9 0x00A9U
#define BDU_H264_RUN_BEF_ZL10 0x00AAU
#define BDU_H264_RUN_BEF_ZL11 0x00ABU
#define BDU_H264_RUN_BEF_ZL12 0x00ACU
#define BDU_H264_RUN_BEF_ZL13 0x00ADU
#define BDU_H264_RUN_BEF_ZL14 0x00AEU
#define BDU_H264_RUN_BEF_ZL15 0x00AFU
#define BDU_H264_RUN_BEF_ZL16 0x00B0U
#define BDU_JPG_PARAMS       0x00B1U
#define BDU_JPG_DECODE       0x00B2U
#define BDU_JPG_MBDC         0x00B3U
#define BDU_IPCM_LUMA        0x00B4U
#define BDU_IPCM_CHROMA      0x00B5U
#define BDU_EXPGOL0          0x00B6U
#define BDU_EXPGOL1          0x00B7U
#define BDU_EXPGOL2          0x00B8U
#define BDU_EXPGOL3          0x00B9U
#define BDU_EXPGOL4          0x00BAU
#define BDU_CAVLC_TOTAL_COEFF 0x00C0U
#define BDU_CAVLC_TRAILING_ONES 0x00C1U
#define BDU_CAVLC_MODE       0x00C2U
#define BDU_CAVLC_STA        0x00C3U
#define BDU_CAVLC_START      0x00C4U
#define BDU_VC1_QUANT        0x00D0U
#define BDU_VC1_TABLES       0x00D1U
#define BDU_VC1_CBP          0x00D2U
#define BDU_VC1_DECODE_INTRA_CHROMA_8x8 0x00D3U
#define BDU_VC1_DECODE_INTRA_LUMA_8x8 0x00D4U
#define BDU_VC1_DECODE_INTER_CHROMA_8x8 0x00D5U
#define BDU_VC1_DECODE_INTER_CHROMA_8x4 0x00D6U
#define BDU_VC1_DECODE_INTER_CHROMA_4x8 0x00D7U
#define BDU_VC1_DECODE_INTER_CHROMA_4x4 0x00D8U
#define BDU_VC1_DECODE_INTER_LUMA_8x8 0x00D9U
#define BDU_VC1_DECODE_INTER_LUMA_8x4 0x00DAU
#define BDU_VC1_DECODE_INTER_LUMA_4x8 0x00DBU
#define BDU_VC1_DECODE_INTER_LUMA_4x4 0x00DCU
/* -------------------------------------------------------------------- */
/*  Bit Stream Packer Unit sub-addresses */
/* -------------------------------------------------------------------- */
#define BPU_RST1             0x0000U
#define BPU_RST23            0x0001U
#define BPU_CFG              0x0002U
#define BPU_GTS              0x0003U
#define BPU_GTB1             0x0004U
#define BPU_GTBh             0x0005U
#define BPU_WR1              0x0020U
#define BPU_FLU1             0x0040U
#define BPU_GTB2             0x0008U
#define BPU_MOV2             0x0009U
#define BPU_WR2              0x0060U
#define BPU_FLU2             0x0080U
#define BPU_MOV3             0x0011U
#define BPU_GTB3             0x0010U
#define BPU_WR3              0x00A0U
#define BPU_FLU3             0x00C0U
/* -------------------------------------------------------------------- */
/*  Memory Transfer Fifos registers */
/* -------------------------------------------------------------------- */
#define MTF_R_REG_SOFT_RESET 0x0000U
#define MTF_W_REG_SOFT_RESET 0x0001U
#define MTF_R_REG_ENR        0x0002U
#define MTF_W_REG_ENR        0x0003U
#define MTF_R_REG_DIR        0x0004U
#define MTF_W_REG_DIR        0x0005U
#define MTF_R_REG_STA        0x0006U
#define MTF_W_REG_STA        0x0007U
#define MTF_R_FIFO_8         0x0008U
#define MTF_R_FIFO_16        0x0009U
#define MTF_R_FIFO_24        0x000AU
#define MTF_R_FIFO_16_SWAP   0x000BU
#define MTF_R_FIFO_24_SWAP   0x000CU
#define MTF_R_FIFO_8_SIGNED  0x000DU
#define MTF_R_FIFO_16_SIGNED 0x000EU
#define MTF_R_FIFO_16_SWAP_SIGNED 0x000FU
#define MTF_R_FIFO_SKIP_BYTES 0x0010U
#define MTF_W_FIFO_8         0x0011U
#define MTF_W_FIFO_16        0x0012U
#define MTF_W_FIFO_24        0x0013U
#define MTF_W_FIFO_8_CLIP16  0x0014U
#define MTF_W_FIFO_8_CLIP24  0x0015U
#define MTF_W_FIFO_16_SWAP   0x0016U
#define MTF_W_FIFO_24_SWAP   0x0017U
/* -------------------------------------------------------------------- */
/*  Interrupt Controller sub-addresses */
/* -------------------------------------------------------------------- */
#define CFG_TIM_L            0x0000U
#define CFG_TIM_H            0x0001U
#define CFG_TIC_L            0x0002U
#define CFG_TIC_H            0x0003U
#define CFG_IIS_L            0x0010U
#define CFG_IIS_H            0x0011U
#define CFG_ISR_L            0x0012U
#define CFG_ISR_H            0x0013U
#define CFG_IMR_L            0x0014U
#define CFG_IMR_H            0x0015U
#define VEC_CTL_L            0x0040U
#define VEC_CTL_H            0x0041U
#define VEC_SEM_L            0x0042U
#define VEC_SEM_H            0x0043U
#define VEC_STA_L            0x0050U
#define VEC_STA_H            0x0051U
#define VEC_ISR_L            0x0052U
#define VEC_ISR_H            0x0053U
#define VEC_IMR_L            0x0054U
#define VEC_IMR_H            0x0055U
#define VDC_CTL_L            0x0060U
#define VDC_CTL_H            0x0061U
#define VDC_SEM_L            0x0062U
#define VDC_SEM_H            0x0063U
#define VDC_STA_L            0x0070U
#define VDC_STA_H            0x0071U
#define VDC_ISR_L            0x0072U
#define VDC_ISR_H            0x0073U
#define VDC_IMR_L            0x0074U
#define VDC_IMR_H            0x0075U
#define GRB_CTL_L            0x0080U
#define GRB_CTL_H            0x0081U
#define GRB_SEM_L            0x0082U
#define GRB_SEM_H            0x0083U
#define GRB_STA_L            0x0090U
#define GRB_STA_H            0x0091U
#define GRB_ISR_L            0x0092U
#define GRB_ISR_H            0x0093U
#define GRB_IMR_L            0x0094U
#define GRB_IMR_H            0x0095U
#define DPL_CTL_L            0x00A0U
#define DPL_CTL_H            0x00A1U
#define DPL_SEM_L            0x00A2U
#define DPL_SEM_H            0x00A3U
#define DPL_STA_L            0x00B0U
#define DPL_STA_H            0x00B1U
#define DPL_ISR_L            0x00B2U
#define DPL_ISR_H            0x00B3U
#define DPL_IMR_L            0x00B4U
#define DPL_IMR_H            0x00B5U
#define TVD_CTL_L            0x00C0U
#define TVD_CTL_H            0x00C1U
#define TVD_SEM_L            0x00C2U
#define TVD_SEM_H            0x00C3U
#define TVD_STA_L            0x00D0U
#define TVD_STA_H            0x00D1U
#define TVD_ISR_L            0x00D2U
#define TVD_ISR_H            0x00D3U
#define TVD_IMR_L            0x00D4U
#define TVD_IMR_H            0x00D5U
#define ITC_CMP0_L           0x0100U
#define ITC_CMP0_H           0x0101U
#define ITC_CMP1_L           0x0102U
#define ITC_CMP1_H           0x0103U
#define ITC_CMP2_L           0x0104U
#define ITC_CMP2_H           0x0105U
#define ITC_CMP3_L           0x0106U
#define ITC_CMP3_H           0x0107U
#define CFG_ISS_L            0x0110U
#define VEC_STS_L            0x0150U
#define VEC_ISS_L            0x0152U
#define VDC_STS_L            0x0170U
#define VDC_ISS_L            0x0172U
#define GRB_STS_L            0x0190U
#define GRB_ISS_L            0x0192U
#define DPL_STS_L            0x01B0U
#define DPL_ISS_L            0x01B2U
#define TVD_STS_L            0x01D0U
#define TVD_ISS_L            0x01D2U
/* -------------------------------------------------------------------- */
/*  Clock Generation Unit sub-addresses */
/* -------------------------------------------------------------------- */
#define CFG_CLK_L            0x0000U
#define CFG_CLK_H            0x0001U
#define CFG_RST_L            0x0002U
#define CFG_RST_H            0x0003U
#define CKG_CKEN1            0x0004U
#define CKG_CKEN2            0x0005U
/*  REG	TST_TCR_L   			CKG_BASE + 0x20U		1       0x1U */
/*  REG	TST_TCR_H   			CKG_BASE + 0x21U		0       0 */
/*  REG	TST_ITIP_L   			CKG_BASE + 0x22U		16      0xffffU */
/*  REG	TST_ITIP_H   			CKG_BASE + 0x23U		3       0x7U */
/*  REG	TST_ITOP_L   			CKG_BASE + 0x24U		6       0x3fU */
/*  REG	TST_ITOP_H   			CKG_BASE + 0x25U		0       0 */
#define IDN_PID0_L           0x00F0U
#define IDN_PID0_H           0x00F1U
#define IDN_PID1_L           0x00F2U
#define IDN_PID1_H           0x00F3U
#define IDN_PID2_L           0x00F4U
#define IDN_PID2_H           0x00F5U
#define IDN_PID3_L           0x00F6U
#define IDN_PID3_H           0x00F7U
#define IDN_PCID0_L          0x00F8U
#define IDN_PCID0_H          0x00F9U
#define IDN_PCID1_L          0x00FAU
#define IDN_PCID1_H          0x00FBU
#define IDN_PCID2_L          0x00FCU
#define IDN_PCID2_H          0x00FDU
#define IDN_PCID3_L          0x00FEU
#define IDN_PCID3_H          0x00FFU
/* -------------------------------------------------------------------- */
/*  <unit> sub-addresses */
/* --------------------------------------------------------------------                                                        */
#define UNIT_REG_SOFT_RESET  0x0000U
#define UNIT_REG_DEF         0x0001U
#define UNIT_REG_CFG         0x0002U
#define UNIT_REG_CMD         0x0003U
#define UNIT_FIFO_CMD        0x0004U
#define UNIT_FIFO_IN         0x0005U
#define UNIT_FIFO_OUT        0x0006U
#define UNIT_REG_CFG2        0x0007U
#define UNIT_REG_CFG3        0x0008U
#define UNIT_FIFO_CMD2       0x0009U
/* -------------------------------------------------------------------- */
/*  H264 Quantizer Unit H264Q sub-addresses */
/* -------------------------------------------------------------------- */
#define H264Q_REG_SOFT_RESET 0x0000U
#define H264Q_REG_DEF        0x0001U
#define H264Q_REG_CMD        0x0003U
#define H264Q_FIFO_CMD       0x0004U
/* -------------------------------------------------------------------- */
/*  H264 Inverse Quantizer Unit H264IQ sub-addresses */
/* -------------------------------------------------------------------- */
#define H264IQ_REG_SOFT_RESET 0x0000U
#define H264IQ_REG_DEF       0x0001U
#define H264IQ_REG_CMD       0x0003U
#define H264IQ_FIFO_CMD      0x0004U
/* -------------------------------------------------------------------- */
/*  H264 Transform Unit H264T sub-addresses */
/* -------------------------------------------------------------------- */
#define H264T_REG_SOFT_RESET 0x0000U
#define H264T_REG_DEF        0x0001U
#define H264T_REG_CMD        0x0003U
#define H264T_FIFO_CMD       0x0004U
/* -------------------------------------------------------------------- */
/*  H264 Inverse Transform Unit H264IT sub-addresses */
/* -------------------------------------------------------------------- */
#define H264IT_REG_SOFT_RESET 0x0000U
#define H264IT_REG_DEF       0x0001U
#define H264IT_REG_CMD       0x0003U
#define H264IT_FIFO_CMD      0x0004U
/* -------------------------------------------------------------------- */
/*  H264 CAVLC Unit H264RLC sub-addresses */
/* -------------------------------------------------------------------- */
#define H264RLC_REG_SOFT_RESET 0x0000U
#define H264RLC_REG_DEF      0x0001U
#define H264RLC_REG_CMD      0x0003U
#define H264RLC_FIFO_CMD     0x0004U
/* -------------------------------------------------------------------- */
/*  Discrete Cosine Transform Unit DCT sub-addresses */
/* -------------------------------------------------------------------- */
#define DCT_REG_SOFT_RESET   0x0000U
#define DCT_REG_CFG          0x0002U
/* -------------------------------------------------------------------- */
/*  Inverse Discrete Cosine Transform Unit IDCT sub-addresses */
/* -------------------------------------------------------------------- */
#define IDCT_REG_SOFT_RESET  0x0000U
#define IDCT_REG_CFG         0x0002U
/* -------------------------------------------------------------------- */
/*  Basic Quantizer Unit Q sub-addresses */
/* -------------------------------------------------------------------- */
#define Q_REG_SOFT_RESET     0x0000U
#define Q_REG_DEF            0x0001U
#define Q_REG_CFG            0x0002U
#define Q_REG_CMD            0x0003U
#define Q_FIFO_CMD           0x0004U
#define Q_REG_CFG2           0x0007U
/* -------------------------------------------------------------------- */
/*  Basic Inverse Quantizer Unit IQ sub-addresses */
/* -------------------------------------------------------------------- */
#define IQ_REG_SOFT_RESET    0x0000U
#define IQ_REG_DEF           0x0001U
#define IQ_REG_CFG           0x0002U
#define IQ_REG_CMD           0x0003U
#define IQ_FIFO_CMD          0x0004U
#define IQ_FIFO_OUT          0x0006U
#define IQ_REG_CFG2          0x0007U
#define IQ_REG_CFG3          0x0008U
#define IQ_FIFO_CMD2         0x0009U
#define IQ_REG_CMD2          0x000AU
/* -------------------------------------------------------------------- */
/*  Scan and Inverse Scan Unit SCN sub-addresses */
/* -------------------------------------------------------------------- */
#define SCN_REG_SOFT_RESET   0x0000U
#define SCN_REG_DEF          0x0001U
#define SCN_REG_CFG          0x0002U
#define SCN_REG_CMD          0x0003U
#define SCN_FIFO_CMD         0x0004U
/* -------------------------------------------------------------------- */
/*  Run Level Coder Unit RLC sub-addresses */
/* -------------------------------------------------------------------- */
#define RLC_REG_SOFT_RESET   0x0000U
#define RLC_REG_DEF          0x0001U
#define RLC_REG_CMD          0x0003U
#define RLC_FIFO_CMD         0x0004U
/* -------------------------------------------------------------------- */
/*  Inverse ACDC Predictor Unit IPRD sub-addresses */
/* -------------------------------------------------------------------- */
#define IPRD_REG_SOFT_RESET  0x0000U
#define IPRD_REG_DEF         0x0001U
#define IPRD_REG_CFG         0x0002U
#define IPRD_REG_CMD         0x0003U
#define IPRD_FIFO_CMD        0x0004U
#define IPRD_FIFO_IN         0x0005U
#define IPRD_FIFO_OUT        0x0006U
/* -------------------------------------------------------------------- */
/*  Quantizer step table Unit QTAB sub-addresses */
/* -------------------------------------------------------------------- */
#define QTAB_REG_SOFT_RESET  0x0000U
#define QTAB_REG_DEF         0x0001U
#define QTAB_REG_CMD         0x0003U
#define QTAB_FIFO_CMD        0x0004U
#define QTAB_FIFO_IN         0x0005U
#define QTAB_FIFO_OUT        0x0006U
/* -------------------------------------------------------------------- */
/*  Fifo Unit FIFO sub-addresses */
/* -------------------------------------------------------------------- */
#define FIFO_REG_SOFT_RESET  0x0000U
/* -------------------------------------------------------------------- */
/*  VC1 Inverse Transform Unit VC1IT sub-addresses */
/* -------------------------------------------------------------------- */
#define VC1IT_REG_SOFT_RESET 0x0000U
#define VC1IT_REG_DEF        0x0001U
#define VC1IT_REG_CMD        0x0003U
#define VC1IT_FIFO_CMD       0x0004U
/* -------------------------------------------------------------------- */
/*  VC1 Buffer Unit VBF sub-addresses */
/* -------------------------------------------------------------------- */
#define VBF_REG_SOFT_RESET   0x0000U
#define VBF_REG_CFG          0x0001U
#define VBF_FIFO_GET_CMD     0x0002U
#define VBF_FIFO_SEND_CMD    0x0003U
#define VBF_REG_STS_EMPTY    0x0004U
#define VBF_REG_STS_FULL     0x0005U
#define VBF_REG_STS_ERROR    0x0006U
#define VBF_REG_GET_ID       0x0010U
#define VBF_REG_SEND_ID      0x0011U
#define VBF_REG_CSBP_0       0x0020U
#define VBF_REG_CSBP_1       0x0021U
#define VBF_REG_CSBP_2       0x0022U
#define VBF_REG_CSBP_3       0x0023U
#define VBF_REG_CSBP_4       0x0024U
#define VBF_REG_CSBP_5       0x0025U
#define VBF_REG_CSBP_6       0x0026U
#define VBF_REG_CSBP_7       0x0027U
#define VBF_REG_CSBP_8       0x0028U
#define VBF_REG_CSBP_9       0x0029U
#define VBF_REG_CSBP_10      0x002AU
#define VBF_REG_CSBP_11      0x002BU
#define VBF_REG_BUF_0        0x0040U
#define VBF_REG_BUF_1        0x0080U
#define VBF_REG_BUF_2        0x00C0U
#define VBF_REG_BUF_3        0x0100U
#define VBF_REG_BUF_4        0x0140U
#define VBF_REG_BUF_5        0x0180U
#define VBF_REG_BUF_6        0x01C0U
#define VBF_REG_BUF_7        0x0200U
#define VBF_REG_BUF_8        0x0240U
#define VBF_REG_BUF_9        0x0280U
#define VBF_REG_BUF_10       0x02C0U
#define VBF_REG_BUF_11       0x0300U
/* -------------------------------------------------------------------- */
/*  VLC sub-addresses */
/* -------------------------------------------------------------------- */
#define VLC_REG_SOFT_RESET   0x0000U
#define VLC_REG_DEF          0x0001U
#define VLC_REG_CFG          0x0002U
#define VLC_REG_CMD          0x0003U
#define VLC_FIFO_CMD         0x0004U
#define VLC_FIFO_DC_PRED     0x0005U
#define VLC_FIFO_DC_CUR      0x0006U
#define VLC_FIFO_CBP         0x0007U
/* -------------------------------------------------------------------- */
/*  Fifo Unit FIFO2 sub-addresses */
/* -------------------------------------------------------------------- */
#define FIFO2_REG_SOFT_RESET 0x0000U
/* -------------------------------------------------------------------- */
/*  Post adder Unit PA sub-addresses */
/* -------------------------------------------------------------------- */
#define PA_REG_SOFT_RESET    0x0000U
#define PA_REG_DEF           0x0001U
#define PA_REG_CMD           0x0003U
#define PA_FIFO_CMD          0x0004U
/* -------------------------------------------------------------------- */
/*  Reconstruction Unit REC sub-addresses */
/* -------------------------------------------------------------------- */
#define REC_REG_SOFT_RESET   0x0000U
#define REC_REG_DEF          0x0001U
#define REC_REG_CMD          0x0003U
#define REC_FIFO_CMD         0x0004U
/* -------------------------------------------------------------------- */
/*  Video Post-Processing Unit sub-addresses */
/* -------------------------------------------------------------------- */
#define VPP_RST              0x0000U
#define VPP_ENR              0x0001U
#define VPP_DIR              0x0002U
#define VPP_STA              0x0003U
#define VPP_HFS              0x0004U
#define VPP_VFS              0x0005U
#define VPP_ISR              0x0006U
#define VPP_CFR              0x000AU
#define VPP_SFW              0x000BU
#define VPP_SFH              0x000CU
#define VPP_HFO              0x000DU
#define VPP_VFO              0x000EU
#define VPP_QP               0x000FU
#define VPP_HSO              0x0010U
#define VPP_VSO              0x0011U
/* -------------------------------------------------------------------- */
/*  H264 Intra Prediction Unit (IPA) sub-addresses */
/* -------------------------------------------------------------------- */
#define IPA_REG_SOFT_RESET   0x0000U
#define IPA_REG_ENR          0x0001U
#define IPA_REG_DIR          0x0002U
#define IPA_REG_STA          0x0003U
#define IPA_REG_IMR          0x0004U
#define IPA_REG_ISR          0x0005U
#define IPA_REG_STORE_RES    0x0006U
#define IPA_REG_CMD_NB       0x0007U
#define IPA_REG_SCAN_MODE    0x0008U
#define IPA_REG_PRED_TYPE    0x0009U
#define IPA_FIFO_MODE        0x0010U
#define IPA_REG_RXM4         0x0020U
#define IPA_REG_RXM3         0x0021U
#define IPA_REG_RXM2         0x0022U
#define IPA_REG_RXM1         0x0023U
#define IPA_REG_RX00         0x0024U
#define IPA_REG_RX01         0x0025U
#define IPA_REG_RX02         0x0026U
#define IPA_REG_RX03         0x0027U
#define IPA_REG_RX04         0x0028U
#define IPA_REG_RX05         0x0029U
#define IPA_REG_RX06         0x002AU
#define IPA_REG_RX07         0x002BU
#define IPA_REG_RX08         0x002CU
#define IPA_REG_RX09         0x002DU
#define IPA_REG_RX10         0x002EU
#define IPA_REG_RX11         0x002FU
#define IPA_REG_RX12         0x0030U
#define IPA_REG_RX13         0x0031U
#define IPA_REG_RX14         0x0032U
#define IPA_REG_RX15         0x0033U
#define IPA_REG_RX16         0x0034U
#define IPA_REG_RX17         0x0035U
#define IPA_REG_RX18         0x0036U
#define IPA_REG_RX19         0x0037U
#define IPA_REG_RYM1         0x0040U
#define IPA_REG_RY00         0x0041U
#define IPA_REG_RY01         0x0042U
#define IPA_REG_RY02         0x0043U
#define IPA_REG_RY03         0x0044U
#define IPA_REG_RY04         0x0045U
#define IPA_REG_RY05         0x0046U
#define IPA_REG_RY06         0x0047U
#define IPA_REG_RY07         0x0048U
#define IPA_REG_RY08         0x0049U
#define IPA_REG_RY09         0x004AU
#define IPA_REG_RY10         0x004BU
#define IPA_REG_RY11         0x004CU
#define IPA_REG_RY12         0x004DU
#define IPA_REG_RY13         0x004EU
#define IPA_REG_RY14         0x004FU
#define IPA_REG_RY15         0x0050U
#define IPA_REG_PACK_RXM4_M3 0x0060U
#define IPA_REG_PACK_RXM2_M1 0x0061U
#define IPA_REG_PACK_RX00_01 0x0062U
#define IPA_REG_PACK_RX02_03 0x0063U
#define IPA_REG_PACK_RX04_05 0x0064U
#define IPA_REG_PACK_RX06_07 0x0065U
#define IPA_REG_PACK_RX08_09 0x0066U
#define IPA_REG_PACK_RX10_11 0x0067U
#define IPA_REG_PACK_RX12_13 0x0068U
#define IPA_REG_PACK_RX14_15 0x0069U
#define IPA_REG_PACK_RX16_17 0x006AU
#define IPA_REG_PACK_RX18_19 0x006BU
#define IPA_REG_PACK_RYM1_00 0x0070U
#define IPA_REG_PACK_RY01_02 0x0071U
#define IPA_REG_PACK_RY03_04 0x0072U
#define IPA_REG_PACK_RY05_06 0x0073U
#define IPA_REG_PACK_RY07_08 0x0074U
#define IPA_REG_PACK_RY09_10 0x0075U
#define IPA_REG_PACK_RY11_12 0x0076U
#define IPA_REG_PACK_RY13_14 0x0077U
#define IPA_REG_PACK_RY15_XX 0x0078U


#endif	/* T1XHV_HAMAC_XBUS_REGS_H */

