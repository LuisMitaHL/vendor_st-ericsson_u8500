/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#undef    OMXCOMPONENT
#define   OMXCOMPONENT ""
#include "osi_trace.h"

#undef    OMX_TRACE_UID
#define   OMX_TRACE_UID 0x8
#include "ENS_Component_Fsm.h"
#include "cm/inc/cm_macros.h"
#include "mmhwbuffer.h"
#include "mmhwbuffer_ext.h"
#include "extradata.h" // Needed for extradata space reservation

#include "../include/wrapper_openmax_tools.h"
#include "../include/openmax_component.h"
#include "../include/openmax_component_proxy.h"
/**
@class Wrapper_OpenMax
@image html ..\misc\Overview.svg "schematic of classes encapsulation"
*/
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT "PortOther"

WRAPPER_OPENMAX_API PortOther::PortOther(const EnsCommonPortData& commonPortData, ENS_Component& ensComponent)
	: ENS_Port(commonPortData, ensComponent)
	, m_Omx_Component ((OpenMax_Component& )ensComponent)
	, m_bReserveExtradataForInput(false)
//*************************************************************************************************************
{
	buffersPool = NULL;

	mParamPortDefinition.bPopulated         = OMX_FALSE;
	mParamPortDefinition.eDomain            = OMX_PortDomainOther;
	mParamPortDefinition.nBufferSize        = commonPortData.mBufferSizeMin;
	mParamPortDefinition.nSize              = sizeof(mParamPortDefinition);
	mParamPortDefinition.bBuffersContiguous = OMX_FALSE;
	mParamPortDefinition.nBufferAlignment   = 0;
	m_ImageInfo.Init();
}

WRAPPER_OPENMAX_API PortOther::~PortOther()
//*************************************************************************************************************
{

}

WRAPPER_OPENMAX_API OMX_ERRORTYPE PortOther::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
//*************************************************************************************************************
{
	OMX_ERRORTYPE error=OMX_ErrorNone;
	mParamPortDefinition.format.other = portdef.format.other;
	//Inform the component that port settings change
	if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
		error=OMX_ErrorUnsupportedSetting;
	return(error);
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE PortOther::checkCompatibility(OMX_HANDLETYPE /*hTunneledComponent*/, OMX_U32 /*nTunneledPort*/) const
//*************************************************************************************************************
{ //Do nothing interesting for the moment
	IN0("\n");
	OMX_ERRORTYPE error=OMX_ErrorNone;
	OUT0("\n");
	return(error);
}

WRAPPER_OPENMAX_API OMX_VIDEO_CODINGTYPE PortOther::Get_CompressionFormat() const
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

WRAPPER_OPENMAX_API OMX_COLOR_FORMATTYPE PortOther::Get_ColorFormat() const
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

WRAPPER_OPENMAX_API OMX_NATIVE_WINDOWTYPE PortOther::Get_NativeWindow() const
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

WRAPPER_OPENMAX_API OMX_U32 PortOther::Get_Bitrate() const
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

WRAPPER_OPENMAX_API OMX_BOOL PortOther::Get_FlagErrorConcealment() const
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

WRAPPER_OPENMAX_API OMX_U32 PortOther::Get_Framerate() const
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
#define   OMXCOMPONENT "PortVideo"

WRAPPER_OPENMAX_API PortVideo::PortVideo(const EnsCommonPortData& commonPortData, ENS_Component& ensComp)
: PortOther(commonPortData, ensComp)
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

WRAPPER_OPENMAX_API PortVideo::~PortVideo()
//*************************************************************************************************************
{
	;
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE PortVideo::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
//*************************************************************************************************************
{
	IN0("\n");
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
		MSG1("Update SliceHeight to %u\n", (unsigned int)Video.nFrameHeight);
		Video.nSliceHeight= Video.nFrameHeight;
	}


	if (Video.nSliceHeight < Video.nFrameHeight)
	{
		MSG2("Invalid SliceHeight=%u and nFrameHeight=%u\n", (unsigned int)Video.nSliceHeight, (unsigned int)Video.nFrameHeight);
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
			OpenMax_Component& Component=(OpenMax_Component&)getENSComponent();
			Component.ReportError(-1, "Port[%d] Invalid stride %d need at least %d", getPortIndex(), Video.nStride, ComputedStride);
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
		OpenMax_Component& Component=(OpenMax_Component&)getENSComponent();
		if(( pFormatDesc!=NULL) && (pFormatDesc->Name!=NULL))
			Component.ReportError(-1, "Port[%d]  format '%s'=%d is NOT supported\n", getPortIndex(), pFormatDesc->Name , Video.eColorFormat);
		else
			Component.ReportError(-1, "Port[%d]  format %d is NOT supported\n", getPortIndex(), Video.eColorFormat);
		error=OMX_ErrorFormatNotDetected;
		m_ImageInfo.Init();

		float bytesPerPixel = ComputePixelDepth(Video.eColorFormat);
		int ComputedStride=(int) (bytesPerPixel * Video.nFrameWidth);
		if (Video.nStride <=0)
		{ //automatic stride
			Video.nStride=ComputedStride;
		}
	#if 0
		else if (ComputedStride > Video.nStride)
		{ //Given ComputedStride is invalid for this format
			Video.nStride=ComputedStride; //fix the stride to minimal value
			OpenMax_Component& Component=(OpenMax_Component&)getENSComponent();
			Component.ReportError(OMX_ErrorBadParameter, "Stride to small for port[%d]", getPortIndex());
			error=OMX_ErrorBadParameter;
		}
	#endif
		ComputedBufferSize = (OMX_U32) ( bytesPerPixel * Video.nFrameWidth * Video./*nFrameHeight*/nSliceHeight);
	}

	if (ComputedBufferSize != 0)
	{
		//ER Bug with nokia splitter when extradata space has been added to input port 
		if ((mParamPortDefinition.eDir==OMX_DirOutput) || (m_bReserveExtradataForInput==true))
		{ //For the moment add extradata more space ....
			ComputedBufferSize += EXTRADATA_STILLPACKSIZE;
		}
		if (ComputedBufferSize == 0) 
		{
			MSG0("Calculated a NULL buffer size ! This should not occur \n");
			error=OMX_ErrorFormatNotDetected;
		}
		mParamPortDefinition.nBufferSize = ComputedBufferSize;
	}
	else
	{ ///Keep current size...
		OpenMax_Component& Component=(OpenMax_Component&)getENSComponent();
		Component.ReportError(-1, "Port[%d] Compute a null size", getPortIndex());
		error=OMX_ErrorBadParameter;
	}

	//Inform the component that port settings change
	if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
		error=OMX_ErrorUnsupportedSetting;

	OUTR(" ", error);
	return error;
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT m_ComponentName

OpenMax_Component::OpenMax_Component(const char *name)
: m_ComponentName(name==NULL?"OpenMax_Component": name)
, m_bUseBufferSharing(false)
//*************************************************************************************************************
{
	GET_AND_SET_TRACE_LEVEL(wrapper);
	IN0("\n");
	m_LastError=eError_NoError;
	OUT0("\n");
}

OpenMax_Component::~OpenMax_Component()
//*************************************************************************************************************
{
	IN0("\n");
	OUT0("\n");
}

#if 1 //Imply dependance with ifm
OMX_ERRORTYPE OpenMax_Component::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
//*************************************************************************************************************
{ // Manage specific index
	if (OpenMax_Component::getExtensionIndex(cParameterName, pIndexType)!= OMX_ErrorNotImplemented)
		return(OMX_ErrorNone);
	return(ENS_Component::getExtensionIndex(cParameterName, pIndexType)); //Call base class
}
#endif

int OpenMax_Component::ReportError(int error, const char *format, ...)
//*************************************************************************************************************
{
	IN0("\n");
	if (error == eError_NoError)
		return(0);
	if (m_LastError ==eError_NoError)
		m_LastError=error;  //Memorize the error
	va_list list;
	va_start(list, format);

#ifndef __SYMBIAN32__
		fprintf (stderr, "\n<Error-%s: %d=0x%X> ", m_ComponentName, error, error);
		vfprintf(stderr, format, list);
#else
		static char ErrorString[1024];
		vsprintf(ErrorString, format, list);
		RDebug::Printf("\n<Error-%s: %d=0x%X: %s>", m_ComponentName, error, error, ErrorString);
#endif
	va_end(list);


	OUT0("\n");
	return(0);
}

/**
Create relevant port versus domain
*/
int OpenMax_Component::createPort (const EnsCommonPortData &aPortData)
//*************************************************************************************************************
{
	IN0("\n");
	if (aPortData.mPortIndex >= getPortCount())
	{
		ReportError(OMX_ErrorBadPortIndex, "Cannot create port %d >= %d\n", aPortData.mPortIndex, getPortCount());
		OUTR(" ", OMX_ErrorBadPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	ENS_Port* port=NULL;
	switch(aPortData.mPortDomain)
	{
	case OMX_PortDomainVideo:
		port= new PortVideo(aPortData, *this);
		break;
	case OMX_PortDomainOther:
		port= new PortOther(aPortData, *this);
		break;
	default:
		// not supported today: OMX_PortDomainAudio, OMX_PortDomainImage, OMX_PortDomainOther,OMX_PortDomainKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
		ReportError(eError_UnsupportedDomain, "Domain %d not supported\n", aPortData.mPortDomain);
		port=NULL;
		OUTR(" ", eError_UnsupportedDomain);
		return eError_UnsupportedDomain;
	}
	if (port == 0)
	{
		ReportError(OMX_ErrorInsufficientResources, "creation of port %d fails\n", aPortData.mPortIndex);
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	else
		addPort(port);

	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE OpenMax_Component::createResourcesDB()
//*************************************************************************************************************
{
	IN0("\n");
	mRMP = new OpenMax_RDB();
	if (mRMP == 0)
	{
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	else
	{
		OUTR(" ", (OMX_ErrorNone));
		return OMX_ErrorNone;
	}
}

#if 0
WRAPPER_OPENMAX_API RM_STATUS_E OpenMax_Component::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData) 
//*************************************************************************************************************
{
	// resource estimation for Camera. See SAS.
	/* TODO: nSiaHwPipe value and eUccBitmap should depend on use-case!(720p? 1080p? Stab?...) */
	pEstimationData->sMcpsSva.nMax         = 0;
	pEstimationData->sMcpsSva.nAvg         = 0;
	pEstimationData->sMcpsSia.nMax         = 3;  // 3 MIPS estimated for GRAB/GRAB DD/ISPCTL. You might have more concrete data (I would expect much less, this is assumed conservative)
	pEstimationData->sMcpsSia.nAvg         = 3;  // same as max
	pEstimationData->sTcmSva.nSizeX        = 0;
	pEstimationData->sTcmSva.nSizeY        = 0;
	pEstimationData->sTcmSia.nSizeX        = 4096; // you should be able to extract that from ELF. In SAS, this is said 3.9kB for X & Y
	pEstimationData->sTcmSia.nSizeY        = 0; // idem
	pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_EMPTY;
	pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIG;
	pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_DEFAULT;
	pEstimationData->eUccBitmap            = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;
	return RM_E_NONE;
}
#else
WRAPPER_OPENMAX_API RM_STATUS_E  OpenMax_Component::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData)
//*************************************************************************************************************
{ //Call base class
	return(ENS_Component::getResourcesEstimation (pCompHdl, pEstimationData));
}
#endif

RM_STATUS_E OpenMax_Component::getCapabilities(OMX_INOUT OMX_U32 &pCapBitmap)
//*************************************************************************************************************
{ //Declare that we work in SIA domain
	pCapBitmap = RM_CAP_DISTUSESIA;
	return RM_E_NONE;
}

#ifdef __SYMBIAN32__
// value is 0x7F000012=2130706450
const OMX_INDEXTYPE IndexConfigSharedChunkMetadata= (OMX_INDEXTYPE)OMX_MMHwBufferIndexConfigSharedChunkMetadata;
#else
// Value is 0x7FD19E5C = 2144443996
const OMX_INDEXTYPE IndexConfigSharedChunkMetadata= (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata;
#endif

/**
Default implementation make a copy of the structur inside the class. If the component has already instantiate the
nmf part of the component, applyConfig will be called for having a mirroring in the host part.
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Component::setConfig(OMX_INDEXTYPE nIndex, OMX_PTR pStructure)
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (pStructure==NULL)
	{
		ReportError(eError_NullPointer, "Null pointer for getConfig");
		OUTR(" ", OMX_ErrorBadParameter);
		return(OMX_ErrorBadParameter);
	}
	size_t Size=0;
	int Offset=0;
	OMX_PTR pConfig=getConfigAddr( nIndex, &Size, &Offset);
	if (pConfig !=NULL)
	{ //Make the copy in the local struct
		memcpy(pConfig, (char *)pStructure+Offset, Size);
	}
	else
	{
		err = ENS_Component::setConfig(nIndex, pStructure);
	}
	if (err!= OMX_ErrorNone)
	{
		if (nIndex == IndexConfigSharedChunkMetadata)
		{
			_fnReport(eReport_Warning, "'%s'.SetConfig(%d=0x%X=SharedChunkMetadata) Ignored !!!", GetComponentName(), nIndex, nIndex);
		}
		else
		{
			_fnReport(eReport_Warning, "'%s'.SetConfig(%d=0x%X) Unsupported index !!!", GetComponentName(), nIndex, nIndex);
		}
	}
	OUTR(" ", (err));
	return err;
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Component::getConfig(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (pStructure==NULL)
	{
		// ReportError(OMX_ErrorBadParameter, "ERROR in setParameter : pComponentParameterStructure is null\n");
		OUTR(" ", OMX_ErrorBadParameter);
		return(OMX_ErrorBadParameter);
	}

	int Offset=0;
	size_t Size=0;
	OMX_PTR pConfig=getConfigAddr( nIndex, &Size, &Offset);
	if (pConfig !=NULL)
	{ 
		memcpy((char *)pStructure+Offset, pConfig, Size);
	}
	else
	{
		err = ENS_Component::getConfig(nIndex, pStructure);
	}
	if (err!= OMX_ErrorNone)
	{
		if (nIndex == IndexConfigSharedChunkMetadata)
		{
			_fnReport(eReport_Warning, "'%s'.GetConfig(%d=0x%X=SharedChunkMetadata) Ignored !!!", GetComponentName(), nIndex, nIndex);
		}
		else
		{
			_fnReport(eReport_Warning, "'%s'.GetConfig(%d=0x%X) Unsupported index !!!", GetComponentName(), nIndex, nIndex);
		}
	}
	OUTR(" ", (err));
	return err;
}

WRAPPER_OPENMAX_API OMX_PTR OpenMax_Component::getConfigAddr(OMX_INDEXTYPE nIndex, size_t *pStructSize, int *pOffset) const
//*************************************************************************************************************
{ //return the address of the config 
	IN0("\n");
	size_t size=0;
	OMX_PTR pStructure=NULL;
	switch ((t_uint32)nIndex)
	{
	case ENS_IndexConfigTraceSetting:
		break; //this index is managed by default in ENS
	default :
		pStructure = NULL;
		size=0;
		break;
	}
	if (pStructSize)
	{ //return the size of the struct
		*pStructSize=size;
	}
	return pStructure;
}


WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Component::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (pComponentParameterStructure == 0)
	{
		ReportError(OMX_ErrorBadParameter, "ERROR in setParameter : pComponentParameterStructure is null\n");
		OUTR(" ", (OMX_ErrorBadParameter));
		return OMX_ErrorBadParameter;
	}

	switch (nParamIndex) 
	{
	default :
		err = ENS_Component::setParameter(nParamIndex, pComponentParameterStructure);
		break;
	}
	OUTR(" ", (err));
	return err;
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Component::getParameter(OMX_INDEXTYPE idx, OMX_PTR p) const
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	switch (idx)
	{
	default :
		err = ENS_Component::getParameter(idx, p);
		break;
	}

	OUTR(" ", (err));
	return err;
}

/**
Called just at the end of fsmInit and allow to do basic initialisation of the processing component
*/
WRAPPER_OPENMAX_API int OpenMax_Component::NMFConfigure()
//*************************************************************************************************************
{ /// Make the constuction of internal structur. Called at the end of the fsminit and give a change
  /// to make initialisation of the processor
	IN0("\n");
	int result=S_OK;
	OUTR(" ", result);
	return(result);
}

/// call when a set param has been done on a port
WRAPPER_OPENMAX_API int OpenMax_Component::PortFormatChanged(ENS_Port & /*port*/)
//*************************************************************************************************************
{ //By default done nothing
	return(S_OK);
}


WRAPPER_OPENMAX_API OpenMax_Proxy & OpenMax_Component::GetProxy()
//*************************************************************************************************************
{ //Give a better name for this framework
	return (OpenMax_Proxy &) getProcessingComponent();
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT "OpenMax_RDB"


OpenMax_RDB::OpenMax_RDB() : ENS_ResourcesDB()
//*************************************************************************************************************
{
	IN0("\n");
	setDefaultNMFDomainType(RM_NMFD_HWPIPESIA);
	OUT0("\n");
}

