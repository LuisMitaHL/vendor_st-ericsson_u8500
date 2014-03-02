/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//System includes
#include <dlfcn.h>

//Multimedia includes
#include <IFM_Types.h>
#include <OMX_CoreExt.h>

#define DBGT_LAYER 2
#define DBGT_PREFIX "Utils "

//Internal includes
#include "STECamOmxUtils.h"
#include "STECamUtils.h"

//Defines
#ifndef STEOMXINT_SO_FILE
#define STEOMXINT_SO_FILE "/system/lib/libste_omxil-interface.so"
#endif

namespace android {

struct OmxComponentHandleName {
    OmxComponentHandleName() {
        hComponent = NULL;
        memset(mName, '\0', 256);
    }
    OMX_HANDLETYPE hComponent;
    char mName[256];
};
static OmxComponentHandleName mOmxComponentHandleName[OmxUtils::EOMXComponentsMax];

#undef _CNAME_
#define _CNAME_ OmxUtils

OmxUtils::OmxUtils() : mDlHandle(NULL), mInterface(NULL)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OmxUtils::~OmxUtils()
{
    DBGT_PROLOG("");

    if(NULL != mDlHandle)
        dlclose(mDlHandle);

    DBGT_EPILOG("");
}

OMX_ERRORTYPE OmxUtils::init()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //open Dl
    mDlHandle = dlopen(STEOMXINT_SO_FILE, RTLD_NOW);
    if( NULL == mDlHandle ) {
        DBGT_CRITICAL("dlopen(%s) failed with error: %s\n", STEOMXINT_SO_FILE, dlerror());
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    typedef OMXCoreInterface* (*pGetCoreInterface)();

    pGetCoreInterface temp;

    temp = (pGetCoreInterface)dlsym(mDlHandle, "GetOMXCoreInterface");

    mInterface  = (*temp)();
    if (mInterface == NULL) {
        DBGT_CRITICAL("dlsym(GetOMXCoreInterface) failed: %s\n", dlerror());

        dlclose(mDlHandle);
        mDlHandle = NULL;

        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    DBGT_EPILOG("");
    return err;
}
#define STRINGIFY_PARAM(aParam, retval) {case aParam: retval = (#aParam); break;}

/*static*/ const char* OmxUtils::name(OMX_ERRORTYPE aParam)
{
    DBGT_PROLOG("");

    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_ErrorNone, retval)
        STRINGIFY_PARAM(OMX_ErrorInsufficientResources, retval)
        STRINGIFY_PARAM(OMX_ErrorUndefined, retval)
        STRINGIFY_PARAM(OMX_ErrorInvalidComponentName, retval)
        STRINGIFY_PARAM(OMX_ErrorComponentNotFound, retval)
        STRINGIFY_PARAM(OMX_ErrorInvalidComponent, retval)
        STRINGIFY_PARAM(OMX_ErrorBadParameter, retval)
        STRINGIFY_PARAM(OMX_ErrorNotImplemented, retval)
        STRINGIFY_PARAM(OMX_ErrorUnderflow, retval)
        STRINGIFY_PARAM(OMX_ErrorOverflow, retval)
        STRINGIFY_PARAM(OMX_ErrorHardware, retval)
        STRINGIFY_PARAM(OMX_ErrorInvalidState, retval)
        STRINGIFY_PARAM(OMX_ErrorStreamCorrupt, retval)
        STRINGIFY_PARAM(OMX_ErrorPortsNotCompatible, retval)
        STRINGIFY_PARAM(OMX_ErrorResourcesLost, retval)
        STRINGIFY_PARAM(OMX_ErrorNoMore, retval)
        STRINGIFY_PARAM(OMX_ErrorVersionMismatch, retval)
        STRINGIFY_PARAM(OMX_ErrorNotReady, retval)
        STRINGIFY_PARAM(OMX_ErrorTimeout, retval)
        STRINGIFY_PARAM(OMX_ErrorSameState, retval)
        STRINGIFY_PARAM(OMX_ErrorResourcesPreempted, retval)
        STRINGIFY_PARAM(OMX_ErrorPortUnresponsiveDuringAllocation, retval)
        STRINGIFY_PARAM(OMX_ErrorPortUnresponsiveDuringDeallocation, retval)
        STRINGIFY_PARAM(OMX_ErrorPortUnresponsiveDuringStop, retval)
        STRINGIFY_PARAM(OMX_ErrorIncorrectStateTransition, retval)
        STRINGIFY_PARAM(OMX_ErrorIncorrectStateOperation, retval)
        STRINGIFY_PARAM(OMX_ErrorUnsupportedSetting, retval)
        STRINGIFY_PARAM(OMX_ErrorUnsupportedIndex, retval)
        STRINGIFY_PARAM(OMX_ErrorBadPortIndex, retval)
        STRINGIFY_PARAM(OMX_ErrorPortUnpopulated, retval)
        STRINGIFY_PARAM(OMX_ErrorComponentSuspended, retval)
        STRINGIFY_PARAM(OMX_ErrorDynamicResourcesUnavailable, retval)
        STRINGIFY_PARAM(OMX_ErrorMbErrorsInFrame, retval)
        STRINGIFY_PARAM(OMX_ErrorFormatNotDetected, retval)
        STRINGIFY_PARAM(OMX_ErrorContentPipeOpenFailed, retval)
        STRINGIFY_PARAM(OMX_ErrorContentPipeCreationFailed, retval)
        STRINGIFY_PARAM(OMX_ErrorSeperateTablesUsed, retval)
        STRINGIFY_PARAM(OMX_ErrorTunnelingUnsupported, retval)
        default:
            retval =  "unknown error";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_COMMANDTYPE aParam)
{
    DBGT_PROLOG("");

    const char* retval = NULL;
    switch (aParam) {
        STRINGIFY_PARAM(OMX_CommandStateSet, retval)
        STRINGIFY_PARAM(OMX_CommandFlush, retval)
        STRINGIFY_PARAM(OMX_CommandPortDisable, retval)
        STRINGIFY_PARAM(OMX_CommandPortEnable, retval)
        STRINGIFY_PARAM(OMX_CommandMarkBuffer, retval)
        default:
            retval =  "unknown command";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_STATETYPE aParam)
{
    DBGT_PROLOG("");

    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_StateInvalid, retval)
        STRINGIFY_PARAM(OMX_StateLoaded, retval)
        STRINGIFY_PARAM(OMX_StateIdle, retval)
        STRINGIFY_PARAM(OMX_StateExecuting, retval)
        STRINGIFY_PARAM(OMX_StatePause, retval)
        STRINGIFY_PARAM(OMX_StateWaitForResources, retval)
        STRINGIFY_PARAM(OMX_StateTestForResources, retval)
        default:
            retval =  "unknown state";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_EVENTTYPE aParam)
{
    DBGT_PROLOG("");

    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_EventCmdComplete, retval)
        STRINGIFY_PARAM(OMX_EventError, retval)
        STRINGIFY_PARAM(OMX_EventMark, retval)
        STRINGIFY_PARAM(OMX_EventPortSettingsChanged, retval)
        STRINGIFY_PARAM(OMX_EventBufferFlag, retval)
        STRINGIFY_PARAM(OMX_EventResourcesAcquired, retval)
        STRINGIFY_PARAM(OMX_EventComponentResumed, retval)
        STRINGIFY_PARAM(OMX_EventDynamicResourcesAvailable, retval)
        STRINGIFY_PARAM(OMX_EventPortFormatDetected, retval)
        STRINGIFY_PARAM(OMX_EventIndexSettingChanged, retval)
        default:
            retval = "unknown event ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_BUFFERSUPPLIERTYPE aParam)
{
    DBGT_PROLOG("");

    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_BufferSupplyUnspecified, retval)
        STRINGIFY_PARAM(OMX_BufferSupplyInput, retval)
        STRINGIFY_PARAM(OMX_BufferSupplyOutput, retval)
        default:
            retval = "unknown buffer supplier type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_DIRTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_DirInput, retval)
        STRINGIFY_PARAM(OMX_DirOutput, retval)
        default:
            retval = "unknown dir type";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_PORTDOMAINTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_PortDomainAudio, retval)
        STRINGIFY_PARAM(OMX_PortDomainVideo, retval)
        STRINGIFY_PARAM(OMX_PortDomainImage, retval)
        STRINGIFY_PARAM(OMX_PortDomainOther, retval)
        default:
            retval = "unknown port domian type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_VIDEO_CODINGTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_VIDEO_CodingUnused, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingAutoDetect, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingMPEG2, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingH263, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingMPEG4, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingWMV, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingRV, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingAVC, retval)
        STRINGIFY_PARAM(OMX_VIDEO_CodingMJPEG, retval)
        default:
            retval = "unknown video coding type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_IMAGE_CODINGTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_IMAGE_CodingUnused, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingAutoDetect, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingJPEG, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingJPEG2K, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingEXIF, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingTIFF, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingGIF, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingPNG, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingLZW, retval)
        STRINGIFY_PARAM(OMX_IMAGE_CodingBMP, retval)
        default:
            retval = "unknown image coding type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_COLOR_FORMATTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_COLOR_FormatUnused, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatMonochrome, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format8bitRGB332, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format12bitRGB444, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format16bitARGB4444, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format16bitARGB1555, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format16bitRGB565, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format16bitBGR565, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format18bitRGB666, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format18bitARGB1665, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format19bitARGB1666, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format24bitRGB888, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format24bitBGR888, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format24bitARGB1887, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format25bitARGB1888, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format32bitBGRA8888, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format32bitARGB8888, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV411Planar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV411PackedPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV420Planar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV420PackedPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV420SemiPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV422Planar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV422SemiPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYCbYCr, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYCrYCb, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatCbYCrY, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatCrYCbY, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV444Interleaved, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatRawBayer8bit, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_COLOR_FormatRawBayer12bit, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatRawBayer10bit, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatRawBayer8bitcompressed, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL2, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL4, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL8, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL16, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL24, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatL32, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV420PackedSemiPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_FormatYUV422PackedSemiPlanar, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format18BitBGR666, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format24BitARGB6666, retval)
        STRINGIFY_PARAM(OMX_COLOR_Format24BitABGR6666, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, retval)
        STRINGIFY_PARAM(OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar, retval)
        default:
            retval = "unknown video coding type";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_FOCUSSTATUSTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_FocusStatusOff, retval)
        STRINGIFY_PARAM(OMX_FocusStatusRequest, retval)
        STRINGIFY_PARAM(OMX_FocusStatusReached, retval)
        STRINGIFY_PARAM(OMX_FocusStatusUnableToReach, retval)
        STRINGIFY_PARAM(OMX_FocusStatusLost, retval)
        default:
            retval = "unknown focus status type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_SYMBIAN_FOCUSRANGETYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch (aParam) {
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeAuto, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeHyperfocal, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeNormal, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeSuperMacro, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeMacro, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRangeInfinity, retval)
        default:
            retval =  "unknown focus range type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_SYMBIAN_LOCKTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_SYMBIAN_LockOff, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_LockImmediate, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_LockAtCapture, retval)
        STRINGIFY_PARAM(OMX_STE_LockLensBestPosition, retval)
        default:
            retval = "unknown focus range type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_SYMBIAN_FOCUSREGIONCONTROL aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRegionAuto, retval)
        STRINGIFY_PARAM(OMX_STE_FocusRegionCenter, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRegionManual, retval)
        STRINGIFY_PARAM(OMX_STE_FocusRegionTouchPriority, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRegionObjectPriority, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FocusRegionFacePriority, retval)
        default:
            retval = "unknown focus region type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_IMAGEFILTERTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_ImageFilterNone, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_ImageFilterGrayScale, retval)
        STRINGIFY_PARAM(OMX_ImageFilterNegative, retval)
        STRINGIFY_PARAM(OMX_ImageFilterSolarize, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_ImageFilterSepia, retval)
        STRINGIFY_PARAM(OMX_STE_ImageFilterPosterize, retval)
        STRINGIFY_PARAM(OMX_STE_ImageFilterGrayscaleNegative, retval)
        STRINGIFY_PARAM(OMX_STE_ImageFilterFilm, retval)
        STRINGIFY_PARAM(OMX_STE_ImageFilterWatercolor, retval)
        default:
            retval = "unknown colour effect type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_WHITEBALCONTROLTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_WhiteBalControlAuto, retval)
        STRINGIFY_PARAM(OMX_WhiteBalControlIncandescent, retval)
        STRINGIFY_PARAM(OMX_WhiteBalControlFluorescent, retval)
        STRINGIFY_PARAM(OMX_WhiteBalControlSunLight, retval)
        STRINGIFY_PARAM(OMX_WhiteBalControlCloudy, retval)
        default:
            retval = "unknown whitebalance type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_IMAGE_FLASHCONTROLTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_IMAGE_FlashControlOff, retval)
        STRINGIFY_PARAM(OMX_IMAGE_FlashControlOn, retval)
        STRINGIFY_PARAM(OMX_IMAGE_FlashControlRedEyeReduction, retval)
        STRINGIFY_PARAM(OMX_IMAGE_FlashControlTorch, retval)
        STRINGIFY_PARAM(OMX_IMAGE_FlashControlAuto, retval)
        default:
            retval = "unknown flash type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_SYMBIAN_FLICKERREMOVALTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_SYMBIAN_FlickerRemovalOff, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FlickerRemovalAuto, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FlickerRemoval50, retval)
        STRINGIFY_PARAM(OMX_SYMBIAN_FlickerRemoval60, retval)
        default:
            retval = "unknown flicker type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_METERINGTYPE aParam)
{
    DBGT_PROLOG("");
    const char* retval = NULL;

    switch ((uint32_t)aParam) {
        STRINGIFY_PARAM(OMX_MeteringModeAverage, retval)
        STRINGIFY_PARAM(OMX_MeteringModeSpot, retval)
        STRINGIFY_PARAM(OMX_MeteringModeMatrix, retval)
        STRINGIFY_PARAM(OMX_STE_MeteringModeCenterWeighted, retval)
        default:
            retval = "unknown metering type ";
    }
    DBGT_EPILOG("aParam = %s", retval);
    return retval;
}

/*static*/ const char* OmxUtils::name(OMX_HANDLETYPE hComponent)
{
    for(int index = 0; index < EOMXComponentsMax; index++) {
        if(mOmxComponentHandleName[index].hComponent == hComponent) {
            return mOmxComponentHandleName[index].mName;
        }
    }
    return NULL;
}

/*static*/ void OmxUtils::initialize(OMX_PARAM_PORTDEFINITIONTYPE& aParam, OMX_PORTDOMAINTYPE aDomain, OMX_U32 aIndex)
{

    DBGT_PROLOG("Domain: %s Index: %lu", OmxUtils::name(aDomain), aIndex);

    //init size and version
    StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(aParam);

    aParam.eDomain = aDomain;
    aParam.nPortIndex = aIndex;

    //force MIME type to be NULL
    switch(aParam.eDomain)
        {
        case OMX_PortDomainAudio:
            aParam.format.audio.cMIMEType = NULL;
            break;
        case OMX_PortDomainVideo:
            aParam.format.video.cMIMEType = NULL;
            break;
        case OMX_PortDomainImage:
            aParam.format.image.cMIMEType = NULL;
            break;
        default:
        break;
        }

    DBGT_EPILOG("");
}

/*static*/ void OmxUtils::dump(OMX_PARAM_PORTDEFINITIONTYPE& aParam)
{
    DBGT_PTRACE("Size: %lu PortIndex: %lu", aParam.nSize, aParam.nPortIndex);
    DBGT_PTRACE("Version Major: %u Minor: %u Revision: %u Step: %u",
                        aParam.nVersion.s.nVersionMajor,
                        aParam.nVersion.s.nVersionMinor,
                        aParam.nVersion.s.nRevision,
                        aParam.nVersion.s.nStep);
    DBGT_PTRACE("Buffer CountActual: %lu CountMin: %lu Size: %lu Alignment: %lu Contiguous: %d",
                        aParam.nBufferCountActual,
                        aParam.nBufferCountMin,
                        aParam.nBufferSize,
                        aParam.nBufferAlignment,
                        aParam.bBuffersContiguous);
    DBGT_PTRACE("Dir: %s Enabled: %d Populated: %d",
                        OmxUtils::name(aParam.eDir),
                        aParam.bEnabled,
                        aParam.bPopulated);

    DBGT_PTRACE("Domain: %s", OmxUtils::name(aParam.eDomain));

    switch(aParam.eDomain)
        {
        case OMX_PortDomainAudio:
            OmxUtils::dump(aParam.format.audio);
            break;
        case OMX_PortDomainVideo:
            OmxUtils::dump(aParam.format.video);
            break;
        case OMX_PortDomainImage:
            OmxUtils::dump(aParam.format.image);
            break;
        case OMX_PortDomainOther:
            OmxUtils::dump(aParam.format.other);
            break;
        default:
        break;
        }
}

/*static*/ void OmxUtils::dump(OMX_AUDIO_PORTDEFINITIONTYPE& aParam)
{
    DBGT_PTRACE("MIMEType: %s FlagErrorConcealment: %d Encoding: %d",
                aParam.cMIMEType,
                aParam.bFlagErrorConcealment,
                aParam.eEncoding);
}

/*static*/ void OmxUtils::dump(OMX_VIDEO_PORTDEFINITIONTYPE& aParam)
{
#if 0 //@TODO : Remove once bug fixed
    MSG1("MIMEType: %s",
                aParam.cMIMEType);
#endif
    DBGT_PTRACE("Frame Width: %lu Height: %lu Stride: %ld SliceHeight: %lu",
                aParam.nFrameWidth,
                aParam.nFrameHeight,
                aParam.nStride,
                aParam.nSliceHeight);
    DBGT_PTRACE("Bitrate: %lu Framerate: %lu FlagErrorConcealment: %d",
                aParam.nBitrate,
                aParam.xFramerate,
                aParam.bFlagErrorConcealment);
    DBGT_PTRACE("CompressionFormat: %s ColorFormat: %s",
                OmxUtils::name(aParam.eCompressionFormat),
                OmxUtils::name(aParam.eColorFormat));

}

/*static*/ void OmxUtils::dump(OMX_IMAGE_PORTDEFINITIONTYPE& aParam)
{
    DBGT_PTRACE("MIMEType: %s",
                aParam.cMIMEType);
    DBGT_PTRACE("Frame Width: %lu Height: %lu Stride: %ld SliceHeight: %lu",
                aParam.nFrameWidth,
                aParam.nFrameHeight,
                aParam.nStride,
                aParam.nSliceHeight);
    DBGT_PTRACE("FlagErrorConcealment: %d",
                aParam.bFlagErrorConcealment);
    DBGT_PTRACE("CompressionFormat: %s ColorFormat: %s",
                OmxUtils::name(aParam.eCompressionFormat),
                OmxUtils::name(aParam.eColorFormat));
}

/*static*/ void OmxUtils::dump(OMX_OTHER_PORTDEFINITIONTYPE& aParam)
{
    DBGT_PTRACE("Format: %d", aParam.eFormat);
}

/*static*/ int OmxUtils::bpp(OMX_COLOR_FORMATTYPE aOmxColorFmt)
{
    DBGT_PROLOG("");
    int bpp = 0;

    switch((uint32_t)aOmxColorFmt) {
        case OMX_COLOR_FormatCbYCrY:
        case OMX_COLOR_Format16bitRGB565:
            bpp = 16;
            break;
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            bpp = 12;
            break;
        default:
            bpp = 0;
    }

    DBGT_EPILOG("");
    return bpp;
}

/*static*/ status_t OmxUtils::convertOmxErrorToStatus(OMX_ERRORTYPE aOmxErr)
{
    DBGT_PROLOG("OmxError: %s", name(aOmxErr));

    status_t status = NO_ERROR;

    switch(aOmxErr) {
        case OMX_ErrorNone:
            status = NO_ERROR;
            break;
        case OMX_ErrorInsufficientResources:
            status = NO_MEMORY;
            break;
            /* Fall through */
        case OMX_ErrorInvalidComponentName:
        case OMX_ErrorInvalidComponent:
        case OMX_ErrorComponentNotFound:
            status = NAME_NOT_FOUND;
            break;
            /* Fall through */
        case OMX_ErrorBadParameter:
        case OMX_ErrorUnsupportedSetting:
            status = BAD_VALUE;
            break;
            /* Fall through */
        case OMX_ErrorIncorrectStateTransition:
        case OMX_ErrorIncorrectStateOperation:
        case OMX_ErrorSameState:
        case OMX_ErrorNotImplemented:
            status = INVALID_OPERATION;
            break;
        case OMX_ErrorNotReady:
            status = NO_INIT;
            break;
        case OMX_ErrorTimeout:
            status = TIMED_OUT;
            break;
            /* Fall through */
        case OMX_ErrorOverflow:
        case OMX_ErrorBadPortIndex:
        case OMX_ErrorUnsupportedIndex:
            status = BAD_INDEX;
            break;
        case OMX_ErrorUnderflow:
            status = NOT_ENOUGH_DATA;
            break;
            /* Fall through */
        case OMX_ErrorUndefined:
        default:
            status = UNKNOWN_ERROR;
            break;
    }
    DBGT_EPILOG("");
    return status;
}

/*static*/ void OmxUtils::setOMXComponentName(OMX_HANDLETYPE hComponent, char* aName, OMXComponents aOMXComponent)
{
    DBGT_PROLOG("hComponent: %p, aName: %s, aOMXComponent: %d",hComponent, aName, aOMXComponent);

    mOmxComponentHandleName[aOMXComponent].hComponent = hComponent;
    snprintf(mOmxComponentHandleName[aOMXComponent].mName,255,"%s(%#x)",aName,(unsigned int)hComponent);

    DBGT_EPILOG("");
    return;
}

OMX_ERRORTYPE OmxUtils::flushPort(OMX_HANDLETYPE hComponent,
                            OMX_U32 port, void *pContext, sem_t* semaphore)
{
    DBGT_PROLOG("hComponent: %s, port: %u",name(hComponent),(unsigned int)port);
    OMX_ERRORTYPE err = OMX_ErrorNone;

    OMX_BOOL enabled = OMX_TRUE;

    if(port != OMX_ALL) {
        OMX_PARAM_PORTDEFINITIONTYPE dummyport;
        OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);
        dummyport.nPortIndex = port;

        err = OMX_GetParameter(hComponent, OMX_IndexParamPortDefinition, &dummyport);
        DBGT_PTRACE("Port current status: %d",dummyport.bEnabled);
        enabled = dummyport.bEnabled;
    }
    DBGT_PTRACE("enabled: %d",enabled);

    if (enabled == OMX_TRUE) {
        err = OMX_SendCommand(hComponent, OMX_CommandFlush, port , NULL);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("Flush failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        if (semaphore) {
            DBGT_PTRACE("wait for flush");
            camera_sem_wait(semaphore, SEM_WAIT_TIMEOUT);
        }
    } else {
        err = OMX_ErrorSameState;
    }
    DBGT_PTRACE("err: %#x", (unsigned int)err);
    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE OmxUtils::setPortState(OMX_HANDLETYPE hComponent,
                    OMX_COMMANDTYPE cmd,OMX_U32 port, void *pContext,
                    sem_t* semaphore)
{
    DBGT_PROLOG("hComponent: %s, cmd: %s, port: %x",name(hComponent),name(cmd),(unsigned int)port);
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if(port == OMX_ALL) {
        err = OMX_SendCommand(hComponent, cmd, port, NULL);
    } else {
        OMX_BOOL enable = OMX_TRUE;
        if(cmd == OMX_CommandPortDisable) {
            enable = OMX_FALSE;
        }
        DBGT_PTRACE("CMD Sent for port: %d",enable);

        OMX_PARAM_PORTDEFINITIONTYPE dummyport;
        OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);
        dummyport.nPortIndex = port;

        err = OMX_GetParameter(hComponent, OMX_IndexParamPortDefinition, &dummyport);
        DBGT_PTRACE("Port current status: %d",dummyport.bEnabled);
        /* Do the state change in case required else return samestate*/
        if (dummyport.bEnabled != enable) {
            err = OMX_SendCommand(hComponent, cmd, port, NULL);
            if(OMX_ErrorNone != err) {
                DBGT_CRITICAL("Port enable/disable hComponent failed err = %d", err);
                DBGT_EPILOG("");
                return err;
            }

            if (semaphore) {
                camera_sem_wait(semaphore, SEM_WAIT_TIMEOUT);
            }
        } else {
            err = OMX_ErrorSameState;
        }
    }
    DBGT_PTRACE("err: %#x", (unsigned int)err);
    DBGT_EPILOG("");
    return err;
}
#undef STRINGIFY_PARAM

}
