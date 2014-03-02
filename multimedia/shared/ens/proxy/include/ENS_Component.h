/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Component.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_COMPONENT_H_
#define _ENS_COMPONENT_H_

#include "ENS_Redefine_Class.h"

#include "ENS_macros.h"
#include <ENS_HwRm.h>
#include "ENS_IOMX.h"

#undef OMX_TRACE_UID
/* depends on OpenMax IL */
#include "OMX_Component.h"
#include "ENS_String.h"
#include "ENS_ResourcesDB.h"

#include "ENS_DBC.h"
#include "ENS_Wrapper_Services.h"

#include "ENS_Fsm.h"

#include "ENS_Trace.h"

// ER338925: do not check struct version and size in all configurations
#ifdef ENS_DONT_CHECK_STRUCT_SIZE_AND_VERSION
#define CHECK_STRUCT_SIZE_AND_VERSION(ptr,type);
#else
#define CHECK_STRUCT_SIZE_AND_VERSION(ptr,type) \
    {type * _tempstruct = (type *)ptr; OMX_VERSIONTYPE version; version.nVersion=0; getOmxIlSpecVersion(&version);\
    if (!(_tempstruct->nSize == sizeof(type) && ((_tempstruct->nVersion.nVersion & 0xFFFF) == (version.nVersion & 0xFFFF)))) DBC_ASSERT(0);}
#endif

#define MAX_PORTS_COUNT_BUF_SHARING 10

typedef enum bufferSharingDirection {
	INPUT_TO_OUTPUT=1,
	OUTPUT_TO_INPUT=2
} ENS_BufSharingDir;

typedef struct bufSharingGraphLink {
	OMX_BOOL isRequested;
	OMX_BOOL isBroken;			// Buffer sharing was requested but not possible due to buffersupplier type mismatches
	ENS_BufSharingDir bufferDir;
} ENS_BufSharingGraphLink;

class ENS_Port;
class ENS_Component_Fsm;
class ProcessingComponent;

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
/// interfaces called ports. Ports are implemented by the ENS_Port class.
/// @{

/// Class implementing the ENS Component 
class ENS_Component: public TraceObject, public ENS_IOMX
{
public:
    /** Enum to define suspension status */
    typedef enum {
        Preempt_None,
        Preempt_Partial,
        Preempt_Total
    } PreemptionState;

public:

        // -----------------------------------------------------------------------
        // Methods which are directly called from the OMX wrapper
        // -----------------------------------------------------------------------

    /// FIXME: All these statics are obsolete and must be removed
#if ENS_VERSION == ENS_VERSION_SHARED
        ENS_API_IMPORT static OMX_ERRORTYPE SendCommand(
                ENS_Component * pEnsComp,
                OMX_COMMANDTYPE Cmd, 
                OMX_U32 nParam, 
                OMX_PTR pCmdData);

        ENS_API_IMPORT static OMX_ERRORTYPE SetParameter(
                ENS_Component * pEnsComp, 
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT static OMX_ERRORTYPE GetParameter(
                ENS_Component * pEnsComp, 
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT static OMX_ERRORTYPE SetConfig(
                ENS_Component * pEnsComp, 
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure);

        ENS_API_IMPORT static OMX_ERRORTYPE GetConfig(
                ENS_Component * pEnsComp, 
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure);

        ENS_API_IMPORT static OMX_ERRORTYPE GetExtensionIndex(
                ENS_Component * pEnsComp,
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType);

        ENS_API_IMPORT static OMX_ERRORTYPE GetState(
                ENS_Component * pEnsComp,
                OMX_STATETYPE* pState);
        
        ENS_API_IMPORT static OMX_ERRORTYPE ComponentTunnelRequest(
                ENS_Component * pEnsComp,
                OMX_U32 nPort,
                OMX_HANDLETYPE hTunneledComp,
                OMX_U32 nTunneledPort,
                OMX_TUNNELSETUPTYPE* pTunnelSetup);

        ENS_API_IMPORT static OMX_ERRORTYPE SetCallbacks(
                ENS_Component * pEnsComp,
                OMX_CALLBACKTYPE* pCallbacks, 
                OMX_PTR pAppData);

        ENS_API_IMPORT static OMX_ERRORTYPE EmptyThisBuffer(
                ENS_Component * pEnsComp,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT static OMX_ERRORTYPE FillThisBuffer(
                ENS_Component * pEnsComp,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT static OMX_ERRORTYPE UseBuffer(
                ENS_Component * pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes,
                OMX_U8* pBuffer);

        ENS_API_IMPORT static OMX_ERRORTYPE AllocateBuffer(
                ENS_Component * pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes);

        ENS_API_IMPORT static OMX_ERRORTYPE FreeBuffer(
                ENS_Component * pEnsComp,
                OMX_U32 nPortIndex,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT static OMX_ERRORTYPE UseEGLImage(
                ENS_Component * pEnsComp,
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                void* eglImage);

        ENS_API_IMPORT static OMX_ERRORTYPE GetComponentVersion(
                ENS_Component * pEnsComp,
                OMX_STRING pComponentName,
                OMX_VERSIONTYPE* pComponentVersion,
                OMX_VERSIONTYPE* pSpecVersion,
                OMX_UUIDTYPE* pComponentUUID);

        ENS_API_IMPORT static OMX_ERRORTYPE ComponentDeInit(
                ENS_Component * pEnsComp);

        ENS_API_IMPORT static OMX_ERRORTYPE ComponentRoleEnum(
                ENS_Component * pEnsComp,
                OMX_U8 *cRole,
                OMX_U32 nIndex);
    /// END OF FIXME
#endif

        ENS_API_IMPORT virtual OMX_ERRORTYPE SendCommand(
                OMX_COMMANDTYPE Cmd, 
                OMX_U32 nParam, 
                OMX_PTR pCmdData);

        ENS_API_IMPORT virtual OMX_ERRORTYPE SetParameter(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE GetParameter(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE SetConfig(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE GetConfig(
                OMX_INDEXTYPE nIndex,
                OMX_PTR pComponentConfigStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE GetExtensionIndex(
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType);

        ENS_API_IMPORT virtual OMX_ERRORTYPE GetState(
                OMX_STATETYPE* pState);
        
        ENS_API_IMPORT virtual OMX_ERRORTYPE ComponentTunnelRequest(
                OMX_U32 nPort,
                OMX_HANDLETYPE hTunneledComp,
                OMX_U32 nTunneledPort,
                OMX_TUNNELSETUPTYPE* pTunnelSetup);

        ENS_API_IMPORT virtual OMX_ERRORTYPE SetCallbacks(
                OMX_CALLBACKTYPE* pCallbacks, 
                OMX_PTR pAppData);

        ENS_API_IMPORT virtual OMX_ERRORTYPE EmptyThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE FillThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE UseBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes,
                OMX_U8* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE AllocateBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes);

        ENS_API_IMPORT virtual OMX_ERRORTYPE FreeBuffer(
                OMX_U32 nPortIndex,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE UseEGLImage(
                OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_U32 nPortIndex,
                OMX_PTR pAppPrivate,
                void* eglImage);

        ENS_API_IMPORT virtual OMX_ERRORTYPE GetComponentVersion(
                OMX_STRING pComponentName,
                OMX_VERSIONTYPE* pComponentVersion,
                OMX_VERSIONTYPE* pSpecVersion,
                OMX_UUIDTYPE* pComponentUUID);

        ENS_API_IMPORT virtual OMX_ERRORTYPE ComponentDeInit();

        ENS_API_IMPORT virtual OMX_ERRORTYPE ComponentRoleEnum(
                OMX_U8 *cRole,
                OMX_U32 nIndex);

        
        /// Constructor
        ///
        /// Create a component with a specified number of ports.
        /// Ports must be subsequently created and added using 
        ///  ENS_Component::addPort member function
        /// \param[in] nPortCount 
        ///    Number of ports of the created component
        
        ENS_API_IMPORT OMX_ERRORTYPE construct(OMX_U32 nPortCount, OMX_U32 nRoleCount=1, bool ArmOnly=false);

        /// Destructor
        ENS_API_IMPORT virtual ~ENS_Component(void) = 0;


        // -----------------------------------------------------------------------
        //                  Getters and Setters
        // -----------------------------------------------------------------------

        ENS_API_IMPORT virtual void *GetProcessingComponent(void);

        ENS_API_IMPORT virtual const char * getName() const;

        ENS_API_IMPORT virtual void setName(const char *cName);

        OMX_U32 getPortCount(void) const { return mPortCount; }

        const ENS_Port * getPort(OMX_U32 nPortIndex) const { 
            DBC_ASSERT((nPortIndex<getPortCount()) && (mPorts[nPortIndex] != 0));
            return mPorts[nPortIndex]; 
        }

        ENS_Port * getPort(OMX_U32 nPortIndex) { 
            DBC_ASSERT((nPortIndex<getPortCount()) && (mPorts[nPortIndex] != 0));
            return mPorts[nPortIndex]; 
        }

        ENS_API_IMPORT virtual OMX_VERSIONTYPE getVersion(void) const;

        virtual OMX_U32 getRoleNB() { return mRoleCount;}
        ENS_API_IMPORT virtual OMX_ERRORTYPE getActiveRole(OMX_U8 * aActiveRole) const;
        ENS_API_IMPORT virtual OMX_ERRORTYPE setActiveRole(const OMX_U8 * aActiveRole); 
         
        virtual OMX_ERRORTYPE getRole(OMX_U8** aRole, const OMX_U32 role_idx) {
            DBC_ASSERT(role_idx <= mRoleCount);
            *aRole = mRoles[role_idx];
            return OMX_ErrorNone;
        }

        const ProcessingComponent & getProcessingComponent(void) const { return *mProcessingComponent; }
        ProcessingComponent & getProcessingComponent(void) { return *mProcessingComponent; }
        void setProcessingComponent(ProcessingComponent *processingComponent) { mProcessingComponent = processingComponent; }        
	
        /** Initialize the wrapper callback structure if any */
        void setEnsWrapperCB(ENSWRAPPER_CALLBACKTYPE *ensWrapperCb);
        ENSWRAPPER_CALLBACKTYPE* getEnsWrapperCB();
        
        /** @name Resource concealment getters/setters */
        //@{
        /** Enable or disable resource concealment support */
        void setResourceConcealmentForbidden(OMX_BOOL resource);

        /** Return resource concealment boolean state */
        OMX_BOOL getResourceConcealmentForbidden() const;

        /** Return the current OMX state of the component */
        ENS_API_IMPORT OMX_STATETYPE
        omxState() const;
        //@}


        /** @name Priority getters/setters */
        //@{
        /** Set the component priority and the associated priority group
         *
         * @param priority The component group priority value.
         * @param groupid  The id of the priority group which the component belongs to
         */
        void setPriorityMgmt(OMX_U32 priority, OMX_U32 groupid);

        /** Return the component group priority value */
        OMX_U32 getGroupPriorityValue() const;

        /** Return the component group priority id */
        OMX_U32 getGroupPriorityId() const;
        //@}
        

        /** @name Suspension policy getters/setters */
        //@{
        /** Set the suspension policy mode of the component */
        void setSuspensionPolicy(OMX_SUSPENSIONPOLICYTYPE policy);

        /** Retrieve the suspension policy mode of the component */
        OMX_SUSPENSIONPOLICYTYPE getSuspensionPolicy() const;
        //@}

        /** @name Preemption policy getters/setters */
        //@{
        /** Set the preemption policy mode of the component */
        void setPreemptionPolicy(OMX_BOOL policy);

        /** Retrieve the preemption policy mode of the component */
        OMX_BOOL getPreemptionPolicy() const;
        //@}
        
        /** Return number of OMX_EventCmdReceived callbacks which will be sent by ENSComponent on a OMX_SendCommand call*/
        /** Set as virtual just to allow ENSWrapper unitary testing (should not be redefined otherwise)*/
        virtual unsigned int getNbCmdReceivedEventsToWait(OMX_COMMANDTYPE Cmd, OMX_U32 nParam) {
            if(((Cmd == OMX_CommandPortDisable) || (Cmd == OMX_CommandPortEnable)) && (nParam == OMX_ALL)) {
                //In case of disablePort/enablePort with index OMX_ALL -> each port will sent the cmdReceived event.
                return getPortCount();
            } else if ((Cmd == OMX_CommandFlush) || (Cmd == OMX_CommandMarkBuffer)) {
                //In this case, ENS component will not send any cmdReceived event.
                return 0;
            } else {
                return 1;
            }
        };
        ENS_API_IMPORT virtual OMX_U32 getIndexEventCmdReceived();
        ENS_API_IMPORT virtual unsigned short getTraceEnable() const ;

        // -----------------------------------------------------------------------
        // Methods relative to Resources Management
        // -----------------------------------------------------------------------
        static void RM_ENS_Notify_Error (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_IN RM_STATUS_E nError);

        static void RM_ENS_Notify_ResReservationProcessed (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_IN OMX_BOOL bResourcesReservGranted);

        static void RM_ENS_Cmd_SetRMConfig (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_IN const RM_SYSCTL_T* pRMcfg);

        static void RM_ENS_Cmd_ReleaseResource (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_IN OMX_BOOL bSuspend);

        static RM_STATUS_E RM_ENS_Get_Capabilities (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_OUT RM_CAPABILITIES_T* pCapabilities);

        virtual ENS_API_IMPORT RM_STATUS_E getCapabilities (OMX_INOUT OMX_U32 &pCapBitmap);

        static RM_STATUS_E RM_ENS_Get_ResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_INOUT RM_EMDATA_T* pEstimationData);
        
        virtual ENS_API_IMPORT RM_STATUS_E getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_INOUT RM_EMDATA_T* pEstimationData);
        
        OMX_ERRORTYPE RegisterForRm(void);

        /** State the component preemption state requested by RM to release resources */
        void setPreemptionState(const PreemptionState& state);

        virtual OMX_ERRORTYPE StartLowPower(OMX_U32 LPADomainID){return OMX_ErrorNone;};
        virtual OMX_ERRORTYPE StopLowPower(){return OMX_ErrorNone;};

        /** Method called by FSM when transitioning to IdleSuspended or PauseSuspended
         * to request component releasing ressources re acquired by RM
         */
        ENS_API_IMPORT virtual OMX_ERRORTYPE
        suspendResources(void);
 
        /** Method called by FSM when quitting IdleSuspended or PauseSuspended state
         * to let component re-aquiring ressources lost by suspension
         */
        ENS_API_IMPORT virtual OMX_ERRORTYPE
        unsuspendResources(void);

        bool
        resourcesSuspended(void) const;
 
        /** Method called by FSM when quitting IdleSuspended or PauseSuspended state
         * to let component re-aquiring ressources lost by suspension
         */
        void
        resourcesSuspended(bool bFlag);

        // -----------------------------------------------------------------------
        // STOST Trace
        // -----------------------------------------------------------------------
        ENS_API_IMPORT TraceInfo_t * getSharedTraceInfoPtr(void);   
        TraceBuilder * getTraceBuilder() const { return mTraceBuilder; };
        ENS_API_IMPORT virtual OMX_U32 getUidTopDictionnary(void);

        // -----------------------------------------------------------------------
        //          Callbacks from ProcessingComponent to proxy
        // -----------------------------------------------------------------------

        ENS_API_IMPORT void eventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);
        ENS_API_IMPORT void emptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT void fillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);

        
        // -----------------------------------------------------------------------
        //          Methods called by either the component or port FSMs
        //          TODO find a way to ensure only FSM can call them
        // -----------------------------------------------------------------------

        ENS_API_IMPORT OMX_ERRORTYPE registerILClientCallbacks(
                OMX_CALLBACKTYPE* pCallbacks, 
                OMX_PTR pAppData);

        ENS_API_IMPORT virtual OMX_ERRORTYPE eventHandlerCB(
                OMX_EVENTTYPE eEvent,
                OMX_U32 nData1,
                OMX_U32 nData2,
                OMX_PTR pEventData); 
                
        ENS_API_IMPORT virtual OMX_ERRORTYPE construct(void);

        OMX_ERRORTYPE emptyBufferDoneCB(
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
        { return mCallbacks.EmptyBufferDone(
                getOMXHandle(), mAppData, pBuffer); }

        OMX_ERRORTYPE fillBufferDoneCB(
                OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
        { return mCallbacks.FillBufferDone(
                getOMXHandle(), mAppData, pBuffer); }

        ENS_API_IMPORT virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,  
                OMX_PTR pComponentParameterStructure) const;

        ENS_API_IMPORT virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nConfigIndex, 
                OMX_PTR pComponentConfigStructure) const;

        ENS_API_IMPORT virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nConfigIndex, 
                OMX_PTR pComponentConfigStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE getExtensionIndex(
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType) const;

        ENS_API_IMPORT virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;


        // TODO could be made private but would need to have bindComponentToHost
        // as a member function   
        OMX_HANDLETYPE getOMXHandle() { return mOmxHandle; }
        ENS_API_IMPORT virtual OMX_ERRORTYPE setOMXHandleAndRegisterRm(OMX_HANDLETYPE hComp);
        
        // TODO remove: should be done from factory method of each component
        ENS_API_IMPORT virtual OMX_ERRORTYPE createResourcesDB(void) = 0;

        OMX_U32 getMemoryPreset(void) const { 
            DBC_ASSERT(mRMP != 0);
            return mRMP->getMemoryPreset(); }

        /** Return the component preemption state on-going */
        const PreemptionState&
        getPreemptionState() const;
        
        ENS_API_IMPORT OMX_U32 getNMFDomainHandle(OMX_U32 nPortIndex=0);
        ENS_API_IMPORT OMX_U32 getNMFDomainHandle(RM_NMFD_E domainType,OMX_U32 nPortIndex=0);
        
        /* The callback function called by NMF when a Panic occurs on DSP, 
        either triggered by an explicit call to Panic(), 
        or because CM detected hanging on DSP.
        Empty by default, needs to be overridden by OMX components */
        ENS_API_IMPORT virtual void NmfPanicCallback(
              void *contextHandler           , 
              t_nmf_service_type serviceType , 
              t_nmf_service_data *serviceData);

        /* This function has been implemented to do some internal
        state-specific work in component proxy */
        ENS_API_IMPORT virtual OMX_ERRORTYPE doSpecificInternalState(const ENS_FsmEvent *evt) { return OMX_ErrorNone; }

        OMX_BOOL allPortsInSameFsmState(FsmState state);

protected:              
        OMX_ERRORTYPE DefaultSetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure);
        OMX_ERRORTYPE DefaultGetConfigOrParameter(OMX_INDEXTYPE nIndex, OMX_PTR pStructure) const;

protected:
        /** Empty constructor */
        ENS_API_IMPORT ENS_Component();

protected:
        ENS_ResourcesDB * mRMP;

        // to be filled by factory methods
        OMX_U32             mRoleCount;
        OMX_U8              mActiveRole[OMX_MAX_STRINGNAME_SIZE];
        OMX_U8*             mRoles[OMX_MAX_STRINGNAME_SIZE];

        // TODO make private
        // used in AFM_Component to dispatch setStateComplete evts 
        ENS_Component_Fsm *         getCompFsm(void) const { return  mCompFsm; }

        ENS_API_IMPORT void                addPort(ENS_Port *);

private :
        ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>   mName; 
        OMX_HANDLETYPE                              mOmxHandle; 
        ENS_Component_Fsm *                         mCompFsm;
        OMX_VERSIONTYPE                             mVersion;   
        OMX_U32                                     mPortCount;
        ENS_Port **                                 mPorts; 
        OMX_CALLBACKTYPE                            mCallbacks;
        OMX_PTR                                     mAppData;
        ProcessingComponent *                       mProcessingComponent;
        RM_CBK_T                                    mRMEcbks;
        PreemptionState                             mPreemptionState;
        TraceBuilder  *                             mTraceBuilder;

        /** 
         * Boolean to enable or disable resource concealment in components.
         * Related with OMX_RESOURCECONCEALMENTTYPE
         */
        OMX_BOOL                                    mResourceConcealmentForbidden;

        /**
         * The priority value of component.
         * Related with OMX_PRIORITYMGMTTYPE
         */
        OMX_U32                                     mGroupPriority;

        /**
         * The group Id of the component.All components from same group have
         * the same priority. Related with OMX_PRIORITYMGMTTYPE
         */
        OMX_U32                                     mGroupID; 
        
        /**
         * Flag to define if component supports suspension or not. It can take
         * two values OMX_SuspensionDisabled (default value) or
         * OMX_SuspensionEnabledRelated. Related with OMX_PARAM_SUSPENSIONPOLICYTYPE
         */
        OMX_SUSPENSIONPOLICYTYPE                    mSuspensionPolicy;
  
        /**
         * Flag to define if component supports standard preemption or raise the 
         * preemption request to IL Client. If false (default value) or then preemption
         * occurs as descirbed in spec v1.1, otherwise component does not transition
         * automatically to Idle then Loaded
         */
        OMX_BOOL                                    mPreemptionPolicy;

        /**
         * Flag to track if the resources of the component are suspened or not.
         * Updated when calling methods (un)suspendResources()
         */
        bool                                        mResourceSuspended;

        /**
         * Callback in ens wrapper to delegate some processing part like
         * construction or destruction of the nmf component graph.
         */
        ENSWRAPPER_CALLBACKTYPE*                    mEnsWrapperCb;

        //----------------------------------------------------------------//
        //          Buffer Sharing methods & data members
        //----------------------------------------------------------------//
private:
        ENS_BufSharingGraphLink    mBufSharingGraph[MAX_PORTS_COUNT_BUF_SHARING][MAX_PORTS_COUNT_BUF_SHARING];
        OMX_BOOL mBufferSharingEnabled;
       
protected:
        ENS_API_IMPORT OMX_ERRORTYPE   connectBufferSharingPorts(OMX_U32 inputPortIndex, OMX_U32 ourtputPortIndex);
        void initBufferSharingGraph();

public:
        void disableBufferSharing();
        void createBufferSharingGraph(void);

        OMX_BOOL isBufferSharingEnabled(){
            return mBufferSharingEnabled;
        }

        //Note: This flag will be removed in future
        bool BUFFER_SHARING_ENABLED;

};


/// pointer typedef
typedef ENS_Component * ENS_Component_p;

inline void
ENS_Component::setResourceConcealmentForbidden(OMX_BOOL resource)      
{ mResourceConcealmentForbidden = resource; }

inline OMX_BOOL
ENS_Component::getResourceConcealmentForbidden() const
{ return mResourceConcealmentForbidden; }

inline void
ENS_Component::setPriorityMgmt(OMX_U32 priority,
                               OMX_U32 groupid)
{
    mGroupPriority = priority;
    mGroupID = groupid;
}

inline OMX_U32
ENS_Component::getGroupPriorityValue() const
{ return mGroupPriority; }

inline OMX_U32
ENS_Component::getGroupPriorityId() const
{ return mGroupID; }

inline void
ENS_Component::setSuspensionPolicy(OMX_SUSPENSIONPOLICYTYPE policy)
{ mSuspensionPolicy = policy; }

inline OMX_SUSPENSIONPOLICYTYPE
ENS_Component::getSuspensionPolicy() const
{ return mSuspensionPolicy; }

inline void
ENS_Component::setPreemptionPolicy(OMX_BOOL policy)
{ mPreemptionPolicy = policy; }

inline OMX_BOOL
ENS_Component::getPreemptionPolicy() const
{ return mPreemptionPolicy; }

inline const ENS_Component::PreemptionState&
ENS_Component::getPreemptionState() const
{ return mPreemptionState; }

inline void
ENS_Component::setPreemptionState(const ENS_Component::PreemptionState& state)
{ mPreemptionState = state; }

inline void
ENS_Component::setEnsWrapperCB(ENSWRAPPER_CALLBACKTYPE *ensWrapperCb)
{ mEnsWrapperCb = ensWrapperCb; }

inline ENSWRAPPER_CALLBACKTYPE*
ENS_Component::getEnsWrapperCB()
{ return mEnsWrapperCb; }

#endif // _ENS_COMPONENT_H_
