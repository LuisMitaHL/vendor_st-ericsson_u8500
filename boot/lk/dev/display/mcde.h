/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __MCDE__H__
#define __MCDE__H__

#include "mcde_kcompat.h"
#include "mcde_device.h"
#include "nova_dsilink.h"
/* BMP header might be stored in LE format,
   use "read_u32" for access.
*/
typedef struct {
	/* Header */
	char signature[2];
	uint32_t	file_size;
	uint32_t	reserved;
	uint32_t	data_offset;
	/* InfoHeader */
	uint32_t	size;
	uint32_t	width;
	uint32_t	height;
	uint16_t	planes;
	uint16_t	bit_count;
	uint32_t	compression;
	uint32_t	image_size;
	uint32_t	x_pixels_per_m;
	uint32_t	y_pixels_per_m;
	uint32_t	colors_used;
	uint32_t	colors_important;
	/* ColorTable */
} __attribute__ ((packed)) bmp_header_t;

typedef struct {
	uint8_t	blue;
	uint8_t	green;
	uint8_t	red;
	uint8_t	reserved;
} __attribute__ ((packed)) bmp_color_table_entry_t;

typedef struct {
	bmp_header_t header;
	/* We use a zero sized array just as a placeholder for variable
	   sized array */
	bmp_color_table_entry_t color_table[0];
} bmp_image_t;

#if BMP_HEADER_IN_LE
#define read_u32(val_)\
	((*(val_)<<24u) | ((*(val_)&0xff00)<<8u) | ((*(val_)&0xff0000)>>8u) |
						((*(val_)&0xff000000)>>24u))
#else
#define read_u32(val_) *(val_)
#endif

/* MCDE channel states
 *
 * Allowed state transitions:
 *   IDLE <-> SUSPEND
 *   IDLE <-> DSI_READ
 *   IDLE <-> DSI_WRITE
 *   IDLE -> SETUP -> (WAIT_TE ->) RUNNING -> STOPPING1 -> STOPPING2 -> IDLE
 *   WAIT_TE -> STOPPED (for missing TE to allow re-enable)
 */
enum chnl_state {
	CHNLSTATE_SUSPEND,   /* HW in suspended mode, initial state */
	CHNLSTATE_IDLE,      /* Channel aquired, but not running, FLOEN==0 */
	CHNLSTATE_DSI_READ,  /* Executing DSI read */
	CHNLSTATE_DSI_WRITE, /* Executing DSI write */
	CHNLSTATE_SETUP,     /* Channel register setup to prepare for running */
	CHNLSTATE_WAIT_TE,   /* Waiting for BTA or external TE */
	CHNLSTATE_RUNNING,   /* Update started, FLOEN=1, FLOEN==1 */
	CHNLSTATE_STOPPING,  /* Stopping, FLOEN=0, FLOEN==1, awaiting VCMP */
	CHNLSTATE_STOPPED,   /* Stopped, after VCMP, FLOEN==0|1 */
};


/* Physical interface types */
enum mcde_port_type {
	MCDE_PORTTYPE_DSI = 0,
	MCDE_PORTTYPE_DPI = 1,
};

/* Interface mode */
enum mcde_port_mode {
	MCDE_PORTMODE_CMD = 0,
	MCDE_PORTMODE_VID = 1,
};

/* MCDE fifos */
enum mcde_fifo {
	MCDE_FIFO_A  = 0,
	MCDE_FIFO_B  = 1,
	MCDE_FIFO_C0 = 2,
	MCDE_FIFO_C1 = 3,
};

/* MCDE channels (pixel pipelines) */
enum mcde_chnl {
	MCDE_CHNL_A  = 0,
	MCDE_CHNL_B  = 1,
	MCDE_CHNL_C0 = 2,
	MCDE_CHNL_C1 = 3,
};

/* Update sync mode */
enum mcde_sync_src {
	MCDE_SYNCSRC_OFF = 0, /* No sync */
	MCDE_SYNCSRC_TE0 = 1, /* MCDE ext TE0 */
	MCDE_SYNCSRC_TE1 = 2, /* MCDE ext TE1 */
	MCDE_SYNCSRC_BTA = 3, /* DSI BTA */
	MCDE_SYNCSRC_TE_POLLING = 4, /* DSI TE_POLLING */
};

/* Frame trig method */
enum mcde_trig_method {
	MCDE_TRIG_HW = 0, /* frame trig from MCDE formatter */
	MCDE_TRIG_SW = 1, /* frame trig from software */
};

/* Interface pixel formats (output) */
/*
* REVIEW: Define formats
* Add explanatory comments how the formats are ordered in memory
*/
enum mcde_port_pix_fmt {
	/* MIPI standard formats */

	MCDE_PORTPIXFMT_DPI_16BPP_C1 =     0x21,
	MCDE_PORTPIXFMT_DPI_16BPP_C2 =     0x22,
	MCDE_PORTPIXFMT_DPI_16BPP_C3 =     0x23,
	MCDE_PORTPIXFMT_DPI_18BPP_C1 =     0x24,
	MCDE_PORTPIXFMT_DPI_18BPP_C2 =     0x25,
	MCDE_PORTPIXFMT_DPI_24BPP =        0x26,

	MCDE_PORTPIXFMT_DSI_16BPP =        0x31,
	MCDE_PORTPIXFMT_DSI_18BPP =        0x32,
	MCDE_PORTPIXFMT_DSI_18BPP_PACKED = 0x33,
	MCDE_PORTPIXFMT_DSI_24BPP =        0x34,

	/* Custom formats */
	MCDE_PORTPIXFMT_DSI_YCBCR422 =     0x40,
};

enum mcde_hdmi_sdtv_switch {
	HDMI_SWITCH,
	SDTV_SWITCH,
	DVI_SWITCH
};

enum mcde_col_convert {
	MCDE_CONVERT_RGB_2_RGB,
	MCDE_CONVERT_RGB_2_YCBCR,
	MCDE_CONVERT_YCBCR_2_RGB,
	MCDE_CONVERT_YCBCR_2_YCBCR,
};

struct mcde_col_transform {
	u16 matrix[3][3];
	u16 offset[3];
};

struct mcde_oled_transform {
	u16 matrix[3][3];
	u16 offset[3];
};

/* DSI video mode */
enum mcde_dsi_vid_mode {
	NON_BURST_MODE_WITH_SYNC_EVENT = 0,
	/* enables tvg, test video generator */
	NON_BURST_MODE_WITH_SYNC_EVENT_TVG_ENABLED = 1,
	BURST_MODE_WITH_SYNC_EVENT  = 2,
	BURST_MODE_WITH_SYNC_PULSE  = 3,
};

enum mcde_vsync_polarity {
	VSYNC_ACTIVE_HIGH = 0,
	VSYNC_ACTIVE_LOW = 1,
};

#define MCDE_PORT_DPI_NO_CLOCK_DIV	0

#define DPI_ACT_HIGH_ALL	0 /* all signals are active high	  */
#define DPI_ACT_LOW_HSYNC	1 /* horizontal sync signal is active low */
#define DPI_ACT_LOW_VSYNC	2 /* vertical sync signal is active low	  */
#define DPI_ACT_LOW_DATA_ENABLE	4 /* data enable signal is active low	  */
#define DPI_ACT_ON_FALLING_EDGE	8 /* drive data on the falling edge of the
				   * pixel clock
				   */

struct mcde_port {
	enum mcde_port_type type;
	enum mcde_port_mode mode;
	enum mcde_port_pix_fmt pixel_format;
	u8 refresh_rate;	/* display refresh rate given in Hz */
	u8 ifc;
	u8 link;
	enum mcde_sync_src sync_src;
	enum mcde_trig_method frame_trig;
	enum mcde_sync_src requested_sync_src;
	enum mcde_trig_method requested_frame_trig;
	enum mcde_vsync_polarity vsync_polarity;
	u8 vsync_clock_div;
	/* duration is expressed as number of (STBCLK / VSPDIV) clock period */
	u16 vsync_min_duration;
	u16 vsync_max_duration;
	bool update_auto_trig;
	enum mcde_hdmi_sdtv_switch hdmi_sdtv_switch;
	union {
		struct dsilink_phy dsi;
		struct {
			u8 bus_width;
			bool tv_mode;
			u16 clock_div; /* use 0 or 1 for no clock divider */
			u32 polarity;    /* see DPI_ACT_LOW_* definitions */
			u32 lcd_freq;
		} dpi;
	} phy;
};

/* Overlay pixel formats (input) *//* REVIEW: Define byte order */
enum mcde_ovly_pix_fmt {
	MCDE_OVLYPIXFMT_RGB565   = 1,
	MCDE_OVLYPIXFMT_RGBA5551 = 2,
	MCDE_OVLYPIXFMT_RGBA4444 = 3,
	MCDE_OVLYPIXFMT_RGB888   = 4,
	MCDE_OVLYPIXFMT_RGBX8888 = 5,
	MCDE_OVLYPIXFMT_RGBA8888 = 6,
	MCDE_OVLYPIXFMT_YCbCr422 = 7,
};

/* Display power modes */
enum mcde_display_power_mode {
	MCDE_DISPLAY_PM_OFF     = 0, /* Power off */
	MCDE_DISPLAY_PM_STANDBY = 1, /* DCS sleep mode */
	MCDE_DISPLAY_PM_ON      = 2, /* DCS normal mode, display on */
};

/* Display rotation */
enum mcde_display_rotation {
	MCDE_DISPLAY_ROT_0       = 0,
	MCDE_DISPLAY_ROT_90_CCW  = 90,
	MCDE_DISPLAY_ROT_180_CCW = 180,
	MCDE_DISPLAY_ROT_270_CCW = 270,
	MCDE_DISPLAY_ROT_90_CW   = MCDE_DISPLAY_ROT_270_CCW,
	MCDE_DISPLAY_ROT_180_CW  = MCDE_DISPLAY_ROT_180_CCW,
	MCDE_DISPLAY_ROT_270_CW  = MCDE_DISPLAY_ROT_90_CCW,
};

/* REVIEW: Verify */
#define MCDE_MIN_WIDTH  16
#define MCDE_MIN_HEIGHT 16
#define MCDE_MAX_WIDTH  2048
#define MCDE_MAX_HEIGHT 2048
#define MCDE_BUF_START_ALIGMENT 8
#define MCDE_BUF_LINE_ALIGMENT 8

/* Tv-out defines */
#define MCDE_CONFIG_TVOUT_BACKGROUND_LUMINANCE		0x83
#define MCDE_CONFIG_TVOUT_BACKGROUND_CHROMINANCE_CB	0x9C
#define MCDE_CONFIG_TVOUT_BACKGROUND_CHROMINANCE_CR	0x2C

/* In seconds */
#define MCDE_AUTO_SYNC_WATCHDOG 5

/* DSI modes */
#define DSI_VIDEO_MODE	0
#define DSI_CMD_MODE	1

/* Video mode descriptor */
struct mcde_video_mode {
	u32 xres;
	u32 yres;
	u32 pixclock;	/* pixel clock in ps (pico seconds) */
	u32 hbp;	/* horizontal back porch: left margin (excl. hsync) */
	u32 hfp;	/* horizontal front porch: right margin (excl. hsync) */
	u32 hsw;	/* horizontal sync width */
	u32 vbp;	/* vertical back porch: upper margin (excl. vsync) */
	u32 vfp;	/* vertical front porch: lower margin (excl. vsync) */
	u32 vsw;	/* vertical sync width*/
	u8  bckcol[4];	/* background color [R, G, B, X] */
	bool interlaced;
	bool force_update; /* when switching between hdmi and sdtv */
};

struct mcde_rectangle {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

struct mcde_overlay_info {
	u32 paddr;
	u32 *vaddr;
	u16 stride; /* buffer line len in bytes */
	enum mcde_ovly_pix_fmt fmt;

	u16 src_x;
	u16 src_y;
	u16 dst_x;
	u16 dst_y;
	u16 dst_z;
	u16 w;
	u16 h;
	struct mcde_rectangle dirty;
};

struct ovly_regs {
	bool enabled;
	bool dirty;
	bool dirty_buf;

	u8   ch_id;
	u32  baseaddress0;
	u32  baseaddress1;
	u8   bits_per_pixel;
	u8   bpp;
	bool bgr;
	bool bebo;
	bool opq;
	u8   col_conv;
	u8   alpha_source;
	u8   alpha_value;
	u8   pixoff;
	u16  ppl;
	u16  lpf;
	u16  cropx;
	u16  cropy;
	u16  xpos;
	u16  ypos;
	u8   z;
	u32  pixfetchwtrmrklevel;
	u32  pixfetchwtrmrklevel_old;
};

struct mcde_ovly_state {
	bool inuse;
	u8 idx; /* MCDE overlay index */
	struct mcde_chnl_state *chnl; /* Owner channel */
	bool dirty;
	bool dirty_buf;

	/* Staged settings */
	u32 paddr;
	u16 stride;
	enum mcde_ovly_pix_fmt pix_fmt;

	u16 src_x;
	u16 src_y;
	u16 dst_x;
	u16 dst_y;
	u16 dst_z;
	u16 w;
	u16 h;

	u8 alpha_source;
	u8 alpha_value;

	/* Applied settings */
	struct ovly_regs regs;

	u32 pixfetch_need;
	u8  pixfetch_factor;
};

/*
 * Three functions for mapping 8 bits colour channels on 12 bits colour
 * channels. The colour channels (ch0, ch1, ch2) can represent (r, g, b) or
 * (Y, Cb, Cr) respectively.
 */
struct mcde_palette_table {
	u16 (*map_col_ch0)(u8);
	u16 (*map_col_ch1)(u8);
	u16 (*map_col_ch2)(u8);
};

struct chnl_regs {
	bool dirty;

	bool floen;
	u16  x;
	u16  y;
	u16  ppl;
	u16  lpf;
	u8   bpp;
	bool internal_clk; /* CLKTYPE field */
	u16  pcd;
	u8   clksel;
	u8   cdwin;
	u16 (*map_r)(u8);
	u16 (*map_g)(u8);
	u16 (*map_b)(u8);
	bool palette_enable;
	bool oled_enable;
	bool background_enable;
	u8   bcd;
	bool roten;
	u8   rotdir;

	/* Blending */
	u8 blend_ctrl;
	bool blend_en;
	u8 alpha_blend;

	/* DSI */
	u8 dsipacking;
};

struct col_regs {
	bool dirty;

	u16 y_red;
	u16 y_green;
	u16 y_blue;
	u16 cb_red;
	u16 cb_green;
	u16 cb_blue;
	u16 cr_red;
	u16 cr_green;
	u16 cr_blue;
	u16 off_y;
	u16 off_cb;
	u16 off_cr;
};

struct tv_regs {
	bool dirty;

	u16 dho; /* TV mode: left border width; destination horizontal offset */
		 /* LCD MODE: horizontal back porch */
	u16 alw; /* TV mode: right border width */
		 /* LCD mode: horizontal front porch */
	u16 hsw; /* horizontal synch width */
	u16 dvo; /* TV mode: top border width; destination horizontal offset */
		 /* LCD MODE: vertical back porch */
	u16 bsl; /* TV mode: bottom border width; blanking start line */
		 /* LCD MODE: vertical front porch */
	/* field 1 */
	u16 bel1; /* TV mode: field total vertical blanking lines */
		 /* LCD mode: vertical sync width */
	u16 fsl1; /* field vbp */
	/* field 2 */
	u16 bel2;
	u16 fsl2;
	u8 tv_mode;
	bool sel_mode_tv;
	bool inv_clk;
	bool interlaced_en;
	u32 lcdtim1;
};

struct oled_regs {
	bool dirty;

	u16 alfa_red;
	u16 alfa_green;
	u16 alfa_blue;
	u16 beta_red;
	u16 beta_green;
	u16 beta_blue;
	u16 gamma_red;
	u16 gamma_green;
	u16 gamma_blue;
	u16 off_red;
	u16 off_green;
	u16 off_blue;
};

struct mcde_chnl_state {
	bool enabled;
	bool reserved;
	enum mcde_chnl id;
	enum mcde_fifo fifo;
	struct mcde_port port;
	struct mcde_ovly_state *ovly0;
	struct mcde_ovly_state *ovly1;
	enum chnl_state state;
	struct clk *clk_dpi;
	struct dsilink_device *dsilink;

	enum mcde_display_power_mode power_mode;
	struct mcde_video_mode vmode;
	enum mcde_display_rotation rotation;
	bool oled_color_conversion;

	struct mcde_col_transform rgb_2_ycbcr;
	struct mcde_col_transform ycbcr_2_rgb;
	struct mcde_col_transform *transform;
	struct mcde_oled_transform *oled_transform;

	/* Blending */
	u8 blend_ctrl;
	bool blend_en;
	u8 alpha_blend;

	/* Applied settings */
	struct chnl_regs regs;
	struct col_regs  col_regs;
	struct tv_regs   tv_regs;
	struct oled_regs oled_regs;

	/* an interlaced digital TV signal generates a VCMP per field */
	bool vcmp_per_field;

	bool formatter_updated;

	bool first_frame_vsync_fix;
};

/* MCDE overlay */
struct mcde_chnl_state;

struct mcde_chnl_state *mcde_chnl_get(enum mcde_chnl chnl_id,
			enum mcde_fifo fifo, const struct mcde_port *port);
int mcde_chnl_set_pixel_format(struct mcde_chnl_state *chnl,
					enum mcde_port_pix_fmt pix_fmt);
int mcde_chnl_set_palette(struct mcde_chnl_state *chnl,
					struct mcde_palette_table *palette);
int mcde_chnl_set_video_mode(struct mcde_chnl_state *chnl,
					struct mcde_video_mode *vmode);
/* TODO: Remove rotbuf* parameters when ESRAM allocator is implemented*/
int mcde_chnl_set_rotation(struct mcde_chnl_state *chnl,
					enum mcde_display_rotation rotation);
int mcde_chnl_set_power_mode(struct mcde_chnl_state *chnl,
				enum mcde_display_power_mode power_mode);

int mcde_chnl_apply(struct mcde_chnl_state *chnl);
int mcde_chnl_update(struct mcde_chnl_state *chnl,
			struct mcde_rectangle *update_area,
			bool tripple_buffer);
void mcde_chnl_put(struct mcde_chnl_state *chnl);

void mcde_chnl_stop_flow(struct mcde_chnl_state *chnl);

void mcde_chnl_enable(struct mcde_chnl_state *chnl);
void mcde_chnl_disable(struct mcde_chnl_state *chnl);
void mcde_formatter_enable(struct mcde_chnl_state *chnl);

/* MCDE overlay */
struct mcde_ovly_state;

struct mcde_ovly_state *mcde_ovly_get(struct mcde_chnl_state *chnl);
void mcde_ovly_set_source_buf(struct mcde_ovly_state *ovly,
	u32 paddr);
void mcde_ovly_set_source_info(struct mcde_ovly_state *ovly,
	u32 stride, enum mcde_ovly_pix_fmt pix_fmt);
void mcde_ovly_set_source_area(struct mcde_ovly_state *ovly,
	u16 x, u16 y, u16 w, u16 h);
void mcde_ovly_set_dest_pos(struct mcde_ovly_state *ovly,
	u16 x, u16 y, u8 z);
void mcde_ovly_apply(struct mcde_ovly_state *ovly);
void mcde_ovly_put(struct mcde_ovly_state *ovly);

/* MCDE dsi */

#define DCS_CMD_ENTER_IDLE_MODE       0x39
#define DCS_CMD_ENTER_INVERT_MODE     0x21
#define DCS_CMD_ENTER_NORMAL_MODE     0x13
#define DCS_CMD_ENTER_PARTIAL_MODE    0x12
#define DCS_CMD_ENTER_SLEEP_MODE      0x10
#define DCS_CMD_EXIT_IDLE_MODE        0x38
#define DCS_CMD_EXIT_INVERT_MODE      0x20
#define DCS_CMD_EXIT_SLEEP_MODE       0x11
#define DCS_CMD_GET_ADDRESS_MODE      0x0B
#define DCS_CMD_GET_BLUE_CHANNEL      0x08
#define DCS_CMD_GET_DIAGNOSTIC_RESULT 0x0F
#define DCS_CMD_GET_DISPLAY_MODE      0x0D
#define DCS_CMD_GET_GREEN_CHANNEL     0x07
#define DCS_CMD_GET_PIXEL_FORMAT      0x0C
#define DCS_CMD_GET_POWER_MODE        0x0A
#define DCS_CMD_GET_RED_CHANNEL       0x06
#define DCS_CMD_GET_SCANLINE          0x45
#define DCS_CMD_GET_SIGNAL_MODE       0x0E
#define DCS_CMD_NOP                   0x00
#define DCS_CMD_READ_DDB_CONTINUE     0xA8
#define DCS_CMD_READ_DDB_START        0xA1
#define DCS_CMD_READ_MEMORY_CONTINE   0x3E
#define DCS_CMD_READ_MEMORY_START     0x2E
#define DCS_CMD_SET_ADDRESS_MODE      0x36
#define DCS_CMD_SET_COLUMN_ADDRESS    0x2A
#define DCS_CMD_SET_DISPLAY_OFF       0x28
#define DCS_CMD_SET_DISPLAY_ON        0x29
#define DCS_CMD_SET_GAMMA_CURVE       0x26
#define DCS_CMD_SET_PAGE_ADDRESS      0x2B
#define DCS_CMD_SET_PARTIAL_AREA      0x30
#define DCS_CMD_SET_PIXEL_FORMAT      0x3A
#define DCS_CMD_SET_SCROLL_AREA       0x33
#define DCS_CMD_SET_SCROLL_START      0x37
#define DCS_CMD_SET_TEAR_OFF          0x34
#define DCS_CMD_SET_TEAR_ON           0x35
#define DCS_CMD_SET_TEAR_SCANLINE     0x44
#define DCS_CMD_SOFT_RESET            0x01
#define DCS_CMD_WRITE_LUT             0x2D
#define DCS_CMD_WRITE_CONTINUE        0x3C
#define DCS_CMD_WRITE_START           0x2C
#define DCS_CMD_TURN_ON_PERIPHERIAL   0x32

#define MCDE_MAX_DCS_READ   4
#define MCDE_MAX_DSI_DIRECT_CMD_WRITE 15

int mcde_dsi_generic_write(struct mcde_chnl_state *chnl, u8* para, int len);
int mcde_dsi_dcs_write(struct mcde_chnl_state *chnl,
		u8 cmd, u8 *data, int len);
int mcde_dsi_dcs_read(struct mcde_chnl_state *chnl,
		u8 cmd, u32 *data, int *len);
int mcde_dsi_set_max_pkt_size(struct mcde_chnl_state *chnl);


#define MASK_LDO_VAUX1		0x03
#define MASK_LDO_VAUX1_SHIFT	0x00
#define VAUXSEL_VOLTAGE_MASK	0x0F
#define VANA_ENABLE_IN_HP_MODE	0x05
#define ENABLE_PWM1		0x01
#define PWM_DUTY_LOW_1024_1024	0xFF
#define PWM_DUTY_HI_1024_1024	0x03

/* MCDE */

/* Driver data */
#define MCDE_IRQ     "MCDE IRQ"
#define MCDE_IO_AREA "MCDE I/O Area"

/*
 * Default pixelfetch watermark levels per overlay.
 * Values are in pixels and 2 basic rules should be followed:
 * 1. The value should be at least 256 bits.
 * 2. The sum of all active overlays pixelfetch watermark level multiplied with
 *    bits per pixel, should be lower than the size of input_fifo_size in bits.
 * 3. The value should be a multiple of a line (256 bits).
 */
#define MCDE_PIXFETCH_WTRMRKLVL_OVL0 48		/* LCD 32 bpp */
#define MCDE_PIXFETCH_WTRMRKLVL_OVL1 64		/* LCD 16 bpp */
#define MCDE_PIXFETCH_WTRMRKLVL_OVL2 128	/* HDMI 32 bpp */
#define MCDE_PIXFETCH_WTRMRKLVL_OVL3 192	/* HDMI 16 bpp */
#define MCDE_PIXFETCH_WTRMRKLVL_OVL4 16
#define MCDE_PIXFETCH_WTRMRKLVL_OVL5 16

struct mcde_platform_data {
	/* DPI */
	u8 outmux[5]; /* MCDE_CONF0.OUTMUXx */
	u8 syncmux;   /* MCDE_CONF0.SYNCMUXx */

	u32 pixelfetchwtrmrk[6];

	int (*platform_set_clocks)(void);
	int (*platform_enable_dsipll)(void);
	int (*platform_disable_dsipll)(void);
};

int mcde_init(void);
void mcde_exit(void);

#endif /* __MCDE__H__ */
