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
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_PortTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { mpeg4enc_port_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { mpeg4enc_port_assert(_error, __LINE__, OMX_FALSE); return (_error); } }


#include "MPEG4Enc_Proxy.h"
#include "MPEG4Enc_Port.h"
#include "VFM_DDepUtility.h"


MPEG4Enc_Port::MPEG4Enc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
: VFM_Port(commonPortData, enscomp)
{
	pProxyComponent = (MPEG4Enc_Proxy *)&enscomp;
	bCheck=OMX_FALSE;
	OstTraceInt0(TRACE_FLOW, "<=> MPEG4Enc_Port::MPEG4Enc_Port() constructor");

	//OstTraceFunctionEntryExt(MPEG4ENC_PORT_CONSTRUCTOR_ENTRY, this);	// OST Trace
	//OstTraceFunctionExitExt(MPEG4ENC_PORT_CONSTRUCTOR_EXIT, this, -1);	//OST Trace
}

OMX_U32 MPEG4Enc_Port::getBufferSize() const
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_Port::getBufferSize()");

	MPEG4Enc_Proxy *comp = ((MPEG4Enc_Proxy*)(&getENSComponent()));
	OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE err_corr;
	OMX_VIDEO_PARAM_MPEG4TYPE mp4_param;
	OMX_VERSIONTYPE version;
	OMX_U32 buffer_size = 0;
	version.nVersion = 0;

	getOmxIlSpecVersion(&version);
	err_corr.nVersion.nVersion = version.nVersion;
	err_corr.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

	mp4_param.nVersion.nVersion = version.nVersion;
	mp4_param.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

	//< minimum buffer size requirement for the output port is dependent on the current mode. If the mode is
	//< non-dp (non data partitioning, i.e. normal case), then the requirement is width*height*1.5 (this could
	//< be changed in feature when EOW is properly suppoted by FW and handled in ddep).
	//< in case of dp minimum buffer size is equal to maximum packet size allowed. Fw considers the
	//< max packet size while generating the packets (GOBs) in dp case. So any one packet size is always less
	//< than the nMaxPacketSize. This enables that atleast one packet is accommodated in the allocated buffer.
	if(getDirection() == OMX_DirOutput)
	{
		OstTraceInt0(TRACE_FLOW, "In MPEG4Enc_Port Computing buffer size for Output Port \n");

/*
		err = comp->getParameter(OMX_IndexParamVideoErrorCorrection, (OMX_PTR)&err_corr);
		if (err != OMX_ErrorNone) {
			OstTraceFiltInst2(TRACE_ERROR, "In MPEG4Enc_Port getBufferSize: Err 0x%x returned from getParameter at line no %d \n",err,__LINE__);
		}

		err = comp->getParameter(OMX_IndexParamVideoMpeg4, (OMX_PTR)&mp4_param);
		if (err != OMX_ErrorNone) {
			OstTraceFiltInst2(TRACE_ERROR, "In MPEG4Enc_Port getBufferSize: Err 0x%x returned from getParameter at line no %d \n",err,__LINE__);
		}
*/

	/* +Change start for ER348785 Data Partitioning */
			buffer_size	= getFrameWidth()*getFrameHeight()*3/2;
	/* -Change end for ER348785 Data Partitioning */

		//>OstTraceInt1(TRACE_NORMAL,MPEG4ENC_PORT_BITSTREAM_BUFFER_MINSIZE,"Bitstream Buffer Minsize(bytes) = %d\n",  mParamPortDefinition.nBufferSize);
	}
	else if(getDirection() == OMX_DirInput)
	{
		OstTraceInt0(TRACE_FLOW, "In MPEG4Enc_Port Computing buffer size for Input Port \n");
		buffer_size = getSizeFrame(getColorFormat(), getFrameWidth(), getFrameHeight());

		//>OstTrace1(TRACE_NORMAL,MPEG4ENC_PORT_IMAGE_BUFFER_MINSIZE,"Image Buffer Minsize(bytes) = %d\n", mParamPortDefinition.nBufferSize);
	}
	else
		buffer_size	= 0;

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_Port::getBufferSize()");

	return buffer_size;
}

OMX_ERRORTYPE MPEG4Enc_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition()");
	MPEG4Enc_Proxy *comp = ((MPEG4Enc_Proxy*)(&getENSComponent()));
	OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portDef->format.video));
	OMX_VIDEO_PORTDEFINITIONTYPE * mvideo_port;
	/* Checking for valid nFilledLen for the input buffer */
	ENS_Port *pt_port_in;
#ifdef __ndk5500_a0_
	RETURN_XXX_IF_WRONG(((pt_video->nFrameHeight)%16 ==0) && ((pt_video->nFrameWidth)%16 == 0), OMX_ErrorBadParameter);
#endif
	//< the resolution check has to be changed here when 1080p is eventually supported
	/* + change for ER 346056 */
	//>RETURN_XXX_IF_WRONG(1 <= pt_video->nFrameHeight && pt_video->nFrameHeight <= 720, OMX_ErrorBadParameter);
	//>RETURN_XXX_IF_WRONG(1 <= pt_video->nFrameWidth && pt_video->nFrameWidth <= 1280, OMX_ErrorBadParameter);
	RETURN_XXX_IF_WRONG(16*16 <= ((pt_video->nFrameHeight)*(pt_video->nFrameHeight)), OMX_ErrorBadParameter);
	RETURN_XXX_IF_WRONG(((pt_video->nFrameHeight)*(pt_video->nFrameHeight)) <= 1280*720, OMX_ErrorBadParameter);
	/* - change for ER 346056 */
	if( portDef->nPortIndex == 0 )
	{
		if(pProxyComponent->isMpcObject)
		{
			RETURN_XXX_IF_WRONG((pt_video->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar)||(pt_video->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)||(pt_video->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque), OMX_ErrorBadParameter);
		}
		else
		{
			RETURN_XXX_IF_WRONG(pt_video->eColorFormat == OMX_COLOR_FormatYUV420Planar|| (pt_video->eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar), OMX_ErrorBadParameter);
		}
		// Setting xEncodeFramerate equal to xFramerate sent by user
		pProxyComponent->mParam.m_framerate.xEncodeFramerate = pt_video->xFramerate;

	}
	else if (portDef->nPortIndex == 1)
	{
		pt_port_in = comp->getPort(VPB+0);
		mvideo_port = pt_port_in->getVideoPortDefinition();
		if(	(((mvideo_port->nStride)/getPixelDepth(mvideo_port->eColorFormat)) > mvideo_port->nFrameWidth) || ((mvideo_port->nSliceHeight) > mvideo_port->nFrameHeight))
		{
			bCheck = OMX_TRUE;
		}
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition() bCheck = %d",bCheck);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition() pt_video->nStride = %d",pt_video->nStride);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition() pt_video->nFrameWidth = %d",pt_video->nFrameWidth);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition() pt_video->nSliceHeight = %d",pt_video->nSliceHeight);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::checkSetFormatInPortDefinition() pt_video->nFrameHeight = %d",pt_video->nFrameHeight);
		//1.
		//The internal decision of whether to set the parameters according to ShortHeader or SimpleProfile
		// is taken on the basis of bSVH flag. If the application only sets eCompressionFormat to OMX_VIDEO_CodingH263
		// through OMX_IndexParamVideoPortFormat and does not explicitly set bSVH flag through OMX_IndexParamVideoMpeg4
		// index (since now the encoding is H263 and not MPEG4), then bSVH has to be set accordingly. This is being done here.
		// some test cases use bSVH flag with OMX_IndexParamVideoMpeg4, but set the eCompressionFormat as MPEG4. This is
		// mainly because SH mode can mean ANY one of two things: MPEG4 ShortHeader OR H263 Encoder.
		// So if the user sets H263, then for sure, the bSVH flag is to be set, but if the user sets MPEG4 and has already
		// set bSVH as 1 (i.e. different from the default value of 0), then we assume that the user implies MPEG4 SH mode.
		// So for OMX_VIDEO_CodingMPEG4, we leave the decision for SP or SH to the user's selection of bSVH flag, and thus,
		// here, we do nothing!
		//2.
		//We call the setOneProfileLevelSupported() API of VFM here instead of its earlier location.
	    if (pt_video->eCompressionFormat == OMX_VIDEO_CodingMPEG4)
    	{
			OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc_Port::checkSetFormatInPortDefinition(): (eCompressionFormat == OMX_VIDEO_CodingMPEG4) for output port");
			pProxyComponent->clearAllProfileLevelSupported();
			pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4LevelMax);
			//do nothing for bSVH flag, decision taken on the basis of bSVH flag set by user in OMX_IndexParamVideoMpeg4.
		}
    	else
    	{
			OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc_Port::checkSetFormatInPortDefinition(): (eCompressionFormat == OMX_VIDEO_CodingH263) for output port");
			pProxyComponent->clearAllProfileLevelSupported();
			pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileBaseline, OMX_VIDEO_H263Level45);
			pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileISWV2, OMX_VIDEO_H263Level45);
    		pProxyComponent->mParam.m_enc_param.bSVH = OMX_TRUE;
    	}
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::checkSetFormatInPortDefinition()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Port::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::getParameter()");

	OMX_ERRORTYPE omx_error;
    switch (nParamIndex) {
        case OMX_IndexParamVideoQuantization:
		case OMX_IndexParamVideoPortFormat:
		case OMX_IndexParamVideoFastUpdate:
		case OMX_IndexParamVideoBitrate:
		case OMX_IndexParamVideoMotionVector:
		case OMX_IndexParamVideoIntraRefresh:
		case OMX_IndexParamVideoErrorCorrection:
		case OMX_IndexParamVideoMpeg4:
		case OMX_IndexParamVideoH263:
			omx_error = pProxyComponent->getParameter(nParamIndex, pt);
			if (OMX_ErrorNone != omx_error)
			{
				OstTraceInt3(TRACE_ERROR, "In MPEG4Enc_Port returning omx error : 0x%x for getParameter Index (0x%x) at line no %d \n",omx_error,nParamIndex,__LINE__);

			}
			return omx_error;

        default:
            // SHAI extension
            if(nParamIndex == (OMX_INDEXTYPE)OMX_IndexParamPixelAspectRatio || nParamIndex ==(OMX_INDEXTYPE)OMX_IndexParamColorPrimary)
            {
                return pProxyComponent->getParameter(nParamIndex, pt);
            }
            else
            {
                RETURN_OMX_ERROR_IF_ERROR(VFM_Port::getParameter(nParamIndex, pt));
            }
    }

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::getParameter()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Port::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::setParameter()");

	OMX_ERRORTYPE omx_error;
    switch (nParamIndex) {
        case OMX_IndexParamVideoQuantization:
		case OMX_IndexParamVideoPortFormat:
		case OMX_IndexParamVideoFastUpdate:
		case OMX_IndexParamVideoBitrate:
		case OMX_IndexParamVideoMotionVector:
		case OMX_IndexParamVideoIntraRefresh:
		case OMX_IndexParamVideoErrorCorrection:
		case OMX_IndexParamVideoMpeg4:
		case OMX_IndexParamVideoH263:
			omx_error = pProxyComponent->setParameter(nParamIndex, pt);
			if (OMX_ErrorNone != omx_error)
			{
				OstTraceInt3(TRACE_ERROR, "In MPEG4Enc_Port returning omx error : 0x%x for getParameter Index (0x%x) at line no %d \n",omx_error,nParamIndex,__LINE__);
			}
			return omx_error;

        default:
            // SHAI extension
            if(nParamIndex ==(OMX_INDEXTYPE)OMX_IndexParamPixelAspectRatio || nParamIndex ==(OMX_INDEXTYPE)OMX_IndexParamColorPrimary)
            {
                omx_error = pProxyComponent->setParameter(nParamIndex, pt);
				if (OMX_ErrorNone != omx_error)
				{
					OstTraceInt3(TRACE_ERROR, "In MPEG4Enc_Port returning omx error : 0x%x for getParameter Index (0x%x) at line no %d \n",omx_error,nParamIndex,__LINE__);
				}
				return omx_error;
            }
            else
            {
                RETURN_OMX_ERROR_IF_ERROR(VFM_Port::setParameter(nParamIndex, pt));
            }
    }

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::setParameter()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Port::setIndexParamVideoPortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::setIndexParamVideoPortFormat()");

	*has_changed = OMX_TRUE;
	OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)pt_org;
	RETURN_XXX_IF_WRONG(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);
	if(pt->nPortIndex==VPB+0)
	{			 // input port
		RETURN_XXX_IF_WRONG(pt->eCompressionFormat == OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG((pt->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar) || (pt->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) || (pt->eColorFormat == OMX_COLOR_FormatYUV420Planar)|| (pt->eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar)||(pt->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque), OMX_ErrorBadParameter);
		// nothing to be checked on pt->nIndex
	}
	else
	{		 // output port
		RETURN_XXX_IF_WRONG((pt->eCompressionFormat == OMX_VIDEO_CodingMPEG4) || (pt->eCompressionFormat == OMX_VIDEO_CodingH263), OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG(pt->eColorFormat == OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
		// nothing to be checked on pt->nIndex
	}

	//1.
	//The internal decision of whether to set the parameters according to ShortHeader or SimpleProfile
	// is taken on the basis of bSVH flag. If the application only sets eCompressionFormat to OMX_VIDEO_CodingH263
	// through OMX_IndexParamVideoPortFormat and does not explicitly set bSVH flag through OMX_IndexParamVideoMpeg4
	// index (since now the encoding is H263 and not MPEG4, then bSVH has to be set accordingly. This is being done here:
	//2.
	//Now that we know that the user has set a particular eCompressionType, we call the setOneProfileLevelSupported()
	// API of VFM here, instead of its earlier location.
	if(pt->eCompressionFormat == OMX_VIDEO_CodingMPEG4)
	{
		pProxyComponent->clearAllProfileLevelSupported();
		pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4LevelMax);
		pProxyComponent->mParam.m_enc_param.bSVH = OMX_FALSE;
	}
	else
	{
		pProxyComponent->clearAllProfileLevelSupported();
		pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileBaseline, OMX_VIDEO_H263Level45);
		if(pProxyComponent->isMpcObject == OMX_FALSE)
		{
		pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileISWV2, OMX_VIDEO_H263Level45);
		}

		pProxyComponent->mParam.m_enc_param.bSVH = OMX_TRUE;
	}

	mParamPortDefinition.format.video.eColorFormat = pt->eColorFormat;
	mParamPortDefinition.format.video.eCompressionFormat = pt->eCompressionFormat;
	mParamPortDefinition.format.video.xFramerate = pt->xFramerate;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::setIndexParamVideoPortFormat()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_Port::getIndexParamVideoPortFormat(OMX_PTR pt_org) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::getIndexParamVideoPortFormat()");

	OMX_VIDEO_PARAM_PORTFORMATTYPE *pt = (OMX_VIDEO_PARAM_PORTFORMATTYPE *)pt_org;
	RETURN_XXX_IF_WRONG(pt->nPortIndex==VPB+0 || pt->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);
	pt->eCompressionFormat = mParamPortDefinition.format.video.eCompressionFormat;
	if(pt->nPortIndex==VPB+0)
	{			 // input port
		pt->eCompressionFormat = OMX_VIDEO_CodingUnused;
		switch (pt->nIndex)
		{
			case 0:
				if(pProxyComponent->isMpcObject == OMX_TRUE) {
					pt->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
				}
				else {
					pt->eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
				}
				break;
			case 1:
				if(pProxyComponent->isMpcObject == OMX_TRUE) {
					pt->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatAndroidOpaque;
				}
				break;
			default:
					return OMX_ErrorNoMore;
		}

	}
	else
	{		// output port
		switch(pt->nIndex)
		{
			case 0:
				 pt->eCompressionFormat = OMX_VIDEO_CodingMPEG4;
				 pt->eColorFormat = OMX_COLOR_FormatUnused;
				 break;
			case 1:
				pt->eCompressionFormat = OMX_VIDEO_CodingH263;
				pt->eColorFormat = OMX_COLOR_FormatUnused;
				break;
			default:
				return OMX_ErrorNoMore;
		}
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::getIndexParamVideoPortFormat()");

	return OMX_ErrorNone;
}

void MPEG4Enc_Port::getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_Port::getSlavedSizeFromGivenMasterSize()");

	*nWidthSlave = 0;
	*nHeightSlave = 0;

   *nWidthSlave = mParamPortDefinition.format.video.nFrameWidth;
   *nHeightSlave = mParamPortDefinition.format.video.nFrameHeight;

   //cases when no cropping and no downscaling
   if ((*nWidthSlave ==0) &&  (*nHeightSlave==0))
   {
	   *nWidthSlave   = nWidthMaster;
	   *nHeightSlave  = nHeightMaster;
   }

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_Port::getSlavedSizeFromGivenMasterSize()");

}

// Set the default value of the port. This function is used by the construct() function of the proxy
// when creating a new instance of the proxy
// FIXME: some parameters are missing => refer to H264Dec_Port
void MPEG4Enc_Port::setDefault()
{
	DBC_ASSERT(mParamPortDefinition.eDomain==OMX_PortDomainVideo);

	switch(mParamPortDefinition.nPortIndex)
	{
		case 0:
			DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirInput);

			// from OMX spec 1.1.2 (p. 366)
			mParamPortDefinition.format.video.nFrameWidth           = 0;//>176;
			mParamPortDefinition.format.video.nFrameHeight          = 0;//>144;
			mParamPortDefinition.format.video.xFramerate            = 15<<16;
			mParamPortDefinition.format.video.eCompressionFormat    = OMX_VIDEO_CodingUnused;
			mParamPortDefinition.format.video.eColorFormat          = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;

			break;
		case 1:

			DBC_ASSERT(mParamPortDefinition.eDir==OMX_DirOutput);

			// from OMX spec 1.1.2 (p. 367)
			mParamPortDefinition.format.video.nFrameWidth           = 0;//>176;
			mParamPortDefinition.format.video.nFrameHeight          = 0;//>144;
			mParamPortDefinition.format.video.nBitrate              = 64000;
			mParamPortDefinition.format.video.xFramerate            = 15<<16;
			mParamPortDefinition.format.video.eCompressionFormat    = OMX_VIDEO_CodingMPEG4;
			mParamPortDefinition.format.video.eColorFormat          = OMX_COLOR_FormatUnused;
			/* +Change for CR 369344 */
			mParamPortDefinition.nBufferCountActual = 5;
            {
                OMX_U32 tempvalue=0;
                tempvalue = VFM_SocCapabilityMgt::getOutputBufferCount();
                OstTraceFiltInst1(TRACE_FLOW, "MPEG4Enc_Port::Output buffer count from environment : %d",tempvalue);
                if (tempvalue)
                {
                    mParamPortDefinition.nBufferCountActual = tempvalue;
                }
                OstTraceFiltInst1(TRACE_FLOW, "MPEG4Enc_Port::value of output buffer count actual %d",mParamPortDefinition.nBufferCountActual);
            }
			/* -Change for CR 369344 */

			break;
		default:
			DBC_ASSERT(0==1);
	}

	mParamPortDefinition.format.video.cMIMEType = (OMX_STRING)"video/mp4";
	mParamPortDefinition.format.video.pNativeRender = NULL;
	mParamPortDefinition.format.video.pNativeWindow = NULL;

	mParamPortDefinition.nBufferSize = getBufferSize();
}
