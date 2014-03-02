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

#ifndef __MCDE_DISPLAY_H
#define __MCDE_DISPLAY_H

#include "mcde.h"
#include "mcde_device.h"

#define MOP500_DISP1_RST_GPIO	65

#define UPDATE_FLAG_PIXEL_FORMAT	0x1
#define UPDATE_FLAG_VIDEO_MODE		0x2
#define UPDATE_FLAG_ROTATION		0x4

struct mcde_display_dsi_platform_data {
	int reset_gpio;
	u8 link;
	u8 num_data_lanes;
	enum mcde_port_mode port_mode;
};

struct mcde_display_device {
	/* MCDE driver static */
	struct device     dev;
	const char       *name;
	int               id;
	struct mcde_port *port;
	struct fb_info   *fbi;
	bool              fictive;

	/* MCDE dss driver internal */
	bool initialized;
	enum mcde_chnl chnl_id;
	enum mcde_fifo fifo;
	bool first_update;

	bool enabled;
	struct mcde_chnl_state *chnl_state;
	struct mcde_rectangle update_area;

	/* Display driver internal */
	u16 native_x_res;
	u16 native_y_res;
	u16 physical_width;
	u16 physical_height;
	enum mcde_display_power_mode power_mode;
	enum mcde_ovly_pix_fmt default_pixel_format;
	enum mcde_ovly_pix_fmt pixel_format;
	enum mcde_display_rotation rotation;
	enum mcde_display_rotation orientation;
	struct mcde_video_mode video_mode;
	int update_flags;
	bool stay_alive;
	int check_transparency;

	/* Driver API */
	void (*get_native_resolution)(struct mcde_display_device *dev,
		u16 *x_res, u16 *y_res);
	enum mcde_ovly_pix_fmt (*get_default_pixel_format)(
		struct mcde_display_device *dev);
	void (*get_physical_size)(struct mcde_display_device *dev,
		u16 *x_size, u16 *y_size);

	int (*set_power_mode)(struct mcde_display_device *dev,
		enum mcde_display_power_mode power_mode);
	enum mcde_display_power_mode (*get_power_mode)(
		struct mcde_display_device *dev);

	int (*try_video_mode)(struct mcde_display_device *dev,
		struct mcde_video_mode *video_mode);
	int (*set_video_mode)(struct mcde_display_device *dev,
		struct mcde_video_mode *video_mode);
	void (*get_video_mode)(struct mcde_display_device *dev,
		struct mcde_video_mode *video_mode);
	int (*set_pixel_format)(struct mcde_display_device *dev,
		enum mcde_ovly_pix_fmt pix_fmt);
	enum mcde_ovly_pix_fmt (*get_pixel_format)(
		struct mcde_display_device *dev);
	enum mcde_port_pix_fmt (*get_port_pixel_format)(
		struct mcde_display_device *dev);

	int (*set_rotation)(struct mcde_display_device *dev,
		enum mcde_display_rotation rotation);
	enum mcde_display_rotation (*get_rotation)(
		struct mcde_display_device *dev);

	int (*apply_config)(struct mcde_display_device *dev);
	int (*invalidate_area)(struct mcde_display_device *dev,
						struct mcde_rectangle *area);
	int (*update)(struct mcde_display_device *dev, bool tripple_buffer);
	int (*on_first_update)(struct mcde_display_device *dev);
	int (*platform_enable)(struct mcde_display_device *dev);
	int (*platform_disable)(struct mcde_display_device *dev);
	int (*ceanr_convert)(struct mcde_display_device *ddev,
			u8 cea, u8 vesa_cea_nr, int buffering,
			u16 *w, u16 *h, u16 *vw, u16 *vh);
};

struct mcde_display_driver {
	int (*probe)(struct mcde_display_device *dev);
	int (*remove)(struct mcde_display_device *dev);
	void (*shutdown)(struct mcde_display_device *dev);
	int (*suspend)(struct mcde_display_device *dev,
		pm_message_t state);
	int (*resume)(struct mcde_display_device *dev);

	struct device_driver driver;
};

/* MCDE dsi (Used by MCDE display drivers) */

int mcde_display_dsi_dcs_write(struct mcde_display_device *dev,
	u8 cmd, u8 *data, int len);
int mcde_display_dsi_dcs_read(struct mcde_display_device *dev,
	u8 cmd, u8 *data, int *len);
int mcde_display_dsi_bta_sync(struct mcde_display_device *dev);

/* MCDE display bus */

int mcde_display_driver_register(struct mcde_display_driver *drv);
void mcde_display_driver_unregister(struct mcde_display_driver *drv);
int mcde_display_device_register(struct mcde_display_device *dev);
void mcde_display_device_unregister(struct mcde_display_device *dev);

void mcde_display_init_device(struct mcde_display_device *dev);

int mcde_display_init(void);
void mcde_display_exit(void);

int mcde_startup_dpi(struct mcde_chnl_state *chnl);
int mcde_start_dsi(void);
int mcde_display_image(struct mcde_chnl_state *chnl,
					enum mcde_display_rotation rotation);

extern struct mcde_display_device *display0;
extern struct mcde_video_mode video_mode;
extern struct mcde_video_mode video_mode_sony;

#endif
