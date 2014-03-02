#ifndef ENS_COMPONENT_FSM_INL
#define ENS_COMPONENT_FSM_INL

inline OMX_ERRORTYPE 
ENS_Component_Fsm::eventHandlerCB(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) const
{
    return mEnsComponent.eventHandlerCB(ev, data1, data2, 0);
}

inline OMX_BOOL 
ENS_Component_Fsm::isPortEnabled(const OMX_U32 portidx) const
{
    return mEnsComponent.getPort(portidx)->isEnabled();
}

inline ENS_Port_Fsm * 
ENS_Component_Fsm::getPortFsm(const OMX_U32 portIdx) const
{
    if (mEnsComponent.getPortCount()<=portIdx || mEnsComponent.getPort(portIdx)==0) {
        return 0;
    }
    return mEnsComponent.getPort(portIdx)->getPortFsm();
}

inline OMX_STATETYPE 
ENS_Component_Fsm::portGetState(const OMX_U32 portIdx) const
{
    if (mEnsComponent.getPortCount()<=portIdx || mEnsComponent.getPort(portIdx)==0) {
        return OMX_StateInvalid;
    }
    ENS_Port_Fsm *portFsm = (mEnsComponent.getPort(portIdx))->getPortFsm();
    return portFsm->getState();
}

#endif // ENS_COMPONENT_FSM_INL
