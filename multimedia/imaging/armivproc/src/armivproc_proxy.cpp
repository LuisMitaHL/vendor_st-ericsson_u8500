/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#define DECLARE_AUTOVAR
#include <stdio.h>
#include <string.h>
#undef    OMXCOMPONENT
#define   OMXCOMPONENT "ArmIVProc"
#undef    OMX_TRACE_UID
#define   OMX_TRACE_UID 0x8
#include "osi_trace.h"

#include "wrapper_openmax_lib.h"
#include "ImageConverter_lib.h"
#include "OMX_IVCommon.h"
#include <OMX_Component.h>

#include "armivproc_proxy.h"
#include "armivproc_process.h"

#include "extradata.h" // used for both extradata & extensions

#ifndef OMX_IFM_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL
	#define OMX_IFM_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL 3
#endif

////Global variable for Xti trace
//int DBGT_VAR=DBGT_VAR_INIT;


openmax_processor *Create_armivproc_NMF()
//*************************************************************************************************************
{ // wrp Factory for ArmIVProc
	return(new ArmIVProc_process);
}

OMX_ERRORTYPE Factory_ArmIVProc(ENS_Component_p * ppENSComponent)
//*************************************************************************************************************
{  //factory method for Red Eye Detection
	GET_AND_SET_TRACE_LEVEL(armivproc);
	IN0("\n");
	ArmIVProc * pArmIVProc = new ArmIVProc();

	if (pArmIVProc  == 0) 
	{
		ReportError(OMX_ErrorInsufficientResources, "Cannot construct ArmIVProc");
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	// now try to really construct the component
	if (pArmIVProc->Construct()!= eNoError)
	{
		ReportError(OMX_ErrorInsufficientResources, "Cannot construct ArmIVProc-Insufficient ressource");
		delete pArmIVProc;
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	OpenMax_Proxy * pProxy = new OpenMax_Proxy("ArmIVProc", *pArmIVProc, Create_armivproc_NMF);
	if (pProxy == 0) 
	{
		ReportError(OMX_ErrorInsufficientResources, "Cannot create proxy for ArmIVProc-Insufficient ressource");
		delete pArmIVProc;
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	pArmIVProc->setProcessingComponent(pProxy);
	*ppENSComponent = pArmIVProc;

	OUTR("", (OMX_ErrorNone));
	return OMX_ErrorNone;
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#ifdef __SYMBIAN32__
	#define   OMXCOMPONENT "ArmIVProc_proxy"
#else
	#define   OMXCOMPONENT GetComponentName()
#endif

ArmIVProc::ArmIVProc():OpenMax_Component("ArmIVProc")
//*************************************************************************************************************
{
	INIT_OMX_STRUCT_INDEX(OMX_CONFIG_ROTATIONTYPE    , m_Rotation);
	m_Rotation.nRotation= 0; //set default value

	INIT_OMX_STRUCT_INDEX(OMX_CONFIG_MIRRORTYPE    , m_Mirror);
	m_Mirror.eMirror =OMX_MirrorNone; //set default value

	m_bAutoRotateEnable=false; //disabled by default
}

ArmIVProc::~ArmIVProc()
//*************************************************************************************************************
{
}

OMX_ERRORTYPE ArmIVProc::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
//*************************************************************************************************************
{
	if (0 == strcmp(cParameterName, OMX_SYMBIAN_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL_NAME)) /* "OMX.Symbian.Index.Config.Common.AutoRotationControl" */
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_IFM_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL;
		return OMX_ErrorNone;
	}
	return (CExtension::getExtensionIndex(cParameterName, pIndexType));
}

int ArmIVProc::Construct()
//*************************************************************************************************************
{ // Construct ports
	IN0("\n");
	int  error;
	int status=eNoError;
	const int NbPorts=2;
	const int MinBufferHeader=1;
	const OMX_U32 nRoleCount=1;
	const bool bArmOnly=true;
	ENS_Component::construct(NbPorts, nRoleCount, bArmOnly); //Construct ports
	//Output port
	const EnsCommonPortData PortData0(0, OMX_DirInput, MinBufferHeader, 0, OMX_PortDomainVideo, OMX_BufferSupplyUnspecified /* OMX_BufferSupplyInput */);
	error=createPort(PortData0);
	if (error!=OMX_ErrorNone)
	{
		ReportError(eError_CannotCreateInputPort, "Cannot create Input port[%d] (video)", 0);
		status=error;
	}

	//Output port
	const EnsCommonPortData PortData1(1, OMX_DirOutput, MinBufferHeader, 0, OMX_PortDomainVideo, OMX_BufferSupplyUnspecified/* OMX_BufferSupplyOutput */);
	error=createPort(PortData1);
	if (error!=OMX_ErrorNone)
	{
		ReportError(eError_CannotCreateOutputPort, "Cannot create Output port[%d]  (other)", 1);
		status=error;
	}
	OUTR("", (status));
	return(status);
}

/**
@brief Give a chance to add proper initialisation when the NMF component is start
Call OpenMax_Proxy::applyConfig for all parameters that have beeen configured before idle state when the processor haven't been started
*/
int ArmIVProc::NMFConfigure()
//*************************************************************************************************************
{	// Configuration before real start
	IN0("\n");
	int status=S_OK;
	ProcessingComponent &Comp=getProcessingComponent();
	Comp.applyConfig(OMX_IndexConfigCommonRotate, (OMX_PTR)&m_Rotation);
	Comp.applyConfig(OMX_IndexConfigCommonMirror, (OMX_PTR)&m_Mirror);
	OUTR("", (status));
	return(status);
}

OMX_PTR ArmIVProc::getConfigAddr(OMX_INDEXTYPE nIndex, size_t *pStructSize, int *pOffset) const
//*************************************************************************************************************
{ //return the address of the config 
	IN0("\n");
	size_t size=0;
	OMX_PTR pStructure=NULL;
	int offset=0;
	switch ((OMX_U32)nIndex) 
	{
	case OMX_IndexConfigCommonRotate:
		size=sizeof(OMX_CONFIG_ROTATIONTYPE);
		pStructure= (void *)&m_Rotation;
		// DBG_MSG(" Query address for rotation \n");
		break;
	case OMX_IndexConfigCommonMirror:
		size=sizeof(OMX_CONFIG_MIRRORTYPE);
		pStructure= (void *)&m_Mirror;
		break;
	case OMX_IFM_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL:
		size      = sizeof(m_bAutoRotateEnable);
		pStructure= (void *)&m_bAutoRotateEnable;
		offset    = (int)offsetof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE, bEnabled);
		break;
	default : // call base class
		return( OpenMax_Component::getConfigAddr(nIndex, pStructSize, pOffset));

	}
	if (pStructSize)
	{ //return the size of the struct
		*pStructSize=size;
	}
	if (pOffset)
	{ //return the offset in the setparam
		*pOffset=offset;
	}
	OUTR("", (int)pStructure);
	return pStructure;
}


/// call when a set param has been done on a port
int ArmIVProc::PortFormatChanged(ENS_Port & port )
//*************************************************************************************************************
{ //By default done nothing
	IN0("\n");
	int res=S_OK;

	PortVideo &Port=(PortVideo &)port;
	OMX_PARAM_PORTDEFINITIONTYPE&  PortDef=Port.getParamPortDefinition();
	//const size_t BufferSize=PortDef.nBufferSize;
	enum_ImageFormat format=GetFormat(PortDef.format.video.eColorFormat);
	const _sSupportedRWFormat *pFuncTable =GetRWFunction(format);
	if (pFuncTable ==NULL)
	{
		WOM_ASSERT(0);
		ReportError(eError_UnsupportedInputFormat , "Unsupported format %d", PortDef.format.video.eColorFormat);
		return(eError_UnsupportedInputFormat);
	}
	if (Port.getPortIndex()==0)
	{
		if (pFuncTable->Read_fn==NULL)
		{
			WOM_ASSERT(0);
			ReportError(eError_UnsupportedInputFormat , "format %d not supported for read", PortDef.format.video.eColorFormat);
			return(eError_UnsupportedInputFormat);
		}
	}
	else
	{ //Check output port
		if (pFuncTable->Write_fn==NULL)
		{
			WOM_ASSERT(0);
			ReportError(eError_UnsupportedOutputFormat , "format %d not supported for write", PortDef.format.video.eColorFormat);
			return(eError_UnsupportedOutputFormat);
		}
		PortVideo  *pPortIn=(PortVideo  *)getPort(0);
		WOM_ASSERT(pPortIn);
		OMX_PARAM_PORTDEFINITIONTYPE&  PortDefIn=pPortIn->getParamPortDefinition();
		if ( (PortDefIn.format.video.eColorFormat !=OMX_COLOR_FormatUnused ) && (PortDefIn.format.video.nFrameWidth !=0) && (PortDefIn.format.video.nFrameHeight !=0))
		{ // Check resolution
			CheckResolution_Downmscale(PortDefIn.format.video.nFrameWidth, PortDefIn.format.video.nFrameHeight, PortDef.format.video.nFrameWidth, PortDef.format.video.nFrameHeight);
		}
	}

	OUTR("", res);
	return(res);
}

