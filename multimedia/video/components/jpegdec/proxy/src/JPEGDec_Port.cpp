/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_jpegdec
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_proxy_src_JPEGDec_PortTraces.h"
#endif


#include "JPEGDec_Port.h"
#include "JPEGDec_Proxy.h"


#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { jpegdec_port_assert(_error, __LINE__, OMX_FALSE); return (_error); } }


void JPEGDec_Port::jpegdec_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECPort :In jpegdec_port_assert ");
    if (OMX_ErrorNone != omxError)
    {
		   OstTraceFiltInst2(TRACE_ERROR, "JPEGDECPort : errorType : 0x%x error line no %d\n", omxError,line);
        //NMF_LOG("JPEGDECPort : errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGDec_Port::JPEGDec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
			  :VFM_Port(commonPortData, enscomp)
{
	if (commonPortData.mDirection == OMX_DirInput){
		// Settings for Input Port
		mParamPortDefinition.format.image.nFrameHeight = 0;
		mParamPortDefinition.format.image.nFrameWidth = 0;
		mParamPortDefinition.format.image.nStride = 0;
		mParamPortDefinition.format.image.pNativeRender = 0;
		mParamPortDefinition.format.image.pNativeWindow = 0;
        mParamPortDefinition.format.image.nSliceHeight = 0;
		mParamPortDefinition.format.image.bFlagErrorConcealment = OMX_FALSE;
		mParamPortDefinition.format.image.cMIMEType = 0;
		mParamPortDefinition.format.image.eColorFormat = OMX_COLOR_FormatUnused;
		mParamPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
		mParamPortDefinition.nBufferSize = 256; //any non-zero value
		mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
		mParamPortDefinition.nBufferAlignment = 16;
	}
	else{
		// Settings for Input Port
		mParamPortDefinition.format.image.nStride = 0;
		mParamPortDefinition.format.image.pNativeRender = 0;
		mParamPortDefinition.format.image.pNativeWindow = 0;
        mParamPortDefinition.format.image.nSliceHeight = 0;
		mParamPortDefinition.format.image.nFrameHeight = 0;
		mParamPortDefinition.format.image.nFrameWidth = 0;
        mParamPortDefinition.format.image.bFlagErrorConcealment = OMX_FALSE;
		mParamPortDefinition.format.image.cMIMEType = 0;
		mParamPortDefinition.format.image.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
		mParamPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
		mParamPortDefinition.nBufferSize = 256; //any non-zero value
		mParamPortDefinition.bBuffersContiguous = OMX_TRUE;
		mParamPortDefinition.nBufferAlignment = 256;
	}

}



OMX_ERRORTYPE JPEGDec_Port::setIndexParamImagePortFormat(OMX_IMAGE_PARAM_PORTFORMATTYPE *portDef){
	return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_Port::getIndexParamImagePortFormat(OMX_IMAGE_PARAM_PORTFORMATTYPE *portDef){
	return OMX_ErrorNone;
}

OMX_U32 JPEGDec_Port::getBufferSize() const
{
	OstTraceFiltInst0(TRACE_API, "JPEGDEC_PORT : getBufferSize \n");
    JPEGDec_Proxy *pProxyComponent = (JPEGDec_Proxy *)(&getENSComponent());
if (mParamPortDefinition.format.image.eColorFormat == OMX_COLOR_FormatUnused){    
   if(pProxyComponent->isMPCobject)
   {
	   OstTraceFiltInst1(TRACE_FLOW, "JPEGDEC_PORT : mpc returning BufferSize - %x\n",(((mParamPortDefinition.format.image.nFrameWidth+0xf)&0xffffff0)*((mParamPortDefinition.format.image.nFrameHeight+0xf)&0xfffffff0)*3)/2);
		return (((mParamPortDefinition.format.image.nFrameWidth+0xf)&0xffffff0)*
				((mParamPortDefinition.format.image.nFrameHeight+0xf)&0xfffffff0)*3)/2;
   }
   else{
	   OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PORT :not mpc returning BufferSize 2048 \n");
		return 2048;
		}
	}
	else{
    if(pProxyComponent->isMPCobject)
	{
		OstTraceFiltInst1(TRACE_FLOW, "JPEGDEC_PORT : mpc returning BufferSize %x \n",((mParamPortDefinition.format.image.nFrameWidth)*0xf*3)/2);
		return ((mParamPortDefinition.format.image.nFrameWidth)*0xf*3)/2;
	}
		
   else
	{
	   OstTraceFiltInst1(TRACE_FLOW, "JPEGDEC_PORT : not mpc returning BufferSize %x \n",(((mParamPortDefinition.format.image.nFrameWidth+0xf)&0xffffff0)*((mParamPortDefinition.format.image.nFrameHeight+0xf)&0xfffffff0)*3)/2);
		return (((mParamPortDefinition.format.image.nFrameWidth+0xf)&0xffffff0)*
				((mParamPortDefinition.format.image.nFrameHeight+0xf)&0xfffffff0)*3)/2;
	}
        
////minimum buffer size is set to width*16*1.5 for slice mode
	}
}


OMX_ERRORTYPE JPEGDec_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDEC_PORT : checkSetFormatInPortDefinition \n");
	OMX_IMAGE_PORTDEFINITIONTYPE *pt_image = (OMX_IMAGE_PORTDEFINITIONTYPE *)(&(portDef->format.image));
	RETURN_XXX_IF_WRONG_OST(1 <= pt_image->nFrameHeight && pt_image->nFrameHeight <= 8176, OMX_ErrorBadParameter);
	RETURN_XXX_IF_WRONG_OST(1 <= pt_image->nFrameWidth && pt_image->nFrameWidth <= 8176, OMX_ErrorBadParameter);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_Port::setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure)
{

	OMX_ERRORTYPE err = OMX_ErrorNone;

	OstTraceFiltInst0(TRACE_API, "JPEGDECPort : Inside setParameter \n");

	//JPEGDec_Proxy   *proxy_component = (JPEGDec_Proxy *)(&(getENSComponent()));
	err = ENS_Port::setParameter(nParamIndex,pComponentParameterStructure);

	jpegdec_port_assert(err, __LINE__, OMX_FALSE);
	return err;
}


void JPEGDec_Port::getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const
{
	OstTraceFiltInst2(TRACE_API, "JPEGDECPort : getSlavedSizeFromGivenMasterSize width : %d  height : %d \n",*nWidthSlave,*nHeightSlave);
   *nWidthSlave = mParamPortDefinition.format.image.nFrameWidth;
   *nHeightSlave = mParamPortDefinition.format.image.nFrameHeight;

   //cases when no cropping and no downscaling
   if ((*nWidthSlave ==0) &&  (*nHeightSlave==0))
   {
	OstTraceFiltInst0(TRACE_FLOW, "JPEGDECPort : getSlavedSizeFromGivenMasterSize ((*nWidthSlave ==0) &&  (*nHeightSlave==0)) ");
	   *nWidthSlave   = nWidthMaster;
	   *nHeightSlave  = nHeightMaster;
   }


}


