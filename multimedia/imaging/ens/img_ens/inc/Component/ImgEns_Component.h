/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_ENS_COMPONENT_H_
#define _IMG_ENS_COMPONENT_H_

#include "ImgEns_IOmx.h"

#undef OMX_TRACE_UID

#include <omxil/OMX_Component.h> /* depends on OpenMax IL */

#include "ImgEns_Fsm.h"
#include "ImgEns_Trace.h"

#define MAX_PORTS_COUNT_BUFFER_SHARING 10

class ImgEns_Port;
class ImgEns_Component_Fsm;
class Img_ProcessingComponent;
class Img_TraceBuilder;

/// @defgroup ens_component_class ENS Component (base class)
/// @ingroup ens_classes
/// Abstract base class for ENS components
///
/// This is the abstract base class providing the common interface of
/// ENS components. Media specific components should derive from this class.
/// Media independent processing should use only the interface provided by this
/// class.
///
/// A ENS Component represents an individual block of functionnality. Components
/// can be sources, sinks, codecs, filter, splitter, mixers, or any other data
/// operator. Data communication to and from a component is conducted through
/// interfaces called ports. Ports are implemented by the ImgEns_Port class.
/// @{

class IMGENS_API ImgEns_Component: public Img_TraceObject, public ImgEns_IOmx
{/// Class implementing the ENS Component

public:
	/// Constructor
	///
	/// Create a component with a specified number of ports.
	/// Ports must be subsequently created and added using
	///  ImgEns_Component::addPort member function
	/// \param[in] nPortCount
	///    Number of ports of the created component

	ImgEns_Component(const char *name, unsigned int instanceNb = 0);
	virtual ~ImgEns_Component(void);

	OMX_ERRORTYPE construct(OMX_U32 nPortCount, OMX_U32 nRoleCount = 1);

	OMX_ERRORTYPE Dispatch(const ImgEns_FsmEvent *pEvent);
	OMX_ERRORTYPE DispatchToPort(unsigned int index, const ImgEns_FsmEvent *pEvent);

public:
	// -----------------------------------------------------------------------
	// Methods which are directly called from the OMX wrapper
	// -----------------------------------------------------------------------
	virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);

	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType);
	virtual OMX_ERRORTYPE GetState(OMX_STATETYPE* pState);

	virtual OMX_ERRORTYPE ComponentTunnelRequest(OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup);

	virtual OMX_ERRORTYPE SetCallbacks(const OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData);

	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer);
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE UseEGLImage(OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void* eglImage);

	virtual OMX_ERRORTYPE GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID);

	virtual OMX_ERRORTYPE ComponentDeInit();
	virtual OMX_ERRORTYPE ComponentRoleEnum(OMX_U8 *cRole, OMX_U32 nIndex);

	// -----------------------------------------------------------------------
	//                  Getters and Setters
	// -----------------------------------------------------------------------
	// virtual OMX_U32 getPortCount(void) const { return mPortCount; }

	const ImgEns_Port * getPort(OMX_U32 nPortIndex) const;
	ImgEns_Port       * getPort(OMX_U32 nPortIndex);

	virtual OMX_VERSIONTYPE getVersion(void) const;

	virtual OMX_U32 getRoleNB() const { return mRoleCount; }
	virtual OMX_ERRORTYPE getActiveRole(OMX_U8 * aActiveRole) const;
	virtual OMX_ERRORTYPE setActiveRole(const OMX_U8 * aActiveRole);
	virtual OMX_ERRORTYPE getRole(OMX_U8** aRole, const OMX_U32 role_idx) const;

	const Img_ProcessingComponent & getProcessingComponent(void) const { return *mProcessingComponent; }
	Img_ProcessingComponent       & getProcessingComponent(void)       { return *mProcessingComponent; }
	void setProcessingComponent(Img_ProcessingComponent *processingComponent) { mProcessingComponent = processingComponent; }

	/** @name Resource concealment getters/setters */
	//@{
	void setResourceConcealmentForbidden(OMX_BOOL resource);/** Enable or disable resource concealment support */
	OMX_BOOL getResourceConcealmentForbidden() const;       /** Return resource concealment boolean state */

	OMX_STATETYPE omxState() const;/** Return the current OMX state of the component */
	//@}

	/** @name Priority getters/setters */
	//@{
	/** Set the component priority and the associated priority group
	 * @param priority The component group priority value.
	 * @param groupid  The id of the priority group which the component belongs to
	 */
	void setPriorityMgmt(OMX_U32 priority, OMX_U32 groupid);
	OMX_U32 getGroupPriorityValue() const;/** Return the component group priority value */
	OMX_U32 getGroupPriorityId   () const;/** Return the component group priority id */
	//@}

	/** @name Suspension policy getters/setters */
	//@{
	void setSuspensionPolicy(OMX_SUSPENSIONPOLICYTYPE policy);/** Set the suspension policy mode of the component */
	OMX_SUSPENSIONPOLICYTYPE getSuspensionPolicy() const; /** Retrieve the suspension policy mode of the component */
	//@}

	/** @name Preemption policy getters/setters */
	//@{
	void setPreemptionPolicy(OMX_BOOL policy);/** Set the preemption policy mode of the component */
	OMX_BOOL getPreemptionPolicy() const;/** Retrieve the preemption policy mode of the component */
	//@}

	virtual unsigned short getTraceEnable() const;

	//Management of preemption state
	
	enum enumPreemptionState
	{/** Enum to define suspension status */
		Preempt_None,
		Preempt_Partial,
		Preempt_Total,
	};

	void setPreemptionState(const enumPreemptionState& state); /** State the component preemption state requested by RM to release resources */
	const enumPreemptionState& getPreemptionState() const; /** Return the component preemption state on-going */

	virtual OMX_ERRORTYPE StartLowPower(OMX_U32 /*LPADomainID*/) {IMGENS_ASSERT(0); return OMX_ErrorNone; }
	virtual OMX_ERRORTYPE StopLowPower() { IMGENS_ASSERT(0);  return OMX_ErrorNone; }

	/** Method called by FSM when transitioning to IdleSuspended or PauseSuspended to request component releasing ressources re acquired by RM */
	virtual OMX_ERRORTYPE suspendResources(void)  { IMGENS_ASSERT(0); return(OMX_ErrorNone);}
	/** Method called by FSM when quitting IdleSuspended or PauseSuspended state to let component re-aquiring ressources lost by suspension */
	virtual OMX_ERRORTYPE unsuspendResources(void) {IMGENS_ASSERT(0);  return(OMX_ErrorNone);}
	bool resourcesSuspended(void) const;
	/** Method called by FSM when quitting IdleSuspended or PauseSuspended state to let component re-aquiring ressources lost by suspension */
	void resourcesSuspended(bool bFlag);

	// -----------------------------------------------------------------------
	// STOST Trace
	// -----------------------------------------------------------------------
	ImgEns_TraceInfo * getSharedTraceInfoPtr(void);
	Img_TraceBuilder * getTraceBuilder() const { return mTraceBuilder;}

	virtual OMX_U32 getUidTopDictionnary(void);

	// -----------------------------------------------------------------------
	//          Callbacks from Img_ProcessingComponent to proxy
	// -----------------------------------------------------------------------
	void eventHandler   (OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);
	void emptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);
	void fillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	// -----------------------------------------------------------------------
	//          Methods called by either the component or port FSMs
	//          TODO find a way to ensure only FSM can call them
	// -----------------------------------------------------------------------
	OMX_ERRORTYPE registerILClientCallbacks(const OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData);
	virtual OMX_ERRORTYPE construct(void);

	virtual OMX_ERRORTYPE eventHandlerCB   (OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData=NULL);
	virtual OMX_ERRORTYPE emptyBufferDoneCB(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE fillBufferDoneCB (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex , OMX_PTR pComponentParameterStructure) const;
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex , OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getConfig   (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;
	virtual OMX_ERRORTYPE setConfig   (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
	virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

	OMX_BOOL allPortsInSameFsmState(ImgEns_Fsm::_fnState state) const;

	const char *GetComponentName() const { return(m_Name); }

protected:
	OMX_ERRORTYPE DefaultSetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure);
	OMX_ERRORTYPE DefaultGetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const;

protected:
	const char * m_FamilyName; // Global name of component
	StaticString<OMX_MAX_STRINGNAME_SIZE>    m_Name; // Name dedicated for instance
	
	// to be filled by factory methods
	OMX_U32 mRoleCount;
	OMX_U8  mActiveRole[OMX_MAX_STRINGNAME_SIZE];
	OMX_U8* mRoles[OMX_MAX_STRINGNAME_SIZE];

	// used in FSM_Component to dispatch setStateComplete evts
	ImgEns_Component_Fsm * getCompFsm(void) const { return mCompFsm; }

	void addPort(ImgEns_Port *);

protected:
	ImgEns_Component_Fsm    *mCompFsm;
	ImgEns_Port            **mPorts;
	OMX_CALLBACKTYPE         mCallbacks;
	OMX_PTR                  mAppData;
	Img_ProcessingComponent* mProcessingComponent;

	enumPreemptionState mPreemptionState;
	Img_TraceBuilder *  mTraceBuilder;

	OMX_BOOL mResourceConcealmentForbidden; /// Boolean to enable or disable resource concealment in components. Related with OMX_RESOURCECONCEALMENTTYPE
	OMX_U32  mGroupPriority;                /// The priority value of component. Related with OMX_PRIORITYMGMTTYPE
	OMX_U32  mGroupID;                      ///The group Id of the component.All components from same group have the same priority. Related with OMX_PRIORITYMGMTTYPE

	/**
	 * Flag to define if component supports suspension or not. It can take two values:
	 * OMX_SuspensionDisabled (default value) or
	 * OMX_SuspensionEnabledRelated. Related with OMX_PARAM_SUSPENSIONPOLICYTYPE
	 */
	OMX_SUSPENSIONPOLICYTYPE mSuspensionPolicy;

	/**
	 * Flag to define if component supports standard preemption or raise the
	 * preemption request to IL Client. If false (default value) or then preemption
	 * occurs as descirbed in spec v1.1, otherwise component does not transition
	 * automatically to Idle then Loaded
	 */
	OMX_BOOL mPreemptionPolicy;

	/**
	 * Flag to track if the resources of the component are suspened or not. Updated when calling methods (un)suspendResources()
	 */
	bool mResourceSuspended;

	//----------------------------------------------------------------//
	//          Buffer Sharing methods & data members
	//----------------------------------------------------------------//
protected:
	typedef struct
	{
		OMX_BOOL isRequested;
		OMX_BOOL isBroken; // Buffer sharing was requested but not possible due to buffersupplier type mismatches
	} BufferSharingGraphLink;

	BufferSharingGraphLink mBufSharingGraph[MAX_PORTS_COUNT_BUFFER_SHARING][MAX_PORTS_COUNT_BUFFER_SHARING];
	OMX_BOOL mBufferSharingEnabled;

	OMX_ERRORTYPE connectBufferSharingPorts(OMX_U32 inputPortIndex, OMX_U32 ourtputPortIndex);
	void initBufferSharingGraph();

public:
	void disableBufferSharing();
	void createBufferSharingGraph(void);

	OMX_BOOL isBufferSharingEnabled() const {return mBufferSharingEnabled;}
};

inline void ImgEns_Component::setResourceConcealmentForbidden(OMX_BOOL resource)
{
	mResourceConcealmentForbidden = resource;
}

inline OMX_BOOL ImgEns_Component::getResourceConcealmentForbidden() const
{
	return mResourceConcealmentForbidden;
}

inline void ImgEns_Component::setPriorityMgmt(OMX_U32 priority, OMX_U32 groupid)
{
	mGroupPriority = priority;
	mGroupID       = groupid;
}

inline OMX_U32 ImgEns_Component::getGroupPriorityValue() const
{
	return mGroupPriority;
}

inline OMX_U32 ImgEns_Component::getGroupPriorityId() const
{
	return mGroupID;
}

inline void ImgEns_Component::setSuspensionPolicy(OMX_SUSPENSIONPOLICYTYPE policy)
{
	mSuspensionPolicy = policy;
}

inline OMX_SUSPENSIONPOLICYTYPE ImgEns_Component::getSuspensionPolicy() const
{
	return mSuspensionPolicy;
}

inline void ImgEns_Component::setPreemptionPolicy(OMX_BOOL policy)
{
	mPreemptionPolicy = policy;
}

inline OMX_BOOL ImgEns_Component::getPreemptionPolicy() const
{
	return mPreemptionPolicy;
}


inline const ImgEns_Component::enumPreemptionState& ImgEns_Component::getPreemptionState() const
{
	return mPreemptionState;
}

inline void ImgEns_Component::setPreemptionState(const ImgEns_Component::enumPreemptionState& state)
{
	mPreemptionState = state;
}


#endif // _IMG_ENS_COMPONENT_H_
