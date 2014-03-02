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

#include "ENS_Wrapper.h"
#include "ENS_WrapperLog.h"
#include "ENS_DBC.h"

#include <OMX_debug.h>

#undef LOG_TAG
#define LOG_TAG "CtxTrace"

#define CTXTRACE_DEBUG_LOG(...)                 \
    if (ENS_Wrapper_CtxTrace::debug_enabled) {  \
        ALOGD(__VA_ARGS__);                      \
    }

#define CTX_TMPLOG_SIZE 1024
#define CTX_TRACE_CMP_LOG_MAXSIZE (64*1024)
#define CTX_TMPLOG_PARAM_CONFIG_SIZE (10*1024)
#define CTX_PARAM_CONFIG_STRUCT_MAX_SIZE (10*1024)
#define CTX_COMPONENT_LIST_SIZE (2*1024)
#define SUMMARY_HEADER "################################################################################"

char ENS_Wrapper_CtxTrace::filename[CTX_TRACE_STRINGNAME_SIZE];
char ENS_Wrapper_CtxTrace::processname[CTX_TRACE_STRINGNAME_SIZE];
int ENS_Wrapper_CtxTrace::inotify_fd = -1;
ctx_trace_history_list_global_t ENS_Wrapper_CtxTrace::historyGlobal[CTX_TRACE_NUMBER_OF_UNIQUE_HISTORY_COMPONENTS];
int ENS_Wrapper_CtxTrace::historyGlobalIdx= 0 ;
bool ENS_Wrapper_CtxTrace::enabled = true;
bool ENS_Wrapper_CtxTrace::debug_enabled = false;
bool ENS_Wrapper_CtxTrace::active_component = false;
unsigned int ENS_Wrapper_CtxTrace::history_depth = 0;
pthread_mutex_t ENS_Wrapper_CtxTrace::mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

static pthread_cond_t private_async_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t incdec_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t logbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

#define PRIVATE_INCDEC_LOCK pthread_mutex_lock(&incdec_mutex)
#define PRIVATE_INCDEC_UNLOCK pthread_mutex_unlock(&incdec_mutex)


#define CTXTRACE_DIR "/data/ste-debug/omx"

#define PRINT_TO_BUFFER(buffer, nb_char, size, ...) \
    do {                                                                \
        pthread_mutex_lock(&logbuf_mutex); \
        nb_char += snprintf(buffer + nb_char, ( size - nb_char > 0 ? size - nb_char : 0), __VA_ARGS__); \
        if ((size - nb_char)<= 0) {                           \
            ALOGW(" Log Buffer Size exceeded %d bytes",size); \
        }                                                                     \
       pthread_mutex_unlock(&logbuf_mutex); \
       } while (0)

#define PRIVATECONTEXTPRINT_MAGIC 0x12344321

__BEGIN_DECLS
static void ctxTracePrivateContextPInc(OMX_PTR myctx)
{
    PRIVATE_INCDEC_LOCK;
    ctx_trace_PrivateContextPrint_t *ctx = (ctx_trace_PrivateContextPrint_t *) myctx;

    if ((ctx == NULL) || (ctx->magic != PRIVATECONTEXTPRINT_MAGIC)) {
        LOGE("ctxTracePrivateContextPInc got invalid dump context - discarding print");
        PRIVATE_INCDEC_UNLOCK;
        return;
    }
    ctx->context_counter ++ ;  //Increment the number of asynchronous clients

    PRIVATE_INCDEC_UNLOCK;

}
static void ctxTracePrivateContextPDec(OMX_PTR myctx)
{
    PRIVATE_INCDEC_LOCK;
    ctx_trace_PrivateContextPrint_t *ctx = (ctx_trace_PrivateContextPrint_t *) myctx;
    if ((ctx == NULL) || (ctx->magic != PRIVATECONTEXTPRINT_MAGIC)) {
        LOGE("ctxTracePrivateContextPDec got invalid dump context - discarding print");
        PRIVATE_INCDEC_UNLOCK;
        return;
    }
    ctx->context_counter -- ; //Decrement the number of asynchronous clients
    if(ctx->context_counter == 0) {
        pthread_cond_broadcast(&private_async_cond);
    }
    PRIVATE_INCDEC_UNLOCK;
}


static void ctxTracePrivateContextPrint(OMX_PTR myctx, const char* fmt, ...)
{
    ctx_trace_PrivateContextPrint_t *ctx = (ctx_trace_PrivateContextPrint_t *) myctx;
    va_list ap;

    if ((ctx == NULL) || (ctx->magic != PRIVATECONTEXTPRINT_MAGIC)) {
        ALOGE("ctxTracePrivateContextPrint got invalid dump context - discarding print");
        return;
    }

    pthread_mutex_lock(&logbuf_mutex);
    va_start(ap, fmt);
    ctx->nb_char += vsnprintf(*(ctx->buffer) + ctx->nb_char,
                              ( ctx->size - ctx->nb_char > 0 ? ctx->size - ctx->nb_char : 0),
                              fmt, ap);
    va_end(ap);
    pthread_mutex_unlock(&logbuf_mutex);
}

__END_DECLS

ENS_Wrapper_CtxTrace::ENS_Wrapper_CtxTrace(ENS_Wrapper *wrapper_arg)
    : wrapper(wrapper_arg), history(NULL), indexSaveIdx(0), NbOfSetParamAndConfigErrors(0), cmdSaveIdx(0), NbOfSendCmdErrors(0)
{
    Lock();
    if (inotify_fd == -1) {
        Init();
    }
    UnLock();

    for (int i = 0; i < CTX_TRACE_CMD_SIZE; i++) {
        cmdSave[i].Cmd = (OMX_COMMANDTYPE) -1;
    }

    for (int i = 0; i < CTX_TRACE_INDEX_SIZE; i++) {
        memset(&indexSave[i], 0, sizeof(ctx_trace_indexSave_t));
        indexSave[i].errorCode = OMX_ErrorNone;
    }

}

ENS_Wrapper_CtxTrace::~ENS_Wrapper_CtxTrace() { }

void ENS_Wrapper_CtxTrace::Lock(void)
{
    pthread_mutex_lock(&mutex);
}

void ENS_Wrapper_CtxTrace::UnLock(void)
{
    pthread_mutex_unlock(&mutex);
}

/**
 * Enable the context trace.
 */
void ENS_Wrapper_CtxTrace::Init(void)
{
    CTXTRACE_DEBUG_LOG("Init()");

    // Create an instance of Inotify
    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        ALOGE("Init inotify_init failed: %s", strerror(errno));
        return;
    }

    // Create directories
    if (mkdir(CTXTRACE_DIR, S_IRWXU | S_IRWXG | S_IRWXO)) {
        if (errno != EEXIST) {
            ALOGE("mkdir of '" CTXTRACE_DIR "' failed: %s",
                 strerror(errno));
        }
    }
    // Create the inotify watch thread
    pthread_t thread;
    pthread_create(&thread, NULL, &ENS_Wrapper_CtxTrace::Thread, NULL);
}

/** This thread looks after the file events. */
void * ENS_Wrapper_CtxTrace::Thread(void *)
{
    CTXTRACE_DEBUG_LOG("Thread");
#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, "WrapperCtxTrace", 0, 0, 0);
#endif


    // Get the process name
    FILE *f = fopen("/proc/self/comm", "r");
    fscanf(f, "%s", processname);
    fclose(f);

    // Name of file used to interact with trace
    sprintf(filename, CTXTRACE_DIR "/%s", processname);

    // Watch the file
    int wd = inotify_add_watch(inotify_fd, filename, IN_CLOSE_WRITE);
    if (wd < 0) {
        LOGE("Init inotify_add_watch failed wd=%d, errno=%s, filename=%s",
             wd, strerror(errno), filename);
        inotify_fd = -1;
        return (void*)0;
    }

    WriteSummary();

    while (1) {
        fd_set fds;
        static char buffer[10 * sizeof(struct inotify_event)];

        FD_ZERO(&fds);
        FD_SET(inotify_fd, &fds);

        /* Wait file event */
        if (select(inotify_fd + 1, &fds, NULL, NULL, 0) <= 0) {
            continue;
        }

        /* Retrieve information about the file event */
        int length = read(inotify_fd, buffer, sizeof(buffer));

        if (length <= 0) {
            ALOGE("Thread read from inotify_fd failed: %s", strerror(errno));
            continue;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            CTXTRACE_DEBUG_LOG("file event wd=%d, mask=0x%08x len=%d, name=%s",
                               event->wd, event->mask, event->len, (event->len ? event->name : ""));
            // File content has been modified
            Lock();
            FILE *file = fopen(filename, "r");
            char actionBuffer[CTX_TRACE_STRINGNAME_SIZE];
            if (file == NULL) {
                ALOGE("open of '%s' for reading failed: %s", filename, strerror(errno));
            } else {
                int got_nb_char = fread(actionBuffer, 1, CTX_TRACE_STRINGNAME_SIZE, file);
                fclose(file);
                // check if it was a user action
                if (strncmp(actionBuffer, SUMMARY_HEADER, strlen(SUMMARY_HEADER))) {
                    // Got a user action
                    if (strncmp(actionBuffer, "log-private", 11) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: log-private");
                        LogPrivateState();
                    } else if (strncmp(actionBuffer, "dump-opened", 11) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: dump-opened");
                        DumpOrLog(false, true, false,true);
                    } else if (strncmp(actionBuffer, "log-opened", 10) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: log-opened");
                        DumpOrLog(true, true, false,true);
                    } else if (strncmp(actionBuffer, "dump-closed", 11) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: dump-closed");
                        DumpOrLog(false, false, true,true);
                    } else if (strncmp(actionBuffer, "log-closed", 10) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: log-closed");
                        DumpOrLog(true, false, true,true);
                    } else if (strncmp(actionBuffer, "dump", 4) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: dump");
                        DumpOrLog(false, true, true,true);
                    } else if (strncmp(actionBuffer, "log", 3) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: log");
                        DumpOrLog(true, true, true,true);
                    } else if (strncmp(actionBuffer, "clear", 5) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: clear");
                        RemoveAllFromHistory();
                    } else if (strncmp(actionBuffer, "help", 4) == 0) {
                        CTXTRACE_DEBUG_LOG("User action: help");
                        PrintHelp();
                    }else if (strncmp(actionBuffer, "OMX.", 4) == 0) { // check if component name
                        CTXTRACE_DEBUG_LOG("User action: component-name");
                        //Extract the full component name from the buffer
                        char * compNameFull = strtok(actionBuffer," #\n");
                        bool found_comp = false;
                        unsigned long comp_id;

                        // Extract the actual component name with the specific ID
                        char * compName = strtok (compNameFull,"_");
                        char * tok = strtok (NULL, "0");
                        if (tok != NULL) {
                            comp_id = atoi(tok);
                        } else {
                            comp_id = 0;
                        }

                        CTXTRACE_DEBUG_LOG("Request for compName %s id %lu",
                                            compName,comp_id);

                        //Validate index from user
                        if (comp_id >= history_depth) {
                            ALOGE("Component ID is out of bounds. Default to latest status");
                            comp_id = 0;
                        }

                        //Log the active status of the component if present in active list
                        //Skip active list if specific Id is requested.
                        if (!comp_id) {
                            unsigned long nbActiveComponents = ENS_Wrapper::NbActiveComponents();
                            for (unsigned long i = 0; i<nbActiveComponents; i++) {
                                OMX_COMPONENTTYPE *hComp = ENS_Wrapper::GetOMX_COMPONENTTYPEforIndex(i);
                                if (strcmp(compName, OMX_HANDLETYPE_GET_NAME(hComp)) == 0) {
                                    CTXTRACE_DEBUG_LOG("Found active component for %s", compName);
                                    ENS_Wrapper *wrapper = OMX_HANDLETYPE_TO_WRAPPER(hComp);
                                    wrapper->ctxTrace->DumpActiveComponent(NULL);
                                    found_comp = true;
                                    break; //break from for loop
                                }
                            }
                        }
                        //Log the latest history status of the component if not active or
                        //Log the specific occurence of the component if requested

                        if (found_comp == false) {
                            ctx_trace_history_list_elem_t *elem = NULL;
                            for (int i = 0; i < historyGlobalIdx; i++) {
                                //find the list corresponding to the component
                                if (strcmp(historyGlobal[i].wrapperName1,compName) == 0) {
                                    CTXTRACE_DEBUG_LOG("Found inactive component for %s", compName);
                                    unsigned int index=0;
                                    //log when the requested Id matches the entry in the list
                                    LIST_FOREACH(elem, &historyGlobal[i].ctx_trace_history_list, list_entry) {
                                        if (comp_id == index) {
                                            CTXTRACE_DEBUG_LOG("Logging index %d of component %s"
                                                ,index,compName);
                                            elem->ctxTrace->DumpInactiveComponent(NULL,elem,i,index);
                                            found_comp = true;
                                            break;//break from LIST_FOREACH loop
                                        }
                                        index++;
                                    }
                                    break; // break from the outer for loop
                                }
                            }
                        }
                        if (found_comp == false) {
                            ALOGE("Requested component %s not found",compNameFull);
                        }
                    }else{
                        //ERROR
                        ALOGE("Invalid Input from User");
                        PrintHelp();
                    }
                    // Re-write file content
                    WriteSummary();
                } else {
                    CTXTRACE_DEBUG_LOG("WriteSummary file close");
                }
            }
            UnLock();
            i += event->len + sizeof (struct inotify_event);
        }
    }
    return 0;
}


void ENS_Wrapper_CtxTrace::MoveToHistory(void)
{
    CTXTRACE_DEBUG_LOG("MoveToHistory %s ", wrapper->name);
    bool list_existing = false;

    // Create a new element and add it to the history list
    history = (ctx_trace_history_list_elem_t *) malloc(sizeof(ctx_trace_history_list_elem_t));

    if (history) {
        history->ctxTrace = this;
        static char log[CTX_TRACE_CMP_LOG_MAXSIZE];
        FillTheLog(this, log, CTX_TRACE_CMP_LOG_MAXSIZE);
        int logLength = strlen(log)+1;
        history->pLog = (char*) malloc(logLength);
        if (history->pLog) {
            memcpy(history->pLog, log, logLength);
        } else {
            ALOGE("MoveToHistory failed to allocate log");
        }
        snprintf(history->wrapperName, CTX_TRACE_STRINGNAME_SIZE,"%s", wrapper->name);
        getCurrentTime(history->timestamp, CTX_TRACE_TIMESTAMP_SIZE);
        CTXTRACE_DEBUG_LOG("MoveToHistory adding %s ", history->wrapperName);

        Lock();

        //If component found in existing list, add the component to the list
        for (int i = 0; i < historyGlobalIdx; i++) {
            if (strcmp(historyGlobal[i].wrapperName1, history->wrapperName) == 0) {
                CTXTRACE_DEBUG_LOG("Component %s found in list %d",history->wrapperName,i);
                list_existing = true;

                //increment number of components in the existing list after checking against depth
                if (historyGlobal[i].numCompInList < history_depth) {
                    historyGlobal[i].numCompInList++ ;
                } else if (historyGlobal[i].numCompInList >= history_depth) {
                    //Remove last component from the coresponding history list entry
                    CTXTRACE_DEBUG_LOG("Removing last component from list %d",i);
                    RemoveLastFromHistory(i);
                }
                //Insert the new component into the existing list
                LIST_INSERT_HEAD(&historyGlobal[i].ctx_trace_history_list, history, list_entry);
                break; //break from the loop
            }
        }

        //Component not found in existing list- Create new list entry
        if (list_existing == false) {
            CTXTRACE_DEBUG_LOG("Component %s not found - Create new list %d "
                                ,history->wrapperName,historyGlobalIdx);
            //Configure the new list entry
            historyGlobal[historyGlobalIdx].numCompInList = 1 ;
            strcpy(historyGlobal[historyGlobalIdx].wrapperName1,history->wrapperName);

            //Insert the new component in the new list
            LIST_INIT(&historyGlobal[historyGlobalIdx].ctx_trace_history_list);
            LIST_INSERT_HEAD(&historyGlobal[historyGlobalIdx].ctx_trace_history_list, history, list_entry);
            //Increment the number of unique components
            historyGlobalIdx ++;
        }
        UnLock();

        // Free the list of param
        for (int i = 0; i < indexSaveIdx; i++) {
            if (indexSave[i].pData) {
                free(indexSave[i].pData);
                //Reset the values in the indexSave explicitly to avoid false positives in set/get param
                indexSave[i].pData = NULL;
                indexSave[i].nIndex =  OMX_IndexComponentStartUnused;
                indexSave[i].errorCode = OMX_ErrorNone;
            }
        }
    } else {
        ALOGE("MoveFileToHistory failed to allocate history elem");
    }
}

void ENS_Wrapper_CtxTrace::RemoveAllFromHistory(void)
{
    CTXTRACE_DEBUG_LOG("RemoveAllFromHistory");
    Lock();
    ctx_trace_history_list_elem_t *elem1 = NULL;
    ctx_trace_history_list_elem_t *elem2 = NULL;
    for (int i = 0; i < historyGlobalIdx; i++) {
        int index = 0;
        LIST_FOREACH_SAFE(elem1, &historyGlobal[i].ctx_trace_history_list, list_entry, elem2) {
            CTXTRACE_DEBUG_LOG("RemoveAllFromHistory Removing %s_0%d",historyGlobal[i].wrapperName1,index);
            LIST_REMOVE(elem1, list_entry);
            if (elem1->pLog) { free(elem1->pLog); elem1->pLog=NULL;}
            if (elem1->ctxTrace) { free(elem1->ctxTrace);elem1->ctxTrace=NULL; }
            if (elem1) free(elem1);
            elem1=NULL;
            index++ ;
        }
        //Reset the values
        historyGlobal[i].numCompInList = 0;
        strcpy(historyGlobal[i].wrapperName1,"\0");
    }
    historyGlobalIdx = 0; //reset counter
    UnLock();
}
void ENS_Wrapper_CtxTrace::RemoveLastFromHistory(int list_index)
{
    CTXTRACE_DEBUG_LOG("RemoveLastFromHistory for list %d",list_index);
    Lock();
    ctx_trace_history_list_elem_t * elem1 = NULL;
    ctx_trace_history_list_elem_t * elem2 = NULL;
    int index = 0;
    LIST_FOREACH_SAFE(elem1, &historyGlobal[list_index].ctx_trace_history_list, list_entry, elem2) {
        if (LIST_NEXT((elem1), list_entry) == NULL) {
            CTXTRACE_DEBUG_LOG("RemoveLastFromHistory Removing %s_0%d"
                        ,historyGlobal[list_index].wrapperName1,index);
            LIST_REMOVE(elem1, list_entry);
            if (elem1->pLog) { free(elem1->pLog); elem1->pLog=NULL;}
            if (elem1->ctxTrace) { free(elem1->ctxTrace);elem1->ctxTrace=NULL; }
            if (elem1) free(elem1);
            elem1=NULL;
            UnLock();
            return;
        }
        index++ ;
    }
    UnLock();
}
/** Dump the status of an active component to the logcat. */
void ENS_Wrapper_CtxTrace::DumpActiveComponent(FILE *file)
{
    static char log[CTX_TRACE_CMP_LOG_MAXSIZE];
    FillTheLog(this, log, CTX_TRACE_CMP_LOG_MAXSIZE);

    if (file == NULL) {
        ALOGI("|----------------------------------------------------------------\n");
        ALOGI("|- ACTIVE COMPONENT [%s]\n", wrapper->name);
        // Special LOGI for big buffer
        SendOutLog(log);
    } else {
        fprintf(file, "|----------------------------------------------------------------\n");
        fprintf(file, "|- ACTIVE COMPONENT [%s]\n", wrapper->name);
    }
}

/**
 * Send the status of an inactive component to the logcat.
 */
void ENS_Wrapper_CtxTrace::DumpInactiveComponent(FILE *file,ctx_trace_history_list_elem_t *elem,int list_index,int index)
{

    CTXTRACE_DEBUG_LOG("DumpInactiveComponent list_index %d index %d of %s\n",
                        list_index,index,historyGlobal[list_index].wrapperName1);
    if (file == NULL) {
        ALOGI("|----------------------------------------------------------------\n");
        if (!index) {
            ALOGI("|- %s: closed component [%s]", elem->timestamp, historyGlobal[list_index].wrapperName1);
        } else {
            ALOGI("|- %s: closed component [%s_0%d]", elem->timestamp, historyGlobal[list_index].wrapperName1,index);
        }
        // Special LOGI for big buffer
        SendOutLog(elem->pLog);
    } else {
        fprintf(file, "|----------------------------------------------------------------\n");
        if (!index) {
            fprintf(file, "|- %s: closed component [%s]\n",
                    elem->timestamp, historyGlobal[list_index].wrapperName1);
        } else {
            fprintf(file, "|- %s: closed component [%s_0%d]\n",
                    elem->timestamp, historyGlobal[list_index].wrapperName1,index);
        }
        fprintf(file, "%s\n", elem->pLog);
    }
}

void ENS_Wrapper_CtxTrace::FillTheLog(ENS_Wrapper_CtxTrace *ctxTrace,
                                      char *logBuf,
                                      int size)
{
    OMX_ERRORTYPE result;

    static char tmpLogBuf[CTX_TMPLOG_SIZE];
    static char tmpLogParamConfBuf[CTX_TMPLOG_PARAM_CONFIG_SIZE];
    static char tmpStruct[CTX_PARAM_CONFIG_STRUCT_MAX_SIZE];

    CTXTRACE_DEBUG_LOG("FillTheLog ctxTrace=0x%x logBuf=0x%x size=%d",
                       (unsigned int) ctxTrace, (int) logBuf, size);

    logBuf[0] = 0;
    int nb_char = 0;

    // StandardComponentRole
    OMX_PARAM_COMPONENTROLETYPE componentRole;
    componentRole.nSize = sizeof(OMX_PARAM_COMPONENTROLETYPE);
    componentRole.nVersion.nVersion = OMX_VERSION;
    OMX_HANDLETYPE_CALL(ctxTrace->wrapper->getOMX_Component(), GetParameter,
                        OMX_IndexParamStandardComponentRole, &componentRole);

    PRINT_TO_BUFFER(logBuf, nb_char, size,
                               "|- StandardComponentRole : %s\n",
                               componentRole.cRole);

    // Handle
    PRINT_TO_BUFFER(logBuf, nb_char, size,
                    "|- Handle : 0x%08x\n",
                    ctxTrace->wrapper->getOMX_Component());

    // Errors in SetParamOrConfig
    if ( ctxTrace->NbOfSetParamAndConfigErrors > 0) {
        PRINT_TO_BUFFER(logBuf, nb_char, size,
                                   "|---------- ! ERRORS ! \n");
        for (int i = 0; i < ctxTrace->indexSaveIdx; i++) {
            if ( ctxTrace->indexSave[i].errorCode != OMX_ErrorNone) {
                        memcpy(tmpStruct, ctxTrace->indexSave[i].pData, ctxTrace->indexSave[i].dataSize);
                        strOMX_INDEXTYPE_struct(ctxTrace->indexSave[i].nIndex, tmpStruct,
                                2 /* level = 2 : dump members */,
                                0 /* prio */,
                                NULL /* write to buffer */,
                                "|- ERROR" /* prefix */,
                                tmpLogParamConfBuf, CTX_TMPLOG_PARAM_CONFIG_SIZE);

                        PRINT_TO_BUFFER(logBuf, nb_char, size,
                                             "|- ERROR --%s SetParameterOrConfig-Failure %s\n%s",
                                        ctxTrace->indexSave[i].timestamp, OMX_TYPE_TO_STR(OMX_ERRORTYPE, ctxTrace->indexSave[i].errorCode), tmpLogParamConfBuf);

            }
        }
    }

    //Error(s) in SendCommands
    if (ctxTrace->NbOfSendCmdErrors > 0) {
        PRINT_TO_BUFFER(logBuf, nb_char, size,
                                   "|---------- ! ERRORS ! \n");
        for (int i = 0; i < ctxTrace->cmdSaveIdx; i++) {
            if ((ctxTrace->cmdSave[i].Cmd != (OMX_COMMANDTYPE) -1) &&
                (ctxTrace->cmdSave[i].errorCode != OMX_ErrorNone)) {
                    PRINT_TO_BUFFER(logBuf,nb_char,size,
                                    "|-  ERROR -- %s : SendCommand-Failure  %s\n",
                                    ctxTrace->cmdSave[i].timestamp,
                                    OMX_TYPE_TO_STR(OMX_ERRORTYPE, ctxTrace->cmdSave[i].errorCode)
                    );
            }
        }
    }

    // Ports
    OMX_PORT_PARAM_TYPE portype;
    portype.nSize = sizeof(OMX_PORT_PARAM_TYPE);
    portype.nVersion.nVersion = OMX_VERSION;
    // Ports
    char *timestamp;
    OMX_ERRORTYPE errorCode = OMX_ErrorNone;
    OMX_INDEXTYPE indexTabInit[] = { OMX_IndexParamAudioInit,
                                     OMX_IndexParamImageInit,
                                     OMX_IndexParamVideoInit,
                                     OMX_IndexParamOtherInit };


    for (int i = 0; i < (int)(sizeof(indexTabInit)/sizeof(OMX_INDEXTYPE)); i++) {

        if ((OMX_HANDLETYPE_CALL(ctxTrace->wrapper->getOMX_Component(), GetParameter,
            indexTabInit[i], &portype) == OMX_ErrorNone) && portype.nPorts) {
            PRINT_TO_BUFFER(logBuf, nb_char, size,
                                           "|- ports\n");
            for (int portIndex = portype.nStartPortNumber ;
                 portIndex < (int)(portype.nStartPortNumber + portype.nPorts) ;
                 portIndex++) {
                //Query current port definition
                if ((result = ctxTrace->GetParamAndConfig(OMX_IndexParamPortDefinition, tmpStruct,
                                                          &timestamp, portIndex)) == OMX_ErrorNone) {
                    PRINT_TO_BUFFER(logBuf, nb_char, size,
                                           "|---------- port %d ----------\n", portIndex);
                    strOMX_INDEXTYPE_struct(OMX_IndexParamPortDefinition, tmpStruct,
                                            2 /* level = 2 : dump members */,
                                            0 /* prio */,
                                            NULL /* write to buffer */,
                                            "|" /* prefix */,
                                            tmpLogParamConfBuf, CTX_TMPLOG_PARAM_CONFIG_SIZE);

                    PRINT_TO_BUFFER(logBuf, nb_char, size,
                                               "|-- %s \n%s",
                                               timestamp, tmpLogParamConfBuf);
                }
                // Query current buffer supplier
                if ((result = ctxTrace->GetParamAndConfig(OMX_IndexParamCompBufferSupplier, tmpStruct,
                                                          &timestamp, portIndex)) == OMX_ErrorNone) {
                    PRINT_TO_BUFFER(logBuf, nb_char, size,
                                "|---------- port %d ----------\n", portIndex);
                    strOMX_INDEXTYPE_struct(OMX_IndexParamCompBufferSupplier, tmpStruct,
                                            2 /* level = 2 : dump members */,
                                            0 /* prio */,
                                            NULL /* write to buffer */,
                                            "|" /* prefix */,
                                            tmpLogParamConfBuf, CTX_TMPLOG_PARAM_CONFIG_SIZE);

                    PRINT_TO_BUFFER(logBuf, nb_char, size,
                                               "|-- %s \n%s",
                                               timestamp, tmpLogParamConfBuf);
                }
            }
        }
    }
    // Parameters and configs
    PRINT_TO_BUFFER(logBuf, nb_char, size,
                               "|---------- parameters and configs ----------\n");

    for (int i = 0; i < ctxTrace->indexSaveIdx; i++) {
        OMX_INDEXTYPE nIndex = ctxTrace->indexSave[i].nIndex;
        switch (nIndex) {
            // Do not display param&config already used
        case OMX_IndexParamAudioInit:
        case OMX_IndexParamImageInit:
        case OMX_IndexParamVideoInit:
        case OMX_IndexParamOtherInit:
        case OMX_IndexParamStandardComponentRole:
        case OMX_IndexParamPortDefinition:
        case OMX_IndexParamCompBufferSupplier:
        case OMX_IndexParamAudioPortFormat:
        case OMX_IndexParamImagePortFormat:
        case OMX_IndexParamVideoPortFormat:
        case OMX_IndexParamOtherPortFormat:
            break;

        default :
                if ((result = ctxTrace->GetParamAndConfig(nIndex, tmpStruct,
                                                          &timestamp)) == OMX_ErrorNone) {
                    strOMX_INDEXTYPE_struct(nIndex, tmpStruct,
                                            2 /* level = 2 : dump members */,
                                            0 /* prio */,
                                            NULL /* write to buffer */,
                                            "|" /* prefix */,
                                            tmpLogParamConfBuf, CTX_TMPLOG_PARAM_CONFIG_SIZE);

                    PRINT_TO_BUFFER(logBuf, nb_char, size,
                                               "|-- %s \n%s",
                                               timestamp, tmpLogParamConfBuf);

                }
        }
    }

   // Commands
    PRINT_TO_BUFFER(logBuf , nb_char,size,
                        "|---------- commands history (max length: %d)\n", CTX_TRACE_CMD_SIZE);
    int index = 0;
    for (int i = ctxTrace->cmdSaveIdx; i < CTX_TRACE_CMD_SIZE; i++) {
        if (ctxTrace->cmdSave[i].Cmd != (OMX_COMMANDTYPE) -1) {
            nb_char += snprintf(logBuf + nb_char,
                                ( size - nb_char > 0 ? size - nb_char : 0),
                                "|  (%d) %s : %s\n",
                                index++,
                                ctxTrace->cmdSave[i].timestamp,
                                strOMX_COMMAND(ctxTrace->cmdSave[i].Cmd,
                                               ctxTrace->cmdSave[i].nParam,
                                               tmpLogBuf,CTX_TMPLOG_SIZE));
        }
    }
    for (int i = 0; i < ctxTrace->cmdSaveIdx; i++) {
        if (ctxTrace->cmdSave[i].Cmd != (OMX_COMMANDTYPE) -1) {
            PRINT_TO_BUFFER(logBuf,nb_char,size,
                                "|  (%d) %s : %s  %s\n",
                                index++, ctxTrace->cmdSave[i].timestamp,
                                strOMX_COMMAND(ctxTrace->cmdSave[i].Cmd,
                                               ctxTrace->cmdSave[i].nParam,
                                               tmpLogBuf, CTX_TMPLOG_SIZE),
                                (ctxTrace->cmdSave[i].errorCode != OMX_ErrorNone ?
                                OMX_TYPE_TO_STR(OMX_ERRORTYPE, ctxTrace->cmdSave[i].errorCode) : "")
                );
        }
    }
    PRINT_TO_BUFFER(logBuf,nb_char,size,
                        "|---------- CommandWaiting = %d\n",
                        ctxTrace->wrapper->getCommandWaiting());

    // private context
    PRINT_TO_BUFFER(logBuf,nb_char,size,
                        "|---------- Private context\n");

    // Private context
    ctx_trace_PrivateContextPrint_t printContext;
    printContext.magic    = PRIVATECONTEXTPRINT_MAGIC;
    printContext.context_counter=0; //To initialize the counter.
    printContext.buffer   = &logBuf;
    printContext.size     = size;
    printContext.nb_char  = nb_char;

    OMX_OTHER_PARAM_PRIVATE_CONTEXT privateContext ;

    privateContext.pPrintPrivate = &printContext;
    privateContext.pPrint = ctxTracePrivateContextPrint;
    privateContext.pInc = ctxTracePrivateContextPInc;
    privateContext.pDec = ctxTracePrivateContextPDec;

    getPrivateContext(ctxTrace->wrapper->getOMX_Component(),&privateContext);

    // Let's wait for contributors to finish private async dump.
    // This is probably already finished, though, let's check this
    PRIVATE_INCDEC_LOCK;
    int val = printContext.context_counter;
    if (val!=0) {
        int retcode = 0;
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += 500000000;
        PRIVATE_INCDEC_UNLOCK;
        // FIXME : consider better use of pthread_cond function family here
        retcode = pthread_cond_timedwait(&private_async_cond,NULL, &timeout);
        if (retcode == ETIMEDOUT) {
            LOGE(" Timeout of 500ms expired : private context might be incomplete");
            result = OMX_ErrorTimeout;
        }
    } else {
        PRIVATE_INCDEC_UNLOCK;
    }
}

int ENS_Wrapper_CtxTrace::getPrivateContext(OMX_COMPONENTTYPE * pHandle, OMX_OTHER_PARAM_PRIVATE_CONTEXT * pPrivateContex) {
    //Call the Asynchronous interface of the component. Component will return NOT implemented if not supported
     int result = OMX_HANDLETYPE_CALL(pHandle, GetParameter,
                                 OMX_IndexParamOtherPrivateContext,
                                 pPrivateContex);
     return result;
}

/**  Logcat limit string to 1024 - send line by line */
void ENS_Wrapper_CtxTrace::SendOutLog(char *log)
{
    char *tolog = log;
    size_t length = strlen(log);
    for (size_t k = 0; k <= length; k++) {
        if (log[k] == '\n') {
            log[k] = '\0';
            ALOGI("%s", tolog);
            log[k] = '\n';
            tolog = log + k + 1;
        }
    }
}

/** Save the index of the parameters and configs used by the current component */
void ENS_Wrapper_CtxTrace::SetParamOrConfig(OMX_INDEXTYPE nIndex,
                                            OMX_PTR pData,
                                            OMX_ERRORTYPE result)
{
    // Get the size of the data
    int dataSize = *((OMX_U32*) pData);
    int portIndex = -1;
    if(nIndex == OMX_IndexParamCompBufferSupplier){
        portIndex = ((OMX_PARAM_BUFFERSUPPLIERTYPE*)pData)->nPortIndex;
    }
    if(nIndex == OMX_IndexParamPortDefinition){
        portIndex = ((OMX_PARAM_PORTDEFINITIONTYPE*)pData)->nPortIndex;
    }
    if ((dataSize < 0) || (dataSize > CTX_PARAM_CONFIG_STRUCT_MAX_SIZE)) {
        ALOGE("SetParamOrConfig got invalid nSize=%d - ignoring", dataSize);
        return;
    }

    // Save if not already done
    bool existing = false;
    int i;
    char tmp[CTX_TMPLOG_SIZE];
    struct timeval tv = { 0 , 0 };
    gettimeofday(&tv,NULL);

    for (i = 0; i < indexSaveIdx; i++) {
        if ((nIndex == indexSave[i].nIndex) && (NULL!=indexSave[i].pData)){
            //if SetParamOrConfig is done twice(or multiple-times) for the Same Index,
            //And if it has an error,it will be stored at the the different location
            if ( result != indexSave[i].errorCode) {
                continue;
            }
            if(nIndex == OMX_IndexParamPortDefinition) {
               if(portIndex == indexSave[i].portIndex) {
                   existing = true;
                   break;
               } else {
                   existing = false;
               }
            }
            else if(nIndex == OMX_IndexParamCompBufferSupplier) {
                if(portIndex == indexSave[i].portIndex) {
                    existing = true;
                    break;
                } else {
                    existing = false;
                }
            }
            else {
            //Entry stored for the nIndex already with valid address
                existing = true;
                break;
            }
        }
    }
    if (existing == false) {
        if (indexSaveIdx < CTX_TRACE_INDEX_SIZE) {
            // Store the param/config in the list
            indexSave[i].nIndex = nIndex;
            indexSave[i].pData = malloc(dataSize);
            indexSave[i].portIndex = portIndex;
            indexSave[i].dataSize = dataSize;
            indexSave[i].errorCode = result;
            indexSaveIdx++;
            if (indexSave[i].errorCode != OMX_ErrorNone)
                NbOfSetParamAndConfigErrors++;
        } else {
            ALOGW("SetParamOrConfig reached context limit of %d for h=0x%x discarding %s",
                 CTX_TRACE_INDEX_SIZE,
                 (unsigned int) wrapper->getOMX_Component(),
                 strOMX_INDEXTYPE(nIndex, tmp, CTX_TMPLOG_SIZE));
        }
    }

    // Copy the latest data
    if (indexSave[i].pData) {
        memcpy(indexSave[i].pData, pData, dataSize);
        stelp_get_timeofday(indexSave[i].timestamp, CTX_TRACE_TIMESTAMP_SIZE);
        indexSave[i].errorCode = result;
    }
    CTXTRACE_DEBUG_LOG("SetParamOrConfig %s existing=%d nIndex=%s h=0x%x"
                       " position=%d indexSaveIdx total %d indexSave.pdata=0x%x pData actual=0x%x size=%d port=%d timestamp=%s errorCode=%s",
                       wrapper->name,existing,strOMX_INDEXTYPE(nIndex, tmp, CTX_TMPLOG_SIZE),
                       (unsigned int) wrapper->getOMX_Component(),i,indexSaveIdx, (int)indexSave[i].pData,(int)pData,
                       dataSize,portIndex, indexSave[i].timestamp, OMX_TYPE_TO_STR(OMX_ERRORTYPE, indexSave[i].errorCode));
}


OMX_ERRORTYPE ENS_Wrapper_CtxTrace::GetParamAndConfig(OMX_INDEXTYPE nIndex,
                                                      OMX_PTR pData,
                                                      char **timestamp, int portIndex)
{
    OMX_ERRORTYPE result = (OMX_ERRORTYPE)-1;
    *timestamp = NULL;

    // Find the param/config in the list
    for (int i = 0; i < indexSaveIdx; i++) {
        if ((nIndex == indexSave[i].nIndex) && (nIndex != OMX_IndexComponentStartUnused) && (portIndex == indexSave[i].portIndex) && (OMX_ErrorNone == indexSave[i].errorCode)) {
            CTXTRACE_DEBUG_LOG("GetParamAndConfig %s position=%d nIndex=%s port=%d pData=0x%x timestamp %s ",
                               wrapper->name, i, OMX_TYPE_TO_STR(OMX_INDEXTYPE, nIndex),portIndex,(int)indexSave[i].pData,
                               indexSave[i].timestamp);
            if (indexSave[i].pData) {
                memcpy(pData, indexSave[i].pData, indexSave[i].dataSize);
             }
             *timestamp = indexSave[i].timestamp;
              result = OMX_ErrorNone;
         }
    }

    return(result);
}


/** Save the index of the parameters and configs used by the current component.
 *  Do not save the index specifique to the ports. */
void ENS_Wrapper_CtxTrace::SendCommand(OMX_COMMANDTYPE Cmd,
                                       OMX_U32 nParam,
                                       OMX_PTR pCmdData,
                                       OMX_ERRORTYPE errorCode )
{
    CTXTRACE_DEBUG_LOG("SendCommand %s Cmd 0x%x, nParam 0x%x, pCmdData 0x%x",wrapper->name,
                       (int)Cmd, (int) nParam, (int) pCmdData);

    struct timeval tv = { 0 , 0 };
    gettimeofday(&tv,NULL);

    cmdSave[cmdSaveIdx].Cmd = Cmd;
    cmdSave[cmdSaveIdx].nParam = nParam;
    cmdSave[cmdSaveIdx].errorCode = errorCode;
    if (cmdSave[cmdSaveIdx].errorCode != OMX_ErrorNone)
        NbOfSendCmdErrors++;
    stelp_get_timeofday(cmdSave[cmdSaveIdx].timestamp, CTX_TRACE_TIMESTAMP_SIZE);

    cmdSaveIdx = (cmdSaveIdx + 1) % CTX_TRACE_CMD_SIZE;
}

void ENS_Wrapper_CtxTrace::getCurrentTime(char *buffer, size_t size)
{
    struct timeval tv;
    struct tm tm;
    size_t pos;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);

    pos = strftime(buffer, size, "%m-%d_%H:%M:%S", &tm);
    snprintf(buffer + pos, size - pos > 0 ? size - pos : 0,
             ".%06d", (int) tv.tv_usec);
}

void ENS_Wrapper_CtxTrace::PrintHelp(void)
{
    ALOGI("####################  VALID COMMANDS FOR CONTEXT TRACE  ########################\n");
    ALOGI("## Clear the history of closed OMX components\n");
    ALOGI("# echo clear > %s\n", filename);
    ALOGI("## Dump to a file in /data/ste-debug the current status (all components)\n");
    ALOGI("# echo dump > %s\n", filename);
    ALOGI("## Dump to log the current status (all components)\n");
    ALOGI("# echo log > %s\n", filename);
    ALOGI("## Dump to a file in /data/ste-debug the current status (all opened components)\n");
    ALOGI("# echo dump-opened > %s\n", filename);
    ALOGI("## Dump to log the current status (all opened components)\n");
    ALOGI("# echo log-opened > %s\n", filename);
    ALOGI("## Dump to a file in /data/ste-debug the current status (all closed components)\n");
    ALOGI("# echo dump-closed > %s\n", filename);
    ALOGI("## Dump to log the current status (all closed components)\n");
    ALOGI("# echo log-closed > %s\n", filename);
    ALOGI("## Print this help menu in logcat\n");
    ALOGI("# echo help > %s\n", filename);
    ALOGI("## Dump to log the status of a given component\n");
    ALOGI("## if component is closed, latest closed status is dumped\n");
    ALOGI("# echo <component name > > %s\n", filename);
    ALOGI("# Ex:echo OMX.STE.HVA.VIDEO.ENCODER.AVC > %s\n", filename);
    ALOGI("# Ex:echo OMX.STE.HVA.VIDEO.ENCODER.AVC_02 > %s\n", filename);
    ALOGI("## Dump to log the private context of all opened components\n");
    ALOGI("# echo log-private > %s\n", filename);
    ALOGI("%s\n", SUMMARY_HEADER);
}
void ENS_Wrapper_CtxTrace::WriteSummary()
{
    static char activeList[CTX_COMPONENT_LIST_SIZE];
    FILE *file;
    CTXTRACE_DEBUG_LOG("WriteSummary()");
    char currentTime[CTX_TRACE_TIMESTAMP_SIZE];
    stelp_get_timeofday(currentTime, CTX_TRACE_TIMESTAMP_SIZE);

    Lock();

    if ((file = fopen(filename, "w+")) == NULL) {
        ALOGE("open of '%s' for writing failed: %s", filename, strerror(errno));
        UnLock();
        return;
    }

    fprintf(file, "%s\n", SUMMARY_HEADER);
    fprintf(file, "# Last update: %s\n", currentTime);
    fprintf(file, "# STE OMX wrapper debug file of process '%s' pid=%d\n",
            processname, (int) getpid());
    fprintf(file, "#### Supported actions:\n");
    fprintf(file, "## Clear the history of closed OMX components\n");
    fprintf(file, "# echo clear > %s\n", filename);
    fprintf(file, "## Dump to a file in /data/ste-debug the current status (all components)\n");
    fprintf(file, "# echo dump > %s\n", filename);
    fprintf(file, "## Dump to log the current status (all components)\n");
    fprintf(file, "# echo log > %s\n", filename);
    fprintf(file, "## Dump to a file in /data/ste-debug the current status (all opened components)\n");
    fprintf(file, "# echo dump-opened > %s\n", filename);
    fprintf(file, "## Dump to log the current status (all opened components)\n");
    fprintf(file, "# echo log-opened > %s\n", filename);
    fprintf(file, "## Dump to a file in /data/ste-debug the current status (all closed components)\n");
    fprintf(file, "# echo dump-closed > %s\n", filename);
    fprintf(file, "## Dump to log the current status (all closed components)\n");
    fprintf(file, "# echo log-closed > %s\n", filename);
    fprintf(file, "## Print this help menu in logcat \n");
    fprintf(file, "# echo help > %s\n", filename);
    fprintf(file, "## Dump to log the status of a given component\n");
    fprintf(file, "## if component is closed, latest closed status is dumped\n");
    fprintf(file, "# echo <component name> > %s\n", filename);
    fprintf(file, "## Dump to log the private context of all opened components\n");
    fprintf(file, "# echo log-private > %s\n", filename);
    fprintf(file, "%s\n", SUMMARY_HEADER);

    //List of active components
    unsigned long nbActiveComponents = ENS_Wrapper::NbActiveComponents();
    fprintf(file, "##List of Active components##\n");
    fprintf(file, "-----------------------------\n");

    for (unsigned long i = 0; i<nbActiveComponents; i++) {
        OMX_COMPONENTTYPE *hComp = ENS_Wrapper::GetOMX_COMPONENTTYPEforIndex(i);
        if (hComp == NULL) {
            ALOGE("WriteSummary: invalid index: %lud", i);
            continue;
        } else {
            ENS_Wrapper *wrapper = OMX_HANDLETYPE_TO_WRAPPER(hComp);
            fprintf(file,"%s\n",wrapper->name);
        }
    }
    // List of history components
    fprintf(file, "\n##List of History components##\n");
    fprintf(file, "--------------------------------\n");

    ctx_trace_history_list_elem_t *elem = NULL;
    for (int i = 0; i < historyGlobalIdx; i++) {
        int index = 0;
        LIST_FOREACH(elem, &historyGlobal[i].ctx_trace_history_list, list_entry) {
            if (!index) {
                fprintf(file, "%s: %s\n", elem->timestamp,historyGlobal[i].wrapperName1);
            } else {
                fprintf(file, "%s: %s_0%d\n", elem->timestamp,historyGlobal[i].wrapperName1,index);
            }
            index++;
        }
    }
    fclose(file);
    UnLock();
}

void ENS_Wrapper_CtxTrace::LogPrivateState() {
    int maxsize = 100*1024*sizeof(char);
    int size = 0;
    char *private_context = new char[maxsize];
    unsigned long nbActiveComponents = ENS_Wrapper::NbActiveComponents();

    // Private context
    ctx_trace_PrivateContextPrint_t* pPrintContext = new(ctx_trace_PrivateContextPrint_t);
    pPrintContext->magic    = PRIVATECONTEXTPRINT_MAGIC;
    pPrintContext->buffer   = &private_context;
    pPrintContext->size     = maxsize;
    pPrintContext->nb_char  = 0;

    pPrintContext->context_counter = 0; //To initialize the counter.

    OMX_OTHER_PARAM_PRIVATE_CONTEXT * pPrivateContext = new(OMX_OTHER_PARAM_PRIVATE_CONTEXT) ;

    pPrivateContext->pPrintPrivate = pPrintContext;
    pPrivateContext->pPrint = ctxTracePrivateContextPrint;
    pPrivateContext->pInc = ctxTracePrivateContextPInc;
    pPrivateContext->pDec = ctxTracePrivateContextPDec;


    if (nbActiveComponents == 0) {
        LOGI("No active OMX components found on process '%s' pid=%d\n",
             processname, (int) getpid());
        goto bail;
    }

    pPrivateContext->pPrint(pPrivateContext->pPrintPrivate,
                            "Dumping OMX components private context of process"
                            " '%s' pid=%d ...\n"
                            "%d active components found:\n",
                            processname, (int) getpid(),
                            nbActiveComponents);
    for (unsigned long i = 0; i < nbActiveComponents; i++) {
        OMX_COMPONENTTYPE *hComp = ENS_Wrapper::GetOMX_COMPONENTTYPEforIndex(i);
        int private_size;

        // Prefix by component name
        pPrivateContext->pPrint(pPrivateContext->pPrintPrivate,
                                ".\n[%s]\n"
                                "|- handle = 0x%08x\n",
                                OMX_HANDLETYPE_GET_NAME(hComp),
                                hComp);
        // Dump component private status (formatted string)
        getPrivateContext(hComp,pPrivateContext);
    }

    PRIVATE_INCDEC_LOCK;
    if (pPrintContext->context_counter != 0) {
        // Let's wait for contributors to finish private dump.
        // This is probably already finished, though, let's check this

        int retcode = 0;
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += 500000000;
        PRIVATE_INCDEC_UNLOCK;
        // FIXME : consider better use of cond_ function family here
        retcode = pthread_cond_timedwait(&private_async_cond, NULL, &timeout);
        if (retcode == ETIMEDOUT) {
            LOGE(" Timeout of 500ms expired . Incomplete dump of private state");
        }
    } else {
        PRIVATE_INCDEC_UNLOCK;
    }
    CTXTRACE_DEBUG_LOG(" Dump of private context done");

    char* strtok_ctx;
    char* strline;
    for (strline = strtok_r(private_context, "\n", &strtok_ctx);
          strline;
          strline=strtok_r(NULL, "\n", &strtok_ctx))
    {
        LOGI("%s",strline);
    }

 bail:
    delete(private_context);
}




void ENS_Wrapper_CtxTrace::DumpOrLog(bool toLog, bool opened, bool closed,bool withtimestamp)
{
    FILE *dumpfile = NULL;
    char dumpFileName[CTX_TRACE_STRINGNAME_SIZE];
    if (!toLog) {
        if (withtimestamp) {
            char currentTime[CTX_TRACE_TIMESTAMP_SIZE];
            stelp_get_timeofday(currentTime, CTX_TRACE_TIMESTAMP_SIZE);
            char * space=0;;
            while((space=strstr(currentTime," "))) {
                *space = '_';
            }
            snprintf(dumpFileName, CTX_TRACE_STRINGNAME_SIZE,
                    CTXTRACE_DIR "_%s-%s.txt", currentTime, processname);
        } else {

            snprintf(dumpFileName, CTX_TRACE_STRINGNAME_SIZE,
                    CTXTRACE_DIR "-%s.txt", processname);

        }
        dumpfile = fopen(dumpFileName, "w+");
        if (dumpfile == NULL) {
            ALOGE("DumpOrLog failed to open %s file for writing log, defaulting to log dump",
                    dumpFileName);
        }
    }
    if (opened) {
        unsigned long nbActiveComponents = ENS_Wrapper::NbActiveComponents();
        for (unsigned long i = 0; i < nbActiveComponents; i++) {
            OMX_COMPONENTTYPE *hComp = ENS_Wrapper::GetOMX_COMPONENTTYPEforIndex(i);
            if (hComp == NULL) {
                ALOGE("DumpOrLog: invalid index: %lud", i);
                continue;
            } else {
                ENS_Wrapper *wrapper = OMX_HANDLETYPE_TO_WRAPPER(hComp);
                static char log[CTX_TRACE_CMP_LOG_MAXSIZE];
                wrapper->ctxTrace->DumpActiveComponent(dumpfile);
            }
        }
    }
    if (closed) {
        ctx_trace_history_list_elem_t *elem = NULL;
        for (int i = 0; i < historyGlobalIdx; i++) {
            int index = 0;
            LIST_FOREACH(elem, &historyGlobal[i].ctx_trace_history_list, list_entry) {
                elem->ctxTrace->DumpInactiveComponent(dumpfile,elem,i,index);
                index++;
            }
        }
    }
    if (dumpfile) {
        fclose(dumpfile);
        sync();
        ALOGI("Wrote %s", dumpFileName);
    }
}
