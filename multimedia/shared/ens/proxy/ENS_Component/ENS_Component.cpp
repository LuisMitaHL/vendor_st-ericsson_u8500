/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Component.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Client.h"
#include "ENS_Component.h"
#include "ENS_Component_Fsm.h"
#include "ENS_Port.h"
#include "ENS_DBC.h"
#include "ProcessingComponent.h"
#define OMXCOMPONENT "ENS_COMPONENT"
#include "osi_trace.h"
#include "ENS_Index.h"
#include <string.h>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Component_ENS_ComponentTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE
#define OMX_API_TRACE

#define MAX_OST_SIZE 56

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::eventHandlerCB(
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData) 
{ 
#ifdef OMX_API_TRACE
    OstTraceFiltInst3(TRACE_OMX_API, "ENS_PROXY: ENS_Component::eventHandlerCB eEvent = %d nData1 = %d, nData2 = %d", (OMX_U32)eEvent, nData1, nData2);   
#endif
    return mCallbacks.EventHandler(
                getOMXHandle(), mAppData, eEvent, nData1, nData2, pEventData);
}

ENS_API_EXPORT ENS_Component::ENS_Component()
    :
    mRMP(0),
    mRoleCount(0),
    mOmxHandle(0),
    mCompFsm(0),
    mVersion(),
    mPortCount(0),
    mPorts(0),
    mCallbacks(),
    mAppData(0),
    mProcessingComponent(0),
    mRMEcbks(),
    mPreemptionState(Preempt_None),
    mTraceBuilder(0),
    mResourceConcealmentForbidden(OMX_TRUE),
    mGroupPriority(0),
    mGroupID(0),
    mSuspensionPolicy(OMX_SuspensionDisabled),
    mPreemptionPolicy(OMX_TRUE),
    mResourceSuspended(false),
    mEnsWrapperCb(0),
    mBufferSharingEnabled(OMX_FALSE)
{
    mActiveRole[0] = 0;
#ifndef BACKWARD_COMPATIBILTY_MODE
    BUFFER_SHARING_ENABLED = true;
#else
    BUFFER_SHARING_ENABLED = false;
#endif
}

bool
ENS_Component::resourcesSuspended(void) const
{ return mResourceSuspended; }
        
void
ENS_Component::resourcesSuspended(bool bFlag)
{ mResourceSuspended = bFlag; }

/// FIXME: All these statics are obsolete and must be removed
#if ENS_VERSION == ENS_VERSION_SHARED
ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SendCommand(
                ENS_Component_p pEnsComp,
                OMX_COMMANDTYPE Cmd, 
                OMX_U32 nParam, 
                OMX_PTR pCmdData)
{
    return pEnsComp->SendCommand(Cmd, nParam, pCmdData);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetParameter(
                ENS_Component_p pEnsComp,
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure)
{
    return pEnsComp->SetParameter(nIndex, pComponentParameterStructure);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetParameter(
                ENS_Component_p pEnsComp,
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure)
{
    return pEnsComp->GetParameter(nIndex, pComponentParameterStructure);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetConfig(
                ENS_Component_p pEnsComp,
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure)
{
    return pEnsComp->SetConfig(nIndex, pComponentConfigStructure);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetConfig(
                ENS_Component_p pEnsComp,
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure)
{
    return pEnsComp->GetConfig(nIndex, pComponentConfigStructure);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetExtensionIndex(
                ENS_Component_p pEnsComp,
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType)
{
    return pEnsComp->GetExtensionIndex(cParameterName, pIndexType);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetState(
                ENS_Component_p pEnsComp,
                OMX_STATETYPE* pState)
{
   return pEnsComp->GetState(pState);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentTunnelRequest(
                ENS_Component_p pEnsComp,
                OMX_U32 nPort,
                OMX_HANDLETYPE hTunneledComp,
                OMX_U32 nTunneledPort,
                OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    return pEnsComp->ComponentTunnelRequest(nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetCallbacks(
                ENS_Component_p pEnsComp,
                OMX_CALLBACKTYPE* pCallbacks,
                OMX_PTR pAppData)
{
    return pEnsComp->SetCallbacks(pCallbacks,  pAppData);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::EmptyThisBuffer(
                ENS_Component_p pEnsComp,
                OMX_BUFFERHEADERTYPE* pBuffer)
{
    return pEnsComp->EmptyThisBuffer(pBuffer);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::FillThisBuffer(
                ENS_Component_p pEnsComp,
                OMX_BUFFERHEADERTYPE* pBuffer)
{
    return pEnsComp->FillThisBuffer(pBuffer);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::UseBuffer(
                ENS_Component_p pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes,
                OMX_U8* pBuffer)
{
    return pEnsComp->UseBuffer(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::AllocateBuffer(
                ENS_Component_p pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes)
{
    return pEnsComp->AllocateBuffer(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::FreeBuffer(
                ENS_Component_p pEnsComp,
                OMX_U32 nPortIndex,
                OMX_BUFFERHEADERTYPE* pBuffer)
{
    return pEnsComp->FreeBuffer(nPortIndex, pBuffer);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::UseEGLImage(
                ENS_Component_p pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                void* eglImage)
{
    return pEnsComp->UseEGLImage(ppBufferHdr, nPortIndex, pAppPrivate, eglImage);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetComponentVersion(
                ENS_Component * pEnsComp,
                OMX_STRING pComponentName,
                OMX_VERSIONTYPE* pComponentVersion,
                OMX_VERSIONTYPE* pSpecVersion,
                OMX_UUIDTYPE* pComponentUUID)
{
    return pEnsComp->GetComponentVersion(pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentDeInit(
                ENS_Component * pEnsComp)
{
    return pEnsComp->ComponentDeInit();
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentRoleEnum(
                ENS_Component * pEnsComp,
                OMX_U8 *cRole,
                OMX_U32 nIndex)
{
    return pEnsComp->ComponentRoleEnum(cRole, nIndex);
}
/// END OF FIXME
#endif

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SendCommand(
                OMX_COMMANDTYPE Cmd,
                OMX_U32 nParam,
                OMX_PTR pCmdData)
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::SendCommand cmd = %d nParam = %d", (OMX_U32)Cmd, nParam);
#endif  
    switch(Cmd){
        case OMX_CommandStateSet:
        {
            OMX_ERRORTYPE error;
            ENS_CmdSetStateEvt evt((OMX_STATETYPE)nParam);
			error = getCompFsm()->dispatch(&evt);
            if(error != OMX_ErrorNone) return error;
            if((OMX_STATETYPE)nParam == OMX_StateLoaded) {
                //send OMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning to loaded state.
                eventHandlerCB(OMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateLoaded, 0);
            }
            return OMX_ErrorNone;
        }
        case OMX_CommandPortDisable:
        {
            ENS_CmdPortEvt evt(OMX_DISABLE_PORT_SIG, nParam);
            if (nParam == OMX_ALL) {
                //Component must disable all ports
                for(OMX_U32 i = 0; i < getPortCount(); i++) {
                    OMX_ERRORTYPE error;
                    ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    error = portFsm->dispatch(&evt);
                    if(error != OMX_ErrorNone) return error;
                }
                return OMX_ErrorNone;
            } else {
                // Disable a specified port number
                if(nParam < getPortCount())
                {
                    ENS_Port_Fsm * portFsm = getPort(nParam)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    return portFsm->dispatch(&evt);
                }
                else
                {
                    return OMX_ErrorBadPortIndex;
                }
            }
        }
        case OMX_CommandPortEnable:
        {
            ENS_CmdPortEvt evt(OMX_ENABLE_PORT_SIG, nParam);
            if (nParam == OMX_ALL) {
                //Component must enable all ports
                for (OMX_U32 i = 0; i < getPortCount(); i++) {
                    OMX_ERRORTYPE error;
                    ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    error = portFsm->dispatch(&evt);
                    if(error != OMX_ErrorNone) return error;
                }
                return OMX_ErrorNone;
            } else {
                // Enable a specified port number
                if(nParam < getPortCount())
                {
                    ENS_Port_Fsm * portFsm = getPort(nParam)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    return portFsm->dispatch(&evt);
                }
                else
                {
                    return OMX_ErrorBadPortIndex;
                }
            }                
        }
        case OMX_CommandFlush:
        {
            ENS_CmdPortEvt evt(OMX_FLUSH_PORT_SIG, nParam);
            if (nParam == OMX_ALL) {
                //Component must flush all ports
                for (OMX_U32 i = 0; i < getPortCount(); i++) {
                    OMX_ERRORTYPE error;
                    ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    error = portFsm->dispatch(&evt);
                    if(error != OMX_ErrorNone) return error;
                }
                return OMX_ErrorNone;
            } else {
                // Flush a specified port number
                if(nParam < getPortCount())
                {
                    ENS_Port_Fsm * portFsm = getPort(nParam)->getPortFsm();
                    DBC_ASSERT(portFsm);
                    return portFsm->dispatch(&evt);
                }
                else
                {
                    return OMX_ErrorBadPortIndex;
                }
            }                
        }
        default:
			return OMX_ErrorNotImplemented;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetParameter(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure)
{
    if(pComponentParameterStructure == 0){
        return OMX_ErrorBadParameter;
    }

#ifdef OMX_API_TRACE
    // select one parameter structure to get back nSize 
    OMX_U32 size =  static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>(pComponentParameterStructure)->nSize;

    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::SetParameter nIndex = %d, size = %d", (OMX_S32)nIndex, size);
    // size is limited to MAX_OST_SIZE bytes
    for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
    {
        OstTraceFiltInstData(TRACE_OMX_API, "ENS_PROXY: ENS_Component::SetParameter ParameterStructure = %{int8[]}", (OMX_U8 *)((OMX_U8 *)pComponentParameterStructure + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }
#endif		

    ENS_CmdSetParamEvt evt(nIndex, pComponentParameterStructure);

    if(isPortSpecificParameter(nIndex)){
        //Dispacth event on port state machine 
        ENS_PORT_INDEX_STRUCT *portIdxStruct =
            static_cast<ENS_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
        
		if(portIdxStruct->nPortIndex == OMX_ALL) {
			for(OMX_U32 i=0; i<getPortCount();i++)
			{
				portIdxStruct->nPortIndex = i;
				ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
				DBC_ASSERT(portFsm);
				OMX_ERRORTYPE ret = portFsm->dispatch(&evt);
				if(ret != OMX_ErrorNone) return ret;
			}
			return OMX_ErrorNone;
			
		}
		else {
			if(portIdxStruct->nPortIndex >= getPortCount()){
	            return OMX_ErrorBadPortIndex;
	        }
	        ENS_Port_Fsm * portFsm = getPort(portIdxStruct->nPortIndex)->getPortFsm();
			DBC_ASSERT(portFsm);
	        return portFsm->dispatch(&evt);
		}
    } else {
        //Dispacth event on component state machine
        return getCompFsm()->dispatch(&evt);
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetParameter(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure)
{
    if(pComponentParameterStructure == 0){
        return OMX_ErrorBadParameter;
    }
#ifdef OMX_API_TRACE
    OstTraceFiltInst1(TRACE_OMX_API, "ENS_PROXY: ENS_Component::GetParameter nIndex = %d", (OMX_U32)nIndex);
#endif

    ENS_CmdGetParamEvt evt(nIndex, pComponentParameterStructure);

    if(isPortSpecificParameter(nIndex)){
        //Dispacth event on port state machine 
        ENS_PORT_INDEX_STRUCT *portIdxStruct =
            static_cast<ENS_PORT_INDEX_STRUCT *>(pComponentParameterStructure);
        
		if(portIdxStruct->nPortIndex== OMX_ALL){
			for(OMX_U32 i=0; i<getPortCount();i++)
			{
				portIdxStruct->nPortIndex = i;
				ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
				DBC_ASSERT(portFsm);
				OMX_ERRORTYPE ret = portFsm->dispatch(&evt);
				if(ret != OMX_ErrorNone) return ret;
			}
			return OMX_ErrorNone;
        }
		else
		{			
			if(portIdxStruct->nPortIndex >= getPortCount())
	            return OMX_ErrorBadPortIndex;
			
			ENS_Port_Fsm * portFsm = getPort(portIdxStruct->nPortIndex)->getPortFsm();
			DBC_ASSERT(portFsm);
			return portFsm->dispatch(&evt);
		}
    } else {
        //Dispacth event on component state machine
        return getCompFsm()->dispatch(&evt);
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetConfig(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure)
{
    if(pComponentConfigStructure == 0){
        return OMX_ErrorBadParameter;
    }

#ifdef OMX_API_TRACE
    // select one config structure to get back nSize 
    OMX_U32 size =  static_cast<OMX_OTHER_CONFIG_STATSTYPE *>(pComponentConfigStructure)->nSize;

    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::SetConfig index = %d, size = %d", (OMX_S32)nIndex, size);
    // size is limited to MAX_OST_SIZE bytes
    for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
    {
        OstTraceFiltInstData(TRACE_OMX_API, "ENS_PROXY: ENS_Component::SetConfig ParameterStructure = %{int8[]}", (OMX_U8 *)((OMX_U8 *)pComponentConfigStructure + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }
#endif   

    ENS_CmdSetConfigEvt evt(nIndex, pComponentConfigStructure);
    return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetConfig(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure)
{
    if(pComponentConfigStructure == 0){
        return OMX_ErrorBadParameter;
    }

#ifdef OMX_API_TRACE
    OstTraceFiltInst1(TRACE_OMX_API, "ENS_PROXY: ENS_Component::GetConfig nIndex = %d", (OMX_U32)nIndex);
#endif

    ENS_CmdGetConfigEvt evt(nIndex, pComponentConfigStructure);
    return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetExtensionIndex(
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType)
{
#ifdef OMX_API_TRACE
    OstTraceFiltInstData(TRACE_OMX_API, "ENS_PROXY: ENS_Component::GetExtensionIndex %{int8[]}", (OMX_U8 *)cParameterName, 128);
#endif
    ENS_CmdGetExtIdxEvt evt(cParameterName, pIndexType);
    return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetState(
                OMX_STATETYPE* pState)
{
    *pState = omxState();
#ifdef OMX_API_TRACE
    OstTraceFiltInst1(TRACE_OMX_API, "ENS_PROXY: ENS_Component::GetState => state = %d", (OMX_U32)*pState);
#endif

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentTunnelRequest(
                OMX_U32 nPort,
                OMX_HANDLETYPE hTunneledComp,
                OMX_U32 nTunneledPort,
                OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst3(TRACE_OMX_API, "ENS_PROXY: ENS_Component::ComponentTunnelRequest Port = %d <=> Tunneled Component-Port = 0x%x-%d", nPort, (OMX_U32)hTunneledComp, nTunneledPort );
#endif
    if (nPort>=getPortCount() || getPort(nPort)==0) {
        return OMX_ErrorBadParameter;
    }
    ENS_Port_Fsm * portFsm = getPort(nPort)->getPortFsm();
    DBC_ASSERT(portFsm);
    ENS_CmdTunnelRequestEvt evt(hTunneledComp, nTunneledPort, pTunnelSetup);
	return portFsm->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::SetCallbacks(
                OMX_CALLBACKTYPE* pCallbacks, 
                OMX_PTR pAppData)
{
    ENS_CmdSetCbEvt evt(pCallbacks, pAppData);
	return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::EmptyThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer)
{
    ENS_CmdEmptyThisBufferEvt evt(pBuffer);
    if(pBuffer == 0) {
        return OMX_ErrorBadParameter;
    }
#ifdef OMX_API_TRACE
    OstTraceFiltInst4(TRACE_OMX_BUFFER, "ENS_PROXY: ENS_Component::EmptyThisBuffer nFilledLen %d, nOffset %d, nFlags %d (port %d)", pBuffer->nFilledLen,  pBuffer->nOffset,  pBuffer->nFlags, pBuffer->nInputPortIndex);
    
    // size is limited to MAX_OST_SIZE bytes
    OMX_U32 size = pBuffer->nFilledLen;
    OMX_U16 sav_id = getId1();    
    setTraceInfo(getTraceInfoPtr(), pBuffer->nInputPortIndex);        
    for (OMX_U32 i = 0; i < size; i+=MAX_OST_SIZE)
    {
         OstTraceFiltInstData(TRACE_OMX_BUFFER, " ENS_PROXY: ENS_Component::EmptyThisBuffer   data = %{int8[]}", (OMX_U8 *)((OMX_U8 *)(pBuffer->pBuffer) + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }
    setTraceInfo(getTraceInfoPtr(), sav_id);        
#endif   

    return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::FillThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer)
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_BUFFER, "ENS_PROXY: ENS_Component::FillThisBuffer nAllocLen = %d (port %d)", pBuffer->nAllocLen, pBuffer->nOutputPortIndex);
#endif
    ENS_CmdFillThisBufferEvt evt(pBuffer);
    if(pBuffer == 0) {
        return OMX_ErrorBadParameter;
    }
    return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::UseBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes,
                OMX_U8* pBuffer)
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::UseBuffer 0x%x (port %d)",  (OMX_U32)pBuffer, nPortIndex);
#endif
    ENS_CmdUseBufferEvt evt(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
	return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::AllocateBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes) 
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::AllocateBuffer size =  %d (port %d)", nSizeBytes, nPortIndex);
#endif
    ENS_CmdAllocBufferEvt evt(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes);
	return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::FreeBuffer(
                OMX_U32 nPortIndex,
                OMX_BUFFERHEADERTYPE* pBuffer) 
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::FreeBuffer 0x%x (port %d)", (OMX_U32)pBuffer, nPortIndex);
#endif
   ENS_CmdFreeBufferEvt evt(nPortIndex, pBuffer);
   return getCompFsm()->dispatch(&evt);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::UseEGLImage(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                void* eglImage) 
{
	return OMX_ErrorNotImplemented;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::GetComponentVersion(
                OMX_STRING pComponentName,
                OMX_VERSIONTYPE* pComponentVersion,
                OMX_VERSIONTYPE* pSpecVersion,
                OMX_UUIDTYPE* pComponentUUID)
{
    DBC_ASSERT(strlen(getName())<=127);       // check, according to OMX specifications
    strcpy(pComponentName, getName());
    *pComponentVersion = getVersion();
    getOmxIlSpecVersion(pSpecVersion);

#ifdef OMX_API_TRACE
    OstTraceFiltInst1(TRACE_OMX_API, "ENS_PROXY: ENS_Component::GetComponentVersion => version = %x", pComponentVersion->nVersion);
#endif

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentDeInit()
{
#ifdef OMX_API_TRACE
    OstTraceFiltInst0(TRACE_OMX_API, "ENS_PROXY: ENS_Component::ComponentDeInit");
#endif
    ENS_HwRm::HWRM_Unregister((OMX_PTR)(mOmxHandle));
    // delete of the component must be done in the wrapper
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::ComponentRoleEnum(
                OMX_U8 *cRole,
                OMX_U32 nIndex) 
{
    OMX_ERRORTYPE err = getRole(&cRole,nIndex);
#ifdef OMX_API_TRACE
    OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component::ComponentRoleEnum for index %d => error = %d", nIndex, (OMX_U32)err);
#endif
    return err;
}

ENS_API_EXPORT OMX_BOOL ENS_Component::isPortSpecificParameter(
        OMX_INDEXTYPE nParamIndex) const
{
    switch(nParamIndex){
        case OMX_IndexParamCompBufferSupplier:
        case OMX_IndexParamPortDefinition:
			return OMX_TRUE;
        default:
			return OMX_FALSE;
    }
}

OMX_ERRORTYPE ENS_Component::DefaultSetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure)
{
    OMX_U32 aPortIndex=0;
    // Important to call GetPortIndexExtension() in order to know if the index is correct or not
    if (MMHwBuffer::GetPortIndexExtension(nIndex, pStructure, aPortIndex) == OMX_ErrorNone) {
        if (aPortIndex>=getPortCount() || getPort(aPortIndex)==0) {
            return OMX_ErrorBadPortIndex;
        }
        MMHwBuffer *sharedChunk=0;
        if (MMHwBuffer::Open(getOMXHandle(), nIndex, pStructure, sharedChunk) != OMX_ErrorNone) {
            return OMX_ErrorUnsupportedIndex;
        }
        getPort(aPortIndex)->setSharedChunk(sharedChunk);
        return OMX_ErrorNone;
    }
    return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE ENS_Component::DefaultGetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const
{
    OMX_U32 aPortIndex;
    // Important to call GetPortIndexExtension() in order to know if the index is correct or not
    if (MMHwBuffer::GetPortIndexExtension(nIndex, pStructure, aPortIndex) == OMX_ErrorNone) {
        if (aPortIndex>=getPortCount() || getPort(aPortIndex)==0) {
            return OMX_ErrorBadPortIndex;
        }
        MMHwBuffer *pSharedChunk = getPort(aPortIndex)->getSharedChunk();
        if (pSharedChunk) {
            return pSharedChunk->GetConfigExtension(nIndex, pStructure, aPortIndex);
        } else {
            // No MMHwBuffer handle on this port, check if this extension allows one to be Opened
            // Very hugly here we need to cast as we are calling a non const function from a const one
            if (MMHwBuffer::Open(((ENS_Component *)this)->getOMXHandle(), nIndex,
                                 pStructure, pSharedChunk) == OMX_ErrorNone) {
                ((ENS_Port *)getPort(aPortIndex))->setSharedChunk(pSharedChunk);
                return OMX_ErrorNone;
            }
        }
    }
    return OMX_ErrorUnsupportedIndex;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::getParameter(
        OMX_INDEXTYPE nParamIndex,  
        OMX_PTR pComponentParameterStructure) const 
{
    switch (nParamIndex) {
        case OMX_IndexParamStandardComponentRole:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_COMPONENTROLETYPE)
                OMX_PARAM_COMPONENTROLETYPE * param = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

                unsigned int i=0;
                while(i < OMX_MAX_STRINGNAME_SIZE && mActiveRole[i] != '\0') {
                    param->cRole[i] = mActiveRole[i];
                    i++;
                }
                param->cRole[i] = param->cRole[OMX_MAX_STRINGNAME_SIZE-1] = '\0' ;

                return OMX_ErrorNone;
            }

        case OMX_IndexParamAudioInit:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PORT_PARAM_TYPE);
                OMX_PORT_PARAM_TYPE *portparam =
                    (OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
                OMX_BOOL audio_port_found = OMX_FALSE;
                portparam->nPorts = portparam->nStartPortNumber = 0;
                // search for audio ports
                for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
                {
                    if (getPort(nPort)->getDomain() == OMX_PortDomainAudio) {
                        if (!audio_port_found) {
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
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PORT_PARAM_TYPE);
                OMX_PORT_PARAM_TYPE *portparam =
                    (OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
                OMX_BOOL video_port_found = OMX_FALSE;
                portparam->nPorts = portparam->nStartPortNumber = 0;
                // search for video ports
                for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
                {
                    if (getPort(nPort)->getDomain() == OMX_PortDomainVideo) {
                        if (!video_port_found) {
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
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PORT_PARAM_TYPE);
                OMX_PORT_PARAM_TYPE *portparam =
                    (OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
                OMX_BOOL image_port_found = OMX_FALSE;
                portparam->nPorts = portparam->nStartPortNumber = 0;
                // search for image ports
                for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
                {
                    if (getPort(nPort)->getDomain() == OMX_PortDomainImage) {
                        if (!image_port_found) {
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
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PORT_PARAM_TYPE);
                OMX_PORT_PARAM_TYPE *portparam =
                    (OMX_PORT_PARAM_TYPE *) pComponentParameterStructure;
                OMX_BOOL other_port_found = OMX_FALSE;
                portparam->nPorts = portparam->nStartPortNumber = 0;
                // search for other ports
                for (OMX_U32 nPort = 0; nPort < getPortCount(); nPort++)
                {
                    if (getPort(nPort)->getDomain() == OMX_PortDomainOther) {
                        if (!other_port_found) {
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
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_RESOURCECONCEALMENTTYPE);
                OMX_RESOURCECONCEALMENTTYPE *pResource = 
                    (OMX_RESOURCECONCEALMENTTYPE *)pComponentParameterStructure ;
                pResource->bResourceConcealmentForbidden = this->getResourceConcealmentForbidden();
                return OMX_ErrorNone;
            }
            
        case OMX_IndexParamPriorityMgmt:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PRIORITYMGMTTYPE);
                OMX_PRIORITYMGMTTYPE *pResource = 
                    (OMX_PRIORITYMGMTTYPE *)pComponentParameterStructure ;
                pResource->nGroupPriority = this->getGroupPriorityValue();
                pResource->nGroupID = this->getGroupPriorityId();
                return OMX_ErrorNone;
            }
            
        case OMX_IndexParamSuspensionPolicy:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_SUSPENSIONPOLICYTYPE);
                OMX_PARAM_SUSPENSIONPOLICYTYPE *pResource = 
                    (OMX_PARAM_SUSPENSIONPOLICYTYPE *)pComponentParameterStructure ;
                pResource->ePolicy = this->getSuspensionPolicy();
                return OMX_ErrorNone;
            }
            
        case OMX_IndexParamComponentSuspended:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_SUSPENSIONTYPE);
                OMX_PARAM_SUSPENSIONTYPE *pResource = 
                    (OMX_PARAM_SUSPENSIONTYPE *)pComponentParameterStructure ;
                pResource->eType = ((this->getPreemptionState()==Preempt_Partial)? OMX_Suspended :OMX_NotSuspended );
                return OMX_ErrorNone;
            }

        default:
            return DefaultGetConfigOrParameter(nParamIndex, pComponentParameterStructure);
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex) {
        case OMX_IndexParamStandardComponentRole:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_PARAM_COMPONENTROLETYPE);
                // TODO : check if role is supported !
                OMX_PARAM_COMPONENTROLETYPE *componentRole = 
                    (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure ;
                return setActiveRole(componentRole->cRole);
            }
            
        case OMX_IndexParamDisableResourceConcealment:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_RESOURCECONCEALMENTTYPE);
                OMX_RESOURCECONCEALMENTTYPE *pResource = 
                    (OMX_RESOURCECONCEALMENTTYPE *)pComponentParameterStructure ;
                this->setResourceConcealmentForbidden(pResource->bResourceConcealmentForbidden);
                return OMX_ErrorNone;
            }

        case OMX_IndexParamPriorityMgmt:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PRIORITYMGMTTYPE);
                OMX_PRIORITYMGMTTYPE *pResource = 
                    (OMX_PRIORITYMGMTTYPE *)pComponentParameterStructure ;
                this->setPriorityMgmt(pResource->nGroupPriority, pResource->nGroupID);
                return OMX_ErrorNone;
            }
            
        case OMX_IndexParamSuspensionPolicy:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure, OMX_PARAM_SUSPENSIONPOLICYTYPE);
                OMX_PARAM_SUSPENSIONPOLICYTYPE *pResource = 
                    (OMX_PARAM_SUSPENSIONPOLICYTYPE *)pComponentParameterStructure ;
                this->setSuspensionPolicy(pResource->ePolicy);
                return OMX_ErrorNone;
            }
            
        default:
            return DefaultSetConfigOrParameter(nParamIndex, pComponentParameterStructure);
    }
}


ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::getConfig(
        OMX_INDEXTYPE nIndex, 
        OMX_PTR pStructure) const
{
    if (pStructure == 0) {
		return OMX_ErrorBadParameter;
    }

    switch ((int)nIndex) {
         
        case ENS_IndexConfigTraceSetting:
        {
            ENS_CONFIG_TRACESETTINGTYPE *pConfig = 
                static_cast<ENS_CONFIG_TRACESETTINGTYPE *>(pStructure);
            
            TraceBuilder * pTraceBuilder = getTraceBuilder();	
            TraceInfo_t * info = pTraceBuilder->getSharedTraceInfoPtr();
            pConfig->nParentHandle = info->parentHandle;
            pConfig->nTraceEnable = info->traceEnable;
            return OMX_ErrorNone;
        }
        
        case OMX_IndexConfigPriorityMgmt:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pStructure, OMX_PRIORITYMGMTTYPE);
            OMX_PRIORITYMGMTTYPE *pResource = 
                (OMX_PRIORITYMGMTTYPE *)pStructure ;
            pResource->nGroupPriority = this->getGroupPriorityValue();
            pResource->nGroupID = this->getGroupPriorityId();
            return OMX_ErrorNone;
        }
            
        default:
            return DefaultGetConfigOrParameter(nIndex, pStructure);
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::setConfig(
        OMX_INDEXTYPE nIndex, 
        OMX_PTR pStructure)
{
    if (pStructure == 0) {
		return OMX_ErrorBadParameter;
    }

    switch ((int)nIndex) {
        case ENS_IndexConfigTraceSetting:
        {
            ENS_CONFIG_TRACESETTINGTYPE *pConfig = 
                static_cast<ENS_CONFIG_TRACESETTINGTYPE *>(pStructure);

            TraceBuilder * pTraceBuilder = getTraceBuilder();
            pTraceBuilder->setTraceZone(pConfig->nParentHandle, pConfig->nTraceEnable);
            return OMX_ErrorNone;
        }
        case ENS_IndexConfigTraceMpc:
        {
            ENS_CONFIG_TRACEMODEMPC *pConfig =
                static_cast<ENS_CONFIG_TRACEMODEMPC *>(pStructure);

            getProcessingComponent().configureTraceModeMpc(pConfig->bEnable);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigPriorityMgmt:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pStructure, OMX_PRIORITYMGMTTYPE);
            OMX_PRIORITYMGMTTYPE *pResource = 
                (OMX_PRIORITYMGMTTYPE *)pStructure ;
            this->setPriorityMgmt(pResource->nGroupPriority, pResource->nGroupID);
            // Notify RM about new priority settings 
            ENS_HwRm::HWRM_Notify_NewPolicyCfg((OMX_PTR)this->getOMXHandle(),
                                               this->getGroupPriorityValue(),
                                               this->getGroupPriorityId(), 0);
            return OMX_ErrorNone;
        }
        default:
            return DefaultSetConfigOrParameter(nIndex, pStructure);
    }
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
    // default case: check this is the Shared Chunk Extension
    return MMHwBuffer::GetIndexExtension(cParameterName, pIndexType);
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::registerILClientCallbacks(
        OMX_CALLBACKTYPE* pCallbacks,
        OMX_PTR pAppData) 
{
    if (pCallbacks == 0) {
		return OMX_ErrorBadParameter;
    }
    
    if (pCallbacks->EventHandler == 0) {
		return OMX_ErrorBadParameter;
    }

    mCallbacks  = *pCallbacks;
    mAppData    = pAppData;
    
	return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::getActiveRole(OMX_U8 * aActiveRole) const {
    if (mActiveRole[0] != '\0') {
        for (unsigned int i=0;i<OMX_MAX_STRINGNAME_SIZE;i++){
            aActiveRole[i] = mActiveRole[i];
        }
        return OMX_ErrorNone;
    }
    return OMX_ErrorUndefined;

}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::setActiveRole(const OMX_U8 * aActiveRole) {
    DBC_ASSERT(strlen((const char *)aActiveRole)<OMX_MAX_STRINGNAME_SIZE);
    strcpy((char*)mActiveRole,(char*)aActiveRole);
    
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::construct(OMX_U32 nPortCount, OMX_U32 nRoleCount, bool ArmOnly)
{
    OMX_ERRORTYPE error  = OMX_ErrorNone;

    DBC_ASSERT(nRoleCount !=0);

    mOmxHandle = 0;
    mPortCount = nPortCount;
    mProcessingComponent = 0;

    mRoleCount = nRoleCount;
    mActiveRole[0] = '\0';
    for (unsigned int i=0;i<nRoleCount;i++) {
        mRoles[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
    }
    
    mRMP        = 0;

    mCompFsm = new ENS_Component_Fsm(*this);
    if(mCompFsm == 0){
		return OMX_ErrorInsufficientResources;
    }
    
    mPorts =  new ENS_Port_p[mPortCount];
    if(mPorts == 0){
		return OMX_ErrorInsufficientResources;
    }

    for (unsigned int i = 0; i < mPortCount; i++) {
        mPorts[i] = 0;
    }
    
	// create trace stuff
	if (ArmOnly){
		mTraceBuilder =  new TraceBuilderHost();
	}
	else {
#ifndef HOST_ONLY 
	 	mTraceBuilder = new TraceBuilderMpc();
#else
		mTraceBuilder =  new TraceBuilderHost();
#endif
	}
    if(mTraceBuilder == 0){
		return OMX_ErrorInsufficientResources;
    }
	mTraceBuilder->createInitialZone();

	setTraceInfo(getSharedTraceInfoPtr(), 0); /* 0: not used */
	mCompFsm->setTraceInfo(getSharedTraceInfoPtr(), 0); /*0: not used*/

	// end trace stuff
    initBufferSharingGraph();


	return error;
}

ENS_API_EXPORT ENS_Component::~ENS_Component(void) {

    if (mProcessingComponent) {
        delete mProcessingComponent;
    }

    for (unsigned int i=0;i<mRoleCount;i++) {
        delete [] mRoles[i];
    }

    for (unsigned int i = 0; i < mPortCount; i++) {
        DBC_ASSERT(mPorts[i] != 0);
        delete mPorts[i];
    }

    delete [] mPorts;

    delete mCompFsm;

    if (mRMP) {
        delete mRMP;
    }

	if (mTraceBuilder) {
		delete mTraceBuilder;
	}
}

ENS_API_EXPORT void ENS_Component::addPort(ENS_Port_p port) {

    OMX_U32 portIndex = port->getPortIndex();

    DBC_PRECONDITION(portIndex < getPortCount());
    DBC_PRECONDITION(mPorts[portIndex] == 0);
    DBC_PRECONDITION(port != 0);

    mPorts[portIndex] = port;
}

ENS_API_EXPORT void ENS_Component::eventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2) {

    getCompFsm()->dspEventHandler(eEvent, nData1, nData2);
}
        
ENS_API_EXPORT void ENS_Component::emptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer) {
    
    DBC_PRECONDITION(pBuffer != 0);
    DBC_PRECONDITION(pBuffer->nInputPortIndex < getPortCount());
    ENS_CmdEmptyBufferDoneEvt evt(pBuffer);
    OMX_ERRORTYPE error = getCompFsm()->dispatch(&evt);
    if (error != OMX_ErrorNone) {
        OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ENS_Component::emptyBufferDone error 0x%x\n", error);
    }
}

ENS_API_EXPORT void ENS_Component::fillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer) {
    
    DBC_PRECONDITION(pBuffer != 0);
    DBC_PRECONDITION(pBuffer->nOutputPortIndex < getPortCount());
    ENS_CmdFillBufferDoneEvt evt(pBuffer);
    OMX_ERRORTYPE error = getCompFsm()->dispatch(&evt);
    if (error != OMX_ErrorNone) {
        OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ENS_Component::fillBufferDone error 0x%x\n", error);
    }
#ifdef OMX_API_TRACE
    else {
        OstTraceFiltInst3(TRACE_OMX_BUFFER, "ENS_PROXY: ENS_Component::fillBufferDone nFilledLen %d, nOffset %d, (port %d)", pBuffer->nFilledLen, pBuffer->nOffset, pBuffer->nOutputPortIndex);

        // size is limited to MAX_OST_SIZE bytes
        OMX_U32 size = pBuffer->nFilledLen;
        OMX_U16 sav_id = getId1();
        setTraceInfo(getTraceInfoPtr(), pBuffer->nOutputPortIndex);
        for (OMX_U32 i = 0; i < size; i+=MAX_OST_SIZE)
        {
             OstTraceFiltInstData(TRACE_OMX_BUFFER, " ENS_PROXY: ENS_Component::fillBufferDone data = %{int8[]}", (OMX_U8 *)((OMX_U8 *)(pBuffer->pBuffer) + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
        }
        setTraceInfo(getTraceInfoPtr(), sav_id);
    }
#endif
}

OMX_ERRORTYPE ENS_Component::RegisterForRm()
{  
    mRMEcbks.CP_Notify_Error = RM_ENS_Notify_Error;
    mRMEcbks.CP_Notify_ResReservationProcessed = RM_ENS_Notify_ResReservationProcessed;
    mRMEcbks.CP_Cmd_SetRMConfig = RM_ENS_Cmd_SetRMConfig;
    mRMEcbks.CP_Cmd_ReleaseResource = RM_ENS_Cmd_ReleaseResource;
    mRMEcbks.CP_Get_Capabilities = RM_ENS_Get_Capabilities;
    mRMEcbks.CP_Get_ResourcesEstimation = RM_ENS_Get_ResourcesEstimation;

    RM_STATUS_E error = ENS_HwRm::HWRM_Register((OMX_PTR)mOmxHandle,&mRMEcbks);
    if (error != RM_E_NONE) {return OMX_ErrorInsufficientResources;}
    
    return OMX_ErrorNone;
}
    
ENS_API_EXPORT OMX_ERRORTYPE
ENS_Component::suspendResources(void)
{
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS_Component::unsuspendResources(void)
{
    return OMX_ErrorNone;
}

#if defined(TRACE_OMX_API_ENABLED)
// to be able to trace TRACE_OMX_API trace only by enabling ENS dictionary
#define MASK_VALUE 0x20 
#else
#define MASK_VALUE 0
#endif

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::setOMXHandleAndRegisterRm(OMX_HANDLETYPE hComp)
{
    OMX_ERRORTYPE error;
    mOmxHandle = hComp;

    error = RegisterForRm();
  
#ifdef  OMX_API_TRACE
    const char * name = getName();
    mTraceBuilder->setTraceZone((OMX_U32)hComp, (OMX_U16)MASK_VALUE | (OMX_U16)getTraceEnable());
    OstTraceFiltInstData(TRACE_OMX_API,"ENS_proxy: ENS_Component::setOMXHandleAndRegisterRm component type %{int8[]}", (OMX_U8 *)name , MAX_OST_SIZE);
    OstTraceFiltInst1(TRACE_OMX_API,"ENS_proxy: ENS_Component::setOMXHandleAndRegisterRm component version %d",getVersion().nVersion);
#endif

    return error;
}


void ENS_Component::RM_ENS_Notify_Error (OMX_IN const OMX_PTR pCompHdl,
                        OMX_IN RM_STATUS_E nError)
{
    //ENS_HwRm::HWRM_Notify_NewPolicyCfg(pCompHdl,0,0,0);
    //ENS_HwRm::HWRM_Notify_Tunnel(NULL,0,NULL,0);
    //ENS_HwRm::HWRM_Get_DefaultDDRDomain(NULL,NULL);
    //ENS_HwRm::HWRM_Get_TentativeLossList(NULL);
}


void ENS_Component::RM_ENS_Notify_ResReservationProcessed (OMX_IN const OMX_PTR pCompHdl, 
                                                           OMX_IN OMX_BOOL bResourcesReservGranted)
{
    ENS_Component * pEnsComp = (ENS_Component *)EnsWrapper_getEnsComponent((OMX_HANDLETYPE)pCompHdl);

    // Send evt to the component
    ENS_CmdResReservationProcessedEvt evt(bResourcesReservGranted);
    OMX_ERRORTYPE error = pEnsComp->getCompFsm()->dispatch(&evt);
    if (error != OMX_ErrorNone) {
        OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ENS_Component::RM_ENS_Notify_ResReservationProcessed error 0x%x\n", error);
    }
}


void ENS_Component::RM_ENS_Cmd_SetRMConfig (OMX_IN const OMX_PTR pCompHdl, 
                                            OMX_IN const RM_SYSCTL_T* pRMcfg)
{
    ENS_Component * pEnsComp = (ENS_Component *)EnsWrapper_getEnsComponent((OMX_HANDLETYPE)pCompHdl);

    switch(pRMcfg->eType)
    {
    case RM_RC_NMF_DOMAINS:
        {
            MSG4("RM_ENS_Cmd_SetRMConfig : RM_RC_NMF_DOMAINS received for handle 0x%x: index = %d | Domains SIA = %d | SVA = %d\n",getOMXHandle(),(int)pRMcfg->ctl.sNMF.nPortIndex,(int)pRMcfg->ctl.sNMF.nDidProcSIA,(int)pRMcfg->ctl.sNMF.nDidProcSVA);
            MSG3("\t\t\t\t\t Domains HWPipeSIA = %d | HWPipeSVA = %d | XP70SIA = %d\n",(int)pRMcfg->ctl.sNMF.nDidHWPipeSIA,(int)pRMcfg->ctl.sNMF.nDidHWPipeSVA,(int)pRMcfg->ctl.sNMF.nDidHWXP70SIA);

            if (pRMcfg->ctl.sNMF.nPortIndex >= (int)(pEnsComp->mPortCount)) {
                ENS_HwRm::HWRM_Notify_RMConfigApplied(pCompHdl,RM_RC_NMF_DOMAINS,pRMcfg->ctl.sNMF.nPortIndex,RM_E_KO);
                return;
            }

            DBC_ASSERT(pEnsComp->mRMP != 0);
            pEnsComp->mRMP->setNMFDomainHandle(pRMcfg);

            ENS_HwRm::HWRM_Notify_RMConfigApplied(pCompHdl,RM_RC_NMF_DOMAINS,pRMcfg->ctl.sNMF.nPortIndex,RM_E_NONE);
            break;
        }

    case RM_RC_AUDIOLOWPOWER_ENTER:
        {
            MSG4("RM_ENS_Cmd_SetRMConfig : RM_RC_AUDIOLOWPOWER_ENTER received for handle 0x%x: nDidAudLP = %d | nDidDmaBuf = %d | nDidAudDec = %d\n",getOMXHandle(),pRMcfg->ctl.sLowPowerEnter.nDidAudLP,pRMcfg->ctl.sLowPowerEnter.nDidDmaBuf,pRMcfg->ctl.sLowPowerEnter.nDidAudDec);
            pEnsComp->StartLowPower(pRMcfg->ctl.sLowPowerEnter.nDidAudLP);
            break;
        }
    case RM_RC_AUDIOLOWPOWER_EXIT:
        {
            MSG1("RM_ENS_Cmd_SetRMConfig : RM_RC_AUDIOLOWPOWER_EXIT received for handle 0x%x\n",getOMXHandle());
            pEnsComp->StopLowPower();
            break;
        }
    default:
        {
            ENS_HwRm::HWRM_Notify_RMConfigApplied(pCompHdl,RM_RC_AUDIOLOWPOWER_EXIT,0,RM_E_KO);
            break;
        }
    }// end of switch
}

void ENS_Component::RM_ENS_Cmd_ReleaseResource (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_IN OMX_BOOL bSuspend)
{
    ENS_Component * pEnsComp = (ENS_Component *)EnsWrapper_getEnsComponent((OMX_HANDLETYPE)pCompHdl);

    // Set the preemption state
    if (!bSuspend) {
        pEnsComp->setPreemptionState(Preempt_Total);
    } else {
        pEnsComp->setPreemptionState(Preempt_Partial);
    }

    // Send evt to the component
    ENS_CmdReleaseResourceEvt evt(bSuspend);
    OMX_ERRORTYPE error = pEnsComp->getCompFsm()->dispatch(&evt);
    if (error != OMX_ErrorNone) {
        OstTraceInt1(TRACE_ERROR, "ENS_PROXY: ENS_Component::RM_ENS_Cmd_ReleaseResource error 0x%x\n", error);
    }
}

RM_STATUS_E ENS_Component::RM_ENS_Get_Capabilities (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_OUT RM_CAPABILITIES_T* pCapabilities)
{
    // Check passed argument
    if ( !pCapabilities ) {
        return RM_E_INVALIDPTR;
    }

    // Retrieve the ens component
    ENS_Component * pEnsComp = (ENS_Component *)EnsWrapper_getEnsComponent((OMX_HANDLETYPE)pCompHdl);
    if ( !pEnsComp ) {
        return RM_E_INVALIDPTR;
    }

    // Initialize the component role
    for (unsigned int i=0;i<OMX_MAX_STRINGNAME_SIZE;i++){
        pCapabilities->sLoadTime.cRole[i] = pEnsComp->mActiveRole[i];
    }

    // Initialize the component name
    strcpy((char *)pCapabilities->sLoadTime.cName, pEnsComp->getName());

    // Initialize component priority settings
    pCapabilities->sPolicy.nGroupID = pEnsComp->getGroupPriorityId();
    pCapabilities->sPolicy.nGroupPriority = pEnsComp->getGroupPriorityValue();
    pCapabilities->sPolicy.bCanSuspend = 
        (pEnsComp->getSuspensionPolicy()==OMX_SuspensionEnabled? OMX_TRUE: OMX_FALSE);

    // Fill eCapBitmap flag
    RM_STATUS_E error = pEnsComp->getCapabilities(pCapabilities->sLoadTime.eCapBitmap);
    if (error != RM_E_NONE) {
        return error;
    }

    // TODO: initialize process/thread Id in server
    pCapabilities->sIds.nProcessId = 0;
    pCapabilities->sIds.nThreadId = 0;

    return RM_E_NONE;
}

ENS_API_EXPORT RM_STATUS_E ENS_Component::getCapabilities(OMX_INOUT OMX_U32 &pCapBitmap)
{
    // Default implementation, if not overloaded by component
    pCapBitmap = 0;
    return RM_E_NONE;
}


RM_STATUS_E ENS_Component::RM_ENS_Get_ResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    ENS_Component *pEnsComp = (ENS_Component *)EnsWrapper_getEnsComponent((OMX_HANDLETYPE)pCompHdl);
    return pEnsComp->getResourcesEstimation(pCompHdl, pEstimationData);
}

ENS_API_EXPORT RM_STATUS_E ENS_Component::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    // Default implementation, if not overloaded by component
    pEstimationData->sMcpsSva.nMax = 0;
    pEstimationData->sMcpsSva.nAvg = 0;
    pEstimationData->sMcpsSia.nMax = 0;
    pEstimationData->sMcpsSia.nAvg = 0;
    pEstimationData->sTcmSva.nSizeX = 0; // MMDSP_SVA_TCMX
    pEstimationData->sTcmSva.nSizeY = 0;   // MMDSP_SVA_TCMY
    pEstimationData->sTcmSia.nSizeX = 0;
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_EMPTY;
    pEstimationData->eUccBitmap = 0;
    return RM_E_NONE;
}


ENS_API_EXPORT OMX_U32 ENS_Component::getNMFDomainHandle(RM_NMFD_E domainType,OMX_U32 nPortIndex)
{
    DBC_ASSERT(mRMP != 0);
    return mRMP->getNMFDomainHandle(domainType,nPortIndex);
}


ENS_API_EXPORT OMX_U32 ENS_Component::getNMFDomainHandle(OMX_U32 nPortIndex)
{
    RM_NMFD_E domain = mRMP->getDefaultNMFDomainType(nPortIndex);
    return getNMFDomainHandle(domain,nPortIndex);
}

OMX_BOOL ENS_Component::allPortsInSameFsmState(FsmState state)
{
    for(OMX_U32 i = 0; i < getPortCount(); i++) {
        ENS_Port_Fsm * portFsm = getPort(i)->getPortFsm();
        DBC_ASSERT(portFsm);
        if(portFsm->getFsmState() != state)  return OMX_FALSE;
    }

    return OMX_TRUE;
}


ENS_API_EXPORT TraceInfo_t * ENS_Component::getSharedTraceInfoPtr(void) {
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

void ENS_Component::createBufferSharingGraph(void){
	OMX_U32 portCount = getPortCount();
    OMX_U32 connectedPortCount = 0,  supplierOutputPorts = 0, nonSupplierOutputPorts = 0;
    // Allocate dynamic array
    OMX_U32 *portList  = new OMX_U32[portCount];
    
	for(OMX_U32 i=0;i<portCount;i++) {
        if(getPort(i)->getDirection()==OMX_DirInput) {

            for(OMX_U32 j=0;j<portCount;j++) {
                if(mBufSharingGraph[i][j].isRequested){
                    portList[connectedPortCount] = j;
                    connectedPortCount++;
                    if(getPort(j)->isBufferSupplier())
                        supplierOutputPorts++;
                    else
                        nonSupplierOutputPorts++;
                }
            }

            if(connectedPortCount>1) {
                // An input port can serve multiple supplier ports
                DBC_ASSERT(nonSupplierOutputPorts==0 && connectedPortCount == supplierOutputPorts);
                if(getPort(i)->isBufferSupplier()) {
                    getPort(i)->setAllocatorRole(OMX_TRUE);
                    for(OMX_U32 k=0;k<connectedPortCount;k++) {
                         getPort(portList[k])->setAllocatorRole(OMX_FALSE);
                    }
                }
                else{
                    getPort(i)->setAllocatorRole(OMX_FALSE);
                    for(OMX_U32 k=0;k<connectedPortCount;k++) {
                         getPort(portList[k])->setAllocatorRole(OMX_FALSE);
                    }
                }
            }else if(connectedPortCount==1){
                if(getPort(i)->isBufferSupplier()) {
                    if(getPort(portList[0])->isBufferSupplier()) {
                        //Make output an allocator
                        getPort(i)->setAllocatorRole(OMX_FALSE);
                        getPort(portList[0])->setAllocatorRole(OMX_TRUE);
                    }
                }
                else{
                    DBC_ASSERT(nonSupplierOutputPorts==0);
                    getPort(i)->setAllocatorRole(OMX_FALSE);
                    getPort(portList[0])->setAllocatorRole(OMX_FALSE);
                }
            }
            else{
                if(getPort(i)->isBufferSupplier()) {
                     getPort(i)->setAllocatorRole(OMX_TRUE);
                }
                else
                     getPort(i)->setAllocatorRole(OMX_FALSE);
            }
        }
        else{ //output port
           for(OMX_U32 j=0;j<portCount;j++) {
                if(mBufSharingGraph[i][j].isRequested){
                    portList[connectedPortCount] = j;
                    connectedPortCount++;
                    if(getPort(j)->isBufferSupplier())
                        supplierOutputPorts++;
                    else
                        nonSupplierOutputPorts++;
                }
            }

            if(connectedPortCount>1) {
                // An output cannot be connected to multiple input ports
                DBC_ASSERT(0);
            }
            else if(connectedPortCount==1) {
                //Input port will set role for this port
            }
            else{
                if(getPort(i)->isBufferSupplier()) {
                     getPort(i)->setAllocatorRole(OMX_TRUE);
                }
                else
                     getPort(i)->setAllocatorRole(OMX_FALSE);
            }
        }

        if(connectedPortCount) {
            ENS_Port_p * connectedPorts = new ENS_Port_p[connectedPortCount];
            for(OMX_U32 j=0;j<connectedPortCount;j++) {
                *(connectedPorts+j) = getPort(portList[j]);
            }

            //Send this list to port
            getPort(i)->setBufferSharingPortList(connectedPorts,connectedPortCount);
            connectedPorts = 0;
        }
        else{
             getPort(i)->setBufferSharingPortList(0,0);
        }
     
         connectedPortCount = 0;
         supplierOutputPorts = 0;
         nonSupplierOutputPorts = 0;
    }

    delete [] portList;
}

/* This Connects an input port to output port and also ensures that an output port should
   be connected to only to one input port.
*/ 
ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::connectBufferSharingPorts(OMX_U32 inputPortIndex,
                                                                   OMX_U32 outputPortIndex){

    OMX_U32 portCount=getPortCount();
    OMX_U32 inputPortCount = 0UL;
    DBC_ASSERT(inputPortIndex<portCount && outputPortIndex<portCount);
	if(getPort(inputPortIndex)->getDirection()==getPort(outputPortIndex)->getDirection()) {
		return OMX_ErrorUndefined;
	}

	mBufSharingGraph[inputPortIndex][outputPortIndex].isRequested = OMX_TRUE;
	mBufSharingGraph[outputPortIndex][inputPortIndex].isRequested = OMX_TRUE;

    // Check output port should be connected to only one input port
    for(OMX_U32 i=0, inputPortCount=0; i<portCount;i++) {
        if(mBufSharingGraph[i][outputPortIndex].isRequested == OMX_TRUE) {
            inputPortCount++;
        }
    }
    if(inputPortCount>1) {
        mBufSharingGraph[inputPortIndex][outputPortIndex].isRequested = OMX_FALSE;
        mBufSharingGraph[outputPortIndex][inputPortIndex].isRequested = OMX_FALSE;
        return OMX_ErrorUndefined;
    }
    
    mBufferSharingEnabled = OMX_TRUE;

    BUFFER_SHARING_ENABLED = true;

    for(OMX_U32 i=0; i<portCount;i++) {
        getPort(i)->bufferSharingEnabled();
        getCompFsm()->bufferSharingEnabled();
    }

    
    return OMX_ErrorNone;
}

void ENS_Component::initBufferSharingGraph(){
    OMX_U32 i,j;
    mBufferSharingEnabled = OMX_FALSE;
    for(i=0;i<MAX_PORTS_COUNT_BUF_SHARING;i++) {
        for(j=0;j<MAX_PORTS_COUNT_BUF_SHARING;j++) {
            mBufSharingGraph[i][j].isRequested = OMX_FALSE;
            mBufSharingGraph[i][j].isBroken = OMX_TRUE;
        }
    }
}

void ENS_Component::disableBufferSharing(){
    for(OMX_U32 i=0;i<getPortCount();i++) {
        if(getPort(i)->isBufferSupplier()){
            getPort(i)->setAllocatorRole(OMX_TRUE);
            getPort(i)->setBufferSharingPortList(0,0);
        } else{
            getPort(i)->setAllocatorRole(OMX_FALSE);
            getPort(i)->setBufferSharingPortList(0,0);
        }
    }
}

ENS_API_EXPORT void
ENS_Component::NmfPanicCallback(void *contextHandler,
                                t_nmf_service_type serviceType,
                                t_nmf_service_data *serviceData)
{}

ENS_API_EXPORT OMX_STATETYPE
ENS_Component::omxState() const
{
    return this->getCompFsm()->getOMXState();
}

ENS_API_EXPORT void ENS_Component::setName(const char *cName)
{
    mName = cName;
}

ENS_API_EXPORT const char * ENS_Component::getName() const
{
    return mName.c_str();
}

ENS_API_EXPORT unsigned short ENS_Component::getTraceEnable() const
{
    return TraceObject::getTraceEnable();
}

ENS_API_EXPORT OMX_VERSIONTYPE ENS_Component::getVersion(void) const
{
    return mVersion;
}

ENS_API_EXPORT OMX_U32 ENS_Component::getUidTopDictionnary(void)
{
    return 0;
};

ENS_API_EXPORT OMX_U32 ENS_Component::getIndexEventCmdReceived()
{
    return OMX_EventCmdReceived;
};

ENS_API_EXPORT OMX_ERRORTYPE ENS_Component::construct(void)
{
    return getProcessingComponent().construct();
};

ENS_API_EXPORT void *ENS_Component::GetProcessingComponent(void)
{
    return (void *)(mProcessingComponent);
};
