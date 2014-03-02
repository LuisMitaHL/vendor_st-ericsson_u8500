/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*----------------------------------------------------------------------------------*/
/* Name: mal_sms.c                                                                  */
/* MAL SMS stub implementation file for SMS Library                                 */
/* version:         0.3                                                             */
/*----------------------------------------------------------------------------------*/

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include "mal_sms.h"
#include "smsipc.h"

#include "r_sms.h"
#define LOG_TAG "MAL_SMS Stub"
#include "r_smslinuxporting.h"
#include "mal_stub.h"

#ifdef SMS_SIM_TEST_PLATFORM

#include <signal.h>
static int internal_pipe_write_fd = -1;

static int mal_sms_client_terminate = 0;
#endif                          // SMS_SIM_TEST_PLATFORM

typedef enum {
    SMSC_Address_PPDN = 1,
    SMSC_Address_OTADP,
    SMSC_Address_Default
} SMSC_Address_Type_t;

typedef enum {
    MAL_SMS_STUB_MEMORY_CAPACITY_AVAILABLE,
    MAL_SMS_STUB_MEMORY_CAPACITY_EXCEEDED
} mal_sms_stub_memory_capacity_t;

static mal_sms_stub_memory_capacity_t mem_status = MAL_SMS_STUB_MEMORY_CAPACITY_AVAILABLE;
static mal_sms_route_info_t route_info = MAL_SMS_ROUTE_PS;

// The following data is taken from 3GPP TS 51.010-4 section 27.22.5.1 (SMS-PP Data Download)
static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_SMS_PPDN = {
    {                           // Address
     0x09,                      // The number of useful semi-octets in the AddressValue.
     0x91,                      // Type of address as defined in the standards.
     {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0xF8}   // The address value in packed format.
     },
    {                           // TPDU
     0x1C,                      // Indicates the length of the TPDU data.
     {0x04, 0x04, 0x91, 0x21, 0x43, 0x7F, 0x16, 0x89, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x53, 0x68, 0x6F, 0x72, 0x74, 0x20, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}   // TPDU data
     }
};


//_________________________________________________________________________ Service Centre Address
//                         ________________________________________________ Deliver length
//                        |  ______________________________________________ TP_MTI_MTS_VPF
//                        | |  ____________________________________________ Sender Address
//                        | | |          __________________________________ PID
//                        | | |         |  ________________________________ Data Coding Scheme
//                        | | |         | |  ______________________________ Timestamp yy mm dd hh mm ss timezone              
//                        | | |         | | |              ________________ User Data Length (septets)
//                        | | |         | | |             |  ______________ User Data Header
//                        | | |         | | |             | |        ______ Total SMs
//                        | | |         | | |             | |       |  ____ SM part number
//                        | | |         | | |             | |       | |  __ 7 bit user data
//                        | | |         | | |             | |       | | |
//0891683108100005F0ffffff7d040D91947126270851F17E008080808080802390C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683C16030180C0683
// This OTADP data structure contains a valid TPDU, however it contains more data that the OTADP data structures
// allow. By using this data we check that SMS Server detects a possible buffer overrun and handles it.
static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_OTADP = {
    {                           // Address
     0x08,                      // The number of useful semi-octets in the AddressValue.
     0x91,                      // Type of address as defined in the standards.
     {0x68, 0x31, 0x08, 0x10, 0x00, 0x05, 0xF0, 0xff, 0xff, 0xff}       // The address value in packed format.
     },
    {                           // TPDU
     0x7d,                      // Indicates the length of the TPDU data.
     {0x04, 0x0D, 0x91, 0x94, 0x71, 0x26, 0x27, 0x08, 0x51, 0xF1, 0x7E, 0x00, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x23, 0x90, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C,
      0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83,
      0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60,
      0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18,
      0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06,
      0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1,
      0x60, 0x30, 0x18, 0x0C, 0x06, 0x83, 0xC1, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x83}     // TPDU data
     }
};

//_________________________________________________________________ Service Centre Address
//                 ________________________________________________ Deliver length
//                |  ______________________________________________ TP_MTI_MTS_VPF
//                | |  ____________________________________________ Sender Address
//                | | |          __________________________________ PID
//                | | |         |  ________________________________ Data Coding Scheme
//                | | |         | |  ______________________________ Timestamp yy mm dd hh mm ss timezone
//                | | |         | | |              ________________ User Data Length (septets)
//                | | |         | | |             |  ______________ 7 bit user data
//                | | |         | | |             | |
//07916407058099f91C44068121436500F102041A082714040D050003120101A061391D1403

static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
    {                           // Address
     0x07,                      // The number of useful semi-octets in the AddressValue.
     0x91,                      // Type of address as defined in the standards.
     {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}       // The address value in packed format.
     },
    {                           // TPDU
     0x1C,                      // Indicates the length of the TPDU data.
     {0x44, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x14, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x12, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03}   // TPDU data
     }
};

static SMSC_Address_Type_t SMSC_Address_Type = SMSC_Address_PPDN;

static const long tTimer = 112; // 110 is same value as used in smsmain.c for timerserver thread
static const long tMAL = 114;

#define NUMBER_OF_CB_DISPATCHES                      10
#define CB_PAGE_DATA_TOP_BYTE_SERIAL_NUMBER_LOCATION  0
#define CB_PAGE_DATA_TOP_BYTE_MESSAGE_ID_LOCATION     2

static const uint8_t CB_PageData[] = {
    // Data taken from R13 CBS Test Harness.
    0xCD, 0xF2, 0x7C, 0x1E, 0x3E, 0x97, 0x41, 0x31, 0x50, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46,
    0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1,
    0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34,
    0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D,
    0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3, 0xD1, 0x68, 0x34, 0x1A, 0x8D, 0x46, 0xA3,
    0xD1, 0x68, 0x0A
};


/**********************************************************************
* CleanUp - This is called by the IPC-lib when a client context has disconnected
*           (= client context socket closed), for example if the application process
*           that was communicating with your demon crashed or closed down, the IPC-lib
*     will detect this and invoke this call-back. Then you can free up the
*     resources associated with this communication and have it ready to accept
*     a new connection.
***********************************************************************/

static void MAL_CleanUp(
    const int EventSocket)
{                               // not implemented for this example
    SMS_LOG_I("MAL_CleanUp called");

    SMS_IDENTIFIER_NOT_USED(EventSocket);
}

/*************************************************************************
**************************************************************************
*
* Process:  MAL_SMS_Client - RX process
*
* This thread operates in a SERVER context, listening for and handling
* various requests. It also dispatches a simple event to subscribing clients.
*
**************************************************************************
**************************************************************************/
static void *MAL_SMS_Client(
    void *threadid)
{
    struct ipc_server_context_t *pMAL_SMS_IPC_ServerContext = NULL;
    union SMS_SIGNAL *ReceivedSignal_p = NULL;

    SMS_LOG_I("Started!, Thread ID = %d ", (int) threadid);

    // create the internal pipe,
    pMAL_SMS_IPC_ServerContext = ipc_server_context_create((int) threadid, MAL_SMS_SOCK_PATH, NULL,     //BuildFDs,
                                                           NULL,        //HandleFD_Changes,
                                                           MAL_CleanUp);

    if (pMAL_SMS_IPC_ServerContext == NULL) {
        SMS_LOG_E("ipc_server_context_create returned NULL - oops");
    }
#ifdef SMS_SIM_TEST_PLATFORM
    internal_pipe_write_fd = ipc_server_context_get_internal_pipe_write_fd(pMAL_SMS_IPC_ServerContext);
#endif                          // SMS_SIM_TEST_PLATFORM

    start_up_sequence_complete();

    while (mal_sms_client_terminate != 1) {

        ReceivedSignal_p = ipc_server_context_await_signal(pMAL_SMS_IPC_ServerContext); // instead of OSE RECEIVE

        SMS_LOG_I("ipc_server_context_await_signal returned");

        if (ReceivedSignal_p == NULL) { // this should not happen since this server context is only interested in signals for it's own socket
            // and does not monitor any additional sockets - sw error
            SMS_LOG_W("ReceivedSignal_p == NULL");
        } else {
#ifdef SMS_SIM_TEST_PLATFORM
            if (SHUTDOWN_MAL_SMS_TEST == ReceivedSignal_p->Primitive) {
                SMS_LOG_I("SHUTDOWN");
                SMS_SIGNAL_FREE(&ReceivedSignal_p);
                break;
            }
#endif
            switch (ReceivedSignal_p->Primitive) {
            case REQUEST_MAL_SMS_SEND_TPDU:
                {
                    mal_request_t *Req_p = (mal_request_t *) ReceivedSignal_p;

                    mal_response_send_TPDU_t *Rsp_p = SMS_SIGNAL_ALLOC(mal_response_send_TPDU_t, RESPONSE_MAL_SMS_SEND_TPDU);
                    mal_response_send_TPDU_t *Rsp2_p = SMS_SIGNAL_ALLOC(mal_response_send_TPDU_t, RESPONSE_MAL_SMS_SEND_TPDU);

                    mal_received_sms_event_t *Sig_p = SMS_SIGNAL_ALLOC(mal_received_sms_event_t, EVENT_MAL_SMS_NEW_MESSAGE);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_SEND_TPDU ] - Received, SENDER:0x%08x", SMS_SENDER(&Req_p));

                    Rsp_p->msg_ref = 0x2B;
                    Rsp_p->err_type = SMS_ERR_OK;

                    SMS_SEND(&Rsp_p, SMS_SENDER(&Req_p));

                    Rsp2_p->msg_ref = 0x3B;
                    Rsp2_p->err_type = SMS_ERR_OK;
                    // Send 2nd response to simulate earlier bug in MAL code which caused SMS Server crash.
                    SMS_SEND(&Rsp2_p, SMS_SENDER(&Req_p));

                    switch (SMSC_Address_Type) {
                    case SMSC_Address_PPDN:
                        memcpy(&Sig_p->SMS_SMSC_Address_TPDU, &SMSC_Address_TPDU_SMS_PPDN, sizeof(SMS_SMSC_Address_TPDU_t));
                        SMSC_Address_Type = SMSC_Address_OTADP;
                        break;

                    case SMSC_Address_OTADP:
                        memcpy(&Sig_p->SMS_SMSC_Address_TPDU, &SMSC_Address_TPDU_OTADP, sizeof(SMS_SMSC_Address_TPDU_t));
                        SMSC_Address_Type = SMSC_Address_Default;
                        break;

                    case SMSC_Address_Default:
                    default:
                        memcpy(&Sig_p->SMS_SMSC_Address_TPDU, &SMSC_Address_TPDU, sizeof(SMS_SMSC_Address_TPDU_t));
                        break;
                    }

                    SMS_LOG_I("[ REQUEST_MAL_SMS_SEND_TPDU ] - Dispatch MT SMS");
                    SMS_SEND(&Sig_p, SMS_SENDER(&Req_p));

                }
                break;

            case REQUEST_MAL_SMS_SEND_DELIVERREPORT:
                {
                    mal_request_send_deliver_report_t *Req_p = (mal_request_send_deliver_report_t *) ReceivedSignal_p;

                    mal_response_send_deliver_report_t *Rsp_p = SMS_SIGNAL_ALLOC(mal_response_send_deliver_report_t, RESPONSE_MAL_SMS_SEND_DELIVERREPORT);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_SEND_DELIVERREPORT ] - Received, SENDER:0x%08x. RPErr=0x%04X", SMS_SENDER(&Req_p), Req_p->RP_Error);

                    // If RP_ERR_Cause is memory capacity exceeded, update the static flag
                    if (Req_p->RP_Error == MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC) {
                        mal_ind_t *Ind_p = SMS_SIGNAL_ALLOC(mal_ind_t, IND_MAL_MEMORY_CAPACITY_EXC);

                        mem_status = MAL_SMS_STUB_MEMORY_CAPACITY_EXCEEDED;
                        SMS_LOG_I("Memory Capacity Exceeded - Flag SET");

                        // Cause MAL_SMS_MEMORY_CAPACITY_EXC_IND to be dispatched
                        SMS_SEND(&Ind_p, SMS_SENDER(&Req_p));
                    }

                    Rsp_p->SMS_Error = SMS_ERR_OK;
                    SMS_SEND(&Rsp_p, SMS_SENDER(&Req_p));
                }
                break;

            case REQUEST_MAL_SMS_SET_MEMORY_CAPACITY_STATUS:
                {
                    mal_request_set_memory_capacity_status_t *Req_p = (mal_request_set_memory_capacity_status_t *) ReceivedSignal_p;

                    mal_response_set_memory_capacity_status_t *Rsp_p = SMS_SIGNAL_ALLOC(mal_response_set_memory_capacity_status_t, RESPONSE_MAL_SMS_SET_MEMORY_CAPACITY_STATUS);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_SET_MEMORY_CAPACITY_STATUS ] - Received, SENDER:0x%08x, MemCapAv=%d.", SMS_SENDER(&Req_p), Req_p->MemoryCapacityAvailable);

                    // According to mal_sms.h, the memory_status parameter of function
                    // mal_sms_request_set_memory_status() should be set to 1 when memory
                    // is available. This function is not called with any other value.
                    if (Req_p->MemoryCapacityAvailable) {
                        mem_status = MAL_SMS_STUB_MEMORY_CAPACITY_AVAILABLE;
                        SMS_LOG_I("Memory Capacity Exceeded - Flag CLEARED");
                    }

                    Rsp_p->SMS_Error = SMS_ERR_OK;
                    SMS_SEND(&Rsp_p, SMS_SENDER(&Req_p));
                }
                break;

            case REQUEST_MAL_SMS_MO_ROUTE_SET:
                {
                    mal_request_route_set_t *Req_p = (mal_request_route_set_t *) ReceivedSignal_p;
                    mal_response_mo_route_set_t *Resp_p = SMS_SIGNAL_ALLOC(mal_response_mo_route_set_t, RESPONSE_MAL_SMS_MO_ROUTE_SET);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_MO_ROUTE_SET ] - Received, SENDER:0x%08x Route:%d", SMS_SENDER(&Req_p), Req_p->sms_route);

                    route_info = Req_p->sms_route;

                    Resp_p->SMS_Error = SMS_ERR_OK;
                    SMS_SEND(&Resp_p, SMS_SENDER(&Req_p));
                }
                break;

            case REQUEST_MAL_SMS_MO_ROUTE_GET:
                {
                    mal_request_t *Req_p = (mal_request_t *) ReceivedSignal_p;
                    mal_response_mo_route_get_t *Resp_p = SMS_SIGNAL_ALLOC(mal_response_mo_route_get_t, RESPONSE_MAL_SMS_MO_ROUTE_GET);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_MO_ROUTE_GET ] - Received, SENDER:0x%08x", SMS_SENDER(&Req_p));

                    Resp_p->MAL_SMS_Error = MAL_SMS_OK;
                    Resp_p->route_info = route_info;

                    SMS_SEND(&Resp_p, SMS_SENDER(&Req_p));
                }
                break;

            case REQUEST_MAL_CBS_REQUEST_ROUTING_SET:
                {
                    static uint8_t SubscriptionNumber = 0;
                    mal_request_t *Req_p = (mal_request_t *) ReceivedSignal_p;

                    mal_response_cbs_routing_set_t *Rsp_p = SMS_SIGNAL_ALLOC(mal_response_cbs_routing_set_t, RESPONSE_MAL_CBS_REQUEST_ROUTING_SET);

                    SMS_LOG_I("[ REQUEST_MAL_CBS_REQUEST_ROUTING_SET ] - Received, SENDER:0x%08x", SMS_SENDER(&Req_p));

                    Rsp_p->CBS_Error = SMS_ERR_OK;
                    Rsp_p->SubscriptionNumber = SubscriptionNumber++;
                    SMS_SEND(&Rsp_p, SMS_SENDER(&Req_p));

                    // Now dispatch a CB Page
                    {
                        mal_received_cbs_routing_event_t *CB_RoutingIndication_p = NULL;

                        CB_RoutingIndication_p = SMS_SIGNAL_ALLOC(mal_received_cbs_routing_event_t, EVENT_MAL_CBS_IND);

                        CB_RoutingIndication_p->number_of_messages = 1;
                        CB_RoutingIndication_p->messages[0].header.message_id = 0x0001;
                        CB_RoutingIndication_p->messages[0].header.serial_number = 0x00B2;
                        CB_RoutingIndication_p->messages[0].header.dcs = 0x01;
                        CB_RoutingIndication_p->messages[0].header.pages = 0x11;
                        (void) memcpy(CB_RoutingIndication_p->messages[0].data, CB_PageData, MAL_RECEIVED_CBS_ROUTING_EVENT_MESSAGE_DATA_SIZE_MAX);
                        SMS_SEND(&CB_RoutingIndication_p, SMS_SENDER(&Req_p));
                    }
                }
                break;

            case REQUEST_MAL_CBS_REQUEST_ROUTING_REMOVE:
                {
                    mal_request_t *Req_p = (mal_request_t *) ReceivedSignal_p;

                    mal_response_cbs_routing_remove_t *Rsp_p = SMS_SIGNAL_ALLOC(mal_response_cbs_routing_remove_t, RESPONSE_MAL_CBS_REQUEST_ROUTING_REMOVE);

                    SMS_LOG_I("[ REQUEST_MAL_CBS_REQUEST_ROUTING_REMOVE ] - Received, SENDER:0x%08x", SMS_SENDER(&Req_p));

                    Rsp_p->CBS_Error = SMS_ERR_OK;
                    SMS_SEND(&Rsp_p, SMS_SENDER(&Req_p));
                }
                break;

            case REQUEST_MAL_SMS_CBS_CONFIGURE:
                {
                    static SMS_Error_t Error = SMS_ERR_SERVER_NOT_READY;
                    mal_request_t *Req_p = (mal_request_t *) ReceivedSignal_p;
                    mal_response_sms_cbs_config_t *Resp_p = SMS_SIGNAL_ALLOC(mal_response_sms_cbs_config_t, RESPONSE_MAL_SMS_CBS_CONFIGURE);

                    SMS_LOG_I("[ REQUEST_MAL_SMS_CBS_CONFIGURE ] - Received, SENDER:0x%08x", SMS_SENDER(&Req_p));

                    Resp_p->SMS_Error = Error;
                    SMS_SEND(&Resp_p, SMS_SENDER(&Req_p));

                    // mal client returns server not ready once to test config retry so clear the error
                    if (Error == SMS_ERR_SERVER_NOT_READY) {
                        Error = SMS_ERR_OK;
                    }
                }
                break;

            case EVENT_MAL_CBS_IND:
                {
                    SMS_LOG_E("!!!ERROR!!![ EVENT_MAL_SMS_NEW_MESSAGE ] - Sent event to self!!!");
                }
                break;

            case EVENT_MAL_SMS_NEW_MESSAGE:
                {
                    SMS_LOG_E("!!!ERROR!!![ EVENT_MAL_SMS_NEW_MESSAGE ] - Sent event to self!!!");
                }
                break;
            default:
                SMS_LOG_W("Default switch statement hit!");
                break;
            }                   // end switch
        }

        if (ReceivedSignal_p != NULL) {
            SMS_SIGNAL_FREE(&ReceivedSignal_p);
        }
    }                           // endless loop

    ipc_server_context_destroy(pMAL_SMS_IPC_ServerContext);

    return NULL;
}

#ifdef SMS_SIM_TEST_PLATFORM
void MAL_SMS_Signal_Handler(int signo) {
    if (SIGTERM == signo) {
        SMS_SigselectWithClientTag_t *Sig_p;
        mal_sms_client_terminate = 1;
        Sig_p = SMS_SIGNAL_ALLOC(SMS_SigselectWithClientTag_t, SHUTDOWN_MAL_SMS_TEST);
        SMS_SEND(&Sig_p, internal_pipe_write_fd);
    }
}
#endif                          // SMS_SIM_TEST_PLATFORM

/*************************************************************************
**************************************************************************
*
* Main Prog - Simply starts up the various threads
*
**************************************************************************
**************************************************************************/
int main(
    void)
{
    int rc;
    pthread_t Thread;

    SMS_LOG_I("\n *** MAL SMS STUB STARTED! ***\n");

#ifdef SMS_SIM_TEST_PLATFORM
    signal(SIGTERM, MAL_SMS_Signal_Handler);
#endif                          // SMS_SIM_TEST_PLATFORM

    start_up_sequence_lock();

    //Start the RX thread
    SMS_LOG_I("Creating thread %ld", tMAL);
    rc = pthread_create(&Thread, NULL, MAL_SMS_Client, (void *) tMAL);

    if (rc) {
        SMS_LOG_E("Return code from pthread_create() is %d", rc);
        exit(1);
    }
    // blocks here until TX has started
    start_up_sequence_wait();
    start_up_sequence_destroy();

    SMS_A_(SMS_LOG_I("MAL_SMS_Thread running... "));
    // Block here until "Thread" terminates...which it probably won't do as it contains an infinite loop!
    pthread_join(Thread, NULL);

    return 0;
}
