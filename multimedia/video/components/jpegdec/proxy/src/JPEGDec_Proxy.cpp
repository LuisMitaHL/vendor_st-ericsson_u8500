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
	#include "video_components_jpegdec_proxy_src_JPEGDec_ProxyTraces.h"
#endif



#include "JPEGDec_Port.h"
#include "JPEGDec_Proxy.h"
#include "JPEGDec_Factory.h"

#define ARMNMF_OBJECT 1
#define MPCNMF_OBJECT 2

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { jpegdec_proxy_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { jpegdec_proxy_assert(_error, __LINE__, OMX_FALSE); return (_error); } }


void JPEGDec_Proxy::jpegdec_proxy_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
	OstTraceFiltInst0(TRACE_API, "Inside jpegdec_proxy_assert");
    if (OMX_ErrorNone != omxError)
    {
        OstTraceFiltInst2(TRACE_ERROR, "JPEGDECProxy : errorType : 0x%x error line no %d\n", omxError,line);
        //NMF_LOG("JPEGDECProxy : errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGDec_Proxy::JPEGDec_Proxy():VFM_Component(&mParam),mParam(this)
{


#ifdef HOST_MPC_COMPONENT_BOTH
      if (mpc_processingComponent)
      {
	      //mSendParamToMpc.set();
	      isMPCobject = OMX_TRUE;
	  }
#endif
      if (arm_processingComponent)
      {
          //mSendParamToARMNMF.set();
          isMPCobject = OMX_FALSE;
	  }
	  isDualDecode = OMX_FALSE;

}


JPEGDec_Proxy::~JPEGDec_Proxy()
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECProxy : ~JPEGDec_Proxy");
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

}

OMX_ERRORTYPE JPEGDec_Proxy::construct()
{

	RETURN_OMX_ERROR_IF_ERROR_OST(VFM_Component::construct(2));
#ifdef HOST_MPC_COMPONENT_BOTH
	if (isMPCobject)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECProxy : Constructing MPC based component \n");
        setProcessingComponent(mpc_processingComponent);

        RETURN_OMX_ERROR_IF_ERROR(mpc_processingComponent->setPortCount(2));
	}
	else
#endif
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECProxy : Constructing ARM based component \n");
		setProcessingComponent(arm_processingComponent);

	    RETURN_OMX_ERROR_IF_ERROR(arm_processingComponent->setPortCount(2));
	}

	OstTraceFiltInst1(TRACE_FLOW, "JPEGDECProxy : In construct size of JPEGDec_Proxy - %d \n",sizeof(JPEGDec_Proxy));
	//NMF_LOG("JPEGDECProxy : In construct size of JPEGDec_Proxy - %d \n",sizeof(JPEGDec_Proxy));
	ENS_Port * portInput = newPort(EnsCommonPortData(IPB+0,
													 OMX_DirInput,
													 1/*nBufferCountMin*/,
													 0/*nBufferSizeMin*/,
													 OMX_PortDomainImage,
													 //direction == OMX_DirInput ? OMX_BufferSupplyInput:OMX_BufferSupplyOutput
													 OMX_BufferSupplyInput),
									*this);
    if (portInput== NULL)
    {
		OstTraceFiltInst1(TRACE_ERROR, "JPEGDec Proxy Insufficient resource for Input newPort line no : %d \n",__LINE__);
		jpegdec_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
		return OMX_ErrorInsufficientResources;
	}

	/*FIXME change minimum buffer req based on lowdelay etc params*/
	ENS_Port * portOutput = newPort(EnsCommonPortData(IPB+1,
													  OMX_DirOutput,
													  1/*nBufferCountMin*/,
													  0/*nBufferSizeMin*/,
													  OMX_PortDomainImage,
													  //direction == OMX_DirInput ? OMX_BufferSupplyInput:OMX_BufferSupplyOutput
													  OMX_BufferSupplyOutput),
									*this);
    if (portOutput == NULL){
		OstTraceFiltInst1(TRACE_ERROR, "JPEGDec Proxy Insufficient resource for Output newPort line no : %d \n",__LINE__);
		delete portInput;
		jpegdec_proxy_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
		return OMX_ErrorInsufficientResources;
	}
	addPort(portInput);
	addPort(portOutput);

	return OMX_ErrorNone;
}

VFM_Port *JPEGDec_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp){
	OstTraceFiltInst2(TRACE_API, "JPEGDECProxy : In newPort for port Number : %d and size - %d \n",commonPortData.mPortIndex,sizeof(JPEGDec_Port));
	//NMF_LOG("Size of JPEGDec Port - %d and Port Index - %d \n",sizeof(JPEGDec_Port),commonPortData.mPortIndex);
	return new JPEGDec_Port(commonPortData, enscomp);
}

OMX_ERRORTYPE JPEGDec_Proxy::reset(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECProxy : reset");
	return mParam.reset();
}

OMX_ERRORTYPE JPEGDec_Proxy::JPEGDec_Processing(OMX_U32 value)
{

	OMX_ERRORTYPE omxError = OMX_ErrorUndefined;

	if (ARMNMF_OBJECT == value)
	{
		isMPCobject = OMX_FALSE;
		OstTraceInt1(TRACE_FLOW, "JPEGDec Proxy Creating Processing Comp for ARM , allocating memory of size : %d \n",sizeof(JPEGDec_ArmNmf_ProcessingComponent));
		arm_processingComponent = new JPEGDec_ArmNmf_ProcessingComponent(*this);
		OstTraceInt1(TRACE_FLOW, "JPEGDECProxy : In JPEGDec_Processing ARMNMF and size of Processing comp - %d \n",sizeof(JPEGDec_ArmNmf_ProcessingComponent));
		//NMF_LOG("Size of JPEGDec Armnmf Processing comp - %d \n",sizeof(JPEGDec_ArmNmf_ProcessingComponent));
		RETURN_XXX_IF_WRONG_OST(arm_processingComponent!= NULL, OMX_ErrorInsufficientResources);
#ifdef HOST_MPC_COMPONENT_BOTH
		mpc_processingComponent = NULL;
#endif
		omxError = OMX_ErrorNone;
	}
#ifdef HOST_MPC_COMPONENT_BOTH
	if (MPCNMF_OBJECT == value)
	{
		isMPCobject = OMX_TRUE;
		OstTraceInt1(TRACE_FLOW, "JPEGDec Proxy Creating Processing Comp for MPC , allocating memory of size : %d \n",sizeof(JPEGDec_ArmNmf_ProcessingComponent));
		mpc_processingComponent = new JPEGDec_NmfMpc_ProcessingComponent(*this);
		OstTraceInt1(TRACE_FLOW, "JPEGDECProxy : In JPEGDec_Processing MPCNMF and size of Processing comp - %d \n",sizeof(JPEGDec_NmfMpc_ProcessingComponent));
		//NMF_LOG("Size of JPEGDec nmfMPC Processing comp - %d \n",sizeof(JPEGDec_NmfMpc_ProcessingComponent));
		RETURN_XXX_IF_WRONG_OST(mpc_processingComponent != NULL, OMX_ErrorInsufficientResources);
		arm_processingComponent = NULL;
		omxError = OMX_ErrorNone;
	}
#endif
	OstTraceInt0(TRACE_ERROR, "JPEGDECProxy : JPEGDec_Processing");
	return omxError;
}

// Mandatory to register the component
OMX_ERRORTYPE JPEGDecFactoryMethodDefault(ENS_Component_p * ppENSComponent,OMX_U32 value)
{
    OMX_ERRORTYPE error;
	OstTraceInt1(TRACE_API, "JPEGDec Proxy Creating Proxy Comp, allocating memory of size : %d \n",sizeof(JPEGDec_Proxy));
    JPEGDec_Proxy *comp = new JPEGDec_Proxy;
	if (comp == NULL)
	{
		OstTraceInt1(TRACE_ERROR, "JPEGDec Proxy Unable to Create JPEGDec _Proxy (OMX_ErrorInsufficientResources) line no %d \n",__LINE__);
		
	}
    RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);

    error = comp->JPEGDec_Processing(value);
	if (error != OMX_ErrorNone)
	{
		OstTraceInt2(TRACE_ERROR, "JPEGDec Proxy OMX error : 0x%x at line no : %d \n",error,__LINE__);
		delete comp;
		return error;
	}
    error = comp->construct();
	if (error != OMX_ErrorNone)
    {
	OstTraceInt2(TRACE_ERROR, "JPEGDec Proxy OMX error : 0x%x at line no : %d \n",error,__LINE__);
		delete comp;
		return error;
	}
    *ppENSComponent = comp;
	OstTraceInt0(TRACE_API, "************ JPEGDec Proxy Factory method REGISTERED *********** ");
    return OMX_ErrorNone;
}



// Mandatory to register the component - default is MPC SVA based decoder
EXPORT_C OMX_ERRORTYPE JPEGDecFactoryMethod(ENS_Component_p * ppENSComponent)
{
#ifdef HOST_MPC_COMPONENT_BOTH
	return JPEGDecFactoryMethodDefault(ppENSComponent,MPCNMF_OBJECT);
#else
	OstTraceInt0(TRACE_ERROR, "JPEGDECProxy : JPEGDecFactoryMethod OMX_ErrorInvalidComponent");
	return OMX_ErrorInvalidComponent;
#endif
}


// Mandatory to register the component - for MPC SVA based decoder
EXPORT_C OMX_ERRORTYPE JPEGDecFactoryMethodMPC(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "********* JPEGDec Proxy Factory method for MPC ********* ");
#ifdef HOST_MPC_COMPONENT_BOTH
	return JPEGDecFactoryMethodDefault(ppENSComponent,MPCNMF_OBJECT);
#else
	OstTraceInt0(TRACE_ERROR, "JPEGDECProxy : JPEGDecFactoryMethodMPC OMX_ErrorInvalidComponent");
	return OMX_ErrorInvalidComponent;
#endif
}

EXPORT_C OMX_ERRORTYPE JPEGDecFactoryMethodDUAL(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "********* JPEGDec Proxy Factory method for DUAL ********* ");
#ifdef HOST_MPC_COMPONENT_BOTH
	OMX_ERRORTYPE error_type;
	error_type = JPEGDecFactoryMethodDefault(ppENSComponent,MPCNMF_OBJECT);
	if (OMX_ErrorNone == error_type)
	{
		JPEGDec_Proxy *pProxyComponent = (JPEGDec_Proxy *)(*ppENSComponent);
		pProxyComponent->isDualDecode = OMX_TRUE;
	}
	else
		OstTraceInt0(TRACE_ERROR, "JPEGDECProxy : JPEGDecFactoryMethodDUAL error");
	return error_type;
#else
	OstTraceInt0(TRACE_ERROR, "JPEGDECProxy : JPEGDecFactoryMethodDUAL OMX_ErrorInvalidComponent");
	return OMX_ErrorInvalidComponent;
#endif
}


// Mandatory to register the component - for SW Encoder
EXPORT_C OMX_ERRORTYPE JPEGDecFactoryMethodHOST(ENS_Component_p * ppENSComponent)
{
	OstTraceInt0(TRACE_API, "********* JPEGDec Proxy Factory method for HOST ********* ");
	return JPEGDecFactoryMethodDefault(ppENSComponent,ARMNMF_OBJECT);
}



OMX_ERRORTYPE JPEGDec_Proxy::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{
	OstTraceFiltInst1(TRACE_API, "JPEGDECProxy : setParameter - %d",nParamIndex);
    switch ((int)nParamIndex){
	case OMX_IndexParamImagePortFormat:
	{
 		OMX_IMAGE_PARAM_PORTFORMATTYPE *pt_image = (OMX_IMAGE_PARAM_PORTFORMATTYPE*)pt;
		if(pt_image->nPortIndex > getPortCount())
        {
            OstTraceFiltInst3(TRACE_ERROR, "JPEGDec Proxy, OMX_ErrorNoMore, PortIndex : %d exceeds actual no of Ports : %d line no : %d \n",pt_image->nPortIndex,getPortCount(),__LINE__);
			return OMX_ErrorNoMore;
        }
		RETURN_OMX_ERROR_IF_ERROR_OST(((JPEGDec_Port*)getPort(pt_image->nPortIndex))->setIndexParamImagePortFormat((OMX_IMAGE_PARAM_PORTFORMATTYPE*)pt));
	}
        break;

	case VFM_IndexParamResourceEstimation:
		RETURN_OMX_ERROR_IF_ERROR(mParam.setIndexParamResourceSvaMcps(pt));
		break;


    default:
        RETURN_OMX_ERROR_IF_ERROR_OST(VFM_Component::setParameter(nParamIndex, pt));
        break;
	}

	OstTraceFiltInst0(TRACE_API, "JPEGDECProxy : Returning from setParameter \n");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_Proxy::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst1(TRACE_API, "JPEGDECProxy : getParameter  -%d",nParamIndex);
    switch ((int)nParamIndex) {
    case OMX_IndexParamImagePortFormat:
	{
 		OMX_IMAGE_PARAM_PORTFORMATTYPE *pt_image = (OMX_IMAGE_PARAM_PORTFORMATTYPE*)pt;
		if(pt_image->nPortIndex > getPortCount())
		{
			OstTraceFiltInst3(TRACE_ERROR, "JPEGDec Proxy, OMX_ErrorNoMore, PortIndex : %d exceeds actual no of Ports : %d line no : %d \n",pt_image->nPortIndex,getPortCount(),__LINE__);
			return OMX_ErrorNoMore;
		}
		RETURN_OMX_ERROR_IF_ERROR(((JPEGDec_Port*)getPort(pt_image->nPortIndex))->getIndexParamImagePortFormat((OMX_IMAGE_PARAM_PORTFORMATTYPE*)pt));
	}
		break;

	case VFM_IndexParamResourceEstimation:
		RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexParamResourceSvaMcps(pt));
			break;


    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getParameter(nParamIndex, pt));
        break;
    }
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
{

    OMX_BOOL has_changed = OMX_FALSE;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE outPortDef ;
    memset(&outPortDef, 0, sizeof(outPortDef));
	OstTraceFiltInst2(TRACE_API, "JPEGDECProxy : SetConfig Called -%d , has_changed - %d",nParamIndex,has_changed);
    switch (nParamIndex){
    case OMX_IndexConfigCommonOutputCrop:
	{
		JPEGDec_Port *port = ( JPEGDec_Port *)(this->getPort(1));



			OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;
			pt_crop->nPortIndex = 0; //setting the cropping for Port 0


		    RETURN_XXX_IF_WRONG_OST(0<=pt_crop->nLeft && (OMX_U32)pt_crop->nLeft<=getFrameWidth(0), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG_OST(0<=pt_crop->nTop && (OMX_U32)pt_crop->nTop<=getFrameHeight(0), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG_OST(0<=pt_crop->nLeft+(t_sint32)pt_crop->nWidth && (pt_crop->nLeft+pt_crop->nWidth<=getFrameWidth(0)), OMX_ErrorBadParameter);
		    RETURN_XXX_IF_WRONG_OST(0<=pt_crop->nTop+(t_sint32)pt_crop->nHeight && pt_crop->nTop+pt_crop->nHeight<=getFrameHeight(0), OMX_ErrorBadParameter);

			has_changed = (OMX_BOOL)((mParam.cropWindowHeight != pt_crop->nHeight       ) ||
			  						 (mParam.cropWindowWidth  != pt_crop->nWidth	    ) ||
									 (mParam.horizontal_offset   != (OMX_U32)pt_crop->nLeft) ||
									 (mParam.vertical_offset    != (OMX_U32)pt_crop->nTop));


		    err = mParam.setIndexConfigCommonOutputCrop(pt);
		    DBC_ASSERT(OMX_ErrorNone == err);
			mParam.cropWindowWidth	= pt_crop->nWidth;
			mParam.cropWindowHeight = pt_crop->nHeight;
    	    mParam.horizontal_offset = pt_crop->nLeft;
			mParam.vertical_offset = pt_crop->nTop;

		OstTraceFiltInst2(TRACE_FLOW, "JPEGDECProxy : Value of has_changed in setConfig at line no : %d \n",has_changed,__LINE__);

		if(has_changed) {
			mParam.configCroppingChanged = OMX_TRUE;
			setFrameHeight(IPB+1, mParam.cropWindowHeight/mParam.downsamplingFactor);
			setFrameWidth(IPB+1, mParam.cropWindowWidth/mParam.downsamplingFactor);
		}
		else
			mParam.configCroppingChanged = OMX_FALSE;

		mParam.isCroppingEnabled = 1;
		if(has_changed){
			outPortDef.nPortIndex = 1;
			outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
			getOmxIlSpecVersion(&outPortDef.nVersion);
			RETURN_OMX_ERROR_IF_ERROR(port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef));
			err = ((JPEGDec_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
			if(err != OMX_ErrorNone) 
			{
				OstTraceFiltInst0(TRACE_ERROR, "JPEGDECProxy : setConfig ERROR \n");
				return err;
			}
		}
	}
	break;

    case OMX_IndexConfigCommonScale:
	{
		JPEGDec_Port *port = ( JPEGDec_Port *)(this->getPort(1));
        RETURN_OMX_ERROR_IF_ERROR_OST(mParam.setIndexConfigCommonScale(pt, &mParam.downsamplingFactor, &has_changed));
		if(has_changed) {
			mParam.configScalingChanged = OMX_TRUE;
			if(mParam.isCroppingEnabled) {
				setFrameHeight(IPB+1, mParam.cropWindowHeight/mParam.downsamplingFactor);
				setFrameWidth(IPB+1, mParam.cropWindowWidth/mParam.downsamplingFactor);
			}
			else{
				setFrameHeight(IPB+1,getFrameHeight(0)/mParam.downsamplingFactor);
				setFrameWidth(IPB+1, getFrameWidth(0)/mParam.downsamplingFactor);
			}
		}
		else
			mParam.configScalingChanged = OMX_FALSE;

		OstTraceFiltInst2(TRACE_FLOW, "JPEGDECProxy : Value of has_changed in setConfig for Scale at line no : %d \n",has_changed,__LINE__);
		mParam.isDownsamplingEnabled = 1;
		if(has_changed){
			if(mParam.downsamplingFactor!=DOWNSAMPLING_FACTOR_1){
				outPortDef.nPortIndex = 1;
				outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
				getOmxIlSpecVersion(&outPortDef.nVersion);
				RETURN_OMX_ERROR_IF_ERROR(port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef));
				err = ((JPEGDec_Port*)getPort(1))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
				if(err != OMX_ErrorNone) 
				{
					OstTraceFiltInst0(TRACE_ERROR, "JPEGDECProxy : setConfig has_changed  ERROR \n");
					return err;
				}
			}
		}
	}
       break;

    default:
        RETURN_OMX_ERROR_IF_ERROR_OST(VFM_Component::setConfig(nParamIndex, pt));
        break;
    }
	if(has_changed){
		//processingComponent.updateDynamicConfiguration(this);
	}

    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_Proxy::getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const
{
	OstTraceFiltInst1(TRACE_API, "JPEGDECProxy : In getConfig nparamIndex-%d",nParamIndex);
	switch (nParamIndex) {
    case OMX_IndexConfigCommonOutputCrop:
        //RETURN_OMX_ERROR_IF_ERROR_OST(mParam.getIndexConfigCommonOutputCrop(pt, mParam.cropWindowWidth,
		//												mParam.cropWindowHeight,mParam.horizontal_offset,
		//												mParam.vertical_offset));
		{
	    	OMX_CONFIG_RECTTYPE *pt_crop = (OMX_CONFIG_RECTTYPE *)pt;

			RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonOutputCrop(pt));
			pt_crop->nHeight 	= mParam.cropWindowHeight;
			pt_crop->nWidth 	= mParam.cropWindowWidth;
			pt_crop->nLeft 		= mParam.horizontal_offset;
			pt_crop->nTop 		= mParam.vertical_offset;
		}

        break;

    case OMX_IndexConfigCommonScale:
        RETURN_OMX_ERROR_IF_ERROR(mParam.getIndexConfigCommonScale(pt, mParam.downsamplingFactor));
        break;

    default:
        RETURN_OMX_ERROR_IF_ERROR(VFM_Component::getConfig(nParamIndex, pt));
        break;
    }

    return OMX_ErrorNone;
}

void JPEGDec_Proxy::setProfileLevelSupported(){
}

OMX_ERRORTYPE JPEGDec_Proxy::detectPortSettingsAndNotify(OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{

	OMX_U16 prev_width = getFrameWidth(0);
	OMX_U16 prev_height = getFrameHeight(0);
	OMX_ERRORTYPE err =OMX_ErrorNone;
	OMX_BOOL has_changed=OMX_FALSE;
	OMX_IMAGE_PORTDEFINITIONTYPE *pt_image = &(portDef->format.image);
	OstTraceFiltInst1(TRACE_FLOW, "JPEGDECProxy detectPortSettingsAndNotify -%d",has_changed);
	OstTraceFiltInst1(TRACE_API, "JPEGDECProxy : In detectPortSettingsAndNotify for port Number : %d \n",portDef->nPortIndex);

    if (portDef->nPortIndex==1)
    {
		err = updateOutputPortSettings(OMX_IndexParamPortDefinition,portDef);
		OstTraceFiltInst2(TRACE_FLOW, "JPEGDECProxy detectPortSettingsAndNotify and errror - %d  and line no - %d\n",err,__LINE__);
		return err;
	}

    if(portDef->nPortIndex!=0)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECProxy detectPortSettingsAndNotify portDef->nPortIndex!=0 ");
		return OMX_ErrorUndefined;
	}

	if(prev_width != pt_image->nFrameWidth ||prev_height != pt_image->nFrameHeight){
		//portDef.nFrameWidth = ptrHeaderBuffer->s_in_parameters.frame_width;
		has_changed = OMX_TRUE;
	}
	//Necessary to initilaize the frame dimensions in case anyone matches.
	//portDef.nFrameWidth = ptrHeaderBuffer->s_in_parameters.frame_width;
	//portDef.nFrameHeight = ptrHeaderBuffer->s_in_parameters.frame_height;

	mParam.frameHeight = pt_image->nFrameHeight;
	mParam.frameWidth = pt_image->nFrameWidth;
	if(!mParam.isCroppingEnabled || mParam.isCroppingInvalid){
		mParam.cropWindowHeight = pt_image->nFrameHeight;
		mParam.cropWindowWidth = pt_image->nFrameWidth;
		mParam.vertical_offset=0;
		mParam.horizontal_offset=0;
    }

    if(has_changed){
		err=JPEG_Proxy_PortSettingsChangedNotify(portDef);
		OstTraceFiltInst2(TRACE_FLOW, "JPEGDECProxy detectPortSettingsAndNotify inside check for has_changed and errror - %d  and line no - %d\n",err,__LINE__);
    }

	jpegdec_proxy_assert(err, __LINE__, OMX_FALSE);
	return err;
}

OMX_ERRORTYPE JPEGDec_Proxy::JPEG_Proxy_PortSettingsChangedNotify(OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{

	OMX_ERRORTYPE err =OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
	OstTraceFiltInst0(TRACE_API, "JPEGDEC_PROXY : JPEG_Proxy_PortSettingsChangedNotify \n");
	JPEGDec_Port *port = (JPEGDec_Port *)(getPort(0));
	outPortDef.nPortIndex = 0;
	outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	getOmxIlSpecVersion(&outPortDef.nVersion);
	err = port->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
	if(err!=OMX_ErrorNone) return err;
	outPortDef.format.image.nFrameHeight = portDef->format.image.nFrameHeight;
	outPortDef.format.image.nFrameWidth = portDef->format.image.nFrameWidth;
	//outPortDef.format.image.nSliceHeight = portDef->nFrameHeight;
	//outPortDef.format.image.nStride = portDef->nFrameWidth*2;
	//err = port->setParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
	//if(err!=OMX_ErrorNone) return err;

	err = ((JPEGDec_Port*)getPort(0))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
	OstTraceFiltInst2(TRACE_FLOW, "JPEGDECProxy JPEG_Proxy_PortSettingsChangedNotify errror - %d  and line no - %d\n",err,__LINE__);
	jpegdec_proxy_assert(err, __LINE__, OMX_FALSE);

	return err;
}


OMX_ERRORTYPE JPEGDec_Proxy::updateOutputPortSettings(OMX_INDEXTYPE nParamIndex,
													  const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDEC_PROXY : updateOutputPortSettings \n");
    OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
	OMX_BOOL has_changed = OMX_FALSE;

	outPortDef.nPortIndex = 1;
	outPortDef.nVersion.s.nVersionMajor    = portDef->nVersion.s.nVersionMajor;
	outPortDef.nVersion.s.nVersionMinor    = portDef->nVersion.s.nVersionMinor;
	outPortDef.nVersion.s.nRevision	= portDef->nVersion.s.nRevision;
	outPortDef.nVersion.s.nStep		= portDef->nVersion.s.nStep;
	outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

	err = ((JPEGDec_Port*)getPort(1))->getParameter(nParamIndex,(OMX_PTR)&outPortDef);
	if(err != OMX_ErrorNone)
	{
		jpegdec_proxy_assert(err, __LINE__, OMX_FALSE);
		return err;
	}

	if(!mParam.isCroppingEnabled || mParam.isCroppingInvalid){
		if(outPortDef.format.image.nFrameHeight != portDef->format.image.nFrameHeight){
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PROXY : updateOutputPortSettings 1 \n");
			outPortDef.format.image.nFrameHeight = portDef->format.image.nFrameHeight/mParam.downsamplingFactor;
			has_changed = OMX_TRUE;
		}

		if(outPortDef.format.image.nFrameWidth != portDef->format.image.nFrameWidth){
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PROXY : updateOutputPortSettings 2 \n");
			outPortDef.format.image.nFrameWidth = portDef->format.image.nFrameWidth/mParam.downsamplingFactor;
			has_changed = OMX_TRUE;
		}

		if(outPortDef.format.image.nStride != portDef->format.image.nStride){
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PROXY : updateOutputPortSettings 3 \n");
			outPortDef.format.image.nStride = portDef->format.image.nStride;
			has_changed = OMX_TRUE;
		}

		//added new for color format
		if (outPortDef.format.image.eColorFormat != portDef->format.image.eColorFormat){
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PROXY : updateOutputPortSettings 4 \n");
			outPortDef.format.image.eColorFormat = portDef->format.image.eColorFormat;
			has_changed = OMX_TRUE;
		}

		//added new for buffer size
		if (outPortDef.nBufferSize != portDef->nBufferSize){
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDEC_PROXY : updateOutputPortSettings 5 \n");
			outPortDef.nBufferSize = portDef->nBufferSize;
			has_changed = OMX_TRUE;
		}


		if(has_changed){
			outPortDef.nPortIndex = 1;
			OstTraceFiltInst1(TRACE_FLOW, "JPEGDECProxy : Updating Setting for Output Port, line no : %d \n",__LINE__);
			err = ((JPEGDec_Port*)getPort(1))->updateSettings(nParamIndex,(OMX_PTR)&outPortDef);
			if(err != OMX_ErrorNone) 
			{
				OstTraceFiltInst0(TRACE_FLOW, "JPEGDECProxy : updateOutputPortSettings ERROR");
				return err;
			}
		}
	}


	return OMX_ErrorNone;
}


void JPEGDec_Proxy::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType,
									   t_nmf_service_data *serviceData)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDEC_PROXY : NmfPanicCallback \n");
if(serviceType==1)
  {
	 // printf("Shutdown");
  }
  else
  {
#ifdef HOST_MPC_COMPONENT_BOTH
	   if (isMPCobject)
	   {
			mpc_processingComponent->errorRecoveryDestroyAll();
	   }
	   else
#endif
	   {
			  arm_processingComponent->errorRecoveryDestroyAll();
	   }
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECProxy : ERROR HARDWARE");
	   eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorHardware,0);
  }
}


RM_STATUS_E JPEGDec_Proxy::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDec_Proxy::getResourcesEstimation\n");
    pEstimationData->sMcpsSva.nMax = mParam.resourceSvaMcps();
    pEstimationData->sMcpsSva.nAvg = mParam.resourceSvaMcps();
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 500; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 0;   // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_DEFAULT;    // RM_ESBUFID_BIG required for 1080p on symbian.  RM_ESBUFID_BIG for H264Encode. RM_ESBUFID_DEFAULT for others
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    OstTraceFiltInst2(TRACE_API, "JPEGDec Proxy, In getResourcesEstimation(), value of sMcpsSva.nMax (%d) sMcpsSva.nAvg (%d) ",pEstimationData->sMcpsSva.nMax,pEstimationData->sMcpsSva.nAvg);
    OstTraceFiltInst3(TRACE_API, "JPEGDec Proxy, In getResourcesEstimation(), value of nSvaHwPipe (%d) nSiaHwPipe (%d) nSiaHwXP70 (%d) ",pEstimationData->sEsramBufs.nSvaHwPipe,pEstimationData->sEsramBufs.nSiaHwPipe,pEstimationData->sEsramBufs.nSiaHwXP70);
    return RM_E_NONE;
}
