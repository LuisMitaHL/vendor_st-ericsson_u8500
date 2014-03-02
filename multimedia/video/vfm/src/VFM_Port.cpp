/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ VFM

#include "ENS_Component.h"
#include "VFM_Component.h"
#include "VFM_Port.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_PortTraces.h"
#endif

// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

inline void VFM_Port::vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        OstTraceInt3(TRACE_ERROR, "VFM: VFM_Port: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

EXPORT_C VFM_Port::VFM_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
            : ENS_Port(commonPortData, enscomp)
{
}

EXPORT_C
OMX_U32 VFM_Port::getSizeFrame(OMX_COLOR_FORMATTYPE eColor, OMX_U32 width, OMX_U32 height)
{
    OMX_U32 height_MB = (height + 0xF)&(~0xF);
	OMX_U32 width_MB = (width + 0xF)&(~0xF);

    switch ((int)eColor) {
    case OMX_COLOR_FormatMonochrome:                  return width_MB*height_MB;
    case OMX_COLOR_Format8bitRGB332:                  return width_MB*height_MB;
    case OMX_COLOR_Format12bitRGB444:                 return (width_MB*height_MB*3)/2;
    case OMX_COLOR_Format16bitARGB4444:               return width_MB*height_MB*2;
    case OMX_COLOR_Format16bitARGB1555:               return width_MB*height_MB*2;
    case OMX_COLOR_Format16bitRGB565:                 return width_MB*height_MB*2;
    case OMX_COLOR_Format16bitBGR565:                 return width_MB*height_MB*2;
    case OMX_COLOR_Format18bitRGB666:                 return (width_MB*height_MB*9)/4;
    case OMX_COLOR_Format18bitARGB1665:               return (width_MB*height_MB*9)/4;
    case OMX_COLOR_Format19bitARGB1666:               return (width_MB*height_MB*19)/8;
    case OMX_COLOR_Format24bitRGB888:                 return width_MB*height_MB*3;
    case OMX_COLOR_Format24bitBGR888:                 return width_MB*height_MB*3;
    case OMX_COLOR_Format24bitARGB1887:               return width_MB*height_MB*3;
    case OMX_COLOR_Format25bitARGB1888:               return (width_MB*height_MB*25)/8;
    case OMX_COLOR_Format32bitBGRA8888:               return width_MB*height_MB*4;
    case OMX_COLOR_Format32bitARGB8888:               return width_MB*height_MB*4;
    case OMX_COLOR_FormatYUV420Planar:                return (width_MB*height_MB*3)/2;
    case OMX_COLOR_FormatYUV420PackedPlanar:          return (width_MB*height_MB*3)/2;
    case OMX_COLOR_FormatYUV420SemiPlanar:            return (width_MB*height_MB*3)/2;
    case OMX_COLOR_FormatYUV422Planar:                return width_MB*height_MB*2;
    case OMX_COLOR_FormatYUV422PackedPlanar:          return width_MB*height_MB*2;
    case OMX_COLOR_FormatYUV422SemiPlanar:            return width_MB*height_MB*2;
    case OMX_COLOR_FormatYCbYCr:                      return width_MB*height_MB*2;
    case OMX_COLOR_FormatYCrYCb:                      return width_MB*height_MB*2;
    case OMX_COLOR_FormatCbYCrY:                      return width_MB*height_MB*2;
    case OMX_COLOR_FormatCrYCbY:                      return width_MB*height_MB*2;
    case OMX_COLOR_FormatYUV444Interleaved:           return width_MB*height_MB*3;
    case OMX_COLOR_FormatYUV420PackedSemiPlanar:      return (width_MB*height_MB*3)/2;
    case OMX_COLOR_FormatYUV422PackedSemiPlanar:      return (width_MB*height_MB*2);
    case OMX_COLOR_Format18BitBGR666:                 return (width_MB*height_MB*9)/4;
    case OMX_COLOR_Format24BitARGB6666:               return width_MB*height_MB*3;
    case OMX_COLOR_Format24BitABGR6666:               return width_MB*height_MB*3;
    case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
    case 0x7FFFFFFE: /* for backward compatibility */
	case OMX_COLOR_FormatAndroidOpaque:
    return (width_MB*height_MB*3)/2;
//MVC
    case OMX_COLOR_Format3DInterleaveLeftViewFirst:
    case OMX_COLOR_Format3DInterleaveRightViewFirst:
    case OMX_COLOR_Format3DTopBottomHalfLeftViewFirst:
    case OMX_COLOR_Format3DTopBottomHalfRightViewFirst:
    case OMX_COLOR_Format3DTopBottomFullLeftViewFirst:
    case OMX_COLOR_Format3DTopBottomFullRightViewFirst:
    case OMX_COLOR_Format3DSideBySideLeftViewFirst:
    case OMX_COLOR_Format3DSideBySideRightViewFirst:
//end MVC
    	return (width_MB*height_MB*3)/2;

    case OMX_COLOR_FormatUnused:
    case OMX_COLOR_FormatYUV411Planar:
    case OMX_COLOR_FormatYUV411PackedPlanar:
    case OMX_COLOR_FormatRawBayer8bit:
    case OMX_COLOR_FormatRawBayer10bit:
    case OMX_COLOR_FormatRawBayer8bitcompressed:
    case OMX_COLOR_FormatL2:
    case OMX_COLOR_FormatL4:
    case OMX_COLOR_FormatL8:
    case OMX_COLOR_FormatL16:
    case OMX_COLOR_FormatL24:
    case OMX_COLOR_FormatL32:
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, eColor);
    }
    return 0;
}

EXPORT_C
OMX_U32 VFM_Port::getStride(OMX_COLOR_FORMATTYPE eColor, OMX_U32 width)
{
    OMX_U32 width_MB = (width + 0xF)&(~0xF);

    switch ((int)eColor) {
    case OMX_COLOR_FormatUnused:                      return 0;     // this is used for encoded stream. Stride is useless in this case
    case OMX_COLOR_FormatMonochrome:                  return width_MB;
    case OMX_COLOR_Format8bitRGB332:                  return width_MB;
    case OMX_COLOR_Format12bitRGB444:                 return (width_MB*3)/2;
    case OMX_COLOR_Format16bitARGB4444:               return width_MB*2;
    case OMX_COLOR_Format16bitARGB1555:               return width_MB*2;
    case OMX_COLOR_Format16bitRGB565:                 return width_MB*2;
    case OMX_COLOR_Format16bitBGR565:                 return width_MB*2;
    case OMX_COLOR_Format18bitRGB666:                 return (width_MB*9)/4;
    case OMX_COLOR_Format18bitARGB1665:               return (width_MB*9)/4;
    case OMX_COLOR_Format19bitARGB1666:               return (width_MB*19)/8;
    case OMX_COLOR_Format24bitRGB888:                 return width_MB*3;
    case OMX_COLOR_Format24bitBGR888:                 return width_MB*3;
    case OMX_COLOR_Format24bitARGB1887:               return width_MB*3;
    case OMX_COLOR_Format25bitARGB1888:               return (width_MB*25)/8;
    case OMX_COLOR_Format32bitBGRA8888:               return width_MB*4;
    case OMX_COLOR_Format32bitARGB8888:               return width_MB*4;
    case OMX_COLOR_FormatYUV420Planar:                return (width_MB*3)/2;
    case OMX_COLOR_FormatYUV420PackedPlanar:          return (width_MB*3)/2;
    case OMX_COLOR_FormatYUV420SemiPlanar:            return (width_MB*3)/2;
    case OMX_COLOR_FormatYUV422Planar:                return width_MB*2;
    case OMX_COLOR_FormatYUV422PackedPlanar:          return width_MB*2;
    case OMX_COLOR_FormatYUV422SemiPlanar:            return width_MB*2;
    case OMX_COLOR_FormatYCbYCr:                      return width_MB*2;
    case OMX_COLOR_FormatYCrYCb:                      return width_MB*2;
    case OMX_COLOR_FormatCbYCrY:                      return width_MB*2;
    case OMX_COLOR_FormatCrYCbY:                      return width_MB*2;
    case OMX_COLOR_FormatYUV444Interleaved:           return width_MB*3;
    case OMX_COLOR_FormatYUV420PackedSemiPlanar:      return (width_MB*3)/2;
    case OMX_COLOR_FormatYUV422PackedSemiPlanar:      return (width_MB*2);
    case OMX_COLOR_Format18BitBGR666:                 return (width_MB*9)/4;
    case OMX_COLOR_Format24BitARGB6666:               return width_MB*3;
    case OMX_COLOR_Format24BitABGR6666:               return width_MB*3;
    case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
	case OMX_COLOR_FormatAndroidOpaque:
    case 0x7FFFFFFE: /* for backward compatibility */
//MVC
    case OMX_COLOR_Format3DInterleaveLeftViewFirst:
    case OMX_COLOR_Format3DInterleaveRightViewFirst:
    case OMX_COLOR_Format3DTopBottomHalfLeftViewFirst:
    case OMX_COLOR_Format3DTopBottomHalfRightViewFirst:
    case OMX_COLOR_Format3DTopBottomFullLeftViewFirst:
    case OMX_COLOR_Format3DTopBottomFullRightViewFirst:
    case OMX_COLOR_Format3DSideBySideLeftViewFirst:
    case OMX_COLOR_Format3DSideBySideRightViewFirst:
//end MVC
    return (width_MB*3)/2;

    case OMX_COLOR_FormatYUV411Planar:
    case OMX_COLOR_FormatYUV411PackedPlanar:
    case OMX_COLOR_FormatRawBayer8bit:
    case OMX_COLOR_FormatRawBayer10bit:
    case OMX_COLOR_FormatRawBayer8bitcompressed:
    case OMX_COLOR_FormatL2:
    case OMX_COLOR_FormatL4:
    case OMX_COLOR_FormatL8:
    case OMX_COLOR_FormatL16:
    case OMX_COLOR_FormatL24:
    case OMX_COLOR_FormatL32:
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, eColor);
    }
    return 0;
}
EXPORT_C
OMX_U32 VFM_Port::getBufferCountMin() const
{
    return mParamPortDefinition.nBufferCountMin;    // default is no change with the defined value
}


EXPORT_C
OMX_U32 VFM_Port::getBufferCountActual() const
{
    return mParamPortDefinition.nBufferCountActual;    // default is no change with the defined value
}



EXPORT_C
void VFM_Port::getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const
{
    // this function is virtual, and may be redefined in a codec in case of downscaling,...
    *nWidthSlave = nWidthMaster;
    *nHeightSlave = nHeightMaster;
}


EXPORT_C
void VFM_Port::getSizeFromParam(const OMX_PARAM_PORTDEFINITIONTYPE& portDef, OMX_U32 *nWidth, OMX_U32 *nHeight) const
{
    switch (getDomain()) {
	case OMX_PortDomainVideo:
        *nWidth = portDef.format.video.nFrameWidth;
        *nHeight = portDef.format.video.nFrameHeight;
        break;
	case OMX_PortDomainImage:
        *nWidth = portDef.format.image.nFrameWidth;
        *nHeight = portDef.format.image.nFrameHeight;
        break;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
        break;
    }
    return;
}

EXPORT_C
OMX_BOOL VFM_Port::isSizeChange(const OMX_PARAM_PORTDEFINITIONTYPE& portDef, const OMX_BOOL bIAmTheMaster) const
{
    OMX_U32 nWidthNew, nHeightNew;
    getSizeFromParam(portDef, &nWidthNew, &nHeightNew);
    if (bIAmTheMaster) {
        if (getFrameWidth()!=nWidthNew || getFrameHeight()!=nHeightNew) {
            return OMX_TRUE;
        } else {
            return OMX_FALSE;
        }
    } else {
        OMX_U32 nWidthMaster, nHeightMaster, nWidthNewSlave, nHeightNewSlave;
        VFM_Port *pPortMaster = getOtherPort();
        nWidthMaster = pPortMaster->getFrameWidth();
        nHeightMaster = pPortMaster->getFrameHeight();
        getSlavedSizeFromGivenMasterSize(nWidthMaster, nHeightMaster, &nWidthNewSlave, &nHeightNewSlave);
        if (nWidthNewSlave!=nWidthNew || nHeightNewSlave!=nHeightNew) {
            return OMX_TRUE;
        } else {
            return OMX_FALSE;
        }
    }
}


EXPORT_C
OMX_ERRORTYPE VFM_Port::updateParamPortDefinitionSlavedPort(const OMX_U32 nWidthMaster, const OMX_U32 nHeightMaster)
{
    OMX_PARAM_PORTDEFINITIONTYPE local_param = mParamPortDefinition;
    OMX_U32 nWidthNewSlave, nHeightNewSlave;
    getSlavedSizeFromGivenMasterSize(nWidthMaster, nHeightMaster, &nWidthNewSlave, &nHeightNewSlave);
    switch (getDomain()) {
	case OMX_PortDomainVideo:
        local_param.format.video.nFrameHeight = nHeightNewSlave;
        local_param.format.video.nFrameWidth = nWidthNewSlave;
        break;
	case OMX_PortDomainImage:
        local_param.format.image.nFrameHeight = nHeightNewSlave;
        local_param.format.image.nFrameWidth = nWidthNewSlave;
        break;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
        return OMX_ErrorUndefined;
    }
    return updateSettings(OMX_IndexParamPortDefinition, &local_param);
}

EXPORT_C
float VFM_Port::getPixelDepth(OMX_COLOR_FORMATTYPE format){
	switch ((int)format) {
	case OMX_COLOR_FormatUnused :

		return 0;
	case OMX_COLOR_FormatMonochrome :
	case OMX_COLOR_Format8bitRGB332 :
	case OMX_COLOR_FormatRawBayer8bit :
	case OMX_COLOR_FormatRawBayer8bitcompressed :
		return 1;

	case OMX_COLOR_Format12bitRGB444 :
	case OMX_COLOR_Format16bitARGB4444 :
	case OMX_COLOR_Format16bitARGB1555 :
	case OMX_COLOR_Format16bitRGB565 :
	case OMX_COLOR_Format16bitBGR565 :
	case OMX_COLOR_FormatYCbYCr :
	case OMX_COLOR_FormatYCrYCb :
	case OMX_COLOR_FormatCbYCrY :
	case OMX_COLOR_FormatCrYCbY :
		return 2;
	case OMX_COLOR_Format18bitRGB666 :
	case OMX_COLOR_Format18bitARGB1665 :
	case OMX_COLOR_Format19bitARGB1666 :
	case OMX_COLOR_Format24bitRGB888 :
	case OMX_COLOR_Format24bitBGR888 :
	case OMX_COLOR_Format24bitARGB1887 :
		return 3;
	case OMX_COLOR_Format25bitARGB1888 :
	case OMX_COLOR_Format32bitBGRA8888 :
	case OMX_COLOR_Format32bitARGB8888 :
	case OMX_COLOR_FormatYUV411Planar :
	case OMX_COLOR_FormatYUV411PackedPlanar :
	case OMX_COLOR_FormatYUV422Planar :
	case OMX_COLOR_FormatYUV422PackedPlanar :
	case OMX_COLOR_FormatYUV422SemiPlanar :
	case OMX_COLOR_FormatYUV422PackedSemiPlanar :

	case OMX_COLOR_FormatYUV444Interleaved :

	case OMX_COLOR_FormatRawBayer10bit : // is used for raw12!!!
	case OMX_SYMBIAN_COLOR_FormatRawBayer12bit : // is used for raw12!!!
		return 1.5 ;

	case OMX_COLOR_FormatYUV420PackedSemiPlanar :
	case OMX_COLOR_FormatYUV420Planar :
	case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
	case OMX_COLOR_FormatYUV420PackedPlanar :
    case OMX_COLOR_FormatYUV420SemiPlanar :
	case OMX_COLOR_FormatAndroidOpaque:
        case OMX_COLOR_FormatYUV420MBPackedSemiPlanar:
		return 1.5;
	case OMX_COLOR_Format18BitBGR666 :
	case OMX_COLOR_Format24BitARGB6666 :
	case OMX_COLOR_Format24BitABGR6666 :
		return 3;

	case OMX_COLOR_FormatL2 :
	case OMX_COLOR_FormatL4 :
	case OMX_COLOR_FormatL8 :
	case OMX_COLOR_FormatL16 :
	case OMX_COLOR_FormatL24 :
	case OMX_COLOR_FormatL32 :
	default :
		return 4;
	}
}

EXPORT_C
OMX_ERRORTYPE VFM_Port::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef) {
    IN0("");
    VFM_Component   *vfm_component = (VFM_Component *)(&(getENSComponent()));

    RETURN_OMX_ERROR_IF_ERROR(checkSetFormatInPortDefinition(&portDef));
    OMX_BOOL bSourceSink = vfm_component->isSourceSink();
    OMX_BOOL bSizeChange=OMX_FALSE, bIAmTheMaster=OMX_FALSE;

    if (vfm_component->getMaxBuffers(getPortIndex())>0 && portDef.nBufferCountActual>(OMX_U32)vfm_component->getMaxBuffers(getPortIndex())) {
        return OMX_ErrorBadParameter;
    }

    if (!bSourceSink) {
        bIAmTheMaster = isMasterPort();
        bSizeChange = isSizeChange(portDef, bIAmTheMaster);
        if (bSizeChange && !bIAmTheMaster) {
            // Only the master can change the slaved parameters
            return OMX_ErrorBadParameter ;
        }
    }

    switch (getDomain()) {
	case OMX_PortDomainVideo:
        // if change of size, reset the errormap
        if (mParamPortDefinition.format.video.nFrameHeight != portDef.format.video.nFrameHeight ||
		    mParamPortDefinition.format.video.nFrameWidth != portDef.format.video.nFrameWidth) {
                Common_Error_Map *error_map = vfm_component->getParamAndConfig()->getErrorMap();
                error_map->lockMutex();
                error_map->reset(portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight);
                error_map->releaseMutex();
        }

		//All except nSliceHeight field and stride that is computed
	mParamPortDefinition.format.video.nSliceHeight = portDef.format.video.nSliceHeight;
        mParamPortDefinition.format.video.bFlagErrorConcealment = portDef.format.video.bFlagErrorConcealment;
		mParamPortDefinition.format.video.cMIMEType = portDef.format.video.cMIMEType;
		mParamPortDefinition.format.video.eColorFormat = portDef.format.video.eColorFormat;
		mParamPortDefinition.format.video.eCompressionFormat = portDef.format.video.eCompressionFormat;
		mParamPortDefinition.format.video.nBitrate = portDef.format.video.nBitrate;
		mParamPortDefinition.format.video.nFrameHeight = portDef.format.video.nFrameHeight;
		mParamPortDefinition.format.video.nFrameWidth = portDef.format.video.nFrameWidth;
		mParamPortDefinition.format.video.pNativeRender = portDef.format.video.pNativeRender;
		mParamPortDefinition.format.video.pNativeWindow = portDef.format.video.pNativeWindow;
		mParamPortDefinition.format.video.xFramerate = portDef.format.video.xFramerate;
		mParamPortDefinition.format.video.nStride = portDef.format.video.nStride;
        break;
	case OMX_PortDomainImage:
		//All except nSliceHeight field and stride that is computed
        mParamPortDefinition.format.image.bFlagErrorConcealment = portDef.format.image.bFlagErrorConcealment;
		mParamPortDefinition.format.image.cMIMEType = portDef.format.image.cMIMEType;
		mParamPortDefinition.format.image.eColorFormat = portDef.format.image.eColorFormat;
		mParamPortDefinition.format.image.eCompressionFormat = portDef.format.image.eCompressionFormat;
		mParamPortDefinition.format.image.nFrameHeight = portDef.format.image.nFrameHeight;
		mParamPortDefinition.format.image.nFrameWidth = portDef.format.image.nFrameWidth;
		mParamPortDefinition.format.image.pNativeRender = portDef.format.image.pNativeRender;
		mParamPortDefinition.format.image.pNativeWindow = portDef.format.image.pNativeWindow;
		mParamPortDefinition.format.image.nStride = getStride(portDef.format.image.eColorFormat, mParamPortDefinition.format.image.nFrameWidth);
        break;
    default:
		OUTR("",OMX_ErrorNotImplemented);
        return OMX_ErrorNotImplemented;
    }

    mParamPortDefinition.nBufferSize = getBufferSize();
    mParamPortDefinition.nBufferCountMin = getBufferCountMin();

    // setFormatInPortDefinition() can be called:
    // - in Loaded mode when call is due to OMx_SetParameter
    // - or in any other state when call is due to an updateSettings(), when a setting has changed
    //   and requires a port disabling
    // so the ports can be populated or not at this point
    mParamPortDefinition.nBufferCountActual = portDef.nBufferCountActual;
    if (mParamPortDefinition.bPopulated) {
        // we must not update nBufferCountActual as it is used to free buffers
    } else {
        if (mParamPortDefinition.nBufferCountActual<mParamPortDefinition.nBufferCountMin) {
            mParamPortDefinition.nBufferCountActual = mParamPortDefinition.nBufferCountMin;
        }
    }

    if (!bSourceSink) {
        if (bSizeChange && bIAmTheMaster) {
            // Update the slaved port
            OMX_U32 nWidthMaster=0, nHeightMaster=0;
            getSizeFromParam(portDef, &nWidthMaster, &nHeightMaster);
            RETURN_OMX_ERROR_IF_ERROR(getOtherPort()->updateParamPortDefinitionSlavedPort(nWidthMaster, nHeightMaster));
        }
    }

    RETURN_OMX_ERROR_IF_ERROR(vfm_component->reset());

    /* + Change for CR 417003 - crash in encoders (ICS) when setprop for recycling delay is enabled */
    if(portDef.nPortIndex ==1)
    {
        if(OMX_FALSE == vfm_component->isDecoder())
        {
	       vfm_component->pParam->getRecyclingDelay()->set(0);
        }
    }
    /* - Change for CR 417003 */

    OUT0("");
    return OMX_ErrorNone;
}

EXPORT_C
OMX_ERRORTYPE VFM_Port::checkCompatibility(OMX_HANDLETYPE hTunneledComponent,OMX_U32 nTunneledPort) const
{
    OMX_ERRORTYPE err;
    OMX_PARAM_PORTDEFINITIONTYPE outputPortDef;

    outputPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    outputPortDef.nPortIndex = nTunneledPort;
    getOmxIlSpecVersion(&outputPortDef.nVersion);
    err = OMX_GetParameter(hTunneledComponent, OMX_IndexParamPortDefinition, &outputPortDef);
    if (err != OMX_ErrorNone) {
        return err;
    }

    // Checks the direction of the ports
    if (mParamPortDefinition.eDir != OMX_DirInput || outputPortDef.eDir != OMX_DirOutput) {
        return OMX_ErrorPortsNotCompatible;
    }

    // Checks the color and the compression format
    OMX_VIDEO_CODINGTYPE compressionComponent, compressionTunneled;
    OMX_COLOR_FORMATTYPE colorComponent, colorTunneled;
    if (mParamPortDefinition.eDomain==OMX_PortDomainVideo) {
        compressionComponent = mParamPortDefinition.format.video.eCompressionFormat;
        colorComponent = mParamPortDefinition.format.video.eColorFormat;
    } else if (mParamPortDefinition.eDomain==OMX_PortDomainImage) {
        compressionComponent = (OMX_VIDEO_CODINGTYPE)mParamPortDefinition.format.image.eCompressionFormat;
        colorComponent = mParamPortDefinition.format.image.eColorFormat;
    } else {
        compressionComponent = OMX_VIDEO_CodingMax;
        colorComponent = OMX_COLOR_FormatUnused;
    }

    if (outputPortDef.eDomain==OMX_PortDomainVideo) {
        compressionTunneled = outputPortDef.format.video.eCompressionFormat;
        colorTunneled = outputPortDef.format.video.eColorFormat;
    } else if (outputPortDef.eDomain==OMX_PortDomainImage) {
        compressionTunneled = (OMX_VIDEO_CODINGTYPE)outputPortDef.format.image.eCompressionFormat;
        colorTunneled = outputPortDef.format.image.eColorFormat;
    } else {
        compressionTunneled = OMX_VIDEO_CodingMax;
        colorTunneled = OMX_COLOR_FormatUnused;
    }

    if (compressionComponent!=compressionTunneled) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (colorComponent!=colorTunneled) {
        return OMX_ErrorPortsNotCompatible;
    }

	/* The following check is not relevant : an image port can be bound to a video port
    if (inputPortDef.eDomain != outputPortDef.eDomain) {
        return OMX_ErrorPortsNotCompatible;
    }
*/
    return OMX_ErrorNone;
}

EXPORT_C
void VFM_Port::setFrameWidth(OMX_U32 frameWidth)
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        mParamPortDefinition.format.video.nFrameWidth = frameWidth;
        break;
    case OMX_PortDomainImage:
        mParamPortDefinition.format.image.nFrameWidth = frameWidth;
        break;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
}

EXPORT_C
void VFM_Port::setFrameHeight(OMX_U32 frameHeight)
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        mParamPortDefinition.format.video.nFrameHeight = frameHeight;
        break;
    case OMX_PortDomainImage:
        mParamPortDefinition.format.image.nFrameHeight = frameHeight;
        break;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
}

EXPORT_C
OMX_U32 VFM_Port::getFrameWidth() const
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        return mParamPortDefinition.format.video.nFrameWidth;
    case OMX_PortDomainImage:
        return mParamPortDefinition.format.image.nFrameWidth;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
    return 0;
}

EXPORT_C
OMX_U32 VFM_Port::getFrameHeight() const
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        return mParamPortDefinition.format.video.nFrameHeight;
    case OMX_PortDomainImage:
        return mParamPortDefinition.format.image.nFrameHeight;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
    return 0;
}

EXPORT_C
OMX_COLOR_FORMATTYPE VFM_Port::getColorFormat() const
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        return mParamPortDefinition.format.video.eColorFormat;
    case OMX_PortDomainImage:
        return mParamPortDefinition.format.image.eColorFormat;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
    return OMX_COLOR_FormatUnused;
}

EXPORT_C
OMX_U32 VFM_Port::getCompressionFormat() const
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        return mParamPortDefinition.format.video.eCompressionFormat;
    case OMX_PortDomainImage:
        return mParamPortDefinition.format.image.eCompressionFormat;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
    return OMX_VIDEO_CodingUnused;
}

EXPORT_C
void VFM_Port::setParamWidthHeight(OMX_PARAM_PORTDEFINITIONTYPE *pNewdef, OMX_U32 frameWidth, OMX_U32 frameHeight)
{
    switch (getDomain()) {
    case OMX_PortDomainVideo:
        pNewdef->format.video.nFrameWidth = frameWidth;
        pNewdef->format.video.nFrameHeight = frameHeight;
        break;
    case OMX_PortDomainImage:
        pNewdef->format.image.nFrameWidth = frameWidth;
        pNewdef->format.image.nFrameHeight = frameHeight;
        break;
    default:
        vfm_assert_static((0==1), __LINE__, OMX_TRUE, getDomain());
    }
}


EXPORT_C OMX_ERRORTYPE VFM_Port::checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt) { return OMX_ErrorNotImplemented; };
EXPORT_C OMX_ERRORTYPE VFM_Port::checkIndexParamImagePortFormat(OMX_IMAGE_PARAM_PORTFORMATTYPE *pt) { return OMX_ErrorNotImplemented; };
