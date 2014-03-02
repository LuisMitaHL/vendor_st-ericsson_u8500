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
#include "VFM_ParamAndConfig.h"
#include "vfm_common.idt"

#include "VFM_DDepUtility.h"

// macro __ndk5500_a0__ commented for ER429711
//#ifdef __ndk5500_a0__
/* + change for ER 350391 */
//utility API for sorting the ARRAY
static OMX_U32 check(OMX_S32 value,OMX_U32 maxValue);
static OMX_U32 cor(OMX_U32 i,OMX_U32 first,OMX_U32 last,OMX_U32 maxValue);
static void sort_reorder(OMX_TICKS arr[],OMX_U32 first,OMX_U32 last,OMX_U32 maxValue);
/* - change for ER 350391 */
//#endif

#ifdef ANDROID
#define PRINTPG
#else
#define PRINTPG
#endif

#define META_BUFFER ((video_metadata_t *)((*pBuffer)->pBuffer))->handle

EXPORT_C VFM_Status::VFM_Status()
{
    status = OMX_TRUE;
}

EXPORT_C VFM_ParamAndConfig::VFM_ParamAndConfig(VFM_Component *component) :
    mColorPrimary(OMX_ColorPrimaryUnknown), mSupportedExtension(1<<0),
    mHeaderDataChanging(OMX_FALSE), mThumbnailGeneration(OMX_FALSE),
    mImmediateRelease(OMX_FALSE), // Change for CR 399075
    /* +Change start for CR333618 Added for IOMX renderer */
    mSharedchunkInPlatformprivate(OMX_FALSE),
/* -Change end for CR333618 Added for IOMX renderer */
/* +Change start for CR 399938 */
    mStoreMetadataInBuffers(OMX_FALSE),
/* -Change start for CR 399938 */


pComponent(component)
{
    setProfileLevel(0, 0);
    setPAR(0, 0);
    setNoCropping();
}


EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamQFactor(OMX_PTR pt_org, OMX_U16 *QFactor, OMX_BOOL *has_changed)
{
    IN0("");
    OMX_IMAGE_PARAM_QFACTORTYPE *pt = (OMX_IMAGE_PARAM_QFACTORTYPE *)pt_org;
    RETURN_XXX_IF_WRONG(1 <= pt->nQFactor && pt->nQFactor <= 100, OMX_ErrorBadParameter);
    *has_changed = (OMX_BOOL)(*QFactor != pt->nQFactor);
    *QFactor = pt->nQFactor;
    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamQFactor(OMX_PTR pt_org, OMX_U16 QFactor) const
{
    IN0("");
    OMX_IMAGE_PARAM_QFACTORTYPE *pt = (OMX_IMAGE_PARAM_QFACTORTYPE *) pt_org;
    pt->nQFactor = QFactor;
    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamVideoProfileLevelCurrent(OMX_PTR pt_org, OMX_U32 *Profile, OMX_U32 *Level, OMX_BOOL *has_changed)
{
    IN0("");
    OMX_VIDEO_PARAM_PROFILELEVELTYPE *pt = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainVideo, OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(pPort->getCompressionFormat()!=OMX_VIDEO_CodingUnused, OMX_ErrorBadPortIndex);

    *has_changed = (OMX_BOOL)(*Profile!=pt->eProfile || *Level!=pt->eLevel);
    *Profile = pt->eProfile;
    *Level = pt->eLevel;
    OUT0("");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamVideoProfileLevelCurrent(OMX_PTR pt_org, OMX_U32 Profile, OMX_U32 Level) const
{
    IN0("");
    OMX_VIDEO_PARAM_PROFILELEVELTYPE *pt = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainVideo, OMX_ErrorBadParameter);

    pt->eProfile = Profile;
    pt->eLevel = Level;
    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamVideoProfileLevelQuerySupported(OMX_PTR pt_org) const
{
    IN0("");
    OMX_VIDEO_PARAM_PROFILELEVELTYPE *pt = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainVideo, OMX_ErrorBadParameter);
    OMX_VIDEO_PORTDEFINITIONTYPE *pVideoParam = pPort->getParamVideoPortDefinition();
    RETURN_XXX_IF_WRONG(pVideoParam->eCompressionFormat!=OMX_VIDEO_CodingUnused, OMX_ErrorBadPortIndex);

    OUT0("");
    return pComponent->getSupportedProfileLevel(pt->nProfileIndex, &(pt->eProfile), &(pt->eLevel));
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamVideoPortFormat(OMX_PTR pt_org)
{
    IN0("");
    OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainVideo, OMX_ErrorNotImplemented);

    RETURN_OMX_ERROR_IF_ERROR(pPort->checkIndexParamVideoPortFormat(pt));
    OMX_VIDEO_PORTDEFINITIONTYPE *pVideoParam = pPort->getParamVideoPortDefinition();
    pVideoParam->eCompressionFormat = pt->eCompressionFormat;
    pVideoParam->eColorFormat = pt->eColorFormat;
    pVideoParam->xFramerate = pt->xFramerate;

    // if it changed the size of the buffers, update the ParamPortDefinition
    if (pPort->getBufferSizeInParam() != pPort->getBufferSize()) {
        OMX_PARAM_PORTDEFINITIONTYPE local_param = *(pPort->getParamPortDefinition());
        pPort->updateSettings(OMX_IndexParamPortDefinition, &local_param);
    }

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamVideoPortFormat(OMX_PTR pt_org) const
{
    IN0("");
    OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainVideo, OMX_ErrorBadParameter);

    OUT0("");
    return pComponent->getSupportedVideoSettings(pt->nPortIndex, pt->nIndex, &(pt->eCompressionFormat), &(pt->eColorFormat), &(pt->xFramerate));
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamThumbnailGeneration(OMX_PTR pt_org)
{
    IN0("");
    VFM_PARAM_THUMBNAIL_GENERATION_TYPE *pt = (VFM_PARAM_THUMBNAIL_GENERATION_TYPE *)pt_org;
    mThumbnailGeneration = pt->bThumbnailGeneration;

    for (int i=0; i<2; i++) {
        VFM_Port *pPort = (VFM_Port *)pComponent->getPort(i);
        pPort->getParamPortDefinition()->nBufferCountMin = pPort->getBufferCountMin();
        pPort->getParamPortDefinition()->nBufferCountActual = pPort->getParamPortDefinition()->nBufferCountMin;
    }

    OUT0("");
    return OMX_ErrorNone;
}

/* +Changes for CR 399075 */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexConfigImmediateRelease(OMX_PTR pt_org)
{
    IN0("");
    VFM_CONFIG_DECODEIMMEDIATE *pt = (VFM_CONFIG_DECODEIMMEDIATE *)pt_org;
    //only updated for decoders, otherwise it is always OMX_FALSE
    if (pComponent->isDecoder())
        mImmediateRelease = pt->bImmediateRelease;

    OUT0("");
    return OMX_ErrorNone;
}
/* -Changes for CR 399075 */

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamThumbnailGeneration(OMX_PTR pt_org) const
{
    IN0("");
    VFM_PARAM_THUMBNAIL_GENERATION_TYPE *pt = (VFM_PARAM_THUMBNAIL_GENERATION_TYPE *)pt_org;
    pt->bThumbnailGeneration = mThumbnailGeneration;
    OUT0("");
    return OMX_ErrorNone;
}

/* +Changes for CR 399075 */
OMX_ERRORTYPE VFM_ParamAndConfig::getIndexConfigImmediateRelease(OMX_PTR pt_org) const
{
    IN0("");
    VFM_CONFIG_DECODEIMMEDIATE *pt = (VFM_CONFIG_DECODEIMMEDIATE *)pt_org;
    pt->bImmediateRelease = mImmediateRelease;
    OUT0("");
    return OMX_ErrorNone;
}
/* -Changes for CR 399075 */

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamImagePortFormat(OMX_PTR pt_org)
{
    IN0("");
    OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainImage, OMX_ErrorNotImplemented);

    RETURN_OMX_ERROR_IF_ERROR(pPort->checkIndexParamImagePortFormat(pt));
    OMX_IMAGE_PORTDEFINITIONTYPE *pImageParam = pPort->getParamImagePortDefinition();
    pImageParam->eCompressionFormat = pt->eCompressionFormat;
    pImageParam->eColorFormat = pt->eColorFormat;

    // if it changed the size of the buffers, update the ParamPortDefinition
    if (pPort->getBufferSizeInParam() != pPort->getBufferSize()) {
        OMX_PARAM_PORTDEFINITIONTYPE local_param = *(pPort->getParamPortDefinition());
        pPort->updateSettings(OMX_IndexParamPortDefinition, &local_param);
    }

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamImagePortFormat(OMX_PTR pt_org) const
{
    IN0("");
    OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);
    RETURN_XXX_IF_WRONG(pPort->getDomain()==OMX_PortDomainImage, OMX_ErrorNotImplemented);

    OMX_IMAGE_PORTDEFINITIONTYPE *pImageParam = pPort->getParamImagePortDefinition();
    pt->eCompressionFormat = pImageParam->eCompressionFormat;
    pt->eColorFormat = pImageParam->eColorFormat;
    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamColorPrimary(OMX_PTR pt_org)
{
    IN0("");
    OMX_CONFIG_COLORPRIMARY *pt = (OMX_CONFIG_COLORPRIMARY *)pt_org;
    if (!pComponent->isCoder()) {
        return OMX_ErrorNotImplemented;
    }

    VFM_Port *pPort = (VFM_Port *)pComponent->getPort(pt->nPortIndex);
    RETURN_XXX_IF_WRONG(pPort!=0, OMX_ErrorBadPortIndex);

    // FIXME: remove the cast with an obsolete format
    mColorPrimary = (OMX_COLORPRIMARYTYPE)pt->eColorPrimary;
    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamColorPrimary(OMX_PTR pt_org) const
{
    IN0("");
    OMX_CONFIG_COLORPRIMARY *pt = (OMX_CONFIG_COLORPRIMARY *)pt_org;
    if (!pComponent->isCoder() && !pComponent->isDecoder()) {
        return OMX_ErrorNotImplemented;
    }

    // FIXME: remove the cast with an direct format
    pt->eColorPrimary = (OMX_SYMBIAN_COLORPRIMARYTYPE)mColorPrimary;
    OUT0("");
    return OMX_ErrorNone;
}

// Need to be exported as JPEG uses it directly
EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::setIndexConfigCommonOutputCrop(OMX_PTR pt_org)
{
    IN0("");
    OMX_CONFIG_RECTTYPE *pt = (OMX_CONFIG_RECTTYPE *)pt_org;

    // we can set the Crop on a coder too just to avoid the EventPortSettingChange
    if (!pComponent->isCoder() && !pComponent->isDecoder()) {
        return OMX_ErrorNotImplemented;
    }

    mOutputCrop = *pt;

    OUT0("");
    return OMX_ErrorNone;
}

// Need to be exported as JPEG uses it directly
EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const
{
    IN0("");
    OMX_CONFIG_RECTTYPE *pt = (OMX_CONFIG_RECTTYPE *)pt_org;
    if (!pComponent->isCoder() && !pComponent->isDecoder()) {
        return OMX_ErrorNotImplemented;
    }

    OMX_U32 port_index = pt->nPortIndex;
    *pt = mOutputCrop;
    pt->nPortIndex = port_index;

    OUT0("");
    return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamPixelAspectRatio(OMX_PTR pt_org) const
{
    IN0("");
    OMX_PARAM_PIXELASPECTRATIO *pt = (OMX_PARAM_PIXELASPECTRATIO *)pt_org;
    // Check the index is on the uncompressed port
    if (pComponent->isCoder()) {
        RETURN_XXX_IF_WRONG(pt->nPortIndex==0, OMX_ErrorBadPortIndex);
    } else if (pComponent->isDecoder()) {
        RETURN_XXX_IF_WRONG(pt->nPortIndex==1, OMX_ErrorBadPortIndex);
    } else {
        return OMX_ErrorNotImplemented;
    }

    pt->nHorizontal = mPARWidth;
    pt->nVertical = mPARHeight;
    OUT0("");
    return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexConfigVideoMacroBlockErrorMap(OMX_PTR pt_org)
{
    OMX_CONFIG_MACROBLOCKERRORMAPTYPE *pt = (OMX_CONFIG_MACROBLOCKERRORMAPTYPE *)pt_org;
    if (!pComponent->isDecoder() || pt->nPortIndex!=1 || pComponent->getPort(pt->nPortIndex)->getDomain()!=OMX_PortDomainVideo) {
        // Only supported on decoder, on the output port, for video
        return OMX_ErrorUnsupportedIndex;
    }
    OMX_U32 width, height, size;
    ((VFM_Component *)getComponent())->getFrameSize(1, &width, &height);
    size = mErrorMap.size_error_map(width, height);
    if (size < pt->nErrMapSize) {
        return OMX_ErrorUndefined;
    }

    // Copy the error map, and reset it (including the fast update)
    mErrorMap.lockMutex();
    memcpy(pt->ErrMap, mErrorMap.mErrorMap, size);
    mErrorMap.reset_with_size(size);
    mErrorMap.releaseMutex();

    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamVideoFastUpdate(OMX_PTR pt_org)
{
    OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *pt = (OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *)pt_org;
    if (!pComponent->isDecoder() || pt->nPortIndex!=1 || pComponent->getPort(pt->nPortIndex)->getDomain()!=OMX_PortDomainVideo) {
        // Only supported on decoder, on the output port, for video
        return OMX_ErrorUnsupportedIndex;
    }
    OMX_U32 width, height;
    ((VFM_Component *)getComponent())->getFrameSize(1, &width, &height);

    // Copy the fast update struture, and reset the error map structure
    mErrorMap.lockMutex();
    pt->nFirstGOB = mErrorMap.mFastUpdate.mFirstGOB;
    pt->nFirstMB = mErrorMap.mFastUpdate.mFirstMB;
    pt->nNumMBs = mErrorMap.mFastUpdate.mNumMBs;
    mErrorMap.reset(width, height);
    mErrorMap.releaseMutex();

    return OMX_ErrorNone;
}

#ifdef PACKET_VIDEO_SUPPORT
EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getPVCapability(OMX_PTR pt_org)
{
    return OMX_ErrorNotImplemented;
}
#endif

EXPORT_C OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamVideoFwPerfData(OMX_PTR pt_org)
{
    VFM_CONFIG_PERFDATATYPE *pt = (VFM_CONFIG_PERFDATATYPE *)pt_org;

	if(pComponent->pFwPerfDataPtr != 0)
	{
		for(unsigned int i=0; i< (sizeof(pt->nControlAlgoTicks)/sizeof(OMX_U32)); i++)
		{
			pt->nControlAlgoTicks[i]	= ENDIANESS_CONVERSION(((ts_ddep_perf_param_out*)(pComponent->pFwPerfDataPtr))->control_algo_ticks[i]);
			pt->nEndAlgoTicks[i]	= ENDIANESS_CONVERSION(((ts_ddep_perf_param_out*)(pComponent->pFwPerfDataPtr))->end_algo_ticks[i]);
		}

		pt->nAccumulatedTicks	= ENDIANESS_CONVERSION(((ts_ddep_perf_param_out*)(pComponent->pFwPerfDataPtr))->acc_ticks);
		pt->nFrameCount	= ENDIANESS_CONVERSION(((ts_ddep_perf_param_out*)(pComponent->pFwPerfDataPtr))->count);
	}
	else
	{
		for(unsigned int i=0; i< (sizeof(pt->nControlAlgoTicks)/sizeof(OMX_U32)); i++)
		{
			pt->nControlAlgoTicks[i]	= 0;
			pt->nEndAlgoTicks[i]	= 0;
		}

		pt->nAccumulatedTicks	= 0;
		pt->nFrameCount	= 0;

	}
	return OMX_ErrorNone;
}

EXPORT_C void VFM_ParamAndConfig::setNoCropping()
{
    memset(&mOutputCrop, 0, sizeof(mOutputCrop));
    mOutputCrop.nSize = sizeof(mOutputCrop);
    getOmxIlSpecVersion(&mOutputCrop.nVersion);
}

void VFM_ParamAndConfig::setCropping(const t_common_frameinfo *pFrameInfo)
{
    setNoCropping();
    if (pFrameInfo->frame_cropping_flag!=0) {
        VFM_Port *pPort = (VFM_Port *)pComponent->getPort(1);
        OMX_VIDEO_PORTDEFINITIONTYPE *pVideoParam = pPort->getParamVideoPortDefinition();

        mOutputCrop.nLeft =   pFrameInfo->frame_crop_left;
        mOutputCrop.nTop =    pFrameInfo->frame_crop_top;
        mOutputCrop.nWidth =  pVideoParam->nFrameWidth - pFrameInfo->frame_crop_right  - pFrameInfo->frame_crop_left;
        mOutputCrop.nHeight = pVideoParam->nFrameHeight - pFrameInfo->frame_crop_bottom - pFrameInfo->frame_crop_top;
    }
}

OMX_BOOL VFM_ParamAndConfig::isChangedCropping_And_Set(const t_common_frameinfo *pFrameInfo)
{
    OMX_CONFIG_RECTTYPE oldCropping = mOutputCrop;
    setCropping(pFrameInfo);
    if (oldCropping.nLeft != mOutputCrop.nLeft
        || oldCropping.nTop != mOutputCrop.nTop
        || oldCropping.nWidth != mOutputCrop.nWidth
        || oldCropping.nHeight != mOutputCrop.nHeight) {
            return OMX_TRUE;
    } else {
            return OMX_FALSE;
    }
}

void VFM_ParamAndConfig::setCroppingInFrameinfo(t_common_frameinfo *pFrameInfo)
{
    if (mOutputCrop.nWidth==0) {
        pFrameInfo->frame_cropping_flag=0;
        pFrameInfo->frame_crop_left = 0;
        pFrameInfo->frame_crop_top = 0;
        pFrameInfo->frame_crop_right = 0;
        pFrameInfo->frame_crop_bottom = 0;
    } else {
        VFM_Port *pPort = (VFM_Port *)pComponent->getPort(1);
        OMX_VIDEO_PORTDEFINITIONTYPE *pVideoParam = pPort->getParamVideoPortDefinition();

        pFrameInfo->frame_cropping_flag=1;
        pFrameInfo->frame_crop_left   = mOutputCrop.nLeft;
        pFrameInfo->frame_crop_top    = mOutputCrop.nTop;
        pFrameInfo->frame_crop_right  = pVideoParam->nFrameWidth  - pFrameInfo->frame_crop_left - mOutputCrop.nWidth;
        pFrameInfo->frame_crop_bottom = pVideoParam->nFrameHeight - pFrameInfo->frame_crop_top - mOutputCrop.nHeight;
    }
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamSupportedExtension(OMX_PTR pt_org)
{
    IN0("");
    VFM_PARAM_SUPPORTEDEXTENSION_TYPE *pt = (VFM_PARAM_SUPPORTEDEXTENSION_TYPE *)pt_org;
    mSupportedExtension = pt->nMask;

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamSupportedExtension(OMX_PTR pt_org) const
{
    IN0("");
    VFM_PARAM_SUPPORTEDEXTENSION_TYPE *pt = (VFM_PARAM_SUPPORTEDEXTENSION_TYPE *)pt_org;
    pt->nMask = mSupportedExtension;

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamHeaderDataChanging(OMX_PTR pt_org)
{
    VFM_PARAM_HEADERDATACHANGING_TYPE *pt = (VFM_PARAM_HEADERDATACHANGING_TYPE *)pt_org;
    mHeaderDataChanging = pt->bHeaderDataChanging;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamArmLoadComputation(OMX_PTR pt_org) const
{
    VFM_PARAM_ARMLOADCOMPUTATION *pt = (VFM_PARAM_ARMLOADCOMPUTATION *)pt_org;
    pt->bDisableFWComputation = ((VFM_Component *)getComponent())->mIsARMLoadComputed;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamArmLoadComputation(OMX_PTR pt_org)
{
    VFM_PARAM_ARMLOADCOMPUTATION *pt = (VFM_PARAM_ARMLOADCOMPUTATION *)pt_org;
    ((VFM_Component *)getComponent())->mIsARMLoadComputed = pt->bDisableFWComputation;
    return OMX_ErrorNone;
}

/* +Change start for CR333618 Added for IOMX renderer */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamSharedchunkInPlatformprivate(OMX_PTR pt_org)
{
    VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE *pt = (VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE *)pt_org;
    mSharedchunkInPlatformprivate = pt->bSharedchunkInPlatformprivate;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamSharedchunkInPlatformprivate(OMX_PTR pt_org) const
{
    VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE *pt = (VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE *)pt_org;
    pt->bSharedchunkInPlatformprivate = mSharedchunkInPlatformprivate;
    return OMX_ErrorNone;
}
/* -Change end for CR333618 Added for IOMX renderer */

/* +Change start for CR 399938 */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamStoreMetadataInBuffers(OMX_PTR pt_org)
{
    VFM_PARAM_STOREMETADATAINBUFFERS *pt = (VFM_PARAM_STOREMETADATAINBUFFERS *)pt_org;
    mStoreMetadataInBuffers = pt->bStoreMetadataInBuffers;
    PRINTPG("Call to setIndexParamStoreMetadataInBuffers value is %x \n",mStoreMetadataInBuffers);
    if (mStoreMetadataInBuffers)
    {
         mMetadataEncoders.initializeEntries();
	}
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamStoreMetadataInBuffers(OMX_PTR pt_org) const
{
    VFM_PARAM_STOREMETADATAINBUFFERS *pt = (VFM_PARAM_STOREMETADATAINBUFFERS *)pt_org;
    pt->bStoreMetadataInBuffers = mStoreMetadataInBuffers;
    return OMX_ErrorNone;
}

OMX_U32 VFM_ParamAndConfig::getBufferPhysicalAddress(OMX_U32 mLogicalAddress)
{
	OMX_U32 mPhysicalAddress;
	mPhysicalAddress = mMetadataEncoders.getPhysicalAddrFromLogical(mLogicalAddress);
	return mPhysicalAddress;
}

OMX_U32 VFM_ParamAndConfig::getBufferLogicalAddress(OMX_U32 mPhysicalAddress)
{
	OMX_U32 mLogicalAddress;
	mLogicalAddress = mMetadataEncoders.getLogicalAddrFromPhysical(mPhysicalAddress);
	return mLogicalAddress;
}


OMX_ERRORTYPE VFM_ParamAndConfig::extraProcessingInputBuffers(OMX_DIRTYPE direction, OMX_BUFFERHEADERTYPE **pBuffer)
{
	OMX_ERRORTYPE errorCode = OMX_ErrorNone;
	/* mMetadataEncoders.printTable(); */
		PRINTPG("Inside extraProcessingInputBuffers with HDR pBuffer 0x%x and pBuffer->pBuffer=0x%x \n",(unsigned int)*pBuffer,(unsigned int)(*pBuffer)->pBuffer);

		if ((pComponent->isEncoder()) && (mStoreMetadataInBuffers))
		{
			PRINTPG("Inside extraProcessingInputBuffers VALID to PROCESS \n");
			if (OMX_DirInput == direction)
			{
				PRINTPG("Inside extraProcessingInputBuffers ::InputSide Buffer \n");
				/* It is assumed that pBuffer is Actually ptr to MetaData */
				/* this MetaData is retrieved by calling MMNativeBuffer::getBuffer() */
                PRINTPG("Checking for METADATA handle 0x%x\n",(OMX_U32)META_BUFFER);
                if (mMetadataEncoders.isMetaDataWrapperFound(META_BUFFER))
                {
					PRINTPG("MetaDataWrapperFound in ARRAY for handle 0x%x\n",(OMX_U32)META_BUFFER);
					if (mMetadataEncoders.isEntryValid(1,(OMX_U32)META_BUFFER,OMX_DirInput))
					{
						PRINTPG("Valid codition found, just to make entry code to VALID \n");
						mMetadataEncoders.changeEntry(1,(OMX_U32)META_BUFFER,OMX_TRUE,(void *)((*pBuffer)->pBuffer));
					}
					else
					{
			             PRINTPG("Erroneous Condition <> pBuffer with repeated MetaData called without EBD \n");
			             errorCode = OMX_ErrorBadParameter;
						 DBC_ASSERT(0==1);
					}
				}
				else
				{
					PRINTPG("Intering New Ptr in ARRAY \n");
					MMNativeBuffer *nativeData = new MMNativeBuffer(META_BUFFER);
                    if (nativeData->init() < 0)
                    {
                        PRINTPG("MMNativeBuffer::init() failed");
                    }
					mMetadataEncoders.insertMetadataWrapper((void **)&nativeData,(void **)&((*pBuffer)->pBuffer));
				}

                /* All processing done by now, so we make pBuffer to LogicalAdress so
                   our OMX component functions as before */
                if (OMX_ErrorNone == errorCode)
                {
					OMX_U32 LogicalAddress = mMetadataEncoders.getLogicalAddressfromNativeHandle((OMX_U32)META_BUFFER);
					(*pBuffer)->pBuffer = (OMX_U8 *)LogicalAddress;
					PRINTPG("Logical Address applied on INPUT!!! LogicalAddress 0x%x \n",LogicalAddress);
				}
			}
			else if (OMX_DirOutput == direction)
			{
				/* here the ->pBuffer would contain the logical address and this has
				  to be replaced by actual NativeHandle */
				PRINTPG("Inside extraProcessingInputBuffers ::OuputSide Buffer for logical address : 0x%x\n ",(OMX_U32)(*pBuffer)->pBuffer);
				if (mMetadataEncoders.isEntryValid(0,(OMX_U32)(*pBuffer)->pBuffer,OMX_DirOutput))
				{
                     OMX_U32 VideoMetadata = mMetadataEncoders.getVideoMetadata((OMX_U32)(*pBuffer)->pBuffer);
                     /* invalidating the entry in the table */
					 mMetadataEncoders.changeEntry(0,(OMX_U32)(*pBuffer)->pBuffer,OMX_FALSE,(void *)0);
                     (*pBuffer)->pBuffer = (OMX_U8 *)VideoMetadata;
                     PRINTPG("Valid ENTRY FOUND (video_metedata *):  0x%x replaced and handle 0x%x \n",VideoMetadata,(OMX_U32)META_BUFFER);
				}
				else
				{
					PRINTPG("ENTRY NOT FOUND for Output Side \n");
					errorCode = OMX_ErrorBadParameter;
					DBC_ASSERT(0==1);
				}
			}
		}
		else if (mStoreMetadataInBuffers)
		{
			PRINTPG("Erroneous Condition METADATA USED BESIDES IN ENCODER \n");
			errorCode = OMX_ErrorBadParameter;
		}
		else
		{
			PRINTPG("Normal WORKNIG CASE - NO METADATA \n");
		}
	PRINTPG("Abt to EXIT from extraProcessing ");
		return errorCode;
}


OMX_ERRORTYPE VFM_ParamAndConfig::destroyMetaDataList()
{
	mMetadataEncoders.destroyList();
	return OMX_ErrorNone;
}

void MetadataEncoders::printTable()
{
    for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
		    PRINTPG("Entries MMNative buffer Ptr : 0x%x , MetaData Ptr : 0x%x isValid : %d ptrMMNativeBuffer->getLogicalAddress() 0x%x Count : %d\n",(OMX_U32)ptrMMNativeBuffer,iMetadataWrapper[index].ptrVideoMetadata,iMetadataWrapper[index].isValid,(OMX_U32)ptrMMNativeBuffer->getLogicalAddress(),(index+1));
		}
	}
}


OMX_U32 MetadataEncoders::getLogicalAddressfromNativeHandle(OMX_U32 nativeHandle)
{
	OMX_U32 nLogicalAddress = 0;
	PRINTPG("Inside getLogicalAddressfromNativeHandle ::nativeHandle %x \n",nativeHandle);
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if ((unsigned)ptrMMNativeBuffer->getBuffer() == nativeHandle)
			{
				if (iMetadataWrapper[index].isValid == OMX_TRUE)
				{
					PRINTPG("Inside getLogicalAddressfromNativeHandle  ::EntryValid \n");
					nLogicalAddress = ptrMMNativeBuffer->getLogicalAddress();
                    break;
			    }
				PRINTPG("Inside getLogicalAddressfromNativeHandle  ::nLogicalAddress %x \n",nLogicalAddress);
			}
		}
	}
	return nLogicalAddress;
}


void MetadataEncoders::initializeEntries()
{
	for (OMX_U32 index=0;index <mMaxiMetadataWrapper;index++)
	{
		iMetadataWrapper[index].pMetaDataWrapper = NULL;
		iMetadataWrapper[index].ptrVideoMetadata = NULL;
		iMetadataWrapper[index].isValid = OMX_FALSE;
	}
}

OMX_U32 MetadataEncoders::getPhysicalAddrFromLogical(OMX_U32 logicalAddress)
{
	OMX_U32 nPhysicalAddress = 0;
	PRINTPG("Inside getPhysicalAddrFromLogical ::logicalAddress %x \n",logicalAddress);
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if (ptrMMNativeBuffer->getLogicalAddress() == logicalAddress)
			{
				if (iMetadataWrapper[index].isValid == OMX_TRUE)
				{
					PRINTPG("Inside getPhysicalAddrFromLogical ::EntryValid for MetaDara 0x%x \n",(OMX_U32)ptrMMNativeBuffer);
					nPhysicalAddress = ptrMMNativeBuffer->getPhysicalAddress();
                    break;
			    }
			}
		}
	}
	PRINTPG("Inside getPhysicalAddrFromLogical :: nPhysicalAddress 0x%x ",nPhysicalAddress);
	return nPhysicalAddress;
}

OMX_U32 MetadataEncoders::getLogicalAddrFromPhysical(OMX_U32 nPhysicalAddress)
{
	OMX_U32 nLogicalAddress = 0;
	PRINTPG("Inside getLogicalAddrFromPhysical ::nPhysicalAddress %x \n",nPhysicalAddress);
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if (ptrMMNativeBuffer->getPhysicalAddress() == nPhysicalAddress)
			{
				if (iMetadataWrapper[index].isValid == OMX_TRUE)
				{
					PRINTPG("Inside getPhysicalAddrFromLogical ::EntryValid \n");
					nLogicalAddress = ptrMMNativeBuffer->getLogicalAddress();
                    break;
			    }
				PRINTPG("Inside getPhysicalAddrFromLogical ::nLogicalAddress %x \n",nLogicalAddress);
			}
		}
	}
	return nLogicalAddress;
}


void MetadataEncoders::destroyList()
{
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
		    delete ptrMMNativeBuffer;
		    ptrMMNativeBuffer = NULL;
		}
	}
}


OMX_U32 MetadataEncoders::getVideoMetadata(OMX_U32 logicalAddress)
{
	OMX_U32 nativeVideoMetadata = 0;
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if (ptrMMNativeBuffer->getLogicalAddress() == logicalAddress)
			{
				if (iMetadataWrapper[index].isValid)
				{
					nativeVideoMetadata = (OMX_U32) iMetadataWrapper[index].ptrVideoMetadata;
					PRINTPG("Inside getVideoMetadata ::nativeMetaData 0x%x \n",nativeVideoMetadata);
                    break;
				}
			}
		}
	}
	return nativeVideoMetadata;
}


/* If type is 0 then input ptrData is treated as Logical Address */
/* If type is 1 then input ptrData is treated as ptr to NativeHandle */
OMX_BOOL MetadataEncoders::isEntryValid(OMX_U32 type, OMX_U32 ptrData,OMX_DIRTYPE direction)
{
	PRINTPG("Inside isEntryValid with type %d <> Data 0x%x \n",type,(OMX_U32)ptrData);
	OMX_BOOL entryValid = OMX_FALSE;
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if (type)
			{
				if (ptrMMNativeBuffer->getBuffer() == (OMX_S32)ptrData)
				{
                    if (OMX_DirInput == direction)
                    {
                        PRINTPG("Checking Valid Entry for Input direction for NATVIVE HANDLE \n");
                        if (!iMetadataWrapper[index].isValid)
                        {
					        entryValid = OMX_TRUE;//iMetadataWrapper[index].isValid;
					        PRINTPG("Inside isEntryValid ::ENTRY FOUND FOR INPUT with Native Handle 0x%x and entryValid %x \n",ptrData,(OMX_U32)entryValid);
                        }
                    }
                    else
                    {
                        if (iMetadataWrapper[index].isValid)
                        {
                            entryValid = OMX_TRUE;//iMetadataWrapper[index].isValid;
                            PRINTPG("Inside isEntryValid ::ENTRY FOUND FOR OUTPUT with Native Handle 0x%x and entryValid %x \n",ptrData,(OMX_U32)entryValid);
                        }
                    }
                    break;
				}
			}
			else /* ptrData treated as Logical Address */
			{
				if (ptrMMNativeBuffer->getLogicalAddress() == ptrData)
				{
                    if (OMX_DirInput == direction)
					{
                        PRINTPG("Checking Valid Entry for Input direction for LOGICAL ADDRESS \n");
                        if (!iMetadataWrapper[index].isValid)
                        {
					        entryValid = OMX_TRUE;//iMetadataWrapper[index].isValid;
					        PRINTPG("Inside isEntryValid ::ENTRY FOUND FOR INPUT with Native Handle 0x%x and entryValid %x \n",ptrData,(OMX_U32)entryValid);
                        }
                    }
					else
                    {
                        if (iMetadataWrapper[index].isValid)
                        {
                            entryValid = OMX_TRUE;//iMetadataWrapper[index].isValid;
                            PRINTPG("Inside isEntryValid ::ENTRY FOUND FOR OUTPUT with Native Handle 0x%x and entryValid %x \n",ptrData,(OMX_U32)entryValid);
                        }
                    }
                    break;
				}
			}
		}
	}
	PRINTPG("Inside isEntryValid ::Return value of entryValid %x \n",(OMX_U32)entryValid);
	return entryValid;
}


/* If type is 0 then input ptrData is treated as Logical Address */
/* If type is 1 then input ptrData is treated as ptr to NativeHandle */
OMX_BOOL MetadataEncoders::changeEntry(OMX_U32 type, OMX_U32 ptrData, OMX_BOOL newValue,void *ptrMetaData)
{
	PRINTPG("Inside changeEntry with type %d <> Data 0x%x \n",type,(OMX_U32)ptrData);
	OMX_BOOL entryChanged = OMX_FALSE;
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if (type)
			{
				if (ptrMMNativeBuffer->getBuffer() == (OMX_S32)ptrData)
				{
					iMetadataWrapper[index].isValid = newValue;
					if (OMX_TRUE == newValue)
					{
						iMetadataWrapper[index].ptrVideoMetadata = ptrMetaData;
					}
					entryChanged = OMX_TRUE;
					PRINTPG("Inside changeEntry ::ENTRY FOUND with Native Handle 0x%x and entryChanged %x \n",ptrData,(OMX_U32)entryChanged);
                    break;
				}
			}
			else /* ptrData treated as Logical Address */
			{
				if ((ptrMMNativeBuffer->getLogicalAddress()) == ptrData)
				{
					iMetadataWrapper[index].isValid = newValue;
					entryChanged = OMX_TRUE;
					PRINTPG("Inside changeEntry ::ENTRY FOUND with logical added 0x%x and entryChanged %x \n",ptrData,(OMX_U32)entryChanged);
                    break;
				}
			}
		}
	}
	PRINTPG("Inside changeEntry ::Return value of entryValid %x \n",(OMX_U32)entryChanged);
	return entryChanged;
}

OMX_BOOL MetadataEncoders::isMetaDataWrapperFound(void *ptrNativeHandle)
{
	PRINTPG("Inside isMetadataWrapperFound with addr 0x%x \n",(OMX_U32)ptrNativeHandle);
	OMX_BOOL entryFound = OMX_FALSE;
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		MMNativeBuffer *ptrMMNativeBuffer = (MMNativeBuffer *)(iMetadataWrapper[index].pMetaDataWrapper);
		if (ptrMMNativeBuffer)
		{
			if ((ptrMMNativeBuffer->getBuffer()) == (OMX_S32)ptrNativeHandle)
			{
				entryFound = OMX_TRUE;
				PRINTPG("Inside isMetadataWrapperFound ::ENTRY FOUND with addR 0x%x and entryFound %x \n",(OMX_U32)ptrNativeHandle,(OMX_U32)entryFound);
                break;
			}
		}
	}
	PRINTPG("Inside isMetadataWrapperFound ::Return value of entryFound %x \n",(OMX_U32)entryFound);
	return entryFound;
}

OMX_BOOL MetadataEncoders::insertMetadataWrapper(void **ptr, void ** ptrVideoMetaInfo)
{
	PRINTPG("Inside insertMetadataWrapper \n");
	OMX_BOOL entryInserted = OMX_FALSE;
	for (OMX_U32 index=0;index < mMaxiMetadataWrapper;index++)
	{
		if (iMetadataWrapper[index].pMetaDataWrapper == NULL)
		{
            iMetadataWrapper[index].pMetaDataWrapper = *ptr;
            iMetadataWrapper[index].ptrVideoMetadata = *ptrVideoMetaInfo;
            iMetadataWrapper[index].isValid = OMX_TRUE;
            entryInserted = OMX_TRUE;
            PRINTPG("New inserted fields pMetaDataWrapper 0x%x ptrVideoMetadata 0x%x \n",(unsigned int)iMetadataWrapper[index].pMetaDataWrapper,(unsigned int)iMetadataWrapper[index].ptrVideoMetadata);
            PRINTPG("Inside insertMetadataWrapper ::New Value inserted entryInserted %x \n",(OMX_U32)entryInserted);
            break;
		}
	}
	PRINTPG("Inside insertMetadataWrapper ::Return value of entryInserted %x \n",(OMX_U32)entryInserted);
	return entryInserted;
}

/* -Change end for CR 399938 */

/* +Change start for CR333616 Delay recycling */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamRecyclingDelay(OMX_PTR pt_org)
{
    VFM_PARAM_RECYCLINGDELAYTYPE *pt = (VFM_PARAM_RECYCLINGDELAYTYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nRecyclingDelay<RecyclingDelayType::mMaxRecycling, OMX_ErrorBadPortIndex);
    mRecyclingDelay.set(pt->nRecyclingDelay);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamRecyclingDelay(OMX_PTR pt_org) const
{
    VFM_PARAM_RECYCLINGDELAYTYPE *pt = (VFM_PARAM_RECYCLINGDELAYTYPE *)pt_org;
    pt->nRecyclingDelay = mRecyclingDelay.get();
    return OMX_ErrorNone;
}
/* -Change end for CR333616 Delay recycling */

/* +Change start for CR330198 Revert to DTS from PTS */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamTimestampDisplayOrder(OMX_PTR pt_org)
{
    VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE *pt = (VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE *)pt_org;
    mDisplayOrder.set(pt->bDisplayOrder);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamTimestampDisplayOrder(OMX_PTR pt_org) const
{
    VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE *pt = (VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE *)pt_org;
    pt->bDisplayOrder = mDisplayOrder.isEnabled();
    return OMX_ErrorNone;
}

/* +Change start for CR332521 IOMX UseBuffer */
OMX_ERRORTYPE VFM_ParamAndConfig::setIndexParamiOMX(OMX_PTR pt_org)
{
    VFM_PARAM_IOMXTYPE *pt = (VFM_PARAM_IOMXTYPE *)pt_org;
    miOMX.set(pt->biOMX);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE VFM_ParamAndConfig::getIndexParamiOMX(OMX_PTR pt_org) const
{
    VFM_PARAM_IOMXTYPE *pt = (VFM_PARAM_IOMXTYPE *)pt_org;
    pt->biOMX = miOMX.get();
    return OMX_ErrorNone;
}
/* -Change end for CR332521 IOMX UseBuffer */

DisplayOrderType::DisplayOrderType() :
    mEnable(VFM_SocCapabilityMgt::getDecoderDisplayOrder()), mFirst(0), mLast(0), mNextSaveMustReset(OMX_TRUE)
{
    memset(mTimestamp,0,mMaxTsBuffers*sizeof(OMX_TICKS));
}

void DisplayOrderType::set(OMX_BOOL enable)
{
    mEnable = enable;
    if (!mEnable) {
        mFirst = mLast = 0;
    }
}

void DisplayOrderType::save(OMX_BUFFERHEADERTYPE* pBuffer)
{
//#ifdef __ndk5500_a0__
	OMX_U32 tempSize;
//#endif
    if (mEnable) {
        if (mNextSaveMustReset) {
            mNextSaveMustReset = OMX_FALSE;
            mFirst = mLast = 0;
        }

        /* +Changes for ER 420424 */
        if (pBuffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
            return;     // sps / pps are sent before with timestamp==0
        }
        /* -Changes for ER 420424 */


        if (pBuffer->nFilledLen==0) {
            return;     // zero-filled buffers leads to nothing apart from information (EOS flag)
        }

        mTimestamp[mLast] = pBuffer->nTimeStamp;
//#ifdef __ndk5500_a0__
        tempSize = mLast;
//#endif
        mLast = (mLast + 1) % mMaxTsBuffers;
        DBC_ASSERT(mLast != mFirst);

//#ifdef __ndk5500_a0__
		/* + change for ER 350391 */
        //arranging the list in ascending order
        sort_reorder(mTimestamp, mFirst,mLast,mMaxTsBuffers);
        /* - change for ER 350391 */
//#endif
    }
}

OMX_TICKS DisplayOrderType::get(OMX_BUFFERHEADERTYPE* pBuffer)
{
    if (mEnable && pBuffer->nFilledLen!=0) {
        DBC_ASSERT(mLast != mFirst);
        OMX_TICKS ts = mTimestamp[mFirst];
        mFirst = (mFirst + 1) % mMaxTsBuffers;
        return ts;
    } else {
        return pBuffer->nTimeStamp;
    }
}

//#ifdef __ndk5500_a0__
/* + change for ER 350391 */

//utility API for sorting the ARRAY
static OMX_U32 check(OMX_S32 value,OMX_U32 mMaxTsBuffers)
{
    if(value>=0)
        return(value % mMaxTsBuffers);
    else
        return(value +  mMaxTsBuffers);
}


//utility API for sorting the ARRAY
static OMX_U32 cor(OMX_U32 i,OMX_U32 first,OMX_U32 last ,OMX_U32 mMaxTsBuffers)
{
    if(last<first)
    {
        if(i<last)
          return(i+mMaxTsBuffers);
              return i;
    }
    else
        return i;
}

//utility API for sorting the ARRAY
static void sort_reorder(OMX_TICKS arr[],OMX_U32 first,OMX_U32 last,OMX_U32 maxValue)
{
    if(first == last)
        return;

    else if(check((first+1),maxValue)== last)
        return;

    else
    {
        int i=0;
        OMX_TICKS new_val = arr[check((last-1),maxValue)];
        for(i=check((last-2),maxValue);cor(i,first,last,maxValue)>=first;)
        {
            if(new_val<arr[i])
                arr[check((i+1),maxValue)]=arr[i];
            else
                break;
            i=check((i-1),maxValue);
            if((i==127)&&(first==0))
                break;
        }
        arr[check((i+1),maxValue)]=new_val;
    }
    return;
}

/* - change for ER 350391 */
//#endif


RecyclingDelayType::RecyclingDelayType() : mRecyclingDelay(VFM_SocCapabilityMgt::getDecoderRecyclingDelay())
{
    for (OMX_U32 i=0; i<mRecyclingDelay+1; i++) {
        mBuffer[i] = 0;
    }
}

OMX_BUFFERHEADERTYPE *RecyclingDelayType::recycleBuffer(OMX_BUFFERHEADERTYPE *pBuffer)
{
    if (mRecyclingDelay==0) {
        return pBuffer;
    }
    for (OMX_U32 i=mRecyclingDelay; i>0; i--) {
        mBuffer[i] = mBuffer[i-1];
    }
    mBuffer[0] = pBuffer;
    return mBuffer[mRecyclingDelay];
}

OMX_BUFFERHEADERTYPE *RecyclingDelayType::flushOneBuffer()
{
    return recycleBuffer(0);
}
/* -Change end for CR330198 Revert to DTS from PTS */

/* +Change start for CR332521 IOMX UseBuffer */
iOMXType::iOMXType() : mEnable(OMX_TRUE)
{
    for (OMX_U32 nPort=0; nPort<2; nPort++) {
        for (OMX_U32 i=0; i<mMaxiOMXBuffers; i++) {
            mBufferiOMX[nPort][i] = mBufferApplication[nPort][i] = 0;
            mSharedChunk[nPort][i] = 0;;
            mResetPortPrivate[nPort][i] = OMX_FALSE;
        }
    }
    mCached[0] = mCached[1] = OMX_FALSE;
    mIsApplied[0] = mIsApplied[1] = OMX_FALSE;
}

void iOMXType::set(OMX_BOOL enable)
{
    // Check all the buffers are empty now
    for (OMX_U32 nPort=0; nPort<2; nPort++) {
        for (OMX_U32 i=0; i<mMaxiOMXBuffers; i++) {
            DBC_ASSERT(mBufferiOMX[nPort][i] == 0);
            DBC_ASSERT(mBufferApplication[nPort][i] == 0);
            DBC_ASSERT(mSharedChunk[nPort][i] == 0);
            DBC_ASSERT(mResetPortPrivate[nPort][i] == OMX_FALSE);
        }
    }

    mEnable = enable;
}

OMX_ERRORTYPE iOMXType::allocate(VFM_Component *pComponent, MMHwBuffer *sharedChunk, OMX_U32 nPortIndex,
    OMX_U32 nBufferIndex, OMX_U32 nSize, OMX_BUFFERHEADERTYPE *pBuffer)
{
    if (!mEnable || sharedChunk) {
        mIsApplied[nPortIndex] = OMX_FALSE;
        return OMX_ErrorNone;
    } else {
        mIsApplied[nPortIndex] = OMX_TRUE;
    }

    DBC_ASSERT(mBufferiOMX[nPortIndex][nBufferIndex] == 0);
    DBC_ASSERT(mBufferApplication[nPortIndex][nBufferIndex] == 0);
    DBC_ASSERT(nBufferIndex<mMaxiOMXBuffers);

    MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
    poolAttrs.iBuffers = 1;                                 // Number of buffers in the pool
    poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
    poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
    poolAttrs.iSize = nSize;                                // Size (in byte) of a buffer
    poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool
                                                            //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
    poolAttrs.iCacheAttr = (mCached[nPortIndex] ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));

    OMX_ERRORTYPE error = MMHwBuffer::Create(poolAttrs, pComponent->getOMXHandle(), mSharedChunk[nPortIndex][nBufferIndex]);
    if (error != OMX_ErrorNone) {
        return OMX_ErrorInsufficientResources;
    }

    MMHwBuffer::TBufferInfo bufferInfo;
    error = mSharedChunk[nPortIndex][nBufferIndex]->BufferInfo(0, bufferInfo);
    if (error != OMX_ErrorNone) {
        return OMX_ErrorInsufficientResources;
    }
    mBufferApplication[nPortIndex][nBufferIndex] = pBuffer->pBuffer;
    mBufferiOMX[nPortIndex][nBufferIndex] = (OMX_U8 *)bufferInfo.iLogAddr;

    return OMX_ErrorNone;
}


void iOMXType::free(OMX_U32 nPortIndex, OMX_U32 nBufferIndex)
{
    if (!mEnable || !mSharedChunk[nPortIndex][nBufferIndex]) {
        return;
    }

    OMX_ERRORTYPE error = MMHwBuffer::Destroy(mSharedChunk[nPortIndex][nBufferIndex]);
    DBC_ASSERT(error == OMX_ErrorNone);
    mBufferiOMX[nPortIndex][nBufferIndex] = 0;
    mBufferApplication[nPortIndex][nBufferIndex] = 0;
    mSharedChunk[nPortIndex][nBufferIndex] = 0;
}


void iOMXType::getiOMXBuffer(OMX_U32 nPort, OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE error;
    DBC_ASSERT(nPort < 2);

    if (!mIsApplied[nPort]) {
        return;
    }

    OMX_U32 i;
    for (i=0; i<mMaxiOMXBuffers; i++) {
        if (mBufferApplication[nPort][i] == pBuffer->pBuffer) {
            pBuffer->pBuffer = mBufferiOMX[nPort][i];
            DBC_ASSERT(mResetPortPrivate[nPort][i] == OMX_FALSE);
            if (nPort==0) {
                // copy the input port becuase it corresponds to an EmptyThisBuffer
                memcpy(((OMX_U8 *)pBuffer->pBuffer)+pBuffer->nOffset, ((OMX_U8 *)mBufferApplication[nPort][i])+pBuffer->nOffset, pBuffer->nFilledLen);
                if (pBuffer->pInputPortPrivate==0) {
                    pBuffer->pInputPortPrivate = mSharedChunk[nPort][i];
                    mResetPortPrivate[nPort][i] = OMX_TRUE;
                }
            } else {
                if (pBuffer->pOutputPortPrivate==0) {
                    pBuffer->pOutputPortPrivate = mSharedChunk[nPort][i];
                    mResetPortPrivate[nPort][i] = OMX_TRUE;
                }
            }
            return ;
        }
    }

    //NMF_LOG("nPort = %u\n", nPort);
    DBC_ASSERT(0==1);
}

void iOMXType::getApplicationBuffer(OMX_U32 nPort, OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE error;
    DBC_ASSERT(nPort < 2);
    if (!mIsApplied[nPort]) {
        return;
    }

    OMX_U32 i;
    for (i=0; i<mMaxiOMXBuffers; i++) {
        if (mBufferiOMX[nPort][i] == pBuffer->pBuffer) {
            pBuffer->pBuffer = mBufferApplication[nPort][i];
            if (nPort==0) {
                if (mResetPortPrivate[nPort][i]) {
                    pBuffer->pInputPortPrivate = 0;
                }
            } else {
                // copy the output port as it corresponds to a FillBufferDone
                memcpy(((OMX_U8 *)pBuffer->pBuffer)+pBuffer->nOffset, ((OMX_U8 *)mBufferiOMX[nPort][i])+pBuffer->nOffset, pBuffer->nFilledLen);
                if (mResetPortPrivate[nPort][i]) {
                    pBuffer->pOutputPortPrivate = 0;
                }
            }
            mResetPortPrivate[nPort][i] = OMX_FALSE;
            return;
        }
    }
    DBC_ASSERT(0==1);
}


void iOMXType::set_cache_properties(OMX_BOOL bInput, OMX_BOOL bOutput)
{
    mCached[0] = bInput;
    mCached[1] = bOutput;
}

EXPORT_C void iOMXType::getBuffer(MMHwBuffer **sharedChunk, OMX_U8 **pDataBuffer, OMX_U32 nPortIndex, OMX_U32 nBufferIndex)
{
    if (!mEnable || mBufferApplication[nPortIndex][nBufferIndex]==0) {
        return;
    }

    *pDataBuffer = mBufferiOMX[nPortIndex][nBufferIndex];
    *sharedChunk = mSharedChunk[nPortIndex][nBufferIndex];
}
/* -Change end for CR332521 IOMX UseBuffer */

