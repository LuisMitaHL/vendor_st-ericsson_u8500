/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI driver header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _DSI_H_
#define _DSI_H_

#include "hcl_defs.h"
#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/*---------------------------------------------------------------------------
 * Define
 *---------------------------------------------------------------------------*/

#define DSI_HCL_VERSION_ID      2
#define DSI_MAJOR_ID            1
#define DSI_MINOR_ID            4

/*******************************************************************************
DSI Main Setting Registers Enums and structures
  ******************************************************************************/
  typedef enum
  {
      DSI_INT_MODE_DISABLE = 0x00,
      DSI_INT_MODE_ENABLE = 0x01  
  }t_dsi_int_mode;

  typedef enum
  {
      DSI_DISABLE = 0x0,
      DSI_ENABLE  = 0x1
  }t_dsi_link_state;

  typedef enum
  {
      DSI_SIGNAL_LOW = 0x0,
      DSI_SIGNAL_HIGH = 0x1
  }t_dsi_signal_state;

  typedef struct
  {
      t_dsi_link_state  swap_pins_clk;
      t_dsi_link_state  hs_invert_clk;
      t_dsi_link_state  swap_pins_dat1;
      t_dsi_link_state  hs_invert_dat1;
      t_dsi_link_state  swap_pins_dat2;
      t_dsi_link_state  hs_invert_dat2;
      t_uint8           ui_x4;
  }t_dphy_static;

  typedef enum
  {
      DSI_LINK0 = 0x00,
      DSI_LINK1 = 0x01,
      DSI_LINK2 = 0x02
  }t_dsi_link;

  typedef enum
  {
      DSI_COMMAND_MODE = 0x0,
      DSI_VIDEO_MODE = 0x1,
  }t_dsi_interface1_mode;

  typedef enum
  {
      DSI_INTERFACE_1 = 0x0,
      DSI_INTERFACE_2 = 0x1
  }t_dsi_interface;

  typedef enum
  {
      DSI_IF1_DISABLE = 0x0,
      DSI_IF1_ENABLE  = 0x1
  }t_dsi_if1_state;

  typedef enum
  {
      DSI_PLL_IN_CLK_27 = 0x0,/*from TV PLL*/
      DSI_PLL_IN_CLK_26 = 0x1 /*from system PLL*/
  }t_dsi_pll_clk_in;

  typedef enum
  {
      DSI_INTERNAL_PLL = 0x0,
      DSI_SYSTEM_PLL = 0x1
  }t_pll_out_sel; /*DPHY HS bit clock select*/

  typedef enum
  {
      DSI_PLL_MASTER = 0x0,
      DSI_PLL_SLAVE  = 0x1
  }t_dsi_pll_mode_sel;
  
  typedef enum
  {
  	DSI_OUTPUT_CLK = 0x0,
  	DSI_OUTPUT_CLK_DIV2 = 0x1
  }t_dsi_sel_div2;
  

  #ifdef ST_8500ED
  
  typedef struct
  {
      t_uint8               multiplier;
      t_uint8               division_ratio;
      t_dsi_pll_clk_in      pll_in_sel;
      t_pll_out_sel         pll_out_sel;
      t_dsi_pll_mode_sel    pll_master;
  }t_dsi_pll_ctl;
  
  #else
  
  typedef struct
  {
      t_uint8               multiplier;
      t_uint8               pll_out_div;
      t_uint8               pll_in_div;
      t_dsi_sel_div2        pll_sel_div2;
      t_pll_out_sel         pll_out_sel;
      t_dsi_pll_mode_sel    pll_master;
  }t_dsi_pll_ctl;
  
  #endif

  typedef enum
  {
      DSI_REG_TE = 0x00,
      DSI_IF_TE = 0x01
  }t_dsi_te_sel;

  typedef struct
  {
      t_dsi_te_sel      te_sel;
      t_dsi_interface   interface;
  }t_dsi_te_en;

  typedef enum
  {
      DSI_CLK_LANE = 0x00,
      DSI_DATA_LANE1 = 0x01,
      DSI_DATA_LANE2 = 0x02
  }t_dsi_lane;

  typedef enum
  {
      DSI_DAT_LANE1 = 0x0,
      DSI_DAT_LANE2 = 0x1
  }t_dsi_data_lane;

  typedef enum
  {
      DSI_CLK_LANE_START = 0x00,
      DSI_CLK_LANE_IDLE  = 0x01,
      DSI_CLK_LANE_HS    = 0x02,
      DSI_CLK_LANE_ULPM  = 0x03
  }t_dsi_clk_lane_state;

  typedef enum
  {
      DSI_DATA_LANE_START = 0x000,
      DSI_DATA_LANE_IDLE  = 0x001,
      DSI_DATA_LANE_WRITE = 0x002,
      DSI_DATA_LANE_ULPM  = 0x003,
      DSI_DATA_LANE_READ  = 0x004
  }t_dsi_data_lane_state;

  typedef struct
  {
      t_uint8  clk_div;
      t_uint16  hs_tx_timeout;
      t_uint16  lp_rx_timeout;
  }t_dsi_dphy_timeout;

  typedef enum
  {
      DSI_PLL_LOCK  = 0x01,
      DSI_CLKLANE_READY = 0x02,
      DSI_DAT1_READY    = 0x04,
      DSI_DAT2_READY    = 0x08,
      DSI_HSTX_TO_ERROR = 0x10,
      DSI_LPRX_TO_ERROR = 0x20,
      DSI_CRS_UNTERM_PCK = 0x40,
      DSI_VRS_UNTERM_PCK = 0x80
   }t_dsi_link_status;

   typedef struct
   {
       t_uint16             if_data;
       t_dsi_signal_state   if_valid;
       t_dsi_signal_state   if_start;
       t_dsi_signal_state   if_frame_sync;
   }t_dsi_int_read;

   typedef enum
   {
       DSI_ERR_SOT_HS_1  = 0x1,
       DSI_ERR_SOT_HS_2  = 0x2,
       DSI_ERR_SOTSYNC_1 = 0x4,
       DSI_ERR_SOTSYNC_2 = 0x8,
       DSI_ERR_EOTSYNC_1 = 0x10,
       DSI_ERR_EOTSYNC_2 = 0x20,
       DSI_ERR_ESC_1     = 0x40,
       DSI_ERR_ESC_2     = 0x80,
       DSI_ERR_SYNCESC_1 = 0x100,
       DSI_ERR_SYNCESC_2 = 0x200,
       DSI_ERR_CONTROL_1 = 0x400,
       DSI_ERR_CONTROL_2 = 0x800,
       DSI_ERR_CONT_LP0_1 = 0x1000,
       DSI_ERR_CONT_LP0_2 = 0x2000,
       DSI_ERR_CONT_LP1_1 = 0x4000,
       DSI_ERR_CONT_LP1_2 = 0x8000,
   }t_dsi_dphy_err;

   typedef enum
   {
       DSI_VIRTUAL_CHANNEL_0 = 0x0,
       DSI_VIRTUAL_CHANNEL_1 = 0x1,
       DSI_VIRTUAL_CHANNEL_2 = 0x2,
       DSI_VIRTUAL_CHANNEL_3 = 0x3
   }t_dsi_virtual_ch;

   typedef enum
   {
       DSI_ERR_NO_TE = 0x1,
       DSI_ERR_TE_MISS = 0x2,
       DSI_ERR_SDI1_UNDERRUN = 0x4,
       DSI_ERR_SDI2_UNDERRUN = 0x8,
       DSI_ERR_UNWANTED_RD   = 0x10,
       DSI_CSM_RUNNING       = 0x20
   }t_dsi_cmd_mode_sts;

   typedef enum
   {
       DSI_COMMAND_DIRECT = 0x0,
       DSI_COMMAND_GENERIC = 0x1
   }t_dsi_cmd_type;

   typedef struct
   {
       t_uint16 rd_size;
       t_dsi_virtual_ch rd_id;
       t_dsi_cmd_type cmd_type;
   }t_dsi_cmd_rd_property;

   typedef enum
   {
       DSI_CMD_WRITE            = 0x0,
       DSI_CMD_READ             = 0x1,
       DSI_CMD_TE_REQUEST       = 0x4,
       DSI_CMD_TRIGGER_REQUEST  = 0x5,
       DSI_CMD_BTA_REQUEST      = 0x6
   }t_dsi_cmd_nat;

   typedef enum
   {
       DSI_CMD_SHORT     = 0x0,
       DSI_CMD_LONG      = 0x1
   }t_dsi_cmd_packet;

   typedef struct
   {
       t_uint8  rddat0;
       t_uint8  rddat1;
       t_uint8  rddat2;
       t_uint8  rddat3;
   }t_dsi_cmd_rddat;

   typedef struct
   {
       t_dsi_cmd_nat        cmd_nature;
       t_dsi_cmd_packet     packet_type;
       t_uint8              cmd_header;
       t_dsi_virtual_ch     cmd_id;
       t_uint8              cmd_size;
       t_dsi_link_state     cmd_lp_enable;
       t_uint8              cmd_trigger_val;
   }t_dsi_cmd_main_setting;

   typedef enum
   {
       DSI_CMD_TRANSMISSION = 0x1,
       DSI_WRITE_COMPLETED  = 0x2,
       DSI_TRIGGER_COMPLETED = 0x4,
       DSI_READ_COMPLETED    = 0x8,
       DSI_ACKNOWLEDGE_RECEIVED = 0x10,
       DSI_ACK_WITH_ERR_RECEIVED = 0x20,
       DSI_TRIGGER_RECEIVED      = 0x40,
       DSI_TE_RECEIVED           = 0x80,
       DSI_BTA_COMPLETED         = 0x100,
       DSI_BTA_FINISHED          = 0x200,
       DSI_READ_COMPLETED_WITH_ERR = 0x400,
       DSI_TRIGGER_VAL             = 0x7800
   }t_dsi_direct_cmd_sts;

   typedef enum
   {
       DSI_TE_256 = 0x00,
       DSI_TE_512 = 0x01,
       DSI_TE_1024 = 0x02,
       DSI_TE_2048 = 0x03
   }t_dsi_te_timeout;

   typedef enum
   {
       DSI_ARB_MODE_FIXED = 0x0,
       DSI_ARB_MODE_ROUNDROBIN = 0x1
   }t_dsi_arb_mode;

   typedef struct
   {
       t_dsi_arb_mode   arb_mode;
       t_dsi_interface  arb_fixed_if;
   }t_dsi_arb_ctl;

   typedef enum
   {
       DSI_STARTON_VSYNC = 0x00,
       DSI_STARTON_VFP   = 0x01,
   }t_dsi_start_mode;

   typedef enum
   {
       DSI_STOPBEFORE_VSYNV = 0x0,
       DSI_STOPAT_LINEEND   = 0x1,
       DSI_STOPAT_ACTIVELINEEND = 0x2,
   }t_dsi_stop_mode;

   typedef enum
   {
       DSI_NO_BURST_MODE = 0x0,
       DSI_BURST_MODE    = 0x1,
   }t_dsi_burst_mode;

   typedef enum
   {
       DSI_VID_MODE_16_PACKED = 0x0,
       DSI_VID_MODE_18_PACKED = 0x1,
       DSI_VID_MODE_16_LOOSELY = 0x2,
       DSI_VID_MODE_18_LOOSELY = 0x3
   }t_dsi_vid_pixel_mode;

   typedef enum
   {
       DSI_SYNC_PULSE_NOTACTIVE = 0x0,
       DSI_SYNC_PULSE_ACTIVE    = 0x1
   }t_dsi_sync_pulse_active;

   typedef enum
   {
       DSI_SYNC_PULSE_HORIZONTAL_NOTACTIVE = 0x0,
       DSI_SYNC_PULSE_HORIZONTAL_ACTIVE    = 0x1
   }t_dsi_sync_pulse_horizontal;

   typedef enum
   {
       DSI_NULL_PACKET = 0x0,
       DSI_BLANKING_PACKET = 0x1,
       DSI_LP_MODE         = 0x2,
   }t_dsi_blanking_packet;

   typedef enum
   {
       DSI_RECOVERY_HSYNC = 0x0,
       DSI_RECOVERY_VSYNC = 0x1,
       DSI_RECOVERY_STOP  = 0x2,
       DSI_RECOVERY_HSYNC_VSYNC = 0x3
   }t_dsi_recovery_mode;

   typedef struct
   {
       t_dsi_start_mode      vid_start_mode;
       t_dsi_stop_mode       vid_stop_mode;
       t_dsi_virtual_ch      vid_id;
       t_uint8               header;
       t_dsi_vid_pixel_mode  vid_pixel_mode;
       t_dsi_burst_mode      vid_burst_mode;
       t_dsi_sync_pulse_active sync_pulse_active;
       t_dsi_sync_pulse_horizontal sync_pulse_horizontal;
       t_dsi_blanking_packet       blkline_mode;
       t_dsi_blanking_packet       blkeol_mode;
       t_dsi_recovery_mode         recovery_mode;
   }t_dsi_vid_main_ctl;

   typedef struct
   {
       t_uint16 vact_length;
       t_uint8  vfp_length;
       t_uint8  vbp_length;
       t_uint8  vsa_length;
   }t_dsi_img_vertical_size;

   typedef struct
   {
       t_uint8  hsa_length;
       t_uint8  hbp_length;
       t_uint16 hfp_length;
       t_uint16 rgb_size;
   }t_dsi_img_horizontal_size;

   typedef struct
   {
       t_uint16  line_val;
       t_uint8 	 line_pos;
       t_uint16  horizontal_val;
       t_uint8   horizontal_pos;
   }t_dsi_img_position;

   typedef enum
   {
       DSI_VSG_RUNNING  = 0x1,
       DSI_ERR_MISSING_DATA = 0x2,
       DSI_ERR_MISSING_HSYNC = 0x4,
       DSI_ERR_MISSING_VSYNC = 0x8,
       DSI_ERR_SMALL_LENGTH  = 0x10,
       DSI_ERR_SMALL_HEIGHT  = 0x20,
       DSI_ERR_BURSTWRITE    = 0x40,
       DSI_ERR_LINEWRITE     = 0x80,
       DSI_ERR_LONGWRITE     = 0x100,
       DSI_ERR_VRS_WRONG_LENGTH = 0x200
   }t_dsi_vid_mode_sts;

   typedef enum
   {
       DSI_NULL_PACK = 0x0,
       DSI_LP          = 0x1,
   }t_dsi_burst_lp;

   typedef struct
   {
       t_dsi_burst_lp  burst_lp;
       t_uint16        max_burst_limit;
       t_uint16        max_line_limit;
       t_uint16        exact_burst_limit;
   }t_dsi_vca_setting;

   typedef struct
   {
       t_uint16     blkeol_pck;
       t_uint16     blkline_event_pck;
       t_uint16     blkline_pulse_pck;
       t_uint16     vert_balnking_duration;
       t_uint16     blkeol_duration;
   }t_dsi_vid_blanking;

   typedef struct
   {
       t_uint8      col_red;
       t_uint8      col_green;
       t_uint8      col_blue;
       t_uint8      pad_val;
   }t_dsi_vid_err_color;

   typedef enum
   {
       DSI_TVG_MODE_UNIQUECOLOR  = 0x0,
       DSI_TVG_MODE_STRIPES      = 0x1,
   }t_dsi_tvg_mode;

   typedef enum
   {
       DSI_TVG_STOP_FRAMEEND = 0x0,
       DSI_TVG_STOP_LINEEND  = 0x1,
       DSI_TVG_STOP_IMMEDIATE = 0x2,
   }t_dsi_tvg_stop_mode;

   

   typedef struct
   {
       t_uint8      tvg_stripe_size;
       t_dsi_tvg_mode  tvg_mode;
       t_dsi_tvg_stop_mode stop_mode;
   }t_dsi_tvg_control;

   typedef struct
   {
       t_uint16   tvg_nbline;
       t_uint16   tvg_line_size;
   }t_dsi_tvg_img_size;

   typedef struct
   {
       t_uint8  col_red;
       t_uint8  col_green;
       t_uint8  col_blue;
   }t_dsi_frame_color;

   typedef enum
   {
       DSI_TVG_COLOR1 = 0x0,
       DSI_TVG_COLOR2 = 0x1
   }t_dsi_color_type;

   typedef enum
   {
       DSI_TVG_STOPPED = 0x0,
       DSI_TVG_RUNNING = 0x1
   }t_dsi_tvg_state;

   typedef enum
   {
       DSI_TBG_STOPPED = 0x0,
       DSI_TBG_RUNNING = 0x1
   }t_dsi_tbg_state;

   typedef enum
   {
       DSI_SEND_1BYTE   = 0x0,
       DSI_SEND_2BYTE   = 0x1,
       DSI_SEND_BURST_STOP_COUNTER = 0x3,
       DSI_SEND_BURST_STOP = 0x4
   }t_dsi_tbg_mode;

/*   typedef enum
   {
       DSI_PLL_LOCK = 0x1,
       DSI_CLKLANE_READY = 0x2,
       DSI_DAT1_READY    = 0x4,
       DSI_DAT2_READY    = 0x8,
       DSI_HSTX_TO_ERR   = 0x10,
       DSI_LPRX_TO_ERR   = 0x20,
       DSI_CRS_UNTERM_PCK_ERR = 0x40,
       DSI_VRS_UNTERM_PCK_ERR = 0x80
   }t_dsi_mctl_main_sts_ctl;

   typedef enum
   {
       DSI_ERR_NO_TE = 0x1,
       DSI_ERR_TE_MISS  = 0x2,
       DSI_ERR_SDI1_UNDERRUN = 0x4,
       DSI_ERR_SDI2_UNDERRUN = 0x8,
       DSI_ERR_UNWANTED_RD  = 0x10,
       DSI_CSM_RUNNING      = 0x20,
   }t_dsi_cmd_mode_sts_ctl;

   typedef enum
   {
       DSI_CMD_TRANSMISSION = 0x1,
       DSI_WRITE_COMPLETED  = 0x2,
       DSI_TRIGGER_COMPLETED = 0x4,
       DSI_READ_COMPLETED    = 0x8,
       DSI_ACKNOWLEDGE_RECEIVED = 0x10,
       DSI_ACKNOWLEDGE_WITH_ERR =0x20,
       DSI_TRIGGER_RECEIVED     = 0x40,
       DSI_TE_RECEIVED          = 0x80,
       DSI_BTA_COMPLETED        = 0x100,
       DSI_BTA_FINISHED         = 0x200,
       DSI_READ_COMPLETED_WITH_ERR  = 0x400
   }t_dsi_direct_cmd_sts_ctl;*/

   typedef enum
   {
       DSI_ERR_FIXED = 0x1,
       DSI_ERR_UNCORRECTABLE = 0x2,
       DSI_ERR_CHECKSUM      = 0x4,
       DSI_ERR_UNDECODABLE   = 0x8,
       DSI_ERR_RECEIVE      = 0x10,
       DSI_ERR_OVERSIZE     = 0x20,
       DSI_ERR_WRONG_LENGTH  = 0x40,
       DSI_ERR_MISSING_EOT   = 0x80,
       DSI_ERR_EOT_WITH_ERR  = 0x100
   }t_dsi_direct_cmd_rd_sts_ctl;

/*   typedef enum
   {
       DSI_VSG_RUNNING   = 0x1,
       DSI_ERR_MISSING_DATA = 0x2,
       DSI_ERR_MISSING_HSYNC = 0x4,
       DSI_ERR_MISSING_VSYNC = 0x8,
       DSI_ERR_SMALL_LENGTH = 0x10,
       DSI_ERR_SMALL_HEIGHT = 0x20,
       DSI_ERR_BURSTWRITE   = 0x40,
       DSI_ERR_LINEWRITE    = 0x80,
       DSI_ERR_LONGWRITE    = 0x100,
       DSI_ERR_VRS_WRONG_LENGTH = 0x200
   }t_dsi_vid_mode_sts_ctl;*/

   typedef enum
   {
       DSI_TVG_STS = 0x1,
       DSi_TBG_STS = 0x2
   }t_dsi_tg_sts_ctl;
   
   typedef enum
   {
    	DISABLE = 0,
    	ENABLE = 1
	}t_dsi_state;


	typedef struct
  	{
      t_dsi_state  interface2;
      t_dsi_state  interface1;
      t_dsi_state  data2_ulpm_req;
      t_dsi_state  data1_ulpm_req;
      t_dsi_state  clk_ulpm_req;
      t_dsi_state  data2_lane;
      t_dsi_state  data1_lane;
      t_dsi_state  clk_lane;
      t_dsi_state  pll_start;
	}t_mctl_main_en;


typedef struct
  {
      t_dsi_state  bta;
      t_dsi_state  read_operation;
      t_dsi_state  te_from_reg;
      t_dsi_state  te_on_interface2;
      t_dsi_state  te_on_interface1;
      t_dsi_state  test_byte_gen;
      t_dsi_state  test_video_gen;
      t_dsi_state  video_stream_gen;
      t_dsi_interface1_mode if1_mode;
      t_dsi_state  link;
}t_mctl_main_data_ctl;

typedef struct
  {
      t_uint8  wait_burst_time;// warning value on 4 bits
      t_dsi_state  dat2_ulpm_en;
      t_dsi_state  dat1_ulpm_en;
      t_dsi_state  clk_ulpm_en;
      t_dsi_state  clk_continuous;
      t_dsi_state  data2_lane_en;
}t_mctl_main_phy_ctl;

typedef struct
{
      t_uint16      reg_line_duration;
      t_uint16      reg_wakeup_time;
}t_vid_dphy_time;

   #define DSI_DPHY_Z_CALIB_OUT_VALID  MASK_BIT0

 /*******************************************************************************
   DSI Error Enums
   ******************************************************************************/
  typedef enum
  {
      DSI_OK 							    = HCL_OK,	    /* No error.*/
      DSI_NO_PENDING_EVENT_ERROR			= HCL_NO_PENDING_EVENT_ERROR,
      DSI_NO_MORE_FILTER_PENDING_EVENT	    = HCL_NO_MORE_FILTER_PENDING_EVENT,
      DSI_NO_MORE_PENDING_EVENT			    = HCL_NO_MORE_PENDING_EVENT,
      DSI_REMAINING_FILTER_PENDING_EVENTS   = HCL_REMAINING_FILTER_PENDING_EVENTS,
      DSI_REMAINING_PENDING_EVENTS		    = HCL_REMAINING_PENDING_EVENTS,
      DSI_INTERNAL_EVENT					= HCL_INTERNAL_EVENT,
      DSI_INTERNAL_ERROR                    = HCL_INTERNAL_ERROR,          
      DSI_NOT_CONFIGURED                    = HCL_NOT_CONFIGURED,          
      DSI_REQUEST_PENDING                   = HCL_REQUEST_PENDING,         
      DSI_REQUEST_NOT_APPLICABLE            = HCL_REQUEST_NOT_APPLICABLE,  
      DSI_INVALID_PARAMETER                 = HCL_INVALID_PARAMETER,       
      DSI_UNSUPPORTED_FEATURE               = HCL_UNSUPPORTED_FEATURE,     
      DSI_UNSUPPORTED_HW                    = HCL_UNSUPPORTED_HW          
   }t_dsi_error;
   /*************************************************************************************
   DSI APIs
   *************************************************************************************/
   
   PUBLIC t_dsi_error DSI_Init(IN t_dsi_link dsi_id,IN t_logical_address);

   /*************************************************************************************
   Main Control APIs
   *************************************************************************************/
   PUBLIC void DSI_SetIntegrationmode(IN t_dsi_link dsi_id,IN t_dsi_int_mode mode);
   PUBLIC void DSI_EnableLink(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableLink(IN t_dsi_link dsi_id);
   PUBLIC void DSI_SetInterface1_Mode(IN t_dsi_link dsi_id,IN t_dsi_interface1_mode mode);
   PUBLIC void DSI_EnableVideoStreamGenerator(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableVideoStreamGenerator(IN t_dsi_link dsi_id);
   PUBLIC t_dsi_error DSI_EnableTestVideoGenerator(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableTestVideoGenerator(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableTestByteGenerator(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableTestByteGenerator(IN t_dsi_link dsi_id);
   PUBLIC void DSI_ReadEnable(IN t_dsi_link dsi_id);
   PUBLIC void DSI_ReadDisable(IN t_dsi_link dsi_id);
   PUBLIC void DSI_BTAEnable(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableDisplayECCGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableDisplayECCGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableDisplayCheksumGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableDisplayCheksumGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableDisplayEOTGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableDisplayEOTGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableHostEOTGen(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableHostEOTGen(IN t_dsi_link dsi_id);
   PUBLIC t_dsi_error DSI_EnableTearingEffect(IN t_dsi_link dsi_id,IN t_dsi_te_en tearing);
   PUBLIC t_dsi_error DSI_DisableTearingEffect(IN t_dsi_link dsi_id,IN t_dsi_te_en tearing);
   PUBLIC void DSI_SetPLLControl(IN t_dsi_link dsi_id,t_dsi_pll_ctl pll_ctl);
   PUBLIC void DSI_EnableLane2(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableLane2(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableForceStopMode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableForceStopMode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableCLK_HS_SendingMode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableCLK_HS_SendingMode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableCLK_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableCLK_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableDAT1_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableDAT1_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_EnableDAT2_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_DisableDAT2_ULPM_Mode(IN t_dsi_link dsi_id);
   PUBLIC void DSI_SetWaitBurstTime(IN t_dsi_link dsi_id,IN t_uint8 delay);
   #ifndef ST_8500ED
   PUBLIC void DSI_ForceClkStop(IN t_dsi_link dsi_id);
   #endif
   PUBLIC t_dsi_error DSI_GetCLKLaneState(IN t_dsi_link dsi_id,OUT t_dsi_clk_lane_state *clklane_state);
   PUBLIC t_dsi_error DSI_GetDataLaneState(IN t_dsi_link dsi_id,IN t_dsi_data_lane data_lane,OUT t_dsi_data_lane_state *datalane_state);
   PUBLIC void DSI_SetDPHY_TimeOut(IN t_dsi_link dsi_id,IN t_dsi_dphy_timeout timeout);
   PUBLIC void DSI_SetDataLaneULP_OutTime(IN t_dsi_link dsi_id,IN t_uint16 timeout);
   PUBLIC void DSI_SetCLKLaneULP_OutTime(IN t_dsi_link dsi_id,IN t_uint16 timeout);
   PUBLIC void DSI_StartPLL(IN t_dsi_link dsi_id);
   PUBLIC void DSI_StopPLL(IN t_dsi_link dsi_id );
   PUBLIC t_dsi_error DSI_StartLane(IN t_dsi_link dsi_id,IN t_dsi_lane lane);
   PUBLIC t_dsi_error DSI_StopLane(IN t_dsi_link dsi_id,IN t_dsi_lane lane);
   PUBLIC t_dsi_error DSI_StartULPM_Mode(IN t_dsi_link dsi_id,IN t_dsi_lane lane);
   PUBLIC t_dsi_error DSI_StopULPM_Mode(IN t_dsi_link dsi_id,IN t_dsi_lane lane);
   PUBLIC t_dsi_error DSI_EnableInterface(IN t_dsi_link dsi_id,IN t_dsi_interface interface);
   PUBLIC t_dsi_error DSI_DisableInterface(IN t_dsi_link dsi_id,IN t_dsi_interface interface);
   PUBLIC t_dsi_error DSI_GetLinkStatus(IN t_dsi_link dsi_id,OUT t_uint8 *p_status);
   PUBLIC t_dsi_error DSI_GetDPHYError(IN t_dsi_link dsi_id,OUT t_uint16 *p_error);
   PUBLIC t_dsi_error DSI_GetStreamObservation(IN t_dsi_link dsi_id,IN t_dsi_interface1_mode mode ,OUT t_dsi_int_read *p_rd_data);
   PUBLIC void DSI_SetDPHY_Static(IN t_dsi_link dsi_id, IN t_dphy_static dhy_static);

   /******************************************************************************
   Integration Mode APIs
   *******************************************************************************/
   PUBLIC t_dsi_error DSI_SetStallSignal(IN t_dsi_link dsi_id,IN t_dsi_interface1_mode mode);
   PUBLIC t_dsi_error DSI_ResetStallSignal(IN t_dsi_link dsi_id,IN t_dsi_interface1_mode mode);
   PUBLIC t_dsi_error DSI_SetInterruptActive(IN t_dsi_link dsi_id);
   PUBLIC t_dsi_error DSI_ResetInterruptActive(IN t_dsi_link dsi_id);

   /*******************************************************************************
   Direct Command APIs
   *******************************************************************************/
   PUBLIC void DSI_DirectCommandSend(IN t_dsi_link dsi_id);
   PUBLIC t_dsi_error DSI_GetDirectCmdReadStatus(IN t_dsi_link dsi_id,OUT t_uint16 *p_read_status);
   PUBLIC t_dsi_error DSI_GetCmdModeStatus(IN t_dsi_link dsi_id,OUT t_uint8 *p_read_status);
   PUBLIC t_dsi_error DSI_WriteDirectCommand(IN t_dsi_link dsi_id, IN t_uint8 data_num, IN t_uint8 *data);
   PUBLIC t_dsi_error DSI_ReadCmdCharacteristics(IN t_dsi_link dsi_id,OUT t_dsi_cmd_rd_property *p_cmd_rd);
   PUBLIC t_dsi_error DSI_Read_CmdDat(IN t_dsi_link dsi_id,OUT t_dsi_cmd_rddat *p_rddat);
   PUBLIC void DSI_SetDirectCmdSettings( IN t_dsi_link dsi_id,IN t_dsi_cmd_main_setting cmd_settings);
   PUBLIC void DSI_SetTE_Timeout(IN t_dsi_link dsi_id, IN t_uint32 te_timeout);
   PUBLIC void DSI_SetPaddingVal( IN t_dsi_link dsi_id,IN t_uint8 padding);
   PUBLIC void DSI_SetArbCtl( IN t_dsi_link dsi_id,IN t_dsi_arb_ctl arb_ctl);
   PUBLIC void DSI_SetDirectCommand(IN t_dsi_link dsi_id,IN t_uint32 cmd0,IN t_uint32 cmd1,IN t_uint32 cmd2,IN t_uint32 cmd3);
   /******************************************************************************
   Video Mode APIs
   ******************************************************************************/
   PUBLIC void DSI_SetVideoMainControl(IN t_dsi_link dsi_id,IN t_dsi_vid_main_ctl vid_ctl);
   PUBLIC void DSI_SetVideoVerticalSize(IN t_dsi_link dsi_id,IN t_dsi_img_vertical_size vid_vsize);
   PUBLIC void DSI_SetVideoHorizontalSize(IN t_dsi_link dsi_id,IN t_dsi_img_horizontal_size vid_hsize);
   PUBLIC void DSI_SetVideoPosition(IN t_dsi_link dsi_id,IN t_dsi_img_position vid_pos);
   PUBLIC t_dsi_error DSI_GetVideoModeStatus(IN t_dsi_link dsi_id,OUT t_uint16 *p_vid_mode_sts);
   PUBLIC void DSI_SetVCAControl(IN t_dsi_link dsi_id,IN t_dsi_vca_setting vca_setting);
   PUBLIC void DSI_SetBlankingControl(IN t_dsi_link dsi_id,IN t_dsi_vid_blanking blksize);
   PUBLIC void DSI_SetVID_ERRColor(IN t_dsi_link dsi_id,IN t_dsi_vid_err_color err_color);
   PUBLIC t_dsi_error DSI_SetMCTL_main_phy_ctl(IN t_dsi_link dsi_id, IN t_mctl_main_phy_ctl mctl_main_phy_ctl_reg);
   PUBLIC t_dsi_error DSI_SetMCTL_main_data_ctl(IN t_dsi_link dsi_id, IN t_mctl_main_data_ctl mctl_main_data_ctl_reg);
   PUBLIC t_dsi_error DSI_SetMCTL_main_en(IN t_dsi_link dsi_id, IN t_mctl_main_en mctl_main_en_reg);
   PUBLIC t_dsi_error DSI_SetVID_dphy_time(IN t_dsi_link dsi_id, IN t_vid_dphy_time vid_dphy_time_reg);
	
   /*****************************************************************************
   TVG and TBG APIs
   *****************************************************************************/
   PUBLIC void DSI_SetTVGControl(IN t_dsi_link dsi_id,IN t_dsi_tvg_control tvg_control);
   PUBLIC void DSI_TVGStart(IN t_dsi_link dsi_id);
   PUBLIC void DSI_TVGStop(IN t_dsi_link dsi_id);
   PUBLIC void DSI_SetTVGImgSize(IN t_dsi_link dsi_id,IN t_dsi_tvg_img_size img_size);
   PUBLIC t_dsi_error DSI_SetTVGColor(IN t_dsi_link dsi_id,IN t_dsi_color_type color_type,IN t_dsi_frame_color color);
   PUBLIC t_dsi_error DSI_GetTVGState(IN t_dsi_link dsi_id,OUT t_dsi_tvg_state *p_tvg_state);
   PUBLIC t_dsi_error DSI_GetTBGState(IN t_dsi_link dsi_id,OUT t_dsi_tbg_state *p_tbg_state);
   /******************************************************************************
   Interrupt Enable and Edge Detection APIs
   ******************************************************************************/
   PUBLIC void  DSI_MCTLMainSTSControlEnable(IN t_dsi_link dsi_id,IN t_dsi_link_status sts_ctl);
   PUBLIC void  DSI_MCTLMainSTSControlDisable(IN t_dsi_link dsi_id,IN t_dsi_link_status sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeMCTLMainSTSControl(IN t_dsi_link dsi_id,IN t_dsi_link_status sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeMCTLMainSTSControl(IN t_dsi_link dsi_id,IN t_dsi_link_status sts_ctl);
   PUBLIC void  DSI_CommandModeSTSControlEnable(IN t_dsi_link dsi_id,IN t_dsi_cmd_mode_sts sts_ctl);
   PUBLIC void  DSI_CommandModeSTSControlDisable(IN t_dsi_link dsi_id,IN t_dsi_cmd_mode_sts sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeCmdModeControl(IN t_dsi_link dsi_id,IN t_dsi_cmd_mode_sts sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeCmdModeControl(IN t_dsi_link dsi_id,IN t_dsi_cmd_mode_sts sts_ctl);
   PUBLIC void  DSI_DirectCmdSTSControlEnable(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_sts sts_ctl);
   PUBLIC void  DSI_DirectCmdSTSControlDisable(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_sts sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeDirectCmdControl(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_sts sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeDirectCmdControl(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_sts sts_ctl);
   PUBLIC void  DSI_DirectCmdRdSTSEnable(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl);
   PUBLIC void  DSI_DirectCmdRdSTSDisable(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeDirectCmdRdSTS(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeDirectCmdRdSTS(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl);
   PUBLIC void  DSI_VID_MODESTSEnable(IN t_dsi_link dsi_id,IN t_dsi_vid_mode_sts sts_ctl);
   PUBLIC void  DSI_VID_MODESTSDisable(IN t_dsi_link dsi_id,IN t_dsi_vid_mode_sts sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeVIDModeSTS(IN t_dsi_link dsi_id,IN t_dsi_vid_mode_sts sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeVIDModeSTS(IN t_dsi_link dsi_id,IN t_dsi_vid_mode_sts sts_ctl);
   PUBLIC void  DSI_TG_STS_Enable(IN t_dsi_link dsi_id,IN t_dsi_tg_sts_ctl sts_ctl);
   PUBLIC void  DSI_TG_STS_Disable(IN t_dsi_link dsi_id,IN t_dsi_tg_sts_ctl sts_ctl);
   PUBLIC void  DSI_SetFallingEdgeTGSTS(IN t_dsi_link dsi_id,IN t_dsi_tg_sts_ctl sts_ctl);
   PUBLIC void  DSI_SetRisingEdgeTGSTS(IN t_dsi_link dsi_id,IN t_dsi_tg_sts_ctl sts_ctl);
   PUBLIC void  DSI_MCTL_DPHY_ERR_Enable(IN t_dsi_link dsi_id,IN t_dsi_dphy_err err_ctl);
   PUBLIC void  DSI_MCTL_DPHY_ERR_Disable(IN t_dsi_link dsi_id,IN t_dsi_dphy_err err_ctl);
   PUBLIC void  DSI_SetFallingEdgeDPHYERR(IN t_dsi_link dsi_id,IN t_dsi_dphy_err err_ctl);
   PUBLIC void  DSI_SetRisingEdgeDPHYERR(IN t_dsi_link dsi_id,IN t_dsi_dphy_err err_ctl);
   PUBLIC void  DSI_DPHY_CLK_TRIM_RD_Enable(IN t_dsi_link dsi_id);
   PUBLIC void  DSI_DPHY_CLK_TRIM_RD_Disable(IN t_dsi_link dsi_id);
   PUBLIC void  DSI_SetFallingEdgeDPHYCLKTRIM(IN t_dsi_link dsi_id);
   PUBLIC void  DSI_SetRisingEdgeDPHYCLKTRIM(IN t_dsi_link dsi_id);
   PUBLIC void  DSI_ClearMCTL_Main_Status(IN t_dsi_link dsi_id,IN t_dsi_link_status sts_ctl);
   PUBLIC void  DSI_ClearCmd_Mode_Status(IN t_dsi_link dsi_id,IN t_dsi_cmd_mode_sts sts_ctl);
   PUBLIC void  DSI_ClearDirectCmd_Status(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_sts sts_ctl);
   PUBLIC void  DSI_ClearDirectCmdRD_Status(IN t_dsi_link dsi_id,IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl);
   PUBLIC void  DSI_ClearVIDMode_Status(IN t_dsi_link dsi_id,IN t_dsi_vid_mode_sts sts_ctl);
   PUBLIC void  DSI_ClearTG_Status(IN t_dsi_link dsi_id,IN t_dsi_tg_sts_ctl sts_ctl);
   PUBLIC void  DSI_ClearDPHY_ERR(IN t_dsi_link dsi_id,IN t_dsi_dphy_err err_ctl);
   PUBLIC void  DSI_ClearDPHY_CLK_TRIM_RD(IN t_dsi_link dsi_id);
   PUBLIC t_dsi_error  DSI_GetMCTL_Main_Status(IN t_dsi_link dsi_id,OUT t_uint8 *status);
   PUBLIC t_dsi_error  DSI_GetCmd_Mode_Status(IN t_dsi_link dsi_id,OUT t_uint8 *status);
   PUBLIC t_dsi_error  DSI_GetDirect_Cmd_Status(IN t_dsi_link dsi_id,OUT t_uint16 *status);
   PUBLIC t_dsi_error  DSI_GetDirect_Cmd_Rd_Status(IN t_dsi_link dsi_id,OUT t_uint16 *status);
   PUBLIC t_dsi_error  DSI_GetVID_Mode_Status(IN t_dsi_link dsi_id,OUT t_uint16 *status);
   PUBLIC t_dsi_error  DSI_GetTG_Status(IN t_dsi_link dsi_id,OUT t_uint8 *status);
   PUBLIC t_dsi_error  DSI_GetDPHYErr_Status(IN t_dsi_link dsi_id,OUT t_uint16 *status);
   PUBLIC t_dsi_error  DSI_GetDPHY_CLK_TRIM_RDStatus(IN t_dsi_link dsi_id,OUT t_uint8 *status);
   PUBLIC t_dsi_error  DSI_GetDirect_Cmd_ReadStatus(IN t_dsi_link dsi_id, OUT t_uint32 *status);

#ifdef __cplusplus
}   
#endif
#endif 

