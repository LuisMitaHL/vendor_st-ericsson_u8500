/*----------------------------------------------------------------------------------*/
/*  copyright STEricsson, 2009.                                            */
/*                                                                                  */
/* This program is free software; you can redistribute it and/or modify it under    */
/* the terms of the GNU General Public License as published by the Free             */
/* Software Foundation; either version 2.1 of the License, or (at your option)      */
/* any later version.                                                               */
/*                                                                                  */
/* This program is distributed in the hope that it will be useful, but WITHOUT      */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.   */
/*                                                                                  */
/* You should have received a copy of the GNU General Public License                */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.             */
/*----------------------------------------------------------------------------------*/

#ifndef _MCDE_IOCTLS_H_
#define _MCDE_IOCTLS_H_

typedef enum
{
  COLOR_CONV_NONE = 0x0,
  COLOR_CONV_YUV_RGB = 0x1,
  COLOR_CONV_RGB_YUV = 0x2,
  COLOR_CONV_YUV422_YUV444 = 0x3
}mcde_colorconv_type;

typedef enum
{
  MCDE_COL_CONV_DISABLE = 0x0,
  MCDE_COL_CONV_NOT_SAT = 0x1,
  MCDE_COL_CONV_SAT = 0x2,
  MCDE_COL_CONV_RESERVED
}mcde_col_conv_ctrl;

struct mcde_ovr_blend_ctrl
{
  char     alpha_value;
  char        ovr_opaq;
  char     ovr_blend;
  char        ovr_zlevel;
  unsigned int       ovr_xpos;
  unsigned int        ovr_ypos;
};

typedef enum
{
  MCDE_ROTATE_BURST_WORD_1 = 0x0,
  MCDE_ROTATE_BURST_WORD_2 = 0x1,
  MCDE_ROTATE_BURST_WORD_4 = 0x2,
  MCDE_ROTATE_BURST_WORD_8 = 0x3,
  MCDE_ROTATE_BURST_WORD_16 = 0x04,
  MCDE_ROTATE_BURST_WORD_RESERVED
 }mcde_rotate_req;

typedef enum
{
  MCDE_OUTSTND_REQ_1 = 0x0,
  MCDE_OUTSTND_REQ_2 = 0x1,
  MCDE_OUTSTND_REQ_4 = 0x2,
  MCDE_OUTSTND_REQ_8 = 0x3,
  MCDE_OUTSTND_REQ_16 = 0x4,
  MCDE_OUTSTND_REQ_RESERVED
}mcde_outsnd_req;
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
}mcde_burst_req;
typedef enum
{
  MCDE_COLOR_KEY_DISABLE = 0x0,
  MCDE_COLOR_KEY_ENABLE = 0x01
}mcde_color_key_ctrl;
typedef enum
{
  MCDE_PAL_GAMA_DISABLE = 0x0,
  MCDE_GAMA_ENABLE = 0x1,
  MCDE_PAL_ENABLE = 0x2,
  MCDE_PAL_GAMA_RESERVED
}mcde_pal_ctrl;
typedef enum
{
  MCDE_OVERLAY_DISABLE = 0x0,
  MCDE_OVERLAY_ENABLE = 0x1
}mcde_overlay_ctrl;
typedef enum
{
  MCDE_PIXEL_ALPHA_SOURCE = 0x0,
  MCDE_CONST_ALPHA_SOURCE = 0x1
}mcde_blend_ctrl;
typedef enum
{
  MCDE_OVR_OPAQUE_DISABLE = 0x0,
  MCDE_OVR_OPAQUE_ENABLE = 0x1
}mcde_ovr_opq_ctrl;
typedef enum
{
  MCDE_OVR_PREMULTIPLIED_ALPHA_DISABLE = 0x0,
  MCDE_OVR_PREMULTIPLIED_ALPHA_ENABLE    = 0x1
}mcde_ovr_alpha_enable;
typedef enum
{
  MCDE_OVR_CLIP_DISABLE = 0x0,
  MCDE_OVR_CLIP_ENABLE  = 0x1
}mcde_ovr_clip_enable;

struct mcde_chconfig
{
  unsigned short  lpf;
  unsigned short  ppl;
};
typedef enum
{
  MCDE_LCD_TV_0 = 0x0,
  MCDE_LCD_TV_1 = 0x1,
  MCDE_MDIF_IN_0 = 0x2,
  MCDE_MDIF_IN_1 = 0x3,
  MCDE_MDIF_OUT_0 = 0x4,
  MCDE_MDIF_OUT_1 = 0x5
}mcde_synchro_out_interface;
typedef enum
{
  MCDE_SYNCHRO_OUTPUT_SOURCE = 0x0,
  MCDE_SYNCHRO_AUTO = 0x1,
  MCDE_SYNCHRO_SOFTWARE = 0x2,
  MCDE_SYNCHRO_EXTERNAL_SOURCE = 0x3
}mcde_synchro_source;
typedef enum
{
  MCDE_NO_ACTION = 0x0,
  MCDE_NEW_FRAME_SYNCHRO = 0x1
}mcde_sw_trigger;
typedef enum
{
  MCDE_VERTICAL_SYNCHRO = 0x00,
  MCDE_BACK_PORCH = 0x01,
  MCDE_ACTIVE_VIDEO = 0x02,
  MCDE_FRONT_PORCH = 0x03
}mcde_frame_events;
struct mcde_ch_bckgrnd_col
{
  unsigned char  red;
  unsigned char  green;
  unsigned char  blue;
};
typedef enum
{
  MCDE_TVCLK_EXTERNAL = 0x0,
  MCDE_TVCLK_INTERNAL = 0x1
}mcde_tv_clk;
typedef enum
{
  MCDE_PCD_ENABLE = 0x0,
  MCDE_PCD_BYPASS = 0x1
}mcde_bcd_ctrl;
typedef enum
{
  MCDE_BPP_1_TO_8 = 0x0,
  MCDE_BPP_12 = 0x1,
  MCDE_BPP_16 = 0x2,
  MCDE_BPP_18 = 0x3,
  MCDE_BPP_24 = 0x4
}mcde_out_bpp;
typedef enum
{
  MCDE_BUS_16_CONF1 = 0x0,
  MCDE_BUS_16_CONF2 = 0x1,
  MCDE_BUS_16_CONF3 = 0x2,
  MCDE_BUS_18_CONF1 = 0x3,
  MCDE_BUS_18_CONF2 = 0x4,
  MCDE_BUS_24       = 0x5
}mcde_lcd_bus;
typedef enum
{
  MCDE_CLK_STBUS = 0x0,
  MCDE_CLK_72 = 0x1,
  MCDE_CLK_42 = 0x2,
  MCDE_CLK_27 = 0x3,
  MCDE_CLK_TVCLK1 = 0x4,
  MCDE_CLK_TVCLK2 = 0x5
}mcde_dpi2_clksel;

struct mcde_chx_control1
{
  mcde_tv_clk   tv_clk;
  mcde_bcd_ctrl bcd_ctrl;
  mcde_out_bpp  out_bpp;
  unsigned short        clk_per_line;
  mcde_lcd_bus  lcd_bus;
  mcde_dpi2_clksel dpi2_clk;
  unsigned short        pcd;
};
typedef enum
{
  MCDE_CLR_KEY_DISABLE = 0x0,
  MCDE_ALPHA_RGB_KEY = 0x1,
  MCDE_RGB_KEY = 0x2,
  MCDE_FALPHA_FRGB_KEY = 0x4,
  MCDE_FRGB_KEY = 0x5
}mcde_key_ctrl;
typedef enum
{
  MCDE_COLORKEY_NORMAL = 0x0,
  MCDE_COLORKEY_FORCE= 0x1
}mcde_colorkey_type;
struct mcde_chx_color_key
{
  unsigned char  alpha;
  unsigned char  red;
  unsigned char  green;
  unsigned char  blue;
};
typedef enum
{
  MCDE_SDTV_656P = 0x0,
  MCDE_HDTV_480P = 0x1,
  MCDE_HDTV_720P = 0x2,
  MCDE_TV_NOTUSED = 0x3,
}mcde_tvmode;
typedef enum
{
  MCDE_ACTIVE_HIGH = 0x0,
  MCDE_ACTIVE_LOW = 0x1
}mcde_signal_level;
typedef enum
{
  MCDE_MODE_LCD = 0x0,
  MCDE_MODE_TV = 0x1
}mcde_display_mode;
typedef enum
{
  MCDE_SCAN_PROGRESSIVE_MODE = 0x0,
  MCDE_SCAN_INTERLACED_MODE = 0x1,
}mcde_scan_mode;

typedef enum
{
  MCDE_TV_PAL = 0x0,
  MCDE_TV_NTSC = 0x1,
}mcde_tv_mode;

struct mcde_chnl_lcd_ctrl_reg
{
  unsigned short  num_lines;
  unsigned short ppl;
  mcde_tvmode tv_mode;
  mcde_signal_level ifield;
  mcde_scan_mode scan_mode;
  mcde_display_mode sel_mode;
};
struct mcde_chnl_lcd_horizontal_timing
{
  unsigned short hbp;
  unsigned short hfp;
  unsigned short hsw;
};
struct mcde_chnl_lcd_vertical_timing
{
  unsigned short vbp;
  unsigned short vfp;
  unsigned short vsw;
};

typedef enum
{
  MCDE_ANTIFLICKER_DISABLE = 0x0,
  MCDE_ANTIFLICKER_ENABLE = 0x1
}mcde_antiflicker_ctrl;

typedef enum
{
  MCDE_PIXEL_ORDER_LITTLE = 0x0,
  MCDE_PIXEL_ORDER_BIG = 0x1
}mcde_pixel_order_in_byte;

typedef enum
{
  MCDE_BYTE_LITTLE = 0x0,
  MCDE_BYTE_BIG = 0x1
}mcde_byte_endianity;

typedef enum
{
  MCDE_COL_RGB = 0x0,
  MCDE_COL_BGR = 0x1
}mcde_rgb_format_sel;

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
}mcde_bpp_ctrl;

typedef enum
{
  MCDE_OVERLAY_0 = 0x0,
  MCDE_OVERLAY_1 = 0x1,
  MCDE_OVERLAY_2 = 0x2,
  MCDE_OVERLAY_3 = 0x3,
  MCDE_OVERLAY_4 = 0x4,
  MCDE_OVERLAY_5 = 0x5,
  MCDE_OVERLAY_6 = 0x6,
  MCDE_OVERLAY_7 = 0x7
}mcde_overlay_id;

typedef enum
{
  MCDE_BUFFER_USED_NONE = 0x0,
  MCDE_BUFFER_USED_1 = 0x1,
  MCDE_BUFFER_USED_2 = 0x2,
  MCDE_BUFFER_USED_3 = 0x3
}mcde_num_buffer_used;

typedef enum
{
  MCDE_BUFFER_ID_0 = 0x0,
  MCDE_BUFFER_ID_1 = 0x1,
  MCDE_BUFFER_ID_2 = 0x2,
  MCDE_BUFFER_ID_RESERVED
}mcde_buffer_id;

typedef enum
{
  MCDE_MASK_DISABLE = 0x0,
  MCDE_MASK_ENABLE = 0x1
}mcde_masking_bit_ctrl;

typedef enum
{
  MCDE_DITHERING_RESET = 0x0,
  MCDE_DITHERING_ACTIVATE = 0x1
}mcde_dithering_control;
typedef enum
{
  MCDE_DITHERING_DISABLE = 0x0,
  MCDE_DITHERING_ENABLE = 0x1
}mcde_dithering_ctrl;
struct mcde_chx_dither_ctrl
{
  unsigned char    y_offset;
  unsigned char    x_offset;
  mcde_masking_bit_ctrl  masking_ctrl;
  unsigned char  mode;
  mcde_dithering_ctrl comp_dithering;
  mcde_dithering_ctrl temp_dithering;
};
struct mcde_chx_dithering_offset
{
  unsigned char  y_offset_rb;
  unsigned char  x_offset_rb;
  unsigned char  y_offset_rg;
  unsigned char  x_offset_rg;
};

typedef enum{
  VMODE_640_350_85_P,
  VMODE_640_400_85_P,
  VMODE_720_400_85_P,
  VMODE_640_480_60_P,
  VMODE_640_480_CRT_60_P,
  VMODE_240_320_60_P,
  VMODE_320_240_60_P,
  VMODE_712_568_60_P,
  VMODE_640_480_75_P,
  VMODE_640_480_85_P,
  VMODE_864_480_60_P,
  VMODE_800_600_56_P,
  VMODE_800_600_60_P,
  VMODE_800_600_72_P,
  VMODE_800_600_75_P,
  VMODE_800_600_85_P,
  VMODE_1024_768_60_P,
  VMODE_1024_768_70_P,
  VMODE_1024_768_75_P,
  VMODE_1024_768_85_P,
  VMODE_1152_864_75_P,
  VMODE_1280_960_60_P,
  VMODE_1280_960_85_P,
  VMODE_1280_1024_60_P,
  VMODE_1280_1024_75_P,
  VMODE_1280_1024_85_P,
  VMODE_1600_1200_60_P,
  VMODE_1600_1200_65_P,
  VMODE_1600_1200_70_P,
  VMODE_1600_1200_75_P,
  VMODE_1600_1200_85_P,
  VMODE_1792_1344_60_P,
  VMODE_1792_1344_75_P,
  VMODE_1856_1392_60_P,
  VMODE_1856_1392_75_P,
  VMODE_1920_1440_60_P,
  VMODE_1920_1440_75_P,
  VMODE_720_480_60_P,
  VMODE_720_480_60_I,
  VMODE_720_576_50_P,
  VMODE_720_576_50_I,
  VMODE_1280_720_50_P,
  VMODE_1280_720_60_P,
  VMODE_1920_1080_50_I,
  VMODE_1920_1080_60_I,
  VMODE_1920_1080_60_P,
}mcde_video_mode;
/*****************************************************************************
IOCTLs for access from user space
*******************************************************************************/

#define MCDE_IOC_MAGIC 'm'

#define MCDE_IOCTL_OVERLAY_CREATE       	_IOWR(MCDE_IOC_MAGIC, 0x0,struct mcde_overlay_create)
#define MCDE_IOCTL_OVERLAY_REMOVE       	_IOR(MCDE_IOC_MAGIC, 0x1,unsigned long)
#define MCDE_IOCTL_COLOR_KEYING_ENABLE       	_IOWR(MCDE_IOC_MAGIC, 0x2,struct mcde_channel_color_key)
#define MCDE_IOCTL_COLOR_KEYING_DISABLE       	_IOR(MCDE_IOC_MAGIC, 0x3,unsigned long)
#define MCDE_IOCTL_COLOR_COVERSION_ENABLE       _IOWR(MCDE_IOC_MAGIC, 0x4,struct mcde_conf_color_conv)
#define MCDE_IOCTL_COLOR_COVERSION_DISABLE      _IOR(MCDE_IOC_MAGIC, 0x5,unsigned long)
#define MCDE_IOCTL_ROTATION_ENABLE       	_IOR(MCDE_IOC_MAGIC, 0x6,unsigned char)
#define MCDE_IOCTL_ROTATION_DISABLE       	_IOR(MCDE_IOC_MAGIC, 0x7,unsigned char)
#define MCDE_IOCTL_SET_VIDEOMODE       		_IOR(MCDE_IOC_MAGIC, 0x8, unsigned long)
#define MCDE_IOCTL_ALLOC_FRAMEBUFFER   		_IOWR(MCDE_IOC_MAGIC, 0x9,struct mcde_sourcebuffer_alloc)
#define MCDE_IOCTL_DEALLOC_FRAMEBUFFER   	_IOWR(MCDE_IOC_MAGIC, 0xA, unsigned long)
#define MCDE_IOCTL_CONFIGURE_EXTSRC   		_IOR(MCDE_IOC_MAGIC, 0xB,struct mcde_ext_conf)
#define MCDE_IOCTL_CONFIGURE_OVRLAY   		_IOR(MCDE_IOC_MAGIC, 0xC,struct mcde_conf_overlay)
#define MCDE_IOCTL_CONFIGURE_CHANNEL		_IOR(MCDE_IOC_MAGIC, 0xD,struct mcde_ch_conf)
#define MCDE_IOCTL_CONFIGURE_PANEL		_IOR(MCDE_IOC_MAGIC, 0xE,struct mcde_chnl_lcd_ctrl)
#define MCDE_IOCTL_MCDE_ENABLE			_IOR(MCDE_IOC_MAGIC, 0xF,unsigned long)
#define MCDE_IOCTL_MCDE_DISABLE			_IOR(MCDE_IOC_MAGIC, 0x10,unsigned long)
#define MCDE_IOCTL_CHANNEL_BLEND_ENABLE		_IOWR(MCDE_IOC_MAGIC,0x11,struct mcde_blend_control)
#define MCDE_IOCTL_CHANNEL_BLEND_DISABLE	_IOR(MCDE_IOC_MAGIC, 0x12,unsigned long)
#define MCDE_IOCTL_CONFIGURE_DENC		_IOR(MCDE_IOC_MAGIC, 0x13,unsigned long)
#define MCDE_IOCTL_CONFIGURE_HDMI		_IOR(MCDE_IOC_MAGIC, 0x14,unsigned long)
#define MCDE_IOCTL_SET_SOURCE_BUFFER		_IOR(MCDE_IOC_MAGIC, 0x15,struct mcde_source_buffer)
#define MCDE_IOCTL_DITHERING_ENABLE		_IOR(MCDE_IOC_MAGIC, 0x16,struct mcde_dithering_ctrl_conf)
#define MCDE_IOCTL_DITHERING_DISABLE		_IOR(MCDE_IOC_MAGIC, 0x16,unsigned long)
#define MCDE_IOCTL_ANTIFLICKER_ENABLE		_IOR(MCDE_IOC_MAGIC, 0x17,unsigned long)
#define MCDE_IOCTL_ANTIFLICKER_DISABLE		_IOR(MCDE_IOC_MAGIC, 0x18,unsigned long)
#define MCDE_IOCTL_TEST_DSI_LPMODE		_IOR(MCDE_IOC_MAGIC, 0x19,unsigned long)
#define MCDE_IOCTL_TEST_DSI_HSMODE		_IOR(MCDE_IOC_MAGIC, 0x1A,unsigned long)
#define MCDE_IOCTL_SET_SCAN_MODE		_IOWR(MCDE_IOC_MAGIC, 0x1B,unsigned long)
#define MCDE_IOCTL_GET_SCAN_MODE		_IOR(MCDE_IOC_MAGIC, 0x1C,unsigned long)

#define MCDE_IOCTL_TV_PLUG_STATUS		_IOR(MCDE_IOC_MAGIC, 0x1D,unsigned long)
#define MCDE_IOCTL_TV_CHANGE_MODE		_IOWR(MCDE_IOC_MAGIC, 0x1E,unsigned long)
#define MCDE_IOCTL_TV_GET_MODE		_IOR(MCDE_IOC_MAGIC, 0x1F,unsigned long)

/**
 * struct mcde_overlay_create - To create overlay
 * @xorig: frame buffer x-offset
 * @yorig: frame buffer y-offset
 * @xwidth: frame buffer x-width
 * @yheight - frame buffer y-height
 * @bpp: input source bits per pixel
 * @fg: if set then overlay goes to foreground,else remains in background
 * @key: unique framebuffer key returned by driver
 * @bgrinput: if set then implies BGR input
 * @usedefault: if set then uses base overlay buffer
 *
 *
 **/
struct mcde_overlay_create {
  unsigned long xorig;
  unsigned long yorig;
  unsigned long xwidth;
  unsigned long yheight;
  char  bpp; 	   /** input source bits per pixel */
  char  fg;		   /** if set then overlay goes to foreground,else remains in background */
  unsigned long key; 	   /** unique framebuffer key returned by driver */
  unsigned long bgrinput; 	 /** if set then implies BGR input */
  char  usedefault; // if set then uses base overlay buffer
};
/**
 * struct mcde_addrmap - source frame buffer address map structure
 * @cpuaddr: logical address of the framebuffer
 * @dmaaddr: physical address of the framebuffer
 * @bufflength: buffer length
 *
 *
 **/
struct mcde_addrmap {
	unsigned long cpuaddr;
	unsigned long dmaaddr;
	unsigned long bufflength;
};
/**
 * struct mcde_source_buffer - source frame buffer addresses structure
 * @buff_addr: frame buffer addresses structure
 * @buffid: buffer id
 *
 *
 **/
struct mcde_source_buffer
{
	struct mcde_addrmap buffaddr;
	unsigned char buffid;
};
/**
 * struct mcde_sourcebuffer_alloc - source frame buffer allocation structure
 * @xwidth: frame buffer x-width
 * @yheight: frame buffer y-height
 * @bpp: input source bits per pixel
 * @doubleBufferingEnabled: double buffer control
 * @key: unique framebuffer key returned by driver
 * @buff_addr: frame buffer addresses structure
 *
 *
 **/
struct mcde_sourcebuffer_alloc {
  unsigned long xwidth;
  unsigned long yheight;
  char  bpp; 	   /** input source bits per pixel */
  char  doubleBufferingEnabled;
  unsigned long key; 	   /** unique framebuffer key returned by driver */
  struct mcde_addrmap buff_addr;
};

/**
 * struct mcde_dithering_ctrl_conf - Structure of overlay configuration
 * @rot_burst_req: rotation burst request
 * @outstnd_req: outstanding request
 * @burst_req: burst request
 * @priority: priority
 * @color_key: color key control
 * @pal_control: pal control
 * @col_ctrl: color conversion control
 * @convert_format: conversion format
 * @ovr_state: overlay control
 * @ovr_ypos: overlay y-position
 * @ovr_xpos: overlay x-position
 * @alpha: alpha control
 * @alpha_value: alpha value
 * @pixoff: pixel offset
 * @ovr_opaq: overlay opaque control
 * @ovr_blend: overlay blend control
 * @watermark_level: watermark level
 * @ovr_zlevel: foreground level
 * @clip: clip control
 * @ytlcoor: clip y-topleft coordinates
 * @xtlcoor: clip x-topleft coordinates
 * @ybrcoor: clip y-bottomright coordinates
 * @xbrcoor: clip x-bottomright coordinates
 * @xwidth: x-width
 * @yheight: y-height
 * @bpp: input bpp
 *
 *
 **/
struct mcde_conf_overlay {
  mcde_rotate_req     		rot_burst_req;
  mcde_outsnd_req     		outstnd_req;
  mcde_burst_req      		burst_req;
  unsigned char                  		priority;
  mcde_color_key_ctrl 		color_key;
  mcde_pal_ctrl       		pal_control;
  mcde_col_conv_ctrl  		col_ctrl;
  mcde_colorconv_type 		convert_format;
  mcde_overlay_ctrl   		ovr_state;

  unsigned short	 	   		ovr_ypos;
  unsigned short 	  			ovr_xpos;

  mcde_ovr_alpha_enable 	alpha;
  unsigned char		      		alpha_value;
  unsigned char		      		pixoff;
  mcde_ovr_opq_ctrl   		ovr_opaq;
  mcde_blend_ctrl     		ovr_blend;
  unsigned long 				watermark_level;
  unsigned char	  			ovr_zlevel;

  mcde_ovr_clip_enable  	clip;
  unsigned long 				ytlcoor;
  unsigned long 				xtlcoor;
  unsigned long 				ybrcoor;
  unsigned long 				xbrcoor;

  unsigned long 				xwidth;
  unsigned long 				yheight;
  char  			bpp; 	   /** input source bits per pixel */

};
/**
 * struct mcde_blend_control - channel configuration structure
 * @chconfig: channel config structure
 * @out_synch_interface: out sync interface
 * @ch_synch_src: channel sync source
 * @sw_trig: software trigger param
 * @swint_vcnt: int count param
 * @swint_vevent: event count param
 * @chbckgrndcolor: channel background color
 * @ch_priority: channel priority
 * @control1: channel control structure
 *
 *
 **/
struct mcde_ch_conf
{
  struct mcde_chconfig 	      chconfig;
  mcde_synchro_out_interface    out_synch_interface;
  mcde_synchro_source           ch_synch_src;
  mcde_sw_trigger               sw_trig;
  unsigned short                           swint_vcnt;
  mcde_frame_events             swint_vevent;
  unsigned short                           hwreq_vcnt;
  mcde_frame_events             hwreq_vevent;
  struct mcde_ch_bckgrnd_col    chbckgrndcolor;
  unsigned char                            ch_priority;
  struct mcde_chx_control1 	control1;
};
/**
 * struct mcde_blend_control - color keying configuration structure
 * @key_ctrl: color key control
 * @color_key_type: color key type
 * @color_key: channel color key structure
 *
 *
 **/
struct mcde_channel_color_key
{
  mcde_key_ctrl	key_ctrl;
  mcde_colorkey_type color_key_type;
  struct mcde_chx_color_key color_key;
};
/**
 * struct mcde_conf_color_conv - color conversion configuration structure
 * @convert_format: convert format type
 * @col_ctrl: color conversion control
 *
 *
 **/
struct mcde_conf_color_conv
{
  mcde_colorconv_type convert_format;
  mcde_col_conv_ctrl col_ctrl;
};
/**
 * struct mcde_blend_control - blend control configuration structure
 * @blenden: blend enable
 * @blend_ctrl: blend control
 * @alpha_blend: alpha blend
 * @ovr1_id: overlay1 ID
 * @ovr2_id: overlay2 ID
 * @ovr2_enable: overlay2 enable
 * @ovr1_blend_ctrl: overlay1 blend control structure
 * @ovr2_blend_ctrl: overlay2 blend control structure
 *
 *
 **/
struct mcde_blend_control
{
  char	blenden;
  char	blend_ctrl;
  char	alpha_blend;
  char    ovr1_id;
  char    ovr2_id;
  char	ovr2_enable;
  struct mcde_ovr_blend_ctrl ovr1_blend_ctrl;
  struct mcde_ovr_blend_ctrl ovr2_blend_ctrl;

};
/**
 * struct mcde_chnl_lcd_ctrl - LCD control structure
 * @lcd_ctrl_reg: lcd control reg structure
 * @lcd_horizontal_timing: lcd horizontal timing structure
 * @lcd_vertical_timing: lcd vertical timing structure
 *
 *
 **/
struct mcde_chnl_lcd_ctrl
{
  struct mcde_chnl_lcd_ctrl_reg lcd_ctrl_reg;
  struct mcde_chnl_lcd_horizontal_timing	lcd_horizontal_timing;
  struct mcde_chnl_lcd_vertical_timing	lcd_vertical_timing;

};
/**
 * struct mcde_ext_conf - External source configuration structure
 * @ovr_pxlorder: overlay pixel order
 * @endianity: byte endianity
 * @rgb_format: format type
 * @bpp: input bpp
 * @provr_id: overlay id
 * @buf_num: number of buffers used
 * @buf_id: buffer id to be used
 *
 *
 **/
struct mcde_ext_conf
{
  mcde_pixel_order_in_byte ovr_pxlorder;
  mcde_byte_endianity      endianity;
  mcde_rgb_format_sel      rgb_format;
  mcde_bpp_ctrl           bpp;
  mcde_overlay_id         provr_id;
  mcde_num_buffer_used    buf_num;
  mcde_buffer_id          buf_id;
};
/**
 * struct mcde_dithering_ctrl_conf - Structure of dithering configuration
 * @dithering_ctrl: dithering control
 * @input_bpp: input bpp
 * @output_bpp: output bpp
 * @mcde_chx_dither_ctrl: structure of dithering control
 * @mcde_chx_dithering_offset: structure of the dithering offset
 *
 *
 **/
struct mcde_dithering_ctrl_conf
{
  mcde_dithering_ctrl dithering_ctrl;
  mcde_bpp_ctrl input_bpp;
  mcde_out_bpp output_bpp;
  struct mcde_chx_dither_ctrl mcde_chx_dither_ctrl;
  struct mcde_chx_dithering_offset mcde_chx_dithering_offset;
};
#endif
