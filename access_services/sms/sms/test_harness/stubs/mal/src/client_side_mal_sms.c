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

#include "r_sms.h"
#include "mal_sms.h"
#include "smsipc.h"

#include "r_sms.h"              // For signal macro definitions
#define LOG_TAG "MAL_SMS Stub"
#include "r_smslinuxporting.h"
#include "mal_stub.h"


static const long tMAL = 114;


int MAL_SMS_fd = SMS_NO_PROCESS;
int MAL_NETLNK_fd = SMS_NO_PROCESS;

static mal_sms_cbs_event_t EventCallback = NULL;

static void MAL_uint8_tBuf_PrintToLog(
    const uint8_t * Address_p,
    const unsigned int BufferLength)
{
    if (Address_p != NULL) {
        unsigned int Index;
        char *data_dump_p = calloc(1, 3 * 20 + 1);
        size_t data_pos = 0;

        if (NULL == data_dump_p) {
            SMS_LOG_E("%s: allocation failed", __func__);
            return;
        }

        for (Index = 0; Index < BufferLength; Index++, Address_p++)    // Using 1-based so that Index % 16 is not TRUE for Index==0.
        {
            if ((Index + 1) % 16 == 0) {
                SMS_LOG_I("%s", data_dump_p);
                data_pos = 0;
            }

            data_pos += sprintf(data_dump_p + data_pos, "%02X ", *Address_p);
        }

        // Ensure that there is a newline at end of output
        SMS_LOG_I("%s", data_dump_p);
        free(data_dump_p);
    } else {
        SMS_LOG_E("%s: INPUT NULL", __func__);
    }
}


/* ----------------------------------------------------------------------- */
/**
 * @brief
 * MAL_API - Startup function for using SMS MAL.
 * Initializes SMS MAL data structures and state, Creates Phonet Socket Session.
 *
 * @return  -  return value
 */
/* ----------------------------------------------------------------------- */
int mal_sms_cbs_init(
    int *fd_sms,
    int *fd_netlnk)
{
    unsigned int ClientID;
    SMS_CBS_req_type_t ReqType = MAL_SMS_CBS_FAIL;

    SMS_LOG_I("Called mal_sms_init");

    // connect to MAL SMS Client (really a server!) to use requesters
    if (ipc_connect_request((int) tMAL, fd_sms, &ClientID, MAL_SMS_SOCK_PATH)) {
        SMS_LOG_I("connected to \"" MAL_SMS_SOCK_PATH "\", SOCKET:0x%08x", *fd_sms);
        MAL_SMS_fd = *fd_sms;
        ReqType = MAL_SMS_CBS_SUCCESS;
    } else {
        SMS_LOG_I("Failed to connect to \"" MAL_SMS_SOCK_PATH "\"\n");
    }

    *fd_netlnk = socket(AF_UNIX, SOCK_DGRAM, 0);
    MAL_NETLNK_fd = *fd_netlnk;

    return ReqType;
}

/* ----------------------------------------------------------------------- */
/**
 * @brief
 * MAL_API - stub.
 *
 * @return  -  return value
 */
/* ----------------------------------------------------------------------- */
int mal_sms_request_set_memory_status(
    uint8_t memory_status)
{
    mal_request_set_memory_capacity_status_t *Sig_p;

    SMS_LOG_I("mal_sms_request_set_memory_status");

    // Need to send to stub and get response to trigger callback
    Sig_p = SMS_SIGNAL_ALLOC(mal_request_set_memory_capacity_status_t, REQUEST_MAL_SMS_SET_MEMORY_CAPACITY_STATUS);
    Sig_p->MemoryCapacityAvailable = memory_status;

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

/* ----------------------------------------------------------------------- */
/**
 * @brief
 * MAL_API - Register Callback.
 *
 * @param in event_cb - Callback function pointer
 * @return  - SMS_req_type return value
 */
/* ----------------------------------------------------------------------- */
int mal_sms_cbs_register_callback(
    mal_sms_cbs_event_t event_cb)
{
    SMS_LOG_I("Called mal_sms_register_callback");

    EventCallback = event_cb;

    return MAL_SMS_CBS_SUCCESS;
}

/* ----------------------------------------------------------------------- */
/**
 * @brief
 * MAL-API - Configures SMS MAL Parameters.
 *
 * @return  - SMS_req_type return value
 *
 * Reads default config parameters or Sets specific config parameters
 * This function should be called only once and immediately after SMS_Mal_Init().
 */
/* ----------------------------------------------------------------------- */
int mal_sms_cbs_config(
    void)
{
    mal_request_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_t, REQUEST_MAL_SMS_CBS_CONFIGURE);

    SMS_LOG_I("Called mal_sms_cbs_config, SOCKET:0x%08x", MAL_SMS_fd);

    // Send to MAL SMS socket to commence MT SMS Received
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

/* ----------------------------------------------------------------------- */
/**
 SMS server call this Api to deinitialize SMS library

 @return SMS_req_type return value

 */
/* ----------------------------------------------------------------------- */
int mal_sms_cbs_deinit(
    void)
{
    SMS_LOG_I("Called mal_sms_deinit");

    if (MAL_SMS_fd != SMS_NO_PROCESS) {
        close(MAL_SMS_fd);
        MAL_SMS_fd = SMS_NO_PROCESS;
    }

    if (MAL_NETLNK_fd != SMS_NO_PROCESS) {
        close(MAL_NETLNK_fd);
        MAL_NETLNK_fd = SMS_NO_PROCESS;
    }

    return MAL_SMS_CBS_SUCCESS;
}

/* ----------------------------------------------------------------------- */
/**
 * @brief
 * MAL-API - SMS Response and Indication Handler .
 *
 * Responses and Indications -
 * @return  - SMS_req_type return value
 * ----------------------------------------------------------------------- */

void mal_sms_cbs_response_handler(
    void)
{
    union SMS_SIGNAL *ReceivedSignal_p = NULL;
    static const SMS_SIGSELECT SignalSelector[] = { 0, 0 };     //An error from Coverity, probably the primitive is not of interest here??? 

    SMS_LOG_I("Called mal_sms_response_handler, SOCKET:0x%08x", MAL_SMS_fd);

    ReceivedSignal_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, MAL_SMS_fd);

    if (ReceivedSignal_p == NULL) {
        // shouldn't happen as this function is only called if a signal is waiting to be read, i.e. FD is set
        SMS_LOG_I("mal_sms_response_handler -->ReceivedSignal_p == NULL \n");
    } else {
        switch (ReceivedSignal_p->Primitive) {
        case EVENT_MAL_SMS_NEW_MESSAGE:
            {
                mal_received_sms_event_t *sms_event_p = (mal_received_sms_event_t *) ReceivedSignal_p;

                SMS_LOG_I("[ EVENT_MAL_SMS_NEW_MESSAGE ]\n");
                (EventCallback) (MAL_SMS_RECEIVED_MSG_IND, (void *) &sms_event_p->SMS_SMSC_Address_TPDU);
            }
            break;

        case RESPONSE_MAL_SMS_SEND_TPDU:
            {
                mal_response_send_TPDU_t *resp_p = (mal_response_send_TPDU_t *) ReceivedSignal_p;
                sms_message_send_resp_t send_resp;

                send_resp.msg_ref = &resp_p->msg_ref;
                send_resp.err_type = (MAL_SMS_CBS_Error_t *) & resp_p->err_type;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_SEND_TPDU ]\n");
                (EventCallback) (MAL_SMS_MESSAGE_SEND_RESP, (void *) &send_resp);
            }
            break;

        case RESPONSE_MAL_SMS_SEND_DELIVERREPORT:
            {
                mal_response_send_deliver_report_t *resp_p = (mal_response_send_deliver_report_t *) ReceivedSignal_p;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_SEND_DELIVERREPORT ]\n");
                (EventCallback) (MAL_SMS_RECEIVED_MSG_REPORT_RESP, (void *) &resp_p->SMS_Error);
            }
            break;

        case RESPONSE_MAL_SMS_SET_MEMORY_CAPACITY_STATUS:
            {
                mal_response_set_memory_capacity_status_t *resp_p = (mal_response_set_memory_capacity_status_t *) ReceivedSignal_p;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_SET_MEMORY_CAPACITY_STATUS ]\n");
                (EventCallback) (MAL_SMS_RECEIVE_MESSAGE_RESP, (void *) &resp_p->SMS_Error);
            }
            break;

        case RESPONSE_MAL_SMS_CBS_CONFIGURE:
            {
                mal_response_sms_cbs_config_t *resp_p = (mal_response_sms_cbs_config_t *) ReceivedSignal_p;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_CBS_CONFIG ]\n");
                (EventCallback) (MAL_SMS_RECEIVE_MESSAGE_RESP, (void *) &resp_p->SMS_Error);
            }
            break;

        case RESPONSE_MAL_SMS_MO_ROUTE_SET:
            {
                mal_response_mo_route_set_t *resp_p = (mal_response_mo_route_set_t *) ReceivedSignal_p;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_MO_ROUTE_SET ]\n");
                (EventCallback) (MAL_SMS_ROUTE_INFO_SET_RESP, (void *) &resp_p->SMS_Error);
            }
            break;

        case RESPONSE_MAL_SMS_MO_ROUTE_GET:
            {
                mal_response_mo_route_get_t *resp_p = (mal_response_mo_route_get_t *) ReceivedSignal_p;
                sms_routing_read_resp_t routing_read_resp;

                SMS_LOG_I("[ RESPONSE_MAL_SMS_MO_ROUTE_GET ]\n");
                routing_read_resp.Error_p = &resp_p->MAL_SMS_Error;
                routing_read_resp.route_info = resp_p->route_info;
                (EventCallback) (MAL_SMS_ROUTE_INFO_GET_RESP, &routing_read_resp);
            }
            break;

        case IND_MAL_MEMORY_CAPACITY_EXC:
            {
                SMS_LOG_I("[ IND_MAL_MEMORY_CAPACITY_EXC ]\n");
                (EventCallback) (MAL_SMS_MEMORY_CAPACITY_EXC_IND, NULL);        // No data payload in this callback.
            }
            break;

        case RESPONSE_MAL_CBS_REQUEST_ROUTING_SET:
            {
                mal_response_cbs_routing_set_t *resp_p = (mal_response_cbs_routing_set_t *) ReceivedSignal_p;
                cbs_routing_resp_t routing_resp;

                routing_resp.Error_p = (MAL_SMS_CBS_Error_t *) & resp_p->CBS_Error;
                routing_resp.SubscriptionNumber_p = resp_p->SubscriptionNumber;

                SMS_LOG_I("[ RESPONSE_MAL_CBS_REQUEST_ROUTING_SET ]\n");
                (EventCallback) (MAL_CB_ROUTING_RESP, (void *) &routing_resp);
            }
            break;

        case RESPONSE_MAL_CBS_REQUEST_ROUTING_REMOVE:
            {
                SMS_LOG_I("[ RESPONSE_MAL_CBS_REQUEST_ROUTING_REMOVE ]");
                SMS_LOG_I("******** NO CALLBACK DEFINED IN MAL API ********\n");
            }
            break;

        case EVENT_MAL_CBS_IND:
            {
                mal_received_cbs_routing_event_t *cbs_ind_p = (mal_received_cbs_routing_event_t *) ReceivedSignal_p;
                cbs_routing_ind_t *mal_routing_ind_p = NULL;

                mal_routing_ind_p = (cbs_routing_ind_t *) SMS_HEAP_ALLOC(mal_received_cbs_routing_event_t);

                (void) memset(mal_routing_ind_p, 0, sizeof(mal_received_cbs_routing_event_t));

                mal_routing_ind_p->number_of_messages = cbs_ind_p->number_of_messages;

                mal_routing_ind_p->messages[0].header.message_id = cbs_ind_p->messages[0].header.message_id;
                mal_routing_ind_p->messages[0].header.serial_number = cbs_ind_p->messages[0].header.serial_number;
                mal_routing_ind_p->messages[0].header.dcs = cbs_ind_p->messages[0].header.dcs;
                mal_routing_ind_p->messages[0].header.pages = cbs_ind_p->messages[0].header.pages;
                mal_routing_ind_p->messages[0].useful_data_length = cbs_ind_p->messages[0].useful_data_length;
                (void) memcpy(mal_routing_ind_p->messages[0].data, cbs_ind_p->messages[0].data, sizeof(MAL_CBS_PAGE_MAX_LENGTH));

                (EventCallback) (MAL_CB_ROUTING_IND, (void *) mal_routing_ind_p);

                SMS_HEAP_FREE(&mal_routing_ind_p);
            }
            break;

        default:
            SMS_LOG_I("[non event] default switch statement hit! \n");
            break;
        }
    }

    SMS_SIGNAL_FREE(&ReceivedSignal_p);
}


/**

  This Api is used to send SMS

  @param sms_tpdu:       TPDU to be sent to SMS server
  @param SMSC_address:   SMSC number
  @param SMS_Route:      0 for CS path, 1 for PS path
  @param More_SMS_toSend:TRUE-> more SMS to send

  @return SMS_req_type return value

  Corresponding RIL command is:
  RIL_REQUEST_SEND_SMS

 */

int mal_sms_request_send_tpdu(
    MAL_SMS_SMSC_Address_TPDU_t * sms_tpdu,
    uint8_t SMS_Route,
    uint8_t More_SMS_toSend)
{
    mal_request_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_t, REQUEST_MAL_SMS_SEND_TPDU);

    SMS_LOG_I("Called mal_sms_request_send_tpdu");
    SMS_LOG_I("SMS_Route                                    = %d", SMS_Route);
    SMS_LOG_I("More_SMS_toSend                              = %d", More_SMS_toSend);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Length        = 0x%02x", sms_tpdu->ServiceCenterAddress.Length);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.TypeOfAddress = 0x%02x", sms_tpdu->ServiceCenterAddress.TypeOfAddress);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Value (Hex):\n");
    MAL_uint8_tBuf_PrintToLog(sms_tpdu->ServiceCenterAddress.AddressValue, sms_tpdu->ServiceCenterAddress.Length);
    SMS_LOG_I("SMSC_Addr.TPDU.Length                        = 0x%02x", sms_tpdu->TPDU.Length);
    SMS_LOG_I("SMSC_Addr.TPDU.Data (Hex):\n");
    MAL_uint8_tBuf_PrintToLog(sms_tpdu->TPDU.Data, sms_tpdu->TPDU.Length);

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}



/**

  This Api is used to acknowledge successful or failed receipt of SMS.

  @param RP_ERR_Cause: Error cause
  @param sms_tpdu:     TPDU to be sent to SMS server

  @return SMS_req_type return value

  Corresponding RIL command is:

  RIL_REQUEST_SMS_ACKNOWLEDGE

*/

int mal_sms_request_send_deliver_report(
    MAL_SMS_RP_ErrorCause_t RP_ERR_Cause,
    MAL_SMS_SMSC_Address_TPDU_t * sms_tpdu)
{
    mal_request_send_deliver_report_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_send_deliver_report_t, REQUEST_MAL_SMS_SEND_DELIVERREPORT);

    SMS_IDENTIFIER_NOT_USED(RP_ERR_Cause);

    SMS_LOG_I("Called mal_sms_request_send_deliver_report");
    SMS_LOG_I("RP_ERR_Cause                                 = 0x%02x", RP_ERR_Cause);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Length        = 0x%02x", sms_tpdu->ServiceCenterAddress.Length);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.TypeOfAddress = 0x%02x", sms_tpdu->ServiceCenterAddress.TypeOfAddress);
    SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Value (Hex):\n");
    MAL_uint8_tBuf_PrintToLog(sms_tpdu->ServiceCenterAddress.AddressValue, sms_tpdu->ServiceCenterAddress.Length);
    SMS_LOG_I("SMSC_Addr.TPDU.Length                        = 0x%02x", sms_tpdu->TPDU.Length);
    SMS_LOG_I("SMSC_Addr.TPDU.Data (Hex):\n");
    MAL_uint8_tBuf_PrintToLog(sms_tpdu->TPDU.Data, sms_tpdu->TPDU.Length);

    // Send to MAL SMS socket
    Sig_p->RP_Error = RP_ERR_Cause;
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}


int mal_cbs_request_routing_set(
    uint8_t SubscribetoAll,
    uint8_t SubscriptionNumber,
    uint8_t SIM_Subscription,
    MAL_CBS_MessageIdentifier_t * CBMI_List,
    uint8_t CBMI_List_Length,
    MAL_CBS_DataCodingScheme_t * DCS_List,
    uint8_t DCS_List_Length)
{
    mal_request_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_t, REQUEST_MAL_CBS_REQUEST_ROUTING_SET);

    SMS_IDENTIFIER_NOT_USED(SubscribetoAll);
    SMS_IDENTIFIER_NOT_USED(SubscriptionNumber);
    SMS_IDENTIFIER_NOT_USED(SIM_Subscription);
    SMS_IDENTIFIER_NOT_USED(CBMI_List);
    SMS_IDENTIFIER_NOT_USED(CBMI_List_Length);
    SMS_IDENTIFIER_NOT_USED(DCS_List);
    SMS_IDENTIFIER_NOT_USED(DCS_List_Length);

    SMS_LOG_I("Called mal_cbs_request_routing_set");
    SMS_LOG_I("Ignoring input parameters and assuming MAL SubscribeToAll is wanted");

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

int mal_cbs_request_routing_remove(
    uint8_t SubscriptionNumber)
{
    mal_request_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_t, REQUEST_MAL_CBS_REQUEST_ROUTING_REMOVE);

    SMS_IDENTIFIER_NOT_USED(SubscriptionNumber);

    SMS_LOG_I("Called mal_cbs_request_routing_remove");
    SMS_LOG_I("SubscriptionNumber = %d", SubscriptionNumber);

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

int mal_sms_set_route(
    mal_sms_route_info_t route_info)
{
    mal_request_route_set_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_route_set_t, REQUEST_MAL_SMS_MO_ROUTE_SET);

    Sig_p->sms_route = route_info;

    SMS_LOG_I("Called mal_sms_set_route");
    SMS_LOG_I("route_info = %d", route_info);

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

int mal_sms_get_route(
    void)
{
    mal_request_t *Sig_p = SMS_SIGNAL_ALLOC(mal_request_t, REQUEST_MAL_SMS_MO_ROUTE_GET);

    SMS_LOG_I("Called mal_sms_get_route");

    // Send to MAL SMS socket
    SMS_SEND(&Sig_p, MAL_SMS_fd);

    return MAL_SMS_CBS_SUCCESS;
}

int32_t netlnk_socket_recv(int32_t netlnkfd, int *msg)
{
    SMS_LOG_I("Called netlnk_socket_recv");
    (void)netlnkfd;
    (void)msg;
    return 0;
}
