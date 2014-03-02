/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "wom_shared.h"
//Block some includes
#define COMMON_INTERFACE_TYPE_H
#define COMMON_STUB_REQUIREAPI_H
#define PROXY_COMMUNICATION_H

#include <omxil/OMX_Component.h>
#include "ImgEns_Component_Fsm.h"
#include "ImgEns_Port.h"
#include "wom_tools.h"
#include "wom_port.h"

#undef    OMXCOMPONENT
#define   OMXCOMPONENT "Wom_PortOther"

#define EXTRADATA_STILLPACKSIZE 1 << 16

WOM_API Wom_PortOther::Wom_PortOther(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComponent)
	: ImgEns_Port(commonPortData, ensComponent)
	, m_Omx_Component ((ImgEns_Component& )ensComponent)
	, m_bReserveExtradataForInput(false)
//*************************************************************************************************************
{
	m_buffersPool = NULL;

	mParamPortDefinition.bPopulated         = OMX_FALSE;
	mParamPortDefinition.eDomain            = OMX_PortDomainOther;
	mParamPortDefinition.nBufferSize        = commonPortData.mBufferSizeMin;
	mParamPortDefinition.nSize              = sizeof(mParamPortDefinition);
	mParamPortDefinition.bBuffersContiguous = OMX_FALSE;
	mParamPortDefinition.nBufferAlignment   = 0;
	//m_ImageInfo.Init();
}

WOM_API Wom_PortOther::~Wom_PortOther()
//*************************************************************************************************************
{

}

WOM_API OMX_ERRORTYPE Wom_PortOther::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
//*************************************************************************************************************
{
	OMX_ERRORTYPE error=OMX_ErrorNone;
	mParamPortDefinition.format.other = portdef.format.other;
	//Inform the component that port settings change
	//if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
	//	error=OMX_ErrorUnsupportedSetting;
	return(error);
}

WOM_API OMX_ERRORTYPE Wom_PortOther::checkCompatibility(OMX_HANDLETYPE /*hTunneledComponent*/, OMX_U32 /*nTunneledPort*/) const
//*************************************************************************************************************
{ //Do nothing interesting for the moment
	OMX_ERRORTYPE error=OMX_ErrorNone;
	return(error);
}

WOM_API OMX_VIDEO_CODINGTYPE Wom_PortOther::Get_CompressionFormat() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.eCompressionFormat);
	case OMX_PortDomainImage:
		return((OMX_VIDEO_CODINGTYPE) mParamPortDefinition.format.image.eCompressionFormat);
	default:
		// ASSERT(0);
		return(OMX_VIDEO_CodingUnused);
	}
}

WOM_API OMX_COLOR_FORMATTYPE Wom_PortOther::Get_ColorFormat() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.eColorFormat);
	case OMX_PortDomainImage:
		return(mParamPortDefinition.format.image.eColorFormat);
	default:
		// ASSERT(0);
		return(OMX_COLOR_FormatUnused);
	}
}

WOM_API OMX_NATIVE_WINDOWTYPE Wom_PortOther::Get_NativeWindow() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.pNativeWindow);
	case OMX_PortDomainImage:
		return(mParamPortDefinition.format.image.pNativeWindow);
	default:
		// ASSERT(0);
		return(NULL);
	}
}

WOM_API OMX_U32 Wom_PortOther::Get_Bitrate() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.nBitrate);
	case OMX_PortDomainImage: //No bitrate for image
	default:
		// ASSERT(0);
		return(0);
	}
}

WOM_API OMX_BOOL Wom_PortOther::Get_FlagErrorConcealment() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.bFlagErrorConcealment);
	case OMX_PortDomainImage:
		return(mParamPortDefinition.format.image.bFlagErrorConcealment);
	default:
		// ASSERT(0);
		return(OMX_FALSE);
	}
}

WOM_API OMX_U32 Wom_PortOther::Get_Framerate() const
//*************************************************************************************************************
{
	switch(mParamPortDefinition.eDomain)
	{
	case OMX_PortDomainVideo:
		return(mParamPortDefinition.format.video.xFramerate);
	case OMX_PortDomainImage: // No framerate for image
	default:
		// ASSERT(0);
		return(0);
	}
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT "Wom_PortVideo"

WOM_API Wom_PortVideo::Wom_PortVideo(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComp)
: Wom_PortOther(commonPortData, ensComp)
//*************************************************************************************************************
{
	m_bReserveExtradataForInput=false;
	mParamPortDefinition.eDomain    = OMX_PortDomainVideo;
	mParamPortDefinition.nBufferSize= 0;

	mParamPortDefinition.format.video.cMIMEType            = ( char *)"";
	mParamPortDefinition.format.video.pNativeRender        = NULL;
	mParamPortDefinition.format.video.nFrameWidth          = 0;
	mParamPortDefinition.format.video.nFrameHeight         = 0;
	mParamPortDefinition.format.video.nStride              = 0;
	mParamPortDefinition.format.video.nSliceHeight         = 2;
	mParamPortDefinition.format.video.nBitrate             = 0;
	mParamPortDefinition.format.video.xFramerate           = 0;
	mParamPortDefinition.format.video.bFlagErrorConcealment= OMX_FALSE;
	mParamPortDefinition.format.video.eCompressionFormat   = OMX_VIDEO_CodingUnused;
	mParamPortDefinition.format.video.eColorFormat         = OMX_COLOR_FormatUnused;
	mParamPortDefinition.format.video.pNativeWindow        = NULL;
}

WOM_API Wom_PortVideo::~Wom_PortVideo()
//*************************************************************************************************************
{
	;
}

WOM_API OMX_ERRORTYPE Wom_PortVideo::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
//*************************************************************************************************************
{
	OMX_ERRORTYPE error=OMX_ErrorNone;
	OMX_VIDEO_PORTDEFINITIONTYPE &Video=mParamPortDefinition.format.video;
	Video = portdef.format.video;

	// NB: Camera ports definition will remain of video type.
	if (portdef.eDomain==OMX_PortDomainVideo)
	{ //make a normal copy
		mParamPortDefinition.eDomain = OMX_PortDomainVideo; //Stay in video
		Video = portdef.format.video;
	}
	else if (portdef.eDomain==OMX_PortDomainImage)
	{ //Make a morphing to video domain
		mParamPortDefinition.eDomain = OMX_PortDomainVideo; //Stay in video
		Video.cMIMEType              = portdef.format.image.cMIMEType;
		Video.pNativeRender          = portdef.format.image.pNativeRender;
		Video.nFrameWidth            = portdef.format.image.nFrameWidth;
		Video.nFrameHeight           = portdef.format.image.nFrameHeight;
		Video.nStride                = portdef.format.image.nStride;
		Video.nSliceHeight           = portdef.format.image.nSliceHeight; //This is NO MORE areadonly field
		Video.nBitrate               = 0;
		Video.xFramerate             = 0;
		Video.bFlagErrorConcealment  = portdef.format.image.bFlagErrorConcealment;
		Video.eCompressionFormat     = (OMX_VIDEO_CODINGTYPE)portdef.format.image.eCompressionFormat;
		Video.eColorFormat           = portdef.format.image.eColorFormat;
		Video.pNativeWindow          = portdef.format.image.pNativeWindow;
	}
	else
	{
		error=OMX_ErrorBadParameter;
	}

	if (Video.nSliceHeight < Video.nFrameHeight)
	{
		// MSG1("Update SliceHeight to %u\n", (unsigned int)Video.nFrameHeight);
		Video.nSliceHeight= Video.nFrameHeight;
	}


	if (Video.nSliceHeight < Video.nFrameHeight)
	{
		// MSG2("Invalid SliceHeight=%u and nFrameHeight=%u\n", (unsigned int)Video.nSliceHeight, (unsigned int)Video.nFrameHeight);
		Video.nSliceHeight= Video.nFrameHeight;
		error=OMX_ErrorBadParameter;
	}

	const _sImageFormatDescription* pFormatDesc= GetImageFormatDescription(Video.eColorFormat);
	WOM_ASSERT(pFormatDesc);
	OMX_U32 ComputedBufferSize=0;
	if(( pFormatDesc!=NULL) && ( pFormatDesc->NbPlan!=0) )
	{ //Known format

		int ComputedStride=(int) (pFormatDesc->PixelDepth0 * Video.nFrameWidth);
		if (Video.nStride <=0)
		{ //automatic stride
			Video.nStride=ComputedStride;
		}
		if ( Video.nStride < ComputedStride)
		{ //Invalid stride take right one
			error=OMX_ErrorBadParameter;
			Video.nStride=ComputedStride;
		}
		m_ImageInfo.pFormatDescription= pFormatDesc;
		m_ImageInfo.StrideInPixel     = (int) (Video.nStride / pFormatDesc->PixelDepth0);
		m_ImageInfo.ImageSize         = (int) ((Video.nStride * Video./*nFrameHeight*/nSliceHeight * pFormatDesc->OverallPixelDepth) / pFormatDesc->PixelDepth0);
		ComputedBufferSize= m_ImageInfo.ImageSize;
	}
	else
	{ //Invalid format
		//Compute pixel depth and stride using default parameters
		WOM_ASSERT(0);
		if(( pFormatDesc!=NULL) && (pFormatDesc->Name!=NULL))
		{
			//Component.ReportError(-1, "Port[%d]  format '%s'=%d is NOT supported\n", getPortIndex(), pFormatDesc->Name , Video.eColorFormat);
		}
		else
		{
			//Component.ReportError(-1, "Port[%d]  format %d is NOT supported\n", getPortIndex(), Video.eColorFormat);
		}
		error=OMX_ErrorFormatNotDetected;
		m_ImageInfo.Init();

		float bytesPerPixel = 3.; //Take biggest one
		int ComputedStride=(int) (bytesPerPixel * Video.nFrameWidth);
		if (Video.nStride <=0)
		{ //automatic stride
			Video.nStride=ComputedStride;
		}
	#if 0
		else if (ComputedStride > Video.nStride)
		{ //Given ComputedStride is invalid for this format
			Video.nStride=ComputedStride; //fix the stride to minimal value
			ImgEns_Component& Component=(ImgEns_Component&)getENSComponent();
			Component.ReportError(OMX_ErrorBadParameter, "Stride to small for port[%d]", getPortIndex());
			error=OMX_ErrorBadParameter;
		}
	#endif
		ComputedBufferSize = (OMX_U32) ( bytesPerPixel * Video.nFrameWidth * Video./*nFrameHeight*/nSliceHeight);
	}

	if (ComputedBufferSize != 0)
	{
		//ER Bug with external splitter when extradata space has been added to input port
		if ((mParamPortDefinition.eDir==OMX_DirOutput) || (m_bReserveExtradataForInput==true))
		{ //For the moment add extradata more space ....
			ComputedBufferSize += EXTRADATA_STILLPACKSIZE;
		}
		if (ComputedBufferSize == 0)
		{
			// MSG0("Calculated a NULL buffer size ! This should not occur \n");
			error=OMX_ErrorFormatNotDetected;
		}
		mParamPortDefinition.nBufferSize = ComputedBufferSize;
	}
	else
	{ ///Keep current size...
		//ImgEns_Component& Component=(ImgEns_Component&)getENSComponent();
		//Component.ReportError(-1, "Port[%d] Compute a null size", getPortIndex());
		error=OMX_ErrorBadParameter;
	}

	//Inform the component that port settings change
/* LR_CHANGE a remettre
	if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
		error=OMX_ErrorUnsupportedSetting;
*/
	return error;
}
