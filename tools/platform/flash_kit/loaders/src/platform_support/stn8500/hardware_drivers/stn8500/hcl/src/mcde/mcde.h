
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE Driver Header
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MCDE_H_
#define _MCDE_H_

#include "hcl_defs.h"
#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif 

/*---------------------------------------------------------------------------
 * Define
 *---------------------------------------------------------------------------*/

#define MCDE_HCL_VERSION_ID      2
#define MCDE_MAJOR_ID            1
#define MCDE_MINOR_ID            4

/*******************************************************************************
 MCDE Control Enums and structures
  ******************************************************************************/
  typedef enum
  {
      MCDE_CH_A_LSB = 0x00,
      MCDE_CH_A_MID = 0x01,
      MCDE_CH_A_MSB = 0x02,
      MCDE_CH_B_LSB = 0x03,
      MCDE_CH_B_MID = 0x04,
      MCDE_CH_B_MSB = 0x05,
      MCDE_CH_C_LSB = 0x06      
  }t_mcde_out_mux_ctrl;

  typedef enum
  {
      MCDE_FIFO_FA = 0x0,
      MCDE_FIFO_FB = 0x1,
      MCDE_FIFO_F0 = 0x2,
      MCDE_FIFO_F1 = 0x3
  }t_mcde_fifo;

  typedef enum
  {
      MCDE_DPI_A = 0x0,
      MCDE_DPI_B = 0x1,
      MCDE_DBI_C0 = 0x2,
      MCDE_DBI_C1 = 0x3,
      MCDE_DSI_VID0 = 0x4,
      MCDE_DSI_VID1 = 0x5,
      MCDE_DSI_VID2 = 0x6,
      MCDE_DSI_CMD0 = 0x7,
      MCDE_DSI_CMD1 = 0x8,
      MCDE_DSI_CMD2 = 0x9
  }t_mcde_fifo_output;

  typedef struct
  {
      t_mcde_fifo_output out_fifoa;
      t_mcde_fifo_output out_fifob;
      t_mcde_fifo_output out_fifo0;
      t_mcde_fifo_output out_fifo1;
  }t_mcde_fifo_ctrl;

  typedef enum
  {
      MCDE_CH_NORMAL_MODE = 0x0,
      MCDE_CH_MUX_MODE    = 0x1
  }t_mcde_ch_ctrl;

  typedef enum
  {
      MCDE_CHIPSELECT_C0 = 0x0,
      MCDE_CHIPSELECT_C1 = 0x1
  }t_mcde_sync_mux_ctrl;

  typedef enum
  {
      MCDE_CH_A_OUT_ENABLE = 0x0,
      MCDE_CH_C_OUT_ENABLE = 0x1
  }t_mcde_sync_mux1_ctrl;

  typedef enum
  {
      MCDE_CH_A_CLK_PANEL = 0x0,
      MCDE_CH_C_CHIP_SELECT = 0x1
  }t_mcde_sync_mux0_ctrl;

  typedef enum
  {
      MCDE_CDI_CH_A = 0x0,
      MCDE_CDI_CH_B = 0x1
  }t_mcde_cdi_ctrl;

  typedef enum
  {
      MCDE_CDI_DISABLE = 0x0,
      MCDE_CDI_ENABLE = 0x1
  }t_mcde_cdi;

  typedef enum
  {
      MCDE_DISABLE = 0x0,
      MCDE_ENABLE = 0x1
  }t_mcde_state;

  typedef struct
  {
      t_mcde_out_mux_ctrl       data_msb1;
      t_mcde_out_mux_ctrl       data_msb0;
      t_mcde_out_mux_ctrl       data_mid;
      t_mcde_out_mux_ctrl       data_lsb1;
      t_mcde_out_mux_ctrl       data_lsb0;
      t_uint8                   ififo_watermark;
  }t_mcde_control;

  typedef enum
  {
      MCDE_OUTPUT_FIFO_B = 0x0,
      MCDE_OUTPUT_FIFO_C1 = 0x1
  }t_mcde_swap_b_c1_ctrl;

  typedef enum
  {
      MCDE_CONF_TVA_DPIC0_LCDB = 0x0,
      MCDE_CONF_TVB_DPIC1_LCDA = 0x1,
      MCDE_CONF_DPIC1_LCDA     = 0x2,
      MCDE_CONF_DPIC0_LCDB     = 0x3,
      MCDE_CONF_LCDA_LCDB      = 0x4
  }t_mcde_output_conf;

  typedef enum
  {
      MCDE_OUTPUT_FIFO_A = 0x0,
      MCDE_OUTPUT_FIFO_C0 = 0x1
  }t_mcde_swap_a_c0_ctrl;

  typedef struct
  {
      t_mcde_ch_ctrl sync_ctrl_chA;
      t_mcde_ch_ctrl sync_ctrl_chB;
      t_mcde_swap_a_c0_ctrl flow_ctrl_chA;
      t_mcde_swap_b_c1_ctrl flow_ctrl_chB;
  }t_mcde_ch_mode_ctrl;
  
  typedef struct
  {
      t_uint32    irq_status;
      t_uint32    event_status;
      t_bool      is_new;
  } t_mcde_irq_status;
  
  typedef enum
  {
    MCDE_EVENT_MASTER_BUS_ERROR = 0x01,
    MCDE_EVENT_END_OF_FRAME_TRANSFER       = 0x02,
    MCDE_EVENT_SYNCHRO_CAPTURE_TRIGGER     = 0x04,
    MCDE_EVENT_HORIZONTAL_SYNCHRO_CAPTURE  = 0x08,
    MCDE_EVENT_VERTICAL_SYNCHRO_CAPTURE    = 0x10
  } t_mcde_event_id;

  typedef struct
  { 
      t_uint8   pllfreq_div;
      t_uint8   datachannels_num;
      t_uint8   delay_clk;
      t_uint8   delay_d2;
      t_uint8   delay_d1;
      t_uint8   delay_d0;
  }t_mcde_cdi_delay;
/*******************************************************************************
  External Source Enums and Structures
  ******************************************************************************/
  typedef enum
  {
  	  MCDE_EXT_SRC_0,
      MCDE_EXT_SRC_1,
      MCDE_EXT_SRC_2,
      MCDE_EXT_SRC_3,
      MCDE_EXT_SRC_4
  }t_mcde_ext_src;


  typedef enum
  {
      MCDE_PIXEL_ORDER_LITTLE = 0x0,
      MCDE_PIXEL_ORDER_BIG = 0x1
  }t_mcde_pixel_order_in_byte;

  typedef enum
  {
      MCDE_BYTE_LITTLE = 0x0,
      MCDE_BYTE_BIG = 0x1
  }t_mcde_byte_endianity;

  typedef enum
  {
      MCDE_COL_RGB = 0x0,
      MCDE_COL_BGR = 0x1
  }t_mcde_rgb_format_sel;

  typedef enum
  {
      MCDE_PAL_1_BIT = 0x0,
      MCDE_PAL_2_BIT = 0x1,
      MCDE_PAL_4_BIT = 0x2,
      MCDE_PAL_8_BIT = 0x3,
      MCDE_RGB444_12_BIT = 0x4,
      MCDE_ARGB_16_BIT = 0x5,
      MCDE_IRGB1555_16_BIT = 0x6,
      MCDE_RGB565_16_BIT = 0x7,
      MCDE_RGB_PACKED_24_BIT = 0x8,
      MCDE_RGB_UNPACKED_24_BIT = 0x9,
      MCDE_ARGB_32_BIT =0xA,
      MCDE_YCbCr_8_BIT = 0xB
    }t_mcde_bpp_ctrl;


  typedef enum
  {
      MCDE_OVERLAY_0,
      MCDE_OVERLAY_1,
      MCDE_OVERLAY_2,
      MCDE_OVERLAY_3,
      MCDE_OVERLAY_4,
      MCDE_OVERLAY_5,
      MCDE_OVERLAY_6,
      MCDE_OVERLAY_7
  }t_mcde_overlay_id;

  typedef enum
  {
      MCDE_FS_FREQ_UNCHANGED = 0x0,
      MCDE_FS_FREQ_DIV_2  = 0x1
  }t_mcde_fs_div;

  typedef enum
  {
      MCDE_FS_FREQ_DIV_ENABLE = 0x0,
      MCDE_FS_FREQ_DIV_DISABLE = 0x1
  }t_mcde_fs_ctrl;

  typedef enum
  {
      MCDE_BUFFER_USED_NONE = 0x0,
      MCDE_BUFFER_USED_1 = 0x1,
      MCDE_BUFFER_USED_2 = 0x2,
      MCDE_BUFFER_USED_3 = 0x3
  }t_mcde_num_buffer_used;

  typedef enum
  {
      MCDE_BUFFER_ID_0 = 0x0,
      MCDE_BUFFER_ID_1 = 0x1,
      MCDE_BUFFER_ID_2 = 0x2,
      MCDE_BUFFER_ID_RESERVED
  }t_mcde_buffer_id;

  typedef enum
  {
      MCDE_MULTI_CH_CTRL_ALL_OVR = 0x0,
      MCDE_MULTI_CH_CTRL_PRIMARY_OVR = 0x1
  }t_mcde_multi_ovr_ctrl;

  typedef enum
  {
      MCDE_BUFFER_SEL_EXT = 0x0,
      MCDE_BUFFER_AUTO_TOGGLE = 0x1,
      MCDE_BUFFER_SOFTWARE_SELECT = 0x2,
      MCDE_BUFFER_RESERVED
  }t_mcde_buffer_sel_mode;

  typedef struct
  {
      t_mcde_pixel_order_in_byte ovr_pxlorder;
      t_mcde_byte_endianity      endianity;
      t_mcde_rgb_format_sel      rgb_format;
      t_mcde_bpp_ctrl           bpp;
      t_mcde_overlay_id         ovr_id;
      t_mcde_num_buffer_used    buf_num;
      t_mcde_buffer_id          buf_id;
  }t_mcde_ext_conf;

  typedef struct
  {
      t_mcde_fs_div            fs_div;
      t_mcde_fs_ctrl           fs_ctrl;
      t_mcde_multi_ovr_ctrl    ovr_ctrl;
      t_mcde_buffer_sel_mode   sel_mode;
  }t_mcde_ext_src_ctrl;

  
/*******************************************************************************
  Overlay Enums and Structures
  ******************************************************************************/
  typedef enum
  {
      MCDE_ROTATE_BURST_WORD_1 = 0x0,
      MCDE_ROTATE_BURST_WORD_2 = 0x1,
      MCDE_ROTATE_BURST_WORD_4 = 0x2,
      MCDE_ROTATE_BURST_WORD_8 = 0x3,
      MCDE_ROTATE_BURST_WORD_16 = 0x04,
      MCDE_ROTATE_BURST_WORD_RESERVED
  }t_mcde_rotate_req;

  typedef enum
  {
      MCDE_OUTSTND_REQ_1 = 0x0,
      MCDE_OUTSTND_REQ_2 = 0x1,
      MCDE_OUTSTND_REQ_4 = 0x2,
      MCDE_OUTSTND_REQ_8 = 0x3,
      MCDE_OUTSTND_REQ_16 = 0x4,
      MCDE_OUTSTND_REQ_RESERVED
  }t_mcde_outsnd_req;

/*  typedef enum
  {
      MCDE_OVERLAY_0,
      MCDE_OVERLAY_1,
      MCDE_OVERLAY_2,
      MCDE_OVERLAY_3,
      MCDE_OVERLAY_4,
      MCDE_OVERLAY_5,
      MCDE_OVERLAY_6,
      MCDE_OVERLAY_7
  }t_mcde_overlay_id; */

  typedef enum
  {
      MCDE_BURST_WORD_1 = 0x00,
      MCDE_BURST_WORD_2 = 0x01,
      MCDE_BURST_WORD_4 = 0x02,
      MCDE_BURST_WORD_8 = 0x03,
      MCDE_BURST_WORD_16 = 0x04,
      MCDE_BURST_WORD_HW_1 = 0x8,
      MCDE_BURST_WORD_HW_2 = 0x09,
      MCDE_BURST_WORD_HW_4 = 0x0A,
      MCDE_BURST_WORD_HW_8 = 0x0B,
      MCDE_BURST_WORD_HW_16 = 0x0C
  }t_mcde_burst_req;

  typedef enum
  {
      MCDE_COLOR_KEY_DISABLE = 0x0,
      MCDE_COLOR_KEY_ENABLE = 0x01
  }t_mcde_color_key_ctrl;

  typedef enum
  {
      MCDE_PAL_GAMA_DISABLE = 0x0,
      MCDE_GAMA_ENABLE = 0x1,
      MCDE_PAL_ENABLE = 0x2,
      MCDE_PAL_GAMA_RESERVED
  }t_mcde_pal_ctrl;

  typedef enum
  {
      MCDE_COL_CONV_DISABLE = 0x0,
      MCDE_COL_CONV_NOT_SAT = 0x1,
      MCDE_COL_CONV_SAT = 0x2,
      MCDE_COL_CONV_RESERVED
  }t_mcde_col_conv_ctrl;

  typedef enum
  {
      MCDE_OVERLAY_DISABLE = 0x0,
      MCDE_OVERLAY_ENABLE = 0x1
  }t_mcde_overlay_ctrl;

  typedef enum
  {
      MCDE_PIXEL_ALPHA_SOURCE = 0x0,
      MCDE_CONST_ALPHA_SOURCE = 0x1
  }t_mcde_blend_ctrl;

  typedef enum
  {
      MCDE_OVR_OPAQUE_DISABLE = 0x0,
      MCDE_OVR_OPAQUE_ENABLE = 0x1
  }t_mcde_ovr_opq_ctrl;

 /* typedef enum
  {
      MCDE_OVR_INTERLACE_DISABLE = 0x0,
      MCDE_OVR_INTERLACE_ENABLE = 0x1
  }t_mcde_ovr_interlace_ctrl;

  typedef enum
  {
      MCDE_OVR_INTERLACE_TOPFIELD = 0x0,
      MCDE_OVR_INTERLACE_BOTTOMFIELD = 0x1
  }t_mcde_ovr_interlace_mode;*/
  
  typedef enum
  {
      MCDE_CH_A = 0x0,
      MCDE_CH_B = 0x1,
      MCDE_CH_C0 = 0x2,
      MCDE_CH_C1 = 0x3
  }t_mcde_ch_id;

  typedef enum
  {
      MCDE_FETCH_INPROGRESS = 0x0,
      MCDE_FETCH_COMPLETE = 0x1
  }t_mcde_ovr_fetch_status;

  typedef enum
  {
      MCDE_OVR_READ_COMPLETE = 0x0,
      MCDE_OVR_READ_INPROGRESS = 0x1
  }t_mcde_ovr_read_status;

  typedef enum
  {
      MCDE_OVR_PREMULTIPLIED_ALPHA_DISABLE = 0x0,
      MCDE_OVR_PREMULTIPLIED_ALPHA_ENABLE    = 0x1
  }t_mcde_ovr_alpha_enable;

  typedef enum
  {
      MCDE_OVR_CLIP_DISABLE = 0x0,
      MCDE_OVR_CLIP_ENABLE  = 0x1
  }t_mcde_ovr_clip_enable;

  typedef struct
  {
      t_mcde_rotate_req     rot_burst_req;
      t_mcde_outsnd_req     outstnd_req;
      t_mcde_burst_req      burst_req;
      t_uint8               priority;
      t_mcde_color_key_ctrl color_key;
      t_mcde_col_conv_ctrl  col_ctrl;
      t_mcde_overlay_ctrl   ovr_state;
      t_mcde_ovr_alpha_enable alpha;
      #ifdef ST_8500ED
      t_mcde_pal_ctrl       pal_control;
      t_mcde_ovr_clip_enable  clip;
      #endif
  }t_mcde_ovr_control;

  typedef struct
  {
      t_uint32 ytlcoor;
      t_uint32 xtlcoor;
      t_uint32 ybrcoor;
      t_uint32 xbrcoor;
  }t_mcde_ovr_clip;

  typedef struct
  {
      t_uint32      line_per_frame;
    /*  t_mcde_ovr_interlace_mode  ovr_interlace;
      t_mcde_ovr_interlace_ctrl  ovr_intlace_ctrl;*/
      t_mcde_ext_src        src_id;
      t_uint16              ovr_ppl;
  }t_mcde_ovr_config;

  typedef struct
  {
      t_uint8               alpha_value;
      t_uint8               pixoff;
      t_mcde_ovr_opq_ctrl        ovr_opaq;
      t_mcde_blend_ctrl     ovr_blend;
      t_uint32 watermark_level;
  }t_mcde_ovr_conf2;

  typedef struct
  {
      t_uint8        ovr_zlevel;
      t_uint16        ovr_ypos;
      t_mcde_ch_id   ch_id;
      t_uint16       ovr_xpos;
  }t_mcde_ovr_comp;
  
  typedef struct
  {
      t_uint16        ovr_ypos;
      t_uint16        ovr_xpos;
  }t_mcde_ovr_xy;

  typedef enum
  {
      MCDE_OVERLAY_NOT_BLOCKED = 0x0,
      MCDE_OVERLAY_BLOCKED     = 0x1
  }t_mcde_ovr_blocked_status;

  typedef struct
  { 
      t_mcde_ovr_blocked_status ovrb_status;
      t_mcde_ovr_fetch_status   ovr_status;
      t_mcde_ovr_read_status    ovr_read;
  }t_mcde_ovr_status;

  /*******************************************************************************
   Channel Configuration Enums and Structures
   ******************************************************************************/
  typedef enum
  {
      MCDE_VERTICAL_SYNCHRO = 0x00,
      MCDE_BACK_PORCH = 0x01,
      MCDE_ACTIVE_VIDEO = 0x02,
      MCDE_FRONT_PORCH = 0x03
  }t_mcde_frame_events;

  typedef enum
  {
      MCDE_LCD_TV_0 = 0x0,
      MCDE_LCD_TV_1 = 0x1,
      MCDE_MDIF_IN_0 = 0x2,
      MCDE_MDIF_IN_1 = 0x3,
      MCDE_MDIF_OUT_0 = 0x4,
      MCDE_MDIF_OUT_1 = 0x5
  }t_mcde_synchro_out_interface;

  typedef enum
  {
      MCDE_SYNCHRO_OUTPUT_SOURCE = 0x0,
      MCDE_SYNCHRO_AUTO = 0x1,
      MCDE_SYNCHRO_SOFTWARE = 0x2,
      MCDE_SYNCHRO_EXTERNAL_SOURCE = 0x3
  }t_mcde_synchro_source;

  typedef enum
  {
      MCDE_NO_ACTION = 0x0,
      MCDE_NEW_FRAME_SYNCHRO = 0x1
  }t_mcde_sw_trigger;

 /* typedef struct
  {
      t_uint16  InitDelay;
      t_uint16  PPDelay;
  }t_mcde_ch_delay;*/

  typedef struct
  {
      t_uint16  chx_lpf;
      t_uint16  chx_ppl;
  }t_mcde_chx_config;

  typedef struct
  {
      t_uint16             swframe_intrdelay;
      t_mcde_frame_events  frame_intrevent;
      t_uint16             autoframe_delay;
      t_mcde_frame_events  frame_synchroevnt;
  }t_mcde_chsyncconf;

  typedef struct
  {
      t_mcde_synchro_out_interface    out_synch_interface;
      t_mcde_synchro_source           ch_synch_src;
  }t_mcde_chsyncmod;

  typedef struct
  {
      t_uint8  red;
      t_uint8  green;
      t_uint8  blue;
  }t_mcde_ch_bckgrnd_col;

  typedef struct
  {
      t_uint8  ch_priority;
  }t_mcde_ch_priority;

  typedef enum
  {
      MCDE_CHNL_RUNNING_NORMALLY = 0x0,
      MCDE_CHNL_ABORT_OCCURRED   = 0x1
  }t_mcde_chnl_abort_state;

  typedef enum
  {
      MCDE_CHNL_READ_ONGOING = 0x0,
      MCDE_CHNL_READ_DONE    = 0x1
  }t_mcde_chnl_read_status;

  typedef struct
  {
      t_mcde_chnl_abort_state   abort_state;
      t_mcde_chnl_read_status   read_state;
  }t_mcde_chnl_state;
  /*******************************************************************************
 Channel A/B Enums and Structures
  ******************************************************************************/
  
  typedef enum
  {
  	  MCDE_CLOCKWISE = 0x0,
  	  MCDE_ANTICLOCKWISE = 0x1
  }t_mcde_rot_dir;
  
  typedef enum
  {
      MCDE_GAMMA_ENABLE = 0x0,
      MCDE_GAMMA_DISABLE = 0x1
  }t_mcde_gamma_ctrl;

  typedef enum
  {
  	  MCDE_INPUT_YCrCb = 0x0,
  	  MCDE_INPUT_RGB = 0x1
  }t_mcde_flicker_format;  
  
  typedef enum
  {
  	  MCDE_ALPHA_INPUTSRC = 0x0,
  	  MCDE_ALPHA_REGISTER = 0x1
  }t_mcde_blend_control;
  
  typedef enum
  {
  	  MCDE_FORCE_FILTER0 = 0x0,
  	  MCDE_ADAPTIVE_FILTER = 0x1,
  	  MCDE_TEST_MODE = 0x2
  }t_mcde_flicker_filter_mode;
  
  typedef enum
  {
  	  MCDE_CLR_KEY_DISABLE = 0x0,
  	  MCDE_ALPHA_RGB_KEY = 0x1,
  	  MCDE_RGB_KEY = 0x2,
  	  MCDE_FALPHA_FRGB_KEY = 0x4,
  	  MCDE_FRGB_KEY = 0x5 
  }t_mcde_key_ctrl;
  
  typedef enum
  {
  	  MCDE_ROTATION_DISABLE = 0x0,
  	  MCDE_ROTATION_ENABLE = 0x1
  }t_mcde_roten;
  
  typedef enum
  {
      MCDE_DITHERING_DISABLE = 0x0,
      MCDE_DITHERING_ENABLE = 0x1	
  }t_mcde_dithering_ctrl;
  
  typedef enum
  {
  	  MCDE_CLR_ENHANCE_DISABLE = 0x0,
  	  MCDE_CLR_ENHANCE_ENABLE = 0x1
  }t_mcde_clr_enhance_ctrl;
  
  typedef enum
  {
      MCDE_ANTIFLICKER_DISABLE = 0x0,
      MCDE_ANTIFLICKER_ENABLE = 0x1
  }t_mcde_antiflicker_ctrl;
  
  typedef enum
  {
  	  MCDE_BLEND_DISABLE = 0x0,
  	  MCDE_BLEND_ENABLE = 0x1
  }t_mcde_blend_status;
  
  typedef enum
  {
  	  MCDE_TVCLK_EXTERNAL = 0x0,
  	  MCDE_TVCLK_INTERNAL = 0x1
  }t_mcde_tv_clk;
  
  typedef enum
  {
  	  MCDE_PCD_ENABLE = 0x0,
  	  MCDE_PCD_BYPASS = 0x1
  }t_mcde_bcd_ctrl;
  
  typedef enum
  {
  	  MCDE_BPP_MONOCHROME_1 = 0x0,
  	  MCDE_BPP_MONOCHROME_2 = 0x1,
      MCDE_BPP_MONOCHROME_4 = 0x2,
      MCDE_BPP_MONOCHROME_8 = 0x3,
      MCDE_BPP_8  = 0x4,
      MCDE_BPP_12 = 0x5,
      MCDE_BPP_15 = 0x6,
  	  MCDE_BPP_16 = 0x7,
  	  MCDE_BPP_18 = 0x8,
  	  MCDE_BPP_24 = 0x9
  }t_mcde_out_bpp;
  
  typedef enum
  {
  	  MCDE_BUS_16_CONF1 = 0x0,
  	  MCDE_BUS_16_CONF2 = 0x1,
  	  MCDE_BUS_16_CONF3 = 0x2,
  	  MCDE_BUS_18_CONF1 = 0x3,
  	  MCDE_BUS_18_CONF2 = 0x4,
  	  MCDE_BUS_24       = 0x5
  }t_mcde_lcd_bus;
  
  typedef enum
  {
  	  MCDE_CLK_4_TO_72_83 = 0x0,
  	  MCDE_CLK_74 = 0x1,
  	  MCDE_CLK_27 = 0x2,
  	  MCDE_CLK_TVCLK1 = 0x3,
  	  MCDE_CLK_TVCLK2 = 0x4,
  	  MCDE_CLK_166 = 0x5
  }t_mcde_dpi2_clksel;
  
  typedef enum
  {
  	  MCDE_SDTV_656P = 0x0,
  	  MCDE_HDTV_480P = 0x1,
  	  MCDE_HDTV_720P = 0x2,
      MCDE_TVMODE_NOTUSED = 0x3
  }t_mcde_tvmode;
  
  typedef enum
  {
  	  MCDE_ACTIVE_HIGH = 0x0,
  	  MCDE_ACTIVE_LOW = 0x1
  }t_mcde_signal_level;
  
  typedef enum
  {
  	  MCDE_MODE_LCD = 0x0,
  	  MCDE_MODE_TV = 0x1
  }t_mcde_display_mode;
  
  typedef enum
  {
      MCDE_VERTICAL_ACTIVE_FRAME_DISABLE = 0x0,
      MCDE_ALL_FRAME_ENABLE = 0x1	  
  }t_mcde_va_enable;
  
  typedef enum
  {
      MCDE_OUTPUT_NORMAL = 0x0,
      MCDE_OUTPUT_TOGGLE = 0x1
  }t_mcde_toggle_enable;
  
  typedef enum
  {
  	  MCDE_SYNCHRO_HBP = 0x0,
  	  MCDE_SYNCHRO_CLP = 0x1,
  	  MCDE_SYNCHRO_HFP = 0x2,
  	  MCDE_SYNCHRO_HSW = 0x3
  }t_mcde_loadsel;
  
  typedef enum
  {
      MCDE_DATA_RISING_EDGE = 0x0,
      MCDE_DATA_FALLING_EDGE = 0x1
  }t_mcde_data_lines;
  
  typedef enum
  {
  	  MCDE_MASK_DISABLE = 0x0,
  	  MCDE_MASK_ENABLE = 0x1
  }t_mcde_masking_bit_ctrl;
  
  typedef enum
  {
  	  MCDE_DITHERING_RESET = 0x0,
  	  MCDE_DITHERING_ACTIVATE = 0x1
  }t_mcde_dithering_control;
  
  typedef struct
  {
      t_mcde_rotate_req    chx_read_request;
      t_uint8              alpha_blend;
      t_mcde_rot_dir       rot_dir;
      t_mcde_gamma_ctrl    gamma_ctrl;
      t_mcde_flicker_format flicker_format;
      t_mcde_flicker_filter_mode  filter_mode;
      t_mcde_blend_control     blend;
      t_mcde_key_ctrl       key_ctrl;
      t_mcde_roten          rot_enable;
      t_mcde_dithering_control dither_ctrl;
      t_mcde_clr_enhance_ctrl color_enhance;
      t_mcde_antiflicker_ctrl anti_flicker;
      t_mcde_blend_status       blend_ctrl;
  }t_mcde_chx_control0;
  
  typedef struct
  {
      t_mcde_tv_clk   tv_clk;
      t_mcde_bcd_ctrl bcd_ctrl;
      t_mcde_out_bpp  out_bpp;
      t_uint16        clk_per_line;
      t_mcde_lcd_bus  lcd_bus;
      t_mcde_dpi2_clksel dpi2_clk;
      t_uint16        pcd;
  }t_mcde_chx_control1;
  
  typedef struct
  {
  	  t_uint8  alpha;
  	  t_uint8  red;
  	  t_uint8  green;
  	  t_uint8  blue;
  }t_mcde_color_key;
  
  typedef struct
  {
  	  t_uint8  f_alpha;
  	  t_uint8  f_red;
  	  t_uint8  f_green;
  	  t_uint8  f_blue;
  }t_mcde_forced_color_key;
  
  typedef struct
  {
  	  t_uint16  Yr_red;
  	  t_uint16  Yr_green;
  	  t_uint16  Yr_blue;
  	  t_uint16  Cr_red;
  	  t_uint16  Cr_green;
  	  t_uint16  Cr_blue;
  	  t_uint16  Cb_red;
  	  t_uint16  Cb_green;
  	  t_uint16  Cb_blue;
  	  t_uint16  Off_red;
  	  t_uint16  Off_green;
  	  t_uint16  Off_blue;
  }t_mcde_rgb_conv_coef;
  
  typedef struct
  {
  	  t_uint8  threshold_ctrl0;
  	  t_uint8  threshold_ctrl1;
  	  t_uint8  threshold_ctrl2;
  	  t_uint8  Coeff0_N3;
  	  t_uint8  Coeff0_N2;
  	  t_uint8  Coeff0_N1;
  	  t_uint8  Coeff1_N3;
  	  t_uint8  Coeff1_N2;
  	  t_uint8  Coeff1_N1;
  	  t_uint8  Coeff2_N3;
  	  t_uint8  Coeff2_N2;
  	  t_uint8  Coeff2_N1;
  }t_mcde_filter_coef;
  
  typedef struct
  {
  	  t_uint16  num_lines;
  	  t_mcde_tvmode tv_mode;
  	  t_mcde_signal_level ifield;
  	  t_mcde_state    interlace_enable;
  	  t_mcde_display_mode sel_mode;
  }t_mcde_tv_control;
  
  typedef struct
  {
  	  t_uint16  blanking_start;
  	  t_uint16  blanking_end;
  }t_mcde_tv_blanking_field;
  
  typedef struct
  {
  	  t_uint16  field2_start_line;
  	  t_uint16  field1_start_line;
  }t_mcde_tv_start_line;
  
  typedef struct
  {
  	  t_uint16  field2_window_offset;
  	  t_uint16  field1_window_offset;
  }t_mcde_tv_dvo_offset;
  
  typedef struct
  {
  	  t_uint16  tv_swh2;
  	  t_uint16  tv_swh1;
  }t_mcde_tv_swh_time;
  
  typedef struct
  {
      t_uint16  src_window_width;
      t_uint16  destination_hor_offset;	  
  }t_mcde_tv_timing1;
  
  typedef struct
  {
  	  t_uint16 active_line_width;
  	  t_uint16 line_blanking_width;
  }t_mcde_tv_lbalw_timing;
  
  typedef struct
  {
  	  t_uint8  background_cr;
  	  t_uint8  background_cb;
  	  t_uint8  background_lu;
  }t_mcde_tv_background_time;
  
  typedef struct
  {
  	  t_mcde_va_enable  rev_va_enable;
  	  t_mcde_toggle_enable  rev_toggle_enable;
  	  t_mcde_loadsel  rev_sync_sel;
  	  t_uint8  rev_delay1;
  	  t_uint8  rev_delay0;
  	  t_mcde_va_enable  ps_va_enable;
  	  t_mcde_toggle_enable  ps_toggle_enable;
  	  t_mcde_loadsel  ps_sync_sel;
  	  t_uint8  ps_delay1;
  	  t_uint8  ps_delay0;
  }t_mcde_lcd_timing0;
  
  typedef struct
  {
      t_mcde_signal_level   io_enable;
      t_mcde_data_lines     ipc;
      t_mcde_signal_level   ihs;
  	  t_mcde_signal_level   ivs;
  	  t_mcde_signal_level   ivp;
  	  t_mcde_signal_level   iclspl;
  	  t_mcde_signal_level   iclrev;
  	  t_mcde_signal_level   iclsp;
  	  t_mcde_va_enable      mcde_spl;
  	  t_mcde_toggle_enable  spltgen;
  	  t_mcde_loadsel        spl_sync_sel;
  	  t_uint8               spl_delay1;
  	  t_uint8               spl_delay0; 
  }t_mcde_lcd_timing1;
  
  typedef struct
  {
  	  t_uint8    y_offset;
  	  t_uint8    x_offset;
  	  t_mcde_masking_bit_ctrl  masking_ctrl;
  	  t_uint8  mode;
  	  t_mcde_dithering_ctrl comp_dithering;
  	  t_mcde_dithering_ctrl temp_dithering;
  }t_mcde_dither_ctrl;
  
  typedef struct
  {
  	  t_uint8  y_offset_rb;
  	  t_uint8  x_offset_rb;
  	  t_uint8  y_offset_rg;
  	  t_uint8  x_offset_rg;
  }t_mcde_dithering_offset;
  
  typedef struct
  {
  	  t_uint16  alphared;
  	  t_uint16  green;
  	  t_uint16  blue;
  }t_mcde_palette;
  
  typedef struct
  {
  	  t_uint32  red;
  	  t_uint32  green;
  	  t_uint32  blue;
  }t_mcde_gamma;

  /*******************************************************************************
   Channel C Enums
   ******************************************************************************/

  typedef enum
  {
      MCDE_SYNCHRO_NONE = 0x0,
      MCDE_SYNCHRO_C0 = 0x1,
      MCDE_SYNCHRO_C1 = 0x2,
      MCDE_SYNCHRO_PINGPONG = 0x3
  }t_mcde_sync_ctrl;

  typedef enum
  {
      MCDE_SIG_INACTIVE_POL_LOW = 0x0,
      MCDE_SIG_INACTIVE_POL_HIGH = 0x1
  }t_mcde_sig_pol;

  typedef enum
  {
      MCDE_CD_LOW = 0x0, /* CD low for data and high for command */
      MCDE_CD_HIGH = 0x1 /* CD high for data and low for command */
  }t_mcde_cd_polarity;

  typedef enum
  {
      MCDE_RES_INACTIVE = 0x0,
      MCDE_RES_ACTIVE = 0x1
  }t_mcde_resen;

  typedef enum
  {
      MCDE_CSEN_DEACTIVATED = 0x0,
      MCDE_CSEN_ACTIVATED = 0x1
  }t_mcde_cs_enable_rw;

  typedef enum
  {
      MCDE_PCLK_166 = 0x0,
      MCDE_PCLK_48 = 0x1,
      MCDE_PCLK_4TO72TO83 = 0x2
  }t_mcde_clk_sel;

  typedef enum
  {
      MCDE_SELECT_OUTBAND = 0x0,
      MCDE_SELECT_INBAND = 0x1,
  }t_mcde_inband_select;

  typedef enum
  {
      MCDE_BUS_SIZE_8 = 0x0,
      MCDE_BUS_SIZE_16 = 0x1
  }t_mcde_bus_size;

  typedef enum
  {
      MCDE_SYNCHRO_CAPTURE_DISABLE = 0x0,
      MCDE_SYNCHRO_CAPTURE_ENABLE = 0x1
  }t_mcde_synchro_capture;

  typedef enum
  {
      MCDE_VERTICAL_SYNCHRO_CAPTURE1 = 0x0,
      MCDE_VERTICAL_SYNCHRO_CHANELA = 0x1,
  }t_mcde_synchro_select;

  typedef enum
  {
      MCDE_FIFO_WMLVL_4 = 0x0,
      MCDE_FIFO_WMLVL_8 = 0x1
  }t_mcde_fifo_wmlvl_sel;

  typedef enum
  {
      MCDE_CHANEL_C_DISABLE = 0x0,
      MCDE_CHANEL_C_ENABLE = 0x1
  }t_mcde_chc_enable;

  typedef enum
  {
      MCDE_POWER_DISABLE = 0x0,
      MCDE_POWER_ENABLE = 0x1
  }t_mcde_powen_select;

  typedef enum
  {
      MCDE_FLOW_DISABLE = 0x0,
      MCDE_FLOW_ENABLE = 0x1
  }t_mcde_flow_select;

  typedef enum
  {
      MCDE_DUPLX_DISABLE = 0x0,
      MCDE_DUPLX_ENABLE  = 0x1
  }t_mcde_duplx_ctrl;

  typedef enum
  {
      MCDE_DUPLX_MODE_NONE = 0x0,
      MCDE_DUPLX_MODE_16_TO_32 = 0x1,
      MCDE_DUPLX_MODE_24_TO_32_RS = 0x2,
      MCDE_DUPLX_MODE_24_TO_32_LS = 0x3
  }t_mcde_duplx_mode_select;

  typedef enum
  {
      MCDE_TRANSFER_8_1 = 0x0,
      MCDE_TRANSFER_8_2 = 0x1,
      MCDE_TRANSFER_8_3 = 0x2,
      MCDE_TRANSFER_16_1 = 0x4,
      MCDE_TRANSFER_16_2 = 0x5
  }t_mcde_bit_segmentation_select;

  typedef enum
  {
      MCDE_TRANSACTION_COMMAND = 0x0,
      MCDE_TRANSACTION_DATA = 0x1
  }t_mcde_transaction_type;

  typedef enum
  {
      MCDE_VSYNC_SELECT = 0x0,
      MCDE_HSYNC_SELECT = 0x1
  }t_mcde_vertical_sync_sel;

  typedef enum
  {
      MCDE_VSYNC_ACTIVE_HIGH = 0x0,
      MCDE_VSYNC_ACTIVE_LOW = 0x1,
  }t_mcde_vertical_sync_polarity;

  typedef enum
  {
      MCDE_STBCLK_DIV_1 = 0x0,
      MCDE_STBCLK_DIV_2 = 0x1,
      MCDE_STBCLK_DIV_4 = 0x2,
      MCDE_STBCLK_DIV_8 = 0x3,
      MCDE_STBCLK_DIV_16 = 0x4,
      MCDE_STBCLK_DIV_32 = 0x5,
      MCDE_STBCLK_DIV_64 = 0x6,
      MCDE_STBCLK_DIV_128 = 0x7
  }t_mcde_synchro_clk_div_factor;

  typedef enum
  {
      MCDE_PANEL_INTEL_SERIES = 0x0,
      MCDE_PANEL_MOTOROLA_SERIES = 0x1
  }t_mcde_panel_protocol;

  typedef enum
  {
      MCDE_PANEL_C0 = 0x0,
      MCDE_PANEL_C1 = 0x1
  }t_mcde_chc_panel;
  
  typedef enum
 {
    MCDE_TXFIFO_WRITE_DATA           = 0,
    MCDE_TXFIFO_READ_DATA,
    MCDE_TXFIFO_WRITE_COMMAND
 } t_mcde_txfifo_request_type;


  typedef struct
  {
      t_mcde_sync_ctrl sync;
      t_mcde_resen    resen;
      t_mcde_synchro_select  synsel;
      t_mcde_clk_sel   clksel;
  }t_mcde_chc_ctrl;

  typedef struct
  {
      t_mcde_sig_pol  res_pol;
      t_mcde_sig_pol  rd_pol;
      t_mcde_sig_pol  wr_pol;
      t_mcde_cd_polarity  cd_pol;
      t_mcde_sig_pol  cs_pol;
      t_mcde_cs_enable_rw csen;
      t_mcde_inband_select inband_mode;
      t_mcde_bus_size      bus_size;
      t_mcde_synchro_capture syncen;
      t_mcde_fifo_wmlvl_sel  fifo_watermark;
      t_mcde_chc_enable      chcen;    
  }t_mcde_chc_config;

  typedef struct
  {
      t_mcde_duplx_ctrl       duplex_ctrl;
      t_mcde_duplx_mode_select  duplex_mode;
      t_mcde_bit_segmentation_select data_segment;
      t_mcde_bit_segmentation_select cmd_segment;
  }t_mcde_pbc_config;

  typedef struct
  {
      t_uint32  imux0;
      t_uint32  imux1;
      t_uint32  imux2;
      t_uint32  imux3;
      t_uint32  imux4;
  }t_mcde_pbc_mux;

  typedef struct
  {
      t_uint32  bit_ctrl0;
      t_uint32  bit_ctrl1;
  }t_mcde_pbc_bitctrl;

  typedef struct
  {
      t_uint8  debounce_length;
      t_mcde_vertical_sync_sel sync_sel;
      t_mcde_vertical_sync_polarity sync_pol;
      t_mcde_synchro_clk_div_factor clk_div;
      t_uint16  vsp_max;
      t_uint16 vsp_min;
  }t_mcde_sync_conf;

  typedef struct
  {
      t_uint16 trigger_delay_cx;
      t_uint8  sync_delay_c1;
      t_uint8  sync_delay_c2;
  }t_mcde_sync_trigger;

  typedef struct
  {
      t_uint8 cs_cd_deactivate;
      t_uint8 cs_cd_activate;
  }t_mcde_cd_timing_activate;

  typedef struct
  {
      t_mcde_panel_protocol panel_type;
      t_uint8  readwrite_activate;
      t_uint8  readwrite_deactivate;
  }t_mcde_rw_timing;

  typedef struct
  {
      t_uint8  data_out_deactivate;
      t_uint8  data_out_activate;
  }t_mcde_data_out_timing;
 
 typedef t_uint32    t_mcde_event;


/*********************************************************************
DSIX typedefs
*********************************************************************/
typedef enum
{
    MCDE_DSI_CH_VID0 = 0x0,
    MCDE_DSI_CH_CMD0 = 0x1,
    MCDE_DSI_CH_VID1 = 0x2,
    MCDE_DSI_CH_CMD1 = 0x3,
    MCDE_DSI_CH_VID2 = 0x4,
    MCDE_DSI_CH_CMD2 = 0x5
}t_mcde_dsi_channel;

typedef enum
{
    MCDE_PLL_OUT_OFF = 0x0,
    MCDE_PLL_OUT_1   = 0x1,
    MCDE_PLL_OUT_2   = 0x2,
    MCDE_PLL_OUT_4   = 0x3
}t_mcde_pll_div_sel;

typedef enum
{
    MCDE_CLK27 = 0x0,
    MCDE_HDMICLK = 0x1,
    MCDE_TV1CLK = 0x2,
    MCDE_TV2CLK = 0x3
}t_mcde_pll_ref_clk;

typedef enum
{
    MCDE_DSI_CLK27 = 0x0,
    MCDE_DSI_MCDECLK = 0x1
}t_mcde_clk_divider;

typedef struct
{
    t_mcde_pll_div_sel  pllout_divsel2;
    t_mcde_pll_div_sel  pllout_divsel1;
    t_mcde_pll_div_sel  pllout_divsel0;
    t_mcde_pll_ref_clk  pll4in_sel;
    t_mcde_clk_divider  txescdiv_sel;
    t_uint32            txescdiv;
}t_mcde_dsi_clk_config;

typedef enum
{
    MCDE_PACKING_RGB565 = 0x0,
    MCDE_PACKING_RGB666 = 0x1,
    MCDE_PACKING_RGB888_R = 0x2,
    MCDE_PACKING_RGB888_B = 0x3,
    MCDE_PACKING_HDTV     = 0x4
}t_mcde_dsi_packing;

typedef enum
{
    MCDE_DSI_OUT_GENERIC_CMD = 0x0,
    MCDE_DSI_OUT_VIDEO_DCS   = 0x1
}t_mcde_dsi_synchro;

typedef enum
{
    MCDE_DSI_NO_SWAP = 0x0,
    MCDE_DSI_SWAP    = 0x1,
}t_mcde_dsi_swap;

typedef enum
{
    MCDE_DSI_CMD_16  = 0x0,
    MCDE_DSI_CMD_8   = 0x1
}t_mcde_dsi_cmd_mode;

typedef enum
{
    MCDE_DSI_CMD_MODE = 0x0,
    MCDE_DSI_VID_MODE = 0x1,
}t_mcde_vid_mode;

typedef struct
{
    t_mcde_dsi_packing  packing;
    t_mcde_dsi_synchro  synchro;
    t_mcde_dsi_swap     byte_swap;
    t_mcde_dsi_swap     bit_swap;
    t_mcde_dsi_cmd_mode cmd_mode;
    t_mcde_vid_mode     vid_mode;
    t_uint8             blanking;
    t_uint32            words_per_frame;
    t_uint32            words_per_packet;
}t_mcde_dsi_conf;



  /*******************************************************************************
   MCDE Error Enums
   ******************************************************************************/
  typedef enum
  {
      MCDE_OK 							    = HCL_OK,	    /* No error.*/
      MCDE_NO_PENDING_EVENT_ERROR			= HCL_NO_PENDING_EVENT_ERROR,
      MCDE_NO_MORE_FILTER_PENDING_EVENT	    = HCL_NO_MORE_FILTER_PENDING_EVENT,
      MCDE_NO_MORE_PENDING_EVENT			= HCL_NO_MORE_PENDING_EVENT,
      MCDE_REMAINING_FILTER_PENDING_EVENTS  = HCL_REMAINING_FILTER_PENDING_EVENTS,
      MCDE_REMAINING_PENDING_EVENTS		    = HCL_REMAINING_PENDING_EVENTS,
      MCDE_INTERNAL_EVENT					= HCL_INTERNAL_EVENT,
      MCDE_INTERNAL_ERROR                   = HCL_INTERNAL_ERROR,          
      MCDE_NOT_CONFIGURED                   = HCL_NOT_CONFIGURED,          
      MCDE_REQUEST_PENDING                  = HCL_REQUEST_PENDING,         
      MCDE_REQUEST_NOT_APPLICABLE           = HCL_REQUEST_NOT_APPLICABLE,  
      MCDE_INVALID_PARAMETER                = HCL_INVALID_PARAMETER,       
      MCDE_UNSUPPORTED_FEATURE              = HCL_UNSUPPORTED_FEATURE,     
      MCDE_UNSUPPORTED_HW                   = HCL_UNSUPPORTED_HW          
   }t_mcde_error;

/*******************************************************************************
      MCDE Top Register APIs
******************************************************************************/

PUBLIC  t_mcde_error   MCDE_Init(IN t_logical_address mcde_base_address);
PUBLIC  t_mcde_error   MCDE_SetDbgLevel(IN t_dbg_level mcde_dbg_level);
PUBLIC  t_mcde_error   MCDE_GetVersion(OUT t_version *pt_version);
PUBLIC  void   		   MCDE_SetState(IN t_mcde_state state);
PUBLIC  void           MCDE_SetControlConfig(IN t_mcde_control *config);
/*PUBLIC  t_mcde_error  MCDE_SetChFlowCtrl(IN t_mcde_ch_id channel,IN t_mcde_ch_ctrl flow_ctrl);*/
PUBLIC  t_mcde_error   MCDE_SetChFSyncCtrl(IN t_mcde_ch_id channel,IN t_mcde_ch_ctrl fsync_ctrl);
PUBLIC  t_mcde_error   MCDE_GetControlConfig(OUT t_mcde_control *config);
PUBLIC  t_mcde_error   MCDE_GetState(OUT t_mcde_state *state);
PUBLIC  void		   MCDE_SetChFlowControl(IN t_mcde_ch_mode_ctrl *mode);
PUBLIC  t_mcde_error   MCDE_GetChFlowControl(OUT t_mcde_ch_mode_ctrl *mode);
PUBLIC  t_mcde_error   MCDE_SetFIFOControl(IN t_mcde_fifo_ctrl *fifo_ctrl);
PUBLIC  t_mcde_error   MCDE_SetOutputConfiguration(IN t_mcde_output_conf output_conf);

/*******************************************************************************
      MCDE External Source Configuration APIs
******************************************************************************/

PUBLIC  t_mcde_error   MCDE_SetBufferBaseAddress(IN t_mcde_ext_src src_id, IN t_mcde_buffer_id buffer_id, IN t_uint32 address);
PUBLIC  void           MCDE_SetExtSrcConfig(IN t_mcde_ext_src src_id, IN t_mcde_ext_conf *config);
PUBLIC  void		   MCDE_SetExtSrcControl(IN t_mcde_ext_src src_id, IN t_mcde_ext_src_ctrl *control);
PUBLIC  t_mcde_error   MCDE_SelectCurrentBuffer(IN t_mcde_ext_src src_id,IN t_mcde_buffer_id buffer_id);
PUBLIC  void		   MCDE_SetNumBufferUsed(IN t_mcde_ext_src src_id,IN t_mcde_num_buffer_used buffer_num);
PUBLIC  void		   MCDE_SetPriOverlay(IN t_mcde_ext_src src_id,IN t_mcde_overlay_id ovr_id);
PUBLIC  void		   MCDE_SetExtSrcBpp(IN t_mcde_ext_src src_id,IN t_mcde_bpp_ctrl bpp);
PUBLIC  void		   MCDE_SetRGBFormat(IN t_mcde_ext_src src_id,IN t_mcde_rgb_format_sel rgb_format);
PUBLIC  void           MCDE_SetBufferSelMode(IN t_mcde_ext_src src_id,IN t_mcde_buffer_sel_mode mode);
PUBLIC  void		   MCDE_SetBufferReleaseMode(IN t_mcde_ext_src src_id,IN t_mcde_multi_ovr_ctrl ovr_ctrl);
PUBLIC  t_mcde_error   MCDE_GetBufferBaseAddress(IN t_mcde_ext_src src_id,IN t_mcde_buffer_id,OUT t_uint32 *addr);
PUBLIC  t_mcde_error   MCDE_GetExtSrcConfig(IN t_mcde_ext_src src_id, OUT t_mcde_ext_conf *config);
PUBLIC  t_mcde_error   MCDE_GetExtSrcControl(IN t_mcde_ext_src src_id, OUT t_mcde_ext_src_ctrl *control);

/*******************************************************************************
      MCDE Overlay Configuration APIs
******************************************************************************/

PUBLIC  void		   MCDE_SetOverlayCtrl(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_control *ovr_cr);
PUBLIC  void		   MCDE_SetOverlayConfig(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_config *ovr_conf);
PUBLIC  void		   MCDE_SetOverlayConf2(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_conf2 *ovr_conf2);
PUBLIC  void		   MCDE_SetOvrLineIncrement(IN t_mcde_overlay_id overlay, IN t_uint32 ovr_ljinc);
PUBLIC  void		   MCDE_SetOvrTopMargin(IN t_mcde_overlay_id overlay, IN t_uint32 ovr_topmargin);
PUBLIC  void		   MCDE_SetOvrLeftMargin(IN t_mcde_overlay_id overlay, IN t_uint16 ovr_leftmargin);
PUBLIC  void		   MCDE_SetOvrComposition(IN t_mcde_overlay_id overlay,IN t_mcde_ovr_comp *ovr_comp);
PUBLIC  void 		   MCDE_SetOvrClip(IN t_mcde_overlay_id overlay, IN t_mcde_ovr_clip *ovr_clip);
PUBLIC  void		   MCDE_ReadOvrStatus(IN t_mcde_overlay_id overlay,OUT t_mcde_ovr_status *ovr_status);
PUBLIC  void		   MCDE_OverlayEnable(IN t_mcde_overlay_id overlay);
PUBLIC  void		   MCDE_OverlayDisable(IN t_mcde_overlay_id overlay);
PUBLIC  void		   MCDE_SetOvrPPL(IN t_mcde_overlay_id overlay,IN t_uint16 ppl);
PUBLIC  void		   MCDE_SetOvrLPF(IN t_mcde_overlay_id overlay,IN t_uint16 lpf);
PUBLIC  void		   MCDE_SetOvrXYPOS(IN t_mcde_overlay_id overlay,IN t_mcde_ovr_xy *xy_pos);
PUBLIC  void		   MCDE_SetOvrZPOS(IN t_mcde_overlay_id overlay,IN t_uint8 z_pos);
PUBLIC  void		   MCDE_OvrAssociateChannel(IN t_mcde_overlay_id overlay,IN t_mcde_ch_id ch_id);
PUBLIC  void		   MCDE_OvrAssociateExtSrc(IN t_mcde_overlay_id overlay,IN t_mcde_ext_src ext_src);
PUBLIC  t_mcde_error   MCDE_GetOverlayCtrl(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_control *ovr_cr);
PUBLIC  t_mcde_error   MCDE_GetOverlayConfig(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_config *ovr_conf);
PUBLIC  t_mcde_error   MCDE_GetOverlayConf2(IN t_mcde_overlay_id overlay, OUT t_mcde_ovr_conf2 *ovr_conf2);
PUBLIC  t_mcde_error   MCDE_GetOvrLineIncrement(IN t_mcde_overlay_id overlay, OUT t_uint32 *ovr_ljinc);
//PUBLIC  t_mcde_error   MCDE_GetOvrTopMargin(IN t_mcde_overlay_id overlay, OUT t_uint32 *ovr_topmargin);
PUBLIC  t_mcde_error   MCDE_GetOvrComposition(IN t_mcde_overlay_id overlay,OUT t_mcde_ovr_comp *ovr_comp);



/*******************************************************************************
      MCDE Channel Configuration APIs
******************************************************************************/

PUBLIC  void		  MCDE_SetChXConfig(IN t_mcde_ch_id channel,IN t_mcde_chx_config *config);
//PUBLIC  t_mcde_error  MCDE_SetChSyncConf(IN t_mcde_ch_id channel,IN t_mcde_chsyncconf conf);
PUBLIC  void		  MCDE_SetChSyncSource(IN t_mcde_ch_id channel, IN t_mcde_chsyncmod *sync_mod);
PUBLIC  void		  MCDE_SetSwSync(IN t_mcde_ch_id channel, IN t_mcde_sw_trigger sw_trig);
PUBLIC  void		  MCDE_SetChBckGndCol(IN t_mcde_ch_id channel, IN t_mcde_ch_bckgrnd_col *color);
PUBLIC  void		  MCDE_SetChPriority(IN t_mcde_ch_id channel,IN t_uint32 priority);
PUBLIC  void		  MCDE_SetOutDeviceLPF(IN t_mcde_ch_id channel, IN t_uint16 lpf);
PUBLIC  void		  MCDE_SetOutDevicePPL(IN t_mcde_ch_id channel,IN t_uint16 ppl);
/*PUBLIC  t_mcde_error  MCDE_SetFrameDelay(IN t_mcde_ch_id channel,IN t_uint16 delay);*/
PUBLIC  t_mcde_error  MCDE_SetFrameIntrGenEvent(IN t_mcde_ch_id channel,IN t_mcde_frame_events event);
PUBLIC  t_mcde_error  MCDE_SetFrameSyncDelay(IN t_mcde_ch_id channel,IN t_uint16 delay);
PUBLIC  t_mcde_error  MCDE_SetFrameSyncGenEvent(IN t_mcde_ch_id channel,IN t_mcde_frame_events event);
PUBLIC  void		  MCDE_SetChSyncSrc(IN t_mcde_ch_id channel,IN t_mcde_synchro_source sync_src);
PUBLIC  void		  MCDE_SetOutSyncSrc(IN t_mcde_ch_id channel,IN t_mcde_synchro_out_interface sync);
//PUBLIC  t_mcde_error  MCDE_SetSwFrameIntrDelay(IN t_mcde_ch_id channel,IN t_uint16 delay);
PUBLIC  void		  MCDE_GetChXConfig(IN t_mcde_ch_id channel,OUT t_mcde_chx_config *config);
//PUBLIC  t_mcde_error  MCDE_GetChSyncConf(IN t_mcde_ch_id channel,OUT t_mcde_chsyncconf *conf);
PUBLIC  void		  MCDE_GetChSyncSource(IN t_mcde_ch_id channel, OUT t_mcde_chsyncmod *sync_mod);
PUBLIC  t_mcde_error  MCDE_GetSwSync(IN t_mcde_ch_id channel, OUT t_mcde_sw_trigger *sw_trig);
PUBLIC  void		  MCDE_GetChBckGndCol(IN t_mcde_ch_id channel, OUT t_mcde_ch_bckgrnd_col *color);
PUBLIC  void		  MCDE_GetChPriority(IN t_mcde_ch_id channel,OUT t_uint32 *priority);
PUBLIC 	void 		  MCDE_GetChannelState(IN t_mcde_ch_id channel, OUT t_mcde_chnl_state *chnl_state);

/*******************************************************************************
      MCDE Channel A/B APIs
******************************************************************************/

PUBLIC t_mcde_error  MCDE_SetChannelXControl(IN t_mcde_ch_id channel,IN t_mcde_chx_control0 *control);
PUBLIC t_mcde_error  MCDE_GetChannelXControl(IN t_mcde_ch_id channel,IN t_mcde_chx_control0 *control);
PUBLIC t_mcde_error  MCDE_ChannelX_PowerEnable(IN t_mcde_ch_id channel);
PUBLIC t_mcde_error  MCDE_ChannelX_PowerDisable(IN t_mcde_ch_id channel);
PUBLIC t_mcde_error  MCDE_ChannelX_FlowEnable(IN t_mcde_ch_id channel);
PUBLIC t_mcde_error  MCDE_ChannelX_FlowDisable(IN t_mcde_ch_id channel);
PUBLIC t_mcde_error  MCDE_SetPanelControl(IN t_mcde_ch_id channel,IN t_mcde_chx_control1 *control);
PUBLIC t_mcde_error  MCDE_GetPanelControl(IN t_mcde_ch_id channel,IN t_mcde_chx_control1 *control);
PUBLIC t_mcde_error  MCDE_SetColorKey(IN t_mcde_ch_id channel,IN t_mcde_color_key *key);
PUBLIC t_mcde_error  MCDE_GetColorKey(IN t_mcde_ch_id channel,IN t_mcde_color_key *key);
PUBLIC t_mcde_error  MCDE_SetForcedColorKey(IN t_mcde_ch_id channel,IN t_mcde_forced_color_key *key);
PUBLIC t_mcde_error  MCDE_GetForcedColorKey(IN t_mcde_ch_id channel,IN t_mcde_forced_color_key *key);
PUBLIC t_mcde_error  MCDE_SetColorConversionMatrix(IN t_mcde_ch_id channel,IN t_mcde_rgb_conv_coef *coef);
PUBLIC t_mcde_error  MCDE_GetColorConversionMatrix(IN t_mcde_ch_id channel,IN t_mcde_rgb_conv_coef *coef);
PUBLIC t_mcde_error  MCDE_SetFlickerFilterCoefficient(IN t_mcde_ch_id channel,IN t_mcde_filter_coef *coef);
PUBLIC t_mcde_error  MCDE_GetFlickerFilterCoefficient(IN t_mcde_ch_id channel,IN t_mcde_filter_coef *coef);
PUBLIC t_mcde_error  MCDE_SetTVControl(IN t_mcde_ch_id channel,IN t_mcde_tv_control *control);
PUBLIC t_mcde_error  MCDE_GetTVControl(IN t_mcde_ch_id channel,IN t_mcde_tv_control *control);
PUBLIC t_mcde_error  MCDE_SetField1Blanking(IN t_mcde_ch_id channel,IN t_mcde_tv_blanking_field *blanking);
PUBLIC t_mcde_error  MCDE_GetField1Blanking(IN t_mcde_ch_id channel,IN t_mcde_tv_blanking_field *blanking);
PUBLIC t_mcde_error  MCDE_SetField2Blanking(IN t_mcde_ch_id channel,IN t_mcde_tv_blanking_field *blanking);
PUBLIC t_mcde_error  MCDE_GetField2Blanking(IN t_mcde_ch_id channel,IN t_mcde_tv_blanking_field *blanking);
PUBLIC t_mcde_error  MCDE_SetTVStartLine(IN t_mcde_ch_id channel,IN t_mcde_tv_start_line *start_line);
PUBLIC t_mcde_error  MCDE_GetTVStartLine(IN t_mcde_ch_id channel,IN t_mcde_tv_start_line *start_line);
PUBLIC t_mcde_error  MCDE_SetTVVerticalOffset(IN t_mcde_ch_id channel,IN t_mcde_tv_dvo_offset *offset);
PUBLIC t_mcde_error  MCDE_GetTVVerticalOffset(IN t_mcde_ch_id channel,IN t_mcde_tv_dvo_offset *offset);
PUBLIC t_mcde_error  MCDE_SetTVSourceWindowHeight(IN t_mcde_ch_id channel,IN t_mcde_tv_swh_time *swh);
PUBLIC t_mcde_error  MCDE_GetTVSourceWindowHeight(IN t_mcde_ch_id channel,IN t_mcde_tv_swh_time *swh);
PUBLIC t_mcde_error  MCDE_SetTVSourceWindowWidth(IN t_mcde_ch_id channel,IN t_mcde_tv_timing1 *sww);
PUBLIC t_mcde_error  MCDE_GetTVSourceWindowWidth(IN t_mcde_ch_id channel,IN t_mcde_tv_timing1 *sww);
PUBLIC t_mcde_error  MCDE_SetTVModeLineBlanking(IN t_mcde_ch_id channel,IN t_mcde_tv_lbalw_timing *balw);
PUBLIC t_mcde_error  MCDE_GetTVModeLineBlanking(IN t_mcde_ch_id channel,IN t_mcde_tv_lbalw_timing *balw);
PUBLIC t_mcde_error  MCDE_SetChannelXBackgroundColor(IN t_mcde_ch_id channel,IN t_mcde_tv_background_time *color);
PUBLIC t_mcde_error  MCDE_GetChannelXBackgroundColor(IN t_mcde_ch_id channel,IN t_mcde_tv_background_time *color);
PUBLIC t_mcde_error  MCDE_SetLCDControl(IN t_mcde_ch_id channel,IN t_mcde_lcd_timing0 *control);
PUBLIC t_mcde_error  MCDE_GetLCDControl(IN t_mcde_ch_id channel,IN t_mcde_lcd_timing0 *control);
PUBLIC t_mcde_error  MCDE_SetLCDTimingControl(IN t_mcde_ch_id channel,IN t_mcde_lcd_timing1 *control);
PUBLIC t_mcde_error  MCDE_GetLCDTimingControl(IN t_mcde_ch_id channel,IN t_mcde_lcd_timing1 *control);
PUBLIC t_mcde_error  MCDE_SetDitherControl(IN t_mcde_ch_id channel,IN t_mcde_dither_ctrl *control);
PUBLIC t_mcde_error  MCDE_GetDitherControl(IN t_mcde_ch_id channel,IN t_mcde_dither_ctrl *control);
PUBLIC t_mcde_error  MCDE_SetDitherOffset(IN t_mcde_ch_id channel,IN t_mcde_dithering_offset *offset);
PUBLIC t_mcde_error  MCDE_GetDitherOffset(IN t_mcde_ch_id channel,IN t_mcde_dithering_offset *offset);
PUBLIC t_mcde_error  MCDE_SetPalette(IN t_mcde_ch_id channel,IN t_mcde_palette *palette);
PUBLIC t_mcde_error  MCDE_GetPalette(IN t_mcde_ch_id channel,IN t_mcde_palette *palette);
PUBLIC t_mcde_error  MCDE_SetGammaCoef(IN t_mcde_ch_id channel,IN t_mcde_gamma *gamma);
PUBLIC t_mcde_error  MCDE_GetGammaCoef(IN t_mcde_ch_id channel,IN t_mcde_gamma *gamma);
PUBLIC t_mcde_error  MCDE_SetRotation0BaseAddress(IN t_mcde_ch_id channel,IN t_uint32 address);
PUBLIC t_mcde_error  MCDE_GetRotation0BaseAddress(IN t_mcde_ch_id channel,IN t_uint32 *address);
PUBLIC t_mcde_error  MCDE_SetRotation1BaseAddress(IN t_mcde_ch_id channel,IN t_uint32 address);
PUBLIC t_mcde_error  MCDE_GetRotation1BaseAddress(IN t_mcde_ch_id channel,IN t_uint32 *address);

/*******************************************************************************
      MCDE Channel C APIs
******************************************************************************/

PUBLIC  t_mcde_error  MCDE_SetChCConfig(IN t_mcde_chc_panel panel_id,IN t_mcde_chc_config *config);
PUBLIC  void		  MCDE_SetChCCtrl(IN t_mcde_chc_ctrl *control);
PUBLIC  void		  MCDE_ChC_PowerEnable(IN t_mcde_powen_select power);
PUBLIC  void		  MCDE_ChC_FlowEnable(IN t_mcde_flow_select flow);
PUBLIC  t_mcde_error  MCDE_ConfigurePBCUnit(IN t_mcde_chc_panel panel_id,IN t_mcde_pbc_config *config);
PUBLIC  t_mcde_error  MCDE_SetPBCMUX(IN t_mcde_chc_panel panel_id,IN t_mcde_pbc_mux *mux);
PUBLIC  t_mcde_error  MCDE_SetPBCBit_Ctrl(IN t_mcde_chc_panel panel_id,IN t_mcde_pbc_bitctrl *bit_control);
PUBLIC  t_mcde_error  MCDE_SetChCSynCaptureConf(IN t_mcde_chc_panel panel_id,IN t_mcde_sync_conf *config);
PUBLIC  t_mcde_error  MCDE_SetChCSynCTriggerDelay(IN t_uint8 delay);
PUBLIC  t_mcde_error  MCDE_GetSyncCaptureStatus(IN t_mcde_chc_panel panel_id,IN t_bool *sync_status);
PUBLIC  t_mcde_error  MCDE_SetBusAccessNum(IN t_mcde_chc_panel panel_id,t_uint8 bcn);
PUBLIC  t_mcde_error  MCDE_SetChipSelectTiming(IN t_mcde_chc_panel panel_id,IN t_mcde_cd_timing_activate *active);
PUBLIC  t_mcde_error  MCDE_SetReadWriteTiming(IN t_mcde_chc_panel panel_id,IN t_mcde_rw_timing *rw_time);
PUBLIC  t_mcde_error  MCDE_SetDataOutTiming(IN t_mcde_chc_panel panel_id,IN t_mcde_data_out_timing *data_time);
PUBLIC  t_mcde_error  MCDE_WriteCommand(IN t_mcde_chc_panel panel_id,IN t_uint32  command);
PUBLIC  t_mcde_error  MCDE_WriteData(IN t_mcde_chc_panel panel_id,IN t_uint32  data);
PUBLIC  t_mcde_error  MCDE_ReadData(IN t_mcde_chc_panel panel_id,IN t_uint16 * data);
PUBLIC  t_mcde_error  MCDE_WriteTxFIFO(IN t_mcde_chc_panel panel_id,IN t_mcde_txfifo_request_type type,IN t_uint32 data);
PUBLIC  t_mcde_error  MCDE_EnableSynchro(IN t_mcde_chc_panel panel_id,IN t_mcde_synchro_capture sync_enable);
PUBLIC  t_mcde_error  MCDE_EnableChannelC(IN t_mcde_chc_panel panel_id);
PUBLIC  t_mcde_error  MCDE_DisableChannelC(IN t_mcde_chc_panel panel_id);
PUBLIC  void		  MCDE_ChCSyncSelect(IN t_mcde_synchro_select sync_select);
PUBLIC  void		  MCDE_ChCClkSelect(IN t_mcde_clk_sel clk_sel);
PUBLIC  t_mcde_error  MCDE_GetChCConfig(IN t_mcde_chc_panel panel_id,IN t_mcde_chc_config *config);
PUBLIC  t_mcde_error  MCDE_GetChCCtrl(IN t_mcde_chc_ctrl *control);
PUBLIC  t_mcde_error  MCDE_SetChCVSYNCDelay(IN t_mcde_chc_panel panel_id,t_uint8 delay);
PUBLIC t_mcde_error  MCDE_Set_Fifo_Watermark(IN t_mcde_chc_panel panel_id, IN t_uint8 fifo_water_mark);
PUBLIC t_mcde_error  MCDE_Get_Fifo_Watermark(IN t_mcde_chc_panel panel_id,OUT t_uint8 *fifo_water_mark);






/*******************************************************************************
      MCDE DSI APIs
******************************************************************************/

#ifdef ST_8500ED
PUBLIC void		    MCDE_SetDSIClock(IN t_mcde_dsi_clk_config *clk_config);
#endif
PUBLIC t_mcde_error MCDE_SetDSICommandWord(IN t_mcde_dsi_channel channel,IN t_uint16 cmdbyte_lsb,IN t_uint16 cmdbyte_msb);
PUBLIC t_mcde_error MCDE_SetDSISyncPulse(IN t_mcde_dsi_channel channel,IN t_uint16 sync_dma,IN t_uint16 sync_sw);
PUBLIC t_mcde_error MCDE_SetDSIConfig(IN t_mcde_dsi_channel channel,IN t_mcde_dsi_conf *dsi_conf);
#ifdef __cplusplus
}   
#endif
#endif 





