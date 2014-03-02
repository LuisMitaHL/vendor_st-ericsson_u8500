/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4enc
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_ProxyTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#include "MPEG4Enc_Proxy.h"
#include "MPEG4Enc_Port.h"
#include "video_chipset_api_index.h"

#define NOTIFY_USER
#define ARMNMF_OBJECT 1
#define MPCNMF_OBJECT 2

MPEG4Enc_Proxy::MPEG4Enc_Proxy():VFM_Component(&mParam),mParam(this)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_Proxy::MPEG4Enc_Proxy() constructor");

#ifdef HOST_MPC_COMPONENT_BOTH
	if (mpcProcessingComponent)
	{
		mSendParamToMpc.set();
		isMpcObject = OMX_TRUE;
	}
#endif
	if (armProcessingComponent)
	{
		mSendParamToArmNmf.set();
		isMpcObject = OMX_FALSE;
	}

	mIsARMLoadComputed = OMX_FALSE;

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_Proxy::MPEG4Enc_Proxy() constructor");

}

OMX_ERRORTYPE MPEG4Enc_Proxy::construct()
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_Proxy::construct()");
	
	RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
#ifdef HOST_MPC_COMPONENT_BOTH
	if (isMpcObject)
	{
		setProcessingComponent(mpcProcessingComponent);
		RETURN_OMX_ERROR_IF_ERROR(mpcProcessingComponent->setPortCount(2));
	}
	else
#endif
	{
		setProcessingComponent(armProcessingComponent);
		RETURN_OMX_ERROR_IF_ERROR(armProcessingComponent->setPortCount(2));
	}

#ifdef HOST_MPC_COMPONENT_BOTH
	if (isMpcObject)
	{
		mpcProcessingComponent->setPriorityLevel((t_uint32)NMF_SCHED_URGENT);
	}
#endif
	ENS_Port * portInput = newPort(
								  EnsCommonPortData(
												   VPB+0, 
												   OMX_DirInput, 
												   1/*nBufferCountMin*/, 
												   0/*nBufferSizeMin*/, 
												   OMX_PortDomainVideo, 
												   OMX_BufferSupplyInput), 
								  *this);
	if(portInput == NULL)
	{
		OstTraceInt1(TRACE_ERROR, "construct(): Error OMX_ErrorInsufficientResources for newPort for input port at line no. %d", __LINE__);
		return OMX_ErrorInsufficientResources;
	}

	ENS_Port * portOutput = newPort(
								   EnsCommonPortData(
													VPB+1, 
													OMX_DirOutput, 
													1/*nBufferCountMin*/, 
													0/*nBufferSizeMin*/, 
													OMX_PortDomainVideo, 
													OMX_BufferSupplyOutput), 
								   *this);
	if(portOutput == NULL)
	{
		OstTraceInt1(TRACE_ERROR, "construct(): Error OMX_ErrorInsufficientResources for newPort for output port at line no. %d", __LINE__);
		delete portInput;
		return OMX_ErrorInsufficientResources;
	}

	//Add only when both ports are successfully allocated
	addPort(portInput);
	addPort(portOutput);

	// set the default value of the ports
	((MPEG4Enc_Port *)getPort(VPB+0))->setDefault();
	((MPEG4Enc_Port *)getPort(VPB+1))->setDefault();

	mParam.setInitialCropParams();

   // setTraceInfo must be called after VFM_Component::construct (only for ParamAndConfig)
	unsigned int  id = getId1();
	TraceInfo_t * traceInfoPtr = getTraceInfoPtr();
	if (traceInfoPtr) {
		mParam.setTraceInfo(traceInfoPtr, id);
	}
	else {
		DBC_ASSERT(0) ;
	}

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_Proxy::construct()");
	
	return OMX_ErrorNone;
}

MPEG4Enc_Proxy::~MPEG4Enc_Proxy()
{
	OstTraceInt0(TRACE_API, "<=> MPEG4Enc_Proxy::~MPEG4Enc_Proxy() destructor");
}

VFM_Port *MPEG4Enc_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
	OstTraceInt1(TRACE_FLOW, "MPEG4Enc_Proxy::newPort(): Creating port, allocating memory of size %d", sizeof(MPEG4Enc_Port));
	return new MPEG4Enc_Port(commonPortData, enscomp);
}

OMX_ERRORTYPE MPEG4Enc_Proxy::MPEG4Enc_Processing(OMX_U32 value)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_Proxy::MPEG4Enc_Processing()");
	
	OMX_ERRORTYPE error = OMX_ErrorUndefined;

	if (value == ARMNMF_OBJECT)
	{
		isMpcObject = OMX_FALSE;
		mSendParamToArmNmf.set();
		OstTraceInt1(TRACE_FLOW, "MPEG4Enc_Proxy::MPEG4Enc_Processing(): Creating Processing Component for ARM, allocating memory of size %d", sizeof(MPEG4Enc_ArmNmf_ProcessingComponent));
		armProcessingComponent = new MPEG4Enc_ArmNmf_ProcessingComponent(*this);
		if (armProcessingComponent ==NULL)
		{
			OstTraceInt1(TRACE_ERROR, "MPEG4Enc_Proxy::MPEG4Enc_Processing(): Unable to create MPEG4Enc_ArmNmf_ProcessingComponent (OMX_ErrorInsufficientResources), line no %d", __LINE__);
			return OMX_ErrorInsufficientResources;
		}


#ifdef HOST_MPC_COMPONENT_BOTH
		mpcProcessingComponent = NULL;
#endif
		error = OMX_ErrorNone;
	}
#ifdef HOST_MPC_COMPONENT_BOTH
	if (value == MPCNMF_OBJECT)
	{
		isMpcObject = OMX_TRUE;
		mSendParamToMpc.set();
		OstTraceInt1(TRACE_FLOW, "MPEG4Enc_Proxy::MPEG4Enc_Processing(): Creating Processing Component for MPC, allocating memory of size %d", sizeof(MPEG4Enc_NmfMpc_ProcessingComponent));
		mpcProcessingComponent = new MPEG4Enc_NmfMpc_ProcessingComponent(*this);
		if (mpcProcessingComponent ==NULL)
		{
			OstTraceInt1(TRACE_ERROR, "MPEG4Enc_Proxy::MPEG4Enc_Processing(): Unable to create MPEG4Enc_NmfMpc_ProcessingComponent (OMX_ErrorInsufficientResources), line no %d", __LINE__);
			return OMX_ErrorInsufficientResources;
		}

		armProcessingComponent = NULL;
		error = OMX_ErrorNone;
	}
#endif

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_Proxy::MPEG4Enc_Processing()");

	return error;
}

// Mandatory to register the component
OMX_ERRORTYPE MPEG4EncFactoryMethodDefault(ENS_Component_p * ppENSComponent, OMX_U32 value)
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault()");

	if(value == ARMNMF_OBJECT) {
		OstTraceInt0(TRACE_FLOW, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault(): Factory Method for ARM encoder");
	}
	else if(value == MPCNMF_OBJECT) {
		OstTraceInt0(TRACE_FLOW, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault(): Factory Method for MPC encoder");
	}

	OMX_ERRORTYPE error = OMX_ErrorNone;

	OstTraceInt1(TRACE_FLOW, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault: Creating proxy component, allocating memory of size: %d",sizeof(MPEG4Enc_Proxy));
	MPEG4Enc_Proxy *comp = new MPEG4Enc_Proxy;
	if (comp == NULL)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault: Unable to create proxy component (OMX_ErrorInsufficientResources), line no. %d \n", __LINE__);
		return OMX_ErrorInsufficientResources;
	}

	error = comp->MPEG4Enc_Processing(value);
	if( error != OMX_ErrorNone)
	{
		OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault: OMX error: 0x%x at line no. %d \n",error,__LINE__);
		delete comp;
		return error;
	}

	error = comp->construct();
	if(error != OMX_ErrorNone)
	{
		OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault: OMX error: 0x%x at line no. %d \n",error,__LINE__);
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Enc_Proxy: MPEG4EncFactoryMethodDefault()");
	return OMX_ErrorNone;
}

// Mandatory to register the component - default for MPC
EXPORT_C OMX_ERRORTYPE MPEG4EncFactoryMethod(ENS_Component_p * ppENSComponent)
{
#ifdef HOST_MPC_COMPONENT_BOTH
	return MPEG4EncFactoryMethodDefault(ppENSComponent, MPCNMF_OBJECT);
#else
	return OMX_ErrorInvalidComponent;
#endif
}

// Mandatory to register the component - for MPC SVA based encoder
EXPORT_C OMX_ERRORTYPE MPEG4EncFactoryMethodMPC(ENS_Component_p * ppENSComponent)
{
#ifdef HOST_MPC_COMPONENT_BOTH
	return MPEG4EncFactoryMethodDefault(ppENSComponent, MPCNMF_OBJECT);
#else
	return OMX_ErrorInvalidComponent;
#endif
}

// Mandatory to register the component - for SW Encoder
EXPORT_C OMX_ERRORTYPE MPEG4EncFactoryMethodHOST(ENS_Component_p * ppENSComponent)
{
	return MPEG4EncFactoryMethodDefault(ppENSComponent, ARMNMF_OBJECT);
}

OMX_ERRORTYPE MPEG4Enc_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Proxy::setParameter() nParamIndex=0x%x", nParamIndex);
	
	OMX_BOOL has_changed;

	switch((int)nParamIndex)
	{
		case OMX_IndexParamVideoPortFormat:
			{
				OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
				if(pt_video->nPortIndex > getPortCount())
				{
					OstTraceInt3(TRACE_ERROR, "MPEG4Enc_Proxy::setParameter(): Error OMX_ErrorNoMore, PortIndex %d exceeds actual no of ports (%d), line no. %d", pt_video->nPortIndex, getPortCount(), __LINE__);
					return OMX_ErrorNoMore;
				}
				RETURN_OMX_ERROR_IF_ERROR(((MPEG4Enc_Port*)getPort(pt_video->nPortIndex))->setIndexParamVideoPortFormat(pt, &has_changed));
			}
			break;

		case OMX_IndexParamVideoQuantization:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoQuantization(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoFastUpdate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoFastUpdate(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoBitrate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoBitrate(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoMotionVector:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoMotionVector(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt), &has_changed));
			break;          
		case OMX_IndexParamVideoIntraRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoIntraRefresh(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamPixelAspectRatio:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamPixelAspectRatio(static_cast<OMX_PARAM_PIXELASPECTRATIO*>(pt), &has_changed));
			break;
		case OMX_IndexParamColorPrimary:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamColorPrimary(static_cast<OMX_PARAM_COLORPRIMARY*>(pt), &has_changed));
			break;

			//< FIXME currently data_partitioning is only supported for resolutions <= CIF
			// when support is extended upto 720p, this check isto be removed
			// currently 9_18, 9_19 testcases in sanity are failing because of this restriction
			// where dp feature is being tested for VGA resolution, which is wrong. But we were not
			// retruning any error previously, hence this error check now.
		case OMX_IndexParamVideoErrorCorrection:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoErrorCorrection(static_cast<OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoH263:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoH263(static_cast<OMX_VIDEO_PARAM_H263TYPE*>(pt), &has_changed));
			break;  
		case OMX_IndexParamVideoMpeg4:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoMpeg4(static_cast<OMX_VIDEO_PARAM_MPEG4TYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamCommonDeblocking:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamCommonDeblocking(static_cast<OMX_PARAM_DEBLOCKINGTYPE*>(pt), &has_changed));
			break;
		case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamResourceSvaMcps(pt));
			break;
		/* +Change start for CR338066 MPEG4Enc DCI */
		case VFM_IndexParamMpeg4ShortDCI:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamMpeg4ShortDCI(static_cast<VFM_PARAM_MPEG4SHORTDCITYPE*>(pt), &has_changed));
			break;
		/* -Change end for CR338066 MPEG4Enc DCI */
		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
			break;
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Proxy::setParameter()");
	
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Proxy::getParameter() nParamIndex=0x%x", nParamIndex);
	
	switch((int)nParamIndex)
	{
		case OMX_IndexParamVideoPortFormat:
			{
				OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
				if(pt_video->nPortIndex > getPortCount())
				{
					OstTraceInt3(TRACE_ERROR, "MPEG4Enc_Proxy::getParameter(): Error OMX_ErrorNoMore, nPortIndex=%d exceeds actual no of ports (%d) line no. %d", pt_video->nPortIndex, getPortCount(), __LINE__);
					return OMX_ErrorNoMore;
				}
				RETURN_OMX_ERROR_IF_ERROR(((MPEG4Enc_Port*)getPort(pt_video->nPortIndex))->getIndexParamVideoPortFormat(pt));
			}
			break;

		case OMX_IndexParamVideoQuantization:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoQuantization(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoFastUpdate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoFastUpdate(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt)));
			break;
		case OMX_IndexParamVideoBitrate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoBitrate(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt)));
			break;
		case OMX_IndexParamVideoMotionVector:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoMotionVector(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoIntraRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoIntraRefresh(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt)));
			break;
		case OMX_IndexParamPixelAspectRatio:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamPixelAspectRatio(static_cast<OMX_PARAM_PIXELASPECTRATIO*>(pt)));
			break;
		case OMX_IndexParamColorPrimary:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamColorPrimary(static_cast<OMX_PARAM_COLORPRIMARY*>(pt)));
			break;
		case OMX_IndexParamVideoErrorCorrection:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoErrorCorrection(static_cast<OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoH263:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoH263(static_cast<OMX_VIDEO_PARAM_H263TYPE*>(pt)));
			break;
		case OMX_IndexParamVideoMpeg4:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoMpeg4(static_cast<OMX_VIDEO_PARAM_MPEG4TYPE*>(pt)));
			break;
		case OMX_IndexParamCommonDeblocking:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamCommonDeblocking(static_cast<OMX_PARAM_DEBLOCKINGTYPE*>(pt)));
			break;
		case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamResourceSvaMcps(pt));
			break;
		/* +Change start for CR338066 MPEG4Enc DCI */
		case VFM_IndexParamMpeg4ShortDCI:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamMpeg4ShortDCI(static_cast<VFM_PARAM_MPEG4SHORTDCITYPE*>(pt)));
			break;
		/* -Change end for CR338066 MPEG4Enc DCI */
		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
			break;
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Proxy::getParameter()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Proxy::setConfig() nParamIndex=0x%x", nParamIndex);

	OMX_BOOL has_changed = OMX_FALSE;
	OMX_STATETYPE state = OMX_StateLoaded;
	MPEG4Enc_Proxy *proxy_component = (MPEG4Enc_Proxy *)this;

	proxy_component->GetState(this, &state);

	OstTraceFiltInst1(TRACE_API, "MPEG4Enc_Proxy::setConfig(): Component State = 0x%x", state);

	switch(nParamIndex)
	{
		case OMX_IndexParamVideoQuantization:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoQuantization(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoFastUpdate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoFastUpdate(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoBitrate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoBitrate(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoMotionVector:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoMotionVector(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt), &has_changed));
			break;          
		case OMX_IndexParamVideoIntraRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoIntraRefresh(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoErrorCorrection:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoErrorCorrection(static_cast<OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*>(pt), &has_changed));
			break;
		case OMX_IndexParamVideoH263:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoH263(static_cast<OMX_VIDEO_PARAM_H263TYPE*>(pt), &has_changed));
			break;  
		case OMX_IndexParamVideoMpeg4:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamVideoMpeg4(static_cast<OMX_VIDEO_PARAM_MPEG4TYPE*>(pt), &has_changed));
			break;  
		case OMX_IndexConfigVideoBitrate:
			{
				OMX_ERRORTYPE err = OMX_ErrorNone;
				OMX_VERSIONTYPE version;
				OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
				MPEG4Enc_Proxy *proxy_component = (MPEG4Enc_Proxy *)this;
				MPEG4Enc_Port *port = ( MPEG4Enc_Port *)(proxy_component->getPort(VPB+1));
				version.nVersion = 0;

				getOmxIlSpecVersion(&version);
				outPortDef.nVersion.nVersion = version.nVersion;
				outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

				RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigVideoBitrate(static_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(pt), &has_changed));
				if(has_changed)
				{
					OstTraceFiltInst1(TRACE_DEBUG, "MPEG4Enc_Proxy::setConfig(): Updating setting for bEnableDataPartitioning TRUE at line no. %d", __LINE__);
					err = port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from GetParameter for OMX_IndexParamPortDefinition at line no. %d", err, __LINE__);
						return err;
					}

					outPortDef.nPortIndex = 1;
					outPortDef.format.video.nBitrate = mParam.m_bitrate.nTargetBitrate;
#ifdef NOTIFY_USER	//< send portsettings changed event to user					
					//< update port 1 bitrate setting
					err = ((MPEG4Enc_Port*)getPort(VPB+1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from updateSettings() at line no. %d", err, __LINE__);
						return err;
					}
#endif
				}
			}
			break;  

		case OMX_IndexConfigVideoFramerate:
			{
				OMX_ERRORTYPE err = OMX_ErrorNone;
				OMX_VERSIONTYPE version;
				OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
				MPEG4Enc_Proxy *proxy_component = (MPEG4Enc_Proxy *)this;
				MPEG4Enc_Port *port = ( MPEG4Enc_Port *)(proxy_component->getPort(VPB+0));
				version.nVersion = 0;

				getOmxIlSpecVersion(&version);
				outPortDef.nVersion.nVersion = version.nVersion;
				outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

				RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigVideoFramerate(static_cast<OMX_CONFIG_FRAMERATETYPE*>(pt), &has_changed));

				if(has_changed)
				{
					err = port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from GetParameter for OMX_IndexParamPortDefinition at line no. %d", err, __LINE__);
						return err;
					}

					outPortDef.nPortIndex = 0;
					outPortDef.format.video.xFramerate = mParam.m_framerate.xEncodeFramerate;
#ifdef NOTIFY_USER	//< send portsettings changed event to user					
					//< update port 0 framerate setting
					err = ((MPEG4Enc_Port*)getPort(VPB+0))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from updateSettings() at line no. %d", err, __LINE__);
						return err;
					}
#endif
				}
			}
			break;  

		case OMX_IndexConfigVideoIntraVOPRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigVideoIntraVOPRefresh(static_cast<OMX_CONFIG_INTRAREFRESHVOPTYPE*>(pt), &has_changed));
			break;
		/* +Change start for CR343589 Rotation */
		case OMX_IndexConfigCommonRotate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigCommonRotate(static_cast<OMX_CONFIG_ROTATIONTYPE*>(pt), &has_changed));
			break;
		/* -Change end for CR343589 Rotation */
		case OMX_IndexConfigCommonOutputCrop:
			{
				//< OMX_U32 nLeft, nTop;
				OMX_ERRORTYPE err = OMX_ErrorNone;
				OMX_VERSIONTYPE version;
				OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
				MPEG4Enc_Proxy *proxy_component = (MPEG4Enc_Proxy *)this;
				MPEG4Enc_Port *port = ( MPEG4Enc_Port *)(proxy_component->getPort(VPB+1));
				version.nVersion = 0;

				getOmxIlSpecVersion(&version);
				outPortDef.nVersion.nVersion = version.nVersion;
				outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

				/*RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigCommonOutputCrop(pt, getFrameWidth(0), getFrameHeight(0), 
																				&mParam.m_crop_param.nWidth, &mParam.m_crop_param.nHeight, 
																				(OMX_U32*)&mParam.m_crop_param.nLeft, (OMX_U32*)&mParam.m_crop_param.nTop, &has_changed));

				*/

				OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;

				RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigCommonOutputCrop(pt));

				RETURN_XXX_IF_WRONG(0<=pt_crop->nLeft && (OMX_U32)pt_crop->nLeft<=getFrameWidth(0), OMX_ErrorBadParameter);
				RETURN_XXX_IF_WRONG(0<=pt_crop->nTop && (OMX_U32)pt_crop->nTop<=getFrameHeight(0), OMX_ErrorBadParameter);
				RETURN_XXX_IF_WRONG(0<=pt_crop->nLeft+(t_sint32)pt_crop->nWidth && (pt_crop->nLeft+pt_crop->nWidth<=getFrameWidth(0)), OMX_ErrorBadParameter);
				RETURN_XXX_IF_WRONG(0<=pt_crop->nTop+(t_sint32)pt_crop->nHeight && pt_crop->nTop+pt_crop->nHeight<=getFrameHeight(0), OMX_ErrorBadParameter);
				
				has_changed = (OMX_BOOL)((mParam.m_crop_param.nHeight != pt_crop->nHeight) || 
									(mParam.m_crop_param.nWidth != pt_crop->nWidth) || 
									(mParam.m_crop_param.nLeft != (OMX_U32)pt_crop->nLeft) || 
									(mParam.m_crop_param.nTop != (OMX_U32)pt_crop->nTop));


				mParam.m_crop_param.nWidth 	= pt_crop->nWidth;
				mParam.m_crop_param.nHeight = pt_crop->nHeight;
				mParam.m_crop_param.nLeft	= pt_crop->nLeft;
				mParam.m_crop_param.nTop 	= pt_crop->nTop;
				setFrameHeight(VPB+1, mParam.m_crop_param.nHeight);
				setFrameWidth(VPB+1, mParam.m_crop_param.nWidth);

				if(has_changed)
				{
					err = port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from GetParameter for OMX_IndexParamPortDefinition at line no. %d", err, __LINE__);
						return err;
					}

					outPortDef.nPortIndex = 1;
					outPortDef.format.video.nFrameWidth = mParam.m_crop_param.nWidth;
					outPortDef.format.video.nFrameHeight = mParam.m_crop_param.nHeight;
#ifdef NOTIFY_USER	//< send portsettings changed event to user
					//< update port 1 resolution setting
					err = ((MPEG4Enc_Port*)getPort(VPB+1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
					if(err != OMX_ErrorNone)
					{
						OstTraceInt2(TRACE_ERROR, "MPEG4Enc_Proxy::setConfig(): OMX Error err=0x%x from updateSettings() at line no. %d", err, __LINE__);
						return err;
					}
#endif
				}
			}
			break;                                                                              
		case OMX_IndexConfigVideoMacroBlockErrorMap:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigVideoMacroBlockErrorMap(static_cast<OMX_CONFIG_MACROBLOCKERRORMAPTYPE*>(pt), &has_changed));
			break;          

			//< FIXME below indices have to be supported through some entension api, especially to 
			//< to provide finer control to user in VBR case
			/*case VFM_IndexConfigMinimumFramerate:
			case VFM_IndexConfigSpatialQuality:
			case VFM_IndexConfigIntraInterval:
			case VFM_IndexConfigHeaderFrequency:
			case VFM_IndexConfigIntraRefresh:
			case VFM_IndexConfigPacketSizenInfo:
			*/

		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
	}

	if((state != OMX_StateLoaded) && (has_changed))
	{
		MPEG4Enc_Proxy *proxy_component = (MPEG4Enc_Proxy *)this;

		#ifdef HOST_MPC_COMPONENT_BOTH
			if(isMpcObject)
			{
				RETURN_OMX_ERROR_IF_ERROR(((MPEG4Enc_NmfMpc_ProcessingComponent*)&proxy_component->getProcessingComponent())->configureAlgo());
				mSendParamToMpc.reset();
			}
			else
		#endif
			{
				RETURN_OMX_ERROR_IF_ERROR(((MPEG4Enc_ArmNmf_ProcessingComponent*)&proxy_component->getProcessingComponent())->configureAlgo());
				mSendParamToMpc.reset();
			}
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Proxy::setConfig()");

	return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Enc_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Proxy::getConfig() nParamIndex=0x%x", nParamIndex);

	switch((int)nParamIndex)
	{
		case OMX_IndexParamVideoQuantization:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoQuantization(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoFastUpdate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoFastUpdate(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt)));
			break;
		case OMX_IndexParamVideoBitrate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoBitrate(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt)));
			break;
		case OMX_IndexParamVideoMotionVector:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoMotionVector(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt)));
			break;          
		case OMX_IndexParamVideoIntraRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoIntraRefresh(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoErrorCorrection:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoErrorCorrection(static_cast<OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*>(pt)));
			break;
		case OMX_IndexParamVideoH263:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoH263(static_cast<OMX_VIDEO_PARAM_H263TYPE*>(pt)));
			break;  
		case OMX_IndexParamVideoMpeg4:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamVideoMpeg4(static_cast<OMX_VIDEO_PARAM_MPEG4TYPE*>(pt)));
			break;  
		case OMX_IndexConfigVideoBitrate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigVideoBitrate(static_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(pt)));
			break;  
		case OMX_IndexConfigVideoFramerate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigVideoFramerate(static_cast<OMX_CONFIG_FRAMERATETYPE*>(pt)));
			break;  
		case OMX_IndexConfigVideoIntraVOPRefresh:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigVideoIntraVOPRefresh(static_cast<OMX_CONFIG_INTRAREFRESHVOPTYPE*>(pt)));
			break;
		/* +Change start for CR343589 Rotation */
		case OMX_IndexConfigCommonRotate:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonRotate(static_cast<OMX_CONFIG_ROTATIONTYPE*>(pt)));
			break;
		/* -Change end for CR343589 Rotation */
		case OMX_IndexConfigCommonOutputCrop:
		{
			/*RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt, mParam.m_crop_param.nWidth, mParam.m_crop_param.nHeight,
																			mParam.m_crop_param.nLeft, mParam.m_crop_param.nTop));
		    */

			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
				
			OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;

			pt_crop->nHeight 	= mParam.m_crop_param.nHeight;
			pt_crop->nWidth 	= mParam.m_crop_param.nWidth;
			pt_crop->nLeft 			= mParam.m_crop_param.nLeft;
			pt_crop->nTop 			= mParam.m_crop_param.nTop;

			break;
		}
		case OMX_IndexConfigVideoMacroBlockErrorMap:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigVideoMacroBlockErrorMap(static_cast<OMX_CONFIG_MACROBLOCKERRORMAPTYPE*>(pt)));
			break;
		case OMX_IndexConfigVideoSequenceHeader:
		{
			OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc_Proxy::getConfig(): getConfig called for OMX_IndexConfigVideoSequenceHeader");
			OMX_U32 filledLen;
			OMX_U32 filledLen_bits;
		
			OMX_VIDEO_CONFIG_SEQUENCEHEADERTYPE *p_sequence_header = (OMX_VIDEO_CONFIG_SEQUENCEHEADERTYPE *)pt;

			if(p_sequence_header->SequenceHeader[0] == 0 && p_sequence_header->nSequenceHeaderSize == 0)
			{
				OstTraceFiltInst2(TRACE_FLOW, "MPEG4Enc_Proxy::getConfig(): (SequenceHeader[0] == 0 and nSequenceHeaderSize == 0). Setting nSequenceHeaderSize = HEADER_BUFFER_SIZE (%d) at line no. %d", HEADER_BUFFER_SIZE, __LINE__);
				p_sequence_header->nSequenceHeaderSize = HEADER_BUFFER_SIZE;
			}
			else
			{
				if(p_sequence_header->nSequenceHeaderSize >= HEADER_BUFFER_SIZE)
				{
					OstTraceFiltInst3(TRACE_FLOW, "MPEG4Enc_Proxy::getConfig(): (nSequenceHeaderSize (%d) >= HEADER_BUFFER_SIZE (%d)) at line no. %d", p_sequence_header->nSequenceHeaderSize, HEADER_BUFFER_SIZE, __LINE__);
#ifdef HOST_MPC_COMPONENT_BOTH
					if(isMpcObject)
					{
						filledLen_bits = mpcProcessingComponent->processingComp.Write_VOS_VO_VOL((MPEG4Enc_Proxy *)this, p_sequence_header->SequenceHeader);
						filledLen = filledLen_bits/8;
						p_sequence_header->nSequenceHeaderSize = filledLen;
					OstTraceFiltInst1(TRACE_FLOW, "MPEG4Enc_Proxy::getConfig(): Filled SequenceHeader= with %d bytes", p_sequence_header->nSequenceHeaderSize);
					}
					else
#endif
					{
						//> FIXME!!
					}
				}
				else
				{
					OstTraceInt3(TRACE_ERROR, "MPEG4Enc_Proxy::getConfig(): OMX_ErrorUnsupportedSetting for p_sequence_header->nSequenceHeaderSize (%d) HEADER_BUFFER_SIZE(%d) p_sequence_header->SequenceHeader[0](%d) \n", p_sequence_header->nSequenceHeaderSize, HEADER_BUFFER_SIZE, p_sequence_header->SequenceHeader[0]);
					return OMX_ErrorUnsupportedSetting;
				}
			}
			break;
		}

			/*case VFM_IndexConfigMinimumFramerate:
			case VFM_IndexConfigSpatialQuality:
			case VFM_IndexConfigIntraInterval:
			case VFM_IndexConfigHeaderFrequency:
			case VFM_IndexConfigIntraRefresh:
			case VFM_IndexConfigPacketSizenInfo:
			*/
		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
			break;
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Proxy::getConfig()");
	return OMX_ErrorNone;
}

OMX_BOOL MPEG4Enc_Proxy::isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Proxy::isPortSpecificParameter() nParamIndex=0x%x", nParamIndex);
	
	OMX_BOOL isPortSpecific = OMX_FALSE;
    
    switch(nParamIndex) 
	{
        case OMX_IndexParamVideoQuantization:
		case OMX_IndexParamVideoPortFormat:
		case OMX_IndexParamVideoFastUpdate:
		case OMX_IndexParamVideoBitrate:
		case OMX_IndexParamVideoMotionVector:
		case OMX_IndexParamVideoIntraRefresh:
		case OMX_IndexParamVideoErrorCorrection:
		case OMX_IndexParamVideoMpeg4:
		case OMX_IndexParamVideoH263:
			isPortSpecific = OMX_TRUE;
			break;
		default:
        // SHAI extension
            if(nParamIndex == (OMX_INDEXTYPE)OMX_IndexParamPixelAspectRatio || nParamIndex == (OMX_INDEXTYPE)OMX_IndexParamColorPrimary) {
                isPortSpecific = OMX_TRUE;
            }
            else {
                isPortSpecific = VFM_Component::isPortSpecificParameter(nParamIndex);
            }
    }

	OstTraceFiltInst1(TRACE_API, "<= MPEG4Enc_Proxy::isPortSpecificParameter() returning %d", isPortSpecific);

    return isPortSpecific;
}

void MPEG4Enc_Proxy::setProfileLevelSupported()
{
	//The VFM API setOneProfileLevelSupported() is now called when the user sets eCompressionType,
	// because the Profile/Levels set by MPEG4Enc and therefore returned to the user would depend
	// on the eCompressionType set - i.e. either CodingMPEG4 or CodingH263
	//Here, we simply set one value as the default value:
	setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4LevelMax);
	//setOneProfileLevelSupported(OMX_VIDEO_H263ProfileBaseline, OMX_VIDEO_H263Level45);
}

RM_STATUS_E MPEG4Enc_Proxy::getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    pEstimationData->sMcpsSva.nMax = mParam.resourceSvaMcps();
    pEstimationData->sMcpsSva.nAvg = mParam.resourceSvaMcps();
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 500; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 0;   // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_EMPTY;    // RM_ESBUFID_BIG required for 1080p on symbian.  RM_ESBUFID_BIG for H264Encode. RM_ESBUFID_DEFAULT for others
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

	//>NMF_LOG("MPEG4Enc_Proxy::getResourcesEstimation : pEstimationData->sMcpsSva.nAvg = 0x%x, pEstimationData->sMcpsSva.nMax = 0x%x \n", pEstimationData->sMcpsSva.nAvg, pEstimationData->sMcpsSva.nMax);
	
    OstTraceFiltInst2(TRACE_FLOW, "sMcpsSva.nMax=%d, sMcpsSva.nAvg=%d ",pEstimationData->sMcpsSva.nMax,pEstimationData->sMcpsSva.nAvg);
    OstTraceFiltInst3(TRACE_FLOW, "nSvaHwPipe=%d, nSiaHwPipe=%d nSiaHwXP70=%d ",pEstimationData->sEsramBufs.nSvaHwPipe,pEstimationData->sEsramBufs.nSiaHwPipe,pEstimationData->sEsramBufs.nSiaHwXP70);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Proxy::getResourcesEstimation()");

    return RM_E_NONE;
}

void MPEG4Enc_Proxy::NmfPanicCallback(void *contextHandler, t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
	/* +Change end for ER 350388 SVA panic during switching camera */
    OstTraceFiltInst1(TRACE_API,"In MPEG4Enc_Proxy : In NmfPanicCallback line no : %d \n",__LINE__);
    OstTraceFiltInst1(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback serviceType : %d \n",serviceType);
	/* -Change end for ER 350388 SVA panic during switching camera */
	if(serviceType==1)
  {
	 // printf("Shutdown");
  }
  else
  {
	  /* +Change end for ER 350388 SVA panic during switching camera */
#ifdef HOST_MPC_COMPONENT_BOTH
	  if (serviceData->panic.panicSource == MPC_EE)
	  {
		  OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPC ");
		  if (serviceData->panic.info.mpc.coreid == SVA_CORE_ID)
		  {

			 if ((serviceData->panic.info.mpc.faultingComponent == mpcProcessingComponent->brcHandle)         ||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpcProcessingComponent->mNmfAlgoHandle)   		||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpcProcessingComponent->mNmfRmHandle)  		||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpcProcessingComponent->cOSTTrace) 	||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpcProcessingComponent->ddHandle))
			 {
		/* -Change end for ER 350388 SVA panic during switching camera */
		   	 	if (isMpcObject)
		   	 	{			  		
					mpcProcessingComponent->errorRecoveryDestroyAll();
					/* +Change end for ER 350388 SVA panic during switching camera */
					eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorHardware,0);
					OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for SVA code id MPC ");
					/* -Change end for ER 350388 SVA panic during switching camera */
	   			}
				/* +Change end for ER 350388 SVA panic during switching camera */
	   			else
	   			{
					OstTraceInt0(TRACE_ERROR,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPC but isMPCobject is OMX_FALSE (NOT POSSIBLE)");
				}
				/* -Change end for ER 350388 SVA panic during switching camera */

				/* +Change end for ER 350388 SVA panic during switching camera */
			 }
			 else
			 {
			 		OstTraceInt0(TRACE_ERROR,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPC but NOT for MPEG4Enc");
			 }
		  }
		  else
		  {
				OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPC but NOT in SVA");
		  }
	  }
	  else
#endif
	  {
		   OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for ARM");
		   if (serviceData->panic.info.host.faultingComponent == (void *)this)
		   {
			   OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPEG4Enc");
			   if (isMpcObject)
			   {
				   OstTraceInt0(TRACE_ERROR,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic for MPEG4Enc ARM but MPC MPEG4Enc is used");
			   }
			   else
			   {
				/* -Change end for ER 350388 SVA panic during switching camera */
			   		armProcessingComponent->errorRecoveryDestroyAll();
			/* +Change end for ER 350388 SVA panic during switching camera */
			   		eventHandler(OMX_EventError,(OMX_U32)OMX_EventError,0);
			   }
		   }
		   else
		   {
			   OstTraceInt0(TRACE_FLOW,"In MPEG4Enc_Proxy : NmfPanicCallback, Panic NOT for MPEG4Enc");
		   }
		   /* -Change end for ER 350388 SVA panic during switching camera */
	  }
  }
}
