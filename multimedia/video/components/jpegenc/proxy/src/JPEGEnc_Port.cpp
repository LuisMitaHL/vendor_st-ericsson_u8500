/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


#define _CNAME_ Vfm_jpegenc
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegenc_proxy_src_JPEGEnc_PortTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE



#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { jpegenc_port_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { jpegenc_port_assert(_error, __LINE__, OMX_FALSE); return (_error); } }



#include "JPEGEnc_Port.h"

#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "jpegenc"
#endif
#include<cutils/log.h>
#endif

void JPEGEnc_Port::jpegenc_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "JPEGENCPort : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGEnc_Port::JPEGEnc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp, JPEGEnc_ParamAndConfig *mParam)
		: VFM_Port(commonPortData, enscomp)
	{
		//pProxyComponent = (JPEGEnc_Proxy *)(&getENSComponent());
		Param =mParam;
		mParamPortDefinition.format.image.cMIMEType = NULL;
		mParamPortDefinition.format.image.nSliceHeight = 16;
		mParamPortDefinition.format.image.nStride = 1;

		mParamPortDefinition.format.image.nFrameWidth = 640;
	    mParamPortDefinition.format.image.nFrameHeight = 480;
  		mParamPortDefinition.format.image.bFlagErrorConcealment = OMX_FALSE;
		if(commonPortData.mPortIndex==1)//Output Port
		{
			mParamPortDefinition.format.image.eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingJPEG;
			mParamPortDefinition.format.image.eColorFormat = (OMX_COLOR_FORMATTYPE)0;
		}
		else //Input Port
		{
			mParamPortDefinition.format.image.eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
			mParamPortDefinition.format.image.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
		}
  		mParamPortDefinition.format.image.pNativeWindow = 0;
		if(commonPortData.mPortIndex==0)
			mParamPortDefinition.nBufferSize = getSizeFrame((OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar,mParamPortDefinition.format.image.nFrameWidth,mParamPortDefinition.format.image.nFrameHeight);
		else
			mParamPortDefinition.nBufferSize = (mParamPortDefinition.format.image.nFrameWidth*mParamPortDefinition.format.image.nFrameHeight*40*3)/(100*2); //40%(Acc to Statics)
  		OstTraceInt1(TRACE_API, "JPEGENCPort : Port Construction line no %d \n", __LINE__);

	}


OMX_ERRORTYPE JPEGEnc_Port::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
	{
		OstTraceFiltInst1(TRACE_API, "In JPEGEnc_Port : In setFormatInPortDefinition <line no %d> ",__LINE__);
		VFM_Component   *vfm_component = (VFM_Component *)(&(getENSComponent()));
		JPEGEnc_Proxy *pProxyComponent = (JPEGEnc_Proxy *)(&getENSComponent());

		RETURN_OMX_ERROR_IF_ERROR_OST(checkSetFormatInPortDefinition(&portDef));

		if (portDef.format.image.nStride <= 0)
		{
			jpegenc_port_assert(OMX_ErrorBadParameter,__LINE__,OMX_TRUE);
			return OMX_ErrorBadParameter;
		}

		{
			OMX_U32 nSliceHeight = mParamPortDefinition.format.image.nSliceHeight;
			mParamPortDefinition.format.image = portDef.format.image;
			mParamPortDefinition.format.image.nSliceHeight = nSliceHeight;
		}

		if (mParamPortDefinition.nPortIndex == 0) //for the input Port
		{
			if (pProxyComponent->isMPCobject)
			{
				RETURN_XXX_IF_WRONG_OST((t_uint32)mParamPortDefinition.format.image.eColorFormat == (t_uint32)OMX_COLOR_FormatYUV420MBPackedSemiPlanar || (t_uint32)mParamPortDefinition.format.image.eColorFormat == (t_uint32)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar, OMX_ErrorBadParameter);
			}
			else
			{
				if (pProxyComponent->isHVABased)
				{
									RETURN_XXX_IF_WRONG_OST(mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatCbYCrY ||
									                    mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatYUV420SemiPlanar,OMX_ErrorBadParameter);
				}
				else
				{
				RETURN_XXX_IF_WRONG_OST(mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatYUV420Planar ||
				                    mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatMonochrome   ||
				                    mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatCbYCrY ||
				                    mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatYUV422Planar,OMX_ErrorBadParameter);
				}
			}
		}

		if(mParamPortDefinition.format.image.eColorFormat==OMX_COLOR_FormatYUV422Planar) //nslicheight changed to 8 for 422 images
		{
				mParamPortDefinition.format.image.nSliceHeight = 8;
		}



		if (mParamPortDefinition.nPortIndex == 1) //for the output Port
		{
			RETURN_XXX_IF_WRONG_OST(mParamPortDefinition.format.image.eColorFormat == OMX_COLOR_FormatUnused,OMX_ErrorBadParameter);
			RETURN_XXX_IF_WRONG_OST(mParamPortDefinition.format.image.eCompressionFormat==OMX_IMAGE_CodingJPEG || mParamPortDefinition.format.image.eCompressionFormat==OMX_IMAGE_CodingEXIF,OMX_ErrorBadParameter);
			Param->setCompressionType(mParamPortDefinition.format.image.eCompressionFormat);
		}


        if (mParamPortDefinition.format.image.eColorFormat == OMX_COLOR_FormatUnused)
		{
			mParamPortDefinition.nBufferSize = (mParamPortDefinition.format.image.nFrameWidth*mParamPortDefinition.format.image.nFrameHeight*3)/2;
			if (!(pProxyComponent->isHVABased)&&(pProxyComponent->isMPCobject))//input size acc. to camera SAS v1.3
			{
				/* + change for ER 346056 */
				OMX_U32 noRefMBs = (176 * 144)/256;
				OMX_U32 computeMBs = (mParamPortDefinition.format.image.nFrameWidth/16)*(mParamPortDefinition.format.image.nFrameHeight/16);
				/* - change for ER 346056 */
				if (computeMBs <= noRefMBs) //change for ER 346056
					mParamPortDefinition.nBufferSize = (mParamPortDefinition.format.image.nFrameWidth*mParamPortDefinition.format.image.nFrameHeight*50*3)/(100*2); //50%
				else
					mParamPortDefinition.nBufferSize = (mParamPortDefinition.format.image.nFrameWidth*mParamPortDefinition.format.image.nFrameHeight*40*3)/(100*2); //40%
			}

			if (mParamPortDefinition.format.image.eCompressionFormat==OMX_IMAGE_CodingEXIF)
			{
				OMX_U32 tempBufferSize;
				//for the output port
				mParamPortDefinition.nBufferSize += (64*1024); //extra 64Kb required for output buffer size

				JPEGEnc_Port *tempPort = (JPEGEnc_Port*)(getENSComponent().getPort(IPB+0));
				tempBufferSize = tempPort->getBufferSize();
				tempBufferSize += (64*1024); //extra 64Kb required for input buffer size
				tempPort->setBufferSize(tempBufferSize);
			}
		}
		else
		{
			mParamPortDefinition.format.image.nStride = getSizeFrame(mParamPortDefinition.format.image.eColorFormat, mParamPortDefinition.format.image.nFrameWidth, mParamPortDefinition.format.image.nFrameHeight)/((mParamPortDefinition.format.image.nFrameHeight+0xf)&(~0xf));
			mParamPortDefinition.nBufferSize = mParamPortDefinition.format.image.nStride*mParamPortDefinition.format.image.nSliceHeight; // openmax IL spec 1.1.1 : see 4.2.2 (page 198)
		}

		//to be used in setting input parameters
		if (mParamPortDefinition.nPortIndex == 0)
		{
			//processingComponent.inputBufferFormat = (OMX_COLOR_FORMATTYPE)mParamPortDefinition.format.image.eColorFormat;
			Param->setColorType(mParamPortDefinition.format.image.eColorFormat);
		}

		RETURN_OMX_ERROR_IF_ERROR_OST(vfm_component->reset());

        /* + change for CR 417003, to resolve crash in encoders when setprop for recycling delay is added */
        if(portDef.nPortIndex ==1)
        {
			vfm_component->pParam->getRecyclingDelay()->set(0);
        }
        /* - change for CR 417003 */

		OstTraceFiltInst1(TRACE_API, "In JPEGEnc_Port : setFormatInPortDefinition completes <line no %d> ",__LINE__);
		return OMX_ErrorNone;
	}


OMX_U32 JPEGEnc_Port::getBufferSize() const
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In getBufferSize <line no %d> ",__LINE__);
		return 0;
	}

OMX_ERRORTYPE JPEGEnc_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In checkSetFormatInPortDefinition <line no %d> ",__LINE__);
		OMX_IMAGE_PORTDEFINITIONTYPE *pt_image = (OMX_IMAGE_PORTDEFINITIONTYPE *)(&(portDef->format.image));
		RETURN_XXX_IF_WRONG_OST(1 <= pt_image->nFrameHeight && pt_image->nFrameHeight <= 8176, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG_OST(1 <= pt_image->nFrameWidth && pt_image->nFrameWidth <= 8176, OMX_ErrorBadParameter);
		return OMX_ErrorNone;
	}

OMX_S32 JPEGEnc_Port::getStride()
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In getStride <line no %d> ",__LINE__);
		OMX_S32 xx = getSizeFrame(mParamPortDefinition.format.image.eColorFormat, mParamPortDefinition.format.image.nFrameWidth, mParamPortDefinition.format.image.nFrameHeight)/((mParamPortDefinition.format.image.nFrameHeight+0xf)&(~0xf));
		return xx;

	}

OMX_U32 JPEGEnc_Port::getSliceHeight()
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In getSliceHeight <line no %d> ",__LINE__);
		return mParamPortDefinition.format.image.nSliceHeight;
	}

OMX_U32 JPEGEnc_Port::getBufferSize()
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In getBufferSize <line no %d> ",__LINE__);
		return mParamPortDefinition.nBufferSize;
	}

void JPEGEnc_Port::setBufferSize(OMX_U32 value)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In setBufferSize <line no %d> ",__LINE__);
		mParamPortDefinition.nBufferSize = value;
	}


void JPEGEnc_Port::getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_Port : In getSlavedSizeFromGivenMasterSize <line no %d> ",__LINE__);
   *nWidthSlave = mParamPortDefinition.format.image.nFrameWidth;
   *nHeightSlave = mParamPortDefinition.format.image.nFrameHeight;

   //cases when no cropping and no downscaling
   if ((*nWidthSlave ==0) &&  (*nHeightSlave==0))
   {
	   OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_Port : In getSlavedSizeFromGivenMasterSize Setting output port resolution <line no %d> ",__LINE__);
	   *nWidthSlave   = nWidthMaster;
	   *nHeightSlave  = nHeightMaster;
   }
}
