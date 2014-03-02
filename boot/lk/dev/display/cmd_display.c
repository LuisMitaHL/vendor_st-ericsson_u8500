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


#if defined (CONFIG_DISPLAY_CMD)
#include <lib/console.h>
#include "target_config.h"
#include "mcde.h"
#include "mcde_display.h"
#if !defined(REMOVE_SPLASH_SCREEN)
#include "splash.h"
#endif
#include <string.h>
#include <printf.h>
#include <malloc.h>
#include <stdlib.h>
#include <db_gpio.h>

extern int board_mcde_display_reset(void);
extern void channelA_config(void);
int channelA_IsConfigured=0;
int display_command_line=0;
extern int init_display_devices(void);
extern int mcde_splash_image(void);

extern struct mcde_display_device *display0;

typedef struct {
    u8 v_type;
    u16 v_Lines;
    u16 v_Columns;
    u32 v_ColorDef1;
    u32 v_ColorDef2;
    u32 v_BufferSize;
    u32 v_DisplayParam;
    u32 *p_RGB32BitBuffer;
    u8 *p_RGB8BitBuffer;
}t_DisplayPattern;

#define DISPLAY_COLOR           0
#define DISPLAY_CHESS           1
#define DISPLAY_GRAY_SCALE      2
#define DISPLAY_CENTER          3
#define DISPLAY_ALIGN           4
#define DISPLAY_VERTCOLORBAR    5

#define DISPLAY_ERROR_OFF       0
#define DISPLAY_BAD_REQ         1
#define DISPLAY_ERROR_CASE      2

void color_choice(char* color1, char* color2, t_DisplayPattern* DisplayPattern)
{
    u16 vl_Red	=0xF800;
    u16 vl_Blue	=0x001F;
    u16 vl_Yellow	=0xFFE0;
    u16 vl_Cyan	=0x1FFE;
    u16 vl_Green	=0x07E0;
    u16 vl_Magenta	=0xF8BD;
    u16 vl_Black	=0x0000;
    u16 vl_White	=0xFFFF;

    if(strncmp(color1,"r",1)==0)
        DisplayPattern->v_ColorDef1 = vl_Red;
    if(strncmp(color1,"g",1)==0)
        DisplayPattern->v_ColorDef1 = vl_Green;
    if(strncmp(color1,"bl",2)==0)
        DisplayPattern->v_ColorDef1 = vl_Blue;
    if(strncmp(color1,"c",1)==0)
        DisplayPattern->v_ColorDef1 = vl_Cyan;
    if(strncmp(color1,"y",1)==0)
        DisplayPattern->v_ColorDef1 = vl_Yellow;
    if(strncmp(color1,"w",1)==0)
        DisplayPattern->v_ColorDef1 = vl_White;
    if(strncmp(color1,"m",1)==0)
        DisplayPattern->v_ColorDef1 = vl_Magenta;
    if(strncmp(color1,"bk",2)==0)
        DisplayPattern->v_ColorDef1 = vl_Black;

    if(strncmp(color2,"r",1)==0)
        DisplayPattern->v_ColorDef2 = vl_Red;
    if(strncmp(color2,"g",1)==0)
        DisplayPattern->v_ColorDef2 = vl_Green;
    if(strncmp(color2,"bl",2)==0)
        DisplayPattern->v_ColorDef2 = vl_Blue;
    if(strncmp(color2,"c",1)==0)
        DisplayPattern->v_ColorDef2 = vl_Cyan;
    if(strncmp(color2,"y",1)==0)
        DisplayPattern->v_ColorDef2 = vl_Yellow;
    if(strncmp(color2,"w",1)==0)
        DisplayPattern->v_ColorDef2 = vl_White;
    if(strncmp(color2,"m",1)==0)
        DisplayPattern->v_ColorDef2 = vl_Magenta;
    if(strncmp(color2,"bk",2)==0)
        DisplayPattern->v_ColorDef2 = vl_Black;
}

/*----------------------------------------------------------------------*
* Procedure    : Display_SetColorBuffer
*----------------------------------------------------------------------*
* Description  : Generate the RGB565_32 bits buffer for the color pattern
*----------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*
*----------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------*/
void Display_SetColorBuffer (t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u16* ptr=NULL;

    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;
    for (i = 0;i<pp_DisplayPattern->v_BufferSize;i++)
    {
        ptr[i]=pp_DisplayPattern->v_ColorDef1;
    }
}


/*----------------------------------------------------------------------*
* Procedure    : Display_SetChessBuffer
*----------------------------------------------------------------------*
* Description  : Generate the RGB565_16 bits buffer for the chess pattern
*----------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*----------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------*/
void Display_SetChessBuffer ( t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u32 j;
    u32 vl_ColorOdd ;
    u32 vl_ColorEven ;
    u16* ptr=NULL;

    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;

    for (i = 0 ; i < pp_DisplayPattern->v_Lines ; i ++)
    {
        if (((i / pp_DisplayPattern->v_DisplayParam)% 2) == 1)
        {
            vl_ColorOdd = pp_DisplayPattern->v_ColorDef1;
            vl_ColorEven = pp_DisplayPattern->v_ColorDef2;
        }
        else
        {
            vl_ColorOdd = pp_DisplayPattern->v_ColorDef2;
            vl_ColorEven = pp_DisplayPattern->v_ColorDef1;
        }
        for (j = 0 ; j < pp_DisplayPattern->v_Columns ; j ++)
        {
            if (((j / pp_DisplayPattern->v_DisplayParam)% 2) == 1)
            {
                ptr[ ((i * pp_DisplayPattern->v_Columns) + j)] = vl_ColorOdd;
            }
            else
            {
                ptr[ ((i * pp_DisplayPattern->v_Columns) + j)]=vl_ColorEven;
            }
        }
    }
}


/*---------------------------------------------------------------------------------*
* Procedure    : Display_SetGrayScaleBuffer
*---------------------------------------------------------------------------------*
* Description  : Generate the RGB565_16 bits buffer for the gray scale pattern
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : None
*---------------------------------------------------------------------------------*/
void Display_SetGrayScaleBuffer (t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u32 j;
    u16 vl_GrayColor = 0x0000 ;
    u32 vl_GrayScaleColumn = 0, vl_FirstColumn = 0, vl_Step;
    u16* ptr=NULL;

    vl_Step = pp_DisplayPattern->v_Columns/8;
    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;

    for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
    {
        /* for 8 columns of gray */
        for (vl_GrayScaleColumn = 0 ; vl_GrayScaleColumn< 8 ; vl_GrayScaleColumn++)
        {
            /* For each column of gray */
            for (j = vl_FirstColumn ; j<(vl_FirstColumn+vl_Step) ; j++)
            {
                ptr[  ((i * pp_DisplayPattern->v_Columns) + j)] = vl_GrayColor;
            }
            /* update for the next column of gray */
            if (vl_GrayScaleColumn == 7)
            {
                vl_GrayColor = 0xFFFF;    /* last column is white */
            }
            else
            {
                vl_GrayColor = vl_GrayColor + 0x2084;
            }
            vl_FirstColumn = vl_FirstColumn + vl_Step;
        }
        /* init for the next line */
        vl_GrayColor = 0x0000;
        vl_FirstColumn = 0;
    }
}


/*---------------------------------------------------------------------------------*
* Procedure    : Display_SetCrossBuffer
*---------------------------------------------------------------------------------*
* Description  : Generate the RGB565_16 bits buffer for the cross pattern
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : None
*---------------------------------------------------------------------------------*/
void Display_SetCrossBuffer ( t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u32 j;
    u16 vl_YCross = pp_DisplayPattern->v_Lines/2, vl_XCross = pp_DisplayPattern->v_Columns/2 ;
    u16* ptr=NULL;

    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;

    for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
    {
        for (j = 0;j<pp_DisplayPattern->v_Columns;j++)
        {
            ptr[(i*pp_DisplayPattern->v_Columns)+j] = pp_DisplayPattern->v_ColorDef1 ; /* background */
        }
    }
    for (j = 0;j<pp_DisplayPattern->v_Columns;j++)
    {
        ptr[(vl_YCross*pp_DisplayPattern->v_Columns)+j] = pp_DisplayPattern->v_ColorDef2; /* horizontal axis  */
    }
    for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
    {
        ptr[(i*pp_DisplayPattern->v_Columns)+vl_XCross] = pp_DisplayPattern->v_ColorDef2; /* vertical axis */
    }
}


/*------------------------------------------------------------------------------------*
* Procedure    : Display_SetAlignBuffer
*------------------------------------------------------------------------------------*
* Description  : Generate the RGB565_16 bits buffer for the Alignment pattern
*------------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*------------------------------------------------------------------------------------*
* Return Value : None
*------------------------------------------------------------------------------------*/
void Display_SetAlignBuffer (t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u32 j;
    u16 vl_Lines = pp_DisplayPattern->v_Lines, vl_Columns = pp_DisplayPattern->v_Columns ;
    u16 vl_VisibleLines = pp_DisplayPattern->v_Lines, vl_VisibleColumns = pp_DisplayPattern->v_Columns;
    u32 vp_BorderSize = pp_DisplayPattern->v_DisplayParam;
    u16 vp_Color1 = pp_DisplayPattern->v_ColorDef1;
    u16 vp_Color2 = pp_DisplayPattern->v_ColorDef2;
    u16* ptr=NULL;

    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;

    for (i = 0;i<vl_Lines;i++)
    {
        for (j = 0;j<vl_Columns;j++)
        {
            if ((i<vp_BorderSize)  /* "vp_BorderSize" first lines */
            ||( (((u32)(vl_VisibleLines-1-vp_BorderSize))<i) && (i<=(((u32)vl_VisibleLines-1)))) /* "vp_BorderSize" last lines */ )
            {
                ptr[((i * vl_Columns) + j)]=vp_Color1;
            }
            else
            {
                if ((j<vp_BorderSize) /* "vp_BorderSize" first columns */
                ||( (((u32)(vl_VisibleColumns-1-vp_BorderSize))<j) && (j<=((u32)(vl_VisibleColumns-1)))) /* "vp_BorderSize" last columns */)
                {
                    ptr[((i * vl_Columns) + j)]=vp_Color1;
                }
                else
                {  /* others pixels */
                    ptr[ ((i * vl_Columns) + j)]=vp_Color2;
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------------*
* Procedure    : Display_SetVerticalColorBarBuffer
*------------------------------------------------------------------------------------*
* Description  : Generate the RGB565_16 bits buffer for the VerticalColor Bar pattern
*------------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*------------------------------------------------------------------------------------*
* Return Value : None
*------------------------------------------------------------------------------------*/
void Display_SetVerticalColorBarBuffer (t_DisplayPattern* pp_DisplayPattern)
{
    u32 i;
    u32 j;
    u16 vl_Lines =pp_DisplayPattern->v_Lines, vl_Columns = pp_DisplayPattern->v_Columns ;
    u16 vl_White=0xFFFF;
    u16 vl_Yellow=0xFFE0;
    u16 vl_Cyan=0x1FFE;
    u16 vl_Green=0x07E0;
    u16 vl_Magenta=0xF8BD;
    u16 vl_Red=0xF800;
    u16 vl_Blue=0x001F;
    u16 vl_Black=0x0000;
    u16* ptr=NULL;

    ptr=(u16*)pp_DisplayPattern->p_RGB8BitBuffer;
    for (i=0;i<vl_Lines;i++)
    {
        for (j=0;j<vl_Columns;j++)
        {
            if (j<(vl_Columns/8))
            {
                ptr[((i * vl_Columns) + j)]=vl_White;
            }
            else if (((vl_Columns/8)<=j)&&(j<((vl_Columns/8)*2)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Yellow;
            }
            else if ((((vl_Columns/8)*2)<=j)&&(j<((vl_Columns/8)*3)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Cyan;
            }
            else if ((((vl_Columns/8)*3)<=j)&&(j<((vl_Columns/8)*4)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Green;
            }
            else if ((((vl_Columns/8)*4)<=j)&&(j<((vl_Columns/8)*5)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Magenta;
            }
            else if ((((vl_Columns/8)*5)<=j)&&(j<((vl_Columns/8)*6)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Red;
            }
            else if ((((vl_Columns/8)*6)<=j)&&(j<((vl_Columns/8)*7)))
            {
                ptr[((i * vl_Columns) + j)]=vl_Blue;
            }
            else
            {
                ptr[((i * vl_Columns) + j)]=vl_Black;
            }
        }
    }
}

/*---------------------------------------------------------------------------------*
* Procedure    : Display_CreatePatternImage
*---------------------------------------------------------------------------------*
* Description  : fill the RGB565_16 bits buffer of the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : u32, error
*---------------------------------------------------------------------------------*/
int Display_CreatePatternImage(t_DisplayPattern* pp_DisplayPattern)
{
    int vl_Error = DISPLAY_ERROR_OFF;
    u32 vl_NbPixels = 0;

    vl_NbPixels = pp_DisplayPattern->v_Lines*pp_DisplayPattern->v_Columns;
    if (vl_NbPixels == 0)
    {
        printf ("vl_NbPixels=0 check WIDTH and HEIGH of picture\n");
        return DISPLAY_BAD_REQ;
    }
    pp_DisplayPattern->v_BufferSize = vl_NbPixels;

    switch (pp_DisplayPattern->v_type)
    {
        case DISPLAY_COLOR:
            Display_SetColorBuffer(pp_DisplayPattern);
        break;
        case DISPLAY_CHESS:
            if (pp_DisplayPattern->v_DisplayParam == 0)
            {
                printf ("Display Pattern :  size couldn't be 0 \n");
                vl_Error = DISPLAY_BAD_REQ;
            }
            else
            {
                Display_SetChessBuffer (pp_DisplayPattern);
            }
        break;
        case DISPLAY_GRAY_SCALE:
            Display_SetGrayScaleBuffer (pp_DisplayPattern);
        break;
        case DISPLAY_CENTER:
            Display_SetCrossBuffer (pp_DisplayPattern);
        break;
        case DISPLAY_ALIGN:
            if (pp_DisplayPattern->v_DisplayParam==0)
            {
                printf ("Display Pattern :  size couldn't be 0 \n");
                vl_Error= DISPLAY_BAD_REQ;
            }
            else
            {
                Display_SetAlignBuffer (pp_DisplayPattern);
            }
            break;
        case DISPLAY_VERTCOLORBAR:
            Display_SetVerticalColorBarBuffer(pp_DisplayPattern);
        break;
        default:
            vl_Error = DISPLAY_ERROR_CASE;
        break;
    }
    return vl_Error ;
}

int do_display(int argc, const cmd_args *argv)
{
    u8* bmp_start=NULL;
    int dst_bpp=0;
    bmp_header_t bmp_header;
    t_DisplayPattern s_DisplayPattern;
    int vl_Error;
    u16 vl_Red	=0xF800;
    u16 vl_Blue	=0x001F;
    u16 vl_Black=0x0000;

#if defined(DISABLE_LCD_API_HARDCODED)
    int ret;

    if(!channelA_IsConfigured)
    {
      ret = mcde_init();
      if (ret)
      {
	printf("%s: mcde_init() returned %d\n",__func__, ret);
	return ret;
      }

      dsilink_init();
      init_display_devices();
      channelA_IsConfigured=1;
    }
    switch (display0->default_pixel_format)
    {
      case MCDE_OVLYPIXFMT_RGB565:
      case MCDE_OVLYPIXFMT_RGBA5551:
      case MCDE_OVLYPIXFMT_RGBA4444:
      case MCDE_OVLYPIXFMT_YCbCr422:
	dst_bpp = 16;
	break;
      case MCDE_OVLYPIXFMT_RGB888:
	dst_bpp = 24;
	break;
      case MCDE_OVLYPIXFMT_RGBX8888:
      case MCDE_OVLYPIXFMT_RGBA8888:
	dst_bpp = 32;
	break;
      default:
	printf("%s: unsupported pixel format %d\n", __func__,display0->default_pixel_format);
	return -1;
	break;
    }
    /* have to force the dst_bpp at 16 like in  mcde_display_image*/
    /* the default pixel format is MCDE_OVLYPIXFMT_RGBA8888 */
    /* so the dst_bpp should have to be equal to 32 !!!*/
    dst_bpp = 16;
    bmp_start = (u8 *) ( CONFIG_SYS_VIDEO_FB_ADRS + dst_bpp / 8 *
				display0->native_x_res *
				display0->native_y_res);
    printf("x_res=%d y_res=%d  cmd display=0x%08x\n",display0->native_x_res,display0->native_y_res,bmp_start);
#else
    dst_bpp = 16;
    bmp_start = (u8 *) ( CONFIG_SYS_VIDEO_FB_ADRS + dst_bpp / 8 *
				CONFIG_SYS_DISPLAY_NATIVE_X_RES2 *
				CONFIG_SYS_DISPLAY_NATIVE_Y_RES);
#endif
    printf("bmp_start cmd display=0x%08x\n",bmp_start);

    if(!display_command_line)
    {
        display_command_line=1;
    }

    bmp_header.signature[0]='B';
    bmp_header.signature[1]='M';
    bmp_header.reserved=0;
    bmp_header.data_offset=70;
    bmp_header.size=56;
    bmp_header.width=480;
    bmp_header.height=854;
    bmp_header.planes=1;
    bmp_header.bit_count=16;
    bmp_header.compression=3;
    bmp_header.image_size = bmp_header.width * bmp_header.height * (bmp_header.bit_count/8);
    bmp_header.file_size = bmp_header.image_size + bmp_header.data_offset;

    bmp_header.x_pixels_per_m=0x0B13;
    bmp_header.y_pixels_per_m=0x0B13;
    bmp_header.colors_used=0;
    bmp_header.colors_important=0;

    if(strncmp(argv[1].str,"on",2)==0)
    {
#if !defined(REMOVE_SPLASH_SCREEN)
        memcpy(bmp_start,_splash_image,sizeof(_splash_image));
#else
	memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_VERTCOLORBAR;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);
#endif
	mcde_splash_image();
    }
    else if(strncmp(argv[1].str,"chess",5)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_CHESS;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	if(argc >3)
	    color_choice((char*)(argv[2].str),(char*)(argv[3].str),&s_DisplayPattern);
	if(argc == 5)
	    s_DisplayPattern.v_DisplayParam = atoul((char*)(argv[4].str));
	else
	    s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp(argv[1].str,"cross",5)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_CENTER;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	if(argc >3)
	    color_choice((char*)(argv[2].str),(char*)(argv[3].str),&s_DisplayPattern);
	if(argc == 5)
	    s_DisplayPattern.v_DisplayParam = atoul((char*)(argv[4].str));
	else
	    s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp((char*)(argv[1].str),"align",5)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_ALIGN;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	if(argc >3)
	    color_choice((char*)(argv[2].str),(char*)(argv[3].str),&s_DisplayPattern);
	if(argc == 5)
	    s_DisplayPattern.v_DisplayParam = atoul((char*)(argv[4].str));
	else
	    s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp((char*)(argv[1].str),"bar",3)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_VERTCOLORBAR;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp((char*)(argv[1].str),"gray",4)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_GRAY_SCALE;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp((char*)(argv[1].str),"color",5)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_COLOR;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Red;
	s_DisplayPattern.v_ColorDef2 = vl_Blue;
	if(argc >2)
	    color_choice((char*)(argv[2].str),(char*)(argv[3].str),&s_DisplayPattern);
	s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else if(strncmp((char*)(argv[1].str),"off",3)==0)
    {
        memcpy(bmp_start,&bmp_header,sizeof(bmp_header_t));
	s_DisplayPattern.v_type = DISPLAY_COLOR;
	s_DisplayPattern.v_Columns = bmp_header.width;
	s_DisplayPattern.v_Lines = bmp_header.height;
	s_DisplayPattern.v_ColorDef1 = vl_Black;
	s_DisplayPattern.v_ColorDef2 = vl_Black;

	s_DisplayPattern.v_DisplayParam = 16;
	s_DisplayPattern.p_RGB8BitBuffer=bmp_start+sizeof(bmp_header_t);
	vl_Error = Display_CreatePatternImage(&s_DisplayPattern);

	mcde_splash_image();
    }
    else
    {
        printf("Usage: display on or display off\n");
    }
    return 0;
}

STATIC_COMMAND_START
    { "display", "display <action> <color1> <color2> <display param>\n-action:on|off|chess|cross|align|bar|gray|color\n-color 1 or 2:r|g|bl|c|y|w|m|bk r|g|bl|c|y|w|m|bk\n-display param:width & height for a basic pattern", &do_display }
STATIC_COMMAND_END(display);

#endif
