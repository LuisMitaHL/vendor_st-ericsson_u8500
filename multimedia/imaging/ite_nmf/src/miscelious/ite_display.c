/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ite_display.h" 
#include "ite_buffer_management.h"

#if !(defined(__PEPS8500_SIA) || defined(_SVP_))

	//For Linux
	#include "OMX_Types.h"
	#include <unistd.h>
	#include <fcntl.h>
#ifdef __ARM_LINUX	
	#include <linux/fb.h>
	#include <sys/mman.h>
	#include <sys/ioctl.h>
#endif //__ARM_LINUX	
	#include <assert.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	//#include <stdint.h>
	#include "mcde_ioctls.h"
#ifdef __ARM_LINUX	
	//#include "b2r2_blt.h"
#include "blt_api.h"
#endif //__ARM_LINUX		
//	#include "b2r2_driver_usr_api.h"
#include "cli.h"
#include "ite_main.h"



extern void MMTE_B2R2_GetMmuOffset(void);
extern volatile t_uint32 g_dplHR_infinite;


void ITE_InitBufferWithBar(tps_siapicturebuffer p_siapictbuffer)
{
   t_uint16 *buffer;
   t_uint16 pixel=0x001f;
   t_uint32 n,m=0;
  
   buffer = (t_uint16 *)(p_siapictbuffer->logAddress);
   
      for (n=0; n<(p_siapictbuffer->framesizeX*p_siapictbuffer->framesizeY); n++) 
      {
      	if(m==p_siapictbuffer->framesizeX)
      	{ m=0;
      	  switch(pixel)
      	  {
      	    case 0x001f: pixel=0x07e0; break;
      	    case 0xf800: pixel=0x001f; break;
      	    case 0x07e0: pixel=0xf800; break;
      	  }
         }
         m++;
         buffer[n] = pixel;
      }

}

void ITE_InitBufferWithColorBar(tps_siapicturebuffer p_siapictbuffer,t_uint8 effect)
{
   t_uint16 *buffer;
   t_uint16 pixel=0x001f;
   t_uint32 n;
  
   buffer = (t_uint16 *)(p_siapictbuffer->logAddress);
      for (n=0; n<(p_siapictbuffer->framesizeX*p_siapictbuffer->framesizeY); n++) 
      {
      	if (effect==0)
      	{
            if(n>(p_siapictbuffer->framesizeX*p_siapictbuffer->framesizeY)/3)pixel=0x07e0;         
            if(n>(p_siapictbuffer->framesizeX*p_siapictbuffer->framesizeY)*2/3)pixel=0xf800;         
         }
         else if(effect==1)pixel=0x001f;
         else if(effect==2)pixel=0x07e0;
         else if(effect== 3)pixel=0xf800;
                 	
            buffer[n] = pixel;
      }

}
void ITE_TestMcdeForDsi(tps_siapicturebuffer p_siapictbuffer)
{
 ITE_InitBufferWithColorBar(p_siapictbuffer,1);
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
MMTE_MCDE_SwFrameSync();
#endif

 LOS_Sleep(200);
  
 ITE_InitBufferWithColorBar(p_siapictbuffer,2);
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
MMTE_MCDE_SwFrameSync();
#endif
 LOS_Sleep(200);
  
 ITE_InitBufferWithColorBar(p_siapictbuffer,3);
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
MMTE_MCDE_SwFrameSync();
#endif
 LOS_Sleep(100);
 
 ITE_InitFrameBuffer(p_siapictbuffer);
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
MMTE_MCDE_SwFrameSync();
#endif
 LOS_Sleep(200);
 
 ITE_InitBufferWithColorBar(p_siapictbuffer,0);
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
MMTE_MCDE_SwFrameSync();
#endif
 LOS_Sleep(200);
 
} 	

 void ITE_StopMcde(void)
{
  // MMTE_StopMcde(MCDE_CH_C0);
  //  SER_DSI_SendDirectCommand_LP(0x05,0x01,0x00000028,0x00000000,0x00000000,0x00000000);   /*display off*/
}

  
void ITE_InitFrameBuffer(tps_siapicturebuffer p_siapictbuffer)
{
#define FRACTAL_MAX_ITER 32
#define FRACTAL_MAX_BOUNDARY 4

/**
 * Draw a fractal on a t_uint16[x][y] framebuffer
 * 
 * \param buffer a pointer to a framebuffer of size XxY and 16bit depth
 * 
 * Potential improvements: 
 *  - change the 'c' value (currently x_init and y_init, when 
 *    calculating x and y increments)
 *  - change the area of interest (currently x=[-2.0,1], y=[-1,1], calculated 
 *    in the first lines of each pixel calculation  
 **/
    t_uint16 *buffer;
    int x_scr, y_scr;
    float x, y, x_init, y_init;
    int iter;
    float xtemp;
    
    buffer = (t_uint16 *)(p_siapictbuffer->logAddress);
    for (x_scr=0;(t_uint32) x_scr<p_siapictbuffer->framesizeX; x_scr++) {
        for (y_scr=0; (t_uint32) y_scr<p_siapictbuffer->framesizeY; y_scr++) {
            iter = 0;
            x = x_init = (float)x_scr * 3.0 / p_siapictbuffer->framesizeX - 2.0;
            y = y_init = (float)y_scr * 2.0 / p_siapictbuffer->framesizeY - 1.0;
            
            while ((iter<FRACTAL_MAX_ITER) && (x*x + y*y)< FRACTAL_MAX_BOUNDARY) {
                xtemp = x*x - y*y +x_init;
                y = 2*x*y + y_init;
                x = xtemp;
                iter++;
            }
            
            if (iter == FRACTAL_MAX_ITER) {
                buffer[y_scr*p_siapictbuffer->framesizeX+x_scr] = 0;
            } else {
                buffer[y_scr*p_siapictbuffer->framesizeX+x_scr] = iter;
            }
        }
    }
}

void ITE_configureMcde(tps_siapicturebuffer p_siapictbuffer)
{
 UNUSED(p_siapictbuffer);
/*
   ts_mcde_src_info mcdeSrcInfo;
   ts_mcde_src_buffer_config mcde_src_buffer_config;
   
   mcdeSrcInfo.color_depth= MCDE_RGB565_16_BIT;
   //mle mcdeSrcInfo.color_depth= p_siapictbuffer->MCDE_colorformat;
   mcdeSrcInfo.src_width=p_siapictbuffer->framesizeX;
   mcdeSrcInfo.src_height=p_siapictbuffer->framesizeY;
   mcde_src_buffer_config.num=MCDE_BUFFER_USED_1;
   mcde_src_buffer_config.buffer_0_base_addr=p_siapictbuffer->address;
   
   MMTE_ConfigureMcde(&mcdeSrcInfo, 
                      &mcde_src_buffer_config, 
                      LCD_SCREEN,
		      mcdeSrcInfo.src_width,
		      MCDE_CH_C0);
    LOS_Sleep(50);
   MMTE_MCDE_SwFrameSync();
   // workaround !!!!!!!
   //anel power up
   SER_DSI_TAALDisplayInit();
    LOS_Sleep(50);
   MMTE_MCDE_SwFrameSync();
   //Panel power up
   SER_DSI_TAALDisplayInit();
    LOS_Sleep(50);
   MMTE_MCDE_SwFrameSync();	      
		      		      		      		      
*/
}	

void ITE_B2R2SwHandler(void)
{
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
if(g_dplHR_infinite!=0) MMTE_MCDE_SwFrameSync();
#endif
}	
/***********************************************************/
/* void ITE_InitB2R2(tps__b2r2_config node,		   */
/* 		     tps_siapicturebuffer p_inputbuffer,   */
/*  		     tps_siapicturebuffer p_outputbuffer)  */
/* initialise a b2r2 node according to InputPicturebuffer  */
/*        and OutputPicturebuffer info			   */
/***********************************************************/

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
void ITE_InitB2R2(tps__b2r2_config node, tps_siapicturebuffer p_inputbuffer, tps_siapicturebuffer p_outputbuffer)
{
 // initialize mmu offset	
 MMTE_B2R2_GetMmuOffset();
 	
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
   node->src_color_format = (t_b2r2_color_form)p_inputbuffer->B2R2_colorformat;
   node->dest_color_format = (t_b2r2_color_form)p_outputbuffer->B2R2_colorformat;
#endif
   node->is420_not422 = (t_bool)p_inputbuffer->is420_not422;
   node->src_frame_width = p_inputbuffer->framesizeX;
   node->src_frame_height = p_inputbuffer->framesizeY;
   node->dest_frame_width = p_outputbuffer->windowsizeX;    // ???
   node->dest_frame_height = p_outputbuffer->windowsizeY;   // ???
   node->source_adr = p_inputbuffer->address;
   node->destination_adr = (t_physical_address)(p_outputbuffer->address);
   node->targ_x_cord = p_outputbuffer->offsetX;
   node->targ_y_cord = p_outputbuffer->offsetY;
   MMTE_B2R2Init(node,p_outputbuffer->windowsizeX);
   
   // B2R2 IT handler connection
   MMTE_B2R2InstallIrq();
   MMTE_B2R2installSwHandler( (t_gic_func_ptr) &ITE_B2R2SwHandler);
};
#endif

t_uint8 ITE_StoreinLCDBuffer(ts_siapicturebuffer* p_Buffer)
{ 
   ts_PelRGB888 color;
   t_uint32 i,j,k;
   t_uint16 sizex,sizey;
   // if p_Buffer bigger than VGA -> crop left top part of picture
   if (p_Buffer->framesizeX < 641) 
       sizex =  p_Buffer->framesizeX;
   else sizex = 640;
   if (p_Buffer->framesizeY < 481) 
       sizey =  p_Buffer->framesizeY;
   else sizey = 480;
   ITE_createLCDBuffer(sizex, sizey,GRBFMT_A8R8G8B8);
   ITE_StopMcde();
   ITE_configureMcde(&LCDBuffer);
   k=0;   
   for (j=0;j<sizey;j++) {
     for (i=0;i<sizex;i++) {
         ITE_GetRGBvalue(p_Buffer,i,j,&color);
 	 *((t_uint8 *)(LCDBuffer.logAddress + k)) = color.BlueValue; k++;
 	 *((t_uint8 *)(LCDBuffer.logAddress + k)) = color.GreenValue; k++;
 	 *((t_uint8 *)(LCDBuffer.logAddress + k)) = color.RedValue; k++;
 	 *((t_uint8 *)(LCDBuffer.logAddress + k)) = 0xff; k++;
         }
     }
   return TRUE;
}
#endif

/* #############################################################################
      Functions written specifically for Linux display support
##############################################################################*/

#ifdef __ARM_LINUX
/***********************************************************/
/* void ITE_Configure_FrameBuffer(mcde_source_buffer)  	   */
/* initialize the framebuffer device to get 		   */
/* the buffer to display			   	   */
/***********************************************************/


void ITE_Configure_FrameBuffer(struct mcde_source_buffer* fb_buffer, int fbfd)
{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	// Get fixed screen infos
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		LOS_Log("Error reading fixed information.\n");
	return;
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		LOS_Log("Error reading variable information.\n");
		return;
	}

	vinfo.reserved[0] = 0;
	vinfo.reserved[1] = 0;
	vinfo.reserved[2] = 0;
	vinfo.xoffset = 0;
	vinfo.yoffset = 0;
	//vinfo.activate = FB_ACTIVATE_NOW;
	vinfo.activate = FB_ACTIVATE_FORCE;

	/*
	 * Explicitly request 5/6/5
	 */
	vinfo.bits_per_pixel = 16;
	vinfo.red.offset     = 11;
	vinfo.red.length     = 5;
	vinfo.green.offset   = 5;
	vinfo.green.length   = 6;
	vinfo.blue.offset    = 0;
	vinfo.blue.length    = 5;
	vinfo.transp.offset  = 0;
	vinfo.transp.length  = 0;

#define NUM_BUFFERS 1

	/*
	 * Request NUM_BUFFERS screens (at lest 2 for page flipping)
	 */
	vinfo.yres_virtual = vinfo.yres * NUM_BUFFERS;

	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
		vinfo.yres_virtual = vinfo.yres;
		printf("FBIOPUT_VSCREENINFO failed, page flipping not supported");
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
 return; /*return -errno;*/ /*void function so return statement must be avoided */


	int refreshRate = 1000000000000000LLU /
	(
	 (unsigned long int )( vinfo.upper_margin + vinfo.lower_margin + vinfo.yres )
	 * ( vinfo.left_margin  + vinfo.right_margin + vinfo.xres )
	 * vinfo.pixclock
	);

	if (refreshRate == 0) {
		// bleagh, bad info from the driver
		refreshRate = 60*1000;  // 60 Hz
	}

	if ((int)(vinfo.width) <= 0 || (int)(vinfo.height) <= 0) {
		// the driver doesn't return that information
		// default to 160 dpi
		vinfo.width  = ((vinfo.xres * 25.4f)/160.0f + 0.5f);
		vinfo.height = ((vinfo.yres * 25.4f)/160.0f + 0.5f);
	}

//	float xdpi = (vinfo.xres * 25.4f) / vinfo.width;
//	float ydpi = (vinfo.yres * 25.4f) / vinfo.height;
//	float fps  = refreshRate / 1000.0f;


	if(fb_buffer->buffid != 0){
		//Specify the buffer to map to the display
		if (ioctl(fbfd, MCDE_IOCTL_SET_SOURCE_BUFFER, fb_buffer)) {
			LOS_Log("Error setting source buffer information.\n");
			return;
		}
	}
	else{
		//The buffer is already known by the display driver
	}


	/* Request update of FB */
	vinfo.yoffset = 0;
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		LOS_Log("Error panning display.\n");
		return;
	}

	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		CLI_disp_msg("Error panning display.\n");
 return; /*return 5*/ /*void function so return statement must be avoided */
	}

	close (fbfd);

}

/********************************************************/
/* ITE_Refresh_FrameBuffer(void)                        */
/* Refresh display managed by FB                        */
/********************************************************/
void ITE_Refresh_FrameBuffer(void)
{

	struct fb_var_screeninfo vinfo;
	char fbname[1024];
	int fbfd = -1;

	strcpy(fbname,"/dev/fb0");
	fbfd = open(fbname, O_RDWR);

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		LOS_Log("Error reading variable information 2.\n");
		return;
	}

	vinfo.reserved[0] = 0;
	vinfo.reserved[1] = 0;
	vinfo.reserved[2] = 0;
	vinfo.xoffset = 0;
	vinfo.yoffset = 0;
	//vinfo.activate = FB_ACTIVATE_NOW;
	vinfo.activate = FB_ACTIVATE_FORCE;

	/* Request update of FB */
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		LOS_Log("Error panning display 2.\n");
		return;
	}

	close(fbfd);
}

/******************************************************************************/
/* ITE_Configure_B2R2(tps_siapicturebuffer, unsigned int *)                   */
/* Configure the B2R2 for converting the buffer in a MCDE supported format    */
/******************************************************************************/
void ITE_Configure_B2R2(tps_siapicturebuffer Buffer, unsigned int * fb_address)
{
 UNUSED(Buffer);
 UNUSED(fb_address);
#if 0
	struct b2r2_blt_req b2r2_request;

	int request_id, ret, b2r2fd = -1;
 UNUSED(ret);

	memset(&b2r2_request, 0, sizeof(b2r2_request));
	b2r2_request.size = sizeof(b2r2_request);

	// Define the destination buffer features
	b2r2_request.dst_img.buf.type = BLT_PTR_PHYSICAL;
	b2r2_request.dst_img.buf.offset = (int)fb_address;
	b2r2_request.dst_img.fmt = BLT_FMT_16_BIT_RGB565; // Need a format name conversion for being compliance
	
	b2r2_request.dst_img.width = 864;
	b2r2_request.dst_img.height = 480;
	
//	b2r2_request.dst_rect.width        // Needed for stride ???
//	b2r2_request.dst_rect.height       // To be checked....
	
	
	// Define the source buffer features
	b2r2_request.src_img.buf.type = BLT_PTR_PHYSICAL;
	b2r2_request.src_img.buf.offset = Buffer->address;
	b2r2_request.src_img.width = Buffer->framesizeX;
	b2r2_request.src_img.height = Buffer->framesizeY;

	b2r2_request.src_rect.width = Buffer->framesizeX;     //Needed for stride
	b2r2_request.src_rect.height = Buffer->framesizeY;    //Do we have some information about that???
	
	if(Buffer->Grb_colorformat == GRBFMT_R5G6B5) {
		b2r2_request.src_img.fmt = BLT_FMT_16_BIT_RGB565;
	} 
	else if (Buffer->Grb_colorformat == GRBFMT_YUV422_MB_SEMIPLANAR ||
			Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED ||
			Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED) {
		b2r2_request.src_img.fmt = BLT_FMT_YUV420_PACKED_SEMIPLANAR_MB_STE;
	}
	else if (Buffer->Grb_colorformat == GRBFMT_YUV422_RASTER_INTERLEAVED ||
			Buffer->Grb_colorformat == GRBFMT_YUV420_RASTER_PLANAR_I420 || 
			Buffer->Grb_colorformat == GRBFMT_YUV420_RASTER_PLANAR_YV12) {
		b2r2_request.src_img.fmt = BLT_FMT_YUV420_PACKED_PLANAR;
	}
	

	// Obtain a file descriptor to the driver
	b2r2fd = open("/dev/b2r2", O_RDWR);

	// Make a request
	//request_id = blt_request(b2r2fd,&b2r2_request);
	request_id = ioctl(b2r2fd, B2R2_BLT_IOC, &b2r2_request);
	
	//blt_synch(b2r2fd, request_id);
	ioctl(b2r2fd, B2R2_BLT_SYNCH_IOC, request_id);

	// Close the driver
	close(b2r2fd);
#endif
}



#endif



