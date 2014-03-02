/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*
* \file
* \brief   <PURPOSE OF THIS FILE>
* \author  ST-Ericsson
 */
/*****************************************************************************/
#define _FAKESOURCE_C_
#undef OMXCOMPONENT
#define OMXCOMPONENT "FAKE_SOURCE"
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x30

#include "fakesource.h"
#include "OMX_Index.h"
#include "IFM_Index.h"
#include <string.h>

#ifdef __SYMBIAN32__
	#include <openmax/il/shai/OMX_Symbian_IVCommonExt.h>
	#include <openmax/il/shai/OMX_Symbian_CameraExt.h>
#else
	#include <OMX_Symbian_CameraExt_Ste.h>
#endif

extern "C"
{
#include "osi_trace.h"
#include <los/api/los_api.h>
}


typedef struct
{
	int         ColorFormat;
	float       OverallPixelDepth;
	int         NbPlan;
	float       PixelDepth0;
	float       PixelDepth1;
	float       PixelDepth2;
	const char *Name;
	const char *Description;
} _sFakeSourceImageFormatDescription;

const _sFakeSourceImageFormatDescription FakeSourceColorFormatInfo[]=
{ 
	{ OMX_COLOR_FormatUnused                           , 0.  , 0, 0.  ,  0.,  0., "OMX_COLOR_FormatUnused"                          , "Format unused, pixelDepth = 0. This should not occur !"},
	{ OMX_COLOR_FormatRawBayer8bit                     , 1.0 , 1, 1.0 , 0.0, 0.0, "OMX_COLOR_FormatRawBayer8bit"                    , "RawBayer 8bits"},
	{ OMX_COLOR_Format16bitRGB565                      , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_Format16bitRGB565"                     , "RGB565"},
	{ OMX_COLOR_Format16bitBGR565                      , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_Format16bitBGR565"                     , "BGR565"},
	{ OMX_COLOR_FormatYUV422PackedPlanar               , 2.0 , 3, 1.0 , 0.5, 0.5, "OMX_COLOR_FormatYUV422PackedPlanar"              , "YUV422PackedPlanar"},
	{ OMX_COLOR_FormatYCbYCr                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYCbYCr"                          , "YCbYCr interlaced"},
	{ OMX_COLOR_FormatYCrYCb                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYCrYCb"                          , "YCrYCb interlaced"},
	{ OMX_COLOR_FormatCbYCrY                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatCbYCrY"                          , "CbYCrY interlaced"},
	{ OMX_COLOR_FormatCrYCbY                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatCrYCbY"                          , "CrYCbY interlaced"},
	{ OMX_COLOR_Format24bitRGB888                      , 3.0 , 1, 3.0 , 0.0, 0.0, "OMX_COLOR_Format24bitRGB888"                     , "RGB888 interlaced"},
	{ OMX_COLOR_Format24bitBGR888                      , 3.0 , 1, 3.0 , 0.0, 0.0, "OMX_COLOR_Format24bitBGR888"                     , "BGR888 interlaced"},
	{ OMX_COLOR_FormatYUV420PackedPlanar               , 1.5 , 3, 1.0 , .25, .25, "OMX_COLOR_FormatYUV420PackedPlanar"              , "YUV420MB old name for 420mb"},
	{ OMX_COLOR_FormatYUV420SemiPlanar                 , 1.5 , 2, 1.0 , 0.5, 0.0, "OMX_COLOR_FormatYUV420SemiPlanar"                , "NV21"},
	{ OMX_COLOR_FormatYUV422SemiPlanar                 , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYUV422SemiPlanar"                , "mapped to STE 422mb"},
	{ OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar , 1.5 , 1, 1.5 , 0.0, 0.0, "OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar", "STE 420MB"},
	{ OMX_SYMBIAN_COLOR_FormatRawBayer12bit            , 1.5 , 1, 1.5 , 0.0, 0.0, "OMX_SYMBIAN_COLOR_FormatRawBayer12bit"           , "RawBayer 12bits"},
	{ OMX_COLOR_Format32bitBGRA8888                    , 4.0 , 1, 4.0 , 0.0, 0.0, "OMX_COLOR_Format32bitBGRA8888"                   , "BGRA8888 32 bits"},
	{ OMX_COLOR_Format32bitARGB8888                    , 4.0 , 1, 4.0 , 0.0, 0.0, "OMX_COLOR_Format32bitARGB8888"                   , "ARGB8888 32 bits"},
	{ OMX_COLOR_FormatRawBayer10bit                    , 1.25, 1, 1.25,  0.,  0., "OMX_COLOR_FormatRawBayer10bit"                   , "RawBayer 10bits"},
	{ OMX_COLOR_Format12bitRGB444                      , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_Format12bitRGB444"                     , "12bits RGB444"},
	{ OMX_COLOR_Format16bitARGB4444                    , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_Format16bitARGB4444"                   , "16bits ARGB4444"},
	{ OMX_COLOR_Format16bitARGB1555                    , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_Format16bitARGB1555"                   , "16bits ARGB1555"},
	{ OMX_COLOR_FormatMonochrome                       , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_FormatMonochrome"                      , "Monochrome"},
	{ OMX_COLOR_Format8bitRGB332                       , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_Format8bitRGB332"                      , "8bit RGB332"},
	{ OMX_COLOR_Format18bitRGB666                      ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18bitRGB666"                     , "18bits RGB666"  },
	{ OMX_COLOR_Format18bitARGB1665                    ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18bitARGB1665"                   , "18bits ARGB1665"},
	{ OMX_COLOR_Format19bitARGB1666                    ,2.375, 1,2.375,  0.,  0., "OMX_COLOR_Format19bitARGB1666"                   , "19bits ARGB1666"},
	{ OMX_COLOR_Format24bitARGB1887                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24bitARGB1887"                   , "24bits ARGB1887"},
	{ OMX_COLOR_Format18BitBGR666                      ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18BitBGR666"                     , "18Bits BGR666"  },
	{ OMX_COLOR_Format24BitARGB6666                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24BitARGB6666"                   , "24Bits ARGB6666"},
	{ OMX_COLOR_Format24BitABGR6666                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24BitABGR6666"                   , "24Bits ABGR6666"},
	{ OMX_COLOR_Format25bitARGB1888                    ,3.125, 1,3.125,  0.,  0., "OMX_COLOR_Format25bitARGB1888"                   , "25bits ARGB1888"},
	{ OMX_COLOR_FormatRawBayer8bitcompressed           ,  1. , 1,  1. ,  0.,  0., "OMX_COLOR_FormatRawBayer8bitcompressed"          , "RawBayer 8bits compressed"},
/*
	{ OMX_COLOR_FormatYUV422Planar                     ,  2. , 1,  2. ,  0.,  0., "OMX_COLOR_FormatYUV422Planar"                    , ""},
	{ OMX_COLOR_FormatYUV422PackedSemiPlanar           ,  2. , 1,  2. ,  0.,  0., "OMX_COLOR_FormatYUV422PackedSemiPlanar"          , ""},
	{ OMX_COLOR_FormatYUV411Planar                     ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV411Planar"                    , ""},
	{ OMX_COLOR_FormatYUV411PackedPlanar               ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV411PackedPlanar"              , ""},
	{ OMX_COLOR_FormatYUV444Interleaved                ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV444Interleaved"               , ""},
	{ OMX_COLOR_FormatYUV420PackedSemiPlanar           , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_FormatYUV420PackedSemiPlanar"          , ""},
	{ OMX_COLOR_FormatYUV420Planar                     , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_FormatYUV420Planar"                    , ""},
*/
	{ OMX_COLOR_FormatL2                               , 0.25, 1, 0.25,  0.,  0., "OMX_COLOR_FormatL2"                              , ""},
	{ OMX_COLOR_FormatL4                               , 0.5 , 1, 0.5 ,  0.,  0., "OMX_COLOR_FormatL4"                              , ""},
	{ OMX_COLOR_FormatL8                               , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_FormatL8"                              , ""},
	{ OMX_COLOR_FormatL16                              , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_FormatL16"                             , ""},
	{ OMX_COLOR_FormatL24                              , 3.  , 1, 3.  ,  0.,  0., "OMX_COLOR_FormatL24"                             , ""},
	{ OMX_COLOR_FormatL32                              , 4.  , 1, 4.  ,  0.,  0., "OMX_COLOR_FormatL32"                             , ""},
};

const _sFakeSourceImageFormatDescription* FakeSourceGetImageFormatDescription(int format)
{ // Return the struct associated to the given format
	const _sFakeSourceImageFormatDescription *ptr=FakeSourceColorFormatInfo;
	size_t Nbr=sizeof(FakeSourceColorFormatInfo)/sizeof(*FakeSourceColorFormatInfo);
	while(Nbr > 0)
	{
		if (ptr->ColorFormat ==format)
			return(ptr);
		++ptr;
		--Nbr;
	}
	return(_sFakeSourceImageFormatDescription*)NULL;
}


void FakeSource__print_color_format(t_uint32 format) 
{
	LOS_Log("%s format = %d\n", __FUNCTION__, format);
	switch(format)
	{
		case OMX_COLOR_FormatUnused :
			LOS_Log("%s format OMX_COLOR_FormatUnused\n", __FUNCTION__);break;
		case OMX_COLOR_FormatMonochrome :
			LOS_Log("%s format OMX_COLOR_FormatMonochrome\n", __FUNCTION__);break;
		case OMX_COLOR_Format8bitRGB332 :
			LOS_Log("%s format OMX_COLOR_Format8bitRGB332\n", __FUNCTION__);break;
		case OMX_COLOR_FormatRawBayer8bit :
			LOS_Log("%s format OMX_COLOR_FormatRawBayer8bit\n", __FUNCTION__);break;
		case OMX_COLOR_Format12bitRGB444 :
			LOS_Log("%s format OMX_COLOR_Format12bitRGB444\n", __FUNCTION__);break;
		case OMX_COLOR_Format16bitARGB4444 :
			LOS_Log("%s format OMX_COLOR_Format16bitARGB4444\n", __FUNCTION__);break;
		case OMX_COLOR_Format16bitARGB1555 :
			LOS_Log("%s format OMX_COLOR_Format16bitARGB1555\n", __FUNCTION__);break;
		case OMX_COLOR_Format16bitRGB565 :
			LOS_Log("%s format OMX_COLOR_Format16bitRGB565\n", __FUNCTION__);break;
		case OMX_COLOR_Format16bitBGR565 :
			LOS_Log("%s format OMX_COLOR_Format16bitBGR565\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYCbYCr :
			LOS_Log("%s format OMX_COLOR_FormatYCbYCr\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYCrYCb :
			LOS_Log("%s format OMX_COLOR_FormatYCrYCb\n", __FUNCTION__);break;
		case OMX_COLOR_FormatCbYCrY :
			LOS_Log("%s format OMX_COLOR_FormatCbYCrY\n", __FUNCTION__);break;
		case OMX_COLOR_FormatCrYCbY :
			LOS_Log("%s format OMX_COLOR_FormatCrYCbY\n", __FUNCTION__);break;
		case OMX_COLOR_Format18bitRGB666 :
			LOS_Log("%s format OMX_COLOR_Format18bitRGB666\n", __FUNCTION__);break;
		case OMX_COLOR_Format18bitARGB1665 :
			LOS_Log("%s format OMX_COLOR_Format18bitARGB1665\n", __FUNCTION__);break;
		case OMX_COLOR_Format19bitARGB1666 :
			LOS_Log("%s format OMX_COLOR_Format19bitARGB1666\n", __FUNCTION__);break;
		case OMX_COLOR_Format24bitRGB888 :
			LOS_Log("%s format OMX_COLOR_Format24bitRGB888\n", __FUNCTION__);break;
		case OMX_COLOR_Format24bitBGR888 :
			LOS_Log("%s format OMX_COLOR_Format24bitBGR888\n", __FUNCTION__);break;
		case OMX_COLOR_Format24bitARGB1887 :
			LOS_Log("%s format OMX_COLOR_Format24bitARGB1887\n", __FUNCTION__);break;
		case OMX_COLOR_Format25bitARGB1888 :
			LOS_Log("%s format OMX_COLOR_Format25bitARGB1888\n", __FUNCTION__);break;
		case OMX_COLOR_Format32bitBGRA8888 :
			LOS_Log("%s format OMX_COLOR_Format32bitBGRA8888\n", __FUNCTION__);break;
		case OMX_COLOR_Format32bitARGB8888 :
			LOS_Log("%s format OMX_COLOR_Format32bitARGB8888\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV411Planar :
			LOS_Log("%s format OMX_COLOR_FormatYUV411Planar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV411PackedPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV411PackedPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV422Planar :
			LOS_Log("%s format OMX_COLOR_FormatYUV422Planar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV422PackedPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV422PackedPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV422SemiPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV422SemiPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV422PackedSemiPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV422PackedSemiPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV444Interleaved :
			LOS_Log("%s format OMX_COLOR_FormatYUV444Interleaved\n", __FUNCTION__);break;
		case OMX_SYMBIAN_COLOR_FormatRawBayer12bit :
			LOS_Log("%s format OMX_SYMBIAN_COLOR_FormatRawBayer12bit\n", __FUNCTION__);break;
		case OMX_COLOR_FormatRawBayer8bitcompressed :
			LOS_Log("%s format OMX_COLOR_FormatRawBayer8bitcompressed\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV420PackedSemiPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV420PackedSemiPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV420Planar :
			LOS_Log("%s format OMX_COLOR_FormatYUV420Planar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatRawBayer10bit :
			LOS_Log("%s format OMX_COLOR_FormatRawBayer10bit\n", __FUNCTION__);break;
		case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
			LOS_Log("%s format OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV420PackedPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV420PackedPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_FormatYUV420SemiPlanar :
			LOS_Log("%s format OMX_COLOR_FormatYUV420SemiPlanar\n", __FUNCTION__);break;
		case OMX_COLOR_Format18BitBGR666 :
			LOS_Log("%s format OMX_COLOR_Format18BitBGR666\n", __FUNCTION__);break;
		case OMX_COLOR_Format24BitARGB6666 :
			LOS_Log("%s format OMX_COLOR_Format24BitARGB6666\n", __FUNCTION__);break;
		case OMX_COLOR_Format24BitABGR6666 :
			LOS_Log("%s format OMX_COLOR_Format24BitABGR6666\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL2 :
			LOS_Log("%s format OMX_COLOR_FormatL2\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL4 :
			LOS_Log("%s format OMX_COLOR_FormatL4\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL8 :
			LOS_Log("%s format OMX_COLOR_FormatL8\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL16 :
			LOS_Log("%s format OMX_COLOR_FormatL16\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL24 :
			LOS_Log("%s format OMX_COLOR_FormatL24\n", __FUNCTION__);break;
		case OMX_COLOR_FormatL32 :
			LOS_Log("%s format OMX_COLOR_FormatL32\n", __FUNCTION__);break;
		default :
			LOS_Log("%s format default : %d\n", __FUNCTION__, format);break;
	}
}


void FakeSource__print_state(t_uint32 state) 
{
	LOS_Log("%s state = %d\n", __FUNCTION__, state);
	switch(state)
	{
		case OMX_StateInvalid:
			LOS_Log("\t %s  tracing state: OMX_StateInvalid\n", __FUNCTION__);
			break;
		case OMX_StateLoaded:
			LOS_Log("\t %s  tracing state: OMX_StateLoaded\n", __FUNCTION__);
			break;
		case OMX_StateIdle:
			LOS_Log("\t %s  tracing state: OMX_StateIdle\n", __FUNCTION__);
			break;
		case OMX_StateExecuting:
			LOS_Log("\t %s  tracing state: OMX_StateExecuting\n", __FUNCTION__);
			break;
		case OMX_StatePause:
			LOS_Log("\t %s  tracing state: OMX_StatePause\n", __FUNCTION__);
			break;
		case OMX_StateTransientToIdle:
			LOS_Log("\t %s  tracing state: OMX_StateTransientToIdle\n", __FUNCTION__);
			break;
		case OMX_StateWaitForResources:
			LOS_Log("\t %s  tracing state: OMX_StateWaitForResources\n", __FUNCTION__);
			break;
		case OMX_StateTransient:
			LOS_Log("\t %s  tracing state: OMX_StateTransient\n", __FUNCTION__);
			break;
		case OMX_StateLoadedToIdleHOST:
			LOS_Log("\t %s  tracing state: OMX_StateLoadedToIdleHOST\n", __FUNCTION__);
			break;
		case OMX_StateLoadedToIdleDSP:
			LOS_Log("\t %s  tracing state: OMX_StateLoadedToIdleDSP\n", __FUNCTION__);
			break;
		case OMX_StateIdleToLoaded:
			LOS_Log("\t %s  tracing state: OMX_StateIdleToLoaded\n", __FUNCTION__);
			break;
		case OMX_StateIdleToExecuting:
			LOS_Log("\t %s  tracing state: OMX_StateIdleToExecuting\n", __FUNCTION__);
			break;
		case OMX_StateIdleToPause:
			LOS_Log("\t %s  tracing state: OMX_StateIdleToPause\n", __FUNCTION__);
			break;
		case OMX_StateExecutingToIdle:
			LOS_Log("\t %s  tracing state: OMX_StateExecutingToIdle\n", __FUNCTION__);
			break;
		case OMX_StateExecutingToPause:
			LOS_Log("\t %s  tracing state: OMX_StateExecutingToPause\n", __FUNCTION__);
			break;
		case OMX_StatePauseToIdle:
			LOS_Log("\t %s  tracing state: OMX_StatePauseToIdle\n", __FUNCTION__);
			break;
		case OMX_StatePauseToExecuting:
			LOS_Log("\t %s  tracing state: OMX_StatePauseToExecuting\n", __FUNCTION__);
			break;
		default:
			LOS_Log("\t %s  tracing state: OMX_State ... unknown = %d\n", state);
			break;
	}
}

OMX_ERRORTYPE fakesourceFactoryMethod(ENS_Component_p * ppENSComponent) 
{
    IN0("");
    *ppENSComponent = (ENS_Component_p)NULL;
    fakesource * fakSource = new fakesource();
    if (fakSource == 0) 
    {
        OUTR("",(OMX_ErrorInsufficientResources));
        return OMX_ErrorInsufficientResources;
    }
    *ppENSComponent = fakSource;
    FakeSourceProcessingComp * source = new FakeSourceProcessingComp(*fakSource);
	if (source == 0) 
	{
		OUTR("",(OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	(*ppENSComponent)->setProcessingComponent((ProcessingComponent*)source);
	
	OUTR("",(OMX_ErrorNone));
	return OMX_ErrorNone;
} 

fakesource::fakesource(void)
{
    IN0("");
	ENS_Component::construct(1);
	memory_handle_iParam_CONTENTURITYPE_data_extended = 0;
	iParam_CONTENTURITYPE_data = NULL;
	memory_handle_iParam_CONTENTURITYPE_extradata_extended = 0;
	iParam_CONTENTURITYPE_extradata = NULL;
    createPort(0, OMX_DirOutput, OMX_BufferSupplyOutput, 1);    
    //createPort(0, OMX_DirOutput, OMX_BufferSupplyInput, 1);
	//createPort(0, OMX_DirOutput, OMX_BufferSupplyUnspecified, 1);

    OMX_PARAM_PORTDEFINITIONTYPE defaultParam;
	defaultParam.eDomain = OMX_PortDomainVideo;
    defaultParam.nPortIndex = 0;
	defaultParam.nBufferSize = 0;
    defaultParam.format.video.nFrameWidth = fakesourceport::DEFAULT_WIDTH;
    defaultParam.format.video.nFrameHeight =fakesourceport::DEFAULT_HEIGHT;
    defaultParam.format.video.nStride =0;
    defaultParam.format.video.nSliceHeight =0;
    defaultParam.format.video.eColorFormat=fakesourceport::DEFAULT_COLOR_FORMAT;
    defaultParam.format.video.xFramerate=fakesourceport::DEFAULT_FRAMERATE;
    fakesourceport* port = (fakesourceport*)getPort(0);
    port->setFormatInPortDefinition(defaultParam);

	iConfig_FRAMERATETYPE.xEncodeFramerate	= 0x20000;
	iConfig_CAPTUREMODETYPE.bContinuous		= OMX_FALSE;
	iConfig_CAPTUREMODETYPE.bFrameLimited	= OMX_FALSE;
	iConfig_CAPTUREMODETYPE.nFrameLimit		= 0;
	iConfig_CAPTURING.bEnabled				= OMX_FALSE;
	iConfig_AUTOPAUSE.bEnabled				= OMX_FALSE;

	OUTR(" ",(OMX_ErrorNone));
}

fakesource::~fakesource()   
{
    IN0("");
	if(memory_handle_iParam_CONTENTURITYPE_data_extended != 0)
	{
		//LOS_Log("fakesource::~fakesource Freeing previous structure data\n", NULL, NULL, NULL, NULL, NULL, NULL);
		//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_data_extended);
		//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
		LOS_Free(memory_handle_iParam_CONTENTURITYPE_data_extended);
		memory_handle_iParam_CONTENTURITYPE_data_extended = 0;
	}
	if(memory_handle_iParam_CONTENTURITYPE_extradata_extended != 0)
	{
		//LOS_Log("fakesource::~fakesource Freeing previous structure extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
		//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
		//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
		LOS_Free(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
		memory_handle_iParam_CONTENTURITYPE_extradata_extended = 0;
	}

    OUTR(" ",(OMX_ErrorNone));
}

OMX_ERRORTYPE fakesource::createPort (OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_U32 nFifoBufferSize)
{
    IN0("");
    if (nPortIndex > getPortCount())
    {
    	OUTR("",OMX_ErrorBadPortIndex);
        return OMX_ErrorBadPortIndex;
    }

    EnsCommonPortData commonPortData(
			nPortIndex,
			eDir,
			nFifoBufferSize,
			0,
			OMX_PortDomainVideo,
			eSupplierPref
			);

    ENS_Port* port = new fakesourceport(commonPortData, *this);
    if (port == 0)
    	{
    		OUTR("",(OMX_ErrorInsufficientResources));
    		return OMX_ErrorInsufficientResources;
    	}

    addPort(port);

    OUTR("",OMX_ErrorNone);
    return OMX_ErrorNone;
}

fakesourceport::fakesourceport(const EnsCommonPortData& commonPortData, ENS_Component& ensComp) : ENS_Port(commonPortData, ensComp) 
{
	mBytesPerPixel = 2;
}

fakesourceport::~fakesourceport() 
{
    IN0("");
    OUTR(" ",(OMX_ErrorNone));
}

float fakesourceport::getPixelDepth(OMX_U32 format)
{
	IN0("");
	float returnValue = 4.0;
	const _sFakeSourceImageFormatDescription* pInfo = FakeSourceGetImageFormatDescription((int) format);
	if((const _sFakeSourceImageFormatDescription*)NULL != pInfo)
	{
		returnValue = pInfo->OverallPixelDepth;
	}

	OUTR("",returnValue);
	return returnValue;
}

OMX_ERRORTYPE fakesourceport::checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const 
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE fakesourceport::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) 
{
	IN0("");
	int minimumStride = 0;
	const _sFakeSourceImageFormatDescription* pInfo = (const _sFakeSourceImageFormatDescription*)NULL;
	t_uint32 width = 0;

	mParamPortDefinition.eDomain = portdef.eDomain;
	switch(portdef.eDomain)
	{
	case OMX_PortDomainVideo:
		{
			LOS_Log("fakesourceport::setFormatInPortDefinition domaine detected : OMX_PortDomainVideo\n");
			mParamPortDefinition.eDomain = portdef.eDomain;
			mParamPortDefinition.format.video.cMIMEType =NULL;
			mParamPortDefinition.format.video.nFrameWidth = portdef.format.video.nFrameWidth;
			mParamPortDefinition.format.video.nFrameHeight = portdef.format.video.nFrameHeight;	
			mParamPortDefinition.format.video.pNativeRender = portdef.format.video.pNativeRender ;
			if(0 == portdef.format.video.nSliceHeight)
			{
				mParamPortDefinition.format.video.nSliceHeight = mParamPortDefinition.format.video.nFrameHeight;
			}
			else
			{
				if(portdef.format.video.nSliceHeight < portdef.format.video.nFrameHeight)
				{
				LOS_Log("\t bad setting of nSliceHeight => change it \n");
					mParamPortDefinition.format.video.nSliceHeight = mParamPortDefinition.format.video.nFrameHeight;
				}
				else
				{
					mParamPortDefinition.format.video.nSliceHeight = portdef.format.video.nSliceHeight;
				}
			}
			LOS_Log("\t nSliceHeight : %d \n", mParamPortDefinition.format.video.nSliceHeight);
			mParamPortDefinition.format.video.nBitrate = 0;	
			mParamPortDefinition.format.video.xFramerate = portdef.format.video.xFramerate;
			mParamPortDefinition.format.video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
			mParamPortDefinition.format.video.eCompressionFormat = portdef.format.video.eCompressionFormat;
			mParamPortDefinition.format.video.eColorFormat = portdef.format.video.eColorFormat;
			mParamPortDefinition.format.video.pNativeWindow = portdef.format.video.pNativeWindow;
			
			if ( mParamPortDefinition.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit )
			{
				width = (((191 + mParamPortDefinition.format.video.nFrameWidth * 12)/192)*192)/12;	
				LOS_Log("\t case  OMX_SYMBIAN_COLOR_FormatRawBayer12bit : used width %d \n", width);
			}
			else if (mParamPortDefinition.format.video.eColorFormat == OMX_COLOR_FormatRawBayer8bit)
			{
				width = (((191 + mParamPortDefinition.format.video.nFrameWidth * 8)/192)*192)/8;
				LOS_Log("\t case  OMX_COLOR_FormatRawBayer8bit : used width %d \n", width);
			}
			else
			{
				width = portdef.format.video.nFrameWidth;
			}

			pInfo = FakeSourceGetImageFormatDescription((int) mParamPortDefinition.format.video.eColorFormat);
			if((const _sFakeSourceImageFormatDescription*)NULL == pInfo)
			{
				LOS_Log("\t Error format not fund \n");
				return OMX_ErrorBadParameter;
			}
			minimumStride = (int)(width * pInfo->PixelDepth0);
			LOS_Log("\t minimumStride : %d \n", minimumStride);
			if(0 == portdef.format.video.nStride)
			{
				mParamPortDefinition.format.video.nStride = minimumStride;
			}
			else
			{
				if(portdef.format.video.nStride < minimumStride)
				{
				LOS_Log("\t bad setting of nStride => change it \n");
					mParamPortDefinition.format.video.nStride = minimumStride;
				}
				else
				{
					mParamPortDefinition.format.video.nStride = portdef.format.video.nStride;
				}
			}
			LOS_Log("\t nStride : %d \n", mParamPortDefinition.format.video.nStride);

			LOS_Log("\t %s OverallPixelDepth : %f \n", pInfo->Name, pInfo->OverallPixelDepth);
			mParamPortDefinition.nBufferSize = (OMX_U32)(width * pInfo->OverallPixelDepth * mParamPortDefinition.format.video.nSliceHeight);
			LOS_Log("\t nBufferSize : %d \n", mParamPortDefinition.nBufferSize);
    		// allocate 30%^2 extra to accomodate stab overscanning! 
			mParamPortDefinition.nBufferSize += (mParamPortDefinition.nBufferSize*7)/10;
			LOS_Log("\t nBufferSize updated after overscan of 30percent^2: %d \n", mParamPortDefinition.nBufferSize);

			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + 4*sizeof(OMX_U32); // for ExtraDataQuantization
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE);
#ifdef NO_DEP_WITH_IFM
#else
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(IFM_BMS_CAPTURE_CONTEXT);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(IFM_DAMPERS_DATA);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(EXTRADATA_EXIFMARKERCONTAINERTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_STE_PRODUCTIONTESTTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_STE_CAPTUREPARAMETERSTYPE);
#endif
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CONFIG_FEEDBACKTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CONFIG_RGBHISTOGRAM);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_ROITYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE);
			mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE); // for ending
			
			LOS_Log("\t nBufferSize updated with added size for extradata: %d \n", mParamPortDefinition.nBufferSize);
			mParamPortDefinition.nBufferSize = (mParamPortDefinition.nBufferSize+0x1000) & 0xFFFFF000; // round to next 4K
			LOS_Log("\t nBufferSize rounded to next 4K: %d \n", mParamPortDefinition.nBufferSize);
		}
		break;
	case OMX_PortDomainImage:
		LOS_Log("fakesourceport::setFormatInPortDefinition domaine detected : OMX_PortDomainImage\n");
		mParamPortDefinition.format.image.nFrameWidth = portdef.format.image.nFrameWidth;
		mParamPortDefinition.format.image.nFrameHeight = portdef.format.image.nFrameHeight;
		mParamPortDefinition.format.image.pNativeRender = portdef.format.image.pNativeRender ;
		mParamPortDefinition.format.image.bFlagErrorConcealment = portdef.format.image.bFlagErrorConcealment;
		mParamPortDefinition.format.image.eCompressionFormat = portdef.format.image.eCompressionFormat;
		mParamPortDefinition.format.image.eColorFormat = portdef.format.image.eColorFormat;
		mParamPortDefinition.format.image.pNativeWindow = portdef.format.image.pNativeWindow;
		
		if(0 == portdef.format.image.nSliceHeight)
		{
			mParamPortDefinition.format.image.nSliceHeight = mParamPortDefinition.format.image.nFrameHeight;
		}
		else
		{
			if(portdef.format.image.nSliceHeight < portdef.format.image.nFrameHeight)
			{
			LOS_Log("\t bad setting of nSliceHeight => change it \n");
				mParamPortDefinition.format.image.nSliceHeight = mParamPortDefinition.format.image.nFrameHeight;
			}
			else
			{
				mParamPortDefinition.format.image.nSliceHeight = portdef.format.image.nSliceHeight;
			}
		}
		LOS_Log("\t nSliceHeight : %d \n", mParamPortDefinition.format.image.nSliceHeight);

		mParamPortDefinition.format.image.bFlagErrorConcealment = portdef.format.image.bFlagErrorConcealment;
		mParamPortDefinition.format.image.eCompressionFormat = portdef.format.image.eCompressionFormat;
		mParamPortDefinition.format.image.eColorFormat = portdef.format.image.eColorFormat;
		mParamPortDefinition.format.image.pNativeWindow = portdef.format.image.pNativeWindow;
		pInfo = FakeSourceGetImageFormatDescription((int) mParamPortDefinition.format.video.eColorFormat);
		if((const _sFakeSourceImageFormatDescription*)NULL == pInfo)
		{
			LOS_Log("\t Error format not fund \n");
			return OMX_ErrorBadParameter;
		}
		width = mParamPortDefinition.format.image.nFrameWidth;
		minimumStride = (int)(width * pInfo->PixelDepth0);
		LOS_Log("\t minimumStride : %d \n", minimumStride);
		if(0 == portdef.format.image.nStride)
		{
			mParamPortDefinition.format.image.nStride = minimumStride;
		}
		else
		{
			if(portdef.format.image.nStride < minimumStride)
			{
			LOS_Log("\t bad setting of nStride => change it \n");
				mParamPortDefinition.format.image.nStride = minimumStride;
			}
			else
			{
				mParamPortDefinition.format.image.nStride = portdef.format.image.nStride;
			}
		}
		LOS_Log("\t nStride : %d \n", mParamPortDefinition.format.image.nStride);

		LOS_Log("\t %s OverallPixelDepth : %f \n", pInfo->Name, pInfo->OverallPixelDepth);
		mParamPortDefinition.nBufferSize = (OMX_U32)(width * pInfo->OverallPixelDepth * mParamPortDefinition.format.video.nSliceHeight);
		LOS_Log("\t nBufferSize : %d \n", mParamPortDefinition.nBufferSize);

		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + 4*sizeof(OMX_U32); // for ExtraDataQuantization
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE);
#ifdef NO_DEP_WITH_IFM
#else
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(IFM_BMS_CAPTURE_CONTEXT);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(IFM_DAMPERS_DATA);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(EXTRADATA_EXIFMARKERCONTAINERTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_STE_PRODUCTIONTESTTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_STE_CAPTUREPARAMETERSTYPE);
#endif
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CONFIG_FEEDBACKTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_CONFIG_RGBHISTOGRAM);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_ROITYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE) + sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE);
		mParamPortDefinition.nBufferSize += sizeof(OMX_OTHER_EXTRADATATYPE); // for ending
		
		LOS_Log("\t nBufferSize updated with added size for extradata: %d \n", mParamPortDefinition.nBufferSize);
		mParamPortDefinition.nBufferSize = (mParamPortDefinition.nBufferSize+0x1000) & 0xFFFFF000; // round to next 4K
		LOS_Log("\t nBufferSize rounded to next 4K: %d \n", mParamPortDefinition.nBufferSize);
		break;
	case OMX_PortDomainAudio:
		//LOS_Log("fakesourceport::setFormatInPortDefinition domaine detected : OMX_PortDomainAudio\n", NULL , NULL, NULL, NULL, NULL, NULL);
		mParamPortDefinition.format.audio.eEncoding = portdef.format.audio.eEncoding;
		mParamPortDefinition.format.audio.pNativeRender = portdef.format.audio.pNativeRender;
		break;
	case OMX_PortDomainOther:
		//LOS_Log("fakesourceport::setFormatInPortDefinition domaine detected : OMX_PortDomainOther\n", NULL , NULL, NULL, NULL, NULL, NULL);
		break;
	default:
		//LOS_Log("fakesourceport::setFormatInPortDefinition domaine detected : not classic : 0x%x\n", portdef.eDomain , NULL, NULL, NULL, NULL, NULL);
		break;
	}
	//LOS_Log("nBufferSize = %d\n", portdef.nBufferSize , NULL, NULL, NULL, NULL, NULL);

	OUTR("",OMX_ErrorNone);
	return OMX_ErrorNone;

}

OMX_ERRORTYPE fakesource::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const 
{
    IN0("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
	//LOS_Log("fakesource::getParameter\n", NULL , NULL, NULL, NULL, NULL, NULL);

    if (pComponentParameterStructure == 0) 
    {
        MSG0("ERROR in getParameter : pComponentParameterStructure is null\n");
        OUTR("",(OMX_ErrorBadParameter));
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex)
    {

		case OMX_IndexParamCompBufferSupplier:
			{
			OMX_PARAM_BUFFERSUPPLIERTYPE *pBuffSup = (OMX_PARAM_BUFFERSUPPLIERTYPE *)pComponentParameterStructure;
			OMX_PARAM_BUFFERSUPPLIERTYPE buffSup;
			LOS_Log("\n\n\t FAKESOURCE_TRACE fct: %s, index OMX_IndexParamCompBufferSupplier \n\n\n", __FUNCTION__);
			buffSup.nPortIndex = pBuffSup->nPortIndex;
			buffSup.nSize = sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE);
			buffSup.nVersion = getVersion();
			buffSup.eBufferSupplier = OMX_BufferSupplyInput;

			memcpy(pComponentParameterStructure, &buffSup, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
			break;
			}
	
		default : 
			err = ENS_Component::getParameter(nParamIndex, pComponentParameterStructure);
    		break;
    }
	OUTR("",(err));
	return err;
}

char* fakesource::my_strstr (char * str1, const char * str2 )
{
	int sizeString_1 = 0, sizeString_2 = 0;
	int count_1 = 0, count_2 = 0;

	sizeString_1 = strlen(str1);
	sizeString_2 = strlen(str2);
	if(sizeString_1 < sizeString_2)
		return (char*)NULL;

	while( count_1 <= sizeString_1 - sizeString_2)
	{
		count_2 = 0;
		while( (count_2 < sizeString_2) && (str1[count_1 + count_2] == str2[count_2]) )
		{
			count_2++;
		}
		if(count_2 == sizeString_2)
		{
			return (str1 + count_1);
		}
		count_1++;
	}
	return (char*)NULL;
}


OMX_ERRORTYPE fakesource::setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure)
{
    IN0("");
    OMX_ERRORTYPE err = OMX_ErrorNone;  
	//LOS_Log("fakesource::setParameter\n", NULL , NULL, NULL, NULL, NULL, NULL);

    if (pComponentParameterStructure == 0) 
	{
        MSG0("ERROR in setParameter : pComponentParameterStructure is null\n");
        OUTR("",(OMX_ErrorBadParameter));
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) 
	{
        case OMX_IndexParamContentURI :
		{
			int sizeInputString = 0, sizeAllocatedString = 0;
			//LOS_Log("fakesource::setParameter OMX_IndexParamContentURI\n", NULL , NULL, NULL, NULL, NULL, NULL);

			if(my_strstr((char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, (char*)FAKESOURCE_PATH_PREFIXE_EXTRADATA))
			{
				if(memory_handle_iParam_CONTENTURITYPE_extradata_extended != 0)
				{
					//LOS_Log("fakesource::setParameter Freeing previous structure extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
					//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
					LOS_Free(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					memory_handle_iParam_CONTENTURITYPE_extradata_extended = 0;
				}
				sizeInputString = strlen( (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI);
				//LOS_Log("fakesource::setParameter sizeInputString = %d\n", sizeInputString , NULL, NULL, NULL, NULL, NULL);
				sizeAllocatedString = sizeInputString + 10;
				//LOS_Log("fakesource::setParameter sizeAllocatedString = %d\n", sizeAllocatedString , NULL, NULL, NULL, NULL, NULL);

				memory_handle_iParam_CONTENTURITYPE_extradata_extended = LOS_Alloc(sizeof(OMX_PARAM_CONTENTURITYPE) + sizeAllocatedString, 4, LOS_MAPPING_DEFAULT);
				if(memory_handle_iParam_CONTENTURITYPE_extradata_extended != 0)
				{
					iParam_CONTENTURITYPE_extradata = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					memcpy(iParam_CONTENTURITYPE_extradata, pComponentParameterStructure, sizeof(OMX_PARAM_CONTENTURITYPE));
					//LOS_Log("fakesource::setParameter strncpy of size = %d\n", sizeInputString + 1 , NULL, NULL, NULL, NULL, NULL);
					strncpy((char*) iParam_CONTENTURITYPE_extradata->contentURI, (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, sizeInputString + 1);
					LOS_Log("fakesource::setParameter iParam_CONTENTURITYPE_extradata->contentURI = %s\n", iParam_CONTENTURITYPE_extradata->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}
			else
			{
				if(memory_handle_iParam_CONTENTURITYPE_data_extended != 0)
				{
					//LOS_Log("fakesource::setParameter Freeing previous structure data\n", NULL, NULL, NULL, NULL, NULL, NULL);
					//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_data_extended);
					//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
					LOS_Free(memory_handle_iParam_CONTENTURITYPE_data_extended);
					memory_handle_iParam_CONTENTURITYPE_data_extended = 0;
				}
				sizeInputString = strlen( (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI);
				//LOS_Log("fakesource::setParameter sizeInputString = %d\n", sizeInputString , NULL, NULL, NULL, NULL, NULL);
				sizeAllocatedString = sizeInputString + 10;
				//LOS_Log("fakesource::setParameter sizeAllocatedString = %d\n", sizeAllocatedString , NULL, NULL, NULL, NULL, NULL);

				memory_handle_iParam_CONTENTURITYPE_data_extended = LOS_Alloc(sizeof(OMX_PARAM_CONTENTURITYPE) + sizeAllocatedString, 4, LOS_MAPPING_DEFAULT);
				if(memory_handle_iParam_CONTENTURITYPE_data_extended != 0)
				{
					iParam_CONTENTURITYPE_data = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_data_extended);
					memcpy(iParam_CONTENTURITYPE_data, pComponentParameterStructure, sizeof(OMX_PARAM_CONTENTURITYPE));
					//LOS_Log("fakesource::setParameter strncpy of size = %d\n", sizeInputString + 1 , NULL, NULL, NULL, NULL, NULL);
					strncpy((char*) iParam_CONTENTURITYPE_data->contentURI, (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, sizeInputString + 1);
					LOS_Log("fakesource::setParameter iParam_CONTENTURITYPE_data->contentURI = %s\n", iParam_CONTENTURITYPE_data->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}				
		}
        break;
		case OMX_IndexParamCompBufferSupplier:
			{
			LOS_Log("\n\n\t FAKESOURCE_TRACE fct: %s, index OMX_IndexParamCompBufferSupplier \n\n\n", __FUNCTION__);
			break;
			}

		default : 
            err = ENS_Component::setParameter(nParamIndex,pComponentParameterStructure);
			break;
    }
    OUTR("",(err));
    return err;
}

OMX_ERRORTYPE fakesource::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const 
{
    IN0("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_U32 portIndex = 0;
 	//LOS_Log("fakesource::getConfig\n", NULL , NULL, NULL, NULL, NULL, NULL);
   
	if (pComponentParameterStructure == 0) 
    {
        MSG0("ERROR in getConfig : pComponentParameterStructure is null\n");
        OUTR("",(OMX_ErrorBadParameter));
        return OMX_ErrorBadParameter;
    }
    
    switch (nParamIndex) 
    {
 		case OMX_IndexConfigCaptureMode :
		{
			//LOS_Log("fakesource::getConfig OMX_IndexConfigCaptureMode\n", NULL , NULL, NULL, NULL, NULL, NULL);
			portIndex = ((OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure)->nPortIndex;
			memcpy(pComponentParameterStructure, &iConfig_CAPTUREMODETYPE, sizeof(OMX_CONFIG_CAPTUREMODETYPE));
			((OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure)->nSize = sizeof(OMX_CONFIG_CAPTUREMODETYPE);
			((OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure)->nVersion.nVersion = FAKESOURCE_VERSION;
			((OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure)->nPortIndex = portIndex;
		}
        break;		

		case OMX_IndexConfigCapturing:
		{
			//LOS_Log("fakesource::getConfig OMX_IndexConfigCapturing\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(pComponentParameterStructure, &iConfig_CAPTURING, sizeof(OMX_CONFIG_BOOLEANTYPE));
			((OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure)->nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
			((OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure)->nVersion.nVersion = FAKESOURCE_VERSION;
		}
        break;		

		case OMX_IndexAutoPauseAfterCapture:
		{
			//LOS_Log("fakesource::getConfig OMX_IndexAutoPauseAfterCapture\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(pComponentParameterStructure, &iConfig_AUTOPAUSE, sizeof(OMX_CONFIG_BOOLEANTYPE));
			((OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure)->nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
			((OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure)->nVersion.nVersion = FAKESOURCE_VERSION;
		}
        break;		

		case OMX_IndexConfigVideoFramerate :
		{
			//LOS_Log("fakesource::getConfig OMX_IndexConfigVideoFramerate\n", NULL , NULL, NULL, NULL, NULL, NULL);
			portIndex = ((OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure)->nPortIndex;
			memcpy(pComponentParameterStructure, &iConfig_FRAMERATETYPE, sizeof(OMX_CONFIG_FRAMERATETYPE));
			((OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure)->nSize = sizeof(OMX_CONFIG_FRAMERATETYPE);
			((OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure)->nVersion.nVersion = FAKESOURCE_VERSION;
			((OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure)->nPortIndex = portIndex;
		}
        break;

		default :
            err = ENS_Component::getConfig(nParamIndex, pComponentParameterStructure);
            break;
    }
    OUTR("",(err));
    return err;
}

OMX_ERRORTYPE fakesource::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) 
{
    IN0("");
    OMX_ERRORTYPE err = OMX_ErrorNone;  
	//LOS_Log("fakesource::setConfig\n", NULL , NULL, NULL, NULL, NULL, NULL);

	if (pComponentParameterStructure == 0) 
	{
		MSG0("ERROR in setConfig : pComponentParameterStructure is null\n");
		OUTR("",OMX_ErrorBadParameter);
		return OMX_ErrorBadParameter;
	}

    switch (nParamIndex) 
    {
 		case (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAnalyzerFeedback :
		{
		}
        break;
 		case OMX_IndexConfigCaptureMode :
		{
			//LOS_Log("fakesource::setConfig OMX_IndexConfigCaptureMode\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(&iConfig_CAPTUREMODETYPE, pComponentParameterStructure, sizeof(OMX_CONFIG_CAPTUREMODETYPE));
		}
        break;		

 		case OMX_IndexConfigCapturing :
		{
			//LOS_Log("fakesource::setConfig OMX_IndexConfigCapturing\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(&iConfig_CAPTURING, pComponentParameterStructure, sizeof(OMX_CONFIG_BOOLEANTYPE));
		}
        break;		

  		case OMX_IndexAutoPauseAfterCapture :
		{
			//LOS_Log("fakesource::setConfig OMX_IndexAutoPauseAfterCapture\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(&iConfig_AUTOPAUSE, pComponentParameterStructure, sizeof(OMX_CONFIG_BOOLEANTYPE));
		}
        break;		

		case OMX_IndexParamContentURI :
		{
			int sizeInputString = 0, sizeAllocatedString = 0;
			//LOS_Log("fakesource::setConfig OMX_IndexParamContentURI\n", NULL , NULL, NULL, NULL, NULL, NULL);

			if(my_strstr((char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, (char*)FAKESOURCE_PATH_PREFIXE_EXTRADATA))
			{
				if(memory_handle_iParam_CONTENTURITYPE_extradata_extended != 0)
				{
					//LOS_Log("fakesource::setConfig Freeing previous structure extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
					//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
					LOS_Free(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					memory_handle_iParam_CONTENTURITYPE_extradata_extended = 0;
				}
				sizeInputString = strlen( (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI);
				//LOS_Log("fakesource::setParameter sizeInputString = %d\n", sizeInputString , NULL, NULL, NULL, NULL, NULL);
				sizeAllocatedString = sizeInputString + 10;
				//LOS_Log("fakesource::setParameter sizeAllocatedString = %d\n", sizeAllocatedString , NULL, NULL, NULL, NULL, NULL);

				memory_handle_iParam_CONTENTURITYPE_extradata_extended = LOS_Alloc(sizeof(OMX_PARAM_CONTENTURITYPE) + sizeAllocatedString, 4, LOS_MAPPING_DEFAULT);
				if(memory_handle_iParam_CONTENTURITYPE_extradata_extended != 0)
				{
					iParam_CONTENTURITYPE_extradata = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_extradata_extended);
					memcpy(iParam_CONTENTURITYPE_extradata, pComponentParameterStructure, sizeof(OMX_PARAM_CONTENTURITYPE));
					//LOS_Log("fakesource::setParameter strncpy of size = %d\n", sizeInputString + 1 , NULL, NULL, NULL, NULL, NULL);
					strncpy((char*) iParam_CONTENTURITYPE_extradata->contentURI, (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, sizeInputString + 1);
					LOS_Log("fakesource::setConfig iParam_CONTENTURITYPE_extradata->contentURI = %s\n", iParam_CONTENTURITYPE_extradata->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}
			else
			{
				if(memory_handle_iParam_CONTENTURITYPE_data_extended != 0)
				{
					//LOS_Log("fakesource::setConfig Freeing previous structure data\n", NULL, NULL, NULL, NULL, NULL, NULL);
					//OMX_PARAM_CONTENTURITYPE *tmp_CONTENTURITYPE = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_data_extended);
					//LOS_Log("previous string = %s\n", tmp_CONTENTURITYPE->contentURI , NULL, NULL, NULL, NULL, NULL);
					LOS_Free(memory_handle_iParam_CONTENTURITYPE_data_extended);
					memory_handle_iParam_CONTENTURITYPE_data_extended = 0;
				}
				sizeInputString = strlen( (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI);
				//LOS_Log("fakesource::setParameter sizeInputString = %d\n", sizeInputString , NULL, NULL, NULL, NULL, NULL);
				sizeAllocatedString = sizeInputString + 10;
				//LOS_Log("fakesource::setParameter sizeAllocatedString = %d\n", sizeAllocatedString , NULL, NULL, NULL, NULL, NULL);

				memory_handle_iParam_CONTENTURITYPE_data_extended = LOS_Alloc(sizeof(OMX_PARAM_CONTENTURITYPE) + sizeAllocatedString, 4, LOS_MAPPING_DEFAULT);
				if(memory_handle_iParam_CONTENTURITYPE_data_extended != 0)
				{
					iParam_CONTENTURITYPE_data = (OMX_PARAM_CONTENTURITYPE*)LOS_GetLogicalAddress(memory_handle_iParam_CONTENTURITYPE_data_extended);
					memcpy(iParam_CONTENTURITYPE_data, pComponentParameterStructure, sizeof(OMX_PARAM_CONTENTURITYPE));
					//LOS_Log("fakesource::setParameter strncpy of size = %d\n", sizeInputString + 1 , NULL, NULL, NULL, NULL, NULL);
					strncpy((char*) iParam_CONTENTURITYPE_data->contentURI, (char*) ((OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure)->contentURI, sizeInputString + 1);
					LOS_Log("fakesource::setConfig iParam_CONTENTURITYPE_data->contentURI = %s\n", iParam_CONTENTURITYPE_data->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}				
		}
        break;

		case OMX_IndexParamCompBufferSupplier:
		{
			LOS_Log("\n\n\t FAKESOURCE_TRACE fct: %s, index OMX_IndexParamCompBufferSupplier \n\n\n", __FUNCTION__);
			break;
		}

		case OMX_IndexConfigVideoFramerate :
		{
			//LOS_Log("fakesource::setConfig OMX_IndexConfigVideoFramerate\n", NULL , NULL, NULL, NULL, NULL, NULL);
			memcpy(&iConfig_FRAMERATETYPE, pComponentParameterStructure, sizeof(OMX_CONFIG_FRAMERATETYPE));
		}
        break;

		default :
            err = ENS_Component::setConfig(nParamIndex, pComponentParameterStructure);
            break;
    }

    OUTR("",(err));
    return err;
}

/*
OMX_ERRORTYPE fakesource::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
{
#if 0
#include "mmhwbuffer_ext.h"
	if (0 == strcmp(cParameterName, OMX_SYMBIAN_INDEX_CONFIG_SHAREDCHUNKMETADATA_NAME))
	{
#ifdef __SYMBIAN32__
		*pIndexType = (OMX_INDEXTYPE)OMX_MMHwBufferIndexConfigSharedChunkMetadata;
#else
		*pIndexType = (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata;
#endif

		MSG1("fakesource::getExtensionIndex=%d\n", *pIndexType);
		return OMX_ErrorNone;
	}
#endif
	return (CExtension::getExtensionIndex(cParameterName, pIndexType));
}
*/

#ifdef NO_DEPENDENCY_ON_IFM_NMF
FakeSourceProcessingComp::FakeSourceProcessingComp(ENS_Component &enscomp) : NmfHost_ProcessingComponent(enscomp)
#else
FakeSourceProcessingComp::FakeSourceProcessingComp(ENS_Component &enscomp) : IFM_HostNmfProcessingComp(enscomp)
#endif
{
	mNmfSource = (fake_source_wrp *)NULL;
	mFakePort = (fakesourceport*)enscomp.getPort(0);
}

OMX_ERRORTYPE FakeSourceProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
/*
	LOS_Log("FakeSourceProcessingComp::doSpecificSendCommand \n", NULL, NULL, NULL, NULL, NULL, NULL);

	switch (eCmd)
	{
		case OMX_CommandStateSet:
			LOS_Log("\t tracing cmd: OMX_CommandStateSet\n");
			{
				switch(nData)
				{
					case OMX_StateInvalid:
						LOS_Log("\t %s tracing state: OMX_StateInvalid\n", __FUNCTION__);
						break;
					case OMX_StateLoaded:
						LOS_Log("\t %s tracing state: OMX_StateLoaded\n", __FUNCTION__);
						break;
					case OMX_StateIdle:
						LOS_Log("\t %s tracing state: OMX_StateIdle\n", __FUNCTION__);
						break;
					case OMX_StateExecuting:
						LOS_Log("\t %s tracing state: OMX_StateExecuting\n", __FUNCTION__);
						break;
					case OMX_StatePause:
						LOS_Log("\t %s tracing state: OMX_StatePause\n", __FUNCTION__);
						break;
					case OMX_StateTransientToIdle:
						LOS_Log("\t %s tracing state: OMX_StateTransientToIdle\n", __FUNCTION__);
						break;
					case OMX_StateWaitForResources:
						LOS_Log("\t %s tracing state: OMX_StateWaitForResources\n", __FUNCTION__);
						break;
					case OMX_StateTransient:
						LOS_Log("\t %s tracing state: OMX_StateTransient\n", __FUNCTION__);
						break;
					case OMX_StateLoadedToIdleHOST:
						LOS_Log("\t %s tracing state: OMX_StateLoadedToIdleHOST\n", __FUNCTION__);
						break;
					case OMX_StateLoadedToIdleDSP:
						LOS_Log("\t %s tracing state: OMX_StateLoadedToIdleDSP\n", __FUNCTION__);
						break;
					case OMX_StateIdleToLoaded:
						LOS_Log("\t %s tracing state: OMX_StateIdleToLoaded\n", __FUNCTION__);
						break;
					case OMX_StateIdleToExecuting:
						LOS_Log("\t %s tracing state: OMX_StateIdleToExecuting\n", __FUNCTION__);
						break;
					case OMX_StateIdleToPause:
						LOS_Log("\t %s tracing state: OMX_StateIdleToPause\n", __FUNCTION__);
						break;
					case OMX_StateExecutingToIdle:
						LOS_Log("\t %s tracing state: OMX_StateExecutingToIdle\n", __FUNCTION__);
						break;
					case OMX_StateExecutingToPause:
						LOS_Log("\t %s tracing state: OMX_StateExecutingToPause\n", __FUNCTION__);
						break;
					case OMX_StatePauseToIdle:
						LOS_Log("\t %s tracing state: OMX_StatePauseToIdle\n", __FUNCTION__);
						break;
					case OMX_StatePauseToExecuting:
						LOS_Log("\t %s tracing state: OMX_StatePauseToExecuting\n", __FUNCTION__);
						break;
					default:
						LOS_Log("\t %s tracing state: OMX_State ... unknown = %d\n", __FUNCTION__, nData);
						break;
				}
			}
			break;
		case OMX_CommandFlush:
			LOS_Log("\t  %s tracing cmd: OMX_CommandFlush\n", __FUNCTION__);break;
		case OMX_CommandPortDisable:
			LOS_Log("\t  %s tracing cmd: OMX_CommandPortDisable\n", __FUNCTION__);break;
		case OMX_CommandPortEnable:
			LOS_Log("\t  %s tracing cmd: OMX_CommandPortEnable\n", __FUNCTION__);break;
		case OMX_CommandMarkBuffer:
			LOS_Log("\t  %s tracing cmd: OMX_CommandMarkBuffer\n", __FUNCTION__);break;
		default:
			LOS_Log("\t  %s tracing cmd: Unknown param : line = %d\n", __FUNCTION__, __LINE__, NULL, NULL, NULL, NULL);break;
	}
*/
	return OMX_ErrorNone;
}

OMX_PTR FakeSourceProcessingComp::retriveStructureFromProxy(OMX_INDEXTYPE nParamIndex, OMX_U32 count)
{
	IN0("");
	fakesource* myFakeSource = (fakesource*)&mENSComponent; 
	OMX_PTR pParameterStructure = 0;
	//LOS_Log("FakeSourceProcessingComp::retriveStructureFromProxy nParamIndex = %d\n", nParamIndex , NULL, NULL, NULL, NULL, NULL);

	switch (nParamIndex) 
	{
		case OMX_IndexConfigCaptureMode :
			//LOS_Log("retriveStructureFromProxy OMX_IndexConfigCaptureMode\n", NULL , NULL, NULL, NULL, NULL, NULL);
			pParameterStructure = &myFakeSource->iConfig_CAPTUREMODETYPE;
	        break;
		case OMX_IndexConfigCapturing :
			//LOS_Log("retriveStructureFromProxy OMX_IndexConfigCapturing\n", NULL , NULL, NULL, NULL, NULL, NULL);
			pParameterStructure = &myFakeSource->iConfig_CAPTURING;
	        break;
		case OMX_IndexAutoPauseAfterCapture :
			//LOS_Log("retriveStructureFromProxy OMX_IndexAutoPauseAfterCapture\n", NULL , NULL, NULL, NULL, NULL, NULL);
			pParameterStructure = &myFakeSource->iConfig_AUTOPAUSE;
	        break;
		case OMX_IndexParamContentURI :
			//LOS_Log("retriveStructureFromProxy OMX_IndexParamContentURI\n", NULL , NULL, NULL, NULL, NULL, NULL);
			if(count == 0)
			{
				pParameterStructure = myFakeSource->iParam_CONTENTURITYPE_data;
			}
			else
			{
				pParameterStructure = myFakeSource->iParam_CONTENTURITYPE_extradata;
			}
	        break;
		case OMX_IndexConfigVideoFramerate :
			//LOS_Log("retriveStructureFromProxy OMX_IndexConfigVideoFramerate\n", NULL , NULL, NULL, NULL, NULL, NULL);
			pParameterStructure = &myFakeSource->iConfig_FRAMERATETYPE;
			break;
		default :
			break;
	}

    OUTR("",(OMX_ErrorNone));
	return pParameterStructure;
}

OMX_ERRORTYPE FakeSourceProcessingComp::applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	OMX_PTR pParameterStructure = 0;
	IN0("");	
	//LOS_Log("FakeSourceProcessingComp::applyConfig index = 0x%x\n", nParamIndex , NULL, NULL, NULL, NULL, NULL);
/*
    switch (nParamIndex) 
    {
 		case OMX_IndexConfigCaptureMode :
		{
			OMX_CONFIG_CAPTUREMODETYPE * tmpStructure = (OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_CONFIG_CAPTUREMODETYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
  		case OMX_IndexConfigCapturing :
		{
			OMX_CONFIG_BOOLEANTYPE * tmpStructure = (OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_CONFIG_BOOLEANTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
  		case OMX_IndexAutoPauseAfterCapture :
		{
			OMX_CONFIG_BOOLEANTYPE * tmpStructure = (OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_CONFIG_BOOLEANTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
        case OMX_IndexParamContentURI :
		{
			OMX_PARAM_CONTENTURITYPE * tmpStructure = (OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_PARAM_CONTENTURITYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
        break;
		case OMX_IndexParamCompBufferSupplier:
		{
			OMX_PARAM_BUFFERSUPPLIERTYPE * tmpStructure = (OMX_PARAM_BUFFERSUPPLIERTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_PARAM_BUFFERSUPPLIERTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
		case OMX_IndexConfigVideoFramerate :
		{
			OMX_CONFIG_FRAMERATETYPE * tmpStructure = (OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::applyConfig : OMX_CONFIG_FRAMERATETYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
        break;
		default :
            break;
    }
*/
	//pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	pParameterStructure = pComponentParameterStructure;
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::applyConfig call setyourconfigitf.setMyConfig index = 0x%x\n", nParamIndex , NULL, NULL, NULL, NULL, NULL);
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	
	OUTR("",OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) 
{
	OMX_PTR pParameterStructure = 0;
	IN0("");	
	//LOS_Log("FakeSourceProcessingComp::setConfig index = 0x%x\n", nParamIndex , NULL, NULL, NULL, NULL, NULL);
/*
    switch (nParamIndex) 
    {
 		case OMX_IndexConfigCaptureMode :
		{
			OMX_CONFIG_CAPTUREMODETYPE * tmpStructure = (OMX_CONFIG_CAPTUREMODETYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_CONFIG_CAPTUREMODETYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
  		case OMX_IndexConfigCapturing :
		{
			OMX_CONFIG_BOOLEANTYPE * tmpStructure = (OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_CONFIG_BOOLEANTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
  		case OMX_IndexAutoPauseAfterCapture :
		{
			OMX_CONFIG_BOOLEANTYPE * tmpStructure = (OMX_CONFIG_BOOLEANTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_CONFIG_BOOLEANTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
        case OMX_IndexParamContentURI :
		{
			OMX_PARAM_CONTENTURITYPE * tmpStructure = (OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_PARAM_CONTENTURITYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
        break;
		case OMX_IndexParamCompBufferSupplier:
		{
			OMX_PARAM_BUFFERSUPPLIERTYPE * tmpStructure = (OMX_PARAM_BUFFERSUPPLIERTYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_PARAM_BUFFERSUPPLIERTYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
		break;
		case OMX_IndexConfigVideoFramerate :
		{
			OMX_CONFIG_FRAMERATETYPE * tmpStructure = (OMX_CONFIG_FRAMERATETYPE*)pComponentParameterStructure;
			LOS_Log("FakeSourceProcessingComp::setConfig : OMX_CONFIG_FRAMERATETYPE = 0x%x\n", tmpStructure , NULL, NULL, NULL, NULL, NULL);
		}
        break;
		default :
            break;
    }
*/
	//pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	pParameterStructure = pComponentParameterStructure;
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::setConfig call setyourconfigitf.setMyConfig index = 0x%x\n", nParamIndex , NULL, NULL, NULL, NULL, NULL);
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	
	OUTR("",OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::instantiate() 
{
    IN0("");
    MSG0("Instantiating arm nmf component\n");
	t_nmf_error error ;
	int fifoSize = 6;

#ifdef NO_DEPENDENCY_ON_IFM_NMF
	LOS_Log("\n\n FakeSourceProcessingComp case NO_DEPENDENCY_ON_IFM_NMF !!!!\n\n", NULL , NULL, NULL, NULL, NULL, NULL);
#else
	LOS_Log("\n\n FakeSourceProcessingComp case undefined NO_DEPENDENCY_ON_IFM_NMF !!!!\n\n", NULL , NULL, NULL, NULL, NULL, NULL);
#endif
	
	mNmfSource = fake_source_wrpCreate();
    if (mNmfSource->construct() != NMF_OK) NMF_PANIC("PANIC - Construct Error\n") ;

	error = mNmfSource->bindFromUser("sendcommand" ,fifoSize,&mIsendCommand) ;
	if (error != NMF_OK) NMF_PANIC("PANIC - bindFromUser sendCommand\n") ;
       
	error = mNmfSource->bindFromUser("setParam", fifoSize, &setparamitf) ;// asynchronous api
	if (error != NMF_OK) NMF_PANIC("PANIC - bindFromUser setParam\n") ;	
	
	error = mNmfSource->bindFromUser("setYourConfigure", fifoSize, &setyourconfigitf) ;// asynchronous api
	if (error != NMF_OK) NMF_PANIC("PANIC - bindFromUser setYourConfigure\n") ;

	error = mNmfSource->getInterface("synchroneYourApi",&synchroneyourapiitf) ; // synchronous api
	if (error != NMF_OK) NMF_PANIC("PANIC - getInterface synchroneYourApi\n") ;

	error = mNmfSource->bindFromUser("fsminit" ,1, &mIfsmInit);
    if (error != NMF_OK) NMF_PANIC("Error: unable to bind fsminit!...\n");

    error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),mNmfSource,"proxy", this->getEventHandlerCB(),2);
    if (error != NMF_OK) NMF_PANIC("Error: unable to bind proxy!...\n");

    error = mNmfSource->bindFromUser("fillthisbuffer" ,mENSComponent.getPort(0)->getBufferCountActual(),&mIfillThisBuffer[0]);
	if (error != NMF_OK) NMF_PANIC("PANIC - bindFromUser etb\n") ;

    error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),mNmfSource,"fillbufferdone",this->getFillBufferDoneCB(),mENSComponent.getPort(0)->getBufferCountActual());
	if (error != NMF_OK) NMF_PANIC("PANIC - bindToUser ebd\n") ;

    OUTR("",(OMX_ErrorNone));
    return OMX_ErrorNone;
}    

OMX_ERRORTYPE FakeSourceProcessingComp::start()
 {
    IN0("");
	//LOS_Log("FakeSourceProcessingComp::start\n", NULL , NULL, NULL, NULL, NULL, NULL);
	mNmfSource->start() ;
    OUTR("",OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::stop()
 {
    IN0("");
	//LOS_Log("FakeSourceProcessingComp::stop\n", NULL , NULL, NULL, NULL, NULL, NULL);
    mNmfSource->stop() ;
    OUTR("",OMX_ErrorNone);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::configure() 
{
    IN0("");
	//LOS_Log("FakeSourceProcessingComp::configure\n", NULL , NULL, NULL, NULL, NULL, NULL);
	OMX_PARAM_PORTDEFINITIONTYPE portDef = mFakePort->getParamPortDefinition();
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = mFakePort->getDirection();
	switch(eDir)
	{
	case OMX_DirInput:
		LOS_Log("FakeSource warning getDirection problem : OMX_DirInput\n");
		break;
	case OMX_DirOutput:
		break;
	default:
		LOS_Log("FakeSource warning getDirection problem : %d\n", eDir);
		break;
	}

	switch(portDef.eDomain)
	{
	case OMX_PortDomainAudio:
		//LOS_Log("FakeSourceProcessingComp::configure domaine detected : OMX_PortDomainAudio\n", NULL , NULL, NULL, NULL, NULL, NULL);
		setparamitf.setParam(0, mFakePort->getBufferCountActual(), eDir, mFakePort->getBufferSupplier(), 0, 0, 0, 0, portDef.format.audio.eEncoding, portDef.nBufferSize, portDef.eDomain);
		break;
	case OMX_PortDomainVideo:
		//LOS_Log("FakeSourceProcessingComp::configure domaine detected : OMX_PortDomainVideo\n", NULL , NULL, NULL, NULL, NULL, NULL);
		setparamitf.setParam(0, mFakePort->getBufferCountActual(), eDir, mFakePort->getBufferSupplier(), portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.format.video.nSliceHeight, portDef.format.video.eColorFormat, portDef.nBufferSize, portDef.eDomain);
		break;
	case OMX_PortDomainImage:
		//LOS_Log("FakeSourceProcessingComp::configure domaine detected : OMX_PortDomainImage\n", NULL , NULL, NULL, NULL, NULL, NULL);
		setparamitf.setParam(0, mFakePort->getBufferCountActual(), eDir, mFakePort->getBufferSupplier(), portDef.format.image.nFrameWidth, portDef.format.image.nFrameHeight, portDef.format.image.nStride, portDef.format.image.nSliceHeight, portDef.format.image.eColorFormat, portDef.nBufferSize, portDef.eDomain);
		break;
	case OMX_PortDomainOther:
		//LOS_Log("FakeSourceProcessingComp::configure domaine detected : OMX_PortDomainOther\n", NULL , NULL, NULL, NULL, NULL, NULL);
		setparamitf.setParam(0, mFakePort->getBufferCountActual(), eDir, mFakePort->getBufferSupplier(), 0, 0, 0, 0, 0, 0, portDef.eDomain);
		break;
	default:
		//LOS_Log("FakeSourceProcessingComp::configure domaine detected : not classic : 0x%x\n", portDef.eDomain , NULL, NULL, NULL, NULL, NULL);
		setparamitf.setParam(0, mFakePort->getBufferCountActual(), eDir, mFakePort->getBufferSupplier(), 0, 0, 0, 0, 0, 0, portDef.eDomain);
		break;
	}

	OMX_INDEXTYPE nParamIndex = (OMX_INDEXTYPE)0;
	OMX_PTR pParameterStructure = NULL;
	nParamIndex = OMX_IndexParamContentURI;
	// file name for data
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexParamContentURI\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	else
	{
		LOS_Log("ERROR FakeSourceProcessingComp::configure CAN NOT call setyourconfigitf.setMyConfig index OMX_IndexParamContentURI\n", NULL , NULL, NULL, NULL, NULL, NULL);
	}
	// file name for extradata
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 1);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexParamContentURI extradata\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}

	nParamIndex = OMX_IndexConfigCaptureMode;
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexConfigCaptureMode\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	else
	{
		LOS_Log("ERROR FakeSourceProcessingComp::configure CAN NOT call setyourconfigitf.setMyConfig index OMX_IndexConfigCaptureMode\n", NULL , NULL, NULL, NULL, NULL, NULL);
	}

	nParamIndex = OMX_IndexConfigCapturing;
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexConfigCapturing\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	else
	{
		LOS_Log("ERROR FakeSourceProcessingComp::configure CAN NOT call setyourconfigitf.setMyConfig index OMX_IndexConfigCapturing\n", NULL , NULL, NULL, NULL, NULL, NULL);
	}

	nParamIndex = OMX_IndexAutoPauseAfterCapture;
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexAutoPauseAfterCapture\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	else
	{
		LOS_Log("ERROR FakeSourceProcessingComp::configure CAN NOT call setyourconfigitf.setMyConfig index OMX_IndexAutoPauseAfterCapture\n", NULL , NULL, NULL, NULL, NULL, NULL);
	}

	nParamIndex = OMX_IndexConfigVideoFramerate;
	pParameterStructure = retriveStructureFromProxy(nParamIndex, 0);
	if (pParameterStructure)
	{
		//LOS_Log("FakeSourceProcessingComp::configure call setyourconfigitf.setMyConfig index OMX_IndexConfigVideoFramerate\n");
		setyourconfigitf.setMyConfig((int)nParamIndex, (void*) pParameterStructure);
	}
	else
	{
		LOS_Log("ERROR FakeSourceProcessingComp::configure CAN NOT call setyourconfigitf.setMyConfig index OMX_IndexConfigVideoFramerate\n", NULL , NULL, NULL, NULL, NULL, NULL);
	}

	OUTR("",(OMX_ErrorNone));
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::deInstantiate() 
{
    IN0("");
	t_nmf_error nmf_err = NMF_OK;
	OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();
	LOS_Log("FakeSourceProcessingComp::deInstantiate\n", NULL , NULL, NULL, NULL, NULL, NULL);
    synchroneyourapiitf.finish();

	nmf_err |= mNmfSource->unbindFromUser("fsminit");
	nmf_err |= mNmfSource->unbindFromUser("sendcommand");
	// do not unbind links which are synchrone
	// do not unbind "synchroneApi" " as it has not been bind ;)
	nmf_err |= mNmfSource->unbindFromUser("setYourConfigure");
	nmf_err |= mNmfSource->unbindFromUser("setParam");
	nmf_err |= mNmfSource->unbindFromUser("fillthisbuffer");
	if (nmf_err != NMF_OK) 
	{
		MSG1("Error: faksource unbindfromUser 0x%x\n", nmf_err);
		OUTR("",(OMX_ErrorHardware));
		return OMX_ErrorHardware;
	}

	nmf_err |= EnsWrapper_unbindToUser(handle, mNmfSource, "proxy");
	nmf_err |= EnsWrapper_unbindToUser(handle, mNmfSource, "fillbufferdone");
	if (nmf_err != NMF_OK) {
		MSG1("Error: mNmfSource unbindToUser 0x%x\n", nmf_err);
		OUTR("",(OMX_ErrorHardware));
		return OMX_ErrorHardware;
	}

	mNmfSource->destroy();
	fake_source_wrpDestroy(mNmfSource);
    OUTR("",(OMX_ErrorNone));
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FakeSourceProcessingComp::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo)
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone; 
	//LOS_Log("buff FakeSourceProcessingComp::useBuffer calls useBuffer\n", NULL , NULL, NULL, NULL, NULL, NULL);

	OMX_PARAM_PORTDEFINITIONTYPE portDef = mFakePort->getParamPortDefinition();
	//LOS_Log("nPortIndex = %d, \n", nPortIndex , NULL, NULL, NULL, NULL, NULL);
	//LOS_Log("nFrameWidth = %d, nFrameHeight = %d, nStride = %d\n, nBufferSize = %d\n", portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.nBufferSize, NULL, NULL);
	//FakeSource__print_color_format(portDef.format.video.eColorFormat);

	OMX_STATETYPE currentState = OMX_StateInvalid;
	fakesource* myFakeSource = (fakesource*)&mENSComponent; 
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = mFakePort->getDirection();
	switch(eDir)
	{
	case OMX_DirInput:
		LOS_Log("FakeSource warning getDirection problem : OMX_DirInput\n");
		break;
	case OMX_DirOutput:
		break;
	default:
		LOS_Log("FakeSource warning getDirection problem : %d\n", eDir);
		break;
	}

	myFakeSource->GetState((ENS_Component_p)myFakeSource, &currentState);
	//FakeSource__print_state(currentState);
	if( currentState == OMX_StateIdle )
	{
		//LOS_Log("\t FakeSourceProcessingComp::useBuffer CAN FORWARD INFO TO PROCESSING COMPONENT!!!!!!!!!!!!!!!!!!!\n");
		setparamitf.setParam(nPortIndex, mFakePort->getBufferCountActual(),eDir, mFakePort->getBufferSupplier(), portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.format.video.nSliceHeight, portDef.format.video.eColorFormat, portDef.nBufferSize, portDef.eDomain);
	}
	else
	{
		//LOS_Log("\t FakeSourceProcessingComp::useBuffer not good state to foward to processing component\n");
	}

#ifdef NO_DEPENDENCY_ON_IFM_NMF
    err = useBufferHeap(nPortIndex, nBufferIndex, pBufferHdr, portPrivateInfo);
#else
	err = IFM_HostNmfProcessingComp::useBuffer(nPortIndex, nBufferIndex, pBufferHdr, portPrivateInfo);
#endif
    OUTR("",(err));
    return err;
}

OMX_ERRORTYPE FakeSourceProcessingComp::useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone; 
	//LOS_Log("buff FakeSourceProcessingComp::useBufferHeader calls useBufferHeader\n", NULL , NULL, NULL, NULL, NULL, NULL);

	OMX_U32 nPortIndex = 0;
	OMX_PARAM_PORTDEFINITIONTYPE portDef = mFakePort->getParamPortDefinition();
	//LOS_Log("nPortIndex = %d, \n", nPortIndex , NULL, NULL, NULL, NULL, NULL);
	//LOS_Log("nFrameWidth = %d, nFrameHeight = %d, nStride = %d\n, nBufferSize = %d\n", portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.nBufferSize, NULL, NULL);
	//FakeSource__print_color_format(portDef.format.video.eColorFormat);

	OMX_STATETYPE currentState = OMX_StateInvalid;
	fakesource* myFakeSource = (fakesource*)&mENSComponent; 
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = mFakePort->getDirection();
	switch(eDir)
	{
	case OMX_DirInput:
		LOS_Log("FakeSource warning getDirection problem : OMX_DirInput\n");
		break;
	case OMX_DirOutput:
		break;
	default:
		LOS_Log("FakeSource warning getDirection problem : %d\n", eDir);
		break;
	}
	myFakeSource->GetState((ENS_Component_p)myFakeSource, &currentState);
	//FakeSource__print_state(currentState);
	if( currentState == OMX_StateIdle )
	{
		//LOS_Log("\t FakeSourceProcessingComp::useBufferHeader CAN FORWARD INFO TO PROCESSING COMPONENT!!!!!!!!!!!!!!!!!!!\n");
		setparamitf.setParam(nPortIndex, mFakePort->getBufferCountActual(),eDir, mFakePort->getBufferSupplier(), portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.format.video.nSliceHeight, portDef.format.video.eColorFormat, portDef.nBufferSize, portDef.eDomain);
	}
	else
	{
		//LOS_Log("\t FakeSourceProcessingComp::useBufferHeader not good state to foward to processing component\n");
	}

#ifdef NO_DEPENDENCY_ON_IFM_NMF
	err = useBufferHeaderHeap(dir, pBuffer);
#else
	err = IFM_HostNmfProcessingComp::useBufferHeader(dir, pBuffer);
#endif
    OUTR("",(err));
    return err;
}    

OMX_ERRORTYPE FakeSourceProcessingComp::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo)
{
    IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone; 
	//LOS_Log("buff FakeSourceProcessingComp::allocateBuffer calls allocateBuffer, nPortIndex = %d\n", nPortIndex , NULL, NULL, NULL, NULL, NULL);
#ifdef NO_DEPENDENCY_ON_IFM_NMF
	err = allocateBufferHeap(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);
#else
	err = IFM_HostNmfProcessingComp::allocateBuffer(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);
#endif
    OUTR("",(err));
    return err;
}
OMX_ERRORTYPE FakeSourceProcessingComp::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo)
{
    IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone; 
	//LOS_Log("buff FakeSourceProcessingComp::freeBuffer calls freeBuffer, nPortIndex = %d\n", nPortIndex , NULL, NULL, NULL, NULL, NULL);
#ifdef NO_DEPENDENCY_ON_IFM_NMF
	err = freeBufferHeap(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
#else
	err = IFM_HostNmfProcessingComp::freeBuffer(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
#endif
    OUTR("",(err));
    return err;
}

OMX_ERRORTYPE fakesource::createResourcesDB() 
{
    IN0("");
    mRMP = new fakesource_RDB;
    if (mRMP == 0) 
    {
        OUTR("",(OMX_ErrorInsufficientResources));
        return OMX_ErrorInsufficientResources;
    } 
    else 
    {
        OUTR("",(OMX_ErrorNone));
        return OMX_ErrorNone;
    }
}

fakesource_RDB::fakesource_RDB() : ENS_ResourcesDB(1)
{
	IN0("");
    setDefaultNMFDomainType(RM_NMFD_PROCSIA);
	OUTR("",(0));
}

