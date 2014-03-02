#ifndef INCLUSION_GUARD_G_SERVICEREGISTER_H
#define INCLUSION_GUARD_G_SERVICEREGISTER_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
*  Service register internal header.
*
*************************************************************************/

//*************************************************************************
/* Includes
*************************************************************************/
#include "t_basicdefinitions.h"
#include "t_serviceregister.h"

//*************************************************************************
/* Types, constants and external variables
*************************************************************************/
#define SR_MAX_LNH_NAME_LENGTH 31
#define SR_MAX_PROCNAME_LENGTH (SR_MAX_NAME_LENGTH+SR_MAX_LNH_NAME_LENGTH)

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        SIGSELECT SigNo;
        PROCESS RequestPid;     /* Pid of original requester */
        char Name[SR_MAX_NAME_LENGTH + 1];      /* service name */
        char ProcName[SR_MAX_PROCNAME_LENGTH + 1];      /* OS process name */
        SR_Options_t Options;
        PROCESS Pid;
        PROCESS Bid;
        const void *Vtbl_p;
        SR_CreateCallBack_t CreateCallBack;
        void *Opaque_p;
        SR_Result_t ReturnValue;
    } SR_Register_t;

    typedef struct {
        SIGSELECT SigNo;
        PROCESS RequestPid;     /* Pid of original requester */
        char Name[SR_MAX_NAME_LENGTH + 1];
        char ProcName[SR_MAX_PROCNAME_LENGTH + 1];
        SR_Options_t Options;
        uint32 Timeout;
        boolean Attach;
        PROCESS Pid;            /* resolved pid */
        const void *Vtbl_p;     /* resolved Vtbl_p */
        SR_Result_t ReturnValue;
    } SR_Resolve_t;

    typedef struct {
        /* note: must be identical to SR_Resolve_t */
        SIGSELECT SigNo;
        PROCESS RequestPid;     /* Pid of original requester */
        char Name[SR_MAX_NAME_LENGTH + 1];
        char ProcName[SR_MAX_PROCNAME_LENGTH + 1];
        SR_Options_t Options;
        uint32 Timeout;
        boolean Attach;
        PROCESS Pid;            /* if NO_PROCESS it indicates signal based */
        const void *Vtbl_p;     /* if NULL indicates Vtbl_p based */
        SR_Result_t ReturnValue;
        /* note: above must be identical to SR_Resolve_t */
        SIGSELECT Signal;       /* signal to send when service is registered */
    } SR_RegisteredSignal_t;

    typedef struct {
        SIGSELECT SigNo;
    } SR_Print_t;


#define SERVICE_REGISTER_PROCESS_NAME "ServiceRegister_Process"
#define SERVICE_REGISTER_DO_CALL_NAME "SR_DoCall"       // Name string with at most
    // 24 chars including the nul
    // terminator


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//************************************************************************
/**
 * SIGNALS
 */
//************************************************************************

// Only for test of Remote SwBP 
    enum {
        REQ_SR_RESOLVE = 1122,
        RESP_SR_RESOLVE = 1133
    };


/*
SIGID_INTERVAL_START_V2(CAT_ID_SERVICEREGISTER)
  SIGID_INTERVAL_DEFINITION_V2(REQ_SERVICEREGISTER_SERVICEREGISTER, 100),
  SIGID_INTERVAL_DEFINITION_V2(RESP_SERVICEREGISTER_SERVICEREGISTER, 100),
  SIGID_INTERVAL_DEFINITION_V2(EVENT_SERVICEREGISTER_SERVICEREGISTER, 0),
  SIGID_INTERVAL_DEFINITION_V2(DO_SERVICEREGISTER_SERVICEREGISTER, 0),
  SIGID_INTERVAL_DEFINITION_V2(DONE_SERVICEREGISTER_SERVICEREGISTER, 0),
  SIGID_INTERVAL_DEFINITION_V2(CONFIRM_SERVICEREGISTER_SERVICEREGISTER, 0),
  SIGID_INTERVAL_DEFINITION_V2(MOD_SERVICEREGISTER_SERVICEREGISTER, 100), 
SIGID_INTERVAL_END_V2(CAT_ID_SERVICEREGISTER)
*/
/*******************************************************************************
 * Request signals
 * @param REQ_SR_REGISTER Request to register a service.
 * @param REQ_SR_REGISTER_MULTIPLE Request to register multiple services at once.
 * @param REQ_SR_RESOLVE Request to resolve a service.
 * @param REQ_SR_RESOLVE_REMOTE Request to resolve a service at another cpu.
 * @param REQ_SR_PRINT Request to print the registered services.
 *
 ******************************************************************************/
/*
SIGID_INTERVAL_START_V2(REQ_SERVICEREGISTER_SERVICEREGISTER)
  SIGID_DEFINITION_V2(REQ_SR_REGISTER,       SR_Register_t),
  SIGID_DEFINITION_V2(REQ_SR_REGISTER_REMOTE, SR_Register_t),
  SIGID_DEFINITION_V2(REQ_SR_REGISTER_MULTIPLE, SR_Register_Multiple_t),
  SIGID_DEFINITION_V2(REQ_SR_RESOLVE,        SR_Resolve_t),
  SIGID_DEFINITION_V2(REQ_SR_RESOLVE_REMOTE, SR_Resolve_t),
  SIGID_DEFINITION_V2(REQ_SR_PRINT,          SR_Print_t),
  SIGID_DEFINITION_V2(REQ_SR_REGISTERED_SIGNAL, SR_RegisteredSignal_t),
  SIGID_DEFINITION_V2(REQ_SR_UNREGISTER,       SR_Register_t),
SIGID_INTERVAL_END_V2(REQ_SERVICEREGISTER_SERVICEREGISTER)
*/
/*******************************************************************************
 * Response signals
 * @param RESP_SR_REGISTER Response to register request.
 * @param RESP_SR_RESOLVE Response to resolve request.
 * @param RESP_SR_RESOLVE_REMOTE Response to resolve a service at another cpu.
 *
 ******************************************************************************/
/*
SIGID_INTERVAL_START_V2(RESP_SERVICEREGISTER_SERVICEREGISTER)
  SIGID_DEFINITION_V2(RESP_SR_REGISTER,       SR_Register_t),
  SIGID_DEFINITION_V2(RESP_SR_REGISTER_REMOTE, SR_Register_t),
  SIGID_DEFINITION_V2(RESP_SR_RESOLVE,        SR_Resolve_t),
  SIGID_DEFINITION_V2(RESP_SR_RESOLVE_REMOTE, SR_Resolve_t),
  SIGID_DEFINITION_V2(RESP_SR_UNREGISTER,       SR_Register_t),
SIGID_INTERVAL_END_V2(RESP_SERVICEREGISTER_SERVICEREGISTER)
*/

//*************************************************************************
/* Declaration of functions
**************************************************************************/

#ifdef __cplusplus
}
#endif
#endif                          /* #ifndef INCLUSION_GUARD_G_SERVICEREGISTER_H */
