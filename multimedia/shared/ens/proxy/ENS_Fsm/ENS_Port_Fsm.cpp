/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Port_Fsm.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Port_Fsm.h"
#include "ProcessingComponent.h"
#include "ENS_Client.h"
#define OMXCOMPONENT "ENS_PORT_FSM"

#include "rme_types.h"

#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Fsm_ENS_Port_FsmTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

ENS_API_EXPORT OMX_STATETYPE ENS_Port_Fsm::getState(void)
{   
    if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded)) {
        return OMX_StateLoaded;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToIdleHOST)) {
        return OMX_StateLoadedToIdleHOST;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToIdleDSP)) {
        return OMX_StateLoadedToIdleDSP;
    }
	else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateCheckRscAvailability)) {
		return OMX_StateCheckRscAvailability;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToTestForResources)) {
		return OMX_StateLoadedToTestForResources;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTestForResources)) {
		return OMX_StateTestForResources;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateWaitForResources)) {
		return OMX_StateWaitForResources;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle)) {
        return OMX_StateIdle;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdleToLoaded)) {
        return OMX_StateIdleToLoaded;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateExecuting)) {
        return OMX_StateExecuting;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStatePause)) {
        return OMX_StatePause;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransient) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToIdle)) {
        return OMX_StateTransient;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitBuffersFromNeighbor) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableHOST) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP) ||
            getFsmState() == static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled)) {
        return mDisabledState;
    }
    else {
        return OMX_StateInvalid;
    }
}

FsmState ENS_Port_Fsm::getFsmParentState(OMX_STATETYPE parentState)
{   
    if (parentState == OMX_StateLoaded) {
        return static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded);
    }
    else if (parentState == OMX_StateIdle) {
        return static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle);
    }
    else if (parentState == OMX_StateExecuting) {
        return static_cast<FsmState>(&ENS_Port_Fsm::OmxStateExecuting);
    }
    else if (parentState == OMX_StatePause) {
        return static_cast<FsmState>(&ENS_Port_Fsm::OmxStatePause);
    }
    else {
        return static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid);;
    }
}

ENS_API_EXPORT void ENS_Port_Fsm::emptybufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{    
	MSG3("emptybufferdone_cb received from ProcessingComponent : h=0x%x portIdx=%d pBuffer=0x%x\n", 
            mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex(), pBuffer);
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: emptybufferdone_cb received pBuffer=0x%x", (unsigned int)pBuffer);
    ENS_CmdBufferHdrEvt evt(OMX_EMPTY_BUFFER_DONE_SIG, pBuffer);
    dispatch(&evt);
}

ENS_API_EXPORT void ENS_Port_Fsm::fillbufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{    
    MSG3("fillbufferdone_cb received from ProcessingComponent : h=0x%x portIdx=%d pBuffer=0x%x\n", 
            mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex(), pBuffer);
	OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: fillbufferdone_cb received pBuffer=0x%x", (unsigned int)pBuffer);
    ENS_CmdBufferHdrEvt evt(OMX_FILL_BUFFER_DONE_SIG, pBuffer);
    dispatch(&evt);
}

inline ProcessingComponent &
ENS_Port_Fsm::getProcessingComponent() {
    return mPort.getENSComponent().getProcessingComponent();
}

OMX_ERRORTYPE ENS_Port_Fsm::checkStandardTunnelingInit(void)
{   
if(!BUFFER_SHARING_ENABLED){
    if (!mPort.getTunneledComponent()
            || !mPort.useStandardTunneling()
            || !mPort.isBufferSupplier()) {
        return OMX_ErrorNone;
    }

}else{
    if (!mPort.getTunneledComponent()
            || !mPort.useStandardTunneling()
            || !mPort.isBufferSupplier()
            || !mPort.isAllocatorPort()) {
        return OMX_ErrorNone;
    }
}

    return mPort.standardTunnelingInit();
}


OMX_ERRORTYPE ENS_Port_Fsm::checkStandardTunnelingDeInit(void)
{
    if (!mPort.getTunneledComponent()
            || !mPort.useStandardTunneling()
            || !mPort.isBufferSupplier())
    {
       return OMX_ErrorNone;
    }

if(!BUFFER_SHARING_ENABLED){
    return mPort.standardTunnelingDeInit();
}else{
    // Case of a Standard tunneling buffer supplier
    if(mPort.isAllocatorPort()) {
        return mPort.standardTunnelingDeInit();
    }
    else{
        return mPort.bufferSharingDeinit();
    }
}
}

OMX_ERRORTYPE ENS_Port_Fsm::goLoadedToIdle(void)
{
	// Case proprietary tunneling or std tunneling buffer supplier
    if (mPort.useProprietaryCommunication() 
            ||  (mPort.getTunneledComponent() && mPort.useStandardTunneling() &&
                 mPort.isBufferSupplier())) {
        stateTransition(static_cast<FsmState>(
                    &ENS_Port_Fsm::OmxStateCheckRscAvailability));
    } else {
        if (mPort.isPopulated()) {
            if (mPort.getTunneledComponent() != (OMX_HANDLETYPE)NULL) {
                stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateCheckRscAvailability));
            } else {
                return OMX_ErrorIncorrectStateOperation;
            }
        } else {
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToIdleHOST));
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port_Fsm::goIdleToLoaded(void)
{
    OMX_ERRORTYPE error;

    //Check if we need to reset port for standard tunneling
    error = checkStandardTunnelingDeInit();
    if(error != OMX_ErrorNone) {
		OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goIdleToLoaded error: %d", error);
        return error;
    }
    if (mPort.useProprietaryCommunication() 
            || !mPort.isPopulated()) {
        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
    } else {
        if (mPort.getBufferCountCurrent() == 0) {
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
        } else {
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdleToLoaded));
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port_Fsm::goToDisable(OMX_STATETYPE currentState, OMX_U32 portIndex)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    mDisabledState = currentState;

    if(mDisabledState == OMX_StateLoaded) {
        mPort.setEnabledFlag(OMX_FALSE);
        //send OMX_EventCmdReceived to ENSWrapper : in loaded state -> processingComponent not created yet. 
        eventHandlerCB(OMX_EventCmdReceived, OMX_CommandPortDisable, mPort.getPortIndex());  
        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
        return OMX_ErrorNone;
    }

    //if standard/proprietary tunneling, check that non-buffer supplier port of tunnel is
    //disabled after buffer supplier port.
    if(mPort.getTunneledComponent() &&
            !mPort.isBufferSupplier()) {
        OMX_ERRORTYPE error;
        OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = mPort.getTunneledPort();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        error = OMX_GetParameter(mPort.getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if (error != OMX_ErrorNone) {
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goToDisable error: %d", error);		
			return error;
		}
        if(tunneledPortDef.bEnabled == OMX_TRUE) {
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goToDisable error: incorrectStateOperation");		
            return OMX_ErrorIncorrectStateOperation;
        }
    }

    // Save component rsc estimation here to check if eSRAM need has changed
    RM_EMDATA_T savedRsc, newRsc;
    mPort.getENSComponent().getResourcesEstimation(0, &savedRsc);

    mPort.setEnabledFlag(OMX_FALSE);

    if(portIndex == OMX_ALL) { 
        mEventAppliedOnEachPorts = OMX_TRUE;
    }

    //If standard tunneling or non-tunneled communication : send OMX_EventCmdReceived to ENSWrapper 
    //--> OMX_SendCommand can not be locked. In current implementation, processingcomponent will not
    //received the sendCommand before all buffers returned to supplier OMX component.
    if(!mPort.useProprietaryCommunication()) {
        eventHandlerCB(OMX_EventCmdReceived, OMX_CommandPortDisable, mPort.getPortIndex());  
    }

    if(!BUFFER_SHARING_ENABLED){
        if(mPort.useStandardTunneling() 
           && mPort.isBufferSupplier()
           && !mPort.isAllBuffersReturnedFromNeighbor()){
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitBuffersFromNeighbor));
        } else {
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
        }
    }else{
        if(mPort.useStandardTunneling() && mPort.isBufferSupplier())
        {
            if(mPort.isAllocatorPort()) {
                if(!mPort.isAllBuffersReturnedFromNeighbor()
                   ||!mPort.isAllBuffersReturnedFromSharingPorts()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitBuffersFromNeighbor));
                }
                else
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
            }
            
            if(mPort.isSharingPort()) {
                if(!mPort.isAllBuffersReturnedFromNeighbor()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitBuffersFromNeighbor));
                }
                else
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
            }
        } else {
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
        }
        
    }

    // Check if eSRAM estimation has changed after disabling
    if (mPort.getENSComponent().omxState()== OMX_StateLoaded) {
        return OMX_ErrorNone;
    }

    mPort.getENSComponent().getResourcesEstimation(0, &newRsc);
    if ( savedRsc.sEsramBufs.nSvaHwPipe != newRsc.sEsramBufs.nSvaHwPipe ||
         savedRsc.sEsramBufs.nSiaHwPipe != newRsc.sEsramBufs.nSiaHwPipe ||
         savedRsc.sEsramBufs.nSiaHwXP70 != newRsc.sEsramBufs.nSiaHwXP70 ) 
    {
        
        if (mPort.getENSComponent().resourcesSuspended()==false) {
            OMX_ERRORTYPE err = mPort.getENSComponent().suspendResources();
            if (err == OMX_ErrorNone) {
                mPort.getENSComponent().resourcesSuspended(true);
                error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mPort.getENSComponent().getOMXHandle(), 
                                                      mPort.getENSComponent().omxState(),
                                                      RM_RS_REQUIRED,
                                                      0);
                if(error != OMX_ErrorNone) return error;
            } else {
                return err;
            }
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Port_Fsm::goToEnable(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;

    if(mDisabledState == OMX_StateLoaded) {
        mPort.setEnabledFlag(OMX_TRUE);
        //send OMX_EventCmdReceived to ENSWrapper : in loaded state -> processingComponent not created yet. 
        eventHandlerCB(OMX_EventCmdReceived, OMX_CommandPortEnable, mPort.getPortIndex());  
        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
        mDisabledState = OMX_StateInvalid;
        return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex());
    }

    //if standard/proprietary tunneling, check that buffer supplier port of tunnel is
    //enabled after buffer supplier port.
    if(mPort.getTunneledComponent() &&
            mPort.isBufferSupplier()) {
        OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;
        tunneledPortDef.nSize       = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        tunneledPortDef.nPortIndex  = mPort.getTunneledPort();
        getOmxIlSpecVersion(&tunneledPortDef.nVersion);
        error = OMX_GetParameter(mPort.getTunneledComponent(), 
                OMX_IndexParamPortDefinition, &tunneledPortDef);
        if (error != OMX_ErrorNone) {
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goToEnable error: %d", error);					
			return error;
		}
        if(tunneledPortDef.bEnabled == OMX_FALSE) {
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goToEnable error: incorrectStateOperation");					
			return OMX_ErrorIncorrectStateOperation;
		}
    }

    // Inform RM that resource estimation may have change
    RM_EMDATA_T savedRsc, newRsc;
    mPort.getENSComponent().getResourcesEstimation(0, &savedRsc);
 
    mPort.setEnabledFlag(OMX_TRUE);

    if(portIndex == OMX_ALL) { 
        mEventAppliedOnEachPorts = OMX_TRUE;
    }

    //Check if we need to initialize port for standard tunneling
    error = checkStandardTunnelingInit();
    if(error != OMX_ErrorNone)  {
        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: goToDisable error: bad standard tunneling init %d", error);		
        return OMX_ErrorNone;   // do not return the error, otherwise the transition is not taken into account
    }

    //If standard tunneling or non-tunneled communication : send OMX_EventCmdReceived to ENSWrapper 
    //--> OMX_SendCommand can not be locked. In current implementation, processingcomponent will not
    //received the sendCommand before reception of OMX_UseBuffer/OMX_AllocateBuffer.
    if(!mPort.useProprietaryCommunication()) {
        eventHandlerCB(OMX_EventCmdReceived, OMX_CommandPortEnable, mPort.getPortIndex());  
    }

    if (mPort.useProprietaryCommunication() 
            || mPort.isPopulated()) {
        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP));
    } else {
        stateTransition(static_cast<FsmState>(
                    &ENS_Port_Fsm::OmxStateTransientToEnableHOST));
    }

    // Check if eSRAM estimation has changed after enabling
    if (mPort.getENSComponent().omxState()== OMX_StateLoaded) {
        return OMX_ErrorNone;
    }

    mPort.getENSComponent().getResourcesEstimation(0, &newRsc);
    if ( savedRsc.sEsramBufs.nSvaHwPipe != newRsc.sEsramBufs.nSvaHwPipe ||
         savedRsc.sEsramBufs.nSiaHwPipe != newRsc.sEsramBufs.nSiaHwPipe ||
         savedRsc.sEsramBufs.nSiaHwXP70 != newRsc.sEsramBufs.nSiaHwXP70 )
    {
        
        if (mPort.getENSComponent().resourcesSuspended()==false) {
            OMX_ERRORTYPE err = mPort.getENSComponent().suspendResources();
            if (err == OMX_ErrorNone) {
                mPort.getENSComponent().resourcesSuspended(true);
                error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mPort.getENSComponent().getOMXHandle(), 
                                                      mPort.getENSComponent().omxState(),
                                                      RM_RS_REQUIRED,
                                                      0);
                if(error != OMX_ErrorNone) return error;
            } else {
                return err;
            }
        }
    }

    return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//! \brief Default Behavior for all states of the port FSM
//!
//! This method implements default behavior for all states of the port FSM.
//! It is intended to be called in the default case of each state handler method.
//! State handlers should handle events only when the behavior is different.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------

OMX_ERRORTYPE ENS_Port_Fsm::defaultBehavior(const ENS_FsmEvent *evt) 
{
    switch (evt->signal()) {
        case ENTRY_SIG:
            return OMX_ErrorNone;

        case EXIT_SIG:
            return OMX_ErrorNone;

        case OMX_GETPARAMETER_SIG:
            {
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                return mPort.getParameter(getParamEvt->getIndex(), getParamEvt->getStructPtr());
            }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                OMX_PARAM_PORTDEFINITIONTYPE old;
                
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                if (updateSettingsEvt->getIndex() == OMX_IndexParamPortDefinition) {
                    old = mPort.getValueParamPortDefinition();
                }
                OMX_ERRORTYPE error = mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior UPDATE_PORT_SETTING_SIG error: %d", error);	
					return error;
				}
                
                if ((updateSettingsEvt->getIndex()!=OMX_IndexParamPortDefinition) || mPort.mustSendEventHandler(&old)) {
#ifdef OLD_EVENT_PORTSETTINGSCHANGED
                    return eventHandlerCB(OMX_EventPortSettingsChanged, mPort.getPortIndex(), 0);
#else
                    return eventHandlerCB(OMX_EventPortSettingsChanged, mPort.getPortIndex(),updateSettingsEvt->getIndex() );
#endif
                } else {
                    return OMX_ErrorNone;
                }
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
            {
                ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
                OMX_ERRORTYPE err = OMX_ErrorNone;

                mPort.bufferReturnedFromNeighbor(pOMXBuffer);
                mPort.bufferSentToProcessingComponent(pOMXBuffer);

                err = getProcessingComponent().emptyThisBuffer(pOMXBuffer);
                if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior EMPTY_THIS_BUFFER_SIG error: %d", err);			
                    mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                }

                return err;
            }

        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ENS_CmdEmptyThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);

                mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                
                if (!mPort.getTunneledComponent()) {
                    mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
                } else {
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                }

                return OMX_ErrorNone;
            }
        case OMX_FILL_THIS_BUFFER_SIG:
            {
                ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
                OMX_ERRORTYPE err = OMX_ErrorNone;

                mPort.bufferReturnedFromNeighbor(pOMXBuffer);
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior OMX_FILL_THIS_BUFFER_SIG 0x%x",(unsigned int)pOMXBuffer);
if(!BUFFER_SHARING_ENABLED){
                mPort.bufferSentToProcessingComponent(pOMXBuffer);

                err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
                if (err != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d", err);			
                    mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                }
}else{
                if(mPort.getNumberInterConnectedPorts()) {
                    err = mPort.forwardOutputBuffer(pOMXBuffer);
                    if(err!=OMX_ErrorNone) return err;

                    mPort.bufferSentToSharingPort(pOMXBuffer);
                }
                else{
                
                    mPort.bufferSentToProcessingComponent(pOMXBuffer);

                    err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
                    if (err != OMX_ErrorNone) {
                        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d", err);			
                        mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    }
                }
}
                return err;
            }

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ENS_CmdFillThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                OMX_BOOL sendBufferToNeighbor;

                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                
                if(sendBufferToNeighbor) {
                    copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                    mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                    if (!mPort.getTunneledComponent()) {
                        mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
                    } else {
                        OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                    }
                }

                return OMX_ErrorNone;

            }

        case OMX_EMPTY_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *emptyBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

if(!BUFFER_SHARING_ENABLED){
                mPort.bufferSentToNeighbor(pOMXBuffer);

                if (!mPort.getTunneledComponent()) {
                    mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
                } else {
                    OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }
}else{
                if(mPort.getNumberInterConnectedPorts()) {
                    OMX_ERRORTYPE error = mPort.forwardInputBuffer(pOMXBuffer);
                    if(error!=OMX_ErrorNone) return error;

                    mPort.bufferSentToSharingPort(pOMXBuffer);
                }
                else{
                    mPort.bufferSentToNeighbor(pOMXBuffer);

                    if (!mPort.getTunneledComponent()) {
                        mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
                    } else {
                        OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                    }
               }
}

                return OMX_ErrorNone;
            }

        case OMX_FILL_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *fillBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);            
                mPort.bufferSentToNeighbor(pOMXBuffer);

                if (!mPort.getTunneledComponent()) {
                    mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
                } else {
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }

                return OMX_ErrorNone;
            }

        case OMX_FREE_BUFFER_SIG:
            {
                ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;
                OMX_ERRORTYPE error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior FREE_BUFFER_SIG error: %d", error);	
					return error;
				}
                return eventHandlerCB(OMX_EventError, (t_uint32)OMX_ErrorPortUnpopulated, mPort.getPortIndex());
            }
        
        case OMX_ENABLE_PORT_SIG:
        case OMX_DISABLE_PORT_SIG:
        case OMX_FLUSH_PORT_SIG:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: Enable / Disable / Flush: incorrect state operation");	
            return eventHandlerCB(OMX_EventError, (t_uint32)OMX_ErrorSameState, mPort.getPortIndex());
            
        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior error: incorrect state operation");	
            return OMX_ErrorIncorrectStateOperation;

    }
}


//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Loaded state
//!
//! This method implements the behavior of the port FSM when it is in Loaded state.
//! When the port FSM is in this state, it means that all tunnel buffers have been 
//! freed.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateLoaded(const ENS_FsmEvent *evt) {
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
			MSG2("OmxStateLoaded ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoaded ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_SETSTATE_SIG:
            {
                ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
                OMX_STATETYPE targetState = setStateEvt->targetState();

                if(targetState == OMX_StateIdle) {
                    OMX_ERRORTYPE err = goLoadedToIdle();
                    return err;
                } else if(targetState == OMX_StateTestForResources) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToTestForResources));
                    return OMX_ErrorNone;
                } else if(targetState == OMX_StateWaitForResources) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateWaitForResources));
                    return OMX_ErrorNone;
                } else if(targetState == OMX_StateLoaded) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoaded SETSTATE_SIG error: same state");			
                    return OMX_ErrorSameState;
                } else {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoaded SETSTATE_SIG error: incorrect state transition");		
                    return OMX_ErrorIncorrectStateTransition;
                }
            }

        case OMX_SETPARAMETER_SIG:
            {
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                return mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
            }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }

        case OMX_DISABLE_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                return goToDisable(OMX_StateLoaded, portEvt->getPortIdx());
            }

        case OMX_COMP_TUNNEL_REQUEST_SIG:
            {
                ENS_CmdTunnelRequestEvt *tunnelReqEvt = (ENS_CmdTunnelRequestEvt *) evt;
                OMX_ERRORTYPE err = mPort.tunnelRequest( tunnelReqEvt->getTunneledCompHdl(), tunnelReqEvt->getTunneledPortIdx(), tunnelReqEvt->getTunnelStructPtr());
                if (OMX_ErrorNone != err)
                    {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoaded COMP_TUNNEL_REQUEST_SIG error: %d", err);			
                    return err;
                    }
                // Notify the RM if port is an output port OR standard Tunneled
                if (mPort.getDirection()==OMX_DirOutput) {
                    ENS_HwRm::HWRM_Notify_Tunnel(mPort.getComponent(),
                                                 mPort.getPortIndex(),
                                                 tunnelReqEvt->getTunneledCompHdl(),
                                                 tunnelReqEvt->getTunneledPortIdx());
                }
                else if (mPort.useStandardTunneling()) {
                     ENS_HwRm::HWRM_Notify_Tunnel(tunnelReqEvt->getTunneledCompHdl(),
                                                  tunnelReqEvt->getTunneledPortIdx(),
                                                  mPort.getComponent(),
                                                  mPort.getPortIndex());
                }
                return OMX_ErrorNone;
            }

        case OMX_FLUSH_PORT_SIG:
            return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());
            
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoaded error: incorrect state operation %d", (unsigned int)evt->signal());			
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }

}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Idle state
//!
//! This method implements the behavior of the port FSM when it is in Idle state.
//! When the port FSM is in this state, it means that the port is populated and 
//! the processing component has been instantiated.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateIdle(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateIdle ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdle ENTRY_SIG: isAllBuffersAtOwner = %d", mPort.isAllBuffersAtOwner());
            DBC_ASSERT(mPort.isAllBuffersAtOwner());
	    
            return OMX_ErrorNone;
        }

        case OMX_SETSTATE_SIG:
            {
                ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
                OMX_STATETYPE targetState = setStateEvt->targetState();

                if (targetState == OMX_StateLoaded) {
                    return goIdleToLoaded();
                } 
                else if(targetState == OMX_StateExecuting || targetState ==  OMX_StatePause) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransient)); 
                    return OMX_ErrorNone;
                } 
                else if (targetState == OMX_StateIdle) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdle SETSTATE_SIG error: same state");
                    return OMX_ErrorSameState;
                } 
                else {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdle SETSTATE_SIG error: incorrect state transition");
                    return OMX_ErrorIncorrectStateTransition;
                }
            }

        case OMX_DISABLE_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                return goToDisable(OMX_StateIdle, portEvt->getPortIdx());
            }

        case OMX_FLUSH_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                if(portEvt->getPortIdx() != OMX_ALL) {
			        return getProcessingComponent().sendCommand(OMX_CommandFlush, mPort.getPortIndex());
                } else if(mPort.getPortIndex() == (mPort.getENSComponent().getPortCount()-1)) {
                    return getProcessingComponent().sendCommand(OMX_CommandFlush, OMX_ALL);
                }
                return OMX_ErrorNone;
            }
        
		case OMX_FLUSHPORTCOMPLETE_SIG:
            return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());

        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
		case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdle error: incorrect state operation %d", (unsigned int)evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Executing state
//!
//! This method implements the behavior of the port FSM when it is in Executing state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateExecuting(const ENS_FsmEvent *evt) {
    
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateExecuting ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateExecuting ENTRY_SIG");
            if (!mPort.useStandardTunneling()) return OMX_ErrorNone;
            return mPort.standardTunnelingTransfertInit();
        }

        case OMX_SETSTATE_SIG:
            {
                ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
                OMX_STATETYPE targetState = setStateEvt->targetState();

                if (targetState == OMX_StateIdle ) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToIdle));
                    return OMX_ErrorNone;
                } else if (targetState == OMX_StatePause) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransient));
                    return OMX_ErrorNone;
                } else if (targetState == OMX_StateExecuting) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateExecuting SETSTATE_SIG error: same state");	
                    return OMX_ErrorSameState;
                } else {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateExecuting SETSTATE_SIG error: incorrect state transition");	
                    return OMX_ErrorIncorrectStateTransition;
                }
            }

        case OMX_DISABLE_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                return goToDisable(OMX_StateExecuting, portEvt->getPortIdx());
            }

        case OMX_FLUSH_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                if(portEvt->getPortIdx() != OMX_ALL) {
			        return getProcessingComponent().sendCommand(OMX_CommandFlush, mPort.getPortIndex());
                } else if(mPort.getPortIndex() == (mPort.getENSComponent().getPortCount()-1)) {
                    return getProcessingComponent().sendCommand(OMX_CommandFlush, OMX_ALL);
                }
                return OMX_ErrorNone;
            }
        
		case OMX_FLUSHPORTCOMPLETE_SIG:
            return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is in OMX Pause state
//!
//! This method implements the behavior of the port FSM when it is in Pause state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStatePause(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
            {
                MSG2("OmxStatePause ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
				OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStatePause ENTRY_SIG");
                return OMX_ErrorNone;
            }

        case OMX_SETSTATE_SIG:
            {
                ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
                OMX_STATETYPE targetState = setStateEvt->targetState();

                if (targetState == OMX_StateIdle ) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToIdle));
                    return OMX_ErrorNone;
                } else if (targetState == OMX_StateExecuting ) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransient));
                    return OMX_ErrorNone;
                } else if(targetState == OMX_StatePause) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStatePause SETSTATE_SIG error: same state");	
                    return OMX_ErrorSameState;
                } else {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStatePause SETSTATE_SIG error: incorrect state transition");	
                    return OMX_ErrorIncorrectStateTransition;
                }
            }

        case OMX_DISABLE_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                return goToDisable(OMX_StatePause, portEvt->getPortIdx());
            }

        case OMX_FLUSH_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                if(portEvt->getPortIdx() != OMX_ALL) {
			        return getProcessingComponent().sendCommand(OMX_CommandFlush, mPort.getPortIndex());
                } else if(mPort.getPortIndex() == (mPort.getENSComponent().getPortCount()-1)) {
                    return getProcessingComponent().sendCommand(OMX_CommandFlush, OMX_ALL);
                }
                return OMX_ErrorNone;
            }
        
		case OMX_FLUSHPORTCOMPLETE_SIG:
            return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandFlush, mPort.getPortIndex());
        
		default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Loaded to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Loaded to Idle. This transient state waits for calls
//! to OMX_AllocateBuffer or OMX_UseBuffer until the port is populated.
//! It then transitions to OmxStateLoadedToIdleDsp.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateLoadedToIdleHOST(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateLoadedToIdleHOST ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToidleHOST ENTRY_SIG");
            
            // From ENS_Port_Fsm::goLoadedToIdle(), this state is not used when we are in standard tunneling and Buffer supplier
            // This is because the component generates itself the allocation of buffers, and does not receive
            // OMX_AllocateBuffer() from the application
            DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
            return OMX_ErrorNone;
        }

        case OMX_SETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED) {
                OMX_ERRORTYPE error;
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                // Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                
                error =  mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;

                error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;
                
                return OMX_ErrorNone;
}
else{
				ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                // Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                return mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
            
}
           }

        case OMX_USE_BUFFER_SIG:
            {
                ENS_CmdUseBufferEvt *useBufferEvt = (ENS_CmdUseBufferEvt *) evt;

                error = mPort.useBuffer(
                        useBufferEvt->getOMXBufferHdrPtr(), useBufferEvt->getAppPrivate(),
                        useBufferEvt->getSizeBytes(), useBufferEvt->getBufferPtr());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToIdleHOST USE_BUFFER_SIG error: %d", error);	
					return error;
				}

                if(mPort.isPopulated()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateCheckRscAvailability));
                }

                return OMX_ErrorNone;
            }

        case OMX_ALLOCATE_BUFFER_SIG:
            {
                ENS_CmdAllocBufferEvt *allocBufferEvt = (ENS_CmdAllocBufferEvt *) evt;

                error = mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(), 
                        allocBufferEvt->getAppPrivate(), allocBufferEvt->getSizeBytes());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToIdleHOST ALLOCATE_BUFFER_SIG error: %d", error);	
					return error;
				}

                if (mPort.isPopulated()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateCheckRscAvailability));
                }

                return OMX_ErrorNone;
            }

        case OMX_GETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED){

                OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
                OMX_ERRORTYPE error;
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
                error =  mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error;

                error = mPort.calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;

                OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                        static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

                // Calculate self requirements
                maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
                maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

                portdef->nBufferSize = maxBufferSize;
                portdef->nBufferCountActual = maxBufferCountActual;
                
                return OMX_ErrorNone;
}
else{
           return defaultBehavior(evt);     
}
            }
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
		case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:

			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToIdleHOST error: incorrect state operation %d", evt->signal());	
            return OMX_ErrorIncorrectStateOperation;


        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Loaded to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Loaded to Idle. This transient state waits for the command complete event
//! from the processing component. It then transitions to the OmxStateIdle state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateLoadedToIdleDSP(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateLoadedToIdleDSP ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToidleDSP ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_SETSTATECOMPLETE_SIG:
            {
                ENS_CmdSetStateCompleteEvt *setStateCompleteEvt = (ENS_CmdSetStateCompleteEvt *) evt;
                OMX_STATETYPE newState = setStateCompleteEvt->newState();

                if(newState == OMX_StateIdle) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                } else {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
                }
                return OMX_ErrorNone;
            }

        case OMX_USE_SHARED_BUFFER_SIG:
        {
            DBC_ASSERT(mPort.getPortRole()==ENS_SHARING_PORT);
            // Sharing ports receives buffer from non supplier/allocator
            // Forward this buffer to tunneled port
            ENS_CmdUseSharedBufferEvt *useSharedBufferevt = (ENS_CmdUseSharedBufferEvt *) evt;
            return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),useSharedBufferevt->getSizeBytes(),
                                  useSharedBufferevt->getBufferPtr(),useSharedBufferevt->getSharedChunk());

        }

        case OMX_SETPARAMETER_SIG:
        {
if(BUFFER_SHARING_ENABLED){
			if(mPort.getPortRole()==ENS_SHARING_PORT) {
                // Its a sharing port
                OMX_ERRORTYPE error;
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                // Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                
                error =  mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;
                
                error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;
                
                return OMX_ErrorNone;
            }
            else
                return OMX_ErrorIncorrectStateOperation;
}else{
            return OMX_ErrorIncorrectStateOperation;
}
        }

        case OMX_GETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED){			
                OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
                OMX_ERRORTYPE error;
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
                error =  mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error;

                error = mPort.calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;

                OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                        static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

                // Calculate self requirements
                maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
                maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

                portdef->nBufferSize = maxBufferSize;
                portdef->nBufferCountActual = maxBufferCountActual;
                
                return OMX_ErrorNone;
}
else{
				return defaultBehavior(evt);
}
            }
        
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToIdleDSP error: incorrect state operation %d", evt->signal());	
            return OMX_ErrorIncorrectStateOperation;


        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler when the port FSM is transitionning from Idle to Loaded
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Idle to Loaded. This transient states entered once the processing component 
//! has been de-instantiated. It waits for calls to OMX_FreeBuffer for all buffer.
//! it then transitions to the OmxStateLoaded state
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateIdleToLoaded(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateIdleToLoaded ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdleToLoaded ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }

        case OMX_FREE_BUFFER_SIG:
            {
                ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;
                error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdleToLoaded FREE_BUFFER_SIG error: %d", error);	
					return error;
				}

                if (mPort.getBufferCountCurrent() == 0) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
                }
                return OMX_ErrorNone;
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
		case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateIdleToLoaded error: incorrect state operation %d", evt->signal());	
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief transient state used when transitionning from Executing/Pause to Idle
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! from Executing or Pause to Idle.
//! In this state, the port waits for buffers to come back to their owner. it 
//! then transitions to the OmxStateIdle state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToIdle(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateTransientToIdle ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_SETSTATECOMPLETE_SIG:
            {
                ENS_CmdSetStateCompleteEvt *setStateCompleteEvt = (ENS_CmdSetStateCompleteEvt *) evt;
                OMX_STATETYPE newState = setStateCompleteEvt->newState();
				OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_SETSTATECOMPLETE_SIG");

                if (newState != OMX_StateIdle) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
                } 
                else if (!mPort.isAllBuffersAtOwner()) {
                    if(mPort.useStandardTunneling() &&mPort.isBufferSupplier())
            		{
                        //Standard Tunneling case : Buffer supplier port is still waiting 
                        //for buffers from non-supplier port.
                        mWaitBuffersFromNeighbor = OMX_TRUE;
            		}
            		else
            		{
                        OstTraceFiltInst0(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_SETSTATECOMPLETE_SIG mWaitBuffersToNeighbor");
                        mWaitBuffersToNeighbor = OMX_TRUE;
            		}
                } else {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }

                return OMX_ErrorNone;
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
            {
                
                ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
               

                if(!mPort.useStandardTunneling() || !mPort.isBufferSupplier()) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_THIS_BUFFER_SIG incorrect state operation");
                    return OMX_ErrorIncorrectStateOperation;
                }

                mPort.bufferReturnedFromNeighbor(pBuffer);
if(BUFFER_SHARING_ENABLED){
                if(mPort.isSharingPort()) {
                    OMX_ERRORTYPE error = mPort.forwardInputBuffer(pBuffer);
                    if(error!=OMX_ErrorNone) return error;

                    mPort.bufferSentToSharingPort(pBuffer);
                }
}
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_THIS_BUFFER_SIG 0x%x",(unsigned int)(emptyThisBufferEvt->getOMXBufferHdrPtr()));

                if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersFromNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }

                return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_BUFFER_SIG:
            {
                
                ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
                

                if(!mPort.useStandardTunneling() || !mPort.isBufferSupplier()) {
                    return OMX_ErrorIncorrectStateOperation;
                }

                mPort.bufferReturnedFromNeighbor(pBuffer);
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_FILL_THIS_BUFFER_SIG 0x%x",(unsigned int)(fillThisBufferEvt->getOMXBufferHdrPtr()));
if(BUFFER_SHARING_ENABLED){
                if(mPort.isSharingPort()) {
                    OMX_ERRORTYPE error = mPort.forwardOutputBuffer(pBuffer);
                    if(error!=OMX_ErrorNone) return error;

                    mPort.bufferSentToSharingPort(pBuffer);
                }
}

                if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersFromNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }

                return OMX_ErrorNone;
            }

        case OMX_EMPTY_BUFFER_DONE_SIG:
            {
                
                ENS_CmdBufferHdrEvt *emptyBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
               

                if (mWaitBuffersFromNeighbor == OMX_TRUE) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
                    return OMX_ErrorNone;
                }

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
if(BUFFER_SHARING_ENABLED){
                if(mPort.isSharingPort()) {
                        OMX_ERRORTYPE error = mPort.forwardInputBuffer(pOMXBuffer);
                        if(error!=OMX_ErrorNone) return error;

                        mPort.bufferSentToSharingPort(pOMXBuffer);
                }
}
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_EMPTY_BUFFER_DONE_SIG 0x%x",(unsigned int)(pOMXBuffer));
                if(!mPort.getTunneledComponent()) {
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
                } else if(!mPort.isBufferSupplier()) {
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }


                if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersToNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }
		 
                return OMX_ErrorNone;
            }

        case OMX_FILL_BUFFER_DONE_SIG:
            {
                
                ENS_CmdBufferHdrEvt *fillBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
                
                if (mWaitBuffersFromNeighbor == OMX_TRUE) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
                    return OMX_ErrorNone;
                }

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);            
if(BUFFER_SHARING_ENABLED){
                DBC_ASSERT(mPort.isSharingPort()==OMX_FALSE);
}
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle OMX_FILL_BUFFER_DONE_SIG 0x%x",(unsigned int)(pOMXBuffer));
                if(!mPort.getTunneledComponent()) {
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
                } else if(!mPort.isBufferSupplier()){
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }

                if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersToNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }
		 
                return OMX_ErrorNone;
            }

        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            {
                
                ENS_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ENS_CmdEmptyThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                
                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

                if(!mPort.isSharingPort()) {
                    if(!mPort.isAllocatorPort()) {
                        mPort.bufferSentToNeighbor(pMappedOMXBuffer);

                         if (!mPort.getTunneledComponent()) {
                             mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
                         } else {
                             OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                         }
                    }
                }
                else
                    return OMX_ErrorIncorrectStateOperation;

                if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersToNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }

                if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersFromNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }
                
                return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
            {
                
                ENS_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ENS_CmdFillThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                OMX_BOOL sendBufferToNeighbor;

                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                
                if(sendBufferToNeighbor) {
                    if(!mPort.isSharingPort()) {
                        if(!mPort.isAllocatorPort()) {
                            copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                            mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                            if (!mPort.getTunneledComponent()) {
                                mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
                            } else {
                                OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                            }
                        }
                    }
                    else
                        return OMX_ErrorIncorrectStateOperation;
                }

                if (mWaitBuffersToNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersToNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }

                if (mWaitBuffersFromNeighbor == OMX_TRUE && mPort.isAllBuffersAtOwner()) {
                    mWaitBuffersFromNeighbor = OMX_FALSE;
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                }


                return OMX_ErrorNone;
            }

        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToIdle FREE_BUFFER_SIG error: incorrect state operation");	
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief transient state used when a command has been sent to processing component
//!
//! This method implements the behavior of the port FSM when  a SendCommand(setState)
//! has been sent to the processing component. It waits for the command complete event
//! to transition to the target state
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransient(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateTransient ENTRY_SIG h=0x%x port=%d \n", mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransient ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_SETSTATECOMPLETE_SIG:
            {
                ENS_CmdSetStateCompleteEvt *setStateCompleteEvt = (ENS_CmdSetStateCompleteEvt *) evt;
                OMX_STATETYPE newState = setStateCompleteEvt->newState();
                if (newState == OMX_StateExecuting) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateExecuting));   
                } else if (newState == OMX_StatePause) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStatePause));
                } else if (newState == OMX_StateIdle) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdle));
                } else {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
                }
                return OMX_ErrorNone;
            }

        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransient FREE_BUFFER_SIG error: incorrect state operation");	
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief state handler when the port is in invalid state
//!
//! This method implements the behavior of the port FSM when it is in
//! the invalid state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateInvalid(const ENS_FsmEvent *evt) {
    switch (evt->signal()) {
        case ENTRY_SIG:
        {
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateInvalid ENTRY_SIG");
            return OMX_ErrorNone;
        }

        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateInvalid error");	
            return OMX_ErrorInvalidState;
    }
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be entered only if the port is tunneled
//! and buffer supplier. In this case it is the first transient state and it waits 
//! for all buffers to be returned from tunneled component. When it is the case,
//! the port FSM transitions to the OmxStateTransientToDisableDSP state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToDisableWaitBuffersFromNeighbor(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateTransientToDisableWaitBuffersFromNeighbor ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor ENTRY_SIG");
            DBC_PRECONDITION(mPort.useStandardTunneling() && mPort.isBufferSupplier());
            return OMX_ErrorNone;
        }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
            {
                ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
               
                mPort.bufferReturnedFromNeighbor(emptyThisBufferEvt->getOMXBufferHdrPtr());
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor OMX_EMPTY_THIS_BUFFER_SIG 0x%x",(unsigned int)(emptyThisBufferEvt->getOMXBufferHdrPtr()));
if(!BUFFER_SHARING_ENABLED){
				if(mPort.isAllBuffersReturnedFromNeighbor()){
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                }
}else{
                if(mPort.isSharingPort()){
                    OMX_ERRORTYPE err;
                    err = mPort.forwardInputBuffer(emptyThisBufferEvt->getOMXBufferHdrPtr());
                    if(err!=OMX_ErrorNone) return err;

                    mPort.bufferSentToSharingPort(emptyThisBufferEvt->getOMXBufferHdrPtr());
                    
                    if(mPort.isAllBuffersReturnedFromNeighbor()){
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                    }
                }

                if(mPort.isAllocatorPort()) {
                    if(mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()){
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                    }
                }
}
                return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_BUFFER_SIG:
            {
                ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;

                mPort.bufferReturnedFromNeighbor(fillThisBufferEvt->getOMXBufferHdrPtr());
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor OMX_FILL_THIS_BUFFER_SIG 0x%x",(unsigned int)(fillThisBufferEvt->getOMXBufferHdrPtr()));
if(!BUFFER_SHARING_ENABLED){
                if(mPort.isAllBuffersReturnedFromNeighbor()){
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                }
}else{
                if(mPort.isSharingPort()){
                    OMX_ERRORTYPE err;
                    err = mPort.forwardOutputBuffer(fillThisBufferEvt->getOMXBufferHdrPtr());
                    if(err!=OMX_ErrorNone) return err;

                    mPort.bufferSentToSharingPort(fillThisBufferEvt->getOMXBufferHdrPtr());

                    if(mPort.isAllBuffersReturnedFromNeighbor()){
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                    }
                }

                if(mPort.isAllocatorPort()) {
                    if(mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()){
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                    }
                }
}
                return OMX_ErrorNone;
            }

        case OMX_EMPTY_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *emptyBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
if(BUFFER_SHARING_ENABLED){
                if(mPort.isSharingPort()){
                    OMX_ERRORTYPE err;
                    err = mPort.forwardInputBuffer(pOMXBuffer);
                    if(err!=OMX_ErrorNone) return err;

                    mPort.bufferSentToSharingPort(pOMXBuffer);
                }
}              
                return OMX_ErrorNone;
            }

        case OMX_FILL_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *fillBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
if(BUFFER_SHARING_ENABLED){                
                DBC_ASSERT(mPort.isSharingPort()==OMX_FALSE);
}
                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

                return OMX_ErrorNone;
            }

        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitBuffersFromNeighbor FREE_BUFFER_SIG error: incorrect state operation");	
            return OMX_ErrorIncorrectStateOperation;

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ENS_CmdFillThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                OMX_BOOL sendBufferToNeighbor;
                    
                if(mPort.isAllocatorPort()) {
                    DBC_ASSERT(OMX_ErrorNone==mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                    sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);

                    if(sendBufferToNeighbor) {
                        copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                        if(mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()){
                            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                        }
                    }
                }
                else{
                    return OMX_ErrorIncorrectStateOperation;
                }

                return OMX_ErrorNone;
            }
            
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ENS_CmdEmptyThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

                if(mPort.isAllocatorPort()) {
                    DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                    mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                    copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                    if(mPort.isAllBuffersReturnedFromNeighbor() && mPort.isAllBuffersReturnedFromSharingPorts()){
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP));
                    }
                }
                else{
                    return OMX_ErrorIncorrectStateOperation;
                }

                return OMX_ErrorNone;
            }

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be either the second or first transient
//! state when disabling a port, depending on wheter the 
//! OmxStateTransientToDisableWaitBuffersFromNeighbor state is entered or not. 
//! In this state, the port FSM sends the disable port command
//! to the processing component as entry action, and then waits for the disable port
//! complete and all buffers to be returned by the processing component.
//! When this is the case, the port FSM transitions to the 
//! OmxStateTransientToDisableWaitForFreeBuffers state if the port is port is not supplier,
//! or it frees all the buffers if it is supplier
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToDisableDSP(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
            {
                MSG2("OmxStateTransientToDisableDSP ENTRY_SIG h=0x%x port=%d \n", 
                        mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
				OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP ENTRY_SIG");
                if(mEventAppliedOnEachPorts == OMX_FALSE) {
                    return getProcessingComponent().sendCommand(OMX_CommandPortDisable, mPort.getPortIndex());
                } else if(mPort.getENSComponent().allPortsInSameFsmState(
                            static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableDSP)) == OMX_TRUE) {
                    mEventAppliedOnEachPorts = OMX_FALSE;
                    return getProcessingComponent().sendCommand(OMX_CommandPortDisable, OMX_ALL);
                } else {
                    mEventAppliedOnEachPorts = OMX_FALSE;
                }
                return OMX_ErrorNone;
            }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }

        case OMX_DISABLEPORTCOMPLETE_SIG:
            {
                if (mPort.getBufferCountCurrent() == 0) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
                } else if (mPort.useStandardTunneling() && mPort.isBufferSupplier()) {
                    OMX_ERRORTYPE error;
if(!BUFFER_SHARING_ENABLED){
                    error = mPort.standardTunnelingDeInit();
                    if(error != OMX_ErrorNone) {
						OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
						return error;
					}
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
}else{
                    if(mPort.isAllocatorPort()) {
                        error = mPort.standardTunnelingDeInit();
                        if(error != OMX_ErrorNone) {
                            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
                            return error;
                        }
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
                    }

                    if(mPort.isSharingPort()) {
                        error = mPort.bufferSharingDeinit();
                        if(error != OMX_ErrorNone) {
                            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP DISABLEPORTCOMPLETE_SIG error: %d", error);
                            return error;
                        }
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
                    }
}

                } else {
if(!BUFFER_SHARING_ENABLED){
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers));
}else{
                    if(mPort.isAllBuffersReturnedFromSharingPorts()) {
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers));
                    }
}
                }
                return OMX_ErrorNone;
            }

        case OMX_EMPTY_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *emptyBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

                if(!mPort.getTunneledComponent()) {
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
                } else {
if(!BUFFER_SHARING_ENABLED){
                    if(mPort.isBufferSupplier()) return OMX_ErrorNone;
}else{
                    if(mPort.isAllocatorPort()) return OMX_ErrorNone;

                    if(mPort.isSharingPort()) {
                        OMX_ERRORTYPE err;
                        err = mPort.forwardInputBuffer(pOMXBuffer);
                        if(err!=OMX_ErrorNone) return err;
                        mPort.bufferSentToSharingPort(pOMXBuffer);
                        return OMX_ErrorNone;
                    }
                    
}
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }
                return OMX_ErrorNone;
            }

        case OMX_FILL_BUFFER_DONE_SIG:
            {
                ENS_CmdBufferHdrEvt *fillBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();

                mPort.bufferReturnedFromProcessingComp(pOMXBuffer);

                if(!mPort.getTunneledComponent()) {
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
                } else {
if(!BUFFER_SHARING_ENABLED){
                    if(mPort.isBufferSupplier()) return OMX_ErrorNone;
}else{
                    if(mPort.isAllocatorPort()) return OMX_ErrorNone;
                    if(mPort.isSharingPort()) DBC_ASSERT(0);
}
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
                }
                return OMX_ErrorNone;
            }

        case OMX_FREE_BUFFER_SIG:
            {
                ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;

                if(mPort.getTunneledComponent() && mPort.isBufferSupplier()) {
					OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP FREE_BUFFER_SIG error: incorrect state operation");
                    return OMX_ErrorIncorrectStateOperation;
                }

                return mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
            }

        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ENS_CmdEmptyThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                DBC_ASSERT(mPort.isBufferSupplier()==OMX_FALSE);
                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);

                mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                
                if (!mPort.getTunneledComponent()) {
                    mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
                } else {
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                }

                if(mPort.isAllBuffersReturnedFromSharingPorts()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers));
                }


                return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ENS_CmdFillThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                OMX_BOOL sendBufferToNeighbor;

                DBC_ASSERT(mPort.isBufferSupplier()==OMX_FALSE);
                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                
                if(sendBufferToNeighbor) {
                    copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                    mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                    if (!mPort.getTunneledComponent()) {
                        mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
                    } else {
                        OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                    }
                }

                if(mPort.isAllBuffersReturnedFromSharingPorts()) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers));
                }

                return OMX_ErrorNone;
            }


        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableDSP error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief transient state when disabling a port
//!
//! This method implements the behavior of the port FSM when it is transitionning
//! to disabled state. This transient state can be entered only if the port is not supplier. 
//! In this state, the port waits for calls to OMX_FreeBuffer for all buffers.
//! When it is the case, the port FSM transitions to the OmxStateDisabled state.
//! For hybrid component (having both ARm and DSP port), buffers can be received (fill/empty_buffer_done)
//! in this state because we cannot ensure nmf message order. For example buffer can be received
//! NMF-DSP callback thread, and disable port complete by HOST-EE callback thread
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToDisableWaitForFreeBuffers(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;

    switch (evt->signal()) {
        case ENTRY_SIG:
            MSG2("OmxStateTransientToDisableWaitForFreeBuffers ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers  ENTRY_SIG");
            DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
            return OMX_ErrorNone;

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }

        case OMX_FREE_BUFFER_SIG:
            {
                ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;

                error = mPort.freeBuffer(freeBufferEvt->getOMXBufferHdrPtr());
                if(error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers FREE_BUFFER_SIG error: %d", error);
					return error;
				}

                if(mPort.getBufferCountCurrent() == 0) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateDisabled));
                }

                return OMX_ErrorNone;
            }

      case OMX_EMPTY_BUFFER_DONE_SIG:
    	{
        	  ENS_CmdBufferHdrEvt *emptyBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
        	  OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
        	  
        	  mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
        	  
        	  if(!mPort.getTunneledComponent()) {
        	    mPort.bufferSentToNeighbor(pOMXBuffer);
        	    mPort.getENSComponent().emptyBufferDoneCB(pOMXBuffer);
        	  } else {
        	    if(mPort.isBufferSupplier()) return OMX_ErrorNone;
        	    mPort.bufferSentToNeighbor(pOMXBuffer);
        	    OMX_FillThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
        	  }
        	  return OMX_ErrorNone;
    	}
	
      case OMX_FILL_BUFFER_DONE_SIG:
    	{
        	  ENS_CmdBufferHdrEvt *fillBufferDoneEvt = (ENS_CmdBufferHdrEvt *) evt;
        	  OMX_BUFFERHEADERTYPE *pOMXBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
        	  
        	  mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
        	  
        	  if(!mPort.getTunneledComponent()) {
        	    mPort.bufferSentToNeighbor(pOMXBuffer);
        	    mPort.getENSComponent().fillBufferDoneCB(pOMXBuffer);
        	  } else {
        	    if(mPort.isBufferSupplier()) return OMX_ErrorNone;
        	    mPort.bufferSentToNeighbor(pOMXBuffer);
        	    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pOMXBuffer);
        	  }
        	  return OMX_ErrorNone;
    	}
        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        	  OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToDisableWaitForFreeBuffers error: incorrect state operation %d", evt->signal());
        	  return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}


//-----------------------------------------------------------------------------
//! \brief  state when a port is disabled
//!
//! This method implements the behavior of the port FSM when the port is disabled 
//! The mDisabledState member variable stores the state to which the port should
//! transition if the port would be enabled
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateDisabled(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateDisabled ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateDisabled  ENTRY_SIG");
            return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortDisable, mPort.getPortIndex());
        }

        case OMX_SETSTATE_SIG:
            {
                ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
                OMX_STATETYPE targetState = setStateEvt->targetState();

                if (mDisabledState == OMX_StateLoaded && targetState == OMX_StateIdle) {
                    mDisabledState = OMX_StateCheckRscAvailability;
                } else if (targetState == OMX_StateLoaded) {
                    mDisabledState = OMX_StateLoaded;
                } else {
                    mDisabledState = OMX_StateTransient;
                }
                return OMX_ErrorNone;
            }

        case OMX_SETSTATECOMPLETE_SIG:
            {
                ENS_CmdSetStateCompleteEvt *setStateCompleteEvt = (ENS_CmdSetStateCompleteEvt *) evt;
                mDisabledState = setStateCompleteEvt->newState();
                return OMX_ErrorNone;
            }

        case OMX_SETPARAMETER_SIG:
            {
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                return mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
            }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
            {
                // no callback
                ENS_CmdUpdateSettingsEvt *updateSettingsEvt = (ENS_CmdUpdateSettingsEvt *) evt;
                return mPort.setParameter(updateSettingsEvt->getIndex(), updateSettingsEvt->getStructPtr());
            }        

        case OMX_ENABLE_PORT_SIG:
            {
                ENS_CmdPortEvt *portEvt = (ENS_CmdPortEvt *) evt;
                return goToEnable(portEvt->getPortIdx());
            }

        case OMX_COMP_TUNNEL_REQUEST_SIG:
            {
                ENS_CmdTunnelRequestEvt *tunnelReqEvt = (ENS_CmdTunnelRequestEvt *) evt;
                OMX_ERRORTYPE err = mPort.tunnelRequest(tunnelReqEvt->getTunneledCompHdl(), 
                                                tunnelReqEvt->getTunneledPortIdx(), tunnelReqEvt->getTunnelStructPtr());
                if (OMX_ErrorNone != err)
                {
                    OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateDisabled COMP_TUNNEL_REQUEST_SIG error: %d", err);
                    return err;
                }
                // Notify the RM if port is an output port OR standard Tunneled
                if (mPort.getDirection()==OMX_DirOutput) {
                    ENS_HwRm::HWRM_Notify_Tunnel(mPort.getComponent(),
                                                 mPort.getPortIndex(),
                                                 tunnelReqEvt->getTunneledCompHdl(),
                                                 tunnelReqEvt->getTunneledPortIdx());
                }
                else if (mPort.useStandardTunneling()) {
                     ENS_HwRm::HWRM_Notify_Tunnel(tunnelReqEvt->getTunneledCompHdl(),
                                                  tunnelReqEvt->getTunneledPortIdx(),
                                                  mPort.getComponent(),
                                                  mPort.getPortIndex());
                }
                return OMX_ErrorNone;
            }

        case OMX_RESRESERVATIONPROCESSED_SIG:
            {
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            OMX_BOOL rscGranted = ackRscEvt->resGranted();

            // Not enough resources
            if(rscGranted == OMX_FALSE)
                mDisabledState = OMX_StateLoaded;
            else
                mDisabledState = OMX_StateLoadedToIdleDSP;
            return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateDisabled error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief  Transient state when a port is being enabled
//!
//! This method implements the behavior of the port FSM when the port is transitionning 
//! to enabled state. This state is entered only if the port is not-supplier.
//! In this state, the port waits for calls to OMX_AllocateBuffer or OMX_UseBuffer until
//! the port is populated. When it is the case, the port transitions to
//! the OmxStateTransientToEnableDSP state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToEnableHOST(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;

    switch (evt->signal()) {
        case ENTRY_SIG:
        {
            MSG2("OmxStateTransientToEnableHOST ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST ENTRY_SIG");

            if(!BUFFER_SHARING_ENABLED){
                DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isBufferSupplier());
            }else{
                DBC_PRECONDITION(!mPort.useProprietaryCommunication() && !mPort.isAllocatorPort());
            }
            return OMX_ErrorNone;
        }

        case OMX_SETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED) {
                OMX_ERRORTYPE error;
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                // Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                
                error =  mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;

                error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;
                
                return OMX_ErrorNone;
}
else{
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                //Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                return mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
}
            }
        case OMX_USE_BUFFER_SIG:
            {
                ENS_CmdUseBufferEvt *useBufferEvt = (ENS_CmdUseBufferEvt *) evt;
                error = mPort.useBuffer(useBufferEvt->getOMXBufferHdrPtr(), useBufferEvt->getAppPrivate(),
                        useBufferEvt->getSizeBytes(), useBufferEvt->getBufferPtr());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG error: %d", error);
					return error;
				}

                if(mPort.isPopulated()) {
                    if (mPort.getDomain() != OMX_PortDomainOther) {
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP));
                    } else {
                        OMX_ERRORTYPE err;
                        err = getProcessingComponent().sendCommand(OMX_CommandPortEnable, mPort.getPortIndex());
                        if (err != OMX_ErrorNone) {
							OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG Populated port error: %d", err);
							return err;
						}
                        stateTransition(getFsmParentState(mDisabledState));
                        mDisabledState = OMX_StateInvalid;
                        return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex()); 
                    }
                }
                
                return OMX_ErrorNone;
            }

        case OMX_ALLOCATE_BUFFER_SIG:
            {
                ENS_CmdAllocBufferEvt *allocBufferEvt = (ENS_CmdAllocBufferEvt *) evt;
                error = mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(), 
                        allocBufferEvt->getAppPrivate(), allocBufferEvt->getSizeBytes());
                if (error != OMX_ErrorNone) {
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST ALLOCATE_BUFFER_SIG error: %d", error);
					return error;
				}

                if(mPort.isPopulated()) {
                    if (mPort.getDomain() != OMX_PortDomainOther) {
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP));
                    } else {
                        OMX_ERRORTYPE err;
                        err = getProcessingComponent().sendCommand(OMX_CommandPortEnable, mPort.getPortIndex());
                        if (err != OMX_ErrorNone) {
							OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST ALLOCATE_BUFFER_SIG Populated port error: %d", err);
							return err;
						}
                        stateTransition(getFsmParentState(mDisabledState));
                        mDisabledState = OMX_StateInvalid;
                        return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex()); 
                    }
                }

                return OMX_ErrorNone;
            }

        case OMX_USE_SHARED_BUFFER_SIG:
        {
            OMX_ERRORTYPE error;
            DBC_ASSERT(mPort.getPortRole()==ENS_SHARING_PORT);
            // Sharing ports receives buffer from non supplier/allocator
            // Forward this buffer to tunneled port
            ENS_CmdUseSharedBufferEvt *useSharedBufferevt = (ENS_CmdUseSharedBufferEvt *) evt;
            error =  mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),useSharedBufferevt->getSizeBytes(),
                                  useSharedBufferevt->getBufferPtr(),useSharedBufferevt->getSharedChunk());

            if (error != OMX_ErrorNone) return error;

            if(mPort.isPopulated()) {
                if (mPort.getDomain() != OMX_PortDomainOther) {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP));
                } else {
                    OMX_ERRORTYPE err;
                    err = getProcessingComponent().sendCommand(OMX_CommandPortEnable, mPort.getPortIndex());
                    if (err != OMX_ErrorNone) {
                        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST USE_BUFFER_SIG Populated port error: %d", err);
                        return err;
                    }
                    stateTransition(getFsmParentState(mDisabledState));
                    mDisabledState = OMX_StateInvalid;
                    return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex()); 
                }
            }
            
            return OMX_ErrorNone;

        }

        case OMX_GETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED){
                OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
                OMX_ERRORTYPE error;
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
                error =  mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error;

                error = mPort.calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;

                OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                        static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

                // Calculate self requirements
                maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
                maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

                portdef->nBufferSize = maxBufferSize;
                portdef->nBufferCountActual = maxBufferCountActual;
                
                return OMX_ErrorNone;
}
else{
				return defaultBehavior(evt);
}
            }
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableHOST error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

//-----------------------------------------------------------------------------
//! \brief  Transient state when a port is being enabled
//!
//! This method implements the behavior of the port FSM when the port is transitionning 
//! to enabled state. This state is entered once the port is populated.
//! In this state, the port FSM sends the enable port command to the processing component
//! as entry action, then waits for the command complete event. 
//! It then transition to the state stored in the mDisabledState member variable
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTransientToEnableDSP(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {

        case ENTRY_SIG:
            {
                MSG2("OmxStateTransientToEnableDSP ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
				OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP ENTRY_SIG");
                if(mEventAppliedOnEachPorts == OMX_FALSE) {
                    return getProcessingComponent().sendCommand(OMX_CommandPortEnable, mPort.getPortIndex());
                } else if(mPort.getENSComponent().allPortsInSameFsmState(
                            static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTransientToEnableDSP)) == OMX_TRUE) {
                    mEventAppliedOnEachPorts = OMX_FALSE;
                    return getProcessingComponent().sendCommand(OMX_CommandPortEnable, OMX_ALL);
                } else {
                    mEventAppliedOnEachPorts = OMX_FALSE;
                }
                return OMX_ErrorNone;
            }

        case EXIT_SIG:
            {
                return eventHandlerCB(OMX_EventCmdComplete, OMX_CommandPortEnable, mPort.getPortIndex());
            }
        case OMX_ENABLEPORTCOMPLETE_SIG:
            {
                stateTransition(getFsmParentState(mDisabledState));
                mDisabledState = OMX_StateInvalid;
                return OMX_ErrorNone;
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
            {
                ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
                OMX_ERRORTYPE err = OMX_ErrorNone;

                if(!mPort.getTunneledComponent() || mPort.isBufferSupplier()) {
                    return OMX_ErrorIncorrectStateOperation;
                }

                mPort.bufferReturnedFromNeighbor(pOMXBuffer);
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP OMX_EMPTY_THIS_BUFFER_SIG 0x%x",(unsigned int)pOMXBuffer);
                mPort.bufferSentToProcessingComponent(pOMXBuffer);

                err = getProcessingComponent().emptyThisBuffer(pOMXBuffer);
                if (err != OMX_ErrorNone) {
                    mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                }

				OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP EMPTY_THIS_BUFFER error: %d", err);
			    return err;
            }
        
        case OMX_FILL_THIS_BUFFER_SIG:
            {
                ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
                OMX_ERRORTYPE err = OMX_ErrorNone;

                if(!mPort.getTunneledComponent() || mPort.isBufferSupplier()) { 
                    return OMX_ErrorIncorrectStateOperation;
                }

                mPort.bufferReturnedFromNeighbor(pOMXBuffer);
				OstTraceFiltInst1(TRACE_DEBUG, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP OMX_FILL_THIS_BUFFER_SIG 0x%x",(unsigned int)pOMXBuffer);
if(!BUFFER_SHARING_ENABLED){
                mPort.bufferSentToProcessingComponent(pOMXBuffer);
        
                err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
                if (err != OMX_ErrorNone) {
                    mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    mPort.bufferSentToNeighbor(pOMXBuffer);
                }

				OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP FILL_THIS_BUFFER error: %d", err);
}else{
                if(mPort.getNumberInterConnectedPorts()) {
                    err = mPort.forwardOutputBuffer(pOMXBuffer);
                    if(err!=OMX_ErrorNone)return err;

                    mPort.bufferSentToSharingPort(pOMXBuffer);
                }
                else{
                    mPort.bufferSentToProcessingComponent(pOMXBuffer);

                    err = getProcessingComponent().fillThisBuffer(pOMXBuffer);
                    if (err != OMX_ErrorNone) {
                        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: defaultBehavior FILL_THIS_BUFFER_SIG error: %d", err);			
                        mPort.bufferReturnedFromProcessingComp(pOMXBuffer);
                    }
                }
}

                return err;
            }

        case OMX_EMPTY_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdEmptyThisSharedBufferEvt *emptyThisSharedBufferEvt = (ENS_CmdEmptyThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = emptyThisSharedBufferEvt->getOMXBufferHdrPtr();
                OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;

                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);

                mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                mPort.bufferSentToNeighbor(pMappedOMXBuffer);

                if (!mPort.getTunneledComponent()) {
                    mPort.getENSComponent().fillBufferDoneCB(pMappedOMXBuffer);
                } else {
                    OMX_EmptyThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                }

                return OMX_ErrorNone;
            }

        case OMX_FILL_THIS_SHARED_BUFFER_SIG:
            {
                ENS_CmdFillThisSharedBufferEvt *fillThisSharedBufferEvt = (ENS_CmdFillThisSharedBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pOMXBuffer = fillThisSharedBufferEvt->getOMXBufferHdrPtr();
				OMX_BUFFERHEADERTYPE *pMappedOMXBuffer;
                OMX_BOOL sendBufferToNeighbor;

                DBC_ASSERT(OMX_ErrorNone == mPort.mapSharedBufferHeader(pOMXBuffer,&pMappedOMXBuffer));
                sendBufferToNeighbor = mPort.bufferReturnedFromSharingPort(pMappedOMXBuffer);
                
                if(sendBufferToNeighbor) {
                    mPort.bufferSentToNeighbor(pMappedOMXBuffer);
                    copyOMXBufferHeaderFields(pOMXBuffer,pMappedOMXBuffer);
                    if (!mPort.getTunneledComponent()) {
                        mPort.getENSComponent().emptyBufferDoneCB(pMappedOMXBuffer);
                    } else {
                        OMX_FillThisBuffer(mPort.getTunneledComponent(), pMappedOMXBuffer);
                    }
                }

                return OMX_ErrorNone;
            }
       
        case OMX_USE_BUFFER_SIG:
            {
                ENS_CmdUseBufferEvt *useBufferEvt = (ENS_CmdUseBufferEvt *) evt;
                return mPort.useBuffer(useBufferEvt->getOMXBufferHdrPtr(), useBufferEvt->getAppPrivate(),
                        useBufferEvt->getSizeBytes(), useBufferEvt->getBufferPtr());
            }

        case OMX_ALLOCATE_BUFFER_SIG:
            {
                ENS_CmdAllocBufferEvt *allocBufferEvt = (ENS_CmdAllocBufferEvt *) evt;
                return mPort.allocateBuffer(allocBufferEvt->getOMXBufferHdrPtr(), 
                        allocBufferEvt->getAppPrivate(), allocBufferEvt->getSizeBytes());
            }

        case OMX_USE_SHARED_BUFFER_SIG:
        {
            DBC_ASSERT(mPort.getPortRole()==ENS_SHARING_PORT);
            // Sharing ports receives buffer from non supplier/allocator
            // Forward this buffer to tunneled port
            ENS_CmdUseSharedBufferEvt *useSharedBufferevt = (ENS_CmdUseSharedBufferEvt *) evt;
            return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),useSharedBufferevt->getSizeBytes(),
                                  useSharedBufferevt->getBufferPtr(),useSharedBufferevt->getSharedChunk());
            

        }

        case OMX_GETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED){
                OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
                OMX_ERRORTYPE error;
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
                error =  mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error;

                error = mPort.calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;

                OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                        static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

                // Calculate self requirements
                maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
                maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

                portdef->nBufferSize = maxBufferSize;
                portdef->nBufferCountActual = maxBufferCountActual;
                
                return OMX_ErrorNone;
}
else{
				return defaultBehavior(evt);
}

            }
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTransientToEnableDSP error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
            {
                return defaultBehavior(evt);
            }
    }
}

OMX_ERRORTYPE ENS_Port_Fsm::OmxStateTestForResources(const ENS_FsmEvent *evt)
{
	switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateTestForResources ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateTestForResources  ENTRY_SIG");
            return OMX_ErrorNone;
        }
        
        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
            OMX_STATETYPE targetState = setStateEvt->targetState();

            if(targetState == OMX_StateIdle) {
				return goLoadedToIdle();
            } else if(targetState == OMX_StateLoaded) {
                stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
				return OMX_ErrorNone;
            } else if(targetState == OMX_StateTestForResources) {
				return OMX_ErrorSameState;
            } else {
				OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTestForResources SETSTATE_SIG error: incorrect state transition");
                return OMX_ErrorIncorrectStateTransition;
            }
        }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_FLUSH_PORT_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateTestForResources error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

OMX_ERRORTYPE ENS_Port_Fsm::OmxStateWaitForResources(const ENS_FsmEvent *evt)
{
	switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateWaitForResources ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateWaitForResources  ENTRY_SIG");
            return OMX_ErrorNone;
        }
        
        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
            OMX_STATETYPE targetState = setStateEvt->targetState();

            if(targetState == OMX_StateIdle) {
				return goLoadedToIdle();
            } else if(targetState == OMX_StateLoaded) {
                stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
				return OMX_ErrorNone;
            } else if(targetState == OMX_StateWaitForResources) {
				return OMX_ErrorSameState;
            } else {
				OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateWaitForResources SETSTATE_SIG error: incorrect state transition");
                return OMX_ErrorIncorrectStateTransition;
            }
        }

        case OMX_UPDATE_PORT_SETTINGS_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_FLUSH_PORT_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateWaitForResources error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}


OMX_ERRORTYPE ENS_Port_Fsm::OmxStateLoadedToTestForResources(const ENS_FsmEvent *evt)
{
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateLoadedToTestForResources ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToTestForResources  ENTRY_SIG");
            return OMX_ErrorNone;
        }
        
        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            OMX_BOOL rscGranted = ackRscEvt->resGranted();
            
            if(rscGranted == OMX_FALSE) {
                stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
            } else  {
                stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateTestForResources));
            }
            return OMX_ErrorNone;
        }

        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_FLUSH_PORT_SIG:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateLoadedToTestForResources error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }    
    }
}


OMX_ERRORTYPE ENS_Port_Fsm::OmxStateCheckRscAvailability(const ENS_FsmEvent *evt)
{
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG2("OmxStateCheckRscAvailability ENTRY_SIG h=0x%x port=%d \n", 
                    mPort.getENSComponent().getOMXHandle(), mPort.getPortIndex());
			OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Port_Fsm: OmxStateCheckRscAvailability ENTRY_SIG");
            return OMX_ErrorNone;
        }
        
        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            OMX_BOOL rscGranted = ackRscEvt->resGranted();
            
            // Not enough resources
            if(rscGranted == OMX_FALSE) {
                // Case non-tunneled or std-tunneled not supplier
                if(!mPort.getTunneledComponent() ||
                   (mPort.useStandardTunneling()&& !mPort.isBufferSupplier()) ) {
                    if (mPort.getBufferCountCurrent() == 0) {
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
                    } else {
                        stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateIdleToLoaded));
                    }
                } else {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));
                }
                return OMX_ErrorNone;
            }
            // Resources Granted
            // Allocate buffers in case std-tunneling supplier
if(!BUFFER_SHARING_ENABLED){
            if (mPort.getTunneledComponent() && mPort.useStandardTunneling()
                && mPort.isBufferSupplier()) {
                OMX_ERRORTYPE error = mPort.standardTunnelingInit();
                if(error != OMX_ErrorNone)  {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", error);
                    return OMX_ErrorNone;   // do not return the error, otherwise the transition is not taken into account
                }
            }
}else{
            if (mPort.getTunneledComponent() && mPort.useStandardTunneling()
                && mPort.isBufferSupplier() && mPort.isAllocatorPort()) {
                OMX_ERRORTYPE error = mPort.standardTunnelingInit();
                if(error != OMX_ErrorNone)  {
                    stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateInvalid));
					OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", error);
                    return OMX_ErrorNone;   // do not return the error, otherwise the transition is not taken into account
                }
            }       
}
            stateTransition(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoadedToIdleDSP));
            return OMX_ErrorNone;
        }
        case OMX_USE_SHARED_BUFFER_SIG:
        {
            DBC_ASSERT(mPort.getPortRole()==ENS_SHARING_PORT);
            // Sharing ports receives buffer from non supplier/allocator
            // Forward this buffer to tunneled port
            ENS_CmdUseSharedBufferEvt *useSharedBufferevt = (ENS_CmdUseSharedBufferEvt *) evt;
            return mPort.useSharedBuffer(useSharedBufferevt->getAppPrivate(),useSharedBufferevt->getSizeBytes(),
                                  useSharedBufferevt->getBufferPtr(),useSharedBufferevt->getSharedChunk());
            

        }

        case OMX_SETPARAMETER_SIG:
        {
if(BUFFER_SHARING_ENABLED){
			if(mPort.getPortRole()==ENS_SHARING_PORT) {
                // Its a sharing port
                OMX_ERRORTYPE error;
                ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
                // Only OMX_IndexParamPortDefinition index is supported in this state
                if(setParamEvt->getIndex() != OMX_IndexParamPortDefinition) return OMX_ErrorIncorrectStateOperation;
                error =  mPort.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;

                error = mPort.sendBufferReqs(OMX_IndexParamPortDefinition,setParamEvt->getStructPtr());
                if(error!=OMX_ErrorNone) return error;
                
                return OMX_ErrorNone;
            }
            else
                return OMX_ErrorIncorrectStateOperation;
}else{
                return OMX_ErrorIncorrectStateOperation;
}
        }

        case OMX_GETPARAMETER_SIG:
            {
if(BUFFER_SHARING_ENABLED){
                OMX_U32 maxBufferSize = 0, maxBufferCountActual = 0;
                OMX_ERRORTYPE error;
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                OMX_PTR pComponentParameterStructure = getParamEvt->getStructPtr();
                error =  mPort.getParameter(getParamEvt->getIndex(), pComponentParameterStructure);
                if(error!=OMX_ErrorNone) return error;

                error = mPort.calculateBufferReqs(&maxBufferSize,&maxBufferCountActual);
                if(error!=OMX_ErrorNone) return error;

                OMX_PARAM_PORTDEFINITIONTYPE *portdef = 
                        static_cast<OMX_PARAM_PORTDEFINITIONTYPE *> (pComponentParameterStructure);

                // Calculate self requirements
                maxBufferSize = MAX(portdef->nBufferSize, maxBufferSize);
                maxBufferCountActual = MAX(portdef->nBufferCountActual,maxBufferCountActual);

                portdef->nBufferSize = maxBufferSize;
                portdef->nBufferCountActual = maxBufferCountActual;
                
                return OMX_ErrorNone;
}
else{
				return defaultBehavior(evt);
}
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_FLUSH_PORT_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Port_Fsm: OmxStateCheckRscAvailability error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;

        default:
        {
            return defaultBehavior(evt);
        }
    }
}


inline void ENS_Port_Fsm::copyOMXBufferHeaderFields(OMX_BUFFERHEADERTYPE *pSrcBuffer,OMX_BUFFERHEADERTYPE *pDstBuffer){
    pDstBuffer->nFilledLen = pSrcBuffer->nFilledLen;
    pDstBuffer->nOffset = pSrcBuffer->nOffset;
    pDstBuffer->nFlags = pSrcBuffer->nFlags;
    pDstBuffer->nTickCount = pSrcBuffer->nTickCount;
    pDstBuffer->nTimeStamp = pSrcBuffer->nTimeStamp;
    pDstBuffer->hMarkTargetComponent = pSrcBuffer->hMarkTargetComponent;
    pDstBuffer->pMarkData = pSrcBuffer->pMarkData;
}
