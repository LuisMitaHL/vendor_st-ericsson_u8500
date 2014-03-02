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
 * The main process loop of the SMS Server.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>

/* Operation System Include Files */
#include "r_smslinuxporting.h"

/* The SMS Server Interface */
#include "c_sms_config.h"
#include "d_sms.h"
#include "g_sms.h"
#include "g_cbs.h"
#include "r_sms.h"

/* Dependant Modules */
#ifdef SMS_USE_MFL_UICC
#include "r_sim.h"
#include "r_sat.h"
#include "r_simlock.h"
#endif                          // SMS_USE_MFL_UICC

#ifdef SMS_USE_MFL_MODEM
// MFL Modem Include files
#include "r_rms.h"
#include "mfl.h"
#include "r_dispatch.h"
#else                           // SMS_USE_MFL_MODEM
// MAL Modem Include files
#include <stdint.h>
#include "mal_sms.h"
#include "shm_netlnk.h"
#endif                          // SMS_USE_MFL_MODEM

/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#include "smbearer.h"
#include "smotadp.h"
#include "smsimtkt.h"
#include "smrouter.h"
#ifndef REMOVE_SMSTRG_LAYER
#include "smstrg.h"
#endif                          //REMOVE_SMSTRG_LAYER
#include "smmngr.h"
#include "smutil.h"
#include "smsmain.h"
#include "smslist.h"
#include "smrouter.h"
#include "cbmngr.h"
#include "cbmsgs.h"
#include "cbsubscribe.h"
#include "smrouter.h"

#include "c_sms_config.h"

/* Master Reset event */
//#include "r_masterreset.h"

#include "sim.h"
#include "cn_client.h"
#include "util_security.h"


#ifdef SMS_SIM_TEST_PLATFORM

#include <signal.h>
static int internal_pipe_write_fd = -1;
#endif                          // SMS_SIM_TEST_PLATFORM

typedef enum {
    SMS_SERVER_ID_SMS,
    SMS_SERVER_ID_CBS
} SMS_ServerId_t;

typedef struct {
    const uint8_t *Path_p;
    size_t PathLen;
    SMS_ServerId_t ServerId;
} SMS_Cat_RefreshFiles_t;

static const uint8_t EF_SMS_FileName[] = { 0x6F, 0x3C };        // EFsms 0x6F3C;
static const uint8_t EF_SMSR_FileName[] = { 0x6F, 0x47 };       // EFsmsr 0x6F47;
static const uint8_t EF_SMSS_FileName[] = { 0x6F, 0x43 };       // EFsmss 0x6F43;
static const uint8_t EF_CBMID_FileName[] = { 0x6F, 0x48 };      // EFcbmid 0x6F48;

static const SMS_Cat_RefreshFiles_t SMS_Cat_RefreshFiles[] = {
    {EF_SMS_FileName, SMS_ELEMENTS_OF_ARRAY(EF_SMS_FileName), SMS_SERVER_ID_SMS},
    {EF_SMSR_FileName, SMS_ELEMENTS_OF_ARRAY(EF_SMSR_FileName), SMS_SERVER_ID_SMS},
    {EF_SMSS_FileName, SMS_ELEMENTS_OF_ARRAY(EF_SMSS_FileName), SMS_SERVER_ID_SMS},
    {EF_CBMID_FileName, SMS_ELEMENTS_OF_ARRAY(EF_CBMID_FileName), SMS_SERVER_ID_CBS}
};

typedef enum {
    SMS_SIM_STATE_READY_WAITING,
    SMS_SIM_STATE_READY_NOT_WAITING,
    SMS_SIM_STATE_READY_RCVD
} sms_sim_state_ready_t;

typedef enum {
    SMS_CAT_EF_FILE_CHANGED_NONE = 0x00,
    SMS_CAT_EF_FILE_CHANGED_SMS = 0x01,
    SMS_CAT_EF_FILE_CHANGED_CBS = 0x02,
    SMS_CAT_EF_FILE_CHANGED_ALL = ~0x00 // Set all bits to 1 (independent of variable size)
} sms_cat_ef_file_changed_t;

typedef struct {
    ste_sim_t *sim_async;       /* sim channel used for events + async calls, including CAT */
    ste_sim_t *sim_sync;        /* sim channel used for sync calls ONLY */
} ste_sms_sim_data_t;

typedef struct {
    uint8_t cat_registered;     /* TRUE if registered with CAT server. */
    uint8_t timer_active;       /* TRUE if CAT refresh timer is active. */
    sms_cat_ef_file_changed_t ef_file_changed;  /* Bit array used to indicate whether the changed file(s) affect the SMS or CBS servers, or both. */
} ste_sms_cat_data_t;

typedef struct {
    int connected;              /* TRUE if connected to SIM server */
    ste_sms_sim_data_t sim_data;
    ste_sms_cat_data_t cat_data;
} ste_sms_sim_context_t;

#define SMS_CAT_REFRESH_TIMER_CLIENT_TAG 0

typedef struct MessagingSystemObject {
    MessagingSystemState_t MessagingSystemState;
    SMS_SwBP_SignalInfo_t SenderSignalInfo;
#ifndef SMS_USE_MFL_MODEM
    MSG_List *MAL_Callback_Data_p;
#endif
#ifdef SMS_USE_MFL_MODEM
    modem_session_t *MFL_Modem_Session_p;
    void *MFL_Modem_SignalData_p;

#else
    uint8_t ConfigureMAL;
    int SMS_Netlnk_fd;
    int ModemResetStatus;
#endif                          // SMS_USE_MFL_MODEM
    int SMS_Modem_fd;
    int CN_connect_attempts;
    int CN_Client_fd;
    int sms_log_fd;
    cn_context_t *cn_context_p;
    ste_cat_ec_response_t mo_sms_control_ec_response;
    ste_sms_sim_context_t *sim_context;
    uint8_t sim_evt_state;
} MessagingSystem_t;
typedef struct {
    MessagingSystemState_t MessagingSystemState;
    const SMS_SIGSELECT *ValidSignals;
} MessagingSystem_ValidSignalsForState_t;


/*
 * Signal states
 */
typedef enum {
    REJECTED = 0,
    ACCEPTED = 1,
} signal_state_t;

static const char *signalState[] = { "rejected", "accepted" };

/*
 * FUNCTION PROTOTYPES
 */

/*
 * Messaging System State Machine
 */
static void MessagingSystem_Initialise(
    void);
static void MessagingSystem_Deinitialise(
    void);
static void *MessagingSystem_Main(
    void *threadid);
static uint8_t SignalValidForState(
    SMS_SIGSELECT Primitive);

static SMS_Error_t MessagingSystem_EventsSubscription(
    const int EventSocket,
    const uint8_t Subscribe);
static SMS_Error_t MessagingSystem_ShortMessageEventsSubscription(
    const int EventSocket,
    const uint8_t Subscribe);
static void sim_connect(
    ste_sim_t * sim);
static void MessagingSystem_CatRefresh_SendResult(
    ste_sim_t * cat,
    uintptr_t client_tag,
    const ste_sim_pc_refresh_type_t refresh_type);
static void MessagingSystem_CatRefresh_HandleFile(
    const sim_path_t * const path_p);
static void MessagingSystem_CatRefresh_Set(
    const sms_cat_ef_file_changed_t ef_file_changed);
static void MessagingSystem_CatRefresh_TimerExpired(
    void);



/*
 * EXTERNAL DATA DEFINITIONS
 */
extern ShortMessageManager_p theShortMessageManager;

/*
 * LOCAL DATA DEFINITIONS
 */
static MessagingSystem_t MessagingSystem = { MESSAGING_SYSTEM_LAST_VALID_STATE, // Messaging system State
    {SMS_NO_PROCESS, SMS_CLIENT_TAG_NOT_USED},  // InitSignalInfo
#ifndef SMS_USE_MFL_MODEM
    NULL,                       // MAL_Callback_Data_p
#endif
#ifdef SMS_USE_MFL_MODEM
    NULL,                       // MFL_Modem_Session_p
    NULL,                       // MFL_Modem_SignalData_p
#else
    TRUE,                       // ConfigureMAL
    SMS_NO_PROCESS,             // SMS_Netlnk_fd
    FALSE,                      // ModemResetStatus
#endif                          // SMS_USE_MFL_MODEM
    SMS_NO_PROCESS,             // SMS_Modem_fd
    0,                          // CN_connect_attempts
    SMS_NO_PROCESS,             // CN_Client_fd
    SMS_NO_PROCESS,             // SMS_log_fd
    NULL,                       // cn_context_p
    {0, {NULL, 0}},             // mo_sms_control_ec_response
    NULL,                       // ste_sms_sim_context_t
    SMS_SIM_STATE_READY_WAITING // Waiting on SIM_STATE_READY
};


/*********************************************************************
 *
 * These signal arrays define what signals are available in which states
 * If the signal is not in the list, then the MTS will return BUSY
 * for that request
 *
 ********************************************************************/
#ifdef SMS_USE_MFL_MODEM
static const SMS_SIGSELECT wait_SIM_ReadyValidSignals[] = { (SMS_SIGSELECT) 15,
    REQUEST_SMS_CB_SESSIONCREATE,
    REQUEST_SMS_CB_SESSIONDESTROY,
    MSG_MAL_CONFIGURE_RETRY_TIMER_IND,
    R_REQ_SMS_PREFERREDSTORAGESET,
    R_REQ_SMS_DELIVERREPORTCONTROLSET,
    R_REQ_SMS_DELIVERREPORTCONTROLGET,
    R_REQ_SMS_MO_ROUTESET,
    R_REQ_SMS_MO_ROUTEGET,
    R_REQ_SMS_RELAYLINKCONTROLGET,
    R_REQ_SMS_RELAYLINKCONTROLSET,
    REQUEST_SMS_EVENTSSUBSCRIBE,
    REQUEST_SMS_EVENTSUNSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE,
    REQUEST_SMS_MEMCAPACITYFULLREPORT
};

static const SMS_SIGSELECT limitedOperationsValidSignals[] = { (SMS_SIGSELECT) 37,
    REQUEST_SMS_CB_SESSIONCREATE,
    REQUEST_SMS_CB_SESSIONDESTROY,
    MSG_STORAGE_TIMER_IND,
    MSG_SMSTORAGE_INIT_TIMER_IND,
    MSG_SMS_DELIVER_REPORT_TIMER_IND,
    MSG_OTADP_TIMER_IND,
    MSG_RELAY_LINK_CONTROL_TIMER_IND,
    MSG_SMS_MORETOSEND_TIMER_IND,
    MSG_MAL_CONFIGURE_RETRY_TIMER_IND,
    EVENT_RMS_V2_MESSAGERECEIVEDIND,
    EVENT_RMS_V2_FAILURERECEIVEDIND,
    RESPONSE_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE,
    RESPONSE_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT,
    RESPONSE_RMS_V2_MESSAGE_ABORT,
    RESPONSE_RMS_V2_MESSAGE_SEND,
    REQUEST_SMS_SHORTMESSAGEDELIVER,
    REQUEST_SMS_DELIVERREPORTSEND,
    R_REQ_SMS_DELIVERREPORTCONTROLSET,
    R_REQ_SMS_DELIVERREPORTCONTROLGET,
    R_REQ_SMS_MO_ROUTESET,
    R_REQ_SMS_MO_ROUTEGET,
    REQUEST_SMS_MEMCAPACITYAVAILABLESEND,
    REQUEST_SMS_SHORTMESSAGEREAD,
    REQUEST_SMS_SHORTMESSAGEWRITE,
    REQUEST_SMS_SHORTMESSAGEDELETE,
    REQUEST_SMS_STATUSREPORTREAD,
    REQUEST_SMS_SHORTMESSAGESTATUSSET,
    REQUEST_SMS_SHORTMESSAGESEND,
    REQUEST_SMS_SHORTMESSAGEINFOREAD,
    REQUEST_SMS_EVENTSSUBSCRIBE,
    REQUEST_SMS_EVENTSUNSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE,
    R_REQ_SMS_PREFERREDSTORAGESET,
    R_REQ_SMS_RELAYLINKCONTROLGET,
    R_REQ_SMS_RELAYLINKCONTROLSET,
    REQUEST_SMS_MEMCAPACITYFULLREPORT
};


#else
static const SMS_SIGSELECT wait_SIM_ReadyValidSignals[] = { (SMS_SIGSELECT) 15,
    REQUEST_SMS_CB_SESSIONCREATE,
    REQUEST_SMS_CB_SESSIONDESTROY,
    MSG_MAL_CONFIGURE_RETRY_TIMER_IND,
    R_REQ_SMS_PREFERREDSTORAGESET,
    R_REQ_SMS_DELIVERREPORTCONTROLSET,
    R_REQ_SMS_DELIVERREPORTCONTROLGET,
    R_REQ_SMS_MO_ROUTESET,
    R_REQ_SMS_MO_ROUTEGET,
    R_REQ_SMS_RELAYLINKCONTROLGET,
    R_REQ_SMS_RELAYLINKCONTROLSET,
    REQUEST_SMS_EVENTSSUBSCRIBE,
    REQUEST_SMS_EVENTSUNSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE,
    REQUEST_SMS_MEMCAPACITYFULLREPORT
};

static const SMS_SIGSELECT limitedOperationsValidSignals[] = { (SMS_SIGSELECT) 31,
    REQUEST_SMS_CB_SESSIONCREATE,
    REQUEST_SMS_CB_SESSIONDESTROY,
    MSG_STORAGE_TIMER_IND,
    MSG_SMSTORAGE_INIT_TIMER_IND,
    MSG_SMS_DELIVER_REPORT_TIMER_IND,
    MSG_OTADP_TIMER_IND,
    MSG_RELAY_LINK_CONTROL_TIMER_IND,
    MSG_SMS_MORETOSEND_TIMER_IND,
    MSG_MAL_CONFIGURE_RETRY_TIMER_IND,
    REQUEST_SMS_SHORTMESSAGEDELIVER,
    REQUEST_SMS_DELIVERREPORTSEND,
    REQUEST_SMS_MEMCAPACITYAVAILABLESEND,
    REQUEST_SMS_MEMCAPACITYGETSTATE,
    REQUEST_SMS_SHORTMESSAGEREAD,
    REQUEST_SMS_SHORTMESSAGEWRITE,
    REQUEST_SMS_SHORTMESSAGEDELETE,
    REQUEST_SMS_STATUSREPORTREAD,
    REQUEST_SMS_SHORTMESSAGESTATUSSET,
    REQUEST_SMS_SHORTMESSAGESEND,
    REQUEST_SMS_SHORTMESSAGEINFOREAD,
    REQUEST_SMS_EVENTSSUBSCRIBE,
    REQUEST_SMS_EVENTSUNSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE,
    R_REQ_SMS_PREFERREDSTORAGESET,
    R_REQ_SMS_DELIVERREPORTCONTROLSET,
    R_REQ_SMS_DELIVERREPORTCONTROLGET,
    R_REQ_SMS_MO_ROUTESET,
    R_REQ_SMS_MO_ROUTEGET,
    R_REQ_SMS_RELAYLINKCONTROLGET,
    R_REQ_SMS_RELAYLINKCONTROLSET,
    REQUEST_SMS_MEMCAPACITYFULLREPORT
};


#endif                          // SMS_USE_MFL_MODEM
static const SMS_SIGSELECT idleSignals[] = { (SMS_SIGSELECT) 0 };       // RECEIVE_ALL


/*********************************************************************
 *
 * These is the heart of the Messaging system, and defines what signals are
 * are valid for that state. This allows the MTS to reject certains signals.
 *
 ********************************************************************/
static const MessagingSystem_ValidSignalsForState_t ValidSignalsForStatesTable[] = {
    // Internal State                                    Valid Signals
    {MESSAGING_SYSTEM_WAIT_SIM_READY, wait_SIM_ReadyValidSignals},
    {MESSAGING_SYSTEM_VALIDATE_SM_STORAGE, limitedOperationsValidSignals},
    {MESSAGING_SYSTEM_IDLE, idleSignals},
    {MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SAT_REFRESH, wait_SIM_ReadyValidSignals},
    {MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART, wait_SIM_ReadyValidSignals},
    {MESSAGING_SYSTEM_LAST_VALID_STATE, NULL}
};


#define MAX_SMS_SERVERSUBSCRIBERS    (10)
#define SMS_SIGNAL_PRIMITIVE_INVALID (SMS_MAX_UINT32)
#define SMS_TIMER_THREAD             (110)
int SMS_SubscriberSocket[MAX_SMS_SERVERSUBSCRIBERS] = { SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS,
    SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS,
    SMS_NO_PROCESS, SMS_NO_PROCESS
};

int SMS_ShortMessageSubscriberSocket[MAX_SMS_SERVERSUBSCRIBERS] = { SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS,
    SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS, SMS_NO_PROCESS,
    SMS_NO_PROCESS,
    SMS_NO_PROCESS
};

#ifdef SMS_SIM_TEST_PLATFORM
/********************************************************************/
/**
*
* @function     sms_main_signal_handler
*
* @description
*
* @param        void
*
* @return       int
*/
/********************************************************************/
void sms_main_signal_handler(int signo)
{
    if (SIGTERM == signo) {
        SMS_SigselectWithClientTag_t *Sig_p;
        Sig_p = SMS_SIGNAL_ALLOC(SMS_SigselectWithClientTag_t, REQUEST_SMS_CB_SHUTDOWN);
        SMS_SEND(&Sig_p, internal_pipe_write_fd);
    }
}
#endif                          // SMS_SIM_TEST_PLATFORM

/********************************************************************/
/**
 *
 * @function     main
 *
 * @description  Main process loop for the Message Transport Server
 *
 * @param        void
 *
 * @return       int
 */
/********************************************************************/
int main(
    void)
{
    long ThreadId = SMS_SERVERTHREAD_;
    long t0 = SMS_TIMER_THREAD;
    pthread_t Timer_ThreadID;
    int ErrorCode;
    pthread_t SMS_ServerThread;

    MessagingSystem.sms_log_fd = sms_log_init();

    // Set umask before anything else
    util_set_restricted_umask();

#ifdef SMS_SIM_TEST_PLATFORM
   signal(SIGTERM, sms_main_signal_handler);
#endif                          //SMS_SIM_TEST_PLATFORM


    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        SMS_A_(SMS_LOG_E("SMS Server: SIG_ERR when ignoring SIGPIPE\n"));
        abort();
    }

    // Lets start the ball rolling...
    // Uncommented code when communication with sim was achieved.
    MessagingSystem_SetState(MESSAGING_SYSTEM_WAIT_SIM_READY);

    // Start the Timer thread
    start_up_sequence_lock();

    if ((ErrorCode = pthread_create(&Timer_ThreadID, NULL, timer_server_thread, (void *) t0)) != 0) {
        SMS_A_(SMS_LOG_E("smsmain.c: main: return code from pthread_create() timer_server_thread is %d", ErrorCode));
    }
    // FIXME: why do we spawn a thread here and just wait for it to die?
    // ...... Just call the MessagingSystem_Main() is enough...

    // Wait here until timer thread has started
    start_up_sequence_wait();
    SMS_A_(SMS_LOG_I("smsmain.c: main: Timer thread started"));

    // Start the SMS Server thread
    start_up_sequence_lock();

    if ((ErrorCode = pthread_create(&SMS_ServerThread, NULL, MessagingSystem_Main, (void *) ThreadId)) != 0) {
        SMS_A_(SMS_LOG_E("smsmain.c: main: return code from pthread_create() is %d", ErrorCode));
    }

    // Wait here until SMS Server thread has started
    start_up_sequence_wait();
    SMS_A_(SMS_LOG_I("smsmain.c: main: SMS server thread started"));

    // Release startup synchronization objects
    start_up_sequence_destroy();

    // Throw away privilege and become a normal user (applies to this thread).
    util_continue_as_non_privileged_user();

    // Blocks here until threads terminates...which it should never do!
    pthread_join(SMS_ServerThread, NULL);
    timer_server_kill_thread();
    pthread_join(Timer_ThreadID, NULL);

    SMS_A_(SMS_LOG_I(" smsmain.c:  SMS_ServerThread Terminated "));

    sms_log_close();

    return 0;
}


/********************************************************************/
/** Callbacks for Handling SIM async calls, events and CAT events.
 *
 * @function     sim_handle_callbacks
 *
 * @description  Main switch statement for callbacks from the sim module.
 *
 * @param        cause : call back reason.

 * @param        client_tag :
 * @param        *data :
 * @param        *user_data :
 * @return       static void
 */
/********************************************************************/
static void sim_handle_callbacks(
    int cause,
    uintptr_t client_tag,
    void *data,
    void *user_data)
{
    ste_sim_t **sim_pp = (ste_sim_t **)user_data;
    ste_sim_t *sim_p = NULL;
    int ret;

    SMS_B_(SMS_LOG_D("smsmain.c: %s. (%d)", __func__, cause));

    if (sim_pp != NULL && *sim_pp != NULL) {
        sim_p = *sim_pp;
    } else {
        SMS_A_(SMS_LOG_E("smsmain.c: ERROR - user_data NULL"));
    }

    switch (cause) {
    case STE_SIM_CAUSE_CONNECT:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_SIM_CAUSE_CONNECT"));
        if (MessagingSystem_UICC_SimAsyncGet() == sim_p) {
            // Register with the CAT server to get CAT Refresh events
            ret = ste_cat_register(sim_p, (uintptr_t)sim_p, STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH);
            // MessagingSystem.sim_context->cat_data.cat_registered is set to TRUE when callback
            if (ret == STE_SIM_SUCCESS) {
                SMS_A_(SMS_LOG_I("main.c: %s: ste_cat_register called", __func__));
            } else {
                SMS_A_(SMS_LOG_E("main.c: %s: ERROR ste_cat_register returned %d", __func__, ret));
            }
        }
        break;

    case STE_SIM_CAUSE_DISCONNECT:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_SIM_CAUSE_DISCONNECT"));
        if (sim_p != NULL) {
            if (MessagingSystem.MessagingSystemState != MESSAGING_SYSTEM_SHUTDOWN_STATE) {
                MessagingSystem_SetState(MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART);
            }
        }
        break;

    case STE_UICC_CAUSE_REQ_GET_SIM_STATE:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_UICC_CAUSE_REQ_GET_SIM_STATE"));
        if (data != NULL) {
            ste_uicc_get_sim_state_response_t *sim_state_response_p = (ste_uicc_get_sim_state_response_t *)data;

            if (SIM_STATE_READY == sim_state_response_p->state) {
                SMS_A_(SMS_LOG_I("smsmain.c: SIM_STATE_READY"));
                if (SMS_SIM_STATE_READY_WAITING == MessagingSystem.sim_evt_state) {
                    MessagingSystem.sim_evt_state = SMS_SIM_STATE_READY_RCVD;
                }
            } else {
                SMS_A_(SMS_LOG_I("smsmain.c: sim_state_response:%d", sim_state_response_p->state));
            }
        }
        break;

    case STE_UICC_CAUSE_SIM_STATE_CHANGED:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_UICC_CAUSE_SIM_STATE_CHANGED"));
        if (data != NULL) {
            ste_uicc_sim_state_changed_t *sim_state_changed_p = (ste_uicc_sim_state_changed_t *) data;

            SMS_A_(SMS_LOG_I("smsmain.c: sim_state_changed:%d", sim_state_changed_p->state));
            if (SIM_STATE_UNKNOWN == sim_state_changed_p->state) {
                if (MessagingSystem.MessagingSystemState != MESSAGING_SYSTEM_SHUTDOWN_STATE) {
                    MessagingSystem_SetState(MESSAGING_SYSTEM_WAIT_SIM_READY);
                }
            } else if (SIM_STATE_READY == sim_state_changed_p->state) {
                if (SMS_SIM_STATE_READY_WAITING == MessagingSystem.sim_evt_state) {
                    MessagingSystem.sim_evt_state = SMS_SIM_STATE_READY_RCVD;
                }
            }
        } else {
            SMS_A_(SMS_LOG_E("smsmain.c: sim_state_changed_p null"));
        }
        break;

    case STE_UICC_CAUSE_SIM_STATUS:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_UICC_CAUSE_SIM_STATUS"));
        break;

    case STE_SIM_CAUSE_SHUTDOWN:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_SIM_CAUSE_SHUTDOWN"));
        MessagingSystem_SetState(MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART);
        break;

    case STE_SIM_CAUSE_PING:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_SIM_CAUSE_PING"));
        break;

    case STE_CAT_CAUSE_EC:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_EC. ClientTag = 0x%08X", (unsigned int) client_tag));
        if (client_tag == SMS_PP_DOWNLOAD_TAG) {
            // CAT Envelope download for SMS.
            SMS_B_(SMS_LOG_D("smsmain.c: STE_CAT_CAUSE_EC. SMS DataDownload"));
            if (data != NULL) {
                ste_cat_ec_response_t *ec_response_p = (ste_cat_ec_response_t *) data;
                SIMTKT_SetEnvelopeData(ec_response_p->apdu.len, ec_response_p->apdu.buf);
            }
        } else if (client_tag == CB_CAT_CB_DOWNLOAD_TAG) {
            // CAT Envelope download for CBS.
            SMS_B_(SMS_LOG_D("smsmain.c: STE_CAT_CAUSE_EC. CB DataDownload"));
            if (data != NULL) {
                ste_cat_ec_response_t *ec_response_p = (ste_cat_ec_response_t *) data;
                SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_EC. CB DataDownload ec_status = 0x%04X", ec_response_p->ec_status));
                SMS_IDENTIFIER_NOT_USED(ec_response_p); // Silence compiler warning if A prints not enabled.
            }
        } else if (client_tag == SMS_CAT_MO_SMS_CONTROL_TAG) {
            // CAT Envelope response for MO SMS Control by USIM.
            SMS_B_(SMS_LOG_D("smsmain.c: STE_CAT_CAUSE_EC. MO SMS Control"));
            if (data != NULL) {
                ste_cat_ec_response_t *ec_response_p = (ste_cat_ec_response_t *) data;
                SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_EC. MO SMS Control ec_status = 0x%04X", ec_response_p->ec_status));

                MessagingSystem.mo_sms_control_ec_response.ec_status = ec_response_p->ec_status;
                MessagingSystem.mo_sms_control_ec_response.apdu.len = ec_response_p->apdu.len;
                MessagingSystem.mo_sms_control_ec_response.apdu.buf = (char *) SMS_HEAP_UNTYPED_ALLOC(MessagingSystem.mo_sms_control_ec_response.apdu.len);

                if (MessagingSystem.mo_sms_control_ec_response.apdu.buf != NULL) {
                    memcpy(MessagingSystem.mo_sms_control_ec_response.apdu.buf, ec_response_p->apdu.buf, MessagingSystem.mo_sms_control_ec_response.apdu.len);
                }
            }
        }
        break;

    case STE_CAT_CAUSE_REGISTER:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_REGISTER"));
        if (MessagingSystem.sim_context != NULL) {
            MessagingSystem.sim_context->cat_data.cat_registered = TRUE;
        }
        break;

    case STE_CAT_CAUSE_DEREGISTER:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_DEREGISTER"));
        // N.B. MessagingSystem.sim_context->cat_data.cat_registered set to FALSE when ste_cat_deregister() called.
        break;

    case STE_CAT_CAUSE_PC_REFRESH_IND:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_PC_REFRESH_IND"));
        // We don't always make any changes to data structures with this callback. If any specific EF files have been
        // updated as a result of the CAT Refresh we will also get the STE_CAT_CAUSE_PC_REFRESH_FILE_IND
        // callback which can be used to modify data structures.  However if all EF files may have changed, then we
        // must handle that here as we will not get STE_CAT_CAUSE_PC_REFRESH_FILE_IND callbacks.
        if (data != NULL) {
            ste_cat_pc_refresh_ind_t *refresh_ind_p = (ste_cat_pc_refresh_ind_t *) data;

            SMS_B_(SMS_LOG_D("smsmain.c: Refresh Type=%d", refresh_ind_p->type));

            // If the refresh type is one of the following types, we won't get STE_CAT_CAUSE_PC_REFRESH_FILE_IND callbacks
            // and must assume that any / all of the files on the SIM have changed:
            //   STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE
            //   STE_SIM_PC_REFRESH_INIT
            //   STE_SIM_PC_REFRESH_3G_APP_RESET
            if (refresh_ind_p->type == STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE || refresh_ind_p->type == STE_SIM_PC_REFRESH_INIT || refresh_ind_p->type == STE_SIM_PC_REFRESH_3G_APP_RESET) {
                MessagingSystem_CatRefresh_Set(SMS_CAT_EF_FILE_CHANGED_ALL);
            }
            // Now try and send a response to the CAT Server.
            if (sim_p != NULL) {
                MessagingSystem_CatRefresh_SendResult(sim_p, refresh_ind_p->simd_tag, refresh_ind_p->type);
            } else {
                // B print here because there is already a general A print at top of function if sim cannot be assigned.
                SMS_B_(SMS_LOG_D("smsmain.c: STE_CAT_CAUSE_PC_REFRESH_IND sim is NULL"));
            }
        } else {
            SMS_A_(SMS_LOG_E("smsmain.c: refresh_ind_p is NULL"));
        }
        break;

    case STE_CAT_CAUSE_PC_REFRESH_FILE_IND:
        SMS_A_(SMS_LOG_I("smsmain.c: STE_CAT_CAUSE_PC_REFRESH_FILE_IND"));
        if (data != NULL) {
            ste_cat_pc_refresh_file_ind_t *refresh_file_ind_p = (ste_cat_pc_refresh_file_ind_t *) data;

            // Handle changed file.
            MessagingSystem_CatRefresh_HandleFile(&refresh_file_ind_p->path);

            // N.B. Response to CAT Server has already been sent in the STE_CAT_CAUSE_PC_REFRESH_IND callback.
        } else {
            SMS_A_(SMS_LOG_E("smsmain.c: refresh_file_ind_p is NULL"));
        }
        break;

    default:
        SMS_A_(SMS_LOG_W("smsmain.c: Received unsupported event cause:%d", cause));
        break;
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_CatRefresh_SendResult
 *
 * @description  Send response to CAT Server after CAT Refresh has
 *               occurred.
 *
 * @param        cat           CAT Server SIM data object
 * @param        client_tag    Client tag to be used.
 * @param        refresh_type  Refresh type being acknowledged.
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_CatRefresh_SendResult(
    ste_sim_t * cat,
    uintptr_t client_tag,
    const ste_sim_pc_refresh_type_t refresh_type)
{
#ifdef CAT_REFRESH_RESULT_FUNCTIONALITY_AVAILABLE
    int ret;

    ret = ste_cat_refresh_result(cat, client_tag, refresh_type, STE_CAT_REFRESH_RESULT_OK);

    if (ret != 0) {
        SMS_A_(SMS_LOG_I("smsmain.c: ste_cat_refresh_result(%d) returned %d.", refresh_type, ret));
    }
#else
    // Implementation of CAT Server being developed against does not currently include an implementation of
    // ste_cat_refresh_result(). However, we have been advised that the current CAT Server design does not
    // require clients to call ste_cat_refresh_result(). Therefore this stub can be left until it is needed.
    SMS_IDENTIFIER_NOT_USED(cat);
    SMS_IDENTIFIER_NOT_USED(client_tag);
    SMS_IDENTIFIER_NOT_USED(refresh_type);
    SMS_B_(SMS_LOG_D("smsmain.c: ********* MessagingSystem_CatRefreshResult() is STUBBED *********"));
    SMS_B_(SMS_LOG_D("smsmain.c: ********* ste_cat_refresh_result() has not been called  *********"));
#endif
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_CatRefresh_Set
 *
 * @description  Update data structure and reset timer as a CAT Refresh
 *               has indicated that SMS or CBS EF files have been updated.
 *
 * @param        ef_file_changed  Bit Array indicating whether SMS and/or
 *                                CBS EF files have been refreshed.
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_CatRefresh_Set(
    const sms_cat_ef_file_changed_t ef_file_changed)
{
    if (MessagingSystem.sim_context != NULL) {
        // If timer has been started, re-start it in case we get any more STE_CAT_CAUSE_PC_REFRESH_FILE_IND
        // callbacks from CAT server. Do this immediately to reduce chance of it expiring.
        if (MessagingSystem.sim_context->cat_data.timer_active) {
            // Reset existing timer.
            timer_server_reset_ms(CAT_REFRESH_FILE_CHANGE_TIMER_IND, SMS_CAT_REFRESH_TIMER_CLIENT_TAG);
            SMS_B_(SMS_LOG_D("smsmain.c: CatRefresh_Set: Timer reset."));
        }
        // Update bit array indicating which files have been changed.
        SMS_A_(SMS_LOG_I("smsmain.c: CatRefresh_Set: Initial ef_file_changed = 0x%02X. Supplied ef_file_changed = 0x%02X.",
                         MessagingSystem.sim_context->cat_data.ef_file_changed, ef_file_changed));
        MessagingSystem.sim_context->cat_data.ef_file_changed |= ef_file_changed;

        // Set timer.
        MessagingSystem.sim_context->cat_data.timer_active = TRUE;
        timer_server_set_ms(CAT_REFRESH_FILE_CHANGE_TIMER_IND, SMS_CAT_REFRESH_TIMER_CLIENT_TAG, SMS_CAT_REFRESH_CHANGED_FILE_IND_TIMEOUT_VALUE);
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_CatRefresh_HandleFile
 *
 * @description  Compare path of EF file, which has been updated by a
 *               CAT Refresh, with those monitored by SMS and CBS. If
 *               a match occurs, set flag so that they will be updated.
 *
 * @param        path_p  Path of updated EF file.
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_CatRefresh_HandleFile(
    const sim_path_t * const path_p)
{
    uint8_t ArrayIndex;

    SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_CatRefresh_HandleFile: PathLen=%d", path_p->pathlen));
    for (ArrayIndex = 0; ArrayIndex < path_p->pathlen; ArrayIndex++) {
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_CatRefresh_HandleFile: path_p->path[%d] = 0x%02X ", ArrayIndex, path_p->path[ArrayIndex]));
    }

    for (ArrayIndex = 0; ArrayIndex < SMS_ELEMENTS_OF_ARRAY(SMS_Cat_RefreshFiles); ArrayIndex++) {
        // Check supplied path is long enough to be what we are looking for.
        if (path_p->pathlen >= SMS_Cat_RefreshFiles[ArrayIndex].PathLen) {
            // Check path is one we are looking for. We are only checking the last octets of the path so there is a small risk
            // that we will trigger more refresh updates of CBS or SMS Server than is absolutely necessary. However this
            // simplifies the handling as we don't have to worry about full SIM paths.
            // However this does mean that we have to check from the end of the paths as path_p->path is longer than the one
            // in our static array.
            uint8_t PathIdentical = TRUE;
            uint8_t OctetsCompared = 0;
            uint8_t CatRefreshPathIndex;
            uint8_t TablePathIndex;

            // Setup indices so that they point to the last octets of the CAT Refresh callback, and SMS_Cat_RefreshFiles, paths.
            CatRefreshPathIndex = path_p->pathlen - 1;
            TablePathIndex = SMS_Cat_RefreshFiles[ArrayIndex].PathLen - 1;

            while (OctetsCompared < SMS_Cat_RefreshFiles[ArrayIndex].PathLen) {
                // Iterate backwards through the paths looking for differences.
                SMS_B_(SMS_LOG_D
                       ("MessagingSystem_CatRefresh_HandleFile SMS_Cat_RefreshFiles[%d].Path_p[%d] = 0x%02X.", ArrayIndex, TablePathIndex,
                        SMS_Cat_RefreshFiles[ArrayIndex].Path_p[TablePathIndex]));
                SMS_B_(SMS_LOG_D("MessagingSystem_CatRefresh_HandleFile path_p->path[%d] = 0x%02X.", CatRefreshPathIndex, path_p->path[CatRefreshPathIndex]));

                if (SMS_Cat_RefreshFiles[ArrayIndex].Path_p[TablePathIndex] != path_p->path[CatRefreshPathIndex]) {
                    PathIdentical = FALSE;
                    break;
                }
                CatRefreshPathIndex--;
                TablePathIndex--;
                OctetsCompared++;
            }

            SMS_B_(SMS_LOG_D("MessagingSystem_CatRefresh_HandleFile: ArrayIndex=%d, PathIdentical=%d", ArrayIndex, PathIdentical));

            if (PathIdentical) {
                // Match found
                sms_cat_ef_file_changed_t ef_file_changed;

                SMS_A_(SMS_LOG_I
                       ("MessagingSystem_CatRefresh_HandleFile(): ArrayIndex=%d Refresh %s SIM EF Data", ArrayIndex,
                        SMS_Cat_RefreshFiles[ArrayIndex].ServerId == SMS_SERVER_ID_SMS ? "SMS" : "CBS"));

                if (SMS_Cat_RefreshFiles[ArrayIndex].ServerId == SMS_SERVER_ID_SMS) {
                    ef_file_changed = SMS_CAT_EF_FILE_CHANGED_SMS;
                } else {
                    ef_file_changed = SMS_CAT_EF_FILE_CHANGED_CBS;
                }

                MessagingSystem_CatRefresh_Set(ef_file_changed);
                break;
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_CatRefresh_TimerExpired
 *
 * @description  Function called when CAT Refresh timer has expired.
 *               This is used to trigger a re-reading of the EF files
 *               which the SMS and CBS servers use.
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_CatRefresh_TimerExpired(
    void)
{
    if (MessagingSystem.sim_context != NULL) {
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_CatRefresh_TimerExpired: ef_file_changed=0x%02X",
                         MessagingSystem.sim_context->cat_data.ef_file_changed));
        SMS_B_ASSERT_(MessagingSystem.sim_context->cat_data.ef_file_changed != SMS_CAT_EF_FILE_CHANGED_NONE);

        // Change SMS Server state.
        MessagingSystem_SetState(MESSAGING_SYSTEM_VALIDATE_SM_STORAGE);

        if (MessagingSystem.sim_context->cat_data.ef_file_changed & SMS_CAT_EF_FILE_CHANGED_SMS) {
            // SMS Server EF files have been refreshed.
            SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_CatRefresh_TimerExpired: Begin SIM SM re-initialisation."));

#ifndef REMOVE_SMSTRG_LAYER
            ShortMessageManager_SIMRefresh();

            // do not have to wait for SIM ready before reinitialising SIM slots
            SmStorage_StartSimRefresh();
#endif                          // REMOVE_SMSTRG_LAYER
        }

        if (MessagingSystem.sim_context->cat_data.ef_file_changed & SMS_CAT_EF_FILE_CHANGED_CBS) {
            // CBS Server EF files have been refreshed.
            SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_CatRefresh_TimerExpired: Begin CBS re-initialisation."));
            CBS_Manager_CAT_Refresh();
        }
        // We have changed the SMS Server state to MESSAGING_SYSTEM_VALIDATE_SM_STORAGE before starting to process the
        // CAT Refresh data. Now we must ensure that is it set back to MESSAGING_SYSTEM_IDLE again.
        // This can happen automatically if storage is enabled AND there were changes to the SMS EF files; it is set to
        // idle when all the EF files have been iterated through and re-read.
        // However if only a CBS CAT refresh has taken place, or storage is disabled, then we need to ensure that the
        // SMS Server state is set back to MESSAGING_SYSTEM_IDLE here.
#ifndef REMOVE_SMSTRG_LAYER
        // Storage enabled
        if (!(MessagingSystem.sim_context->cat_data.ef_file_changed & SMS_CAT_EF_FILE_CHANGED_SMS)) {
            MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
        }
#else
        // Storage disabled
        MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
#endif

        // Reset flags as we have now processed the CAT Refresh
        MessagingSystem.sim_context->cat_data.ef_file_changed = SMS_CAT_EF_FILE_CHANGED_NONE;
        MessagingSystem.sim_context->cat_data.timer_active = FALSE;
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_UICC_SimSyncGet
 *
 * @description  Gets the SIM API data object for the Synchronous SIM
 *               API.
 *
 * @param        void
 *
 * @return       ste_sim_t*
 */
/********************************************************************/
ste_sim_t *MessagingSystem_UICC_SimSyncGet(
    void)
{
    ste_sim_t *sim_obj;

    if (MessagingSystem.sim_context != NULL) {
        sim_obj = MessagingSystem.sim_context->sim_data.sim_sync;
    } else {
        sim_obj = NULL;
    }

    return sim_obj;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_UICC_SimAsyncGet
 *
 * @description  Gets the SIM API data object for the SIM Asynschronous
 *               API.
 *
 * @param        void
 *
 * @return       ste_sim_t*
 */
/********************************************************************/
ste_sim_t *MessagingSystem_UICC_SimAsyncGet(
    void)
{
    ste_sim_t *sim_obj;

    if (MessagingSystem.sim_context != NULL) {
        sim_obj = MessagingSystem.sim_context->sim_data.sim_async;
    } else {
        sim_obj = NULL;
    }

    return sim_obj;
}


/**
 * Do the necessary setups towards the modem
 */


/********************************************************************/
/**
 *
 * @function     MessagingSystem_cat_deregister
 *
 * @description  Deregisters from CAT server.
 *
 * @param        *cat_registered_p : Flag to be updated.
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_cat_deregister(
    uint8_t * const cat_registered_p)
{
    if (*cat_registered_p) {
        ste_sim_t *cat_obj_p = MessagingSystem_UICC_SimAsyncGet();

        if (cat_obj_p != NULL) {
            int ret;
            uintptr_t client_tag = (uintptr_t) cat_obj_p;

            ret = ste_cat_deregister(cat_obj_p, client_tag);

            if (ret == 0) {
                *cat_registered_p = FALSE;
            } else {
                SMS_A_(SMS_LOG_E("main.c: MessagingSystem_cat_deregister: ste_cat_register returned %d", ret));
            }
        } else {
            SMS_A_(SMS_LOG_E("main.c: MessagingSystem_cat_deregister: cat_obj_p NULL"));
        }
    } else {
        SMS_A_(SMS_LOG_I("main.c: MessagingSystem_cat_deregister: CAT not registered"));
    }
}


/********************************************************************/
/**
 *
 * @function     sim_connect
 *
 * @description  Do the necessary connect towards the modem via the sim
 *               module.
 *
 * @param        *sim :
 * @return       void
 */
/********************************************************************/
static void sim_connect(
    ste_sim_t * sim)
{
    int ret;
    uintptr_t client_tag;

    // Use the sim ptr as client tag for connects!
    client_tag = (uintptr_t) sim;
    ret = -1;
    for (;;) {
        ret = ste_sim_connect(sim, client_tag);
        if (ret == 0)
            break;

        SMS_A_(SMS_LOG_E("main.c: ste_sim_connect ret = %d", ret));
        sleep(1);
    }
}


/********************************************************************/
/**
 *
 * @function     sim_disconnect
 *
 * @description  Do the necessary connect towards the modem via the sim
 *               module.
 *
 * @param        *sim : 
 * @return       void
 */
/********************************************************************/
static void sim_disconnect(
    ste_sim_t * sim)
{
    int ret;
    uintptr_t client_tag;

    // Use the sim ptr as client tag for connects!
    client_tag = (uintptr_t) sim;
    ret = ste_sim_disconnect(sim, client_tag);
    SMS_A_(SMS_LOG_I("main.c: ste_sim_disconnect ret = %d", ret));
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_create_sim_context
 *
 * @description  Creates a sim context.
 *
 * @return       ste_sms_sim_context_t*
 */
/********************************************************************/
static ste_sms_sim_context_t *MessagingSystem_create_sim_context(
    void)
{
    ste_sms_sim_context_t *sc;
    ste_sim_closure_t sim_closure = { NULL, NULL };
    sc = SMS_HEAP_UNTYPED_ALLOC(sizeof(*sc));

    if (sc != NULL) {
        // Initialise the entire buffer to NULL / FALSE.
        memset(sc, 0, sizeof(*sc));

        // Create the event context
        sim_closure.func = sim_handle_callbacks;
        sim_closure.user_data = &sc->sim_data.sim_async;    // user_data set to pointer to ste_sim_t object pointer.
        sc->sim_data.sim_async = ste_sim_new_st(&sim_closure);
        SMS_A_(SMS_LOG_I("main.c: %s: sim_async = %p", __func__, sc->sim_data.sim_async));

        // Create the sync context
        sim_closure.func = sim_handle_callbacks;
        sim_closure.user_data = &sc->sim_data.sim_sync;     // user_data set to pointer to ste_sim_t object pointer.
        sc->sim_data.sim_sync = ste_sim_new_st(&sim_closure);
        SMS_A_(SMS_LOG_I("main.c: %s: sim_sync = %p", __func__, sc->sim_data.sim_sync));
    } else {
        SMS_A_(SMS_LOG_E("main.c: MessagingSystem_create_sim_context: malloc failed"));
    }

    return sc;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_destroy_sim_context
 *
 * @description  Destroys a sim context.
 *
 * @param        ste_sms_sim_context_t*
 */
/********************************************************************/
static void MessagingSystem_destroy_sim_context(
    ste_sms_sim_context_t * sc)
{
    if (sc != NULL) {
        MessagingSystem_cat_deregister(&sc->cat_data.cat_registered);
        ste_sim_delete(sc->sim_data.sim_async, 0);
        ste_sim_delete(sc->sim_data.sim_sync, 0);
        SMS_HEAP_FREE(&sc);
    }
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_connect_sim_context
 *
 * @description  Connects to the sim.
 *
 * @param        sc : context to use.
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_connect_sim_context(
    ste_sms_sim_context_t * sc)
{
    SMS_B_(SMS_LOG_D("main.c: %s called", __func__));

    if (sc != NULL) {
        if (sc->connected != TRUE) {
            sc->cat_data.cat_registered = FALSE;
            sim_connect(sc->sim_data.sim_async);
            sim_connect(sc->sim_data.sim_sync);
            sc->connected = TRUE;
        }
    } else {
        SMS_A_(SMS_LOG_E("main.c: MessagingSystem_connect_sim_context: sc NULL"));
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_disconnect_sim_context
 *
 * @description  Disconnects to the sim.
 *
 * @param        sc : context to use.
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_disconnect_sim_context(
    ste_sms_sim_context_t * sc)
{
    SMS_B_(SMS_LOG_D("main.c: %s called", __func__));

    if (sc != NULL) {
        if (sc->connected == TRUE) {
            sim_disconnect(sc->sim_data.sim_async);
            sim_disconnect(sc->sim_data.sim_sync);
            sc->connected = FALSE;
        }
    } else {
        SMS_A_(SMS_LOG_E("main.c: MessagingSystem_disconnect_sim_context: sc NULL"));
    }
}


/********************************************************************/
/**
 *
 * @function     call_network_connect
 *
 * @description  Do the necessary connect towards the Call & Network
 *               server.
 *
 * @param        **cn_context_t :
 * @return       void
 */
/********************************************************************/
static void call_network_connect(
        cn_context_t **cn_context_pp,
        int *fd_p)
{
    cn_error_code_t result;
    int attempt = 0;
    int event_fd;

    // Loop until a connection is achieved
    for (;;) {
        attempt++;
        result = cn_client_init(cn_context_pp);
        SMS_A_(SMS_LOG_I("main.c: cn_client_init attempt %d result = %d", attempt, result));
        if (CN_SUCCESS == result) {
            // Get request socket file descriptor
            result = cn_client_get_request_fd(*cn_context_pp, fd_p);

            if (CN_SUCCESS != result) {
                SMS_A_(SMS_LOG_E("smmain.c: cn_client_get_request_fd result: %d ", result));
            }

            // Get event socket file descriptor, and close it
            result = cn_client_get_event_fd(*cn_context_pp, &event_fd);

            if (CN_SUCCESS == result && !(0 > event_fd)) {
                close(event_fd);
            }
            break;
        }
        sleep(1);
    }
}


/********************************************************************/
/**
 *
 * @function     call_network_disconnect
 *
 * @description  Do the necessary disconnect from the Call & Network
 *               server.
 *
 * @param        *cn_context :
 * @return       void
 */
/********************************************************************/
static void call_network_disconnect(
        cn_context_t **cn_context_pp)
{
    cn_error_code_t result;

    result = cn_client_shutdown(*cn_context_pp);
    SMS_A_(SMS_LOG_I("main.c: cn_client_shutdown result = %d", result));
    if (CN_SUCCESS == result) {
        *cn_context_pp = NULL;
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_connect_call_network
 *
 * @description  Connects to Call & Network server.
 *
 * @param        **cn_context_t :
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_connect_call_network(
        cn_context_t **cn_context_pp,
        int *fd_p)
{
    SMS_B_(SMS_LOG_D("main.c: %s called", __func__));

    if (*cn_context_pp == NULL && fd_p) {
        call_network_connect(cn_context_pp, fd_p);
    } else {
        SMS_A_(SMS_LOG_E("main.c: %s: cn_context_p not NULL or fd_p NULL", __func__));
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_disconnect_call_network
 *
 * @description  Disconnects from Call & Network server.
 *
 * @param        sc : context to use.
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_disconnect_call_network(
        cn_context_t **cn_context_pp)
{
    SMS_B_(SMS_LOG_D("main.c: %s called", __func__));

    if (*cn_context_pp != NULL) {
        call_network_disconnect(cn_context_pp);
    } else {
        SMS_A_(SMS_LOG_E("main.c: %s: cn_context_p is NULL", __func__));
    }
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_SMS_SignalProcessed
 *
 * @description  Main switch statement to farm out functionality
 *
 * @param        **RecBuf_pp : as type of union SIGNAL.
 * @param        MFL_Primitive. Only contains a valid value if handling
                 a signal from MFL modem. This is used because signals
                 from MFL modem are of type union SIGNAL** and it is possible
                 that this may differ from union SMS_SIGNAL** at a later time.
 *
 * @return       static void
 */
/********************************************************************/
static uint8_t MessagingSystem_SMS_SignalProcessed(
    union SMS_SIGNAL
    **RecBuf_pp,
    const uint32_t MFL_Primitive)
{
    EventData_t eventData = {NULL, NULL, 0, NULL, 0};
    uint8_t signalProcessed = TRUE;
    uint8_t IsValidSignal;
    uint32_t Primitive;
    if (MFL_Primitive != SMS_SIGNAL_PRIMITIVE_INVALID) {
        Primitive = MFL_Primitive;
    }

    else {
        Primitive = (*RecBuf_pp)->Primitive;
    }
    switch (Primitive) {
        // ==========================================================
        // Internal events generated from within the Messaging system
        // ==========================================================
    case CAT_REFRESH_FILE_CHANGE_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(CAT_REFRESH_FILE_CHANGE_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: CAT_REFRESH_FILE_CHANGE_TIMER_IND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                MessagingSystem_CatRefresh_TimerExpired();
            }
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

        //========================================================================
    case MSG_STORAGE_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(MSG_STORAGE_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: MSG_STORAGE_TIMER_IND %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                ShortMessageManager_PollStorage();
            }
#endif                          // REMOVE_SMSTRG_LAYER
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

        //========================================================================
    case MSG_SMSTORAGE_INIT_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(MSG_SMSTORAGE_INIT_TIMER_IND);
            SMS_B_(SMS_LOG_D("smsmain.c: MSG_SMSTORAGE_INIT_TIMER_IND %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SmStorage_EventHandler(SMSTORAGEEVENT_TIMEOUT);
            }
#endif                          // REMOVE_SMSTRG_LAYER
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

        //========================================================================
    case MSG_SMS_DELIVER_REPORT_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(MSG_SMS_DELIVER_REPORT_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: MSG_SMS_DELIVER_REPORT_TIMER_IND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                (void)ShortMessageManager_ShortMessageNack(theShortMessageManager);
            }
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

        //========================================================================
    case MSG_SMS_MORETOSEND_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(MSG_SMS_MORETOSEND_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: MSG_SMS_MORETOSEND_TIMER_IND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_MORETOSEND_TIMEOUT, &eventData);
            }
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

        //========================================================================
    case MSG_RELAY_LINK_CONTROL_TIMER_IND:
        {
            IsValidSignal = SignalValidForState(MSG_RELAY_LINK_CONTROL_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: MSG_RELAY_LINK_CONTROL_TIMER_IND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_RELAY_LINK_TIMEOUT, &eventData);
            }
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

#ifdef SMS_USE_MFL_MODEM
        // ==========================================================
        // Events from Network Signalling
        // ==========================================================
    case EVENT_RMS_V2_MESSAGERECEIVEDIND:
        {
            uint8_t SignalValidForRule = TRUE;
            IsValidSignal = SignalValidForState(EVENT_RMS_V2_MESSAGERECEIVEDIND);
            SMS_A_(SMS_LOG_I("smsmain.c: EVENT_RMS_V2_MESSAGERECEIVEDIND %s", signalState[IsValidSignal && SignalValidForRule]));
            if (IsValidSignal && SignalValidForRule) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_RECEIVE_SHORT_MESSAGE, &eventData);
            }

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case EVENT_RMS_V2_FAILURERECEIVEDIND:
        {
            IsValidSignal = SignalValidForState(EVENT_RMS_V2_FAILURERECEIVEDIND);
            SMS_A_(SMS_LOG_I("smsmain.c: EVENT_RMS_V2_FAILURERECEIVEDIND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                SMS_B_(SMS_LOG_D("NS connection has been lost, resetting state."));
                // Stop the timer which would NACK the message if it expired
                Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_DELIVER_REPORT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);

                // Clear the information on the last short message
                theShortMessageManager->LastReceived_SMS_PID = 0;
                theShortMessageManager->LastReceived_SMS_Position = SMS_STORAGE_POSITION_INVALID;

                // Check if client app has requested a deliver report send... they might want an answer
                if (theShortMessageManager->DeliverReportSenderSignalInfo.ProcessId != SMS_NO_PROCESS) {

                    // We've sent the deliver report request to NS and then they've reported a failure -
                    // they will not respond the the deliver report request so here we respond to the
                    // client app with a failure indication.
                    SMS_DeliverReportSend_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportSend_Resp_t,
                                                                               RESPONSE_SMS_DELIVERREPORTSEND);
                    RMS_V2_Status_t FailureCause;

                    // Set the client tag in the send signal from 'global' data
                    Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);

                    // Unpack the NS event
                    if (Event_RMS_V2_FailureReceivedInd(eventData.DataPointer, &FailureCause) == GS_EVENT_OK) {

                        // Failure cause from NS...
                        SendBuf_p->ErrorCode = SmUtil_TranslateErrorCodes(FailureCause.ErrorCode);
                    }

                    else {

                        // Failed to unpack event... default failure cause
                        SendBuf_p->ErrorCode = SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED;
                    }
                    SMS_B_(SMS_LOG_D("Client has requested DELIVERREPORTSEND, sending failure response."));
                    SMS_SEND(&SendBuf_p, theShortMessageManager->DeliverReportSenderSignalInfo.ProcessId);

                    // Reset state: delivery report send is no longer in progress
                    theShortMessageManager->DeliverReportSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
                }
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_RECEIVE_FAILURE, &eventData);
            }

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE:
        {
            IsValidSignal = SignalValidForState(R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE);
            SMS_A_(SMS_LOG_I("smsmain.c: R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_NETWORK_ACK, &eventData);
            }

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT:
        {
            IsValidSignal = SignalValidForState(R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT);
            SMS_A_(SMS_LOG_I("smsmain.c: R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT %s", signalState[IsValidSignal != 0]));
            // Clear the information on the last short message
            theShortMessageManager->LastReceived_SMS_PID = 0;
            theShortMessageManager->LastReceived_SMS_Position = SMS_STORAGE_POSITION_INVALID;
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_DELIVERREPORT_CONFIRM, &eventData);
            }

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case R_RESP_RMS_V2_MESSAGE_ABORT:
        {
            IsValidSignal = SignalValidForState(R_RESP_RMS_V2_MESSAGE_ABORT);
            SMS_A_(SMS_LOG_I("smsmain.c: R_RESP_RMS_V2_MESSAGE_ABORT %s", signalState[IsValidSignal != 0]));
            // Dummy requester responses which are called with NO_WAIT
            // to prevent lockup waiting for responses from SMR
            // No need to do anything with them.

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case R_RESP_RMS_V2_MESSAGE_SEND:
        {
            IsValidSignal = SignalValidForState(R_RESP_RMS_V2_MESSAGE_SEND);
            SMS_A_(SMS_LOG_I("smsmain.c: R_RESP_RMS_V2_MESSAGE_SEND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_NETWORK_ACK, &eventData);
            }

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;

        //========================================================================
    case RESPONSE_EVENTCHANNEL_SUBSCRIBE:
        {
            ClientTag_t ClientTag;
            ClientTag = mfl_get_client_tag(*RecBuf_pp);
            IsValidSignal = Response_EventChannel_Subscribe(*RecBuf_pp) == REQUEST_OK;
#ifdef SMS_PRINT_A_
            char *client_tag_str;
            if (ClientTag == EVENT_RMS_V2_MESSAGERECEIVEDIND) {
                client_tag_str = "EVENT_RMS_V2_MESSAGERECEIVEDIND";
            } else if (ClientTag == EVENT_RMS_V2_FAILURERECEIVEDIND) {
                client_tag_str = "EVENT_RMS_V2_FAILURERECEIVEDIND";
            } else {
                if (0 > asprintf(&client_tag_str, "Unexpected Event 0x%08X", ClientTag)) {
                    client_tag_str = "?ClientTag?";
                }
            }
            SMS_LOG_I("smsmain.c: RESPONSE_EVENTCHANNEL_SUBSCRIBE: %s %s", client_tag_str, signalState[IsValidSignal != 0]));
#endif

            // Free signal using MFL macro
            SIGNAL_FREE((void **) RecBuf_pp);
        } break;
#else
        //========================================================================
    case MSG_MAL_CONFIGURE_RETRY_TIMER_IND:
        {
            int Result;
            IsValidSignal = SignalValidForState(MSG_MAL_CONFIGURE_RETRY_TIMER_IND);
            SMS_A_(SMS_LOG_I("smsmain.c: MSG_MAL_CONFIGURE_RETRY_TIMER_IND %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SMS_A_(SMS_LOG_I("smsmain.c: retry mal_sms_cbs_config()"));
                if (MAL_SMS_CBS_SUCCESS != (Result = mal_sms_cbs_config())) {
                    SMS_A_(SMS_LOG_E("smsmain.c: mal_sms_cbs_config ErrorCause: %d ", Result));
                }
            }

            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;

#endif                          // SMS_USE_MFL_MODEM
        //========================================================================
    case REQUEST_SMS_CB_SESSIONCREATE:
        {
            SMS_CB_SessionCreate_Req_t *RecBuf_p = (SMS_CB_SessionCreate_Req_t *) * RecBuf_pp;
            SMS_CB_SessionCreate_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_CB_SessionCreate_Resp_t,
                                                                      RESPONSE_SMS_CB_SESSIONCREATE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_CB_SESSIONCREATE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_CB_SESSIONCREATE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = MessagingSystem_EventsSubscription(SMS_EVENT_RECEIVER(RecBuf_p), TRUE);  // Subscribe
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_CB_SESSIONDESTROY:
        {
            SMS_CB_SessionDestroy_Req_t *RecBuf_p = (SMS_CB_SessionDestroy_Req_t *) * RecBuf_pp;
            SMS_CB_SessionDestroy_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_CB_SessionDestroy_Resp_t,
                                                                       RESPONSE_SMS_CB_SESSIONDESTROY);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_CB_SESSIONDESTROY"));
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);
            SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            // TODO Undo call to MessagingSystem_EventsSubscription() in "case REQUEST_SMS_CB_SESSIONCREATE:"
            // I.e. handle the event unsubscription.
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

#if 0
        // TODO Probably not needed here.
    case REQUEST_SMS_CB_SHUTDOWN:
        {
            SMS_CB_Shutdown_Req_t *RecBuf_p = (SMS_CB_Shutdown_Req_t *) * RecBuf_pp;
            IsValidSignal = SignalValidForState(REQUEST_SMS_CB_SHUTDOWN);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_CB_SHUTDOWN %s", signalState[IsValidSignal != 0]));
            // TODO Not yet added to state machine
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

#endif                          /*  */
        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEDELIVER:
        {
            SMS_ShortMessageDeliver_Req_t *RecBuf_p = (SMS_ShortMessageDeliver_Req_t *) * RecBuf_pp;
            SMS_ShortMessageDeliver_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageDeliver_Resp_t,
                                                                         RESPONSE_SMS_SHORTMESSAGEDELIVER);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEDELIVER);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEDELIVER %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                eventData.DataPointer = *RecBuf_pp;
                ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_TPDUDELIVER, &eventData);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        // ===============================================================================
        // Request signal for client application Short Message acknowledge with a confirm
        // ===============================================================================
    case REQUEST_SMS_DELIVERREPORTSEND:
        {
            SMS_DeliverReportSend_Req_t *RecBuf_p = (SMS_DeliverReportSend_Req_t *) * RecBuf_pp;
            SMS_Error_t MSG_Error = SMS_ERROR_INTERN_NO_ERROR;

            IsValidSignal = SignalValidForState(REQUEST_SMS_DELIVERREPORTSEND);
            SMS_A_(SMS_LOG_I("smsmain.c : REQUEST_SMS_DELIVERREPORTSEND Process = 0x%08x, ClientTag = 0x%08x %s",
                             (unsigned int) MessagingSystem.SenderSignalInfo.ProcessId,
                             (unsigned int) MessagingSystem.SenderSignalInfo.ClientTag,
                             signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                MSG_Error = ShortMessageManager_SendDeliverReport(theShortMessageManager, RecBuf_p->RP_ErrorCause, &RecBuf_p->TPDU, &MessagingSystem.SenderSignalInfo);
            } else {
                MSG_Error = SMS_ERROR_INTERN_SERVER_BUSY;
            }

            // In this instance, we never send the signal to NS so we need to send
            // the response signal back ourselves
            if (MSG_Error != SMS_ERROR_INTERN_NO_ERROR) {
                SMS_DeliverReportSend_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportSend_Resp_t,
                                                                           RESPONSE_SMS_DELIVERREPORTSEND);

                // Set the client tag in the send signal from 'global' data
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SendBuf_p->ErrorCode = MSG_Error;
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            }
            // If the reqest was successful, we do not need to do a SEND here, as it happens
            // when NS ack's the message using MessagingSystem_DeliverReportConfirm()
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        // ================================================================
        // Request signal for client application Memory Capacity Available
        // ================================================================
    case REQUEST_SMS_MEMCAPACITYAVAILABLESEND:
        {
            SMS_MemCapacityAvailableSend_Req_t *RecBuf_p = (SMS_MemCapacityAvailableSend_Req_t *) * RecBuf_pp;
            SMS_Error_t MSG_Error = SMS_ERROR_INTERN_NO_ERROR;

            IsValidSignal = SignalValidForState(REQUEST_SMS_MEMCAPACITYAVAILABLESEND);
            SMS_A_(SMS_LOG_I("smsmain.c : REQUEST_SMS_MEMCAPACITYAVAILABLESEND Process = 0x%08x, ClientTag = 0x%08x %s",
                             (unsigned int) MessagingSystem.SenderSignalInfo.ProcessId,
                             (unsigned int) MessagingSystem.SenderSignalInfo.ClientTag,
                             signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                MSG_Error = ShortMessageManager_Send_MemCapacityAvailable(theShortMessageManager, &eventData, &MessagingSystem.SenderSignalInfo);
            } else {
                MSG_Error = SMS_ERROR_INTERN_SERVER_BUSY;
            }

            // In this instance, we never send the signal to NS so we need to send
            // the response signal back ourselves
            if (MSG_Error != SMS_ERROR_INTERN_NO_ERROR) {
                SMS_MemCapacityAvailableSend_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityAvailableSend_Resp_t,
                                                                                  RESPONSE_SMS_MEMCAPACITYAVAILABLESEND);

                // Get the client tag to 'global' data to be able to use it in the send
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SendBuf_p->ErrorCode = MSG_Error;
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            }
            // If the request was successful, we do not need to do a SEND here, as it happens
            // when NS ack's the message using MessagingSystem_Send_MemCapacityAvailableResponse
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        // ================================================================
        // Request signal for client application Memory Capacity Get State
        // ================================================================
    case REQUEST_SMS_MEMCAPACITYGETSTATE:
        {
            SMS_MemCapacityGetState_Req_t *RecBuf_p = (SMS_MemCapacityGetState_Req_t *) * RecBuf_pp;
            uint8_t memFullState = 0;
            SMS_Error_t MSG_Error = SMS_ERROR_INTERN_NO_ERROR;

            IsValidSignal = SignalValidForState(REQUEST_SMS_MEMCAPACITYGETSTATE);
            SMS_A_(SMS_LOG_I("smsmain.c : REQUEST_SMS_MEMCAPACITYGETSTATE Process = 0x%08x, ClientTag = 0x%08x %s",
                             (unsigned int) MessagingSystem.SenderSignalInfo.ProcessId,
                             (unsigned int) MessagingSystem.SenderSignalInfo.ClientTag,
                             signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                memFullState = ShortMessageManager_Send_MemCapacityGetState();
            } else {
                MSG_Error = SMS_ERROR_INTERN_SERVER_BUSY;
            }

            SMS_MemCapacityGetState_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityGetState_Resp_t,
                                                                         RESPONSE_SMS_MEMCAPACITYGETSTATE);

            // Get the client tag to 'global' data to be able to use it in the send
            Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
            SendBuf_p->ErrorCode = MSG_Error;
            SendBuf_p->memFullState = memFullState;
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEREAD:
        {
            SMS_ShortMessageRead_Req_t *RecBuf_p = (SMS_ShortMessageRead_Req_t *) * RecBuf_pp;
            SMS_ShortMessageRead_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageRead_Resp_t,
                                                                      RESPONSE_SMS_SHORTMESSAGEREAD);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEREAD);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEREAD %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_Read(&(RecBuf_p->Slot), &(SendBuf_p->Status), &(SendBuf_p->SMSC_Address_TPDU));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEWRITE:
        {
            SMS_ShortMessageWrite_Req_t *RecBuf_p = (SMS_ShortMessageWrite_Req_t *) * RecBuf_pp;
            SMS_ShortMessageWrite_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageWrite_Resp_t,
                                                                       RESPONSE_SMS_SHORTMESSAGEWRITE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEWRITE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEWRITE %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_Write(RecBuf_p->Status, &(RecBuf_p->SMSC_Address_TPDU), &(RecBuf_p->Slot), &(SendBuf_p->Slot));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEDELETE:
        {
            SMS_ShortMessageDelete_Req_t *RecBuf_p = (SMS_ShortMessageDelete_Req_t *) * RecBuf_pp;
            SMS_ShortMessageDelete_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageDelete_Resp_t,
                                                                        RESPONSE_SMS_SHORTMESSAGEDELETE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEDELETE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEDELETE %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_Delete(&(RecBuf_p->Slot));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_STATUSREPORTREAD:
        {
            SMS_StatusReportRead_Req_t *RecBuf_p = (SMS_StatusReportRead_Req_t *) * RecBuf_pp;
            SMS_StatusReportRead_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_StatusReportRead_Resp_t,
                                                                      RESPONSE_SMS_STATUSREPORTREAD);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_STATUSREPORTREAD);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_STATUSREPORTREAD %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_StatusReportRead(&(RecBuf_p->Slot), &(SendBuf_p->TPDU_StatusReport));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGESTATUSSET:
        {
            SMS_ShortMessageStatusSet_Req_t *RecBuf_p = (SMS_ShortMessageStatusSet_Req_t *) * RecBuf_pp;
            SMS_ShortMessageStatusSet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageStatusSet_Resp_t,
                                                                           RESPONSE_SMS_SHORTMESSAGESTATUSSET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGESTATUSSET);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGESTATUSSET %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_StatusSet(&(RecBuf_p->Slot), RecBuf_p->Status);
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGESEND:
        {
            SMS_ShortMessageSend_Req_t *RecBuf_p = (SMS_ShortMessageSend_Req_t *) * RecBuf_pp;
            SMS_Error_t MSG_Error = SMS_ERROR_INTERN_NO_ERROR;

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGESEND);
            SMS_A_(SMS_LOG_I("smsmain.c : REQUEST_SMS_SHORTMESSAGESEND Process = 0x%08x, ClientTag = 0x%08x %s",
                             (unsigned int) MessagingSystem.SenderSignalInfo.ProcessId,
                             (unsigned int) MessagingSystem.SenderSignalInfo.ClientTag,
                             signalState[IsValidSignal != 0]));

            if (IsValidSignal) {
                MSG_Error = ShortMessageManager_SMS_Send(theShortMessageManager,
                                                         &(RecBuf_p->SMSC_Address_TPDU), &(RecBuf_p->Slot), RecBuf_p->MoreToSend, RecBuf_p->SatInitiated, &MessagingSystem.SenderSignalInfo);
            } else {
                MSG_Error = SMS_ERROR_INTERN_SERVER_BUSY;
            }

            // In this instance, we never send the signal to NS so we need to send
            // the response signal back ourselves
            if (MSG_Error != SMS_ERROR_INTERN_NO_ERROR) {
                SMS_ShortMessageSend_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageSend_Resp_t,
                                                                          RESPONSE_SMS_SHORTMESSAGESEND);

                // Set the client tag in the send signal from 'global' data
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SendBuf_p->ErrorCode = MSG_Error;
                SMS_A_(SMS_LOG_I("smsmain.c : RESPONSE_SMS_SHORTMESSAGESEND Process = 0x%08x, ClientTag = 0x%08x, ErrorCode = 0x%04x",
                                 (unsigned int) MessagingSystem.SenderSignalInfo.ProcessId,
                                 (unsigned int) MessagingSystem.SenderSignalInfo.ClientTag, SendBuf_p->ErrorCode));
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            }
            // If the reqest was successful, we do not need to do a SEND here, as it happens
            // when NS ack's the message using MessagingSystem_SMAcknowledge
            SMS_SIGNAL_FREE(&RecBuf_p);
        } break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEFIND:
        {
            SMS_ShortMessageFind_Req_t *RecBuf_p = (SMS_ShortMessageFind_Req_t *) * RecBuf_pp;
            SMS_ShortMessageFind_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageFind_Resp_t,
                                                                      RESPONSE_SMS_SHORTMESSAGEFIND);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEFIND);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEFIND %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_Find(RecBuf_p->SearchInfo.Slot, RecBuf_p->SearchInfo.Status, RecBuf_p->SearchInfo.BrowseOption, &(SendBuf_p->Slot));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_STORAGECAPACITYGET:
        {
            SMS_StorageCapacityGet_Req_t *RecBuf_p = (SMS_StorageCapacityGet_Req_t *) * RecBuf_pp;
            SMS_StorageCapacityGet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_StorageCapacityGet_Resp_t,
                                                                        RESPONSE_SMS_STORAGECAPACITYGET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_STORAGECAPACITYGET);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_STORAGECAPACITYGET %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_CapacityGet(RecBuf_p->Storage, &(SendBuf_p->SlotInfo), &(SendBuf_p->StorageStatus));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_STORAGECLEAR:
        {
            SMS_StorageClear_Req_t *RecBuf_p = (SMS_StorageClear_Req_t *) * RecBuf_pp;
            SMS_StorageClear_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_StorageClear_Resp_t,
                                                                  RESPONSE_SMS_STORAGECLEAR);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_STORAGECLEAR);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_STORAGECLEAR %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_StorageClear(RecBuf_p->Storage, RecBuf_p->AllThisICCID_Messages, RecBuf_p->AllOtherICCID_Messages);
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_SHORTMESSAGEINFOREAD:
        {
            SMS_ShortMessageInfoRead_Req_t *RecBuf_p = (SMS_ShortMessageInfoRead_Req_t *) * RecBuf_pp;
            SMS_ShortMessageInfoRead_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageInfoRead_Resp_t,
                                                                          RESPONSE_SMS_SHORTMESSAGEINFOREAD);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_SHORTMESSAGEINFOREAD);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_SHORTMESSAGEINFOREAD %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_SMS_InformationRead(&(RecBuf_p->Slot), &(SendBuf_p->ReadInformation));
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_MEMCAPACITYFULLREPORT:
        {
            SMS_MemCapacityFullReport_Req_t *RecBuf_p = (SMS_MemCapacityFullReport_Req_t *) * RecBuf_pp;
            SMS_MemCapacityFullReport_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityFullReport_Resp_t,
                                                                           RESPONSE_SMS_MEMCAPACITYFULLREPORT);

            IsValidSignal = SignalValidForState(REQUEST_SMS_MEMCAPACITYFULLREPORT);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_MEMCAPACITYFULLREPORT %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                ShortMessageManager_SMS_MemCapacityFullSet();
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_PREFERREDSTORAGESET:
        {
            SMS_PreferredStorageSet_Req_t *RecBuf_p = (SMS_PreferredStorageSet_Req_t *) * RecBuf_pp;
            SMS_PreferredStorageSet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_PreferredStorageSet_Resp_t,
                                                                         R_RESP_SMS_PREFERREDSTORAGESET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(R_REQ_SMS_PREFERREDSTORAGESET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_PREFERREDSTORAGESET %s", signalState[IsValidSignal != 0]));
#ifndef REMOVE_SMSTRG_LAYER
            if (IsValidSignal) {
                ShortMessageManager_SMS_PreferredStoreSet(RecBuf_p->PreferredStorage);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else
#endif                          // REMOVE_SMSTRG_LAYER
            {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_DELIVERREPORTCONTROLSET:
        {
            SMS_DeliverReportControlSet_Req_t *RecBuf_p = (SMS_DeliverReportControlSet_Req_t *) * RecBuf_pp;
            SMS_DeliverReportControlSet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportControlSet_Resp_t,
                                                                             R_RESP_SMS_DELIVERREPORTCONTROLSET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(R_REQ_SMS_DELIVERREPORTCONTROLSET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_DELIVERREPORTCONTROLSET %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                ShortMessageManager_SMS_DeliverReportControlSet(RecBuf_p->NetworkAcknowledge);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_DELIVERREPORTCONTROLGET:
        {
            SMS_DeliverReportControlGet_Req_t *RecBuf_p = (SMS_DeliverReportControlGet_Req_t *) * RecBuf_pp;
            SMS_DeliverReportControlGet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportControlGet_Resp_t,
                                                                             R_RESP_SMS_DELIVERREPORTCONTROLGET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(R_REQ_SMS_DELIVERREPORTCONTROLGET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_DELIVERREPORTCONTROLGET %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                ShortMessageManager_SMS_DeliverReportControlGet(&(SendBuf_p->NetworkAcknowledge));
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_MO_ROUTESET:
        {
            SMS_MO_RouteSet_Req_t *RecBuf_p = (SMS_MO_RouteSet_Req_t *) *RecBuf_pp;

            IsValidSignal = SignalValidForState(R_REQ_SMS_MO_ROUTESET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_MO_ROUTESET %s", signalState[IsValidSignal != 0]));

            if (IsValidSignal) {
                SMS_MO_RouteSet_Resp_t *SendBuf_p = NULL;

                /* synchronous signal handling (cache the route setting) */
                ShortMessageManager_SMS_MO_RouteSet(RecBuf_p->Route);

                SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteSet_Resp_t, R_RESP_SMS_MO_ROUTESET);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            } else {
                SMS_MO_RouteSet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteSet_Resp_t,
                                                                     R_RESP_SMS_MO_ROUTESET);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            }
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_MO_ROUTEGET:
        {
            SMS_MO_RouteGet_Req_t *RecBuf_p = (SMS_MO_RouteGet_Req_t *) *RecBuf_pp;

            IsValidSignal = SignalValidForState(R_REQ_SMS_MO_ROUTEGET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_MO_ROUTEGET %s", signalState[IsValidSignal != 0]));

            if (IsValidSignal) {
                SMS_MO_RouteGet_Resp_t *SendBuf_p = NULL;

                /* synchronous signal handling (read route setting from cache) */
                SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteGet_Resp_t, R_RESP_SMS_MO_ROUTEGET);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
                SendBuf_p->Route = ShortMessageManager_SMS_MO_RouteGet();
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            } else {
                SMS_MO_RouteGet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteGet_Resp_t,
                                                                     R_RESP_SMS_MO_ROUTEGET);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
                Do_SMS_ClientTag_Set(SendBuf_p, MessagingSystem.SenderSignalInfo.ClientTag);
                SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            }
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_RELAYLINKCONTROLSET:
        {
            SMS_RelayLinkControlSet_Req_t *RecBuf_p = (SMS_RelayLinkControlSet_Req_t *) * RecBuf_pp;
            SMS_RelayLinkControlSet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_RelayLinkControlSet_Resp_t,
                                                                         R_RESP_SMS_RELAYLINKCONTROLSET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(R_REQ_SMS_RELAYLINKCONTROLSET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_RELAYLINKCONTROLSET %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                ShortMessageManager_SMS_RelayLinkControl_Set(RecBuf_p->RelayLinkControl);
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case R_REQ_SMS_RELAYLINKCONTROLGET:
        {
            SMS_RelayLinkControlGet_Req_t *RecBuf_p = (SMS_RelayLinkControlGet_Req_t *) * RecBuf_pp;
            SMS_RelayLinkControlGet_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_RelayLinkControlGet_Resp_t,
                                                                         R_RESP_SMS_RELAYLINKCONTROLGET);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(R_REQ_SMS_RELAYLINKCONTROLGET);
            SMS_A_(SMS_LOG_I("smsmain.c: R_REQ_SMS_RELAYLINKCONTROLGET %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                ShortMessageManager_SMS_RelayLinkControl_Get(&(SendBuf_p->RelayLinkControl));
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_APPLICATIONPORTSUBSCRIBE:
        {
            SMS_ApplicationPort_Req_t *RecBuf_p = (SMS_ApplicationPort_Req_t *) * RecBuf_pp;
            SMS_ApplicationPort_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ApplicationPort_Resp_t,
                                                                     RESPONSE_SMS_APPLICATIONPORTSUBSCRIBE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_APPLICATIONPORTSUBSCRIBE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_APPLICATIONPORTSUBSCRIBE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_AddPortSubscription(SMS_EVENT_RECEIVER(RecBuf_p), MessagingSystem.SenderSignalInfo.ClientTag, &RecBuf_p->ApplicationPortRange);
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE:
        {
            SMS_ApplicationPort_Req_t *RecBuf_p = (SMS_ApplicationPort_Req_t *) * RecBuf_pp;
            SMS_ApplicationPort_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_ApplicationPort_Resp_t,
                                                                     RESPONSE_SMS_APPLICATIONPORTUNSUBSCRIBE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = ShortMessageManager_DeletePortSubscription(SMS_EVENT_RECEIVER(RecBuf_p), MessagingSystem.SenderSignalInfo.ClientTag, &RecBuf_p->ApplicationPortRange);
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_EVENTSSUBSCRIBE:
        {
            SMS_Events_Req_t *RecBuf_p = (SMS_Events_Req_t *) * RecBuf_pp;
            SMS_Events_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_Events_Resp_t,
                                                            RESPONSE_SMS_EVENTSSUBSCRIBE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_EVENTSSUBSCRIBE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_EVENTSSUBSCRIBE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = MessagingSystem_ShortMessageEventsSubscription(SMS_EVENT_RECEIVER(RecBuf_p), TRUE);      // Subscribe
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_SMS_EVENTSUNSUBSCRIBE:
        {
            SMS_Events_Req_t *RecBuf_p = (SMS_Events_Req_t *) * RecBuf_pp;
            SMS_Events_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(SMS_Events_Resp_t,
                                                            RESPONSE_SMS_EVENTSUNSUBSCRIBE);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            IsValidSignal = SignalValidForState(REQUEST_SMS_EVENTSUNSUBSCRIBE);
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_EVENTSUNSUBSCRIBE %s", signalState[IsValidSignal != 0]));
            if (IsValidSignal) {
                SendBuf_p->ErrorCode = MessagingSystem_ShortMessageEventsSubscription(SMS_EVENT_RECEIVER(RecBuf_p), FALSE);     // Unsubscribe
            } else {
                SendBuf_p->ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;
            }
            SMS_SEND(&SendBuf_p, MessagingSystem.SenderSignalInfo.ProcessId);
            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_CBS_SUBSCRIBE:
        {
            CBS_Subscription_Req_t *RecBuf_p;
            CBS_Subscription_Resp_t *SendBuf_p;
            CBS_MessagesList_t *Messages_p = NULL;
            CBS_Error_t Error = CBS_ERROR_NONE;

            RecBuf_p = (CBS_Subscription_Req_t *) * RecBuf_pp;
            SendBuf_p = SMS_SIGNAL_ALLOC(CBS_Subscription_Resp_t, RESPONSE_CBS_SUBSCRIBE);

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_SUBSCRIBE %s", signalState[ACCEPTED]));

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->Error = CBS_Manager_MessageSubscribe(SMS_EVENT_RECEIVER(RecBuf_p),
                                                            RecBuf_p->SigselectWithClientTag.ClientTag,
                                                            RecBuf_p->All_CBMI, FALSE, RecBuf_p->NumberOfSubscriptions, &RecBuf_p->FirstSubscription[0], &Messages_p);

            Error = SendBuf_p->Error;

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));
            if (CBS_ERROR_NONE == Error) {
                CBS_Subscribe_ClientWithClientTagList_t Client = { NULL, SMS_EVENT_RECEIVER(RecBuf_p),
                                                                   RecBuf_p->SigselectWithClientTag.ClientTag, FALSE };
                CBS_MessagesList_t UmtsBuffer = { { { 0, 0, 0, 0 }, 0, { 0 } }, NULL };
                CBS_MessagesList_t *Message_p = NULL;

                // Iterate through the cached Messages_p, which is a list of previously cached CB messages which should be
                // dispatched as a consequence of the new subscription which has just been processed.
                // We do that now, so that the client will have received the response from its subscription request before
                // it receives any CB Messages.
                for (Message_p = Messages_p; NULL != Message_p; Message_p = Message_p->Next_p) {
                    Error = CBS_Manager_HandleMessage(&Client, &Message_p->Message, TRUE, &UmtsBuffer, NULL, NULL);

                    if (CBS_ERROR_NONE != Error) {
                        SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_SUBSCRIBE handle message failed Error:%d", Error));
                        break;
                    }
                }

                while (NULL != Messages_p) {
                    Message_p = Messages_p;
                    Messages_p = Message_p->Next_p;
                    SMS_HEAP_FREE(&Message_p);
                }

                if (CBS_ERROR_NONE == Error && NULL != UmtsBuffer.Next_p) {
                    while (UmtsBuffer.Next_p) {
                        Messages_p = CBS_Msgs_GetNextUmtsMessage(&UmtsBuffer.Next_p);

                        if (NULL != Messages_p) {
                            Error = CBS_Manager_DispatchUmtsBuffer(&Client, Messages_p);

                            if (CBS_ERROR_NONE != Error) {
                                SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_SUBSCRIBE Dispatch UMTS message failed Error:%d", Error));
                            }

                            while (NULL != Messages_p) {
                                Message_p = Messages_p;
                                Messages_p = Message_p->Next_p;
                                SMS_HEAP_FREE(&Message_p);
                            }
                        }
                    }
                }

                while (NULL != UmtsBuffer.Next_p) {
                    Message_p = UmtsBuffer.Next_p;
                    UmtsBuffer.Next_p = Message_p->Next_p;
                    SMS_HEAP_FREE(&Message_p);
                }
            }

            SMS_SIGNAL_FREE(&RecBuf_p);

            // Check that the modem is in the correct state to receive CB messages after this subscribe has been processed.
            CBS_Manager_CheckActivation();
        }
        break;

        //========================================================================
    case REQUEST_CBS_UNSUBSCRIBE:
        {
            CBS_Subscription_Req_t *RecBuf_p;
            CBS_Subscription_Resp_t *SendBuf_p;

            RecBuf_p = (CBS_Subscription_Req_t *) * RecBuf_pp;
            SendBuf_p = SMS_SIGNAL_ALLOC(CBS_Subscription_Resp_t, RESPONSE_CBS_UNSUBSCRIBE);

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_UNSUBSCRIBE %s", signalState[ACCEPTED]));

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->Error = CBS_Manager_MessageUnSubscribe(SMS_EVENT_RECEIVER(RecBuf_p),
                                                              RecBuf_p->All_CBMI, FALSE, RecBuf_p->NumberOfSubscriptions, &RecBuf_p->FirstSubscription[0]);

            // Ensure that modem is still in the correct state.
            (void) CBS_Manager_CheckActivation();

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);

            // Check that the modem is in the correct state to receive CB messages after this unsubscribe has been processed.
            CBS_Manager_CheckActivation();
        }
        break;

        //========================================================================
    case REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS:
        {
            CBS_GetSubscriptions_Req_t *RecBuf_p = (CBS_GetSubscriptions_Req_t *) * RecBuf_pp;
            CBS_GetSubscriptions_Resp_t *SendBuf_p = SMS_SIGNAL_ALLOC(CBS_GetSubscriptions_Resp_t, RESPONSE_CBS_GETNUMBEROFSUBSCRIPTIONS);

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS %s", signalState[ACCEPTED]));

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);
            SendBuf_p->NumberOfSubscriptions = CBS_Manager_MessageNumberOfSubscriptionsGet(SMS_EVENT_RECEIVER(RecBuf_p), RecBuf_p->AllSubscriptions, FALSE);

            if (SendBuf_p->NumberOfSubscriptions > 0) {
                SendBuf_p->Error = CBS_ERROR_NONE;
            } else {
                SendBuf_p->Error = CBS_ERROR_INVALID_SUBSCRIPTION;
            }

            // Initialise the rest of the buffer although its values are not relevant for this request / response pair.
            SendBuf_p->FirstSubscription[0].FirstMessageIdentifier = 0;
            SendBuf_p->FirstSubscription[0].LastMessageIdentifier = 0;
            SendBuf_p->FirstSubscription[0].FirstDataCodingScheme = 0;
            SendBuf_p->FirstSubscription[0].LastDataCodingScheme = 0;

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_CBS_GETSUBSCRIPTIONS:
        {
            uint8_t NumSubs;
            CBS_GetSubscriptions_Req_t *RecBuf_p = (CBS_GetSubscriptions_Req_t *) * RecBuf_pp;
            CBS_GetSubscriptions_Resp_t *SendBuf_p;

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_GETSUBSCRIPTIONS %s", signalState[ACCEPTED]));

            // Get the maximum number of subscriptions which could be returned.
            NumSubs = CBS_Manager_MessageNumberOfSubscriptionsGet(SMS_EVENT_RECEIVER(RecBuf_p), RecBuf_p->AllSubscriptions, FALSE);

            // Limit the number of subscriptions to be no larger than the client has requested.
            NumSubs = SMS_MIN(NumSubs, RecBuf_p->NumberOfSubscriptions);

            // Allocate the response signal buffer.
            SendBuf_p = SMS_SIGNAL_UNTYPED_ALLOC(sizeof(CBS_GetSubscriptions_Resp_t) + (NumSubs * sizeof(CBS_Subscription_t)), RESPONSE_CBS_GETSUBSCRIPTIONS);

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->NumberOfSubscriptions = NumSubs;
            if (NumSubs > 0) {
                SendBuf_p->Error = CBS_ERROR_NONE;
                CBS_Manager_Message_SubscriptionsGet(SMS_EVENT_RECEIVER(RecBuf_p), RecBuf_p->AllSubscriptions, NumSubs, FALSE,
                                                     &SendBuf_p->FirstSubscription[0]);
            } else {
                SendBuf_p->Error = CBS_ERROR_INVALID_SUBSCRIPTION;
            }

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_CBS_ACTIVATE:
        {
            CBS_Activation_Req_t *RecBuf_p;
            CBS_Activation_Resp_t *SendBuf_p;

            RecBuf_p = (CBS_Activation_Req_t *) * RecBuf_pp;
            SendBuf_p = SMS_SIGNAL_ALLOC(CBS_Activation_Resp_t, RESPONSE_CBS_ACTIVATE);

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_ACTIVATE %s", signalState[ACCEPTED]));

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->Error = CBS_Manager_Activate();

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_CBS_DEACTIVATE:
        {
            CBS_Activation_Req_t *RecBuf_p;
            CBS_Activation_Resp_t *SendBuf_p;

            RecBuf_p = (CBS_Activation_Req_t *) * RecBuf_pp;
            SendBuf_p = SMS_SIGNAL_ALLOC(CBS_Activation_Resp_t, RESPONSE_CBS_DEACTIVATE);

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_DEACTIVATE %s", signalState[ACCEPTED]));

            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->Error = CBS_Manager_Deactivate();

            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

        //========================================================================
    case REQUEST_CBS_CELLBROADCASTMESSAGEREAD:
        {
            CBS_CellBroadcastMessageRead_Req_t *RecBuf_p = (CBS_CellBroadcastMessageRead_Req_t *) * RecBuf_pp;
            CBS_CellBroadcastMessageRead_Resp_t *SendBuf_p;
            CBS_MessagesList_t *Messages_p = NULL;
            CBS_MessagesList_t *Message_p = NULL;
            uint8_t *Pdu_p = NULL;
            uint16_t PduLength = 0;
            CBS_Error_t Error = CBS_ERROR_NONE;

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_CELLBROADCASTMESSAGEREAD %s", signalState[ACCEPTED]));

            Error = CBS_Msgs_RetrieveMessage(&RecBuf_p->MessageHeader, &Messages_p);

            if (CBS_ERROR_NONE == Error) {
                CBS_Subscribe_ClientWithClientTagList_t Client = { NULL, SMS_EVENT_RECEIVER(RecBuf_p),
                                                                   RecBuf_p->SigselectWithClientTag.ClientTag, FALSE };
                CBS_MessagesList_t UmtsBuffer = { { { 0, 0, 0, 0 }, 0, { 0 } }, NULL };

                for (Message_p = Messages_p; 0 == PduLength && NULL != Message_p; Message_p = Message_p->Next_p) {
                    Error = CBS_Manager_HandleMessage(&Client, &Message_p->Message, FALSE, &UmtsBuffer, &PduLength, &Pdu_p);

                    if (CBS_ERROR_NONE != Error) {
                        SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_CELLBROADCASTMESSAGEREAD handle message failed Error:%d", Error));
                        break;
                    }
                }

                if (CBS_ERROR_NONE != Error && 0 == PduLength && NULL != UmtsBuffer.Next_p) {
                    Error = CBS_Manager_ConvertUmtsMessagesToPdu(UmtsBuffer.Next_p, &PduLength, &Pdu_p);

                    if (CBS_ERROR_NONE != Error) {
                        SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_CBS_CELLBROADCASTMESSAGEREAD convert message to PDU failed Error:%d", Error));
                    }
                }

                while (NULL != UmtsBuffer.Next_p) {
                    Message_p = UmtsBuffer.Next_p;
                    UmtsBuffer.Next_p = Message_p->Next_p;
                    SMS_HEAP_FREE(&Message_p);
                }
            } else {
                SMS_A_(SMS_LOG_I("smsmain.c: CBS_Msgs_RetrieveMessage handle message failed Error:%d", Error));
            }

            while (NULL != Messages_p) {
                Message_p = Messages_p;
                Messages_p = Message_p->Next_p;
                SMS_HEAP_FREE(&Message_p);
            }

            // Allocate the response signal buffer.
            SendBuf_p = SMS_SIGNAL_UNTYPED_ALLOC(sizeof(CBS_CellBroadcastMessageRead_Resp_t), RESPONSE_CBS_CELLBROADCASTMESSAGEREAD);
            Do_SMS_ClientTag_Copy(SendBuf_p, RecBuf_p);

            SendBuf_p->Error = Error;
            SendBuf_p->PduLength = PduLength;

            if (NULL != Pdu_p) {
                SMS_HEAP_FREE(&Pdu_p);
                SendBuf_p->Pdu[0] = 1;
            } else {
                SendBuf_p->Pdu[0] = 1;
            }
            SMS_SEND(&SendBuf_p, SMS_SENDER(&RecBuf_p));

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;

#ifdef CBS_USE_MFL_MODEM
        //========================================================================
    case EVENT_CEB_PAGE:
        {
            SMS_A_(SMS_LOG_I("smsmain.c: EVENT_CEB_PAGE %s", signalState[ACCEPTED]));

            (void) CBS_Manager_HandleNewCbIndication(RecBuf_p);

            SMS_SIGNAL_FREE(&RecBuf_p);
        }
        break;
#endif                          // CBS_USE_MFL_MODEM

#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
        //========================================================================
    case REQUEST_SMS_TEST_UTIL_MEMORY_STATUS_GET:
        {
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_TEST_UTIL_MEMORY_STATUS_GET %s", signalState[ACCEPTED]));
            SMS_SIGNAL_FREE(RecBuf_pp);
            (void) SmUtil_HeapStatus();
        }
        break;
#endif

#ifdef SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
        //========================================================================
    case REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_IND:
        {
            int cause;
            uintptr_t client_tag;
            void *data;
            void *user_data;
            SMS_Test_CAT_Cause_PC_Refresh_Ind_t *RefreshInd_p = (SMS_Test_CAT_Cause_PC_Refresh_Ind_t *) * RecBuf_pp;

            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_IND %s", signalState[ACCEPTED]));

            cause = STE_CAT_CAUSE_PC_REFRESH_IND;
            client_tag = 0;     // not required.
            data = &RefreshInd_p->refresh_ind_data;
            user_data = MessagingSystem_UICC_SimAsyncGet();
            SMS_A_(SMS_LOG_I("smsmain.c: Calling sim_handle_callbacks() directly to simulate CAT Refresh"));
            sim_handle_callbacks(cause, client_tag, data, user_data);
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        // Compile time asserts to check that the UICC CAT Server data structures we have mimicked, to support this
        // CAT Refresh simulation functionality, have not changed.
        SMS_COMPILE_TIME_ASSERT_STATEMENT(sizeof(SMS_Test_copy_ste_sim_pc_refresh_type_t) == sizeof(ste_sim_pc_refresh_type_t));
        SMS_COMPILE_TIME_ASSERT_STATEMENT(sizeof(SMS_Test_copy_ste_cat_pc_refresh_ind_t) == sizeof(ste_cat_pc_refresh_ind_t));
        SMS_COMPILE_TIME_ASSERT_STATEMENT(sizeof(SMS_Test_copy_sim_path_t) == sizeof(sim_path_t));
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_SIM_MAX_PATH_LENGTH == SIM_MAX_PATH_LENGTH);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE == STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_FILE_CHANGE == STE_SIM_PC_REFRESH_FILE_CHANGE);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE == STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT == STE_SIM_PC_REFRESH_INIT);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_UICC_RESET == STE_SIM_PC_REFRESH_UICC_RESET);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_3G_APP_RESET == STE_SIM_PC_REFRESH_3G_APP_RESET);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_3G_SESSION_RESET == STE_SIM_PC_REFRESH_3G_SESSION_RESET);
        SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_TEST_COPY_STE_SIM_PC_REFRESH_RESERVED == STE_SIM_PC_REFRESH_RESERVED);
        break;

        //========================================================================
    case REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_FILE_IND:
        {
            int cause;
            uintptr_t client_tag;
            void *data;
            void *user_data;
            SMS_Test_CAT_Cause_PC_Refresh_File_Ind_t *RefreshFileInd_p = (SMS_Test_CAT_Cause_PC_Refresh_File_Ind_t *) * RecBuf_pp;
            SMS_A_(SMS_LOG_I("smsmain.c: REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_FILE_IND %s", signalState[ACCEPTED]));

            cause = STE_CAT_CAUSE_PC_REFRESH_FILE_IND;
            client_tag = 0;     // not required.
            data = &RefreshFileInd_p->refresh_file_ind_data;
            user_data = MessagingSystem_UICC_SimAsyncGet();
            SMS_A_(SMS_LOG_I("smsmain.c: Calling sim_handle_callbacks() directly to simulate CAT Refresh"));
            sim_handle_callbacks(cause, client_tag, data, user_data);
            SMS_SIGNAL_FREE(RecBuf_pp);
        }
        break;
#endif

        //========================================================================
    default:
        {
            SMS_A_(SMS_LOG_I("smsmain.c: Received an unhandled signal, Primitive %d", Primitive));
            signalProcessed = FALSE;
        }
        break;
    }                           // switch (*RecBuf_pp->Primitive)
    return signalProcessed;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_BuildFds
 *
 * @description  Call-back function, invoked to add FDs to the Read/Write FD set for the IPC_ServerContext_AwaitSignal.
 *               The IPC server process shall add its specific FDs to the incoming FD sets.
 *
 * @param
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_BuildFds(
    fd_set * const pReadFDs,
    fd_set * const pWriteFDs,
    int *const pHighestFD)
{
    SMS_IDENTIFIER_NOT_USED(pWriteFDs);
    if (MessagingSystem.SMS_Modem_fd == SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_BuildFds() Modem fd NOT Initialised"));
    } else {
        FD_SET(MessagingSystem.SMS_Modem_fd, pReadFDs);
        SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_BuildFds() Modem fd initialised"));

        if (MessagingSystem.SMS_Modem_fd > *pHighestFD) {
            *pHighestFD = MessagingSystem.SMS_Modem_fd;
        }
    }

    if (MessagingSystem.sms_log_fd == SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_BuildFds() sms log fd NOT Initialised"));
    } else {
        FD_SET(MessagingSystem.sms_log_fd, pReadFDs);
        SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_BuildFds() sms log fd initialised"));
        if (MessagingSystem.sms_log_fd > *pHighestFD) {
            *pHighestFD = MessagingSystem.sms_log_fd;
        }
    }

    if (MessagingSystem.CN_Client_fd == SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_BuildFds() Call Network fd NOT Initialised"));
    } else {
        FD_SET(MessagingSystem.CN_Client_fd, pReadFDs);
        SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_BuildFds() Call Network fd initialised"));

        if (MessagingSystem.CN_Client_fd > *pHighestFD) {
            *pHighestFD = MessagingSystem.CN_Client_fd;
        }
    }
#ifndef SMS_USE_MFL_MODEM
    if (MessagingSystem.SMS_Netlnk_fd == SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_BuildFds() MAL Netlnk fd not initialised"));
    } else {
        FD_SET(MessagingSystem.SMS_Netlnk_fd, pReadFDs);
        SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_BuildFds() MAL Netlnkfd initialised"));

        if (MessagingSystem.SMS_Netlnk_fd > *pHighestFD) {
            *pHighestFD = MessagingSystem.SMS_Netlnk_fd;
        }
    }

#endif                          /*SMS_USE_MFL_MODEM */
    if (MessagingSystem.sim_context) {
        // Add SIM event FD
        if (MessagingSystem.sim_context->sim_data.sim_async) {
            int fd;
            fd = ste_sim_fd(MessagingSystem.sim_context->sim_data.sim_async);
            SMS_B_(SMS_LOG_D("MessagingSystem_BuildFds - SIM:%d", fd));
            if (fd >= 0) {
                SMS_B_(SMS_LOG_D("MessagingSystem_BuildFds - FD_SET(SIM)"));
                FD_SET(fd, pReadFDs);
                if (fd > *pHighestFD) {
                    SMS_B_(SMS_LOG_D("MessagingSystem_BuildFds - pHighestFD = SIM"));
                    *pHighestFD = fd;
                }
            }
        }
    } else {
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_BuildFds() SIM context NOT Initialised"));
    }
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_HandleSIMFdChanges
 *
 * @description  Checks to see if a signal, from the UICC CAT Server
 *               is on its associated socket.
 *               If it is, the signal will be received from the socket.
 *
 * @param        pReadFDs: Read FDs returned by select().
 * @param        sim: SIM API data object.
 *
 * @return       status, negative value means SIM server disconnect.
 */
/********************************************************************/
static int MessagingSystem_HandleSIMFdChanges(
    fd_set * const pReadFDs,
    ste_sim_t * sim)
{
    int status = 0;
    SMS_B_(SMS_LOG_D("MessagingSystem_HandleSIMFdChanges"));
    if (sim) {
        int fd = ste_sim_fd(sim);
        int isset = FD_ISSET(fd, pReadFDs);
        SMS_B_(SMS_LOG_D("MessagingSystem_HandleSIMFdChanges: fd:%d isset:%d", fd, isset));
        if (fd >= 0 && isset) {
            FD_CLR(fd, pReadFDs);
            SMS_B_(SMS_LOG_D("MessagingSystem_HandleSIMFdChanges: sim read"));
            status = ste_sim_read(sim);
            SMS_B_(SMS_LOG_D("ste_sim_read :status:%d", status));
            if (status < 0) {
                SMS_A_(SMS_LOG_W("%s: disconnect by SIM server, status %d", __func__, status));
            }
        }
    }
    return status;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_HandleCallNetworkChanges
 *
 * @description  Processes the Cell Registration Information from the
 *               Call Netwrok module.
 *
 * @param        void.
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_HandleCallNetworkChanges(
    )
{
    cn_uint32_t unread;
    cn_message_t *msg_p = NULL;
    cn_cell_info_t *cn_cell_info_p = NULL;
    cn_error_code_t result = 0;
    ShortMessageManager_CN_CellInfo_t SmMngr_CN_CellInfo;

    // Currently the SMS CB Server only calls the registration info request
    // so the reponse to that request is all that is handled here
    result = cn_message_receive(MessagingSystem.CN_Client_fd, &unread, &msg_p);

    SmMngr_CN_CellInfo.data_info = SMS_CN_REG_DATA_ERROR;

    if (result == CN_SUCCESS && msg_p != NULL) {
        if (msg_p->type == CN_RESPONSE_CELL_INFO && msg_p->error_code == CN_SUCCESS) {
            if (msg_p->payload_size == sizeof(cn_cell_info_t)) {
                uint8_t len = 0;

                cn_cell_info_p = (cn_cell_info_t *) msg_p->payload;
                SmMngr_CN_CellInfo.cid = cn_cell_info_p->current_cell_id;
                SmMngr_CN_CellInfo.lac = cn_cell_info_p->current_ac;

                SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_HandleCallNetworkChanges RAT %d, CID 0x%08X, LAC 0x%04X, MCC_MNC :%s:",
                        cn_cell_info_p->rat, cn_cell_info_p->current_cell_id, cn_cell_info_p->current_ac, cn_cell_info_p->mcc_mnc));

                // Determine whether or not Extended Cell ID is valid
                switch (cn_cell_info_p->rat) {
                case CN_CELL_RAT_WCDMA:
                    {
                        SmMngr_CN_CellInfo.use_extended_cid = TRUE;
                    }
                    break;
                default:
                    {
                        SmMngr_CN_CellInfo.use_extended_cid = FALSE;
                    }
                    break;
                }

                // Convert MCC MNC string to 3 byte BCD
                if ((len = strlen((char *) cn_cell_info_p->mcc_mnc)) > 4) {
                    uint8_t nibble1 = 0;
                    uint8_t nibble2 = 0;
                    uint8_t nibble3 = 0;
                    uint8_t nibble4 = 0;
                    uint8_t nibble5 = 0;
                    uint8_t nibble6 = 0;

                    nibble1 = cn_cell_info_p->mcc_mnc[0] - '0';
                    nibble2 = cn_cell_info_p->mcc_mnc[1] - '0';
                    nibble3 = cn_cell_info_p->mcc_mnc[2] - '0';

                    switch (len) {
                    case 5:    /* MCC 3 digits, MNC 2 digits */
                        {
                            nibble4 = 0xF;
                            nibble5 = cn_cell_info_p->mcc_mnc[3] - '0';
                            nibble6 = cn_cell_info_p->mcc_mnc[4] - '0';
                        }
                        break;
                    case 6:    /* MCC 3 digits, MNC 3 digits */
                        {
                            nibble4 = cn_cell_info_p->mcc_mnc[5] - '0';
                            nibble5 = cn_cell_info_p->mcc_mnc[3] - '0';
                            nibble6 = cn_cell_info_p->mcc_mnc[4] - '0';
                        }
                        break;
                    }

                    SmMngr_CN_CellInfo.mcc_mnc[0] = (nibble2 << 4) | nibble1;
                    SmMngr_CN_CellInfo.mcc_mnc[1] = (nibble4 << 4) | nibble3;
                    SmMngr_CN_CellInfo.mcc_mnc[2] = (nibble6 << 4) | nibble5;

                    SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_HandleCallNetworkChanges MCC MNC nibbles: %X%X%X%X%X%X", nibble1, nibble2, nibble3, nibble4, nibble5, nibble6));
                    SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_HandleCallNetworkChanges MCC MNC output: byte1: %X, byte2: %X, byte3: %X ",
                            SmMngr_CN_CellInfo.mcc_mnc[0], SmMngr_CN_CellInfo.mcc_mnc[1], SmMngr_CN_CellInfo.mcc_mnc[2]));

                    // Call Network registration info decoded without error
                    SmMngr_CN_CellInfo.data_info = SMS_CN_REG_DATA_PRESENT;
                } else {
                    SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_HandleCallNetworkChanges MCC MNC length %d", len));
                }
            } else {
                SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_HandleCallNetworkChanges payload_size %d, expected %d", msg_p->payload_size, sizeof(cn_registration_info_t)));
            }
        } else {
            SMS_C_(SMS_LOG_V("smsmain.c: MessagingSystem_HandleCallNetworkChanges cn message type %d, error_code %d", msg_p->type, msg_p->error_code));
        }
    } else if (result == CN_FAILURE) {
        SMS_A_(SMS_LOG_W("%s: disconnect by CallNetwork server %d", __func__, result));

        // The socket peer (CallNetwork server) has closed its end of the socket.
        // Clean up at this end and let the main loop connect again.
        MessagingSystem_disconnect_call_network(&MessagingSystem.cn_context_p);
        MessagingSystem.CN_Client_fd = SMS_NO_PROCESS;

    } else {
        SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_HandleCallNetworkChanges result %d", result));
    }

    if (unread != 0) {
        SMS_B_(SMS_LOG_W("smsmain.c: MessagingSystem_HandleCallNetworkChanges %d unexpected bytes left in receive buffer", unread));
    }

    if (msg_p) {
        free(msg_p);
    }
    // Always write CN registration to Short Message Manager to trigger response to client
    ShortMessageManager_CN_CellInfoSet(&SmMngr_CN_CellInfo);
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_HandleFdChanges
 *
 * @description  Checks to see if a signal is on the specified socket,
 *               if so the signal will be received from the socket
 *
 * @param
 *
 * @return       void
 */
/********************************************************************/
static void MessagingSystem_HandleFdChanges(
    fd_set * const pReadFDs,
    fd_set * const pWriteFDs)
{
    SMS_IDENTIFIER_NOT_USED(pWriteFDs);
    if (MessagingSystem.SMS_Modem_fd == SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_E("smsmain.c: MessagingSystem_HandleFdChanges() MAL fd not initialised"));
    } else if (FD_ISSET(MessagingSystem.SMS_Modem_fd, pReadFDs)) {

        // A "signal" has been detected on the SMS_Modem_fd file descriptor. The signal is
        // handled here rather than returned by the ipc_server_context_await_signal() function.
        // This is because the MFL modem uses its own set of functions / macros which are not
        // fully compatible with the ipc_server code, and the MAL modem uses a callback mechanism
        // to handle the signal data.
        // In both cases we cannot, and do not, return signal data when
        // ipc_server_context_await_signal() returns.

        // Clear the FD flag so that there is no attempt to handle the "signal" elsewhere.
        FD_CLR(MessagingSystem.SMS_Modem_fd, pReadFDs);
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_HandleFdChanges() READ activity on SMS_Modem_fd:0x%08x", MessagingSystem.SMS_Modem_fd));
#ifdef SMS_USE_MFL_MODEM
        // Get MFL Modem Signal Data
        MessagingSystem.MFL_Modem_SignalData_p = mfl_session_get_signal(MessagingSystem_GetModemSessionData());

#else                           // SMS_USE_MFL_MODEM
        // Handle MAL modem callbacks to get data.
        mal_sms_cbs_response_handler();
#endif
    }
    else if (FD_ISSET(MessagingSystem.SMS_Netlnk_fd, pReadFDs)) {
        int netlnk_msg = -1;
        // A "signal" has been detected on the SMS_Netlnk_fd file descriptor. The signal is
        // handled here rather than returned by the ipc_server_context_await_signal() function.

        // Clear the FD flag so that there is no attempt to handle the "signal" elsewhere.
        FD_CLR(MessagingSystem.SMS_Netlnk_fd, pReadFDs);
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_HandleFdChanges() for SMS_Netlnk_fd READ activity on SOCKET:0x%08x", MessagingSystem.SMS_Netlnk_fd));
        netlnk_socket_recv(MessagingSystem.SMS_Netlnk_fd, &netlnk_msg);
        if (netlnk_msg == MODEM_RESET_IND) {
            MessagingSystem.ModemResetStatus = TRUE;
            SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_HandleFdChanges()SET modem reset status:0x%08x", MessagingSystem.ModemResetStatus));
        }
    }
    else if (FD_ISSET(MessagingSystem.CN_Client_fd, pReadFDs)) {
        // Clear the FD flag so that there is no attempt to handle the "signal" elsewhere.
        FD_CLR(MessagingSystem.CN_Client_fd, pReadFDs);
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_HandleFdChanges() READ activity on CN_Client_fd:0x%08x", MessagingSystem.CN_Client_fd));

        // Handle Call Network Client callback to get data.
        MessagingSystem_HandleCallNetworkChanges();
    }

    else if (FD_ISSET(MessagingSystem.sms_log_fd, pReadFDs)) {
        // Clear the FD flag so that there is no attempt to handle the "signal" elsewhere.
        FD_CLR(MessagingSystem.sms_log_fd, pReadFDs);
        SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_HandleFdChanges() READ activity on sms_log_fd:0x%08x", MessagingSystem.sms_log_fd));

        // Handle log file changes
        sms_log_select_callback(MessagingSystem.sms_log_fd,NULL);
    }

    if (MessagingSystem.sim_context != NULL && MessagingSystem.sim_context->connected) {
        int status = MessagingSystem_HandleSIMFdChanges(pReadFDs, MessagingSystem.sim_context->sim_data.sim_async);
        if (status < 0) {
            // SIM server has disconnected. Disconnect this end also.
            // Main loop will reconnect.
            MessagingSystem_SetState(MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART);
            MessagingSystem_disconnect_sim_context(MessagingSystem.sim_context);
            // Sleep short while to let SIM server recover its composure
            sleep(2);
        }
    }
}


/**********************************************************************
* CleanUp - This is called by the IPC-lib when a client context has disconnected
*           (= client context socket closed), for example if the application process
*           that was communicating with your demon crashed or closed down, the IPC-lib
*                       will detect this and invoke this call-back. Then you can free up the
*                       resources associated with this communication and have it ready to accept
*                       a new connection.
***********************************************************************/
static void CleanUp(
    const int EventSocket)
{                               // not implemented for this example
    SMS_IDENTIFIER_NOT_USED(EventSocket);
    SMS_B_(SMS_LOG_D("smsmain.c: CleanUp() called"));
}


#ifndef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     MessagingSystem_HandleMalCallbackData
 *
 * @description  Handler for queued MAL Callback data objects which are
 *               held in a list.
 *
 * @param  in    FreeListOnly: TRUE if we are only ensuring that the
 *               that the list is empty.
 *
 * @return       static void
 */
/********************************************************************/
static void MessagingSystem_HandleMalCallbackData(
    const uint8_t FreeListOnly)
{
    SMS_C_(uint32_t Count = 1);
    SMS_MAL_Callback_Data_t *MalCallbackData_p = MSG_ListGetFirst(MessagingSystem.MAL_Callback_Data_p);

    while (MalCallbackData_p != NULL) {
        if (!FreeListOnly) {
            SMS_C_(SMS_LOG_V("smsmain.c: MessagingSystem_HandleMalCallbackData: Loopcount %d, Callback type %d.", Count++, MalCallbackData_p->CallbackDataType));

            ShortMessageBearer_HandleMalCallbackData(MalCallbackData_p);
        }
        // Remove from list, free it, and get the next
        (void) MSG_ListRemove(MessagingSystem.MAL_Callback_Data_p, MalCallbackData_p);
        SMS_HEAP_FREE(&MalCallbackData_p);
        MalCallbackData_p = MSG_ListGetFirst(MessagingSystem.MAL_Callback_Data_p);
    }
}
#endif

/********************************************************************/
/**
 *
 * @function     MessagingSystem_Main
 *
 * @description  Main loop of SMS Server thread
 *
 * @param        threadid : The thread id.
 *
 * @return       static void
 */
/********************************************************************/
static void *MessagingSystem_Main(
    void *threadid)
{
    struct ipc_server_context_t *SMS_Server_IPC_ServerContext_p = NULL;
    union SMS_SIGNAL *RecBuf_p = NULL;

#ifdef SMS_USE_MFL_MODEM
    uint8_t MFL_SignalData = FALSE;     // Used to indicate an MFL modem signal is being processed.
#endif                          /*  */

    SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_Main"));

#ifndef SMS_USE_MFL_MODEM
#ifndef SMS_MAL_CALLBACK_LIST_CREATE_FAILURE_TIMER
#define SMS_MAL_CALLBACK_LIST_CREATE_FAILURE_TIMER 5
#endif
    // Initialise MAL Callback data handler.
    while (MessagingSystem.MAL_Callback_Data_p == NULL) {
        MessagingSystem.MAL_Callback_Data_p = MSG_ListCreate();
        if (MessagingSystem.MAL_Callback_Data_p == NULL) {
            SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_Main MessagingSystem.MAL_Callback_Data_p is NULL. Initialisation failed"));
            sleep(SMS_MAL_CALLBACK_LIST_CREATE_FAILURE_TIMER);  // Wait and then try again
        }
    }
#endif

    //Initialise Short Message Bearer object
    SMS_A_(SMS_LOG_I("smsmain.c: ShortMessageBearer_Initialise"));
    ShortMessageBearer_Initialise(&MessagingSystem.SMS_Modem_fd
#ifndef SMS_USE_MFL_MODEM
                                  , &MessagingSystem.SMS_Netlnk_fd
#endif
        );

    // Create SIM server contexts
    MessagingSystem.sim_context = MessagingSystem_create_sim_context();
    if (MessagingSystem.sim_context == NULL) {
        SMS_A_(fprintf(stdout, "\nSMS: smsmain.c: ERROR: SMS Server SIM context create FAILED!"));
        goto error_start_up;
    }
    SMS_A_(SMS_LOG_I("smsmain.c: SMS Server SIM context created"));

    // create the internal pipe,
    // create the ipc server and
    // register to timer server
    SMS_Server_IPC_ServerContext_p = ipc_server_context_create((int) threadid, SMS_SERVER_SOCK_PATH, MessagingSystem_BuildFds, MessagingSystem_HandleFdChanges, CleanUp);
    if (SMS_Server_IPC_ServerContext_p == NULL) {
        SMS_A_(SMS_LOG_E("smsmain.c: ERROR: SMS Server IPC context create FAILED!"));
        goto error_start_up;
    } else {
        SMS_A_(SMS_LOG_I("smsmain.c: SMS Server IPC context created"));
#ifdef SMS_SIM_TEST_PLATFORM
        internal_pipe_write_fd = ipc_server_context_get_internal_pipe_write_fd(SMS_Server_IPC_ServerContext_p);
#endif                          // SMS_SIM_TEST_PLATFORM
        // Register with the timer server to get timeouts.
        timer_server_user_register(ipc_server_context_get_internal_pipe_write_fd(SMS_Server_IPC_ServerContext_p));
        SMS_A_(SMS_LOG_I("smsmain.c: SMS Server IPC timer registered"));
        // Initialise the CB Server
        CBS_Manager_Init();

        // Signal that startup sequence is complete
        start_up_sequence_complete();

        // Main loop
        while (TRUE) {

            // Initialise SIM communication
            if (MessagingSystem.sim_context->connected != TRUE) {
                ste_uicc_get_sim_state_response_t sim_state = {0, SIM_STATE_UNKNOWN};
                ste_sim_t *sim_p = MessagingSystem_UICC_SimSyncGet();
                int status;

                // Connect (the call will block until connection is achieved)
                SMS_A_(SMS_LOG_I("smsmain.c: Connect to SIM server ..."));
                MessagingSystem.sim_evt_state = SMS_SIM_STATE_READY_WAITING;
                MessagingSystem_connect_sim_context(MessagingSystem.sim_context);

                // Get current SIM state
                SMS_A_(SMS_LOG_I("smsmain.c: Get SIM state ..."));
                status = ste_uicc_sim_get_state_sync(sim_p, (uintptr_t)sim_p, &sim_state);
                if (status == STE_SIM_SUCCESS) {
                    if (sim_state.state == SIM_STATE_READY) {
                        SMS_A_(SMS_LOG_I("%s SIM_STATE_READY", __func__));
                        MessagingSystem.sim_evt_state = SMS_SIM_STATE_READY_RCVD;
                    } else {
                        SMS_A_(SMS_LOG_I("%s sim_state:%d", __func__, sim_state.state));
                    }
                }
            }

            if (MessagingSystem.sim_evt_state == SMS_SIM_STATE_READY_RCVD) {
                SMS_A_(SMS_LOG_I("MessagingSystem_Main -> MessagingSystem_Initialise()"));
                MessagingSystem_Initialise();
                MessagingSystem.sim_evt_state = SMS_SIM_STATE_READY_NOT_WAITING;
            }

            // Initialise Call & Network communication
            if (MessagingSystem.cn_context_p == NULL) {

                // Connect (the call will block until connection is achieved)
                SMS_A_(SMS_LOG_I("smsmain.c: Connect to Call&Network server ..."));
                MessagingSystem_connect_call_network(&MessagingSystem.cn_context_p,
                                                     &MessagingSystem.CN_Client_fd);
            }

            // Check for Call Network registration info which indicates
            // that MO SMS Control by USIM is in progress
            if (ShortMessageManager_CN_CellInfoAvailable()) {
                // Format and send envelope to USIM
                ShortMessageManager_SMS_ControlSendEnvelope();
            }

            // Check for MO SMS Control by USIM envelope response
            if (MessagingSystem.mo_sms_control_ec_response.apdu.buf != NULL) {
                // Process CAT MO SMS Control
                ShortMessageManager_SMS_ControlHandleResponse(&MessagingSystem.mo_sms_control_ec_response);
            }
#ifndef SMS_USE_MFL_MODEM
            // Handle any callbacks from the MAL which have added entries to MAL_Callback_Data_p list.
            MessagingSystem_HandleMalCallbackData(FALSE);
#endif

            // Handle any delivery reports for CAT SMS Data downloads.
            if (SIMTKT_GetEnvelopeStatus()) {
                // If we have previously received an SMS which was downloaded to the CAT, we will receive a callback from the UICC CAT Server.
                // This CAT callback will have setup data which is used to create a suitable deliver report which we now need to send.
                SMS_A_(SMS_LOG_I("MessagingSystem_Main -> build SMSTK delivery report"));
                SIMTKT_HandleEvent(SIMTKT_BUILD_DELIVERY_REPORT, NULL);
            }
#ifdef SMS_USE_MFL_MODEM
            MFL_SignalData = FALSE;
            if (MessagingSystem.MFL_Modem_SignalData_p != NULL) {

                // Previously received signal data from MFL modem exists, so use it.
                RecBuf_p = MessagingSystem.MFL_Modem_SignalData_p;
                MessagingSystem.MFL_Modem_SignalData_p = NULL;
                MFL_SignalData = TRUE;
            }

            else
#endif
            {
                // Get next signal from queue.  (Note that this will be NULL if the signal is from
                // the MFL modem and there is no other signal waiting in the queue.)
                RecBuf_p = ipc_server_context_await_signal(SMS_Server_IPC_ServerContext_p);
            }
#ifndef SMS_USE_MFL_MODEM
            if (MessagingSystem.ModemResetStatus) {
                SMS_A_(SMS_LOG_I("smsmain.c: Received modem reset indication!, MessagingSystem_Main() SHUTDOWN "));
                break;
            }
#endif                          /*SMS_USE_MFL_MODEM */
            if (RecBuf_p != NULL) {
                uint32_t MFL_Primitive = SMS_SIGNAL_PRIMITIVE_INVALID;

#ifdef SMS_USE_MFL_MODEM
                // If we have a signal from the MFL modem, we have to use the matching set of
                // signal handling macros.
                if (MFL_SignalData) {

                    // Signals from the MFL modem take 1, of 2, forms:
                    //   1) Asynchronous events, e.g. MT SMS received.
                    //   2) Response signals for Request functions which have been called in
                    //      NO_WAIT mode.
                    // In both these circumstances, it is not necessary to send anything back
                    // to the process from which the signal has come from. Therefore, we do not
                    // call the SENDER() macro or get the client tag information.
                    MFL_Primitive = ((union SIGNAL *) RecBuf_p)->Primitive;     // MFL signals are of type "union SIGNAL*"
                }

                else
#endif
                {

                    // Store Sender information.
                    (void) Do_SMS_ClientTag_Get(RecBuf_p, &MessagingSystem.SenderSignalInfo.ClientTag);
                    MessagingSystem.SenderSignalInfo.ProcessId = SMS_SENDER(&RecBuf_p);
                }
                // check if this is shutdown
                if (MFL_Primitive == SMS_SIGNAL_PRIMITIVE_INVALID) {
                    if (REQUEST_SMS_CB_SHUTDOWN == RecBuf_p->Primitive) {
                        SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_Main() SHUTDOWN "));
                        break;
                    }
                }
                if (MessagingSystem_SMS_SignalProcessed(&RecBuf_p, MFL_Primitive)) {

                    // Everything ok.
                    ;           // Empty statement.
                } else {
                    SMS_A_(SMS_LOG_I("smsmain.c: Received unknown primitive %d", (unsigned int) RecBuf_p->Primitive));
                    // Free signal
#ifdef SMS_USE_MFL_MODEM
                    if (MFL_SignalData) {

                        // Use MFL modem signal macros
                        SIGNAL_FREE((void **) &RecBuf_p);
                    } else
#endif
                    {
                        SMS_SIGNAL_FREE(&RecBuf_p);
                    }
                }
            } else {
                SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_Main(): RecBuf_p NULL"));
            }
        } //while (TRUE)

        MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_SHUTDOWN_STATE;

        if (RecBuf_p) {

            // Free signal
#ifdef SMS_USE_MFL_MODEM
            if (MFL_SignalData) {
                // Use MFL modem signal macros
                SIGNAL_FREE((void **) &RecBuf_p);
            } else
#endif
            {
                SMS_SIGNAL_FREE(&RecBuf_p);
            }
        }

        MessagingSystem_Deinitialise();

#ifndef SMS_USE_MFL_MODEM
        mal_sms_cbs_deinit();
#endif
        timer_server_user_deregister();
        ipc_server_context_destroy(SMS_Server_IPC_ServerContext_p);

        MessagingSystem_destroy_sim_context(MessagingSystem.sim_context);
        MessagingSystem.sim_context = NULL;

        MessagingSystem_disconnect_call_network(&MessagingSystem.cn_context_p);
        MessagingSystem.CN_Client_fd = SMS_NO_PROCESS;

#ifndef REMOVE_SMSTRG_LAYER
       SmStorage_Clear_FS_Working_Dir();
#endif                          // REMOVE_SMSTRG_LAYER

#ifndef SMS_USE_MFL_MODEM
        // Ensure that the MAL_Callback_Data_p list is empty.
        MessagingSystem_HandleMalCallbackData(TRUE);
        // Destroy MAL_Callback_Data_p list.
        MSG_ListDestroy(MessagingSystem.MAL_Callback_Data_p);
#endif
    }
    return NULL;

error_start_up:
    start_up_sequence_complete();
    return NULL;
}


/********************************************************************/
/**
 *
 * @function     SignalValidForState
 *
 * @description  Determine if the given signal should be handled in
 *               the given messaging state.
 *
 * @param        SMS_SIGSELECT: Primitive to check against
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SignalValidForState(
    SMS_SIGSELECT Primitive)
{
    uint8_t IsValidSignal = FALSE;
    uint8_t matchFound = FALSE;
    uint8_t index;
    const MessagingSystem_ValidSignalsForState_t *TablePtr = ValidSignalsForStatesTable;

    // Find the corect signal array for internal state
    while (TablePtr->MessagingSystemState != MESSAGING_SYSTEM_LAST_VALID_STATE && matchFound == FALSE) {

        //if the event matches and
        if (TablePtr->MessagingSystemState == MessagingSystem.MessagingSystemState) {
            matchFound = TRUE;
        }
        //that entry was not a match
        else {

            //move to next entry in Event Handler table
            TablePtr++;
        }
    }
    if (matchFound) {

        // Determine if all signals are valid for the given state
        if (TablePtr->ValidSignals[0] == 0) {
            IsValidSignal = TRUE;
        }

        else {

            // Search signal table associated with given state to determine if the signal should be serviced
            for (index = 1; index <= TablePtr->ValidSignals[0]; index++) {
                if (Primitive == TablePtr->ValidSignals[index]) {
                    IsValidSignal = TRUE;
                    break;
                }
            }
        }
    }

    else {

        //Match wasn't found in table
        IsValidSignal = FALSE;
    }
    return IsValidSignal;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_SetState
 *
 * @description  This function sets the messaging system state that dictates
 *               the signals to recieve. If changing state to NewState is
 *               an invalid state change the currnt state is changed
 *               to MESSAGING_SYSTEM_IDLE.
 *
 * @param        NewState : The new state to be set.
 *
 * @return       void
 */
/********************************************************************/
void MessagingSystem_SetState(
    const MessagingSystemState_t NewState)
{
    SMS_A_(SMS_LOG_I("smsmain.c: State %d", MessagingSystem.MessagingSystemState));
    // translate the external to internal state
    switch (NewState) {
    case MESSAGING_SYSTEM_WAIT_SIM_READY:
        switch (MessagingSystem.MessagingSystemState) {
        case MESSAGING_SYSTEM_LAST_VALID_STATE:
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_WAIT_SIM_READY;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_BUSY);
            break;
        default:
            SMS_A_(SMS_LOG_I("smsmain.c: Error invalid state %d defaulting to MESSAGING_SYSTEM_IDLE ", NewState));
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
            break;
        }
        break;
    case MESSAGING_SYSTEM_VALIDATE_SM_STORAGE:
        switch (MessagingSystem.MessagingSystemState) {
        case MESSAGING_SYSTEM_IDLE:
        case MESSAGING_SYSTEM_WAIT_SIM_READY:
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_VALIDATE_SM_STORAGE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_BUSY);
            break;
        default:
            SMS_A_(SMS_LOG_I("smsmain.c: Error invalid state %d defaulting to MESSAGING_SYSTEM_IDLE", NewState));
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
            break;
        }
        break;
    case MESSAGING_SYSTEM_IDLE:
        switch (MessagingSystem.MessagingSystemState) {
        case MESSAGING_SYSTEM_IDLE:
            break;
        case MESSAGING_SYSTEM_VALIDATE_SM_STORAGE:
        case MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SAT_REFRESH:
        case MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART:
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
            break;
        default:
            SMS_A_(SMS_LOG_I("smsmain.c: Error invalid state %d defaulting to MESSAGING_SYSTEM_IDLE", NewState));
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
            break;
        }
        break;
    case MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SAT_REFRESH:
        switch (MessagingSystem.MessagingSystemState) {
        case MESSAGING_SYSTEM_VALIDATE_SM_STORAGE:
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SAT_REFRESH;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_BUSY);
            break;
        default:
            SMS_A_(SMS_LOG_I("smsmain.c: Error invalid state %d defaulting to MESSAGING_SYSTEM_IDLE", NewState));
            MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
            break;
        }
        break;
    case MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART:
        // SIM can restart just about any time
        MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_WAIT_SIM_READY_AFTER_SIM_RESTART;
        Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_BUSY);
        break;
    default:
        SMS_A_(SMS_LOG_I("smsmain.c: Error invalid new state %d defaulting to MESSAGING_SYSTEM_IDLE", NewState));
        MessagingSystem.MessagingSystemState = MESSAGING_SYSTEM_IDLE;
        Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
        break;
    }
    SMS_A_(SMS_LOG_I(" changed to state %d", MessagingSystem.MessagingSystemState));
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_GetState
 *
 * @description  This function return the current state - used for intrusive
 * testing by the test harness
 *
 * @param        State : The state returned
 *
 * @return       void
 */
/********************************************************************/
void MessagingSystem_GetState(
    MessagingSystemState_t * State)
{
    *State = MessagingSystem.MessagingSystemState;
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_Get_CN_Context
 *
 * @description  Function to get the Call Network context.
 *
 * @param        void
 *
 * @return       cn_context_t
 */
/********************************************************************/
cn_context_t *MessagingSystem_Get_CN_Context(
    void)
{
    return MessagingSystem.cn_context_p;
}



#ifdef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     MessagingSystem_SetModemSessionData
 *
 * @description  Function to set the MFL Modem Session Data.
 *
 * @param        MFL_Modem_Session_p: Session Data
 *
 * @return       void
 */
/********************************************************************/
void MessagingSystem_SetModemSessionData(
    modem_session_t * const MFL_Modem_Session_p)
{
    SMS_B_(SMS_LOG_D("smsmain.c: MessagingSystem_SetModemSessionData: %sNULL", MFL_Modem_Session_p != NULL ? "NOT " : ""));
    MessagingSystem.MFL_Modem_Session_p = MFL_Modem_Session_p;
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_SetModemSessionData
 *
 * @description  Function to get the MFL Modem Session Data.
 *
 * @param        void
 *
 * @return       Pointer to Session Data
 */
/********************************************************************/
modem_session_t *MessagingSystem_GetModemSessionData(
    void)
{
    return MessagingSystem.MFL_Modem_Session_p;
}
#endif                          // SMS_USE_MFL_MODEM

#ifndef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     MessagingSystem_MAL_Callback_AddData
 *
 * @description  Function to add the supplied callback data to the list
 *               of MAL callback data.
 *
 * @param        SMS_MAL_Callback_Data_p
 *
 * @return       static void
 */
/********************************************************************/
void MessagingSystem_MAL_Callback_AddData(
    const SMS_MAL_Callback_Data_t * const SMS_MAL_Callback_Data_p)
{
    (void) MSG_ListInsertLast(MessagingSystem.MAL_Callback_Data_p, SMS_MAL_Callback_Data_p);
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_Set_MAL_ConfigurationState
 *
 * @description  Function to get the MAL configuration state.
 *
 * @param        SMS_MAL_ConfigState
 *
 * @return       void
 */
/********************************************************************/
void MessagingSystem_Set_MAL_ConfigurationState(
    const uint8_t SMS_MAL_ConfigState)
{
    MessagingSystem.ConfigureMAL = SMS_MAL_ConfigState;
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_Get_MAL_ConfigurationState
 *
 * @description  Function to set the MAL configuration state.
 *
 * @param        void
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t MessagingSystem_Get_MAL_ConfigurationState(
    void)
{
    return MessagingSystem.ConfigureMAL;
}
#endif

/********************************************************************/
/**
 *
 * @function     MessagingSystem_Initialise
 *
 * @description  Function for messaging system to initialise itself
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void MessagingSystem_Initialise(
    void)
{
    // Initialise Routing Rules Table
    SMS_A_(SMS_LOG_I("smsmain.c: SMS_InitialiseShortMessageRoutingRules"));
    (void) SMS_InitialiseShortMessageRoutingRules();

#ifndef REMOVE_SMSTRG_LAYER
    SMS_A_(SMS_LOG_I("smsmain.c: SmStorage_Initialise"));
    SmStorage_Initialise();
#endif                          // REMOVE_SMSTRG_LAYER

    // Initialise Short Message Object
    SMS_A_(SMS_LOG_I("smsmain.c: ShortMessage_Initialise"));
    ShortMessage_Initialise();

    // Initialise Short Message Bearer Memory Capacity Available state
    SMS_A_(SMS_LOG_I("smsmain.c: ShortMessageBearer_MemCapacityInitialise"));
    ShortMessageBearer_MemCapacityInitialise();

    // Initialise CBS subscriptions from EFcbmid file on SIM card.
    SMS_A_(SMS_LOG_I("smsmain.c: CBS_Manager_Init_CBMID"));
    CBS_Manager_Init_CBMID();

    // Allow SMS process to manage the retrieval of EfSms Record(s) from Pd
    MessagingSystem_SetState(MESSAGING_SYSTEM_VALIDATE_SM_STORAGE);

#ifdef REMOVE_SMSTRG_LAYER
    // We are not using storage, so set State to IDLE as this is the end
    // of the initialisation process. (Note that we still need the call to
    // set state to MESSAGING_SYSTEM_VALIDATE_SM_STORAGE, above, so that
    // we transition the state machine through the expected states.)
    MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
#endif
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_Deinitialise
 *
 * @description  Function for messaging system to deinitialise itself
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void MessagingSystem_Deinitialise(
    void)
{
#ifndef REMOVE_SMSTRG_LAYER
    SMS_A_(SMS_LOG_I("smsmain.c: SmStorage_Deinitialise"));
    SmStorage_Deinitialise();
#endif                          // REMOVE_SMSTRG_LAYER

    SMS_A_(SMS_LOG_I("smsmain.c: ShortMessage_Deinitialise"));
    ShortMessage_Deinitialise();

    SMS_A_(SMS_LOG_I("smsmain.c: ShortMessageRouter_Deinitialise"));
    ShortMessageRouter_Deinitialise();
}

/********************************************************************/
/**
 *
 * @function     MessagingSystem_EventsSubscription
 *
 * @description  Add an SMS events subscription
 *
 * @param        EventSocket   : Socket to send unsolicited events.
 * @param        Subscribe     : TRUE if the client is subscibing, FALSE if unsubscribing.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t MessagingSystem_EventsSubscription(
    const int EventSocket,
    const uint8_t Subscribe)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    int Result;                 // SMS_req_type
    int SearchSocket = SMS_NO_PROCESS;
    int SubscribeSocket = SMS_NO_PROCESS;
    int SocketIndex;
    SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_EventsSubscription EventSocket = 0x%08x, Subscribe = %d", EventSocket, Subscribe));
    if (Subscribe) {

        // Initialise to replace a default socket with the subscription socket
        SubscribeSocket = EventSocket;

#ifdef SMS_USE_MFL_MODEM
        IDENTIFIER_NOT_USED(Result);

#else
        // If this is the 1st client subscription the MAL must be configured
        if (MessagingSystem.ConfigureMAL) {
            SMS_B_(SMS_LOG_D("smsmain.c: call mal_sms_cbs_config()"));
            if (MAL_SMS_CBS_SUCCESS != (Result = mal_sms_cbs_config())) {
                SMS_A_(SMS_LOG_E("smsmain.c: mal_sms_cbs_config ErrorCause: %d ", Result));
            }
        }
#endif                          // SMS_USE_MFL_MODEM
    }

    else {

        // Initialise to replace the subscription socket with a default socket
        SearchSocket = EventSocket;
    }
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_SubscriberSocket[SocketIndex] == SearchSocket) {
            SMS_SubscriberSocket[SocketIndex] = SubscribeSocket;
            MSG_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            break;
        }
    }
    return (MSG_ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     MessagingSystem_ShortMessageEventsSubscription
 *
 * @description  Add an SMS events subscription
 *
 * @param        EventSocket   : Socket to send unsolicited events.
 * @param        Subscribe     : TRUE if the client is subscibing, FALSE if unsubscribing.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t MessagingSystem_ShortMessageEventsSubscription(
    const int EventSocket,
    const uint8_t Subscribe)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    int SearchSocket = SMS_NO_PROCESS;
    int SubscribeSocket = SMS_NO_PROCESS;
    int SocketIndex;
    SMS_A_(SMS_LOG_I("smsmain.c: MessagingSystem_ShortMessageEventsSubscription EventSocket = 0x%08x, Subscribe = %d", EventSocket, Subscribe));
    if (Subscribe) {

        // Initialise to replace a default socket with the subscription socket
        SubscribeSocket = EventSocket;
    }

    else {

        // Initialise to replace the subscription socket with a default socket
        SearchSocket = EventSocket;
    }
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_ShortMessageSubscriberSocket[SocketIndex] == SearchSocket) {
            SMS_ShortMessageSubscriberSocket[SocketIndex] = SubscribeSocket;
            MSG_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            break;
        }
    }
    return (MSG_ErrorCode);
}


/*
**========================================================================
** Short Message Layer Dispatch function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Dispatch_SMS_ServerStatusChanged
 *
 * @description  The MTS has completed initialisation.
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void Dispatch_SMS_ServerStatusChanged(
    const SMS_ServerStatus_t ServerStatus)
{
    int SocketIndex;
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_SubscriberSocket[SocketIndex] != SMS_NO_PROCESS) {
            Event_SMS_ServerStatusChanged_t *Event_p = SMS_SIGNAL_ALLOC(Event_SMS_ServerStatusChanged_t,
                                                                        EVENT_SMS_SERVERSTATUSCHANGED);
            if (Event_p != SMS_NIL)     // Check for memory allocation failure
            {
                SMS_C_(SMS_LOG_V("smsmain.c: Dispatch_SMS_ServerStatusChanged: Dispatched %d to 0x%08X.", ServerStatus, SMS_SubscriberSocket[SocketIndex]));
                Event_p->ServerStatus = ServerStatus;
                SMS_SEND(&Event_p, SMS_SubscriberSocket[SocketIndex]);
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     Dispatch_SMS_ErrorOccurred
 *
 * @description  Response Function
 *
 * @param        Error :
 *
 * @return       void
 */
/********************************************************************/
void Dispatch_SMS_ErrorOccurred(
    const SMS_Error_t Error)
{
    int SocketIndex;
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_SubscriberSocket[SocketIndex] != SMS_NO_PROCESS) {
            Event_SMS_ErrorOccurred_t *Event_p = SMS_SIGNAL_ALLOC(Event_SMS_ErrorOccurred_t,
                                                                  EVENT_SMS_ERROROCCURRED);
            if (Event_p != SMS_NIL)     // Check for memory allocation failure
            {
                Event_p->ErrorCode = Error;
                SMS_A_(SMS_LOG_E("smsmain.c: Dispatch_SMS_ErrorOccurred to socket: 0x%08x", SMS_SubscriberSocket[SocketIndex]));
                SMS_SEND(&Event_p, SMS_SubscriberSocket[SocketIndex]);
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     Dispatch_SMS_ShortMessageReceived
 *
 * @description  A Short Message has arrived
 *
 * @param        ShortMessageData_p : SMS TPDU and storage slot if applicable
 *
 * @return       void
 */
/********************************************************************/
void Dispatch_SMS_ShortMessageReceived(
    const SMS_ShortMessageReceived_t * const ShortMessageData_p)
{
    int SocketIndex;
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_ShortMessageSubscriberSocket[SocketIndex] != SMS_NO_PROCESS) {
            Event_SMS_ShortMessageReceived_t *Event_p = SMS_SIGNAL_ALLOC(Event_SMS_ShortMessageReceived_t,
                                                                         EVENT_SMS_SHORTMESSAGERECEIVED);
            if (Event_p != SMS_NIL)     // Check for memory allocation failure
            {
                Event_p->Data.Category = ShortMessageData_p->Category;
                Event_p->Data.IsReplaceType = ShortMessageData_p->IsReplaceType;
                memcpy(&Event_p->Data.SMSC_Address_TPDU, &ShortMessageData_p->SMSC_Address_TPDU, sizeof(SMS_SMSC_Address_TPDU_t));
                memcpy(&Event_p->Data.Slot, &ShortMessageData_p->Slot, sizeof(SMS_Slot_t));
                SMS_A_(SMS_LOG_I("smsmain.c: Dispatch_SMS_ShortMessageReceived to socket: 0x%08x", SMS_ShortMessageSubscriberSocket[SocketIndex]));
                SMS_SEND(&Event_p, SMS_ShortMessageSubscriberSocket[SocketIndex]);
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     Dispatch_SMS_StatusReportReceived
 *
 * @description  A Status Report has arrived
 *
 * @param        Slot      : The slot number of the SMS that the status report relates to
 * @param        StatusReport_p : SMSC address and Status Report TPDU
 *
 * @return       void
 */
/********************************************************************/
void Dispatch_SMS_StatusReportReceived(
    const SMS_Slot_t Slot,
    const SMS_SMSC_Address_TPDU_t * const StatusReport_p)
{
    int SocketIndex;
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_ShortMessageSubscriberSocket[SocketIndex] != SMS_NO_PROCESS) {
            Event_SMS_StatusReportReceived_t *Event_p = SMS_SIGNAL_ALLOC(Event_SMS_StatusReportReceived_t,
                                                                         EVENT_SMS_STATUSREPORTRECEIVED);
            if (Event_p != SMS_NIL)     // Check for memory allocation failure
            {
                memcpy(&Event_p->TPDU_StatusReport, StatusReport_p, sizeof(SMS_SMSC_Address_TPDU_t));
                memcpy(&Event_p->SlotNumber, &Slot, sizeof(SMS_Slot_t));
                SMS_A_(SMS_LOG_I("smsmain.c: Dispatch_SMS_StatusReportReceived to socket: 0x%08x", SMS_ShortMessageSubscriberSocket[SocketIndex]));
                SMS_SEND(&Event_p, SMS_ShortMessageSubscriberSocket[SocketIndex]);
            }
        }
    }
}


/*
**========================================================================
** Short Message Storage Layer Dispatch function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Dispatch_SMS_StorageStatusChanged
 *
 * @description  The State of the Short Message Store
 *
 * @param        StorageStatus :
 *
 * @return       void
 */
/********************************************************************/
void Dispatch_SMS_StorageStatusChanged(
    const SMS_StorageStatus_t StorageStatus)
{
    int SocketIndex;
    for (SocketIndex = 0; SocketIndex < MAX_SMS_SERVERSUBSCRIBERS; SocketIndex++) {
        if (SMS_SubscriberSocket[SocketIndex] != SMS_NO_PROCESS) {
            Event_SMS_StorageStatusChanged_t *Event_p = SMS_SIGNAL_ALLOC(Event_SMS_StorageStatusChanged_t,
                                                                         EVENT_SMS_STORAGESTATUSCHANGED);
            if (Event_p != SMS_NIL)     // Check for memory allocation failure
            {
                Event_p->StorageStatus = StorageStatus;
                SMS_C_(SMS_LOG_V("smsmain.c: Dispatch_SMS_StorageStatusChanged to socket: 0x%08x", SMS_SubscriberSocket[SocketIndex]));
                SMS_SEND(&Event_p, SMS_SubscriberSocket[SocketIndex]);
            }
        }
    }
}
