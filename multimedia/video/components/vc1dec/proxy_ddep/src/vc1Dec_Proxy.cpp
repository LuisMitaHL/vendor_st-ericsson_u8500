/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Contains the main class of the OMX video component vc1dec.
 * \author  ST-Ericsson
 */


#define _CNAME_ vc1Dec_Proxy
#include "osi_trace.h"

#include "t1xhv_types.h"
#include "vc1Dec_Proxy.h"
#include <cm/inc/cm_macros.h>
#include "vc1Dec_Port.h"
#include "vc1Dec_Factory.h"
#include "VFM_Index.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_proxy_ddep_src_vc1Dec_ProxyTraces.h"
#endif
// Constructor: Nothing to be done as error management cannot be performed.
// The real constructor is in construct()
vc1Dec_Proxy::vc1Dec_Proxy():
    VFM_Component(&mParam,OMX_TRUE),
    mParam(this),
    processingComponent(*this)
{
    IN0("");
    // +CR332521 CHANGE START FOR
#ifdef _CACHE_OPT_
    mParam.setiOMXCacheProperties(OMX_TRUE, OMX_FALSE);
#endif
    // -CR 332521 CHANGE END OF
    OUT0("");
}

// The real constructor of the proxy, to be able to have error management
OMX_ERRORTYPE vc1Dec_Proxy::construct()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: construct()\n");
    RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
    // setTraceInfo must be called after VFM_Component::construct (only for ParamAndConfig)
    unsigned int  id = getId1();
    TraceInfo_t * traceInfoPtr = getTraceInfoPtr();
    if (traceInfoPtr)
    {
        mParam.setTraceInfo(traceInfoPtr, id);
    }
    else
    {
        DBC_ASSERT(0);
    }
    RETURN_OMX_ERROR_IF_ERROR(processingComponent.setPortCount(2));

    // Set the processing component. To be done after VFM_Component::construct
    // as ENS construct() reset it
    setProcessingComponent(&processingComponent);

    // min buffer count for input is 2 (we need two input buffer to output first frame), +3 for pipe size
    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+0, OMX_DirInput, OMX_BufferSupplyInput, 2+3, 0, OMX_PortDomainVideo));

    // Here is defined the min Buffer Count, we set it to 3 (needed intrinsicly by decoder), +4 for display queue, +3 for pipe size
    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+1, OMX_DirOutput, OMX_BufferSupplyOutput, 3+4+3, 0, OMX_PortDomainVideo));

    // set the default value of the ports
    ((vc1Dec_Port *)getPort(VPB+0))->setDefault();
    ((vc1Dec_Port *)getPort(VPB+1))->setDefault();

    // set the role of the component
    DBC_ASSERT(1 == getRoleNB());
    ENS_String<20> role = "video_decoder.vc1";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    return OMX_ErrorNone;
}

// Implemention is required if MyProxy::construct() makes use of createSpecificPort()
VFM_Port *vc1Dec_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
    return new vc1Dec_Port(commonPortData, enscomp);
}

VFM_Component *vc1Dec_Proxy::newProxy()
{
    return new vc1Dec_Proxy;
}

vc1Dec_Proxy::~vc1Dec_Proxy()
{
    IN0("");
    // very important as the delete of the ProcessingComponent is performed in the ENS automatically
    setProcessingComponent(0);
    OUT0("");
}


EXPORT_C
OMX_ERRORTYPE VC1DecFactoryMethod(ENS_Component_p * ppENSComponent)
{
    return VFM_Component::VFMFactoryMethod(ppENSComponent, vc1Dec_Proxy::newProxy);
}



OMX_ERRORTYPE vc1Dec_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    IN0("");

    OstTraceFiltInst1(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: setParameter(), nParamIndex=%d\n", nParamIndex);
    switch ((int)nParamIndex) {
  // +CR349396 CHANGE START FOR
    case VFM_IndexParamRecyclingDelay:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setRecyclingDelay(pt));
        break;
// -CR349396 CHANGE START FOR
    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE vc1Dec_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    IN0("");

    OstTraceFiltInst1(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: getParameter(), nParamIndex=%d\n", nParamIndex);
    switch (nParamIndex) {
    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE vc1Dec_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    IN0("");
    OstTraceFiltInst1(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: setConfig(), nParamIndex=%d\n", nParamIndex);
    switch (nParamIndex) {
    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
        break;
    }
    OUT0("");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE vc1Dec_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    IN0("");
    OstTraceFiltInst1(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: getConfig(), nParamIndex=%d\n", nParamIndex);
	switch (nParamIndex) {
		case OMX_IndexConfigCommonOutputCrop:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
			break;
		default:
    		RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
			break;
}
    OUT0("");
    return OMX_ErrorNone;
}


void vc1Dec_Proxy::setProfileLevelSupported()
{
    // Used for OMX_IndexParamVideoProfileLevelQuerySupported

    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: setProfileLevelSupported()\n");
    setOneProfileLevelSupported(OMX_VIDEO_VC1ProfileSimple,OMX_VIDEO_VC1LevelMedium);
    setOneProfileLevelSupported(OMX_VIDEO_VC1ProfileMain,  OMX_VIDEO_VC1LevelHigh);
}

void vc1Dec_Proxy::setVideoSettingsSupported()
{
    OstTraceInt0(TRACE_API,  "VC1Dec: proxy_ddep : vc1Dec_Proxy: setVideoSettingsSupported()\n");
    setOneVideoSettingsSupported(0, static_cast<OMX_VIDEO_CODINGTYPE>(OMX_SYMBIAN_VIDEO_CodingVC1), OMX_COLOR_FormatUnused, 15);
    setOneVideoSettingsSupported(0, static_cast<OMX_VIDEO_CODINGTYPE>(9), OMX_COLOR_FormatUnused, 15); // VC1 previous value

    // +CR334366 CHANGE START FOR
    // +VC1 codec validation
    setOneVideoSettingsSupported(0, static_cast<OMX_VIDEO_CODINGTYPE>(OMX_VIDEO_CodingWMV), OMX_COLOR_FormatUnused, 15); // VC1 previous value
#ifndef PACKET_VIDEO_SUPPORT
    setOneVideoSettingsSupported(1, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, 0);
#endif
     // -VC1 codec validation
     // -CR334366 CHANGE END OF

    setOneVideoSettingsSupported(1, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, 0); // YUV420MB previous value
}

EXPORT_C RM_STATUS_E vc1Dec_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    DBC_ASSERT(getScratchEsram()==OMX_TRUE);

    // These default values must be redefined in each codec
    pEstimationData->sMcpsSva.nMax = 10; // MIPS = MMDSP_SVA_MCPS.
    pEstimationData->sMcpsSva.nAvg = 10;
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 500; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 500; // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_DEFAULT; // RM_ESBUFID_BIG for H264Encode. RM_ESBUFID_DEFAULT for others
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    return RM_E_NONE;
}
 /* +ER 341788 CHANGE START FOR */
void vc1Dec_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
  OstTraceInt1(TRACE_WARNING, "VC1Dec: proxy_ddep : vc1Dec_Proxy: NmfPanicCallback(), user_data=%d\n",serviceData->panic.info.mpc.panicInfo1);

  switch (serviceType)
  {
  case NMF_SERVICE_PANIC:
    //NMF_LOG("In error case and abt to give msg \n");
    if (serviceData->panic.panicSource==HOST_EE || serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SVA_CORE_ID))
    {
		NMF_LOG("Returning from here %d and core %d \n",serviceData->panic.panicSource,serviceData->panic.info.mpc.coreid);
        return;
    }
    OstTraceInt0(TRACE_WARNING, "VC1Dec: proxy_ddep : vc1Dec_Proxy: NmfPanicCallback(): calls errorRecoveryDestroyAll\n");


     processingComponent.errorRecoveryDestroyAll();
     eventHandler(OMX_EventError, OMX_ErrorHardware, 0);
 OstTraceInt0(TRACE_ERROR, "VC1Dec: proxy_ddep : vc1Dec_Proxy: NmfPanicCallback(): error event dispatched OMX_ErrorHardware");
    NMF_LOG("error event dispatched \n");
    break;
  default: break;
    OstTraceInt1(TRACE_ERROR, "VC1Dec: proxy_ddep : vc1Dec_Proxy: NmfPanicCallback(): unknown service %d\n", serviceType);
  }
}
 /* +ER 341788 CHANGE START FOR */

