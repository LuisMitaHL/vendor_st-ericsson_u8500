/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ VFM

#include "VFM_Component.h"
#include "VFM_Index.h"
#include "VFM_Types.h"
#include "VFM_DDepUtility.h"

#ifdef ANDROID
#include "mmhwbuffer_ext.h"
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_ComponentTraces.h"
#endif

// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG 
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

inline void VFM_Component::vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        OstTraceInt3(TRACE_ERROR, "VFM: VFM_Component: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

VFM_Component_RDB::VFM_Component_RDB(OMX_U32 nbOfDomains) : ENS_ResourcesDB(nbOfDomains)
{
    IN0("");

    VFM_Component::vfm_assert_static((nbOfDomains==1 || nbOfDomains==2), __LINE__, OMX_TRUE, nbOfDomains);
    setDefaultNMFDomainType(RM_NMFD_PROCSVA, 0);
    if (nbOfDomains==2) {
        setDefaultNMFDomainType(RM_NMFD_HWPIPESVA, 1);
    }
    OUT0("");
}

EXPORT_C VFM_Component::VFM_Component(VFM_ParamAndConfig *Param, OMX_BOOL scratchEsram) :
            pParam(Param), mNbProfileLevelSupported(0), mScratchEsram(scratchEsram)
{
    pFwPerfDataPtr = (OMX_PTR)0;
    mIsARMLoadComputed = OMX_FALSE;
    mNbVideoSettingsSupported[0] = mNbVideoSettingsSupported[1] = 0;
    mConstructTime = VFM_HostPerformances::getTime();
}

EXPORT_C RM_STATUS_E VFM_Component::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    vfm_assert_static((!mScratchEsram), __LINE__, OMX_TRUE);
    return RM_E_NONE;
}

EXPORT_C
VFM_Component::~VFM_Component()
{
    IN0("");
    OUT0("");
}

EXPORT_C
OMX_ERRORTYPE VFM_Component::createResourcesDB() {
    IN0("");
    if (mScratchEsram) {
        mRMP = new VFM_Component_RDB(2);
    } else {
        mRMP = new VFM_Component_RDB(1);
    }
    if (mRMP == 0) {
        OUT0("");
        return OMX_ErrorInsufficientResources;
    }

    OUT0("");
    return OMX_ErrorNone;
}

//+ER420808
EXPORT_C
OMX_ERRORTYPE VFM_Component::doSpecificInternalState(const ENS_FsmEvent *evt)
{
	IN0("");
	OstTraceInt2(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, evt->signal() == %d", (unsigned int)getOMXHandle(), evt->signal());

	OMX_U32 priority = 0, groupid = 0;

	if(getCompFsm()->getState() == OMX_StateLoaded)
	{
		OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, inside OMX_StateLoaded", (unsigned int)getOMXHandle());
		switch(evt->signal())
		{
			case ENTRY_SIG:
				OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, inside ENTRY_SIG", (unsigned int)getOMXHandle());
				//We now decrease priority of the current component instance
				priority = getGroupPriorityValue() + 1;
				groupid = getGroupPriorityId() + 1;

				OstTraceInt3(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x Setting priority %d and groupid %d", (unsigned int)getOMXHandle(), priority, groupid);

				setPriorityMgmt(priority, groupid);
				RETURN_OMX_ERROR_IF_ERROR(ENS_HwRm::HWRM_Notify_NewPolicyCfg((OMX_PTR)getOMXHandle(), priority, groupid, 0));
				break;

			case OMX_SETSTATE_SIG:
				OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, inside OMX_SETSTATE_SIG", (unsigned int)getOMXHandle());
				//We now decrease priority of the current component instance
				priority = getGroupPriorityValue() + 1;
				groupid = getGroupPriorityId() + 1;

				OstTraceInt3(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x Setting priority %d and groupid %d", (unsigned int)getOMXHandle(), priority, groupid);

				setPriorityMgmt(priority, groupid);
				RETURN_OMX_ERROR_IF_ERROR(ENS_HwRm::HWRM_Notify_NewPolicyCfg((OMX_PTR)getOMXHandle(), priority, groupid, 0));
				break;

			default:
				break;
		}
	}
	else if(getCompFsm()->getState() == OMX_StateCheckRscAvailability)
	{
		OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, inside OMX_StateCheckRscAvailability", (unsigned int)getOMXHandle());
		if(evt->signal() == OMX_RESRESERVATIONPROCESSED_SIG)
		{
			OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, inside OMX_RESRESERVATIONPROCESSED_SIG", (unsigned int)getOMXHandle());

			ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
			if (OMX_TRUE == ackRscEvt->resGranted()) {
				//We now increase priority of the current component instance
				if(getGroupPriorityValue() > 0) {
					OstTraceInt1(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x, ackRscEvt->resGranted() == TRUE ", (unsigned int)getOMXHandle());
					priority = getGroupPriorityValue() - 1;
					groupid = getGroupPriorityId() - 1;
				}
				OstTraceInt3(TRACE_INFO, "doSpecificInternalState(): OMX handle 0x%x Setting priority %d and groupid %d", (unsigned int)getOMXHandle(), priority, groupid);
				setPriorityMgmt(priority, groupid);
				RETURN_OMX_ERROR_IF_ERROR(ENS_HwRm::HWRM_Notify_NewPolicyCfg((OMX_PTR)getOMXHandle(), priority, groupid, 0));
			}
		}
	}
return OMX_ErrorNone;
	OUT0("");
}
//-ER420808

EXPORT_C
OMX_ERRORTYPE VFM_Component::createPort(
        OMX_U32 nPortIndex,
        OMX_DIRTYPE eDir,
        OMX_BUFFERSUPPLIERTYPE eSupplierPref,
        OMX_U32 nBufferCountMin,
        OMX_U32 nBufferSizeMin,
        OMX_PORTDOMAINTYPE domain)
{
    IN0("");

    RETURN_XXX_IF_WRONG((nPortIndex<getPortCount()), OMX_ErrorBadPortIndex);
    ENS_Port * port = newPort(
        EnsCommonPortData(
            nPortIndex,
            eDir,
            nBufferCountMin,
            nBufferSizeMin,
            domain,
            eSupplierPref
        ),
        *this);

    RETURN_XXX_IF_WRONG((port!=0), OMX_ErrorInsufficientResources);
    addPort(port);
    OUT0("");
    return OMX_ErrorNone;
}


EXPORT_C
OMX_ERRORTYPE VFM_Component::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    IN0("");
    switch ((int)nParamIndex) {
    case OMX_IndexParamVideoProfileLevelCurrent:
        OMX_U32 Profile, Level;
        pParam->getProfileLevel(&Profile, &Level);
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamVideoProfileLevelCurrent(pt, Profile, Level));
        break;

    case OMX_IndexParamVideoProfileLevelQuerySupported:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamVideoProfileLevelQuerySupported(pt));
        break;

    case OMX_IndexParamVideoPortFormat:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamVideoPortFormat(pt));
        break;

    case OMX_IndexParamImagePortFormat:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamImagePortFormat(pt));
        break;

    case OMX_IndexParamColorPrimary:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamColorPrimary(pt));
        break;

    case OMX_IndexParamPixelAspectRatio:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamPixelAspectRatio(pt));
        break;

    case VFM_IndexParamSupportedExtension:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamSupportedExtension(pt));
        break;

#ifdef PACKET_VIDEO_SUPPORT
    case PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getPVCapability(pt));
        break;
#endif

    case VFM_IndexParamARMLoadComputation:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamArmLoadComputation(pt));
        break;

    case VFM_IndexParamThumbnailGeneration:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamThumbnailGeneration(pt));
        break;

    /* +Change start for CR333616 Delay recycling */
    case VFM_IndexParamRecyclingDelay:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamRecyclingDelay(pt));
        break;
    /* -Change end for CR333616 Delay recycling */

    /* +Change start for CR330198 Revert to DTS from PTS */
    case VFM_IndexParamTimestampDisplayOrder:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamTimestampDisplayOrder(pt));
        break;
    /* -Change end for CR330198 Revert to DTS from PTS */

    /* +Change start for CR333618 Added for IOMX renderer */
    case VFM_IndexParamSharedchunkInPlatformprivate:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamSharedchunkInPlatformprivate(pt));
        break;
    /* -Change end for CR333618 Added for IOMX renderer */

    default:
        RETURN_OMX_ERROR_IF_ERROR(ENS_Component::getParameter(nParamIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C
OMX_ERRORTYPE VFM_Component::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    IN0("");
    OMX_BOOL has_changed;

    switch ((int)nParamIndex) {
    case OMX_IndexParamVideoProfileLevelCurrent:
        OMX_U32 Profile, Level;
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamVideoProfileLevelCurrent(pt, &Profile, &Level, &has_changed));
        pParam->setProfileLevel(Profile, Level);
        break;

    case OMX_IndexParamVideoPortFormat:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamVideoPortFormat(pt));
        break;

    case OMX_IndexParamImagePortFormat:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamImagePortFormat(pt));
        break;

    case OMX_IndexParamColorPrimary:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamColorPrimary(pt));
        break;

    case VFM_IndexParamSupportedExtension:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamSupportedExtension(pt));
        break;

    case VFM_IndexParamHeaderDataChanging:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamHeaderDataChanging(pt));
        break;

    case VFM_IndexParamARMLoadComputation:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamArmLoadComputation(pt));
        break;

    case VFM_IndexParamThumbnailGeneration:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamThumbnailGeneration(pt));
        break;

    /* +Change start for CR333616 Delay recycling */
    case VFM_IndexParamRecyclingDelay:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamRecyclingDelay(pt));
        break;
    /* -Change end for CR333616 Delay recycling */

    /* +Change start for CR330198 Revert to DTS from PTS */
    case VFM_IndexParamTimestampDisplayOrder:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamTimestampDisplayOrder(pt));
        break;
    /* -Change end for CR330198 Revert to DTS from PTS */

    /* +Change start for CR333618 Added for IOMX renderer */
    case VFM_IndexParamSharedchunkInPlatformprivate:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamSharedchunkInPlatformprivate(pt));
        break;
    /* -Change end for CR333618 Added for IOMX renderer */

	/* +Change Start for CR 399938 */
#ifdef ANDROID
    case OMX_google_android_index_storeMetaDataInBuffers:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexParamStoreMetadataInBuffers(pt));
        break;
#endif
	/* -Change End for CR 399938 */

    default:
        RETURN_OMX_ERROR_IF_ERROR(ENS_Component::setParameter(nParamIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C
OMX_ERRORTYPE VFM_Component::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pt) const
{
    IN0("");
    switch ((int)nConfigIndex) {
    case OMX_IndexConfigVideoMacroBlockErrorMap:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexConfigVideoMacroBlockErrorMap(pt));
        break;

    case OMX_IndexParamVideoFastUpdate:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamVideoFastUpdate(pt));
        break;

    case VFM_IndexParamVideoFwPerfData:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexParamVideoFwPerfData(pt));
        break;

    case OMX_IndexConfigCommonOutputCrop:
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexConfigCommonOutputCrop(pt));
        break;

    /* +Changes for CR 399075 */
    case VFM_IndexConfigDecodeImmediate :
        RETURN_OMX_ERROR_IF_ERROR(pParam->getIndexConfigImmediateRelease(pt));
        break;
    /* -Changes for CR 399075 */

    default:
        RETURN_OMX_ERROR_IF_ERROR(ENS_Component::getConfig(nConfigIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_Component::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pt)
{
    IN0("");
    switch ((int)nConfigIndex) {
    case OMX_IndexConfigCommonOutputCrop:
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexConfigCommonOutputCrop(pt));
        break;

	/* +Changes for CR 399075 */
    case VFM_IndexConfigDecodeImmediate :
        RETURN_OMX_ERROR_IF_ERROR(pParam->setIndexConfigImmediateRelease(pt));
        break;
	/* -Changes for CR 399075 */

    default:
        RETURN_OMX_ERROR_IF_ERROR(ENS_Component::setConfig(nConfigIndex, pt));
        break;
    }

    OUT0("");
    return OMX_ErrorNone;
}



EXPORT_C
OMX_ERRORTYPE VFM_Component::VFMFactoryMethod(ENS_Component_p * ppENSComponent, VFM_Component *(*newProxy)(void))
{
    IN0("");
    OMX_ERRORTYPE error;
    VFM_Component *comp = newProxy();
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

EXPORT_C void VFM_Component::setOneProfileLevelSupported(OMX_U32 profile, OMX_U32 level)
{
    IN0("");
    vfm_assert_static((mNbProfileLevelSupported<(OMX_U32)mMaxProfileLevelSupported), __LINE__, OMX_TRUE, mNbProfileLevelSupported, mMaxProfileLevelSupported);
    mProfileSupported[mNbProfileLevelSupported] = profile;
    mLevelSupported  [mNbProfileLevelSupported] = level;
    mNbProfileLevelSupported ++;
    OUT0("");
}

EXPORT_C void VFM_Component::clearAllProfileLevelSupported()
{
    IN0("");
    mNbProfileLevelSupported = 0;
    OUT0("");
}

OMX_ERRORTYPE VFM_Component::getSupportedProfileLevel(OMX_U32 index, OMX_U32 *profile, OMX_U32 *level)
{
    IN0("");
    RETURN_XXX_IF_WRONG((index<mNbProfileLevelSupported), OMX_ErrorNoMore);
    *profile = mProfileSupported[index];
    *level = mLevelSupported[index];
    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C void VFM_Component::setVideoSettingsSupported()
{
}

EXPORT_C void VFM_Component::setOneVideoSettingsSupported(OMX_U32 nPortIndex, OMX_VIDEO_CODINGTYPE eCompressionFormat, OMX_COLOR_FORMATTYPE eColorFormat, OMX_U32 xFrameRate)
{
    IN0("");
    OMX_U32 index = mNbVideoSettingsSupported[nPortIndex];
    vfm_assert_static((index<(OMX_U32)mMaxVideoSettingsSupported), __LINE__, OMX_TRUE, index, mMaxVideoSettingsSupported);

    mVideoSettingsSupported[nPortIndex][index].eCompressionFormat = eCompressionFormat;
    mVideoSettingsSupported[nPortIndex][index].eColorFormat = eColorFormat;
    mVideoSettingsSupported[nPortIndex][index].xFramerate = xFrameRate;
    mNbVideoSettingsSupported[nPortIndex] ++;

    OUT0("");
}

OMX_ERRORTYPE VFM_Component::getSupportedVideoSettings(OMX_U32 nPortIndex, OMX_U32 index, OMX_VIDEO_CODINGTYPE *eCompressionFormat, OMX_COLOR_FORMATTYPE *eColorFormat, OMX_U32 *xFrameRate)
{
    IN0("");
    RETURN_XXX_IF_WRONG((index<mNbVideoSettingsSupported[nPortIndex]), OMX_ErrorNoMore);
    *eCompressionFormat = mVideoSettingsSupported[nPortIndex][index].eCompressionFormat;
    *eColorFormat = mVideoSettingsSupported[nPortIndex][index].eColorFormat;
    *xFrameRate = mVideoSettingsSupported[nPortIndex][index].xFramerate;
    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C OMX_BOOL VFM_Component::isCoder()
{
    OMX_COLOR_FORMATTYPE color0, color1;
    VFM_Port *pPort0, *pPort1;
    vfm_assert_static((getPortCount()==1 || getPortCount()==2), __LINE__, OMX_TRUE, getPortCount());

    if (getPortCount()==1)  return OMX_FALSE;

    pPort0 = (VFM_Port *)getPort(0);
    pPort1 = (VFM_Port *)getPort(1);
    vfm_assert_static((pPort0->getDirection() != pPort1->getDirection()), __LINE__, OMX_TRUE);
    vfm_assert_static((pPort0->getDomain() == pPort1->getDomain()), __LINE__, OMX_TRUE);
    color0 = pPort0->getColorFormat();
    color1 = pPort1->getColorFormat();
    vfm_assert_static((color0==OMX_COLOR_FormatUnused || color1==OMX_COLOR_FormatUnused), __LINE__, OMX_TRUE, color0, color1);
    vfm_assert_static((color0!=OMX_COLOR_FormatUnused || color1!=OMX_COLOR_FormatUnused), __LINE__, OMX_TRUE, color0, color1);
    if (color0==OMX_COLOR_FormatUnused) {
        return OMX_FALSE;
    } else {
        return OMX_TRUE;
    }
}

EXPORT_C OMX_BOOL VFM_Component::isDecoder()
{
    if (getPortCount()==1)  return OMX_FALSE;
    if (isCoder()) {
        return OMX_FALSE;
    } else {
        return OMX_TRUE;
    }
}

/* +Change for CR 399938 */
EXPORT_C OMX_BOOL VFM_Component::isEncoder()
{
    if (getPortCount()==1)  return OMX_FALSE;
    if (isCoder()) {
        return OMX_TRUE;
    } else {
        return OMX_FALSE;
    }
}
/* -Change for CR 399938 */

EXPORT_C OMX_BOOL VFM_Component::isSourceSink()
{
    if (getPortCount()==1)  {
        return OMX_TRUE;
    } else {
        return OMX_FALSE;
    }
}

EXPORT_C
OMX_ERRORTYPE VFM_Component::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_CONFIG_GPSLOCATION_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexConfigImageGPSLocation;
        return OMX_ErrorNone;
    }

    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_PARAM_COMMON_COLORPRIMARY_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamColorPrimary;
        return OMX_ErrorNone;
    }

#ifdef OMX_SYMBIAN_INDEX_PARAM_VIDEO_SEQUENCEHEADER_NAME
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_PARAM_VIDEO_SEQUENCEHEADER_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamVideoSequenceHeader;
        return OMX_ErrorNone;
    }
#endif

#ifdef OMX_SYMBIAN_INDEX_CONFIG_VIDEO_SEQUENCEHEADER_NAME
    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_CONFIG_VIDEO_SEQUENCEHEADER_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamVideoSequenceHeader;
        return OMX_ErrorNone;
    }
#endif

    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == OMX_SYMBIAN_INDEX_PARAM_COMMON_PIXELASPECTRATIO_NAME) {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamPixelAspectRatio;
        return OMX_ErrorNone;
    }

    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.memory_status") {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexConfigMemory;
        return OMX_ErrorNone;
    }

    if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.PAR") {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamPixelAspectRatio;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.fw_perf_data")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamVideoFwPerfData;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.SupportedExtension")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamSupportedExtension;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.HeaderDataChanging")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamHeaderDataChanging;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.NalStreamFormatSupported")
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexKhronosExtParamNALStreamFormatSupported;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.NalStreamFormat")
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexKhronosExtParamNALStreamFormat;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.ThumbnailGeneration")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamThumbnailGeneration;
        return OMX_ErrorNone;
    }

    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.StaticDPBSize")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamStaticDPBSize;
        return OMX_ErrorNone;
    }

    /* +Change start for CR333616 Delay recycling */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.RecyclingDelay")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamRecyclingDelay;
        return OMX_ErrorNone;
    }
    /* -Change end for CR333616 Delay recycling */

    /* +Change start for CR330198 Revert to DTS from PTS */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.TimestampDisplayOrder")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamTimestampDisplayOrder;
        return OMX_ErrorNone;
    }
    /* -Change end for CR330198 Revert to DTS from PTS */

    /* +Change start for CR333618 Added for IOMX renderer */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.SharedchunkInPlatformprivate")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamSharedchunkInPlatformprivate;
        return OMX_ErrorNone;
    }
    /* -Change end for CR333618 Added for IOMX renderer */

    /* +Change start for CR332521 IOMX UseBuffer */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.iOMX")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamiOMX;
        return OMX_ErrorNone;
    }
    /* -Change end for CR332521 IOMX UseBuffer */

	/* +Change start for CR338066 MPEG4Enc DCI */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.MPEG4SHORTDCI")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexParamMpeg4ShortDCI;
        return OMX_ErrorNone;
    }
	/* -Change end for CR338066 MPEG4Enc DCI */

	/* +Change start for CR399075 */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.DecodeImmediateRelease")
    {
        *pIndexType = (OMX_INDEXTYPE)VFM_IndexConfigDecodeImmediate;
        return OMX_ErrorNone;
    }
	/* -Change end for CR399075 */

	/* +Change start for CR368983 */
    if(ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName) == "OMX.ST.VFM.BRCClipping")
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_IndexParamVideoBRCClipping;
        return OMX_ErrorNone;
    }
	/* -Change end for CR368983 */

    return ENS_Component::getExtensionIndex(cParameterName, pIndexType);
}

OMX_BOOL VFM_Component::skipEmptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    if (pBuffer->nFilledLen != 0) {
        return OMX_FALSE;
    }

    OMX_U32 nFlags = pBuffer->nFlags;
    nFlags &= (~OMX_BUFFERFLAG_DECODEONLY);
    nFlags &= (~OMX_BUFFERFLAG_DATACORRUPT);
    nFlags &= (~OMX_BUFFERFLAG_ENDOFFRAME);
    nFlags &= (~OMX_BUFFERFLAG_SYNCFRAME);
    nFlags &= (~OMX_BUFFERFLAG_CODECCONFIG);
    // OMX_BUFFERFLAG_EOS, OMX_BUFFERFLAG_STARTTIME and
    // OMX_BUFFERFLAG_EXTRADATA are passed

    if (nFlags!=0) {
        return OMX_FALSE;
    }

    if (pBuffer->pMarkData) {
        return OMX_FALSE;
    }

    return OMX_TRUE;
}

EXPORT_C OMX_S32 VFM_Component::getMaxBuffers(OMX_U32 portIndex) const
{
    return -1;
}

EXPORT_C OMX_ERRORTYPE VFM_Component::construct(OMX_U32 nPortCount)
{
    setProfileLevelSupported();
    setVideoSettingsSupported();
    return ENS_Component::construct(nPortCount);
}


