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
	#include "video_components_jpegenc_proxy_src_JPEGEnc_ProxyTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#include "JPEGEnc_factory.h"
#include "JPEGEnc_Proxy.h"
#include "JPEGEnc_Port.h"

#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "jpegenc"
#endif
#include<cutils/log.h>
#endif

#define ARMNMF_OBJECT 1
#define MPCNMF_OBJECT 2

void JPEGEnc_Proxy::jpegenc_proxy_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "JPEGENCProxy : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}

OMX_S32 JPEGEnc_Proxy::getStride()
{
    return ((JPEGEnc_Port*)getPort(IPB+0))->getStride();
}

OMX_U32 JPEGEnc_Proxy::getSliceHeight()
{
    return ((JPEGEnc_Port*)getPort(IPB+0))->getSliceHeight();
}

JPEGEnc_Proxy::JPEGEnc_Proxy():VFM_Component(&mParam),mParam(this)
{
	OstTraceInt1(TRACE_API, "In JPEGEnc_Proxy : In Contructor <line no %d> ",__LINE__);

        isHVABased = OMX_FALSE;

	sva_mcps = 0;

	#ifdef FULL_POWER_MANAGEMENT
		is_full_powermgt = OMX_TRUE;
	#else
		is_full_powermgt = OMX_FALSE;
	#endif

}

OMX_ERRORTYPE JPEGEnc_Proxy::construct()
{
	OstTraceInt1(TRACE_API, "In JPEGEnc_Proxy : In contruct <line no %d> ",__LINE__);
	RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(2));
#ifdef HOST_MPC_COMPONENT_BOTH
	if (isMPCobject)
	{
        setProcessingComponent(mpc_processingComponent);
        RETURN_OMX_ERROR_IF_ERROR(mpc_processingComponent->setPortCount(2));
	}
	else
#endif
	{
		setProcessingComponent(arm_processingComponent);
		RETURN_OMX_ERROR_IF_ERROR(arm_processingComponent->setPortCount(2));
	}

#ifdef HOST_MPC_COMPONENT_BOTH
     if (isMPCobject)
     {
	      mpc_processingComponent->setPriorityLevel((t_uint32)NMF_SCHED_URGENT);
     }
#endif

	 // setTraceInfo must be called after VFM_Component::construct (only for ParamAndConfig)
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

	// port creation and common initialization
	//RETURN_OMX_ERROR_IF_ERROR(createPort(IPB+0, OMX_DirInput, OMX_BufferSupplyOutput, 1, 0, OMX_PortDomainImage));
	ENS_Port * portInput= newPort(EnsCommonPortData(IPB+0,OMX_DirInput,1,0,OMX_PortDomainImage,OMX_BufferSupplyInput), *this);
	if (portInput == NULL)
	{
		OstTraceInt1(TRACE_ERROR, "In JPEGEnc_Proxy : OMX_ErrorInsufficientResources While creating input port  <line no %d> ",__LINE__);
		jpegenc_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
		return OMX_ErrorInsufficientResources;
	}

	//RETURN_OMX_ERROR_IF_ERROR(createPort(IPB+1, OMX_DirOutput, OMX_BufferSupplyOutput, 1, 0, OMX_PortDomainImage));
	ENS_Port * portOutput= newPort(EnsCommonPortData(IPB+1,OMX_DirOutput,1,0,OMX_PortDomainImage,OMX_BufferSupplyOutput), *this);
	if (portOutput == NULL)
	{
		delete portInput;
		OstTraceInt1(TRACE_ERROR, "In JPEGEnc_Proxy : OMX_ErrorInsufficientResources While creating output port <line no %d> ",__LINE__);
		jpegenc_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
		return OMX_ErrorInsufficientResources;
	}
    addPort(portInput);
	addPort(portOutput);
    setFrameSize(IPB+0, 640, 480);
    setFrameSize(IPB+1, 640, 480);

    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : Component construction done line no : %d \n", __LINE__);

	return OMX_ErrorNone;
}


JPEGEnc_Proxy::~JPEGEnc_Proxy()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_Proxy : In Destructor <line no %d> ",__LINE__);
    setProcessingComponent(0);
    if (isMPCobject)
    {
#ifdef HOST_MPC_COMPONENT_BOTH
		delete mpc_processingComponent;
#endif
	}
	else
	{
    	delete arm_processingComponent;
	}

	OstTraceFiltInst1(TRACE_API,"JPEGEnc_Proxy : Component destruction done line no : %d \n", __LINE__);
}


VFM_Port *JPEGEnc_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In newPort() line no : %d \n", __LINE__);
	return new JPEGEnc_Port(commonPortData, enscomp,&mParam);
}


OMX_ERRORTYPE JPEGEnc_Proxy::JPEGEnc_Processing(OMX_U32 value)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In JPEGEnc_Processing line no : %d \n", __LINE__);
	OMX_ERRORTYPE omxError = OMX_ErrorUndefined;

#ifdef HOST_MPC_COMPONENT_BOTH
    if(value == MPCNMF_OBJECT)
    {
	mSendParamToMpc.set();
	isMPCobject = OMX_TRUE;
    }
#endif
    if (value == ARMNMF_OBJECT)
    {
	mSendParamToARMNMF.set();
	isMPCobject = OMX_FALSE;
    }

    if (ARMNMF_OBJECT == value)
    {
	isMPCobject = OMX_FALSE;
	arm_processingComponent = new JPEGEnc_ArmNmfProcessingComponent(*this);
	if (NULL == arm_processingComponent)
	{
	    OstTraceInt1(TRACE_ERROR, "In JPEGEnc_Proxy : OMX_ErrorInsufficientResources While creating JPEGEnc_ArmNmfProcessingComponent <line no %d> ",__LINE__);
	    jpegenc_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
	    return OMX_ErrorInsufficientResources;
	}

#ifdef HOST_MPC_COMPONENT_BOTH
		mpc_processingComponent = NULL;
#endif
		omxError = OMX_ErrorNone;
	}
#ifdef HOST_MPC_COMPONENT_BOTH
	if (MPCNMF_OBJECT == value)
	{
		isMPCobject = OMX_TRUE;
		mpc_processingComponent = new JPEGEnc_NmfMpc_ProcessingComponent(*this);
		if (NULL == mpc_processingComponent)
		{
			OstTraceInt1(TRACE_ERROR, "In JPEGEnc_Proxy : OMX_ErrorInsufficientResources While creating JPEGEnc_NmfMpc_ProcessingComponent <line no %d> ",__LINE__);
			jpegenc_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			return OMX_ErrorInsufficientResources;
		}

		arm_processingComponent = NULL;
		omxError = OMX_ErrorNone;
	}
#endif
	return omxError;
}

// Mandatory to register the component
OMX_ERRORTYPE JPEGEncFactoryMethodDefault(ENS_Component_p * ppENSComponent,OMX_U32 value)
{
    OstTraceInt2(TRACE_API,"In JPEGEnc_Proxy : In JPEGEncFactoryMethodDefault Value : %d line no : %d \n",value, __LINE__);
    OMX_ERRORTYPE error;
    JPEGEnc_Proxy *comp = new JPEGEnc_Proxy;
    RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);

    error = comp->JPEGEnc_Processing(value);
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}

	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
		delete comp;
		return error;
	}
    *ppENSComponent = comp;
    OstTraceInt2(TRACE_API,"In JPEGEnc_Proxy : JPEGEncFactoryMethodDefault DONE value : %d line no : %d \n",value, __LINE__);
    return OMX_ErrorNone;
}



// Mandatory to register the component - default for MPC
EXPORT_C OMX_ERRORTYPE JPEGEncFactoryMethod(ENS_Component_p * ppENSComponent)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In JPEGEncFactoryMethod line no : %d \n", __LINE__);
#ifdef HOST_MPC_COMPONENT_BOTH
	return JPEGEncFactoryMethodDefault(ppENSComponent,MPCNMF_OBJECT);
#else
	return OMX_ErrorInvalidComponent;
#endif
}


// Mandatory to register the component - for MPC SVA based encoder
EXPORT_C OMX_ERRORTYPE JPEGEncFactoryMethodMPC(ENS_Component_p * ppENSComponent)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In JPEGEncFactoryMethodMPC line no : %d \n", __LINE__);
#ifdef HOST_MPC_COMPONENT_BOTH
	return JPEGEncFactoryMethodDefault(ppENSComponent,MPCNMF_OBJECT);
#else
	return OMX_ErrorInvalidComponent;
#endif
}

// Mandatory to register the component - for SW Encoder
EXPORT_C OMX_ERRORTYPE JPEGEncFactoryMethodHVA(ENS_Component_p * ppENSComponent)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In JPEGEncFactoryMethodHVA line no : %d \n", __LINE__);
#ifdef HVA_JPEGENC
	OMX_ERRORTYPE error_type;
	error_type = JPEGEncFactoryMethodDefault(ppENSComponent,ARMNMF_OBJECT);
	if (OMX_ErrorNone == error_type)
	{
		JPEGEnc_Proxy *pProxyComponent = (JPEGEnc_Proxy *)(*ppENSComponent);
		pProxyComponent->isHVABased = OMX_TRUE;
	}
    OstTraceInt1(TRACE_ERROR,"In JPEGEnc_Proxy : In JPEGEncFactoryMethodHVA Error at line no : %d \n", __LINE__);
	return error_type;
#else
	return OMX_ErrorInvalidComponent;
#endif
}


// Mandatory to register the component - for SW Encoder
EXPORT_C OMX_ERRORTYPE JPEGEncFactoryMethodHOST(ENS_Component_p * ppENSComponent)
{
    OstTraceInt1(TRACE_API,"In JPEGEnc_Proxy : In JPEGEncFactoryMethodHOST line no : %d \n", __LINE__);
	return JPEGEncFactoryMethodDefault(ppENSComponent,ARMNMF_OBJECT);
}


OMX_ERRORTYPE JPEGEnc_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : In setParameter IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
    OMX_BOOL has_changed;
    switch ((t_uint32)nParamIndex) {
	case OMX_IndexParamQFactor:
	case OMX_IndexParamQuantizationTable:
    case OMX_IndexParamHuffmanTable:
        RETURN_OMX_ERROR_IF_ERROR(setConfig(nParamIndex, pt));
        break;

    case OMX_IndexParamImagePortFormat:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamImagePortFormat(pt, &has_changed));
        break;

	case VFM_IndexParamResourceEstimation:
		RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamResourceSvaMcps(pt));
		break;

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setParameter(nParamIndex, pt));
        break;
    }

	OstTraceFiltInst1(TRACE_API,"JPEGEnc_Proxy : Component SetParam done line no : %d \n", __LINE__);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : In getParameter IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
    switch ((t_uint32)nParamIndex) {
	case OMX_IndexParamQFactor:
	case OMX_IndexParamQuantizationTable:
    case OMX_IndexParamHuffmanTable:
        RETURN_OMX_ERROR_IF_ERROR(getConfig(nParamIndex, pt));
        break;

    case OMX_IndexParamImagePortFormat:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamImagePortFormat(pt));
        break;

	case VFM_IndexParamResourceEstimation:
			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamResourceSvaMcps(pt));
			break;

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : getParameter DONE IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceFiltInst1(TRACE_API,"JPEGEnc_Proxy : SetConfig done line no : %d \n", __LINE__);
	if(OMX_ErrorNone!=setConfigInternal(nParamIndex, pt))
	{
		RETURN_OMX_ERROR_IF_ERROR(VFM_Component::setConfig(nParamIndex, pt));
	}

	OstTraceFiltInst1(TRACE_API,"JPEGEnc_Proxy : SetConfig done line no : %d \n", __LINE__);
	return OMX_ErrorNone;
}


// TODO: not a standard component as index OMX_IndexParamQuantizationTable is not supported
OMX_ERRORTYPE JPEGEnc_Proxy::setConfigInternal(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : In setConfigInternal IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
	/* ER 449942 */
    OMX_BOOL has_changed = OMX_FALSE;
	/* ER 449942 */
   // OMX_ERRORTYPE err;
    //OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
    //JPEGEnc_Port *port = ( JPEGEnc_Port *)(this->getPort(1));
	//outPortDef.nPortIndex = 1;

    switch ((t_uint32)nParamIndex) {
	case OMX_IndexParamQuantizationTable:
		RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamQuantizationTable(pt, &has_changed));
		mParam.HeaderToGenerate.update(has_changed);
        mParam.QuantToCompute.update(has_changed);
        mParam.isOptimizeQuantTableEnable = OMX_FALSE;
		break;
	case OMX_IndexParamHuffmanTable:
		RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamHuffmanTable(pt, &has_changed));
		mParam.HeaderToGenerate.update(has_changed);
        break;
    case OMX_IndexParamQFactor:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamQFactor(pt, &(mParam.QFactor), &has_changed));
      /*  if(has_changed)
		{
			port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
			err = ((JPEGEnc_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
				if(err != OMX_ErrorNone) return err;
		} */
        mParam.HeaderToGenerate.update(has_changed);
        mParam.QuantToCompute.update(has_changed);
        mParam.isOptimizeQuantTableEnable = OMX_FALSE;
	OstTraceFiltInst2(TRACE_FLOW,"In JPEGEnc_Proxy : In setConfigInternal mParam.QFactor : %d  line no : %d \n",mParam.QFactor, __LINE__);
		//updating the min buffer size requirement
		//assuming that it is calculated on the bases of 50% qualityfactor
		//it is currently valid  for MPC and SW component
		if (!(isHVABased))
		{
			/* + change for ER 346056 */
			OMX_U32 noRefMBs = (176 * 144)/256;
			OMX_U32 computeMBs;
			/* - change for ER 346056 */

			OMX_U32 width,height,tempBufferSize;
			OMX_IMAGE_CODINGTYPE compressionType;
			JPEGEnc_Port *tempPort;
			OMX_U32 quality_factor = mParam.QFactor;

			width = getFrameWidth(1);
			height = getFrameHeight(1);
			/* + change for ER 346056 */
			computeMBs = (width/16)*(height/16);
			/* - change for ER 346056 */
			tempPort = (JPEGEnc_Port*)(this->getPort(IPB+1));
			compressionType = (OMX_IMAGE_CODINGTYPE)tempPort->getCompressionFormat();
			tempBufferSize = (width * height * 3)/2;
			if(isMPCobject)
			{
			    if (computeMBs < noRefMBs)  //change for ER 346056
			    {
				    if(quality_factor <=20) { tempBufferSize = (tempBufferSize * 25)/100; }
				    else if(quality_factor > 20 && quality_factor <=40) { tempBufferSize = (tempBufferSize * 45)/100;  }
				    else if(quality_factor > 40 && quality_factor <=60) { tempBufferSize = (tempBufferSize * 50)/100;  }
				    else if(quality_factor > 60 && quality_factor <=80) { tempBufferSize = (tempBufferSize * 75)/100;  }
				    else { tempBufferSize = (tempBufferSize * 120)/100; }
			    }
			    else if (computeMBs == noRefMBs) //change for ER 346056
			    {
				    if(quality_factor <=20) { tempBufferSize = (tempBufferSize * 20)/100; }
				    else if(quality_factor > 20 && quality_factor <=40) { tempBufferSize = (tempBufferSize * 30)/100;  }
				    else if(quality_factor > 40 && quality_factor <=60) { tempBufferSize = (tempBufferSize * 50)/100;  }
				    else if(quality_factor > 60 && quality_factor <=80) { tempBufferSize = (tempBufferSize * 60)/100;  }
				    else { tempBufferSize = (tempBufferSize * 100)/100; }//ER 359776
			    }
			    else
			    {
				    if(quality_factor <=20) { tempBufferSize = (tempBufferSize * 10)/100;  }
				    else if(quality_factor > 20 && quality_factor <=40) { tempBufferSize = (tempBufferSize * 20)/100;  }
				    else if(quality_factor > 40 && quality_factor <=60) { tempBufferSize = (tempBufferSize * 40)/100;  }
				    else if(quality_factor > 60 && quality_factor <=80) { tempBufferSize = (tempBufferSize * 50)/100;  }
				    else { tempBufferSize = (tempBufferSize * 100)/100;  }//ER 359776
			    }
			}
			else
			{
			    if((computeMBs < noRefMBs)&&(quality_factor>80))
				   tempBufferSize = (tempBufferSize * 120)/100;
			}

			if (compressionType==OMX_IMAGE_CodingEXIF)
			{
				tempBufferSize += (64*1024); //extra 64Kb required for output buffer size
			}
			tempPort->setBufferSize(tempBufferSize);
		}
        break;

	case OMX_IndexConfigCommonOutputCrop:
		{
			OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;
		    OstTraceFiltInst3(TRACE_FLOW,"JPEGENCProxy Component SetConfigInternal OMX_IndexConfigCommonOutputCrop Cropped width : %d and Cropped Height %d Line : %d \n",pt_crop->nWidth,pt_crop->nHeight,__LINE__);
		    RETURN_XXX_IF_WRONG(0<=pt_crop->nLeft && (OMX_U32)pt_crop->nLeft<=getFrameWidth(0), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG(0<=pt_crop->nTop && (OMX_U32)pt_crop->nTop<=getFrameHeight(0), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG(0<=pt_crop->nLeft+(t_sint32)pt_crop->nWidth && (pt_crop->nLeft+pt_crop->nWidth<=getFrameWidth(0)), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG(0<=pt_crop->nTop+(t_sint32)pt_crop->nHeight && pt_crop->nTop+pt_crop->nHeight<=getFrameHeight(0), OMX_ErrorBadParameter);

			has_changed = (OMX_BOOL)((mParam.cropping_parameter.nHeight != pt_crop->nHeight       ) ||
			  						 (mParam.cropping_parameter.nWidth  != pt_crop->nWidth	    ) ||
									 (mParam.cropping_parameter.nLeft   != (OMX_U32)pt_crop->nLeft) ||
									 (mParam.cropping_parameter.nTop    != (OMX_U32)pt_crop->nTop));


		    RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigCommonOutputCrop(pt));
		    mParam.cropping_parameter.nWidth 	= pt_crop->nWidth;
		    mParam.cropping_parameter.nHeight = pt_crop->nHeight;
		    mParam.cropping_parameter.nLeft = pt_crop->nLeft;
		    mParam.cropping_parameter.nTop = pt_crop->nTop;
		    setFrameHeight(IPB+1, mParam.cropping_parameter.nHeight);
		    setFrameWidth(IPB+1, mParam.cropping_parameter.nWidth);
		    mParam.HeaderToGenerate.set();
		}
        break;

    case OMX_IndexConfigCommonRotate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigCommonRotate(pt, &mParam.rotation_val, &has_changed));
        if (has_changed)
        {
			//swapping the value of height and width */
			OMX_U32 tempValue=0;
			OMX_U32 tempWidth = getFrameWidth(IPB+1);
			OMX_U32 tempHeight = getFrameHeight(IPB+1);
			tempValue = tempWidth;
			tempWidth = tempHeight;
			tempHeight = tempValue;
			setFrameHeight(IPB+1,tempHeight);
			setFrameWidth(IPB+1,tempWidth);
			tempWidth = getFrameWidth(IPB+1);
			tempHeight = getFrameHeight(IPB+1);
			OMX_U32 tempWidth0 = getFrameWidth(IPB+0);
			OMX_U32 tempHeight0 = getFrameHeight(IPB+0);
			tempValue = tempWidth0;
			tempWidth0 = tempHeight0;
			tempHeight0 = tempValue;
			setFrameHeight(IPB+0,tempHeight0 );
			setFrameWidth(IPB+0, tempWidth0);
			tempWidth0 = getFrameWidth(IPB+0);
			tempHeight0 = getFrameHeight(IPB+0);
			mParam.HeaderToGenerate.set();
			mSendParamToARMNMF.set();
			/* ER 449942 */
			//mParam.ConfigApplicable.set();
			/* ER 449942 */
			
			/* ER 439022 */
			mSendParamToMpc.set();
			/* ER 439022 */
       }
	/* ER 449942 */
	else
	{
		mParam.ConfigApplicable.reset();
	}
	/* ER 449942 */
        break;

    case VFM_IndexConfigTargetbpp:
        RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexConfigTargetbpp(pt, &mParam.targetBpp, &has_changed));
	/*	if(has_changed)
		{
				port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
				err = ((JPEGEnc_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
				if(err != OMX_ErrorNone) return err;

		}*/
        mParam.isOptimizeQuantTableEnable = OMX_TRUE;
        break;

	default:
	        return OMX_ErrorBadParameter;
    }

     mParam.ConfigApplicable.update(has_changed);

     OstTraceFiltInst1(TRACE_API,"JPEGENCProxy Component SetConfigInternal done line no : %d \n", __LINE__);

      return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : In getConfig IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
    switch ((t_uint32)nParamIndex) {
	case OMX_IndexParamQuantizationTable:
		RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamQuantizationTable(pt));
		break;
	case OMX_IndexParamHuffmanTable:
		RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamHuffmanTable(pt));
		break;
    case OMX_IndexParamQFactor:
        RETURN_XXX_IF_WRONG(mParam.isOptimizeQuantTableEnable==FALSE, OMX_ErrorUndefined);
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamQFactor(pt, mParam.QFactor));
        break;

    case OMX_IndexConfigCommonOutputCrop:
    {

    	OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;

		RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
		pt_crop->nHeight 	= mParam.cropping_parameter.nHeight;
		pt_crop->nWidth 	= mParam.cropping_parameter.nWidth;
		pt_crop->nLeft 		= mParam.cropping_parameter.nLeft;
		pt_crop->nTop 		= mParam.cropping_parameter.nTop;
	}
        break;

    case OMX_IndexConfigCommonRotate:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonRotate(pt, mParam.rotation_val));
        break;

    case VFM_IndexConfigTargetbpp:
        RETURN_XXX_IF_WRONG(mParam.isOptimizeQuantTableEnable==TRUE, OMX_ErrorUndefined);
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigTargetbpp(pt, mParam.targetBpp));
        break;

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
        break;
    }
    OstTraceFiltInst2(TRACE_API,"In JPEGEnc_Proxy : getConfig DONE IndexType : %d  line no : %d \n",nParamIndex, __LINE__);
    return OMX_ErrorNone;
}


void JPEGEnc_Proxy::setProfileLevelSupported()
{

}

OMX_ERRORTYPE JPEGEnc_Proxy::detectPortSettingsAndNotify(OMX_U32 size)
{
    OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy : In detectPortSettingsAndNotify line no : %d \n",__LINE__);
    OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
	OMX_BOOL has_changed = OMX_FALSE;

	outPortDef.nPortIndex = 1;
	outPortDef.nVersion.s.nVersionMajor    = 1;
	outPortDef.nVersion.s.nVersionMinor    = 1;
	outPortDef.nVersion.s.nRevision	= 1;
	outPortDef.nVersion.s.nStep		= 0;
	outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

	OstTraceFiltInst1(TRACE_DEBUG,"In JPEGEnc_Proxy : detectPortSettingsAndNotify done line no : %d \n", __LINE__);

	err = ((JPEGEnc_Port*)getPort(1))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
	if(err != OMX_ErrorNone)
	{
		OstTraceInt0(TRACE_ERROR,"JPEGEnc_Proxy : detectPortSettingsAndNotify :: Error while getting settings for output port \n");
		jpegenc_proxy_assert(err,__LINE__,OMX_TRUE);
		return err;
	}

	//added new for buffer size
	if (outPortDef.nBufferSize != size)
	{
		outPortDef.nBufferSize = size;
		has_changed = OMX_TRUE;
	}

	if(has_changed)
	{
			outPortDef.nPortIndex = 1;
			OstTraceFiltInst1(TRACE_FLOW,"JPEGENCProxy Updating settings for output port line no : %d \n", __LINE__);
			err = ((JPEGEnc_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
			if(err != OMX_ErrorNone)
			{
				OstTraceInt0(TRACE_ERROR,"JPEGEnc_Proxy : Error while Updating settings for output port \n");
				jpegenc_proxy_assert(err,__LINE__,OMX_TRUE);
				return err;
			}
	}
    OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy : detectPortSettingsAndNotify DONE line no : %d \n",__LINE__);
	return OMX_ErrorNone;
}

RM_STATUS_E JPEGEnc_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
		OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy : In getResourcesEstimation line no : %d \n",__LINE__);

	OMX_U32 cyclesperMB,frameRate;
	double margin=1.1;
	OMX_U32 height_MB = ((getFrameHeight(0) + 0xF)&(~0xF)) / 16;
	OMX_U32 width_MB  = ((getFrameWidth(0)  + 0xF)&(~0xF)) / 16;

	cyclesperMB=388;
	frameRate=1;

    sva_mcps = ((OMX_U32)(((height_MB * width_MB) * cyclesperMB * frameRate * margin)) / 1000000) + 1;

	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec_Proxy::getResourcesEstimation : %d",sva_mcps);


     //clipping the max value of sva_mcps
	 if (sva_mcps>200) {
		 sva_mcps = 200;
	}

    pEstimationData->sMcpsSva.nMax = sva_mcps;
    pEstimationData->sMcpsSva.nAvg = sva_mcps;
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
		OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy :  getResourcesEstimation DONE line no : %d \n",__LINE__);
    return RM_E_NONE;
}


void JPEGEnc_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType,
									   t_nmf_service_data *serviceData)
{
  OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy : In NmfPanicCallback line no : %d \n",__LINE__);
  OstTraceFiltInst1(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback serviceType : %d \n",serviceType);
  if(serviceType==1)
  {
	 // printf("Shutdown");
  }
  else
  {
	  /* + change for ER 348253/344813 */
	  if (serviceData->panic.panicSource == MPC_EE)
	  {
		  OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for MPC ");
		  if (serviceData->panic.info.mpc.coreid == SVA_CORE_ID)
		  {
#ifdef HOST_MPC_COMPONENT_BOTH //change for ER 352222
			 if ((serviceData->panic.info.mpc.faultingComponent == mpc_processingComponent->ddHandle)         ||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpc_processingComponent->VPPHandle)   		||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpc_processingComponent->cOSTTrace)  		||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpc_processingComponent->mNmfAlgoHandle) 	||
			 	 (serviceData->panic.info.mpc.faultingComponent == mpc_processingComponent->mNmfRmHandle))
			 {
		   	 	if (isMPCobject)
		   	 	{
			  		OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for SVA code id MPC ");
					mpc_processingComponent->errorRecoveryDestroyAll();
					eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorHardware,0);
	   			}
	   			else
	   			{
					OstTraceInt0(TRACE_ERROR,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for MPC but isMPCobject is OMX_FALSE (NOT POSSIBLE)");
				}
			 }
			 else
			 {
			 		OstTraceInt0(TRACE_ERROR,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for MPC but NOT for JPEGEnc");
			 }
#endif //change for ER 352222
		  }
		  else
		  {
				OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for MPC but NOT in SVA");
		  }
	  }
	  else
	  {
		   OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for ARM");
		   if (serviceData->panic.info.host.faultingComponent == (void *)this)
		   {
			   OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for JPEGEnc");
			   if (isMPCobject)
			   {
				   OstTraceInt0(TRACE_ERROR,"In JPEGEnc_Proxy : NmfPanicCallback, Panic for JPEGEnc ARM but MPC JPEGEnc is used");
			   }
			   else
			   {
			   		arm_processingComponent->errorRecoveryDestroyAll();
			   		eventHandler(OMX_EventError,(OMX_U32)OMX_EventError,0);
			   }
		   }
		   else
		   {
			   OstTraceInt0(TRACE_FLOW,"In JPEGEnc_Proxy : NmfPanicCallback, Panic NOT for JPEGEnc");
		   }
	  }
	  /* - change for ER 348253/344813 */
  }
  OstTraceFiltInst1(TRACE_API,"In JPEGEnc_Proxy : NmfPanicCallback DONE line no : %d \n",__LINE__);
}

