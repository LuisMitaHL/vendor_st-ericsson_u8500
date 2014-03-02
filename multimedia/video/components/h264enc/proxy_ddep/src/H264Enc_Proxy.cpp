/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ H264Enc_Proxy
#include "osi_trace.h"

//#include "t1xhv_types.h"
#include "H264Enc_Proxy.h"
#include <cm/inc/cm_macros.h>
#include "H264Enc_Port.h"
#include "H264Enc_Factory.h"
#include "video_chipset_api_index.h"
#include "video_generic_chipset_api.h"
#include "VFM_Index.h"

#include "VFM_DDepUtility.h"

#ifdef OMX_COLOR_FormatYUV420MBPackedSemiPlanar
#undef OMX_COLOR_FormatYUV420MBPackedSemiPlanar
#endif

#define OMX_COLOR_FormatYUV420MBPackedSemiPlanar 0x7FFFFFFE

/* +Change for 372717 */
#define make16multiple(x) (((x + 15)/16)*16)
/* -Change for 372717 */

#ifndef H264ENC_TRACE_GROUP_PROXY
    #include "OstTraceDefinitions_ste.h"
    #include "OpenSystemTrace_ste.h"
    #ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_proxy_ddep_src_H264Enc_ProxyTraces.h"
    #endif
#endif
typedef OMX_CONFIG_COLORPRIMARY OMX_PARAM_COLORPRIMARY;

// Constructor: Nothing to be done as error management cannot be performed.
// The real constructor is in construct()
H264Enc_Proxy::H264Enc_Proxy():
    VFM_Component(&mParam, OMX_TRUE),
    processingComponent(*this),
    mParam(this)
{
OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::H264Enc_Proxy");
    //+ code for CR 332873
	isSpecialCTCase = OMX_FALSE;
	//- code for CR 332873
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::H264Enc_Proxy");
}

// The real constructor of the proxy, to be able to have error management
OMX_ERRORTYPE H264Enc_Proxy::construct()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::construct");
    RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
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

	/* + change for CR 343589 */
    mParam.setProxyHandle(this);
    /* - change for CR 343589 */

    // Set the processing component. To be done after VFM_Component::construct
    // as ENS construct() reset it
    setProcessingComponent(&processingComponent);

    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+0, OMX_DirInput, OMX_BufferSupplyInput, 1, 0, OMX_PortDomainVideo));
    // min size is defined later on
    RETURN_OMX_ERROR_IF_ERROR(createPort(VPB+1, OMX_DirOutput, OMX_BufferSupplyOutput, 2, 0, OMX_PortDomainVideo));

    // set the default value of the ports
    ((H264Enc_Port *)getPort(VPB+0))->setDefault();
    ((H264Enc_Port *)getPort(VPB+1))->setDefault();

    // set the role of the component: video_encoder.avc
    DBC_ASSERT(1 == getRoleNB());
    ENS_String<20> role = "video_encoder.avc";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)OMX_SYMBIAN_INDEX_PARAM_COMMON_PIXELASPECTRATIO_NAME,   &mIndexParamPixelAspectRatio));
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)OMX_SYMBIAN_INDEX_PARAM_COMMON_COLORPRIMARY_NAME,       &mIndexParamColorPrimary));
#ifdef OMX_SYMBIAN_INDEX_PARAM_VIDEO_SEQUENCEHEADER_NAME
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)OMX_SYMBIAN_INDEX_PARAM_VIDEO_SEQUENCEHEADER_NAME,      &mIndexConfigVideoSequenceHeader));
#endif
#ifdef OMX_SYMBIAN_INDEX_CONFIG_VIDEO_SEQUENCEHEADER_NAME
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)OMX_SYMBIAN_INDEX_CONFIG_VIDEO_SEQUENCEHEADER_NAME,     &mIndexConfigVideoSequenceHeader));
#endif
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)OMX_SYMBIAN_INDEX_PARAM_VIDEO_AVCSEI_NAME,              &mIndexParamVideoAvcSei));
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)"OMX.ST.VFM.HeaderDataChanging",                        &mIndexParamHeaderDataChanging));
    RETURN_OMX_ERROR_IF_ERROR(getExtensionIndex((char*)"OMX.ST.VFM.CpbBufferSize",                             &mIndexParamCpbBufferSize));

    return OMX_ErrorNone;
}

// Implemention is required if MyProxy::construct() makes use of createSpecificPort()
VFM_Port *H264Enc_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
    return new H264Enc_Port(commonPortData, enscomp);
}


VFM_Component *H264Enc_Proxy::newProxy()
{
    return new H264Enc_Proxy;
}


H264Enc_Proxy::~H264Enc_Proxy()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::~H264Enc_Proxy");
    // very important as the delete of the ProcessingComponent is performed in the ENS automatically
    setProcessingComponent(0);
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::~H264Enc_Proxy");
}



EXPORT_C
OMX_ERRORTYPE H264EncFactoryMethod(ENS_Component_p * ppENSComponent)
{
    return VFM_Component::VFMFactoryMethod(ppENSComponent, H264Enc_Proxy::newProxy);
}

//+ code for CR 332873
//New handle added as a special case to select HDTV firmware for resolution above qHD 960x540
EXPORT_C
OMX_ERRORTYPE H264EncFactoryMethodCTCall(ENS_Component_p * ppENSComponent)
{
    IN0("");
    OMX_ERRORTYPE error;
    H264Enc_Proxy *comp = new H264Enc_Proxy;
    comp->isSpecialCTCase = OMX_TRUE; //flag is made TRUE here
    RETURN_XXX_IF_WRONG(comp!=0, OMX_ErrorInsufficientResources);
    error = comp->construct();
    if (error != OMX_ErrorNone) {
        delete comp;
        *ppENSComponent = 0;
        return error;
    }
    *ppENSComponent = comp;
    OUT0("");
    return OMX_ErrorNone;

}

//- code for CR 332873


OMX_ERRORTYPE H264Enc_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::setParameter");

    switch (nParamIndex) {
    case OMX_IndexParamVideoAvc:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_AVCTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoQuantization:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoSliceFMO:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_AVCSLICEFMO*>(pt)));
        break;
    case OMX_IndexParamVideoFastUpdate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt)));
        break;
    case OMX_IndexParamVideoIntraRefresh:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoBitrate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt)));
        break;
    case OMX_IndexParamVideoVBSMC:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_VBSMCTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoMotionVector:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt)));
        break;
    default:
        // SHAI extension
        if (nParamIndex==mIndexParamPixelAspectRatio) // todo: no structure defined in SHAI???
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_PARAM_PIXELASPECTRATIO*>(pt)))
        }
        else if (nParamIndex==mIndexParamColorPrimary)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*>(pt)));
        }
        else if (nParamIndex==mIndexParamVideoAvcSei)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE*>(pt)));
        }
        // Private extension
        else if (nParamIndex==mIndexParamHeaderDataChanging)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<VFM_PARAM_HEADERDATACHANGING_TYPE*>(pt)));
        }
        //+ code for CR 332873
       else if (nParamIndex==mIndexParamCpbBufferSize)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setParam(static_cast<OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE*>(pt)));
        }
        //- code for CR 332873
        // Default
        else
        {
            RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
        }
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::setParameter");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::getParameter");

    switch (nParamIndex) {
	case OMX_IndexParamPortDefinition:
	    RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_PARAM_PORTDEFINITIONTYPE*>(pt)));
	    break;
    case OMX_IndexParamVideoAvc:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_AVCTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoQuantization:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_QUANTIZATIONTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoSliceFMO:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_AVCSLICEFMO*>(pt)));
        break;
    case OMX_IndexParamVideoFastUpdate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE*>(pt)));
        break;
    case OMX_IndexParamVideoIntraRefresh:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_INTRAREFRESHTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoBitrate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_BITRATETYPE*>(pt)));
        break;
    case OMX_IndexParamVideoVBSMC:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_VBSMCTYPE*>(pt)));
        break;
    case OMX_IndexParamVideoMotionVector:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_MOTIONVECTORTYPE*>(pt)));
        break;
    default:
        // SHAI extension
        if (nParamIndex==mIndexParamPixelAspectRatio) // todo: no structure defined in SHAI???
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_PARAM_PIXELASPECTRATIO*>(pt)));
        }
        else if (nParamIndex==mIndexParamColorPrimary)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*>(pt)));
        }
        else if (nParamIndex==mIndexParamVideoAvcSei)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE*>(pt)));
        }
        // Private extension
        else if (nParamIndex==mIndexParamHeaderDataChanging)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<VFM_PARAM_HEADERDATACHANGING_TYPE*>(pt)));
        }
        //+ code for CR 332873
        else if (nParamIndex==mIndexParamCpbBufferSize)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getParam(static_cast<OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE*>(pt)));
        }
        //- code for CR 332873
        else
        {
            RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
        }
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::getParameter");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::setConfig");
    switch (nParamIndex) {
    case OMX_IndexConfigVideoNalSize:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_VIDEO_CONFIG_NALSIZE*>(pt)));
        break;
    case OMX_IndexConfigVideoBitrate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(pt)));
        break;
    case OMX_IndexConfigVideoFramerate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_CONFIG_FRAMERATETYPE*>(pt)));
        break;
    case OMX_IndexConfigVideoIntraVOPRefresh:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_CONFIG_INTRAREFRESHVOPTYPE*>(pt)));
        break;
    case OMX_IndexConfigVideoMacroBlockErrorMap:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_CONFIG_MACROBLOCKERRORMAPTYPE*>(pt)));
        break;
    case OMX_IndexConfigCommonInputCrop:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_CONFIG_RECTTYPE*>(pt)));
        break;
    /* + change for CR 343589 to support rotation */
    case OMX_IndexConfigCommonRotate:
    	RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_CONFIG_ROTATIONTYPE*>(pt)));
    	break;
    /* - change for CR 343589 to support rotation */
    default:
    // SHAI extension
        if (nParamIndex==mIndexConfigVideoSequenceHeader)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.setConfig(static_cast<OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE*>(pt)));
        }
        else
        {
            RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
        }
    }
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::setConfig");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::getConfig");
    switch (nParamIndex) {
    case OMX_IndexConfigVideoNalSize:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_VIDEO_CONFIG_NALSIZE*>(pt)));
        break;
    case OMX_IndexConfigVideoBitrate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(pt)));
        break;
    case OMX_IndexConfigVideoFramerate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_CONFIG_FRAMERATETYPE*>(pt)));
        break;
    case OMX_IndexConfigVideoIntraVOPRefresh:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_CONFIG_INTRAREFRESHVOPTYPE*>(pt)));
        break;
    case OMX_IndexConfigCommonInputCrop:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_CONFIG_RECTTYPE*>(pt)));
        break;
    /* + change for CR 343589 to support rotation */
    case OMX_IndexConfigCommonRotate:
    	RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_CONFIG_ROTATIONTYPE*>(pt)));
    	break;
    /* - change for CR 343589 to support rotation */
    //todo: OMX_IndexConfigVideoMacroBlockErrorMap
    default:
    // SHAI extension
        if (nParamIndex==mIndexConfigVideoSequenceHeader)
        {
            RETURN_OMX_ERROR_IF_ERROR(mParam.getConfig(static_cast<OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE*>(pt)));
        }
        else
        {
            RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
        }
    }
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::getConfig");
    return OMX_ErrorNone;
}

OMX_BOOL H264Enc_Proxy::isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_Proxy::isPortSpecificParameter");

    OMX_BOOL isPortSpecific = OMX_FALSE;

    switch(nParamIndex) {
        case OMX_IndexParamVideoAvc:
        case OMX_IndexParamVideoQuantization:
        case OMX_IndexParamVideoSliceFMO:
        case OMX_IndexParamVideoFastUpdate:
        case OMX_IndexParamVideoIntraRefresh:
        case OMX_IndexParamVideoBitrate:
        case OMX_IndexParamVideoVBSMC:
        case OMX_IndexParamVideoMotionVector:
            isPortSpecific = OMX_TRUE;
            break;
        default:
        // SHAI extension
            if (nParamIndex == mIndexParamPixelAspectRatio ||
                nParamIndex == mIndexParamColorPrimary     ||
                nParamIndex == mIndexParamVideoAvcSei      ||
                nParamIndex == mIndexParamCpbBufferSize)
            {
                isPortSpecific = OMX_TRUE;
            }
            else
            {
                isPortSpecific = VFM_Component::isPortSpecificParameter(nParamIndex);
            }
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_Proxy::isPortSpecificParameter");
    return isPortSpecific;
}

void H264Enc_Proxy::setProfileLevelSupported()
{
    // TODO: update
    // Targeted profile level supported (8500 V1 board + perf enhencement needed)
    /* +Change for ER 400650,(orignal ER 360599) */
    OMX_U32 maxLevelSet = VFM_SocCapabilityMgt::getLevelCapabilities();
    if (maxLevelSet == 32)
    {
        setOneProfileLevelSupported(OMX_VIDEO_AVCProfileBaseline,    OMX_VIDEO_AVCLevel31);
        setOneProfileLevelSupported(OMX_VIDEO_AVCProfileMain,        OMX_VIDEO_AVCLevel31);
        setOneProfileLevelSupported(OMX_VIDEO_AVCProfileHigh,        OMX_VIDEO_AVCLevel31);
	}
	else
	{
    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileBaseline,    OMX_VIDEO_AVCLevel4);
    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileMain,        OMX_VIDEO_AVCLevel4);
    setOneProfileLevelSupported(OMX_VIDEO_AVCProfileHigh,        OMX_VIDEO_AVCLevel4);
	}
	/* -Change for ER 400650,(orignal ER 360599) */
}

void H264Enc_Proxy::setVideoSettingsSupported()
{
    setOneVideoSettingsSupported(0, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar, 0);
    setOneVideoSettingsSupported(0, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, 0);
    setOneVideoSettingsSupported(0, OMX_VIDEO_CodingUnused, (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque, 0);
    setOneVideoSettingsSupported(1, OMX_VIDEO_CodingAVC, OMX_COLOR_FormatUnused, 15<<16);
}


OMX_ERRORTYPE H264Enc_Proxy::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_PARAM_VIDEO_AVCSEI_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamVideoAvcSei;
        return OMX_ErrorNone;
    }

	//+ code for CR 332873
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.CpbBufferSize") {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamVideoCpbBufferSize;
        return OMX_ErrorNone;
    }
    //- code for CR 332873

    return VFM_Component::getExtensionIndex(cParameterName, pIndexType);
}



EXPORT_C RM_STATUS_E H264Enc_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    DBC_ASSERT(getScratchEsram()==OMX_TRUE);

    OMX_U32 svaHwPipe = RM_ESBUFID_EMPTY;
    OMX_U32 width;
    OMX_U32 height;
    OMX_U32 nbOfMacroBlocks;
    OMX_U32 nbOfMacroBlocksWVGA;

    getFrameSize(VPB+0, &width, &height);
    /* +Change for 372717 */
    width = make16multiple(width);
    height = make16multiple(height);
    /* -Change for 372717 */
    nbOfMacroBlocks = (width*height)/(16*16);

    nbOfMacroBlocksWVGA = (864*480)/(16*16);

    OstTraceFiltInst1(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation => nbOfMacroBlocksWVGA (%d) ",nbOfMacroBlocksWVGA);

    /* + change for ER 346056 */
	OMX_U32 thresholdValue = (1280/16)*(720/16);
    if ( nbOfMacroBlocks >= thresholdValue ) 
    {
	/* - change for ER 346056 */
		OstTraceFiltInst0(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation - In this case 1");
		OstTraceFiltInst3(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation =>Width (%d) Height (%d) nbOfMacroBlocks (%d) ",width,height,nbOfMacroBlocks);
        svaHwPipe = RM_ESBUFID_DEFAULT;
    }
    else if( nbOfMacroBlocks <= nbOfMacroBlocksWVGA ) // Visio firmware with "size < WVGA"
    {
		OstTraceFiltInst0(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation - In this case 2");
		OstTraceFiltInst3(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation =>Width (%d) Height (%d) nbOfMacroBlocks (%d) ",width,height,nbOfMacroBlocks);
        svaHwPipe = RM_ESBUFID_DEFAULT;
    }
    else // Visio firmware with "WVGA < size <= 720p"
    {
		OstTraceFiltInst0(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation - In this case 3");
		OstTraceFiltInst3(TRACE_FLOW,"H264Enc_Proxy::getResourcesEstimation =>Width (%d) Height (%d) nbOfMacroBlocks (%d) ",width,height,nbOfMacroBlocks);
        svaHwPipe = RM_ESBUFID_BIG;
    }

    // These default values must be redefined in each codec
    pEstimationData->sMcpsSva.nMax = 10; // MIPS = MMDSP_SVA_MCPS.
    pEstimationData->sMcpsSva.nAvg = 10;
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 500; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 500; // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = svaHwPipe;    // RM_ESBUFID_BIG required for 1080p on symbian.  RM_ESBUFID_BIG for H264Encode. RM_ESBUFID_DEFAULT for others
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    return RM_E_NONE;
}


void H264Enc_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType,
									   t_nmf_service_data *serviceData)
{
    OstTraceFiltInst0(TRACE_FLOW, "H264Enc_Proxy::Enter NmfPanicCallback serviceType");
	switch (serviceType) {
		case NMF_SERVICE_PANIC:
		  if(serviceData->panic.panicSource==MPC_EE)
		  {
              OstTraceFiltInst0(TRACE_FLOW, "panicSource = MPC");
			  if (serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SVA_CORE_ID))
			  {
                  OstTraceFiltInst0(TRACE_FLOW, "panicSource core is not SVA");
				  return;
			  }
			  else
			  {
			      if (processingComponent.isNMFPanicSource(MPC_EE, serviceData->panic.info.mpc.faultingComponent))
			      {
                     OstTraceFiltInst0(TRACE_FLOW, "H264Enc_Proxy MPC generated Panic");
                     eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
				     processingComponent.errorRecoveryDestroyAll();
				     OstTraceFiltInst1(TRACE_FLOW, "H264Enc_Proxy::NmfPanicCallback serviceType =%d \n",serviceType);
			      }
			      else
			      {
					  OstTraceInt0(TRACE_WARNING,"panicSource is SVA but NOT H264ENC");
				  }
			  }
		  }
		  else
		  {
			  OstTraceFiltInst0(TRACE_FLOW, "panicSource core is HOSTEE");
                  if (processingComponent.isNMFPanicSource(HOST_EE, (OMX_U32)serviceData->panic.info.host.faultingComponent))
			      {
                     OstTraceFiltInst0(TRACE_FLOW, "H264Enc_Proxy ARMNMF generated Panic");
                     eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
				     processingComponent.errorRecoveryDestroyAll();
				     OstTraceFiltInst1(TRACE_FLOW, "H264Enc_Proxy::NmfPanicCallback serviceType =%d \n",serviceType);
			      }
			      else
			      {
					  //**//OstTraceInt0(TRACE_WARNING,"panicSource is HOSTEE but NOT H264ENC");
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
		  OstTraceInt1(TRACE_ERROR, "H264Enc_Proxy::NmfPanicCallback: unknown service %d", serviceType);
		  break;
	}
}



