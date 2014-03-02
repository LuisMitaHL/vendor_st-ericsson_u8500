/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ENS_WRAPPER_CTXTRACE_H_
#define _ENS_WRAPPER_CTXTRACE_H_

#include <stdio.h>

#include "ENS_Wrapper.h"

#define CTX_TRACE_STRINGNAME_SIZE 256
#define CTX_TRACE_INDEX_SIZE 128
#define CTX_TRACE_CMD_SIZE 20
#define CTX_TRACE_TIMESTAMP_SIZE 128
#define CTX_TRACE_NUMBER_OF_UNIQUE_HISTORY_COMPONENTS 100

typedef struct ctx_trace_indexSave {
    OMX_INDEXTYPE nIndex;
    OMX_PTR pData;
    int dataSize;
    int portIndex;
    OMX_ERRORTYPE errorCode;
    char timestamp[CTX_TRACE_TIMESTAMP_SIZE];
} ctx_trace_indexSave_t;

typedef struct ctx_trace_cmdSave {
    OMX_COMMANDTYPE Cmd;
    OMX_U32 nParam;
    char timestamp[CTX_TRACE_TIMESTAMP_SIZE];
    OMX_ERRORTYPE errorCode;
} ctx_trace_cmdSave_t;

typedef struct ctx_trace_PrivateContextPrint {
    OMX_U32 magic;
    int context_counter;
    char **buffer;
    size_t size;
    size_t nb_char;
} ctx_trace_PrivateContextPrint_t;

/********************************************************************************
 * List element to store history of the ens components
 ********************************************************************************/

class ENS_Wrapper_CtxTrace;

typedef struct ctx_trace_history_list_elem {
    LIST_ENTRY(ctx_trace_history_list_elem) list_entry;
    char *pLog;
    ENS_Wrapper_CtxTrace *ctxTrace;
    char wrapperName[CTX_TRACE_STRINGNAME_SIZE];
    char timestamp[CTX_TRACE_TIMESTAMP_SIZE];
} ctx_trace_history_list_elem_t;

typedef LIST_HEAD(ctx_trace_history_list_head, ctx_trace_history_list_elem) ctx_trace_history_list_head_t;

typedef struct ctx_trace_history_list_global {
    unsigned int numCompInList;
    char wrapperName1[CTX_TRACE_STRINGNAME_SIZE];
    struct ctx_trace_history_list_head ctx_trace_history_list;
} ctx_trace_history_list_global_t;

class ENS_Wrapper_CtxTrace {

    ENS_Wrapper *wrapper;
    ctx_trace_history_list_elem_t *history;
    static int inotify_fd;
    static char processname[CTX_TRACE_STRINGNAME_SIZE];
    static char filename[CTX_TRACE_STRINGNAME_SIZE];

    /* list containing the history of the components */
    static ctx_trace_history_list_global_t historyGlobal[CTX_TRACE_NUMBER_OF_UNIQUE_HISTORY_COMPONENTS];
    static int historyGlobalIdx;

    /* Save the param & config */
    ctx_trace_indexSave_t indexSave[CTX_TRACE_INDEX_SIZE];
    int indexSaveIdx;
    int NbOfSetParamAndConfigErrors;

    /* Save the sent cmd */
    ctx_trace_cmdSave_t cmdSave[CTX_TRACE_CMD_SIZE];
    int cmdSaveIdx;
    int NbOfSendCmdErrors;

    static pthread_mutex_t mutex;

    static void Init(void);
    static void Lock(void);
    static void UnLock(void);
    static void DumpOrLog(bool toLog, bool opened, bool closed,bool withtimestamp);
    static void * Thread(void *);
    static void SendOutLog(char *log);
    static void RemoveAllFromHistory(void);
    static void RemoveLastFromHistory(int list_index);
    static void FillTheLog(ENS_Wrapper_CtxTrace *ctxTrace, char *logBuf, int size);
    static int getPrivateContext(OMX_COMPONENTTYPE * pHandle,OMX_OTHER_PARAM_PRIVATE_CONTEXT* context);
    static void LogPrivateState(void);


    void DumpActiveComponent(FILE *file);
    void DumpInactiveComponent(FILE *file,ctx_trace_history_list_elem_t *elem,int list_index,int index);

    OMX_ERRORTYPE GetParamAndConfig(OMX_INDEXTYPE nIndex,
                                    OMX_PTR pData,
                                    char **timestamp, int portIndex = -1);

  public:

    static bool enabled;
    static bool debug_enabled;
    static bool active_component;
    static bool latest_in_history;
    static unsigned int history_depth;

    ENS_Wrapper_CtxTrace(ENS_Wrapper *wrapper);
    ~ENS_Wrapper_CtxTrace();

    void MoveToHistory(void);
    static void PrintHelp(void);
    static void WriteSummary();
    static void getCurrentTime(char *buffer, size_t size);
    void SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData,OMX_ERRORTYPE errorCode);
    void SetParamOrConfig(OMX_INDEXTYPE nIndex, OMX_PTR pData,OMX_ERRORTYPE errorCode);
    static const char* GetFileName() { return filename;}

};

#endif /* _ENS_WRAPPER_CTXTRACE_H_ */


