/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "OMX_Wrapper.h"
#include "ENS_WrapperLog.h"
#include "ENS_Wrapper.h"
#include <ENS_DBC.h>

#include <osttrace.h>

#include "mmhwbuffer_ext.h"
#include <OMX_debug.h>

#ifdef ANDROID
#include "media/hardware/HardwareAPI.h"
using namespace android;
#endif

// Timeout in seconds reporting wrapper global lock acquisition failure
#define WRAPPER_LOCK_TIMEOUT_SEC 3
// Timeout in seconds reporting non matching ENS Command received events
#define WRAPPER_CMD_RECEIVED_TIMEOUT_SEC 4
// Timeout in seconds reporting wrapper OMX list lock acquisition failure
#define WRAPPER_OMX_LIST_LOCK_TIMEOUT_SEC 1

/********************************************************************************
 * OMX wrapper
 ********************************************************************************/

extern "C" {
    OMX_COMPONENTTYPE * OMX_Wrapper_create(OMX_COMPONENTTYPE *comp, OMX_STRING cComponentName) {
        OMX_COMPONENTTYPE *wrapper = (OMX_COMPONENTTYPE *) calloc(1, sizeof(OMX_COMPONENTTYPE));
        if (wrapper == NULL) {
            return NULL;
        }
        ENS_Wrapper *enswrapper = new ENS_Wrapper(comp, wrapper, cComponentName);
        if (enswrapper == NULL) {
            free(wrapper);
            return NULL;
        }
        return wrapper;
    }
    void OMX_Wrapper_destroy(OMX_COMPONENTTYPE *comp) {
        delete OMX_HANDLETYPE_TO_WRAPPER(comp);
    }
}

/********************************************************************************
 * Constructors
 ********************************************************************************/

ENS_Wrapper::ENS_Wrapper(OMX_COMPONENTTYPE *cmpToWrap,
                         OMX_COMPONENTTYPE *omxComponent,
                         OMX_STRING cComponentName)
    : ensComponent(NULL),
      wrappedComponent(cmpToWrap),
      omxComponent(omxComponent),
      mOMXWrapperCB(new OMX_CALLBACKTYPE),
      mRMEcbks(NULL),
      ctxTrace(NULL)
{
    strcpy(name, cComponentName);
    Init();
    InitLog();
}

ENS_Wrapper::ENS_Wrapper(ENS_IOMX *ensComponent,
                         OMX_COMPONENTTYPE *omxComponent,
                         OMX_STRING cComponentName)
    : ensComponent(ensComponent),
      wrappedComponent(NULL),
      omxComponent(omxComponent),
      mOMXWrapperCB(new OMX_CALLBACKTYPE),
      mRMEcbks(new RM_CBK_T),
      ctxTrace(NULL)
{
    strcpy(name, cComponentName);
    Init();

    // The component RME callbacks we register to RME
    mRMEcbks->CP_Get_Capabilities               = RME2CMP_Get_Capabilities;
    mRMEcbks->CP_Get_ResourcesEstimation        = RME2CMP_Get_ResourcesEstimation;
    mRMEcbks->CP_Notify_Error                   = postRME2CMP_Notify_Error;
    mRMEcbks->CP_Notify_ResReservationProcessed = postRME2CMP_Notify_ResReservationProcessed;
    mRMEcbks->CP_Cmd_SetRMConfig                = postRME2CMP_Cmd_SetRMConfig;
    mRMEcbks->CP_Cmd_ReleaseResource            = postRME2CMP_Cmd_ReleaseResource;

    InitNMF();

    // We aquire the global lock to update trace and finalize ENS init
    WRAPPER_LOCK(omxComponent);
    InitLog();

    // Register the wrapper callback
    ensComponent->setEnsWrapperCB(&mENSWrapperCallback);

    WRAPPER_UNLOCK(omxComponent);
}

/*
 * Called from:
 * OMX_FreeHandle
 * -> BOSA_STE_DestroyComponent
 *    -> ENS_Wrapper::ComponentDeInit
 *       -> ENS_Component::ComponentDeInit
 *          -> ENS_HwRm::HWRM_Unregister
 *          -> delete ENS_Component
 *    -> ENS_Wrapper::~ENS_Wrapper
 * Or
 * OMX_FreeHandle
 * -> BOSA_ST_DestroyComponent
 *    -> ENS_Wrapper::ComponentDeInit
 *    -> ENS_Wrapper::~ENS_Wrapper
 *
 * !!! We do not delete the ctxTrace object as it is managed by ENS_Wrapper_CtxTrace history !!!
 */
ENS_Wrapper::~ENS_Wrapper()
{
    if (mRMEcbks) {
        delete mRMEcbks;
    }
    delete mOMXWrapperCB;
    if (ENS_Wrapper_CtxTrace::enabled) {
        ENS_Wrapper_CtxTrace::WriteSummary();
    }
    if (wrappedComponent) {
        // Remove compnonent from active list
        RemoveHandleFromActiveList(omxComponent);
        //stopOMXWrapperServices();
    } else {
        LOS_SemaphoreDestroy(mWaitingForCmdReceived);
#ifdef EXECUTE_COMMANDS_SEQUENTIALLY
        LOS_SemaphoreDestroy(mwaitingForCmdToComplete);
        LOS_MutexDestroy(mWrapperMutex_id);
#endif
        //stopENSWrapperServices();
    }
}

/********************************************************************************
 * Global lock implementation
 ********************************************************************************/

#ifndef ANDROID

#include <pthread.h>

static int pthread_mutex_lock_timeout_np(pthread_mutex_t *mutex, unsigned msecs)
{
    struct timespec abstime;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    abstime.tv_sec  = tv.tv_sec + msecs/1000;
    abstime.tv_nsec = tv.tv_usec*1000 + (msecs%1000)*1000000;
    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_sec++;
        abstime.tv_nsec -= 1000000000;
    }
    return pthread_mutex_timedlock(mutex, &abstime);
}

#endif

void ENS_Wrapper::Prologue(OMX_COMPONENTTYPE *handle, const char *func)
{
    // Check if we already hold the lock
    t_los_process_id tid = LOS_GetCurrentId();

    if(mThread_id_locking == tid) {
        // Thread already holds the lock - let's rock
        mThread_lock_depth++;
        if(omxComponentHoldingLock != handle) {
            WRAPPER_DEBUG_LOG("LOCK   - lock depth increased to %d\n"
                              "         by " OMX_COMPONENT_DEF_STR " from '%s'\n"
                              "         while lock holder is " OMX_COMPONENT_DEF_STR " from '%s'",
                              mThread_lock_depth,
                              OMX_COMPONENT_DEF_ARG(handle), func,
                              OMX_COMPONENT_DEF_ARG(omxComponentHoldingLock), funcHoldingLock);
        } else {
            WRAPPER_DEBUG_LOG("LOCK   - lock depth increased to %d by "
                              OMX_COMPONENT_DEF_STR " from '%s'",
                              mThread_lock_depth, OMX_COMPONENT_DEF_ARG(handle), func);
        }
    } else {
        // Use a timed lock to detect soft lockup
        if (pthread_mutex_lock_timeout_np(mMutex_id,
                                          1000 * WRAPPER_LOCK_TIMEOUT_SEC) == 0) {
            mThread_id_locking = tid;
            mThread_lock_depth++;
            omxComponentHoldingLock = handle;
            funcHoldingLock = func;
            WRAPPER_DEBUG_LOG("LOCK   - lock acquired by " OMX_COMPONENT_DEF_STR " from '%s'",
                              OMX_COMPONENT_DEF_ARG(handle), func);
        } else {
            ALOGE("failed to aquire lock within %d seconds for "
                 OMX_COMPONENT_DEF_STR " from '%s'\n"
                 "                        held by tid=%d by "
                 OMX_COMPONENT_DEF_STR " from '%s'",
                 WRAPPER_LOCK_TIMEOUT_SEC,
                 OMX_COMPONENT_DEF_ARG(handle), func,
                 (int) mThread_id_locking,
                 OMX_COMPONENT_DEF_ARG(omxComponentHoldingLock), funcHoldingLock);
            Dump(LOG_OPENED | DUMP_FATAL, "LockTimeOut");
        }
    }
}

void ENS_Wrapper::Epilogue(OMX_COMPONENTTYPE *handle, const char *func)
{
    if (mThread_lock_depth == 0) {
        ALOGE("UNLOCK - called by " OMX_COMPONENT_DEF_STR " while lock not held  - from '%s'",
             OMX_COMPONENT_DEF_ARG(handle), func);
    }
    mThread_lock_depth--;
    if (mThread_lock_depth == 0) {
        if(omxComponentHoldingLock != handle) {
            ALOGE("UNLOCK - lock released by " OMX_COMPONENT_DEF_STR " from '%s'\n"
                 "         != lock holder " OMX_COMPONENT_DEF_STR " - from '%s'",
                 OMX_COMPONENT_DEF_ARG(handle), funcHoldingLock,
                 OMX_COMPONENT_DEF_ARG(omxComponentHoldingLock), func);
        }

        // Then release the lock to allow other threads to enter OMX component
        mThread_id_locking = ENS_WRAPPER_INVALID_TID;
        omxComponentHoldingLock = NULL;

        pthread_mutex_unlock(mMutex_id);
        WRAPPER_DEBUG_LOG("UNLOCK - lock released by " OMX_COMPONENT_DEF_STR " from '%s'",
                          OMX_COMPONENT_DEF_ARG(handle), func);
    } else {
        if ((omxComponentHoldingLock != handle) && ! OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(handle)) {
            WRAPPER_DEBUG_LOG("UNLOCK - lock depth decreased to %d by "
                              OMX_COMPONENT_DEF_STR " from '%s'\n"
                              "         != lock holder " OMX_COMPONENT_DEF_STR " - from '%s'",
                              mThread_lock_depth,
                              OMX_COMPONENT_DEF_ARG(handle), func,
                              OMX_COMPONENT_DEF_ARG(omxComponentHoldingLock), funcHoldingLock);
        } else {
            WRAPPER_DEBUG_LOG("UNLOCK - lock depth decreased to %d by "
                              OMX_COMPONENT_DEF_STR " from '%s'",
                              mThread_lock_depth, OMX_COMPONENT_DEF_ARG(handle), func);
        }
    }
}

bool ENS_Wrapper::IsComponentActive(OMX_COMPONENTTYPE *comphdl)
{
    omx_cmp_list_elem_t *elem = NULL;
    OMXlistLock();
    LIST_FOREACH(elem, &omx_cmp_list, list_entry) {
        if(elem->omx_component == comphdl) {
            OMXlistUnLock();
            return true;
        }
    }
    OMXlistUnLock();
    return false;
}

OMX_COMPONENTTYPE * ENS_Wrapper::GetComponentWrapperForHandle(OMX_HANDLETYPE hComponent)
{
    omx_cmp_list_elem_t *elem = NULL;
    OMXlistLock();
    LIST_FOREACH(elem, &omx_cmp_list, list_entry) {
        if(OMX_HANDLETYPE_TO_OMXCOMPONENT(elem->omx_component) ==
           OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent)) {
            OMXlistUnLock();
            return elem->omx_component;
        }
    }
    OMXlistUnLock();
    return NULL;
}

OMX_COMPONENTTYPE * ENS_Wrapper::GetOMX_COMPONENTTYPEforID(unsigned long id)
{
    omx_cmp_list_elem_t *elem = NULL;
    OMX_COMPONENTTYPE *result = NULL;
    OMXlistLock();
    LIST_FOREACH(elem, &omx_cmp_list, list_entry) {
        if(((ENS_Wrapper_p) elem->omx_component->pComponentPrivate)->mID == id) {
            result = elem->omx_component;
            break;
        }
    }
    OMXlistUnLock();
    return result;
}

OMX_COMPONENTTYPE * ENS_Wrapper::GetOMX_COMPONENTTYPEforIndex(unsigned long index)
{
    omx_cmp_list_elem_t *elem = NULL;
    OMX_COMPONENTTYPE *result = NULL;
    OMXlistLock();
    unsigned long i = 0;
    LIST_FOREACH(elem, &omx_cmp_list, list_entry) {
        if(i == index) {
            result = elem->omx_component;
            break;
        }
        i++;
    }
    OMXlistUnLock();
    return result;
}

void ENS_Wrapper::RemoveHandleFromActiveList(OMX_COMPONENTTYPE *comphdl)
{
    omx_cmp_list_elem_t *iter_elem = NULL;
    int found_elem = 0;
    OMXlistLock();
    LIST_FOREACH(iter_elem, &omx_cmp_list, list_entry) {
        if(iter_elem->omx_component == comphdl) {
            LIST_REMOVE(iter_elem,list_entry);
            found_elem = 1;
            break;
        }
    }
    OMXlistUnLock();
    DBC_ASSERT(found_elem);
    if (OMX_WRAPPER_LOG_LEVEL) {
        PrintComponentList();
    }
}

int ENS_Wrapper::NbActiveComponents(bool onlyENScomponents)
{
    omx_cmp_list_elem_t * iter_elem = NULL;
    int result = 0;
    OMXlistLock();
    LIST_FOREACH(iter_elem, &omx_cmp_list, list_entry) {
        if (! onlyENScomponents) {
            result++;
        } else {
            if (OMX_HANDLETYPE_IS_ENSCOMPONENT(iter_elem->omx_component)) {
                result++;
            }
        }
    }
    OMXlistUnLock();
    return result;
}

static int (*OMX_debug_log)(char *buffer, size_t size, const char *fmt, ...);

#ifdef ANDROID
static int OMX_debug_android_log_print(char *buffer, size_t size, const char *fmt, ...)
{
    va_list ap;
    (void) buffer;
    (void) size;
    va_start(ap, fmt);
    int ret = __android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ap);
    va_end(ap);
    return ret;
}
#endif

static int OMX_debug_sprintf(char *buffer, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(buffer, size, fmt, ap);
    va_end(ap);
    return ret;
}

#define BUFFER (buffer + nb_char), ( size - nb_char > 0 ? size - nb_char : 0)

void ENS_Wrapper::PrintComponentList(char *buffer, size_t size)
{
    omx_cmp_list_elem_t *iter_elem = NULL;
    int nb_char = 0;
    OMX_debug_log = OMX_debug_sprintf;
#ifdef ANDROID
    if (buffer == NULL) { OMX_debug_log = OMX_debug_android_log_print; }
#endif
    OMXlistLock();
    nb_char += OMX_debug_log(BUFFER, "List of active OMX components\n");
    nb_char += OMX_debug_log(BUFFER, "|   ID   |   Handle   |   Wrapped  | CR | Name\n");
    LIST_FOREACH(iter_elem, &omx_cmp_list, list_entry) {
        ENS_Wrapper *wrapper = OMX_HANDLETYPE_TO_WRAPPER(iter_elem->omx_component);
        nb_char += OMX_debug_log(BUFFER, "  %6ld - 0x%08x - 0x%08x - %2ld - %s\n",
                wrapper->mID,
                (unsigned int) iter_elem->omx_component,
                (OMX_HANDLETYPE_IS_ENSCOMPONENT(iter_elem->omx_component) ?
                (unsigned int) OMX_HANDLETYPE_TO_ENSCOMPONENT(iter_elem->omx_component) :
                (unsigned int) OMX_HANDLETYPE_TO_OMXCOMPONENT(iter_elem->omx_component)),
                (unsigned long) wrapper->mNbCmdReceivedToWait,
                OMX_HANDLETYPE_GET_NAME(iter_elem->omx_component));
    }
    OMXlistUnLock();
}

void ENS_Wrapper::OMXlistLock()
{
    if (pthread_mutex_lock_timeout_np(&omx_cmp_list_mutex,
                                      1000 * WRAPPER_OMX_LIST_LOCK_TIMEOUT_SEC) != 0) {
        ALOGE("failed to aquire OMX list lock within %d seconds", WRAPPER_OMX_LIST_LOCK_TIMEOUT_SEC);
        __builtin_trap(); // Force trap to allow service restart and debugging
    }
}

void ENS_Wrapper::OMXlistUnLock()
{
    pthread_mutex_unlock(&omx_cmp_list_mutex);
}

/********************************************************************************
 * Wrapper funtions being called by client to then call ENS code
 ********************************************************************************/

// SetCallbacks wrapper
OMX_ERRORTYPE ENS_Wrapper::SetCallbacks(OMX_HANDLETYPE hComponent,
                                        OMX_CALLBACKTYPE *pCallbacks,
                                        OMX_PTR pAppData)
{
    WRAPPER_OMX_LOG(hComponent, "SetCallbacks " OMX_COMPONENT_DEF_STR
                    ", pCallbacks=0x%08x, pAppData=0x%08x",
                    OMX_COMPONENT_DEF_ARG(hComponent),
                    (unsigned int) pCallbacks, (unsigned int) pAppData);

    // We register those new callbacks to the wrapper
    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB = pCallbacks;

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    // and call the component to update pAppData with wrapper's CB
    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, SetCallbacks,
                                               OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXWrapperCB,
                                               pAppData);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "SetCallbacks", OMX_FALSE, "");

    return result;
}

// ComponentDeInit wrapper
OMX_ERRORTYPE ENS_Wrapper::ComponentDeInit(OMX_HANDLETYPE hComponent)
{
    WRAPPER_OMX_LOG(hComponent, "FreeHandle " OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));

// Save the context of the component in history before freeing it
    if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace){
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace->MoveToHistory();
    }

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)) {
        // Don't hold wrapper lock when calling NMF CM layer !!!
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->DeInitNMF(hComponent);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    // and call the component to update pAppData with wrapper's CB
    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, ComponentDeInit);

    updateTraceLevel(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)) {
        // Upon ComponentDeInit ENSComponent object is ready to be deleted
        delete OMX_HANDLETYPE_TO_ENSCOMPONENT(hComponent);
        OMX_HANDLETYPE_TO_ENSCOMPONENT(hComponent) = NULL;
    }

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "FreeHandle", OMX_FALSE, "");

    return result;
}

static pthread_mutex_t GetComponentVersion_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// GetComponentVersion wrapper
OMX_ERRORTYPE ENS_Wrapper::GetComponentVersion(OMX_HANDLETYPE hComponent,
                                               OMX_STRING pComponentName,
                                               OMX_VERSIONTYPE* pComponentVersion,
                                               OMX_VERSIONTYPE* pSpecVersion,
                                               OMX_UUIDTYPE* pComponentUUID)
{
    WRAPPER_OMX_LOG(hComponent, "GetComponentVersion " OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, GetComponentVersion,
                                               pComponentName,
                                               pComponentVersion,
                                               pSpecVersion,
                                               pComponentUUID);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (WRAPPER_OMX_TRACE(hComponent) || (result != OMX_ErrorNone)) {

        pthread_mutex_lock(&GetComponentVersion_str_mutex);

        static char CV_str[WRAPPER_MAX_DEBUG_BUFFER];
        static char SV_str[WRAPPER_MAX_DEBUG_BUFFER];
        static char CUUID_str[WRAPPER_MAX_DEBUG_BUFFER];

        strOMX_VERSIONTYPE(pComponentVersion, CV_str, WRAPPER_MAX_DEBUG_BUFFER);
        strOMX_VERSIONTYPE(pSpecVersion, SV_str, WRAPPER_MAX_DEBUG_BUFFER);
        strOMX_UUIDTYPE(pComponentUUID, CUUID_str, WRAPPER_MAX_DEBUG_BUFFER);

        WRAPPER_OMX_LOG_RETURN(hComponent, result, "GetComponentVersion", OMX_FALSE,
                               "\n"
                               "      ComponentName=%s\n"
                               "      ComponentVersion=%s\n"
                               "      SpecVersion=%s\n"
                               "      ComponentUUID=%s",
                               pComponentName, CV_str, SV_str, CUUID_str);

        pthread_mutex_unlock(&GetComponentVersion_str_mutex);
    }

    return result;
}

static pthread_mutex_t SendCommand_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// SendCommand wrapper
OMX_ERRORTYPE ENS_Wrapper::SendCommand(OMX_HANDLETYPE hComponent,
                                       OMX_COMMANDTYPE Cmd,
                                       OMX_U32 nParam,
                                       OMX_PTR pCmdData)
{
#ifdef EXECUTE_COMMANDS_SEQUENTIALLY

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)) {

        LOS_MutexLock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);

        if (OMX_TRUE == OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCmdBeingProcessed) {
            OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCommandWaiting++;
            LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);

            WRAPPER_OMX_LOG(hComponent, "EXECUTE_COMMANDS_SEQUENTIALLY: holding incomming SendCommand "
                            "request on " OMX_COMPONENT_DEF_STR, OMX_COMPONENT_DEF_ARG(hComponent));

            LOS_SemaphoreWait(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mwaitingForCmdToComplete);

            WRAPPER_OMX_LOG(hComponent, "EXECUTE_COMMANDS_SEQUENTIALLY: released SendCommand "
                            "request on " OMX_COMPONENT_DEF_STR, OMX_COMPONENT_DEF_ARG(hComponent));

            LOS_MutexLock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
            OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCommandWaiting--;
            LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
        } else {
            if (Cmd != OMX_CommandPortEnable) {
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCmdBeingProcessed = OMX_TRUE;
            }
            LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
        }
    }

#endif // #ifdef EXECUTE_COMMANDS_SEQUENTIALLY

    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&SendCommand_str_mutex);

        static char sendcommand_str[WRAPPER_MAX_DEBUG_BUFFER];
        strOMX_COMMAND(Cmd, nParam, sendcommand_str, WRAPPER_MAX_DEBUG_BUFFER);
        WRAPPER_OMX_LOG(hComponent, "SendCommand " OMX_COMPONENT_DEF_STR " : %s",
                        OMX_COMPONENT_DEF_ARG(hComponent), sendcommand_str);

        pthread_mutex_unlock(&SendCommand_str_mutex);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)) {
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait =
            OMX_HANDLETYPE_TO_ENSCOMPONENT(hComponent)->
            getNbCmdReceivedEventsToWait(Cmd, nParam);
    }

    // Use local value for nbCmdReceivedtoWait and not
    // OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait as we must
    // wait on semaphore even if notified first
    unsigned int nbCmdReceivedtoWait = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait;

    WRAPPER_OMX_LOG(hComponent, "nbCmdReceivedtoWait=%d: SendCommand request on "
                    OMX_COMPONENT_DEF_STR, nbCmdReceivedtoWait, OMX_COMPONENT_DEF_ARG(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, SendCommand,
                                               Cmd, nParam, pCmdData);

    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent)) {

#ifdef EXECUTE_COMMANDS_SEQUENTIALLY

        if (OMX_ErrorNone != result) {
            // In case SendCommand has failed then no call back will be returned
            // so we must release next commands here
            LOS_MutexLock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
            if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCommandWaiting != 0) {
                // We have a SendCommand on hold : release held lock and notify to unblock thread
                LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
                LOS_SemaphoreNotify(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mwaitingForCmdToComplete);
            } else {
                // no waiting thread, just reset property
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCmdBeingProcessed = OMX_FALSE;
                LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
            }
        }

#endif // #ifdef EXECUTE_COMMANDS_SEQUENTIALLY

        /* To ensure correct execution order from client of SendCommand to
           multiple components to change to a state needing information exchange
           between components the ENS layer sends a CmdReceived event to release
           the client thread */
        if (result == OMX_ErrorNone) {
            WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));
            if(nbCmdReceivedtoWait != 0) {
                WRAPPER_OMX_LOG(hComponent, OMX_COMPONENT_DEF_STR " - released lock, "
                                "awaiting %d EventCmdReceived within %ds",
                                OMX_COMPONENT_DEF_ARG(hComponent), nbCmdReceivedtoWait,
                                WRAPPER_CMD_RECEIVED_TIMEOUT_SEC);
                // Expect nbCmdReceivedtoWait back from ENS
                if (LOS_SemaphoreWaitTimeOut(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->
                                             mWaitingForCmdReceived,
                                             WRAPPER_CMD_RECEIVED_TIMEOUT_SEC * 1000)) {
                    ALOGE(OMX_COMPONENT_DEF_STR " - %ds timeout expired missing %d EventCmdReceived",
                         OMX_COMPONENT_DEF_ARG(hComponent), WRAPPER_CMD_RECEIVED_TIMEOUT_SEC,
                         OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait);
                    PrintComponentList();
                    if(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait)
                    result = OMX_ErrorTimeout;
                }
            }
        } else {
            OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait = 0;
            WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));
        }
    }

    // Save the sendCommand if context trace is enabled
    if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace) {
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace->
            SendCommand(Cmd, nParam, pCmdData,result);
    }
    if (OMX_ErrorNone != result) {
        // Trace client request in case of error
        static char sendcommand_str[WRAPPER_MAX_DEBUG_BUFFER];
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SendCommand", OMX_FALSE, "",
            strOMX_COMMAND(Cmd, nParam, sendcommand_str, WRAPPER_MAX_DEBUG_BUFFER));
        // Dump current component status
        Dump(LOG_CURRENT, "SendCommandFailure", OMX_HANDLETYPE_TO_WRAPPER(hComponent));
    } else {
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SendCommand", OMX_FALSE, "");
    }
    return result;
}

static pthread_mutex_t GetParameter_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// GetParameter wrapper
OMX_ERRORTYPE ENS_Wrapper::GetParameter(OMX_HANDLETYPE hComponent,
                                        OMX_INDEXTYPE nIndex,
                                        OMX_PTR pComponentParameterStructure)
{
    WRAPPER_OMX_LOG(hComponent, "GetParameter " OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, GetParameter,
                                               nIndex, pComponentParameterStructure);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&GetParameter_str_mutex);
        static char param_str[WRAPPER_MAX_DEBUG_BUFFER];
        strOMX_INDEXTYPE_struct(nIndex, pComponentParameterStructure,
                                OMX_WRAPPER_LOG_LEVEL, ANDROID_LOG_DEBUG, LOG_TAG, NULL,
                                param_str, WRAPPER_MAX_DEBUG_BUFFER);
#ifndef ANDROID
        WRAPPER_OMX_LOG(hComponent, "%s", param_str);
#endif
        pthread_mutex_unlock(&GetParameter_str_mutex);
    }

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "GetParameter", OMX_FALSE, "");

    return result;
}

#ifdef ANDROID
static OMX_ERRORTYPE ENS_Wrapper_useAndroidNativeBuffer(OMX_HANDLETYPE hComponent,
                                                        OMX_INDEXTYPE nIndex,
                                                        OMX_PTR pComponentParameterStructure)
{
    UseAndroidNativeBufferParams *params =
        static_cast<UseAndroidNativeBufferParams *>(pComponentParameterStructure);

    // SetConfig is to be called as component is no more in Idle state
    OMX_ERRORTYPE result = ENS_Wrapper::SetConfig(hComponent, nIndex, pComponentParameterStructure);

    if(result != OMX_ErrorNone)
        return result;

    // Retreive the logical address of the registered buffer
    OMX_OSI_CONFIG_SHARED_CHUNK_METADATA conf;
    conf.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    conf.nVersion.s.nVersionMajor = 1;
    conf.nVersion.s.nVersionMinor = 0;
    conf.nVersion.s.nRevision     = 0;
    conf.nVersion.s.nStep         = 0;
    conf.nPortIndex = params->nPortIndex;

    result = ENS_Wrapper::GetConfig(hComponent,
                                    (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                                    &conf);

    if(result != OMX_ErrorNone)
        return result;

    // Call UseBuffer for the component's state to update itself
    result = ENS_Wrapper::UseBuffer(hComponent, params->bufferHeader, params->nPortIndex,
                                    params->pAppPrivate, conf.nBufferSize,
                                    (OMX_U8*)conf.nBaseLogicalAddr);

    return result;
}
#endif //ANDROID

static pthread_mutex_t SetParameter_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// SetParameter wrapper
OMX_ERRORTYPE ENS_Wrapper::SetParameter(OMX_HANDLETYPE hComponent,
                                        OMX_INDEXTYPE nIndex,
                                        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE result;

    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&SetParameter_str_mutex);
        static char param_str[WRAPPER_MAX_DEBUG_BUFFER];
        WRAPPER_OMX_LOG(hComponent, "SetParameter " OMX_COMPONENT_DEF_STR,
                        OMX_COMPONENT_DEF_ARG(hComponent));
        strOMX_INDEXTYPE_struct(nIndex, pComponentParameterStructure,
                                OMX_WRAPPER_LOG_LEVEL, ANDROID_LOG_DEBUG, LOG_TAG, NULL,
                                param_str, WRAPPER_MAX_DEBUG_BUFFER);
#ifndef ANDROID
        WRAPPER_OMX_LOG(hComponent, "%s", param_str);
#endif
        pthread_mutex_unlock(&SetParameter_str_mutex);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

#ifdef ANDROID
    if (OMX_HANDLETYPE_IS_ENSCOMPONENT(hComponent) &&
        (nIndex == (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer)) {

        result = ENS_Wrapper_useAndroidNativeBuffer(hComponent,
                                                    nIndex,
                                                    pComponentParameterStructure);

    } else
#endif //ANDROID

        result = OMX_HANDLETYPE_CALL(hComponent, SetParameter,
                                     nIndex, pComponentParameterStructure);

    //Save the context if context trace is enabled
    if ((OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace)) {
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace->
           SetParamOrConfig(nIndex, pComponentParameterStructure,result);
    }

    if (OMX_ErrorNone != result) {
        static char param_str[WRAPPER_MAX_DEBUG_BUFFER];
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SetParameter", OMX_FALSE, OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));
        Dump(LOG_CURRENT, "SetParameterFailure", OMX_HANDLETYPE_TO_WRAPPER(hComponent));
    } else {
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SetParameter", OMX_FALSE, "");
    }

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));
    return result;
}

static pthread_mutex_t GetConfig_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// GetConfig wrapper
OMX_ERRORTYPE ENS_Wrapper::GetConfig(OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nIndex,
                                     OMX_PTR pComponentConfigStructure)
{
    WRAPPER_OMX_LOG(hComponent, "GetConfig " OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, GetConfig,
                                               nIndex, pComponentConfigStructure);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&GetConfig_str_mutex);
        static char config_str[WRAPPER_MAX_DEBUG_BUFFER];
        strOMX_INDEXTYPE_struct(nIndex, pComponentConfigStructure,
                                OMX_WRAPPER_LOG_LEVEL, ANDROID_LOG_DEBUG, LOG_TAG, NULL,
                                config_str, WRAPPER_MAX_DEBUG_BUFFER);
#ifndef ANDROID
        WRAPPER_OMX_LOG(hComponent, "%s", config_str);
#endif
        pthread_mutex_unlock(&GetConfig_str_mutex);
    }

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "GetConfig", OMX_FALSE, "");

    return result;
}

static pthread_mutex_t SetConfig_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// SetConfig wrapper
OMX_ERRORTYPE ENS_Wrapper::SetConfig(OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nIndex,
                                     OMX_PTR pComponentConfigStructure)
{

    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&SetConfig_str_mutex);
        static char config_str[WRAPPER_MAX_DEBUG_BUFFER];
        WRAPPER_OMX_LOG(hComponent, "SetConfig " OMX_COMPONENT_DEF_STR,
                        OMX_COMPONENT_DEF_ARG(hComponent));
        strOMX_INDEXTYPE_struct(nIndex, pComponentConfigStructure,
                                OMX_WRAPPER_LOG_LEVEL, ANDROID_LOG_DEBUG, LOG_TAG, NULL,
                                config_str, WRAPPER_MAX_DEBUG_BUFFER);
#ifndef ANDROID
        WRAPPER_OMX_LOG(hComponent, "%s", config_str);
#endif
        pthread_mutex_unlock(&SetConfig_str_mutex);
    }
    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, SetConfig,
                                               nIndex, pComponentConfigStructure);

     //Save the context if context trace is enabled
    if ((OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace)){
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->ctxTrace->
            SetParamOrConfig(nIndex, pComponentConfigStructure,result);
    }

    if (result != OMX_ErrorNone) {
        static char config_str[WRAPPER_MAX_DEBUG_BUFFER];
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SetConfig", OMX_FALSE, OMX_COMPONENT_DEF_STR,
                        OMX_COMPONENT_DEF_ARG(hComponent));
        Dump(LOG_CURRENT, "SetConfigFailure", OMX_HANDLETYPE_TO_WRAPPER(hComponent));
    } else {
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "SetConfig", OMX_FALSE, "");
    }

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));
    return result;
}


// GetExtensionIndex wrapper
OMX_ERRORTYPE ENS_Wrapper::GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                             OMX_STRING cParameterName,
                                             OMX_INDEXTYPE *pIndexType)
{
    OMX_BOOL noErrorLog = OMX_FALSE ;
    WRAPPER_OMX_LOG(hComponent, "GetExtensionIndex " OMX_COMPONENT_DEF_STR " cParameterName=%s",
                    OMX_COMPONENT_DEF_ARG(hComponent), cParameterName);

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, GetExtensionIndex,
                                               cParameterName,pIndexType);
    if ((strcmp(cParameterName, "OMX.google.android.index.useAndroidNativeBuffer2") == 0)
                || (strcmp(cParameterName, "STE.ADM") == 0)) {
        noErrorLog = OMX_TRUE;
    }
    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    char index_str[WRAPPER_DEFAULT_STR_SIZE];
    WRAPPER_OMX_LOG_RETURN(hComponent, result, "GetExtensionIndex", noErrorLog,
                           " cParameterName=%s IndexType=%s", cParameterName,
                           (pIndexType ? strOMX_INDEXTYPE(*pIndexType, index_str,
                                                          WRAPPER_DEFAULT_STR_SIZE) : "NULL"));

    return result;
}

// GetState wrapper
OMX_ERRORTYPE ENS_Wrapper::GetState(OMX_HANDLETYPE hComponent,
                                    OMX_STATETYPE* pState)
{
    WRAPPER_OMX_LOG(hComponent, "GetState " OMX_COMPONENT_DEF_STR,
                    OMX_COMPONENT_DEF_ARG(hComponent));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, GetState,
                                               pState);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    char state_str[WRAPPER_DEFAULT_STR_SIZE];
    WRAPPER_OMX_LOG_RETURN(hComponent, result, "GetState", OMX_FALSE, " pState=%s",
                           (pState ? strOMX_STATETYPE(*pState, state_str, WRAPPER_DEFAULT_STR_SIZE) :
                            "NULL"));

    return result;
}

static pthread_mutex_t ComponentTunnelRequest_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// ComponentTunnelRequest wrapper
OMX_ERRORTYPE ENS_Wrapper::ComponentTunnelRequest(OMX_HANDLETYPE hComponent,
                                                  OMX_U32 nPort,
                                                  OMX_HANDLETYPE hTunneledComp,
                                                  OMX_U32 nTunneledPort,
                                                  OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    static char tunnelsetup_str[WRAPPER_MAX_DEBUG_BUFFER];
    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&ComponentTunnelRequest_str_mutex);
        strOMX_TUNNELSETUPTYPE(pTunnelSetup, tunnelsetup_str, WRAPPER_MAX_DEBUG_BUFFER);
        WRAPPER_OMX_LOG(hComponent,
                        "ComponentTunnelRequest: " OMX_COMPONENT_DEF_STR " nPort=%d\n"
                        "       hTunneledComp=0x%08x pTunneledPort=%d\n"
                        "       %s",
                        OMX_COMPONENT_DEF_ARG(hComponent),
                        (int) nPort, (unsigned int) hTunneledComp, (int) nTunneledPort,
                        tunnelsetup_str);
        pthread_mutex_unlock(&ComponentTunnelRequest_str_mutex);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, ComponentTunnelRequest,
                                               nPort, hTunneledComp, nTunneledPort, pTunnelSetup);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (WRAPPER_OMX_TRACE(hComponent) || (result != OMX_ErrorNone)) {
        pthread_mutex_lock(&ComponentTunnelRequest_str_mutex);
        strOMX_TUNNELSETUPTYPE(pTunnelSetup, tunnelsetup_str, WRAPPER_MAX_DEBUG_BUFFER);
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "ComponentTunnelRequest", OMX_FALSE, " nPort=%d\n"
                               "       hTunneledComp=0x%08x pTunneledPort=%d\n"
                               "       %s",
                               (int) nPort, (unsigned int) hTunneledComp, (int) nTunneledPort,
                               tunnelsetup_str);
        pthread_mutex_unlock(&ComponentTunnelRequest_str_mutex);
    }

    return result;
}

static pthread_mutex_t UseBuffer_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// UseBuffer wrapper
OMX_ERRORTYPE ENS_Wrapper::UseBuffer(OMX_HANDLETYPE hComponent,
                                     OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                     OMX_U32 nPortIndex,
                                     OMX_PTR pAppPrivate,
                                     OMX_U32 nSizeBytes,
                                     OMX_U8* pBuffer)
{
    static char header_str[WRAPPER_MAX_DEBUG_BUFFER];
    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&UseBuffer_str_mutex);
        WRAPPER_OMX_LOG(hComponent, "UseBuffer " OMX_COMPONENT_DEF_STR
                        " nPortIndex=%d pAppPrivate=0x%08x nSizeBytes=%d pBuffer=0x%08x",
                        OMX_COMPONENT_DEF_ARG(hComponent),
                        (int) nPortIndex, (unsigned int) pAppPrivate,
                        (int) nSizeBytes, (unsigned int) pBuffer);
        pthread_mutex_unlock(&UseBuffer_str_mutex);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, UseBuffer,
                                               ppBufferHdr, nPortIndex, pAppPrivate,
                                               nSizeBytes, pBuffer);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (WRAPPER_OMX_TRACE(hComponent) || (result != OMX_ErrorNone)) {
        pthread_mutex_lock(&UseBuffer_str_mutex);
        if (ppBufferHdr) {
            strOMX_BUFFERHEADERTYPE(*ppBufferHdr, header_str, WRAPPER_MAX_DEBUG_BUFFER);
        }
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "UseBuffer", OMX_FALSE, " *pBufferHdr=0x%08x\n%s",
                               (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0),
                               (ppBufferHdr ? header_str : "[ NULL ]"));
        pthread_mutex_unlock(&UseBuffer_str_mutex);
    }

    return result;
}

static pthread_mutex_t AllocateBuffer_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// AllocateBuffer wrapper
OMX_ERRORTYPE ENS_Wrapper::AllocateBuffer(OMX_HANDLETYPE hComponent,
                                          OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                          OMX_U32 nPortIndex,
                                          OMX_PTR pAppPrivate,
                                          OMX_U32 nSizeBytes)
{
    static char header_str[WRAPPER_MAX_DEBUG_BUFFER];
    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&AllocateBuffer_str_mutex);
        WRAPPER_OMX_LOG(hComponent, "AllocateBuffer " OMX_COMPONENT_DEF_STR
                        " nPortIndex=%d pAppPrivate=0x%08x nSizeBytes=%d",
                        OMX_COMPONENT_DEF_ARG(hComponent),
                        (int) nPortIndex, (unsigned int) pAppPrivate, (int) nSizeBytes);
        pthread_mutex_unlock(&AllocateBuffer_str_mutex);
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, AllocateBuffer,
                                               ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (result == OMX_ErrorNone) {
        assert(ppBufferHdr);
        assert(*ppBufferHdr);
        pthread_mutex_lock(&AllocateBuffer_str_mutex);
        strOMX_BUFFERHEADERTYPE(*ppBufferHdr, header_str, WRAPPER_MAX_DEBUG_BUFFER);
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "AllocateBuffer", OMX_FALSE, " ppBufferHdr=0x%08x\n%s",
                               (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0),
                               (ppBufferHdr ? header_str : "[ NULL ]"));
        pthread_mutex_unlock(&AllocateBuffer_str_mutex);
    } else {
        // Trace client request in case of error
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "AllocateBuffer", OMX_FALSE,
                                " nPortIndex=%d pAppPrivate=0x%08x nSizeBytes=%d",
                                (int) nPortIndex, (unsigned int) pAppPrivate, (int) nSizeBytes );
    }

    return result;
}

// FreeBuffer wrapper
OMX_ERRORTYPE ENS_Wrapper::FreeBuffer(OMX_HANDLETYPE hComponent,
                                      OMX_U32 nPortIndex,
                                      OMX_BUFFERHEADERTYPE* pBuffer)
{
    WRAPPER_OMX_LOG(hComponent, "FreeBuffer " OMX_COMPONENT_DEF_STR " nPortIndex=%d pBuffer=0x%08x",
                    OMX_COMPONENT_DEF_ARG(hComponent), (int) nPortIndex, (int) pBuffer);

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, FreeBuffer,
                                               nPortIndex, pBuffer);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "FreeBuffer", OMX_FALSE, "");

    return result;
}

static pthread_mutex_t EmptyThisBuffer_str_mutex = PTHREAD_MUTEX_INITIALIZER;

// EmptyThisBuffer wrapper
OMX_ERRORTYPE ENS_Wrapper::EmptyThisBuffer(OMX_HANDLETYPE hComponent,
                                           OMX_BUFFERHEADERTYPE* pBuffer)
{
    if (WRAPPER_OMX_TRACE(hComponent)) {
        pthread_mutex_lock(&EmptyThisBuffer_str_mutex);
        static char header_buffer[WRAPPER_MAX_DEBUG_BUFFER];

        WRAPPER_OMX_LOG_BUFFER(hComponent,
                               "EmptyThisBuffer " OMX_COMPONENT_DEF_STR " pBuffer=0x%08x\n%s",
                               OMX_COMPONENT_DEF_ARG(hComponent), (unsigned int)  pBuffer,
                               strOMX_BUFFERHEADERTYPE(pBuffer, header_buffer,
                                                       WRAPPER_MAX_DEBUG_BUFFER));
        pthread_mutex_unlock(&EmptyThisBuffer_str_mutex);
    }
    if (OMX_dump_enabled && (OMX_dump_dir != 1) &&
        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->dumpBuffer) {
        if (OMX_dump_hex) {
            ALOGD("%s", strOMX_BUFFERDATA(pBuffer,
                                         &OMX_dump_input_buffer,
                                         &OMX_dump_input_buffer_size,
                                         OMX_dump_size,
                                         OMX_BUFFERDATA_NB_END_BYTES,
                                         OMX_BUFFERDATA_NB_BYTES_PER_LINES));
        } else if (!OMX_dump_hex && (OMX_dump_idx == pBuffer->nInputPortIndex)) {
            OMX_HANDLETYPE_TO_WRAPPER(hComponent)->dumpData(pBuffer,
                                                        "IN",
                                                        OMX_dump_path,
                                                        OMX_dump_idx,
                                                        OMX_dump_size,
                                                        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->name,
                                                        hComponent);
        }
    }

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, EmptyThisBuffer, pBuffer);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (IS_WRAPPER_OMX_LOG_LEVEL(OMX_LOG_LEVEL_BUFFER)) {
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "EmptyThisBuffer", OMX_FALSE, " pBuffer=0x%08x",
                               (unsigned int) pBuffer);
    }

    return result;
}

// FillThisBuffer wrapper
OMX_ERRORTYPE ENS_Wrapper::FillThisBuffer(OMX_HANDLETYPE hComponent,
                                          OMX_BUFFERHEADERTYPE *pBuffer)
{
    WRAPPER_OMX_LOG_BUFFER(hComponent,
                           "FillThisBuffer " OMX_COMPONENT_DEF_STR " pBuffer=0x%08x",
                           OMX_COMPONENT_DEF_ARG(hComponent), (unsigned int) pBuffer);

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, FillThisBuffer, pBuffer);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    if (IS_WRAPPER_OMX_LOG_LEVEL(OMX_LOG_LEVEL_BUFFER)) {
        WRAPPER_OMX_LOG_RETURN(hComponent, result, "FillThisBuffer", OMX_FALSE, " pBuffer=0x%08x",
                               (unsigned int) pBuffer);
    }

    return result;
}

// UseEGLImage wrapper
OMX_ERRORTYPE ENS_Wrapper::UseEGLImage(OMX_HANDLETYPE hComponent,
                                       OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                       OMX_U32 nPortIndex,
                                       OMX_PTR pAppPrivate,
                                       void* eglImage)
{

    WRAPPER_OMX_LOG(hComponent, "UseEGLImage " OMX_COMPONENT_DEF_STR
                    " nPortIndex=%d pAppPrivate=0x%08x eglImage=0x%08x",
                    OMX_COMPONENT_DEF_ARG(hComponent),
                    (int) nPortIndex, (unsigned int) pAppPrivate, (unsigned int) eglImage);

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, UseEGLImage,
                                               ppBufferHdr, nPortIndex, pAppPrivate, eglImage);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "UseEGLImage", OMX_FALSE, "");

    return result;
}

// ComponentRoleEnum wrapper
OMX_ERRORTYPE ENS_Wrapper::ComponentRoleEnum(OMX_HANDLETYPE hComponent,
                                             OMX_U8* cRole,
                                             OMX_U32 nIndex)
{
    WRAPPER_OMX_LOG(hComponent, "ComponentRoleEnum " OMX_COMPONENT_DEF_STR " nIndex=%d",
                    OMX_COMPONENT_DEF_ARG(hComponent),
                    (int) nIndex);

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    OMX_ERRORTYPE result = OMX_HANDLETYPE_CALL(hComponent, ComponentRoleEnum,
                                               cRole, nIndex);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));

    WRAPPER_OMX_LOG_RETURN(hComponent, result, "ComponentRoleEnum", OMX_FALSE, " Role=\"%s\"",
                           (result == OMX_ErrorNone ? cRole: (OMX_U8*) ""));

    return result;
}

/********************************************************************************
 * OMX IL callbacks
 ********************************************************************************/

static pthread_mutex_t EventHandler_str_mutex = PTHREAD_MUTEX_INITIALIZER;

OMX_ERRORTYPE ENS_Wrapper::EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                                        OMX_IN OMX_PTR pAppData,
                                        OMX_IN OMX_EVENTTYPE eEvent,
                                        OMX_IN OMX_U32 nData1,
                                        OMX_IN OMX_U32 nData2,
                                        OMX_IN OMX_PTR pEventData)
{
    // hComponent may be an non ENS component, first find back the corresponding wrapper
    OMX_COMPONENTTYPE *omx_wrapper = GetComponentWrapperForHandle(hComponent);
    if (omx_wrapper == NULL) {
        CB_Message *message = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->popPreAllocatedCBMessage();

        message->type = OMX_EVENTHANDLER;
        message->hComponent = OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent);
        message->args.eventHandler.pAppData   = pAppData;
        message->args.eventHandler.eEvent     = eEvent;
        message->args.eventHandler.nData1     = nData1;
        message->args.eventHandler.nData2     = nData2;
        message->args.eventHandler.pEventData = pEventData;

        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->pushCBMessage(message);

        // We never return any error ...
        return OMX_ErrorNone;
    } else {
        if (WRAPPER_OMX_TRACE(omx_wrapper)) {
            CB_Message message;
            pthread_mutex_lock(&EventHandler_str_mutex);
            static char EventHandler_message_buffer[WRAPPER_MAX_DEBUG_BUFFER];
            message.type = OMX_EVENTHANDLER;
            message.args.eventHandler.eEvent = eEvent;
            message.args.eventHandler.nData1 = nData1;
            message.args.eventHandler.nData2 = nData2;
            message.args.eventHandler.pEventData = pEventData;
            sprintCBMessage(EventHandler_message_buffer, &message, WRAPPER_MAX_DEBUG_BUFFER);
            WRAPPER_OMX_LOG(omx_wrapper, OMX_COMPONENT_DEF_STR " sending %s",
                            OMX_COMPONENT_DEF_ARG(omx_wrapper), EventHandler_message_buffer);
            pthread_mutex_unlock(&EventHandler_str_mutex);
        }

        // Use the component thread to call the client CB
        if (OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB &&
            OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->EventHandler) {
            return OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->
                EventHandler(omx_wrapper, pAppData, eEvent, nData1, nData2, pEventData);
        }
    }
    return OMX_ErrorNone;
}

static pthread_mutex_t EmptyBufferDone_str_mutex = PTHREAD_MUTEX_INITIALIZER;

OMX_ERRORTYPE ENS_Wrapper::EmptyBufferDone(OMX_HANDLETYPE hComponent,
                                           OMX_PTR pAppData,
                                           OMX_BUFFERHEADERTYPE* pBuffer)
{
    // hComponent may be an non ENS component, first find back the corresponding wrapper
    OMX_COMPONENTTYPE *omx_wrapper = GetComponentWrapperForHandle(hComponent);
    if (omx_wrapper == NULL) {
        CB_Message *message = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->popPreAllocatedCBMessage();

        message->type = OMX_EMPTYBUFFERDONE;
        message->hComponent = OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent);
        message->args.bufferDone.pAppData = pAppData;
        message->args.bufferDone.pBuffer  = pBuffer;

        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->pushCBMessage(message);

        // We never return any error ...
        return OMX_ErrorNone;
    } else {

        if (IS_WRAPPER_OMX_LOG_LEVEL(OMX_LOG_LEVEL_BUFFER)) {
            CB_Message message;
            pthread_mutex_lock(&EmptyBufferDone_str_mutex);
            static char EmptyBufferDone_message_buffer[WRAPPER_MAX_DEBUG_BUFFER];
            message.type = OMX_EMPTYBUFFERDONE;
            message.args.bufferDone.pBuffer = pBuffer;
            sprintCBMessage(EmptyBufferDone_message_buffer, &message, WRAPPER_MAX_DEBUG_BUFFER);
            WRAPPER_OMX_LOG_BUFFER(omx_wrapper, OMX_COMPONENT_DEF_STR " sending %s",
                                   OMX_COMPONENT_DEF_ARG(omx_wrapper), EmptyBufferDone_message_buffer);
            pthread_mutex_unlock(&EmptyBufferDone_str_mutex);
        }

        // Use the component thread to call the client CB
        if (OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB &&
            OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->EmptyBufferDone) {
            return OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->
                EmptyBufferDone(omx_wrapper, pAppData, pBuffer);
        }
    }
    return OMX_ErrorNone;
}

static pthread_mutex_t FillBufferDone_str_mutex = PTHREAD_MUTEX_INITIALIZER;

OMX_ERRORTYPE ENS_Wrapper::FillBufferDone(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_BUFFERHEADERTYPE* pBuffer)
{
    // hComponent may be an non ENS component, first find back the corresponding wrapper
    OMX_COMPONENTTYPE *omx_wrapper = GetComponentWrapperForHandle(hComponent);
    if (omx_wrapper == NULL) {
        CB_Message *message = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->popPreAllocatedCBMessage();

        message->type = OMX_FILLBUFFERDONE;
        message->hComponent = OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent);
        message->args.bufferDone.pAppData = pAppData;
        message->args.bufferDone.pBuffer  = pBuffer;

        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->pushCBMessage(message);

        // We never return any error ...
        return OMX_ErrorNone;
    } else {

        if (IS_WRAPPER_OMX_LOG_LEVEL(OMX_LOG_LEVEL_BUFFER)) {
            CB_Message message;
            pthread_mutex_lock(&FillBufferDone_str_mutex);
            static char FillBufferDone_message_buffer[WRAPPER_MAX_DEBUG_BUFFER];
            message.type = OMX_FILLBUFFERDONE;
            message.args.bufferDone.pBuffer = pBuffer;
            sprintCBMessage(FillBufferDone_message_buffer, &message, WRAPPER_MAX_DEBUG_BUFFER);
            WRAPPER_OMX_LOG_BUFFER(omx_wrapper, OMX_COMPONENT_DEF_STR " sending %s",
                                   OMX_COMPONENT_DEF_ARG(omx_wrapper), FillBufferDone_message_buffer);
            pthread_mutex_unlock(&FillBufferDone_str_mutex);
        }
        if (OMX_dump_enabled && OMX_dump_dir &&
            OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->dumpBuffer) {
            if (OMX_dump_hex) {
                ALOGD("%s", strOMX_BUFFERDATA(pBuffer,
                                             &OMX_dump_output_buffer,
                                             &OMX_dump_output_buffer_size,
                                             OMX_dump_size,
                                             OMX_BUFFERDATA_NB_END_BYTES,
                                             OMX_BUFFERDATA_NB_BYTES_PER_LINES));
            } else if (!OMX_dump_hex && (OMX_dump_idx == pBuffer->nInputPortIndex)) {
                OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->dumpData(pBuffer,
                                                        "OUT",
                                                        OMX_dump_path,
                                                        OMX_dump_idx,
                                                        OMX_dump_size,
                                                        OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->name,
                                                        omx_wrapper);
            }
        }

        if (OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB &&
            OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->FillBufferDone) {
            return OMX_HANDLETYPE_TO_WRAPPER(omx_wrapper)->mOMXClientCB->
                FillBufferDone(omx_wrapper, pAppData, pBuffer);
        }
    }
    return OMX_ErrorNone;
}

/********************************************************************************
 * RME callbacks
 ********************************************************************************/

RM_CBK_T * ENS_Wrapper::getRMEwrappercbks()
{
    return mRMEcbks;
}

const RM_CBK_T * ENS_Wrapper::getRMEcbks()
{
    return pcbks;
}

void ENS_Wrapper::setRMEcbks(const RM_CBK_T * cbks) {
    pcbks = cbks;
}

/********************************************************************************
 * The wrapper callbacks register to RME - defined to be synchronous (direct call)
 ********************************************************************************/

RM_STATUS_E ENS_Wrapper::RME2CMP_Get_Capabilities(OMX_IN const OMX_PTR pCompHdl,
                                                  OMX_OUT RM_CAPABILITIES_T* pCapabilities)
{
    DEBUG_RME_PRINTF("RME2CMP_Get_Capabilities - ID=%d", (unsigned int) pCompHdl);

    OMX_COMPONENTTYPE *hComponent = GetOMX_COMPONENTTYPEforID((unsigned int) pCompHdl);
    RM_STATUS_E res = RM_E_INVALIDPTR;

    if(hComponent) {
        WRAPPER_LOCK(hComponent);

        res = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->getRMEcbks()->
            CP_Get_Capabilities(hComponent, pCapabilities);

        WRAPPER_UNLOCK(hComponent);
    } else {
        ALOGE("RME2CMP_Get_Capabilities called with invalid RMEID=%d",
             (unsigned int) pCompHdl);
    }

    return res;
}

RM_STATUS_E ENS_Wrapper::RME2CMP_Get_ResourcesEstimation(OMX_IN const OMX_PTR pCompHdl,
                                                         OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    DEBUG_RME_PRINTF("RME2CMP_Get_ResourcesEstimation - ID=%d", (unsigned int) pCompHdl);

    OMX_COMPONENTTYPE *hComponent = GetOMX_COMPONENTTYPEforID((unsigned int) pCompHdl);
    RM_STATUS_E res = RM_E_INVALIDPTR;

    if(hComponent) {
        WRAPPER_LOCK(hComponent);

        res = OMX_HANDLETYPE_TO_WRAPPER(hComponent)->getRMEcbks()->
            CP_Get_ResourcesEstimation(hComponent,pEstimationData);

        WRAPPER_UNLOCK(hComponent);
    } else {
        ALOGE("RME2CMP_Get_ResourcesEstimation called with invalid RMEID=%d",
             (unsigned int) pCompHdl);
    }

    return res;
}

/********************************************************************************
 * The wrapper callbacks register to RME - defined to be asynchronous (indirect call)
 * The below functions are called from the thread reading from the RME2CMP message
 * queue and the pCompHdl is the OMX_COMPONENTTYPE
 ********************************************************************************/

void ENS_Wrapper::RME2CMP_Notify_Error(OMX_IN const OMX_PTR pCompHdl,
                                       OMX_IN RM_STATUS_E nError)
{
    DEBUG_RME_PRINTF("RME2CMP_Notify_Error " OMX_COMPONENT_DEF_STR,
                     OMX_COMPONENT_DEF_ARG(pCompHdl));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));

    OMX_HANDLETYPE_TO_WRAPPER(pCompHdl)->getRMEcbks()->
        CP_Notify_Error(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl), nError);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));
}

void ENS_Wrapper::RME2CMP_Notify_ResReservationProcessed(OMX_IN const OMX_PTR pCompHdl,
                                                         OMX_IN OMX_BOOL bResourcesReservGranted)
{
    DEBUG_RME_PRINTF("RME2CMP_Notify_ResReservationProcessed " OMX_COMPONENT_DEF_STR,
                     OMX_COMPONENT_DEF_ARG(pCompHdl));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));

    OMX_HANDLETYPE_TO_WRAPPER(pCompHdl)->getRMEcbks()->
        CP_Notify_ResReservationProcessed(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl),
                                          bResourcesReservGranted);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));
}

void ENS_Wrapper::RME2CMP_Cmd_SetRMConfig(OMX_IN const OMX_PTR pCompHdl,
                                          OMX_IN const RM_SYSCTL_T* pRMcfg)
{
    DEBUG_RME_PRINTF("RME2CMP_Cmd_SetRMConfig " OMX_COMPONENT_DEF_STR,
                     OMX_COMPONENT_DEF_ARG(pCompHdl));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));

    OMX_HANDLETYPE_TO_WRAPPER(pCompHdl)->getRMEcbks()->
        CP_Cmd_SetRMConfig(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl), pRMcfg);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));
}

void ENS_Wrapper::RME2CMP_Cmd_ReleaseResource(OMX_IN const OMX_PTR pCompHdl,
                                              OMX_IN OMX_BOOL bSuspend)
{
    DEBUG_RME_PRINTF("RME2CMP_Cmd_ReleaseResource " OMX_COMPONENT_DEF_STR,
                     OMX_COMPONENT_DEF_ARG(pCompHdl));

    WRAPPER_LOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));

    OMX_HANDLETYPE_TO_WRAPPER(pCompHdl)->getRMEcbks()->
        CP_Cmd_ReleaseResource(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl), bSuspend);

    WRAPPER_UNLOCK(OMX_HANDLETYPE_TO_COMPONENTTYPE(pCompHdl));
}

/********************************************************************************
 * The wrapper callbacks register to RME - defined to be asynchronous (using msg queue)
 ********************************************************************************/

void ENS_Wrapper::postRME2CMP_Notify_Error(OMX_IN const OMX_PTR pCompHdl,
                                           OMX_IN RM_STATUS_E nError)
{
    ENS_RME_Wrapper *rmeWrapper = ENS_RME_Wrapper::getInstance();

    DEBUG_RME_PRINTF("postRME2CMP_Notify_Error ID=%d", (unsigned int) pCompHdl);

    RME2CMP_Message *RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();

    DBC_ASSERT(RME2CMPmsg != NULL);

    RME2CMPmsg->type = MSG_RME2CMP_CP_Notify_Error;
    RME2CMPmsg->pCompHdl = pCompHdl;
    RME2CMPmsg->args.CP_Notify_Error.nError = nError;

    rmeWrapper->postRME2CMPmsg(RME2CMPmsg);
}

void ENS_Wrapper::postRME2CMP_Notify_ResReservationProcessed(OMX_IN const OMX_PTR pCompHdl,
                                                             OMX_IN OMX_BOOL bResourcesReservGranted)
{
    ENS_RME_Wrapper *rmeWrapper = ENS_RME_Wrapper::getInstance();

    DEBUG_RME_PRINTF("postRME2CMP_Notify_ResReservationProcessed ID=%d", (unsigned int) pCompHdl);

    RME2CMP_Message *RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();

    DBC_ASSERT(RME2CMPmsg != NULL);

    RME2CMPmsg->type = MSG_RME2CMP_CP_Notify_ResReservationProcessed;
    RME2CMPmsg->pCompHdl = pCompHdl;
    RME2CMPmsg->args.CP_Notify_ResReservationProcessed.bResourcesReservGranted = bResourcesReservGranted;

    rmeWrapper->postRME2CMPmsg(RME2CMPmsg);
}


void ENS_Wrapper::postRME2CMP_Cmd_SetRMConfig(OMX_IN const OMX_PTR pCompHdl,
                                              OMX_IN const RM_SYSCTL_T* pRMcfg)
{
    ENS_RME_Wrapper *rmeWrapper = ENS_RME_Wrapper::getInstance();

    DEBUG_RME_PRINTF("postRME2CMP_Cmd_SetRMConfig ID=%d", (unsigned int) pCompHdl);

    RME2CMP_Message *RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();

    DBC_ASSERT(RME2CMPmsg != NULL);

    RME2CMPmsg->type = MSG_RME2CMP_CP_Cmd_SetRMConfig;
    RME2CMPmsg->pCompHdl = pCompHdl;
    memcpy(&(RME2CMPmsg->args.CP_Cmd_SetRMConfig.sRMcfg),pRMcfg,sizeof(RM_SYSCTL_T));

    rmeWrapper->postRME2CMPmsg(RME2CMPmsg);
}


void ENS_Wrapper::postRME2CMP_Cmd_ReleaseResource(OMX_IN const OMX_PTR pCompHdl,
                                                  OMX_IN OMX_BOOL bSuspend)
{
    ENS_RME_Wrapper *rmeWrapper = ENS_RME_Wrapper::getInstance();

    DEBUG_RME_PRINTF("postRME2CMP_Cmd_ReleaseResource ID=%d", (unsigned int) pCompHdl);

    RME2CMP_Message *RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();

    DBC_ASSERT(RME2CMPmsg != NULL);

    RME2CMPmsg->type = MSG_RME2CMP_CP_Cmd_ReleaseResource;
    RME2CMPmsg->pCompHdl = pCompHdl;
    RME2CMPmsg->args.CP_Cmd_ReleaseResource.bSuspend = bSuspend;

    rmeWrapper->postRME2CMPmsg(RME2CMPmsg);
}
