#ifndef _ENS_PORT_INL_
#define _ENS_PORT_INL_

inline OMX_BOOL 
ENS_Port::isPopulated(void) const
{
    return (OMX_TRUE == mParamPortDefinition.bPopulated ? OMX_TRUE : OMX_FALSE);
}

inline OMX_DIRTYPE
ENS_Port::getDirection(void) const
{
    return mParamPortDefinition.eDir;
}

inline OMX_VIDEO_PORTDEFINITIONTYPE
*ENS_Port::getVideoPortDefinition(void) {
    return &mParamPortDefinition.format.video;
}

inline OMX_IMAGE_PORTDEFINITIONTYPE 
*ENS_Port::getImagePortDefinition(void) {
    return &mParamPortDefinition.format.image;
}

inline OMX_U32
ENS_Port::getPortIndex(void) const
{
    return mParamPortDefinition.nPortIndex;
}


inline OMX_U32 
ENS_Port::getBufferCountActual(void) {
	return mParamPortDefinition.nBufferCountActual;
}

inline OMX_U32
ENS_Port::getBufferCountCurrent(void) {
	return mBufferCountCurrent;
}

inline ENS_BUFFERHEADER_TABLETYPE *
ENS_Port::getEnsBufferHdrTablePtr(void){
	return mBufHdrTablePtr;
}

inline void 
ENS_Port::setSupplierPreference(const OMX_BUFFERSUPPLIERTYPE eBufferSupplier)
{
    mSupplierPreference = eBufferSupplier;
}

inline OMX_BUFFERSUPPLIERTYPE 
ENS_Port::getSupplierPreference(void) const
{
    return mSupplierPreference; 
}

inline void 
ENS_Port::setBufferSupplier(const OMX_BUFFERSUPPLIERTYPE eBufferSupplier)
{
    mBufferSupplier = eBufferSupplier;
}

inline OMX_BUFFERSUPPLIERTYPE 
ENS_Port::getBufferSupplier(void) const
{
    return mBufferSupplier; 
}

inline void 
ENS_Port::setTunnelFlags(const OMX_U32 nTunnelFlags)
{
    mTunnelFlags = nTunnelFlags;
}

inline OMX_U32 
ENS_Port::getTunnelFlags(void) const
{
	return mTunnelFlags;
}

inline void 
ENS_Port::setTunneled(
        OMX_HANDLETYPE hTunneledComponent,  
        OMX_U32 nTunneledPort,
        OMX_U32 nTunnelFlags) 
{
    mTunneledComponent = hTunneledComponent;
    mTunneledPort = nTunneledPort;
    mTunnelFlags = nTunnelFlags;
}

inline OMX_BOOL
ENS_Port::isEnabled(void) const
{
    return mParamPortDefinition.bEnabled;
}

inline void 
ENS_Port::setEnabledFlag(const OMX_BOOL enabled)
{
    mParamPortDefinition.bEnabled = enabled;
}

inline OMX_HANDLETYPE
ENS_Port::getTunneledComponent(void) const {
    return mTunneledComponent;
}

inline OMX_U32
ENS_Port::getTunneledPort(void) const {
    return mTunneledPort;
}

inline OMX_PORTDOMAINTYPE ENS_Port::getDomain() const {
    return mParamPortDefinition.eDomain;
}

inline OMX_BOOL
ENS_Port::useProprietaryCommunication(void) const
{
    if(mTunneledComponent && !mUseStandardTunneling) {
        return OMX_TRUE;
    }
    
    return OMX_FALSE;
}

inline OMX_BOOL
ENS_Port::useStandardTunneling(void) const
{
    if(mTunneledComponent && mUseStandardTunneling) {
        return OMX_TRUE;
    }

    return OMX_FALSE;
}

inline OMX_BOOL
ENS_Port::isBufferSupplier(void) const
{
    if ((mBufferSupplier == OMX_BufferSupplyOutput
            && mParamPortDefinition.eDir == OMX_DirOutput) 
        ||
        (mBufferSupplier == OMX_BufferSupplyInput
            && mParamPortDefinition.eDir == OMX_DirInput)) {
        return OMX_TRUE;
    } 

    return OMX_FALSE;
}

inline const ENS_Component &
ENS_Port::getENSComponent() const {
    return mEnsComponent;
}
    
inline ENS_Component &
ENS_Port::getENSComponent() {
    return mEnsComponent;
}
    
inline OMX_HANDLETYPE
ENS_Port::getComponent(void) const
{
    OMX_HANDLETYPE hComp = mEnsComponent.getOMXHandle();
    DBC_ASSERT(hComp != 0);
    return hComp;
}





inline ENS_Port_Fsm *
ENS_Port::getPortFsm(void) const
{
    return mPortFsm;
}


inline ENS_Port * 
ENS_Port::getENSTunneledPort(void) const
{
    ENS_Component * tunneledComp = 
        (ENS_Component *)EnsWrapper_getEnsComponent(getTunneledComponent());
    return tunneledComp->getPort(getTunneledPort());
}

inline void
ENS_Port::forceStandardTunneling() {
    mUseStandardTunneling = OMX_TRUE;
}

inline bool
ENS_Port::isMpc(void) const
{
  return mIsMpc;
}

inline void
ENS_Port::setMpc(const bool mpc)
{
  mIsMpc = mpc;
}

inline OMX_PARAM_PORTDEFINITIONTYPE 
ENS_Port::getValueParamPortDefinition(void)
{
  return mParamPortDefinition; 
}


#endif // _ENS_PORT_INL_
