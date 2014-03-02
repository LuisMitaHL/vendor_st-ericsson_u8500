/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Display (Main, Sub, TV-OUT), Camera)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*#define NO_FB_TEST*/

#include <stdint.h>		/* uint32_t ... */
#include <stdio.h>		/* printf ... */
#include <linux/fb.h>		/* fb struct & var */
#include <fcntl.h>		/* O_RDWR */
#include <sys/mman.h>		/* mmap() ... */
#include <sys/stat.h>		/* mknod .. */
#include <unistd.h>		/* fork ... */
#include <string.h>		/* memcpy .. */
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>		/* malloc(), free() */
#include <pngrw.h>		/*read_png ... */
#include <signal.h>		/*kill ... */

#include <sys/ioctl.h>

typedef unsigned char __8;
typedef unsigned char u8;
typedef unsigned char bool;
#include <video/av8100.h>
#include <video/hdmi.h>
#include <hdmi_loc.h>

#include "VideoServices.h"
#include "VideoServices.hi"

/*  pix_fmt_map needs to be aligned with
 *  /kernel/drivers/video/mcde/mcde_fb.c */
struct pix_fmt_info pix_fmt_map[] = {
	{
	 /* RGB565 */
	 .bpp = 16,
	 .r = {.offset = 11, .length = 5},
	 .g = {.offset = 5, .length = 6},
	 .b = {.offset = 0, .length = 5},
	 },
	{
	 /* RGB888 */
	 .bpp = 24,
	 .r = {.offset = 16, .length = 8},
	 .g = {.offset = 8, .length = 8},
	 .b = {.offset = 0, .length = 8},
	 },
	{
	 /* RGBA8888 */
	 .bpp = 32,
	 .r = {.offset = 16, .length = 8},
	 .g = {.offset = 8, .length = 8},
	 .b = {.offset = 0, .length = 8},
	 .a = {.offset = 24, .length = 8},
	 },
	{
	 /* RGBX8888 */
	 .bpp = 32,
	 .r = {.offset = 16, .length = 8},
	 .g = {.offset = 8, .length = 8},
	 .b = {.offset = 0, .length = 8},
	 }
};

/*  video_mode_map needs to be aligned with
 *  /kernel/drivers/video/mcde/display_av8100.c */
static const struct video_mode video_mode_map[] = {
	{			/* 640_480_60_P */
	 .xres = 640, .yres = 480, .pixclock = 39682,
	 .hbp = 112, .hfp = 48, .vbp = 33, .vfp = 12,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* 720_480_60_P */
	 .xres = 720, .yres = 480, .pixclock = 37000,
	 .hbp = 104, .hfp = 34, .vbp = 30, .vfp = 15,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* 720_576_50_P */
	 .xres = 720, .yres = 576, .pixclock = 37037,
	 .hbp = 132, .hfp = 12, .vbp = 44, .vfp = 5,
	 .framerate = 50, .scanmode = 0,
	 },
	{			/* 1280_720_60_P */
	 .xres = 1280, .yres = 720, .pixclock = 13468,
	 .hbp = 256, .hfp = 114, .vbp = 20, .vfp = 10,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* 1280_720_50_P */
	 .xres = 1280, .yres = 720, .pixclock = 13468,
	 .hbp = 260, .hfp = 440, .vbp = 25, .vfp = 5,
	 .framerate = 50, .scanmode = 0,
	 },
	{			/* 1280_720_30_P */
	 .xres = 1280, .yres = 720, .pixclock = 13468,
	 .hbp = 260, .hfp = 1760, .vbp = 20, .vfp = 10,
	 .framerate = 30, .scanmode = 0,
	 },
	{			/* 1280_720_24_P */
	 .xres = 1280, .yres = 720, .pixclock = 16835,
	 .hbp = 260, .hfp = 1760, .vbp = 20, .vfp = 10,
	 .framerate = 24, .scanmode = 0,
	 },
	{			/* 1280_720_25_P */
	 .xres = 1280, .yres = 720, .pixclock = 13468,
	 .hbp = 260, .hfp = 2420, .vbp = 20, .vfp = 10,
	 .framerate = 25, .scanmode = 0,
	 },
	{			/* 1920_1080_30_P */
	 .xres = 1920, .yres = 1080, .pixclock = 13468,
	 .hbp = 189, .hfp = 91, .vbp = 36, .vfp = 9,
	 .framerate = 30, .scanmode = 0,
	 },
	{			/* 1920_1080_24_P */
	 .xres = 1920, .yres = 1080, .pixclock = 13468,
	 .hbp = 170, .hfp = 660, .vbp = 36, .vfp = 9,
	 .framerate = 24, .scanmode = 0,
	 },
	{			/* 1920_1080_25_P */
	 .xres = 1920, .yres = 1080, .pixclock = 13468,
	 .hbp = 192, .hfp = 528, .vbp = 36, .vfp = 9,
	 .framerate = 25, .scanmode = 0,
	 },
	{			/* 720_480_60_I */
	 .xres = 720, .yres = 480, .pixclock = 74074,
	 .hbp = 126, .hfp = 12, .vbp = 44, .vfp = 1,
	 .framerate = 60, .scanmode = 1,
	 },
	{			/* 720_576_50_I */
	 .xres = 720, .yres = 576, .pixclock = 74074,
	 .hbp = 132, .hfp = 12, .vbp = 44, .vfp = 5,
	 .framerate = 50, .scanmode = 1,
	 },
	{			/* 1920_1080_60_I */
	 .xres = 1920, .yres = 1080, .pixclock = 13468,
	 .hbp = 192, .hfp = 528, .vbp = 20, .vfp = 25,
	 .framerate = 60, .scanmode = 1,
	 },
	{			/* 1920_1080_50_I */
	 .xres = 1920, .yres = 1080, .pixclock = 13468,
	 .hbp = 192, .hfp = 88, .vbp = 20, .vfp = 25,
	 .framerate = 50, .scanmode = 1,
	 },
	{			/* VESA #9 800_600_60_P */
	 .xres = 800, .yres = 600, .pixclock = 25000,
	 .hbp = 168, .hfp = 88, .vbp = 23, .vfp = 5,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #14 848_480_60_P */
	 .xres = 848, .yres = 480, .pixclock = 29630,
	 .hbp = 128, .hfp = 112, .vbp = 23, .vfp = 14,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #16 1024_768_60_P */
	 .xres = 1024, .yres = 768, .pixclock = 15385,
	 .hbp = 160, .hfp = 160, .vbp = 29, .vfp = 9,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #22 1280_768_60_P */
	 .xres = 1280, .yres = 768, .pixclock = 14652,
	 .hbp = 80, .hfp = 80, .vbp = 12, .vfp = 10,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #23 1280_768_60_P */
	 .xres = 1280 , .yres = 768, .pixclock = 12579,
	 .hbp = 192, .hfp = 192, .vbp = 20, .vfp = 10,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #27 1280_800_60_P */
	 .xres = 1280, .yres = 800, .pixclock = 14085,
	 .hbp = 80, .hfp = 80, .vbp = 14, .vfp = 9,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #28 1280_800_60_P */
	 .xres = 1280, .yres = 800, .pixclock = 11976,
	 .hbp = 200, .hfp = 200, .vbp = 22, .vfp = 9,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #39 1360_768_60_P */
	 .xres = 1360, .yres = 768, .pixclock = 11696,
	 .hbp = 176, .hfp = 256, .vbp = 18, .vfp = 9,
	 .framerate = 60, .scanmode = 0,
	 },
	{			/* VESA #81 1366_768_60_P */
	 .xres = 1366, .yres = 768, .pixclock = 11662,
	 .hbp = 213, .hfp = 213, .vbp = 24, .vfp = 6,
	 .framerate = 60, .scanmode = 0,
	 },
};

static const struct timing_info timingtable[] = {
	{13468, 256, 114, 20, 10, 0, 0},	/* 1280_720_60_P */
	{39682, 112, 48, 33, 12, 0, 0},	/* 640_480_60_P */
	{37000, 104, 34, 30, 15, 0, 0},	/* 720_480_60_P */
	{37037, 132, 12, 44, 5, 0, 0},	/* 720_576_50_P */
	{13468, 260, 440, 25, 5, 0, 0},	/* 1280_720_50_P */
	{13468, 170, 660, 36, 9, 0, 0},	/* 1920_1080_24_P */
	{13468, 192, 528, 36, 9, 0, 0},	/* 1920_1080_25_P */
	{13468, 189, 91, 36, 9, 0, 0},	/* 1920_1080_30_P */
	{74074, 126, 12, 44, 1, 0, 0},	/* 720_480_60_I */
	{74074, 132, 12, 44, 5, 0, 0},	/* 720_576_50_I */
	{13468, 192, 528, 20, 25, 0, 0},	/* 1920_1080_50_I */
	{13468, 192, 88, 20, 25, 0, 0},	/* 1920_1080_60_I */
};

static const struct resolutions_info resolutionstable[] = {
	{1280, 720, 1280, 1440, 60, 0, &timingtable[0]},
	{640, 480, 640, 960, 60, 0, &timingtable[1]},
	{720, 480, 720, 960, 60, 0, &timingtable[2]},
	{720, 576, 720, 1152, 50, 0, &timingtable[3]},
	{1280, 720, 1280, 1440, 50, 0, &timingtable[4]},
	{1920, 1080, 1920, 2160, 24, 0, &timingtable[5]},
	{1920, 1080, 1920, 2160, 25, 0, &timingtable[6]},
	{1920, 1080, 1920, 2160, 30, 0, &timingtable[7]},
	{720, 480, 720, 960, 60, 1, &timingtable[8]},
	{720, 576, 720, 1152, 50, 1, &timingtable[9]},
	{1920, 1080, 1920, 2160, 50, 1, &timingtable[10]},
	{1920, 1080, 1920, 2160, 60, 1, &timingtable[11]},
};

/*
 * VidServ_DisplayInit - Service allowing to initialize a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplayInit(e_VidServ_DisplayId_t DisplayId)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	FILE *file_ptr;
	unsigned char command[200];
	uint8_t vl_hdmiswitchValue = 0;
	t_VidServ_DisplayResolution sl_displayProp = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	/* Check Display Id */
	switch (DisplayId) {

	case DISPLAY_MAIN:
	case DISPLAY_SUB:
	case DISPLAY_TV_OUT_CVBS_AB8500:
	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:

		/* Set Backlight to 100% when initialize display */
		if ((DisplayId == DISPLAY_MAIN) || (DisplayId == DISPLAY_SUB)) {
			if (VidServ_BacklightSetPWMValue(DisplayId, 255) !=
			    VIDSERV_NO_ERROR) {
				printf
				    ("VidServ_DisplayInit KO: Error during set default value of backlight operation\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}
		}

		/* Enable AV8100 framebuffer + dynamic selection of AV8100 Mode (HDMI or TV-OUT CVBS) */
		if ((DisplayId == DISPLAY_TV_OUT_CVBS_AV8100)
		    || (DisplayId == DISPLAY_TV_OUT_HDMI)) {
			/* find hdmi sys device to enable av8100 framebuffer (720p) using sys device */
			/* example: echo 010104 >  /sys/devices/av8100_hdmi.2/disponoff */
			snprintf((char *)command, 200,
				 "echo 010104 >  /sys/devices/`ls /sys/devices| grep -i hdmi`/disponoff");
			system((char *)command);

			if (DisplayId == DISPLAY_TV_OUT_CVBS_AV8100)
				vl_hdmiswitchValue = 1;
			else
				vl_hdmiswitchValue = 0;

			/* example: echo 0 >  /sys/devices/av8100_hdmi.2/hdmisdtvswitch */
			snprintf((char *)command, 200,
				 " echo %d >  /sys/devices/`ls /sys/devices| grep -i hdmi`/hdmisdtvswitch",
				 vl_hdmiswitchValue);
			system((char *)command);
		}

		/* If the node is already created, don't try to create it again */
		file_ptr = fopen(DisplayToDev[DisplayId], "r");
		if (file_ptr != NULL) {
			/* node is already created */
			fclose(file_ptr);
		} else {
			/* create node => mknod /dev/fbX c Major Minor */
			if (mknod
			    (DisplayToDev[DisplayId], S_IFCHR,
			     (VIDSERV_FB_MAJOR_NUMBER << 8) +
			     DisplayToMin[DisplayId]) < 0) {
				printf
				    ("VidServ_DisplayInit KO: Error during mknod operation\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}
		}

		/* Select a default resolution (720p for HDMI, 576i for CVBS) for AV8100 HDMI/CVBS
		 * so that we can display an image without doing a SetProperties */
		if ((DisplayId == DISPLAY_TV_OUT_CVBS_AV8100)
		    || (DisplayId == DISPLAY_TV_OUT_HDMI)) {
			if (DisplayId == DISPLAY_TV_OUT_CVBS_AV8100) {
				sl_displayProp.Width = 720;
				sl_displayProp.Height = 576;
				sl_displayProp.Frequency = 50;
				sl_displayProp.ScanMode = INTERLACED;
			} else {
				sl_displayProp.Width = 1280;
				sl_displayProp.Height = 720;
				sl_displayProp.Frequency = 60;
				sl_displayProp.ScanMode = PROGRESSIVE;
			}

			if (VidServ_DisplaySetProperties
			    (DisplayId, &sl_displayProp)) {
				printf
				    ("VidServ_DisplayInit KO: Error during AV8100 Set Resolution\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}
		}
		break;

	default:
		printf("VidServ_DisplayInit: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */

	}			/* switch(DisplayId) */

	return VIDSERV_NO_ERROR;

error:
	return vl_error;
}

/*
 * VidServ_DisplayDeInit - Service allowing to de-initialize a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplayDeInit(e_VidServ_DisplayId_t DisplayId)
{
	unsigned char command[200];

	/* Check Display Id */
	switch (DisplayId) {
	case DISPLAY_TV_OUT_HDMI:
	case DISPLAY_TV_OUT_CVBS_AV8100:
		/* Disable AV8100 dynamic framebuffer */
		/* find hdmi sys device to disable av8100 framebuffer using sys device */
		/* example: echo 000000 >  /sys/devices/av8100_hdmi.2/disponoff */
		snprintf((char *)command, 200,
			 " echo 000000 >  /sys/devices/`ls /sys/devices| grep -i hdmi`/disponoff");
		system((char *)command);
		break;

	case DISPLAY_MAIN:
	case DISPLAY_SUB:
	case DISPLAY_TV_OUT_CVBS_AB8500:
		/*TBC Nothing to do? */
		break;

	default:
		printf("VidServ_DisplayDeInit KO: Bad Display ID parameter\n");
		return VIDSERV_INVALID_PARAMETER;
		break;		/* default */
	}			/* switch(DisplayId) */

	return VIDSERV_NO_ERROR;
}

/*
 * VidServ_DisplaySetPowerMode - Service allowing to set the power mode of a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplaySetPowerMode(e_VidServ_DisplayId_t DisplayId,
				     e_VidServ_PowerMode_t PowerMode)
{
	uint32_t vl_fb_filedesc;
	uint32_t vl_error = VIDSERV_NO_ERROR;
	int32_t power;

	/* Check Display Id */
	switch (DisplayId) {
	case DISPLAY_MAIN:
	case DISPLAY_SUB:
	case DISPLAY_TV_OUT_CVBS_AB8500:
	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:
		/* 1- open FB device Access  */
		/* open an instance of framebuffer to get paramaters */
		if (VidServ_OpenFb
		    (DisplayToDev[DisplayId], &vl_fb_filedesc, NULL, NULL,
		     NULL) != 0) {
			printf
			    ("VidServ_DisplaySetProperties: Error: device opening KO\n");
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* 2 - set the power mode */
		switch (PowerMode) {
		case POWER_OFF:
			power = FB_BLANK_POWERDOWN;
			printf("VidServ_DisplaySetPowerMode FBIOBLANK %d\n", power);
			if (ioctl(vl_fb_filedesc, FBIOBLANK, power) < 0) {
				/* error during IOCTL execution */
				printf
				    ("VidServ_DisplaySetPowerMode: FBIOBLANK IOCTL KO\n");
				vl_error = VIDSERV_FAILED;
				close(vl_fb_filedesc);
				goto error;
			}
			break;

		case POWER_ON:
			power = FB_BLANK_UNBLANK;
			printf("VidServ_DisplaySetPowerMode FBIOBLANK %d\n", power);
			if (ioctl(vl_fb_filedesc, FBIOBLANK, power) < 0) {
				/* error during IOCTL execution */
				printf
				    ("VidServ_DisplaySetPowerMode: FBIOBLANK IOCTL KO\n");
				vl_error = VIDSERV_FAILED;
				close(vl_fb_filedesc);
				goto error;
			}
			break;

		case LOW_POWER:
			/* Not supported for the moment */
			printf
			    ("VidServ_DisplaySetPowerMode: Low Power not supported\n");
			vl_error = VIDSERV_NOT_AVAILABLE;
			goto out_free;
			break;

		default:
			vl_error = VIDSERV_INVALID_PARAMETER;
			goto out_free;
			break;
		}

		/*3- close FB device  Access */
		close(vl_fb_filedesc);
		break;

	default:
		printf
		    ("VidServ_DisplaySetPowerMode: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */

	}			/* switch(DisplayId) */

	return VIDSERV_NO_ERROR;

/* Manage Errors */
out_free:
	close(vl_fb_filedesc);
error:
	return vl_error;
}

/*
 * VidServ_DisplaySetProperties - Service allowing to set the properties of a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplaySetProperties(e_VidServ_DisplayId_t DisplayId,
				      t_VidServ_DisplayResolution *
				      DisplayResolution)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint32_t vl_fb_filedesc;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;

	/* Check Display Id */
	switch (DisplayId) {

		/* NA for MAIN and SUB */
	case DISPLAY_MAIN:
	case DISPLAY_SUB:
		printf
		    ("VidServ_DisplaySetProperties: Display ID Main and Sub not applicable\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;

	case DISPLAY_TV_OUT_CVBS_AB8500:
	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:
		{
			/*int32_t vl_av8100_filedesc;*/

			/* open an instance of framebuffer to get paramaters */
			if (VidServ_OpenFb
			    (DisplayToDev[DisplayId], &vl_fb_filedesc, &fb_fix, &fb_var,
			     NULL) != 0) {
				printf
				    ("VidServ_DisplaySetProperties: Error: device opening KO\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}

			/* Convert to fb parameters */
			if (VidServ_ConvertToFbVar
			    (DisplayResolution, &fb_var,
			     &pix_fmt_map[PIX_FMT_INFO_ENUM - 1]) !=
			    VIDSERV_NO_ERROR) {
				printf
				    ("VidServ_DisplaySetProperties: Error: Properties not supported\n");
				vl_error = VIDSERV_INVALID_PARAMETER;
				/*close(vl_av8100_filedesc);*/
				goto out_free;
			}

			/* Set DisplayResolution */
			if (ioctl(vl_fb_filedesc, FBIOPUT_VSCREENINFO, &fb_var)
			    < 0) {
				printf
				    ("Error setting mode using FBIOPUT_VSCREENINFO\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}

			/* close instance of frame buffer */
			VidServ_CloseFb(vl_fb_filedesc, NULL, 0);
		}
		break;

	default:
		printf
		    ("VidServ_DisplaySetProperties: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */

	}			/* switch(DisplayId) */
	return VIDSERV_NO_ERROR;

/* Manage Errors */
out_free:
	VidServ_CloseFb(vl_fb_filedesc, NULL, 0);
error:
	return vl_error;
}

/*
 * VidServ_DisplayGetProperties - Service allowing to get the properties of a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplayGetProperties(e_VidServ_DisplayId_t DisplayId,
				      t_VidServ_DisplayResolution *
				      DisplayResolution)
{
	uint32_t vl_fb_filedesc;
	struct fb_fix_screeninfo sl_fb_fixinfo;
	struct fb_var_screeninfo sl_fb_varinfo;
	uint32_t vl_error = VIDSERV_NO_ERROR;

	/* Check Display Id */
	switch (DisplayId) {

	case DISPLAY_MAIN:
	case DISPLAY_SUB:
	case DISPLAY_TV_OUT_CVBS_AB8500:
	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:
		/* open an instance of framebuffer to get paramaters */
		if (VidServ_OpenFb
		    (DisplayToDev[DisplayId], &vl_fb_filedesc, &sl_fb_fixinfo,
		     &sl_fb_varinfo, NULL) != 0) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		DisplayResolution->Width = sl_fb_varinfo.xres;
		DisplayResolution->Height = sl_fb_varinfo.yres;
		DisplayResolution->Depth = sl_fb_varinfo.bits_per_pixel;

		if ((DisplayId == DISPLAY_MAIN) || (DisplayId == DISPLAY_SUB)) {
			(*DisplayResolution).ScanMode = NONE;
		} else {
			/* Get ScanMode for TV-OUT */
			switch (sl_fb_varinfo.vmode & FB_VMODE_MASK) {
			case FB_VMODE_NONINTERLACED:
				(*DisplayResolution).ScanMode = PROGRESSIVE;
				break;
			case FB_VMODE_INTERLACED:
			case FB_VMODE_DOUBLE:
				DisplayResolution->ScanMode = INTERLACED;
				break;
			default:
				printf
				    ("VidServ_DisplayGetProperties KO: Unknown ScanMode for TV-OUT\n");
				vl_error = VIDSERV_FAILED;
				goto out_free;
				break;
			}
		}

		/* close instance of frame buffer */
		VidServ_CloseFb(vl_fb_filedesc, NULL, 0);
		break;

	default:
		printf("VidServ_DisplayDeInit: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */

	}			/* switch(DisplayId) */

	return VIDSERV_NO_ERROR;

/* Manage Errors */
out_free:
	VidServ_CloseFb(vl_fb_filedesc, NULL, 0);
error:
	return vl_error;
}

/*
 * VidServ_DisplayPicture - Service allowing to display a picture on a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplayPicture(e_VidServ_DisplayId_t DisplayId,
				t_VidServ_ImageProperties *ImageProperties,
				e_VidServ_LoopMode_t LoopMode,
				uint32_t ImageDuration,
				e_VidServ_Rotation_t Image1Rotation,
				t_VidServ_ImageProperties *Image2Properties,
				uint32_t Image2Duration,
				e_VidServ_Rotation_t Image2Rotation)
{
#ifdef NO_FB_TEST
	FILE *fp;
#endif
	uint32_t vl_fb_filedesc;
	struct fb_fix_screeninfo sl_fb_fixinfo;
	struct fb_var_screeninfo sl_fb_varinfo;
	uint8_t *pl_mmap_buffer = NULL;
	uint32_t vl_fb_mem_size;
	uint8_t *pl_bufferToWrite = NULL;
	uint32_t vl_nb_buffer = 1;
	uint32_t vl_error = VIDSERV_NO_ERROR;
	t_VidServ_LoopProperties sl_LoopProperties;

	/* Check Display Id */
	switch (DisplayId) {

	case DISPLAY_MAIN:
	case DISPLAY_SUB:
	case DISPLAY_TV_OUT_CVBS_AB8500:
	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:
		/* Check pointers of inputs parameters */
		if (ImageProperties == NULL) {
			vl_error = VIDSERV_INVALID_PARAMETER;
			goto error;
		}
		if (ImageProperties->BufferAddressOrFilePath == NULL) {
			vl_error = VIDSERV_INVALID_PARAMETER;
			goto error;
		}
		if ((LoopMode == TOGGLE_LOOP) && (Image2Properties == NULL)) {
			vl_error = VIDSERV_INVALID_PARAMETER;
			goto error;
		}

		/* open an instance of framebuffer and mmap the framebuffer memory */
		if (VidServ_OpenFb
		    (DisplayToDev[DisplayId], &vl_fb_filedesc, &sl_fb_fixinfo,
		     &sl_fb_varinfo, (uint32_t *) (&pl_mmap_buffer)) != 0) {
			vl_error = VIDSERV_FAILED;
			printf("VidServ_DisplayPicture: VidServ_OpenFb KO\n");
			goto error;
		}

		vl_fb_mem_size =
		    sl_fb_varinfo.xres * sl_fb_varinfo.yres *
		    sl_fb_varinfo.bits_per_pixel / 8;
		vl_nb_buffer = sl_fb_varinfo.yres_virtual/sl_fb_varinfo.yres;		    

		if (Image1Rotation != sl_fb_varinfo.rotate) {
			int switch_width_height = (Image1Rotation%FB_ROTATE_UD != sl_fb_varinfo.rotate%FB_ROTATE_UD);
			if(switch_width_height) {
				uint32_t old_xres = 0;
				uint32_t old_width = 0;

				old_xres = sl_fb_varinfo.xres;
				sl_fb_varinfo.xres = sl_fb_varinfo.yres;
				sl_fb_varinfo.yres = old_xres;
				sl_fb_varinfo.xres_virtual = sl_fb_varinfo.xres;
				sl_fb_varinfo.yres_virtual = vl_nb_buffer * sl_fb_varinfo.yres;
				old_width = sl_fb_varinfo.width;
				sl_fb_varinfo.yoffset = sl_fb_varinfo.yoffset/sl_fb_varinfo.xres*sl_fb_varinfo.yres;
			}
			sl_fb_varinfo.rotate = Image1Rotation;
			Image2Rotation = 0;

			printf("VidServ_DisplayPicture: FBIOPUT_VSCREENINFO\n");
			printf("xres=%d, yres=%d,\nxres_virtual=%d yres_virtual=%d, rotate=%d,\nxoffset=%d, yoffset=%d\n",
				sl_fb_varinfo.xres,
				sl_fb_varinfo.yres,
				sl_fb_varinfo.xres_virtual,
				sl_fb_varinfo.yres_virtual,
				sl_fb_varinfo.rotate,
				sl_fb_varinfo.xoffset,
				sl_fb_varinfo.yoffset);
			if (ioctl
			    (vl_fb_filedesc, FBIOPUT_VSCREENINFO,
			     &sl_fb_varinfo) < 0) {
				printf
				    ("VidServ_DisplayPicture: Error setting mode using FBIOPUT_VSCREENINFO\n");
				vl_error = VIDSERV_FAILED;
				goto error;
			}
		}
    
		/* If loop mode, data must be kept in an intermediate buffer to be displayed again => allocate memory */
		if (LoopMode != NO_LOOP) {
			sl_LoopProperties.DisplayId = DisplayId;
			sl_LoopProperties.BufferAddressImage1 =
			    (uint8_t *) malloc(vl_fb_mem_size);
			memset(sl_LoopProperties.BufferAddressImage1, 0x00,
			       vl_fb_mem_size);
			if (LoopMode == TOGGLE_LOOP) {
				sl_LoopProperties.BufferAddressImage2 =
				    (uint8_t *) malloc(vl_fb_mem_size);
				memset(sl_LoopProperties.BufferAddressImage2,
				       0x00, vl_fb_mem_size);
			}
			pl_bufferToWrite =
			    sl_LoopProperties.BufferAddressImage1;
		} else {	/* write directly to the framebuffer of the display */
			/* check we are in multibuffer mode before copy to "back buffer" */
			printf("yres_virtual=%d, sl_fb_varinfo.yres=%d, sl_fb_varinfo.yoffset=%d\n", sl_fb_varinfo.yres_virtual, sl_fb_varinfo.yres, sl_fb_varinfo.yoffset);
			if (vl_nb_buffer > 1) {
				printf("multibuffer mode\n");
				/* We are in multibuffer mode, we should write image to back buffer, front buffer is
				   locked  => write to yoffset=0 if front is yoffset=yres else write to yoffset=yres
				   yoffset=vl_fb_mem_size */
				if (sl_fb_varinfo.yoffset == (vl_nb_buffer-1)*sl_fb_varinfo.yres) {
					pl_bufferToWrite = pl_mmap_buffer;
					sl_fb_varinfo.yoffset = 0;
					sl_fb_varinfo.xoffset = 0;
				} else {
					sl_fb_varinfo.yoffset = sl_fb_varinfo.yoffset+sl_fb_varinfo.yres;
					sl_fb_varinfo.xoffset = 0;
					pl_bufferToWrite =
					    pl_mmap_buffer + (sl_fb_varinfo.yoffset/sl_fb_varinfo.yres)*vl_fb_mem_size;
				}
			} else {	/* single buffer */
				printf("singlebuffer mode\n");
				pl_bufferToWrite = pl_mmap_buffer;
 			}
		}

		/* Decode the image to a buffer */
		if (VidServ_PictureDecodeToBuffer
		    (ImageProperties, pl_bufferToWrite,
		     &sl_fb_varinfo) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			printf
			    ("VidServ_DisplayPicture: VidServ_PictureDecodeToBuffer Image 1 KO\n");
			goto out_free;
		}

		/* Decode the second image for toggle loop */
		if (LoopMode == TOGGLE_LOOP) {
			if (VidServ_PictureDecodeToBuffer
			    (Image2Properties,
			     sl_LoopProperties.BufferAddressImage2,
			     &sl_fb_varinfo) != VIDSERV_NO_ERROR) {
				vl_error = VIDSERV_FAILED;
				printf
				    ("VidServ_DisplayPicture: VidServ_PictureDecodeToBuffer Image 2 KO\n");
				goto out_free;
			}
		}
#ifdef NO_FB_TEST
		if (LoopMode == NO_LOOP) {
			fp = fopen("/tmp/rgb888.rgb", "wb");
			fwrite(pl_mmap_buffer, 1, vl_fb_mem_size, fp);
			fclose(fp);
		}
#endif
		printf("VidServ_DisplayPicture: FBIOPAN_DISPLAY\n");
		if (ioctl(vl_fb_filedesc, FBIOPAN_DISPLAY, &sl_fb_varinfo) < 0) {
			printf
			    ("VidServ_DisplayPicture: IOCTL  FBIOPAN_DISPLAY KO\n");
			vl_error = VIDSERV_FAILED;
			goto out_free;
		}

		/* close instance of frame buffer */
		VidServ_CloseFb(vl_fb_filedesc, pl_mmap_buffer,
			sl_fb_varinfo.xres_virtual * sl_fb_varinfo.yres_virtual * (sl_fb_varinfo.bits_per_pixel >> 3));
				
		/* If loop mode, call loop function */
		if (LoopMode != NO_LOOP) {
			/* Initialize data for loop */
			sl_LoopProperties.LoopMode = LoopMode;
			sl_LoopProperties.ImageDuration = ImageDuration;
			sl_LoopProperties.Image2Duration = Image2Duration;

			vl_error = VidServ_ExecuteDisplayLoop(&sl_LoopProperties);
			if (vl_error != 0)
				goto error;
		}
		goto everything_is_ok;
		break;

	default:		/* switch(DisplayId) */
		printf("VidServ_DisplayPicture: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */

	}			/* switch(DisplayId) */

/* Manage Errors */
out_free:
	VidServ_CloseFb(vl_fb_filedesc, pl_mmap_buffer, 
		sl_fb_varinfo.xres_virtual * sl_fb_varinfo.yres_virtual * (sl_fb_varinfo.bits_per_pixel >> 3));
	/* Free Memory allocated for loop */
	if (LoopMode != NO_LOOP) {
		free(sl_LoopProperties.BufferAddressImage1);
		if (LoopMode == TOGGLE_LOOP) {
			free(sl_LoopProperties.BufferAddressImage2);
		}
	}
error:
	return vl_error;

everything_is_ok:
	return VIDSERV_NO_ERROR;
}

/*
 * VidServ_StopDisplayPictureLoop - Service allowing to stop a image display loop
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_StopDisplayPictureLoop(e_VidServ_DisplayId_t DisplayId)
{
	char al_TmpFile[30];
	FILE *pl_tmp_fd;
	pid_t vl_ChildPid;

	snprintf(al_TmpFile, 22, "/tmp/tmp.displayLoop%d\n", DisplayId);

	/* Retrieve the PID of the thread started for display loop is saved in a temporary file  /tmp/tmp.VideoServPID */
	pl_tmp_fd = fopen(al_TmpFile, "r");
	if (pl_tmp_fd != NULL) {
		fread(&vl_ChildPid, 1, sizeof(pid_t), pl_tmp_fd);
		fclose(pl_tmp_fd);
	} else {
		return VIDSERV_FAILED;
	}

	/* Erase the temporary file */
	remove(al_TmpFile);

	/* Kill the thread started for display loop */
	/*snprintf(command, 255, "kill -2 %d",vl_ChildPid);    */
	/*system(command); */
	kill(vl_ChildPid, SIGTERM);

	return VIDSERV_NO_ERROR;
}

/*
 * VidServ_GetAlsValue - Service allowing to get ALS value
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_GetAlsValue(uint32_t *AlsValue)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint8_t command[200];
	FILE *file_ptr;
	char als_value_string[10];

	/*  Check parameter */
	if (AlsValue == NULL) {
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
	}

	/* Power ON ALS */
	snprintf((char *)command, 200,
		 "echo 3 > /sys/class/i2c-dev/i2c-2/device/2-0029/power_state");
	printf("%s\n", (char *)command);
	system((char *)command);

	/* Wait around 500ms to be sure that 1 acquisition is done, from bh1780gli spec, device will take a new illumination reading every 250msec */
	usleep(500000);

	/* Get ALS value */
	snprintf((char *)command, 200,
		 "cat /sys/class/i2c-dev/i2c-2/device/2-0029/lux > /tmp/lux.txt");
	printf("%s\n", (char *)command);
	system((char *)command);

	file_ptr = fopen("/tmp/lux.txt", "r");
	if (file_ptr == NULL) {
		vl_error = VIDSERV_FAILED;
		goto error;
	} else {
		fgets(als_value_string, 10, file_ptr);
		*AlsValue = (uint8_t) strtol(als_value_string, NULL, 10);
		printf("ALS value=%d\n", *AlsValue);
		fclose(file_ptr);
	}
	/* Clear the output file */
	system("rm -f /tmp/lux.txt");

	/* Power OFF ALS */
	snprintf((char *)command, 200,
		 "echo 0 > /sys/class/i2c-dev/i2c-2/device/2-0029/power_state");
	printf("%s\n", (char *)command);
	system((char *)command);

	goto no_error;

error:
	return vl_error;
no_error:
	return VIDSERV_NO_ERROR;

}

/*
 * VidServ_BacklightGetPWMValue - Service allowing to get PWM backlight value of a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_BacklightGetPWMValue(e_VidServ_DisplayId_t DisplayId,
				      uint32_t *PwmValue)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint8_t command[200];

	/*  Check parameter */
	if (PwmValue == NULL) {
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
	}

	/* Check Display Id */
	switch (DisplayId) {
	case DISPLAY_MAIN:
		snprintf((char *)command, 200,
			 "cat /sys/devices/platform/leds_pwm.0/leds/lcd-backlight/brightness");
		printf("%s\n", (char *)command);
		system((char *)command);
		goto no_error;
		break;		/* DISPLAY_MAIN */
	case DISPLAY_SUB:
		snprintf((char *)command, 200,
			 "cat /sys/devices/platform/leds_pwm.0/leds/sec-lcd-backlight/brightness");
		printf("%s\n", (char *)command);
		system((char *)command);
		goto no_error;
		break;		/* DISPLAY_SUB */
	default:
		printf
		    ("VidServ_BacklightGetPWMValue: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */
	}			/* switch(DisplayId) */

error:
	return vl_error;
no_error:
	return VIDSERV_NO_ERROR;
}

/*
 * VidServ_BacklightSetPWMValue - Service allowing to set PWM backlight value of a display
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_BacklightSetPWMValue(e_VidServ_DisplayId_t DisplayId,
				      uint32_t PwmValue)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint8_t command[200];

	/*  Check parameter:  < max_brightness */
	if (PwmValue > 255) {
		printf("\nValue cannot be > max_brightness (255).\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
	}

	/* Check Display Id */
	switch (DisplayId) {
	case DISPLAY_MAIN:
		snprintf((char *)command, 200,
			 "echo %d > /sys/devices/platform/leds_pwm.0/leds/lcd-backlight/brightness",
			 PwmValue);
		system((char *)command);
		goto no_error;
		break;		/* DISPLAY_MAIN */
	case DISPLAY_SUB:
		snprintf((char *)command, 200,
			 "echo %d > /sys/devices/platform/leds_pwm.0/leds/sec-lcd-backlight/brightness",
			 PwmValue);
		system((char *)command);
		goto no_error;
		break;		/* DISPLAY_SUB */
	default:
		printf
		    ("VidServ_BacklightGetPWMValue: Bad Display ID parameter\n");
		vl_error = VIDSERV_INVALID_PARAMETER;
		goto error;
		break;		/* default */
	}			/* switch(DisplayId) */

error:
	return vl_error;
no_error:
	return VIDSERV_NO_ERROR;

}

/*
 * VidServ_DisplayIsPlugged - Service allowing to know if a plug is detected on TV-OUT output
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_DisplayIsPlugged(e_VidServ_DisplayId_t DisplayId,
				  uint32_t *IsPlugged)
{
	int32_t vl_hdmi_filedesc;
	uint32_t vl_error = VIDSERV_NO_ERROR;
	struct av8100_status vl_av8100_status;
	uint8_t RegisterData = 0;
	/* Check Display Id */
	switch (DisplayId) {
	case DISPLAY_TV_OUT_CVBS_AB8500:
		/* Enable plug/unplug detection in TVoutCtrl Register each 0.5 second */
		RegisterData = RegisterData | VIDSERV_DENC_TVOUTCTRL_ENABLE;

		if (VidServ_GetSetAB8500Value
		    (VIDSERV_DENC_TVOUTCTRL_BANK, VIDSERV_DENC_TVOUTCTRL_OFFSET,
		     &RegisterData,
		     AB8500_REGISTER_WRITE) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* Wait for detection, detection is configured to be done each
		 * 0.5 second */
		sleep(1);

		/* Get the value of PlugTVdet in ITSource1 Register  */
		if (VidServ_GetSetAB8500Value
		    (VIDSERV_INTERRUPT_SOURCE1_BANK,
		     VIDSERV_INTERRUPT_SOURCE1_OFFSET, &RegisterData,
		     AB8500_REGISTER_READ) != VIDSERV_NO_ERROR) {
			goto error;
		}
		if (((RegisterData & 0x04) >> 2) == 1) {
			*IsPlugged = 1;
		} else {
			*IsPlugged = 0;
		}
		return VIDSERV_NO_ERROR;
		break;

	case DISPLAY_TV_OUT_CVBS_AV8100:
	case DISPLAY_TV_OUT_HDMI:
		/* 1- open HDMI device Access  */
		vl_hdmi_filedesc = open(VIDSERV_HDMI_DEVICE_NAME, O_RDWR);
		if (vl_hdmi_filedesc < 0) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* 2- retrieve the AV8100 component status */
		if (ioctl
		    (vl_hdmi_filedesc, IOC_HDMI_STATUS_GET,
		     &vl_av8100_status) < 0) {
			/* error during IOCTL execution */
			vl_error = VIDSERV_FAILED;
			close(vl_hdmi_filedesc);
			goto error;
		} else {
			/* CVBS plug status */
			if (DisplayId == DISPLAY_TV_OUT_CVBS_AV8100) {
				if (vl_av8100_status.av8100_plugin_status ==
				    AV8100_CVBS_PLUGIN) {
					*IsPlugged = 1;
				} else {
					*IsPlugged = 0;
				}
			}
			/* HDMI plug status */
			else {
				if (vl_av8100_status.av8100_plugin_status ==
				    AV8100_HDMI_PLUGIN) {
					*IsPlugged = 1;
				} else {
					*IsPlugged = 0;
				}
			}
		}

		/*3- close HDMI device  Access */
		close(vl_hdmi_filedesc);
		break;

	default:
		printf("VidServ_DisplayIsPlugged: Bad Display ID parameter\n");
		return VIDSERV_INVALID_PARAMETER;
		break;		/* default */

	}			/* switch(DisplayId) */

	return VIDSERV_NO_ERROR;

/* Manage Errors */
error:
	return vl_error;
}

/*
 * VidServ_AVConnectorSourceSelection - Service allowing to select the source to connect to AudioVideo connector
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t VidServ_AVConnectorSourceSelection(e_VidServ_AVConnectorSource_t
					    SourceId)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint8_t vl_AB8500Version;
	uint8_t GpioRegisterData = 0;

	/* Select only if AB8500 version is at least V1 (service not available on AB8500 ED) */
	if (VidServ_GetAB8500Version(&vl_AB8500Version) != VIDSERV_NO_ERROR) {
		printf
		    ("VidServ_AVConnectorSourceSelection: AB8500 get version Error\n");
		vl_error = VIDSERV_FAILED;
		goto error;
	}

	/* If ED, Nothing to Do */
	if (vl_AB8500Version == 0) {
		printf
		    ("VidServ_AVConnectorSourceSelection: Nothing done as AV Connector is not managed with AB8500 ED\n");
		return VIDSERV_NO_ERROR;
	}

	/* Check Display Id */
	switch (SourceId) {
	case AUDIO_SOURCE:
	case VIDEO_SOURCE:

		/* 2- Select GPIO Direction as Output */

		/* Get the current value */
		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_DIR_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_READ) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* Set GPIO_VIDEOCTRL bit at output direction => 1 */
		GpioRegisterData =
		    GpioRegisterData | (1 <<
					VIDSERV_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER);

		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_DIR_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_WRITE) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* 3- Select GPIO Output value */

		/* Get the current value */
		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_OUT_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_READ) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* Set GPIO_VIDEOCTRL at output value, 1 for Audio, 0 for Video */
		if (SourceId == AUDIO_SOURCE)
			GpioRegisterData =
			    GpioRegisterData | (1 <<
						VIDSERV_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER);
		else
			GpioRegisterData =
			    GpioRegisterData & (0xFF -
						(1 <<
						 VIDSERV_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER));

		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_OUT_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_WRITE) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* 4- Select GPIO Pull Down inactive value */

		/* Get the current value */
		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_PUD_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_READ) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}

		/* Set GPIO_VIDEOCTRL at pull down inative => 1 */
		GpioRegisterData =
		    GpioRegisterData | (1 <<
					VIDSERV_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER);

		if (VidServ_GetSetAB8500Value
		    (VIDSERV_MISC_BANK, VIDSERV_GPIO_VIDEOCTRL_PUD_REG_OFFSET,
		     &GpioRegisterData,
		     AB8500_REGISTER_WRITE) != VIDSERV_NO_ERROR) {
			vl_error = VIDSERV_FAILED;
			goto error;
		}
		/* 5- Everything OK, return with no error and close the AB8500 device access */
		else {
			goto everything_is_ok;
		}

		break;

	default:
		printf
		    ("VidServ_AVConnectorSourceSelection: Bad Source ID parameter\n");
		return VIDSERV_INVALID_PARAMETER;
		break;		/* default */
	}			/* switch(DisplayId) */

error:
	return vl_error;

everything_is_ok:
	return VIDSERV_NO_ERROR;

}

/*
 * VidServ_DisplayAV8100InputVideoFormat - Service allowing to change the input
 * video format of the AV8100 chip (data transferred on DSI Link 2)
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */

uint32_t
VidServ_DisplayAV8100InputVideoFormat(e_VidServ_DisplayAV8100InputVideoFormat_t
				      AV8100InputVideoFormat)
{
	uint8_t command[200];
	uint32_t vl_InputVideoFormat = 0;

	/* Disable AV8100 dynamic framebuffer */
	/* find hdmi sys device to change av8100 input pixel format */
	/* example: echo 64 > /sys/devices/av8100_hdmi.2/input_pixel_format */
	switch (AV8100InputVideoFormat) {
	case VID_AV8100_INPUT_PIX_RGB565:
		vl_InputVideoFormat = VIDSERV_PORTPIXFMT_DSI_16BPP;
		break;
	case VID_AV8100_INPUT_PIX_RGB666:
		vl_InputVideoFormat = VIDSERV_PORTPIXFMT_DSI_18BPP;
		break;
	case VID_AV8100_INPUT_PIX_RGB666P:
		vl_InputVideoFormat = VIDSERV_PORTPIXFMT_DSI_18BPP_PACKED;
		break;
	case VID_AV8100_INPUT_PIX_RGB888:
		vl_InputVideoFormat = VIDSERV_PORTPIXFMT_DSI_24BPP;
		break;
	case VID_AV8100_INPUT_PIX_YCBCR422:
		vl_InputVideoFormat = VIDSERV_PORTPIXFMT_DSI_YCBCR422;
		break;
	default:
		return VIDSERV_INVALID_PARAMETER;
		break;
	}
	snprintf((char *)command, 200,
		 " echo %d >  /sys/devices/`ls /sys/devices| grep -i hdmi`/input_pixel_format",
		 vl_InputVideoFormat);
	system((char *)command);

	/* Need to de-activate/re-activate HDMI framebuffer for YUV settings to
	 * take into account */
	if (VidServ_DisplayDeInit(DISPLAY_TV_OUT_HDMI)
	    || VidServ_DisplayInit(DISPLAY_TV_OUT_HDMI))
		return VIDSERV_FAILED;

	return VIDSERV_NO_ERROR;
}

/******************************************************************************************************/
/* 								INTERNAL used functions 	*/
/******************************************************************************************************/

 /*
  * VidServ_OpenFb - Initialise the framebuffer, getting fix and variable information
  *
  * @param devname -  Framebuffer filename ("/dev/fb1" for instance)
  * @param fb_filedesc - Framebuffer cescriptor
  * @param fb_varinfo - Framebuffer variable information => no FBIOGET_VSCREENINFO if NULL  (for some functions, it is not need to get vscreen infos)
  * @param mmap_buffer - pointer to Framebuffer memory mmap => no mmap if NULL (for some functions, it is not need to mmap)
  *
  * */
int32_t VidServ_OpenFb(char *devname, uint32_t *fb_filedesc,
		       struct fb_fix_screeninfo *fb_fixinfo,
		       struct fb_var_screeninfo *fb_varinfo,
		       uint32_t *p_mmap_buffer)
{
	int32_t vl_filedesc = 0;
#ifdef NO_FB_TEST
	if (p_mmap_buffer != NULL) {
		*p_mmap_buffer = (int)malloc(480 * 360 * 3);
	}

	if (fb_varinfo != NULL) {
		fb_varinfo->xres = 480;
		fb_varinfo->yres = 360;
		fb_varinfo->bits_per_pixel = 24;
		fb_varinfo->vmode = FB_VMODE_INTERLACED;
	}
#else
	vl_filedesc = open(devname, O_RDWR);
	if (vl_filedesc < 0) {
		goto error;
	} else {
		*fb_filedesc = vl_filedesc;
	}

	if (fb_varinfo != NULL) {
		if (ioctl(*fb_filedesc, FBIOGET_VSCREENINFO, fb_varinfo) < 0) {
			goto out_free;
		}
	}
	if (fb_fixinfo != NULL) {
		if (ioctl(*fb_filedesc, FBIOGET_FSCREENINFO, fb_fixinfo) < 0) {
			goto out_free;
		}
	}

	if (p_mmap_buffer != NULL) {
		/* fb_varinfo is needed before mmap */
		if (fb_varinfo == NULL)
			goto out_free;

		*p_mmap_buffer =
		    (int)mmap(NULL,
			      fb_varinfo->xres_virtual * fb_varinfo->yres_virtual * (fb_varinfo->bits_per_pixel >> 3),
			      PROT_READ | PROT_WRITE, MAP_SHARED, *fb_filedesc,
			      0);

		if (!(*p_mmap_buffer)) {
			goto out_free;
		}
	}
#endif

	return VIDSERV_NO_ERROR;

out_free:
	/* close frame buffer access */
	close(*fb_filedesc);
error:
	return VIDSERV_FAILED;
}

 /*
  * VidServ_CloseFb - De-initialise the framebuffer
  *
  * @param fb_filedesc - Framebuffer cescriptor
  * @param mmap_buffer - Framebuffer memory mmap => no munmap if NULL
  * @param pp_fb_mem_size - Framebuffer memory mmap size
  *
  * */
void VidServ_CloseFb(uint32_t fb_filedesc, uint8_t *mmap_buffer,
		     uint32_t pp_fb_mem_size)
{
	printf("VidServ_CloseFb\n");
#ifdef NO_FB_TEST
	if (mmap_buffer != NULL) {
		free(mmap_buffer);
	}
#else
	if (mmap_buffer != NULL) {
		munmap(mmap_buffer, pp_fb_mem_size);
	}
	close(fb_filedesc);
#endif
}

/*
 * read_rgb - read a rgb file to an output rgb buffer (convertion from RGB888 to RGB565 is supported if output buffer is 16BPP). No resize is managed
 * 
 * @from_file:
 * @input_filename: path of the file where image is provided
 * @output: buffer/frame where converted image is written
 * @output_width: width of the output image
 * @output_height: height of the output image
 * @output_bpp: number of bits per pixel of the output images
 * @return: VIDSERV_NO_ERROR if ok.
 *
 * */
int32_t read_rgb(uint32_t from_file,
		 uint8_t *in_buffer,
		 uint32_t in_width, uint32_t in_height,
		 uint32_t in_bpp, 
		 uint8_t *out_buffer,
		 uint32_t out_width, uint32_t out_height,
		 uint32_t out_bpp)
{
	FILE *fp;
	uint32_t in_buffersize = in_width * in_height * (in_bpp>>3);
	uint8_t *tmp_buffer = NULL;
	uint32_t read_index = 0;
	uint32_t write_index = 0;
	uint32_t line_size = out_width*(out_bpp>>3);
	int32_t remaining_cols = out_width - in_width;
	int32_t remaining_lines = out_height - in_height;
	uint32_t out_buffersize = out_width*out_height*(out_bpp>>3);

	tmp_buffer = (uint8_t *) malloc(in_buffersize*sizeof(uint8_t));
	if(tmp_buffer == NULL) {
		printf("read_rgb KO\n");
		goto error;
	}
	if(from_file) {
		/* open rgb file */
		fp = fopen((char*)in_buffer, "rb");
		if (fp == NULL) {
			printf("Could not open the file %s for reading the rgb image\n",
			      (char*)in_buffer);
			goto error;
		}
		if (fread(tmp_buffer, 1, in_buffersize, fp) < in_buffersize) {
			printf
			    ("Error : not enough data in RGB file for specified size %dx%d\n",
			     in_width, in_height);
			goto out_close;
		}
		in_buffer = tmp_buffer;
		fclose(fp);
	}

	/* Check if convertion is needed */
	VidServ_Convert(in_buffer, tmp_buffer,
		in_bpp, out_bpp, 
		in_width, 
		in_height);
		
	while (write_index < out_buffersize && 
		read_index < in_buffersize) {
		if(remaining_cols > 0) {
			memcpy(out_buffer+write_index, 
				tmp_buffer+read_index, 
				in_width*(in_bpp>>3));
			memset(out_buffer+write_index+in_width*(in_bpp>>3), 
				0x00, 
				remaining_cols*(out_bpp>>3));
		}
		else memcpy(out_buffer+write_index, 
			tmp_buffer+read_index, 
			line_size);
		write_index = write_index + out_width*(out_bpp>>3);
		read_index = read_index + in_width*(in_bpp>>3);
	}
	if(remaining_lines > 0) {
		memset(out_buffer+write_index, 
			0x00, 
			remaining_lines*out_width*(out_bpp>>3));
	}
	printf("sortie read_rgb\n");
	/* Everything is ok */
	return VIDSERV_NO_ERROR;

	/* Error managements */
out_close:
	/* close the file */
	fclose(fp);
error:
	/* go out with an error value */
	return VIDSERV_FAILED;
}

/*
 * VidServ_PictureDecodeToBuffer - Decode image to a buffer
 *
 * @ImageProperties: Properties of the image to decode
 * @bufferToWrite: buffer where the decoded image is stored
 * @sl_fb_varinfo: properties of the frame buffer device
 *
 * */
int32_t VidServ_PictureDecodeToBuffer(t_VidServ_ImageProperties *ImageProperties,
				      uint8_t *bufferToWrite,
				      struct fb_var_screeninfo *pl_fb_varinfo)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint8_t in_bpp = 32;
	uint32_t framebuffer_size = (*pl_fb_varinfo).xres*(*pl_fb_varinfo).yres*((*pl_fb_varinfo).bits_per_pixel>>3);

	printf("PictureDecodeToBuffer in_format=%d in_width=%d in_height=%d in_buffersize=%d "\
		"out_bpp=%d out_width=%d out_height=%d out_buffersize=%d\n",
		ImageProperties->InputPictureFormat,
		ImageProperties->InputPictureResolution.Width,
		ImageProperties->InputPictureResolution.Height,
		ImageProperties->BufferSize,
		(*pl_fb_varinfo).bits_per_pixel,
		(*pl_fb_varinfo).xres,
		(*pl_fb_varinfo).yres,
		framebuffer_size);

	switch(ImageProperties->InputPictureFormat) {
		case ARGB8888:
			in_bpp = 32;
			break;
		case RGB888:
			in_bpp = 24;
			break;
		case RGB565:
			in_bpp = 16;
			break;
		default:
			in_bpp = 32;
	}

	switch (ImageProperties->InputPictureFormat) {
		case PNG:
			/* Write PNG to buffer */
#ifndef EXCLUDE_PNG
			/* Picture should be read from a file */
			if (ImageProperties->FromFile) {
				memset(bufferToWrite, 0, framebuffer_size);
				if (read_png
				    ((char *)ImageProperties->BufferAddressOrFilePath,
				    bufferToWrite, (*pl_fb_varinfo).xres,
				    (*pl_fb_varinfo).yres,
				    (*pl_fb_varinfo).bits_per_pixel) != 0) {
					vl_error = VIDSERV_FAILED;
				}
			}
#endif
			break;
		case ARGB8888:
		case RGB888:
		case RGB565:
			if (read_rgb(ImageProperties->FromFile, 
				ImageProperties->BufferAddressOrFilePath,
				ImageProperties->InputPictureResolution.Width,
				ImageProperties->InputPictureResolution.Height,
				in_bpp,
				bufferToWrite, 
				(*pl_fb_varinfo).xres,
				(*pl_fb_varinfo).yres,
				(*pl_fb_varinfo).bits_per_pixel) != 0) {
				vl_error = VIDSERV_FAILED;
			}
			break;
		default:
			printf
			    ("VidServ_DisplayPicture: Picture Format not supported\n");
			vl_error = VIDSERV_NOT_AVAILABLE;
			break;
	}		/* switch(ImageProperties->InputPictureFormat) */
	return vl_error;
}

/*
 * VidServ_ExecuteDisplayLoop - Execute a display in loop
 * - call a specific application to execute infinite loop
 *
 * @LoopProperties: properties of the images and loop
 * @return: VIDSERV_NO_ERROR if ok.
 *
 * */
int32_t VidServ_ExecuteDisplayLoop(t_VidServ_LoopProperties *LoopProperties)
{
	char al_TmpFile[255];
	FILE *pl_tmp_fd = NULL;
	uint32_t vl_error = VIDSERV_NO_ERROR;
	uint32_t vl_nb_buffer = 0;
	uint8_t *pl_mmap_buffer = NULL;
	uint32_t vl_fb_filedesc;
	struct fb_fix_screeninfo sl_fb_fixinfo;
	struct fb_var_screeninfo sl_fb_varinfo;
	uint32_t vl_fb_mem_size;

	snprintf(al_TmpFile, 22, "/tmp/tmp.displayLoop%d",
		 LoopProperties->DisplayId);

	/* if temporary PID loop file already exist */
	/* => a loop is already on going, return error */
	pl_tmp_fd = fopen(al_TmpFile, "r");
	if (pl_tmp_fd != NULL) {
		printf("Error: a display picture loop is already launched, \
			please stop previous display loop \
			before launching a new one\n");
		fclose(pl_tmp_fd);
		vl_error = VIDSERV_DEVICE_ALREADY_USED;
		goto error;
	}

    /** Prepare display buffer with image(s) content **/

	/* open an instance of framebuffer and mmap the framebuffer memory */
	if (VidServ_OpenFb
	    (DisplayToDev[LoopProperties->DisplayId], &vl_fb_filedesc, &sl_fb_fixinfo,
	     &sl_fb_varinfo, (uint32_t *) (&pl_mmap_buffer)) != 0) {
		vl_error = VIDSERV_FAILED;
		printf("VidServ_ExecuteDisplayLoop: VidServ_OpenFb KO\n");
		goto error;
	}

	/* copy image(s) data, use multibuffer functionnality */

	vl_fb_mem_size =
		sl_fb_varinfo.xres * sl_fb_varinfo.bits_per_pixel / 8 *
		sl_fb_varinfo.yres;
	sl_fb_varinfo.xoffset = 0;
	sl_fb_varinfo.yoffset = 0;
	vl_nb_buffer = (sl_fb_varinfo.yres_virtual/sl_fb_varinfo.yres);

	memcpy(pl_mmap_buffer, LoopProperties->BufferAddressImage1,
	       vl_fb_mem_size);

	/* check we are in multibuffer before copy to "second buffer" */
	if (vl_nb_buffer > 1) {
		if (LoopProperties->LoopMode == TOGGLE_LOOP) {
			memcpy((pl_mmap_buffer + vl_fb_mem_size),
			       LoopProperties->BufferAddressImage2,
			       vl_fb_mem_size);
		} else {	/* FIXED_RATE or MAX_SPEED */
			memcpy((pl_mmap_buffer + vl_fb_mem_size),
			       LoopProperties->BufferAddressImage1,
			       vl_fb_mem_size);
		}
	} else {
		vl_error = VIDSERV_FAILED;
		printf("VidServ_ExecuteDisplayLoop: \
			framebuffer size KO for loop, \
			multibuffer not activated\n");
		goto out_free;
	}


	/* Free Memory allocated for loop */
	free(LoopProperties->BufferAddressImage1);
	if (LoopProperties->LoopMode == TOGGLE_LOOP)
		free(LoopProperties->BufferAddressImage2);

	VidServ_CloseFb(vl_fb_filedesc, pl_mmap_buffer, 
		sl_fb_varinfo.xres_virtual * sl_fb_varinfo.yres_virtual * (sl_fb_varinfo.bits_per_pixel >> 3));

    /** Call a specific application to execute infinite loop **/
	snprintf(al_TmpFile, 255, "ExecuteDisplayLoop %d %d %d %d",
		 LoopProperties->DisplayId, LoopProperties->LoopMode,
		 LoopProperties->ImageDuration, LoopProperties->Image2Duration);

	system(al_TmpFile);

	return vl_error;

out_free:
	VidServ_CloseFb(vl_fb_filedesc, pl_mmap_buffer, 
		sl_fb_varinfo.xres_virtual * sl_fb_varinfo.yres_virtual * (sl_fb_varinfo.bits_per_pixel >> 3));

error:
	/* Free Memory allocated for loop */
	if (LoopProperties->LoopMode != NO_LOOP) {
		free(LoopProperties->BufferAddressImage1);
		if (LoopProperties->LoopMode == TOGGLE_LOOP)
			free(LoopProperties->BufferAddressImage2);
	}
	return vl_error;

}

/*
 * VidServ_Convert - Convert a buffer to a buffer
 *
 * @inputBuffer -
 * @outputBuffer -
 * @in_bpp -
 * @out_bpp -
 * @input_width -
 * @input_height -
 * @return: void
 *
 * */
void VidServ_Convert(uint8_t *inputBuffer,
			uint8_t *outputBuffer,
			uint8_t in_bpp, uint8_t out_bpp,
			uint32_t input_width, uint32_t input_height)
{
	printf("VidServ_Convert in_bpp=%d, out_bpp=%d, input_width=%d, input_height=%d\n", in_bpp, out_bpp, input_width, input_height);
	
	if(in_bpp == out_bpp) {
		if(outputBuffer == NULL) printf("outputBuffer null\n");
		if(inputBuffer == NULL) printf("inputBuffer null\n");
		memcpy(outputBuffer, inputBuffer, input_width*input_height*(in_bpp>>3));
	}
	else {
		
		if(in_bpp == 32) {
			if(out_bpp == 24) {
				VidServ_ConvertARGB8888TO888
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}
			if(out_bpp == 16) {
				  VidServ_ConvertARGB8888TO565
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}
		}
		if(in_bpp == 24) {
			if(out_bpp == 32) {
				VidServ_ConvertRGB888TO8888
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}
			if(out_bpp == 16) {
				  VidServ_ConvertRGB888TO565
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}			  
		}
		if(in_bpp == 16) {
			if(out_bpp == 32) {
				VidServ_ConvertRGB565TO8888
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}
			if(out_bpp == 24) {
				  VidServ_ConvertRGB565TO888
				    (inputBuffer,
				      outputBuffer,
				      input_width,
				      input_height);
			}
		}
	}
}


void VidServ_ConvertRGB888TO8888(uint8_t __attribute__ ((unused)) *inputRGB888Buffer,
				  uint8_t __attribute__ ((unused)) *outputARGB8888Buffer,
				  uint32_t __attribute__ ((unused)) output_width, uint32_t __attribute__ ((unused)) output_height) {
}
void VidServ_ConvertRGB565TO8888(uint8_t __attribute__ ((unused)) *inputRGB565Buffer,
				  uint8_t __attribute__ ((unused)) *outputARGB8888Buffer,
				  uint32_t __attribute__ ((unused)) output_width, uint32_t __attribute__ ((unused)) output_height) {
}
void VidServ_ConvertRGB565TO888(uint8_t __attribute__ ((unused)) *inputRGB565Buffer,
				  uint8_t __attribute__ ((unused)) *outputRGB888Buffer,
				  uint32_t __attribute__ ((unused)) output_width, uint32_t __attribute__ ((unused)) output_height) {
}

/*
 * VidServ_ConvertARGB888TO888 - Convert a ARGB8888 buffer to a RGB888 buffer
 *
 * @inputARGB888Buffer -
 * @outputRGB888Buffer -
 * @output_width -
 * @output_height -
 * @return: void
 *
 * */
void VidServ_ConvertARGB8888TO888(uint8_t *inputARGB8888Buffer,
				  uint8_t *outputRGB888Buffer,
				  uint32_t output_width, uint32_t output_height)
{
	uint8_t *pOutput8;
	uint8_t *pInput8;
	uint32_t loop_nb;

	/* fill the output */
	pOutput8 = (uint8_t *) (outputRGB888Buffer);
	pInput8 = inputARGB8888Buffer;
	for (loop_nb = output_width * output_height; loop_nb > 0; loop_nb--) {
		*pOutput8 = *pInput8;	/*B*/
		pOutput8++;
		pInput8++;
		*pOutput8 = *pInput8;	/*G*/
		pOutput8++;
		pInput8++;
		*pOutput8 = *pInput8;	/*R*/
		pOutput8++;
		pInput8 += 2;		/*Skip A*/
	}
}

/*
 * VidServ_ConvertARGB8888TO565 - Convert a ARG8B888 buffer to a RGB565 buffer
 *
 * @inputARGB8888Buffer -
 * @outputRGB565Buffer -
 * @output_width -
 * @output_height -
 * @return: void
 *
 * */
void VidServ_ConvertARGB8888TO565(uint8_t *inputARGB8888Buffer,
				  uint8_t *outputRGB565Buffer,
				  uint32_t output_width, uint32_t output_height)
{
	VidServ_ConvertRGB888TO565(inputARGB8888Buffer, outputRGB565Buffer,
				   output_width, output_height);
}

/*
 * VidServ_ConvertRGB888TO565 - Convert a RGB888 buffer to a RGB565 buffer
 *
 * @inputRGB888Buffer -
 * @outputRGB565Buffer -
 * @output_width -
 * @output_height -
 * @return: void
 *
 * */
#define RGB888TO565(p_pixel) ((uint16_t)((((*p_pixel)>>3)<<11) + \
			      (((*(p_pixel+1))>>2)<<5) + ((*(p_pixel+2))>>3)))

void VidServ_ConvertRGB888TO565(uint8_t *inputRGB888Buffer,
				uint8_t *outputRGB565Buffer,
				uint32_t output_width, uint32_t output_height)
{
	uint16_t *pOutput16;
	uint8_t *pInput8;
	uint32_t loop_nb;

	/* fill the output */
	pOutput16 = (uint16_t *) (outputRGB565Buffer);
	pInput8 = inputRGB888Buffer;
	for (loop_nb = output_width * output_height; loop_nb > 0; loop_nb--) {
		*pOutput16 = RGB888TO565(pInput8);
		pOutput16++;
		pInput8 = pInput8 + 3;
	}
}

extern int abxxxx_read(uint16_t addr, uint8_t *val);
extern int abxxxx_write(uint16_t addr, uint8_t val);

/*
 * VidServ_GetSetAB8500Value - Used to get or set a value of AB8500
 *
 * @vp_block - AB8500 bank to write
 * @vp_addr - AB8500 offset to write
 * @pp_pwmRegisterData - if get => returned pwm register value
 *               if set => pwm register value to set
 * @vp_Type - 0 if Get, 1 if Set
 * @return: VIDSERV_NO_ERROR if ok.
 *
 * */
int32_t VidServ_GetSetAB8500Value(uint8_t vp_block, uint8_t vp_addr,
				  uint8_t *pp_pwmRegisterData, int32_t vp_Type)
{

	if (vp_Type == AB8500_REGISTER_READ) {	/* Get */
		if (abxxxx_read(((vp_block << 8) + vp_addr), pp_pwmRegisterData)
		    < 0)
			return VIDSERV_FAILED;

	} else {		/* Set */

		if (abxxxx_write
		    (((vp_block << 8) + vp_addr), *pp_pwmRegisterData) < 0)
			return VIDSERV_FAILED;
	}

	return VIDSERV_NO_ERROR;
}

/*
 * VidServ_GetAB8500Version - Used to get the AB8500 version
 *
 * @Version - pointer where AB8500 version is returned
 * @return: VIDSERV_NO_ERROR if ok.
 *
 * */
uint32_t VidServ_GetAB8500Version(uint8_t *p_Version)
{
	uint32_t vl_error = VIDSERV_NO_ERROR;

	if (VidServ_GetSetAB8500Value
	    (VIDSERV_MISC_BANK, VIDSERV_REVISION_REG_OFFSET, p_Version,
	     AB8500_REGISTER_READ) != VIDSERV_NO_ERROR) {
		vl_error = VIDSERV_FAILED;
		goto error;
	}

	return VIDSERV_NO_ERROR;

error:
	return vl_error;
}

int32_t VidServ_ConvertToFbVar(t_VidServ_DisplayResolution *DisplayResolution,
			       struct fb_var_screeninfo *fb_var,
			       const struct pix_fmt_info *pixf)
{
	int32_t result = VIDSERV_FAILED;
	uint32_t index;

	if (!DisplayResolution || !fb_var)
		return result;

	/* Convert DisplayResolution to an index in resolutionstable */
	index = 0;
	for (index = 0;
	     index < sizeof(resolutionstable) / sizeof(resolutionstable[0]);
	     index++) {
		if ((DisplayResolution->Width == resolutionstable[index].width)
		    && (DisplayResolution->Height ==
			resolutionstable[index].height)
		    && (DisplayResolution->Frequency ==
			resolutionstable[index].framerate)
		    && (((DisplayResolution->ScanMode == PROGRESSIVE)
			 && (resolutionstable[index].interlace == 0))
			|| ((DisplayResolution->ScanMode == INTERLACED)
			    && (resolutionstable[index].interlace == 1)))) {
			/* Found */
			break;
		}
	}

	if (index < sizeof(resolutionstable) / sizeof(resolutionstable[0])) {
		/* Fill in fb_var */
		pixf = &pix_fmt_map[PIX_FMT_INFO_ENUM - 1];

		fb_var->bits_per_pixel = pixf->bpp;
		fb_var->red = pixf->r;
		fb_var->green = pixf->g;
		fb_var->blue = pixf->b;
		fb_var->xres = resolutionstable[index].width;
		fb_var->yres = resolutionstable[index].height;
		fb_var->xres_virtual = resolutionstable[index].width_virtual;
		fb_var->yres_virtual = resolutionstable[index].height_virtual;
		fb_var->pixclock = resolutionstable[index].timing->pixclock;
		fb_var->left_margin =
		    resolutionstable[index].timing->left_margin;
		fb_var->right_margin =
		    resolutionstable[index].timing->right_margin;
		fb_var->upper_margin =
		    resolutionstable[index].timing->upper_margin;
		fb_var->lower_margin =
		    resolutionstable[index].timing->lower_margin;
		fb_var->hsync_len = resolutionstable[index].timing->hsync_len;
		fb_var->vsync_len = resolutionstable[index].timing->vsync_len;
		if (resolutionstable[index].interlace == 1)
			fb_var->vmode = FB_VMODE_INTERLACED;
		else
			fb_var->vmode = FB_VMODE_NONINTERLACED;

		result = VIDSERV_NO_ERROR;
	}

	return result;
}

/* Functions to simulate smbus access. */
static int32_t vidserv_i2c_smbus_access(int32_t file, int8_t read_write,
					int8_t command, int32_t size,
					union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file, I2C_SMBUS, &args);
}

int32_t vidserv_i2c_smbus_read_byte_data(int32_t file, int8_t command)
{
	union i2c_smbus_data data;
	if (vidserv_i2c_smbus_access(file, I2C_SMBUS_READ, command,
				     I2C_SMBUS_BYTE_DATA, &data))
		return -1;
	else
		return 0x0FF & data.byte;
}

int32_t vidserv_i2c_smbus_write_byte_data(int32_t file, int8_t command,
					  int8_t value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return vidserv_i2c_smbus_access(file, I2C_SMBUS_WRITE, command,
					I2C_SMBUS_BYTE_DATA, &data);
}
