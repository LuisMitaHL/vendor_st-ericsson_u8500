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

#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "config.h"
#include "target_config.h"
#include "mcde.h"
#include "mcde_display.h"
#include "toc.h"

#define MCDE_TOC_SPLASH_NAME	"SPLASH"

extern int display_command_line;
static void copy_indexed_to_fb(u8 *dst_ptr, u8 *src_ptr,
			int dst_pitch, int src_pitch,
			int dst_bpp, int width, int height, u8 *palette)
{
	int i, j, pad;
	u16 val16;
	u32 w;
	u8 r,g,b;
	u8 *src;

	pad = dst_pitch - dst_bpp * width / 8;
	for (i = height - 1; i >= 0; i--) {
		src = src_ptr + i * src_pitch;
		for (j = 0; j < width; j++) {
			w = *src++;		/* get index into palette */
			w <<= 2;		/* make offset */
			w += (u32) palette;	/* make address */
			r = *((u8 *) w++);
			g = *((u8 *) w++);
			b = *((u8 *) w);

			if (dst_bpp == 32) {
				*dst_ptr++ = r;
				*dst_ptr++ = g;
				*dst_ptr++ = b;
				*dst_ptr++ = 255;
			} else if (dst_bpp == 24) {
				*dst_ptr++ = r;
				*dst_ptr++ = g;
				*dst_ptr++ = b;
			} else if (dst_bpp == 16) {
				val16 = r >> 3;
				val16 |= (g >> 2) << 5;
				val16 |= (b >> 3) << 11;
				*((u16 *) dst_ptr) = val16;
				dst_ptr += 2;
			}
		}
		dst_ptr += pad;
	}
}

static void copy_direct_to_fb(u8 *dst_ptr, u8 *src_ptr, int dst_pitch,
				int src_pitch, int dst_bpp, int src_bpp,
				int width, int height)
{
	int i;
	if (src_bpp != dst_bpp) return;
	src_ptr += height * src_pitch;
	for (i = 0; i < height; i++) {
		src_ptr -= src_pitch;
		memcpy(dst_ptr, src_ptr, src_pitch);
		dst_ptr += dst_pitch;
	}
}

static void copy_to_fb(u8 *dst_ptr, u8 *src_ptr,
						int dst_pitch, int src_pitch,
						int dst_bpp, int src_bpp,
						int width, int height,
						u8 *palette)
{
	if (src_bpp == 8)
		copy_indexed_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
					dst_bpp, width, height, palette);
	else
		copy_direct_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
					dst_bpp, src_bpp, width, height);
}

int mcde_display_image(struct mcde_chnl_state *chnl,
					enum mcde_display_rotation rotation)
{
	int err = 0;
	u8 *bmp_start;
	bmp_header_t *bmp_header;
	u32 bmp_offset;
	u32 dib_bytesz;
	u32 dib_width;
	u32 dib_height;
	u32 dib_compression;
	u16 dib_bpp;
	u32 dib_header_size;
	u8 *palette;
	u32 palette_size;
	struct mcde_ovly_state *ovly;

	u8 *src_ptr;
	int src_pitch;

	u8 *dst_ptr;
	u32 dst_pitch;
	int dst_bpp = 16;	/* output format (same as input)*/
	int dst_delta_x;
	int dst_delta_y;

	u32 xpos = 0;
	u32 ypos = 0;

	ovly = mcde_ovly_get(chnl);
	if (!ovly) {
		dprintf(CRITICAL, "%s: mcde_ovly_get returned NULL\n",
								__func__);
		return -1;
	}

	/* Place the bitmap data just after the frame buffer in memory */;
	bmp_start = (u8 *) (CONFIG_SYS_VIDEO_FB_ADRS + dst_bpp / 8 *
			chnl->vmode.xres * chnl->vmode.yres);

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if defined(CONFIG_DISPLAY_CMD)
	if(!display_command_line)
	{
#endif
	/* get bmp_image */
	if ((struct toc_entry *)~0 == toc_load_id(MCDE_TOC_SPLASH_NAME, (u32) bmp_start)) {
		dprintf (CRITICAL, "%s: %s not found\n", __func__, MCDE_TOC_SPLASH_NAME);
		return ERR_NOT_FOUND;
	}
#if defined(CONFIG_DISPLAY_CMD)
	}
#endif
#endif
	/* check BMP magic */
	bmp_header = (bmp_header_t *)bmp_start;
	if (bmp_header->signature[0] != 'B' || bmp_header->signature[1] != 'M')
		return ERR_NOT_SUPPORTED;

	/* get offset to bitmap-data from the BMP header */
	bmp_offset = read_u32(&bmp_header->data_offset);
	dib_width = read_u32((uint32_t *)&bmp_header->width);
	dib_height = read_u32((uint32_t *)&bmp_header->height);
	dib_bytesz = read_u32(&bmp_header->image_size);
	dib_bpp = bmp_header->bit_count;
	dib_header_size = read_u32(&bmp_header->size);
	dib_compression = read_u32(&bmp_header->compression);

	/* calculate palette address */
	palette = (u8 *) &bmp_header->size + dib_header_size;
	palette_size = (bmp_start + bmp_offset) - palette;
	/* if same as image start: no palette */
	if (palette_size == 0) palette = NULL;

	/* check validity */
	if ((dib_width > chnl->vmode.xres) ||
					(dib_height > chnl->vmode.yres)) {
		dprintf(CRITICAL,
			"%s: image [%dx%d] does not fit dispay [%dx%d] \n",
					__func__, dib_width, dib_height,
					chnl->vmode.xres, chnl->vmode.yres);
		err++;
	}
	if (bmp_header->planes != 1) {
		dprintf(CRITICAL,
			"%s: image has too many planes (%d) \n", __func__,
							bmp_header->planes);
		err++;
	}
	if (dib_compression != 0 && dib_compression != 3) {
		dprintf(CRITICAL,
			"%s: image has unsupported compression (%d) \n",
						__func__, dib_compression);
		err++;
	}
	if (err != 0)
		return ERR_NOT_VALID;

	src_pitch = dib_width * dib_bpp / 8;
	src_pitch = (src_pitch + 3) & 0xFFFFFFFC; /* pad to 32-bit boundary */
	dst_pitch = chnl->vmode.xres * dst_bpp / 8;
	dst_pitch = (dst_pitch + 7) & 0xFFFFFFF8; /* pad to 64-bit boundary */

	/* image is stored upside-down in the file */
	src_ptr = (u8 *) (bmp_start + bmp_offset);
	dst_ptr = (u8 *) CONFIG_SYS_VIDEO_FB_ADRS;

	switch (rotation) {
	case MCDE_DISPLAY_ROT_0:
		dst_delta_x = 1;
		dst_delta_y = dst_pitch;
		break;
	case MCDE_DISPLAY_ROT_90_CCW:
		dst_delta_x = -dst_pitch;
		dst_delta_y = 1;
		break;
	case MCDE_DISPLAY_ROT_180_CCW:
		dst_delta_x = -1;
		dst_delta_y = -dst_pitch;
		break;
	case MCDE_DISPLAY_ROT_270_CCW:
		dst_delta_x = dst_pitch;
		dst_delta_y = -1;
		break;
	default:
		dprintf(CRITICAL, "%s: unsupported rotation %d \n", __func__,
								rotation);
		return ERR_NOT_ALLOWED;
	}
	/* TODO: handle rotation properly */
	copy_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
		dst_bpp, dib_bpp, dib_width, dib_height, palette);
	mcde_ovly_set_source_buf(ovly, (u32)dst_ptr);
	mcde_ovly_set_source_info(ovly, dst_pitch, MCDE_OVLYPIXFMT_RGB565);
	mcde_ovly_set_source_area(ovly, 0, 0, dib_width, dib_height);

	if (dib_width != chnl->vmode.xres)
				xpos = (chnl->vmode.xres - dib_width) / 2;

	if (dib_height != chnl->vmode.yres)
		ypos = ( chnl->vmode.yres - dib_height) / 2;

	mcde_ovly_set_dest_pos(ovly, xpos, ypos, 1);
	mcde_ovly_apply(ovly);

	return 0;
}
