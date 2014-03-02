/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*----------------------------------------------------------------------------------*/
/* Name: mfl_cas_stub.c                                                             */
/* MFL Framework stub implementation file for SMS Library                           */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/


#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

#include "mfl.h"
#include "smsipc.h"

#include "t_sms.h"
#define LOG_TAG "MFL SMS Stub (StubSide)"
#include "r_smslinuxporting.h"
#include "mfl_stub.h"
#include "r_dispatch.h"

static const long tTimer = 122;
static const long tMFL = MFL_MODEM_THREAD;



// Test harness "version" of dispatch function. It actually only sends data to the SMS Server process
// and has no "subscriptions" running behind it.
// Sends a fake MT SMS message to the SMS Server.
void Dispatch_RMS_V2_MessageReceivedInd_Stub(
    const RMS_V2_Address_t * const SC_Address_p,
    const RMS_V2_UserData_t * const UserData_p,
    const boolean AcknowledgeRequired,
    int fd)
{
    SMS_LOG_I("Called Dispatch_RMS_V2_MessageReceivedInd_Stub");

    if (SC_Address_p != NULL && UserData_p != NULL) {
        RMS_Event_V2_MessageReceivedInd_Stub_t *Event_p = SMS_SIGNAL_ALLOC(RMS_Event_V2_MessageReceivedInd_Stub_t, EVENT_RMS_V2_MESSAGERECEIVEDIND);

        if (Event_p != NULL) {
            memcpy(&Event_p->SC_Address, SC_Address_p, sizeof(RMS_V2_Address_t));
            memcpy(&Event_p->UserData, UserData_p, sizeof(RMS_V2_UserData_t));
            Event_p->AcknowledgeRequired = AcknowledgeRequired;
            SMS_SEND(&Event_p, fd);
            SMS_LOG_I("Dispatch_RMS_V2_MessageReceivedInd_Stub Event Dispatched OK");
        } else {
            SMS_LOG_E("Dispatch_RMS_V2_MessageReceivedInd_Stub: Signal alloc failed");
        }
    } else {
        SMS_LOG_E("Dispatch_RMS_V2_MessageReceivedInd_Stub: NULL pointers");
    }
}


/* Stub process/thread implementation */
/**********************************************************************
* CleanUp - This is called by the IPC-lib when a client context has disconnected
*           (= client context socket closed), for example if the application process
*           that was communicating with your demon crashed or closed down, the IPC-lib
*     will detect this and invoke this call-back. Then you can free up the
*     resources associated with this communication and have it ready to accept
*     a new connection.
***********************************************************************/

static void MFL_CleanUp(
    const int EventSocket)
{                               // not implemented for this example
    SMS_LOG_I("MFL_CleanUp called");

    SMS_IDENTIFIER_NOT_USED(EventSocket);
}


/*
typedef struct
{
  uint8_t  Length;
  uint8_t  TypeOfAddress;
  uint8_t  AddressValue[SMS_MAX_PACKED_ADDRESS_LENGTH];
} SMS_PackedAddress_t;
typedef struct
{
  uint8_t  Length;
  uint8_t  Data[SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH];
} SMS_TPDU_t;
typedef struct
{
  SMS_PackedAddress_t  ServiceCenterAddress;
  SMS_TPDU_t           TPDU;
} SMS_SMSC_Address_TPDU_t;*/
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




/*************************************************************************
**************************************************************************
*
* Process:  MFL_SMS_Client - RX process
*
* This thread operates in a SERVER context, listening for and handling
* various requests. It also dispatches a simple event to subscribing clients.
*
**************************************************************************
**************************************************************************/
static void *MFL_SMS_Client(
    void *threadid)
{
    struct ipc_server_context_t *pMFL_SMS_IPC_ServerContext = NULL;
    union SMS_SIGNAL *ReceivedSignal_p = NULL;

    SMS_LOG_I("Started!, Thread ID = %d ", (int) threadid);

    pMFL_SMS_IPC_ServerContext = ipc_server_context_create((int) threadid, MFL_SMS_SOCK_PATH, NULL,     //BuildFDs,
                                                           NULL,        //HandleFD_Changes,
                                                           MFL_CleanUp);

    if (pMFL_SMS_IPC_ServerContext == NULL) {
        SMS_LOG_I("ipc_server_context_create returned NULL - oops");
    }

    start_up_sequence_complete();

    while (TRUE) {              // endless loop
        ReceivedSignal_p = ipc_server_context_await_signal(pMFL_SMS_IPC_ServerContext); // instead of OSE RECEIVE

        SMS_LOG_I("ipc_server_context_await_signal returned");

        if (ReceivedSignal_p == NULL) { // this should not happen since this server context is only interested in signals for it's own socket
            // and does not monitor any additional sockets - sw error
            SMS_LOG_E("ReceivedSignal_p == NULL");
        } else {
            switch (ReceivedSignal_p->Primitive) {
            case R_REQ_RMS_V2_MESSAGE_SEND:
                {
                    RMS_Req_MessageSend_Stub_t *Req_p = (RMS_Req_MessageSend_Stub_t *) ReceivedSignal_p;
                    RMS_Resp_MessageSend_Stub_t *Resp_p = SMS_SIGNAL_ALLOC(RMS_Resp_MessageSend_Stub_t, R_RESP_RMS_V2_MESSAGE_SEND);

                    SMS_LOG_I("[ R_REQ_RMS_V2_MESSAGE_SEND ] - Received, SENDER:0x%08X", SMS_SENDER(&ReceivedSignal_p));

                    if (Resp_p != NULL) {
                        Resp_p->SigselectWithClientTag.ClientTag = Req_p->SigselectWithClientTag.ClientTag;
                        Resp_p->RMS_ErrorCode = RMS_V2_ERROR_CODE_INTERN_NO_ERROR;
                        memcpy(&Resp_p->UserData, &Req_p->UserData, sizeof(RMS_V2_UserData_t));

                        SMS_SEND(&Resp_p, SMS_SENDER(&ReceivedSignal_p));
                    } else {
                        SMS_LOG_E("[ R_REQ_RMS_V2_MESSAGE_SEND ] - Signal Alloc Failed");
                    }

                    // Now try sending an async SMS received event
                    {
                        RMS_V2_Address_t *SC_Address_p = malloc(sizeof(RMS_V2_Address_t));
                        RMS_V2_UserData_t *UserData_p = malloc(sizeof(RMS_V2_UserData_t));
                        boolean AcknowledgeRequired = FALSE;

                        if (SC_Address_p != NULL && UserData_p != NULL) {
                            SC_Address_p->Length = MIN(RMS_V2_MAX_SM_ADDRESS_LENGTH, SMSC_Address_TPDU.ServiceCenterAddress.Length);
                            SC_Address_p->TypeOfAddess = SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress;
                            memcpy(SC_Address_p->AddressValue, SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, SC_Address_p->Length);

                            UserData_p->UserDataLength = MIN(RMS_V2_MAX_RP_USER_DATA_LENGTH + 1, SMSC_Address_TPDU.TPDU.Length);
                            memcpy(UserData_p->UserDataBody, SMSC_Address_TPDU.TPDU.Data, UserData_p->UserDataLength);

                            Dispatch_RMS_V2_MessageReceivedInd_Stub(SC_Address_p, UserData_p, AcknowledgeRequired, SMS_SENDER(&ReceivedSignal_p));
                        }

                        free(SC_Address_p);
                        free(UserData_p);
                    }
                    break;
                }

            case R_REQ_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT:
                {
                    RMS_Req_Message_SendAcknowledgement_Stub_t *Req_p = (RMS_Req_Message_SendAcknowledgement_Stub_t *) ReceivedSignal_p;
                    RMS_Resp_Message_SendAcknowledgement_Stub_t *Resp_p = SMS_SIGNAL_ALLOC(RMS_Resp_Message_SendAcknowledgement_Stub_t,
                                                                                           R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT);

                    SMS_LOG_I("[ R_REQ_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT ] - Received, SENDER:0x%08X", SMS_SENDER(&ReceivedSignal_p));

                    if (Resp_p != NULL) {
                        Resp_p->SigselectWithClientTag.ClientTag = Req_p->SigselectWithClientTag.ClientTag;
                        Resp_p->RMS_ErrorCode = 0x2000; // Equivalent to SMS_ERROR_INTERN_NO_ERROR

                        SMS_SEND(&Resp_p, SMS_SENDER(&ReceivedSignal_p));
                    } else {
                        SMS_LOG_E("[ R_REQ_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT ] - Signal Alloc Failed");
                    }
                    break;
                }

            case REQUEST_EVENTCHANNEL_SUBSCRIBE:
                {
                    Request_EventChannel_Subscribe_Stub_t *Req_p = (Request_EventChannel_Subscribe_Stub_t *) ReceivedSignal_p;
                    Response_EventChannel_Subscribe_Stub_t *Resp_p = SMS_SIGNAL_ALLOC(Response_EventChannel_Subscribe_Stub_t,
                                                                                      RESPONSE_EVENTCHANNEL_SUBSCRIBE);

                    SMS_LOG_I("[ REQUEST_EVENTCHANNEL_SUBSCRIBE ] - Received, SENDER:0x%08X", SMS_SENDER(&ReceivedSignal_p));

                    if (Resp_p != NULL) {
                        Resp_p->SigselectWithClientTag.ClientTag = Req_p->SigselectWithClientTag.ClientTag;
                        Resp_p->Status = REQUEST_OK;
                        SMS_SEND(&Resp_p, SMS_SENDER(&ReceivedSignal_p));
                    } else {
                        SMS_LOG_E("[ REQUEST_EVENTCHANNEL_SUBSCRIBE ] - Signal Alloc Failed");
                    }
                    break;
                }

            default:
                SMS_LOG_I("- default switch statement hit!");
                break;
            }                   // end switch
        }

        if (ReceivedSignal_p != NULL) {
            SMS_SIGNAL_FREE(&ReceivedSignal_p);
        }
    }                           // endless loop

    pthread_exit(NULL);         // There is no escape :-)
    return NULL;
}


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
    pthread_t Timer_ThreadID;

    SMS_LOG_I("\n*** MFL SMS (StubSide) STUB STARTED! ***\n");

    start_up_sequence_lock();

    //Start the Timer thread
    if (pthread_create(&Timer_ThreadID, NULL, timer_server_thread, (void *) tTimer)) {
        SMS_LOG_E("failed to start Timer, FATAL");
        exit(1);
    }

    SMS_LOG_I("waiting for timer thread to start");

    // blocks here until timer thread has started
    start_up_sequence_wait();

    start_up_sequence_lock();

    //Start the RX thread
    SMS_LOG_I("creating thread %ld", tMFL);
    rc = pthread_create(&Thread, NULL, MFL_SMS_Client, (void *) tMFL);

    if (rc) {
        SMS_LOG_I("ERROR; return code from pthread_create() is %d", rc);
        exit(1);
    }

    // blocks here until thread has started
    start_up_sequence_wait();
    start_up_sequence_destroy();

    SMS_A_(SMS_LOG_I("MFL_SMS_Thread running... "));
    // Block here until "Thread" returns.
    pthread_join(Thread, NULL);

    return 0;
}
