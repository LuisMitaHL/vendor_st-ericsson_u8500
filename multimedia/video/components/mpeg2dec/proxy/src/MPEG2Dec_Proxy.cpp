/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_mpeg2dec
#include "osi_trace.h"
#include "MPEG2Dec_Port.h"
#include "MPEG2Dec_Proxy.h"
#include "MPEG2Dec_Factory.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_proxy_src_MPEG2Dec_ProxyTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "mpeg2dec"
#include<cutils/log.h>
#endif

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { mpeg2dec_proxy_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { mpeg2dec_proxy_assert(_error, __LINE__, OMX_FALSE); return (_error); } }

void MPEG2Dec_Proxy::mpeg2dec_proxy_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "MPEG2Dec_Proxy : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}

MPEG2Dec_Proxy::MPEG2Dec_Proxy():VFM_Component(&mParam),processingComponent(*this),mParam(this){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy_Constructor");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy_Constructor");
}

MPEG2Dec_Proxy::~MPEG2Dec_Proxy(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy_Destructor");
	setProcessingComponent(0);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy_Destructor");
}

VFM_Port *MPEG2Dec_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::newPort");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::newPort");
	return new MPEG2Dec_Port(commonPortData, enscomp);
}

OMX_ERRORTYPE MPEG2Dec_Proxy::construct()
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::construct");
    RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
    setProcessingComponent(&processingComponent);
	RETURN_OMX_ERROR_IF_ERROR(processingComponent.setPortCount(2));

	ENS_Port * portInput = newPort(
		EnsCommonPortData(
			VPB+0,
			OMX_DirInput,
			2/*nBufferCountMin*/,
			0/*nBufferSizeMin*/,
			OMX_PortDomainVideo,
            OMX_BufferSupplyInput
		),
		*this);

	if (portInput == 0)
		return OMX_ErrorInsufficientResources;


	/*FIXME change minimum buffer req based on lowdelay etc params*/
	ENS_Port * portOutput = newPort(
		EnsCommonPortData(
			VPB+1,
			OMX_DirOutput,
			4/*nBufferCountMin*/,
			0/*nBufferSizeMin*/,
			OMX_PortDomainVideo,
            OMX_BufferSupplyOutput
		),
		*this);

	if (portOutput == 0)
	{
		delete portInput;
		return OMX_ErrorInsufficientResources;
	}

	//Add only when both ports are successfully allocated
	addPort(portInput);
	addPort(portOutput);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::construct");
	return OMX_ErrorNone;
}



// Mandatory to register the component
EXPORT_C  OMX_ERRORTYPE MPEG2DecFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2DecFactoryMethod");
    OMX_ERRORTYPE error;
    MPEG2Dec_Proxy *comp = new MPEG2Dec_Proxy;
	RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2DecFactoryMethod");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::setParameter");
    switch ((int)nParamIndex) {
		case OMX_IndexParamVideoPortFormat:
			{
			OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
			if(pt_video->nPortIndex > getPortCount())
				return OMX_ErrorNoMore;
			RETURN_OMX_ERROR_IF_ERROR(((MPEG2Dec_Port*)getPort(pt_video->nPortIndex))->setIndexParamVideoPortFormat((OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt));
			}
			break;

		case OMX_IndexParamVideoErrorCorrection:
			break;

		case OMX_IndexParamVideoMpeg2:
			break;
		case OMX_IndexParamCommonDeblocking:
			break;
		case OMX_IndexParamPriorityMgmt:
			break;

		case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamResourceSvaMcps(pt));
			break;

		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
			break;
    }
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::setParameter");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG2Dec_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::getParameter");
    switch ((int)nParamIndex) {

		case OMX_IndexParamVideoPortFormat:
			{
			OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
			if(pt_video->nPortIndex > getPortCount())
				return OMX_ErrorNoMore;
			RETURN_OMX_ERROR_IF_ERROR(((MPEG2Dec_Port*)getPort(pt_video->nPortIndex))->getIndexParamVideoPortFormat((OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt));
			}
			break;
		case OMX_IndexParamVideoErrorCorrection:
			break;

		case OMX_IndexParamVideoMpeg2:
			break;
			//only getparam
		case OMX_IndexParamVideoProfileLevelQuerySupported:
			break;
			//only getparam
		case OMX_IndexParamVideoProfileLevelCurrent:
			break;
		case OMX_IndexParamCommonDeblocking:
			break;
		//FIXME only for conformance tests, should be in ENS
		case OMX_IndexParamPriorityMgmt:
			break;
#ifdef PACKET_VIDEO_SUPPORT
        case PV_OMX_COMPONEN	T_CAPABILITY_TYPE_INDEX:   //what is this
            {
                if (pt == NULL)
                {
                    return OMX_ErrorBadParameter;
                }
                PV_OMXComponentCapabilityFlagsType *pCaps
                    = (PV_OMXComponentCapabilityFlagsType *)pt;
                // allow the output external buffers, keep frame by frame mode
                pCaps->iIsOMXComponentMultiThreaded = OMX_TRUE;
                pCaps->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_TRUE;
                //pCaps->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_FALSE;
                pCaps->iOMXComponentSupportsExternalInputBufferAlloc = OMX_FALSE;
                pCaps->iOMXComponentSupportsMovableInputBuffers = OMX_FALSE;
                pCaps->iOMXComponentSupportsPartialFrames = OMX_FALSE;
                pCaps->iOMXComponentUsesNALStartCode = OMX_FALSE;
                pCaps->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;
                pCaps->iOMXComponentUsesFullAVCFrames = OMX_TRUE;
                break;
            }
#endif

		case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamResourceSvaMcps(pt));
			break;

		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
			break;
    }
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::getParameter");
    return OMX_ErrorNone;
}

void MPEG2Dec_Proxy::setProfileLevelSupported(){
    // Used for OMX_IndexParamVideoProfileLevelQuerySupported
    setOneProfileLevelSupported(OMX_VIDEO_MPEG2ProfileSimple, OMX_VIDEO_MPEG2LevelLL);
}

OMX_ERRORTYPE MPEG2Dec_Proxy::updateOutputPortSettings(t_uint32 width,t_uint32 height)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::updateOutputPortSettings");
    OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE outputPortDef;
	OMX_PARAM_PORTDEFINITIONTYPE inputPortDef;
	inputPortDef.nPortIndex = 0;
	outputPortDef.nPortIndex = 1;

	//Fire portSettingsChangeEvent on output port
    outputPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	getOmxIlSpecVersion(&outputPortDef.nVersion);
	err = ((MPEG2Dec_Port*)getPort(1))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outputPortDef);
	if(err != OMX_ErrorNone) return err;

	if(getFrameWidth(1)!=width || getFrameHeight(1)!=height) {
		outputPortDef.format.video.nFrameHeight = height;
		outputPortDef.format.video.nFrameWidth =width;

		err = ((MPEG2Dec_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outputPortDef);
		if(err != OMX_ErrorNone) return err;
	}

	//Fire portSettingsChangeEvent on input port
	inputPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	getOmxIlSpecVersion(&inputPortDef.nVersion);
	err = ((MPEG2Dec_Port*)getPort(0))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&inputPortDef);
	if(err != OMX_ErrorNone) return err;

	if(getFrameWidth(0)!=width || getFrameHeight(0)!=height) {
		inputPortDef.format.video.nFrameHeight = height;
		inputPortDef.format.video.nFrameWidth =width;

		err = ((MPEG2Dec_Port*)getPort(0))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&inputPortDef);
		if(err != OMX_ErrorNone) return err;
	}
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::updateOutputPortSettings");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::setConfig");
    switch (nParamIndex){
		case OMX_IndexConfigVideoMBErrorReporting:
			{
				OMX_CONFIG_MBERRORREPORTINGTYPE *pt_video = (OMX_CONFIG_MBERRORREPORTINGTYPE*)pt;
				processingComponent.SetIndex_macroblock_error_reporting(pt_video->bEnabled);
			}
			break;

        default:
            RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
	}
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::setConfig");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_Proxy::getConfig");
	switch(nParamIndex)
	{
		case OMX_IndexConfigVideoMBErrorReporting:
			{
				OMX_CONFIG_MBERRORREPORTINGTYPE *pt_video = (OMX_CONFIG_MBERRORREPORTINGTYPE*)pt;
				pt_video->bEnabled =processingComponent.GetIndex_macroblock_error_reporting();
			}
			break;
		case OMX_IndexConfigVideoMacroBlockErrorMap:
			{
			    processingComponent.GetIndex_macroblock_error_map(pt);
            }
			break;

   		case OMX_IndexConfigCommonOutputCrop:
        	RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
        	break;

		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
			break;
	}

	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_Proxy::getConfig");
    return OMX_ErrorNone;
}


RM_STATUS_E MPEG2Dec_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG2Dec_Proxy::getResourcesEstimation()");

	OMX_U32 cyclesperMB,frameRate;
	OMX_U32 sva_mcps;
	double margin=1.1;
	OMX_U32 height_MB = ((getFrameHeight(0) + 0xF)&(~0xF)) / 16;
	OMX_U32 width_MB  = ((getFrameWidth(0)  + 0xF)&(~0xF)) / 16;

	cyclesperMB=1650;
	frameRate=30;

	/* +change for 346056 */
	OMX_U32 thresholdValue = (1280/16)*(720/16);
	OMX_U32 actualMBs = (height_MB*width_MB);
	if (actualMBs > thresholdValue)
	/* -change for 346056 */
	{
		cyclesperMB=775;
		margin=1.05;
	}



    sva_mcps = ((OMX_U32)(((height_MB * width_MB) * cyclesperMB * frameRate * margin)) / 1000000) + 1;

	OstTraceFiltInst1(TRACE_FLOW, "getResourcesEstimation() sva_mcps = %d", sva_mcps);


     //clipping the max value of sva_mcps
	 if (sva_mcps>200) {
		 sva_mcps = 200;
	}

    pEstimationData->sMcpsSva.nMax = 50;//sva_mcps;//processingComponent.resourceSvaMcps();
    pEstimationData->sMcpsSva.nAvg = 50;//sva_mcps;//processingComponent.resourceSvaMcps();
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

	OstTraceFiltInst0(TRACE_API, "<= MPEG2Dec_Proxy::getResourcesEstimation()");
    return RM_E_NONE;
}


void MPEG2Dec_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType,
									   t_nmf_service_data *serviceData)
{
    OstTraceInt1(TRACE_API, "=> MPEG4Dec_Proxy::NmfPanicCallback() serviceType = %d", serviceType);

	switch (serviceType) {
		case NMF_SERVICE_PANIC:
		  if(serviceData->panic.panicSource==MPC_EE)
  {
              OstTraceInt0(TRACE_FLOW, "panicSource = MPC");
			  if (serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SVA_CORE_ID)) {
                  OstTraceInt0(TRACE_FLOW, "panicSource core is not SVA");
				  return;
  }
			  if (processingComponent.isNMFPanicSource(MPC_EE, serviceData->panic.info.mpc.faultingComponent)){
                  OstTraceInt0(TRACE_FLOW, "MPEG2Dec_Proxy MPC generated Panic");
  processingComponent.errorRecoveryDestroyAll();
				  eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
				  OstTraceInt1(TRACE_FLOW, "MPEG2Dec_Proxy::NmfPanicCallback serviceType =%d \n",serviceType);
			  }
			  else{
                  OstTraceInt0(TRACE_FLOW, "isMPCobject is NULL not possible");
			  }
		  }
		  break;
        case NMF_SERVICE_SHUTDOWN:
          OstTraceInt0(TRACE_FLOW, "MPC Shutdown");
		  if (serviceData->shutdown.coreid != ((t_nmf_core_id)SVA_CORE_ID)) {
              OstTraceInt0(TRACE_FLOW, "MPC shut down is not SVA");
			  return;
    }
          OstTraceInt0(TRACE_FLOW, "MPC shut down is SVA");
		  break;
		default:
		  OstTraceInt1(TRACE_ERROR, "MPEG4Dec_Proxy::NmfPanicCallback: unknown service %d", serviceType);
  }

    OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::NmfPanicCallback()");
}

