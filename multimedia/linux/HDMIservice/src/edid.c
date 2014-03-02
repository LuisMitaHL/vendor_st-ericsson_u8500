/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Per Persson per.xb.persson@stericsson.com for
 * ST-Ericsson.
 *
 * License terms:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <linux/types.h>
#include <errno.h>      /* Errors */
#include <stdarg.h>
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#ifdef ANDROID
#include <utils/Log.h>
#endif
#include "../include/hdmi_service_api.h"
#include "../include/hdmi_service_local.h"

struct edid_stdtim_ar {
	int x;
	int y;
};

const __u8 edid_block0_start[] = {
			0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
const __u8 edid_stdtim9_flag[] = {0x00, 0x00, 0x00, 0xFA, 0x00};
const __u8 edid_esttim3_flag[] = {0x00, 0x00, 0x00, 0xF7, 0x00};
const __u8 edid_esttim1_2_offset[] = {EDID_BL0_ESTTIM1_OFFSET,
					EDID_BL0_ESTTIM2_OFFSET};
const __u8 edid_esttim3_flag_offset[] = {EDID_BL1_ESTTIM3_1_FLAG_OFFSET,
					EDID_BL1_ESTTIM3_2_FLAG_OFFSET,
					EDID_BL1_ESTTIM3_3_FLAG_OFFSET};
const __u8 edid_stdtim9_flag_offset[] = {EDID_BL1_STDTIM9_1_FLAG_OFFSET,
					EDID_BL1_STDTIM9_2_FLAG_OFFSET,
					EDID_BL1_STDTIM9_3_FLAG_OFFSET};
/* Aspect ratios */
const struct edid_stdtim_ar edid_stdtim_ar[] = {
		{16, 10},
		{4, 3},
		{5, 4},
		{16, 9}
};

struct vesacea_modes {
	int xres;
	int yres;
	int freq;
	int cea;
	int vesaceanr;
	int interlaced;
};

static struct vesacea_modes vesacea_modes[] = {
	{800, 600, 60, 0, 9, 0},
	{848, 480, 60, 0, 14, 0},
	{1024, 768, 60, 0, 16, 0},
	{1280, 768, 60, 0, 23, 0},
	{1280, 800, 60, 0, 28, 0},
	{1360, 768, 60, 0, 39, 0},
	{1366, 768, 60, 0, 81, 0},
	{640, 480, 60, 1, 1, 0},
	{720, 480, 60, 1, 3, 0},
	{720, 576, 50, 1, 18, 0},
	{1280, 720, 60, 1, 4, 0},
	{1280, 720, 50, 1, 19, 0},
	{1280, 720, 30, 1, 62, 0},
	{1280, 720, 24, 1, 60, 0},
	{1280, 720, 25, 1, 61, 0},
	{1920, 1080, 30, 1, 34, 0},
	{1920, 1080, 24, 1, 32, 0},
	{1920, 1080, 25, 1, 33, 0},
	{720, 240, 60, 1, 7, 1},
	{720, 288, 50, 1, 22, 1},
	{1920, 540, 50, 1, 20, 1},
	{1920, 540, 60, 1, 5, 1}
};

static int get_vesanr_from_est_timing(int timing, int byte, int bit)
{
	int vesa_nr = -1;

	LOGHDMILIB2("timing:%d bit:%d", timing, bit);

	switch (timing) {
	/* Established Timing 1 */
	case 1:
		switch (bit) {
		case 5:
			vesa_nr = 4;
			break;
		case 0:
			vesa_nr = 9;
			break;
		default:
			break;
		}
		break;
	/* Established Timing 2 */
	case 2:
		switch (bit) {
		case 3:
			vesa_nr = 16;
			break;
		default:
			break;
		}
		break;
	/* Established Timing 3 */
	case 3:
		switch (byte) {
		case 6:
			switch (bit) {
			case 3:
				vesa_nr = 14;
				break;
			default:
				break;
			}
			break;
		case 7:
			switch (bit) {
			case 7:
				vesa_nr = 23;
				break;
			case 6:
				vesa_nr = 22;
				break;
			default:
				break;
			}
			break;
		case 8:
			switch (bit) {
			case 7:
				vesa_nr = 39;
				break;
			default:
				break;
			}
			break;
		}
		break;
	default:
		break;
	}

	return vesa_nr;
}

static int get_vesaceanr_from_timing(int xres, int yres, int freq, int intlcd,
						int *cea, int *vesaceanr)
{
	int nr_of_timings;
	int index;

	*cea = 0;
	*vesaceanr = -1;
	nr_of_timings = sizeof(vesacea_modes)/sizeof(vesacea_modes[0]);
	for (index = 0; index < nr_of_timings; index++) {
		if ((xres == vesacea_modes[index].xres) &&
				(yres == vesacea_modes[index].yres) &&
				(abs((long)freq -
				(long)vesacea_modes[index].freq) <= 1) &&
				(intlcd == vesacea_modes[index].interlaced)) {
			*cea = vesacea_modes[index].cea;
			*vesaceanr = vesacea_modes[index].vesaceanr;
			break;
		}
	}

	return 0;
}

static int get_freq(int ceanr)
{
	int value;

	switch (ceanr) {
	case 60:
	case 32:
		value = 24;
		break;
	case 61:
	case 33:
		value = 25;
		break;
	case 62:
	case 34:
		value = 30;
		break;
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 37:
	case 38:
	case 39:
		value = 50;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 35:
	case 36:
		value = 60;
		break;
	default:
		value = 0;
		break;
	}

	return value;
}

/* Request and read EDID message for specified block */
int edid_read(__u8 block, __u8 *data)
{
	int edidread;
	int res;
	int result = 0;
	__u8 buf[16];
	int size;

	LOGHDMILIB("EDID read blk %d", block);
	/* Request edid block 0 */
	edidread = open(EDIDREAD_FILE, O_RDWR);
	if (edidread < 0) {
		LOGHDMILIBE("***** Failed to open %s *****", EDIDREAD_FILE);
		result = -1;
		goto edid_read_end2;
	}

	buf[0] = 0xA0;
	buf[1] = block;
	size = 2;
	res = write(edidread, buf, size);
	if (res < 0) {
		LOGHDMILIBE("***** Failed to write %s *****", EDIDREAD_FILE);
		result = -2;
		goto edid_read_end1;
	}

	/* Check edid response */
	lseek(edidread, 0, SEEK_SET);
	res = read(edidread, data, EDIDREAD_SIZE);
	if (res != EDIDREAD_SIZE) {
		LOGHDMILIBE("***** %s read error size: %d *****", EDIDREAD_FILE,
				res);
		result = -3;
		goto edid_read_end1;
	}

edid_read_end1:
	close(edidread);
edid_read_end2:
	return result;
}

/* Parse EDID block 0 */
int edid_parse0(__u8 *data, __u8 *extensions, struct video_format formats[],
			int nr_formats)
{
	__u8 version;
	__u8 revision;
	__u8 est_timing;
	int vesa_nr;
	int cea;
	int vesaceanr;
	int bit;
	int cnt;
	int index;
	int xres;
	int yres;
	int byte;
	int ar_index;
	int freq;
	__u8 edidp;
	int pixclk;
	int horblank;
	int vertblank;
	int intlcd;

	/* Header */
	if (memcmp(data + EDID_BL0_HEADER_OFFSET, edid_block0_start, 8) != 0) {
		LOGHDMILIB("edid response:\n%02x %02x %02x %02x %02x %02x %02x "
				"%02x",
				*(data + 1),
				*(data + 2),
				*(data + 3),
				*(data + 4),
				*(data + 5),
				*(data + 6),
				*(data + 7),
				*(data + 8)
				);
		return EDIDREAD_FAIL;
	} else {
		LOGHDMILIB("%s", "--- EDID block 0 start OK ---");
	}

	/* Ver and Rev */
	version = *(data + EDID_BL0_VERSION_OFFSET);
	revision = *(data + EDID_BL0_REVISION_OFFSET);
	LOGHDMILIB("Ver:%d Rev:%d", version, revision);

	/* Read Established Timings 1&2 and set sink_support */
	for (index = 0; index <= 1; index++) {
		est_timing = *(data + edid_esttim1_2_offset[index]);
		LOGHDMILIB2("EstTim%d:%02x", index + 1, est_timing);
		if (est_timing == 0)
			continue;

		for (bit = 7; bit >= 0; bit--) {
			if (est_timing & (1 << bit)) {
				vesa_nr = get_vesanr_from_est_timing(index + 1,
									0, bit);
				LOGHDMILIB2("vesa_nr:%d", vesa_nr);
				if (vesa_nr < 1)
					continue;

				LOGHDMILIB2("EstTim1&2 try vesa_nr:%d",
							vesa_nr);
				for (cnt = 0; cnt < nr_formats; cnt++) {
					LOGHDMILIB3("with:%d",
							formats[cnt].vesaceanr);
					if ((formats[cnt].cea == 0) &&
						(formats[cnt].vesaceanr ==
								vesa_nr)) {
						formats[cnt].sink_support = 1;
						LOGHDMILIB("EstTim1&2 %d "
								"vesa_nr:%d",
								index + 1,
								vesa_nr);
						break;
					}
				}
			}
		}
	}

	/* Read Standard Timings 1-8 and set sink_support*/
	for (index = 0; index < EDID_BL0_STDTIM1_SIZE; index++) {
		edidp = EDID_BL0_STDTIM1_OFFSET + index * 2;
		xres = (*(data + edidp) + 31) * 8;
		byte = *(data + edidp + 1);
		ar_index = (byte & EDID_STDTIM_AR_MASK) >> EDID_STDTIM_AR_SHIFT;
		yres = xres * edid_stdtim_ar[ar_index].y /
					edid_stdtim_ar[ar_index].x;
		freq = 60 + ((byte & EDID_STDTIM_FREQ_MASK) >>
				EDID_STDTIM_FREQ_SHIFT);
		LOGHDMILIB2("xres:%d yres:%d freq:%d", xres, yres, freq);
		get_vesaceanr_from_timing(xres, yres, freq, 0, &cea,
								&vesaceanr);
		if (vesaceanr < 1)
			continue;

		LOGHDMILIB2("StdTim1to8 try cea %d ceavesanr:%d", cea,
								vesaceanr);
		for (cnt = 0; cnt < nr_formats; cnt++) {
			LOGHDMILIB3("with:%d",
				formats[cnt].vesaceanr);
			if ((formats[cnt].cea == cea) &&
					(formats[cnt].vesaceanr ==
							vesaceanr)) {
				formats[cnt].sink_support = 1;
				LOGHDMILIB("StdTim1to8 %d cea %d vesaceanr:%d",
						index + 1, cea,	vesaceanr);
				break;
			}
		}
	}

	pixclk = *(data + EDID_BL0_PIXCLK_OFFSET);
	pixclk += *(data + EDID_BL0_PIXCLK_OFFSET + 1) << 8;
	if (pixclk == 0)
		goto edid_parse0_ext;

	/* Detailed timing definition */
	pixclk *= 10000;
	xres = *(data + EDID_BL0_HORPIXLOW_OFFSET);
	xres += (*(data + EDID_BL0_HORPIXHIGH_OFFSET) & 0xF0) << 4;
	yres = *(data + EDID_BL0_VERTPIXLOW_OFFSET);
	yres += (*(data + EDID_BL0_VERTPIXHIGH_OFFSET) & 0xF0) << 4;
	horblank = *(data + EDID_BL0_HORBLANKLOW_OFFSET);
	horblank += (*(data + EDID_BL0_HORBLANKHIGH_OFFSET) & 0x0F) << 8;
	vertblank = *(data + EDID_BL0_VERTBLANKLOW_OFFSET);
	vertblank += (*(data + EDID_BL0_VERTBLANKHIGH_OFFSET) & 0x0F) << 8;
	intlcd = (*(data + EDID_BL0_INTLCD_OFFSET) & 0x80) >> 7;

	LOGHDMILIB2("Dettim: pixclk:%d xres:%d yres:%d horbl:%d vertbl:%d "
			"intlcd:%d",
			pixclk, xres, yres, horblank, vertblank, intlcd);

	if ((xres == 0) || (yres == 0))
		goto edid_parse0_ext;

	freq = pixclk / ((xres + horblank) * (yres + vertblank));
	LOGHDMILIB2("Dettim: freq:%d", freq);
	get_vesaceanr_from_timing(xres, yres, freq, intlcd, &cea, &vesaceanr);
	if (vesaceanr > 0) {
		LOGHDMILIB2("DetTim try cea %d vesa_nr:%d", cea, vesaceanr);
		for (cnt = 0; cnt < nr_formats; cnt++) {
			LOGHDMILIB3("with:%d", formats[cnt].vesaceanr);
			if ((formats[cnt].cea == cea) &&
					(formats[cnt].vesaceanr == vesaceanr)) {
				formats[cnt].sink_support = 1;
				LOGHDMILIB("DetTim cea %d vesaceanr:%d",
						cea, vesaceanr);
				break;
			}
		}
	}

edid_parse0_ext:
	*extensions = *(data + EDID_BL0_EXTFLAG_OFFSET);

	return RESULT_OK;
}

int edid_parse_ext_map(__u8 *data)
{
	__u8 tag;

	tag = *(data + EDID_BL1_TAG_OFFSET);
	if (tag == EDID_TAG_EXT_BLOCK_MAP)
		return RESULT_OK;
	else
		return EDIDREAD_EXTENSION_MAP_ERR;
}

static int set_mandatory_3d_sink_support(struct threed_info *info)
{
	int cnt;

	LOGHDMILIB("%s", __func__);

	/*
	 * HDMI spec ch. 8.3.2
	 * 50 Hz supported: find and set mandatory support for cea 4,5,32
	 * 60 Hz supported: find and set mandatory support for cea 19,20,32
	 */

	if (info->support_2d_60)
		for (cnt = 0; cnt < MAX_VIC_3D; cnt++)
			switch (info->format[cnt].ceanr) {
			case 4:
			case 32:
				info->format[cnt].format |=
						FORMAT_3D_TOP_BOTTOM;
				info->format[cnt].format |=
						FORMAT_3D_FRAME_PACKING;
				break;
			case 5:
				info->format[cnt].format |= FORMAT_3D_SIDE_SIDE;
				break;
			default:
				break;
			}


	if (info->support_2d_50)
		for (cnt = 0; cnt < MAX_VIC_3D; cnt++)
			switch (info->format[cnt].ceanr) {
			case 19:
			case 32:
				info->format[cnt].format |=
						FORMAT_3D_TOP_BOTTOM;
				info->format[cnt].format |=
						FORMAT_3D_FRAME_PACKING;
				break;
			case 20:
				info->format[cnt].format |= FORMAT_3D_SIDE_SIDE;
				break;
			default:
				break;
			}

	for (cnt = 0; cnt < MAX_VIC_3D; cnt++)
		if (info->format[cnt].format)
			LOGHDMILIB("cea:%02x fmt:%02x",
					info->format[cnt].ceanr,
					info->format[cnt].format);

	return 0;
}

static int set_3d_structure_all_sink_support(struct threed_info *info,
						__u16 all,
						__u16 mask)
{
	int cnt;

	LOGHDMILIB("%s all:%04x mask:%04x", __func__, all, mask);

	for (cnt = 0; cnt < MAX_VIC_3D; cnt++)
		if ((mask >> cnt) & 0x01) {
			if (all & EDID_VSD_3D_STR_ALL_FRAME_PACKING)
				info->format[cnt].format |=
						FORMAT_3D_FRAME_PACKING;
			if (all & EDID_VSD_3D_STR_ALL_TOP_BOTTOM)
				info->format[cnt].format |=
						FORMAT_3D_TOP_BOTTOM;
			if (all & EDID_VSD_3D_STR_ALL_SIDE_SIDE)
				info->format[cnt].format |=
						FORMAT_3D_SIDE_SIDE;

			LOGHDMILIB("cea:%02x fmt:%02x",
						info->format[cnt].ceanr,
						info->format[cnt].format);
		}

	return 0;
}

static int set_3d_structure_sink_support(struct threed_info *info,
					__u8 threed_structure[])
{
	int cnt;

	LOGHDMILIB("%s", __func__);

	for (cnt = 0; cnt < MAX_VIC_3D; cnt++) {
		int log = 0;

		switch (threed_structure[cnt]) {
		case EDID_VSD_3D_STR_FRAME_PACKING:
			info->format[cnt].format |= FORMAT_3D_FRAME_PACKING;
			log = 1;
			break;
		case EDID_VSD_3D_STR_TOP_BOTTOM:
			info->format[cnt].format |= FORMAT_3D_TOP_BOTTOM;
			log = 1;
			break;
		case EDID_VSD_3D_STR_SIDE_SIDE:
			info->format[cnt].format |= FORMAT_3D_SIDE_SIDE;
			log = 1;
			break;
		default:
			break;
		}

		if (log)
			LOGHDMILIB("cea:%02x fmt:%02x",
						info->format[cnt].ceanr,
						info->format[cnt].format);
	}
	return 0;
}

static int edid_parse_vsdb(__u8 *datap, __u8 length,
		struct edid_latency *edid_latency,
		struct threed_info *info_3d)
{
	__u8 *p;
	__u8 latency_present;
	__u8 i_latency_present;
	__u8 video_present;
	__u8 video_offset;
	__u8 threed_present;
	__u8 threed_multi_present;
	__u8 vic_len;
	__u8 threed_len;
	__u8 cnt;
	__u8 threed_vic[VIC_3D_SIZE];
	__u16 threed_structure_all = 0;
	__u16 threed_mask = 0xFFFF;
	__u8 threed_len_rem;
	__u8 threed_structure[16] = {0};
	__u8 threed_detail[16] = {0};
	int index;

	p = datap;
	/* Source Physical Address */
	if (length >= (EDID_VSD_PHYS_SRC + 1)) {
		LOGHDMILIB("source physaddr:%02x%02x",
			*(p + EDID_VSD_PHYS_SRC),
			*(p + EDID_VSD_PHYS_SRC + 1));

		/*TODO logical addr (HDMI spec p.192)*/
	}

	latency_present = (*(p + EDID_VSD_FLAG_IND) &
			EDID_VSD_LAT_FLD_MASK) != 0;
	i_latency_present = (*(p + EDID_VSD_FLAG_IND) &
			EDID_VSD_INTLCD_LAT_FLD_MASK) != 0;
	video_present = (*(p + EDID_VSD_FLAG_IND) &
			EDID_VSD_VIDEO_FLD_MASK) != 0;

	/* Video and Audio latency */
	if ((length >= EDID_VSD_AUD_LAT) && latency_present) {
		edid_latency->video_latency = 2 * (*(p + EDID_VSD_VID_LAT) - 1);
		edid_latency->audio_latency = 2 * (*(p + EDID_VSD_AUD_LAT) - 1);
	}

	/* Interlaced Video and Audio latency */
	if ((length >= EDID_VSD_INTLCD_AUD_LAT) && i_latency_present) {
		edid_latency->intlcd_video_latency =
				2 * (*(p + EDID_VSD_INTLCD_VID_LAT) - 1);
		edid_latency->audio_latency =
				2 * (*(p + EDID_VSD_INTLCD_AUD_LAT) - 1);
	}

	if (!video_present)
		goto edid_parse_vsdb_end;

	/* 3D support */
	video_offset = EDID_VSD_FLAG_IND + 1;
	if (latency_present)
		video_offset += 2;
	if (i_latency_present)
		video_offset += 2;
	if (length < (video_offset + 1))
		goto edid_parse_vsdb_end;

	p += video_offset;
	threed_present = ((*p) & EDID_VSD_3D_MASK) != 0;
	if (threed_present)
		set_mandatory_3d_sink_support(info_3d);

	threed_multi_present = ((*p) & EDID_VSD_3DM_MASK) >> EDID_VSD_3DM_SHIFT;
	LOGHDMILIB("threed_multi_present:%d", threed_multi_present);

	p++;
	vic_len = ((*p) & EDID_VSD_VICLEN_MASK) >> EDID_VSD_VICLEN_SHIFT;
	threed_len = ((*p) & EDID_VSD_3DLEN_MASK);
	threed_len_rem = threed_len;
	LOGHDMILIB("viclen:%d 3dlen:%d", vic_len, threed_len);

	/* 3D VIC */
	if (length < (video_offset + 1 + vic_len + threed_len))
		goto edid_parse_vsdb_end;

	/*
	 * Extended resolutions 4k x 2k
	 * Currently parsed but not handled
	 */
	for (cnt = 0; cnt < vic_len; cnt++) {
		p++;
		threed_vic[cnt] = *p;
		threed_len_rem--;
	}

	/* 3D Structure All */
	if ((threed_multi_present == 1) || (threed_multi_present == 2)) {
		p++;
		threed_structure_all = (*p) << THREED_STRUCTURE_ALL_HIGH_SHIFT;
		threed_len_rem--;
		p++;
		threed_structure_all += *p;
		threed_len_rem--;
	}

	/* 3D Mask */
	if (threed_multi_present == 2) {
		p++;
		threed_mask = (*p) << THREED_MULTIPRES_HIGH_SHIFT;
		threed_len_rem--;
		p++;
		threed_mask += *p;
		threed_len_rem--;
	}

	set_3d_structure_all_sink_support(info_3d, threed_structure_all,
							threed_mask);

	if (threed_len_rem == 0)
		goto edid_parse_vsdb_end;

	/* 2D VIC order, 3D Structure, 3D Detail */
	cnt = 0;
	while (threed_len_rem) {
		if (cnt >= 16)
			break;

		p++;
		index = ((*p) & TWOD_VIC_ORDER_MASK) >> TWOD_VIC_ORDER_SHIFT;
		threed_structure[index] = (*p) & THREED_STRUCTURE_MASK;
		threed_len_rem--;
		if (threed_len_rem && (threed_structure[index] > 0x07)) {
			/* 3D Detail is present */
			p++;
			/* 3D Detail is currently not used */
			threed_detail[cnt] = ((*p) & THREED_DETAIL_MASK) >>
						THREED_DETAIL_SHIFT;
			threed_len_rem--;
		}
		cnt++;
	}

	set_3d_structure_sink_support(info_3d, threed_structure);

edid_parse_vsdb_end:
	return 0;
}

/* Parse EDID extension block 1 */
int edid_parse1(__u8 *data, struct video_format formats[], int nr_formats,
		int *basic_audio_support, struct edid_latency *edid_latency,
		int *hdmi, struct threed_info *info_3d)
{
	__u8 tag;
	__u8 rev;
	__u8 offset;
	__u8 blockp;
	__u8 code;
	__u8 length = 0;
	__u8 ceanr;
	int index;
	int index2;
	int cnt;
	__u8 est_timing3;
	int byte;
	int bit;
	int cea;
	int vesa_nr;
	int vesaceanr;
	int xres;
	int yres;
	int ar_index;
	int freq;
	__u8 edidp;
	__u8 vsdb_p = 0;
	__u8 vsdb_len = 0;

	tag = *(data + EDID_BL1_TAG_OFFSET);
	rev = *(data + EDID_BL1_REVNR_OFFSET);

	if (tag != EDID_BL1_TAG_EXPECTED) {
		LOGHDMILIB("edid bl1 tag:%02x or rev:%02x", tag, rev);
		return EDIDREAD_BL1_TAG_REV_ERR;
	}

	if (rev >= EDID_EXTVER_3)
		*hdmi = 1;
	else
		*hdmi = 0; /* Only DVI */

	offset = *(data + EDID_BL1_OFFSET_OFFSET);
	if (offset <= EDID_NO_DATA)
		return RESULT_OK;

	LOGHDMILIB("rev:%d offset:%d", rev, offset);

	/* Check Audio support */
	if (*(data + EDID_BL1_AUDIO_SUPPORT_OFFSET) &
			EDID_BASIC_AUDIO_SUPPORT_MASK) {
		*basic_audio_support = 1;
	}

	for (edidp = EDID_BLK_START; edidp < offset;
				edidp = edidp + length + 1) {
		code = (*(data + edidp) & EDID_BLK_CODE_MSK) >>
						EDID_BLK_CODE_SHIFT;
		length = *(data + edidp) & EDID_BLK_LENGTH_MSK;

		if ((offset + length) >= EDIDREAD_SIZE)
			return EDIDREAD_FAIL;

		LOGHDMILIB2("code:%d blklen:%d", code, length);

		switch (code) {
		case EDID_CODE_VIDEO:
			index = 0;
			for (blockp = edidp + 1; blockp < edidp + 1 + length;
							blockp++) {
				ceanr = *(data + blockp) & EDID_SVD_ID_MASK;
				LOGHDMILIB2("try ceanr:%d", ceanr);
				for (cnt = 0; cnt < nr_formats; cnt++) {
					LOGHDMILIB3("with:%d",
							formats[cnt].vesaceanr);
					if ((formats[cnt].cea == 1) &&
						(formats[cnt].vesaceanr ==
								ceanr)) {
						formats[cnt].sink_support = 1;
						LOGHDMILIB("cea:%d", ceanr);
						break;
					}
				}

				/* The first VICs are used for 3D support */
				if (index < MAX_VIC_3D)
					info_3d->format[index++].ceanr = ceanr;

				/* Freq is needed for mandatory 3D support */
				switch (get_freq(ceanr)) {
				case 50:
					if (info_3d->support_2d_50 == 0)
						LOGHDMILIB("%s",
							"support_2d_50");
					info_3d->support_2d_50 = 1;
					break;
				case 60:
					if (info_3d->support_2d_60 == 0)
						LOGHDMILIB("%s",
							"support_2d_60");
					info_3d->support_2d_60 = 1;
					break;
				default:
					break;
				}
			}
			break;

		/* Vendor Specific Data Block */
		case EDID_CODE_VSDB:
			vsdb_p = edidp;
			vsdb_len = length;
			break;

		default:
			break;
		}
	}

	if (vsdb_p)
		edid_parse_vsdb(data + vsdb_p, vsdb_len, edid_latency, info_3d);

	/* Read Established Timing 3 and set sink_support */
	for (index = 0; index <= 2; index++) {
		edidp = edid_esttim3_flag_offset[index];

		/* Check for Established Timing3 flag */
		if (memcmp(data + edidp, edid_esttim3_flag,
				sizeof(edid_esttim3_flag)) != 0)
			/* Flag mismatch, this is not Established Timing 3 */
			continue;

		for (byte = EDID_BL1_ESTTIM3_BYTE_START;
				byte <= EDID_BL1_ESTTIM3_BYTE_END; byte++) {
			est_timing3 = *(data + edidp + byte);
			for (bit = 7; bit >= 0; bit--) {
				if ((est_timing3 & (1 << bit)) == 0)
					/* Not supported in sink */
					continue;

				vesa_nr = get_vesanr_from_est_timing(3, byte,
									bit);
				/* Set sink_suuport */
				LOGHDMILIB2("EstTim3 try vesa_nr:%d", vesa_nr);
				for (cnt = 0; cnt < nr_formats; cnt++) {
					LOGHDMILIB3("with:%d",
							formats[cnt].vesaceanr);
					if ((formats[cnt].cea == 0) &&
						(formats[cnt].vesaceanr ==
								vesa_nr)) {
						formats[cnt].sink_support = 1;
						LOGHDMILIB("EstTim3 vesa_nr:%d",
								vesa_nr);
						break;
					}
				}
			}
		}
	}

	/* Read Standard Timings 9-16 and set sink_support*/
	for (index2 = 0; index2 <= 2; index2++) {
		edidp = edid_stdtim9_flag_offset[index2];

		/* Check for Standard Timing flag */
		if (memcmp(data + edidp, edid_stdtim9_flag,
				sizeof(edid_stdtim9_flag)) != 0)
			/* Flag mismatch, this is not Standard Timing 9-16 */
			continue;

		for (index = 0; index < EDID_BL1_STDTIM9_SIZE; index++) {
			edidp += EDID_BL1_STDTIM9_BYTE_START + index * 2;
			xres = (*(data + edidp) + 31) * 8;
			byte = *(data + edidp + 1);
			ar_index = (byte & EDID_STDTIM_AR_MASK) >>
						EDID_STDTIM_AR_SHIFT;
			yres = xres * edid_stdtim_ar[ar_index].y /
						edid_stdtim_ar[ar_index].x;
			freq = 60 + ((byte & EDID_STDTIM_FREQ_MASK) >>
						EDID_STDTIM_FREQ_SHIFT);
			LOGHDMILIB2("xres:%d yres:%d freq:%d", xres, yres,
									freq);
			get_vesaceanr_from_timing(xres, yres, freq, 0, &cea,
								&vesaceanr);
			LOGHDMILIB2("StdTim9to16 try vesa_nr:%d", vesaceanr);
			for (cnt = 0; cnt < nr_formats; cnt++) {
				LOGHDMILIB3("with:%d",
					formats[cnt].vesaceanr);
				if ((formats[cnt].cea == cea) &&
						(formats[cnt].vesaceanr ==
								vesaceanr)) {
					formats[cnt].sink_support = 1;
					LOGHDMILIB("StdTim9to16 %d cea %d"
							"vesaceanr:%d",
						index + 1, cea, vesaceanr);
					break;
				}
			}
		}
	}

	return RESULT_OK;
}

/* Get EDID message of specified block and send it on client socket */
int edidreq(__u8 block, __u32 cmd_id)
{
	int res = 0;
	int ret = 0;
	int edidsize = 0;
	int val;
	__u8 buf[512];
	__u8 ediddata[EDIDREAD_SIZE];

	LOGHDMILIB("%s begin", __func__);

	/* Request EDID */
	res = edid_read(block, ediddata);
	if (res == 0)
		edidsize = EDIDREAD_SIZE;

	val = HDMI_EDIDRESP;
	memcpy(&buf[CMD_OFFSET], &val, 4);
	memcpy(&buf[CMDID_OFFSET], &cmd_id, 4);
	val = edidsize + 1;
	memcpy(&buf[CMDLEN_OFFSET], &val, 4);
	buf[CMDBUF_OFFSET] = res;
	memcpy(&buf[CMDBUF_OFFSET + 1], ediddata, edidsize);

	/* Send on socket */
	ret = clientsocket_send(buf, CMDBUF_OFFSET + val);

	LOGHDMILIB("%s end", __func__);
	return ret;
}
