/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/select.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/prctl.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <osttrace.h>

#include "OMX_Wrapper.h"
#include "ENS_WrapperLog.h"

#define NB_CLIENT_CB_ALLOCATED_MESSAGES 1024
static CB_Message mCBMessageArray[NB_CLIENT_CB_ALLOCATED_MESSAGES];

t_los_sem_id ENS_Wrapper::waitingForClientCallbackProcessing_sem;
t_los_sem_id ENS_Wrapper::waitingForClientCallbackExit_sem;
pthread_mutex_t *ENS_Wrapper::mMutex_id;
pthread_mutex_t ENS_Wrapper::omx_cmp_list_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
volatile t_los_process_id ENS_Wrapper::mThread_id_locking;
volatile unsigned int ENS_Wrapper::mThread_lock_depth;
volatile OMX_COMPONENTTYPE *ENS_Wrapper::omxComponentHoldingLock;
volatile const char *ENS_Wrapper::funcHoldingLock;

ENS_Queue ENS_Wrapper::mClientCBmessageQueue = ENS_Queue(NB_CLIENT_CB_ALLOCATED_MESSAGES);
ENS_Queue ENS_Wrapper::mClientCBfreeMessageQueue = ENS_Queue(NB_CLIENT_CB_ALLOCATED_MESSAGES);

int ENS_Wrapper::OMX_trace_level = 0;
int ENS_Wrapper::DEBUG_trace_enabled = 0;
int ENS_Wrapper::RME_trace_enabled = 0;
int ENS_Wrapper::NMF_trace_level = 0;
int ENS_Wrapper::OST_trace_enabled = 0;
int ENS_Wrapper::OST_trace_mpc_mode = 0;
unsigned int ENS_Wrapper::OST_trace_default_mask = 0;

int ENS_Wrapper::OMX_dump_hex = 0; //0=binary(default), 1=hexa
int ENS_Wrapper::OMX_dump_dir = 0; //Dump input by default
unsigned long ENS_Wrapper::OMX_dump_size = 0;
int ENS_Wrapper::OMX_dump_idx;
int ENS_Wrapper::OMX_dump_enabled = 0;
char ENS_Wrapper::OMX_dump_path[OMX_MAX_STRINGNAME_SIZE] = "";
char ENS_Wrapper::OMX_trace_name[OMX_MAX_STRINGNAME_SIZE] = "";
char ENS_Wrapper::OMX_dump_name[OMX_MAX_STRINGNAME_SIZE] = "";
char *ENS_Wrapper::OMX_dump_input_buffer = NULL;
char *ENS_Wrapper::OMX_dump_output_buffer = NULL;
size_t ENS_Wrapper::OMX_dump_input_buffer_size = 0;
size_t ENS_Wrapper::OMX_dump_output_buffer_size = 0;

omx_cmp_list_head_t ENS_Wrapper::omx_cmp_list;
// Must NOT start with ID 0 as RME is checking for NULL pointer
volatile unsigned long ENS_Wrapper::mUniqueIDCounter = 1;

ENS_ASIC_ID ENS_Wrapper::ASIC_ID = ENS_ASIC_ID_INVALID;

ENS_WrapperConstruct *ENS_Wrapper::graphBuilder = NULL;
ENSWRAPPER_CALLBACKTYPE ENS_Wrapper::mENSWrapperCallback = { ConstructENSProcessingPartAsync, NULL};

/********************************************************************************
 * Wrapper property names
 ********************************************************************************/

#ifdef ANDROID
#define OMX_TRACE_LEVEL_PROPERTY_NAME "ste.omx.wrapper.trace"
#define DEBUG_TRACE_ENABLED_PROPERTY_NAME "ste.omx.wrapper.debug"
#define RME_TRACE_ENABLED_PROPERTY_NAME "ste.omx.wrapper.rme"
#define NMF_TRACE_LEVEL_PROPERTY_NAME "ste.omx.wrapper.NMF.CM"
#define NMF_STM_ARM_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.arm.trace"
#define NMF_STM_SIA_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.sia.trace"
#define NMF_STM_SVA_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.sva.trace"
#define STE_OMX_TRACE_PROPERTY_NAME "ste.omx.trace"
#define STE_OMX_TRACE_DEFAULT_MASK_PROPERTY_NAME "ste.omx.trace.default.mask"
#define OMX_DUMP_SIZE_PROPERTY_NAME "ste.omx.dump.size"
#define OMX_DUMP_FORMAT_PROPERTY_NAME "ste.omx.dump.hex"
#define OMX_DUMP_OUTPUT_PROPERTY_NAME "ste.omx.dump.dir"
#define OMX_DUMP_INDEX_PROPERTY_NAME "ste.omx.dump.idx"
#define OMX_DUMP_NAME_PROPERTY_NAME "ste.omx.dump.name"
#define OMX_DUMP_PATH_PROPERTY_NAME "ste.omx.dump.path"
#define OMX_TRACE_NAME_PROPERTY_NAME  "ste.omx.trace.name"
#define LIBC_DEBUG_MALLOC_PROPERTY_NAME "libc.debug.malloc"
#define STE_OST_MPC_TRACE_TOSTM_PROPERTY_NAME "ste.ost.mpc.trace.tostm"
#define STE_OMX_CTX_PROPERTY_NAME "ste.omx.ctx"
#define STE_OMX_CTX_DEBUG_PROPERTY_NAME "ste.omx.ctx.debug"
#define STE_OMX_CTX_HISTORY_DEPTH "ste.omx.ctx.history.depth"
#else
// The shell identifier compliant properties names
#define OMX_TRACE_LEVEL_PROPERTY_NAME "ste_omx_wrapper_trace"
#define DEBUG_TRACE_ENABLED_PROPERTY_NAME "ste_omx_wrapper_debug"
#define RME_TRACE_ENABLED_PROPERTY_NAME "ste_omx_wrapper_rme"
#define NMF_TRACE_LEVEL_PROPERTY_NAME "ste_omx_wrapper_NMF_CM"
#define NMF_STM_ARM_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_arm_trace"
#define NMF_STM_SIA_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_sia_trace"
#define NMF_STM_SVA_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_sva_trace"
#define STE_OMX_TRACE_PROPERTY_NAME "ste_omx_trace"
#define STE_OMX_TRACE_DEFAULT_MASK_PROPERTY_NAME "ste_omx_trace_default_mask"
#define OMX_DUMP_SIZE_PROPERTY_NAME "ste_omx_dump_size"
#define OMX_DUMP_FORMAT_PROPERTY_NAME "ste.omx.dump.hex"
#define OMX_DUMP_OUTPUT_PROPERTY_NAME "ste_omx_dump_dir"
#define OMX_DUMP_INDEX_PROPERTY_NAME "ste_omx_dump_idx"
#define OMX_DUMP_NAME_PROPERTY_NAME  "ste_omx_dump_name"
#define OMX_DUMP_PATH_PROPERTY_NAME "ste_omx_dump_path"
#define OMX_TRACE_NAME_PROPERTY_NAME  "ste_omx_trace_name"
#define LIBC_DEBUG_MALLOC_PROPERTY_NAME "libc_debug_malloc"
#define STE_OST_MPC_TRACE_TOSTM_PROPERTY_NAME "ste_ost_mpc_trace_tostm"
#define STE_OMX_CTX_PROPERTY_NAME "ste_omx_ctx"
#define STE_OMX_CTX_DEBUG_PROPERTY_NAME "ste_omx_ctx_debug"
#define STE_OMX_CTX_HISTORY_DEPTH "ste_omx_ctx_history_depth"
#endif

void ENS_Wrapper::Init()
{
    // Start wrapper services if not yet started
    if (ensComponent) {
        initENSWrapperServices();
    } else {
        initOMXWrapperServices();
    }

    // Store this wrapper instance
    omxComponent->pComponentPrivate      = this;
    omxComponent->GetComponentVersion    = GetComponentVersion;
    omxComponent->SendCommand            = SendCommand;
    omxComponent->GetParameter           = GetParameter;
    omxComponent->SetParameter           = SetParameter;
    omxComponent->GetConfig              = GetConfig;
    omxComponent->SetConfig              = SetConfig;
    omxComponent->GetExtensionIndex      = GetExtensionIndex;
    omxComponent->GetState               = GetState;

    omxComponent->ComponentTunnelRequest = ComponentTunnelRequest;
    omxComponent->UseBuffer              = UseBuffer;
    omxComponent->AllocateBuffer         = AllocateBuffer;
    omxComponent->FreeBuffer             = FreeBuffer;
    omxComponent->EmptyThisBuffer        = EmptyThisBuffer;
    omxComponent->FillThisBuffer         = FillThisBuffer;

    omxComponent->SetCallbacks           = SetCallbacks;
    omxComponent->ComponentDeInit        = ComponentDeInit;

    omxComponent->UseEGLImage            = UseEGLImage;
    omxComponent->ComponentRoleEnum      = ComponentRoleEnum;

    // The component OMX IL Client call backs we'll pass to OSI component
    mOMXWrapperCB->EventHandler    = EventHandler;
    mOMXWrapperCB->EmptyBufferDone = EmptyBufferDone;
    mOMXWrapperCB->FillBufferDone  = FillBufferDone;

    if (ensComponent) {
        mWaitingForCmdReceived = LOS_SemaphoreCreate(0);
#ifdef EXECUTE_COMMANDS_SEQUENTIALLY
        mCmdBeingProcessed = OMX_FALSE;
        mCommandWaiting = 0;
        mwaitingForCmdToComplete = LOS_SemaphoreCreate(0);
        mWrapperMutex_id = LOS_MutexCreate();
#endif
    } else {
        mWaitingForCmdReceived = 0;
    }
    mNbCmdReceivedToWait = 0;

    mOMXClientCB = NULL;
    pcbks = NULL;

    list_elem.omx_component = omxComponent;

    OMXlistLock();
    mID = mUniqueIDCounter++;
    LIST_INSERT_HEAD(&omx_cmp_list, &list_elem, list_entry);
    OMXlistUnLock();
}

void ENS_Wrapper::InitLog()
{
    // We update trace level on every component creation/destruction in case property has been set
    updateTraceLevel(omxComponent);

    if (ENS_Wrapper_CtxTrace::enabled) {
        ctxTrace = new ENS_Wrapper_CtxTrace(this);
    }

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(omxComponent)) {
        WRAPPER_OMX_LOG(omxComponent, "GetHandle " OMX_COMPONENT_DEF_STR " pAppData=0x%08x",
                        OMX_COMPONENT_DEF_ARG(omxComponent),
                        (unsigned int) omxComponent->pApplicationPrivate);
    } else {
        WRAPPER_OMX_LOG(omxComponent, "GetHandle returning to client "
                        OMX_COMPONENT_DEF_STR " wrapping " OMX_COMPONENT_DEF_STR " pAppData=0x%08x",
                        OMX_COMPONENT_DEF_ARG(omxComponent),
                        OMX_COMPONENT_DEF_ARG_STR("OMX component",
                                                  OMX_HANDLETYPE_TO_OMXCOMPONENT(omxComponent)),
                        (unsigned int) omxComponent->pApplicationPrivate);
    }

    if (OMX_WRAPPER_LOG_LEVEL) {
        PrintComponentList();
    }
}

void memStatus(void);

static void sigAction(int signum, siginfo* info, void*)
{
#ifdef ANDROID
    memStatus();
#else
    ALOGE("Memory leak tracing not implemented");
#endif
}

extern "C" {
    // OMX Clients call back thread
    void ENS_CallBackToClientThread(void * args) {
        ENS_Wrapper::ENS_CallBackToClientThread(args);
    }
}

static bool ENSWrapperServicesInitDone = 0;
static bool OMXWrapperServicesInitDone = 0;
// use a recursive mutex as we have OMX and ENS layers
static pthread_mutex_t init_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

#define INIT_LOCK pthread_mutex_lock(&init_mutex)
#define INIT_UNLOCK pthread_mutex_unlock(&init_mutex)

OMX_ERRORTYPE ENS_Wrapper::initOMXWrapperServices()
{
    INIT_LOCK;
    if (OMXWrapperServicesInitDone) {
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }

    ALOGI("initOMXWrapperServices");

    // Init OMX cmp list as emtpy
    LIST_INIT(&omx_cmp_list);

    // Compute the ASIC ID
    computeASIC_ID();
    ALOGI("ASIC_ID=%d", (int) ENS_Wrapper::ASIC_ID);

    int prop_value;
    GET_PROPERTY(LIBC_DEBUG_MALLOC_PROPERTY_NAME, value, "-1");
    prop_value = atoi(value);
    if(prop_value == 1) {
        // Register a signal handler to trig memory leaks dump when requested
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = sigAction;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGUSR2, &sa, NULL);
    }
    {
        GET_PROPERTY(STE_OMX_CTX_DEBUG_PROPERTY_NAME, value, "0");
        ENS_Wrapper_CtxTrace::debug_enabled = atoi(value);
    }

    {
        GET_PROPERTY(STE_OMX_CTX_HISTORY_DEPTH, value, "5");
        ENS_Wrapper_CtxTrace::history_depth = atoi(value);
    }
    {
        GET_PROPERTY(STE_OMX_CTX_PROPERTY_NAME, value, "1");
        ENS_Wrapper_CtxTrace::enabled = atoi(value) != 0;
    }

    OMXWrapperServicesInitDone = true;
    ALOGI("initOMXWrapperServices Done");

    INIT_UNLOCK;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Wrapper::initENSWrapperServices()
{
    INIT_LOCK;
    OMX_ERRORTYPE init_error = initOMXWrapperServices();
    if (init_error != OMX_ErrorNone) {
        INIT_UNLOCK;
        return init_error;
    }
    if (ENSWrapperServicesInitDone) {
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }
    ALOGI("initENSWrapperServices");
    // Create the mutex : we have ONE mutex for ALL wrappers.
    mMutex_id = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if (!mMutex_id) {
        ALOGE("initENSWrapperServices: Error: failed to allocate wrapper lock");
        INIT_UNLOCK;
        return OMX_ErrorInsufficientResources;
    }
    pthread_mutex_init(mMutex_id, NULL);

    mThread_id_locking = ENS_WRAPPER_INVALID_TID;
    mThread_lock_depth = 0;
    omxComponentHoldingLock = NULL;

    // Init OMX cmp trace list as emtpy
    LIST_INIT(&omx_cmp_trace_list);

    // Init the CB client queues
    if (mClientCBfreeMessageQueue.nbElem() == 0) {
        for (int i = 0 ; i < NB_CLIENT_CB_ALLOCATED_MESSAGES; i++) {
            pushPreAllocatedCBMessage(&(mCBMessageArray[i]));
        }
    }

    waitingForClientCallbackProcessing_sem = LOS_SemaphoreCreate(0);
    waitingForClientCallbackExit_sem = LOS_SemaphoreCreate(0);

    graphBuilder = new ENS_WrapperConstruct();

    // Start client CB thread
    LOS_ThreadCreate(ENS_CallBackToClientThread, NULL, 0,
                     LOS_USER_URGENT_PRIORITY, "ENS-OMX-UserCB");

    // OST Trace setup
    OSTTraceInit();

    // Update the trace level based on environment properties
    WRAPPER_LOCK(ENS_WRAPPER_UPDATETRACELEVEL_HANDLE_ID);
    updateTraceLevel(ENS_WRAPPER_UPDATETRACELEVEL_HANDLE_ID);
    WRAPPER_UNLOCK(ENS_WRAPPER_UPDATETRACELEVEL_HANDLE_ID);

    init_error = initNMFWrapperServices();
    if (init_error != OMX_ErrorNone) {
        INIT_UNLOCK;
        return init_error;
    }

    ENSWrapperServicesInitDone = true;
    ALOGI("initENSWrapperServices Done");
    INIT_UNLOCK;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Wrapper::stopOMXWrapperServices()
{
    INIT_LOCK;
    if (ENS_Wrapper::NbActiveComponents() != 0) {
        // We still have some active components - don't do anything
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }

    if (!OMXWrapperServicesInitDone) {
        // Already stopped
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }

    ALOGI("stopOMXWrapperServices");

    if(OMX_dump_input_buffer)
        free(OMX_dump_input_buffer);

    if(OMX_dump_output_buffer)
        free(OMX_dump_output_buffer);

    OMXWrapperServicesInitDone = false;
    ALOGI("stopOMXWrapperServices Done");
    INIT_UNLOCK;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Wrapper::stopENSWrapperServices()
{
    INIT_LOCK;
    if (ENS_Wrapper::NbActiveComponents(true) != 0) {
        // We still have some active ENS components - don't do anything
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }

    OMX_ERRORTYPE error = stopNMFWrapperServices();
    if (error != OMX_ErrorNone) {
        INIT_UNLOCK;
        return error;
    }

    if (!ENSWrapperServicesInitDone) {
        // Already stopped
        INIT_UNLOCK;
        return OMX_ErrorNone;
    }

    ALOGI("stopENSWrapperServices");

    // Send the CBthread exit message to exit ENS_CallBackToClientThread
    CB_Message *message = ENS_Wrapper::popPreAllocatedCBMessage();
    message->type = CB_thread_exit;
    message->hComponent = NULL;
    pushCBMessage(message);
    // Wait for ENS_CallBackToClientThread to exit
    LOS_SemaphoreWait(waitingForClientCallbackExit_sem);

    delete graphBuilder;

    cleanupOSTtraceFilteringSpec(OST_trace_mpc_mode);

    free(mMutex_id);

    LOS_SemaphoreDestroy(waitingForClientCallbackProcessing_sem);
    LOS_SemaphoreDestroy(waitingForClientCallbackExit_sem);

    int tmpNbElem = mClientCBfreeMessageQueue.nbElem();
    if (tmpNbElem != NB_CLIENT_CB_ALLOCATED_MESSAGES) {
        ALOGE("stopENSWrapperServices: mClientCBfreeMessageQueue is not full : %d != %d",
             tmpNbElem, NB_CLIENT_CB_ALLOCATED_MESSAGES);
    }

    tmpNbElem = mClientCBmessageQueue.nbElem();
    if (tmpNbElem) {
        ALOGE("stopENSWrapperServices: mClientCBmessageQueue is not empty : %d ", tmpNbElem);
    }

    error = stopOMXWrapperServices();
    if (error != OMX_ErrorNone) {
        INIT_UNLOCK;
        return error;
    }
    ENSWrapperServicesInitDone = false;
    ALOGI("stopENSWrapperServices Done");
    INIT_UNLOCK;

    return OMX_ErrorNone;
}

#define PROC_CPUINFO "/proc/cpuinfo"
#define BUFFER_SIZE 1024

void ENS_Wrapper::computeASIC_ID()
{
    char buffer[BUFFER_SIZE];
    FILE * fd = fopen(PROC_CPUINFO, "r");
    int cpu_implementer = -1;
    int cpu_architecture = -1;
    int cpu_variant = -1;
    int cpu_part = -1;

    if(fd == NULL) {
        ALOGE("Failed to open " PROC_CPUINFO);
        return;
    }
    while(fgets(buffer, BUFFER_SIZE, fd) != '\0') {
        /* replace the ':' with ' ' to have "architecture" as token instead of "architecture:" */
        char * tmp = buffer;
        while(*tmp != '\0') {
            if(*tmp == ':')
                *tmp = ' ';
            tmp++;
        }
        /* we want to make sure we are on a DB8500 so read all fields */
        sscanf(buffer, "CPU implementer %i", &cpu_implementer);
        sscanf(buffer, "CPU architecture %i", &cpu_architecture);
        sscanf(buffer, "CPU variant %i", &cpu_variant);
        sscanf(buffer, "CPU part %i", &cpu_part);
    }
    fclose(fd);

    if((cpu_implementer != -1) && (cpu_architecture != -1) &&
       (cpu_variant != -1) && (cpu_part != -1)) {
        ASIC_ID = (ENS_ASIC_ID) cpu_variant;
    }
}

/** Update variables impacting wrapper execution from environment properties */
void ENS_Wrapper::updateTraceLevel(OMX_COMPONENTTYPE *handle)
{
    {
        GET_PROPERTY(OMX_TRACE_LEVEL_PROPERTY_NAME, value, "0");
        OMX_trace_level = atoi(value);
    }
    {
        GET_PROPERTY(DEBUG_TRACE_ENABLED_PROPERTY_NAME, value, "0");
        DEBUG_trace_enabled = atoi(value);
    }
    {
        GET_PROPERTY(RME_TRACE_ENABLED_PROPERTY_NAME, value, "0");
        RME_trace_enabled = atoi(value);
    }
    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(handle)) {
        int arm_prop_value = 0, sia_prop_value = 0, sva_prop_value = 0;
        {
            GET_PROPERTY(NMF_STM_ARM_TRACE_LEVEL_PROPERTY_NAME, value, "0");
            arm_prop_value = atoi(value);
        }
#ifndef HOST_ONLY
        {
            GET_PROPERTY(NMF_TRACE_LEVEL_PROPERTY_NAME, value, "0");
            NMF_trace_level = atoi(value);
        }
        {
            GET_PROPERTY(NMF_STM_SIA_TRACE_LEVEL_PROPERTY_NAME, value, "0");
            sia_prop_value = atoi(value);
        }
        {
            GET_PROPERTY(NMF_STM_SVA_TRACE_LEVEL_PROPERTY_NAME, value, "0");
            sva_prop_value = atoi(value);
        }
#endif // #ifndef HOST_ONLY

        updateNMFTraceLevel(arm_prop_value, sia_prop_value, sva_prop_value, NMF_trace_level);

        {
            GET_PROPERTY(STE_OMX_TRACE_DEFAULT_MASK_PROPERTY_NAME, value, "0xa3");
            // Defaults to bits 0 [ERROR],1 [WARNING],5 [OMX_API], 7 [ALWAYS]
            OST_trace_default_mask = strtoul(value, NULL, 0);
        }
        {
            GET_PROPERTY(STE_OST_MPC_TRACE_TOSTM_PROPERTY_NAME, value, "0");
            OST_trace_mpc_mode = atoi(value);
        }
        {
            GET_PROPERTY(STE_OMX_TRACE_PROPERTY_NAME, value, "0");

            // Read the OST trace spec file if any
            processOSTtraceFilteringSpec(atoi(value), OST_trace_mpc_mode);
        }
    }
    {
        GET_PROPERTY(OMX_DUMP_NAME_PROPERTY_NAME, value, "");
        strncpy(OMX_dump_name, value, OMX_MAX_STRINGNAME_SIZE);
    }
    if (strlen(OMX_dump_name)) {
        OMX_dump_enabled = 0;
        {   GET_PROPERTY(OMX_DUMP_SIZE_PROPERTY_NAME, value, "0");
            OMX_dump_size = atoi(value);
        }
        {
            GET_PROPERTY(OMX_DUMP_FORMAT_PROPERTY_NAME, value, "0");
            OMX_dump_hex = atoi(value);
        }
        {
            GET_PROPERTY(OMX_DUMP_OUTPUT_PROPERTY_NAME, value, "0");
            OMX_dump_dir = atoi(value);
        }
        {
            GET_PROPERTY(OMX_DUMP_INDEX_PROPERTY_NAME, value, "0");
            OMX_dump_idx = atoi(value);
        }
        {
            GET_PROPERTY(OMX_DUMP_PATH_PROPERTY_NAME, value, "/data/ste-debug/omx/");
            strncpy(OMX_dump_path, value, OMX_MAX_STRINGNAME_SIZE);
            if (mkdir(OMX_dump_path, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                if (errno != EEXIST) {
                    LOGE("mkdir of ' %s ' failed (err=%s), discarding data dump", OMX_dump_path, strerror(errno));
                    goto nodatadump;
                }
            }
        }

        OMX_dump_enabled = 1;
        if (OMX_dump_hex) {
            if ((!OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle)) && (strcmp(OMX_dump_name, OMX_HANDLETYPE_GET_NAME(handle)) == 0)) {
                LOGI("OMX buffer dump in hexa format enabled on %s of %s for %d bytes dumped per buffer, output in logcat ",
                    OMX_dump_dir ? "output" : "input", OMX_dump_name, OMX_dump_size ? OMX_dump_size :OMX_BUFFERDATA_NB_END_BYTES*2);
            }
        } else {
            char str[64] ;
            if ((!OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle)) && (strcmp(OMX_dump_name, OMX_HANDLETYPE_GET_NAME(handle)) == 0)) {
                sprintf(str, "%s%s_%s_%d_0x%08x.bin", OMX_dump_path, OMX_dump_name, OMX_dump_dir ?"OUT":"IN", OMX_dump_idx, handle);
                LOGI("OMX binary buffer dump enabled on port index %d of %s port of %s ,%s%d%s,output in file %s",
                     OMX_dump_idx,
                     OMX_dump_dir ? "output" : "input",
                     OMX_dump_name,
                     OMX_dump_size ? "dump will stop after " : "",
                     OMX_dump_size,
                     OMX_dump_size ? " bytes " : "",
                     str);
            }
        }
    }
    nodatadump:
    {
        GET_PROPERTY(OMX_TRACE_NAME_PROPERTY_NAME, value, "");
        strncpy(OMX_trace_name, value, OMX_MAX_STRINGNAME_SIZE);
    }
    if (!OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle)) {
        if (OMX_dump_name[0] == '\0') {
            OMX_HANDLETYPE_TO_WRAPPER(handle)->dumpBuffer = true;
        } else {
            if (strcmp(OMX_dump_name, OMX_HANDLETYPE_GET_NAME(handle))) {
                OMX_HANDLETYPE_TO_WRAPPER(handle)->dumpBuffer = false;
            } else {
                OMX_HANDLETYPE_TO_WRAPPER(handle)->dumpBuffer = true;
            }
        }
        if (OMX_trace_name[0] == '\0') {
            OMX_HANDLETYPE_TO_WRAPPER(handle)->traceActivity = true;
        } else {
            if (strcmp(OMX_trace_name, OMX_HANDLETYPE_GET_NAME(handle))) {
                OMX_HANDLETYPE_TO_WRAPPER(handle)->traceActivity = false;
            } else {
                OMX_HANDLETYPE_TO_WRAPPER(handle)->traceActivity = true;
            }
        }
    }
}
