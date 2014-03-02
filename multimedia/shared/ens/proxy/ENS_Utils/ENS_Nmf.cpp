/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Nmf.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_DBC.h"
#include "ENS_Trace.h"
#include "ENS_Nmf.h"
#define OMXCOMPONENT "ENS_NMF"
#include "osi_trace.h"
#include <unwind.h>
#include <stdio.h>
/* defines "memcpy" from libc */
#include <string.h>

#define MAX_FIFOSIZE_WARNING 15

typedef struct {
    t_cm_allocator_status   mpc_tcm24x;
    t_cm_allocator_status   mpc_tcm24y;
    t_cm_allocator_status   mpc_esram16;
    t_cm_allocator_status   mpc_esram24;
    t_cm_allocator_status   mpc_sdram16;
    t_cm_allocator_status   mpc_sdram24;
} t_memory_status_core;

typedef struct {
    t_memory_status_core    sia;
    t_memory_status_core    sva;
} t_memory_status;

typedef struct {
    int tcm24x;
    int tcm24y;
    int esram16;
    int esram24;
    int sdram16;
    int sdram24;
    int stack[3];
} t_memory_usage_core;

typedef struct {
    t_memory_usage_core    sia;
    t_memory_usage_core    sva;
} t_memory_usage;

static void getMemoryStatusCore(t_nmf_core_id core_id, t_memory_status_core* status) {
    t_cm_error error = CM_OK;
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_TCM24  , &status->mpc_tcm24x);
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_TCM24_Y, &status->mpc_tcm24y);
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_ESRAM16, &status->mpc_esram16);
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_ESRAM24, &status->mpc_esram24);
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_SDRAM16, &status->mpc_sdram16);
    error = CM_GetMpcMemoryStatusEx(core_id, CM_MM_MPC_SDRAM24, &status->mpc_sdram24);
}

void getMemoryStatus(t_memory_status* status) {
    getMemoryStatusCore(SIA_CORE_ID, &status->sia);
    getMemoryStatusCore(SVA_CORE_ID, &status->sva);
}

static void getMemoryUsageCore(t_nmf_core_id core_id, t_memory_status_core* status1, t_memory_status_core* status2, t_memory_usage_core* memory_usage) {
    memory_usage->tcm24x  = status1->mpc_tcm24x.global.accumulate_used_memory - status2->mpc_tcm24x.global.accumulate_used_memory;
    memory_usage->tcm24y  = status1->mpc_tcm24y.global.accumulate_used_memory - status2->mpc_tcm24y.global.accumulate_used_memory;
    memory_usage->esram16 = status1->mpc_esram16.global.accumulate_used_memory - status2->mpc_esram16.global.accumulate_used_memory;
    memory_usage->esram24 = status1->mpc_esram24.global.accumulate_used_memory - status2->mpc_esram24.global.accumulate_used_memory;
    memory_usage->sdram16 = status1->mpc_sdram16.global.accumulate_used_memory - status2->mpc_sdram16.global.accumulate_used_memory;
    memory_usage->sdram24 = status1->mpc_sdram24.global.accumulate_used_memory - status2->mpc_sdram24.global.accumulate_used_memory;
    for (int i=0; i<3; i++) {
        memory_usage->stack[i] = status1->mpc_tcm24x.stack[core_id].sizes[i] - status2->mpc_tcm24x.stack[core_id].sizes[i];
    }
}

void getMemoryUsage(t_memory_status* status1, t_memory_status* status2, t_memory_usage* memory_usage) {
    getMemoryUsageCore(SIA_CORE_ID, &status1->sia, &status2->sia, &memory_usage->sia);
    getMemoryUsageCore(SVA_CORE_ID, &status1->sva, &status2->sva, &memory_usage->sva);
}

static void traceMemoryUsageCore(const char* core, t_memory_usage_core* memory_usage) {
    printf("%s[%d,%d,%d,%d,%d,%d] (%d,%d,%d)",
            core, memory_usage->tcm24x, memory_usage->tcm24y,
            memory_usage->esram16, memory_usage->esram24, memory_usage->sdram16, memory_usage->sdram24,
            memory_usage->stack[0], memory_usage->stack[1], memory_usage->stack[2]);
}

void traceMemoryUsage(const char* function, unsigned int handle, const char* component, t_memory_usage* memory_usage) {
    if (!isMemoryTraceEnabled())
        return;

    printf("***[%s] %s (0x%08X):", function, component, handle);
    traceMemoryUsageCore("SIA", &memory_usage->sia);
//    printf(",");
//    traceMemoryUsageCore("SVA", &memory_usage->sva);
    printf("\n");
}

static void traceMemoryStatusCore(const char* core, t_memory_status_core* memory_status) {
    printf("%s[%d,%d,%d,%d,%d,%d]",
            core,
            (int)memory_status->mpc_tcm24x.global.accumulate_free_memory + (int)memory_status->mpc_tcm24x.global.accumulate_used_memory,
            (int)memory_status->mpc_tcm24y.global.accumulate_free_memory + (int)memory_status->mpc_tcm24y.global.accumulate_used_memory,
            (int)memory_status->mpc_esram16.global.accumulate_free_memory + (int)memory_status->mpc_esram16.global.accumulate_used_memory,
            (int)memory_status->mpc_esram24.global.accumulate_free_memory + (int)memory_status->mpc_esram24.global.accumulate_used_memory,
            (int)memory_status->mpc_sdram16.global.accumulate_free_memory + (int)memory_status->mpc_sdram16.global.accumulate_used_memory,
            (int)memory_status->mpc_sdram24.global.accumulate_free_memory + (int)memory_status->mpc_sdram24.global.accumulate_used_memory);
}

void traceMemoryStatus() {
    if (!isMemoryTraceEnabled())
        return;

    t_memory_status memory_status;
    getMemoryStatus(&memory_status);

    printf("***= ");
    traceMemoryStatusCore("SIA", &memory_status.sia);
//    printf(",");
//    traceMemoryStatusCore("SVA", &memory_status.sva);
    printf("\n");
}

char trace_buffer [128];

static void print_callstack_line(unsigned int addr)
{
    FILE* f=fopen("/proc/self/maps", "rb");
    if (f) {
        unsigned int start, end;
        char line[120];
        while (fgets(line, 120, f)) {
            sscanf(line, "%X-%X", &start, &end);
            if (addr >= start && addr <= end) {
                if (*line) {
                    line[strlen(line)-1] = 0; // chop \n
                    char* libname = line + strlen(line);
                    while (libname > line && *libname != ' ') libname--;
                    // Almost same format as debuggerd to re-use scripts..
                    printf("   (debug callstack) #00  pc %x   %s\n", addr - start, libname);
                    fclose(f);
                    return;
                }
            }
        }
        fclose(f);
    }

    printf("(debug callstack) unresolved %X\n", addr);
}

static _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
   _Unwind_Word ip = _Unwind_GetIP(context);
   if (ip) {
            print_callstack_line((unsigned int) ip);
        }
   return _URC_OK;
}

static void print_backtrace()
{
    printf("Start of callstack dump for debug\n");
    _Unwind_Backtrace(trace_function, 0);
    printf("End of callstack dump for debug\n");
}



ENS_API_EXPORT OMX_ERRORTYPE
ENS::OMX_Error(t_cm_error cm_error) {
    switch (cm_error) {
        case CM_NO_MORE_MEMORY: {
            print_backtrace();
            return OMX_ErrorInsufficientResources;
        }

        case CM_OK: return OMX_ErrorNone;

        default: print_backtrace();
                 DBGT_CRITICAL("CM ERROR: %s\n", CM_StringError(cm_error));
                 return OMX_ErrorUndefined;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::allocMpcMemory(
        OMX_U32                 domainId,
        t_cm_mpc_memory_type	memType,
        t_cm_size	            size,
        t_cm_memory_alignment	memAlignment,
        t_cm_memory_handle *	pHandle)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_AllocMpcMemory(
                static_cast<t_cm_domain_id>(domainId), memType, size, memAlignment, pHandle);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("allocMpcMemory", *pHandle, "", &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::freeMpcMemory(t_cm_memory_handle handle)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_FreeMpcMemory(handle);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("freeMpcMemory", handle, "", &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::instantiateNMFComponent(
        OMX_U32 domainId,
        const char * type,
        const char * localName,
        t_cm_instance_handle * pHandle,
        t_nmf_ee_priority priority)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_InstantiateComponent(
            type, static_cast<t_cm_domain_id>(domainId), priority, localName, pHandle);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("instantiateNMFComponent", *pHandle, localName, &memory_usage);
    }

    return OMX_Error(error);
}


ENS_API_EXPORT OMX_ERRORTYPE
ENS::destroyNMFComponent(
        t_cm_instance_handle handle)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_DestroyComponent(handle);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("destroyNMFComponent", handle, "", &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::startNMFComponent(
        t_cm_instance_handle handle)
{
    return OMX_Error(CM_StartComponent(handle));
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::stopNMFComponent(
        t_cm_instance_handle handle)
{
    return OMX_Error(CM_StopComponent(handle));
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::localCreateFullContext(OMX_PTR& aCtx,
        t_nmf_channel_flag flag){
    return OMX_Error(EnsWrapper_local_cm_CreateFullContext(aCtx, flag));
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::localDestroyFullContext(OMX_PTR& aCtx){
    return OMX_Error(EnsWrapper_local_cm_DestroyFullContext(aCtx));
}


ENS_API_EXPORT OMX_ERRORTYPE
ENS::localBindComponentToUser(OMX_PTR aCtx,
        t_cm_instance_handle component,
        const char *itfname,
        NMF::InterfaceDescriptor *itfref,
        t_uint32 fifosize){
    return OMX_Error(EnsWrapper_local_cm_bindComponentToUser(
                aCtx, component, itfname, itfref, fifosize));
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::localUnbindComponentToUser(
        OMX_PTR aCtx,
        const t_cm_instance_handle client,
        const char* itfname
        ){
    OMX_ERRORTYPE error = OMX_ErrorNone;

    error = OMX_Error(EnsWrapper_local_cm_unbindComponentToUser(aCtx, client, itfname));

    return error;
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentFromHost(
        t_cm_instance_handle component,
        const char *itfname,
        NMF::InterfaceReference *itfref,
        t_uint32 fifosize)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponentFromUser(
                component, itfname, fifosize, itfref);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("bindComponentFromHost", (unsigned int)itfref, itfname, &memory_usage);
        if (fifosize >= MAX_FIFOSIZE_WARNING) {
        printf("***!bindComponentFromHost: Fifo size = %u\n", (unsigned int)fifosize);
    }
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentFromHostEx(
        t_cm_instance_handle component,
        const char *itfname,
        NMF::InterfaceReference *itfref,
        t_uint32 fifosize)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponentFromUserEx(
                component, itfname, fifosize, CM_MM_MPC_SDRAM24, itfref);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("bindComponentFromHostEx", (unsigned int)itfref, itfname, &memory_usage);
        if (fifosize >= MAX_FIFOSIZE_WARNING) {
        printf("***!bindComponentFromHostEx: Fifo size = %u\n", (unsigned int)fifosize);
    }
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentToHost(
        OMX_HANDLETYPE hComponent,
        t_cm_instance_handle component,
        const char *itfname,
        NMF::InterfaceDescriptor *itfref,
        t_uint32 fifosize)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = EnsWrapper_cm_bindComponentToUser(
                hComponent, component, itfname, itfref, fifosize);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("bindComponentToHost", component, itfname, &memory_usage);
        if (fifosize >= MAX_FIFOSIZE_WARNING) {
        printf("***!bindComponentToHost: Fifo size = %u\n", (unsigned int)fifosize);
    }
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentToVoid(
        t_cm_instance_handle component,
        const char *itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponentToVoid(component, itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("bindComponentToVoid", component, itfname, &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentAsynchronous(
        t_cm_instance_handle client,
        const char *client_itfname,
        t_cm_instance_handle server,
        const char *server_itfname,
        t_uint32 fifosize)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponentAsynchronous(
                client, client_itfname, server, server_itfname, fifosize);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    sprintf(trace_buffer, "%s -> %s", client_itfname, server_itfname);
    traceMemoryUsage("bindComponentAsynchronous", client, trace_buffer, &memory_usage);
        if (fifosize >= MAX_FIFOSIZE_WARNING) {
        printf("***!bindComponentAsynchronous: Fifo size = %u\n", (unsigned int)fifosize);
    }
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponentAsynchronousEx(
        t_cm_instance_handle client,
        const char *client_itfname,
        t_cm_instance_handle server,
        const char *server_itfname,
        t_uint32 fifosize)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponentAsynchronousEx(
            client, client_itfname, server, server_itfname, fifosize, CM_MM_MPC_SDRAM24);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    sprintf(trace_buffer, "%s -> %s", client_itfname, server_itfname);
    traceMemoryUsage("bindComponentAsynchronousEx", client, trace_buffer, &memory_usage);
        if (fifosize >= MAX_FIFOSIZE_WARNING) {
        printf("***!bindComponentAsynchronousEx: Fifo size = %u\n", (unsigned int)fifosize);
    }
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::bindComponent(
        t_cm_instance_handle client,
        const char *client_itfname,
        t_cm_instance_handle server,
        const char *server_itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_BindComponent(client, client_itfname, server, server_itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    sprintf(trace_buffer, "%s -> %s", client_itfname, server_itfname);
    traceMemoryUsage("bindComponent", client, trace_buffer, &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::unbindComponent(
        t_cm_instance_handle client,
        const char *client_itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_UnbindComponent(client, client_itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("unbindComponent", client, client_itfname, &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::unbindComponentFromHost(NMF::InterfaceReference *itfref)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_UnbindComponentFromUser(itfref);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("unbindComponentFromHost", (unsigned int)itfref, "", &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::unbindComponentToHost(
        OMX_HANDLETYPE hComponent,
        t_cm_instance_handle client,
        const char *client_itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = EnsWrapper_cm_unbindComponentToUser(hComponent, client, client_itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("unbindComponentToHost", client, client_itfname, &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::unbindComponentToVoid(
        t_cm_instance_handle component,
        const char *itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_UnbindComponent(component, itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("unbindComponentToVoid", component, itfname, &memory_usage);
    }

    return OMX_Error(error);
}

ENS_API_EXPORT OMX_ERRORTYPE
ENS::unbindComponentAsynchronous(
        t_cm_instance_handle client,
        const char *client_itfname)
{
    t_memory_status memory_status_before, memory_status_after;
    t_memory_usage memory_usage;

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_before);
    }

    t_cm_error error = CM_UnbindComponentAsynchronous(client, client_itfname);

    if (isMemoryTraceEnabled()) {
    getMemoryStatus(&memory_status_after);
    getMemoryUsage(&memory_status_after, &memory_status_before, &memory_usage);
    traceMemoryUsage("unbindComponentAsynchronous", client, client_itfname, &memory_usage);
    }

    return OMX_Error(error);
}

/* Deprecated method. */
void
ENS::memcpy(char * dst, char *src, int size)
{
    ::memcpy(dst, src, size);
}
