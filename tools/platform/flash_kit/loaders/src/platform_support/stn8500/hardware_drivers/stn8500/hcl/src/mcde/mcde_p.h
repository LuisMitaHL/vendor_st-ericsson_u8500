
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE Private File
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "hcl_defs.h"
#include "mcde.h"


/*******************************************************************
Peripheral IDs(yet to do)
*******************************************************************/
#define MCDE_PID_MAJOR_VERSION  =0x02
#define MCDE_PID_MINOR_VERSION  =0x00
#define MCDE_PID_DEV_VERSION    =0x07
#define MCDE_PID_METALFIX_VERSION =0x00
/*******************************************************************
   MCDE Control Register Fields
********************************************************************/
#define MCDE_SET_BIT  0x1
#define MCDE_CLEAR_BIT 0x0

#define MCDE_CTRL_MCDEEN_MASK   MASK_BIT31
#define MCDE_CTRL_DPIA_EN_MASK  MASK_BIT9
#define MCDE_CTRL_DPIB_EN_MASK  MASK_BIT8
#define MCDE_CTRL_DBIC0_EN_MASK MASK_BIT7
#define MCDE_CTRL_DBIC1_EN_MASK MASK_BIT6
#define MCDE_CTRL_FABMUX_MASK   MASK_BIT17
#define MCDE_CTRL_F01MUX_MASK   MASK_BIT16
#define MCDE_DSIVID0_EN_MASK    MASK_BIT5
#define MCDE_DSIVID1_EN_MASK    MASK_BIT4
#define MCDE_DSIVID2_EN_MASK    MASK_BIT3
#define MCDE_DSICMD2_EN_MASK    MASK_BIT0
#define MCDE_DSICMD0_EN_MASK    MASK_BIT2
#define MCDE_DSICMD1_EN_MASK    MASK_BIT1
#define MCDE_CTRL_OUTMUX4_MASK  (MASK_BIT28 | MASK_BIT29 | MASK_BIT30)
#define MCDE_CTRL_OUTMUX3_MASK  (MASK_BIT25 | MASK_BIT26 | MASK_BIT27)
#define MCDE_CTRL_OUTMUX2_MASK  (MASK_BIT22 | MASK_BIT23 | MASK_BIT24)
#define MCDE_CTRL_OUTMUX1_MASK  (MASK_BIT19 | MASK_BIT20 | MASK_BIT21)
#define MCDE_CTRL_OUTMUX0_MASK  (MASK_BIT16 | MASK_BIT17 | MASK_BIT18)
#define MCDE_CTRL_FSYNCTRLB_MASK MASK_BIT11
#define MCDE_CTRL_FSYNCTRLA_MASK MASK_BIT10
#define MCDE_CTRL_CTRLB_MASK     MASK_BIT9
#define MCDE_CTRL_CTRLA_MASK     MASK_BIT8
#define MCDE_CTRL_SSPENABLE_MASK MASK_BIT11
#define MCDE_CTRL_SYNCMUX3_MASK  MASK_BIT6
#define MCDE_CTRL_SYNCMUX2_MASK  MASK_BIT5
#define MCDE_CTRL_SYNCMUX1_MASK  MASK_BIT4
#define MCDE_CTRL_SYNCMUX0_MASK  MASK_BIT3
#define MCDE_CTRL_CDICTRL_MASK   MASK_BIT2
#define MCDE_CTRL_CDIEN_MASK     MASK_BIT1
#define MCDE_CTRL_CDIDELAYD0_MASK (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define MCDE_CTRL_CDIDELAYD1_MASK (MASK_BIT3 | MASK_BIT4 | MASK_BIT5)
#define MCDE_CTRL_CDIDELAYD2_MASK (MASK_BIT6 | MASK_BIT7 | MASK_BIT8)
#define MCDE_CTRL_CDIDELAYCLK_MASK (MASK_BIT9 | MASK_BIT10 | MASK_BIT11)
#define MCDE_CTRL_CDILS_MASK       (MASK_BIT12 | MASK_BIT13)
#define MCDE_CTRL_CDIPHIASEL_MASK  (MASK_BIT14)
#define MCDE_TVA_DPIC0_LCDB_MASK   0x06
#define MCDE_TVB_DPIC1_LCDA_MASK   0xD4
#define MCDE_DPIC1_LCDA_MASK       0xF8
#define MCDE_DPIC0_LCDB_MASK       0x07
#define MCDE_LCDA_LCDB_MASK        0x00
#define MCDE_SYNCMUX_MASK          0xFF
#define MCDE_IFIFO_WATERMARK_MASK  0x7000

#define MCDE_IFIFO_WATERMARK_SHIFT  12
#define MCDE_CTRL_MCDEEN_SHIFT   31
#define MCDE_CTRL_DPIA_EN_SHIFT  9
#define MCDE_CTRL_DPIB_EN_SHIFT  8
#define MCDE_CTRL_DBIC0_EN_SHIFT 7
#define MCDE_CTRL_DBIC1_EN_SHIFT 6
#define MCDE_CTRL_FABMUX_SHIFT   17
#define MCDE_CTRL_F01MUX_SHIFT   16
#define MCDE_DSIVID0_EN_SHIFT    5
#define MCDE_DSIVID1_EN_SHIFT    4
#define MCDE_DSIVID2_EN_SHIFT    3
#define MCDE_DSICMD0_EN_SHIFT    2
#define MCDE_DSICMD1_EN_SHIFT    1
#define MCDE_CTRL_OUTMUX4_SHIFT  28
#define MCDE_CTRL_OUTMUX3_SHIFT  25
#define MCDE_CTRL_OUTMUX2_SHIFT  22
#define MCDE_CTRL_OUTMUX1_SHIFT  19
#define MCDE_CTRL_OUTMUX0_SHIFT  16
#define MCDE_CTRL_FSYNCTRLB_SHIFT  11
#define MCDE_CTRL_FSYNCTRLA_SHIFT 10
#define MCDE_CTRL_CTRLB_SHIFT    9
#define MCDE_CTRL_CTRLA_SHIFT    8
#define MCDE_CTRL_SSPENABLE_SHIFT 11
#define MCDE_CTRL_SYNCMUX3_SHIFT 6
#define MCDE_CTRL_SYNCMUX2_SHIFT 5
#define MCDE_CTRL_SYNCMUX1_SHIFT 4
#define MCDE_CTRL_SYNCMUX0_SHIFT 3
#define MCDE_CTRL_CDICTRL_SHIFT  2   
#define MCDE_CTRL_CDIEN_SHIFT    1
#define MCDE_CTRL_CDIDELAYD1_SHIFT 3
#define MCDE_CTRL_CDIDELAYD2_SHIFT 6
#define MCDE_CTRL_CDIDELAYCLK_SHIFT 9
#define MCDE_CTRL_CDILS_SHIFT  12
#define MCDE_CTRL_CDIPHIASEL_SHIFT 14


/*******************************************************************
   MCDE External Source Register Fields
********************************************************************/
#define MCDE_EXT_BUFFER_MASK      /*(MASK_HALFWORD1 | MASK_BYTE1 | MASK_QUARTET1 | MASK_BIT3)*/0xFFFFFFFF
#define MCDE_EXT_PRI_OVR_MASK     MASK_QUARTET1
#define MCDE_EXT_BUFFER_NUM_MASK  (MASK_BIT2 | MASK_BIT3)
#define MCDE_EXT_BUFFER_ID_MASK   (MASK_BIT0 | MASK_BIT1)
#define MCDE_EXT_FORCEFSDIV_MASK   MASK_BIT4
#define MCDE_EXT_FSDISABLE_MASK    MASK_BIT3
#define MCDE_EXT_OVR_CTRL_MASK     MASK_BIT2
#define MCDE_EXT_BUF_MODE_MASK     (MASK_BIT0 | MASK_BIT1)
#define MCDE_EXT_BEPO_MASK        MASK_BIT14
#define MCDE_EXT_BEBO_MASK        MASK_BIT13
#define MCDE_EXT_BGR_MASK         MASK_BIT12
#define MCDE_EXT_BPP_MASK         (MASK_BIT11 | MASK_BIT10 | MASK_BIT9 | MASK_BIT8)


#define MCDE_EXT_BUFFER_SHIFT    0
#define MCDE_EXT_PRI_OVR_SHIFT   SHIFT_QUARTET1
#define MCDE_EXT_BUFFER_NUM_SHIFT 2
#define MCDE_EXT_OVR_CTRL_SHIFT   2
#define MCDE_EXT_FORCEFSDIV_SHIFT 4
#define MCDE_EXT_FSDISABLE_SHIFT  3
#define MCDE_EXT_BEPO_SHIFT       14
#define MCDE_EXT_BEBO_SHIFT       13
#define MCDE_EXT_BGR_SHIFT        12
#define MCDE_EXT_BPP_SHIFT        8   
/*******************************************************************
   MCDE Overlay Register Fields
********************************************************************/
#define MCDE_OVR_OVLEN_MASK        MASK_BIT0
#define MCDE_OVR_COLCTRL_MASK     (MASK_BIT1 | MASK_BIT2)
#define MCDE_OVR_PALCTRL_MASK     (MASK_BIT3 | MASK_BIT4)
#ifdef ST_8500ED
#define MCDE_OVR_CKEYEN_MASK      (MASK_BIT5)
#define MCDE_OVR_ALPHAPMEN_MASK    MASK_BIT6
#else
#define MCDE_OVR_CKEYEN_MASK      (MASK_BIT3)
#define MCDE_OVR_ALPHAPMEN_MASK    MASK_BIT4
#endif
#define MCDE_OVR_CLIPEN_MASK      MASK_BIT7
#define MCDE_OVR_STBPRIO_MASK     MASK_QUARTET4
#define MCDE_OVR_BURSTSZ_MASK     MASK_QUARTET5
#define MCDE_OVR_MAXREQ_MASK      MASK_QUARTET6
#define MCDE_OVR_ROTBURSTSIZE_MASK MASK_QUARTET7
#define MCDE_OVR_BLEND_MASK       MASK_BIT0
#define MCDE_OVR_OPQ_MASK         MASK_BIT9
#define MCDE_OVR_INTERMDE_MASK    MASK_BIT29
#define MCDE_OVR_INTERON_MASK     MASK_BIT28
#define MCDE_OVR_LPF_MASK        (MASK_BYTE2 | MASK_BIT24 |MASK_BIT25 | MASK_BIT26)
#define MCDE_OVR_PPL_MASK         (MASK_BYTE0 | MASK_BIT8 |MASK_BIT9 |MASK_BIT10)
#define MCDE_ALPHAVALUE_MASK      (MASK_BYTE0 << 1)
#define MCDE_EXT_SRCID_MASK       (MASK_BIT14 | MASK_BIT13 | MASK_BIT12 | MASK_BIT11)
#define MCDE_PIXOFF_MASK          (MASK_BIT10 | MASK_BIT11 |MASK_QUARTET3)
#define MCDE_OVR_ZLEVEL_MASK      (MASK_BIT30 | MASK_BIT27 | MASK_BIT28 | MASK_BIT29)
#define MCDE_OVR_YPOS_MASK        (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_OVR_CHID_MASK        (MASK_BIT14 | MASK_BIT13 | MASK_BIT12 | MASK_BIT11)
#define MCDE_OVR_XPOS_MASK        (MASK_BYTE0 | MASK_BIT8 |MASK_BIT9 |MASK_BIT10)
#define MCDE_OVR_BLOCKED_MASK     MASK_BIT10
#define MCDE_OVR_READ_MASK        MASK_BIT9
#define MCDE_OVR_FETCH_MASK       MASK_BIT8
#define MCDE_WATERMARK_MASK      (MASK_BYTE2 | MASK_QUARTET6 | MASK_BIT28)
#define MCDE_LINEINCREMENT_MASK   0xFFFFFFFF
#define MCDE_YCLIP_MASK           0x3FFFFF
#define MCDE_XCLIP_MASK           0xFFC00000
#define MCDE_XBRCOOR_MASK         0x7FF
#define MCDE_YBRCOOR_MASK         0x07FF0000

#define MCDE_OVR_COLCTRL_SHIFT   1
#define MCDE_OVR_PALCTRL_SHIFT   3
#ifdef ST_8500ED
#define MCDE_OVR_CKEYEN_SHIFT    5
#define MCDE_OVR_ALPHAPMEN_SHIFT 6
#else
#define MCDE_OVR_CKEYEN_SHIFT    3
#define MCDE_OVR_ALPHAPMEN_SHIFT 4
#endif
#define MCDE_OVR_CLIPEN_SHIFT    7
#define MCDE_OVR_STBPRIO_SHIFT   SHIFT_QUARTET4
#define MCDE_OVR_BURSTSZ_SHIFT   SHIFT_QUARTET5
#define MCDE_OVR_MAXREQ_SHIFT     SHIFT_QUARTET6
#define MCDE_OVR_ROTBURSTSIZE_SHIFT SHIFT_QUARTET7
#define MCDE_OVR_OPQ_SHIFT        9
#define MCDE_OVR_INTERMDE_SHIFT   29
#define MCDE_OVR_INTERON_SHIFT    28
#define MCDE_OVR_LPF_SHIFT        SHIFT_HALFWORD1
#define MCDE_ALPHAVALUE_SHIFT     1
#define MCDE_EXT_SRCID_SHIFT      11
#define MCDE_OVR_ZLEVEL_SHIFT     27
#define MCDE_OVR_YPOS_SHIFT       SHIFT_HALFWORD1
#define MCDE_OVR_CHID_SHIFT       11
#define MCDE_OVR_BLOCKED_SHIFT    10
#define MCDE_OVR_READ_SHIFT       9
#define MCDE_OVR_FETCH_SHIFT      8
#define MCDE_WATERMARK_SHIFT      SHIFT_HALFWORD1
#define MCDE_PIXOFF_SHIFT         10
#define MCDE_LINEINCREMENT_SHIFT  0
#define MCDE_YCLIP_SHIFT          0
#define MCDE_XCLIP_SHIFT          22
#define MCDE_YBRCOOR_SHIFT        16


/*******************************************************************
   MCDE Channel Configuration Register Fields
********************************************************************/
#define MCDE_INITDELAY_MASK      MASK_HALFWORD1
#define MCDE_PPDELAY_MASK        MASK_HALFWORD0
#define MCDE_SWINTVCNT_MASK      (MASK_BYTE3 | MASK_QUARTET5 | MASK_BIT18 |MASK_BIT19)
#define MCDE_SWINTVEVENT_MASK    (MASK_BIT16 | MASK_BIT17)
#define MCDE_HWREQVCNT_MASK      (MASK_BYTE1 | MASK_QUARTET1 | MASK_BIT3 | MASK_BIT2)
#define MCDE_HWREQVEVENT_MASK    (MASK_BIT0 | MASK_BIT1)
#define MCDE_OUTINTERFACE_MASK   (MASK_BIT4 | MASK_BIT3 |MASK_BIT2)
#define MCDE_SRCSYNCH_MASK       (MASK_BIT0 | MASK_BIT1)
#define MCDE_SW_TRIG_MASK        MASK_BIT0
#define MCDE_REDCOLOR_MASK       MASK_BYTE2
#define MCDE_GREENCOLOR_MASK     MASK_BYTE1
#define MCDE_BLUECOLOR_MASK      MASK_BYTE0
#define MCDE_CHPRIORITY_MASK     MASK_QUARTET0
#define MCDE_CHXLPF_MASK         (0x07FF0000)
#define MCDE_CHXPPL_MASK         (MASK_BYTE0 | MASK_BIT8 |MASK_BIT9 |MASK_BIT10)
#define MCDE_CHX_ABORT_MASK       MASK_BIT1
#define MCDE_CHX_READ_MASK        MASK_BIT0

#define MCDE_INITDELAY_SHIFT     SHIFT_HALFWORD1
#define MCDE_SWINTVCNT_SHIFT     18
#define MCDE_SWINTVEVENT_SHIFT   SHIFT_HALFWORD1
#define MCDE_HWREQVCNT_SHIFT     2
#define MCDE_OUTINTERFACE_SHIFT  2
#define MCDE_REDCOLOR_SHIFT      SHIFT_HALFWORD1
#define MCDE_GREENCOLOR_SHIFT    SHIFT_BYTE1
#define MCDE_CHPRIORITY_SHIFT    SHIFT_QUARTET7
#define MCDE_CHXLPF_SHIFT        16
#define MCDE_CHX_ABORT_SHIFT     1

/*******************************************************************
   MCDE Channel A/B Register Fields
********************************************************************/
#define MCDE_CHX_BURSTSIZE_MASK   (MASK_QUARTET6 & 0x07000000)
#define MCDE_CHX_ALPHA_MASK       (MASK_BYTE2)
#define MCDE_CHX_ROTDIR_MASK      (MASK_BIT15)
#define MCDE_CHX_GAMAEN_MASK      (MASK_BIT14)
#define MCDE_FLICKFORMAT_MASK     (MASK_BIT13)
#define MCDE_FLICKMODE_MASK       (MASK_BIT11 | MASK_BIT12)
#define MCDE_BLENDCONTROL_MASK    (MASK_BIT10)
#define MCDE_KEYCTRL_MASK         (MASK_BIT7|MASK_BIT8|MASK_BIT9)
#define MCDE_ROTEN_MASK           (MASK_BIT6)
#define MCDE_DITHEN_MASK          (MASK_BIT5)
#define MCDE_CEAEN_MASK           (MASK_BIT4)
#define MCDE_AFLICKEN_MASK        (MASK_BIT3)
#define MCDE_BLENDEN_MASK         (MASK_BIT2)
#define MCDE_CLK_MASK             (MASK_BIT30)
#define MCDE_BCD_MASK             (MASK_BIT29)
#define MCDE_OUTBPP_MASK          (MASK_BIT26 | MASK_BIT27 | MASK_BIT28)
#define MCDE_CLP_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25)
#define MCDE_CDWIN_MASK           (MASK_BIT13 | MASK_BIT14 | MASK_BIT15)
#define MCDE_CLOCKSEL_MASK        (MASK_BIT12 | MASK_BIT11 | MASK_BIT10)
#define MCDE_PCD_MASK             (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9)
#define MCDE_KEYA_MASK            (MASK_BYTE3)
#define MCDE_KEYR_MASK            (MASK_BYTE2)
#define MCDE_KEYG_MASK            (MASK_BYTE1)
#define MCDE_KEYB_MASK            (MASK_BYTE0)
#define MCDE_RGB_MASK1            (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25)
#define MCDE_RGB_MASK2            (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9)
#define MCDE_THRESHOLD_MASK       (MASK_QUARTET6)
#define MCDE_COEFFN3_MASK         (MASK_BYTE2)
#define MCDE_COEFFN2_MASK         (MASK_BYTE1)
#define MCDE_COEFFN1_MASK         (MASK_BYTE0)
#define MCDE_TV_LINES_MASK        (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_TVMODE_MASK          (MASK_BIT3 | MASK_BIT4)
#define MCDE_IFIELD_MASK          (MASK_BIT2)
#define MCDE_INTEREN_MASK         (MASK_BIT1)
#define MCDE_SELMODE_MASK         (MASK_BIT0)
#define MCDE_BSL_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_BEL_MASK             (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_FSL2_MASK            (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_FSL1_MASK            (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_DVO2_MASK            (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_DVO1_MASK            (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_SWH2_MASK            (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_SWH1_MASK            (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_SWW_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_DHO_MASK             (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_ALW_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_LBW_MASK             (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#define MCDE_TVBCR_MASK            MASK_BYTE2
#define MCDE_TVBCB_MASK            MASK_BYTE1
#define MCDE_TVBLU_MASK            MASK_BYTE0
#define MCDE_REVVAEN_MASK          MASK_BIT31
#define MCDE_REVTGEN_MASK          MASK_BIT30
#define MCDE_REVLOADSEL_MASK       (MASK_BIT28 | MASK_BIT29)
#define MCDE_REVDEL1_MASK          (MASK_QUARTET6)
#define MCDE_REVDEL0_MASK          (MASK_BYTE2)
#define MCDE_PSVAEN_MASK           (MASK_BIT15)
#define MCDE_PSTGEN_MASK           (MASK_BIT14)
#define MCDE_PSLOADSEL_MASK        (MASK_BIT12 | MASK_BIT13)
#define MCDE_PSDEL1_MASK           (MASK_QUARTET2)
#define MCDE_PSDEL0_MASK           (MASK_BYTE0)
#define MCDE_IOE_MASK              (MASK_BIT23)
#define MCDE_IPC_MASK              (MASK_BIT22)
#define MCDE_IHS_MASK              (MASK_BIT21)
#define MCDE_IVS_MASK              (MASK_BIT20)
#define MCDE_IVP_MASK              (MASK_BIT19)
#define MCDE_ICLSPL_MASK           (MASK_BIT18)
#define MCDE_ICLREV_MASK           (MASK_BIT17)
#define MCDE_ICLSP_MASK            (MASK_BIT16)
#define MCDE_SPLVAEN_MASK          (MASK_BIT15)
#define MCDE_SPLTGEN_MASK          (MASK_BIT14)
#define MCDE_SPLLOADSEL_MASK       (MASK_BIT12 | MASK_BIT13)
#define MCDE_SPLDEL1_MASK          (MASK_QUARTET2)
#define MCDE_SPLDEL0_MASK          (MASK_BYTE0)
#define MCDE_FOFFY_MASK            (MASK_BIT14 | MASK_BIT13 | MASK_BIT12 | MASK_BIT11 | MASK_BIT10)
#define MCDE_FOFFX_MASK            (MASK_BIT5 | MASK_BIT6 | MASK_BIT7 | MASK_BIT8 | MASK_BIT9)
#define MCDE_MASK_BITCTRL_MASK     (MASK_BIT4)
#define MCDE_MODE_MASK             (MASK_BIT2 | MASK_BIT3)
#define MCDE_COMP_MASK             (MASK_BIT1)
#define MCDE_TEMP_MASK             (MASK_BIT0)
#define MCDE_YB_MASK               (MASK_BIT28 | MASK_BIT27 | MASK_BIT26 | MASK_BIT25 | MASK_BIT24)
#define MCDE_XB_MASK               (MASK_BIT20 | MASK_BIT19 | MASK_BIT18 | MASK_BIT17 | MASK_BIT16)
#define MCDE_YG_MASK               (MASK_BIT12 | MASK_BIT11 | MASK_BIT10 | MASK_BIT9 | MASK_BIT8 | MASK_BIT7 | MASK_BIT6 | MASK_BIT5)
#define MCDE_XG_MASK               (MASK_BIT0 | MASK_BIT1 | MASK_BIT2 | MASK_BIT3 | MASK_BIT4)
#ifdef ST_8500ED
#define MCDE_ARED_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25)
#define MCDE_GREEN_MASK            (MASK_BYTE1)
#define MCDE_BLUE_MASK             (MASK_BYTE0)
#else
#define MCDE_ARED_MASK			   (0xFFF)
#define MCDE_GREEN_MASK			   (0xFFF)
#define MCDE_BLUE_MASK			   (0xFFF)
#define MCDE_GAM_MASK              0xFFFFFF
#endif
#define MCDE_RED_MASK              (MASK_BYTE2)

#define MCDE_CHX_BURSTSIZE_SHIFT  SHIFT_QUARTET6 
#define MCDE_CHX_ALPHA_SHIFT      SHIFT_HALFWORD1 
#define MCDE_CHX_ROTDIR_SHIFT     15
#define MCDE_CHX_GAMAEN_SHIFT     14 
#define MCDE_FLICKFORMAT_SHIFT    13
#define MCDE_FLICKMODE_SHIFT      11
#define MCDE_BLENDCONTROL_SHIFT   10
#define MCDE_KEYCTRL_SHIFT        7 
#define MCDE_ROTEN_SHIFT          6 
#define MCDE_DITHEN_SHIFT         5 
#define MCDE_CEAEN_SHIFT          4
#define MCDE_AFLICKEN_SHIFT       3
#define MCDE_BLENDEN_SHIFT        2
#define MCDE_CLK_SHIFT            30   
#define MCDE_BCD_SHIFT            29 
#define MCDE_OUTBPP_SHIFT         26
#define MCDE_CLP_SHIFT            SHIFT_HALFWORD1
#define MCDE_CDWIN_SHIFT          13
#define MCDE_CLOCKSEL_SHIFT       10
#define MCDE_KEYA_SHIFT           SHIFT_BYTE3
#define MCDE_KEYR_SHIFT           SHIFT_BYTE2
#define MCDE_KEYG_SHIFT           SHIFT_BYTE1
#define MCDE_RGB_SHIFT            SHIFT_HALFWORD1
#define MCDE_THRESHOLD_SHIFT      SHIFT_QUARTET6 
#define MCDE_COEFFN3_SHIFT        SHIFT_BYTE2
#define MCDE_COEFFN2_SHIFT        SHIFT_BYTE1
#define MCDE_COEFFN1_SHIFT        (SHIFT_BYTE0)
#define MCDE_TV_LINES_SHIFT       SHIFT_HALFWORD1
#define MCDE_TVMODE_SHIFT         3
#define MCDE_IFIELD_SHIFT         2
#define MCDE_INTEREN_SHIFT        1
#define MCDE_BSL_SHIFT            16
#define MCDE_FSL2_SHIFT           16
#define MCDE_DVO2_SHIFT           16
#define MCDE_SWW_SHIFT            16
#define MCDE_ALW_SHIFT            16
#define MCDE_TVBCR_SHIFT          SHIFT_BYTE2
#define MCDE_TVBCB_SHIFT          SHIFT_BYTE1
#define MCDE_REVVAEN_SHIFT        31
#define MCDE_REVTGEN_SHIFT        30
#define MCDE_REVLOADSEL_SHIFT     28
#define MCDE_REVDEL1_SHIFT        SHIFT_QUARTET6
#define MCDE_REVDEL0_SHIFT        SHIFT_HALFWORD1
#define MCDE_PSVAEN_SHIFT         15
#define MCDE_PSTGEN_SHIFT         14
#define MCDE_PSLOADSEL_SHIFT      12
#define MCDE_PSDEL1_SHIFT         8
#define MCDE_IOE_SHIFT            23 
#define MCDE_IPC_SHIFT            22
#define MCDE_IHS_SHIFT            21
#define MCDE_IVS_SHIFT            20
#define MCDE_IVP_SHIFT            19
#define MCDE_ICLSPL_SHIFT         18
#define MCDE_ICLREV_SHIFT         17
#define MCDE_ICLSP_SHIFT          16
#define MCDE_SPLVAEN_SHIFT        15
#define MCDE_SPLTGEN_SHIFT        14
#define MCDE_SPLLOADSEL_SHIFT     12
#define MCDE_SPLDEL1_SHIFT        8
#define MCDE_FOFFY_SHIFT          10
#define MCDE_FOFFX_SHIFT          5
#define MCDE_MASK_BITCTRL_SHIFT   4
#define MCDE_MODE_SHIFT           2
#define MCDE_COMP_SHIFT           1
#define MCDE_YB_SHIFT             24
#define MCDE_XB_SHIFT             SHIFT_HALFWORD1
#define MCDE_YG_SHIFT             5
#ifdef ST_8500ED
#define MCDE_ARED_SHIFT           SHIFT_HALFWORD1
#define MCDE_GREEN_SHIFT          SHIFT_BYTE1
#else
#define MCDE_ARED_SHIFT			  0
#define MCDE_GREEN_SHIFT          16
#endif

/*******************************************************************
   MCDE Channel C Register Fields
********************************************************************/
#define MCDE_SYNCCTRL_MASK    (MASK_BIT30 | MASK_BIT29)
#define MCDE_RESEN_MASK       (MASK_BIT18)
#define MCDE_CLKSEL_MASK      (MASK_BIT14 | MASK_BIT13)
#define MCDE_SYNCSEL_MASK     MASK_BIT6
#define MCDE_RES2_MASK        MASK_BIT28
#define MCDE_RES1_MASK        MASK_BIT27
#define MCDE_RD2_MASK         MASK_BIT26
#define MCDE_RD1_MASK         MASK_BIT25
#define MCDE_WR2_MASK         MASK_BIT24
#define MCDE_WR1_MASK         MASK_BIT23
#define MCDE_CD2_MASK         MASK_BIT22
#define MCDE_CD1_MASK         MASK_BIT21
#define MCDE_CS2_MASK         MASK_BIT20
#define MCDE_CS1_MASK         MASK_BIT19
#define MCDE_CS2EN_MASK       MASK_BIT17
#define MCDE_CS1EN_MASK       MASK_BIT16
#define MCDE_INBAND2_MASK     MASK_BIT12
#define MCDE_INBAND1_MASK     MASK_BIT11
#define MCDE_BUSSIZE2_MASK    MASK_BIT10
#define MCDE_BUSSIZE1_MASK    MASK_BIT9
#define MCDE_SYNCEN2_MASK     MASK_BIT8
#define MCDE_SYNCEN1_MASK     MASK_BIT7
#define MCDE_WMLVL2_MASK      MASK_BIT5
#define MCDE_WMLVL1_MASK      MASK_BIT4
#define MCDE_C2EN_MASK        MASK_BIT3
#define MCDE_C1EN_MASK        MASK_BIT2
#define MCDE_POWEREN_MASK     MASK_BIT1
#define MCDE_FLOEN_MASK       MASK_BIT0
#define MCDE_PDCTRL_MASK      (MASK_BIT10 |MASK_BIT11 | MASK_BIT12)
#define MCDE_DUPLEXER_MASK    (MASK_BIT7 |MASK_BIT8 | MASK_BIT9)
#define MCDE_BSDM_MASK        (MASK_BIT5 | MASK_BIT4 | MASK_BIT3)
#define MCDE_BSCM_MASK        (MASK_BIT2 | MASK_BIT1 | MASK_BIT0)
#define MCDE_VSDBL_MASK       (MASK_BIT29 | MASK_BIT30 | MASK_BIT31)
#define MCDE_VSSEL_MASK       MASK_BIT28
#define MCDE_VSPOL_MASK       MASK_BIT27
#define MCDE_VSPDIV_MASK      (MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define MCDE_VSPMAX_MASK      (MASK_BYTE2 | MASK_QUARTET3)
#define MCDE_VSPMIN_MASK      (MASK_BYTE0 | MASK_QUARTET2)
#define MCDE_TRDELC_MASK      (MASK_BYTE2 | MASK_QUARTET6)
#define MCDE_SYNCDELC1_MASK   (MASK_BYTE1)
#define MCDE_SYNCDELC0_MASK   (MASK_BYTE0)
#define MCDE_VSTAC1_MASK       MASK_BIT1
#define MCDE_VSTAC0_MASK       MASK_BIT0
#define MCDE_BCN_MASK          MASK_BYTE0
#define MCDE_CSCDDEACT_MASK    MASK_BYTE1
#define MCDE_CSCDACT_MASK      MASK_BYTE0
#define MCDE_MOTINT_MASK       MASK_BIT16
#define MCDE_RWDEACT_MASK      MASK_BYTE1
#define MCDE_RWACT_MASK        MASK_BYTE0
#define MCDE_DODEACT_MASK      MASK_BYTE1
#define MCDE_DOACT_MASK        MASK_BYTE0
#define MCDE_READDATA_MASK     MASK_HALFWORD0
#define MCDE_DATACOMMANDMASK   0x01FFFFFF

#define MCDE_SYNCCTRL_SHIFT   29
#define MCDE_RESEN_SHIFT      18
#define MCDE_CLKSEL_SHIFT     13
#define MCDE_SYNCSEL_SHIFT    6
#define MCDE_RES2_SHIFT       28
#define MCDE_RES1_SHIFT       27
#define MCDE_RD2_SHIFT        26
#define MCDE_RD1_SHIFT        25
#define MCDE_WR2_SHIFT        24
#define MCDE_WR1_SHIFT        23
#define MCDE_CD2_SHIFT        22
#define MCDE_CD1_SHIFT        21
#define MCDE_CS2_SHIFT        20
#define MCDE_CS1_SHIFT        19
#define MCDE_CS2EN_SHIFT      17 
#define MCDE_CS1EN_SHIFT      16
#define MCDE_INBAND2_SHIFT    12 
#define MCDE_INBAND1_SHIFT    11 
#define MCDE_BUSSIZE2_SHIFT   10 
#define MCDE_BUSSIZE1_SHIFT    9 
#define MCDE_SYNCEN2_SHIFT    8 
#define MCDE_SYNCEN1_SHIFT    7
#define MCDE_WMLVL2_SHIFT     5
#define MCDE_WMLVL1_SHIFT     4
#define MCDE_C2EN_SHIFT       3
#define MCDE_C1EN_SHIFT       2
#define MCDE_POWEREN_SHIFT    1
#define MCDE_PDCTRL_SHIFT     12
#define MCDE_DUPLEXER_SHIFT   7
#define MCDE_BSDM_SHIFT       3
#define MCDE_VSDBL_SHIFT      29
#define MCDE_VSSEL_SHIFT      28
#define MCDE_VSPOL_SHIFT      27
#define MCDE_VSPDIV_SHIFT     24
#define MCDE_VSPMAX_SHIFT     12
#define MCDE_TRDELC_SHIFT     SHIFT_HALFWORD1
#define MCDE_SYNCDELC1_SHIFT  SHIFT_BYTE1
#define MCDE_VSTAC1_SHIFT     1
#define MCDE_CSCDDEACT_SHIFT  SHIFT_BYTE1
#define MCDE_MOTINT_SHIFT     SHIFT_HALFWORD1
#define MCDE_RWDEACT_SHIFT    8
#define MCDE_DODEACT_SHIFT    SHIFT_BYTE1

/*******************************************************************
   MCDE DSI Register Fields
********************************************************************/
#define MCDE_PLLOUT_DIVSEL1_MASK        (MASK_BIT22 | MASK_BIT23)
#define MCDE_PLLOUT_DIVSEL0_MASK        (MASK_BIT20 | MASK_BIT21)
#define MCDE_PLL4IN_SEL_MASK            (MASK_BIT16 | MASK_BIT17)
#define MCDE_TXESCDIV_SEL_MASK              MASK_BIT8
#define MCDE_TXESCDIV_MASK              0xFF
#define MCDE_CMDBYTE_LSB_MASK           0xFFFF
#define MCDE_CMDBYTE_MSB_MASK           0xFFFF0000
#define MCDE_DSI_SW_MASK                0xFFF0000
#define MCDE_DSI_DMA_MASK               0xFFF
#define MCDE_DSI_PACK_MASK              (MASK_BIT20 | MASK_BIT21 | MASK_BIT22)
#define MCDE_DSI_DCSVID_MASK            MASK_BIT18
#define MCDE_DSI_BYTE_SWAP_MASK         MASK_BIT17
#define MCDE_DSI_BIT_SWAP_MASK          MASK_BIT16
#define MCDE_DSI_CMD8_MASK              MASK_BIT13
#define MCDE_DSI_VID_MODE_MASK          MASK_BIT12
#define MCDE_BLANKING_MASK              MASK_QUARTET0
#define MCDE_DSI_FRAME_MASK             (MASK_HALFWORD0 | MASK_BYTE2)
#define MCDE_DSI_PACKET_MASK            MASK_HALFWORD0

#define MCDE_PLLOUT_DIVSEL1_SHIFT       22
#define MCDE_PLLOUT_DIVSEL0_SHIFT       20
#define MCDE_PLL4IN_SEL_SHIFT           16
#define MCDE_TXESCDIV_SEL_SHIFT         8
#define MCDE_CMDBYTE_MSB_SHIFT          16 
#define MCDE_DSI_SW_SHIFT               16
#define MCDE_DSI_PACK_SHIFT             20
#define MCDE_DSI_DCSVID_SHIFT           18
#define MCDE_DSI_BYTE_SWAP_SHIFT        17
#define MCDE_DSI_BIT_SWAP_SHIFT         16
#define MCDE_DSI_CMD8_SHIFT             13
#define MCDE_DSI_VID_MODE_SHIFT         12
/*******************************************************************
   Register Structure
********************************************************************/

typedef volatile struct
{
    t_uint32 mcde_extsrc_a0;
    t_uint32 mcde_extsrc_a1;
    t_uint32 mcde_extsrc_a2;
    t_uint32 mcde_extsrc_conf;
    t_uint32 mcde_extsrc_cr;
    t_uint32 mcde_unused1[3];
}t_mcde_ext_src_reg;

typedef volatile struct
{
    t_uint32 mcde_ovl_cr;
    t_uint32 mcde_ovl_conf;
    t_uint32 mcde_ovl_conf2;
    t_uint32 mcde_ovl_ljinc;
    t_uint32 mcde_ovl_crop;
	#ifdef ST_8500ED
    t_uint32 mcde_unused2[2];
    t_uint32 mcde_ovl_comp;
	#else
    t_uint32 mcde_ovl_comp;
    t_uint32 mcde_unused2[2];
	#endif
}t_mcde_ovl_reg;

typedef volatile struct
{
    t_uint32 mcde_ch_conf;
    t_uint32 mcde_chsyn_stat;
    t_uint32 mcde_chsyn_mod;
    t_uint32 mcde_chsyn_sw;
    t_uint32 mcde_chsyn_bck;
    t_uint32 mcde_chsyn_prio;
    t_uint32 mcde_unused3[2];
}t_mcde_ch_synch_reg;

typedef volatile struct
{
    t_uint32 mcde_ch_cr0;
    t_uint32 mcde_ch_cr1;
    t_uint32 mcde_ch_colkey;
    t_uint32 mcde_ch_fcolkey;
    t_uint32 mcde_ch_rgbconv1;
    t_uint32 mcde_ch_rgbconv2;
    t_uint32 mcde_ch_rgbconv3;
    t_uint32 mcde_ch_rgbconv4;
    t_uint32 mcde_ch_rgbconv5;
    t_uint32 mcde_ch_rgbconv6;
    t_uint32 mcde_ch_ffcoef0;
    t_uint32 mcde_ch_ffcoef1;
    t_uint32 mcde_ch_ffcoef2;
    t_uint32 unused;
    t_uint32 mcde_ch_tvcr;
    t_uint32 mcde_ch_tvbl1;
    t_uint32 mcde_ch_tvisl;
    t_uint32 mcde_ch_tvdvo;
    t_uint32 mcde_ch_tvswh;
    t_uint32 mcde_ch_tvtim1;
    t_uint32 mcde_ch_tvbalw;
    t_uint32 mcde_ch_tvbl2;
    t_uint32 mcde_ch_tvblu;
    t_uint32 mcde_ch_lcdtim0;
    t_uint32 mcde_ch_lcdtim1;
    t_uint32 mcde_ch_ditctrl;
    t_uint32 mcde_ch_ditoff;
    #ifdef ST_8500ED
    t_uint32 mcde_ch_pal;
    t_uint32 mcde_ch_gam;
    #else
    t_uint32 mcde_ch_pal0;
    t_uint32 mcde_ch_pal1;
    #endif
    t_uint32 mcde_rotadd0;
    t_uint32 mcde_rotadd1;
    #ifndef ST_8500ED
    t_uint32 mcde_rotconf;
    #endif
    t_uint32 mcde_synchconf;
    #ifdef ST_8500ED
    t_uint32 mcde_unused7[96];
    #else   
    t_uint32 unused99;
    t_uint32 mcde_gam0;
    t_uint32 mcde_gam1;
    t_uint32 mcde_gam2;
    t_uint32 mcde_oledconv1;
    t_uint32 mcde_oledconv2;
    t_uint32 mcde_oledconv3;
    t_uint32 mcde_oledconv4;
    t_uint32 mcde_oledconv5;
    t_uint32 mcde_oledconv6;
    t_uint32 mcde_unused7[85];
    #endif
}t_mcde_ch_reg;

typedef struct
{
    t_uint32 mcde_chc_crc;
    t_uint32 mcde_chc_pbcrc0;
    t_uint32 mcde_chc_pbcrc1; 
    t_uint32 mcde_chc_pbcbmrc0[5];
    t_uint32 mcde_chc_pbcbmrc1[5];
    t_uint32 mcde_chc_pbcbcrc0[2];
    t_uint32 mcde_unused5[3];
    t_uint32 mcde_chc_pbcbcrc1[2];
    t_uint32 mcde_unused6[3];
    t_uint32 mcde_chc_vscrc[2];
    t_uint32 mcde_chc_sctrc;
    t_uint32 mcde_chc_scsr;
    t_uint32 mcde_chc_bcnr[2];
    t_uint32 mcde_chc_cscdtr[2];
    t_uint32 mcde_chc_rdwrtr[2];
    t_uint32 mcde_chc_dotr[2];
    t_uint32 mcde_chc_wcmd[2];
    t_uint32 mcde_chc_wd[2];
    t_uint32 mcde_chc_rdata[2]; 
	t_uint32 mcde_chc_stac[2];
	t_uint32 mcde_chc_ctrl[2];
}t_mcde_chc_reg;

typedef struct
{
    t_uint32 mcde_dsi_conf0;
    t_uint32 mcde_dsi_frame;
    t_uint32 mcde_dsi_pkt;
    t_uint32 mcde_dsi_sync;
    t_uint32 mcde_dsi_cmd;
    t_uint32 mcde_reserved2[3];
}t_mcde_dsi_reg;

typedef volatile struct
{
    t_uint32 mcde_cr;
    t_uint32 mcde_conf0;
    #ifdef ST_8500ED
    t_uint32 mcde_reserved[62];
    t_uint32 mcde_ais;
    t_uint32 mcde_imsc;
    t_uint32 mcde_ris;
    t_uint32 mcde_mis;
    t_uint32 mcde_sis;
    t_uint32 mcde_ssp;
    t_uint32 mcde_reserved3[57];
    #else
    t_uint32 mcde_ssp;
    t_uint32 mcde_reserved[61];
    t_uint32 mcde_ais;
    t_uint32 mcde_imscpp;
    t_uint32 mcde_imscovl;
    t_uint32 mcde_imscchnl;
    t_uint32 mcde_imscerr;
    t_uint32 mcde_rispp;
    t_uint32 mcde_risovl;
    t_uint32 mcde_rischnl;
    t_uint32 mcde_riserr;
    t_uint32 mcde_mispp;
    t_uint32 mcde_misovl;
    t_uint32 mcde_mischnl;
    t_uint32 mcde_miserr;
    t_uint32 mcde_sispp;
    t_uint32 mcde_sisovl;
    t_uint32 mcde_sischnl;
    t_uint32 mcde_siserr;
    t_uint32 mcde_reserved3[46];
    #endif
    t_uint32 mcde_pid;
    t_mcde_ext_src_reg ext_src[16];
    t_mcde_ovl_reg ovr[16];
    t_mcde_ch_synch_reg chnsynch[16];
    t_mcde_ch_reg ch_x_reg[2];
    t_mcde_chc_reg ch_c_reg;
    t_uint32    mcde_reserved1[84];
    t_mcde_dsi_reg mcde_dsi_reg[6];
    t_uint32       mcde_reserved4[12];
    #ifdef ST_8500ED
    t_uint32       mcde_clkdsi;
    #endif
}t_mcde_register;

/*******************************************************************
   System Context
********************************************************************/
typedef struct
{
    t_mcde_register *p_mcde_register;
    t_mcde_ch_id     dsi_formatter_plugged_channel[6];
}t_mcde_system_context;


