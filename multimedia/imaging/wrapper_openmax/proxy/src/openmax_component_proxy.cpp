/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#undef    OMXCOMPONENT
#define   OMXCOMPONENT ""
#undef    OMX_TRACE_UID
#define   OMX_TRACE_UID 0x8
#include "osi_trace.h"

#include "ENS_Component_Fsm.h"
#include "SharedBuffer.h"
#include <OMX_CoreExt.h>

#include "../include/wrapper_openmax_tools.h"
#include "../include/openmax_component.h"
#include "../include/openmax_component_proxy.h"

#ifndef OMX_EventCmdReceived
	#define OMX_EventCmdReceived 9
#endif

void OpenMax_Component_ConfigCB::setConfig(t_sint32 index, void * opaque_ptr)
//*************************************************************************************************************
{
	IN0("\n");
	mOMXProxy->setConfigFromProcessing((OMX_INDEXTYPE)index, (OMX_PTR)opaque_ptr);
	OUT0("\n");
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT GetComponentName()
#define   TRACENAME "OpenMax_Proxy"


 WRAPPER_OPENMAX_API OpenMax_Proxy::OpenMax_Proxy(const char *Compname, OpenMax_Component &enscomp, fnCreateNmfComponent _fnCreateNmfComponent)
: NmfHost_ProcessingComponent(enscomp)
, m_fnCreateNmfComponent(_fnCreateNmfComponent)
, m_Name(Compname?Compname: TRACENAME)
, m_UserName(NULL)
, m_pProcessor(NULL)
, m_nmfPriority(1) // Default to NMF Normal priority
//*************************************************************************************************************
{ 
	IN0("\n");
	// WARNING(" obsolete constructor");
	mConfigCB = new OpenMax_Component_ConfigCB(this);
	FormatUserComponentName(m_UserName, TRACENAME, m_Name);
	for (int i=0; i<MAX_PORTS_COUNT; ++i)
		m_NbPortAllocated[i]=0;
	// WOM_BREAKPOINT;
	OUT0("\n");
}

#if 0
/* OBSOLETE */WRAPPER_OPENMAX_API OpenMax_Proxy::OpenMax_Proxy(const char *Compname, OpenMax_Component &enscomp, openmax_processor* ProcComp)
: NmfHost_ProcessingComponent(enscomp)
, m_fnCreateNmfComponent(NULL)
, m_Name(Compname?Compname: TRACENAME)
, m_UserName(NULL)
, m_pProcessor(ProcComp)
//*************************************************************************************************************
{//Obsolete should be removed because the processor should be construct later and not now
	IN0("\n");
	mConfigCB = new OpenMax_Component_ConfigCB(this);
	FormatUserComponentName(m_UserName, TRACENAME, m_Name);
	for (int i=0; i<MAX_PORTS_COUNT; ++i)
		m_NbPortAllocated[i]=0;
	// WOM_BREAKPOINT;
	OUT0("\n");
}
#endif

WRAPPER_OPENMAX_API OpenMax_Proxy::~OpenMax_Proxy()
//*************************************************************************************************************
{
	IN0("\n");
	if (mConfigCB)
		delete mConfigCB;
	mConfigCB = 0;
	if (m_UserName)
		delete [] m_UserName;
	OUT0("\n");
}

//Put 0 for asynchronous  !0 for synchronous
#define SYNCHRONOUS 1

/** 
brief Instantiate the nmf component and bind interfaces
Called during Loaded to Idle transition
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::instantiate()
//*************************************************************************************************************
{ //binding all interfaces
	OMX_ERRORTYPE omx_error=OMX_ErrorNone;
	IN0("\n");
	// Init Components
	// Bind of arm nmf components
	MSG1("OpenMax_Proxy(%s) Instantiating arm nmf component\n", GetComponentName());
	openmax_processor *pProcessor=NULL;
	if (m_pProcessor != NULL)
		pProcessor = m_pProcessor;
	else if (m_fnCreateNmfComponent !=NULL)
	{ 
		m_pProcessor= m_fnCreateNmfComponent();
		pProcessor  = m_pProcessor;
	}

	m_pNmfProcWrp = openmax_processor_wrpCreate(pProcessor);

	m_pNmfProcWrp->priority = m_nmfPriority;

	if (m_pNmfProcWrp->construct() != NMF_OK)
		NMF_PANIC("PANIC - Construct Error\n") ;

	t_nmf_error error ;
	// bindFromUser --> Asynchronous
	// getInterface --> Synchronous

	error = m_pNmfProcWrp->bindFromUser("sendcommand", 2, &mIsendCommand) ;
	if (error != NMF_OK)
		NMF_PANIC("PANIC - bindFromUser sendCommand\n") ;
#if (SYNCHRONOUS ==0)
	error = m_pNmfProcWrp->bindFromUser("Param", 2*mENSComponent.getPortCount()+1, &m_IParam) ; // One setparam per port + 1
	if (error != NMF_OK)
		NMF_PANIC("PANIC - bindFromUser Param\n") ;

	error = m_pNmfProcWrp->bindFromUser("Config", 2*mENSComponent.getPortCount()+1, &m_IConfig) ; // One setparam per port + 1
	if (error != NMF_OK)
		NMF_PANIC("PANIC - bindFromUser Config\n") ;
#else //Bind synchronous interfaces
	//interfaces are binded in a synchronous way
	error = m_pNmfProcWrp->getInterface("Param", &m_IParam);
	if (error != NMF_OK)
		NMF_PANIC("PANIC - getInterface Param\n") ;

	error = m_pNmfProcWrp->getInterface("Config", &m_IConfig);
	if (error != NMF_OK)
		NMF_PANIC("PANIC - getInterface Config\n") ;
#endif
	error = m_pNmfProcWrp->bindFromUser("fsminit", 1 , &mIfsmInit);
	if (error != NMF_OK)
		NMF_PANIC("Error: unable to bind fsminit!...\n");

	error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(), m_pNmfProcWrp, "proxy", getEventHandlerCB(), 8);
	if (error != NMF_OK)
		NMF_PANIC("Error: unable to bind proxy!...\n");

	error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(), m_pNmfProcWrp, "ToOMXComponent", mConfigCB, 1);
	if (error != NMF_OK)
		NMF_PANIC("Error: unable to bind ToOMXComponent!...\n");

	char name[20]; //reserve some char for formatting callback name
	size_t iNbInput =0;
	size_t iNbOutput=0;
	PortVideo* pPort;
	for (unsigned int Index=0; Index< mENSComponent.getPortCount(); ++Index)
	{
		pPort = (PortVideo*)mENSComponent.getPort(Index);
		MSG3("\n %s Port %d : direction =%d\n", GetComponentName(), Index, pPort->getDirection());
		//reset port info at this point 
		//NO it's too late!
#if 0
		if (pPort->getBufferSupplier() !=OMX_BufferSupplyUnspecified)
		{
			printf("wwwwwwwwwwwwwwwwwwwwwwwwwwww    %s.port[%d] reset state(%d) to OMX_BufferSupplyUnspecified\n", GetComponentName(), Index, pPort->getBufferSupplier());
			pPort->setBufferSupplier(OMX_BufferSupplyUnspecified);
		}
#endif
		switch(pPort->getDirection())
		{
		case OMX_DirInput: //bind emptythisbuffer, emptybufferdone for each input port
			if (iNbInput < MAX_PORTS_COUNT)
			{
				sprintf(name, "emptythisbuffer[%d]", Index);
				error = m_pNmfProcWrp->bindFromUser(name, pPort->getBufferCountActual(), &mIemptyThisBuffer[Index]);
				if (error != NMF_OK)
					NMF_PANIC("PANIC %s->bindFromUser %s\n", GetComponentName(), name) ;

				sprintf(name, "emptybufferdone[%d]", Index);
				error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),
					m_pNmfProcWrp,
					name ,
					this->getEmptyBufferDoneCB(),
					pPort->getBufferCountActual());

				if (error != NMF_OK)
					NMF_PANIC("PANIC %s->bindToUser %s\n", GetComponentName(), name) ;

				++iNbInput;
			}
			else
			{
				// ReportError(
			}
			break;
		case OMX_DirOutput://bind fillthisbuffer, fillbufferdone for each output port
			if (iNbOutput < MAX_PORTS_COUNT)
			{
				sprintf(name, "fillthisbuffer[%d]", Index);
				error = m_pNmfProcWrp->bindFromUser(name, pPort->getBufferCountActual(), &mIfillThisBuffer[Index]);
				if (error != NMF_OK)
					NMF_PANIC("PANIC %s->bindFromUser %s\n", GetComponentName(), name) ;

				sprintf(name, "fillbufferdone[%d]", Index);
				error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),
					m_pNmfProcWrp,
					name,
					this->getFillBufferDoneCB(),
					pPort->getBufferCountActual());
				if (error != NMF_OK)
					NMF_PANIC("PANIC %s->bindToUser %s\n", GetComponentName(), name) ;
				++iNbOutput;
			}
			else
			{
				// ReportError(
			}

			break;
		default:
			break;
		}
	}
	// Configure the number of ports
	m_IConfig.setTargetComponent(mENSComponent.getPortCount() , &mENSComponent);
	OUTR(" ", (omx_error));
	return omx_error;
}

/**
@brief applyConfig is called during loaded to idle transition by the ENS
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStructure)
//*************************************************************************************************************
{ //Call the process->setConfig 
	IN0("\n");
	if(pConfigStructure != 0)
	{
		m_IConfig.setConfig((int)nConfigIndex, (void*) pConfigStructure);
	}
	else
	{// TODO : handle error her
	}
	OUT0("\n");
	return OMX_ErrorNone;
}

/**
@brief Usually called by the processing component via nmf for backfitting info
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::setConfigFromProcessing(OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStructure)
//*************************************************************************************************************
{ // set config from processor to component_proxy
	IN0("\n");
	OMX_ERRORTYPE res;
	if ((nConfigIndex < OMX_IndexComponentStartUnused) && (nConfigIndex < (OMX_INDEXTYPE)OMX_CommandMax))
	{ //It's a set param
		res=ENS_Component::SendCommand(&mENSComponent, (OMX_COMMANDTYPE)nConfigIndex, (OMX_U32 )pConfigStructure, 0);
	}
	else
	{ //Make the set config
		res=mENSComponent.setConfig(nConfigIndex, pConfigStructure);
	}
	OUTR(" ", res);
	return res;
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::doSpecificEventHandler_cb(OMX_EVENTTYPE _event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferredEvent)
//*************************************************************************************************************
{
	IN0("\n");
	switch ((unsigned int)_event)
	{
	case OMX_EventCmdComplete:
		if (nData1 == OMX_CommandStateSet)
		{ //Only idle and executing are trapped!
			MSG3("OMX_EventCmdComplete %s.%s(%s)\n", GetComponentName(), GetStringOmxCommand(nData1), GetStringOmxState(nData2) );
		}
		else // encapsulate OMX_EventPortSettingsChanged & OMX_EventBufferFlag
			MSG3("OMX_EventCmdComplete %s.%s(%d)\n", GetComponentName(), GetStringOmxCommand(nData1), (int)nData2);
		break;
	case OMX_EventError:
		MSG1("OMX_EventError %ld\n", nData1); break;
	case OMX_EventMark:
		MSG0("OMX_EventMark\n"); break;
	case OMX_EventPortSettingsChanged:
		MSG1("OMX_EventPortSettingsChanged(%d)\n", (int)nData1); break;
	case OMX_EventBufferFlag:
		if(nData2 == OMX_BUFFERFLAG_EOS)
		{
			MSG1("OMX_EventBufferFlag('EOS') on port %ld\n", nData1);
		}
		else
			MSG2("OMX_EventBufferFlag(%ld) on port %ld\n", nData2, nData1);
		break;
	case OMX_EventResourcesAcquired:
		MSG0("OMX_EventResourcesAcquired\n"); break;
	case OMX_EventComponentResumed:
		MSG0("OMX_EventComponentResumed\n"); break;
	case OMX_EventDynamicResourcesAvailable:
		MSG0("OMX_EventDynamicResourcesAvailable\n"); break;
	case OMX_EventPortFormatDetected:
		MSG0("OMX_EventPortFormatDetected\n"); break;
	case OMX_EventIndexSettingChanged:
		MSG0("OMX_EventIndexSettingChanged\n"); break;
	case OMX_EventCmdReceived:
		if (nData1 == OMX_CommandStateSet)
		{ //Only idle and executing are trapped!
			MSG3("OMX_EventCmdReceived %s.%s(%s)\n", GetComponentName(), GetStringOmxCommand(nData1), GetStringOmxState(nData2) );
		}
		else 
			MSG3("OMX_EventCmdReceived %s.%s(%ld)\n", GetComponentName(), GetStringOmxCommand(nData1), (long)nData2);
		break;
	default:
		MSG3("unknown OMX_Event(%d, %d, %d)\n", _event, (int)nData1, (int)nData2); break;
	}
	// Don't send any more this command because it duplicate OMX_EventBufferFlag messages
	//((OpenMax_Component*)&mENSComponent)->eventHandler(_event, nData1, nData2) ;
	OUT0("\n");
	return OMX_ErrorNone;
}

/**
Call by the ens framework when a command port is sending.
Note that Enable/Disable port does NOT hit this function
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
//*************************************************************************************************************
{
	switch(eCmd)
	{
	case OMX_CommandStateSet: // only transition from idle -> execute and execute --> idle are visible!!!
		MSG3(" %s.SetState( %s=%lu)\n", GetComponentName(), GetStringOmxState(nData), nData);
		break;
	case OMX_CommandFlush:
		MSG2(" %s.Flush(%lu)\n", GetComponentName(), nData);
		break;
	case OMX_CommandPortDisable:
		MSG2("%s.DisablePort(%lu)\n", GetComponentName(), nData);
		break;
	case OMX_CommandPortEnable:
		MSG2("%s.EnablePort(%lu)\n", GetComponentName(), nData);
		break;
	case OMX_CommandMarkBuffer:
		MSG2("%s.MarkBuffer(%lu)\n", GetComponentName(), nData);
		break;
	default:
		MSG3("%s.Unknow specific command %d- %lu\n", GetComponentName(), eCmd, nData);
		break;
	}
	return OMX_ErrorNone;
}

/**
@brief Called when the processing is ready to be started
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::start()
//*************************************************************************************************************
{ // Call during Loaded to Idle transition
	IN0("\n");
	m_pNmfProcWrp->start();
	OUT0("\n");
	return OMX_ErrorNone;
}

/**
@brief Called when the processing is ready to be stopped
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::stop()
//*************************************************************************************************************
{// Call Deinstantiation of component
	IN0("\n");
	m_pNmfProcWrp->stop();
#if 0
	//ER355733 Try to fix ens bug that doesn't reset bufferSupplier type 
	PortVideo* pPort;
	for (unsigned int Index=0; Index< mENSComponent.getPortCount(); ++Index)
	{
		pPort = (PortVideo*)mENSComponent.getPort(Index);
		if (pPort->getBufferSupplier() !=OMX_BufferSupplyUnspecified)
		{
			MSG3("wwwwwwwwwwwwwwwwwwwwwwwwwwww    %s.port[%d] reset state(%d) to OMX_BufferSupplyUnspecified\n", GetComponentName(), Index, pPort->getBufferSupplier());
			pPort->setBufferSupplier(OMX_BufferSupplyUnspecified);
		}
	}
#endif
	OUT0("\n");
	return OMX_ErrorNone;
}

/**
@brief Configure the processing component by sending all relevant info 
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::configure()
//*************************************************************************************************************
{ // Send info to the host part
	IN0("\n"); 
	PortOther * pPort; //
	OMX_ERRORTYPE result=OMX_ErrorNone;
	for (unsigned int portIndex=0; portIndex< mENSComponent.getPortCount(); ++portIndex)
	{ //Iterate on each port
		pPort = (PortOther*)mENSComponent.getPort(portIndex);
		// Normally we could remove following function call
		m_IParam.setParam(
			portIndex,
			pPort->getBufferCountActual(),
			pPort->getDirection(),
			pPort->isBufferSupplier(),
			pPort->getPortIndex(),
			pPort->getParamPortDefinition().format.video.nFrameWidth,
			pPort->getParamPortDefinition().format.video.nFrameHeight,
			pPort->getParamPortDefinition().format.video.eColorFormat,
			pPort->getParamPortDefinition().format.video.nStride);

		//and use only this one
		//do the same but this time with the openmax structure
		OMX_PARAM_PORTDEFINITIONTYPE* portdef = new OMX_PARAM_PORTDEFINITIONTYPE;
		*portdef = pPort->getParamPortDefinition();
		m_IParam.setParameter(OMX_IndexParamPortDefinition, portdef);
	}

	// Call User Configure to make default initialisation
	OpenMax_Component *pComp= GetOpenMax_Component();
	if (pComp!=NULL)
	{
		int res=pComp->NMFConfigure(); 
		if (res!=S_OK)
		result=OMX_ErrorUndefined;
	}

	OUTR(" ", result);
	return result;
}

/**
@brief Configure the processing component by sending all relevant info 
*/
WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::deInstantiate()
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE OmxError=OMX_ErrorNone;
	t_nmf_error nmf_err = NMF_OK;
	OMX_HANDLETYPE handle = mENSComponent.getOMXHandle();

	nmf_err = m_pNmfProcWrp->unbindFromUser("sendcommand");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: Imageproc unbindfromUser sendcommand 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}
	nmf_err = m_pNmfProcWrp->unbindFromUser("fsminit");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: Imageproc unbindfromUser fsminit 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}
#if (SYNCHRONOUS ==0)
	nmf_err |= m_pNmfProcWrp->unbindFromUser("Config");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: Imageproc unbindfromUser Config 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}

	nmf_err = m_pNmfProcWrp->unbindFromUser("Param");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: Imageproc unbindfromUser Param 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}
#endif
	nmf_err |= EnsWrapper_unbindToUser(handle, m_pNmfProcWrp, "proxy");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: m_pNmfProcWrp unbindToUser proxy 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}

	nmf_err |= EnsWrapper_unbindToUser(handle, m_pNmfProcWrp, /* "get_config" */ "ToOMXComponent");
	if (nmf_err != NMF_OK)
	{
		MSG1("Error: m_pNmfProcWrp unbindToUser ToOMXComponent 0x%x\n", nmf_err);
		OmxError=OMX_ErrorHardware;
	}

	char name[20]; //reserve some char for formatting callback name
	PortVideo* pPort;
	for (unsigned int Index=0; Index< mENSComponent.getPortCount(); ++Index)
	{
		pPort = (PortVideo*)mENSComponent.getPort(Index);
		switch(pPort->getDirection())
		{
		case OMX_DirInput: //bind emptythisbuffer, emptybufferdone for each input port
			sprintf(name, "emptythisbuffer[%u]", Index);
			nmf_err |= m_pNmfProcWrp->unbindFromUser(name);
			sprintf(name, "emptybufferdone[%u]", Index);
			nmf_err |= EnsWrapper_unbindToUser(handle, m_pNmfProcWrp, name);
			break;
		case OMX_DirOutput: //bind emptythisbuffer, emptybufferdone for each input port
			sprintf(name, "fillthisbuffer[%u]", Index);
			nmf_err |= m_pNmfProcWrp->unbindFromUser(name);
			sprintf(name, "fillbufferdone[%u]", Index);
			nmf_err |= EnsWrapper_unbindToUser(handle, m_pNmfProcWrp, name);
			break;
		default:
			WARNING(0);
			break;
		}
	}
	if (nmf_err != NMF_OK)
	{
		OmxError=OMX_ErrorHardware;
	}

	m_pNmfProcWrp->destroy(); 
	openmax_processor_wrpDestroy(m_pNmfProcWrp);
	m_pProcessor=NULL; // for releasing the 'processor'
	OUTR(" ", (OmxError));
	return OmxError;
}

#ifdef WORKSTATION 
//Not arm target --> x86-linux
//Basic implementation without regarding type of buffers
WRAPPER_OPENMAX_API  OMX_ERRORTYPE OpenMax_Proxy::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo)
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	MSG4("OpenMax_Proxy[%s]::useBuffer[%lu] buffer=%lu Header=%p\n", GetComponentName(), nPortIndex, nBufferIndex, pBufferHdr);
	err = useBufferHeap(nPortIndex, nBufferIndex, pBufferHdr, portPrivateInfo);
	OUTR(" ", (err));
	return err;
}

WRAPPER_OPENMAX_API  OMX_ERRORTYPE OpenMax_Proxy::useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{
	IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	int portNb;
	if (dir == OMX_DirInput)
		portNb=pBufferHdr->nInputPortIndex;
	else
		portNb=pBufferHdr->nOutputPortIndex;

	MSG4("OpenMax_Proxy[%s]::useBufferHeader port %d dir=%u Header=%p\n", GetComponentName(), portNb, dir, pBufferHdr);
	err = useBufferHeaderHeap(dir, pBufferHdr);
	OUTR(" ", (err));
	return err;
}

WRAPPER_OPENMAX_API  OMX_ERRORTYPE OpenMax_Proxy::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo)
//*************************************************************************************************************
{
	IN0("\n");
	MSG3("Allocate buffer PortIndex %lu, nBufferIndex=%lu, nSizeBytes %lu\n", nPortIndex, nBufferIndex, nSizeBytes);
	OMX_ERRORTYPE err = OMX_ErrorNone;
	err = allocateBufferHeap(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);
	++m_NbPortAllocated[nPortIndex];
	OUTR("", (err));
	return err;
}

WRAPPER_OPENMAX_API  OMX_ERRORTYPE OpenMax_Proxy::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo)
//*************************************************************************************************************
{
	IN0("\n");
	MSG3("freeBuffer nPortIndex %lu- nBufferIndex %lu bBufferAllocated=%s\n", nPortIndex, nBufferIndex, bBufferAllocated==true?"true":"false");
	// WOM_BREAKPOINT;
	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (m_NbPortAllocated[nPortIndex] > 0)
	{
		err = freeBufferHeap(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
		--m_NbPortAllocated[nPortIndex];
	}
	OUTR("", (err));
	return err;
}
#else
#define USE_CAMERA_BUFFER 0

WRAPPER_OPENMAX_API  OMX_ERRORTYPE OpenMax_Proxy::allocateBuffer(
	OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes,
	OMX_U8 **ppData, void **ppBufferMetaData, void **portPrivateInfo)
//*************************************************************************************************************
{
	IN0("\n");
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
	OMX_ERRORTYPE error = OMX_ErrorNone;

	OMX_PORTDOMAINTYPE domain;
	OMX_PARAM_PORTDEFINITIONTYPE   portdef;
	getOmxIlSpecVersion(&portdef.nVersion);
	portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	port->getParameter(OMX_IndexParamPortDefinition, &portdef);
	domain= portdef.eDomain;

	if ( (domain == OMX_PortDomainVideo) /*&& (pTunneledPort !=NULL)*/)
	{
		OMX_BOOL isCached=OMX_FALSE; // For the moment non cachable OMX_TRUE
		error = allocateBufferVisual(nPortIndex, nBufferIndex, nSizeBytes, ppData, 0, portPrivateInfo, isCached);
		if (error != OMX_ErrorNone) 
		{
			ReportError(error, "Cannot allocateBufferVisual[%d] (video)", nPortIndex);
			return error;
		}
		/* add a SharedBuffer tied to allocated buffers, because for HSMCamera we need to exchange Buffers
		* with DSP. TODO: other IFM-based components don't all need this, I should make this optional */
		OMX_U32 bufPhysicalAddr; 
		error = ((MMHwBuffer *)(*portPrivateInfo))->PhysAddress((OMX_U32)*ppData, nSizeBytes, bufPhysicalAddr);
		if (error != OMX_ErrorNone)
		{
			ReportError(error, "Cannot PhysAddress");
			return error;
		}
#if USE_CAMERA_BUFFER
		SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle() /* RM_NMFD_HWPIPESIA RM_NMFD_PROCSIA */, nSizeBytes, *ppData, bufPhysicalAddr, 0, NULL, error);
		if (sharedBuf == 0)
		{
			ReportError(error, "Cannot create SharedBuffer[%d] (video)", nPortIndex);
			freeBufferVisual(nPortIndex, nBufferIndex, OMX_TRUE, NULL, *portPrivateInfo);
			return OMX_ErrorInsufficientResources;
		}
		if (error != OMX_ErrorNone)
		{
			ReportError(error, "error in SharedBuffer[%d] (video)", nPortIndex);
			freeBufferVisual(nPortIndex, nBufferIndex, OMX_TRUE, NULL, *portPrivateInfo);
			return error;
		}
		*portPrivateInfo = (void *)sharedBuf;
#endif
		// Initialize extradata segment to 0.
		unsigned int BuffSize = (unsigned int) (portdef.format.video.nStride * portdef.format.video.nFrameHeight /** getPixelDepth(portdef.format.video.eColorFormat)*/);
		if (BuffSize < nSizeBytes)
		{
			/*for (; BuffSize < nSizeBytes; BuffSize++)
			{
				(*ppData)[BuffSize] = 0;
			}rt*/
			// Best replacment
			//memset(ppData+ BuffSize, 0, nSizeBytes-BuffSize);
		}
		++m_NbPortAllocated[nPortIndex];
	}
	else 
	{ /* Other ports don't need visual buffers */
		error = allocateBufferHeap(nPortIndex, nBufferIndex, nSizeBytes, ppData, ppBufferMetaData, portPrivateInfo);
		if (error != OMX_ErrorNone)
		{
			ReportError(error, "Cannot allocateBufferHeap");
			return error;
		}
		++m_NbPortAllocated[nPortIndex];
	}
	MSG1("pBuffer = 0x%p\n", *ppData);
	MSG1("pBufferMetaData = 0x%p\n", *ppBufferMetaData);
	OUTR(" ", error);
	return error;
}


WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::useBufferHeader( OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{
	OMX_PORTDOMAINTYPE domain;
	ENS_Port * port;
	if(!pBufferHdr)
	{
		ReportError(OMX_ErrorBadParameter, "Buffer header is null");
		return OMX_ErrorBadParameter;
	}
	if(dir==OMX_DirOutput) 
		port = mENSComponent.getPort(pBufferHdr->nOutputPortIndex);
	else 
		port = mENSComponent.getPort(pBufferHdr->nInputPortIndex);

	domain= port->getDomain();
	if(domain == OMX_PortDomainVideo)
	{
#if USE_CAMERA_BUFFER
		SharedBuffer *sharedBuf = 0;

		if(dir == OMX_DirInput) 
		{
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
		} else 
		{
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
		}
		if(!sharedBuf)
		{
			ReportError(OMX_ErrorBadParameter, "Cannot retreive buffer address");
			return OMX_ErrorBadParameter;
		}
		return sharedBuf->setOMXHeader(pBufferHdr);

#endif
	}
	else
	{
	}
	return OMX_ErrorNone;
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::freeBuffer(
	OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo)
//*************************************************************************************************************
{
	IN0("\n");
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
	OMX_PORTDOMAINTYPE domain;
	domain= port->getDomain();
	if(domain == OMX_PortDomainVideo)
	{
#if USE_CAMERA_BUFFER
		//will free depending on bBufferAllocated
		OMX_ERRORTYPE error;
		ENS_Port *port = mENSComponent.getPort(nPortIndex);
		MMHwBuffer *sharedChunk = (MMHwBuffer *)port->getSharedChunk();
		if (sharedChunk)
		{
			if (nBufferIndex == port->getBufferCountActual()-1)
			{
				if (bBufferAllocated) 
				{
					error = MMHwBuffer::Destroy(sharedChunk);
					if (error !=OMX_ErrorNone)
					{
						ReportError(error, "Cannot delete buffer");
						DBC_ASSERT(error == OMX_ErrorNone);
					}
				} 
				else 
				{
					error = MMHwBuffer::Close(sharedChunk);
					if (error !=OMX_ErrorNone)
					{
						ReportError(error, "Cannot close buffer");
						DBC_ASSERT(error == OMX_ErrorNone);
					}
				}
				port->setSharedChunk(0);
			}
		}//
		else 
		{ //error case should be managed
			// DBC_ASSERT(0);
		}
		/* whatever : delete the sharedBuffer as in both case (allocation or use) it is allocated*/
		delete (SharedBuffer *)portPrivateInfo;
#else
		if (m_NbPortAllocated[nPortIndex] > 0)
		{
			freeBufferVisual(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
			--m_NbPortAllocated[nPortIndex];
		}
#endif
	}
	else /* clock domain */
	{
		if (m_NbPortAllocated[nPortIndex] > 0)
		{
			freeBufferHeap(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
			--m_NbPortAllocated[nPortIndex];
		}
	}
	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}



WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::useBuffer(
	OMX_U32 nPortIndex,
	OMX_U32 nBufferIndex,
	OMX_BUFFERHEADERTYPE* pBufferHdr,
	void **portPrivateInfo)
//*************************************************************************************************************
{	/* do not call useBufferVisual as it is implemented for Video which does not do the same than imaging */
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
	OMX_PORTDOMAINTYPE domain;

	domain= port->getDomain();
	if(domain == OMX_PortDomainVideo)
	{
#if USE_CAMERA_BUFFER
		OMX_ERRORTYPE error;
		void *bufferAllocInfo = 0;
		OMX_U8 *pBuffer;
		bufferAllocInfo = (void *)(port->getSharedChunk()); // bufferAllocINfo is now a MMHWBUffer
		if(!bufferAllocInfo) DBC_ASSERT(0);
		error = ((MMHwBuffer *)bufferAllocInfo)->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
		if (error != OMX_ErrorNone) 
		{
			ReportError(error, "Cannot AddBufferInfo");
			return error;
		}

		pBuffer = pBufferHdr->pBuffer;
		/* it is freed when freeBuffer is called*/
		OMX_U32 bufPhysicalAddr;

		error = ((MMHwBuffer *)bufferAllocInfo)->PhysAddress((OMX_U32)pBuffer, pBufferHdr->nAllocLen, bufPhysicalAddr);
		if (error != OMX_ErrorNone)
		{
			ReportError(error, "Cannot get physical address");
			return error;
		}

		SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(), pBufferHdr->nAllocLen, pBuffer, bufPhysicalAddr, NULL, NULL, error);
		// actually we dont care about bufferAllocInfo in case where we are buffer supplier but we do care in case where we are not buffer supplier, thus keep it anyway in both cases.
		if (sharedBuf == 0) 
		{
			ReportError(error, "Cannot create shared buffer");
			return OMX_ErrorInsufficientResources;
		}
		if (error != OMX_ErrorNone) 
		{
			ReportError(error, "Cannot create shared buffer");
			return error;
		}

		sharedBuf->setOMXHeader(pBufferHdr);
		*portPrivateInfo = sharedBuf;
#endif
	}
	else
	{
		/* do nothing: useBufferHeap is empty*/
		*portPrivateInfo = NULL;

	}
	return OMX_ErrorNone;
}
#endif // end of WORKSTATION


WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{ //By default just call the base class
	MSG2("::emptyThisBuffer sent to Port[%u](BufferHdr=%p)\n", (unsigned int)pBufferHdr->nInputPortIndex, pBufferHdr);
	OMX_ERRORTYPE err=NmfHost_ProcessingComponent::emptyThisBuffer(pBufferHdr);
	return(err);
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{ //By default just call the base class
	MSG2("::fillThisBuffer sent to Port[%u](BufferHdr=%p)\n", (unsigned int)pBufferHdr->nOutputPortIndex, pBufferHdr);
	OMX_ERRORTYPE err=NmfHost_ProcessingComponent::fillThisBuffer(pBufferHdr);
	return(err);
}

WRAPPER_OPENMAX_API OMX_ERRORTYPE OpenMax_Proxy::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
//*************************************************************************************************************
{ //By default just call the base class
	MSG2("sendCommand %s(%ld)\n", GetStringOmxCommand(eCmd), nData);
	OMX_ERRORTYPE err=NmfHost_ProcessingComponent::sendCommand(eCmd, nData);
	return(err);
}

WRAPPER_OPENMAX_API void OpenMax_Proxy::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{
	MSG2("::doSpecificEmptyBufferDone_cb[%u](BufferHdr=%p)\n", (unsigned int)pBufferHdr->nInputPortIndex, pBufferHdr);
}

WRAPPER_OPENMAX_API void OpenMax_Proxy::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{
	MSG2("::doSpecificFillBufferDone_cb[%u](BufferHdr=%p)\n", (unsigned int)pBufferHdr->nOutputPortIndex, pBufferHdr);
}

