/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*----------------------------------------------------------------------------------*/
/* Name: client_side_mfl_cas_stub.c                                                 */
/* MFL Framework stub implementation file for SMS Library                           */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/


#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include "mfl.h"
#include "smsipc.h"

#include "t_sms.h"
#define LOG_TAG "MFL Stub (ClientSide)"
#include "r_smslinuxporting.h"
#include "smutil.h"
#include "mfl_stub.h"


static const long tMFL = MFL_MODEM_THREAD;



// Struct taken from real mfl.c. It is what modem_session_t is abstracted from.
struct modem_session_s {
    // Parameters added for stub implementation
    int mfl_stub_Request_fd;
    int mfl_stub_Event_fd;

    // Parameters found in original structure
    int fd_caif;                /* File descriptor CAIF channel */
    char *sig_buf_ptr;          /* Signal read buffer used by a session */
};



/* Stub Versions of MFL Framework Functions */
modem_session_t *mfl_session_begin(
    void)
{
    modem_session_t *Session_p;

    SMS_LOG_I("Called mfl_session_begin");

    Session_p = malloc(sizeof(struct modem_session_s));

    if (Session_p != NULL) {
        Session_p->mfl_stub_Request_fd = SMS_NO_PROCESS;
        Session_p->mfl_stub_Event_fd = SMS_NO_PROCESS;
        Session_p->fd_caif = SMS_NO_PROCESS;
        Session_p->sig_buf_ptr = NULL;
    } else {
        SMS_LOG_E("mfl_session_begin: Malloc Failed");
    }

    return Session_p;
}


void mfl_session_get_file_descriptor(
    modem_session_t * modem_session_ptr,
    int *fd_ptr)
{
    SMS_LOG_I("Called mfl_session_get_file_descriptor");

    if (modem_session_ptr != NULL && fd_ptr != NULL) {
        if (modem_session_ptr->mfl_stub_Request_fd == SMS_NO_PROCESS) {
            unsigned int ClientID;
            SMS_LOG_I("mfl_session_get_file_descriptor: IPC Request");

            // connect to MFL Client (really a server!) to use requesters
            if (ipc_connect_request((int) tMFL, fd_ptr, &ClientID, MFL_SMS_SOCK_PATH)) {
                SMS_LOG_I("connected to MFL_SMS_SOCK_PATH, SOCKET:0x%08x, ClientID:0x%08X", *fd_ptr, ClientID);
                modem_session_ptr->mfl_stub_Request_fd = *fd_ptr;
            } else {
                SMS_LOG_E("Failed to connect to MFL_SMS_SOCK_PATH");
            }

            // connect to MFL Client (really a server!) to use events
            if (ipc_connect_event((int) tMFL, &modem_session_ptr->mfl_stub_Event_fd, ClientID, MFL_SMS_SOCK_PATH)) {
                SMS_LOG_I("connected to Event MFL_SMS_SOCK_PATH, SOCKET:0x%08x, ClientID:0x%08X", modem_session_ptr->mfl_stub_Event_fd, ClientID);
            } else {
                SMS_LOG_E("Failed to connect to Event MFL_SMS_SOCK_PATH");
            }
        } else {
            SMS_LOG_I("mfl_session_get_file_descriptor: Copy FD");
            *fd_ptr = modem_session_ptr->mfl_stub_Request_fd;
        }
    } else {
        SMS_LOG_E("mfl_session_get_file_descriptor: NULL parameters");
    }
}


RequestControlBlock_t *mfl_request_control_block_alloc(
    modem_session_t * const modem_session_ptr,
    const ClientTag_t client_tag)
{
    RequestControlBlock_t *ReqCtrlBlk_p = NULL;

    SMS_LOG_I("Called mfl_request_control_block_alloc");

    if (modem_session_ptr != NULL) {
        ReqCtrlBlk_p = malloc(sizeof(RequestControlBlock_t));

        if (ReqCtrlBlk_p != NULL) {
            ReqCtrlBlk_p->WaitMode = NO_WAIT_MODE;
            ReqCtrlBlk_p->ClientTag = client_tag;
            ReqCtrlBlk_p->modem_session_ptr = modem_session_ptr;
            ReqCtrlBlk_p->client_client_tag = client_tag;
            ReqCtrlBlk_p->mfl_modem_stub_socket = SMS_NO_PROCESS;
        } else {
            SMS_LOG_E("mfl_request_control_block_alloc: Malloc Failed");
        }
    } else {
        SMS_LOG_E("mfl_request_control_block_alloc: NULL parameters");
    }

    return ReqCtrlBlk_p;
}


void mfl_request_control_block_free(
    RequestControlBlock_t * request_control_block_ptr)
{
    SMS_LOG_I("Called mfl_request_control_block_free");

    if (request_control_block_ptr != NULL) {
        free((uint8_t *) request_control_block_ptr);
    } else {
        SMS_LOG_E("mfl_request_control_block_free: NULL parameters");
    }
}


void *mfl_session_get_signal(
    modem_session_t * const modem_session_ptr)
{
    void *SignalData_p = NULL;

    SMS_LOG_I("mfl_session_get_signal");

    if (modem_session_ptr != NULL) {
        int fd;

        mfl_session_get_file_descriptor(modem_session_ptr, &fd);

        // Get signal data by calling into ipc code.
        SignalData_p = ipc_receive_signal_over_socket(fd);

        if (SignalData_p == NULL) {
            SMS_LOG_E("mfl_session_get_signal: SignalData_p NULL");
        }
    } else {
        SMS_LOG_E("mfl_session_get_signal: NULL parameters");
    }

    return SignalData_p;
}


void mfl_session_end(
    modem_session_t * modem_session_ptr)
{
    IDENTIFIER_NOT_USED(modem_session_ptr);
    SMS_LOG_W("mfl_session_end: ********* Not implemented *********");
}

modem_session_t *mfl_session_begin_named_socket(
    const char *const name_ptr)
{
    IDENTIFIER_NOT_USED(name_ptr);
    SMS_LOG_W("mfl_session_begin_named_socket: ********* Not implemented *********");
    return NULL;
}

void *mfl_session_get_signal_with_timeout(
    modem_session_t * const modem_session_ptr,
    int timeout_in_milliseconds)
{
    IDENTIFIER_NOT_USED(modem_session_ptr);
    IDENTIFIER_NOT_USED(timeout_in_milliseconds);
    SMS_LOG_W("mfl_session_get_signal_with_timeout: ********* Not implemented *********");
    return NULL;
}

void mfl_signal_free(
    void **signal_ptr_ptr)
{
    IDENTIFIER_NOT_USED(signal_ptr_ptr);
    SMS_LOG_W("mfl_signal_free: ********* Not implemented *********");
}

void mfl_set_client_tag(
    RequestControlBlock_t * request_control_block_ptr,
    const ClientTag_t client_tag)
{
    SMS_C_(SMS_LOG_I("mfl_set_client_tag"));
    if (request_control_block_ptr != NULL) {
        request_control_block_ptr->ClientTag = client_tag;
        request_control_block_ptr->client_client_tag = client_tag;
    }
}

ClientTag_t mfl_get_client_tag(
    void *signal_ptr)
{
    ClientTag_t ClientTag = 0xDEADBEEF;
    SMS_SignalHeaderStructure_t *Sig_p = (SMS_SignalHeaderStructure_t *) signal_ptr;

    SMS_C_(SMS_LOG_I("mfl_get_client_tag"));

    if (Sig_p != NULL) {
        ClientTag = Sig_p->SigselectWithClientTag.ClientTag;
    }

    return ClientTag;
}
