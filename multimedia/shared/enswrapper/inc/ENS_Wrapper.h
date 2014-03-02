/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * This wrapper will ensure run-to-completion execution model OSI OMX components
 * rely on. This wrapper takes care of creating threads requiered in Linux cases
 * to manage NMF MPC to HOST calls.
 */

/*****************************************************************************/

#ifndef _ENS_WRAPPER_H_
#define _ENS_WRAPPER_H_

#include <OMX_Component.h>

extern "C" {
  #include <los/api/los_api.h>
}

#include "ENS_IOMX.h"
#include "ENS_Wrapper_Services.h"
#include "BSD_list.h"
#include "ENS_Queue.h"
#include "ENS_RME_Wrapper.h"
#include "ENS_Wrapper_Construct.h"

#include <signal.h>
#include <pthread.h>

#if (! defined(ANDROID) &&  defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP))
  #define PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif

class ENS_Wrapper;
#include "ENS_Wrapper_CtxTrace.h"

 enum dump_actions {
    LOG_CURRENT = 0x1,    /*Current Component*/
    LOG_OPENED  = 0x2,    /*All Opened Components*/
    DUMP_FATAL  = 0x4,    /*Fatal dump*/
    UNKNOWN
};

void startENS_WrapperConstructThread();
void stopENS_WrapperConstructThread();
void ConstructENSProcessingPartAsync(OMX_HANDLETYPE hComponent);
void memStatus(void);

/********************************************************************************
 * List element to store active components
 ********************************************************************************/
typedef struct omx_cmp_list_elem {
    LIST_ENTRY(omx_cmp_list_elem) list_entry;
    OMX_COMPONENTTYPE * omx_component;
} omx_cmp_list_elem_t;

typedef LIST_HEAD(omx_cmp_list_head, omx_cmp_list_elem) omx_cmp_list_head_t;

/********************************************************************************
 * List element to store active components
 ********************************************************************************/
typedef struct omx_cmp_trace_list_elem {
    LIST_ENTRY(omx_cmp_trace_list_elem) list_entry;
    /* The name of component for which OSTtrace is to be activated */
    char name[OMX_MAX_STRINGNAME_SIZE];
    /* The Group bitmask for this component */
    OMX_U16 nTraceEnable;
} omx_cmp_trace_list_elem_t;

typedef LIST_HEAD(omx_cmp_trace_list_head, omx_cmp_trace_list_elem) omx_cmp_trace_list_head_t;

/********************************************************************************
 * Wrapper FIFOs sizes
 ********************************************************************************/
/// NMF MPC to HOST Callback message size: parameter size + 2 x 32-bits int
#define MAX_NMF_MESSAGE_SIZE 1024
#define NMF_CM_CB_FIFO_SIZE 256

/* This should be defined by the LOS API */
#define ENS_WRAPPER_INVALID_TID (t_los_process_id) 0xffffffff

#define ENS_WRAPPER_NO_HANDLE_ID_MASK            0xfffffff0
#define ENS_WRAPPER_ACTIVATEOSTTRACE_HANDLE_ID   (OMX_COMPONENTTYPE*)0xfffffffe
#define ENS_WRAPPER_LOCAL_HANDLE_ID              (OMX_COMPONENTTYPE*)0xfffffffd
#define ENS_WRAPPER_UPDATETRACELEVEL_HANDLE_ID   (OMX_COMPONENTTYPE*)0xfffffffc

/********************************************************************************
 * Helper functions for OMX_HANDLETYPE manipulation
 ********************************************************************************/

#define OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent) \
  ((OMX_COMPONENTTYPE *) hComponent)

#define OMX_HANDLETYPE_TO_WRAPPER(hComponent) \
  ((ENS_Wrapper *) OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent)->pComponentPrivate)

#define OMX_HANDLETYPE_TO_ENSCOMPONENT(hComponent) \
  (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ensComponent)

#define OMX_HANDLETYPE_TO_OMXCOMPONENT(hComponent) \
  (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->wrappedComponent)

#define OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)                      \
    ((!OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(hComponent)) &&              \
     (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ensComponent != NULL))

#define OMX_HANDLETYPE_GET_NAME(hComponent) \
  ((const char *) (OMX_HANDLETYPE_TO_WRAPPER(hComponent))->name)

#define OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(hComponent) \
  (((unsigned int)(hComponent) & ENS_WRAPPER_NO_HANDLE_ID_MASK) ==      \
   ENS_WRAPPER_NO_HANDLE_ID_MASK)

#define OMX_HANDLETYPE_CALL(hComponent, func, ...)                      \
    ( OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent) ?                      \
      OMX_HANDLETYPE_TO_ENSCOMPONENT(hComponent)->func(__VA_ARGS__) :   \
      OMX_HANDLETYPE_TO_OMXCOMPONENT(hComponent)->                \
      func(OMX_HANDLETYPE_TO_OMXCOMPONENT(hComponent), ##__VA_ARGS__) )

/********************************************************************************
 * Structure to hold and OMX CallBack from ENS components to client
 ********************************************************************************/
typedef enum {
    OMX_EVENTHANDLER,
    OMX_EMPTYBUFFERDONE,
    OMX_FILLBUFFERDONE,
    CB_thread_exit,
} CbMessageType;

typedef struct {
    CbMessageType type;
    OMX_COMPONENTTYPE *hComponent;
    union {
        struct {
            OMX_PTR pAppData;
            OMX_EVENTTYPE eEvent;
            OMX_U32 nData1;
            OMX_U32 nData2;
            OMX_PTR pEventData;
        } eventHandler;
        struct {
            OMX_PTR pAppData;
            OMX_BUFFERHEADERTYPE* pBuffer;
        } bufferDone;
    } args;
} CB_Message;

#define WRAPPER_LOCK(handle)                                    \
    do {                                                        \
        if (OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle) ||      \
            OMX_HANDLETYPE_IS_ENSCOMPONENT(handle)) {           \
            ENS_Wrapper::Prologue(handle, __func__);            \
        }                                                       \
    } while(0)

#define WRAPPER_UNLOCK(handle)                                  \
    do {                                                        \
        if (OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle) ||      \
            OMX_HANDLETYPE_IS_ENSCOMPONENT(handle)) {           \
            ENS_Wrapper::Epilogue(handle, __func__);            \
        }                                                       \
    } while(0)

/********************************************************************************
 * ENS_Wrapper related definitions
 ********************************************************************************/

class ENS_Wrapper ;
typedef ENS_Wrapper * ENS_Wrapper_p;

class ENS_Wrapper {

public:
    /** OMX IL handle of the wrapped ENS component
     * NULL when wrapper is not wrapping an ENS component */
    ENS_IOMX *ensComponent;

    /** OMX IL handle of the wrapped OMX component
     * NULL when wrapper is wrapping an ENS component */
    OMX_COMPONENTTYPE *wrappedComponent;

    /** OMX component provided to OMX IL Client */
    OMX_COMPONENTTYPE *omxComponent;

    /** The component name */
    char name[OMX_MAX_STRINGNAME_SIZE];

private:
    /** Initialize the wrapper members */
    void Init();
    /** Finalize initialization related to log */
    void InitLog();
    /** Finalize initialization related to NMF */
    void InitNMF();
    /** Finalize de-initialization related to NMF */
    void DeInitNMF(OMX_HANDLETYPE hComponent);

    /** OMX IL Client call backs */
    OMX_CALLBACKTYPE * mOMXClientCB;
    /** OMX IL wrapper call backs registered to the OSI component */
    OMX_CALLBACKTYPE * mOMXWrapperCB;

    /** Flag to indicate if a command is already being processed by OSI component */
    volatile  OMX_BOOL mCmdBeingProcessed ;
    /** To indicate how many threads are waiting for send command completion */
    volatile unsigned int mCommandWaiting ;

    /** If a Cmd is already processed by the OMX coponent, caller thread should wait
     *  till that command completes before sending another command */
    volatile t_los_sem_id mwaitingForCmdToComplete;
    /** Lock to modify cmd related variables */
    volatile t_los_mutex_id mWrapperMutex_id;

    /** Semaphore to hold client thread on SendCommand waiting for ENS response */
    volatile t_los_sem_id mWaitingForCmdReceived;
    volatile unsigned int mNbCmdReceivedToWait;

    /** The message queue members to decouple call to IL Client
        CB to clients are posted in this queue and processed when lock released */
    static ENS_Queue mClientCBmessageQueue;
    /** Queue of preallocated ClientCB messages  */
    static ENS_Queue mClientCBfreeMessageQueue;

    /* Queue a CB client message to be executed once lock released */
    static void pushCBMessage(CB_Message *);

    /* The RME wrapper instance */
    static ENS_RME_Wrapper * mRME_Wrapper;
    /** The RM callbacks passed by the component upon initial register to the RME */
    const RM_CBK_T *pcbks;
    /** The ENS wrapper call backs we pass to RME */
    RM_CBK_T *mRMEcbks;

    /** This methods performs the delayed execution of all the posted message
        during the execution of the OSI part no mutex required and thread safe */
    static void processCBMessage();

#ifndef HOST_ONLY
    static void NMF_DSP_ServiceCallback(void *contextHandler,
                                        t_nmf_service_type serviceType,
                                        t_nmf_service_data *serviceData);
#endif

    /** For now we create only one mutex and FIFO for all ENS components
        We may managed domain of execution later */
    static t_nmf_channel mNMF_DSP_CallBackChannel;
    static t_nmf_channel mNMF_SMPEE_CallBackChannel;

    static t_los_sem_id waitingForClientCallbackProcessing_sem;
    static t_los_sem_id waitingForClientCallbackExit_sem;
    static t_los_sem_id waitingForMpcFlushMessage_sem;
    static t_los_sem_id waitingForHostFlushMessage_sem;

    /** The mutex to acquire to execute OSI code */
    static pthread_mutex_t *mMutex_id;
    static volatile t_los_process_id mThread_id_locking;
    static volatile unsigned int     mThread_lock_depth;
    static volatile OMX_COMPONENTTYPE *omxComponentHoldingLock;
    static volatile const char *funcHoldingLock;

    /** The list of active OMX components */
    static omx_cmp_list_head_t omx_cmp_list;
    /** Mutex for omx_cmp_list manipulation */
    static pthread_mutex_t omx_cmp_list_mutex;

    static void OMXlistLock();
    static void OMXlistUnLock();

    /** The counter we'll use to assign a unique id to each component */
    static volatile unsigned long mUniqueIDCounter;
    /** The list structure to register this wrapper instance to omx_cmp_list */
    omx_cmp_list_elem_t list_elem;
    /** The list of OMX component trace sepc */
    static omx_cmp_trace_list_head_t omx_cmp_trace_list;

    /** The NMF graph builder */
    static ENS_WrapperConstruct *graphBuilder;
    static ENSWRAPPER_CALLBACKTYPE mENSWrapperCallback;

    /** Add to OST activation list given component with group */
    static void addOSTtraceFilteringSpec(const char *componentName, OMX_U16 traceEnableMask);

    static void displaySendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd,
                                   OMX_U32 nParam, OMX_PTR pCmdData);

public:

    /** Wrap an OMX component */
    ENS_Wrapper(OMX_COMPONENTTYPE *cmpToWrap,
                OMX_COMPONENTTYPE *omxComponent,
                OMX_STRING cComponentName);

    /** Wrap an ENS component */
    ENS_Wrapper(ENS_IOMX *ensComponent,
                OMX_COMPONENTTYPE *omxComponent,
                OMX_STRING cComponentName);

    ~ENS_Wrapper();

    ENS_IOMX * getENS_Component() { return ensComponent; };
    OMX_COMPONENTTYPE * getOMX_Component() { return omxComponent; };
    unsigned int getCommandWaiting() { return mCommandWaiting; } ;
    static inline ENS_WrapperConstruct * getWrapperConstruct() { return graphBuilder; };

    /** Code to be executed by wrapper before calling wrapped function */
    static void Prologue(OMX_COMPONENTTYPE *handle, const char *func);
    /** Code to be executed by wrapper after calling wrapped function */
    static void Epilogue(OMX_COMPONENTTYPE *handle, const char *func);

    static CB_Message * popPreAllocatedCBMessage();
    static void pushPreAllocatedCBMessage(CB_Message *);

    /** The cpu ID of the platform we are running on */
    static ENS_ASIC_ID ASIC_ID;

    /** Compute the CPU id for this platform */
    static void computeASIC_ID();

    /** Process the OSTtrace filtering specification */
    static void processOSTtraceFilteringSpec(int enable, int mpc_trace_mode);

    /** Activate the OSTtrace on active components based on filtering specification */
    static void activateOSTtraceFilteringSpec(int mpc_trace_mode);

    /** Cleanup the OSTtrace filtering specification */
    static void cleanupOSTtraceFilteringSpec(int mpc_trace_mode);

#ifndef HOST_ONLY // don't need this interface when no MPC

    static t_cm_error cm_bindComponentToUser(OMX_HANDLETYPE hComp,
                                             t_cm_instance_handle component,
                                             const char *itfname,
                                             NMF::InterfaceDescriptor *itfref,
                                             t_uint32 fifosize);

    static t_cm_error cm_unbindComponentToUser(OMX_HANDLETYPE hComp,
                                               const t_cm_instance_handle client,
                                               const char* requiredItfClientName);

#endif //HOST_ONLY

    t_nmf_error bindToUser(NMF::Composite *component,
                           const char *itfname,
                           NMF::InterfaceDescriptor *itfref,
                           t_uint32 fifosize);

    t_nmf_error unbindToUser(NMF::Composite *component,
                             const char *itfname);

    /** Get debug and trace verbosity level from environment  */
    static void updateTraceLevel(OMX_COMPONENTTYPE *handle);
    /** Update NMF trace level */
    static void updateNMFTraceLevel(int arm, int sia, int sva, int cm_level);

    /** Initialize wrapper OMX related services */
    static OMX_ERRORTYPE initOMXWrapperServices();

    /** Initialize wrapper ENS related services */
    static OMX_ERRORTYPE initENSWrapperServices();

    /** Initialize wrapper NMF related services */
    static OMX_ERRORTYPE initNMFWrapperServices();

    /** Stop wrapper OMX related services if there is no more OMX and ENS components */
    static OMX_ERRORTYPE stopOMXWrapperServices();

    /** Stop wrapper ENS related services if there is no more ENS components */
    static OMX_ERRORTYPE stopENSWrapperServices();

    /** Stop wrapper NMF related services */
    static OMX_ERRORTYPE stopNMFWrapperServices();

#ifndef HOST_ONLY
    static void NMF_DSP_CallBackThread(void *);
#endif
    static void NMF_SMPEE_CallBackThread(void * args);
    static void ENS_CallBackToClientThread(void * args);

    /** Debug trace enablers */
    static int OMX_trace_level;
    static int DEBUG_trace_enabled;
    static int RME_trace_enabled;
    static int NMF_trace_level;
    static int OST_trace_enabled;
    static int OST_trace_mpc_mode;
    static unsigned int OST_trace_default_mask;

    static int NMF_STM_ARM_trace_level;
    static int NMF_STM_SIA_trace_level;
    static int NMF_STM_SVA_trace_level;

    /* Buffer content dump */
    static int OMX_dump_dir; /* 0=dump input, 1=dump output, 2=dump input&output */
    static int OMX_dump_hex; /* 0=binary(default), 1=hexa */
    static int OMX_dump_idx;
    static int OMX_dump_enabled;
    static int mDataDumpEnabled;
    static unsigned long mCurrentDataDumpSize;
    static unsigned long OMX_dump_size;
    static char OMX_dump_path[OMX_MAX_STRINGNAME_SIZE];
    static char OMX_trace_name[OMX_MAX_STRINGNAME_SIZE]; /* Name of component to trace */
    static char OMX_dump_name[OMX_MAX_STRINGNAME_SIZE]; /* Name of component's buffers to dump */
    static char *OMX_dump_input_buffer;
    static char *OMX_dump_output_buffer;
    static size_t OMX_dump_input_buffer_size;
    static size_t OMX_dump_output_buffer_size;

    RM_CBK_T * getRMEwrappercbks();
    const RM_CBK_T * getRMEcbks();
    void setRMEcbks(const RM_CBK_T * cbks);

    /** The unique ID of this component instance  */
    unsigned long mID;

    /** If true then component's activity will be traced */
    bool traceActivity;
    /** If true then component's buffers content will be traced */
    bool dumpBuffer;

    /** The context tracing object of the instance */
    ENS_Wrapper_CtxTrace *ctxTrace;

    /** Return the OMX_COMPONENTTYPE corresponding to the requested id
     * NULL is returned in case no such matching ID */
    static OMX_COMPONENTTYPE * GetOMX_COMPONENTTYPEforID(unsigned long id);

    /** Return the OMX_COMPONENTTYPE matching the index in the list of active component
     * NULL is returned in case no such matching index in the list of active component */
    static OMX_COMPONENTTYPE * GetOMX_COMPONENTTYPEforIndex(unsigned long index);

    /** Return the OMX_COMPONENTTYPE of the wrapper wrapping provided handle
     * NULL is returned in case no such matching wrapped handle */
    static OMX_COMPONENTTYPE * GetComponentWrapperForHandle(OMX_HANDLETYPE hComponent);

    /** Remove from list of active components the provided handle */
    static void RemoveHandleFromActiveList(OMX_COMPONENTTYPE *);

    /** Print the list of active components */
    static void PrintComponentList(char *buffer = NULL, size_t size = 0);

    /** Request OST trace activation on this component based on the filtering spec */
    OMX_ERRORTYPE ActivateOSTtrace(int);

    /** Dump buffer's data in binary format on this component */
    void dumpData(OMX_BUFFERHEADERTYPE* pBuffer, char *portType, char *path, int portIndex, unsigned long size, const char *comp_name, OMX_HANDLETYPE hComponent);

    /** Return true in case OMX_COMPONENTTYPE is still active */
    static bool IsComponentActive(OMX_COMPONENTTYPE *);

    /** Return the number of active components */
    static int NbActiveComponents(bool onlyENScomponents = false);

    /**********************************************************************************/

    /*
     * The OMX components interfaces the OMX IL client will call
     * The wrapper will return the following functions as component entry point
     */

    static OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent,
                                             OMX_STRING pComponentName,
                                             OMX_VERSIONTYPE* pComponentVersion,
                                             OMX_VERSIONTYPE* pSpecVersion,
                                             OMX_UUIDTYPE* pComponentUUID);

    static OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent,
                                     OMX_COMMANDTYPE Cmd,
                                     OMX_U32 nParam,
                                     OMX_PTR pCmdData);

    static OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent,
                                      OMX_INDEXTYPE nIndex,
                                      OMX_PTR pComponentParameterStructure);

    static OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent,
                                      OMX_INDEXTYPE nIndex,
                                      OMX_PTR pComponentParameterStructure);

    static OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent,
                                   OMX_INDEXTYPE nIndex,
                                   OMX_PTR pComponentConfigStructure);

    static OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent,
                                   OMX_INDEXTYPE nIndex,
                                   OMX_PTR pComponentConfigStructure);

    static OMX_ERRORTYPE GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                           OMX_STRING cParameterName,
                                           OMX_INDEXTYPE* pIndexType);

    static OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent,
                                  OMX_STATETYPE* pState);

    static OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComponent,
                                                OMX_U32 nPort,
                                                OMX_HANDLETYPE hTunneledComp,
                                                OMX_U32 nTunneledPort,
                                                OMX_TUNNELSETUPTYPE* pTunnelSetup);

    static OMX_ERRORTYPE UseBuffer(OMX_HANDLETYPE hComponent,
                                   OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                   OMX_U32 nPortIndex,
                                   OMX_PTR pAppPrivate,
                                   OMX_U32 nSizeBytes,
                                   OMX_U8* pBuffer);

    static OMX_ERRORTYPE AllocateBuffer(OMX_HANDLETYPE hComponent,
                                        OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                        OMX_U32 nPortIndex,
                                        OMX_PTR pAppPrivate,
                                        OMX_U32 nSizeBytes);

    static OMX_ERRORTYPE FreeBuffer(OMX_HANDLETYPE hComponent,
                                    OMX_U32 nPortIndex,
                                    OMX_BUFFERHEADERTYPE* pBuffer);

    static OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent,
                                         OMX_BUFFERHEADERTYPE* pBuffer);

    static OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent,
                                        OMX_BUFFERHEADERTYPE* pBuffer);

    static OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent,
                                      OMX_CALLBACKTYPE* pCallbacks,
                                      OMX_PTR pAppData);

    static OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent);

    static OMX_ERRORTYPE UseEGLImage(OMX_HANDLETYPE hComponent,
                                     OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                     OMX_U32 nPortIndex,
                                     OMX_PTR pAppPrivate,
                                     void* eglImage);

    static OMX_ERRORTYPE ComponentRoleEnum(OMX_HANDLETYPE hComponent,
                                           OMX_U8 *cRole,
                                           OMX_U32 nIndex);

    /**
     * The OMX IL client call backs passed to the OSI components
     */

    static OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                                      OMX_IN OMX_PTR pAppData,
                                      OMX_IN OMX_EVENTTYPE eEvent,
                                      OMX_IN OMX_U32 nData1,
                                      OMX_IN OMX_U32 nData2,
                                      OMX_IN OMX_PTR pEventData);

    static OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                         OMX_IN OMX_PTR pAppData,
                                         OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

    static OMX_ERRORTYPE FillBufferDone(OMX_OUT OMX_HANDLETYPE hComponent,
                                        OMX_OUT OMX_PTR pAppData,
                                        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);

    /********************************************************************************/

    /**
     * The callbacks passed to the RME in place of the components ones
     */

    static RM_STATUS_E RME2CMP_Get_Capabilities(OMX_IN const OMX_PTR pCompHdl,
                                                OMX_OUT RM_CAPABILITIES_T* pCapabilities);


    static RM_STATUS_E RME2CMP_Get_ResourcesEstimation(OMX_IN const OMX_PTR pCompHdl,
                                                       OMX_INOUT RM_EMDATA_T* pEstimationData);

    static void RME2CMP_Notify_Error(OMX_IN const OMX_PTR pCompHdl,
                                     OMX_IN RM_STATUS_E nError);
    static void postRME2CMP_Notify_Error(OMX_IN const OMX_PTR pCompHdl,
                                         OMX_IN RM_STATUS_E nError);

    static void RME2CMP_Notify_ResReservationProcessed(OMX_IN const OMX_PTR pCompHdl,
                                                       OMX_IN OMX_BOOL bResourcesReservGranted);
    static void postRME2CMP_Notify_ResReservationProcessed(OMX_IN const OMX_PTR pCompHdl,
                                                           OMX_IN OMX_BOOL bResourcesReservGranted);

    static void RME2CMP_Cmd_SetRMConfig(OMX_IN const OMX_PTR pCompHdl,
                                        OMX_IN const RM_SYSCTL_T* pRMcfg);
    static void postRME2CMP_Cmd_SetRMConfig(OMX_IN const OMX_PTR pCompHdl,
                                            OMX_IN const RM_SYSCTL_T* pRMcfg);

    static void RME2CMP_Cmd_ReleaseResource(OMX_IN const OMX_PTR pCompHdl,
                                            OMX_IN OMX_BOOL bSuspend);
    static void postRME2CMP_Cmd_ReleaseResource(OMX_IN const OMX_PTR pCompHdl,
                                                OMX_IN OMX_BOOL bSuspend);
    //Dump the Context
    static void Dump(int action, const char* reason,ENS_Wrapper_p ensWrapperPointer = NULL);
};

#ifndef HOST_ONLY // don't need this when no MPC

///Structure used to store client context and wrapper mutex id
///Used for messages from DSP to ARM.
class ENSWrapperCtx {
public:
    /// The itfref to be called during a MPC to Host communication.
    t_nmf_interface_desc *mItfref;

    // The OMX component it is assocaited to
    OMX_COMPONENTTYPE *mHandle;

    // Constructor
    ENSWrapperCtx(t_nmf_interface_desc *itfref, OMX_COMPONENTTYPE *handle) :
    mItfref(itfref), mHandle(handle) { }

    virtual ~ENSWrapperCtx(void) { }
};
#endif // HOST_ONLY

class ENSHostWrapperCtx {
public:
    /// Pointer to the wrapper instance
    ENS_Wrapper_p mWrapper;
    /// The itfref to be called during a MPC to Host communication.
    NMF::InterfaceDescriptor *mItfref;

    // Constructor
    ENSHostWrapperCtx(ENS_Wrapper_p aWrapper, NMF::InterfaceDescriptor *itfref) :
    mWrapper(aWrapper), mItfref(itfref) { }

    virtual ~ENSHostWrapperCtx(void) { }
};

#endif /* _ENS_WRAPPER_H_ */

