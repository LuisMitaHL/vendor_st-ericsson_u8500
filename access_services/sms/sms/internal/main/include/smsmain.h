#ifndef SMSMAIN_H
#define SMSMAIN_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 * Main process loop for the SMS Server.
 *
 *************************************************************************/
#include "t_sms.h"
#include "t_cbs.h"

#ifdef SMS_USE_MFL_MODEM
#include "mfl.h"
#endif

#include "sim.h"
#include "cn_client.h"
#include "util_log.h"

typedef enum {
    MESSAGING_SYSTEM_WAIT_SIM_READY,
    MESSAGING_SYSTEM_VALIDATE_SM_STORAGE,
    MESSAGING_SYSTEM_IDLE,
    MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SAT_REFRESH,
    MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART,
    MESSAGING_SYSTEM_LAST_VALID_STATE,
    MESSAGING_SYSTEM_SHUTDOWN_STATE,
} MessagingSystemState_t;

#ifndef SMS_USE_MFL_MODEM
typedef enum SMS_MAL_Callback_DataType_e {
    SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_IND,
    SMS_MAL_CALLBACK_DATA_TYPE_MESSAGE_SEND_RESP,
    SMS_MAL_CALLBACK_DATA_TYPE_RECEIVE_MESSAGE_RESP,
    SMS_MAL_CALLBACK_DATA_TYPE_MEMORY_CAPACITY_EXC_IND,
    SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_REPORT_RESP,
    SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_RESP,
    SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_IND,
    SMS_MAL_CALLBACK_DATA_TYPE_ROUTE_INFO_SET_RESP,
    SMS_MAL_CALLBACK_DATA_TYPE_ROUTE_INFO_GET_RESP
} SMS_MAL_Callback_DataType_t;

// Data types for SMS_MAL_Callback handlers
// SMS_MAL_CALLBACK_RECEIVED_MSG_IND
typedef struct {
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
} SMS_MAL_Callback_Received_Msg_Ind_Data_t;

// SMS_MAL_CALLBACK_MESSAGE_SEND_RESP
typedef struct {
    SMS_TP_MessageReference_t Message_Ref;
    SMS_Error_t Error;
} SMS_MAL_Callback_Message_Send_Resp_Data_t;

// SMS_MAL_CALLBACK_RECEIVE_MESSAGE_RESP
typedef struct {
    SMS_Error_t Error;
} SMS_MAL_Callback_Receive_Message_Resp_Data_t;

// SMS_MAL_CALLBACK_MEMORY_CAPACITY_EXC_IND
typedef struct {
    // No payload data. However we define an "unused" structure.
    // This allows common code to be written for all callback
    // handlers.
    uint8_t Unused;
} SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data_t;

// SMS_MAL_CALLBACK_RECEIVED_MSG_REPORT_RESP
typedef struct {
    SMS_Error_t Error;
} SMS_MAL_Callback_Received_Msg_Report_Resp_Data_t;

// SMS_MAL_CALLBACK_CB_ROUTING_RESP
typedef struct {
    SMS_Error_t Error;
    uint8_t SubscriptionNumber;
} SMS_MAL_Callback_CB_Routing_Resp_Data_t;


// SMS_MAL_CALLBACK_ROUTE_INFO_SET_RESP
typedef struct {
    SMS_Error_t Error;
} SMS_MAL_Callback_Route_Info_Set_Resp_Data_t;

// SMS_MAL_CALLBACK_ROUTE_INFO_GET_RESP
typedef struct {
    SMS_Error_t Error;
    SMS_MO_Route_t Route;
} SMS_MAL_Callback_Route_Info_Get_Resp_Data_t;

union SMS_MAL_Callback_Data_u {
    SMS_MAL_Callback_Received_Msg_Ind_Data_t SMS_MAL_Callback_Received_Msg_Ind_Data;
    SMS_MAL_Callback_Message_Send_Resp_Data_t SMS_MAL_Callback_Message_Send_Resp_Data;
    SMS_MAL_Callback_Receive_Message_Resp_Data_t SMS_MAL_Callback_Receive_Message_Resp_Data;
    SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data_t SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data;
    SMS_MAL_Callback_Received_Msg_Report_Resp_Data_t SMS_MAL_Callback_Received_Msg_Report_Resp_Data;
    SMS_MAL_Callback_CB_Routing_Resp_Data_t SMS_MAL_Callback_CB_Routing_Resp_Data;
    SMS_MAL_Callback_CB_Routing_Ind_Data_t SMS_MAL_Callback_CB_Routing_Ind_Data;
    SMS_MAL_Callback_Route_Info_Set_Resp_Data_t SMS_MAL_Callback_Route_Info_Set_Resp_Data;
    SMS_MAL_Callback_Route_Info_Get_Resp_Data_t SMS_MAL_Callback_Route_Info_Get_Resp_Data;
};

typedef struct {
    SMS_MAL_Callback_DataType_t CallbackDataType;
    union SMS_MAL_Callback_Data_u Data;
} SMS_MAL_Callback_Data_t;
#endif


typedef struct SMS_MO_RouteData {
    SMS_SwBP_SignalInfo_t RouteSet;
    SMS_SwBP_SignalInfo_t RouteGet;
} SMS_MO_RouteData_t;


/*
 * FUNCTION PROTOTYPES
 */

//Special functions to control signal processing during initialisation
void MessagingSystem_SetState(
    const MessagingSystemState_t NewState);
void MessagingSystem_GetState(
    MessagingSystemState_t * State);

#ifndef SMS_USE_MFL_MODEM
void MessagingSystem_MAL_Callback_AddData(
    const SMS_MAL_Callback_Data_t * const SMS_MAL_Callback_Data_p);
void MessagingSystem_Set_MAL_ConfigurationState(
    const uint8_t SMS_MAL_ConfigState);
uint8_t MessagingSystem_Get_MAL_ConfigurationState(
    void);
#endif


#ifdef SMS_USE_MFL_MODEM
//Special functions to Get and Set the MFL modem Session data
void MessagingSystem_SetModemSessionData(
    modem_session_t * const MFL_Modem_Session_p);
modem_session_t *MessagingSystem_GetModemSessionData(
    void);
#endif

// Functions to get the SIM API objects
ste_sim_t *MessagingSystem_UICC_SimSyncGet(
    void);
ste_sim_t *MessagingSystem_UICC_SimAsyncGet(
    void);

// Functions to get Call Network objects
cn_context_t *MessagingSystem_Get_CN_Context(
    void);

#endif                          // SMSMAIN_H
