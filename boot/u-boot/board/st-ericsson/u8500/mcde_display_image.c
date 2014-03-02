/*
* Copyright (C) ST-Ericsson SA 2010
*
* Author: Torbjorn Svensson <torbjorn.x.svensson@stericsson.com>
* for ST-Ericsson.
*
* License terms: GNU General Public License (GPL), version 2.
*/

#include <common.h>
#include <command.h>
#include <linux/err.h>
#include <part.h>
#include <mmc.h>
#include <bmp_layout.h>
#include <asm/arch/common.h>
#include "mcde.h"
#include "mcde_display.h"

/* bmp compression constants */
#define BI_RGB		0
#define BI_RLE8		1	/* RLE 8-bit/pixel */
#define BI_RLE4		2	/* RLE 4-bit/pixel */
#define BI_BITFIELDS	3

static uint32_t read_unaligned32(uint32_t *val)
{
	uint32_t ret;
	memcpy(&ret, val, sizeof(int32_t));
	return ret;
}

static void copy_indexed_to_fb(u8 *dst_ptr, u8 *src_ptr,
			int dst_pitch, int src_pitch,
			int dst_bpp, int width, int height, u8 *palette)
{
	int i, j;
	int pad;
	u16 val16;
	u32 w;
	u8 r;
	u8 g;
	u8 b;
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
				/* Skip Alpha/unused component */
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

	if (src_bpp != dst_bpp) {
		printf("%s: source format not compatible with destination "
			"format (src_bpp=%d, dst_bpp=%d)\n",
			__func__, src_bpp, dst_bpp);
		return;
	}

	/* Image is up side down */
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
	switch (src_bpp) {
	case 8:
		copy_indexed_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
					dst_bpp, width, height, palette);
		break;

	case 16:
	case 24:
	case 32:
		/*
		 * Note that the splash screen has to be in the same pixel
		 * format as the framebuffer for 16, 24 and 32 bpp. No format
		 * conversion is performed. copy_direct_to_fb will fail if
		 * the bits per pixel for the splash image and the
		 * framebuffer are not the same.
		 */
		copy_direct_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
					dst_bpp, src_bpp, width, height);
		break;

	default:
		printf("%s: unsupported src_bpp=%d\n", __func__, src_bpp);
		break;
	}
}

int mcde_display_image(struct mcde_chnl_state *chnl)
{
	int err = 0;
	struct mmc *emmc_dev;
	u8 *bmp_start;
	struct bmp_header *bmp_header;
	u32 bmp_offset;
	u32 dib_width;
	u32 dib_height;
	u32 dib_compression;
	u16 dib_bpp;
	u32 dib_header_size;
	u8 *palette;
	u32 palette_size;

	u8 *src_ptr;
	int src_pitch;

	u8 *dst_ptr;
	u32 dst_pitch;
	int dst_bpp;

	struct mcde_ovly_state *ovly;
	u32 xpos = 0;
	u32 ypos = 0;

	if (main_display.default_pixel_format == MCDE_OVLYPIXFMT_RGB565) {
		dst_bpp = 16;
	} else if (main_display.default_pixel_format ==
			MCDE_OVLYPIXFMT_RGB888) {
		dst_bpp = 24;
	} else if (main_display.default_pixel_format ==
			MCDE_OVLYPIXFMT_RGBA8888) {
		dst_bpp = 32;
	} else {
		printf("%s: unsupported pixel format %d\n", __func__,
				main_display.default_pixel_format);
		return -EINVAL;
	}

	debug("%s: Enter\n", __func__);
	emmc_dev = find_mmc_device(CONFIG_EMMC_DEV_NUM);
	if (emmc_dev == NULL) {
		printf("mcde_display_image: emmc not found.\n");
		return -ENODEV;
	}

	/* Place the bitmap data just after the frame buffer in memory */;
	bmp_start = (u8 *) (CONFIG_SYS_VIDEO_FB_ADRS + dst_bpp / 8 *
				CONFIG_SYS_DISPLAY_NATIVE_X_RES *
				CONFIG_SYS_DISPLAY_NATIVE_Y_RES);
	debug("%s: bmp start = 0x%p\n", __func__, (void *)bmp_start);

	/* get bmp_image */
	if (toc_load_toc_entry(&emmc_dev->block_dev, MCDE_TOC_SPLASH_NAME, 0,
			       0, (u32) bmp_start)) {
		printf("mcde_display_image: no splash image found.\n");
		return -ENOENT;
	}


	/* check BMP magic */
	bmp_header = (struct bmp_header *)bmp_start;
	if (bmp_header->signature[0] != 'B' ||
		bmp_header->signature[1] != 'M') {
		printf("%s: unsupported filetype, must be BMP\n", __func__);
		return -EILSEQ;
	}

	/* get offset to bitmap-data from the BMP header */
	bmp_offset = read_unaligned32(&bmp_header->data_offset);
	dib_width = read_unaligned32((uint32_t *)&bmp_header->width);
	dib_height = read_unaligned32((uint32_t *)&bmp_header->height);
	dib_bpp = bmp_header->bit_count;
	dib_header_size = read_unaligned32(&bmp_header->size);
	dib_compression = read_unaligned32(&bmp_header->compression);

	debug("bmp filesz = %d\n", read_unaligned32(&bmp_header->file_size));
	debug("bmp offset = %d\n", bmp_offset);
	debug("dib header_sz = %d\n", dib_header_size);
	debug("dib width = %d\n", dib_width);
	debug("dib height = %d\n", dib_height);
	debug("dib nplanes = %d\n", bmp_header->planes);
	debug("dib bpp = %d\n", dib_bpp);
	debug("dib compress_type = %d\n", dib_compression);

	/* calculate palette address */
	palette = (u8 *) &bmp_header->size + dib_header_size;
	palette_size = (bmp_start + bmp_offset) - palette;
	/* if same as image start: no palette */
	if (palette_size == 0)
		palette = NULL;

	debug("palette size = %d\n", palette_size);
	debug("palette = 0x%08x\n", (u32) palette);

	/* check validity */
	if ((dib_width > main_display.native_x_res) ||
			(dib_height > main_display.native_y_res)) {
		printf("%s: image to large, must be [%d,%d] or smaller\n",
			__func__, main_display.native_x_res,
			main_display.native_y_res);
		err++;
	}
	if (bmp_header->planes != 1) {
		printf("%s: unsupported nplanes, must be 1\n", __func__);
		err++;
	}

	if (dib_compression != BI_RGB && dib_compression != BI_BITFIELDS)
		err++;

	if (err != 0)
		return -EINVAL;

	src_pitch = dib_width * dib_bpp / 8;
	src_pitch = (src_pitch + 3) & 0xFFFFFFFC; /* pad to 32-bit boundary */

	dst_pitch = dib_width * dst_bpp / 8;
	dst_pitch = (dst_pitch + 7) & 0xFFFFFFF8; /* pad to 64-bit boundary */

	debug("src_pitch=%d\n", src_pitch);
	debug("dst_pitch=%d\n", dst_pitch);

	/* image is stored upside-down in the file */
	src_ptr = (u8 *) (bmp_start + bmp_offset);
	dst_ptr = (u8 *) CONFIG_SYS_VIDEO_FB_ADRS;
	debug("bmp copy dst=0x%08x, src=0x%08x\n",
		(uint32_t)bmp_dst, (uint32_t)src_ptr);

	copy_to_fb(dst_ptr, src_ptr, dst_pitch, src_pitch,
			dst_bpp, dib_bpp, dib_width, dib_height, palette);
	debug("%s: image OK\n", __func__);

	/* dss_enable_overlay */
	ovly = mcde_ovly_get(chnl);
	if (IS_ERR(ovly)) {
		err = PTR_ERR(ovly);
		printf("%s: Failed to get channel\n", __func__);
		return -err;
	}
	debug("ovly=%p dst_ptr=%p\n", (void *) ovly, (void *) dst_ptr);
	mcde_ovly_set_source_buf(ovly, (u32) dst_ptr);
	mcde_ovly_set_source_info(ovly, dst_pitch,
					main_display.default_pixel_format);
	mcde_ovly_set_source_area(ovly, 0, 0, dib_width, dib_height);
	if (dib_width == main_display.native_x_res)
		xpos = 0;
	else
		xpos = (main_display.native_x_res - dib_width) / 2;

	if (dib_height == main_display.native_y_res)
		ypos = 0;
	else
		ypos = (main_display.native_y_res - dib_height) / 2;
	mcde_ovly_set_dest_pos(ovly, xpos, ypos, 0);
	mcde_ovly_apply(ovly);

	return 0;
}

