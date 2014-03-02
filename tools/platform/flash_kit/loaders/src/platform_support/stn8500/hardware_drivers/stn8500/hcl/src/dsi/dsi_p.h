/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI private header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "hcl_defs.h"
#include "dsi.h"



#define DSI_SET_BIT                     0x1
#define DSI_CLEAR_BIT                   0x0
#define DSI_MCTL_INTMODE_MASK           MASK_BIT0
#define DSI_MCTL_LINKEN_MASK            MASK_BIT0
#define DSI_MCTL_INTERFACE1_MODE_MASK   MASK_BIT1
#define DSI_MCTL_VID_EN_MASK            MASK_BIT2
#define DSI_MCTL_TVG_SEL_MASK           MASK_BIT3
#define DSI_MCTL_TBG_SEL_MASK           MASK_BIT4
#define DSI_MCTL_READEN_MASK            MASK_BIT8
#define DSI_MCTL_BTAEN_MASK             MASK_BIT9
#define DSI_MCTL_DISPECCGEN_MASK        MASK_BIT10
#define DSI_MCTL_DISPCHECKSUMGEN_MASK   MASK_BIT11
#define DSI_MCTL_HOSTEOTGEN_MASK        MASK_BIT12
#define DSI_MCTL_DISPEOTGEN_MASK        MASK_BIT13
#ifdef ST_8500ED
#define DSI_PLL_MASTER_MASK             MASK_BIT16
#define DSI_PLL_OUT_SEL_MASK            MASK_BIT11
#define DSI_PLL_IN_SEL_MASK             MASK_BIT10
#define DSI_PLL_DIV_MASK                (MASK_BIT7 | MASK_BIT8 | MASK_BIT9)
#define DSI_PLL_MULT_MASK               (MASK_BYTE0 & 0x7F)
#else
#define DSI_PLL_MASTER_MASK             MASK_BIT31
#define DSI_PLL_OUT_SEL_MASK            MASK_BIT18
#define DSI_PLL_OUT_DIV_MASK			(MASK_BYTE1 & 0x3F)
#define DSI_PLL_IN_DIV_MASK				(MASK_BIT14 | MASK_BIT15 | MASK_BIT16)
#define DSI_PLL_SEL_DIV2_MASK			MASK_BIT17
#define DSI_PLL_MULT_MASK               MASK_BYTE0
#endif
#define DSI_REG_TE_MASK                 MASK_BIT7
#define DSI_IF1_TE_MASK                 MASK_BIT5
#define DSI_IF2_TE_MASK                 MASK_BIT6
#define DSI_LANE2_EN_MASK               MASK_BIT0
#define DSI_FORCE_STOP_MODE_MASK        MASK_BIT1
#define DSI_CLK_CONTINUOUS_MASK         MASK_BIT2
#define DSI_CLK_ULPM_EN_MASK            MASK_BIT3
#define DSI_DAT1_ULPM_EN_MASK           MASK_BIT4
#define DSI_DAT2_ULPM_EN_MASK           MASK_BIT5
#define DSI_WAIT_BURST_MASK             (MASK_BIT6 | MASK_BIT7 | MASK_BIT8 | MASK_BIT9)
#define DSI_CLK_FORCESTOP_MASK          MASK_BIT10
#define DSI_CLKLANESTS_MASK             (MASK_BIT0 | MASK_BIT1)
#define DSI_DATALANE1STS_MASK           (MASK_BIT2 | MASK_BIT3 | MASK_BIT4)
#define DSI_DATALANE2STS_MASK           (MASK_BIT5 | MASK_BIT6)
#define DSI_CLK_DIV_MASK                MASK_QUARTET0
#define DSI_HSTX_TO_MASK                (MASK_QUARTET1 | MASK_BYTE1 | MASK_BIT16 | MASK_BIT17)
#define DSI_LPRX_TO_MASK                (MASK_BYTE3 | MASK_QUARTET5 | MASK_BIT19 | MASK_BIT18)
#define DSI_CLK_ULPOUT_MASK             (MASK_BYTE0 | MASK_BIT8)
#define DSI_DATA_ULPOUT_MASK            (MASK_QUARTET3 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11 | MASK_BIT16 | MASK_BIT17)
#define DSI_PLL_START_MASK              MASK_BIT0
#define DSI_CKLANE_EN_MASK              MASK_BIT3
#define DSI_DAT1_EN_MASK                MASK_BIT4
#define DSI_DAT2_EN_MASK                MASK_BIT5
#define DSI_CLK_ULPM_MASK               MASK_BIT6
#define DSI_DAT1_ULPM_MASK              MASK_BIT7
#define DSI_DAT2_ULPM_MASK              MASK_BIT8
#define DSI_IF1_EN_MASK                 MASK_BIT9
#define DSI_IF2_EN_MASK                 MASK_BIT10
#define DSI_MAIN_STS_MASK               MASK_BYTE0
#ifdef ST_8500ED
#define DSI_DPHY_ERROR_MASK             MASK_HALFWORD0
#else
#define DSI_DPHY_ERROR_MASK             (MASK_BYTE1 | MASK_BIT6 | MASK_BIT7)
#endif
#define DSI_IF_DATA_MASK                MASK_HALFWORD0
#define DSI_IF_VALID_MASK               MASK_BIT16
#define DSI_IF_START_MASK               MASK_BIT17
#define DSI_IF_FRAME_SYNC_MASK          MASK_BIT18
#define DSI_IF_STALL_MASK               MASK_BIT0
#define DSI_INT_VAL_MASK                MASK_BIT0
#define DSI_DIRECT_CMD_RD_STS_MASK      (MASK_BYTE0 | MASK_BIT8)
#define DSI_CMD_MODE_STS_MASK           (MASK_QUARTET0 | MASK_BIT4 | MASK_BIT5)
#define DSI_RD_ID_MASK                  (MASK_BIT16 | MASK_BIT17 )
#define DSI_RD_DCSNOTGENERIC_MASK       MASK_BIT18
#define DSI_CMD_NAT_MASK                (MASK_BIT0 | MASK_BIT1 | MASK_BIT2)
#define DSI_CMD_LONGNOTSHORT_MASK       MASK_BIT3
#define DSI_CMD_HEAD_MASK               (MASK_QUARTET2 | MASK_BIT12 | MASK_BIT13)
#define DSI_CMD_ID_MASK                 (MASK_BIT14 | MASK_BIT15)
#define DSI_CMD_SIZE_MASK               (MASK_QUARTET4 | MASK_BIT20)
#define DSI_CMD_LP_EN_MASK              (MASK_BIT21)
#define DSI_TRIGGER_VAL_MASK            MASK_QUARTET6
#define DSI_TE_LOWERBIT_MASK            MASK_BYTE2
#define DSI_TE_UPPERBIT_MASK            (MASK_BIT24 | MASK_BIT25)
#define DSI_FIL_VAL_MASK                MASK_BYTE1
#define DSI_ARB_MODE_MASK               MASK_BIT6
#define DSI_ARB_PRI_MASK                MASK_BIT7
#define DSI_START_MODE_MASK             (MASK_BIT0 | MASK_BIT1 )
#define DSI_STOP_MODE_MASK              (MASK_BIT2 | MASK_BIT3)
#define DSI_VID_ID_MASK                 (MASK_BIT4 | MASK_BIT5)
#define DSI_HEADER_MASK                 (MASK_BIT6 | MASK_BIT7 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11)
#define DSI_PIXEL_MODE_MASK             (MASK_BIT12 | MASK_BIT13)
#define DSI_BURST_MODE_MASK             (MASK_BIT14)
#define DSI_SYNC_PULSE_ACTIVE_MASK      (MASK_BIT15)
#define DSI_SYNC_PULSE_HORIZONTAL_MASK  (MASK_BIT16)
#define DSI_BLKLINE_MASK                (MASK_BIT17 | MASK_BIT18)
#define DSI_BLKEOL_MASK                 (MASK_BIT19 | MASK_BIT20)
#define DSI_RECOVERY_MODE_MASK          (MASK_BIT21 | MASK_BIT22)
#define DSI_VSA_LENGTH_MASK             (MASK_QUARTET0 | MASK_BIT4 | MASK_BIT5)
#define DSI_VBP_LENGTH_MASK             (MASK_BIT6 | MASK_BIT7 | MASK_QUARTET2)
#define DSI_VFP_LENGTH_MASK             (MASK_QUARTET3 | MASK_QUARTET4)
#define DSI_VACT_LENGTH_MASK             (MASK_QUARTET5 | MASK_QUARTET6 |MASK_BIT28 | MASK_BIT29 | MASK_BIT30)
#define DSI_HSA_LENGTH_MASK              (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9)
#define DSI_HBP_LENGTH_MASK             (MASK_BIT10 | MASK_BIT11 | MASK_QUARTET3 | MASK_QUARTET4)
#define DSI_HFP_LENGTH_MASK              (MASK_QUARTET5 | MASK_QUARTET6 |MASK_BIT28 | MASK_BIT29 | MASK_BIT30)
#define DSI_RGB_SIZE_MASK               (MASK_BYTE0 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_LINE_POS_MASK               (MASK_BIT0 | MASK_BIT1)
#define DSI_LINE_VAL_MASK               (MASK_BIT2 | MASK_BIT3 | MASK_QUARTET1 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_HORI_POS_MASK               (MASK_BIT0 | MASK_BIT1 |MASK_BIT2)
#define DSI_HORI_VAL_MASK               (MASK_BYTE1 | MASK_QUARTET1 | MASK_BIT3)
#ifdef ST_8500ED
#define DSI_VID_MODE_STS_MASK           (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9)
#else
#define DSI_VID_MODE_STS_MASK           (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10)
#endif
#define DSI_BURST_LP_MASK               MASK_BIT16
#define DSI_MAX_BURST_LIMIT_MASK        MASK_HALFWORD0
#define DSI_MAX_LINE_LIMIT_MASK         MASK_HALFWORD1
#define DSI_EXACT_BURST_LIMIT_MASK      MASK_HALFWORD0
#define DSI_BLKLINE_EVENT_MASK          (MASK_BYTE0 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_BLKEOL_PCK_MASK             (MASK_BYTE2 | MASK_BIT15 | MASK_BIT14 | MASK_BIT13 | MASK_BIT24 | MASK_BIT25)
#define DSI_BLKLINE_PULSE_PCK_MASK      (MASK_BYTE0 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_BLKEOL_DURATION_MASK        (MASK_BYTE0 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_VERT_BLANK_DURATION_MASK    (MASK_BYTE2 | MASK_BIT15 | MASK_BIT14 | MASK_BIT13 | MASK_BIT24 | MASK_BIT25)
#define DSI_COL_RED_MASK                MASK_BYTE0
#define DSI_COL_GREEN_MASK              MASK_BYTE1
#define DSI_COL_BLUE_MASK               MASK_BYTE2
#define DSI_PAD_VAL_MASK                MASK_BYTE3
#define DSI_TVG_STRIPE_MASK             (MASK_BIT5 | MASK_BIT6 | MASK_BIT7)
#define DSI_TVG_MODE_MASK               (MASK_BIT3 | MASK_BIT4 )
#define DSI_TVG_STOPMODE_MASK           (MASK_BIT1 | MASK_BIT2 )
#define DSI_TVG_RUN_MASK                MASK_BIT0
#define DSI_TVG_NBLINE_MASK             (MASK_BYTE2 | MASK_BIT24 | MASK_BIT25 | MASK_BIT26)
#define DSI_TVG_LINE_SIZE_MASK          (MASK_BYTE0 | MASK_QUARTET2 | MASK_BIT12)
#define DSI_CMD_MODE_STATUS_MASK           (MASK_QUARTET0 | MASK_BIT4 | MASK_BIT5 )
#define DSI_DIRECT_CMD_STS_MASK         (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10 )
#define DSI_DIRECT_CMD_RD_STATUS_MASK      (MASK_BYTE0 | MASK_BIT8 )
#ifdef ST_8500ED
#define DSI_VID_MODE_STATUS_MASK           (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 )
#else
#define DSI_VID_MODE_STATUS_MASK           (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10 )
#endif
#define DSI_TG_STS_MASK                 (MASK_BIT0 | MASK_BIT1)
#define DSI_CLK_TRIM_RD_MASK            MASK_BIT0
#define DSI_SWAP_PINS_CLK_MASK          MASK_BIT0
#define DSI_HS_INVERT_CLK_MASK          MASK_BIT1
#define DSI_SWAP_PINS_DAT1_MASK         MASK_BIT2
#define DSI_HS_INVERT_DAT1_MASK         MASK_BIT3
#define DSI_SWAP_PINS_DAT2_MASK         MASK_BIT4
#define DSI_HS_INVERT_DAT2_MASK         MASK_BIT5
#define DSI_UI_X4_MASK                  (MASK_QUARTET2 | MASK_BIT6 |MASK_BIT7 )

#define DSI_MCTL_INTERFACE1_MODE_SHIFT  1
#define DSI_MCTL_VID_EN_SHIF            2
#define DSI_MCTL_TVG_SEL_SHIFT          3
#define DSI_MCTL_TBG_SEL_SHIFT          4
#define DSI_MCTL_READEN_SHIFT           8
#define DSI_MCTL_BTAEN_SHIFT            9
#define DSI_MCTL_DISPECCGEN_SHIFT       10
#define DSI_MCTL_DISPCHECKSUMGEN_SHIFT  11
#define DSI_MCTL_HOSTEOTGEN_SHIFT       12
#define DSI_MCTL_DISPEOTGEN_SHIFT       13
#ifdef ST_8500ED
#define DSI_PLL_MASTER_SHIFT            16
#define DSI_PLL_OUT_SEL_SHIFT           11
#define DSI_PLL_IN_SEL_SHIFT            10
#else
#define DSI_PLL_MASTER_SHIFT            31
#define DSI_PLL_OUT_SEL_SHIFT           18
#define DSI_PLL_OUT_DIV_SHIFT			8
#define DSI_PLL_IN_DIV_SHIFT			14
#define DSI_PLL_SEL_DIV2_SHIFT			17
#endif
#define DSI_MCTL_VID_EN_SHIFT			2
#define DSI_PLL_DIV_SHIFT               8 
#define DSI_REG_TE_SHIFT                7 
#define DSI_IF1_TE_SHIFT                5
#define DSI_IF2_TE_SHIFT                6 
#define DSI_FORCE_STOP_MODE_SHIFT       1    
#define DSI_CLK_CONTINUOUS_SHIFT        2
#define DSI_CLK_ULPM_EN_SHIFT           3
#define DSI_DAT1_ULPM_EN_SHIFT          4 
#define DSI_DAT2_ULPM_EN_SHIFT          5   
#define DSI_WAIT_BURST_SHIFT            6 
#define DSI_CLK_FORCESTOP_SHIFT         10
#define DSI_DATALANE1STS_SHIFT          2  
#define DSI_DATALANE2STS_SHIFT          5 
#define DSI_HSTX_TO_SHIFT               4
#define DSI_LPRX_TO_SHIFT               18
#define DSI_DATA_ULPOUT_SHIFT           9
#define DSI_CKLANE_EN_SHIFT             3 
#define DSI_DAT1_EN_SHIFT               4
#define DSI_DAT2_EN_SHIFT               5
#define DSI_CLK_ULPM_SHIFT              6
#define DSI_DAT1_ULPM_SHIFT             7
#define DSI_DAT2_ULPM_SHIFT             8
#define DSI_IF1_EN_SHIFT                9
#define DSI_IF2_EN_SHIFT                10
#define DSI_IF_VALID_SHIFT              16
#define DSI_IF_START_SHIFT              17
#define DSI_IF_FRAME_SYNC_SHIFT         18
#define DSI_RD_ID_SHIFT                 16
#define DSI_RD_DCSNOTGENERIC_SHIFT      18
#define DSI_CMD_LONGNOTSHORT_SHIFT      3
#define DSI_CMD_HEAD_SHIFT              8
#define DSI_CMD_ID_SHIFT                14
#define DSI_CMD_SIZE_SHIFT              16
#define DSI_CMD_LP_EN_SHIFT             21
#define DSI_TRIGGER_VAL_SHIFT           24
#define DSI_TE_LOWERBIT_SHIFT           16
#define DSI_TE_UPPERBIT_SHIFT           24
#define DSI_FIL_VAL_SHIFT               8
#define DSI_ARB_MODE_SHIFT              6
#define DSI_ARB_PRI_SHIFT               7       
#define DSI_STOP_MODE_SHIFT             2
#define DSI_VID_ID_SHIFT                4
#define DSI_HEADER_SHIFT                6
#define DSI_PIXEL_MODE_SHIFT            12
#define DSI_BURST_MODE_SHIFT            14
#define DSI_SYNC_PULSE_ACTIVE_SHIFT     15
#define DSI_SYNC_PULSE_HORIZONTAL_SHIFT 16
#define DSI_BLKLINE_SHIFT               17
#define DSI_BLKEOL_SHIFT                19
#define DSI_RECOVERY_MODE_SHIFT         21
#define DSI_VBP_LENGTH_SHIFT            6
#define DSI_VFP_LENGTH_SHIFT            12
#define DSI_VACT_LENGTH_SHIFT           20
#define DSI_HBP_LENGTH_SHIFT            10
#define DSI_HFP_LENGTH_SHIFT            20
#define DSI_LINE_VAL_SHIFT              2
#define DSI_HORI_VAL_SHIFT              3  
#define DSI_BURST_LP_SHIFT              16 
#define DSI_MAX_LINE_LIMIT_SHIFT        16
#define DSI_BLKEOL_PCK_SHIFT            13
#define DSI_VERT_BLANK_DURATION_SHIFT   13
#define DSI_COL_GREEN_SHIFT             8
#define DSI_COL_BLUE_SHIFT              16
#define DSI_PAD_VAL_SHIFT               24
#define DSI_TVG_STRIPE_SHIFT            5
#define DSI_TVG_MODE_SHIFT              3
#define DSI_TVG_STOPMODE_SHIFT          1
#define DSI_TVG_NBLINE_SHIFT            16
#define DSI_HS_INVERT_CLK_SHIFT         1
#define DSI_SWAP_PINS_DAT1_SHIFT        2
#define DSI_HS_INVERT_DAT1_SHIFT        3
#define DSI_SWAP_PINS_DAT2_SHIFT        4
#define DSI_HS_INVERT_DAT2_SHIFT        5
#define DSI_UI_X4_SHIFT                 6


#define DSI_VID_REG_LINE_DURATION_MASK             (MASK_BYTE0 | MASK_BIT8 | MASK_BIT9 | MASK_BIT10 | MASK_BIT11 | MASK_BIT12)
#define DSI_VID_REG_WAKEUP_TIME_MASK               (MASK_BIT13 | MASK_BIT14 | MASK_BIT15 | MASK_BYTE3)
#define DSI_VID_REG_LINE_DURATION_SHIFT             0
#define DSI_VID_REG_WAKEUP_TIME_SHIFT               13


typedef volatile struct
{
    /*Main control registers*/
    t_uint32 mctl_integration_mode;
    t_uint32 mctl_main_data_ctl;
    t_uint32 mctl_main_phy_ctl;
    t_uint32 mctl_pll_ctl;
    t_uint32 mctl_lane_sts;
    t_uint32 mctl_dphy_timeout;
    t_uint32 mctl_ulpout_time;
    t_uint32 mctl_dphy_static;
    t_uint32 mctl_main_en;
    t_uint32 mctl_main_sts;
    t_uint32 mctl_dphy_err;
    t_uint32 reserved7;
    /*
     integration mode registers*/
    t_uint32 int_vid_rddata;
    t_uint32 int_vid_gnt;
    t_uint32 int_cmd_rddata;
    t_uint32 int_cmd_gnt;
    t_uint32 int_interrupt_ctl;
    t_uint32 reserved8[3];
    /*
     Command mode registers*/
    t_uint32 cmd_mode_ctl;
    t_uint32 cmd_mode_sts;
    t_uint32 reserved9[2];
    /*
      Direct Command registers*/
    t_uint32 direct_cmd_send;
    t_uint32 direct_cmd_main_settings;
    t_uint32 direct_cmd_sts;
    t_uint32 direct_cmd_rd_init;
    t_uint32 direct_cmd_wrdat0;
    t_uint32 direct_cmd_wrdat1;
    t_uint32 direct_cmd_wrdat2;
    t_uint32 direct_cmd_wrdat3;
    t_uint32 direct_cmd_rddat;
    t_uint32 direct_cmd_rd_property;
    t_uint32 direct_cmd_rd_sts;
    t_uint32 reserved10;
    /*
      Video mode registers*/
    t_uint32 vid_main_ctl;
    t_uint32 vid_vsize;
    t_uint32 vid_hsize1;
    t_uint32 vid_hsize2;
    t_uint32 vid_blksize1;
    t_uint32 vid_blksize2;
    t_uint32 vid_pck_time;
    t_uint32 vid_dphy_time;
    t_uint32 vid_err_color;
    t_uint32 vid_vpos;
    t_uint32 vid_hpos;
    t_uint32 vid_mode_sts;
    t_uint32 vid_vca_setting1;
    t_uint32 vid_vca_setting2;
    /*
      Test Video Mode regsiter*/
    t_uint32 tvg_ctl;
    t_uint32 tvg_img_size;
    t_uint32 tvg_color1;
    t_uint32 tvg_color2;
    t_uint32 tvg_sts;
    t_uint32 reserved11;
    /*
      Test Byte generator register*/
    t_uint32 tbg_ctl;
    t_uint32 tbg_setting;
    t_uint32 tbg_sts;
    t_uint32 reserved12;
    /*
      Interrupt Enable and Edge detection register*/
    t_uint32 mctl_main_sts_ctl;
    t_uint32 cmd_mode_sts_ctl;
    t_uint32 direct_cmd_sts_ctl;
    t_uint32 direct_cmd_rd_sts_ctl;
    t_uint32 vid_mode_sts_ctl;
    t_uint32 tg_sts_ctl;
    t_uint32 mctl_dphy_err_ctl;
    t_uint32 dphy_clk_trim_rd_ctl;
    /*
      Error/Interrupt Clear Register*/
    t_uint32 mctl_main_sts_clr;
    t_uint32 cmd_mode_sts_clr;
    t_uint32 direct_cmd_sts_clr;
    t_uint32 direct_cmd_rd_sts_clr;
    t_uint32 vid_mode_sts_clr;
    t_uint32 tg_sts_clr;
    t_uint32 mctl_dphy_err_clr;
    t_uint32 dphy_clk_trim_rd_clr;
    /*
      Flag registers*/
    t_uint32 mctl_main_sts_flag;
    t_uint32 cmd_mode_sts_flag;
    t_uint32 direct_cmd_sts_flag;
    t_uint32 direct_cmd_rd_sts_flag;
    t_uint32 vid_mode_sts_flag;
    t_uint32 tg_sts_flag;
    t_uint32 mctl_dphy_err_flag;
    t_uint32 dphy_clk_trim_rd_flag;
    t_uint32 dhy_lanes_trim;
}t_dsi_link_registers;

typedef struct
{
    t_dsi_link_registers    *dsi_register;
    t_dsi_interface1_mode   dsi_if1_mode;
    t_dsi_if1_state         dsi_if1_state;
    t_dsi_link_state        dsi_link_state;
    t_dsi_int_mode          dsi_int_mode;
    t_dsi_link              dsi_link;
}t_dsi_system_context;
