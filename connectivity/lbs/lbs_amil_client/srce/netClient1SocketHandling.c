/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/

#define NETCLIENT_SESSIONHANDLING_C

#include "netClientSocketHandling.h"

#define MAX_STRING_MCC_MNC  256

#define ENABLE_REPORTING 1

static int v_NetReqRespSckt = K_NETCLIENT_UNDEFINED_SOCKET;
static int v_NetEventSckt   = K_NETCLIENT_UNDEFINED_SOCKET;

cn_context_t *cn_context_ptr = 0;

const char             *netClient_cn_rat_type_name_t[CN_RAT_TYPE_EPS + 1] = {
    "CN_RAT_TYPE_UNKNOWN",
    "CN_RAT_TYPE_GPRS",
    "CN_RAT_TYPE_EDGE",
    "CN_RAT_TYPE_UMTS",
    "CN_RAT_TYPE_IS95A",
    "CN_RAT_TYPE_IS95B",
    "CN_RAT_TYPE_1xRTT",
    "CN_RAT_TYPE_EVDO_REV_0",
    "CN_RAT_TYPE_EVDO_REV_A",
    "CN_RAT_TYPE_HSDPA",
    "CN_RAT_TYPE_HSUPA",
    "CN_RAT_TYPE_HSPA",
    "CN_RAT_TYPE_GSM",
    "CN_RAT_TYPE_EPS",
};


/**
 * Initializes CN client parameters
 * Request registration status for Normal and GPRS
 * Request set event reporting
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
bool netClient1_1Init(int8_t vp_handle)
{
    bool vl_Error = FALSE;

    long vl_ThreadId = K_NETCLIENT_UICC_INIT_THREAD_ID_VALUE;
    pthread_t NetClientSocketHandlerThread;

    INF("%s: called\n", __func__);

    if(   ! cn_context_ptr
        && (v_NetReqRespSckt == K_NETCLIENT_UNDEFINED_SOCKET)
        && (v_NetEventSckt   == K_NETCLIENT_UNDEFINED_SOCKET))
    {
        netClient1_3CreateCallNetSession(vp_handle);
    }

    if (  ! cn_context_ptr
         || (v_NetReqRespSckt == K_NETCLIENT_UNDEFINED_SOCKET)
         || (v_NetEventSckt   == K_NETCLIENT_UNDEFINED_SOCKET) )
    {
        vl_Error = TRUE;
        INF("NetClient create failed\n");
    }

    return vl_Error;
}


/**
 * @brief Creates the Call Net Server session and gets the Net Server sockets.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
static void netClient1_3CreateCallNetSession(int8_t vp_handle)
{
    cn_error_code_t result,cn_result;
    INF("%s: called\n", __func__);

    /* Initialize client and update socket handles */
    result = cn_client_init(&cn_context_ptr);

    if (CN_SUCCESS != result || !cn_context_ptr) {
        ERR("cn: %s, Failed to connect to CN (result = 0x%x)\n", __func__, result);
        goto error_exit;
    } else {
        cn_result = cn_client_get_request_fd(cn_context_ptr, &v_NetReqRespSckt);
        if (CN_SUCCESS != cn_result) {
            ERR("cn_client_get_request_fd failed!\n");
            goto error_exit;
        }

        cn_result = cn_client_get_event_fd(cn_context_ptr, &v_NetEventSckt);
        if (CN_SUCCESS != cn_result) {
            ERR("cn_client_get_event_fd failed!\n");
            goto error_exit;
        }

        INF("%s: SOCKETS CREATED WITH FD: request_fd=%d event_fd=%d\n",__func__, v_NetReqRespSckt, v_NetEventSckt);
    }

    /* Send CN_REQUEST_REGISTRATION_STATE_NORMAL */
    result = cn_request_registration_state_normal(cn_context_ptr, (cn_client_tag_t) 113);
    if (CN_SUCCESS != result) {
        ERR("cn_request_registration_state_normal failed(%d)!\n",result);
        goto error_exit;
    } else {
        INF("cn_request_registration_state_normal Success\n");
    }

    /* Send CN_REQUEST_REGISTRATION_STATE_GPRS */
    result = cn_request_registration_state_gprs(cn_context_ptr, (cn_client_tag_t) 904);
    if (CN_SUCCESS != result) {
        ERR("cn_request_registration_state_gprs failed(%d)!\n",result);
        goto error_exit;
    }else {
        INF("cn_request_registration_state_gprs Success\n");
    }

    /* Sets event reporting for CN_MODEM_REGISTRATION_STATUS */
    result = cn_request_set_event_reporting(cn_context_ptr, CN_MODEM_REGISTRATION_STATUS, 1, (cn_client_tag_t) 8);
    if (CN_SUCCESS != result) {
        ERR("cn_request_set_event_reporting CN_MODEM_REGISTRATION_STATUS failed(%d)!\n", result);
        goto error_exit;
    }else {
        INF("cn_request_set_event_reporting Success\n");
    }
    result = cn_request_reg_status_event_config(cn_context_ptr, CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC,(cn_client_tag_t) 8);
        if (CN_SUCCESS != result) {
            ERR("cn_request_reg_status_event_config CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC failed(%d)!\n", result);
            goto error_exit;
        }else {
            INF("cn_request_reg_status_event_config Success\n");
        }

    /* The below routine is used to enable or disable unsolicited neighbour cell information for associated RAT type ( CN_NEIGHBOUR_RAT_TYPE_2G).
         Expected event type in case of successful invocation of this routine is CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_set_neighbour_cells_reporting(cn_context_ptr,ENABLE_REPORTING, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t)8);
    if (CN_SUCCESS != result) {
        ERR("cn_request_set_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_2G failed(%d)!\n",result);
        goto error_exit;
    } else {
        INF("cn_request_set_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_2G Success\n");
    }


    /* The below routine is used to enable or disable unsolicited neighbour cell information for associated RAT type ( CN_NEIGHBOUR_RAT_TYPE_3G).
         Expected event type in case of successful invocation of this routine is CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_set_neighbour_cells_reporting(cn_context_ptr,ENABLE_REPORTING, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t)8);
    if (CN_SUCCESS != result) {
        ERR("cn_request_set_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_3G failed(%d)!\n",result);
        goto error_exit;
    } else {
        INF("cn_request_set_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_3G Success\n");
    }


    /*The below routine is used to query unsolicited neighbour cell reporting status for RAT type (CN_NEIGHBOUR_RAT_TYPE_2G) . Expected event type in case
       of successful invocation of this routine is CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_reporting(cn_context_ptr, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t)8);
    if (CN_SUCCESS != result) {
        ERR("cn_request_get_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_2G failed(%d)!\n",result);
        goto error_exit;
    } else {
        INF("cn_request_get_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_2G Success\n");
    }


    /*The below routine is used to query unsolicited neighbour cell reporting status for RAT type (CN_NEIGHBOUR_RAT_TYPE_3G) . Expected event type in case
       of successful invocation of this routine is CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_reporting(cn_context_ptr, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t)8);
    if (CN_SUCCESS != result) {
        ERR("cn_request_get_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_3G failed(%d)!\n",result);
        goto error_exit;
    } else {
        INF("cn_request_get_neighbour_cells_reporting CN_NEIGHBOUR_RAT_TYPE_3G Success\n");
    }


    /* Register Call Net Server sockets */
    Amil1_06RegisterForListen(v_NetReqRespSckt, &netClient1_5HandleCnSocketRead, vp_handle);
    Amil1_06RegisterForListen(v_NetEventSckt,   &netClient1_5HandleCnSocketRead, vp_handle);

    INF("%s: done\n", __func__);

    return;

error_exit:
    ERR("%s: ERROR EXIT\n", __func__);
    return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
bool netClient1_5HandleCnSocketRead(void* pp_FileDes, int32_t vp_SelectReturn, int8_t vp_handle)
{
    bool vl_closed = FALSE;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_error_code_t result = CN_UNKNOWN;
    cn_error_code_t cn_result = CN_UNKNOWN;
    cn_registration_info_t *registration_info_p = NULL;
    cn_neighbour_cells_info_t *neighbour_cells_ecid_info_p = NULL;
    cn_bool_t *bool_type_p = NULL;
    int32_t socket = *((int32_t*)pp_FileDes);

    if( vp_SelectReturn )
    {
        result = cn_message_receive(socket, &size, &msg_p);

        if (CN_SUCCESS == result)
        {
            registration_info_p = (cn_registration_info_t *) msg_p->payload;

            switch(msg_p->type)
            {
                case CN_RESPONSE_REGISTRATION_STATE_NORMAL:
                    INF("Rx: CN_RESPONSE_REGISTRATION_STATE_NORMAL \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    netClient1_6HandleRespEvntRegistration(registration_info_p);
                    break;

                case CN_RESPONSE_REGISTRATION_STATE_GPRS:
                    INF("Rx: CN_RESPONSE_REGISTRATION_STATE_GPRS \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    netClient1_6HandleRespEvntRegistration(registration_info_p);
                    break;

                case CN_RESPONSE_SET_EVENT_REPORTING:
                    INF("\nRx: CN_RESPONSE_SET_EVENT_REPORTING : No Payload \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    break;

                case CN_EVENT_MODEM_REGISTRATION_STATUS:
                    INF("Rx: CN_EVENT_MODEM_REGISTRATION_STATUS \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    netClient1_6HandleRespEvntRegistration(registration_info_p);

                    if (registration_info_p->reg_status == CN_REGISTERED_TO_HOME_NW || registration_info_p->reg_status == CN_REGISTERED_ROAMING )
                    {

                        /*The below outine is used to query neighbouring cells information for RAT type (CN_NEIGHBOUR_RAT_TYPE_2G). Expected event type in case of successful
                           invocation of this routine is CN_RESPONSE_GET_NEIGHBOUR_CELLS_ECID_INFO */
                        cn_result = cn_request_get_neighbour_cells_extd_info(cn_context_ptr, CN_NEIGHBOUR_RAT_TYPE_2G,(cn_client_tag_t)8);
                        if (CN_SUCCESS != cn_result) {
                            ERR("cn_request_get_neighbour_cells_extd_info CN_NEIGHBOUR_RAT_TYPE_2G failed(%d)!\n",cn_result);
                        } else {
                            INF("cn_request_get_neighbour_cells_extd_info CN_NEIGHBOUR_RAT_TYPE_2G Success\n");
                        }


                        /*The below outine is used to query neighbouring cells information for RAT type (CN_NEIGHBOUR_RAT_TYPE_3G). Expected event type in case of successful
                           invocation of this routine is CN_RESPONSE_GET_NEIGHBOUR_CELLS_ECID_INFO */
                        cn_result = cn_request_get_neighbour_cells_extd_info(cn_context_ptr, CN_NEIGHBOUR_RAT_TYPE_3G,(cn_client_tag_t)8);
                        if (CN_SUCCESS != cn_result) {
                            ERR("cn_request_get_neighbour_cells_extd_info CN_NEIGHBOUR_RAT_TYPE_3G failed(%d)!\n",cn_result);
                        } else {
                            INF("cn_request_get_neighbour_cells_extd_info CN_NEIGHBOUR_RAT_TYPE_3G Success\n");
                        }
                    }


                    break;

                case CN_RESPONSE_REG_STATUS_EVENT_CONFIG:
                    INF("Rx: CN_RESPONSE_REG_STATUS_EVENT_CONFIG : No Payload \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    break;

                case CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING:
                    INF("Rx: CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    break;

                case CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING:
                    bool_type_p = (cn_bool_t *) msg_p->payload;
                    INF("Rx: CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    netClient1_15HandleReportingStatus(*bool_type_p);
                    break;

                case CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO:
                    neighbour_cells_ecid_info_p = (cn_neighbour_cells_info_t *) msg_p->payload;
                    INF("Rx: CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO \n");
                    DBG("%s:Socket=%d, type=%d, client_tag=%d, payload_size=%d, error_code=%d,  size_p=%d\n",
                         __func__, socket,msg_p->type, msg_p->client_tag, msg_p->payload_size, msg_p->error_code, size);
                    netClient1_14HandleECI(neighbour_cells_ecid_info_p);
                    break;

                default:
                    //ERR("Received Unexpected Message Type %d\n", msg_p->type);
                    break;
            }
            free(msg_p);
            msg_p = NULL;
        } else if (CN_FAILURE == result) {
            /*
             * A result code of CN_FAILURE means that the CN server has
             * disconnected its end of the socket. As a CN client this
             * process must disconnect its end and then try connecting
             * again. By returning TRUE the main loop will de-initilize
             * this module and then try initializing it again.
             *
             * However, because there are two sockets, one for request/
             * response, and one for events, both sockets must be
             * disconnected before TRUE is returned. Just deregister
             * monitoring of the socket to prevent the select syscall
             * from looping on it.
             */
            ERR("%s : Call Net Server disconnect detected, fd %d.\n", __func__, socket);

            // Stop monitoring the socket
            if (socket == v_NetReqRespSckt) {
                Amil1_07DeRegister(v_NetReqRespSckt, vp_handle);
                v_NetReqRespSckt = K_NETCLIENT_UNDEFINED_SOCKET;
            } else if (socket == v_NetEventSckt) {
                Amil1_07DeRegister(v_NetEventSckt, vp_handle);
                v_NetEventSckt = K_NETCLIENT_UNDEFINED_SOCKET;
            } else {
                ERR("%s : Unexpected socket fd%d!\n", __func__, socket);
            }

            // Both sockets failing?
            if (   v_NetReqRespSckt == K_NETCLIENT_UNDEFINED_SOCKET
                && v_NetEventSckt   == K_NETCLIENT_UNDEFINED_SOCKET ) {
                // Yes, return TRUE and the DeInit function will be called
                vl_closed = TRUE;
            }

        } else {
            ERR("%s : cn_message_receive() unexpected result code %d\n", __func__, result);
        }
    }

    return vl_closed;
}



/**
 * @brief Handles Response for registration state normal, GPRS and CN_EVENT_MODEM_REGISTRATION_STATUS
 *           and also updates structure s_gnsSUPL_MobileInfo
 *
 * @param registration_info_p  registration info ptr rcvd from server.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6

void netClient1_6HandleRespEvntRegistration(cn_registration_info_t *registration_info_p)
{
    s_gnsSUPL_MobileInfo vl_MobileInfo;
    uint16_t vl_MCC, vl_MNC;
    memset (&vl_MobileInfo, 0, sizeof(vl_MobileInfo));

    INF("RAW REG INFO==> MCC_MNC:\"%s\",REG STATUS:%d, RAT:%d, CID:%d, LAC:%d, SEARCH MODE:%d, Long OP Name:%s,Short OP Name:%s\n",
        registration_info_p->mcc_mnc, registration_info_p->reg_status, registration_info_p->rat, registration_info_p->cid,
        registration_info_p->lac, registration_info_p->search_mode, registration_info_p->long_operator_name,
        registration_info_p->short_operator_name);

    netClient1_7GetMccandMnc(registration_info_p->mcc_mnc, &vl_MCC, &vl_MNC);

    switch(registration_info_p->reg_status) {
        case CN_NOT_REG_NOT_SEARCHING:
            vl_MobileInfo.v_RegistrationState = E_gnsSUPL_REGISTRATION_MOBILE_OFF;
            INF("Registration State(%d): CN_NOT_REG_NOT_SEARCHING\n",registration_info_p->reg_status);
            break;
        case CN_REGISTERED_TO_HOME_NW:
        case CN_REGISTERED_ROAMING:
            vl_MobileInfo.v_RegistrationState = E_gnsSUPL_REGISTRATION_SERVICE_ALLOWED;
            INF("Registration State(%d): CN_REGISTERED_TO_HOME_NW(1)/CN_REGISTERED_ROAMING(5)\n",registration_info_p->reg_status);
            break;
        case CN_NOT_REG_SEARCHING_OP:
            vl_MobileInfo.v_RegistrationState = E_gnsSUPL_REGISTRATION_NO_CARRIER;
            INF("Registration State(%d): CN_NOT_REG_SEARCHING_OP\n",registration_info_p->reg_status);
            break;
        case CN_REGISTRATION_DENIED:
            vl_MobileInfo.v_RegistrationState = E_gnsSUPL_REGISTRATION_EMERGENCY_IDLE;
            INF("Registration State(%d): CN_REGISTRATION_DENIED\n",registration_info_p->reg_status);
            break;
           default:
            INF("Registration State(%d): CN_REGISTRATION_UNKNOWN\n",registration_info_p->reg_status);
            break;
    }

    vl_MobileInfo.v_CellInfoStatus = E_gnsSUPL_CELL_INFO_CURRENT;

    switch( registration_info_p->rat) {
        case CN_RAT_TYPE_GSM :
        case CN_RAT_TYPE_GPRS :
        case CN_RAT_TYPE_EDGE :
            vl_MobileInfo.v_CellInfoType = E_gnsSUPL_GSM_CELL;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MCC = vl_MCC;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MNC = vl_MNC;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_CellIdentity = (uint16_t)registration_info_p->cid;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC = (uint32_t) registration_info_p->lac;
            //vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells = 0;
            INF("RAT Type: %s, MCC: %d, MNC: %d, CID: %d, LAC: %d\n",netClient_cn_rat_type_name_t[registration_info_p->rat],vl_MCC, vl_MNC,(uint16_t)registration_info_p->cid,(uint32_t) registration_info_p->lac );
            break;

        case CN_RAT_TYPE_UMTS :
        case CN_RAT_TYPE_HSDPA :
        case CN_RAT_TYPE_HSUPA :
        case CN_RAT_TYPE_HSPA :
            vl_MobileInfo.v_CellInfoType = E_gnsSUPL_WCDMA_FDD_CELL;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MCC = vl_MCC;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MNC = vl_MNC;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_CellIdentity = registration_info_p->cid;
            INF("RAT Type: %s, MCC: %d, MNC: %d, CID: %d \n",netClient_cn_rat_type_name_t[registration_info_p->rat],vl_MCC, vl_MNC,(uint16_t)registration_info_p->cid );
            break;

        default:
            ERR("RAT (%d): %s, not supported\n",registration_info_p->rat, netClient_cn_rat_type_name_t[registration_info_p->rat]);
            break;
    }

    if( (vl_MCC != 0) && (registration_info_p->reg_status != CN_NOT_REG_NOT_SEARCHING)
        && (registration_info_p->reg_status != CN_REGISTRATION_UNKNOWN)
        && (registration_info_p->reg_status != CN_REGISTRATION_DENIED)
        && (registration_info_p->reg_status != CN_NOT_REG_SEARCHING_OP) )
    {
        #ifdef AGPS_UP_FTR
            GNS_SuplMobileInfoInd(&vl_MobileInfo);
            INF("GNS_SuplMobileInfoInd Updated \n");
        #endif
    }
    else
    {
        INF("GNS_SuplMobileInfoInd Not Updated \n");
    }
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7

void netClient1_7GetMccandMnc(char *mcc_mnc, uint16_t *val_Mcc, uint16_t *val_Mnc)
{
    char vl_Temp[MAX_STRING_MCC_MNC];
    int mnc_len;

    if(mcc_mnc == NULL || strcmp(mcc_mnc,"")== 0) {
        *val_Mcc = 0;
        *val_Mnc = 0;
        //ERR("netClient1_7GetMccandMnc: MCC_MNC string contains No Data\n");
        return;
    }
    mnc_len = strlen(mcc_mnc)-3;

    memset(vl_Temp, 0, MAX_STRING_MCC_MNC);

    /* Copy the MCC field into temporary area */
    strncpy(vl_Temp, mcc_mnc, 3);

    /* Extract the MCC as a number */
    *val_Mcc = (uint16_t)atoi( vl_Temp );

    memset(vl_Temp, 0, MAX_STRING_MCC_MNC);
    if( mnc_len >0) {
        /* Copy the MNC field into temporary area */
        strncpy(vl_Temp, mcc_mnc+3, strlen(mcc_mnc) - 3);
        /* Extract the MCC as a number */
        *val_Mnc = (uint16_t)atoi( vl_Temp );
        } else {
        *val_Mnc = 0;
    }
    //DBG("netClient1: Mobile info - MCC: %u, MNC:%u\n", *val_Mcc,*val_Mnc);
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8

static int netClient1_8GetHighestSocketValue (void)
{
   // Return highest socket number
   return (v_NetReqRespSckt > v_NetEventSckt) ? v_NetReqRespSckt : v_NetEventSckt;

}

/**
 * @brief Returns a timeval structure which can be used in a select() call
 *        so that a basic timeout can be supported. If no timeout is
 *        currently active, NULL is returned.
 *
 * @return timeval structure, or NULL, for use with select().
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
static struct timeval *netClient1_9GetTimeoutForSelect(void)
{
    t_netClient_TimeoutInfo *pl_TimeoutInfo;
    struct timeval* pl_SelectTimeout = NULL;

    pl_TimeoutInfo = netClient1_10GetTimeoutInfo();

    if (pl_TimeoutInfo != NULL && pl_TimeoutInfo->v_TimerSet) {
        struct timeval vl_TimeNow;

        // When v_TimerSet is TRUE a timer has been set. However when select() is called it is possible that
        // data will be detected on a socket before the timer has expired.  This could happen in such a way
        // that the timer should have expired before select() is called again.  Therefore if this happens we
        // should continue to use a timeout with select(), however the timeout value should be 0. This will
        // force an immediate timer expiry from select(); again however there is a possiblity that there will
        // have been data detected on one of the sockets so the timer is not reset...and we go around the
        // loop again with a timeout of 0.  When select() finally returns due to the timeout, v_TimerSet is
        // set to FALSE and the timeout "actions" performed; and then the timer functionality is disabled.

        // We are not using the tv_usec member of struct timeval so set it to 0 and forget about it.
        pl_TimeoutInfo->v_SelectTimeout.tv_usec = 0;

        // Now check if any time has elapsed since smsClient1_select_timeout_set() was called and which
        // should be deducted from the timeout value passed to select().
        if (gettimeofday(&vl_TimeNow, NULL) == 0) {
            if (vl_TimeNow.tv_sec > pl_TimeoutInfo->v_TimerCreatedNow.tv_sec) {
                // Time has elapsed since timeout was first set
                const long vl_TimeDiff = vl_TimeNow.tv_sec - pl_TimeoutInfo->v_TimerCreatedNow.tv_sec;

                if (vl_TimeDiff < pl_TimeoutInfo->v_TimerLengthSecs) {
                    // Time elapsed is less than the initial length of the timer.
                    pl_TimeoutInfo->v_SelectTimeout.tv_sec = pl_TimeoutInfo->v_TimerLengthSecs - vl_TimeDiff;
                } else {
                    // Time elapsed is more than, or equal to, the initial length of the timer. Therefore
                    // set the timeout value to be 0 which will force an immediate expiry when select()
                    // is called.
                    pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
                }
            } else {
                // No time has elapsed since timeout was first set.
                pl_TimeoutInfo->v_SelectTimeout.tv_sec = pl_TimeoutInfo->v_TimerLengthSecs;
            }
        } else {
            // Something is wrong as we can't get the current time. Set the timeout value to be 0 which
            // will force an immediate expiry when select() is called.
            pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
            ERR("gettimeofday() FAILED\n");
        }

        INF("v_SelectTimeout %ld secs\n", pl_TimeoutInfo->v_SelectTimeout.tv_sec);
        pl_SelectTimeout = &pl_TimeoutInfo->v_SelectTimeout;
    }
    //INF("tv_sec = %ld secs, usec = %ld usecs\n", pl_SelectTimeout->tv_sec, pl_SelectTimeout->tv_usec);
    return pl_SelectTimeout;
}


/**
 * @brief Returns a data structure needed to support the timer functionality. If the
 *        data structure has not been created, previously, it is malloc'd and initialised.
 *
 * @return Data structure.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
static t_netClient_TimeoutInfo *netClient1_10GetTimeoutInfo(void)
{
    static t_netClient_TimeoutInfo *pl_TimeoutInfo = NULL;

    //INF("pl_TimeoutInfo %08X\n", (uint32_t)pl_TimeoutInfo);

    if (pl_TimeoutInfo == NULL) {
        // Memory has not been allocated previously
        pl_TimeoutInfo = (t_netClient_TimeoutInfo *) OSA_Malloc(sizeof(t_netClient_TimeoutInfo));

        if (pl_TimeoutInfo != NULL) {
            pl_TimeoutInfo->v_TimerSet = FALSE;
            pl_TimeoutInfo->v_TimerLengthSecs = 0;
            pl_TimeoutInfo->v_TimerCreatedNow.tv_sec = 0;
            pl_TimeoutInfo->v_TimerCreatedNow.tv_usec = 0;
            pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
            pl_TimeoutInfo->v_SelectTimeout.tv_usec = 0;
        } else
            ERR("Malloc failed\n");
    }

    return pl_TimeoutInfo;
}

/**
 * @brief Sets initial data needed to specify a timeout with select(). If
 *        called with a non-zero value, before a previous timer has expired,
 *        the new timer value will overwrite the previous value.
 *
 * @param vp_Seconds Duration, in seconds, for timer.  Set to 0 to clear a timer.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
void netClient1_11SelectTimeoutSet(const long vp_Seconds)
{
    t_netClient_TimeoutInfo *pl_TimeoutInfo;

    INF("timeout: %lds\n", vp_Seconds);

    pl_TimeoutInfo = netClient1_10GetTimeoutInfo();

    if (pl_TimeoutInfo != NULL) {
        if (vp_Seconds > 0) {
            // Setting a new timer.
            pl_TimeoutInfo->v_TimerSet        = TRUE;
            pl_TimeoutInfo->v_TimerLengthSecs = vp_Seconds;

            // Store time when timer was set.
            (void) gettimeofday(&pl_TimeoutInfo->v_TimerCreatedNow, NULL);
        } else {
            // Clearing a timer.
            pl_TimeoutInfo->v_TimerSet        = FALSE;
            pl_TimeoutInfo->v_TimerLengthSecs = 0;
        }
    } else
        ERR("pl_TimeoutInfo is NULL\n");
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12

void netClient1_12Deinit(int8_t vp_handle)
{

    INF("%s : called\n", __func__);

    if (   cn_context_ptr   != NULL
        || v_NetReqRespSckt != K_NETCLIENT_UNDEFINED_SOCKET
        || v_NetEventSckt   != K_NETCLIENT_UNDEFINED_SOCKET) {
        // Deregister both the FDs even if any one of the FD is defined.

        netClient1_13ShutdownCallNetSession(vp_handle);
    }

    INF("%s : done\n", __func__);
}


/**
 * @brief Shuts down the Call Net Server session and de-registers the sockets.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
static void netClient1_13ShutdownCallNetSession(int8_t vp_handle)
{
    cn_error_code_t result;

    INF("%s: called\n", __func__);

    if (!cn_context_ptr) {
        return;
    }

    /* De-register request/response socket */
    if (v_NetReqRespSckt != K_NETCLIENT_UNDEFINED_SOCKET) {
        INF("%s: FD:%d,  Handle : %d\n", __func__, v_NetReqRespSckt, vp_handle);
        Amil1_07DeRegister(v_NetReqRespSckt, vp_handle);
        v_NetReqRespSckt = K_NETCLIENT_UNDEFINED_SOCKET;
    }

    /* De-register event socket */
    if (v_NetEventSckt != K_NETCLIENT_UNDEFINED_SOCKET) {
        INF("%s: FD:%d,  Handle : %d\n", __func__, v_NetEventSckt, vp_handle);
        Amil1_07DeRegister(v_NetEventSckt, vp_handle);
        v_NetEventSckt = K_NETCLIENT_UNDEFINED_SOCKET;
    }

    /* Shut down client */
    result = cn_client_shutdown(cn_context_ptr);

    if (CN_SUCCESS != result) {
        INF("%s: cn_client_shutdown() returns error %d\n", __func__, result);
    }

    /* Context pointer is no longer valid, reset */
    cn_context_ptr = 0;

    INF("%s: done\n", __func__);
}


/**
 * @brief Populate Enhanced cell id parameters which includes
 * Serving cell information and Neighbour cell information
 * both for 2G/3G
 *
 * @param neighboring_cell_p, Information related to gsm/umts serving and neighbour cells
 */

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
void netClient1_14HandleECI(cn_neighbour_cells_info_t *neighboring_cell_p)
{
    DBG("+netClient1_14HandleECI");

    int8_t v1_neighbourcellcounter = 0;
    int8_t v1_numcellscounter = -1;
    int8_t v1_neighbourcellmeasured = 0;
    int8_t v1_gsmnmrlist = 0;
    int8_t v1_cpichrscp = 0;
    int8_t v1_rxlev = 0;

    uint16_t vl_MCC, vl_MNC;

    switch(neighboring_cell_p->rat_type)
    {
        case CN_NEIGHBOUR_RAT_TYPE_3G:
        {
            ALOGD("netClient1_14HandleECI : in 3G");

            /* RAT Type registered */

            vl_MobileInfo.v_CellInfoType = neighboring_cell_p->rat_type;

            /* Serving Cell information */

            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MCC = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MNC = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_CellIdentity = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_PrimaryScramblingCode = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_psc;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_Uarfcn_DL = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn;

            /* Neighbour Cell information */

            for (v1_neighbourcellcounter = 0;v1_neighbourcellcounter<vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells; )
            {
                v1_numcellscounter +=1;

                if (neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_numcellscounter].num_of_cells == 0)
                {
                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells -=1;
                    continue;
                }

                vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_Uarfcn_DL
                   = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].dl_uarfcn;
                vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_UTRACarrierRSS
                   = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].utra_carrier_rssi;
                vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_NumberOfCellsMeasured
                   = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].num_of_cells;


                for (v1_neighbourcellmeasured = 0;v1_neighbourcellmeasured<vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_NumberOfCellsMeasured;v1_neighbourcellmeasured++)
                {

                    v1_cpichrscp = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].umts_cell_detailed_info[v1_neighbourcellmeasured].cpich_rscp;
                    if (v1_cpichrscp < -120) {
                        /* No value with id -127 is thereby reported as 0 */
                        v1_cpichrscp = -120;
                    } else if (v1_cpichrscp > 11) {
                        v1_cpichrscp = 11;
                    }

                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_RSCP
                       = (uint8_t)((((double)(120 + v1_cpichrscp) / 131)) * 91);

                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_PathLoss
                       = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].umts_cell_detailed_info[v1_neighbourcellmeasured].cpich_pathloss;
                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_EcNo
                       = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].umts_cell_detailed_info[v1_neighbourcellmeasured].cpich_ecno;
                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CellIdentity
                       = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].umts_cell_detailed_info[v1_neighbourcellmeasured].ucid;
                    vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_PrimaryScramblingCode
                       = neighboring_cell_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[v1_neighbourcellcounter].umts_cell_detailed_info[v1_neighbourcellmeasured].psc;


                }

                v1_neighbourcellcounter++;

            }

            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells = vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells + 1;

            /* Appending Serving cell information to Neighbour cell information */

            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_Uarfcn_DL
                  = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_UTRACarrierRSS
                  = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_NumberOfCellsMeasured +=1;


            v1_cpichrscp = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp;
            if (v1_cpichrscp < -120) {
                /* No value with id -127 is thereby reported as 0 */
                v1_cpichrscp = -120;
            } else if (v1_cpichrscp > 11) {
                v1_cpichrscp = 11;
            }

            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[0].v_CPICH_RSCP
               = (uint8_t)((((double)(120 + v1_cpichrscp) / 131)) * 91);

            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[0].v_CPICH_PathLoss
               = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[0].v_CPICH_EcNo
               = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[0].v_CellIdentity
               = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid;
            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[0].v_PrimaryScramblingCode
               = neighboring_cell_p->cells_info.umts_cells_info.umts_serving_cell_info.current_psc;


            ALOGD("DEBUG netClient1_14HandleECI WCDMA message Serving Cell :MCC =%u,MNC =%u,CellIdentity =%u,PrimaryScramblingCode =%u, NumNeighbouringCells =%u, Uarfcn_DL =%u",
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MCC, \
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MNC, \
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_CellIdentity,\
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_PrimaryScramblingCode,\
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells,\
                     vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_Uarfcn_DL
                     );



            for (v1_neighbourcellcounter = 0;v1_neighbourcellcounter<vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells;v1_neighbourcellcounter++)
            {

                ALOGD("DEBUG netClient1_14HandleECI WCDMA Neighbouring Cell 1 :Uarfcn_DL =%u,UTRACarrierRSS =%u,NumberOfCellsMeasured =%u",
                         vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_Uarfcn_DL, \
                         vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_UTRACarrierRSS, \
                         vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_NumberOfCellsMeasured
                         );

                for (v1_neighbourcellmeasured = 0;v1_neighbourcellmeasured<vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].v_NumberOfCellsMeasured;v1_neighbourcellmeasured++)
                    {
                        ALOGD("DEBUG netClient1_14HandleECI WCDMA Neighbouring Cell 2 :CPICH_RSCP =%d,CPICH_PathLoss =%u,CPICH_EcNo =%u, UCID =%u, PSC = %u",
                            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_RSCP, \
                            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_PathLoss, \
                            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CPICH_EcNo, \
                            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_CellIdentity, \
                            vl_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.a_WcdmaMeasResultsListFDD[v1_neighbourcellcounter].a_CellMeasuredResultsList[v1_neighbourcellmeasured].v_PrimaryScramblingCode
                            );
                    }
            }



            GNS_SuplMobileInfoInd(&vl_MobileInfo);
            INF("GNS_SuplMobileInfoInd Updated for 3G \n");


        }
        break;
        case CN_NEIGHBOUR_RAT_TYPE_2G:
        {
            ALOGD("netClient1_14HandleECI : in 2G");

            vl_MobileInfo.v_CellInfoType = neighboring_cell_p->rat_type;

            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_TimingAdvance
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MCC
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MNC
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_CellIdentity
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours;


            for (v1_gsmnmrlist = 0;v1_gsmnmrlist<vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells;v1_gsmnmrlist++)
            {
                vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_ARFCN
                    = neighboring_cell_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[v1_gsmnmrlist].arfcn;
                vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_BSIC
                    = neighboring_cell_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[v1_gsmnmrlist].bsic;

                v1_rxlev = neighboring_cell_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[v1_gsmnmrlist].rxlev;
                /* range of value from CN is -110 to -47 */
                if (v1_rxlev < 0)
                {
                    if (v1_rxlev < -110) {
                        v1_rxlev = -110;
                    }

                    if (v1_rxlev > -47) {
                        v1_rxlev = -47;
                    }
                    v1_rxlev += 110;
                }

                vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_RXLev
                    = (uint8_t)v1_rxlev;


            }


            /* Appending Serving cell information to Neighbour cell information */

            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells +=1;

            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_ARFCN
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn;
            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_BSIC
                = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic;

            v1_rxlev = neighboring_cell_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev;
            /* range of value from CN is -110 to -47 */
            if (v1_rxlev < 0)
            {
                if (v1_rxlev < -110) {
                    v1_rxlev = -110;
                }

                if (v1_rxlev > -47) {
                    v1_rxlev = -47;
                }
                v1_rxlev += 110;
            }

            vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_RXLev
                = (uint8_t)v1_rxlev;


            ALOGD("netClient1_14HandleECI GSM serving cell :NumNeighbouringCells =%u,TimingAdvance =%u,MCC =%u,MNC =%u, LAC =%u, CellIdentity =%u",
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells, \
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_TimingAdvance, \
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MCC,\
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MNC,\
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC,\
                     vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_CellIdentity
                     );

            for (v1_gsmnmrlist = 0;v1_gsmnmrlist<vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells;v1_gsmnmrlist++)
            {

                ALOGD("netClient1_14HandleECI GSM neighbour cell: gsmnmrlist =%u, ARFCN =%u, BSIC = %u, RXLev=%d",
                              v1_gsmnmrlist,\
                              vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_ARFCN, \
                              vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_BSIC, \
                              vl_MobileInfo.v_CellInfo.v_GsmCellInfo.a_GsmNMR[v1_gsmnmrlist].v_RXLev
                              );

            }


            GNS_SuplMobileInfoInd(&vl_MobileInfo);
            INF("GNS_SuplMobileInfoInd Updated for 2G \n");


        }
        break;
        case CN_NEIGHBOUR_RAT_TYPE_UNKNOWN:
           ERR("CN_NEIGHBOUR_RAT_TYPE_UNKNOWN type is not handled");
           INF("GNS_SuplMobileInfoInd Not Updated \n");
           break;
        default:
           INF("RAT Type Unhandled - (%d)\n",neighboring_cell_p->rat_type);
           break;
    }


    DBG("-netClient1_14HandleECI");
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
void netClient1_15HandleReportingStatus(cn_bool_t bool_type_p)
{
    INF("Unsolicited neighbour cell reporting status = %d\n",bool_type_p);

    vl_GetNeighbourCellsReporting = bool_type_p;

}
