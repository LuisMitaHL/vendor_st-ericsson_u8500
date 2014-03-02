/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 File generated from file : t1xhv_hamac_xbus_regs.def
 date : Wed Jul 15 15:21:27 MEST 2009
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
/*  26.09.2008 : AB update of registers for 8500 V1 */
/*  29.09.2008 : JPA :  */
/*  24.09.2008 : JPA addition of BDU H264 MB registers */
/*  09.06.2008 : MECC registers removed and IMC registers added */
/*  27.06.2008 : OS : add H264FITQ register defnition */
/*  07.07.2008 : OS : update IPA, HED, DMA register for 8500 */
/*  08.08.2008 : OS : Add the DFI address for H264FITQ and change DFI MECC in DFI IMC */
/*  24.07.2008 : OS : Modification of the IPA fifos definition */
/*  31.07.2008 : OS : Change the DFI_xxx_ID */
/*  28.08.2008 : JPA change IME_BASE and addition of IME_SOx registers(most register addresses have been shifted) */
/*  20.09.2008 : JCC update of registers for the DMA 8500 V1 */
/*  06.10.2008 : JCC update of registers for the DMA 8500 V1 */
/*  20.10.2008 : JPA correction of BDU_H264_MB_STA register and BDU registers acces type */
/*  31.10.2008 : JPA addition of BDU_JPG_DCPRED_XX registers */
/*  21.04.2009 : JCC modif register STA EOR for DMA */
/*  13.05.2009 : JPA addition of H264 HDR registers */
/* ---------------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/*  Base address definition */
/* -------------------------------------------------------------------- */
/* toberemovedwhensoftwareaware!! */
#define CFG_BASE             0x0000U
/* -------------------------------------------------------------------- */
#define DMA_BASE             0xC200U
#define CKG_BASE             0xC400U
#define MTF_BASE             0xC440U
#define VPP_BASE             0xC480U
#define HED_BASE             0xC4C0U
#define DFI_BASE             0xC600U
#define BDU_BASE             0xC700U
#define IME_BASE             0xC800U
#define IMC_BASE             0xC900U
#define MUX3TO1_BASE         0xC980U
#define IPA_BASE             0xCA00U
#define H264FITQ_BASE        0xCB00U
#define H264VLC_BASE         0xCC00U
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
#define FIFO2_BASE           0xCF40U
#define VLC_BASE             0xCF80U
#define CUP_BASE             0xD000U
#define VBF_BASE             0xD800U
#define BPU_BASE             0xDC00U
#define ITC_BASE             0xDE00U
/*  CUP Context base-addresses - to be used to build context sub_address */
#define CUP_CTX_A            0x0000U
#define CUP_CTX_X            0x0100U
#define CUP_CTX_Y            0x0200U
#define CUP_CTX_Z            0x0300U
#define CUP_CTX_D            0x0400U
#define CUP_CTX_B            0x0500U
#define CUP_CTX_C            0x0600U
#define CUP_CTX_E            0x0700U
#define CUP_REG_BASE         0x07C0U
#define DFI_BDU_ID           0x0008U
#define DFI_BPU_ID           0x0010U
#define DFI_IMC_ID           0x0018U
#define DFI_IPA_ID           0x0020U
#define DFI_DCT_ID           0x0028U
#define DFI_IDCT_ID          0x0030U
#define DFI_H264FITQ_ID      0x0038U
#define DFI_RES0_ID          0x0040U
#define DFI_Q_ID             0x0048U
#define DFI_IQ_ID            0x0050U
#define DFI_RES1_ID          0x0058U
#define DFI_RES2_ID          0x0060U
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
#define HV_HW_REL            0x0766U
/* -------------------------------------------------------------------- */
/*  Software registers (from 0x0000U to 0x01FFU, mmdsp data memory area) */
/* -------------------------------------------------------------------- */

#ifndef T1XHV_HOST

/* Following registers should get undefined reset value. Don't know how to specify this.  */
/*  To protect this, force H */
/* REG     CFG_PSA                 CFG_BASE + 0x0000U   16  0xffffU  UNDEFINED   ROH */
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
#define IDN_HRV              0x0004U

#endif

/* -------------------------------------------------------------------- */
/*  Motion Estimation sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                      	     mask    reset   	access  xiordy */
#define IMC_REG_SOFT_RESET   0x0000U
#define IMC_REG_ENR          0x0001U
#define IMC_REG_DIR          0x0002U
#define IMC_REG_STA          0x0003U
#define IMC_REG_FILTER_MODE  0x0005U
#define IMC_REG_RND          0x0006U
#define IMC_REG_IMAGE_WIDTH  0x0007U
#define IMC_REG_RANGE_REDUCTION 0x0008U
#define IMC_REG_INTENSITY_COMP 0x0009U
#define IMC_REG_IC_FWD       0x000AU
#define IMC_REG_IC_BWD       0x000BU
#define IMC_REG_WP_TYPE      0x000CU
#define IMC_REG_WP_LOGWD_L   0x000DU
#define IMC_REG_WP_LOGWD_C   0x000EU
#define IMC_FIFO_CMD         0x0010U
#define IMC_FIFO_MPX         0x0011U
#define IMC_FIFO_MPY         0x0012U
#define IMC_FIFO_REF_ID      0x0013U
#define IMC_FIFO_SIZE        0x0014U
#define IMC_FIFO_DST_BUF     0x0015U
#define IMC_FIFO_WP_FLAG     0x0016U
#define IMC_FIFO_WP_W0_L     0x0017U
#define IMC_FIFO_WP_W1_L     0x0018U
#define IMC_FIFO_WP_O0_L     0x0019U
#define IMC_FIFO_WP_O1_L     0x001AU
#define IMC_FIFO_WP_W0_CB    0x001BU
#define IMC_FIFO_WP_W1_CB    0x001CU
#define IMC_FIFO_WP_O0_CB    0x001DU
#define IMC_FIFO_WP_O1_CB    0x001EU
#define IMC_FIFO_WP_W0_CR    0x001FU
#define IMC_FIFO_WP_W1_CR    0x0020U
#define IMC_FIFO_WP_O0_CR    0x0021U
#define IMC_FIFO_WP_O1_CR    0x0022U
/* -------------------------------------------------------------------- */
/*  DFI (Data Flow Interconnect) sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                           mask    reset   access  xiordy */
#define DFI_REG_SOFT_RESET   0x0000U
#define DFI_REG_MACRO_DEF    0x0001U
#define DFI_REG_MACRO_CFG    0x0002U
#define DFI_REG_MACRO_CMD    0x0003U
#define DFI_FIFO_MACRO_CMD   0x0004U
/*  DFI_all: */
/*                                                               	mask    reset   access  xiordy */
#define DFI_REG_DEF          0x0000U
#define DFI_REG_CMD          0x0001U
#define DFI_FIFO_CMD         0x0002U
#define DFI_FIFO_SRC_LSB     0x0003U
#define DFI_FIFO_SRC_MSB     0x0004U
#define DFI_FIFO_DST_LSB     0x0005U
#define DFI_FIFO_DST_MSB     0x0006U
#define DFI_REG_STA_LAST     0x0007U
/*  BDU */
/*                                                                                 mask    reset   access  xiordy */
#define DFI_REG_BDU_DEF      0x0008U
#define DFI_REG_BDU_CMD      0x0009U
#define DFI_FIFO_BDU_CMD     0x000AU
#define DFI_FIFO_BDU_SRC_LSB 0x000BU
#define DFI_FIFO_BDU_SRC_MSB 0x000CU
#define DFI_FIFO_BDU_DST_LSB 0x000DU
#define DFI_FIFO_BDU_DST_MSB 0x000EU
#define DFI_REG_BDU_STA_LAST 0x000FU
/*  BPU */
/*                                                                                 mask    reset   access  xiordy */
#define DFI_REG_BPU_DEF      0x0010U
#define DFI_REG_BPU_CMD      0x0011U
#define DFI_FIFO_BPU_CMD     0x0012U
#define DFI_FIFO_BPU_SRC_LSB 0x0013U
#define DFI_FIFO_BPU_SRC_MSB 0x0014U
#define DFI_FIFO_BPU_DST_LSB 0x0015U
#define DFI_FIFO_BPU_DST_MSB 0x0016U
#define DFI_REG_BPU_STA_LAST 0x0017U
/*  IMC */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_IMC_DEF      0x0018U
#define DFI_REG_IMC_CMD      0x0019U
#define DFI_FIFO_IMC_CMD     0x001AU
#define DFI_FIFO_IMC_SRC_LSB 0x001BU
#define DFI_FIFO_IMC_SRC_MSB 0x001CU
#define DFI_FIFO_IMC_DST_LSB 0x001DU
#define DFI_FIFO_IMC_DST_MSB 0x001EU
#define DFI_REG_IMC_STA_LAST 0x001FU
/*  IPA */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_IPA_DEF      0x0020U
#define DFI_REG_IPA_CMD      0x0021U
#define DFI_FIFO_IPA_CMD     0x0022U
#define DFI_FIFO_IPA_SRC_LSB 0x0023U
#define DFI_FIFO_IPA_SRC_MSB 0x0024U
#define DFI_FIFO_IPA_DST_LSB 0x0025U
#define DFI_FIFO_IPA_DST_MSB 0x0026U
#define DFI_REG_IPA_STA_LAST 0x0027U
/*  DCT */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_DCT_DEF      0x0028U
#define DFI_REG_DCT_CMD      0x0029U
#define DFI_FIFO_DCT_CMD     0x002AU
#define DFI_FIFO_DCT_SRC_LSB 0x002BU
#define DFI_FIFO_DCT_SRC_MSB 0x002CU
#define DFI_FIFO_DCT_DST_LSB 0x002DU
#define DFI_FIFO_DCT_DST_MSB 0x002EU
#define DFI_REG_DCT_STA_LAST 0x002FU
/*  IDCT */
/*                                                                                 mask    reset   access  xiordy */
#define DFI_REG_IDCT_DEF     0x0030U
#define DFI_REG_IDCT_CMD     0x0031U
#define DFI_FIFO_IDCT_CMD    0x0032U
#define DFI_FIFO_IDCT_SRC_LSB 0x0033U
#define DFI_FIFO_IDCT_SRC_MSB 0x0034U
#define DFI_FIFO_IDCT_DST_LSB 0x0035U
#define DFI_FIFO_IDCT_DST_MSB 0x0036U
#define DFI_REG_IDCT_STA_LAST 0x0037U
/*  Q */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_Q_DEF        0x0048U
#define DFI_REG_Q_CMD        0x0049U
#define DFI_FIFO_Q_CMD       0x004AU
#define DFI_FIFO_Q_SRC_LSB   0x004BU
#define DFI_FIFO_Q_SRC_MSB   0x004CU
#define DFI_FIFO_Q_DST_LSB   0x004DU
#define DFI_FIFO_Q_DST_MSB   0x004EU
#define DFI_REG_Q_STA_LAST   0x004FU
/*  IQ */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_IQ_DEF       0x0050U
#define DFI_REG_IQ_CMD       0x0051U
#define DFI_FIFO_IQ_CMD      0x0052U
#define DFI_FIFO_IQ_SRC_LSB  0x0053U
#define DFI_FIFO_IQ_SRC_MSB  0x0054U
#define DFI_FIFO_IQ_DST_LSB  0x0055U
#define DFI_FIFO_IQ_DST_MSB  0x0056U
#define DFI_REG_IQ_STA_LAST  0x0057U
/*  SCN */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_SCN_DEF      0x0068U
#define DFI_REG_SCN_CMD      0x0069U
#define DFI_FIFO_SCN_CMD     0x006AU
#define DFI_FIFO_SCN_SRC_LSB 0x006BU
#define DFI_FIFO_SCN_SRC_MSB 0x006CU
#define DFI_FIFO_SCN_DST_LSB 0x006DU
#define DFI_FIFO_SCN_DST_MSB 0x006EU
#define DFI_REG_SCN_STA_LAST 0x006FU
/*  RLC */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_RLC_DEF      0x0070U
#define DFI_REG_RLC_CMD      0x0071U
#define DFI_FIFO_RLC_CMD     0x0072U
#define DFI_FIFO_RLC_SRC_LSB 0x0073U
#define DFI_FIFO_RLC_SRC_MSB 0x0074U
#define DFI_FIFO_RLC_DST_LSB 0x0075U
#define DFI_FIFO_RLC_DST_MSB 0x0076U
#define DFI_REG_RLC_STA_LAST 0x0077U
/*  H264RLC */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_H264RLC_DEF  0x0078U
#define DFI_REG_H264RLC_CMD  0x0079U
#define DFI_FIFO_H264RLC_CMD 0x007AU
#define DFI_FIFO_H264RLC_SRC_LSB 0x007BU
#define DFI_FIFO_H264RLC_SRC_MSB 0x007CU
#define DFI_FIFO_H264RLC_DST_LSB 0x007DU
#define DFI_FIFO_H264RLC_DST_MSB 0x007EU
#define DFI_REG_H264RLC_STA_LAST 0x007FU
/*  IPRD */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_IPRD_DEF     0x0080U
#define DFI_REG_IPRD_CMD     0x0081U
#define DFI_FIFO_IPRD_CMD    0x0082U
#define DFI_FIFO_IPRD_SRC_LSB 0x0083U
#define DFI_FIFO_IPRD_SRC_MSB 0x0084U
#define DFI_FIFO_IPRD_DST_LSB 0x0085U
#define DFI_FIFO_IPRD_DST_MSB 0x0086U
#define DFI_REG_IPRD_STA_LAST 0x0087U
/*  PA */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_PA_DEF       0x0088U
#define DFI_REG_PA_CMD       0x0089U
#define DFI_FIFO_PA_CMD      0x008AU
#define DFI_FIFO_PA_SRC_LSB  0x008BU
#define DFI_FIFO_PA_SRC_MSB  0x008CU
#define DFI_FIFO_PA_DST_LSB  0x008DU
#define DFI_FIFO_PA_DST_MSB  0x008EU
#define DFI_REG_PA_STA_LAST  0x008FU
/*  REC */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_REC_DEF      0x0090U
#define DFI_REG_REC_CMD      0x0091U
#define DFI_FIFO_REC_CMD     0x0092U
#define DFI_FIFO_REC_SRC_LSB 0x0093U
#define DFI_FIFO_REC_SRC_MSB 0x0094U
#define DFI_FIFO_REC_DST_LSB 0x0095U
#define DFI_FIFO_REC_DST_MSB 0x0096U
#define DFI_REG_REC_STA_LAST 0x0097U
/*  FIFO */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_FIFO_DEF     0x0098U
#define DFI_REG_FIFO_CMD     0x0099U
#define DFI_FIFO_FIFO_CMD    0x009AU
#define DFI_FIFO_FIFO_SRC_LSB 0x009BU
#define DFI_FIFO_FIFO_SRC_MSB 0x009CU
#define DFI_FIFO_FIFO_DST_LSB 0x009DU
#define DFI_FIFO_FIFO_DST_MSB 0x009EU
#define DFI_REG_FIFO_STA_LAST 0x009FU
/*  QTAB */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_QTAB_DEF     0x00A0U
#define DFI_REG_QTAB_CMD     0x00A1U
#define DFI_FIFO_QTAB_CMD    0x00A2U
#define DFI_FIFO_QTAB_SRC_LSB 0x00A3U
#define DFI_FIFO_QTAB_SRC_MSB 0x00A4U
#define DFI_FIFO_QTAB_DST_LSB 0x00A5U
#define DFI_FIFO_QTAB_DST_MSB 0x00A6U
#define DFI_REG_QTAB_STA_LAST 0x00A7U
/*  VC1IT */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_VC1IT_DEF    0x00A8U
#define DFI_REG_VC1IT_CMD    0x00A9U
#define DFI_FIFO_VC1IT_CMD   0x00AAU
#define DFI_FIFO_VC1IT_SRC_LSB 0x00ABU
#define DFI_FIFO_VC1IT_SRC_MSB 0x00ACU
#define DFI_FIFO_VC1IT_DST_LSB 0x00ADU
#define DFI_FIFO_VC1IT_DST_MSB 0x00AEU
#define DFI_REG_VC1IT_STA_LAST 0x00AFU
/*  VBF */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_VBF_DEF      0x00B0U
#define DFI_REG_VBF_CMD      0x00B1U
#define DFI_FIFO_VBF_CMD     0x00B2U
#define DFI_FIFO_VBF_SRC_LSB 0x00B3U
#define DFI_FIFO_VBF_SRC_MSB 0x00B4U
#define DFI_FIFO_VBF_DST_LSB 0x00B5U
#define DFI_FIFO_VBF_DST_MSB 0x00B6U
#define DFI_REG_VBF_STA_LAST 0x00B7U
/*  FIFO2 */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_FIFO2_DEF    0x00B8U
#define DFI_REG_FIFO2_CMD    0x00B9U
#define DFI_FIFO_FIFO2_CMD   0x00BAU
#define DFI_FIFO_FIFO2_SRC_LSB 0x00BBU
#define DFI_FIFO_FIFO2_SRC_MSB 0x00BCU
#define DFI_FIFO_FIFO2_DST_LSB 0x00BDU
#define DFI_FIFO_FIFO2_DST_MSB 0x00BEU
#define DFI_REG_FIFO2_STA_LAST 0x00BFU
/*  VLC */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_VLC_DEF      0x00C0U
#define DFI_REG_VLC_CMD      0x00C1U
#define DFI_FIFO_VLC_CMD     0x00C2U
#define DFI_FIFO_VLC_SRC_LSB 0x00C3U
#define DFI_FIFO_VLC_SRC_MSB 0x00C4U
#define DFI_FIFO_VLC_DST_LSB 0x00C5U
#define DFI_FIFO_VLC_DST_MSB 0x00C6U
#define DFI_REG_VLC_STA_LAST 0x00C7U
/*  H264FITQ */
/*                                                                               mask    reset   access  xiordy */
#define DFI_REG_H264FITQ_DEF 0x0038U
#define DFI_REG_H264FITQ_CMD 0x0039U
#define DFI_FIFO_H264FITQ_CMD 0x003AU
#define DFI_FIFO_H264FITQ_SRC_LSB 0x003BU
#define DFI_FIFO_H264FITQ_SRC_MSB 0x003CU
#define DFI_FIFO_H264FITQ_DST_LSB 0x003DU
#define DFI_FIFO_H264FITQ_DST_MSB 0x003EU
#define DFI_REG_H264FITQ_STA_LAST 0x003FU
/* -------------------------------------------------------------------- */
/*  DMA sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                   mask    reset   access  xiordy */
#define DMA_ENR_L            0x0000U
#define DMA_ENR_H            0x0001U
#define DMA_DIR_L            0x0002U
#define DMA_DIR_H            0x0003U
#define DMA_IMR_L            0x0004U
#define DMA_IMR_H            0x0005U
#define DMA_ISR_L            0x0006U
#define DMA_ISR_H            0x0007U
#define DMA_SOFT_RST_L       0x0008U
#define DMA_SOFT_RST_H       0x0009U
#define DMA_FIFO_STA_L       0x000AU
#define DMA_FIFO_STA_H       0x000BU
#define DMA_MTF_R_TFL        0x000CU
#define DMA_MTF_R_INCR       0x000DU
#define DMA_MTF_R_BSA_L      0x000EU
#define DMA_MTF_R_BSA_H      0x000FU
#define DMA_MTF_W_TFL        0x0010U
#define DMA_MTF_W_INCR       0x0011U
#define DMA_MTF_W_BSA_L      0x0012U
#define DMA_MTF_W_BSA_H      0x0013U
#define DMA_CDR_BWS_L        0x0014U
#define DMA_CDR_BWS_H        0x0015U
#define DMA_CDR_BWE_L        0x0016U
#define DMA_CDR_BWE_H        0x0017U
#define DMA_CDR_CMD          0x0018U
#define DMA_CDR_CBP_L        0x0019U
#define DMA_CDR_CBP_H        0x0020U
#define DMA_BDU_CBP_L        0x0021U
#define DMA_BDU_CBP_H        0x0022U
#define DMA_CDW_BWS_L        0x0023U
#define DMA_CDW_BWS_H        0x0024U
#define DMA_CDW_BWE_L        0x0025U
#define DMA_CDW_BWE_H        0x0026U
#define DMA_REC_DLS_L        0x0027U
#define DMA_REC_DLS_H        0x0028U
#define DMA_REC_DCS_L        0x0029U
#define DMA_REC_DCS_H        0x0030U
#define DMA_REC_DFW          0x0031U
#define DMA_REC_DFH          0x0032U
#define DMA_REC_DMC          0x0033U
#define DMA_DEB_SLS_L        0x0034U
#define DMA_DEB_SLS_H        0x0035U
#define DMA_DEB_SCS_L        0x0036U
#define DMA_DEB_SCS_H        0x0037U
#define DMA_DEB_SCS2_L       0x0038U
#define DMA_DEB_SCS2_H       0x0039U
#define DMA_DEB_SFH          0x003AU
#define DMA_DEB_SFW          0x003BU
#define DMA_DEB_RASTER_IN    0x003CU
#define DMA_DEB_RASTER_OUT   0x003DU
#define DMA_DEB_DLS_L        0x003EU
#define DMA_DEB_DLS_H        0x003FU
#define DMA_DEB_DCS_L        0x0040U
#define DMA_DEB_DCS_H        0x0041U
#define DMA_DEB_DCS2_L       0x0042U
#define DMA_DEB_DCS2_H       0x0043U
#define DMA_DEB_DFW          0x0044U
#define DMA_DEB_DFH          0x0045U
#define DMA_DEB_PS_L         0x0046U
#define DMA_DEB_PS_H         0x0047U
#define DMA_DEB_PE_L         0x0048U
#define DMA_DEB_PE_H         0x0049U
#define DMA_DEB_DUMMYS_L     0x004AU
#define DMA_DEB_DUMMYS_H     0x004BU
#define DMA_DEB_DUMMYE_L     0x004CU
#define DMA_DEB_DUMMYE_H     0x004DU
#define DMA_IME_CWL_L        0x0050U
#define DMA_IME_CWL_H        0x0051U
#define DMA_IME_CWL_SFW      0x0052U
#define DMA_IME_CWL_SFH      0x0053U
#define DMA_IME_TPL_L        0x0054U
#define DMA_IME_TPL_H        0x0055U
#define DMA_IME_TPL_SFW      0x0056U
#define DMA_IME_TPL_SFH      0x0057U
#define DMA_IME_SWL_L        0x0058U
#define DMA_IME_SWL_H        0x0059U
#define DMA_IME_SWL_SFW      0x005AU
#define DMA_IME_SWL_SFH      0x005BU
#define DMA_IME_MVF_SA_L     0x005CU
#define DMA_IME_MVF_SA_H     0x005DU
#define DMA_IME_MVF_EA_L     0x005EU
#define DMA_IME_MVF_EA_H     0x005FU
#define DMA_IME_CCM_SA_L     0x0060U
#define DMA_IME_CCM_SA_H     0x0061U
#define DMA_IME_CCM_EA_L     0x0062U
#define DMA_IME_CCM_EA_H     0x0063U
#define DMA_IPA_TPL_L        0x0064U
#define DMA_IPA_TPL_H        0x0065U
#define DMA_IPA_TPC_L        0x0066U
#define DMA_IPA_TPC_H        0x0067U
#define DMA_IPA_TP_SFW       0x0068U
#define DMA_IPA_TP_SFH       0x0069U
#define DMA_RECF_DLS_L       0x006AU
#define DMA_RECF_DLS_H       0x006BU
#define DMA_RECF_DCS_L       0x006CU
#define DMA_RECF_DCS_H       0x006DU
#define DMA_CUP_CTX_W_SA_L   0x006FU
#define DMA_CUP_CTX_W_SA_H   0x0070U
#define DMA_CUP_CTX_W_EA_L   0x0071U
#define DMA_CUP_CTX_W_EA_H   0x0072U
#define DMA_CUP_DBP_W_SA_L   0x0073U
#define DMA_CUP_DBP_W_SA_H   0x0074U
#define DMA_CUP_DBP_W_EA_L   0x0075U
#define DMA_CUP_DBP_W_EA_H   0x0076U
#define DMA_CUP_CTX_R_SA_L   0x0077U
#define DMA_CUP_CTX_R_SA_H   0x0078U
#define DMA_CUP_CTX_R_EA_L   0x0079U
#define DMA_CUP_CTX_R_EA_H   0x007AU
#define DMA_IMC_ORG_SFW      0x007BU
#define DMA_IMC_ORG_SFH      0x007CU
#define DMA_IMC_REF_SFW      0x007DU
#define DMA_IMC_REF_SFH      0x007EU
#define DMA_IMC_MZY0         0x007FU
#define DMA_REF0_FLS_L       0x0080U
#define DMA_REF0_FLS_H       0x0081U
#define DMA_REF0_FCS_L       0x0082U
#define DMA_REF0_FCS_H       0x0083U
#define DMA_REF1_FLS_L       0x0084U
#define DMA_REF1_FLS_H       0x0085U
#define DMA_REF1_FCS_L       0x0086U
#define DMA_REF1_FCS_H       0x0087U
#define DMA_REF2_FLS_L       0x0088U
#define DMA_REF2_FLS_H       0x0089U
#define DMA_REF2_FCS_L       0x008AU
#define DMA_REF2_FCS_H       0x008BU
#define DMA_REF3_FLS_L       0x008CU
#define DMA_REF3_FLS_H       0x008DU
#define DMA_REF3_FCS_L       0x008EU
#define DMA_REF3_FCS_H       0x008FU
#define DMA_REF4_FLS_L       0x0090U
#define DMA_REF4_FLS_H       0x0091U
#define DMA_REF4_FCS_L       0x0092U
#define DMA_REF4_FCS_H       0x0093U
#define DMA_REF5_FLS_L       0x0094U
#define DMA_REF5_FLS_H       0x0095U
#define DMA_REF5_FCS_L       0x0096U
#define DMA_REF5_FCS_H       0x0097U
#define DMA_REF6_FLS_L       0x0098U
#define DMA_REF6_FLS_H       0x0099U
#define DMA_REF6_FCS_L       0x009AU
#define DMA_REF6_FCS_H       0x009BU
#define DMA_REF7_FLS_L       0x009CU
#define DMA_REF7_FLS_H       0x009DU
#define DMA_REF7_FCS_L       0x009EU
#define DMA_REF7_FCS_H       0x009FU
#define DMA_REF8_FLS_L       0x00A0U
#define DMA_REF8_FLS_H       0x00A1U
#define DMA_REF8_FCS_L       0x00A2U
#define DMA_REF8_FCS_H       0x00A3U
#define DMA_REF9_FLS_L       0x00A4U
#define DMA_REF9_FLS_H       0x00A5U
#define DMA_REF9_FCS_L       0x00A6U
#define DMA_REF9_FCS_H       0x00A7U
#define DMA_REF10_FLS_L      0x00A8U
#define DMA_REF10_FLS_H      0x00A9U
#define DMA_REF10_FCS_L      0x00AAU
#define DMA_REF10_FCS_H      0x00ABU
#define DMA_REF11_FLS_L      0x00ACU
#define DMA_REF11_FLS_H      0x00ADU
#define DMA_REF11_FCS_L      0x00AEU
#define DMA_REF11_FCS_H      0x00AFU
#define DMA_REF12_FLS_L      0x00B0U
#define DMA_REF12_FLS_H      0x00B1U
#define DMA_REF12_FCS_L      0x00B2U
#define DMA_REF12_FCS_H      0x00B3U
#define DMA_REF13_FLS_L      0x00B4U
#define DMA_REF13_FLS_H      0x00B5U
#define DMA_REF13_FCS_L      0x00B6U
#define DMA_REF13_FCS_H      0x00B7U
#define DMA_REF14_FLS_L      0x00B8U
#define DMA_REF14_FLS_H      0x00B9U
#define DMA_REF14_FCS_L      0x00BAU
#define DMA_REF14_FCS_H      0x00BBU
#define DMA_REF15_FLS_L      0x00BCU
#define DMA_REF15_FLS_H      0x00BDU
#define DMA_REF15_FCS_L      0x00BEU
#define DMA_REF15_FCS_H      0x00BFU
#define DMA_REF16_FLS_L      0x00C0U
#define DMA_REF16_FLS_H      0x00C1U
#define DMA_REF16_FCS_L      0x00C2U
#define DMA_REF16_FCS_H      0x00C3U
#define DMA_SWF_DLS_L        0x00C4U
#define DMA_SWF_DLS_H        0x00C5U
#define DMA_SWF_DFH          0x00C6U
#define DMA_SYNCHRO_ENR      0x00C7U
#define DMA_STA_EOR          0x00C8U
#define DMA_RLCB_SYNC        0x00C9U
#define DMA_REC_XIF_PCX      0x00CAU
#define DMA_REC_XIF_PCY      0x00CBU
#define DMA_REC_PCX_STA      0x00CCU
#define DMA_REC_PCY_STA      0x00CDU
#define DMA_SWF_PREFETCH     0x00CEU
#define DMA_SWF_PREFETCHED   0x00CFU
#define DMA_BSM_L            0x00EFU
#define DMA_BSM_H            0x00F0U
#define DMA_ERR_ITS          0x00F1U
#define DMA_ERR_ITS_BSET     0x00F2U
#define DMA_ERR_ITS_BCLR     0x00F3U
#define DMA_ERR_ITM          0x00F4U
#define DMA_ERR_ITM_BSET     0x00F5U
#define DMA_ERR_ITM_BCLR     0x00F6U
#define DMA_ERR_INFO         0x00F7U
#define DMA_ERR_INFO_CLR     0x00F8U

#ifndef T1XHV_HOST

#define DMA_MAX_OPC_SZ0      0x0100U
#define DMA_MAX_OPC_SZ1      0x0101U
#define DMA_MAX_OPC_SZ2      0x0102U
#define DMA_MAX_OPC_SZ3      0x0103U
#define DMA_MAX_OPC_SZ4      0x0104U
#define DMA_MAX_OPC_SZ5      0x0105U
#define DMA_MAX_OPC_SZ6      0x0106U
#define DMA_MAX_OPC_SZ7      0x0107U
#define DMA_MAX_OPC_SZ8      0x0108U
#define DMA_MAX_OPC_SZ9      0x0109U
#define DMA_MAX_OPC_SZ10     0x010AU
#define DMA_MAX_OPC_SZ11     0x010BU
#define DMA_MAX_OPC_SZ12     0x010CU
#define DMA_MAX_OPC_SZ13     0x010DU
#define DMA_MAX_OPC_SZ14     0x010EU
#define DMA_MAX_OPC_SZ15     0x010FU
#define DMA_MAX_OPC_SZ16     0x0110U
#define DMA_MAX_OPC_SZ17     0x0111U
#define DMA_MAX_OPC_SZ18     0x0112U
#define DMA_MAX_OPC_SZ19     0x0113U
#define DMA_MAX_OPC_SZ20     0x0114U
#define DMA_MAX_OPC_SZ21     0x0115U
#define DMA_MAX_OPC_SZ22     0x0116U
#define DMA_MAX_OPC_SZ23     0x0117U
#define DMA_MAX_OPC_SZ24     0x0118U
#define DMA_PRIO_LEVEL0      0x0120U
#define DMA_PRIO_LEVEL1      0x0121U
#define DMA_PRIO_LEVEL2      0x0122U
#define DMA_PRIO_LEVEL3      0x0123U
#define DMA_PRIO_LEVEL4      0x0124U
#define DMA_PRIO_LEVEL5      0x0125U
#define DMA_PRIO_LEVEL6      0x0126U
#define DMA_PRIO_LEVEL7      0x0127U
#define DMA_PRIO_LEVEL8      0x0128U
#define DMA_PRIO_LEVEL9      0x0129U
#define DMA_PRIO_LEVEL10     0x012AU
#define DMA_PRIO_LEVEL11     0x012BU
#define DMA_PRIO_LEVEL12     0x012CU
#define DMA_PRIO_LEVEL13     0x012DU
#define DMA_PRIO_LEVEL14     0x012EU
#define DMA_PRIO_LEVEL15     0x012FU
#define DMA_PRIO_LEVEL16     0x0130U
#define DMA_PRIO_LEVEL17     0x0131U
#define DMA_PRIO_LEVEL18     0x0132U
#define DMA_PRIO_LEVEL19     0x0133U
#define DMA_PRIO_LEVEL20     0x0134U
#define DMA_PRIO_LEVEL21     0x0135U
#define DMA_PRIO_LEVEL22     0x0136U
#define DMA_PRIO_LEVEL23     0x0137U
#define DMA_PRIO_LEVEL24     0x0138U
#define DMA_MAX_MSSG_SZ0     0x0140U
#define DMA_MAX_MSSG_SZ1     0x0141U
#define DMA_MAX_MSSG_SZ2     0x0142U
#define DMA_MAX_MSSG_SZ3     0x0143U
#define DMA_MAX_MSSG_SZ4     0x0144U
#define DMA_MAX_MSSG_SZ5     0x0145U
#define DMA_MAX_MSSG_SZ6     0x0146U
#define DMA_MAX_MSSG_SZ7     0x0147U
#define DMA_MAX_MSSG_SZ8     0x0148U
#define DMA_MAX_MSSG_SZ9     0x0149U
#define DMA_MAX_MSSG_SZ10    0x014AU
#define DMA_MAX_MSSG_SZ11    0x014BU
#define DMA_MAX_MSSG_SZ12    0x014CU
#define DMA_MAX_MSSG_SZ13    0x014DU
#define DMA_MAX_MSSG_SZ14    0x014EU
#define DMA_MAX_MSSG_SZ15    0x014FU
#define DMA_MAX_MSSG_SZ16    0x0150U
#define DMA_MAX_MSSG_SZ17    0x0151U
#define DMA_MAX_MSSG_SZ18    0x0152U
#define DMA_MAX_MSSG_SZ19    0x0153U
#define DMA_MAX_MSSG_SZ20    0x0154U
#define DMA_MAX_MSSG_SZ21    0x0155U
#define DMA_MAX_MSSG_SZ22    0x0156U
#define DMA_MAX_MSSG_SZ23    0x0157U
#define DMA_MAX_MSSG_SZ24    0x0158U
#define DMA_MAX_CHCK_SZ0     0x0160U
#define DMA_MAX_CHCK_SZ1     0x0161U
#define DMA_MAX_CHCK_SZ2     0x0162U
#define DMA_MAX_CHCK_SZ3     0x0163U
#define DMA_MAX_CHCK_SZ4     0x0164U
#define DMA_MAX_CHCK_SZ5     0x0165U
#define DMA_MAX_CHCK_SZ6     0x0166U
#define DMA_MAX_CHCK_SZ7     0x0167U
#define DMA_MAX_CHCK_SZ8     0x0168U
#define DMA_MAX_CHCK_SZ9     0x0169U
#define DMA_MAX_CHCK_SZ10    0x016AU
#define DMA_MAX_CHCK_SZ11    0x016BU
#define DMA_MAX_CHCK_SZ12    0x016CU
#define DMA_MAX_CHCK_SZ13    0x016DU
#define DMA_MAX_CHCK_SZ14    0x016EU
#define DMA_MAX_CHCK_SZ15    0x016FU
#define DMA_MAX_CHCK_SZ16    0x0170U
#define DMA_MAX_CHCK_SZ17    0x0171U
#define DMA_MAX_CHCK_SZ18    0x0172U
#define DMA_MAX_CHCK_SZ19    0x0173U
#define DMA_MAX_CHCK_SZ20    0x0174U
#define DMA_MAX_CHCK_SZ21    0x0175U
#define DMA_MAX_CHCK_SZ22    0x0176U
#define DMA_MAX_CHCK_SZ23    0x0177U
#define DMA_MAX_CHCK_SZ24    0x0178U
#define DMA_MIN_OPC_SZ       0x017AU
#define DMA_PAGE_SZ_SRAM     0x017BU
#define DMA_PAGE_SZ_DRAM     0x017CU
#define DMA_PORT_CTRL_L      0x017DU
#define DMA_PORT_CTRL_H      0x017EU
#define DMA_MAX_PRI          0x017FU

#endif

#define NOD_INIT1_PRIORITY   0x0000U
#define NOD_INIT2_PRIORITY   0x0002U
#define NOD_INIT3_PRIORITY   0x0004U
#define NOD_INIT1_LATENCY    0x0006U
#define NOD_INIT2_LATENCY    0x0008U
#define NOD_INIT3_LATENCY    0x000AU
#define NOD_INIT1_BANDWIDTH  0x000CU
#define NOD_INIT2_BANDWIDTH  0x000EU
#define NOD_INIT3_BANDWIDTH  0x0010U
#define NOD_INIT1_LIMIT_L    0x0012U
#define NOD_INIT1_LIMIT_H    0x0013U
#define NOD_INIT2_LIMIT_L    0x0014U
#define NOD_INIT2_LIMIT_H    0x0015U
#define NOD_INIT3_LIMIT_L    0x0016U
#define NOD_INIT3_LIMIT_H    0x0017U
#define NOD_TARG_PRIORITY    0x0018U
#define NOD_IT_EN            0x0019U
#define NOD_IT               0x001AU
#define NOD_DEBUG_REG        0x001BU
/* -------------------------------------------------------------------- */
/*  Bit Stream De-Packer Unit sub-addresses */
/* -------------------------------------------------------------------- */
/*        REG_NAME    - BASE_ADD + OFFSET - BW -- BW_MASK - INIT_VAL - MODE   */
/*  getbits/showbits addresses are assigned to speed up commands decode=> they */
/*  should be modified in respect with bdu xif special (getbits/showbits) decoding logic */
/*  0x11U-0x1FU addresses are excluded according to xif address decode */
#define BDU_GTB1             0x0001U
#define BDU_GTB2             0x0002U
#define BDU_GTB3             0x0003U
#define BDU_GTB4             0x0004U
#define BDU_GTB5             0x0005U
#define BDU_GTB6             0x0006U
#define BDU_GTB7             0x0007U
#define BDU_GTB8             0x0008U
#define BDU_GTB9             0x0009U
#define BDU_GTB10            0x000AU
#define BDU_GTB11            0x000BU
#define BDU_GTB12            0x000CU
#define BDU_GTB13            0x000DU
#define BDU_GTB14            0x000EU
#define BDU_GTB15            0x000FU
#define BDU_GTB16            0x0010U
#define BDU_RST              0x0020U
#define BDU_CFR              0x0021U
#define BDU_ENA              0x0022U
#define BDU_STOP             0x0023U
#define BDU_CBP_L            0x0024U
#define BDU_CBP_H            0x0025U
#define BDU_SFT              0x0026U
#define BDU_SCS_ANY          0x0027U
#define BDU_PXP0             0x0028U
#define BDU_PXP1             0x0029U
#define BDU_REV              0x002AU
#define BDU_FLU              0x002BU
#define BDU_CBR              0x002CU
#define BDU_HERR             0x002DU
#define BDU_TCOEF_START      0x0030U
#define BDU_TCOEF_GET_STATUS 0x0031U
#define BDU_H264_PREFIX      0x0036U
#define BDU_H264_TZC_TC1     0x0039U
#define BDU_H264_TZC_TC2     0x003AU
#define BDU_H264_TZC_TC3     0x003BU
#define BDU_H264_MB_START    0x003CU
#define BDU_H264_MB_STA      0x003DU
#define BDU_H264_MB_COEFNB   0x003FU
#define BDU_H264_HDR_START   0x002EU
#define BDU_H264_HDR_STA     0x002FU
/*  getbits/showbits addresses are assigned to speed up commands decode=> they */
/*  should be modified in respect with bdu xif special (getbits/showbits) decoding logic */
/*  0x70U-0x7FU addresses are excluded according to xif address decode */
#define BDU_SHB1             0x0041U
#define BDU_SHB2             0x0042U
#define BDU_SHB3             0x0043U
#define BDU_SHB4             0x0044U
#define BDU_SHB5             0x0045U
#define BDU_SHB6             0x0046U
#define BDU_SHB7             0x0047U
#define BDU_SHB8             0x0048U
#define BDU_SHB9             0x0049U
#define BDU_SHB10            0x004AU
#define BDU_SHB11            0x004BU
#define BDU_SHB12            0x004CU
#define BDU_SHB13            0x004DU
#define BDU_SHB14            0x004EU
#define BDU_SHB15            0x004FU
#define BDU_SHB16            0x0050U
#define BDU_SHB17            0x0051U
#define BDU_SHB18            0x0052U
#define BDU_SHB19            0x0053U
#define BDU_SHB20            0x0054U
#define BDU_SHB21            0x0055U
#define BDU_SHB22            0x0056U
#define BDU_SHB23            0x0057U
#define BDU_SHB24            0x0058U
#define BDU_SHB25            0x0059U
#define BDU_SHB26            0x005AU
#define BDU_SHB27            0x005BU
#define BDU_SHB28            0x005CU
#define BDU_SHB29            0x005DU
#define BDU_SHB30            0x005EU
#define BDU_SHB31            0x005FU
#define BDU_SHB32            0x0060U
#define BDU_H264_CT_NCM1     0x008FU
#define BDU_H264_CT_NC0      0x0090U
#define BDU_H264_CT_NC1      0x0091U
#define BDU_H264_CT_NC2      0x0092U
#define BDU_H264_CT_NC3      0x0093U
#define BDU_H264_CT_NC4      0x0094U
#define BDU_H264_CT_NC5      0x0095U
#define BDU_H264_CT_NC6      0x0096U
#define BDU_H264_CT_NC7      0x0097U
#define BDU_H264_CT_NC8      0x0098U
#define BDU_H264_CT_NC9      0x0099U
#define BDU_H264_CT_NC10     0x009AU
#define BDU_H264_CT_NC11     0x009BU
#define BDU_H264_CT_NC12     0x009CU
#define BDU_H264_CT_NC13     0x009DU
#define BDU_H264_CT_NC14     0x009EU
#define BDU_H264_CT_NC15     0x009FU
#define BDU_H264_CT_NC16     0x00A0U
#define BDU_H264_TZ_TC1      0x00A1U
#define BDU_H264_TZ_TC2      0x00A2U
#define BDU_H264_TZ_TC3      0x00A3U
#define BDU_H264_TZ_TC4      0x00A4U
#define BDU_H264_TZ_TC5      0x00A5U
#define BDU_H264_TZ_TC6      0x00A6U
#define BDU_H264_TZ_TC7      0x00A7U
#define BDU_H264_TZ_TC8      0x00A8U
#define BDU_H264_TZ_TC9      0x00A9U
#define BDU_H264_TZ_TC10     0x00AAU
#define BDU_H264_TZ_TC11     0x00ABU
#define BDU_H264_TZ_TC12     0x00ACU
#define BDU_H264_TZ_TC13     0x00ADU
#define BDU_H264_TZ_TC14     0x00AEU
#define BDU_H264_TZ_TC15     0x00AFU
#define BDU_H264_RUN_BEF_ZL0 0x00B0U
#define BDU_H264_RUN_BEF_ZL1 0x00B1U
#define BDU_H264_RUN_BEF_ZL2 0x00B2U
#define BDU_H264_RUN_BEF_ZL3 0x00B3U
#define BDU_H264_RUN_BEF_ZL4 0x00B4U
#define BDU_H264_RUN_BEF_ZL5 0x00B5U
#define BDU_H264_RUN_BEF_ZL6 0x00B6U
#define BDU_H264_RUN_BEF_ZL7 0x00B7U
#define BDU_H264_RUN_BEF_ZL8 0x00B8U
#define BDU_H264_RUN_BEF_ZL9 0x00B9U
#define BDU_H264_RUN_BEF_ZL10 0x00BAU
#define BDU_H264_RUN_BEF_ZL11 0x00BBU
#define BDU_H264_RUN_BEF_ZL12 0x00BCU
#define BDU_H264_RUN_BEF_ZL13 0x00BDU
#define BDU_H264_RUN_BEF_ZL14 0x00BEU
#define BDU_H264_RUN_BEF_ZL15 0x00BFU
#define BDU_H264_RUN_BEF_ZL16 0x00C0U
#define BDU_JPG_PARAMS       0x00C1U
#define BDU_JPG_START_DECODE 0x00C2U
#define BDU_JPG_GET_DECODE_STATUS 0x00C3U
#define BDU_JPG_MBDC         0x00C4U
#define BDU_JPG_DCPRED_LU    0x00D7U
#define BDU_JPG_DCPRED_CB    0x00D8U
#define BDU_JPG_DCPRED_CR    0x00D9U
#define BDU_IPCM_START       0x00C5U
#define BDU_IPCM_GET_STATUS  0x00C6U
#define BDU_EXPGOL0          0x00C8U
#define BDU_EXPGOL1          0x00C9U
#define BDU_EXPGOL2          0x00CAU
#define BDU_EXPGOL3          0x00CBU
#define BDU_EXPGOL4          0x00CCU
#define BDU_CAVLC_TOTAL_COEFF 0x00D0U
#define BDU_CAVLC_TRAILING_ONES 0x00D1U
#define BDU_CAVLC_MODE       0x00D2U
#define BDU_CAVLC_STA        0x00D3U
#define BDU_CAVLC_START      0x00D4U
#define BDU_VC1_HDR_CFG      0x00CDU
#define BDU_VC1_HDR_MBQUANT  0x00CEU
#define BDU_VC1_HDR_TABLES   0x00CFU
#define BDU_VC1_HDR_QUANTOUT 0x00D5U
#define BDU_VC1_HDR_CBPCY    0x00D6U
#define BDU_VC1_HDR_MVDX1    0x00E6U
#define BDU_VC1_HDR_MVDX2    0x00E7U
#define BDU_VC1_HDR_MVDX3    0x00E8U
#define BDU_VC1_HDR_MVDX4    0x00E9U
#define BDU_VC1_HDR_MVDY1    0x00EAU
#define BDU_VC1_HDR_MVDY2    0x00EBU
#define BDU_VC1_HDR_MVDY3    0x00ECU
#define BDU_VC1_HDR_MVDY4    0x00EDU
#define BDU_VC1_HDR_MVFLAGS  0x00EEU
#define BDU_VC1_HDR_START    0x00DAU
#define BDU_VC1_HDR_GET_STATUS 0x00DBU
#define BDU_VC1_QUANT        0x00DCU
#define BDU_VC1_TABLES       0x00DDU
#define BDU_VC1_CSBP         0x00DEU
#define BDU_VC1_START_DECODE 0x00DFU
#define BDU_VC1_GET_DECODE_STATUS 0x00E0U
#define BDU_BTS_H            0x00E1U
#define BDU_BTS_L            0x00E2U
#define BDU_MOREDATA         0x00E3U
#define BDU_MP4_GET_MVD      0x00E5U
/*  should be modified in respect with bdu xif special decoding logic */
/*  bit 4,5 of address are used to select between MVD and RES */
#define BDU_H264_UEG_MVD     0x00EFU
#define BDU_H264_UEG_RES     0x00F0U
/* -------------------------------------------------------------------- */
/*  Bit Stream Packer Unit sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                mask    reset   access  xiordy    */
/*  Initialisation */
#define BPU_RST1             0x0000U
#define BPU_RST23456789      0x0001U
/*  DFI interface */
#define BPU_REG_DEF          0x0002U
#define BPU_REG_CMD          0x0003U
#define BPU_FIFO_CMD         0x0004U
#define BPU_SDIF_STATUS      0x0005U
/*  JPEG encode 00 byte stuffing mode configuration */
#define BPU_CFG              0x0006U
#define BPU_GTS              0x0007U
/*  bpu_flush_h264_new */
#define BPU_FLUSH_H264_NEW   0x0008U
/*  BPU_get_mb_size */
#define BPU_GET_MB_SIZE      0x0009U
#define BPU_GTB_SUM          0x000AU
/*  channel 1 command  */
#define BPU_GTB1             0x0010U
#define BPU_GTBh             0x0011U
#define BPU_WR1_EXG          0x001EU
#define BPU_WR1              0x001FU
#define BPU_FLU1             0x0030U
/*  channel 2 command  */
#define BPU_GTB2             0x0040U
#define BPU_MOV2             0x0041U
#define BPU_WR2_EXG          0x004EU
#define BPU_WR2              0x004FU
#define BPU_FLU2             0x0060U
/*  channel 3 command  */
#define BPU_GTB3             0x0070U
#define BPU_MOV3             0x0071U
#define BPU_WR3_EXG          0x007EU
#define BPU_WR3              0x007FU
#define BPU_FLU3             0x0090U
/*  channel 4 command  */
#define BPU_GTB4             0x00A0U
#define BPU_MOV4             0x00A1U
#define BPU_WR4_EXG          0x00AEU
#define BPU_WR4              0x00AFU
#define BPU_FLU4             0x00C0U
/*  channel 5 command  */
#define BPU_GTB5             0x00D0U
#define BPU_MOV5             0x00D1U
#define BPU_WR5_EXG          0x00DEU
#define BPU_WR5              0x00DFU
#define BPU_FLU5             0x00F0U
/*  channel 6 command  */
#define BPU_GTB6             0x0100U
#define BPU_MOV6             0x0101U
#define BPU_WR6_EXG          0x010EU
#define BPU_WR6              0x010FU
#define BPU_FLU6             0x0120U
/*  channel 7 command  */
#define BPU_GTB7             0x0130U
#define BPU_MOV7             0x0131U
#define BPU_WR7_EXG          0x013EU
#define BPU_WR7              0x013FU
#define BPU_FLU7             0x0150U
/*  channel 8 command  */
#define BPU_GTB8             0x0160U
#define BPU_MOV8             0x0161U
#define BPU_WR8_EXG          0x016EU
#define BPU_WR8              0x016FU
#define BPU_FLU8             0x0180U
/*  channel 9 command  */
#define BPU_GTB9             0x0190U
#define BPU_MOV9             0x0191U
#define BPU_WR9_EXG          0x019EU
#define BPU_WR9              0x019FU
#define BPU_FLU9             0x01B0U
/* -------------------------------------------------------------------- */
/*  Memory Transfer Fifos registers */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset   access  xiordy                                                                                                                   */
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
#define MTF_R_REG_CDE_STATIC 0x0018U
/* -------------------------------------------------------------------- */
/*  Interrupt Controller sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                   mask    reset   access  xiordy */
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
/*                                                       mask    reset   access  xiordy */
#define CKG_RST_L            0x0000U
#define CKG_RST_H            0x0001U
/*  must be tested separately due to side effects */
#define CKG_CKEN_L           0x0002U
/*  must be tested separately due to side effects */
#define CKG_CKEN_H           0x0003U
/*  will be defined separately */
#define CKG_IDN_HRV_L        0x0004U
/*  will be defined separately */
#define CKG_IDN_HRV_H        0x0005U
/* --------------------------------------------------------------------                         */
/* PXP UNIT address map */
/* -------------------------------------------------------------------- */
/*                                                       mask    reset   access  xiordy                               */
/* REG     UNIT_REG_SOFT_RESET     0x00U    1 */
/* REG     UNIT_REG_DEF            0x01U    1 */
/* REG     UNIT_REG_CFG            0x02U    1 */
/* REG     UNIT_REG_CMD            0x03U    1 */
/* REG     UNIT_FIFO_CMD           0x04U    1 */
/* REG     UNIT_FIFO_IN            0x05U    1 */
/* REG     UNIT_FIFO_OUT           0x06U    1 */
/* REG     UNIT_REG_CFG2           0x07U    1 */
/* REG     UNIT_REG_CFG3           0x08U    1 */
/* REG     UNIT_FIFO_CMD2          0x09U    1 */
/* REG     UNIT_REG_CMD2           0x0aU    1 */
/* -------------------------------------------------------------------- */
/*  H264 CAVLC Unit H264RLC sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                           mask    reset   access  xiordy */
#define H264RLC_REG_SOFT_RESET 0x0000U
#define H264RLC_REG_DEF      0x0001U
#define H264RLC_REG_CFG      0x0002U
#define H264RLC_REG_CMD      0x0003U
#define H264RLC_FIFO_CMD     0x0004U
/* -------------------------------------------------------------------- */
/*  Discrete Cosine Transform Unit DCT sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset   access  xiordy */
#define DCT_REG_SOFT_RESET   0x0000U
#define DCT_REG_CFG          0x0002U
/* -------------------------------------------------------------------- */
/*  Inverse Discrete Cosine Transform Unit IDCT sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                       mask    reset   access  xiordy */
#define IDCT_REG_SOFT_RESET  0x0000U
#define IDCT_REG_CFG         0x0002U
/* -------------------------------------------------------------------- */
/*  Basic Quantizer Unit Q sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset  access  xiordy */
#define Q_REG_SOFT_RESET     0x0000U
#define Q_REG_DEF            0x0001U
#define Q_REG_CFG            0x0002U
#define Q_REG_CMD            0x0003U
#define Q_FIFO_CMD           0x0004U
#define Q_REG_CFG2           0x0007U
/* -------------------------------------------------------------------- */
/*  Basic Inverse Quantizer Unit IQ sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset  access  xiordy */
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
/*                                                               mask    reset  access  xiordy */
#define SCN_REG_SOFT_RESET   0x0000U
#define SCN_REG_DEF          0x0001U
#define SCN_REG_CFG          0x0002U
#define SCN_REG_CMD          0x0003U
#define SCN_FIFO_CMD         0x0004U
#define SCN_FIFO_OUT         0x0006U
/* -------------------------------------------------------------------- */
/*  Run Level Coder Unit RLC sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset  access  xiordy */
#define RLC_REG_SOFT_RESET   0x0000U
#define RLC_REG_DEF          0x0001U
#define RLC_REG_CMD          0x0003U
#define RLC_FIFO_CMD         0x0004U
/* -------------------------------------------------------------------- */
/*  Inverse ACDC Predictor Unit IPRD sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                           mask    reset   access  xiordy */
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
/*                                                                       mask    reset   access  xiordy */
#define QTAB_REG_SOFT_RESET  0x0000U
#define QTAB_REG_DEF         0x0001U
#define QTAB_REG_CMD         0x0003U
#define QTAB_FIFO_CMD        0x0004U
#define QTAB_FIFO_IN         0x0005U
#define QTAB_FIFO_OUT        0x0006U
/* -------------------------------------------------------------------- */
/*  Fifo Unit FIFO sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                       mask    reset   access  xiordy */
#define FIFO_REG_SOFT_RESET  0x0000U
/* -------------------------------------------------------------------- */
/*  VC1 Inverse Transform Unit VC1IT sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                       mask    reset   access  xiordy */
#define VC1IT_REG_SOFT_RESET 0x0000U
#define VC1IT_REG_DEF        0x0001U
/* REG     VC1IT_REG_CFG           VC1IT_BASE + 0x01U   0       0x0U     0       RWH */
#define VC1IT_REG_CMD        0x0003U
#define VC1IT_FIFO_CMD       0x0004U
/* -------------------------------------------------------------------- */
/*  VC1 Buffer Unit VBF sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                     mask    reset   access  xiordy */
#define VBF_REG_SOFT_RESET   0x0000U
#define VBF_REG_CFG          0x0001U
#define VBF_FIFO_GET_CMD     0x0002U
#define VBF_FIFO_SEND_CMD    0x0003U
#define VBF_REG_STS_EMPTY    0x0004U
#define VBF_REG_STS_FULL     0x0005U
#define VBF_REG_STS_ERROR    0x0006U
#define VBF_FIFO_OVSM_CMD    0x0007U
#define VBF_REG_STS_OVSM     0x0008U
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
#define VBF_REG_TOP_0        0x0340U
#define VBF_REG_TOP_1        0x0350U
#define VBF_REG_TOP_2        0x0360U
#define VBF_REG_TOP_3        0x0370U
#define VBF_REG_TOP_4        0x0380U
#define VBF_REG_TOP_5        0x0390U
#define VBF_REG_TOP_6        0x03A0U
#define VBF_REG_TOP_7        0x03B0U
/* -------------------------------------------------------------------- */
/*  CUP sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                   mask    reset   access  xiordy */
#define CUP_REG_RST          0x07C0U
#define CUP_REG_CFG          0x07C1U
#define CUP_FIF_CMD          0x07C2U
#define CUP_REG_STS          0x07C3U
#define CUP_REG_TLID         0x07C4U
#define CUP_REG_MBA          0x07C5U
#define CUP_REG_SFW          0x07C6U
#define CUP_REG_SFH          0x07C7U
#define CUP_REG_MBX          0x07C8U
#define CUP_REG_MBY          0x07C9U
#define CUP_REG_SPP          0x07CAU
#define CUP_REG_SHP          0x07CBU
#define CUP_REG_ICP          0x07CCU
#define CUP_REG_IPN          0x07CDU
#define CUP_REG_CRP          0x07CDU
#define CUP_REG_SCI          0x07CEU
#define CUP_REG_EXC1         0x07CFU
#define CUP_REG_PIC          0x07D0U
#define CUP_FIF_IPAM         0x07D1U
#define CUP_FIF_PMPX         0x07D2U
#define CUP_FIF_PMPY         0x07D3U
#define CUP_FIF_PSIZ         0x07D4U
#define CUP_FIF_PDST         0x07D5U
#define CUP_FIF_PREF         0x07D6U
#define CUP_FIF_IPRD         0x07D7U
#define CUP_REG_MBP          0x07D8U
#define CUP_REG_HYB          0x07D9U
#define CUP_REG_BLK          0x07DAU
#define CUP_REG_SCLFACT      0x07DBU
#define CUP_REG_COLMVX       0x07DCU
#define CUP_REG_COLMVY       0x07DDU
#define CUP_REG_CBP          0x07DEU
#define CUP_FIF_RLC          0x07DFU
#define CUP_REG_RIP          0x07E0U
#define CUP_REG_RIP0         0x07E0U
#define CUP_REG_RIP1         0x07E1U
#define CUP_REG_RIP2         0x07E2U
#define CUP_REG_RIP3         0x07E3U
#define CUP_REG_RIP4         0x07E4U
#define CUP_REG_RIP5         0x07E5U
#define CUP_REG_RIP6         0x07E6U
#define CUP_REG_RIP7         0x07E7U
#define CUP_REG_RIP8         0x07E8U
#define CUP_REG_RIP9         0x07E9U
#define CUP_REG_RIP10        0x07EAU
#define CUP_REG_RIP11        0x07EBU
#define CUP_REG_RIP12        0x07ECU
#define CUP_REG_RIP13        0x07EDU
#define CUP_REG_RIP14        0x07EEU
#define CUP_REG_RIP15        0x07EFU
#define CUP_REG_MVD          0x07F0U
#define CUP_REG_MVD0         0x07F0U
#define CUP_REG_MVD1         0x07F1U
#define CUP_REG_MVD2         0x07F2U
#define CUP_REG_MVD3         0x07F3U
#define CUP_REG_MVD4         0x07F4U
#define CUP_REG_MVD5         0x07F5U
#define CUP_REG_MVD6         0x07F6U
#define CUP_REG_MVD7         0x07F7U
#define CUP_REG_MVD8         0x07F8U
#define CUP_REG_MVD9         0x07F9U
#define CUP_REG_MVD10        0x07FAU
#define CUP_REG_MVD11        0x07FBU
#define CUP_REG_MVD12        0x07FCU
#define CUP_REG_MVD13        0x07FDU
#define CUP_REG_MVD14        0x07FEU
#define CUP_REG_MVD15        0x07FFU
#define CUP_REG_MVDX         0x07F0U
#define CUP_REG_MVDX0        0x07F0U
#define CUP_REG_MVDX1        0x07F1U
#define CUP_REG_MVDX2        0x07F2U
#define CUP_REG_MVDX3        0x07F3U
#define CUP_REG_MVDY         0x07F4U
#define CUP_REG_MVDY0        0x07F4U
#define CUP_REG_MVDY1        0x07F5U
#define CUP_REG_MVDY2        0x07F6U
#define CUP_REG_MVDY3        0x07F7U
/* Used for H264 Encode stabilization */
/* Re-mapping of CUP_REG_RIP0 and CUP_REG_RIP1 which are unused in encode */
#define CUP_REG_SOX        0x07E0U 
#define CUP_REG_SOY        0x07E1U
#define CUP_REG_MVSCALE0     0x07F8U
#define CUP_REG_MVSCALE1     0x07F9U
#define CUP_REG_MVSCALE2     0x07FAU
#define CUP_REG_MVSCALE3     0x07FBU
/*  CUP Context sub-addresses are relative to context base address: */
/*  CUP_CTX_X, CUP_CTX_A, CUP_CTX_B, CUP_CTX_C, CUP_CTX_D, CUP_CTX_E */
/*  relative addressing with CFG_BASE=0 ; real addresses are: */
/*  CUP_CTX_<context>_<var>[i] = CUP_BASE + CUP_CTX_<context> + CUP_CTX_<var> + step*i */
/*  All address refers to SRAM content, so access is RWH and reset is not possible */
/*  VC1 context definition */
/*       VC1 context definition                              mask    reset   access xiordy */
#define CUP_CTX_VBLK         0x0000U
#define CUP_CTX_VMBTYP       0x0006U
#define CUP_CTX_VFREE        0x0007U
#define CUP_CTX_VMVX         0x0008U
#define CUP_CTX_VMVY         0x000AU
#define CUP_CTX_VMVBX        0x0020U
#define CUP_CTX_VMVBY        0x0022U
#define CUP_CTX_DC_QUANT     0x0024U
#define CUP_CTX_AC_QUANT     0x0025U
#define CUP_CTX_VFREE2       0x0026U
#define CUP_CTX_C_Y2         0x0028U
#define CUP_CTX_C_Y3         0x0038U
#define CUP_CTX_C_CB         0x0048U
#define CUP_CTX_C_CR         0x0058U
/*  H264 decode context definition */
#define CUP_CTX_MBTYP        0x0000U
#define CUP_CTX_QPY          0x0001U
#define CUP_CTX_QPCB         0x0002U
#define CUP_CTX_QPCR         0x0003U
#define CUP_CTX_NSLIC        0x0004U
#define CUP_CTX_USER         0x0006U
#define CUP_CTX_ERROR        0x0007U
#define CUP_CTX_I4X4         0x0008U
#define CUP_CTX_RFPIC_L0     0x0010U
#define CUP_CTX_RFPIC_L1     0x0014U
#define CUP_CTX_RFIDX_L0     0x0018U
#define CUP_CTX_RFIDX_L1     0x001CU
#define CUP_CTX_NZC          0x0020U
#define CUP_CTX_SUB          0x0038U
#define CUP_CTX_SHP          0x003CU
#define CUP_CTX_MVX_L0       0x0040U
#define CUP_CTX_MVY_L0       0x0042U
#define CUP_CTX_MVX_L1       0x0080U
#define CUP_CTX_MVY_L1       0x0082U
/*  H264 encode context definition */
#define CUP_E_CTX_MBTYPE     0x0000U
#define CUP_E_CTX_QPY        0x0001U
#define CUP_E_CTX_QPC        0x0002U
#define CUP_E_CTX_NSLICE     0x0004U
#define CUP_E_CTX_NZC        0x0008U
#define CUP_E_CTX_MVX        0x0020U
#define CUP_E_CTX_MVY        0x0022U
#define CUP_E_CTX_I4X4       0x0030U
/* -------------------------------------------------------------------- */
/*  VLC sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                     mask    reset   access  xiordy */
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
/*                                                                       mask    reset   access  xiordy */
#define FIFO2_REG_SOFT_RESET 0x0000U
/* -------------------------------------------------------------------- */
/*  Post adder Unit PA sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset   access  xiordy */
#define PA_REG_SOFT_RESET    0x0000U
#define PA_REG_DEF           0x0001U
#define PA_REG_CMD           0x0003U
#define PA_FIFO_CMD          0x0004U
/* -------------------------------------------------------------------- */
/*  Reconstruction Unit REC sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                               mask    reset   access  xiordy */
#define REC_REG_SOFT_RESET   0x0000U
#define REC_REG_DEF          0x0001U
#define REC_REG_CMD          0x0003U
#define REC_FIFO_CMD         0x0004U
/* -------------------------------------------------------------------- */
/*  Video Post-Processing Unit sub-addresses */
/* -------------------------------------------------------------------- */
/*                                                                       mask    reset   access  xiordy                                                                         */
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
#define IPA_REG_CFG          0x0001U
#define IPA_REG_DIR          0x0002U
#define IPA_REG_STA          0x0003U
#define IPA_REG_CHR          0x0004U
#define IPA_REG_MBX_MAX      0x0005U
#define IPA_REG_MBY_MAX      0x0006U
#define IPA_REG_PREFETCH_EOR 0x0007U
#define IPA_FIFO_MBA         0x0010U
#define IPA_FIFO_MBX         0x0011U
#define IPA_FIFO_MBY         0x0012U
#define IPA_FIFO_MB_TYPE     0x0013U
#define IPA_FIFO_MODE        0x0014U
/* -------------------------------------------------------------------- */
/*  IME sub-addresses */
/* -------------------------------------------------------------------- */
/*        REG_NAME    - BASE_ADD + OFFSET - BW -- BW_MASK - INIT_VAL - MODE   */
#define IME_REG_RST          0x0000U
#define IME_REG_CFG          0x0001U
#define IME_REG_SFW          0x0002U
#define IME_REG_SFH          0x0003U
#define IME_REG_SOX          0x0004U
#define IME_REG_SOY          0x0005U
#define IME_REG_MBX          0x0006U
#define IME_REG_MBY          0x0007U
#define IME_REG_CMD          0x0008U
#define IME_REG_IE_DLEFT     0x0009U
#define IME_REG_IE_DTOP      0x000AU
#define IME_REG_ME_QUANT     0x000BU
#define IME_REG_ME_PVX       0x000CU
#define IME_REG_ME_PVY       0x000DU
#define IME_REG_ME_CLB       0x000EU
#define IME_REG_STS          0x000FU
#define IME_REG_IT_EOT       0x0010U
#define IME_REG_RCC          0x0011U
#define IME_REG_LM_SUM       0x0012U
#define IME_REG_LM_MAD       0x0013U
#define IME_REG_IE_DIR       0x0014U
#define IME_REG_IE_SAD       0x0015U
#define IME_REG_ME_MVX       0x0016U
#define IME_REG_ME_MVY       0x0017U
#define IME_REG_ME_SAD       0x0018U
#define IME_REG_L4x4_MAD     0x0019U
#define IME_REG_L8x8_MAD     0x001AU
#define IME_REG_I4x4_DY1     0x001BU
#define IME_REG_I4x4_DY2     0x001CU
#define IME_REG_I4x4_DY3     0x001DU
#define IME_REG_I4x4_DY4     0x001EU
#define IME_REG_I4x4_SAD     0x001FU
#define IME_REG_I8x8_DIR     0x0020U
#define IME_REG_I8x8_SAD     0x0021U
#define IME_REG_M16x8_V1X    0x0022U
#define IME_REG_M16x8_V1Y    0x0023U
#define IME_REG_M16x8_V2X    0x0024U
#define IME_REG_M16x8_V2Y    0x0025U
#define IME_REG_M16x8_SAD    0x0026U
#define IME_REG_M8x16_V1X    0x0027U
#define IME_REG_M8x16_V1Y    0x0028U
#define IME_REG_M8x16_V2X    0x0029U
#define IME_REG_M8x16_V2Y    0x002AU
#define IME_REG_M8x16_SAD    0x002BU
#define IME_REG_M8x8_V1X     0x002CU
#define IME_REG_M8x8_V1Y     0x002DU
#define IME_REG_M8x8_V2X     0x002EU
#define IME_REG_M8x8_V2Y     0x002FU
#define IME_REG_M8x8_V3X     0x0030U
#define IME_REG_M8x8_V3Y     0x0031U
#define IME_REG_M8x8_V4X     0x0032U
#define IME_REG_M8x8_V4Y     0x0033U
#define IME_REG_M8x8_SAD     0x0034U
/* -------------------------------------------------------------------- */
/*  H264 FITQ sub-addresses */
/* -------------------------------------------------------------------- */
#define H264FITQ_REG_RST     0x0000U
#define H264FITQ_REG_CFG     0x0001U
#define H264FITQ_FIFO_CMD    0x0002U
#define H264FITQ_FIFO_QPY    0x0003U
#define H264FITQ_FIFO_QPCB   0x0004U
#define H264FITQ_FIFO_QPCR   0x0005U
#define H264FITQ_REG_STS     0x0006U
/*  H264FITQ scaling list matrix base addresses */
#define H264FITQ_SLM_BASE    0x0008U
#define H264FITQ_SLM_I_Y4X4  0x0008U
#define H264FITQ_SLM_I_U4X4  0x0018U
#define H264FITQ_SLM_I_V4X4  0x0028U
#define H264FITQ_SLM_P_Y4X4  0x0038U
#define H264FITQ_SLM_P_U4X4  0x0048U
#define H264FITQ_SLM_P_V4X4  0x0058U
#define H264FITQ_SLM_I_Y8X8  0x0068U
#define H264FITQ_SLM_P_Y8X8  0x00A8U
/* -------------------------------------------------------------------- */
/*  HED sub-addresses */
/* ----- */
#define PP_READ_START_L      0x0000U
#define PP_READ_START_H      0x0001U
#define PP_READ_STOP_L       0x0002U
#define PP_READ_STOP_H       0x0003U
#define PP_BBG_L             0x0004U
#define PP_BBG_H             0x0005U
#define PP_BBS_L             0x0006U
#define PP_BBS_H             0x0007U
#define PP_ISBG_L            0x0008U
#define PP_ISBG_H            0x0009U
#define PP_IPBG_L            0x000AU
#define PP_IPBG_H            0x000BU
#define PP_IPBS_L            0x000CU
#define PP_IPBS_H            0x000DU
#define PP_WDL_L             0x000EU
#define PP_WDL_H             0x000FU
#define PP_CFG_L             0x0010U
#define PP_CFG_H             0x0011U
#define PP_PICWIDTH_L        0x0012U
#define PP_PICWIDTH_H        0x0013U
#define PP_CODELENGTH_L      0x0014U
#define PP_CODELENGTH_H      0x0015U
#define PP_START_L           0x0016U
#define PP_MAX_OPC_SIZE_L    0x0018U
#define PP_MAX_CHUNK_SIZE_L  0x001AU
#define PP_MAX_MESSAGE_SIZE_L 0x001CU
#define PP_ITS_L             0x001EU
#define PP_ITM_L             0x0020U
#define PP_SRS_L             0x0022U
#define PP_DFV_OUTCTRL_L     0x0024U
#define PP_READ_BW_LIMITER_L 0x0026U
#define PP_WRITE_BW_LIMITER_L 0x0028U
#define PP_IBCG_L            0x002AU
#define PP_IBCG_H            0x002BU
#define PP_IBCS_L            0x002CU
#define PP_IBCS_H            0x002DU
#define PP_CHKSYN_DIS_L      0x0030U
#define PP_CHKSYN_DIS_H      0x0031U
/* --------------------------------------------------------------------- */
/*  bridge register */
/* --------------------------------------------------------------------- */
#define PP_STATUS_L          0x0032U
#define PP_STATUS_H          0x0033U
/* ------------------------------------------------------------ */


#endif	/* T1XHV_HAMAC_XBUS_REGS_H */

