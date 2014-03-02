/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#define ITE_SIA_BUFFER_C_


#include "ite_sia_buffer.h"
#include "ite_main.h"

//For Linux
#include <stdio.h>
#include <mmhwbuffer.h>
#include "MMIO_Camera.h"

#include <assert.h>

#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_focuscontrol_functions.h"
#include "ite_vpip.h"


#include <string.h> // for size_t

extern ts_sia_usecase usecase;
extern ts_stat_test_usecase Glace_uc;
extern tps_frameParamStatus pFrameParamStatus;

volatile NVMBuffersStorage_t    g_NVMStatsBufferStorage;
volatile NVMBuffersStorage_t*   g_pNVMStatsBufferStorage = &g_NVMStatsBufferStorage;
//volatile StatsBuffersStorage_t  g_StatsBufferStorage;
volatile StatsBuffersStorage_t* g_pStatsBufferStorage/* = &g_StatsBufferStorage*/;

//ISP FW Trace property
TraceProp_ts g_TraceProp = {TraceMsgOutput_e_XTI, 0x0, 0x0};

/********************************************************/
/* 		"Public" functions 	 		*/
/********************************************************/



/********************************************************/
/* int ITE_createSiaPictureBuffer_withoutallocation(			*/
/* 		tps_siapicturebuffer p_siapictbuffer,	*/
/*		t_uint32 PictureSizeX,			*/
/*		t_uint32 PictureSizeY,			*/
/*		t_uint32 ColorFormat)			*/
/* initialize SIA picture buffer structure		*/
/********************************************************/
void ITE_createSiaPictureBuffer_withoutallocation(tps_siapicturebuffer p_siapictbuffer,
			       t_uint32 PictureSizeX,
			       t_uint32 PictureSizeY,
			       enum e_grabFormat ColorFormat)
{
    // Note ColorFormat is GRAB color Format
    OutputFormat_te PipeOutputformat;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
    t_b2r2_color_form b2r2colorformat = (t_b2r2_color_form)0xff;
    t_mcde_src_image_format	MCDEcolorformat;
#endif
    t_bool is420_not422 = (t_bool) FALSE;
    int BufferSizeByte;
    int pelorder;
    int nonactivelines;
    // should implement a verification of pipe output valid format HR/LR
    // int PipeErrorType;


switch (ColorFormat) {
  case (GRBFMT_YUV422_RASTER_INTERLEAVED):      // Y Cb Y Cr  could be managed direct by MCDE

    PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_YCbCr_422;
        b2r2colorformat = (t_b2r2_color_form)0xff;  // Warning Not known for B2R2 point of view
#endif
	is420_not422 = (t_bool) FALSE;
	BufferSizeByte = PictureSizeX * PictureSizeY * 2;
  	pelorder=0;
	nonactivelines=0;
break;
  case (GRBFMT_YUV422_MB_SEMIPLANAR):		//

    PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = YCBCR42XMBN;
#endif
	is420_not422 = (t_bool) FALSE;
	BufferSizeByte = PictureSizeX * PictureSizeY * 2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_YUV420_RASTER_PLANAR_I420):

    PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = (t_b2r2_color_form)YCBCR42XR2B;  // Warning Not known for B2R2 point of view
#endif
	is420_not422 = (t_bool) TRUE;
	BufferSizeByte = (PictureSizeX * PictureSizeY * 3)/2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_YUV420_RASTER_PLANAR_YV12):

       PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = (t_b2r2_color_form)YCBCR42XR2B;  // Warning Not known for B2R2 point of view
#endif
	is420_not422 = (t_bool) TRUE;
	BufferSizeByte = (PictureSizeX * PictureSizeY * 3)/2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
       PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = YCBCR42XMBN;
#endif
	is420_not422 = (t_bool) TRUE;
	BufferSizeByte = (PictureSizeX * PictureSizeY * 3)/2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
        PipeOutputformat = OutputFormat_e_YUV;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = YCBCR42XMBN;
#endif
	is420_not422 = (t_bool) TRUE;
	BufferSizeByte = (PictureSizeX * PictureSizeY * 3)/2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_RGB30):
        PipeOutputformat = OutputFormat_e_RGB101010_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = (t_b2r2_color_form)0xff;  // Warning Not known for B2R2 point of view
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 4;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_RAW8):
  case (GRBFMT_FAST_RAW8):
        PipeOutputformat = (OutputFormat_te)0xFF; // Warning Not Know, only for BMS/BML
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = (t_b2r2_color_form)0xff;  // Warning Not known for B2R2 point of view
	if(usecase.sensor == 1)    // sensor 558
		{pelorder=0;
 		 nonactivelines=5;}
	else    {pelorder=0;
 		 nonactivelines=2;}
#endif
#if ( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	{pelorder=0;
        nonactivelines=0;}
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
        PictureSizeX = (((191+ (PictureSizeX * 8))/192) *192)/8;
	BufferSizeByte = PictureSizeX * PictureSizeY ;
	break;
  case (GRBFMT_RAW12):
  case (GRBFMT_FAST_RAW12):
        PipeOutputformat = (OutputFormat_te)0xFF; // Warning Not Know, only for BMS/BML
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = (t_mcde_src_image_format)0xff;  // format non compatible with MCDE
        b2r2colorformat = (t_b2r2_color_form)0xff;  // Warning Not known for B2R2 point of view
	if(usecase.sensor == 1)    // sensor 558
		{pelorder=0;
 		 nonactivelines=5;}
	else    {pelorder=0;
 		 nonactivelines=2;}
#endif
#if ( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	{pelorder=0;
        nonactivelines=0;}
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable

        PictureSizeX = (((191+ (PictureSizeX * 12))/192) *192)/12;
	BufferSizeByte = (PictureSizeX * PictureSizeY *3)/2 ;
	break;
  case (GRBFMT_A4R4G4B4):
        PipeOutputformat = OutputFormat_e_RGB444_UNPACKED_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_16BPP_ARGB_4444;
        b2r2colorformat = ARGB4444;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_A1R5G5B5):
        PipeOutputformat = OutputFormat_e_RGB555_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_16BPP_IRGB_1555;
        b2r2colorformat = ARGB1555;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_R5G6B5):
        PipeOutputformat = OutputFormat_e_RGB565_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_16BPP_RGB_565;
        b2r2colorformat = RGB565;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 2;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_R8G8B8):
        PipeOutputformat = OutputFormat_e_RGB888_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_24BPP_PACKED_RGB_888;  // OR ???? SRC_FORMAT_24BPP_UNPACKED_RGB_888
        b2r2colorformat = RGB888;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 3;
	pelorder=0;
	nonactivelines=0;
	break;
  case (GRBFMT_A8R8G8B8):
        PipeOutputformat = OutputFormat_e_RGB888_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_32BPP_ARGB_8888;
        b2r2colorformat = ARGB8888;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 4;
	pelorder=0;
	nonactivelines=0;
	break;
  default : 	// Just to avoid compilation Warning
        PipeOutputformat = OutputFormat_e_RGB888_PEAKED;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	MCDEcolorformat = SRC_FORMAT_32BPP_ARGB_8888;
        b2r2colorformat = ARGB8888;
#endif
	is420_not422 = (t_bool) FALSE;  	 // not applicable
	BufferSizeByte = PictureSizeX * PictureSizeY * 4;
	pelorder=0;
	nonactivelines=0;
	break;
	}

	p_siapictbuffer->buffersizeByte = BufferSizeByte;
	p_siapictbuffer->Grb_colorformat = ColorFormat;
        p_siapictbuffer->PipeOutputformat = PipeOutputformat;
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	p_siapictbuffer->B2R2_colorformat = b2r2colorformat;
        p_siapictbuffer->MCDE_colorformat = MCDEcolorformat;
        p_siapictbuffer->pelorder = pelorder;
	p_siapictbuffer->nonactivelines = nonactivelines;
#endif
#if ( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
        p_siapictbuffer->pelorder = pelorder; //__NO_WARNING__ 0;
	p_siapictbuffer->nonactivelines = nonactivelines; //__NO_WARNING__ 0;
#endif
	p_siapictbuffer->is420_not422 = is420_not422;
	p_siapictbuffer->framesizeX = PictureSizeX;
	p_siapictbuffer->framesizeY = PictureSizeY;
	p_siapictbuffer->windowsizeX = PictureSizeX;
	p_siapictbuffer->windowsizeY = PictureSizeY;
	p_siapictbuffer->offsetX = 0;
	p_siapictbuffer->offsetY = 0;
}

/********************************************************/
/* int ITE_createSiaPictureBuffer(			*/
/* 		tps_siapicturebuffer p_siapictbuffer,	*/
/*		t_uint32 PictureSizeX,			*/
/*		t_uint32 PictureSizeY,			*/
/*		t_uint32 ColorFormat)			*/
/* initialize SIA picture buffer structure		*/
/* alloc associated memory				*/
/* return allocation information			*/
/********************************************************/
int ITE_createSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer,
			       t_uint32 PictureSizeX,
			       t_uint32 PictureSizeY,
			       enum e_grabFormat ColorFormat,
			       t_siabuffer_pipe pipe)
{

	t_uint32 i; //__NO_WARNING__
	t_uint8 *p;
	static OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PTR dummy;
	OMX_U8 *pLogicalAddress;
	UNUSED(i); UNUSED(p); UNUSED(dummy); //__NO_WARNING__ 
	
	if (p_siapictbuffer->address) { // __NO_WARNING__ != NULL) {
     	        LOS_Log("WARNING Picture Buffer already allocated\n");
		LOS_Log("Free Picture Buffer automatically and reallocated another one\n");
		ITE_FreeSiaPictureBuffer(p_siapictbuffer);
		}

	ITE_createSiaPictureBuffer_withoutallocation(p_siapictbuffer, PictureSizeX, PictureSizeY, ColorFormat);

	//DG: for debug
	/*
	{
	static nb=0;
	nb++;

		if (nb==6)
		{
			LOS_GetAllocatedChunk();
			LOS_DumpAllocatedChunk();
		}

	}
	*/
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	p_siapictbuffer->handle = LOS_Alloc(p_siapictbuffer->buffersizeByte, 256, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	p_siapictbuffer->address = ILOS_GetPhysicalAddress(p_siapictbuffer->handle);
	p_siapictbuffer->logAddress = LOS_GetLogicalAddress(p_siapictbuffer->handle);
	p_siapictbuffer->pipe = pipe;

	if(p_siapictbuffer->handle != 0)
	{

		//init buffer: mandatory for buffer fill test
		for (i=0; i<p_siapictbuffer->buffersizeByte; i++)
		{
			p = (t_uint8 *) p_siapictbuffer->logAddress + i;
			*p = 0xff;
		}
	}
	else LOS_Log("!!!!!!!!!!!! Allocate error !!!!!!!!!!!!\n",  NULL,NULL,NULL,NULL,NULL,NULL);

#endif

#if ( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) )
	//Use of mm_hw_buffer

	MMHwBuffer *sharedChunk=0;
	MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
	OMX_BOOL isCached=OMX_FALSE;
	//mel poolAttrs.iBuffers = 0;      			// Number of buffers in the pool
	poolAttrs.iBuffers = 1;      				// Number of buffers in the pool
	poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
	poolAttrs.iSize = p_siapictbuffer->buffersizeByte;     // Size (in byte) of a buffer
	poolAttrs.iAlignment = -1;                             // Alignment applied to the base address of each buffer in the pool
							       //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
	poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
	sharedChunk = NULL;
	//Buffer creation
	err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

	if(err!=OMX_ErrorNone)
	{
		printf("Not Enough Resources..\nMemory not allocated.\n");
  LOS_Log("ERROR - ITE_createSiaPictureBuffer: MMHwBuffer::Create Memory Not allocated\n");
		return 0;
	}

	//Buffer physical address
	OMX_ERRORTYPE error = OMX_ErrorNone;
	MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
	//__NO_WARNING__
	if(error!=OMX_ErrorNone)
	{
		printf("Buffer Physical Address returned Error\n");
        LOS_Log("ERROR - ITE_createSiaPictureBuffer: MMHwBuffer::TBufferInfo Failed address:0x%x\n",(t_uint32)bufferInfo.iPhyAddr);
		return 0;
	}

	//p_siapictbuffer->address = (t_uint32) HwBuffer::GetBufferPhysicalAddress(dummy);
	p_siapictbuffer->address = (t_uint32)bufferInfo.iPhyAddr;
	p_siapictbuffer->pipe = pipe;
	p_siapictbuffer->handle = (OMX_U32) sharedChunk;
	pLogicalAddress = (OMX_U8 *)bufferInfo.iLogAddr;

	if(NULL == memset (pLogicalAddress, 0xAA, (OMX_U32) p_siapictbuffer->buffersizeByte))
	{
		printf("memcpy failed\n");
        LOS_Log("WARNING - ITE_createSiaPictureBuffer: memset Failed\n");
	} else
		printf("memcpy passed\n");

	LOS_Log("SiaPictureBuffer LogicalAddress : 0x%x\n",(unsigned int) pLogicalAddress);
	p_siapictbuffer->logAddress = (t_los_logical_address) pLogicalAddress;
	LOS_Log("SiaPictureBuffer PhysicalAddress : 0x%x\n", p_siapictbuffer->address);	
#endif

	return(p_siapictbuffer->address);
}



/********************************************************/
/* int ITE_useSiaPictureBuffer(			        */
/* 		tps_siapicturebuffer p_siapictbuffer,	*/
/*		t_uint32 PictureSizeX,			*/
/*		t_uint32 PictureSizeY,			*/
/*		t_uint32 ColorFormat)			*/
/*		t_uint32 PhysicalAdress)		*/
/* initialize SIA picture buffer structure		*/
/********************************************************/
void ITE_useSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer,
       t_uint32 PictureSizeX,
       t_uint32 PictureSizeY,
       enum e_grabFormat ColorFormat,
       t_uint32 PhysicalAdress,
       t_los_logical_address    LogicalAdress)
{

    ITE_createSiaPictureBuffer_withoutallocation(p_siapictbuffer, PictureSizeX, PictureSizeY, ColorFormat);

    p_siapictbuffer->handle = 0 ;
    p_siapictbuffer->address = PhysicalAdress;
    p_siapictbuffer->logAddress = LogicalAdress;
}



/***********************************************************************/
/* void ITE_FreeSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer) */
/* free memory allocation					       */
/***********************************************************************/
void ITE_FreeSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer) {

    static OMX_ERRORTYPE err = OMX_ErrorNone;

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
    LOS_Free(p_siapictbuffer->handle);
#endif

#if ( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
    err = MMHwBuffer::Destroy((MMHwBuffer *&)p_siapictbuffer->handle);

    if(err!=OMX_ErrorNone)
    {
       printf("MMHwBuffer::Destroy Failed.\n");
       LOS_Log("ERROR - ITE_FreeSiaPictureBuffer: MMHwBuffer::Destroy FAILED\n");
       return;
    }
#endif
    p_siapictbuffer->handle = 0;
    p_siapictbuffer->address = 0;
    p_siapictbuffer->logAddress = 0;
}

/***********************************************************************/
/* void ITE_unuseSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer) */
/* free memory allocation					       */
/***********************************************************************/
void ITE_unuseSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer)
{

    p_siapictbuffer->handle = 0;
    p_siapictbuffer->address = 0;
    p_siapictbuffer->logAddress = 0;
}


void ITE_Initialize_SiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer)
{

   t_los_file *out_file = NULL;
   size_t file_size;
   size_t read;

   t_uint16 header_size;


switch (p_siapictbuffer->Grb_colorformat) {
  case (GRBFMT_YUV422_RASTER_INTERLEAVED):
	out_file = LOS_fopen("clownyuv422ri.gam","rb");
	break;
  case (GRBFMT_YUV422_MB_SEMIPLANAR):
	break;
  case (GRBFMT_YUV420_RASTER_PLANAR_I420):
	break;
  case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
	break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
	break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
	break;
  case (GRBFMT_RGB30):
	break;
  case (GRBFMT_RAW8):
  case (GRBFMT_FAST_RAW8):
	break;
  case (GRBFMT_RAW12):
  case (GRBFMT_FAST_RAW12):
	break;
  case (GRBFMT_A4R4G4B4):
	out_file = LOS_fopen("clownargb4444.gam","rb");
	break;
  case (GRBFMT_A1R5G5B5):
	out_file = LOS_fopen("clownargb1555.gam","rb");
	break;
  case (GRBFMT_R5G6B5):
	out_file = LOS_fopen("clownrgb565.gam","rb");
	break;
  case (GRBFMT_R8G8B8):
	out_file = LOS_fopen("clownrgb888.gam","rb");
	break;
  case (GRBFMT_A8R8G8B8):
	out_file = LOS_fopen("clownargb8888.gam","rb");
	break;
  default : 	// Just to avoid compilation Warning
	break;
	}
 if(out_file==NULL)
    {LOS_Log("Open file error !!!!!!!!!!!!\n",  NULL,NULL,NULL,NULL,NULL,NULL);}
 LOS_fseek(out_file, 0, LOS_SEEK_END);
 file_size = (size_t)LOS_ftell(out_file);
 LOS_fseek(out_file, 0, LOS_SEEK_SET);
 LOS_Log("file size (header + data) is %i bytes\n",  (void*)file_size, NULL, NULL, NULL, NULL, NULL);
 read = LOS_fread((void *)(&header_size), 1,2, out_file);
 if ((header_size == 6) || (header_size == 8)) {
    header_size = 4*header_size;
    read = LOS_fread((void *)(p_siapictbuffer->logAddress), 1,header_size-2, out_file);}
 else {
    header_size = 0;
    LOS_fseek(out_file, 0, LOS_SEEK_SET);}
 read = LOS_fread((void *)(p_siapictbuffer->logAddress), 1, file_size-header_size, out_file);
 if (read != (file_size-header_size))
     {LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)(file_size-header_size), NULL, NULL, NULL, NULL);}
 LOS_fclose(out_file);
 LOS_Log("output file %s written\n","clownrgb565.gam",NULL,NULL,NULL,NULL,NULL);
}


void ITE_createLCDBuffer(t_uint16 sizex, t_uint16 sizey,enum e_grabFormat format) {

   if (LCDBuffer.address) /* __NO_WARNING__ != NULL)*/ ITE_FreeSiaPictureBuffer(&LCDBuffer);
   ITE_createSiaPictureBuffer(&LCDBuffer,sizex,sizey,format,BUF);
}






void ITE_NMF_createStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData)
{

    t_uint32 FrameParamStatusBufferAddr=0;
    int i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;
    t_uint32 size_ts_frameParamStatus = ITE_readPE(FrameParamStatus_Extn_u32_SizeOfFrameParamStatus_Byte0);
    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
    int size = Glace_uc.gridWidth * Glace_uc.gridHeight * 4 + sizeof(StatsBuffersStorage_t) + size_ts_frameParamStatus;  // Configurable size for CNips3AAPI::TGrid

    g_pStatsBufferStorage = NULL; // Initialization

    if (pBufferMetaData->handle == 0)
    {
    //    pBufferMetaData->handle = LOS_Alloc(size, 1024*1024*16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    //    pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);


        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
        poolAttrs.iSize = size;     // Size (in byte) of a buffer
        poolAttrs.iAlignment = 64;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
            LOS_Log("ERROR - ITE_NMF_createStatsBuffer: MMHwBuffer::Create FAILED\n");
            return;
        }

        //Buffer physical address
        OMX_ERRORTYPE error = OMX_ErrorNone;;
        MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
        //__NO_WARNING__
		if(error!=OMX_ErrorNone)
        {
            LOS_Log("ERROR - ITE_NMF_createStatsBuffer: MMHwBuffer::TBufferInfo FAILED\n");
            return;
        }

        pBufferMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pBufferMetaData->handle = (OMX_U32) sharedChunk;
        pBufferMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pBufferMetaData->logAddress), 0xAA, (OMX_U32) size))
        {
                printf("memcpy failed\n");
				pBufferMetaData->size = 0;
        } 
		else
         {      
                printf("memcpy passed\n");
				pBufferMetaData->size = size;
		 }

        printf("LogicalAddress : 0x%x\n", pBufferMetaData->logAddress);




    }
    else LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

    if(pBufferMetaData->handle != 0)
    {	//init buffer: mandatory for buffer fill test
	  for (i=0; i<size; i++)
	      * ((t_uint8 *) pBufferMetaData->logAddress + i) = 0xff;
    }
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    //pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pBufferMetaData->phyaddress);
    pBufferMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pBufferMetaData->phyaddress);

    g_pStatsBufferStorage = (StatsBuffersStorage_t *) (pBufferMetaData->logAddress + 4 * Glace_uc.gridWidth * Glace_uc.gridHeight);
    g_pStatsBufferStorage->pGridR = (t_uint8 *) (pBufferMetaData->logAddress);
    g_pStatsBufferStorage->pGridG = (t_uint8 *) (pBufferMetaData->logAddress + Glace_uc.gridWidth * Glace_uc.gridHeight);
    g_pStatsBufferStorage->pGridB = (t_uint8 *) (pBufferMetaData->logAddress + Glace_uc.gridWidth * Glace_uc.gridHeight * 2);
    g_pStatsBufferStorage->pGridS = (t_uint8 *) (pBufferMetaData->logAddress + Glace_uc.gridWidth * Glace_uc.gridHeight * 3);


    g_pStatsBufferStorage->glaceStats.u32_TargetRedAddress 	= pBufferMetaData->ISPBufferBaseAddress
	    							  + (t_uint32) g_pStatsBufferStorage->pGridR - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->glaceStats.u32_TargetGreenAddress      = pBufferMetaData->ISPBufferBaseAddress
	    							  + (t_uint32) g_pStatsBufferStorage->pGridG - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->glaceStats.u32_TargetBlueAddress       = pBufferMetaData->ISPBufferBaseAddress
	    							  + (t_uint32) g_pStatsBufferStorage->pGridB - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->glaceStats.u32_TargetSaturationAddress = pBufferMetaData->ISPBufferBaseAddress
	    							  + (t_uint32) g_pStatsBufferStorage->pGridS - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->glaceStats.u32_HGridSize                = Glace_uc.gridWidth;
    g_pStatsBufferStorage->glaceStats.u32_VGridSize                = Glace_uc.gridHeight;

    g_pStatsBufferStorage->ispHistAddr_R = pBufferMetaData->ISPBufferBaseAddress + (t_uint32) g_pStatsBufferStorage->apHistR - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->ispHistAddr_G = pBufferMetaData->ISPBufferBaseAddress + (t_uint32) g_pStatsBufferStorage->apHistG - (t_uint32) pBufferMetaData->logAddress;
    g_pStatsBufferStorage->ispHistAddr_B = pBufferMetaData->ISPBufferBaseAddress + (t_uint32) g_pStatsBufferStorage->apHistB - (t_uint32) pBufferMetaData->logAddress;

    g_pStatsBufferStorage->ispGlaceStatsAddr = pBufferMetaData->ISPBufferBaseAddress + (t_uint32) &(g_pStatsBufferStorage->glaceStats) - (t_uint32) pBufferMetaData->logAddress;

    ITE_writePE(Glace_Control_ptrGlace_Statistics_Byte0, g_pStatsBufferStorage->ispGlaceStatsAddr);

    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_R);
    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_G);
    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_B);

    //Add FrameParamStatus management =
    pFrameParamStatus = (tps_frameParamStatus) (pBufferMetaData->logAddress + 4 * Glace_uc.gridWidth * Glace_uc.gridHeight + sizeof(StatsBuffersStorage_t));
    FrameParamStatusBufferAddr = pBufferMetaData->ISPBufferBaseAddress + (t_uint32) pFrameParamStatus - (t_uint32) pBufferMetaData->logAddress;

    ITE_writePE(FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte0, FrameParamStatusBufferAddr);
	LOS_Log("STATS BUFFER Logical Address = 0x%x, SIZE = %d\n", (unsigned int)pBufferMetaData->logAddress, (int)size);
	LOS_Log("STATS BUFFER Physical Address = 0x%x, SIZE = %d\n", (unsigned int)pBufferMetaData->ISPBufferBaseAddress,(int)size);	
	LOS_Log("STATS BUFFER SIZE = %d\n",(int)size_ts_frameParamStatus);


    //return(pStatsBufferStorage);

}



void ITE_NMF_createFocusStatsBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData, t_uint32 bufSize)
{
    int i;
    int size = bufSize;
    static OMX_ERRORTYPE err = OMX_ErrorNone;

    if (pFocusBufferMetaData->handle == 0) {

        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
        poolAttrs.iSize = size;     // Size (in byte) of a buffer
        poolAttrs.iAlignment = 64;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
            LOS_Log("ERROR - ITE_NMF_createFocusStatsBuffer: MMHwBuffer::Create FAILED\n");
            return;
        }

        //Buffer physical address
        MMHwBuffer::TBufferInfo bufferInfo;
        err = sharedChunk->BufferInfo(0, bufferInfo);
        if(err!=OMX_ErrorNone)
        {
            LOS_Log("sharedChunk->BufferInfo in ITE_NMF_freeStatsBuffer Failed.");
            return;
        }

        pFocusBufferMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pFocusBufferMetaData->handle = (OMX_U32) sharedChunk;
        pFocusBufferMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pFocusBufferMetaData->logAddress), 0xAA, (OMX_U32) size))
        {
                printf("memcpy failed\n");
			    pFocusBufferMetaData->size = 0;
        } 
		else
	    {
                printf("memcpy passed\n");
				pFocusBufferMetaData->size = size;
		}
        printf("LogicalAddress : 0x%x\n", pFocusBufferMetaData->logAddress);

        }
    else LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

    if(pFocusBufferMetaData->handle != 0)
	{	//init buffer: mandatory for buffer fill test
	  for (i=0; i<size; i++)
	      * ((t_uint8 *) pFocusBufferMetaData->logAddress + i) = 0xff;
	}
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    pFocusBufferMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pFocusBufferMetaData->phyaddress);

    //g_pFocusStatsBufferStorage = (FocusStatsBuffersStorage_t *) pFocusBufferMetaData->logAddress;
	 LOS_Log("FOCUS BUFFER Logical Address = 0x%x, SIZE = %d\n", pFocusBufferMetaData->logAddress,size);
	 LOS_Log("FOCUS BUFFER Physical Address = 0x%x, SIZE = %d\n", pFocusBufferMetaData->ISPBufferBaseAddress,size);

    //return(pFocusStatsBufferStorage);
}



void ITE_NMF_createNVMBuffer(tps_siaNVMbuffer pNVMBufferMetaData)
{
    t_uint32 i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;
	t_uint32 size;
	size =ITE_readPE(Sensor_Tuning_Status_u16_NVM_Data_Size_Byte0);

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
    //int size = Glace_uc.gridWidth * Glace_uc.gridHeight * 4 + sizeof(StatsBuffersStorage_t) + sizeof (ts_frameParamStatus);  // Configurable size for CNips3AAPI::TGrid

    if (pNVMBufferMetaData->handle == 0 && size != 0)
    {
    //    pBufferMetaData->handle = LOS_Alloc(size, 1024*1024*16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    //    pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);


        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
		poolAttrs.iSize = size;
        poolAttrs.iAlignment = 16;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
                printf("Not Enough Resources..\nMemory not allocated.\n");
                //return 0;
        }

        //Buffer physical address
        MMHwBuffer::TBufferInfo bufferInfo;
        err = sharedChunk->BufferInfo(0, bufferInfo);
        if(err!=OMX_ErrorNone)
        {
            LOS_Log("sharedChunk->BufferInfo in ITE_NMF_createNVMBuffer Failed.");
            return;
        }

        pNVMBufferMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pNVMBufferMetaData->handle = (OMX_U32) sharedChunk;
        pNVMBufferMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pNVMBufferMetaData->logAddress), 0xAA, (OMX_U32) size))
        {
                printf("memcpy failed\n");
				pNVMBufferMetaData->size = 0;
        } 
		else
	    {
                printf("memcpy passed\n");
				pNVMBufferMetaData->size = (t_uint32)bufferInfo.iAllocatedSize;
	    }

        LOS_Log("NVM: LogicalAddress : 0x%x\n",  pNVMBufferMetaData->logAddress);
		LOS_Log("NVM: PhysicalAddress : 0x%x\n", pNVMBufferMetaData->phyaddress);
		LOS_Log("NVM: Allcated Size : 0x%x\n",   pNVMBufferMetaData->size);


    }
    else 
    {
        LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");
        return;
    }

   
   if(pNVMBufferMetaData->handle != 0)
	   {   //init buffer: mandatory for buffer fill test
		 for (i=0; i<size; i++)
			 * ((t_uint8 *) pNVMBufferMetaData->logAddress + i) = 0xff;
	   }
	   else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");
   
    //pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pBufferMetaData->phyaddress);
    pNVMBufferMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pNVMBufferMetaData->phyaddress);

	g_pNVMStatsBufferStorage->ispNVMAddr = pNVMBufferMetaData->ISPBufferBaseAddress;

	LOS_Log("NVM BUFFER Logical Address = 0x%x, SIZE = %d\n", pNVMBufferMetaData->logAddress,size);

	//return(pNVMStatsBufferStorage);

}

void* ITE_NMF_create_Streaming_Config_Data_Buffer(tps_siaMetaDatabuffer pBufferMetaData)
{
    unsigned int* pBufferStorage;
    t_uint32 i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;
    t_uint32 size = ITE_readPE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
    //int size = Glace_uc.gridWidth * Glace_uc.gridHeight * 4 + sizeof(StatsBuffersStorage_t) + sizeof (ts_frameParamStatus);  // Configurable size for CNips3AAPI::TGrid

    if (pBufferMetaData->handle == 0)
    {
    //    pBufferMetaData->handle = LOS_Alloc(size, 1024*1024*16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    //    pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);


        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
		poolAttrs.iSize = size;
        poolAttrs.iAlignment = 16;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
                printf("Not Enough Resources..\nMemory not allocated.\n");
                return 0;
        }

        //Buffer physical address
        OMX_ERRORTYPE error;
        MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
        if(error!=OMX_ErrorNone)
        {
            LOS_Log("sharedChunk->BufferInfo in ITE_NMF_create_Streaming_Config_Data_Buffer Failed.");
             return 0;
        }

        pBufferMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pBufferMetaData->handle = (OMX_U32) sharedChunk;
        pBufferMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pBufferMetaData->logAddress), 0xFF, (OMX_U32) size))
        {
            printf("memcpy failed\n");
        }
		else
    	{
            printf("memcpy passed\n");
    	}
       LOS_Log("LogicalAddress : 0x%x\n", pBufferMetaData->logAddress);
	   LOS_Log("PhysicalAddress : 0x%x\n", pBufferMetaData->phyaddress);
    }
    else LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

   if(pBufferMetaData->handle != 0)
	   {   //init buffer: mandatory for buffer fill test
		 for (i=0; i<size; i++)
			 * ((t_uint8 *) pBufferMetaData->logAddress + i) = 0xff;
	   }
	   else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    //pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pBufferMetaData->phyaddress);
    pBufferMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pBufferMetaData->phyaddress);

	pBufferStorage = (unsigned int*) pBufferMetaData->ISPBufferBaseAddress;
	 LOS_Log("Streaming_Config_Data BUFFER Logical Address = 0x%x, SIZE = %d\n", pBufferMetaData->logAddress,size);
	 LOS_Log("Streaming_Config_Data BUFFER Physical Address = 0x%x, SIZE = %d\n", pBufferMetaData->ISPBufferBaseAddress,size);
	return((void*)pBufferStorage);

}

void* ITE_NMF_create_SENSOR_TUNNING_Buffer(tps_siasensortunningbuffer pBuffersensortunning)
{
    unsigned int* pBufferStorage;
    t_uint32 i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;

    t_uint32 size= 4096;

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
    //int size = Glace_uc.gridWidth * Glace_uc.gridHeight * 4 + sizeof(StatsBuffersStorage_t) + sizeof (ts_frameParamStatus);  // Configurable size for CNips3AAPI::TGrid

    if (pBuffersensortunning->handle == 0)
    {
        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
        poolAttrs.iSize = size;
        poolAttrs.iAlignment = 16;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
                LOS_Log("Not Enough Resources..\nMemory not allocated.\n");
                return 0;
        }

        //Buffer physical address
        OMX_ERRORTYPE error = OMX_ErrorNone;
        MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
        error = sharedChunk->BufferInfo(0, bufferInfo);
        if(error!=OMX_ErrorNone)
        {
                LOS_Log("Not Enough Resources..\nBuffer physical address Memory not allocated.\n");
                return 0;
        }


        pBuffersensortunning->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pBuffersensortunning->handle = (OMX_U32) sharedChunk;
        pBuffersensortunning->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pBuffersensortunning->logAddress), 0xFF, (OMX_U32) size))
        {
            LOS_Log("memcpy failed\n");
        }
        else
        {
            LOS_Log("memcpy passed\n");
        }
        LOS_Log("LogicalAddress : 0x%x\n", pBuffersensortunning->logAddress);
        LOS_Log("PhysicalAddress : 0x%x\n", pBuffersensortunning->phyaddress);
    }
    else LOS_Log("WARNING pBuffersensortunning already exist!!!!!!\n");

    if(pBuffersensortunning->handle != 0)
    {
         // Init buffer: mandatory for buffer fill test
         for (i=0; i<size; i++)
         * ((t_uint8 *) pBuffersensortunning->logAddress + i) = 0xff;
    }
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    pBuffersensortunning->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pBuffersensortunning->phyaddress);
    pBuffersensortunning->size = size;

    pBufferStorage = (unsigned int*) pBuffersensortunning->ISPBufferBaseAddress;
    LOS_Log("SENSOR_TUNNING BUFFER Logical Address = 0x%x, SIZE = %d\n", pBuffersensortunning->logAddress,size);
    LOS_Log("SENSOR_TUNNING BUFFER Physical Address = 0x%x, SIZE = %d\n", pBuffersensortunning->ISPBufferBaseAddress,size);
    return((void*)pBufferStorage);

}

void ITE_NMF_free_SENSOR_TUNNING_Buffer(tps_siasensortunningbuffer pBuffersensortunning)
{
    static OMX_ERRORTYPE err = OMX_ErrorNone;
    if (pBuffersensortunning->handle != 0)
    {
        err = MMHwBuffer::Destroy((MMHwBuffer *&)pBuffersensortunning->handle);
        if (err != OMX_ErrorNone)
        {
            LOS_Log("\n ITE_NMF_free_SENSOR_TUNNING_Buffer MMMHwBuffer::Destroy %d", err);
        }
        pBuffersensortunning->handle = 0;
        pBuffersensortunning->phyaddress = 0;
        pBuffersensortunning->logAddress = 0;
        pBuffersensortunning->ISPBufferBaseAddress = 0;
        pBuffersensortunning->size = 0;
    }
}


StatsBuffersStorage_t* ITE_NMF_createTraceBuffer(tps_siaMetaDatabuffer pBufferMetaData)
{
    //ER 427679, if trace mechanism is not memory dump, no need to allocate buffer.
    if(TraceMsgOutput_e_MEMORY_DUMP == g_TraceProp.traceMechanism)
    {

    StatsBuffersStorage_t* pStatsBufferStorage = NULL;
    //t_uint32 FrameParamStatusBufferAddr=0;
    int i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
//    int size = (1024 * 1024) + 16;  // Configurable size for CNips3AAPI::TGrid
        int size = CAM_SHARED_MEM_TRACE_BUFFER_SIZE;  // Configurable size for CNips3AAPI::TGrid

    if (pBufferMetaData->handle == 0)
    {
    //    pBufferMetaData->handle = LOS_Alloc(size, 1024*1024*16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    //    pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);


        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
        poolAttrs.iSize = size;     // Size (in byte) of a buffer
        poolAttrs.iAlignment = 64;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
                printf("Not Enough Resources..\nMemory not allocated.\n");
                return 0;
        }

        //Buffer physical address
        OMX_ERRORTYPE error = OMX_ErrorNone;
        MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
        if(error!=OMX_ErrorNone)
        {
                printf("Not Enough Resources..\nBuffer physical address Memory not allocated.\n");
                return 0;
        }

        pBufferMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pBufferMetaData->handle = (OMX_U32) sharedChunk;
        pBufferMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pBufferMetaData->logAddress), 0xAA, (OMX_U32) size))
        {
                printf("memcpy failed\n");
        } else
                printf("memcpy passed\n");

        printf("LogicalAddress : 0x%x\n", pBufferMetaData->logAddress);




    }
    else LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

    if(pBufferMetaData->handle != 0)
    {	//init buffer: mandatory for buffer fill test
	  for (i=0; i<size; i++)
	      * ((t_uint8 *) pBufferMetaData->logAddress + i) = 0x0;
}
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    //pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pBufferMetaData->phyaddress);
    pBufferMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pBufferMetaData->phyaddress);
////////////////////////////////////////////////////////////////////////////////
		printf("Trace buffer allocation");
		printf("pBufferMetaData->phyaddress : 0x%x\n", (unsigned int)pBufferMetaData->phyaddress);
	    printf("pBufferMetaData->logAddress : 0x%x\n", (unsigned int)pBufferMetaData->logAddress);
	    printf("pBufferMetaData->ISPBufferBaseAddress : 0x%x\n", (unsigned int)pBufferMetaData->ISPBufferBaseAddress);

///////////////////////////////////////////////////////////////////////////////
	ITE_writePE(TraceLogsControl_u32_BufferAddr_Byte0, pBufferMetaData->ISPBufferBaseAddress);//ajay
	ITE_writePE(TraceLogsControl_u32_BufferSize_Byte0, size);//ajay
        ITE_writePE(TraceLogsControl_u32_LogLevels_Byte0, g_TraceProp.traceLogLevel);//ajay
        ITE_writePE(TraceLogsControl_u8_LogEnable_Byte0, g_TraceProp.traceEnable);//ajay
        ITE_writePE(TraceLogsControl_u8_TraceMechanismSelect_Byte0, g_TraceProp.traceMechanism);//ajay

        if(g_TraceProp.traceEnable && MMIO_Camera::setTraceBuffer(pBufferMetaData->logAddress, CAM_SHARED_MEM_TRACE_BUFFER_SIZE) == 0)
        {
            LOS_Log("ISP FW traces (memory dumps) enabled.\n");
        }
        else
        {
            LOS_Log("ISP FW traces (memory dumps) disabled.\n");
        }


    return(pStatsBufferStorage);
   }

	return 0;
}





void ITE_NMF_createGammaBuffer(tps_siaGammaLUTbuffer pGammaLUTMetaData)
{
    t_uint32 i;
    static OMX_ERRORTYPE err = OMX_ErrorNone;
    t_uint32 size = NUMBER_OF_LUT_TABLES * GAMMA_LUT_TABLE_SIZE;

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).

    if (pGammaLUTMetaData->handle == 0)
    {
        //    pGammaLUTMetaData->handle = LOS_Alloc(size, 1024*1024*16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
        //    pGammaLUTMetaData->logAddress = LOS_GetLogicalAddress(pGammaLUTMetaData->handle);


        MMHwBuffer *sharedChunk=0;
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        OMX_BOOL isCached=OMX_FALSE;
        //mel poolAttrs.iBuffers = 0;                           // Number of buffers in the pool
        poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;
        poolAttrs.iSize = size;     // Size (in byte) of a buffer
        poolAttrs.iAlignment = 64;                             // Alignment applied to the base address of each buffer in the pool
                                                               //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)

        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        sharedChunk = (MMHwBuffer *)NULL;
        //Buffer creation
        err = MMHwBuffer::Create(poolAttrs,NULL,sharedChunk);

        if(err!=OMX_ErrorNone)
        {
            LOS_Log("ERROR - ITE_NMF_createGammaBuffer: MMHwBuffer::Create FAILED\n");
            return;
        }

        //Buffer physical address
        OMX_ERRORTYPE error = OMX_ErrorNone;;
        MMHwBuffer::TBufferInfo bufferInfo;
        error = sharedChunk->BufferInfo(0, bufferInfo);
        //__NO_WARNING__
        if(error!=OMX_ErrorNone)
        {
            LOS_Log("ERROR - ITE_NMF_createGammaBuffer: MMHwBuffer::TBufferInfo FAILED\n");
            return;
        }

        pGammaLUTMetaData->phyaddress = (t_uint32)bufferInfo.iPhyAddr;
        pGammaLUTMetaData->handle = (OMX_U32) sharedChunk;
        pGammaLUTMetaData->logAddress = (t_los_logical_address)bufferInfo.iLogAddr;

        if(NULL == memset ((void *) (pGammaLUTMetaData->logAddress), 0xAA, (OMX_U32) size))
        {
                printf("memcpy failed\n");
                pGammaLUTMetaData->size = 0;
        }
        else
        {
            printf("memcpy passed\n");
            pGammaLUTMetaData->size = size;
        }

        printf("LogicalAddress : 0x%x\n", pGammaLUTMetaData->logAddress);

    }


    else
        LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

    if(pGammaLUTMetaData->handle != 0)
    {   //init buffer: mandatory for buffer fill test
      for (i=0; i<size; i++)
          * ((t_uint8 *) pGammaLUTMetaData->logAddress + i) = 0xff;
    }
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    //pGammaLUTMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pGammaLUTMetaData->phyaddress);
    pGammaLUTMetaData->ISPBufferBaseAddress = MMIO_Camera::mapStatisticsMemArea(pGammaLUTMetaData->phyaddress);

    LOS_Log("GAMMA BUFFER Logical Address = 0x%x, SIZE = %d\n", pGammaLUTMetaData->logAddress,size);
    LOS_Log("GAMMA BUFFER Physical Address = 0x%x, SIZE = %d\n", pGammaLUTMetaData->ISPBufferBaseAddress,size);

    ITE_writePE(CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 0)));
    ITE_writePE(CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 1)));
    ITE_writePE(CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 2)));
    ITE_writePE(CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 3)));
    ITE_writePE(CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 4)));
    ITE_writePE(CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 5)));

    ITE_writePE(CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 6)));
    ITE_writePE(CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 7)));
    ITE_writePE(CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 8)));
    ITE_writePE(CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 9)));
    ITE_writePE(CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 10)));
    ITE_writePE(CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte0, (t_uint32) (pGammaLUTMetaData->ISPBufferBaseAddress + (GAMMA_LUT_TABLE_SIZE * 11)));

    LOS_Log("\n\nITE_NMF_createGammaBuffer 8\n\n");
}





void ITE_NMF_freeStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData)
{
static OMX_ERRORTYPE err = OMX_ErrorNone;
if (pBufferMetaData->handle != 0) {
    err = MMHwBuffer::Destroy((MMHwBuffer *&)pBufferMetaData->handle);

    if(err!=OMX_ErrorNone)
    {
        printf("MMHwBuffer::Destroy Failed.\n");
        return;
    }

    pBufferMetaData->handle = 0;
    pBufferMetaData->phyaddress = 0;
    pBufferMetaData->logAddress = 0;
    pBufferMetaData->ISPBufferBaseAddress = 0;
    pBufferMetaData->size = 0;
    }
}

void ITE_NMF_freeFocusBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData)
{
static OMX_ERRORTYPE err = OMX_ErrorNone;
if (pFocusBufferMetaData->handle != 0) {
    err = MMHwBuffer::Destroy((MMHwBuffer *&)pFocusBufferMetaData->handle);

    if(err!=OMX_ErrorNone)
    {
        printf("MMHwBuffer::Destroy Failed.\n");
        return;
    }

    pFocusBufferMetaData->handle = 0;
    pFocusBufferMetaData->phyaddress = 0;
    pFocusBufferMetaData->logAddress = 0;
    pFocusBufferMetaData->ISPBufferBaseAddress = 0;
	pFocusBufferMetaData->size = 0;
    }
}


void ITE_NMF_freeGammaBuffer(tps_siaGammaLUTbuffer pGammaLUTMetaData)
{
    static OMX_ERRORTYPE err = OMX_ErrorNone;
    if (pGammaLUTMetaData->handle != 0)
    {
        err = MMHwBuffer::Destroy((MMHwBuffer *&)pGammaLUTMetaData->handle);

        if(err!=OMX_ErrorNone)
        {
            printf("MMHwBuffer::Destroy Failed.\n");
            return;
        }

        pGammaLUTMetaData->handle = 0;
        pGammaLUTMetaData->phyaddress = 0;
        pGammaLUTMetaData->logAddress = 0;
        pGammaLUTMetaData->ISPBufferBaseAddress = 0;
        pGammaLUTMetaData->size = 0;
    }
}



void ITE_NMF_freeNVMBuffer(tps_siaNVMbuffer pNVMBufferMetaData)
{

static OMX_ERRORTYPE err = OMX_ErrorNone;
if (pNVMBufferMetaData->handle != 0) {
    err = MMHwBuffer::Destroy((MMHwBuffer *&)pNVMBufferMetaData->handle);

    if(err!=OMX_ErrorNone)
    {
        printf("MMHwBuffer::Destroy Failed.\n");
        return;
    }

    pNVMBufferMetaData->handle = 0;
    pNVMBufferMetaData->phyaddress = 0;
    pNVMBufferMetaData->logAddress = 0;
    pNVMBufferMetaData->ISPBufferBaseAddress = 0;
	pNVMBufferMetaData->size = 0 ;
    }

}

void ITE_NMF_freeSensorOutputModeBuffer(tps_siaMetaDatabuffer pBufferMetaData)
{
static OMX_ERRORTYPE err = OMX_ErrorNone;
if (pBufferMetaData->handle != 0) {
    err = MMHwBuffer::Destroy((MMHwBuffer *&)pBufferMetaData->handle);

    if(err!=OMX_ErrorNone)
    {
        printf("MMHwBuffer::Destroy Failed.\n");
        return;
    }

    pBufferMetaData->handle = 0;
    pBufferMetaData->phyaddress = 0;
    pBufferMetaData->logAddress = 0;
    pBufferMetaData->ISPBufferBaseAddress = 0;
	pBufferMetaData->size = 0;
    }
}

void ITE_NMF_freeFocusStatsBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData)
{
ITE_NMF_freeFocusBuffer(pFocusBufferMetaData);
}



void ITE_NMF_ResetNVMBuffer(tps_siaNVMbuffer pNVMBufferMetaData)
{
				  
	if (pNVMBufferMetaData->handle != 0) 
    {
	    pNVMBufferMetaData->handle = 0;
	    pNVMBufferMetaData->phyaddress = 0;
	    pNVMBufferMetaData->logAddress = 0;
	    pNVMBufferMetaData->ISPBufferBaseAddress = 0;
		pNVMBufferMetaData->size = 0 ;
    }

 }

 void ITE_NMF_freeTraceBuffer(tps_siaMetaDatabuffer pBufferMetaData)
 {
     //ER 427679, if trace mechanism is not memory dump, no need to free buffer.
     if(g_TraceProp.traceEnable && (TraceMsgOutput_e_MEMORY_DUMP == g_TraceProp.traceMechanism))
    {
        //Need to reset the trace buffer to finish the trace thread responsible for dumping FW traces
        OMX_ERRORTYPE ret_val = MMIO_Camera::resetTraceBuffer();
        if ( ret_val != OMX_ErrorNone) {
            printf("Problem resetting trace buffer (err=%d)\n", ret_val);
        }

 static OMX_ERRORTYPE err = OMX_ErrorNone;
 if (pBufferMetaData->handle != 0) {
    err = MMHwBuffer::Destroy((MMHwBuffer *&)pBufferMetaData->handle);

    if(err!=OMX_ErrorNone)
    {
        printf("MMHwBuffer::Destroy Failed.\n");
        return;
    }

 	if (pBufferMetaData->handle != 0) 
	{
		pBufferMetaData->handle = 0;
	    pBufferMetaData->phyaddress = 0;
	    pBufferMetaData->logAddress = 0;
	    pBufferMetaData->ISPBufferBaseAddress = 0;
		pBufferMetaData->size = 0 ;
	}
  }
 	}
}
