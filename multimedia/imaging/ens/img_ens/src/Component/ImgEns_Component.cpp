/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "ImgEns_Shared.h"
#include <stdio.h>
#include <string.h>
#include "omxil/OMX_Core.h"
#include "ImgEns_Fsm.h"
#include "ImgEns_Trace.h"
#include "ImgEns_Component.h"
#include "ImgEns_Component_Fsm.h"
#include "ImgEns_Port.h"
#include "ImgEns_ProcessingComponent.h"

#include "ImgEns_Index.h"
#include "mmhwbuffer.h"

#define OMXCOMPONENT "ImgEns_COMPONENT"
// Declaration of the global variable for debugging
#define DECLARE_AUTOVAR

#define UNUSED(a)  if (a) {};


#ifndef _MSC_VER
	#include "osi_trace.h"
	//#include "OstTraceDefinitions_ste.h"
	#include "OpenSystemTrace_ste.h"

	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "ens_proxy_ENS_Component_ENS_ComponentTraces.h"
	#endif //OST_TRACE_COMPILER_IN_USE
	#define OMX_API_TRACE
	#define MAX_OST_SIZE 56

#else
	// #define OMX_API_TRACE
	#define TRACE_OMX_API     "ImgEns_Component"
	#define TRACE_OMX_BUFFER  "ImgEns_Component"
#endif

static size_t CptCounter=0;


extern const OMX_CALLBACKTYPE g_DefaultCallback;  // Default function callback

ImgEns_Component::ImgEns_Component(const char *name, unsigned int instanceNb)
: m_FamilyName(name), mRoleCount(0), mCompFsm(0)/*, mVersion(), mPortCount(0)*/
, mPorts(0)
, mCallbacks(g_DefaultCallback)
, mAppData(0)
, mProcessingComponent(0)
, mPreemptionState(Preempt_None), mTraceBuilder(0)
, mResourceConcealmentForbidden(OMX_TRUE), mGroupPriority(0), mGroupID(0)
, mSuspensionPolicy(OMX_SuspensionDisabled), mPreemptionPolicy(OMX_TRUE)
, mResourceSuspended(false), mBufferSharingEnabled(OMX_FALSE)
{
	mActiveRole[0] = 0;
	if (name==NULL)
	{ // init default parameters
		instanceNb = ++CptCounter;
		name       ="ImgEns_Component";
	}
	if (instanceNb > 0)
	{
		m_Name.Format("%s%d", name, instanceNb);
	}
	else
		m_Name = name;
}

ImgEns_Component::~ImgEns_Component(void)
{
	if (mProcessingComponent)
	{
		delete mProcessingComponent;
	}

	for (unsigned int i = 0; i < mRoleCount; i++)
	{
		delete[] mRoles[i];
	}

	for (unsigned int i = 0; i < mPortCount; i++)
	{
		IMGENS_ASSERT(mPorts[i] != 0);
		if (mPorts[i])
		{
			delete mPorts[i];
			mPorts[i] = NULL;
		}
	}
	delete[] mPorts;
	mPorts= NULL;
	delete mCompFsm;
	mCompFsm=NULL;
	if (mTraceBuilder)
	{
		delete mTraceBuilder;
		mTraceBuilder = NULL;
	}
}



OMX_ERRORTYPE ImgEns_Component::eventHandlerCB(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	if (mCallbacks.EventHandler)
	{ // Send to OMX user callback
		return mCallbacks.EventHandler(getOMXHandle(), mAppData, eEvent, nData1, nData2, pEventData); // Propagate the callback
	}
	else
		return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Component::emptyBufferDoneCB( OMX_IN OMX_BUFFERHEADERTYPE* pBuffer) 
{
	if (mCallbacks.EmptyBufferDone)
		return mCallbacks.EmptyBufferDone(getOMXHandle(), mAppData, pBuffer);
	else
		return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEns_Component::fillBufferDoneCB( OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer) 
{
	if (mCallbacks.FillBufferDone)
		return mCallbacks.FillBufferDone(getOMXHandle(), mAppData, pBuffer);
	else
		return OMX_ErrorNone;
}



bool ImgEns_Component::resourcesSuspended(void) const
{
	return mResourceSuspended;
}

void ImgEns_Component::resourcesSuspended(bool bFlag)
{
	mResourceSuspended = bFlag;
}

 OMX_ERRORTYPE ImgEns_Component::SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR /*pCmdData*/)
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::SendCommand cmd = %d nParam = %d", (OMX_U32)Cmd, nParam);
#endif
	switch (Cmd)
	{
	case OMX_CommandStateSet:
		{
			OMX_ERRORTYPE error;
			ImgEns_CmdSetStateEvt evt((OMX_STATETYPE) nParam);
			error = Dispatch(&evt);
			if (error != OMX_ErrorNone)
				return error;
			if ((OMX_STATETYPE) nParam == OMX_StateLoaded)
			{ // will not be sent by processingComponent when transitionning to loaded state.

				eventHandlerCB(( OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateLoaded, 0);
			}
			return OMX_ErrorNone;
		}
	case OMX_CommandPortDisable:
		{
			ImgEns_CmdPortEvt evt(OMX_DISABLE_PORT_SIG, nParam);
			return(DispatchToPort(nParam, &evt) );
		}
	case OMX_CommandPortEnable:
		{
			ImgEns_CmdPortEvt evt(OMX_ENABLE_PORT_SIG, nParam);
		return(DispatchToPort(nParam, &evt));
		}
	case OMX_CommandFlush:
		{
			ImgEns_CmdPortEvt evt(OMX_FLUSH_PORT_SIG, nParam);
		return(DispatchToPort(nParam, &evt));
		}
	default:
		return OMX_ErrorNotImplemented;
	}
}

 OMX_ERRORTYPE ImgEns_Component::SetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure)
{
	if (pComponentParameterStructure == 0)
	{
		IMGENS_ASSERT(0);
		return OMX_ErrorBadParameter;
	}
#ifdef OMX_API_TRACE
	// select one parameter structure to get back nSize
	OMX_U32 size =static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>(pComponentParameterStructure)->nSize;
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::SetParameter nIndex = %d, size = %d", (OMX_S32)nIndex, size);
	// size is limited to MAX_OST_SIZE bytes
	for (OMX_U32 i = 0; i < size; i += MAX_OST_SIZE)
	{
		OstTraceFiltInstData(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::SetParameter ParameterStructure = %{int8[]}", (OMX_U8 *)((OMX_U8 *)pComponentParameterStructure + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
	}
#endif
	ImgEns_CmdSetParamEvt evt(nIndex, pComponentParameterStructure);
	if (isPortSpecificParameter(nIndex))
	{	//Dispacth event on port state machine
		ImgEns_PORT_INDEX_STRUCT *portIdxStruct =static_cast<ImgEns_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
		return(DispatchToPort(portIdxStruct->nPortIndex, &evt));
	}
	else
	{//Dispacth event on component state machine
		return Dispatch(&evt);
	}
}

 OMX_ERRORTYPE ImgEns_Component::GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure)
{
	if (pComponentParameterStructure == 0)
	{
		return OMX_ErrorBadParameter;
	}
#ifdef OMX_API_TRACE
	OstTraceFiltInst1(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::GetParameter nIndex = %d", (OMX_U32)nIndex);
#endif

	ImgEns_CmdGetParamEvt evt(nIndex, pComponentParameterStructure);

	if (isPortSpecificParameter(nIndex))
	{//Dispacth event on port state machine
		ImgEns_PORT_INDEX_STRUCT *portIdxStruct = static_cast<ImgEns_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
		return DispatchToPort(portIdxStruct->nPortIndex, &evt);
	}
	else
	{
		//Dispacth event on component state machine
		return Dispatch(&evt);
	}
}

 OMX_ERRORTYPE ImgEns_Component::SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
{
	if (pComponentConfigStructure == 0)
	{
		return OMX_ErrorBadParameter;
	}
#ifdef OMX_API_TRACE
	// select one config structure to get back nSize
	OMX_U32 size = static_cast<OMX_OTHER_CONFIG_STATSTYPE *>(pComponentConfigStructure)->nSize;
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::SetConfig index = %d, size = %d", (OMX_S32)nIndex, size);
	// size is limited to MAX_OST_SIZE bytes
	for (OMX_U32 i = 0; i < size; i += MAX_OST_SIZE)
	{
		OstTraceFiltInstData(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::SetConfig ParameterStructure = %{int8[]}", (OMX_U8 *)((OMX_U8 *)pComponentConfigStructure + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
	}
#endif

	ImgEns_CmdSetConfigEvt evt(nIndex, pComponentConfigStructure);
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
{
	if (pComponentConfigStructure == 0)
	{
		return OMX_ErrorBadParameter;
	}

#ifdef OMX_API_TRACE
	OstTraceFiltInst1(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::GetConfig nIndex = %d", (OMX_U32)nIndex);
#endif
	ImgEns_CmdGetConfigEvt evt(nIndex, pComponentConfigStructure);
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType)
{
#ifdef OMX_API_TRACE
	OstTraceFiltInstData(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::GetExtensionIndex %{int8[]}", (OMX_U8 *)cParameterName, 128);
#endif
	ImgEns_CmdGetExtIdxEvt evt(cParameterName, pIndexType);
	return Dispatch(&evt);
}

OMX_ERRORTYPE ImgEns_Component::GetState(OMX_STATETYPE* pState)
{
	*pState = omxState();
#ifdef OMX_API_TRACE
	OstTraceFiltInst1(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::GetState => state = %d", (OMX_U32)*pState);
#endif
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE ImgEns_Component::ComponentTunnelRequest( OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst3(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::ComponentTunnelRequest Port = %d <=> Tunneled Component-Port = 0x%x-%d", nPort, (OMX_U32)hTunneledComp, nTunneledPort);
#endif
	ImgEns_CmdTunnelRequestEvt evt(hTunneledComp, nTunneledPort, pTunnelSetup);
	return(DispatchToPort(nPort, &evt));
}

OMX_ERRORTYPE ImgEns_Component::SetCallbacks(const OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData)
{
	const ImgEns_CmdSetCbEvt evt(pCallbacks, pAppData);
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (pBuffer == 0)
	{
		return OMX_ErrorBadParameter;
	}
	ImgEns_CmdEmptyThisBufferEvt evt(pBuffer);
#ifdef OMX_API_TRACE
	OstTraceFiltInst4(TRACE_OMX_BUFFER, "ImgEns_Proxy: ImgEns_Component::EmptyThisBuffer nFilledLen %d, nOffset %d, nFlags %d (port %d)", pBuffer->nFilledLen, pBuffer->nOffset, pBuffer->nFlags, pBuffer->nInputPortIndex);
	// size is limited to MAX_OST_SIZE bytes
	OMX_U32 size = pBuffer->nFilledLen;
	for (OMX_U32 i = 0; i < size; i += MAX_OST_SIZE)
	{
		OstTraceFiltInstData(TRACE_OMX_BUFFER, " ENS_PROXY: ImgEns_Component::EmptyThisBuffer   data = %{int8[]}", (OMX_U8 *)((OMX_U8 *)(pBuffer->pBuffer) + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
	}
#endif
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::FillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst2(TRACE_OMX_BUFFER, "ImgEns_Proxy: ImgEns_Component::FillThisBuffer nAllocLen = %d (port %d)", pBuffer->nAllocLen, pBuffer->nOutputPortIndex);
#endif
	ImgEns_CmdFillThisBufferEvt evt(pBuffer);
	if (pBuffer == 0)
	{
		return OMX_ErrorBadParameter;
	}
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::UseBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer)
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::UseBuffer 0x%x (port %d)", (OMX_U32)pBuffer, nPortIndex);
#endif
	ImgEns_CmdUseBufferEvt evt(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::AllocateBuffer( OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
#ifdef OMX_API_TRACE
		OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::AllocateBuffer size =  %d (port %d)", nSizeBytes, nPortIndex);
#endif
		ImgEns_CmdAllocBufferEvt evt(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes);
		return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer) 
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::FreeBuffer 0x%x (port %d)", (OMX_U32)pBuffer, nPortIndex);
#endif
	ImgEns_CmdFreeBufferEvt evt(nPortIndex, pBuffer);
	return Dispatch(&evt);
}

 OMX_ERRORTYPE ImgEns_Component::UseEGLImage(OMX_BUFFERHEADERTYPE** /*ppBufferHdr*/, OMX_U32 /*nPortIndex*/, OMX_PTR /*pAppPrivate*/, void* /*eglImage*/)
{
	return OMX_ErrorNotImplemented;
}

 
OMX_ERRORTYPE ImgEns_Component::GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion,OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* /*pComponentUUID*/)
{
	IMGENS_ASSERT( strlen(getName()) < OMX_MAX_STRINGNAME_SIZE); // check, according to OMX specifications
	strncpy(pComponentName, getName(), OMX_MAX_STRINGNAME_SIZE-1);
	*pComponentVersion = getVersion();
	getImgOmxIlSpecVersion(pSpecVersion);
#ifdef OMX_API_TRACE
	OstTraceFiltInst1(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::GetComponentVersion => version = %x", pComponentVersion->nVersion);
#endif
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE ImgEns_Component::ComponentDeInit()
{
#ifdef OMX_API_TRACE
	OstTraceFiltInst0(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::ComponentDeInit");
#endif
	// delete of the component must be done in the wrapper
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE ImgEns_Component::ComponentRoleEnum(OMX_U8 *cRole, OMX_U32 nIndex)
{
	OMX_ERRORTYPE err = getRole(&cRole, nIndex);
#ifdef OMX_API_TRACE
	OstTraceFiltInst2(TRACE_OMX_API, "ImgEns_Proxy: ImgEns_Component::ComponentRoleEnum for index %d => error = %d", nIndex, (OMX_U32)err);
#endif
	return err;
}

 OMX_BOOL ImgEns_Component::isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const
{
	switch (nParamIndex)
	{
	case OMX_IndexParamCompBufferSupplier:
	case OMX_IndexParamPortDefinition:
		return OMX_TRUE;
	default:
		return OMX_FALSE;
	}
}

OMX_ERRORTYPE ImgEns_Component::DefaultSetConfigOrParameter(OMX_INDEXTYPE nIndex/**/, OMX_PTR pStructure/**/)
{
#ifndef _MSC_VER
	OMX_U32 aPortIndex = 0;
	// Important to call GetPortIndexExtension() in order to know if the index is correct or not
	if (MMHwBuffer::GetPortIndexExtension(nIndex, pStructure, aPortIndex)== OMX_ErrorNone)
	{
		if (aPortIndex >= getPortCount() || getPort(aPortIndex) == 0) 
		{
			return OMX_ErrorBadPortIndex;
		}
		MMHwBuffer *sharedChunk = 0;
		if (MMHwBuffer::Open(getOMXHandle(), nIndex, pStructure, sharedChunk)!= OMX_ErrorNone)
		{
			return OMX_ErrorUnsupportedIndex;
		}
		getPort(aPortIndex)->setSharedChunk(sharedChunk);
		return OMX_ErrorNone;
	}
#endif
	if (nIndex || pStructure)
	{}
	return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE ImgEns_Component::DefaultGetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const
{
#ifndef _MSC_VER
	OMX_U32 aPortIndex;
	// Important to call GetPortIndexExtension() in order to know if the index is correct or not
	if (MMHwBuffer::GetPortIndexExtension(nIndex, pStructure, aPortIndex)== OMX_ErrorNone)
	{
		if (aPortIndex >= getPortCount() || getPort(aPortIndex) == 0)
		{
			return OMX_ErrorBadPortIndex;
		}
		MMHwBuffer *pSharedChunk = getPort(aPortIndex)->getSharedChunk();
		if (pSharedChunk)
		{
			return pSharedChunk->GetConfigExtension(nIndex, pStructure, aPortIndex);
		}
		else
		{
			return OMX_ErrorBadPortIndex;
		}
	}
#endif
	if (nIndex || pStructure)
	{}
	return OMX_ErrorUnsupportedIndex;
}


OMX_ERRORTYPE ImgEns_Component::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
	switch (nParamIndex)
	{
	case OMX_IndexParamStandardComponentRole:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_COMPONENTROLETYPE) 
			OMX_PARAM_COMPONENTROLETYPE * param =(OMX_PARAM_COMPONENTROLETYPE *) pComponentParameterStructure;
			unsigned int i = 0;
			while (mActiveRole[i] != '\0' && i < OMX_MAX_STRINGNAME_SIZE)
			{
				param->cRole[i] = mActiveRole[i];
				i++;
			}
			param->cRole[i] = param->cRole[OMX_MAX_STRINGNAME_SIZE-1] = '\0';
			return OMX_ErrorNone;
		}
	case OMX_IndexParamAudioInit:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PORT_PARAM_TYPE);
			OMX_PORT_PARAM_TYPE *portparam = (OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
			OMX_BOOL audio_port_found = OMX_FALSE;
			portparam->nPorts = portparam->nStartPortNumber = 0;
			// search for audio ports
			for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
			{
				if (getPort(nPort)->getDomain() == OMX_PortDomainAudio)
				{
					if (!audio_port_found)
					{
						portparam->nStartPortNumber = nPort;
						audio_port_found = OMX_TRUE;
					}
					portparam->nPorts++;
				}
			}
			return OMX_ErrorNone;
		}

	case OMX_IndexParamVideoInit:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,
				OMX_PORT_PARAM_TYPE);
			OMX_PORT_PARAM_TYPE *portparam =
				(OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
			OMX_BOOL video_port_found = OMX_FALSE;
			portparam->nPorts = portparam->nStartPortNumber = 0;
			// search for video ports
			for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
			{
				if (getPort(nPort)->getDomain() == OMX_PortDomainVideo)
				{
					if (!video_port_found) 
					{
						portparam->nStartPortNumber = nPort;
						video_port_found = OMX_TRUE;
					}
					portparam->nPorts++;
				}
			}
			return OMX_ErrorNone;
		}

	case OMX_IndexParamImageInit: 
	{
		CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PORT_PARAM_TYPE);
		OMX_PORT_PARAM_TYPE *portparam =(OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
		OMX_BOOL image_port_found = OMX_FALSE;
		portparam->nPorts = portparam->nStartPortNumber = 0;
		// search for image ports
		for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
		{
			if (getPort(nPort)->getDomain() == OMX_PortDomainImage)
			{
				if (!image_port_found) 
				{
					portparam->nStartPortNumber = nPort;
					image_port_found = OMX_TRUE;
				}
				portparam->nPorts++;
			}
		}
		return OMX_ErrorNone;
	}

	case OMX_IndexParamOtherInit:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PORT_PARAM_TYPE);
			OMX_PORT_PARAM_TYPE *portparam =(OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
			OMX_BOOL other_port_found = OMX_FALSE;
			portparam->nPorts = portparam->nStartPortNumber = 0;
			// search for other ports
			for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
			{
				if (getPort(nPort)->getDomain() == OMX_PortDomainOther)
				{
					if (!other_port_found)
					{
						portparam->nStartPortNumber = nPort;
						other_port_found = OMX_TRUE;
					}
					portparam->nPorts++;
				}
			}
			return OMX_ErrorNone;
		}

	case OMX_IndexParamDisableResourceConcealment:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,
				OMX_RESOURCECONCEALMENTTYPE);
			OMX_RESOURCECONCEALMENTTYPE *pResource =
				(OMX_RESOURCECONCEALMENTTYPE *) pComponentParameterStructure;
			pResource->bResourceConcealmentForbidden =
				this->getResourceConcealmentForbidden();
			return OMX_ErrorNone;
		}

	case OMX_IndexParamPriorityMgmt:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,
				OMX_PRIORITYMGMTTYPE);
			OMX_PRIORITYMGMTTYPE *pResource =
				(OMX_PRIORITYMGMTTYPE *) pComponentParameterStructure;
			pResource->nGroupPriority = this->getGroupPriorityValue();
			pResource->nGroupID = this->getGroupPriorityId();
			return OMX_ErrorNone;
		}

	case OMX_IndexParamSuspensionPolicy:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,
				OMX_PARAM_SUSPENSIONPOLICYTYPE);
			OMX_PARAM_SUSPENSIONPOLICYTYPE *pResource =
				(OMX_PARAM_SUSPENSIONPOLICYTYPE *) pComponentParameterStructure;
			pResource->ePolicy = this->getSuspensionPolicy();
			return OMX_ErrorNone;
		}

	case OMX_IndexParamComponentSuspended:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_SUSPENSIONTYPE);
			OMX_PARAM_SUSPENSIONTYPE *pResource = (OMX_PARAM_SUSPENSIONTYPE *) pComponentParameterStructure;
			pResource->eType = ( (this->getPreemptionState() == Preempt_Partial) ? OMX_Suspended : OMX_NotSuspended);
			return OMX_ErrorNone;
		}

	default:
		return DefaultGetConfigOrParameter(nParamIndex, pComponentParameterStructure);
	}
}

 OMX_ERRORTYPE ImgEns_Component::setParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	switch (nParamIndex)
	{
	case OMX_IndexParamStandardComponentRole:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_COMPONENTROLETYPE);
			// TODO : check if role is supported !
			OMX_PARAM_COMPONENTROLETYPE *componentRole =(OMX_PARAM_COMPONENTROLETYPE *) pComponentParameterStructure;
			return setActiveRole(componentRole->cRole);
		}

	case OMX_IndexParamDisableResourceConcealment:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_RESOURCECONCEALMENTTYPE);
			OMX_RESOURCECONCEALMENTTYPE *pResource =(OMX_RESOURCECONCEALMENTTYPE *) pComponentParameterStructure;
			this->setResourceConcealmentForbidden( pResource->bResourceConcealmentForbidden);
			return OMX_ErrorNone;
		}

	case OMX_IndexParamPriorityMgmt:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PRIORITYMGMTTYPE);
			OMX_PRIORITYMGMTTYPE *pResource = (OMX_PRIORITYMGMTTYPE *) pComponentParameterStructure;
			this->setPriorityMgmt(pResource->nGroupPriority, pResource->nGroupID);
			return OMX_ErrorNone;
		}

	case OMX_IndexParamSuspensionPolicy:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_SUSPENSIONPOLICYTYPE);
			OMX_PARAM_SUSPENSIONPOLICYTYPE *pResource = (OMX_PARAM_SUSPENSIONPOLICYTYPE *) pComponentParameterStructure;
			this->setSuspensionPolicy(pResource->ePolicy);
			return OMX_ErrorNone;
		}

	default:
		return DefaultSetConfigOrParameter(nParamIndex, pComponentParameterStructure);
	}
}

 OMX_ERRORTYPE ImgEns_Component::getConfig(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const
{
	if (pStructure == 0)
	{
		return OMX_ErrorBadParameter;
	}
	switch ((int)nIndex)
	{
	case (OMX_INDEXTYPE)eEnsIndex_ConfigTraceSetting:
		{
			ImgEns_ConfigTraceSettingType *pConfig =static_cast<ImgEns_ConfigTraceSettingType *>(pStructure);
			Img_TraceBuilder * pTraceBuilder  = getTraceBuilder();
			ImgEns_TraceInfo * info           = pTraceBuilder->getSharedTraceInfoPtr();
			pConfig->nParentHandle            = info->parentHandle;
			pConfig->nTraceEnable             = info->traceEnable;
			return OMX_ErrorNone;
		}
	case OMX_IndexConfigPriorityMgmt:
		{
			CHECK_STRUCT_SIZE_AND_VERSION(pStructure, OMX_PRIORITYMGMTTYPE);
			OMX_PRIORITYMGMTTYPE *pResource = (OMX_PRIORITYMGMTTYPE *) pStructure;
			pResource->nGroupPriority       = this->getGroupPriorityValue();
			pResource->nGroupID             = this->getGroupPriorityId();
			return OMX_ErrorNone;
		}

	default:
		return DefaultGetConfigOrParameter(nIndex, pStructure);
	}
}

 OMX_ERRORTYPE ImgEns_Component::setConfig(OMX_INDEXTYPE nIndex, OMX_PTR pStructure)
{
	if (pStructure == 0)
	{
		return OMX_ErrorBadParameter;
	}

	switch ((int)nIndex)
	{
	case (OMX_INDEXTYPE)eEnsIndex_ConfigTraceSetting:
		{
			ImgEns_ConfigTraceSettingType *pConfig = static_cast<ImgEns_ConfigTraceSettingType *>(pStructure);
			Img_TraceBuilder * pTraceBuilder = getTraceBuilder();
			pTraceBuilder->setTraceZone(pConfig->nParentHandle, pConfig->nTraceEnable);
			return OMX_ErrorNone;
		}
	default:
		return DefaultSetConfigOrParameter(nIndex, pStructure);
	}
}

 OMX_ERRORTYPE ImgEns_Component::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
{	// default case: check this is the Shared Chunk Extension
	return MMHwBuffer::GetIndexExtension(cParameterName, pIndexType);
}

 OMX_ERRORTYPE ImgEns_Component::registerILClientCallbacks(const OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData)
{
	if ((pCallbacks == 0) || (pCallbacks->EventHandler == 0) )
	{
		return OMX_ErrorBadParameter;
	}
	mCallbacks = *pCallbacks;
	mAppData   = pAppData;
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE ImgEns_Component::getActiveRole(OMX_U8 * aActiveRole) const
{
	if (mActiveRole[0] != '\0')
	{
#if 1
		memcpy(aActiveRole, mActiveRole, OMX_MAX_STRINGNAME_SIZE);
#else
		for (unsigned int i = 0; i < OMX_MAX_STRINGNAME_SIZE; i++)
		{
			aActiveRole[i] = mActiveRole[i];
		}
#endif
		return OMX_ErrorNone;
	}
	return OMX_ErrorUndefined;

}

 OMX_ERRORTYPE ImgEns_Component::setActiveRole(const OMX_U8 * aActiveRole)
{
	IMGENS_ASSERT(strlen((const char *) aActiveRole) < OMX_MAX_STRINGNAME_SIZE);
	strncpy((char*) mActiveRole, (char*) aActiveRole, OMX_MAX_STRINGNAME_SIZE);
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE ImgEns_Component::construct(OMX_U32 nPortCount, OMX_U32 nRoleCount)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	IMGENS_ASSERT(nRoleCount != 0);

	mPortCount           = nPortCount;
	mProcessingComponent = 0;

	mRoleCount = nRoleCount;
	mActiveRole[0] = '\0';
	for (unsigned int i = 0; i < nRoleCount; i++)
	{
		mRoles[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
	}
	mCompFsm = new ImgEns_Component_Fsm(*this);
	if (mCompFsm == 0)
	{
		return OMX_ErrorInsufficientResources;
	}

	mPorts = new ImgEns_Port *[mPortCount];
	if (mPorts == 0)
	{
		return OMX_ErrorInsufficientResources;
	}

	for (unsigned int i = 0; i < mPortCount; i++)
	{ // Initialise all port pointerrs to NULL
		mPorts[i] = NULL;
	}

	// create trace stuff
	mTraceBuilder = new ImgEns_TraceBuilderHost();
	if (mTraceBuilder == 0)
	{
		IMGENS_ASSERT(mTraceBuilder);
		// return OMX_ErrorInsufficientResources;
	}
	else
	{
		mTraceBuilder->createInitialZone();
	}

	setTraceInfo(getSharedTraceInfoPtr(), 0); /* 0: not used */

	mCompFsm->setTraceInfo(getSharedTraceInfoPtr(), 0); /*0: not used*/

	// end trace stuff
	initBufferSharingGraph();

	return error;
}

 void ImgEns_Component::addPort(ImgEns_Port * port)
{
	OMX_U32 portIndex = port->getPortIndex();
	DBC_PRECONDITION(portIndex < getPortCount());
	DBC_PRECONDITION(mPorts[portIndex] == 0);
	DBC_PRECONDITION(port != 0);
	mPorts[portIndex] = port;
}

 void ImgEns_Component::eventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)
{
	getCompFsm()->dspEventHandler(eEvent, nData1, nData2);
}

 void ImgEns_Component::emptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
	DBC_PRECONDITION(pBuffer != 0);
	DBC_PRECONDITION(pBuffer->nInputPortIndex < getPortCount());
	ImgEns_CmdEmptyBufferDoneEvt evt(pBuffer);
	OMX_ERRORTYPE error = Dispatch(&evt);
	if (error != OMX_ErrorNone)
	{
		OstTraceInt1(TRACE_ERROR, "ImgEns_Proxy: ImgEns_Component::emptyBufferDone error 0x%x\n", error);
	}
}

 void ImgEns_Component::fillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
	DBC_PRECONDITION(pBuffer != 0);
	DBC_PRECONDITION(pBuffer->nOutputPortIndex < getPortCount());
	ImgEns_CmdFillBufferDoneEvt evt(pBuffer);
	OMX_ERRORTYPE error = Dispatch(&evt);
	if (error != OMX_ErrorNone)
	{
		OstTraceInt1(TRACE_ERROR, "ImgEns_Proxy: ImgEns_Component::fillBufferDone error 0x%x\n", error);
	}
}

#if defined(TRACE_OMX_API_ENABLED)
// to be able to trace TRACE_OMX_API trace only by enabling ENS dictionary
	#define MASK_VALUE 0x20
#else
	#define MASK_VALUE 0
#endif

OMX_BOOL ImgEns_Component::allPortsInSameFsmState(ImgEns_Fsm::_fnState state) const
{
	for (OMX_U32 i = 0; i < getPortCount(); i++)
	{
		ImgEns_Port_Fsm * portFsm = getPort(i)->getPortFsm();
		IMGENS_ASSERT(portFsm);
		if (portFsm->getFsmState() != state)
			return OMX_FALSE;
	}
	return OMX_TRUE;
}

ImgEns_TraceInfo * ImgEns_Component::getSharedTraceInfoPtr(void)
{
	return mTraceBuilder->getSharedTraceInfoPtr();
}

// ENS Assumes that port indices start from 0 to portCount-1

/*
bufferSharingGraph data structure has created the possible graph:
1) It rules out the possibilty of multiple input ports connected to single output port.
2) All output ports connected to single input port are known

Now check the direction of buffersharing among the input and output ports.
Only combination specified in OpenMAX IL Spec are to be allowed.

NS:NonSupplier port ; S:Supplier Port but not allocator; A:Allocator Port
Possible combinations are:
1) NS--->S (S should have a tunneled port)
2) A---->S (S should have a tunneled port)
3) Exactly one port has to be supplier.
*/

/*
NS:NonSupplier port ; S:Supplier Port but not allocator ; A:Allocator Port
NS-XX-NS | S<---NS | A-XX-NS
NS--->S  | S-XX-S  | A--->S
NS-XX-A	 | S<---A  | A-XX-A
*/
// We will check the graph by selecting single input port at a time
// and then check one input & outpuit port combination
void ImgEns_Component::createBufferSharingGraph(void)
{
	OMX_U32 portCount = getPortCount();
	OMX_U32 connectedPortCount = 0, supplierOutputPorts = 0, nonSupplierOutputPorts = 0;
	// Allocate dynamic array
	OMX_U32 *portList = new OMX_U32[portCount];

	for (OMX_U32 i = 0; i < portCount; i++)
	{
		if (getPort(i)->getDirection() == OMX_DirInput)
		{
			for (OMX_U32 j = 0; j < portCount; j++)
			{
				if (mBufSharingGraph[i][j].isRequested)
				{
					portList[connectedPortCount] = j;
					connectedPortCount++;
					if (getPort(j)->isBufferSupplier())
						supplierOutputPorts++;
					else
						nonSupplierOutputPorts++;
				}
			}

			if (connectedPortCount > 1)
			{
				// An input port can serve multiple supplier ports
				IMGENS_ASSERT(nonSupplierOutputPorts == 0 && connectedPortCount == supplierOutputPorts);
				if (getPort(i)->isBufferSupplier())
				{
					getPort(i)->setAllocatorRole(OMX_TRUE);
					for (OMX_U32 k = 0; k < connectedPortCount; k++)
					{
						getPort(portList[k])->setAllocatorRole(OMX_FALSE);
					}
				}
				else
				{
					getPort(i)->setAllocatorRole(OMX_FALSE);
					for (OMX_U32 k = 0; k < connectedPortCount; k++)
					{
						getPort(portList[k])->setAllocatorRole(OMX_FALSE);
					}
				}
			} else if (connectedPortCount == 1)
			{
				if (getPort(i)->isBufferSupplier())
				{
					if (getPort(portList[0])->isBufferSupplier())
					{
						//Make output an allocator
						getPort(i)->setAllocatorRole(OMX_FALSE);
						getPort(portList[0])->setAllocatorRole(OMX_TRUE);
					}
				}
				else
				{
					IMGENS_ASSERT(nonSupplierOutputPorts == 0);
					getPort(i)->setAllocatorRole(OMX_FALSE);
					getPort(portList[0])->setAllocatorRole(OMX_FALSE);
				}
			}
			else {
				if (getPort(i)->isBufferSupplier())
				{
					getPort(i)->setAllocatorRole(OMX_TRUE);
				} else
					getPort(i)->setAllocatorRole(OMX_FALSE);
			}
		} else { //output port
			for (OMX_U32 j = 0; j < portCount; j++) 
			{
				if (mBufSharingGraph[i][j].isRequested) 
				{
					portList[connectedPortCount] = j;
					connectedPortCount++;
					if (getPort(j)->isBufferSupplier())
						supplierOutputPorts++;
					else
						nonSupplierOutputPorts++;
				}
			}

			if (connectedPortCount > 1)
			{// An output cannot be connected to multiple input ports
				IMGENS_ASSERT(0);
			} else if (connectedPortCount == 1)
			{
				//Input port will set role for this port
			}
			else
			{
				if (getPort(i)->isBufferSupplier())
				{
					getPort(i)->setAllocatorRole(OMX_TRUE);
				} else
					getPort(i)->setAllocatorRole(OMX_FALSE);
			}
		}

		if (connectedPortCount)
		{
			ImgEns_Port * * connectedPorts = new ImgEns_Port *[connectedPortCount];
			for (OMX_U32 j = 0; j < connectedPortCount; j++)
			{
				*(connectedPorts + j) = getPort(portList[j]);
			}
			//Send this list to port
			getPort(i)->setBufferSharingPortList(connectedPorts, connectedPortCount);
			connectedPorts = 0;
		}
		else
		{
			getPort(i)->setBufferSharingPortList(0, 0);
		}
		connectedPortCount     = 0;
		supplierOutputPorts    = 0;
		nonSupplierOutputPorts = 0;
	}
	delete[] portList;
}

OMX_ERRORTYPE ImgEns_Component::connectBufferSharingPorts(OMX_U32 inputPortIndex, OMX_U32 outputPortIndex)
{/// This Connects an input port to output port and also ensures that an output port should be connected to only to one input port. */
	OMX_U32 portCount = getPortCount();
	OMX_U32 inputPortCount = 0UL;
	IMGENS_ASSERT(inputPortIndex < portCount && outputPortIndex < portCount);
	if (getPort(inputPortIndex)->getDirection()== getPort(outputPortIndex)->getDirection())
	{
		return OMX_ErrorUndefined;
	}

	mBufSharingGraph[inputPortIndex][outputPortIndex].isRequested = OMX_TRUE;
	mBufSharingGraph[outputPortIndex][inputPortIndex].isRequested = OMX_TRUE;

	// Check output port should be connected to only one input port
	for (OMX_U32 i = 0, inputPortCount = 0; i < portCount; i++)
	{
		if (mBufSharingGraph[i][outputPortIndex].isRequested == OMX_TRUE)
		{
			inputPortCount++;
		}
	}
	if (inputPortCount > 1)
	{
		mBufSharingGraph[inputPortIndex][outputPortIndex].isRequested = OMX_FALSE;
		mBufSharingGraph[outputPortIndex][inputPortIndex].isRequested = OMX_FALSE;
		return OMX_ErrorUndefined;
	}

	mBufferSharingEnabled = OMX_TRUE;

	for (OMX_U32 i = 0; i < portCount; i++)
	{
		getPort(i)->bufferSharingEnabled();
		getCompFsm()->bufferSharingEnabled();
	}
	return OMX_ErrorNone;
}

void ImgEns_Component::initBufferSharingGraph()
{
	OMX_U32 i, j;
	mBufferSharingEnabled = OMX_FALSE;
	const unsigned int MaxPortCount=MAX_PORTS_COUNT_BUFFER_SHARING;
	for (i = 0; i < MaxPortCount; i++)
	{
		for (j = 0; j < MaxPortCount; j++)
		{
			mBufSharingGraph[i][j].isRequested = OMX_FALSE;
			mBufSharingGraph[i][j].isBroken    = OMX_TRUE;
		}
	}
}

void ImgEns_Component::disableBufferSharing()
{
	for (OMX_U32 i = 0; i < getPortCount(); i++)
	{
		if (getPort(i)->isBufferSupplier())
		{
			getPort(i)->setAllocatorRole(OMX_TRUE);
			getPort(i)->setBufferSharingPortList(0, 0);
		}
		else
		{
			getPort(i)->setAllocatorRole(OMX_FALSE);
			getPort(i)->setBufferSharingPortList(0, 0);
		}
	}
}

OMX_STATETYPE ImgEns_Component::omxState() const
{
	return this->getCompFsm()->getOMXState();
}


OMX_U32 ImgEns_Component::getUidTopDictionnary(void)
{
	return 0;
}

OMX_ERRORTYPE ImgEns_Component::construct(void)
{
	return getProcessingComponent().construct();
}

 unsigned short ImgEns_Component::getTraceEnable() const
{
	return Img_TraceObject::getTraceEnable();
}

OMX_VERSIONTYPE ImgEns_Component::getVersion(void) const
{
	return getImgOmxIlSpecVersion();
}

const ImgEns_Port *ImgEns_Component:: getPort(OMX_U32 nPortIndex) const
{
	if ( (nPortIndex < getPortCount()) && (mPorts[nPortIndex] != 0) )
		return mPorts[nPortIndex];
	// Wrong case
	IMGENS_ASSERT(0);
	return (NULL);
}

ImgEns_Port * ImgEns_Component::getPort(OMX_U32 nPortIndex)
{
	if ( (nPortIndex < getPortCount()) && (mPorts[nPortIndex] != 0) )
		return mPorts[nPortIndex];
	IMGENS_ASSERT(0);
	return (NULL);
}


OMX_ERRORTYPE ImgEns_Component::Dispatch(const ImgEns_FsmEvent *pEvent)
//*************************************************************************************************************
{
	int status= getCompFsm()->dispatch(pEvent);
	if (status== ImgEns_Fsm::eError_IncorrectStateOperation)
		status= OMX_ErrorIncorrectStateOperation; //translate to right OMX error index
	return((OMX_ERRORTYPE)status);
}

OMX_ERRORTYPE ImgEns_Component::DispatchToPort(unsigned int index, const ImgEns_FsmEvent *pEvent)
//*************************************************************************************************************
{ // dispatch event to port fsm
	int  status= OMX_ErrorNone;
	OMX_ERRORTYPE error;
	if (index == OMX_ALL)
	{
		for (unsigned int i = 0; i < getPortCount(); i++)
		{
			error=DispatchToPort(i, pEvent);
			if (error != OMX_ErrorNone)
				status = error;
		}
		return (OMX_ERRORTYPE)status;
	}
	else
	{
		ImgEns_Port *pPort=getPort(index);
		if (pPort==NULL)
		{
			IMGENS_ASSERT(0);
			return(OMX_ErrorBadPortIndex);
		}
		status = pPort->Dispatch(pEvent);
		if (status== ImgEns_Fsm::eError_IncorrectStateOperation)
			status= OMX_ErrorIncorrectStateOperation; //translate to right OMX error index
		return((OMX_ERRORTYPE)status);
	}
}

OMX_ERRORTYPE ImgEns_Component::getRole(OMX_U8** aRole, const OMX_U32 role_idx) const
{
	if (role_idx >= mRoleCount)
	{ // Index to big!
		IMGENS_ASSERT(role_idx < mRoleCount);
		*aRole = NULL;
		return(OMX_ErrorBadParameter);
	}
	else
	{
		*aRole = mRoles[role_idx];
		return OMX_ErrorNone;
	}
}

static OMX_ERRORTYPE Default_EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
//*************************************************************************************************************
{
	UNUSED( hComponent || pAppData || eEvent || nData1 || nData2 || pEventData);
	return(OMX_ErrorNone);
}

static OMX_ERRORTYPE Default_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	UNUSED( hComponent || pAppData || pBuffer);
	return(OMX_ErrorNone);
}

static OMX_ERRORTYPE Default_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	UNUSED( hComponent || pAppData || pBuffer);
	return(OMX_ErrorNone);
}

// Default definition of useer callback for ImgEns_Component
const OMX_CALLBACKTYPE g_DefaultCallback={Default_EventHandler, Default_EmptyBufferDone, Default_FillBufferDone};

