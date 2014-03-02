/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_ENS_PORT_H_
#define _IMG_ENS_PORT_H_

class ImgEns_Component;
class ImgEns_Port_Fsm;
class ImgEns_CommonPortData;
class MMHwBuffer;
class ImgEns_Port;
class Img_ProcessingComponent;

//Specify buffer location
enum ImgEns_PORT_BUFFER_LOCATIONTYPE
{
	ImgEns_PORT_BUFFER_ALLOCATED        = 1,
	ImgEns_PORT_BUFFER_ATCOMPONENT      = 2,
	ImgEns_PORT_BUFFER_ATPROCESSINGCOMP = 4,
	ImgEns_PORT_BUFFER_ATNEIGHBOR       = 8,
	ImgEns_PORT_BUFFER_ATSHARINGPORT    = 16 // To keep track of buffer give & take among internally connected ports.
};

//Specify Role of Port
enum ImgEns_PORT_BUFFER_SUPPLY_ROLE
{    /// @brief Port is a supplier only. It reuses buffers provided by other port
	ImgEns_SHARING_PORT,      /// on same component. It is "NOT" an Allocator.
	ImgEns_NON_SUPPLIER_PORT, /// @brief Port is acting as a Non supplier. Its gets buffers from its tunneled port
	ImgEns_ALLOCATOR_PORT,    /// @brief Port is an allocator of its own buffers & also supplies buffers to tunneled and sharingport.
	ImgEns_NON_TUNNELED_PORT, /// @brief Port is non-tunneled.
};

/// @brief Buffer Header table that is used to store Buffer
/// header along with buffer current location
/// @allmedia
typedef struct ImgEns_BUFFERHEADER_TABLETYPE
{
	OMX_BUFFERHEADERTYPE * mBufHdrPtr;      /// @brief Buffer header pointer
	OMX_U32                mBufHdrStatus;   /// @brief Buffer flags to store current status of buffer
	OMX_U32                mAtSharingPorts; /// @brief Count to keep record of a shared buffer sent to multiple shairng ports.
} ImgEns_BUFFERHEADER_TABLETYPE;

/// Ports of same component which are internally connected are referred to InterConnectecd ports here
/// and are sharing buffers with each other.
typedef struct ImgEns_INTER_CONNECTED_PORT_INFO
{
	ImgEns_Port ** mBufSharingPorts;
	OMX_U32        numPorts;
} ImgEns_INTER_CONNECTED_PORT_INFO;

/// @defgroup ens_port_class ENS Ports
/// Abstract base class for ports of ENS components
/// @ingroup ens_component_class
/// @{

class IMGENS_API ImgEns_Port: public Img_TraceObject
{
public:
	//----------------------------------------------------------------
	//      Constructors and Destructors
	//----------------------------------------------------------------

	ImgEns_Port(const ImgEns_CommonPortData& commonPortData, ImgEns_Component &enscomp);
	virtual ~ImgEns_Port(void) = 0;

	OMX_ERRORTYPE Dispatch(const ImgEns_FsmEvent *pEvent);

	//----------------------------------------------------------------
	//      Getters and Setters
	//----------------------------------------------------------------

	inline ImgEns_Port_Fsm * getPortFsm(void) const;
	inline OMX_BOOL isPopulated(void)         const;
	inline OMX_DIRTYPE getDirection(void)     const;

	inline OMX_VIDEO_PORTDEFINITIONTYPE *getVideoPortDefinition(void);
	inline OMX_IMAGE_PORTDEFINITIONTYPE *getImagePortDefinition(void);

	inline OMX_U32 getPortIndex(void) const;

	/// Set/Get the bit flags that will be used for tunneling
	inline void setTunnelFlags(const OMX_U32 nTunnelFlags);
	inline OMX_U32 getTunnelFlags(void) const;

	inline OMX_BUFFERSUPPLIERTYPE getSupplierPreference(void) const;
	inline void setSupplierPreference(const OMX_BUFFERSUPPLIERTYPE eBufferSupplier);
	inline OMX_BOOL isBufferSupplier(void) const;
	inline OMX_BUFFERSUPPLIERTYPE getBufferSupplier(void) const;
	inline void setBufferSupplier(const OMX_BUFFERSUPPLIERTYPE eBufferSupplier);

	inline OMX_BOOL isEnabled(void) const;
	inline void setEnabledFlag(const OMX_BOOL enabled);

	inline void setTunneled(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort, OMX_U32 nTunnelFlags);
	inline OMX_HANDLETYPE getTunneledComponent(void) const;
	inline OMX_U32 getTunneledPort(void) const;

	inline OMX_PORTDOMAINTYPE getDomain() const;

	inline OMX_U32 getBufferCountActual(void);

	inline OMX_U32 getBufferCountCurrent(void);

	/// Return OMX_TRUE if standard Tunneling is used for this port
	inline OMX_BOOL useStandardTunneling(void) const;

	/// Return OMX_TRUE if proprietary communication is used for this port
	inline OMX_BOOL useProprietaryCommunication(void) const;

	// Returns OMX component handle.
	inline       ImgEns_Component& getENSComponent(void);
	inline const ImgEns_Component& getENSComponent(void) const;

	// Returns OMX component handle.
	inline OMX_HANDLETYPE getComponent(void) const;

	// Get pointer to the ENS Tunneled port
	//inline ImgEns_Port * getENSTunneledPort(void) const;

	// to update port settings from within the component
	// for eg when stream was parsed or to implement slaving behavior
	OMX_ERRORTYPE updateSettings(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

	// in case proprietary communication is supported, force
	// standard tunneling instead. For testing purposes.
	inline void forceStandardTunneling();

	// Only used by NmfHostMpc_ProcessingComponent derived classes
	inline bool isMpc(void) const;
	inline void setMpc(const bool mpc);

	//----------------------------------------------------------------
	//      Methods called from port FSM
	//----------------------------------------------------------------
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,
			OMX_PTR pComponentParameterStructure) const;


	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,
			OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE useBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer);


	OMX_ERRORTYPE allocateBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);


	OMX_ERRORTYPE freeBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE tunnelRequest(OMX_HANDLETYPE hTunneledComponent,
			OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup);

	//Update Buffer location to MPC or Neighbor
	OMX_BOOL isAllBuffersAtOwner();
	OMX_BOOL isAllBuffersReturnedFromNeighbor(void);
	OMX_BOOL isAllBuffersReturnedFromSharingPorts();
	void bufferSentToProcessingComponent(OMX_BUFFERHEADERTYPE* pBuffer);
	void bufferReturnedFromProcessingComp(OMX_BUFFERHEADERTYPE* pBuffer);
	void bufferSentToNeighbor(OMX_BUFFERHEADERTYPE* pBuffer);
	void bufferReturnedFromNeighbor(OMX_BUFFERHEADERTYPE* pBuffer);

	/// These methods are only called when port is buffer supplier
	/// in order to init/reset standard tunneling procedure.

	OMX_ERRORTYPE standardTunnelingInit(void);
	OMX_ERRORTYPE standardTunnelingDeInit(void);

	/// These method is called when port is transitioning
	/// from idle to executing in order to initialize standard tunneling
	/// data transfert.

	OMX_ERRORTYPE standardTunnelingTransfertInit(void);

	inline MMHwBuffer *getSharedChunk() const { return mSharedChunk; }
	void setSharedChunk(MMHwBuffer *sharedChunk);

	/// @brief Check whether or not an event handler must be sent on a change of
	/// OMX_PARAM_PORTDEFINITIONTYPE value
	/// @param[in] pOld old value. New one can be taken from the port itself, in mParamPortDefinition
	/// @return OMX_TRUE if the event handler must be raised. OMX_FALSE otherwise
	virtual OMX_BOOL mustSendEventHandler(const OMX_PARAM_PORTDEFINITIONTYPE *pOld);

	inline OMX_PARAM_PORTDEFINITIONTYPE getValueParamPortDefinition(void) const { return mParamPortDefinition; }

protected:
	/// OpenMAX IL port definition structure
	OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;

	//----------------------------------------------------------------
	/// Following methods must be implemented by derived classes.
	//----------------------------------------------------------------

	/// When the IL Client calls SetParameter with
	/// OMX_IndexParamPortDefinition index, this method is called
	/// to set the "format" field in the OMX_PARAM_PORTDEFINITIONTYPE.
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef) = 0;

	virtual OMX_ERRORTYPE proprietaryCommunicationRequest(OMX_HANDLETYPE /*hTunneledComponent*/, OMX_U32 /*nTunneledPort*/)
	{
		return OMX_ErrorNotImplemented;
	}

	virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const = 0;

	//Get index of Buffer in buffer header table
	OMX_ERRORTYPE getBufferHdrTableIndex(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32& Index);
	inline ImgEns_BUFFERHEADER_TABLETYPE * getEnsBufferHdrTablePtr(void);

protected:
	ImgEns_Port_Fsm  * mPortFsm;
	ImgEns_Component & mEnsComponent;

	OMX_HANDLETYPE         mTunneledComponent;
	OMX_U32                mTunneledPort;
	OMX_U32                mTunnelFlags;

	OMX_BUFFERSUPPLIERTYPE mBufferSupplier;
	OMX_BUFFERSUPPLIERTYPE mSupplierPreference;
	OMX_BOOL               mUseStandardTunneling;
	bool                   mIsBufferAllocator;
	bool                   mIsMpc;

	ImgEns_BUFFERHEADER_TABLETYPE * mBufHdrTablePtr;
	OMX_U32 mBufferCountCurrent;

	MMHwBuffer *mSharedChunk;

	//----------------------------------------------------------------
	// Private methods
	//----------------------------------------------------------------

	OMX_BUFFERHEADERTYPE * allocateBufferHeader(OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	void freeBufferHeader(OMX_BUFFERHEADERTYPE *pBufferHdr);

	Img_ProcessingComponent & getProcessingComponent();
	const Img_ProcessingComponent & getProcessingComponent() const;

	//-----------------------------------------------------------//
	//		Buffer Sharing related methods & data members
	//-----------------------------------------------------------//
protected:
	ImgEns_INTER_CONNECTED_PORT_INFO mBufSharingPortList;
	OMX_BOOL mIsAllocator;

	OMX_ERRORTYPE getSharingReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const;
	OMX_ERRORTYPE setSharingReqs(OMX_U32 bufferCountActual);

	OMX_ERRORTYPE forwardBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_PTR pAppPrivate, OMX_U32 bufferSize, OMX_U8* pBuffer, void *bufferAllocInfo, void *portPrivateInfo);

public:
	OMX_ERRORTYPE calculateBufferReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const;
	OMX_ERRORTYPE sendBufferReqs(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

	void bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_BOOL bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE bufferSharingDeinit();

	OMX_ERRORTYPE forwardInputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr);
	OMX_ERRORTYPE forwardOutputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr);

	OMX_ERRORTYPE mapSharedBufferHeader(OMX_BUFFERHEADERTYPE* pBuffer, OMX_BUFFERHEADERTYPE** ppLocalBuffer);
	OMX_U32 getNumberInterConnectedPorts() const { return mBufSharingPortList.numPorts; }

	OMX_ERRORTYPE useSharedBuffer(OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer, void *pSharedChunk);

	OMX_BOOL isSharingPort()          const ;
	OMX_BOOL isBufferSharingPort()    const;
	OMX_BOOL isBufferSharingEnabled() const ;
	void setBufferSharingPortList(ImgEns_Port **portList, OMX_U32 numPorts);
	ImgEns_PORT_BUFFER_SUPPLY_ROLE getPortRole() const;
	void setAllocatorRole(OMX_BOOL value);

	OMX_BOOL isAllocatorPort() const;

	//Note: This flag will be removed in future
	bool m_bBufferSharingEnabled;
	void bufferSharingEnabled(bool enable=true);
};

inline OMX_BOOL ImgEns_Port::isPopulated(void) const
{
	return (OMX_TRUE == mParamPortDefinition.bPopulated ? OMX_TRUE : OMX_FALSE);
}

inline OMX_DIRTYPE ImgEns_Port::getDirection(void) const
{
	return mParamPortDefinition.eDir;
}

inline OMX_VIDEO_PORTDEFINITIONTYPE *ImgEns_Port::getVideoPortDefinition(void)
{
	return &mParamPortDefinition.format.video;
}

inline OMX_IMAGE_PORTDEFINITIONTYPE *ImgEns_Port::getImagePortDefinition(void) {
	return &mParamPortDefinition.format.image;
}

inline OMX_U32 ImgEns_Port::getPortIndex(void) const {
	return mParamPortDefinition.nPortIndex;
}

inline OMX_U32 ImgEns_Port::getBufferCountActual(void) {
	return mParamPortDefinition.nBufferCountActual;
}

inline OMX_U32 ImgEns_Port::getBufferCountCurrent(void) {
	return mBufferCountCurrent;
}

inline ImgEns_BUFFERHEADER_TABLETYPE *
ImgEns_Port::getEnsBufferHdrTablePtr(void) {
	return mBufHdrTablePtr;
}

inline void ImgEns_Port::setSupplierPreference(
		const OMX_BUFFERSUPPLIERTYPE eBufferSupplier) {
	mSupplierPreference = eBufferSupplier;
}

inline OMX_BUFFERSUPPLIERTYPE ImgEns_Port::getSupplierPreference(void) const {
	return mSupplierPreference;
}

inline void ImgEns_Port::setBufferSupplier(
		const OMX_BUFFERSUPPLIERTYPE eBufferSupplier) {
	mBufferSupplier = eBufferSupplier;
}

inline OMX_BUFFERSUPPLIERTYPE ImgEns_Port::getBufferSupplier(void) const {
	return mBufferSupplier;
}

inline void ImgEns_Port::setTunnelFlags(const OMX_U32 nTunnelFlags) {
	mTunnelFlags = nTunnelFlags;
}

inline OMX_U32 ImgEns_Port::getTunnelFlags(void) const {
	return mTunnelFlags;
}

inline void ImgEns_Port::setTunneled(OMX_HANDLETYPE hTunneledComponent,
		OMX_U32 nTunneledPort, OMX_U32 nTunnelFlags) {
	mTunneledComponent = hTunneledComponent;
	mTunneledPort = nTunneledPort;
	mTunnelFlags = nTunnelFlags;
}

inline OMX_BOOL ImgEns_Port::isEnabled(void) const {
	return mParamPortDefinition.bEnabled;
}

inline void ImgEns_Port::setEnabledFlag(const OMX_BOOL enabled) {
	mParamPortDefinition.bEnabled = enabled;
}

inline OMX_HANDLETYPE ImgEns_Port::getTunneledComponent(void) const {
	return mTunneledComponent;
}

inline OMX_U32 ImgEns_Port::getTunneledPort(void) const {
	return mTunneledPort;
}

inline OMX_PORTDOMAINTYPE ImgEns_Port::getDomain() const {
	return mParamPortDefinition.eDomain;
}

inline OMX_BOOL ImgEns_Port::useProprietaryCommunication(void) const {
	if (mTunneledComponent && !mUseStandardTunneling) {
		return OMX_TRUE;
	}

	return OMX_FALSE;
}

inline OMX_BOOL ImgEns_Port::useStandardTunneling(void) const {
	if (mTunneledComponent && mUseStandardTunneling) {
		return OMX_TRUE;
	}

	return OMX_FALSE;
}

inline OMX_BOOL ImgEns_Port::isBufferSupplier(void) const {
	if ((mBufferSupplier == OMX_BufferSupplyOutput
			&& mParamPortDefinition.eDir == OMX_DirOutput)
			|| (mBufferSupplier == OMX_BufferSupplyInput
					&& mParamPortDefinition.eDir == OMX_DirInput)) {
		return OMX_TRUE;
	}

	return OMX_FALSE;
}

inline const ImgEns_Component & ImgEns_Port::getENSComponent() const
{
	return mEnsComponent;
}

inline ImgEns_Component & ImgEns_Port::getENSComponent()
{
	return mEnsComponent;
}

inline ImgEns_Port_Fsm * ImgEns_Port::getPortFsm(void) const
{
	return mPortFsm;
}


inline void ImgEns_Port::forceStandardTunneling()
{
	mUseStandardTunneling = OMX_TRUE;
}

inline bool ImgEns_Port::isMpc(void) const
{
	return mIsMpc;
}

inline void ImgEns_Port::setMpc(const bool mpc)
{
	mIsMpc = mpc;
}


/// Structure used to pass the common information that an ENS port needs at instantiation time.
class IMGENS_API ImgEns_CommonPortData
{
public:
	// Constructor
	ImgEns_CommonPortData(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferCountMin, OMX_U32 nBufferSizeMin, OMX_PORTDOMAINTYPE eDomain, OMX_BUFFERSUPPLIERTYPE eSupplierPref);
	OMX_U32                mPortIndex;          /// Port index
	OMX_DIRTYPE            mDirection;          /// Direction (input or output) of this port
	OMX_U32                mBufferCountMin;     /// The minimum number of buffers this port requires
	OMX_U32                mBufferSizeMin;      /// Minimum size, in bytes, for buffers to be used for this port
	OMX_PORTDOMAINTYPE     mPortDomain;         /// Domain of the port
	OMX_BUFFERSUPPLIERTYPE mBufferSupplierPref; /// Port supplier preference when tunneling between two ports
};
/// @}

#endif // _IMG_ENS_PORT_H_
