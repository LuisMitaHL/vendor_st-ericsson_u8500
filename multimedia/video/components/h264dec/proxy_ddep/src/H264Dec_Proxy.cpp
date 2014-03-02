/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ H264Dec_Proxy

#include "t1xhv_types.h"
#include "H264Dec_Proxy.h"
#include <cm/inc/cm_macros.h>
#include "H264Dec_Port.h"
#include "H264Dec_Factory.h"

#include "VFM_Index.h"
#include "VFM_DDepUtility.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_proxy_ddep_src_H264Dec_ProxyTraces.h"
#endif

// Traces
//#ifdef PACKET_VIDEO_SUPPORT
//#undef LOG_TAG
//#define LOG_TAG "h264dec"
//#include<cutils/log.h>
//#define NMF_LOG LOGI
//#endif

// Constructor: Nothing to be done as error management cannot be performed.
// The real constructor is in construct()
H264Dec_Proxy::H264Dec_Proxy():
    VFM_Component(&mParam, OMX_TRUE),
    mParam(this),
    processingComponent(*this)
{
	OstTraceInt0(TRACE_API,"Enter H264Dec_Proxy::constructor");
    // +CR332521 CHANGE START FOR
#ifdef _CACHE_OPT_
    mParam.setiOMXCacheProperties(OMX_TRUE, OMX_FALSE);
#endif
    // -CR332521 CHANGE END OF
	OstTraceInt0(TRACE_API,"Exit H264Dec_Proxy::constructor");
}

inline void H264Dec_Proxy::h264dec_assert(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceFiltInst1(TRACE_ERROR, "H264DEC: proxy_ddep: H264Dec_Proxy: h264dec_assert: error line %d", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

inline void H264Dec_Proxy::h264dec_assert_static(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceInt1(TRACE_ERROR, "H264DEC: proxy_ddep: H264Dec_Proxy: h264dec_assert: error line %d", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

// The real constructor of the proxy, to be able to have error management
OMX_ERRORTYPE H264Dec_Proxy::construct()
{
    OstTraceInt0(TRACE_API,"Enter H264Dec_Proxy::construct");
    // Note ostTrcFiltInst0 can not be used at that time: ENS has not alloc memory yet

    RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
    RETURN_OMX_ERROR_IF_ERROR(processingComponent.setPortCount(2));

    // Set the processing component. To be done after VFM_Component::construct
    // as ENS construct() reset it
    setProcessingComponent(&processingComponent);
	OstTraceInt0(TRACE_FLOW,"H264Dec_Proxy::construct():Create Input port");
    // 1 buffer... but this number is recomputed in the port depending the image size
    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+0, OMX_DirInput, OMX_BufferSupplyInput, 1, 0, OMX_PortDomainVideo));
	OstTraceInt0(TRACE_FLOW,"H264Dec_Proxy::construct():Create Output port");
    // 1 buffer... but this number is recomputed in the port depending the image size
    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+1, OMX_DirOutput, OMX_BufferSupplyOutput, 1, 0, OMX_PortDomainVideo));

    // set the default value of the ports
    ((H264Dec_Port *)getPort(VPB+0))->setDefault();
    ((H264Dec_Port *)getPort(VPB+1))->setDefault();

    // set the role of the component: video_decoder.avc
    h264dec_assert((1 == getRoleNB()), __LINE__, OMX_TRUE);
    ENS_String<20> role = "video_decoder.avc";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);
    OstTraceInt0(TRACE_API,"Exit H264Dec_Proxy::construct");
    return OMX_ErrorNone;
}

// Implemention is required if MyProxy::construct() makes use of createSpecificPort()
VFM_Port *H264Dec_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
	OstTraceInt0(TRACE_API,"Enter H264Dec_Proxy::newPort");
	OstTraceInt0(TRACE_API,"Exit H264Dec_Proxy::newPort");
    return new H264Dec_Port(commonPortData, enscomp);
}

VFM_Component *H264Dec_Proxy::newProxy()
{
	OstTraceInt0(TRACE_API,"Enter H264Dec_Proxy::newProxy");
        OstTraceInt0(TRACE_API,"Exit H264Dec_Proxy::newProxy");
	return new H264Dec_Proxy;
}

H264Dec_Proxy::~H264Dec_Proxy()
{
	// very important as the delete of the ProcessingComponent is performed in the ENS automatically
    setProcessingComponent(0);
}


OMX_ERRORTYPE H264Dec_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst1(TRACE_API, "Enter H264Dec_Proxy::setParameter, nParamIndex=%d", nParamIndex);
    //Change for warning removal
    switch ((t_uint32)nParamIndex) {
    case OMX_IndexParamCommonDeblocking:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamCommonDeblocking(pt));
        break;

    case H264DEC_IndexParam_ParallelDeblocking:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamParallelDeblocking(pt));
        break;

    case OMX_IndexKhronosExtParamNALStreamFormat:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamNalStreamFormat(pt));
        break;

// +CR324558 CHANGE START FOR
    case VFM_IndexParamStaticDPBSize :
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamStaticDpbSize(pt));
        break;
// -CR324558 CHANGE END OF
// +CR349396 CHANGE START FOR
    case VFM_IndexParamRecyclingDelay:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setRecyclingDelay(pt));
        break;
// -CR349396 CHANGE START FOR
    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst0(TRACE_API, "Exit H264Dec_Proxy::setParameter");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst1(TRACE_API, "Enter H264Dec_Proxy::getParameter nParamIndex=%d", nParamIndex);
    //Change for warning removal
    switch ((t_uint32)nParamIndex) {
    case OMX_IndexParamCommonDeblocking:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamCommonDeblocking(pt));
        break;

    case H264DEC_IndexParam_ParallelDeblocking:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamParallelDeblocking(pt));
        break;

    case OMX_IndexKhronosExtParamNALStreamFormat:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamNalStreamFormat(pt));
        break;

    case OMX_IndexKhronosExtParamNALStreamFormatSupported:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamNalStreamFormatSupported(pt));
        break;

// +CR324558 CHANGE START FOR
    case VFM_IndexParamStaticDPBSize :
		RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamStaticDpbSize(pt));
        break;
// -CR324558 CHANGE END OF

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst0(TRACE_API, "Exit H264Dec_Proxy::getParameter.");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Dec_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst1(TRACE_API, "Enter H264Dec_Proxy::setConfig nParamIndex=%d", nParamIndex);
    switch (nParamIndex) {
    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst0(TRACE_API, "Exit H264Dec_Proxy::setConfig.");
	return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Dec_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst1(TRACE_API, "Enter H264Dec_Proxy::getConfig nParamIndex=%d", nParamIndex);
    //Change for warning removal
    switch ((t_uint32)nParamIndex) {
    case VFM_IndexConfigMemory:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigMemory(pt));
        break;

   case OMX_IndexConfigCommonOutputCrop:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
        break;

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst0(TRACE_API, "Exit H264Dec_Proxy::getConfig");
    return OMX_ErrorNone;
}


void H264Dec_Proxy::setProfileLevelSupported()
{
	// Used for OMX_IndexParamVideoProfileLevelQuerySupported
    // Requirements are (cf. .xls):
    // H.264 BP Level 1 and 1b decoding
    // H.264 BP Level 1.1 decoding
    // H.264 BP Level 1.2 decoding
    // H.264 BP Level 1.3 decoding
    // H.264 BP Level 2 decoding
    // H.264 BP Level 2.1 decoding
    // H.264 BP Level 2.2 decoding
    // H.264 BP Level 3 decoding
    // H.264 BP Level 4 decoding
    // H.264 BP Level 3.1 decoding
    // H.264 BP Level 4 decoding
    // H.264 MP Decoder standard tools support
    // H.264 MP Level 1 and 1b decoding
    // H.264 MP Level 1.1 decoding
    // H.264 MP Level 1.2 decoding
    // H.264 MP Level 1.3 decoding
    // H.264 MP Level 2 decoding
    // H.264 MP Level 2.1 decoding
    // H.264 MP Level 2.2 decoding
    // H.264 MP Level 3 decoding
    // H.264 MP Level 3.1 decoding
    // H.264 MP Level 4 decoding
    // H.264 HP Decoder standard tools support
    // H.264 HP Decoder standard tools support
    // H.264 HP Level 1 and 1b decoding
    // H.264 HP Level 1.1 decoding
    // H.264 HP Level 1.2 decoding
    // H.264 HP Level 1.3 decoding
    // H.264 HP Level 2 decoding
    // H.264 HP Level 2.1 decoding
    // H.264 HP Level 2.2 decoding
    // H.264 HP Level 3 decoding
    // H.264 HP Level 3.1 decoding
    // H.264 HP Level 4 decoding

    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel4);
    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileMain,     OMX_VIDEO_AVCLevel4);
    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileHigh,     OMX_VIDEO_AVCLevel4);
}

void H264Dec_Proxy::setVideoSettingsSupported()
{
	setOneVideoSettingsSupported(0, OMX_VIDEO_CodingAVC, OMX_COLOR_FormatUnused, 0);

	/* Change required for ICS migration, hack put to give this change under ste.video.dec.mpeg4.in.size property
	   which is used for ICS only */
	if (VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize())
	{
    	setOneVideoSettingsSupported(1, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, 0);
	}
	else
	{
    	setOneVideoSettingsSupported(1, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)0x7FFFFFFE , 0); // to be removed !
	}
}


OMX_ERRORTYPE H264Dec_Proxy::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
	OstTraceFiltInst0(TRACE_FLOW,"H264Dec_Proxy::getExtensionIndex");
	if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.memory_status") {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexConfigMemory;
        return OMX_ErrorNone;
    }

    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.H264DEC.ParallelDeblocking") {
        *pIndexType = (OMX_INDEXTYPE)H264DEC_IndexParam_ParallelDeblocking;
        return OMX_ErrorNone;
    }
	return VFM_Component::getExtensionIndex(cParameterName, pIndexType);
}

int H264Dec_Proxy::getDpbSizeInBytes(OMX_VIDEO_AVCLEVELTYPE level)
{
	//  These numbers have been taken from file arm_nmf/decoder/src/host_alloc.c
    switch (level) {
    case OMX_VIDEO_AVCLevel1:   return 152064;
    case OMX_VIDEO_AVCLevel1b:  return 152064;
    case OMX_VIDEO_AVCLevel11:  return 345600;
    case OMX_VIDEO_AVCLevel12:  return 912384;
    case OMX_VIDEO_AVCLevel13:  return 912384;
    case OMX_VIDEO_AVCLevel2:   return 912384;
    case OMX_VIDEO_AVCLevel21:  return 1824768;
    case OMX_VIDEO_AVCLevel22:  return 3110400;
    case OMX_VIDEO_AVCLevel3:   return 3110400;
    case OMX_VIDEO_AVCLevel31:  return 6912000;
    case OMX_VIDEO_AVCLevel32:  return 7864320;
    case OMX_VIDEO_AVCLevel4:   return 12582912;
    case OMX_VIDEO_AVCLevel41:  return 12582912;
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
    case OMX_VIDEO_AVCLevel42:  return 13369344;	//Table A7 Annex A ITU-T Rec H.264(03/2010)
    case OMX_VIDEO_AVCLevel5:  return 42393600;
    case OMX_VIDEO_AVCLevel51:  return 70778880;
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
    default: h264dec_assert_static((0==1), __LINE__, OMX_TRUE); break;
    }
    return -1;
}

EXPORT_C RM_STATUS_E H264Dec_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    h264dec_assert((getScratchEsram()==OMX_TRUE), __LINE__, OMX_TRUE);
	OstTraceFiltInst0(TRACE_FLOW,"Enter H264Dec_Proxy::getResourcesEstimation");
	// These default values must be redefined in each codec
    pEstimationData->sMcpsSva.nMax = 10; // MIPS = MMDSP_SVA_MCPS.
    pEstimationData->sMcpsSva.nAvg = 10;
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 500; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 500; // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_BIG;    // RM_ESBUFID_BIG required for 1080p on symbian.  RM_ESBUFID_BIG for H264Encode. RM_ESBUFID_DEFAULT for others
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    return RM_E_NONE;
}

OMX_S32 H264Dec_Proxy::getMaxBuffers(OMX_U32 portIndex) const
{
    return 128;
}


void H264Dec_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
  OstTraceInt1(TRACE_WARNING, "Enter H264Dec_Proxy::NmfPanicCallback, user_data=%d",serviceData->panic.info.mpc.panicInfo1);

  switch (serviceType) {
  case NMF_SERVICE_PANIC:
    if (serviceData->panic.panicSource==HOST_EE || serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SVA_CORE_ID)) {
        return;
    }
    OstTraceInt0(TRACE_WARNING, "H264Dec_Proxy::NmfPanicCallback: calls errorRecoveryKillMpc");
    processingComponent.errorRecoveryKillMpc(mpc_domain_id);
    break;
  case NMF_SERVICE_SHUTDOWN:
    if (serviceData->shutdown.coreid != ((t_nmf_core_id)SVA_CORE_ID)) {
        return;
    }
    if ((getCompFsm()->getOMXState() != OMX_StateLoaded) && !processingComponent.isGoToLoaded()) {

        OstTraceInt0(TRACE_WARNING, "H264Dec_Proxy::NmfPanicCallback: calls errorRecoveryRestartMpc");
        processingComponent.errorRecoveryRestartMpc(mpc_domain_id);
    } else {
        OstTraceInt0(TRACE_WARNING, "H264Dec_Proxy::NmfPanicCallback: NMF_SERVICE_SHUTDOWN, but we are in loaded mode");
    }
    break;
  default:
    OstTraceInt1(TRACE_ERROR, "H264Dec_Proxy::NmfPanicCallback: unknown service %d", serviceType);
  }
}

// +CR324558 CHANGE START FOR
OMX_U32 H264Dec_Proxy::addExtraPipeBuffers(OMX_U32 bufferCount){
    // Number is DPB + (HAMAC_PIPE_SIZE+1) (number of parallel processing) + 1 (for ddep) + 1 (for display)
    bufferCount += (HAMAC_PIPE_SIZE+1);      // number of parallel processing
    bufferCount += NB_BUFFERS_OUTPUT_DDEP;   // received in the ddep
// +CR349396 CHANGE START FOR
#ifdef  PACKET_VIDEO_SUPPORT
    bufferCount += NB_BUFFERS_IN_DISPLAY;
    bufferCount += getParamAndConfig()->getRecyclingDelay()->get();
#endif
// -CR349396 CHANGE START FOR
    return bufferCount;
}

OMX_U32 H264Dec_Proxy::subtractExtraPipeBuffers(OMX_U32 bufferCount){
    // Number is DPB + (HAMAC_PIPE_SIZE+1) (number of parallel processing) + 1 (for ddep) + 1 (for display)
    bufferCount -= (HAMAC_PIPE_SIZE+1);      // number of parallel processing
    bufferCount -= NB_BUFFERS_OUTPUT_DDEP;   // received in the ddep
// +CR349396 CHANGE START FOR
#ifdef  PACKET_VIDEO_SUPPORT
    bufferCount -= NB_BUFFERS_IN_DISPLAY;
    bufferCount -= getParamAndConfig()->getRecyclingDelay()->get();
#endif
// -CR349396 CHANGE START FOR
    return bufferCount;
}
// -CR324558 CHANGE END OF


#undef ENS_Component_p
#undef ENS_Component
class ENS_Component;
EXPORT_C
OMX_ERRORTYPE H264DecFactoryMethod(ENS_Component ** ppENSComponent)
{
    OstTraceInt0(TRACE_API,"Enter H264DecFactoryMethod");
    OstTraceInt0(TRACE_API,"Exit H264DecFactoryMethod");
#ifdef ENS_VERSION
#if ENS_VERSION == ENS_VERSION_VIDEO
    return VFM_Component::VFMFactoryMethod((ENS_Component_Video **)ppENSComponent, H264Dec_Proxy::newProxy);
#endif
#endif

    return VFM_Component::VFMFactoryMethod((ENS_Component **)ppENSComponent, H264Dec_Proxy::newProxy);
}
