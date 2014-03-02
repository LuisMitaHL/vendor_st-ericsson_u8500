/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define _CNAME_ Vfm_mpeg4dec
#include "osi_trace.h"
#include "MPEG4Dec_Proxy.h"
#include "VFM_DDepUtility.h"
#include "MPEG4Dec_Factory.h"
#include "VFM_Port.h" //jitender:: fix this create a new header and source file for port
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_proxy_src_MPEG4Dec_ProxyTraces.h"
#endif


/**********************************************************************************/
class VFM_MPEG4Dec_Port:public VFM_Port
{
private:
#ifdef PACKET_VIDEO_SUPPORT
    OMX_U32 mSuggestedBufferSize;
#endif
	MPEG4Dec_Proxy *pProxyComponent;
public:
	OMX_BOOL MPCOBJECT;
	VFM_MPEG4Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp,OMX_BOOL isMPCobject)
		: VFM_Port(commonPortData, enscomp)
	{
		OstTraceInt0(TRACE_API, "=> VFM_MPEG4Dec_Port::VFM_MPEG4Dec_Port() constructor");

		pProxyComponent = (MPEG4Dec_Proxy *)&enscomp;

        MPCOBJECT=isMPCobject;
#ifdef PACKET_VIDEO_SUPPORT
        mSuggestedBufferSize = 0;
#endif
		if (commonPortData.mDirection == OMX_DirInput) {            // input port
			mParamPortDefinition.format.video.nStride = 0;
			mParamPortDefinition.format.video.pNativeRender = 0;
			mParamPortDefinition.format.video.pNativeWindow = 0;
			mParamPortDefinition.format.video.xFramerate = 0;
			mParamPortDefinition.format.video.nSliceHeight = 0;
			mParamPortDefinition.format.video.nFrameHeight = 480;
			mParamPortDefinition.format.video.nFrameWidth = 640;
			mParamPortDefinition.format.video.nBitrate = 0;
			mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
			mParamPortDefinition.format.video.cMIMEType = 0;

			mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
			mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;

			mParamPortDefinition.nBufferSize = 256; //any non-zero value

			mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
			mParamPortDefinition.nBufferAlignment = 16;
			OstTraceInt1(TRACE_FLOW, "commonPortData.mDirection = %d", commonPortData.mDirection);
		} else  {        // output port
			mParamPortDefinition.format.video.nStride = 0;
			mParamPortDefinition.format.video.pNativeRender = 0;
			mParamPortDefinition.format.video.pNativeWindow = 0;
			mParamPortDefinition.format.video.xFramerate = 0;
			mParamPortDefinition.format.video.nSliceHeight = 0;
			mParamPortDefinition.format.video.nFrameHeight = 480;
			mParamPortDefinition.format.video.nFrameWidth = 640;
			mParamPortDefinition.format.video.nBitrate = 0;
			mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
			mParamPortDefinition.format.video.cMIMEType = 0;
			if(MPCOBJECT==OMX_TRUE)
			{
				if (VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize())
				{
					mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
				}
				else
				{
					mParamPortDefinition.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
				}
			}
			else
				mParamPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
            mParamPortDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;

			mParamPortDefinition.nBufferSize = 460800;

			mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
			mParamPortDefinition.nBufferAlignment = 256;
			OstTraceInt1(TRACE_FLOW, "commonPortData.mDirection = %d", commonPortData.mDirection);
		}
		OstTraceInt0(TRACE_API, "<= VFM_MPEG4Dec_Port::VFM_MPEG4Dec_Port() constructor");
	}

	virtual OMX_U32 getBufferSize() const
	{
		OstTraceInt0(TRACE_API, "=> VFM_MPEG4Dec_Port::getBufferSize()");

        OMX_U32 default_return = 256;

         //+CR369244
         if(VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize()) {
             default_return = VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize();
         }
         //-CR369244

            if (mParamPortDefinition.format.video.eColorFormat != OMX_COLOR_FormatUnused) {
                int iWidth = mParamPortDefinition.format.video.nFrameWidth;
                int iHeight = mParamPortDefinition.format.video.nFrameHeight;
                iWidth = ((iWidth + 15) >> 4) << 4;
                iHeight = ((iHeight + 15) >> 4) << 4;
                if(!iWidth || !iHeight) {
                    default_return = 512;
                } else {
                    if( (MPCOBJECT==OMX_TRUE) || ((iWidth % 32) == 0) || ((iHeight % 32) == 0) ) {
                        default_return = getSizeFrame(mParamPortDefinition.format.video.eColorFormat,
                                                      iWidth, iHeight);
                    } else {
#ifdef __MPEG4DEC_SOFT_DECODER
                        default_return =  getSizeFrame(mParamPortDefinition.format.video.eColorFormat,
                                                       iWidth, iHeight);
#else
                        default_return =  getSizeFrame(mParamPortDefinition.format.video.eColorFormat,
                                                       iWidth, iHeight) + 128;
#endif
                    }
		}
            }
#ifdef PACKET_VIDEO_SUPPORT
            if(mSuggestedBufferSize != 0) {
                default_return = mSuggestedBufferSize;
            }
#endif
			OstTraceInt0(TRACE_API, "<= VFM_MPEG4Dec_Port::getBufferSize()");
            return default_return;
    }

    virtual OMX_U32 getBufferCountMin() const
    {
		OstTraceInt0(TRACE_API, "<=> VFM_MPEG4Dec_Port::getBufferCountMin()");

		VFM_Component *pComp = (VFM_Component *)(&getENSComponent());
		if (pComp->getParamAndConfig()->getThumbnailGeneration() == OMX_TRUE)
		{
             switch (getDirection()) {
             case OMX_DirInput:
                 return 2;
             case OMX_DirOutput:
                 return 1;
             default:
                 return OMX_ErrorNoMore;
			}
		}
		else
		{
			switch (getDirection()) {
			case OMX_DirInput:
				return 2;
                case OMX_DirOutput:
                //+ER406374
				return 7;
                //-ER406374
                default:
				return OMX_ErrorNoMore;
			}
		}
	}

	virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
	{
        //+CR369593
        MPEG4Dec_Proxy *pComp = (MPEG4Dec_Proxy *)(&getENSComponent());
        MPEG4Dec_ParamAndConfig *pParam = ((MPEG4Dec_ParamAndConfig *)(pComp->getParamAndConfig()));
        //-CR369593

		OstTraceInt1(TRACE_API, "=> VFM_MPEG4Dec_Port::checkSetFormatInPortDefinition() for nPortIndex = %d", portDef->nPortIndex);

        OMX_VIDEO_PORTDEFINITIONTYPE *pt_video = (OMX_VIDEO_PORTDEFINITIONTYPE *)(&(portDef->format.video));

        /* + Changes for CR 334359 */
		OMX_U32 maxSupportedWidth, maxSupportedHeight;
		if(VFM_SocCapabilityMgt::isSoc1080pCapable() == OMX_TRUE)
	    {
		    maxSupportedWidth = 1920;
			maxSupportedHeight = 1088;
			OstTraceInt0(TRACE_FLOW, "checkSetFormatInPortDefinition - isSoc1080pCapable returned TRUE");
	    }
	    else
		{
		    maxSupportedWidth = 1280;
		    maxSupportedHeight = 720;
			OstTraceInt0(TRACE_FLOW, "checkSetFormatInPortDefinition - isSoc1080pCapable returned FALSE");
		}
		OstTraceInt2(TRACE_FLOW, "max supported Width %d Height %d", maxSupportedWidth, maxSupportedHeight);

		if (portDef->nPortIndex == 1)
		{
			if (pt_video->eCompressionFormat == OMX_VIDEO_CodingMPEG4)
			{
				pProxyComponent->clearAllProfileLevelSupported();
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4Level5);
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileAdvancedSimple,OMX_VIDEO_MPEG4Level5);
			}
			else
			{
				pProxyComponent->clearAllProfileLevelSupported();
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileBaseline, OMX_VIDEO_H263Level45);
				if(MPCOBJECT==OMX_FALSE)
				{
					pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileISWV2,OMX_VIDEO_H263Level45);
				}
			}
		}
		OstTraceInt2(TRACE_FLOW, "max supported Width %d Height %d", maxSupportedWidth, maxSupportedHeight);

		//+ER329503
		/* +change for 346056 */
		//RETURN_XXX_IF_WRONG(0 <= pt_video->nFrameHeight && pt_video->nFrameHeight <= maxSupportedHeight, OMX_ErrorBadParameter);
		//RETURN_XXX_IF_WRONG(0 <= pt_video->nFrameWidth && pt_video->nFrameWidth <= maxSupportedWidth, OMX_ErrorBadParameter);
		/* +Change for ER 418389 */
		RETURN_XXX_IF_WRONG(0 <=(signed)(pt_video->nFrameWidth*pt_video->nFrameHeight),OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG((pt_video->nFrameWidth*pt_video->nFrameHeight) <= (maxSupportedWidth*maxSupportedHeight), OMX_ErrorBadParameter);
		/* -Change for ER 418389 */
		/* -change for 346056 */
        //-ER329503
		/* - Changes for CR 334359 */

        //+CR369593
        if(portDef->nPortIndex == 0)
        {
			/* + Change for ER 429329, 426137 */
			if (!(pParam->CropWidth) && !(pParam->CropHeight))
			{
                //The width/height sent by the user may be non-multiple of 16. So we make it a multiple of 16
                //
                //Storing the actual width/height sent by the user for later use in sending Crop parameters
                pParam->CropWidth = pt_video->nFrameWidth;
                pParam->CropHeight = pt_video->nFrameHeight;
			}
			/* - Change for ER 429329, 426137 */
        }

        //Making it x16
        pt_video->nFrameWidth = ((pt_video->nFrameWidth + 0xF) & (~0xF));
        pt_video->nFrameHeight = ((pt_video->nFrameHeight + 0xF) & (~0xF));

		OstTraceFiltInst4(TRACE_FLOW, "VFM_MPEG4Dec_Port::checkSetFormatInPortDefinition(): (Checking for x16 sizes) OLD: pParam->CropWidth = %d, pParam->CropHeight = %d, NEW: pt_video->nFrameWidth = %d, pt_video->nFrameHeight = %d", pParam->CropWidth, pParam->CropHeight, pt_video->nFrameWidth, pt_video->nFrameHeight);
		//-CR369593

#ifdef PACKET_VIDEO_SUPPORT
                if(portDef->nBufferSize > getBufferSize()) {
                    mSuggestedBufferSize = portDef->nBufferSize;
                }
#endif
		OstTraceInt0(TRACE_API, "<= VFM_MPEG4Dec_Port::checkSetFormatInPortDefinition()");
		return OMX_ErrorNone;
     }

	OMX_ERRORTYPE setIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef)
	{
        OstTraceFiltInst0(TRACE_API, "=> VFM_MPEG4Dec_Port::setIndexParamVideoPortFormat()");
        RETURN_XXX_IF_WRONG(portDef->nPortIndex==VPB+0 || portDef->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);
		if (portDef->nPortIndex==VPB+0) {            // input port
			RETURN_XXX_IF_WRONG((portDef->eCompressionFormat == OMX_VIDEO_CodingMPEG4)||(portDef->eCompressionFormat == OMX_VIDEO_CodingH263)||(portDef->eCompressionFormat == 12), OMX_ErrorBadParameter);
			RETURN_XXX_IF_WRONG(portDef->eColorFormat==OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
			// nothing to be check on pt->nIndex
			mParamPortDefinition.format.video.eColorFormat = portDef->eColorFormat;
			mParamPortDefinition.format.video.eCompressionFormat = portDef->eCompressionFormat;

		} else  {        // output port
			RETURN_XXX_IF_WRONG(portDef->eCompressionFormat == OMX_VIDEO_CodingUnused, OMX_ErrorBadParameter);
			RETURN_XXX_IF_WRONG(((portDef->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) || (portDef->eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar) || (portDef->eColorFormat ==OMX_COLOR_FormatYUV420Planar)), OMX_ErrorBadParameter);
			// nothing to be check on pt->nIndex
			mParamPortDefinition.format.video.eColorFormat = portDef->eColorFormat;
			mParamPortDefinition.format.video.eCompressionFormat = portDef->eCompressionFormat;
			if (portDef->eCompressionFormat == OMX_VIDEO_CodingMPEG4)
			{
				pProxyComponent->clearAllProfileLevelSupported();
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4Level5);
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileAdvancedSimple,OMX_VIDEO_MPEG4Level5);
			}
			else
			{
				pProxyComponent->clearAllProfileLevelSupported();
				pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileBaseline, OMX_VIDEO_H263Level45);
				if(MPCOBJECT==OMX_FALSE)
					pProxyComponent->setOneProfileLevelSupported(OMX_VIDEO_H263ProfileISWV2,OMX_VIDEO_H263Level45);
			}

		}
        OstTraceFiltInst0(TRACE_API, "<= VFM_MPEG4Dec_Port::setIndexParamVideoPortFormat()");
        return OMX_ErrorNone;
	}

	OMX_ERRORTYPE getIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef) const
	{
        OstTraceFiltInst0(TRACE_API, "=> VFM_MPEG4Dec_Port::getIndexParamVideoPortFormat()");
        RETURN_XXX_IF_WRONG(portDef->nPortIndex==VPB+0 || portDef->nPortIndex==VPB+1, OMX_ErrorBadPortIndex);
		portDef->eCompressionFormat = mParamPortDefinition.format.video.eCompressionFormat;
		if(portDef->nPortIndex == VPB+0)
		{
			switch(portDef->nIndex)
			{
				case 0:
					 portDef->eCompressionFormat = OMX_VIDEO_CodingMPEG4;
					 portDef->eColorFormat = OMX_COLOR_FormatUnused;
					 break;
				case 1:
					portDef->eCompressionFormat = OMX_VIDEO_CodingH263;
					portDef->eColorFormat = OMX_COLOR_FormatUnused;
					break;
					case 2:
					portDef->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)12;
					portDef->eColorFormat = OMX_COLOR_FormatUnused;
					break;
				default:
					return OMX_ErrorNoMore;
			}
		} else {
			switch (portDef->nIndex)
			{
			case 0:
				if(MPCOBJECT==OMX_TRUE)
				{
					if (VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize())
					{
						portDef->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
					}
					else
					{
						portDef->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
					}
				}
				else
				{
					portDef->eColorFormat = OMX_COLOR_FormatYUV420Planar;
				}
					break;
                default:
					return OMX_ErrorNoMore;
			}
		}
        OstTraceFiltInst0(TRACE_API, "<= VFM_MPEG4Dec_Port::getIndexParamVideoPortFormat()");
        return OMX_ErrorNone;
	}
};

MPEG4Dec_Proxy::MPEG4Dec_Proxy():VFM_Component(&mParam),processingComponent(*this),mParam(this)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Dec_Proxy::MPEG4Dec_Proxy constructor");

    isMPCobject=OMX_TRUE;
	sorenson_flag=OMX_FALSE;
	thumbnail=OMX_FALSE;
	sva_mcps = 0;

	#ifdef FULL_POWER_MANAGEMENT
		is_full_powermgt = OMX_TRUE;
	#else
		is_full_powermgt = OMX_FALSE;
	#endif

	OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::MPEG4Dec_Proxy constructor");
}

VFM_Port *MPEG4Dec_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
	OstTraceInt0(TRACE_API, "<=> MPEG4Dec_Proxy::newPort()");
	return new VFM_MPEG4Dec_Port(commonPortData, enscomp,isMPCobject);
}

inline void MPEG4Dec_Proxy::mpeg4dec_assert(int condition, int line, OMX_BOOL isFatal)
{
    if (!condition) {
        OstTraceInt1(TRACE_ERROR, "MPEG4Dec_Proxy::mpeg4dec_assert(): error line %d", line);
        NMF_LOG("HMPEG4Dec_Proxy::mpeg4dec_assert(): error line %d\n", line);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}
OMX_ERRORTYPE MPEG4Dec_Proxy::construct()
{
	OstTraceInt0(TRACE_API, "=> MPEG4Dec_Proxy::construct()");

    RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
    setProcessingComponent(&processingComponent);
    RETURN_OMX_ERROR_IF_ERROR(processingComponent.setPortCount(2));

	if(thumbnail==OMX_FALSE)
	{
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
					7/*nBufferCountMin - Modified for ER 349396*/,
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
	}
	else
	{
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
				  1/*nBufferCountMin*/,
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
	}

    // set the role of the component: video_decoder.mpeg4
    mpeg4dec_assert((1 == getRoleNB()), __LINE__, OMX_TRUE);
    ENS_String<20> role = "video_decoder.mpeg4";
    role.put((OMX_STRING)mRoles[0]);
    setActiveRole(mRoles[0]);

	OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::construct()");

	return OMX_ErrorNone;
}

MPEG4Dec_Proxy::~MPEG4Dec_Proxy()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Dec_Proxy::~MPEG4Dec_Proxy() destructor");

	setProcessingComponent(0);  //is there some error in this

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Dec_Proxy::~MPEG4Dec_Proxy() destructor");
}

EXPORT_C  OMX_ERRORTYPE ThumbNailMPEG4DecFactoryMethod(ENS_Component_p * ppENSComponent)
{
    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_TRUE;
	comp->sorenson_flag=OMX_FALSE;
	comp->thumbnail=OMX_TRUE;
	RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;
	OstTraceInt2(TRACE_FLOW, "MPEG4Dec_Proxy::ThumbNailMPEG4DecFactoryMethod thumbnail =%d isMPCobject =%d \n",comp->thumbnail,comp->isMPCobject);
    return OMX_ErrorNone;
}
EXPORT_C  OMX_ERRORTYPE ThumbNailMPEG4DecFactoryMethodHost(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "=> ThumbNailMPEG4DecFactoryMethodHost() in MPEG4Dec_Proxy");

    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_FALSE;
	comp->sorenson_flag=OMX_FALSE;
	comp->thumbnail=OMX_TRUE;
	RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt2(TRACE_API, "<= ThumbNailMPEG4DecFactoryMethodHost() in MPEG4Dec_Proxy thumbnail =%d isMPCobject =%d", comp->thumbnail, comp->isMPCobject);
    return OMX_ErrorNone;
}
// Mandatory to register the component
EXPORT_C  OMX_ERRORTYPE MPEG4DecFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "=> MPEG4DecFactoryMethod() in MPEG4Dec_Proxy");

    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_TRUE;
	comp->sorenson_flag=OMX_FALSE;
	RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt0(TRACE_API, "<= MPEG4DecFactoryMethod() in MPEG4Dec_Proxy");

    return OMX_ErrorNone;
}

// Mandatory to register the component
EXPORT_C  OMX_ERRORTYPE SORENSONFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "=> SORENSONFactoryMethod() in MPEG4Dec_Proxy");

    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_TRUE;
	comp->sorenson_flag=OMX_TRUE;
    RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt0(TRACE_API, "<= SORENSONFactoryMethod() in MPEG4Dec_Proxy");

    return OMX_ErrorNone;
}

EXPORT_C  OMX_ERRORTYPE SORENSONFactoryMethodHOST(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "=> SORENSONFactoryMethodHOST() in MPEG4Dec_Proxy");

    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_FALSE;
	comp->sorenson_flag=OMX_TRUE;
    RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt0(TRACE_API, "<= SORENSONFactoryMethodHOST() in MPEG4Dec_Proxy");

    return OMX_ErrorNone;
}

// Mandatory to register the component
EXPORT_C  OMX_ERRORTYPE MPEG4DecFactoryMethodHOST(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "=> MPEG4DecFactoryMethodHOST() in MPEG4Dec_Proxy");

    OMX_ERRORTYPE error;
    MPEG4Dec_Proxy *comp = new MPEG4Dec_Proxy;
	comp->isMPCobject=OMX_FALSE;
	comp->sorenson_flag=OMX_FALSE;
	RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
	*ppENSComponent = comp;

	OstTraceInt0(TRACE_API, "<= MPEG4DecFactoryMethodHOST() in MPEG4Dec_Proxy");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceInt1(TRACE_API, "=> MPEG4Dec_Proxy::setParameter() nParamIndex = 0x%x", nParamIndex);

    switch ((int)nParamIndex) {

		case OMX_IndexParamVideoPortFormat:
			{
			OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
			if(pt_video->nPortIndex > getPortCount())
				return OMX_ErrorNoMore;
			RETURN_OMX_ERROR_IF_ERROR(((VFM_MPEG4Dec_Port*)getPort(pt_video->nPortIndex))->setIndexParamVideoPortFormat((OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt));
			}
			break;

		case OMX_IndexParamVideoErrorCorrection:
			break;
		case OMX_IndexParamPixelAspectRatio:
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.setIndexParamPixelAspectRatio(pt));
			break;
		case OMX_IndexParamColorPrimary:
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.setIndexParamColorPrimary(pt));
			break;
		case OMX_IndexParamVideoMpeg4:
			break;
        //FIXME only for conformance tests, should be in ENS
		case OMX_IndexParamPriorityMgmt:
			break;
        case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.setIndexParamResourceSvaMcps(pt));
			break;
		//case VFM_IndexParamSupportedExtension:
		   // RETURN_OMX_ERROR_IF_ERROR(processingComponent.setIndexParamSupportedExtension(pt));
		   // break;
	case OMX_IndexParamCommonDeblocking:
		{
            OMX_PARAM_DEBLOCKINGTYPE *pt_video = (OMX_PARAM_DEBLOCKINGTYPE*)pt;
			//+ER344943
				processingComponent.SetDeblockingFlag(pt_video->bDeblocking);
			//-ER344943
		}
		break;
	case VFM_IndexParamThumbnailGeneration:
        RETURN_OMX_ERROR_IF_ERROR(processingComponent.setIndexParamthumbnailgeneration(pt));
		RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
			break;
        default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
			break;
    }

	OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::setParameter()");

    return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Dec_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceInt1(TRACE_API, "=> MPEG4Dec_Proxy::getParameter() nParamIndex = 0x%x", nParamIndex);

    switch ((int)nParamIndex) {

		case OMX_IndexParamVideoPortFormat:
			{
			OMX_VIDEO_PARAM_PORTFORMATTYPE *pt_video = (OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt;
			if(pt_video->nPortIndex > getPortCount())
				return OMX_ErrorNoMore;
			RETURN_OMX_ERROR_IF_ERROR(((VFM_MPEG4Dec_Port*)getPort(pt_video->nPortIndex))->getIndexParamVideoPortFormat((OMX_VIDEO_PARAM_PORTFORMATTYPE*)pt));
			}
			break;
		case OMX_IndexParamVideoErrorCorrection:
			break;
		case OMX_IndexParamPixelAspectRatio:
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.getIndexParamPixelAspectRatio(pt));
			break;
		case OMX_IndexParamColorPrimary:
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.getIndexParamColorPrimary(pt));
			break;
		case OMX_IndexParamVideoMpeg4:
			break;
	case OMX_IndexParamCommonDeblocking:
			{
                OMX_PARAM_DEBLOCKINGTYPE *pt_video = (OMX_PARAM_DEBLOCKINGTYPE*)pt;
				pt_video->bDeblocking =processingComponent.GetDeblockingFlag();
			}
			break;
		//FIXME only for conformance tests, should be in ENS
		case OMX_IndexParamPriorityMgmt:
			break;
#ifdef PACKET_VIDEO_SUPPORT
        case PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX:   //what is this
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
			RETURN_OMX_ERROR_IF_ERROR(processingComponent.getIndexParamResourceSvaMcps(pt));
			break;
		//case VFM_IndexParamSupportedExtension:
			//RETURN_OMX_ERROR_IF_ERROR(processingComponent.getIndexParamSupportedExtension(pt));
       // break;
		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
			break;
    }

	OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::getParameter()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceInt1(TRACE_API, "=> MPEG4Dec_Proxy::setConfig() nParamIndex = 0x%x", nParamIndex);

    switch (nParamIndex)
	{
		case OMX_IndexConfigVideoMBErrorReporting:
			{
				OMX_CONFIG_MBERRORREPORTINGTYPE *pt_video = (OMX_CONFIG_MBERRORREPORTINGTYPE*)pt;
				processingComponent.SetIndex_macroblock_error_reporting(pt_video->bEnabled);
			}
			break;
		case OMX_IndexParamVideoFastUpdate:
			{
				OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *pt_video = (OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *)pt;
				processingComponent.SetIndexParamVideoFastUpdate(pt_video->bEnableVFU);
			}
			break;
       default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
			break;
    }

	OstTraceInt0(TRACE_API, "<= MPEG4Dec_Proxy::setConfig()");

	return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Dec_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst1(TRACE_API, "=> MPEG4Dec_Proxy::getConfig() nParamIndex = 0x%x", nParamIndex);

    switch (nParamIndex)
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
		case OMX_IndexParamVideoFastUpdate:
			{
				processingComponent.GetIndexParamVideoFastUpdate(pt);
			}
        break;
		//+CR369593
		case OMX_IndexConfigCommonOutputCrop:
		{
				OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;

				pt_crop->nLeft 		= 0;
				pt_crop->nTop 		= 0;
				pt_crop->nWidth 	= mParam.CropWidth;
				pt_crop->nHeight 	= mParam.CropHeight;

				OstTraceFiltInst4(TRACE_API, "MPEG4Dec_Proxy::getConfig() (OMX_IndexConfigCommonOutputCrop) Returning crop rectangle: nLeft = %d, nTop = %d, nWidth = %d, nHeight = %d", pt_crop->nLeft, pt_crop->nTop, pt_crop->nWidth, pt_crop->nHeight);
		}
		break;
		//-CR369593
		default:
			RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
			break;
    }

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Dec_Proxy::getConfig()");

    return OMX_ErrorNone;
}
//jitender:: fix this
void MPEG4Dec_Proxy::setProfileLevelSupported()
{
   OstTraceInt0(TRACE_API, "<=> MPEG4Dec_Proxy::setProfileLevelSupported()");
   setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4Level5);
   setOneProfileLevelSupported(OMX_VIDEO_MPEG4ProfileAdvancedSimple,OMX_VIDEO_MPEG4Level5);
}

OMX_ERRORTYPE MPEG4Dec_Proxy::updateOutputPortSettings(OMX_INDEXTYPE nParamIndex,t_uint32 width,t_uint32 height)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Dec_Proxy::updateOutputPortSettings()");

    OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
	OMX_PARAM_PORTDEFINITIONTYPE inputPortDef;
	inputPortDef.nPortIndex = 1;
	outPortDef.nPortIndex = 1;

	inputPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	getOmxIlSpecVersion(&inputPortDef.nVersion);
	err = ((VFM_MPEG4Dec_Port*)getPort(0))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&inputPortDef);
	if(err != OMX_ErrorNone) return err;

	inputPortDef.format.video.nFrameHeight = height;
	inputPortDef.format.video.nFrameWidth =width;
	//+reslolution for ER 335883
	err = ((VFM_MPEG4Dec_Port*)getPort(0))->setParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&inputPortDef);
	if(err != OMX_ErrorNone) return err;

	//err = ((VFM_MPEG4Dec_Port*)getPort(0))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&inputPortDef);
	//if(err != OMX_ErrorNone) return err;

   // outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
   // getOmxIlSpecVersion(&outPortDef.nVersion);
   // err = ((VFM_MPEG4Dec_Port*)getPort(1))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
   // if(err != OMX_ErrorNone) return err;
   //
   // outPortDef.format.video.nFrameHeight = height;
   // outPortDef.format.video.nFrameWidth =width;
   //
   // err = ((VFM_MPEG4Dec_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
   // if(err != OMX_ErrorNone) return err;
	//-reslolution for ER 335883

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Dec_Proxy::updateOutputPortSettings()");

    return OMX_ErrorNone;
}
RM_STATUS_E MPEG4Dec_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Dec_Proxy::getResourcesEstimation()");

	OMX_U32 cyclesperMB,frameRate;
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

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Dec_Proxy::getResourcesEstimation()");

    return RM_E_NONE;
}
void MPEG4Dec_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType,
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
			  if (isMPCobject && processingComponent.isNMFPanicSource(MPC_EE, serviceData->panic.info.mpc.faultingComponent)){
                  OstTraceInt0(TRACE_FLOW, "MPEG4Dec MPC generated Panic");
				  processingComponent.errorRecoveryDestroyAll();
				  eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
				  OstTraceInt1(TRACE_FLOW, "MPEG4Dec_Proxy::NmfPanicCallback serviceType =%d \n",serviceType);
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

