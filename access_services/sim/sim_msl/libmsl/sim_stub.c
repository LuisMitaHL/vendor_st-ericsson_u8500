/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_stub.c
 * Description     : Simulated modem implementation.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */


#include "sim.h"
#include "simd.h"
#include "sim_stub.h"
#include "msgq.h"
#include "cat_internal.h"
#include "sim_internal.h"
#include "cat_barrier.h"
#include "catd.h"
#include "uiccd.h"
#include "uiccd_msg.h"
#include "func_trace.h"
#include "catd_modem.h"
#include "apdu.h"

//header files for SMS stub functions
#include "r_sms.h"
#include "r_sms_cb_session.h"
//header files for COPS stub functions
#include "cops.h"
//header files for CN stub functions
#include "cn_client.h"
//header files for Netlnk functions
#include "shm_netlnk.h"
#include "util_security.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "sim_log.h"
#include <stdarg.h>
#include <math.h>


static char *log_type_string[] = {
    /* UTIL_LOG_TYPE_UNKNOWN */ "unknown",
    /* UTIL_LOG_TYPE_ERROR   */ "error",
    /* UTIL_LOG_TYPE_WARNING */ "warning",
    /* UTIL_LOG_TYPE_INFO    */ "info",
    /* UTIL_LOG_TYPE_DEBUG   */ "debug",
    /* UTIL_LOG_TYPE_VERBOSE */ "verbose"
};


#define MAX_SIZE_OF_FORMAT_STRING 1000
#define UNUSED(T,X) do{T unused_var=(X);(X)=unused_var;}while(0)

typedef struct sim_stub_msg_s sim_stub_msg_t;
typedef struct sim_stub_data_s sim_stub_data_t;
int thread_started = 0;
static ste_cat_status_words_t modem_response_status_word = {0x90, 0x00};

// Set at READY indication
static ste_sim_app_type_t current_app_type = STE_SIM_APP_TYPE_USIM;

/* Need to make use of sim_stub_response_type by enhancing the values for negative cases */
static int uicc_fail_tests_flag = 0;

// EF SMSP file data
#define  EF_SMSP_FILE_ID   0x6F42

#define EF_RAT_FILE_ID 0x4F36

// EF ECC file data
#define EF_FDN_FILE_ID 0x6F3B

#define EF_FDN_RECORD_SIZE 38
#define EF_FDN_FILE_SIZE_IN_RECORDS 10
#define EF_FDN_FILE_SIZE_IN_BYTES (EF_FDN_RECORD_SIZE*EF_FDN_FILE_SIZE_IN_RECORDS)
static const uint8_t ef_fdn[EF_FDN_FILE_SIZE_IN_RECORDS][EF_FDN_RECORD_SIZE] = {
  { // 1 Stefan
    0x53, 0x74, 0x65, 0x66, 0x61, 0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x91, 0x64, 0x07, 0x46, 0x66, 0x35, 0xf8,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 2 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  {  // 3 Long, with an ext2 record id at the end
    0x4c, 0x6f, 0x6e, 0x67, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0b, 0x81, 0x10, 0x32, 0x54, 0x76, 0x98, 0x10,
    0x32, 0x54, 0x76, 0x98, 0xff, 0x01
  },
  {// 4 Pott
    0x50, 0x6f, 0x74, 0x74, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0x81, 0x1a, 0x11, 0xfb, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  {// 5 Max
    0x4d, 0x61, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0b, 0x81, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    0x66, 0x77, 0x88, 0x99, 0xff, 0x02
  },
  {// 6 Blocked
    0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x65, 0x64, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0b, 0x81, 0xaa, 0x50, 0x1a, 0x23, 0x34, 0x45,
    0xa6, 0x21, 0x43, 0x1a, 0xff, 0x03
  },
  { // 7 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 8 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 9 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 10 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
};
#define EF_EXT2_FILE_ID 0x6F4B

#define EF_EXT2_RECORD_SIZE 13
#define EF_EXT2_FILE_SIZE_IN_RECORDS 5

static const uint8_t ef_ext2[EF_EXT2_FILE_SIZE_IN_RECORDS][EF_EXT2_RECORD_SIZE] = {
  { // 1 Long
    0x02, 0x03, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 2 Max
    0x02, 0x0a, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xff
  },
  { // 3 Blocked
    0x02, 0x02, 0x32, 0xb4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 4 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  },
  { // 5 Empty
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
  }
};

#define EF_ECC_FILE_ID 0x6FB7

#define EF_ECC_USIM_RECORD_SIZE 14
#define EF_ECC_USIM_FILE_SIZE_IN_RECORDS 5
#define EF_ECC_USIM_FILE_SIZE_IN_BYTES (EF_ECC_USIM_RECORD_SIZE*EF_ECC_USIM_FILE_SIZE_IN_RECORDS)
static const uint8_t usim_ef_ecc[EF_ECC_USIM_FILE_SIZE_IN_RECORDS][EF_ECC_USIM_RECORD_SIZE] = {
  { 0x11, 0xf2, 0xff,  'E',  'u',  'r',  'o',  ' ',  'E',  'm',  'e',  'r', 0xff, 0x00 },
  { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
  { 0x99, 0xf9, 0xff,  'U',  'K',  ' ',  'E',  'm',  'e',  'r', 0xff, 0xff, 0xff, 0x01 },
  { 0x19, 0xf1, 0xff,  'U',  'S',  ' ',  'E',  'm',  'e',  'r', 0xff, 0xff, 0xff, 0x02 },
  { 0x09, 0x00, 0xf0,  'S',  'v',  'e',  'r',  'i',  'g',  'e', 0xff, 0xff, 0xff, 0x04 }
};

#define EF_ECC_SIM_FILE_SIZE_IN_BYTES 9
static const uint8_t sim_ef_ecc[EF_ECC_SIM_FILE_SIZE_IN_BYTES] = { 0x11, 0xf2, 0xff, 0x99, 0xf9, 0xff, 0x09, 0x00, 0xf0 };

#define EF_PLMNwAcT_FILE_ID 0x6F60
#define EF_PLMNsel_FILE_ID 0x6F30

#define EF_AD_FILE_ID 0x6FAD
#define EF_IMSI_FILE_ID 0x6F07


/* Pipe for MSR */
static int pipe_wr = -1;
static int pipe_rd = -1;

struct sim_stub_data_s
{
  sim_stub_closure_t      closure;
  ste_msgq_t            * mq;
  uintptr_t               client_tag;
  sim_stub_socket_t     * socket_p;
};

struct sim_stub_msg_s
{
    STE_MSG_COMMON;
    void                   *payload;
    int                     length;
};

struct ste_modem_s
{
  int                     mal_fd;     // -1 indicates modem is not connected
  int                     shmnetlnk_fd;     // -1 indicates modem is not connected
  int                     modem_reset_status; // -1 indicates modem reset in not indicated
};

typedef struct {
    int fd_response_pipe[2];
    int fd_event_pipe[2];
} sms_stub_data_t;

sms_stub_data_t gsim_stub_sms;

// SMS stub signal defines
#define SIM_STUB_SMS_SIGNAL_ARRAY_LEN  10
#define SIM_STUB_SMS_SIGNAL_ARRAY_SIZE (SIM_STUB_SMS_SIGNAL_ARRAY_LEN * sizeof(uint32_t))
#define SIM_STUB_SMS_POS_PRIMITIVE     0
#define SIM_STUB_SMS_POS_CLIENTTAG     1
#define SIM_STUB_SMS_POS_REQSTATUS     2
#define SIM_STUB_SMS_POS_SMS_ERROR     3
#define SIM_STUB_SMS_POS_SM_REFERENCE  4

// Messages
typedef enum {
    SIM_STUB_MSG_STOP,
    SIM_STUB_MSG_TERMINAL_PROFILE,
    SIM_STUB_MSG_MIRROR_REQ,
    SIM_STUB_MSG_MIRROR_TIME_OUT,
    SIM_STUB_MSG_PIN_VERIFY,
    SIM_STUB_MSG_READ_RECORD,
    SIM_STUB_MSG_UPDATE_RECORD,
    SIM_STUB_MSG_PIN_CHANGE,
    SIM_STUB_MSG_APP_LIST,
    SIM_STUB_MSG_APP_HACT,
    SIM_STUB_MSG_APP_DACT,
    SIM_STUB_MSG_MAL_UICC_STATUS,
    SIM_STUB_MSG_READ_BINARY,
    SIM_STUB_MSG_UPDATE_BINARY,
    SIM_STUB_MSG_GET_FORMAT,
    SIM_STUB_MSG_GET_INFORMATION,
    SIM_STUB_MSG_PIN_ENABLE,
    SIM_STUB_MSG_PIN_DISABLE,
    SIM_STUB_MSG_PIN_INFO,
    SIM_STUB_MSG_UICC_NOT_READY,
    SIM_STUB_MSG_PIN_UNBLOCK,
    SIM_STUB_MSG_MAL_CAT_STATUS,
    SIM_STUB_MSG_MODEM_DISABLE,
    SIM_STUB_MSG_MODEM_ENABLE,
    SIM_STUB_MSG_APDU_SEND,
    SIM_STUB_MSG_SIM_CHANNEL_SEND,
    SIM_STUB_MSG_SIM_CHANNEL_OPEN,
    SIM_STUB_MSG_SIM_CHANNEL_CLOSE,
    SIM_STUB_MSG_CAT_NOT_SUPPORTED,
    SIM_STUB_MSG_SIM_CONNECT,
    SIM_STUB_MSG_SIM_DISCONNECT,
    SIM_STUB_MSG_APP_SHUTDOWN,
    SIM_STUB_MSG_SET_SIM_TYPE,
    SIM_STUB_MSG_SAP_ATR_GET,
    SIM_STUB_MSG_SAP_APDU_DATA,
    SIM_STUB_MSG_SAP_WARM_RESET_CARD,
    SIM_STUB_MSG_SAP_COLD_RESET_CARD,
    SIM_STUB_MSG_SAP_DEACT_CARD,
    SIM_STUB_MSG_SAP_ACT_CARD,
    SIM_STUB_MSG_CARD_STATUS,

    // Messages for CAT testing
    SIM_STUB_MSG_MODEM_SHUTDOWN,
    SIM_STUB_MSG_PC_DISPLAY_TEXT,
    SIM_STUB_MSG_PC_SETUP_MENU,
    SIM_STUB_MSG_PC_SELECT_ITEM,
    SIM_STUB_MSG_PC_GET_INKEY,
    SIM_STUB_MSG_PC_GET_INPUT,
    SIM_STUB_MSG_PC_SETUP_EVENT_LIST,
    SIM_STUB_MSG_PC_PROVIDE_LOCAL_INFO,
    SIM_STUB_MSG_ENVELOPE_COMMAND,
    SIM_STUB_MSG_TERMINAL_RESPONSE,
    SIM_STUB_MSG_PC_SEND_SHORT_MESSAGE,
    SIM_STUB_MSG_PC_SEND_DTMF,
    SIM_STUB_MSG_PC_SEND_SS,
    SIM_STUB_MSG_PC_SEND_USSD,
    SIM_STUB_MSG_PC_SETUP_CALL,
    SIM_STUB_MSG_PC_POLL_INTERVAL,
    SIM_STUB_MSG_PC_POLLING_OFF,
    SIM_STUB_MSG_PC_MORE_TIME,
    SIM_STUB_MSG_PC_TIMER_MANAGEMENT,
    SIM_STUB_MSG_PC_REFRESH,
    SIM_STUB_MSG_REFRESH_RESET_IND,
    SIM_STUB_MSG_PC_PLAY_TONE,
    SIM_STUB_MSG_SET_POLL_INTERVAL,
    SIM_STUB_MSG_REFRESH,
    SIM_STUB_MSG_APP_SELECTED,
    SIM_STUB_MSG_ADAPT_DL,
    SIM_STUB_MSG_PC_LANGUAGE_NOTIFICATION,
    SIM_STUB_MSG_CARD_FALLBACK_INDICATION,
    SIM_STUB_MSG_PC_OPEN_CHANNEL,
    SIM_STUB_MSG_PC_CLOSE_CHANNEL,
    SIM_STUB_MSG_PC_SEND_DATA,
    SIM_STUB_MSG_PC_RECEIVE_DATA,
    SIM_STUB_MSG_POLL_REQ,
    // SIM stub internal
    SIM_STUB_MSG_INTERNAL_SET_RESPONSE_TYPE,
    SIM_STUB_MSG_INTERNAL_SET_CALL_STATUS
} sim_stub_msg_type_t;

//###############################################################
//structures for CAT test
//###############################################################
typedef struct
{
    //ste_cat_status_words_t   status_words; This is commented out until we get the status words in the modem.
    int                    status;
    ste_apdu_t             * apdu;
} sim_stub_cat_cat_response_t;

typedef struct
{
    uint8_t                 session_id;
    ste_apdu_t             *apdu;
} sim_stub_sim_channel_send_response_t;

typedef struct
{
    uint16_t                session_id;
} sim_stub_sim_channel_open_response_t;

//###############################################################
//global variable for the stub data
//###############################################################
sim_stub_data_t gStubData;

#define SIM_STUB_APPLICATION_ID  1

static int send_terminal_profile_error = 1; // Terminal Profile error once at start-up
static uint8_t ste_sim_stub_pin_enabled = 0; // Sends PIN IND esp. at startup

static ste_sim_stub_resp_type_t sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
static sim_stub_cat_cat_response_t* sim_stub_store_cat_rsp = NULL;
static uintptr_t sim_stub_store_client_tag = NULL;
static uint8_t sim_stub_is_timer_expiry_sent = 0;

cn_call_state_t call_status = CN_CALL_STATE_IDLE;

//###############################################################
//Functions
//###############################################################

cn_context_t* get_faked_cn_context_with_log(const char* f, int l);
cn_context_t* get_faked_cn_context();
cn_error_code_t send_cn_request(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag);
cn_error_code_t send_cn_event(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag);
sim_stub_msg_t     *sim_stub_msg_new(int type, uintptr_t client_tag);
#define CN_CONTEXT (get_faked_cn_context_with_log(__func__, __LINE__))

int ste_modem_sim_connect(ste_modem_t * m, uintptr_t client_tag) {
  sim_stub_msg_t                * msg;

  UNUSED(ste_modem_t *, m);
  UNUSED(uintptr_t, client_tag);
  msg = sim_stub_msg_new(SIM_STUB_MSG_SIM_CONNECT, client_tag);
  msg->payload = 0;
  msg->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

  return 0;
}
int ste_modem_sim_disconnect(ste_modem_t * m, uintptr_t client_tag) {
  sim_stub_msg_t                * msg;

  UNUSED(ste_modem_t *, m);
  UNUSED(uintptr_t, client_tag);
  msg = sim_stub_msg_new(SIM_STUB_MSG_SIM_DISCONNECT, client_tag);
  msg->payload = 0;
  msg->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
  return 0;
}

static void set_modem_response_status_word(ste_cat_status_words_t status_word)
{
    modem_response_status_word.sw1 = status_word.sw1;
    modem_response_status_word.sw2 = status_word.sw2;
}

static void sim_stub_response(int eventID, void *vedata, uintptr_t client_tag)
{
    sim_uicc_status_code_t              uicc_status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    sim_uicc_status_word_t              status_word = {1, 2}; // Expect these values in sw1 and sw2
                                                        // for the SIM I/O and PIN operations

    const char* const s[] = { "UNKNOWN", "INIT", "PIN_NEEDED", "PUK_NEEDED",
                              "PIN2_NEEDED", "PUK2_NEEDED",
                              "PIN_VERIFIED", "PIN2_VERIFIED", "READY",
                              "INVALID_CARD", "NO_CARD", "CLOSED" };


    printf("sim_stub : sim stub RESPONSE %x %p. \n", eventID, (void*)vedata);

    switch (eventID)
    {
        case SIM_STUB_MSG_UICC_NOT_READY:
        {
            printf("sim_stub : RESPONSE UICC_NOT_READY \n");
            catd_sig_modem_not_ready((uintptr_t)client_tag);

            catd_sig_cat_session_expired((uintptr_t)client_tag,0);
        }
        break;

        case SIM_STUB_MSG_CAT_NOT_SUPPORTED:
        {
            printf("sim_stub : RESPONSE CAT_NOT_SUPPORTED \n");
            catd_sig_modem_cat_not_supported((uintptr_t)client_tag);
        }
        break;

        case SIM_STUB_MSG_MAL_UICC_STATUS:
        {
            if (vedata)
            {
                sim_stub_uicc_status_t *uicc_status_p = (sim_stub_uicc_status_t *) vedata;;

                if (uicc_status_p->status == STE_UICC_STATUS_READY
                    || uicc_status_p->status == STE_UICC_STATUS_INIT) {
                // status == READY or INIT
                // Set app_type to value != UNKNOWN, else ServiceTable tests will fail
                  printf("sim_stub : INDICATION SIM_STUB_MSG_MAL_UICC_STATUS, status: %s \n", s[(int)uicc_status_p->status]);
                  uiccd_sig_uicc_status_payload_ind((uintptr_t) client_tag, uicc_status_p->status, current_app_type, STE_UICC_CARD_TYPE_UICC);
                }
                else {
                  uiccd_sig_uicc_status_ind((uintptr_t) client_tag, uicc_status_p->status);
                }

                printf("sim_stub : INDICATION SIM_STUB_MSG_MAL_UICC_STATUS, status: %d \n", uicc_status_p->status);
                free(uicc_status_p);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_MAL_CAT_STATUS:
        {
            if (vedata)
            {
                sim_stub_cat_status_t *cat_status_p;

                cat_status_p = (sim_stub_cat_status_t *) vedata;
                if (cat_status_p->status == 0) {
                    catd_sig_modem_ready((uintptr_t) client_tag);
                }
                else {
                    catd_sig_modem_not_ready((uintptr_t) client_tag);
                }
                printf("sim_stub : INDICATION SIM_STUB_MSG_MAL_CAT_STATUS, status: %d \n", cat_status_p->status);
                free(cat_status_p);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_CARD_STATUS:
        {
            if (vedata)
            {
                sim_stub_card_status_t *card_status_p;

                card_status_p = (sim_stub_card_status_t *) vedata;
                printf("sim_stub : INDICATION SIM_STUB_MSG_CARD_STATUS, status: %d \n", card_status_p->status);

                uiccd_sig_sim_status((uintptr_t) client_tag, card_status_p->status);
                free(card_status_p);

                sleep(1);

                uiccd_sig_server_card_status((uintptr_t) client_tag, 0,
                                   STE_SIM_CARD_STATUS_READY);

            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_APP_LIST:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                int no_of_apps = 1;

                uiccd_sig_app_list((uintptr_t)client_tag, 0, no_of_apps);
                printf("sim_stub : RESPONSE SIM_STUB_MSG_APP_LIST\n");
            }
        }
        break;

        case SIM_STUB_MSG_TERMINAL_PROFILE:
        {
            if (vedata || send_terminal_profile_error )
            {
               catd_sig_tpdl_status((uintptr_t)client_tag, 1);
               printf("sim_stub : RESPONSE SIM_STUB_MSG_TERMINAL_PROFILE IS NOT SENT!\n");
               send_terminal_profile_error = send_terminal_profile_error ? send_terminal_profile_error - 1 : 0;
            }
            else
            {
                //respond with default data you want
                catd_sig_tpdl_status((uintptr_t)client_tag, 0);
                printf("sim_stub : RESPONSE SIM_STUB_MSG_TERMINAL_PROFILE\n");
                sleep(2);
                catd_sig_modem_ind_reg_ok((uintptr_t)client_tag);
            }
        }
        break;
        case SIM_STUB_MSG_APP_HACT:
        {
          if (vedata)
          {
          }
          else
          {
              //respond with default data you want
              ste_sim_app_type_t app_type = STE_SIM_APP_TYPE_USIM;
              int app_id = 1;
              uiccd_sig_app_activate((uintptr_t)client_tag, 0, app_id, app_type);
              printf("sim_stub : RESPONSE SIM_STUB_MSG_APP_HACT\n");
          }
        }
        break;

        case SIM_STUB_MSG_APP_DACT:
        {
          if (vedata)
          {
          }
          else
          {
              //respond with default data you want
              ste_sim_app_type_t app_type = STE_SIM_APP_TYPE_USIM;
              int app_id = SIM_STUB_APPLICATION_ID;
              uiccd_sig_app_activate((uintptr_t)client_tag, 0, app_id, app_type);
              printf("sim_stub : RESPONSE SIM_STUB_MSG_APP_HACT\n");
          }
        }
        break;

        case SIM_STUB_MSG_SIM_CONNECT:
        {
            if ( vedata ) {
            } else {
              uiccd_sig_sim_connect_response((uintptr_t) client_tag,
                                                 SIM_UICC_STATUS_CODE_OK,
                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS);
            }
            break;
        }
        case SIM_STUB_MSG_SIM_DISCONNECT:
        {
            if ( vedata ) {
            } else {
              uiccd_sig_sim_disconnect_response((uintptr_t) client_tag,
                                                 SIM_UICC_STATUS_CODE_OK,
                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS);

              uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_DISCONNECTED_CARD);
            }
            break;
        }

        case SIM_STUB_MSG_PIN_VERIFY:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                uiccd_sig_pin_verify_response((uintptr_t)client_tag,
                                              uicc_status_code,
                                              uicc_status_code_fail_details,
                                              status_word);
                printf("sim_stub : SIM_STUB_MSG_PIN_VERIFY\n");
            }
        }
        break;

        case SIM_STUB_MSG_PIN_INFO:
        {
          if (vedata)
            {
            }
          else
            {
              //respond with default data you want
              int attempts = 3;
              int attempts2 = 10;

              uiccd_sig_pin_info_response((uintptr_t)client_tag,
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          0, attempts, attempts2);
              printf("sim_stub : SIM_STUB_MSG_PIN_INFO\n");
            }
        }
        break;
        case SIM_STUB_MSG_PIN_ENABLE:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                uiccd_sig_pin_enable_response((uintptr_t)client_tag,
                                              uicc_status_code,
                                              uicc_status_code_fail_details,
                                              status_word);
                printf("sim_stub : SIM_STUB_MSG_PIN_ENABLE\n");
            }
        }
        break;
        case SIM_STUB_MSG_PIN_DISABLE:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                uiccd_sig_pin_disable_response((uintptr_t)client_tag,
                                              uicc_status_code,
                                              uicc_status_code_fail_details,
                                              status_word);
                printf("sim_stub : SIM_STUB_MSG_PIN_DISABLE\n");
            }
        }
        break;

        case SIM_STUB_MSG_PIN_CHANGE:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                uiccd_sig_pin_change_response((uintptr_t)client_tag,
                                              uicc_status_code,
                                              uicc_status_code_fail_details,
                                              status_word);
                printf("sim_stub : SIM_STUB_MSG_PIN_CHANGE\n");
            }
        }
        break;

        case SIM_STUB_MSG_PIN_UNBLOCK:
        {
            if (vedata)
            {
            }
            else
            {
                //respond with default data you want
                uiccd_sig_pin_unblock_response((uintptr_t)client_tag,
                                              uicc_status_code,
                                              uicc_status_code_fail_details,
                                              status_word);
                printf("sim_stub : SIM_STUB_MSG_PIN_UNBLOCK\n");
            }
        }
        break;

        case SIM_STUB_MSG_READ_RECORD:
        {
            if (vedata)
            {
                ste_uicc_sim_file_read_record_response_t *read_resp;

                read_resp = (ste_uicc_sim_file_read_record_response_t *) vedata;
                uiccd_sig_read_sim_file_record_response((uintptr_t)client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word,
                                                        read_resp->data, read_resp->length);
                if (read_resp->data != NULL)
                {
                    free(read_resp->data);
                }
                printf("sim_stub : RESPONSE UICC_CAT_READ_RECORD\n");

                free(read_resp);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_READ_BINARY:
        {
            if (vedata)
            {
                uiccd_msg_read_sim_file_binary_response_t *read_resp;

                read_resp = (uiccd_msg_read_sim_file_binary_response_t *) vedata;
                uiccd_sig_read_sim_file_binary_response((uintptr_t)client_tag,
                                                        read_resp->uicc_status_code,
                                                        read_resp->uicc_status_code_fail_details,
                                                        read_resp->status_word, read_resp->data, read_resp->len);
                if (read_resp->data != NULL)
                {
                    free(read_resp->data);
                }
                printf("sim_stub : RESPONSE UICC_CAT_READ_BINARY\n");

                free(read_resp);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_UPDATE_RECORD:
        {
            if (vedata)
            {
                ste_uicc_update_sim_file_record_response_t *update_resp;

                update_resp = (ste_uicc_update_sim_file_record_response_t *) vedata;
                uiccd_sig_update_sim_file_record_response((uintptr_t)client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word, NULL, 0);

                printf("sim_stub : RESPONSE UICC_CAT_UPDATE_RECORD\n");
                free(update_resp);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_UPDATE_BINARY:
        {
            if (vedata)
            {
                ste_uicc_update_sim_file_binary_response_t *update_resp;

                update_resp = (ste_uicc_update_sim_file_binary_response_t *) vedata;
                uiccd_sig_update_sim_file_binary_response((uintptr_t)client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word, NULL, 0);

                printf("sim_stub : RESPONSE UICC_CAT_UPDATE_BINARY\n");
                free(update_resp);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_GET_FORMAT:
        {
            if (vedata)
            {
                uiccd_msg_sim_file_get_format_response_t *format_resp;
                uiccd_msg_sim_file_get_format_response_t *rsp_msg_p;

                format_resp = (uiccd_msg_sim_file_get_format_response_t *) vedata;
                rsp_msg_p = uiccd_msg_sim_file_get_format_response_create((uintptr_t)client_tag,
                                                        format_resp->uicc_status_code,
                                                        format_resp->uicc_status_code_fail_details,
                                                        format_resp->status_word,
                                                        format_resp->file_type,
                                                        format_resp->file_size,
                                                        format_resp->record_len,
                                                        format_resp->num_records);

                rsp_msg_p->file_type = format_resp->file_type;
                rsp_msg_p->record_len = format_resp->record_len;
                rsp_msg_p->num_records = format_resp->num_records;
                rsp_msg_p->file_size = format_resp->file_size;

                uiccd_sig_msg( (ste_msg_t *) rsp_msg_p);
                free(format_resp);
                printf("sim_stub : RESPONSE SIM_STUB_MSG_GET_FORMAT\n");
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_GET_INFORMATION:
        {
            if (vedata)
            {
                ste_uicc_get_file_information_response_t *file_info;

                file_info = (ste_uicc_get_file_information_response_t *) vedata;
                uiccd_sig_get_file_information_response((uintptr_t)client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word, file_info->fcp, file_info->length);
                if (file_info->fcp != NULL)
                {
                    free(file_info->fcp);
                }
                printf("sim_stub : RESPONSE SIM_STUB_MSG_GET_INFORMATION\n");

                free(file_info);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_APDU_SEND:
        {
            if (vedata)
            {
                ste_uicc_appl_apdu_send_response_t *file_info;

                file_info = (ste_uicc_appl_apdu_send_response_t *) vedata;
                uiccd_sig_appl_apdu_send_response((uintptr_t)client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word, file_info->data, file_info->length);
                if (file_info->data != NULL)
                {
                    free(file_info->data);
                }
                printf("sim_stub : RESPONSE SIM_STUB_MSG_APDU_SEND\n");

                free(file_info);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

         case SIM_STUB_MSG_MODEM_DISABLE:
        {
            if (vedata)
            {
            }
            else
            {
                catd_sig_disable_status(client_tag, 0);
                printf("sim_stub : SIM_STUB_MSG_MODEM_DISABLE\n");
            }
        }
        break;
        case SIM_STUB_MSG_MODEM_ENABLE:
        {
            if (vedata)
            {
            }
            else
            {
                catd_sig_enable_status(client_tag, 0);
                printf("sim_stub : SIM_STUB_MSG_MODEM_ENABLE\n");
            }
        }
        break;
        case SIM_STUB_MSG_SIM_CHANNEL_SEND:
        {
            if (vedata)
            {
                sim_stub_sim_channel_send_response_t    *p = vedata;

                uiccd_sig_sim_channel_send_response(client_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    ste_apdu_get_raw(p->apdu),
                                                    ste_apdu_get_raw_length(p->apdu));
                if (p->apdu)
                {
                    ste_apdu_delete(p->apdu);
                }
                free(p);
            }
            else
            {
            }
        }
        break;
        case SIM_STUB_MSG_SIM_CHANNEL_OPEN:
        {
            if (vedata)
            {
                sim_stub_sim_channel_open_response_t    *p = vedata;

                uiccd_sig_sim_channel_open_response(client_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    p->session_id);
                free(p);
            }
            else
            {
            }
        }
        break;
        case SIM_STUB_MSG_SIM_CHANNEL_CLOSE:
        {
            if (vedata)
            {
            }
            else
            {
                uiccd_sig_sim_channel_close_response(client_tag,
                                                     uicc_status_code,
                                                     uicc_status_code_fail_details,
                                                     status_word);
            }
        }
        break;

//####################################################################################
//For CAT testing
//####################################################################################

        case SIM_STUB_MSG_ENVELOPE_COMMAND:
        {
            if (vedata)
            {
                if ( sim_stub_response_type == STE_SIM_STUB_HOLD_TIMER_EC_RESPONSE_TILL_TR_RESPONSE ) {
                    sim_stub_store_cat_rsp = (sim_stub_cat_cat_response_t *)vedata;
                    sim_stub_store_client_tag = client_tag;
                } else {
                sim_stub_cat_cat_response_t *cat_rsp_p;

                cat_rsp_p = (sim_stub_cat_cat_response_t *) vedata;
                catd_sig_modem_cat_response(client_tag, cat_rsp_p->apdu, cat_rsp_p->status);

                printf("sim_stub : send CAT Response for EC to CATD. Client_tag 0x%x. \n", client_tag);

                if (cat_rsp_p->apdu)
                {
                    ste_apdu_delete(cat_rsp_p->apdu);
                }
                free(cat_rsp_p);
                }
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_TERMINAL_RESPONSE:
        {
            if (vedata)
            {
                sim_stub_cat_cat_response_t *cat_rsp_p;
                cat_rsp_p = (sim_stub_cat_cat_response_t *) vedata;

                if ( sim_stub_response_type == STE_SIM_STUB_HOLD_TR_RESPONSE_FOR_ONE_MINUTE ) {
                    sleep(60);
                    sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                }

                catd_sig_modem_cat_response(client_tag, cat_rsp_p->apdu, cat_rsp_p->status);

                printf("sim_stub : send CAT Response for TR to CATD. Client_tag: 0x%x. \n", client_tag);
                if (cat_rsp_p->apdu)
                {
                    ste_apdu_delete(cat_rsp_p->apdu);
                }
                free(cat_rsp_p);

                if ( sim_stub_response_type == STE_SIM_STUB_HOLD_TIMER_EC_RESPONSE_TILL_TR_RESPONSE &&
                     sim_stub_is_timer_expiry_sent )
                {
                    sim_stub_is_timer_expiry_sent = 0;
                    sim_stub_response_type  = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                    if ( sim_stub_store_cat_rsp == NULL ) {
                        printf("sim_stub: Error. No stored response for EC for delayed response.\n");
                        break;
                    }
                    catd_sig_modem_cat_response(sim_stub_store_client_tag,
                    sim_stub_store_cat_rsp->apdu,
                    sim_stub_store_cat_rsp->status);
                    printf("sim_stub: send delayed TR response for Envelope"
                           "to CATD. Client_tag: 0x%x.\n", sim_stub_store_client_tag );
                    if ( sim_stub_store_cat_rsp->apdu )
                    {
                        ste_apdu_delete(sim_stub_store_cat_rsp->apdu);
                    }
                    free(sim_stub_store_cat_rsp);
                    sim_stub_store_cat_rsp = NULL;
                    sim_stub_store_client_tag = NULL;
                }
            }
            else
            {
                //respond with default data you want
            }
            //now tell stub socket that terminal response has been done and cat response is returned to CATD
            //so ready to proceed with next PC
            sim_stub_socket_terminal_response_done();
        }
        break;

        case SIM_STUB_MSG_PC_DISPLAY_TEXT:
        case SIM_STUB_MSG_PC_SETUP_MENU:
        case SIM_STUB_MSG_PC_SELECT_ITEM:
        case SIM_STUB_MSG_PC_GET_INKEY:
        case SIM_STUB_MSG_PC_GET_INPUT:
        case SIM_STUB_MSG_PC_PROVIDE_LOCAL_INFO:
        case SIM_STUB_MSG_PC_SETUP_EVENT_LIST:
        case SIM_STUB_MSG_PC_SEND_SHORT_MESSAGE:
        case SIM_STUB_MSG_PC_SEND_DTMF:
        case SIM_STUB_MSG_PC_SEND_SS:
        case SIM_STUB_MSG_PC_SEND_USSD:
        case SIM_STUB_MSG_PC_SETUP_CALL:
        case SIM_STUB_MSG_PC_POLL_INTERVAL:
        case SIM_STUB_MSG_PC_POLLING_OFF:
        case SIM_STUB_MSG_PC_MORE_TIME:
        case SIM_STUB_MSG_PC_TIMER_MANAGEMENT:
        case SIM_STUB_MSG_PC_REFRESH:
        case SIM_STUB_MSG_PC_LANGUAGE_NOTIFICATION:
        case SIM_STUB_MSG_PC_OPEN_CHANNEL:
        case SIM_STUB_MSG_PC_CLOSE_CHANNEL:
        case SIM_STUB_MSG_PC_SEND_DATA:
        case SIM_STUB_MSG_PC_RECEIVE_DATA:
        {
            if (vedata)
            {
                ste_apdu_t             *apdu;

                apdu = (ste_apdu_t *) vedata;
                catd_sig_apdu(apdu, client_tag);
                printf("sim_stub : send Proactive Command to CATD. \n");
                ste_apdu_delete(apdu);
            }
            else
            {
                //respond with default data you want
            }
        }
        break;

        case SIM_STUB_MSG_REFRESH_RESET_IND:
        {
            printf("Recieved SIM_STUB_MSG_REFRESH_RESET_IND");
            catd_sig_modem_refresh_ind((uintptr_t)client_tag, STE_APDU_REFRESH_UICC_RESET, STE_CAT_REFRESH_IND_START, 0);
        }
        break;

        case SIM_STUB_MSG_INTERNAL_SET_RESPONSE_TYPE:
        {
            if ( vedata ) {
                sim_stub_response_type = *((unsigned char*)vedata);
                free(vedata);
            } else {
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
            }
        }
        break;

        case SIM_STUB_MSG_INTERNAL_SET_CALL_STATUS:
        {
            if ( vedata ) {
                call_status = *((unsigned char*)vedata);
                free(vedata);
            } else {
                call_status = CN_CALL_STATE_IDLE;
            }
        }
        break;

        case SIM_STUB_MSG_SET_POLL_INTERVAL:
            {
                if (vedata) {
                    int* interval_used_p = (int*) vedata;
                    if ( sim_stub_response_type == STE_SIM_STUB_SEND_NON_ZERO_POLL_INTERVAL ) {
                        *interval_used_p = 10;
                        catd_sig_modem_set_poll_interval_response(0, *interval_used_p, client_tag);
                    } else if ( sim_stub_response_type == STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE ) {
                        catd_sig_modem_set_poll_interval_response(-1, 0, client_tag);
                    } else {
                        catd_sig_modem_set_poll_interval_response(0, *interval_used_p, client_tag);
                    }
                    free(vedata);
                } else {
                    catd_sig_modem_set_poll_interval_response(-1, 0, client_tag);
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                break;
            }

        case SIM_STUB_MSG_REFRESH:
            {
                if (vedata) {
                } else {
                    switch( sim_stub_response_type ) {
                        default:
                        case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                            catd_sig_modem_refresh_response(client_tag, 0);
                            break;
                        case STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE:
                            catd_sig_modem_refresh_response(client_tag, -1);
                            break;
                    }
                    sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                }
                break;
            }

        case SIM_STUB_MSG_APP_SELECTED:
        {
          if (vedata)
            {
            }
          else
            {
              //respond with default data you want
              catd_sig_app_ready((uintptr_t)client_tag, 0);
              printf("sim_stub : SIM_STUB_MSG_APP_SELECTED\n");
            }
        }
        break;

        case SIM_STUB_MSG_APP_SHUTDOWN:
        {
          if ( vedata )
          {
          }
          else
          {
            uiccd_sig_app_shutdown_init((uintptr_t)client_tag,0);
            printf("sim_stub : SIM_STUB_MSG_APP_SHUTDOWN\n");
          }
        }
        break;

        case SIM_STUB_MSG_ADAPT_DL:
        {
          if (vedata)
            {
            }
          else
            {
              printf("Adapt status = %x\n",catd_adapt_get_status());
              printf("SMS CB event socket = %x\n",catd_get_sms_cb_event_socket());

              //respond with default data you want
              catd_sig_adapt_terminal_support_table_update_status((uintptr_t)client_tag, true, status_word);
              status_word.sw1 = 0x90;
              status_word.sw2 = 0x00;
              catd_sig_adapt_terminal_support_table_update_status((uintptr_t)client_tag, true, status_word);
              printf("sim_stub : SIM_STUB_MSG_ADAPT_DL\n");
              //sleep(2);
              catd_sig_cat_server_ready((uintptr_t)client_tag, 0);
            }
        }
        break;

        case SIM_STUB_MSG_CARD_FALLBACK_INDICATION:
        {
            if ( vedata ) {
            } else {
              catd_sig_modem_reset((uintptr_t)client_tag);
              uiccd_sig_card_fallback((uintptr_t)client_tag, 1, 0, 1); /* ICC app, Unknown App, ICC card */
            }
        }
        break;
        case SIM_STUB_MSG_SAP_ATR_GET:
        {
            uiccd_sig_sap_session_atr_get_response((uintptr_t)client_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    NULL, 0);
        }
        break;

        case SIM_STUB_MSG_SAP_APDU_DATA:
        {
            uiccd_sig_sap_session_apdu_data_response((uintptr_t) client_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    NULL, 0);
        }
        break;

        case SIM_STUB_MSG_SAP_WARM_RESET_CARD:
        {
            uiccd_sig_sap_session_ctrlcard_warmreset_response((uintptr_t) client_tag, 0);
        }
        break;

        case SIM_STUB_MSG_SAP_COLD_RESET_CARD:
        {
            uiccd_sig_sap_session_ctrlcard_coldreset_response((uintptr_t) client_tag, 0);
        }
        break;

        case SIM_STUB_MSG_SAP_DEACT_CARD:
        {
            uiccd_sig_sap_session_ctrlcard_deactivate_response((uintptr_t) client_tag, 0);
        }
        break;

        case SIM_STUB_MSG_SAP_ACT_CARD:
        {
            uiccd_sig_sap_session_ctrlcard_activate_response((uintptr_t)client_tag, 0);
        }
        break;

        case SIM_STUB_MSG_POLL_REQ:
        {
          if( vedata ) {
              sim_stub_cat_cat_response_t *cat_rsp_p;
              cat_rsp_p = (sim_stub_cat_cat_response_t *) vedata;
              printf("sim_stub : SIM_STUB_MSG_POLL_REQ\n");

              catd_sig_modem_cat_response(client_tag, cat_rsp_p->apdu, cat_rsp_p->status);
              if( cat_rsp_p->apdu) {
                  ste_apdu_delete(cat_rsp_p->apdu);
              }
              free (cat_rsp_p);
          } else {
            //respond with default data
              catd_sig_modem_cat_response(client_tag, NULL, -1);
          }
        }
        break;

        default:
        {
            printf("sim_stub : WRONG EVENT ID!\n");
        }
        break;
    }

}


//####################################################################################################
//####################################################################################################

void sim_stub_msg_delete(ste_msg_t * vmsg)
{
    sim_stub_msg_t     *msg_p;
    msg_p = (sim_stub_msg_t *) vmsg;

    free(msg_p);
}


sim_stub_msg_t     *sim_stub_msg_new(int type, uintptr_t client_tag)
{
    sim_stub_msg_t     *msg = malloc(sizeof(*msg));
    if (msg) {
        msg->type = type;
        msg->delete_func = sim_stub_msg_delete;
        msg->payload = 0;
        msg->length = 0;
        msg->client_tag = client_tag;
    }
    return msg;
}


// -----------------------------------------------------------------------------


void
sim_stub_callback(sim_stub_data_t * data, int cause, const void *cd, uintptr_t client_tag)
{
    if (data && data->closure.func)
    {
        (data->closure.func) (cause, cd, data->closure.user_data, client_tag);
    }
}



static void            *sim_stub_thread_func(void *vptr)
{
    sim_stub_data_t        *data = (sim_stub_data_t *) vptr;
    cat_barrier_t          *bar = 0;
    uintptr_t               client_tag;

    setThreadName("simstub");
    client_tag = data->client_tag;

    printf("sim_stub : %s : New thread started 0x%08lx. \n",
            __func__, (unsigned long)pthread_self());

    gsim_stub_sms.fd_response_pipe[0] = -1;
    gsim_stub_sms.fd_response_pipe[1] = -1;
    gsim_stub_sms.fd_event_pipe[0] = -1;
    gsim_stub_sms.fd_event_pipe[1] = -1;

    /*
     * For MSL, during startup, simd_do_startup() will call ste_catd_modem_connect() and also
     * send startup messages to uiccd and catd. Calling ste_catd_modem_connect() will eventually
     * launch this thread and also tell it to 'send messages' to uiccd and catd.
     *
     * This means that there is a race in place, and unfortunately uiccd can not handle messages
     * from the modem until it has processed the startup message from  simd_do_startup().
     *
     * In order to prevent this in the MSL build, we add a short pause here to make sure this
     * thread does not start to process its messages before catd and uiccd has had a chance to
     * process their respective startup message.
     *
     * In other words, if this thread does not process it's messages, it wont know it has been told
     * to send messages somewhere else...
     */
    printf("sim_stub : %s : thread 0x%08lx 'slow start' pause. \n",
            __func__, (unsigned long)pthread_self());
    usleep(100000); // Sleep is needed, see comment above!!!

    printf("sim_stub : %s : thread 0x%08lx enter main loop. \n",
            __func__, (unsigned long)pthread_self());
    for (;;)
    {
        sim_stub_msg_t     *msg = 0;
        uintptr_t client_tag;

        msg = (sim_stub_msg_t *) ste_msgq_pop(data->mq);

        if (!msg)
        {
            continue;
        }

        if (msg->type == SIM_STUB_MSG_STOP)
        {
            bar = msg->payload;
            ste_msg_delete((ste_msg_t *)msg);
            break;
        }
        client_tag = msg->client_tag;
        msg->client_tag = 0;

        printf("sim_stub : Calling sim_stub_response with client_tag 0x%x. \n", client_tag);
        sim_stub_response(msg->type, msg->payload, client_tag);
        sim_stub_msg_delete((ste_msg_t *)msg);
        //sleep(1);
    }

    printf("sim_stub : %s : thread 0x%08lx exit main loop (thread dying). \n",
            __func__, (unsigned long)pthread_self());

    ste_msgq_delete(data->mq);

    cat_barrier_release(bar, 0);
    return 0;
}



static int sim_stub_connect(sim_stub_data_t * stub_p)
{
    pthread_t               tid;
    int                     rv;

    stub_p->mq = ste_msgq_new();

    rv = sim_launch_thread(&tid, PTHREAD_CREATE_DETACHED, sim_stub_thread_func, stub_p);

    if (rv != 0)
    {
        printf("sim_stub : launch thread failed \n");
        return -1;
    }

    stub_p->socket_p = sim_stub_socket_new();
    if (!stub_p->socket_p)
    {
      printf("sim_stub : create socket thread failed \n");
      return -1;
    }

    rv = sim_stub_socket_start(stub_p->socket_p);
    if (rv)
    {
      printf("sim_stub : start socket failed \n");
      return -1;
    }

    thread_started = 1;
    return rv;
}



static int sim_stub_disconnect(sim_stub_data_t * stub_p)
{
    sim_stub_msg_t     *msg;
    cat_barrier_t      *bar;        /* Barrier to sync shutdown */
    int                 rv;

    rv = sim_stub_socket_stop(stub_p->socket_p);
    if (rv) {
        printf("sim_stub : stop socket failed \n");
        return -1;
    }
    sim_stub_socket_delete(stub_p->socket_p);
    stub_p->socket_p = 0;

    bar = cat_barrier_new();
    if (!bar)
    {
        return -1;
    }
    cat_barrier_set(bar);

    msg = sim_stub_msg_new(SIM_STUB_MSG_STOP, stub_p->client_tag);
    msg->payload = bar;
    ste_msgq_add(stub_p->mq, (ste_msg_t *) msg);

    cat_barrier_wait(bar, 0);   /* Wait for thread to die */
    cat_barrier_delete(bar);

    return 0;
}


// -----------------------------------------------------------------------------
// Exposed interface

ste_modem_t            *ste_catd_modem_new()
{
    ste_modem_t            *m = calloc(1, sizeof(*m));
    if (m)
    {
        m->mal_fd = -1;
        m->shmnetlnk_fd = -1;
        m->modem_reset_status = -1;
    }
    printf("sim_stub : modem created. \n");

    return m;
}


void ste_catd_modem_delete(ste_modem_t * m, uintptr_t client_tag)
{
    UNUSED(uintptr_t, client_tag);
    free(m);
    printf("sim_stub : modem deleted. \n");
}

static ssize_t ste_modem_reset_stub_func(char *buf, char *buf_max, void *ud)
{
    UNUSED(char*, buf);
    UNUSED(char*, buf_max);
    UNUSED(void*, ud);
    int reset_val = 2;

    if (buf)
    { /* Normal operation */
    }
    else
    {    // ES is shutting down (as per docs for ES)
        if (pipe_rd != -1)
        {
            close(pipe_rd);
            pipe_rd = -1;
        }
        if (pipe_wr != -1)
        {
            close(pipe_wr);
            pipe_wr = -1;
        }
    }

    return reset_val;
}


int ste_catd_modem_connect(ste_modem_t * m, uintptr_t client_tag)
{
    int rv = 0;
    sim_stub_msg_t                * msg1;
    sim_stub_msg_t                * msg2;
    sim_stub_msg_t                * msg3;
    sim_stub_msg_t                * msg4;
    sim_stub_uicc_status_t        * uicc_status_p;
    sim_stub_uicc_status_t        * uicc_status_p2;
    sim_stub_cat_status_t         * cat_status_p;
    sim_stub_card_status_t         * card_status_p;
    ste_es_t                      * es = 0;
    ste_es_parser_closure_t       pc;

    UNUSED(ste_modem_t*, m);

    gStubData.client_tag = client_tag;

    if (thread_started == 0) {
        printf("Starting MSL thread\n");
        rv = sim_stub_connect(&gStubData);
        if (rv != 0) {
          printf("Could not start MSL thread\n");
          return rv;
        }
    }

    printf("sim_stub : modem connected. \n");

    //the Signal sequence to init the modem
    uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p)
    {
        return -1;
    }
    memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p->status = STE_UICC_STATUS_INIT;
    msg1 = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, client_tag);
    msg1->payload = uicc_status_p;
    msg1->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg1);

    //then send a cat ready ind
    cat_status_p = malloc(sizeof(sim_stub_cat_status_t));
    if (!cat_status_p)
    {
        return -1;
    }
    memset(cat_status_p, 0, sizeof(sim_stub_cat_status_t));

    cat_status_p->status = 0;
    msg2 = sim_stub_msg_new(SIM_STUB_MSG_MAL_CAT_STATUS, client_tag);
    msg2->payload = cat_status_p;
    msg2->length = sizeof(sim_stub_cat_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg2);

    //send card status indication
    card_status_p = malloc(sizeof(sim_stub_card_status_t));
    if (!card_status_p)
    {
        return -1;
    }
    memset(card_status_p, 0, sizeof(sim_stub_card_status_t));

    //the Signal sequence to init the modem
    uicc_status_p2 = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p2)
    {
        return -1;
    }
    memset(uicc_status_p2, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p2->status = STE_UICC_STATUS_STARTUP_COMPLETED;
    msg4 = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, client_tag);
    msg4->payload = uicc_status_p2;
    msg4->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg4);

    card_status_p->status = STE_REASON_NO_CARD;
    msg3 = sim_stub_msg_new(SIM_STUB_MSG_CARD_STATUS, client_tag);
    msg3->payload = card_status_p;
    msg3->length = sizeof(sim_stub_card_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg3);

    /* create a pipe */
    rv = sim_create_pipe(&pipe_wr, &pipe_rd);
    if (rv < 0)
    {
        return rv;
    }
    /* Assign read fd to shmnetlnk_fd */
    m->shmnetlnk_fd = pipe_rd;
    if ( m->shmnetlnk_fd < 0 )
    {
        rv = -1;
        return rv;
    }

    pc.func = ste_modem_reset_stub_func;
    pc.user_data = m;

    es = ste_es_new_nonbuf(m->shmnetlnk_fd, &pc);
    rv = catd_add_es(es);
    if (rv != 0) {
        printf("sim_stub : internal failure%d. \n", rv);
        return -1;
    }
    return rv;
}



int ste_catd_modem_disconnect(ste_modem_t * m, uintptr_t client_tag)
{
    int rv;

    UNUSED(ste_modem_t*, m);

    gStubData.client_tag = client_tag;

    if (pipe_rd != -1)
    {
        catd_rem_es(pipe_rd);
    }

    rv = sim_stub_disconnect(&gStubData);
    printf("sim_stub : modem disconnected. \n");
    return rv;
}





int ste_catd_modem_ec(ste_modem_t * m, uintptr_t client_tag, ste_apdu_t * apdu)
{
    sim_stub_msg_t                * msg;
    sim_stub_cat_cat_response_t   * cat_rsp_p;
    ste_apdu_t                    * cc_apdu_p;
    unsigned char           APDU_CC_RSP_Call[] =
    {
        0x02, //allowed with modification
        0x1D, //Length
        0x06, 0x05, 0x01, 0x21, 0x43, 0x65, 0x87,  //address
        0x07, 0x01, 0x01,  //ccp
        0x08, 0x02, 0x21, 0x43,  //sub address
        0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, //Alpha ID
        0x2A,0x01,0x02,
        0x90, 0x00 // Status words
    };
    unsigned char           APDU_CC_RSP_SS[] =
    {
        0x02, //allowed with modification
        0x07, //Length
        0x09, 0x05, 0x2a, 0x34, 0x35, 0x36, 0x23, //SS String
        0x90, 0x00 // Status words
    };
    unsigned char           APDU_CC_RSP_USSD[] =
    {
        0x02, //allowed with modification
        0x07, //Length
        0x0a, 0x05, 0x2a, 0x31, 0x31, 0x31, 0x23, //Ussd String
        0x90, 0x00 // Status words
    };
    unsigned char           APDU_CC_RSP_PDP[] =
    {
        0x02, //allowed with modification
        0x1d, //Length
        0x52, 0x1b, 0x69, 0x70, 0x76, 0x34, 0x3b, //PDP address
        0x74, 0x65, 0x6c, 0x69, 0x61, 0x3b, 0x78,
        0x78, 0x78, 0x40, 0x73, 0x74, 0x65, 0x2e,
        0x63, 0x65, 0x6d, 0x3a, 0x38, 0x30, 0x38,
        0x38,
        0x90, 0x00 // Status words
    };
    unsigned char            APDU_CC_RSP_NOT_ALLOWED[] = {
        0x00, // Not allowed
        0x00, // Length
        0x90, 0x00 // Status words
    };
    unsigned char            APDU_EC_OK[] = {
        0x90, 0x00 // Status Words
    };
    unsigned char            APDU_EC_NOK[] = {
        0x6f, 0x00 // Status Words
    };
    uint8_t                  *raw_data = ste_apdu_get_raw(apdu);

    UNUSED(ste_modem_t*, m);

    printf("sim_stub : envelope command. \n");
    printf("sim_stub : APDU length = %d. \n", ste_apdu_get_raw_length(apdu));
    printf("sim_stub : Client tag  = 0x%x. \n", client_tag);

    cat_rsp_p = malloc(sizeof(sim_stub_cat_cat_response_t));
    if (!cat_rsp_p)
    {
        return -1;
    }
    memset(cat_rsp_p, 0, sizeof(sim_stub_cat_cat_response_t));

    cat_rsp_p->status = 0;

    switch(raw_data[0]) {
        default:
        {
          // Check if we want to on test status word != 0x9000.
          if (modem_response_status_word.sw1 != 0x90) {
              APDU_EC_NOK[0] = modem_response_status_word.sw1;
              APDU_EC_NOK[1] = modem_response_status_word.sw2;
          }
          cc_apdu_p = ste_apdu_new(APDU_EC_NOK, sizeof(APDU_EC_NOK));
        }
        break;

        case 0xd7: // Timer Expiry
        {
            if ( sim_stub_response_type == STE_SIM_STUB_HOLD_TIMER_EC_RESPONSE_TILL_TR_RESPONSE ) {
                    sim_stub_is_timer_expiry_sent = 1;
            }
        } // Fall through intended
        case 0xd3: // Menu selection
        case 0xd6: // Event Download
        {
          // Check if we want to on test status word != 0x9000.
          if (modem_response_status_word.sw1 != 0x90) {
              APDU_EC_OK[0] = modem_response_status_word.sw1;
              APDU_EC_OK[1] = modem_response_status_word.sw2;
          }
          cc_apdu_p = ste_apdu_new(APDU_EC_OK, sizeof(APDU_EC_OK));
        }
        break;

        case 0xd4: // Call Control
        case 0xd5: // SMS control
        {
            switch( raw_data[6] & 0x7f ) {
                case 0x06 /*ADDRESS_TAG*/:
                    // Check if we want to on test status word != 0x9000.
                    if (modem_response_status_word.sw1 != 0x90) {
                        APDU_CC_RSP_Call[16] = modem_response_status_word.sw1;
                        APDU_CC_RSP_Call[17] = modem_response_status_word.sw2;
                    }
                    cc_apdu_p = ste_apdu_new(APDU_CC_RSP_Call, sizeof(APDU_CC_RSP_Call));

                    break;
                case 0x09 /*SS_STRING_TAG*/:
                    // Check if we want to on test status word != 0x9000.
                    if (modem_response_status_word.sw1 != 0x90) {
                        APDU_CC_RSP_SS[9] = modem_response_status_word.sw1;
                        APDU_CC_RSP_SS[10] = modem_response_status_word.sw2;
                    }
                    cc_apdu_p = ste_apdu_new(APDU_CC_RSP_SS, sizeof(APDU_CC_RSP_SS));

                    break;
                case 0x0a /*USSD_STRING_TAG*/:
                    // Check if we want to on test status word != 0x9000.
                    if (modem_response_status_word.sw1 != 0x90) {
                        APDU_CC_RSP_USSD[9] = modem_response_status_word.sw1;
                        APDU_CC_RSP_USSD[10] = modem_response_status_word.sw2;
                    }
                    cc_apdu_p = ste_apdu_new(APDU_CC_RSP_USSD, sizeof(APDU_CC_RSP_USSD));

                    break;
                case 0x52 /*PDP_CONTEXT_ACTIVATION_TAG*/:
                    // Check if we want to on test status word != 0x9000.
                    if (modem_response_status_word.sw1 != 0x90) {
                        APDU_CC_RSP_PDP[31] = modem_response_status_word.sw1;
                        APDU_CC_RSP_PDP[32] = modem_response_status_word.sw2;
                    }
                    cc_apdu_p = ste_apdu_new(APDU_CC_RSP_PDP, sizeof(APDU_CC_RSP_PDP));

                    break;
                default:
                    // Check if we want to on test status word != 0x9000.
                    if (modem_response_status_word.sw1 != 0x90) {
                        APDU_CC_RSP_Call[2] = modem_response_status_word.sw1;
                        APDU_CC_RSP_Call[3] = modem_response_status_word.sw2;
                    }
                    cc_apdu_p = ste_apdu_new(APDU_CC_RSP_NOT_ALLOWED, sizeof(APDU_CC_RSP_NOT_ALLOWED));

                    break;
            }
        }
        break;
    }

    if (!cc_apdu_p)
    {
        printf("sim_stub : ste_catd_modem_ec, create APDU failed. \n");
        free(cat_rsp_p);
        return -1;
    }
    cat_rsp_p->apdu = cc_apdu_p;

    msg = sim_stub_msg_new(SIM_STUB_MSG_ENVELOPE_COMMAND, client_tag);
    msg->payload = cat_rsp_p;
    msg->length = sizeof(sim_stub_cat_cat_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}



int ste_catd_modem_tpdl(ste_modem_t * m, uintptr_t client_tag, uint8_t * tp, size_t tp_size)
{
    sim_stub_msg_t                * msg;
    int                     rv;

    UNUSED(ste_modem_t*, m);
    UNUSED(uint8_t *, tp);
    UNUSED(size_t, tp_size);

    printf("sim_stub : terminal profile download. \n");

    if (thread_started == 0) {
        printf("Starting MSL Thread\n");
        rv = sim_stub_connect(&gStubData);
        if (rv != 0) {
          printf("Could not start MSL thread\n");
          return rv;
        }
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_TERMINAL_PROFILE, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

// TODO: Decide how to handle status returned from the modem to terminal. Please
//       use the same strategy for EC, ER and so on, where applicable.
//       Code changes below are done to adjust for EC, where returned status can
//       be modified.
int ste_catd_modem_tr(ste_modem_t * m, uintptr_t client_tag, ste_apdu_t * apdu)
{
    sim_stub_msg_t                * msg;
    sim_stub_cat_cat_response_t   * cat_rsp_p;
    ste_apdu_t                    * cc_apdu_p;
    unsigned char           APDU_CC_RSP[] =
    {
        0x90, 0x00 // Status words
    };

    UNUSED(ste_modem_t*, m);
    UNUSED(ste_apdu_t*, apdu);

    printf("sim_stub : terminal response. \n");
    cat_rsp_p = malloc(sizeof(sim_stub_cat_cat_response_t));
    if (!cat_rsp_p)
    {
        return -1;
    }

    memset(cat_rsp_p, 0, sizeof(sim_stub_cat_cat_response_t));
    cat_rsp_p->status = 0;

    // Check if we want to test on status word != 0x9000.
    if (modem_response_status_word.sw1 != 0x90) {
        APDU_CC_RSP[0] = modem_response_status_word.sw1;
        APDU_CC_RSP[1] = modem_response_status_word.sw2;
    }

    cc_apdu_p = ste_apdu_new(APDU_CC_RSP, sizeof(APDU_CC_RSP));
    if (!cc_apdu_p)
    {
        printf("sim_stub : ste_catd_modem_tr, create APDU failed. \n");
        free(cat_rsp_p);
        return -1;
    }

    cat_rsp_p->apdu = cc_apdu_p;

    msg = sim_stub_msg_new(SIM_STUB_MSG_TERMINAL_RESPONSE, client_tag);
    msg->payload = cat_rsp_p;
    msg->length = sizeof(sim_stub_cat_cat_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_catd_modem_set_poll_intervall(ste_modem_t * m, uintptr_t client_tag, int interval)
{
    sim_stub_msg_t* msg;
    int*            interval_used_p;

    UNUSED(ste_modem_t*, m);

    if ( sim_stub_response_type == STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return -1;
    }

    interval_used_p = malloc(sizeof(*interval_used_p));
    if (!interval_used_p) {
        return -1;
    }

    *interval_used_p = (interval / 10) * 10; // Truncate interval to nearest lower second (simulate behavior of one actual modem)

    msg = sim_stub_msg_new(SIM_STUB_MSG_SET_POLL_INTERVAL, client_tag);
    msg->payload = interval_used_p;
    msg->length = sizeof(*interval_used_p);
    ste_msgq_add(gStubData.mq, (ste_msg_t*)msg);

    return 0;
}

int ste_catd_modem_mirror(ste_modem_t * m, uintptr_t client_tag, int fd, ste_apdu_t * apdu)
{
    UNUSED(uintptr_t, client_tag);
    UNUSED(ste_modem_t*, m);
    UNUSED(ste_apdu_t*, apdu);
    UNUSED(int, fd);

    printf("sim_stub : #####################  NOT IMPLEMENTED YET. \n");

    return 0;
}

int ste_modem_pin_verify(ste_modem_t * m,
                         uintptr_t client_tag,
                         int app_id,
                         sim_uicc_pin_id_t pin_id,
                         const char *pin,
                         unsigned len)
{
    sim_stub_msg_t                * msg;

    UNUSED(int, app_id);
    UNUSED(sim_uicc_pin_id_t, pin_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char*, pin);
    UNUSED(unsigned, len);

    printf("sim_stub : pin verify. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_VERIFY, client_tag);

    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

/*******************************************************
  Internal Function to change the card_type to SIM card. Default
  is UICC card
********************************************************/
int ste_stub_set_sim_type( )
{
  sim_stub_msg_t                * msg;
  msg = sim_stub_msg_new(SIM_STUB_MSG_SET_SIM_TYPE, 0);
  msg->payload = 0;
  msg->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
  return 0;

}

int ste_modem_pin_change(ste_modem_t * m, uintptr_t client_tag, int app_id,
                         sim_uicc_pin_id_t pin_id,
                         const char *old_pin, unsigned old_pin_len,
                         const char *new_pin, unsigned new_pin_len)
{
    sim_stub_msg_t                * msg;

    UNUSED(int, app_id);
    UNUSED(sim_uicc_pin_id_t, pin_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char*, old_pin);
    UNUSED(const char*, new_pin);
    UNUSED(unsigned, old_pin_len);
    UNUSED(unsigned, new_pin_len);
    UNUSED(sim_uicc_pin_id_t, pin_id);

    printf("sim_stub : pin change. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_CHANGE, client_tag);

    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


int ste_modem_pin_unblock(ste_modem_t * m,
                          uintptr_t client_tag,
                          int app_id,
                          sim_uicc_pin_id_t pin_id,
                          const char *pin, unsigned pin_len,
                          const char *puk, unsigned puk_len)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);
    UNUSED(int, app_id);
    UNUSED(sim_uicc_pin_id_t, pin_id);
    UNUSED(const char*, pin);
    UNUSED(unsigned, pin_len);
    UNUSED(const char *, puk);
    UNUSED(unsigned, puk_len);

    printf("sim_stub : pin unblock. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_UNBLOCK, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

// Use this as template for the binary and record response
static const ste_uicc_sim_file_read_record_response_t read_record_default = {
  STE_UICC_STATUS_CODE_FAIL,
  STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,
  {0,0},
  NULL,
  0};

int ste_modem_file_read_record(ste_modem_t * m,
                               uintptr_t client_tag,
                               int app_id,
                               int file_id,
                               int rec_id,
                               int length,
                               const char *file_path)
{
    sim_stub_msg_t                             *msg;
    ste_uicc_sim_file_read_record_response_t *rsp_p;
    uint8_t                                    * data_p;
    uint8_t                                      data_len;
    const uint8_t                                active_data[] = "Active";
    const uint8_t                                sc_data[] = "SC";
    uint8_t                                    * index_p;
    const uint8_t                                SMSC_len = 0x1C;
    char                                       * rsp_data_p;

    UNUSED(int, app_id);
    UNUSED(int, file_id);
    UNUSED(int, rec_id);
    UNUSED(int, length);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);

    printf("sim_stub : read record. \n");
    printf("sim_stub : rec_id = %d, length = %d. \n", rec_id, length);

    rsp_p = malloc(sizeof(ste_uicc_sim_file_read_record_response_t));
    if (!rsp_p)
    {
        return -1;
    }
    memcpy(rsp_p, &read_record_default, sizeof(*rsp_p));
    // Manufacture your data_p and data_len, make change to rsp_p except
    // inserting data_p and data_len.
    switch (file_id) {
    case UICCD_FILE_ID_MSISDN:{
        uint8_t msisdn_record[38] = {0x00,0xFF,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0x05,0x81,0x21,0x43,
                                     0x34,0x12,0xFF,0xFF,
                                     0xFF,0xFF,0xFF,0xFF,
                                     0xFF,0xFF};

        data_len = sizeof(msisdn_record);

        data_p = malloc(data_len);
        if (!data_p)
        {
            free(rsp_p);
            return -1;
        }

        memset(data_p, 0xFF, data_len);
        memcpy(data_p, msisdn_record, data_len);

        rsp_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rsp_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        break;
    }
    case EF_FDN_FILE_ID: {
      data_len = EF_FDN_RECORD_SIZE;
      data_p = malloc(data_len);
      if (!data_p)
        {
          free(rsp_p);
          return -1;
        }
      memcpy(data_p, ef_fdn[rec_id-1], data_len);

      rsp_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
      rsp_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      rsp_p->status_word.sw1 = 0;
      rsp_p->status_word.sw2 = 0;
      break;
    }
    case EF_EXT2_FILE_ID: {
      data_len = EF_EXT2_RECORD_SIZE;
      data_p = malloc(data_len);
      if (!data_p)
        {
          free(rsp_p);
          return -1;
        }
      memcpy(data_p, ef_ext2[rec_id-1], data_len);

      rsp_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
      rsp_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      rsp_p->status_word.sw1 = 0;
      rsp_p->status_word.sw2 = 0;
      break;
    }
    case EF_ECC_FILE_ID:
      data_len = EF_ECC_USIM_RECORD_SIZE;
      data_p = malloc(data_len);
      if (!data_p)
        {
          free(rsp_p);
          return -1;
        }
      memcpy(data_p, usim_ef_ecc[rec_id-1], data_len);
      rsp_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
      rsp_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      break;
    default:
        if (rec_id == 3)
        {
            rsp_data_p = (char *)active_data;
        }
        else
        {
            rsp_data_p = (char *)sc_data;
        }

        data_len = strlen((char*)rsp_data_p);

        data_p = malloc(data_len + 1 + SMSC_len);
        if (!data_p)
        {
            free(rsp_p);
            return -1;
        }

        memset(data_p, 0, data_len + 1 + SMSC_len);
        memcpy(data_p, rsp_data_p, data_len);

        index_p = data_p + data_len;
        *index_p++ = 0xE0;  //indicators for SMSP
        *index_p++ = 0x05;  //length for dest address
        *index_p++ = 0x01;  //TON NPI for dest address
        *index_p++ = 0x06;  //number byte
        *index_p++ = 0x06;  //number byte
        *index_p++ = 0x06;  //number byte
        *index_p++ = 0x06;  //number byte
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        *index_p++ = 0x07;  //length for smsc
        *index_p++ = 0x01;  //TON NPI for smsc
        *index_p++ = 0x08;  //number byte
        *index_p++ = 0x08;  //number byte
        *index_p++ = 0x08;  //number byte
        *index_p++ = 0x08;  //number byte
        *index_p++ = 0x08;  //number byte
        *index_p++ = 0xF8;  //number byte
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        index_p++;          //leave untouched
        *index_p++ = 0x01;  //protocol id
        *index_p++ = 0x01;  //coding scheme
        *index_p++ = 0x01;  //valid period

        data_len += SMSC_len;
    }

    rsp_p->data = data_p;
    rsp_p->length = data_len;
    msg = sim_stub_msg_new(SIM_STUB_MSG_READ_RECORD, client_tag);
    msg->payload = rsp_p;
    msg->length = sizeof(ste_uicc_sim_file_read_record_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

static const uiccd_msg_read_sim_file_binary_response_t read_binary_default = {
  UICCD_MSG_READ_SIM_FILE_BINARY_RSP,
  0,
  0,
  0,
  0,
  STE_UICC_STATUS_CODE_OK,
  STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
  {0,0}};

int ste_modem_file_read_binary(ste_modem_t * m,
                               uintptr_t client_tag,
                               int app_id,
                               int file_id,
                               int offset,
                               int length,
                               const char *file_path)
{
    sim_stub_msg_t                             * msg;
    uiccd_msg_read_sim_file_binary_response_t  * rsp_p;
    uint8_t                                    * data_p;
    size_t                                       data_len = 0;
    const char                                   rsp_data[] = {0x08, 0x18, 0x32, 0x54, 0x76, 0x68, 0x68, 0x68, 0x68, 0x00};
    const char                                   resp_data_one_byte[] = {0xFF, 0x00};
    const char                                   resp_data_rat[] = { 0x01 };

    /*PLMN MCC: 240 MNC: 01 AcT: UTRAN, GSMcomp and GSM set. Element 2 is set to be undefined. */
    const unsigned char                          resp_data_plmn_wact[] = {0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0xff, 0xff, 0xff, 0xff, 0xff,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0,
                                                                     0x42, 0xf0, 0x10, 0x80, 0xc0 };
    /*PLMN MCC: 240 MNC: 01 Element 2 is set to be undefined. */
    const unsigned char                          resp_data_plmn_sel[] = {0x42, 0xf0, 0x10,
                                                                        0x42, 0xf0, 0x10,
                                                                        0xff, 0xff, 0xff,
                                                                        0x42, 0xf0, 0x10,
                                                                        0x42, 0xf0, 0x10,
                                                                        0x42, 0xf0, 0x10,
                                                                        0x42, 0xf0, 0x10,
                                                                        0x42, 0xf0, 0x10, };
    /* PLMN MNC length set: 2 . Dummy data in three first bytes. */
    const unsigned char                          resp_data_ef_ad_mnc_2[] = {0x00, 0x00, 0x12, 0x02 };

    /* PLMN MNC length set: 3 . Dummy data in three first bytes. */
//    const unsigned char                          resp_data_ef_ad_mnc_3[] = {0x00, 0x00, 0x12, 0x03 };

    /* MNC length: 3 . IMSI length 8 MCC=302 MNC=610 */
    const unsigned char                          resp_data_ef_imsi_302[] = {0x08, 0x31, 0x20, 0x16, 0x10, 0x32, 0x54, 0x76, 0x98  };

    /* MNC length: 2 . IMSI length 8 MCC=218 MNC=5 */
//    const unsigned char                          resp_data_ef_imsi_218[] = {0x08, 0x21, 0x81, 0x50, 0x10, 0x32, 0x54, 0x76, 0xF8  };

    /* MNC length: 3 . IMSI length 8 MCC=310 MNC=002 */
//    const unsigned char                          resp_data_ef_imsi_310[] = {0x08, 0x31, 0x01, 0x00, 0x12, 0x32, 0x54, 0x76, 0x98  };

    UNUSED(int, app_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);

    printf("sim_stub : read binary. \n");
    rsp_p = malloc(sizeof(uiccd_msg_read_sim_file_binary_response_t));
    if (!rsp_p) {
        return -1;
    }
    memcpy(rsp_p, &read_binary_default, sizeof(*rsp_p));
    switch (file_id) {
    case EF_RAT_FILE_ID:
        data_len = sizeof(resp_data_rat);
        data_p = malloc(data_len + 1);
        if ( !data_p) {
            free(rsp_p);
            return -1;
        }
        memset(data_p, 0, data_len + 1);
        memcpy(data_p, resp_data_rat, data_len + 1);
        break;
    case EF_PLMNwAcT_FILE_ID:
      data_len = sizeof(resp_data_plmn_wact);
      data_p = malloc(data_len + 1);
      if (!data_p) {
        free(rsp_p);
        return -1;
      }
      memset(data_p, 0, data_len + 1);
      memcpy(data_p, resp_data_plmn_wact, data_len);
      break;
    case EF_PLMNsel_FILE_ID:
        data_len = sizeof(resp_data_plmn_sel);
        data_p = malloc(data_len + 1);
        if (!data_p) {
            free(rsp_p);
            return -1;
        }
        memset(data_p, 0, data_len + 1);
        memcpy(data_p, resp_data_plmn_sel, data_len);
        break;
    case EF_ECC_FILE_ID:
      assert(length == 3);
      assert(offset+3 <= EF_ECC_SIM_FILE_SIZE_IN_BYTES);
      data_len = length;
      data_p = malloc(length);
      if (data_p)
        memcpy(data_p, sim_ef_ecc+offset, length);
      else
        data_len = 0;
      break;
    case EF_AD_FILE_ID:
        data_len = sizeof(resp_data_ef_ad_mnc_2);
        data_p = malloc(data_len + 1);
        if (!data_p) {
          free(rsp_p);
          return -1;
        }
        memset(data_p, 0, data_len + 1);
        memcpy(data_p, resp_data_ef_ad_mnc_2, data_len);
        break;
    case EF_IMSI_FILE_ID:
        data_len = sizeof(resp_data_ef_imsi_302);
        data_p = malloc(data_len + 1);
        if (!data_p) {
          free(rsp_p);
          return -1;
        }
        memset(data_p, 0, data_len + 1);
        memcpy(data_p, resp_data_ef_imsi_302, data_len);
        break;
    default: {
        if (length == 1) {
            // ServiceTable requests expect one byte only to be returned
            printf("sim_stub : requested data length = 1 \n");

            data_len = strlen(resp_data_one_byte);

            data_p = malloc(data_len + 1);
            if (!data_p) {
                free(rsp_p);
                return -1;
            }
            memset(data_p, 0, data_len + 1);
            memcpy(data_p, resp_data_one_byte, data_len);
        } else {
            data_len = strlen(rsp_data);

            data_p = malloc(data_len + 1);
            if (!data_p) {
                free(rsp_p);
                return -1;
            }
            memset(data_p, 0, data_len + 1);
            memcpy(data_p, rsp_data, data_len);
        }
    }
      break;
    }
    rsp_p->data = data_p;
    rsp_p->len = data_len;

    msg = sim_stub_msg_new(SIM_STUB_MSG_READ_BINARY, client_tag);
    msg->payload = rsp_p;
    msg->length = sizeof(ste_uicc_sim_file_read_binary_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}



int ste_modem_file_update_record(ste_modem_t * m,
                                 uintptr_t client_tag,
                                 int app_id,
                                 int file_id,
                                 int rec_id,
                                 int length,
                                 const char *file_path,
                                 uint8_t *data)
{
    sim_stub_msg_t                             * msg;
    ste_uicc_update_sim_file_record_response_t * update_rsp_p = NULL;

    UNUSED(int, app_id);
    UNUSED(int, file_id);
    UNUSED(int, rec_id);
    UNUSED(int, length);
    UNUSED(uint8_t*, data);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);

    printf("sim_stub : update record. \n");
    printf("sim_stub : rec_id = %d, length = %d. \n", rec_id, length);

//temp test
    {
        int i;
        printf("data = ");
        for(i = 0; i < length; i++)
        {
            printf("  %02x", data[i]);
        }
        printf("\n");
    }
//temp test end

    update_rsp_p = malloc(sizeof(ste_uicc_update_sim_file_record_response_t));
    if (!update_rsp_p)
    {
        return -1;
    }
    update_rsp_p->uicc_status_code = 1;
    update_rsp_p->uicc_status_code_fail_details = 2;
    update_rsp_p->status_word.sw1 = 0;
    update_rsp_p->status_word.sw2 = 0;
    msg = sim_stub_msg_new(SIM_STUB_MSG_UPDATE_RECORD, client_tag);

    msg->payload = update_rsp_p;
    msg->length = sizeof(ste_uicc_update_sim_file_record_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_file_update_binary(ste_modem_t * m,
                                 uintptr_t client_tag,
                                 int app_id,
                                 int file_id,
                                 int offset,
                                 int length,
                                 const char *file_path,
                                 uint8_t *data)
{
    sim_stub_msg_t                             * msg;
    ste_uicc_update_sim_file_binary_response_t * update_rsp_p;

    UNUSED(int, app_id);
    UNUSED(int, file_id);
    UNUSED(int, offset);
    UNUSED(int, length);
    UNUSED(uint8_t*, data);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);

    printf("sim_stub : update binary. \n");

    update_rsp_p = malloc(sizeof(ste_uicc_update_sim_file_binary_response_t));
    if (!update_rsp_p)
    {
        return -1;
    }

    update_rsp_p->uicc_status_code = 1;
    update_rsp_p->uicc_status_code_fail_details = 2;
    update_rsp_p->status_word.sw1 = 0;
    update_rsp_p->status_word.sw2 = 0;

    msg = sim_stub_msg_new(SIM_STUB_MSG_UPDATE_BINARY, client_tag);

    msg->payload = update_rsp_p;
    msg->length = sizeof(ste_uicc_update_sim_file_binary_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


int ste_modem_get_file_information(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   int app_id,
                                   int file_id,
                                   const char *file_path,
                                   sim_uicc_get_file_info_type_t type)
{
    sim_stub_msg_t                             * msg;
    ste_uicc_get_file_information_response_t   * file_info_p;
    uint8_t                                    * data_p;
    int                                          data_len;
    const char                                   rsp_data[] = {0x00, 0x00, 0x15, 0xe0, 0x6f, 0x3a, 0x04, 0x00, 0x11, 0x00, 0x22, 0x01, 0x02, 0x01, 0x1c};
    UNUSED(int, app_id);
    UNUSED(int, file_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);
    UNUSED(sim_uicc_get_file_info_type_t, type);

    printf("sim_stub : get file information.\n");


    file_info_p = malloc(sizeof(ste_uicc_get_file_information_response_t));
    if (!file_info_p)
    {
        return -1;
    }
    data_len = sizeof(rsp_data);

    data_p = malloc(data_len + 1);
    if (!data_p)
    {
        free(file_info_p);
        return -1;
    }
    memset(data_p, 0, data_len + 1);
    memcpy(data_p, rsp_data, data_len);

    file_info_p->uicc_status_code = 1;
    file_info_p->uicc_status_code_fail_details = 2;
    file_info_p->status_word.sw1 = 0;
    file_info_p->status_word.sw2 = 0;

    file_info_p->fcp = data_p;
    file_info_p->length = data_len;

    msg = sim_stub_msg_new(SIM_STUB_MSG_GET_INFORMATION, client_tag);
    msg->payload = file_info_p;
    msg->length = sizeof(ste_uicc_get_file_information_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sim_file_get_format(ste_modem_t * m,
                                  uintptr_t client_tag,
                                  int app_id,
                                  int file_id,
                                  const char * file_path)
{
    sim_stub_msg_t                             * msg;
    uiccd_msg_sim_file_get_format_response_t    * format_p;
    const uiccd_msg_sim_file_get_format_response_t format_init = {
      UICCD_MSG_SIM_FILE_GET_FORMAT_RSP,
      0,
      client_tag,
      STE_UICC_STATUS_CODE_FAIL,
      STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,
      {0,0},
      SIM_FILE_STRUCTURE_LINEAR_FIXED,
      1024,
      256,
      4};

    UNUSED(int, app_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);

    printf("sim_stub : get format. file_id: %X\n", file_id);
    format_p = malloc(sizeof(uiccd_msg_sim_file_get_format_response_t));
    if (!format_p) {
        return -1;
    }
    memcpy(format_p, &format_init, sizeof(*format_p));
    switch (file_id) {
    case 0xDEAD:
    case 0xFFFF:
        printf("sim_stub : file id for get format is incorrect, return error. \n");
        free(format_p);
        return -1;
    case EF_PLMNwAcT_FILE_ID:
        printf("sim_stub : read binary EF_PLMNwAcT_FILE_ID \n");
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        format_p->status_word.sw1 = 0;
        format_p->status_word.sw2 = 0;
        format_p->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
        format_p->file_size = 8 * 5; // 5 bytes * 8 mandatory plmn:s
        format_p->record_len = -1;
        format_p->num_records = -1;
        break;
    case EF_PLMNsel_FILE_ID:
        printf("sim_stub : read binary EF_PLMNsel_FILE_ID \n");
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        format_p->status_word.sw1 = 0;
        format_p->status_word.sw2 = 0;
        format_p->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
        format_p->file_size = 8 * 3; // 3 bytes * 8 mandatory plmn:s
        format_p->record_len = -1;
        format_p->num_records = -1;
        break;
    case EF_RAT_FILE_ID:
        printf("sim_stub : read binary EF_RAT_FILE_ID \n");
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        format_p->status_word.sw1 = 0;
        format_p->status_word.sw2 = 0;
        format_p->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
        format_p->file_size = 1; // 3 bytes * 8 mandatory plmn:s
        format_p->record_len = -1;
        format_p->num_records = -1;
        break;
    case UICCD_FILE_ID_MSISDN:
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        format_p->file_type = SIM_FILE_STRUCTURE_LINEAR_FIXED;
        format_p->record_len = 38;
        format_p->num_records = 1;
        format_p->file_size = format_p->record_len * format_p->num_records;
    break;
    case EF_FDN_FILE_ID:
      format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
      format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      format_p->status_word.sw1 = 0;
      format_p->status_word.sw2 = 0;

      format_p->file_type = SIM_FILE_STRUCTURE_LINEAR_FIXED;
      format_p->record_len = EF_FDN_RECORD_SIZE;
      format_p->num_records = EF_FDN_FILE_SIZE_IN_RECORDS;
      format_p->file_size = EF_FDN_FILE_SIZE_IN_BYTES;
    break;
    case EF_ECC_FILE_ID:
      format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
      format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      format_p->status_word.sw1 = 0;
      format_p->status_word.sw2 = 0;

      switch(current_app_type) {
      case STE_SIM_APP_TYPE_USIM:
        format_p->file_type =   SIM_FILE_STRUCTURE_LINEAR_FIXED;
        format_p->file_size =   EF_ECC_USIM_FILE_SIZE_IN_BYTES;
        format_p->record_len =  EF_ECC_USIM_RECORD_SIZE;
        format_p->num_records = EF_ECC_USIM_FILE_SIZE_IN_RECORDS;
        break;
      case STE_SIM_APP_TYPE_SIM:
        format_p->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
        format_p->file_size = EF_ECC_SIM_FILE_SIZE_IN_BYTES;
        format_p->record_len = 0;
        format_p->num_records = 0;
        break;
      default:
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND;

        format_p->file_type = SIM_FILE_STRUCTURE_UNKNOWN;
        format_p->file_size = 0;
        format_p->record_len = 0;
        format_p->num_records = 0;
        break;
      }
      break;
    case EF_SMSP_FILE_ID:
        format_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        format_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        format_p->status_word.sw1 = 0;
        format_p->status_word.sw2 = 0;

        format_p->file_type = SIM_FILE_STRUCTURE_LINEAR_FIXED;
        format_p->record_len = 256;
        format_p->num_records = 4;
        format_p->file_size = 1024;
      break;

    default:
      // Use default format.
        break;
    }
    msg = sim_stub_msg_new(SIM_STUB_MSG_GET_FORMAT, client_tag);
    msg->payload = format_p;
    msg->length = sizeof(uiccd_msg_sim_file_get_format_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_appl_apdu_send(ste_modem_t * m,
                             uintptr_t client_tag,
                             int app_id,
                             int cmd_force,
                             int apdu_len,
                             uint8_t *apdu,
                             int file_id,
                             const char *file_path)
{
    sim_stub_msg_t                             * msg;
    ste_uicc_appl_apdu_send_response_t         * apdu_send_resp_p;
    uint8_t                                    * data_p;
    int                                          data_len;
    const char                                   resp_data_two_bytes[] = {0x98, 0x10, 0x00};

    UNUSED(int, app_id);
    UNUSED(int, file_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char *, file_path);
    UNUSED(int, cmd_force);
    UNUSED(int, apdu_len);
    UNUSED(uint8_t *, apdu);


    printf("sim_stub : ste_modem_appl_apdu_send start. \n");
    apdu_send_resp_p = malloc(sizeof(ste_uicc_appl_apdu_send_response_t));
    if (!apdu_send_resp_p)
    {
        return -1;
    }

    data_len = strlen(resp_data_two_bytes);

    data_p = malloc(data_len + 1);
    if (!data_p)
    {
        free(data_p);
        return -1;
    }
    memset(data_p, 0, data_len + 1);
    memcpy(data_p, resp_data_two_bytes, data_len);


    apdu_send_resp_p->uicc_status_code = 1;
    apdu_send_resp_p->uicc_status_code_fail_details = 2;
    apdu_send_resp_p->status_word.sw1 = 0;
    apdu_send_resp_p->status_word.sw2 = 0;

    apdu_send_resp_p->data = data_p;
    apdu_send_resp_p->length = data_len;

    msg = sim_stub_msg_new(SIM_STUB_MSG_APDU_SEND, client_tag);
    msg->payload = apdu_send_resp_p;
    msg->length = sizeof(ste_uicc_appl_apdu_send_response_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;

}

int ste_modem_card_status(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t *, m);

    printf("sim_stub : ste_modem_card_status.\n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_CARD_STATUS, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


int ste_modem_application_list_and_select(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t *, m);

    printf("sim_stub : get application list. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_APP_LIST, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_activate_selected_app(ste_modem_t *m, uintptr_t client_tag) {
  // Copied from ste_modem_application_host_activate
  sim_stub_msg_t                * msg1;
  sim_stub_msg_t                * msg2;
  sim_stub_msg_t                * msg3;
  sim_stub_uicc_status_t        * uicc_status_p;

  UNUSED(ste_modem_t*, m);

  printf("sim_stub : host activate selected application. \n");

  //first send the response for activate app back
  msg1 = sim_stub_msg_new(SIM_STUB_MSG_APP_HACT, client_tag);
  msg1->payload = 0;
  msg1->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg1);

  //send the ind that app has been selected
  msg3 = sim_stub_msg_new(SIM_STUB_MSG_APP_SELECTED, client_tag);
  msg3->payload = 0;
  msg3->length = 0;
  ste_msgq_push(gStubData.mq, (ste_msg_t *) msg3);

  //then send a uicc card ready ind
  uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
  if (!uicc_status_p)
    {
      return -1;
    }
  memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

  uicc_status_p->status = STE_UICC_STATUS_READY;
  msg2 = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, client_tag);
  msg2->payload = uicc_status_p;
  msg2->length = sizeof(sim_stub_uicc_status_t);
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg2);

  return 0;
}

int ste_modem_deactivate_selected_app(ste_modem_t *m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg1;
    sim_stub_msg_t                * msg2;
    sim_stub_msg_t                * msg3;
    sim_stub_uicc_status_t        * uicc_status_p;

    UNUSED(ste_modem_t*, m);

    printf("sim_stub : host deactivate application. \n");
    return 0;
}

int ste_modem_app_initiate_shutdown(ste_modem_t *m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);
    UNUSED(uintptr_t, client_tag);

    msg = sim_stub_msg_new(SIM_STUB_MSG_APP_SHUTDOWN, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_application_host_activate(ste_modem_t * m,
                                        uintptr_t client_tag,
                                        int app_type,
                                        int app_id)
{
    sim_stub_msg_t                * msg1;
    sim_stub_msg_t                * msg2;
    sim_stub_msg_t                * msg3;
    sim_stub_uicc_status_t        * uicc_status_p;

    UNUSED(int, app_type);
    UNUSED(int, app_id);
    UNUSED(ste_modem_t*, m);

    printf("sim_stub : host activate application. \n");

    //first send the response for activate app back
    msg1 = sim_stub_msg_new(SIM_STUB_MSG_APP_HACT, client_tag);
    msg1->payload = 0;
    msg1->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg1);

    //send the ind that app has been selected
    msg3 = sim_stub_msg_new(SIM_STUB_MSG_APP_SELECTED, client_tag);
    msg3->payload = 0;
    msg3->length = 0;
    ste_msgq_push(gStubData.mq, (ste_msg_t *) msg1);

    //then send a uicc card ready ind
    uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p)
    {
        return -1;
    }
    memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p->status = STE_UICC_STATUS_READY;
    msg2 = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, client_tag);
    msg2->payload = uicc_status_p;
    msg2->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg2);

    return 0;
}


int ste_modem_pin_disable(ste_modem_t * m, uintptr_t client_tag, int app_id, int pin_id,
                         const char* pin, unsigned len)
{
    sim_stub_msg_t                * msg;

    UNUSED(int, app_id);
    UNUSED(int, pin_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char*, pin);
    UNUSED(unsigned, len);

    printf("sim_stub : pin disable. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_DISABLE, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


int ste_modem_pin_enable(ste_modem_t * m, uintptr_t client_tag, int app_id, int pin_id,
                         const char* pin, unsigned len)
{
    sim_stub_msg_t                * msg;

    UNUSED(int, app_id);
    UNUSED(int, pin_id);
    UNUSED(ste_modem_t*, m);
    UNUSED(const char*, pin);
    UNUSED(unsigned, len);

    printf("sim_stub : pin enable. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_ENABLE, client_tag);;
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


int ste_modem_pin_info(ste_modem_t * m, uintptr_t client_tag, int app_id, sim_uicc_pin_puk_id_t pin)
{
    sim_stub_msg_t                * msg;

    UNUSED(int, app_id);
    UNUSED(sim_uicc_pin_puk_id_t, pin);
    UNUSED(ste_modem_t*, m);

    printf("sim_stub : pin info. \n");

    if (uicc_fail_tests_flag) return -1;


    msg = sim_stub_msg_new(SIM_STUB_MSG_PIN_INFO, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_catd_modem_enable(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    printf("sim_stub : modem enable. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_MODEM_ENABLE, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_catd_modem_disable(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    printf("sim_stub : modem disable. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_MODEM_DISABLE, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

//####################################################################################
//For CAT testing
//####################################################################################

void ste_stub_pc_display_text()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_DisplayText[] =
    {
        0xD0, // Proactive Command
        0x22, // Length
        0x81, 0x03, 0x01, 0x21, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Identities
        0x8D, 0x04, 0x04, 0x43, 0x41, 0x54, // Text String
        0x1E, 0x02, 0x01, 0x01, // Icon ID
        0x2b, 0x00, // Immediate Response
        0x04, 0x02, 0x01, 0x20, // Duration
        0xD0, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 // Frame ID
    };

    printf("sim_stub : pc display text test. \n");

    apdu = ste_apdu_new(APDU_DisplayText, sizeof(APDU_DisplayText));
    if (!apdu)
    {
        printf("sim_stub : pc display test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_DISPLAY_TEXT, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_setup_menu()
{
    ste_apdu_t             *apdu;
    sim_stub_msg_t         *msg;
    unsigned char           APDU_SetUpMenu[] =
    {
        0xD0, // Proactive Command
        0x81, // Two byte Length Ind
        0x83, // Length
        0x81, 0x03, 0x01, 0x25, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x02, // Device Identities
        0x85, 0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, // Alpha Id
        0x65, 0x6E, 0x75,
        0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, // Item
        0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, // Item
        0x8F, 0x07, 0x03, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x33, // Item
        0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x34, // Item
        0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x35, // Item
        0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x36, // Item
        0x18, 0x04, 0x13, 0x10, 0x15, 0x26, // Next Action Ind
        0x1E, 0x02, 0x01, 0x01, // Icon ID
        0x1f, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // Icon Id List
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x51, 0x18, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, // Item Text Attribute List
        0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02,
        0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
        0x68, 0x01, 0x01, // Frame Id
    };

    printf("sim_stub : pc setup menu test. \n");

    apdu = ste_apdu_new(APDU_SetUpMenu, sizeof(APDU_SetUpMenu));
    if (!apdu)
    {
        printf("sim_stub : pc setup menu test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_MENU, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_select_item()
{
    ste_apdu_t             *apdu;
    sim_stub_msg_t         *msg;
    unsigned char           APDU_SelectItem[] = {
      0xD0, // Proactive Command
      0x6C, // Length
      0x81,0x03,0x01,0x24,0x00, // Command Details
      0x82,0x02,0x81,0x82, // Device Identities
      0x05,0x15,0x41,0x78,0x61,0x6C,0x74,0x6F,0x20,0x48,0x61,0x6E,0x64,0x73,0x65, // Alpha Id
      0x74,0x20,0x54,0x65,0x73,0x74,0x65,0x72,
      0x0F,0x0D,0x01,0x43,0x61,0x6C,0x6C,0x20,0x43,0x6F,0x6E,0x74,0x72,0x6F,0x6C, // Item
      0x0F,0x0D,0x02,0x43,0x61,0x6C,0x6C,0x20,0x43,0x6F,0x6E,0x6E,0x65,0x63,0x74, // Item
      0x0F,0x0C,0x03,0x53,0x4D,0x53,0x20,0x43,0x6F,0x6E,0x74,0x72,0x6F,0x6C, // Item
      0x0F,0x08,0x04,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Item
      0x0F,0x09,0x05,0x3E,0x20,0x4E,0x75,0x6D,0x62,0x65,0x72, // Item
      0x10,0x01,0x01, // Item Id
      0x1F,0x06,0x01,0x01,0x01,0x01,0x01,0x01 //Icon Id List
    };

    printf("sim_stub : pc select item test. \n");

    apdu = ste_apdu_new(APDU_SelectItem, sizeof(APDU_SelectItem));
    if (!apdu)
    {
        printf("sim_stub : pc select item test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SELECT_ITEM, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_setup_event_list()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SetupEventList[] =
    {
        0xD0, // PC tag
        0x12, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x05, // Type of command (Set up event list)
        0x00, // Command qualifier (RFU)

        //Device Identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        //Event List
        0x99, // Event List tag
        0x07, // Event List length
        0x00,
        0x01,
        0x02,
        0x03,
        0x05,
        0x0E,
        0x0B
    };

    printf("sim_stub : pc setup event list test. \n");

    apdu = ste_apdu_new(APDU_SetupEventList, sizeof(APDU_SetupEventList)); //apdu == ste_apdu_t
    if (!apdu)
    {
        printf("sim_stub : pc setup event list test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_EVENT_LIST, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_GPRS_CELL;
    (void)send_cn_event(CN_EVENT_CELL_INFO, CN_CONTEXT, 0);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_NO_COVERAGE;
    (void)send_cn_event(CN_EVENT_CELL_INFO, CN_CONTEXT, 0);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_LIMITED_COVERAGE;
    (void)send_cn_event(CN_EVENT_CELL_INFO, CN_CONTEXT, 0);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_WCDMA_CELL;
    (void)send_cn_event(CN_EVENT_CELL_INFO, CN_CONTEXT, 0);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_REG_INFO_MANUAL_MODE;
    (void)send_cn_event(CN_EVENT_MODEM_REGISTRATION_STATUS, CN_CONTEXT, 0);

    sleep(1);
    sim_stub_response_type = STE_SIM_STUB_SEND_CN_REG_INFO_AUTO_MODE;
    (void)send_cn_event(CN_EVENT_MODEM_REGISTRATION_STATUS, CN_CONTEXT, 0);

    sleep(1);
    call_status = CN_CALL_STATE_INCOMING;
    (void)send_cn_event(CN_EVENT_CALL_STATE_CHANGED, CN_CONTEXT, 0);

    sleep(1);
    call_status = CN_CALL_STATE_IDLE;
    (void)send_cn_event(CN_EVENT_CALL_STATE_CHANGED, CN_CONTEXT, 0);

    ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
}

void ste_stub_pc_setup_event_list_no_list()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SetupEventList[] =
    {
        0xD0, // PC tag
        0x09, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x05, // Type of command (Set up event list)
        0x00, // Command qualifier (RFU)

        //Device Identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)
    };

    printf("sim_stub : pc setup event list no list test. \n");

    apdu = ste_apdu_new(APDU_SetupEventList, sizeof(APDU_SetupEventList)); //apdu == ste_apdu_t
    if (!apdu)
    {
        printf("sim_stub : pc setup event list no list test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_EVENT_LIST, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

}

void ste_stub_pc_provide_local_info(uint8_t command_qualifier)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_ProvideLocalInfo[] =
    {
        0xD0, //PC tag
        0x09, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x26, // Type of command (provide local information)
        command_qualifier, // Command qualifier

        //Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        //UTRAN MEASUREMENT QUALIFIER
        0x69, //UTRAN MEASUREMENT QUALIFIER TAG
        0x01, //UTRAN MEASUREMENT QUALIFIER length
        0x02  //Qualifier, either intra-freq, inter-freq or inter-RAT

    };

    printf("sim_stub : pc provide local info test. \n");

    apdu = ste_apdu_new(APDU_ProvideLocalInfo, sizeof(APDU_ProvideLocalInfo));
    if (!apdu)
    {
        printf("sim_stub : pc setup event list test, create APDU failed. \n");
        return;
    }
    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_PROVIDE_LOCAL_INFO, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_get_input()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_GetInput[] =
    {
        0xD0, // Proactive Command
        0x2a, // Length
        0x81, 0x03, 0x01, 0x23, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Identities
        0x8D, 0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, // Text String
        0x33, 0x34, 0x35,
        0x91, 0x02, 0x05, 0x05, // Response Length
        0x17, 0x00, // Default Text
        0x1E, 0x02, 0x01, 0x01, //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 //Frame id
    };

    printf("sim_stub : pc get input test. \n");

    apdu = ste_apdu_new(APDU_GetInput, sizeof(APDU_GetInput));
    if (!apdu)
    {
        printf("sim_stub : pc get input test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_GET_INPUT, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_get_inkey()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_GetInkey[] =
    {
        0xD0, // Proactive COmmand
        0x26, // Length
        0x81, 0x03, 0x01, 0x22, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82, // Device Identities
        0x8D, 0x0A, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x22, 0x2B, 0x22, // Text String
        0x1E, 0x02, 0x01, 0x01, // Icon id
        0x04, 0x02, 0x02, 0x20,  //duration
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04, // Text Attribute
        0x68, 0x01, 0x01 //Frame id
    };

    printf("sim_stub : pc get inkey test. \n");

    apdu = ste_apdu_new(APDU_GetInkey, sizeof(APDU_GetInkey));
    if (!apdu)
    {
        printf("sim_stub : pc get inkey test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_GET_INKEY, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_short_message_packing_required()
{
  sim_stub_msg_t         *msg;
  ste_apdu_t             *apdu;
  unsigned char           APDU_SendSMS[] =  {
      0xD0, // Proactive Command
      0x23, // Length
      0x81,0x03,0x01,0x13,0x01, // Command Details
      0x82,0x02,0x81,0x83, // Device Identities
      0x8B,0x18,0x01,0x00,0x09,0x91,0x10,0x32,0x54,0x76,0xF8,0x40,0xF4,0x0C,0x54, // SMS TPDU
      0x65,0x73,0x74,0x20,0x4D,0x65,0x73,0x73,0x61,0x67,0x65
  };
    printf("sim_stub : pc send short message packing required test. \n");

    apdu = ste_apdu_new(APDU_SendSMS, sizeof(APDU_SendSMS));
    if (!apdu)
    {
        printf("sim_stub : pc send short message packing required test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_SHORT_MESSAGE, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_short_message_wrong_dcs()
{
  sim_stub_msg_t         *msg;
  ste_apdu_t             *apdu;
  unsigned char           APDU_SendSMS[] =  {
      0xD0, // Proactive Command
      0x23, // Length
      0x81,0x03,0x01,0x13,0x01, // Command Details
      0x82,0x02,0x81,0x83, // Device Identities
      0x8B,0x18,0x01,0x00,0x09,0x91,0x10,0x32,0x54,0x76,0xF8,0x40,0xc0,0x0C,0x54, // SMS TPDU
      0x65,0x73,0x74,0x20,0x4D,0x65,0x73,0x73,0x61,0x67,0x65
  };
    printf("sim_stub : pc send short message packing required test. \n");

    apdu = ste_apdu_new(APDU_SendSMS, sizeof(APDU_SendSMS));
    if (!apdu)
    {
        printf("sim_stub : pc send short message packing required test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_SHORT_MESSAGE, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_short_message()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SendSMS[] =
    {
      0xD0, // Proactive Command
      0x41, // Length
      0x81,0x03,0x01,0x13,0x00, // Command Details - SEND SMS, packing not required
      0x82,0x02,0x81,0x83, // Device Identities SIM->NW
      0x05,0x08,0x53,0x65,0x6e,0x64,0x20,0x53,0x4d,0x53, // Alpha ID
      0x06,0x05,0x01,0x21,0x43,0x65,0x87, // Address
      0x8B,0x18,0x01,0x00,0x09,0x91,0x10,0x32,0x54,0x76,0xF8, // SMS TPDU
      0x40,0xF4,0x0C,0x54,0x65,0x73,0x74,0x20,0x4D,0x65,0x73,
      0x73,0x61,0x67,0x65,
      0x1E,0x02,0x01,0x01, //Icon id
      0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute
      0x68,0x01,0x01 //Frame id
    };

    printf("sim_stub : pc send short message test. \n");

    apdu = ste_apdu_new(APDU_SendSMS, sizeof(APDU_SendSMS));
    if (!apdu)
    {
        printf("sim_stub : pc send short message test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_SHORT_MESSAGE, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}


void ste_stub_pc_send_dtmf()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SendDTMF[] =
    {
        0xD0, //PC tag
        0x22, //Length
        0x81, 0x03, 0x01, 0x14, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x2C, 0x05, 0xC1, 0xD2, 0xE3, 0x54, 0x6F,  //dtmf string (somewhat bad, should result in DTMF 1p2345 (6 should be truncated by 'bad' end marker)
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id
    };

    printf("sim_stub : pc send dtmf test. \n");

    apdu = ste_apdu_new(APDU_SendDTMF, sizeof(APDU_SendDTMF));
    if (!apdu)
    {
        printf("sim_stub : pc send dtmf test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_DTMF, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
    sleep(1);
}

void ste_stub_pc_send_dtmf_01()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    /* Missing DTMF string for hitting error scenario */
    unsigned char           APDU_SendDTMF[] =
    {
        0xD0, //PC tag
        0x1b, //Length
        0x81, 0x03, 0x01, 0x14, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id
    };

    printf("sim_stub : pc send dtmf test. \n");

    apdu = ste_apdu_new(APDU_SendDTMF, sizeof(APDU_SendDTMF));
    if (!apdu)
    {
        printf("sim_stub : pc send dtmf test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_DTMF, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
    sleep(1);
}

void ste_stub_set_response_type( ste_sim_stub_resp_type_t type )
{
  sim_stub_msg_t         *msg;
  unsigned char          *response_type;

  response_type = (ste_sim_stub_resp_type_t*)calloc( 1, sizeof( ste_sim_stub_resp_type_t ) );
  *response_type = type;
  msg = sim_stub_msg_new(SIM_STUB_MSG_INTERNAL_SET_RESPONSE_TYPE, 0);
  msg->payload = response_type;
  msg->length = sizeof(ste_sim_stub_resp_type_t);
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_set_call_status( cn_call_state_t state )
{
  sim_stub_msg_t         *msg;
  unsigned char          *response_type;

  response_type = (cn_call_state_t*)calloc( 1, sizeof( cn_call_state_t ) );
  *response_type = state;
  msg = sim_stub_msg_new(SIM_STUB_MSG_INTERNAL_SET_CALL_STATUS, 0);
  msg->payload = response_type;
  msg->length = sizeof(cn_call_state_t);
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_ss()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SendSS[] =
    {
        0xD0, //PC tag
        0x22, //Length
        0x81, 0x03, 0x01, 0x11, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x09, 0x05, 0x23, 0x21, 0x43, 0x65, 0x87,  //ss string
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id

        //Extra test
        //0xD0,0x1D,0x81,0x03,0x01,0x11,0x00,0x82,0x02,0x81,0x83,0x89,0x0E,0x91,0xAA,0x12,0x0A,0x21,0x43,0x65,0x87,0x09,0x21,0x43,0x65,0x87,0xB9,0x9E,0x02,0x01,0x01
    };

    printf("sim_stub : pc send ss test. \n");

    apdu = ste_apdu_new(APDU_SendSS, sizeof(APDU_SendSS));
    if (!apdu)
    {
        printf("sim_stub : pc send ss test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_SS, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_ss_no_string()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SendSS[] =
    {
        0xD0, //PC tag
        0x1b, //Length
        0x81, 0x03, 0x01, 0x11, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id
    };

    printf("sim_stub : pc send ss no string test. \n");

    apdu = ste_apdu_new(APDU_SendSS, sizeof(APDU_SendSS));
    if (!apdu)
    {
        printf("sim_stub : pc send ss test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_SS, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_ussd()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;

    unsigned char           APDU_SendUSSD[] =
    {
        0xD0, //PC tag
        0x22, //Length
        0x81, 0x03, 0x01, 0x12, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x0A, 0x05, 0x01, 0x21, 0x43, 0x65, 0x87,  //ussd string
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id
    };

    printf("sim_stub : pc send ussd test. \n");

    apdu = ste_apdu_new(APDU_SendUSSD, sizeof(APDU_SendUSSD));
    if (!apdu)
    {
        printf("sim_stub : pc send ussd test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_USSD, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    sleep(1); // Make sure the terminal has time to process the send ussd pc
    (void)send_cn_event(CN_EVENT_USSD, CN_CONTEXT, 0);
}

void ste_stub_pc_send_ussd_no_string()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;

    unsigned char           APDU_SendUSSD[] =
    {
        0xD0, //PC tag
        0x1b, //Length
        0x81, 0x03, 0x01, 0x12, 0x00, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x12, 0x34, 0x56, //alpha id
        0x1E, 0x02, 0x01, 0x01,  //Icon id
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute
        0x68, 0x01, 0x01  //Frame id
    };

    printf("sim_stub : pc send ussd no string test. \n");

    apdu = ste_apdu_new(APDU_SendUSSD, sizeof(APDU_SendUSSD));
    if (!apdu)
    {
        printf("sim_stub : pc send ussd test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_USSD, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    sleep(1); // Make sure the terminal has time to process the send ussd pc
    (void)send_cn_event(CN_EVENT_USSD, CN_CONTEXT, 0);
}

void ste_stub_pc_setup_call(uint8_t apdu_option)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char          APDU_SetupCall[] ={
            0xD0, //tag
            0x1E, // length
            0x81, 0x03, 0x01, 0x10, apdu_option, //details
            0x82, 0x02, 0x81, 0x83, //device id
            0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // alpha id 1
            0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C // address
            };

    printf("sim_stub : pc setup call test. \n");

    apdu = ste_apdu_new(APDU_SetupCall, sizeof(APDU_SetupCall));
    if (!apdu)
    {
        printf("sim_stub : pc setup call test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_CALL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_setup_ecall(uint8_t apdu_option)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char          APDU_SetupCall[] ={
            0xD0, //tag
            0x18, // length
            0x81, 0x03, 0x01, 0x10, apdu_option, //details
            0x82, 0x02, 0x81, 0x83, //device id
            0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // alpha id 1
            0x86, 0x03, 0x91, 0x11, 0xf2 // emg-address
            };

    printf("sim_stub : pc setup call test. \n");

    apdu = ste_apdu_new(APDU_SetupCall, sizeof(APDU_SetupCall));
    if (!apdu)
    {
        printf("sim_stub : pc setup call test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_CALL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_setup_call_no_addr(uint8_t apdu_option)
{
  sim_stub_msg_t         *msg;
  ste_apdu_t             *apdu;
  unsigned char           APDU_SetupCall[] = {
    0xD0, //tag
    0x13, // length
    0x81, 0x03, 0x01, 0x10, apdu_option, //details
    0x82, 0x02, 0x81, 0x83, //device id
    0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79 // alpha id 1
  };
  printf("sim_stub : pc setup call test. \n");
  apdu = ste_apdu_new(APDU_SetupCall, sizeof(APDU_SetupCall));
  if (!apdu)
  {
      printf("sim_stub : pc setup call test, create APDU failed. \n");
      return;
  }
  msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_CALL, 0);
  msg->payload = apdu;
  msg->length = ste_apdu_get_raw_length(apdu);
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_setup_call_with_sub_addr(uint8_t apdu_option)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SetupCall[] =
    {
        0xD0, //PC tag
        0x3d, //Length
        0x81, 0x03, 0x01, 0x10, apdu_option, //command details
        0x82, 0x02, 0x81, 0x83,  //device id
        0x05, 0x03, 0x11, 0x11, 0x11, //alpha id 1
        0x06, 0x05, 0x01, 0x21, 0x43, 0x65, 0x87,  //address
        0x07, 0x02, 0x01, 0x02,  //ccp
        0x08, 0x02, 0x21, 0x43,  //sub address
        0x04, 0x02, 0x01, 0x20,  //duration
        0x1E, 0x02, 0x01, 0x01,  //Icon id 1
        0x05, 0x03, 0x22, 0x22, 0x22, //alpha id 2
        0x1E, 0x02, 0x02, 0x02,  //Icon id 2
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute 1
        0x50, 0x04, 0x01, 0x02, 0x03, 0x04,  //Text Attribute 2
        0x68, 0x01, 0x01  //Frame id
    };


    printf("sim_stub : pc setup call test. \n");

    apdu = ste_apdu_new(APDU_SetupCall, sizeof(APDU_SetupCall));
    if (!apdu)
    {
        printf("sim_stub : pc setup call test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_CALL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}


void ste_stub_pc_setup_call_with_duration(uint8_t apdu_option)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char          APDU_SetupCall[] ={
            0xD0, //tag
            0x22, // length
            0x81, 0x03, 0x01, 0x10, apdu_option, //details
            0x82, 0x02, 0x81, 0x83, //device id
            0x85, 0x08, 0x4E, 0x6F, 0x74, 0x20, 0x62, 0x75, 0x73, 0x79, // alpha id 1
            0x86, 0x09, 0x91, 0x10, 0x32, 0x04, 0x21, 0x43, 0x65, 0x1C, 0x2C, // address
            0x04, 0x02, 0x01, 0x01 //duration
            };

    printf("sim_stub : pc setup call call_with_duration test. \n");

    apdu = ste_apdu_new(APDU_SetupCall, sizeof(APDU_SetupCall));
    if (!apdu)
    {
        printf("sim_stub : pc setup call test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SETUP_CALL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_poll_interval()
{
    sim_stub_msg_t* msg;
    ste_apdu_t*     apdu;
    unsigned char   APDU_PollInterval[] =
    {
        0xD0, // PC tag
        0x0D, // PC total length

        // Command details for poll interval
        0x81, // Command details tag
        0x03, // Command details lenght
        0x01, // Command number
        0x03, // Type of command (poll interval)
        0x00, // Command qualifier (RFU for poll interval)

        // Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        // Duration TLV
        0x04, // Duration tag
        0x02, // Duration TLV lenght
        0x02, // Duration unit (00 minutes, 01 secs, 02 1/10th of secs)
        0xFF, // Duration in above unit
    };

    printf("sim_stub : pc poll inverval test. \n");

    apdu = ste_apdu_new(APDU_PollInterval, sizeof(APDU_PollInterval));
    if (!apdu) {
        printf("sim_stub : pc poll inverval test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_POLL_INTERVAL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_polling_off()
{
    sim_stub_msg_t* msg;
    ste_apdu_t*     apdu;
    unsigned char   APDU_PollingOff[] =
    {
        0xD0, // PC tag
        0x09, // PC total length

        // Command details for poll interval
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x04, // Type of command (polling off)
        0x00, // Command qualifier (RFU for polling off)

        // Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)
    };

    printf("sim_stub : pc polling off test. \n");

    apdu = ste_apdu_new(APDU_PollingOff, sizeof(APDU_PollingOff));
    if (!apdu) {
        printf("sim_stub : pc polling off test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_POLLING_OFF, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_more_time()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_MoreTime[] =
    {
        0xD0, // Proactive Command
        0x09, // Length
        0x81, 0x03, 0x01, 0x02, 0x00, // Command Details
        0x82, 0x02, 0x81, 0x82 // Device Identities
    };

    printf("sim_stub : pc more time test. \n");

    apdu = ste_apdu_new(APDU_MoreTime, sizeof(APDU_MoreTime));
    if (!apdu)
    {
        printf("sim_stub : pc more time test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_MORE_TIME, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_play_tone()
{
  sim_stub_msg_t         *msg;
  ste_apdu_t             *apdu;
  unsigned char           APDU_PlayTone[] =
  {
      0xD0, // Proactive Command
      0x0c, // Length
      0x81, 0x03, 0x01, 0x20, 0x00, // Command Details
      0x82, 0x02, 0x81, 0x82, // Device Identities
      0x0e, 0x01, 0x11 // Tone
  };
  printf("sim_stub : pc play tone test. \n");
  apdu = ste_apdu_new(APDU_PlayTone, sizeof(APDU_PlayTone));
  if (!apdu)
  {
      printf("sim_stub : pc play tone test, create APDU failed. \n");
      return;
  }
  msg = sim_stub_msg_new(SIM_STUB_MSG_PC_PLAY_TONE, 0);
  msg->payload = apdu;
  msg->length = ste_apdu_get_raw_length(apdu);
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

int ste_stub_pc_timer_management_01()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char          TIMER_DEACTIVATE = 0x01;
    unsigned char          TIMER_ID = 0x01;
    unsigned char          TIMER_H = 0x00;
    unsigned char          TIMER_M = 0x00;
    unsigned char          TIMER_S = 0x00;
    unsigned char          COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_DEACTIVATE,    // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to deactivate a timer, but since the timer has not yet been
         * created, there is no timer to deactivate. */
        printf("\n sim_stub : ste_stub_pc_timer_management_01\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_01, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    } // End for loop.

    return 0;
}

int ste_stub_pc_timer_management_02()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_GET_TIMER_VALUE = 0x02;
    unsigned char TIMER_ID = 0x05;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_GET_TIMER_VALUE, // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to read a timer's value. But since the timer has not yet been
         * created, there is no time to read.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_02\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_02, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_03()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x06;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x00;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start a timer with timer value zero. To start a timer, the
         * value must be larger than zero, so it is not possible to start it.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_03\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : pc timer management test, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_04()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x02;
    unsigned char TIMER_H = 0x19; // 25 hours.
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x00;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start a timer with timer value larger than 24 hours. Since
         * highest timer value is 24 hours, it should not be possible to start
         * the timer.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_04\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_04, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.
        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_05()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x04;
    unsigned char TIMER_H = 0x18; // 24 hours.
    unsigned char TIMER_M = 0x01;
    unsigned char TIMER_S = 0x00;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start a timer with timer value larger than 24 hours. Since
         * highest timer value is 24 hours, it should not be possible to start
         * the timer.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_05\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_05, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.
        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_06()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x04;
    unsigned char TIMER_H = 0x18; // 24 hours.
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x01;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start a timer with timer value larger than 24 hours. Since
         * highest timer value is 24 hours, it should not be possible to start
         * the timer.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_06\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_06, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_07()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_DEACTIVATE = 0x01;
    unsigned char TIMER_GET_TIMER_VALUE = 0x02;
    unsigned char TIMER_ID = 0x05;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x08;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Start a timer and then try to read the timer value. The timer should
         * return the remaining time before expiration.
         *
         * Then deactivate the timer. The timer should return the remaining time
         * before expiration.
         *
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_07\n\n");
        APDU_TimerManagement[6] = TIMER_START;
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_07, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        APDU_TimerManagement[6] = TIMER_GET_TIMER_VALUE;
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_07, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        APDU_TimerManagement[6] = TIMER_DEACTIVATE;
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_07, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.

        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_08()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x00;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start a timer with timer ID zero. This should not be possible
         * since ID zero is a not valid value.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_08\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_08, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_09()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_DEACTIVATE = 0x01;
    unsigned char TIMER_ID = 0x00;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_DEACTIVATE,    // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to deactivate a timer with ID zero. This should not be possible
         * since ID zero is not a valid value.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_09\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_09, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.
        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_10()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_GET_TIMER_VALUE = 0x02;
    unsigned char TIMER_ID = 0x00;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_GET_TIMER_VALUE,// Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to get timer value from a timer with ID zero. This should not
         * be possible since ID zero is not a valid value.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_10\n\n");
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_10, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_11()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    int                    k;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x05;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x0A;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Try to start eight timers with timer ID 1-8 and make them expire.
         * Since all timer ID's are valid ID's, they should be started and also
         * eventually expire. Make sure the timers expire in correct order.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_11\n\n");

        for (k=1; k <= 8; k++) {
            APDU_TimerManagement[13] = k;
            apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
            if (!apdu) {
                printf("sim_stub : ste_stub_pc_timer_management_11, create APDU failed. \n");
                return -1;
            }

            msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
            msg->payload = apdu;
            msg->length = ste_apdu_get_raw_length(apdu);
            ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

            // sleep(1);  // To ensure that the terminal has time to handle this PC.
        }

/*        sleep(TIMER_S + 2); // Give the timers time to expire.

        sleep(TIMER_S); // To ensure that the terminal has time to handle this PC.
        */ sleep(1);
        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_12()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x01;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x03;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;
    ste_cat_status_words_t status_word = {0x00, 0x00};

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Start a timer and wait for it to expire. When the terminal sends
         * the Timer Expiration EC, the modem response status is busy. After
         * the terminal has sent a Terminal Response, the modem status is still
         * busy, so the Timer Expiration EC will not be resent. After one more
         * Terminal Response, the modem status is OK, and the Timer Expiration
         * EC can be sent.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_12\n\n");
        status_word.sw1 = 0x93;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word); // Status busy.

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_12, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(TIMER_S + 1); // Give the timer time to expire.
        // Following will generate a Terminal Response, but status is still busy.
        ste_stub_pc_polling_off();

        sleep(1);

        status_word.sw1 = 0x90;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word);  // Status OK.
        /* Following will generate a Terminal Response, with status OK. This will
         * make the terminal resending the Timer Expiration EC. */
        ste_stub_pc_polling_off();
        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }
    // Set status to "default" value in order not to interfere with other test cases.
    status_word.sw1 = 0x90;
    status_word.sw2 = 0x00;
    set_modem_response_status_word(status_word);

    return 0;
}

int ste_stub_pc_timer_management_13()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_ID = 0x07;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;
    ste_cat_status_words_t status_word = {0x00, 0x00};

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Start three timers and wait for them to expire. Make sure their Timer
         * Expiration EC are sent in correct order. When the terminal tries to
         * send the first Timer Expiration EC, the modem response status is
         * busy, so it must be resent when the modem status to a Terminal
         * Response is OK. When the two other timers expire, their Timer
         * Expiration EC's cannot be sent since the first EC is still waiting
         * to be resent.
         *
         * Each time a modem status on a Terminal Response is OK, send one of
         * the Timer Expiration EC's (but with updated time value) to the modem,
         * starting with the EC that has waited longest time.
         *
         * By making TIMER_7, TIMER_3 and TIMER_5 expiring in that order, we
         * not only check that the APDU's are resent in correct order, but also
         * that we choose "elements" from the table with respect of last, first
         * and middle to ensure that this "algorithm" works.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_13\n\n");
        status_word.sw1 = 0x93;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word); // Status busy.

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_13, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        APDU_TimerManagement[13] = 0x03; // Timer ID 3.
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_13, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        APDU_TimerManagement[13] = 0x05; // Timer ID 5.
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_13, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(TIMER_S); // Give the timers time to expire.

        // Following will generate a Terminal Response, but status is still busy.
        ste_stub_pc_polling_off();

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        status_word.sw1 = 0x90;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word);  // Status OK.
        /* Following will generate a Terminal Response, with status OK. This will
         * make the terminal resending the Timer Expiration EC. */
        ste_stub_pc_polling_off();  // The first timeout APDU should now be sent.
        sleep(1);  // To ensure that the terminal has time to handle this PC.

        ste_stub_pc_polling_off();  // The second timeout APDU should now be sent.
        sleep(1);  // To ensure that the terminal has time to handle this PC.

        ste_stub_pc_polling_off();  // The third timeout APDU should now be sent.
        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }
    // Set status to "default" value in order not to interfere with other test cases.
    status_word.sw1 = 0x90;
    status_word.sw2 = 0x00;
    set_modem_response_status_word(status_word);

    return 0;
}

int ste_stub_pc_timer_management_14()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_DEACTIVATE = 0x01;
    unsigned char TIMER_ID = 0x05;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;
    ste_cat_status_words_t status_word = {0x00, 0x00};

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Start a timer and wait for it to expire. When the terminal tries to
         * send the Timer Expiration EC, the modem status is busy, so it must
         * be resent. Start the timer again with a new timer expiration time.
         * This will remove/delete the Timer Expiration EC APDU that has not
         * been acknowledged (OK) by the SIM. When the modem status on a
         * Terminal Response is OK, there should be no APDU to be resent as
         * the timer has been restarted but not expired (again). Then deactivate
         * the timer before it expires.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_14\n\n");
        status_word.sw1 = 0x93;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word); // Status busy.

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_14, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(TIMER_S + 1); // Give the timer time to expire.

        // Following will generate a Terminal Response, but status is still busy.
        ste_stub_pc_polling_off();

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        status_word.sw1 = 0x90;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word);  // Status OK.

        // Start the timer again, with a new timer expiration value.
        TIMER_S = 0x06;
        APDU_TimerManagement[18] = ste_apdu_encode_bcd(TIMER_S);
        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_14, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        /* Following will generate a Terminal Response, with status OK. But,
         * there should no longer be any Timer Expiration EC to resend. */
        ste_stub_pc_polling_off();
        sleep(1);  // To ensure that the terminal has time to handle this PC.

        APDU_TimerManagement[6] = TIMER_DEACTIVATE;

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_14, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }
    // Set status to "default" value in order not to interfere with other test cases.
    status_word.sw1 = 0x90;
    status_word.sw2 = 0x00;
    set_modem_response_status_word(status_word);

    return 0;
}

int ste_stub_pc_timer_management_15()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x0e,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[13] = ste_apdu_encode_bcd(APDU_TimerManagement[13]);
    APDU_TimerManagement[14] = ste_apdu_encode_bcd(APDU_TimerManagement[14]);
    APDU_TimerManagement[15] = ste_apdu_encode_bcd(APDU_TimerManagement[15]);

    // Loop twice. One time with comprehension mask not set, and one time when it's set.
    for (i=0; i<2; i++) {
        //#####################################################################
        /* Start a timer and wait for it to expire. When the terminal tries to
         * send the Timer Expiration EC, the modem status is busy, so it must
         * be resent. Start the timer again with a new timer expiration time.
         * This will remove/delete the Timer Expiration EC APDU that has not
         * been acknowledged (OK) by the SIM. When the modem status on a
         * Terminal Response is OK, there should be no APDU to be resent as
         * the timer has been restarted but not expired (again). Then deactivate
         * the timer before it expires.
         * */
        printf("\n sim_stub : ste_stub_pc_timer_management_15\n\n");

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_15, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        //#####################################################################
        // Now loop one more time, but this time with comprehension bit set.
        APDU_TimerManagement[11] = APDU_TimerManagement[11] | COMPREHENSION_REQUIRED_MASK;
    }

    return 0;
}

int ste_stub_pc_timer_management_16()

{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    int                    i;
    unsigned char TIMER_START = 0x00;
    unsigned char TIMER_DEACTIVATE = 0x01;
    unsigned char TIMER_ID = 0x05;
    unsigned char TIMER_H = 0x00;
    unsigned char TIMER_M = 0x00;
    unsigned char TIMER_S = 0x04;
    unsigned char COMPREHENSION_REQUIRED_MASK = 0x80;
    ste_cat_status_words_t status_word = {0x00, 0x00};

    unsigned char           APDU_TimerManagement[] =
    {
        0xD0,                                        // Proactive UICC command tag.
        0x11,                                        // Length.
        0x81, 0x03, 0x01, 0x27, TIMER_START,         // Command details (command details tag, length, command number, type of command, command qualifier).
        0x82, 0x02, 0x81, 0x82,                      // Device identities (device identities tag, length, source, destination).
        0x24, 0x01, TIMER_ID,                        // Timer identifier (timer identifier tag, length, timer ID).
        0x25, 0x03, TIMER_H, TIMER_M, TIMER_S        // Timer value (timer tag value, length, hours, minutes, seconds).
    };

    APDU_TimerManagement[16] = ste_apdu_encode_bcd(APDU_TimerManagement[16]);
    APDU_TimerManagement[17] = ste_apdu_encode_bcd(APDU_TimerManagement[17]);
    APDU_TimerManagement[18] = ste_apdu_encode_bcd(APDU_TimerManagement[18]);

    //#####################################################################
    /* Start a timer and wait for it to expire. When the terminal tries to
    * send the Timer Expiration EC, the modem status is busy, so it must
    * be resent. Start the timer again with a new timer expiration time.
    * This will remove/delete the Timer Expiration EC APDU that has not
    * been acknowledged (OK) by the SIM. When the modem status on a
    * Terminal Response is OK, there should be no APDU to be resent as
    * the timer has been restarted but not expired (again). Then deactivate
    * the timer before it expires.
    * */
        printf("\n sim_stub : ste_stub_pc_timer_management_16\n\n");
        status_word.sw1 = 0x93;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word); // Status busy.

        apdu = ste_apdu_new(APDU_TimerManagement, sizeof(APDU_TimerManagement));
        if (!apdu)
        {
            printf("sim_stub : ste_stub_pc_timer_management_16, create APDU failed. \n");
            return -1;
        }

        msg = sim_stub_msg_new(SIM_STUB_MSG_PC_TIMER_MANAGEMENT, 0);
        msg->payload = apdu;
        msg->length = ste_apdu_get_raw_length(apdu);
        ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

        sleep(TIMER_S + 1); // Give the timer time to expire.

        // Following will generate a Terminal Response, but status is still busy.
        ste_stub_pc_polling_off();

        sleep(1);  // To ensure that the terminal has time to handle this PC.

        status_word.sw1 = 0x90;
        status_word.sw2 = 0x00;
        set_modem_response_status_word(status_word);  // Status OK.


    // Set status to "default" value in order not to interfere with other test cases.
    status_word.sw1 = 0x90;
    status_word.sw2 = 0x00;
    set_modem_response_status_word(status_word);

    return 0;
}

//PC refresh
void ste_stub_pc_refresh()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_Refresh[] =
    {
        0xD0, //PC tag
        0x33, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x01, // Type of command (refresh)
        0x02, // Command qualifier

        //Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        0x92, //file list
        0x0f,
        0x02,

        0x3F, //RAT path
        0x00,
        0x7f,
        0x66,
        0x5F,
        0x30,
        0x4F,
        0x36,

        0x3F,
        0x00,
        0x7F,
        0x20,
        0x6F,
        0x46,

        0xAF, //AID
        0x01,
        0x01,

        0x05,0x07,0x52,0x65,0x66,0x72,0x65,0x73,0x68, // Alpha Id

        0x1E,0x02,0x01,0x01, // Icon Id

        0x50,0x04,0x01,0x02,0x03,0x04, // Text Attribute

        0x68,0x01,0x01 //Frame id
    };

    printf("sim_stub : pc refresh test. \n");

    apdu = ste_apdu_new(APDU_Refresh, sizeof(APDU_Refresh));
    if (!apdu)
    {
        printf("sim_stub : pc refresh test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_REFRESH, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

//refresh reset ind
void ste_stub_refresh_reset_ind()
{
    sim_stub_msg_t         *msg;

    printf("sim_stub : refresh reset ind test. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_REFRESH_RESET_IND, 0);
    msg->payload = NULL;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

//Modem ready ind
void ste_stub_send_uicc_ready()
{
    sim_stub_msg_t         *msg;
    sim_stub_uicc_status_t *uicc_status_p;

    printf("sim_stub : UICC ready ind test. \n");

    //the Signal sequence to init the modem
    uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p)
    {
        return -1;
    }
    memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p->status = STE_UICC_STATUS_READY;
    msg = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, 0);
    msg->payload = uicc_status_p;
    msg->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}


//cat not ready ind
void ste_stub_cat_not_ready_ind()
{
    sim_stub_msg_t         *msg;

    printf("sim_stub : cat not ready ind test. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_UICC_NOT_READY, 0);
    msg->payload = NULL;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

//cat not supported
void ste_stub_cat_not_supported()
{
    sim_stub_msg_t         *msg;

    printf("sim_stub : cat not supported test. \n");

    msg = sim_stub_msg_new(SIM_STUB_MSG_CAT_NOT_SUPPORTED, 0);
    msg->payload = NULL;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}


//card removed ind
void ste_stub_card_removed_ind()
{
    sim_stub_msg_t                * msg;
    sim_stub_uicc_status_t        * uicc_status_p;

    //the Signal sequence to init the modem
    uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p)
    {
        return -1;
    }
    memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p->status = STE_UICC_STATUS_REMOVED_CARD;
    msg = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, 0);
    msg->payload = uicc_status_p;
    msg->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

}

//card shutdown
void ste_stub_card_shutdown()
{
    sim_stub_msg_t                * msg;
    sim_stub_uicc_status_t        * uicc_status_p;

    //the Signal sequence to init the modem
    uicc_status_p = malloc(sizeof(sim_stub_uicc_status_t));
    if (!uicc_status_p)
    {
        return -1;
    }
    memset(uicc_status_p, 0, sizeof(sim_stub_uicc_status_t));

    uicc_status_p->status = STE_UICC_STATUS_CLOSED;
    msg = sim_stub_msg_new(SIM_STUB_MSG_MAL_UICC_STATUS, 0);
    msg->payload = uicc_status_p;
    msg->length = sizeof(sim_stub_uicc_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

}


//uicc fail tests
void ste_stub_set_uicc_fail_tests()
{
    uicc_fail_tests_flag = 1;
}

//uicc fail tests
void ste_stub_reset_uicc_fail_tests()
{
    uicc_fail_tests_flag = 0;
}

// INIT And File Change, File List but no AID
void ste_stub_pc_refresh_no_aid()
  {
      sim_stub_msg_t         *msg;
      ste_apdu_t             *apdu;
      unsigned char           APDU_Refresh[] =
      {
          0xD0, //PC tag
          0x1a, // PC total length
          //Command details
          0x81, // Command details tag
          0x03, // Command details length
          0x01, // Command number
          0x01, // Type of command (refresh)
          0x02, // Command qualifier
          //Device identities
          0x82, // Device identities tag
          0x02, // Device identities length
          0x81, // Source device identity (UICC in this case)
          0x82, // Destination device identity (terminal in this case)
          0x92, //file list
          0x0f,
          0x02,
          0x3F, // Added RAT path
          0x00,
          0x7f,
          0x66,
          0x5F,
          0x30,
          0x4F,
          0x36,
          0x3F,
          0x00,
          0x7F,
          0x20,
          0x6F,
          0x46
      };
      printf("sim_stub : pc refresh test. \n");
      apdu = ste_apdu_new(APDU_Refresh, sizeof(APDU_Refresh));
      if (!apdu)
      {
          printf("sim_stub : pc refresh test, create APDU failed. \n");
          return;
      }
      msg = sim_stub_msg_new(SIM_STUB_MSG_PC_REFRESH, 0);
      msg->payload = apdu;
      msg->length = ste_apdu_get_raw_length(apdu);
      ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
  }

//PC refresh - File change but no file list
void ste_stub_pc_refresh_no_file_list()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_Refresh[] =
    {
        0xD0, //PC tag
        0x0c, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x01, // Type of command (refresh)
        0x02, // Command qualifier

        //Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        0xAF, //AID
        0x01,
        0x01
    };

    printf("sim_stub : pc refresh test. \n");

    apdu = ste_apdu_new(APDU_Refresh, sizeof(APDU_Refresh));
    if (!apdu)
    {
        printf("sim_stub : pc refresh test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_REFRESH, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

//PC refresh - UICC Reset Ind
void ste_stub_pc_refresh_opt( unsigned char opt)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_Refresh[] =
    {
        0xD0, //PC tag
        0x09, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x01, // Type of command (refresh)
        opt, // Command qualifier

        //Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82  // Destination device identity (terminal in this case)
    };

    printf("sim_stub : pc refresh test. \n");

    apdu = ste_apdu_new(APDU_Refresh, sizeof(APDU_Refresh));
    if (!apdu)
    {
        printf("sim_stub : pc refresh test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_REFRESH, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

//PC Refresh - With AID
void ste_stub_pc_refresh_opt_with_aid( unsigned char opt)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_Refresh[] =
    {
        0xD0, //PC tag
        0x0c, // PC total length

        //Command details
        0x81, // Command details tag
        0x03, // Command details length
        0x01, // Command number
        0x01, // Type of command (refresh)
        opt, // Command qualifier

        //Device identities
        0x82, // Device identities tag
        0x02, // Device identities length
        0x81, // Source device identity (UICC in this case)
        0x82, // Destination device identity (terminal in this case)

        0xAF, //AID
        0x01,
        0x01

    };

    printf("sim_stub : pc refresh test. \n");

    apdu = ste_apdu_new(APDU_Refresh, sizeof(APDU_Refresh));
    if (!apdu)
    {
        printf("sim_stub : pc refresh test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_REFRESH, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}


//stub functions for PC Refresh request
int ste_catd_modem_refresh_request(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   ste_apdu_refresh_type_t type,
                                   ste_apdu_application_id_t * app_id,
                                   ste_apdu_file_list_t * file_list)
{
  sim_stub_msg_t                * msg;

  UNUSED(ste_modem_t *, m);
  UNUSED(uintptr_t, client_tag);
  UNUSED(ste_apdu_refresh_type_t, type);
  UNUSED(ste_apdu_application_id_t *, app_id);
  UNUSED(ste_apdu_file_list_t *, file_list);

  printf("sim_stub : request refresh. \n");
  if ( sim_stub_response_type == STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE ) {
      sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
      return -1;
  }

  msg = sim_stub_msg_new(SIM_STUB_MSG_REFRESH, client_tag);
  msg->payload = 0;
  msg->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

  return 0;
}
void ste_stub_ind_modem_reset()
{
    int reset_val = MODEM_RESET_IND;
    write(pipe_wr, &reset_val, sizeof(reset_val));
}

void ste_stub_card_fallback_indication()
{
  sim_stub_msg_t         *msg;

  printf("sim_stub : card fallback indication test. \n");

  msg = sim_stub_msg_new(SIM_STUB_MSG_CARD_FALLBACK_INDICATION, 0);
  msg->payload = 0;
  msg->length = 0;
  ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

}

void ste_stub_pc_language_notification()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_LanguageNotification[] =
    {
        0xD0, 0x0D, 0x81, 0x03, 0x01, 0x35, 0x01, 0x82, 0x02,
        0x81, 0x82, 0xAD, 0x02, 0x7A, 0x68
    };

    printf("sim_stub : pc language notification test. \n");

    apdu = ste_apdu_new(APDU_LanguageNotification, sizeof(APDU_LanguageNotification));

    if (!apdu)
    {
        printf("sim_stub : pc language notification test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_LANGUAGE_NOTIFICATION, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_open_channel()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_OpenChannel[] = {
        0xD0,
        0x3D,
        0x81, 0x03, 0x01, 0x40, 0x01,
        0x82, 0x02, 0x81, 0x82,
        0x35, 0x07, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02,
        0x39, 0x02, 0x05, 0x78,
        0x47, 0x07, 0x06, 0x6F, 0x72, 0x61, 0x6E, 0x67, 0x65,
        0x0D, 0x07, 0xF4, 0x6F, 0x72, 0x61, 0x6E, 0x67, 0x65,
        0x0D, 0x07, 0xF4, 0x6F, 0x72, 0x61, 0x6E, 0x67, 0x65,
        0x3C, 0x03, 0x01, 0x21, 0xDA,
        0x3E, 0x05, 0x21, 0xC2, 0xFE, 0xA1, 0xD7
    };

    printf("sim_stub : pc open channel test. \n");

    apdu = ste_apdu_new(APDU_OpenChannel, sizeof(APDU_OpenChannel));

    if (!apdu)
    {
        printf("sim_stub : pc open channel test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_OPEN_CHANNEL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_close_channel()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_CloseChannel[] = {
        0xD0,
        0x09,
        0x81, 0x03, 0x01, 0x41, 0x00,
        0x82, 0x02, 0x81, 0x21
    };

    printf("sim_stub : pc close channel test. \n");

    apdu = ste_apdu_new(APDU_CloseChannel, sizeof(APDU_CloseChannel));

    if (!apdu)
    {
        printf("sim_stub : pc close channel test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_CLOSE_CHANNEL, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_send_data()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_SendData[] = {
        0xD0,
        0x2C,
        0x81, 0x03, 0x01, 0x43, 0x01,
        0x82, 0x02, 0x81, 0x21,
        0x36, 0x21, 0x80, 0x00, 0x00, 0x21, 0x00, 0x01, 0x04, 0x0D, 0x00, 0x00, 0xFA,
        0x59, 0x00, 0x00, 0x00, 0x01, 0x17, 0x64, 0x05, 0x78, 0x10, 0x78, 0x0A, 0x98,
        0x33, 0x10, 0x11, 0x06, 0x00, 0x00, 0x00, 0x73, 0x05
    };

    printf("sim_stub : pc send data test. \n");

    apdu = ste_apdu_new(APDU_SendData, sizeof(APDU_SendData));

    if (!apdu)
    {
        printf("sim_stub : pc send data test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_SEND_DATA, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_pc_receive_data()
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *apdu;
    unsigned char           APDU_ReceiveData[] = {
        0xD0,
        0x0C,
        0x81, 0x03, 0x01, 0x42, 0x00,
        0x82, 0x02, 0x81, 0x21,
        0xB7, 0x01, 0x2F
    };

    printf("sim_stub : pc receive data test. \n");

    apdu = ste_apdu_new(APDU_ReceiveData, sizeof(APDU_ReceiveData));

    if (!apdu)
    {
        printf("sim_stub : pc receive data test, create APDU failed. \n");
        return;
    }

    msg = sim_stub_msg_new(SIM_STUB_MSG_PC_RECEIVE_DATA, 0);
    msg->payload = apdu;
    msg->length = ste_apdu_get_raw_length(apdu);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);
}

void ste_stub_send_cat_ready()
{
    sim_stub_msg_t         *msg;
    sim_stub_cat_status_t         * cat_status_p;

    printf("sim_stub : send cat ready test. \n");

    //then send a cat ready ind
    cat_status_p = malloc(sizeof(sim_stub_cat_status_t));
    if (!cat_status_p)
    {
        return;
    }
    memset(cat_status_p, 0, sizeof(sim_stub_cat_status_t));

    cat_status_p->status = 0;

    msg = sim_stub_msg_new(SIM_STUB_MSG_MAL_CAT_STATUS, 0);
    msg->payload = cat_status_p;
    msg->length = sizeof(sim_stub_cat_status_t);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    /* Send Profile Download Error Once */
    send_terminal_profile_error = 1;
}

//####################################################################################
//stub functions for SMS
//####################################################################################


// TODO: Replace ~90 instances of UNUSED macro on a rainy day...
//
// This solution is less than optimal for two reasons:
// 1. The local UNUSED macro tries to assign to the source identifier, which
// being const causes "error: assignment of read-only location"
// 2. The much better UNUSED macro in sim_unused.h which allows unused parameters
// to be properly flagged obviously clashes with the same name.
// Replacing approx 90 instances of the UNUSED macro is for another day...

#ifndef UNUSED2
#define UNUSED2(T,X) do {T unused_var=(X); unused_var=unused_var;} while (0)
#endif

SMS_RequestStatus_t Request_SMS_CB_SessionCreate(
        int*         const  RequestResponseSocket_p,
        int*         const  EventSocket_p,
        SMS_Error_t* const  Error_p)
{
    int rc1 = -1, rc2 = -1;

    if (RequestResponseSocket_p == NULL || EventSocket_p == NULL || Error_p == NULL)
    {
        printf("sim_stub : Request_SMS_CB_SessionCreate failed. \n");
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    if  ( (gsim_stub_sms.fd_response_pipe[0] == -1) &&
          (gsim_stub_sms.fd_response_pipe[1] == -1) &&
          (gsim_stub_sms.fd_event_pipe[0] == -1)    &&
          (gsim_stub_sms.fd_event_pipe[1] == -1)     ) {

        rc1 = pipe(gsim_stub_sms.fd_response_pipe);
        rc2 = pipe(gsim_stub_sms.fd_event_pipe);

        if (!rc1  && !rc2) {
            *RequestResponseSocket_p = gsim_stub_sms.fd_response_pipe[0];
            *EventSocket_p = gsim_stub_sms.fd_event_pipe[0];
            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
            return SMS_REQUEST_OK;
        }
    }

    if (!rc1) {
        printf("sim_stub : Request_SMS_CB_SessionCreate : create response pipe failed. \n");
        close(gsim_stub_sms.fd_response_pipe[0]);
        close(gsim_stub_sms.fd_response_pipe[1]);
    }

    if (!rc2) {
        printf("sim_stub : Request_SMS_CB_SessionCreate : create event pipe failed. \n");
        close(gsim_stub_sms.fd_event_pipe[0]);
        close(gsim_stub_sms.fd_event_pipe[1]);
    }

    gsim_stub_sms.fd_response_pipe[0] = -1;
    gsim_stub_sms.fd_response_pipe[1] = -1;
    gsim_stub_sms.fd_event_pipe[0] = -1;
    gsim_stub_sms.fd_event_pipe[1] = -1;

    return SMS_REQUEST_FAILED;
}

SMS_RequestStatus_t Request_SMS_CB_SessionDestroy(
  const SMS_RequestControlBlock_t* const  RequestCtrl_p,
  const int                               EventSocket,
        SMS_Error_t*               const  Error_p)
{
    if (RequestCtrl_p == NULL || Error_p == NULL)
    {
        printf("sim_stub : Request_SMS_CB_SessionDestroy : RequestCtrl_p == NULL || Error_p == NULL. \n");
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    if  ( (gsim_stub_sms.fd_response_pipe[0] != -1)  &&
          (gsim_stub_sms.fd_response_pipe[1] != -1)  &&
          (gsim_stub_sms.fd_event_pipe[0] != -1)     &&
          (gsim_stub_sms.fd_event_pipe[1] != -1)     &&
          (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) ) {

        if ( (RequestCtrl_p->Socket == gsim_stub_sms.fd_response_pipe[0]) &&
             (EventSocket == gsim_stub_sms.fd_event_pipe[0])) {

            close(gsim_stub_sms.fd_response_pipe[0]);
            close(gsim_stub_sms.fd_response_pipe[1]);
            close(gsim_stub_sms.fd_event_pipe[0]);
            close(gsim_stub_sms.fd_event_pipe[1]);

            gsim_stub_sms.fd_response_pipe[0] = -1;
            gsim_stub_sms.fd_response_pipe[1] = -1;
            gsim_stub_sms.fd_event_pipe[0] = -1;
            gsim_stub_sms.fd_event_pipe[1] = -1;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
            return SMS_REQUEST_OK;
        }
    }
    return SMS_REQUEST_FAILED;
}

void* Util_SMS_SignalReceiveOnSocket(const int Socket, uint32_t* const Primitive_p, SMS_ClientTag_t* const ClientTag_p)
{
    int n;
    uint32_t count = 0;
    uint8_t* buffer = malloc(SIM_STUB_SMS_SIGNAL_ARRAY_SIZE);

    if ( (Socket == gsim_stub_sms.fd_response_pipe[0]) ||
         (Socket == gsim_stub_sms.fd_event_pipe[0])     ) {

        while (count < SIM_STUB_SMS_SIGNAL_ARRAY_SIZE)
        {
            n = read(Socket, (buffer + count), (SIM_STUB_SMS_SIGNAL_ARRAY_SIZE - count));
            if (n > 0) {
                count += n;
            } else {
                break;
            }
        }
    }

    if (count == SIM_STUB_SMS_SIGNAL_ARRAY_SIZE) {
        uint32_t* array = (uint32_t*)buffer;
        *Primitive_p = array[SIM_STUB_SMS_POS_PRIMITIVE];
        *ClientTag_p = array[SIM_STUB_SMS_POS_CLIENTTAG];

        return buffer;
    }

    free(buffer);
    return NULL;
}

void Util_SMS_SignalFree(void* Signal_p)
{
    free(Signal_p);
}

SMS_RequestStatus_t Request_SMS_ShortMessageSend(
  const SMS_RequestControlBlock_t* const  RequestCtrl_p,
  const SMS_SMSC_Address_TPDU_t*   const  SMSC_Address_TPDU_p,
  const SMS_Slot_t*                const  Slot_p,
  const uint8_t                           MoreToSend,
        uint8_t*                   const  SM_Reference_p,
        SMS_Error_t*               const  Error_p)
{
    uint32_t array[SIM_STUB_SMS_SIGNAL_ARRAY_LEN];
    int n;

    UNUSED2(uint8_t,MoreToSend);

    if (!RequestCtrl_p || !SMSC_Address_TPDU_p || !Error_p || Slot_p || !SM_Reference_p)
    {
      return SMS_REQUEST_FAILED_PARAMETER;
    }

    if ( (RequestCtrl_p->WaitMode != SMS_NO_WAIT_MODE)                ||
         (RequestCtrl_p->Socket != gsim_stub_sms.fd_response_pipe[0])  ) {
        return SMS_REQUEST_FAILED_APPLICATION;
    }

    memset(array, 0, SIM_STUB_SMS_SIGNAL_ARRAY_SIZE);
    array[SIM_STUB_SMS_POS_PRIMITIVE]    = RESPONSE_SMS_SHORTMESSAGESEND;
    array[SIM_STUB_SMS_POS_CLIENTTAG]    = RequestCtrl_p->ClientTag;
    array[SIM_STUB_SMS_POS_REQSTATUS]    = SMS_REQUEST_OK;
    array[SIM_STUB_SMS_POS_SMS_ERROR]    = SMS_ERROR_INTERN_NO_ERROR;
    array[SIM_STUB_SMS_POS_SM_REFERENCE] = 42;

    n = write(gsim_stub_sms.fd_response_pipe[1], array, SIM_STUB_SMS_SIGNAL_ARRAY_SIZE);

    if (n != SIM_STUB_SMS_SIGNAL_ARRAY_SIZE) {
        return SMS_REQUEST_FAILED_IPC;
    }

    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Request_SMS_SatShortMessageSend(
  const SMS_RequestControlBlock_t* const  RequestCtrl_p,
  const SMS_SMSC_Address_TPDU_t*   const  SMSC_Address_TPDU_p,
  const SMS_Slot_t*                const  Slot_p,
  const uint8_t                           MoreToSend,
        uint8_t*                   const  SM_Reference_p,
        SMS_Error_t*               const  Error_p)
{
    return Request_SMS_ShortMessageSend(RequestCtrl_p,
                                        SMSC_Address_TPDU_p,
                                        Slot_p,
                                        MoreToSend,
                                        SM_Reference_p,
                                        Error_p);
}

SMS_RequestStatus_t Response_SMS_ShortMessageSend(
  const void*            const  SigStruct_p,
        uint8_t*         const  SM_Reference_p,
        SMS_Error_t*     const  Error_p)
{
    uint32_t* array = (uint32_t*)SigStruct_p;
    SMS_RequestStatus_t ret_val;

    if (!SigStruct_p || !SM_Reference_p || !Error_p)
    {
      return SMS_REQUEST_FAILED_PARAMETER;
    }

    *SM_Reference_p = (uint8_t)     array[SIM_STUB_SMS_POS_SM_REFERENCE];

    switch( sim_stub_response_type ) {
        default:
        case STE_SIM_STUB_RSP_TYPE_SUCCESS:
            *Error_p        = (SMS_Error_t) array[SIM_STUB_SMS_POS_SMS_ERROR];
            ret_val = (SMS_RequestStatus_t)array[SIM_STUB_SMS_POS_REQSTATUS];
            break;
        case STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE:
            *Error_p = SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED;
            ret_val = SMS_REQUEST_FAILED_APPLICATION;
            break;
        case STE_SIM_STUB_RSP_TYPE_FAILURE: /* Other error */
            *Error_p = SMS_REQUEST_FAILED;
            ret_val = SMS_ERR_CS_INACTIVE;
            break;
        }
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return ret_val;
}

//####################################################################################
//stub functions for COPS
//####################################################################################
cops_return_code_t
cops_context_create(cops_context_id_t **ctxpp,
                    const cops_event_callbacks_t *event_cbs, void *event_aux)
{
    UNUSED(const cops_event_callbacks_t *, event_cbs);
    UNUSED(void *, event_aux);

    *ctxpp = malloc(1024);  //Allocated dummy memory
    return (*ctxpp) ? COPS_RC_OK : COPS_RC_IPC_ERROR;
}

cops_return_code_t cops_read_imei(cops_context_id_t *ctxp, cops_imei_t *imei)
{
    cops_imei_t test_imei  = { {'7','2','3','3','3','2','1','2','3','2','6','5','4','8','9'} };

    if (!ctxp) {
        return COPS_RC_IPC_ERROR;
    }

    memcpy(imei, &test_imei, sizeof(test_imei));

    return COPS_RC_OK;
}

void cops_context_destroy(cops_context_id_t **ctxpp)
{
    cops_context_id_t *ctx;

    if (ctxpp == NULL || *ctxpp == NULL)
        return;

    ctx = *ctxpp;
    free(ctx);
    *ctxpp = NULL;
}

//####################################################################################
//stub functions for SAP
//####################################################################################
int ste_modem_sap_session_atr_get(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_ATR_GET, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sap_session_apdu_data(ste_modem_t * m, uintptr_t client_tag, uint8_t *data, size_t length)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);
    UNUSED(uint8_t*, data);
    UNUSED(size_t, length);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_APDU_DATA, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sap_session_ctrl_warmreset_card(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_WARM_RESET_CARD, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sap_session_ctrl_coldreset_card(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_COLD_RESET_CARD, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sap_session_ctrl_deactivate_card(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_DEACT_CARD, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_sap_session_ctrl_activate_card(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t                * msg;

    UNUSED(ste_modem_t*, m);

    msg = sim_stub_msg_new(SIM_STUB_MSG_SAP_ACT_CARD, client_tag);
    msg->payload = 0;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}


//####################################################################################
//Mock for CN
//####################################################################################
/* Briefly how it works:
 *
 * When the CTAD_CN client initializes (cn_client_init called) two pipes are setup, one for
 * request/responses and another for events. The client (CATD) then request for the response
 * and event fds to listen on (the pipes[OUT])and adds them to the reader thread.
 *
 * When a cn_request_xxx function is called the cn_message_type_t (request) is written to the
 * request/response pipe. The request written to the pipe[IN] are directly available at the
 * opposite end pipe[OUT], when this happens the client calls cn_message_receive to get the actual
 * cn_message that we also create here.
 *
 */
#define OUT              0
#define IN               1

int fd_cn_response_pipe[2] = { -1, -1 };
int fd_cn_event_pipe[2] = { -1, -1 };


typedef struct {
    cn_message_type_t type;
    cn_client_tag_t   tag;
} cn_message_data_t;

//**************** Private helper functions ************

cn_message_t *create_cn_message(cn_message_type_t type, cn_error_code_t error_code, cn_client_tag_t client_tag, cn_uint32_t payload_size, void *payload_p)
{
    cn_message_t *message_p = NULL;
    int msg_size = 0;

    msg_size = sizeof(*message_p) + payload_size;
    message_p = calloc(1, msg_size);

    if (!message_p) {
        printf("calloc failed for message_p\n");
        goto error;
    }

    message_p->type         = (cn_uint32_t)type;
    message_p->client_tag   = client_tag;
    message_p->error_code   = error_code;
    message_p->payload_size = payload_size;

    if (payload_size && payload_p) {
        memcpy(message_p->payload, payload_p, payload_size);
    }

    return message_p;

error:
    return NULL;
}

cn_context_t* get_faked_cn_context_with_log(const char* f, int l)
{
    cn_context_t* c = get_faked_cn_context();
    if (!c) {
        printf("sim_stub : cn: get_faked_cn_context from %s line %d failed\n", f, l);
    }
    return c;
}

cn_context_t* get_faked_cn_context()
{
    if ( (fd_cn_response_pipe[IN]  != -1) &&
         (fd_cn_response_pipe[OUT] != -1) &&
         (fd_cn_event_pipe[IN]     != -1) &&
         (fd_cn_event_pipe[OUT]    != -1) ) {
        return (cn_context_t*)1;
    }
    return NULL;
}

cn_error_code_t send_cn_request(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag)
{
    int n;
    cn_message_data_t data = {type, client_tag};

    printf("sim_stub : cn: ENTER %s 0x%X\n", __func__, type);

    if (!context_p) {
        printf("sim_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }

    if ( sim_stub_response_type == STE_SIM_STUB_RSP_TYPE_CN_REQ_FAILURE ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return CN_FAILURE;
    }

    n = write(fd_cn_response_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("sim_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_response_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("sim_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

cn_error_code_t send_cn_event(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag)
{
    int n;
    printf("sim_stub : cn: ENTER %s 0x%X\n", __func__, type);
    cn_message_data_t data = {type, client_tag};

    if (!context_p) {
        printf("sim_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }
    n = write(fd_cn_event_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("sim_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_response_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("sim_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

cn_error_code_t generate_cn_event(cn_message_type_t type, cn_context_t *context_p)
{
    int n;
    cn_message_data_t data = {type, 0};

    printf("sim_stub : cn: ENTER %s 0x%X\n", __func__, type);

    if (!context_p) {
        printf("sim_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }
    n = write(fd_cn_event_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("sim_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_event_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("sim_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

//**************** Stub functions from cn_client.h ************

cn_error_code_t cn_client_init(cn_context_t **context_pp) {
    int rc1 = -1, rc2 = -1;

    if (!context_pp) {
        return CN_FAILURE;
    }

    if ((fd_cn_response_pipe[IN] == -1 && fd_cn_response_pipe[OUT] == -1)
            && (fd_cn_event_pipe[IN] == -1 && fd_cn_event_pipe[OUT] == -1)) {

        rc1 = pipe(fd_cn_response_pipe);
        rc2 = pipe(fd_cn_event_pipe);

        if (!rc1 && !rc2) {
            printf("sim_stub : cn: %s - response pipe in: %d out %d\n", __func__,
                    fd_cn_response_pipe[IN], fd_cn_response_pipe[OUT]);

            printf("sim_stub : cn: %s - event pipe in: %d out %d\n", __func__,
                    fd_cn_event_pipe[IN], fd_cn_event_pipe[OUT]);

            *context_pp = CN_CONTEXT;
            return CN_SUCCESS;
        }
    }

    if (!rc1) {
        close(fd_cn_response_pipe[OUT]);
        close(fd_cn_response_pipe[IN]);
    }

    if (!rc2) {
        close(fd_cn_event_pipe[OUT]);
        close(fd_cn_event_pipe[IN]);
    }

    fd_cn_response_pipe[IN] = -1;
    fd_cn_response_pipe[OUT] = -1;
    fd_cn_event_pipe[IN] = -1;
    fd_cn_event_pipe[OUT] = -1;

    *context_pp = CN_CONTEXT;
    return CN_FAILURE;
}

cn_error_code_t cn_client_get_request_fd(cn_context_t *context_p, int *fd_p) {
    UNUSED(cn_context_t*, context_p);
    if (fd_p && fd_cn_response_pipe[OUT] != -1) {
        *fd_p = fd_cn_response_pipe[OUT];
        return CN_SUCCESS;
    } else {
        printf("sim_sub : cn: %s failed\n", __func__);
        return CN_FAILURE;
    }
}

cn_error_code_t cn_client_get_event_fd(cn_context_t *context_p, int *fd_p) {
    UNUSED(cn_context_t*, context_p);
    if (fd_p && fd_cn_event_pipe[OUT] != -1) {
        *fd_p = fd_cn_event_pipe[OUT];
        return CN_SUCCESS;
    } else {
        printf("sim_sub : cn: %s failed\n", __func__);
        return CN_FAILURE;
    }
}

cn_error_code_t cn_client_shutdown(cn_context_t *context_p) {
    if (!context_p) {
        printf("sim_stub : cn: context_p is null.");
        return CN_FAILURE;
    }

    printf("sim_stub : cn: %s - response pipe in: %d out %d\n", __func__,
            fd_cn_response_pipe[IN], fd_cn_response_pipe[OUT]);

    printf("sim_stub : cn: %s - event pipe in: %d out %d\n", __func__,
            fd_cn_event_pipe[IN], fd_cn_event_pipe[OUT]);

    if (fd_cn_response_pipe[OUT] != -1) {
        close(fd_cn_response_pipe[OUT]);
        fd_cn_response_pipe[OUT] = -1;
    }

    if (fd_cn_event_pipe[OUT] != -1) {
        close(fd_cn_event_pipe[OUT]);
        fd_cn_event_pipe[OUT] = -1;
    }

    if (fd_cn_response_pipe[IN] != -1) {
        close(fd_cn_response_pipe[IN]);
        fd_cn_response_pipe[IN] = -1;
    }

    if (fd_cn_event_pipe[IN] != -1) {
        close(fd_cn_event_pipe[IN]);
        fd_cn_event_pipe[IN] = -1;
    }

    return CN_SUCCESS;
}

cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp) {
    int n;
    cn_message_data_t data   = {0, 0};
    uint32_t          count  = 0;
    uint8_t*          buffer = (uint8_t*)&data;
    cn_error_code_t   result  = CN_SUCCESS;
    static int toggle_nmr = 0;

    if (!msg_pp) {
        goto error;
    }

    if (!size_p) {
        goto error;
    }

    if (fd == fd_cn_response_pipe[OUT] || fd == fd_cn_event_pipe[OUT]) {

        while (count < sizeof(data))
        {
            n = read(fd, (buffer + count), (sizeof(data) - count));
            if (n > 0) {
                count += n;
            } else {
                break;
            }
        }

        if (count != sizeof(data)) {
            goto error;
        }

        printf("sim_stub : cn: %s - received msg 0x%X client_tag 0x%X\n", __func__, data.type, data.tag);

        switch (data.type) {
            case CN_REQUEST_REGISTRATION_STATE_NORMAL:
            {
                /* Setup test data for provide_local_info_search_mode */
                cn_registration_info_t reg_info;
                reg_info.search_mode = CN_NETWORK_SEARCH_MODE_MANUAL;
                *msg_pp = create_cn_message(CN_RESPONSE_REGISTRATION_STATE_NORMAL,
                                            CN_SUCCESS,
                                            data.tag,
                                            sizeof(reg_info),
                                            &reg_info);
                if (!msg_pp) {
                    goto error;
                }
                break;
            }
            case CN_REQUEST_NET_QUERY_MODE:
            {
                /* Setup test data here */
                cn_registration_info_t reg_info;
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                reg_info.gprs_attached = 1;
                reg_info.rat = CN_RAT_TYPE_UMTS;
                reg_info.lac = 0x003D;
                reg_info.cid = 0x0085AB54;
                (void)strncpy(reg_info.mcc_mnc, (const char *)"24001", CN_MAX_STRING_SIZE);
                *msg_pp = create_cn_message(CN_RESPONSE_NET_QUERY_MODE,
                                            CN_SUCCESS,
                                            data.tag,
                                            sizeof(reg_info),
                                            &reg_info);
                if (!msg_pp) {
                    goto error;
                }
                break;
            }
            case CN_REQUEST_DIAL:
            {
                cn_response_dial_t dial_rsp;
                memset(&dial_rsp, 0, sizeof(dial_rsp));

                switch(sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                        dial_rsp.service_type = CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL;
                        dial_rsp.data.call_id = 0;
                        *msg_pp = create_cn_message(CN_RESPONSE_DIAL, CN_SUCCESS, data.tag, sizeof(dial_rsp), &dial_rsp);
                        break;

                    case STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_DURATION:
                        dial_rsp.service_type = CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL;
                        dial_rsp.data.call_id = 1;
                        *msg_pp = create_cn_message(CN_RESPONSE_DIAL, CN_SUCCESS, data.tag, sizeof(dial_rsp), &dial_rsp);
                        break;

                    case STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE:
                        *msg_pp = create_cn_message(CN_RESPONSE_DIAL, CN_REQUEST_CC_REJECTED, data.tag, sizeof(dial_rsp), &dial_rsp);
                        break;

                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_CC_UNKNOWN:
                        dial_rsp.service_type = CN_SERVICE_TYPE_AFTER_SAT_CC_UNKNOWN;
                        dial_rsp.data.call_id = 0;
                        *msg_pp = create_cn_message(CN_RESPONSE_DIAL, CN_SUCCESS, data.tag, sizeof(dial_rsp), &dial_rsp);
                        break;
                    case STE_SIM_STUB_SEND_USDD_AFTER_CC:
                    {
                        cn_response_ussd_t ussd_rsp;
                        ussd_rsp.session_id = 1;
                        ussd_rsp.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
                        ussd_rsp.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_NONE;

                        *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(ussd_rsp), &ussd_rsp);
                        break;
                    }
                    case STE_SIM_STUB_SEND_USDD_AFTER_CC_ERROR:
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_FAILURE, data.tag, sizeof(dial_rsp), &dial_rsp);
                      break;
                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_DIAL_FAILURE:
                      *msg_pp = create_cn_message(CN_RESPONSE_DIAL, CN_FAILURE, data.tag, sizeof(dial_rsp), &dial_rsp);
                      break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                if (!msg_pp) {
                    goto error;
                }

                result = CN_SUCCESS;
                break;
            }
//        case CN_EVENT_CALL_STATE_CHANGED: {
//            cn_call_context_t call_context;
//            memset(&call_context, 0, sizeof(call_context));
//            call_context.call_state = CN_CALL_STATE_ACTIVE;
//            *msg_pp = create_cn_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, 0, sizeof(call_context), &call_context);
//            break;
//        }

            case CN_EVENT_CALL_STATE_CHANGED: {
                cn_call_context_t call_context;
                memset(&call_context, 0, sizeof(call_context));
                call_context.call_state = call_status;
                call_context.call_cause.detailed_cause_length = 4;
                call_context.transaction_id = 11;
//                (void)strncpy(call_context.number, (const char *)"123456789", 20);
//                call_context.call_cause.detailed_cause[0] = 0x23;
//                call_context.call_cause.detailed_cause[1] = 0x66;
//                call_context.call_cause.detailed_cause[2] = 0x75;
//                call_context.call_cause.detailed_cause[3] = 0x63;

                *msg_pp = create_cn_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, 0, sizeof(call_context), &call_context);
                if (!msg_pp) {
                    goto error;
                }
            }
            break;

            case CN_EVENT_CELL_INFO: {
                /* Setup test data here */
                cn_cell_info_t cell_info;
                memset(&cell_info, 0, sizeof(cell_info));

                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_WCDMA_CELL: {
                        cell_info.country_code = 1;
                        cell_info.network_code = 1;
                        cell_info.rat = CN_CELL_RAT_WCDMA;
                        cell_info.current_ac = 2;
                        cell_info.current_cell_id = 2;
                        cell_info.service_status = CN_NW_SERVICE;
                    }
                    break;

                    case STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_GPRS_CELL:
                    {
                        cell_info.country_code = 101;
                        cell_info.network_code = 111;
                        cell_info.rat = CN_CELL_RAT_GSM;
                        cell_info.current_ac = 2;
                        cell_info.current_cell_id = 2;
                        cell_info.service_status = CN_NW_SERVICE;
                        strcpy( cell_info.mcc_mnc, "101111" );
                    }
                    break;

                    case STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_LIMITED_COVERAGE:
                    {
                        cell_info.country_code = 1;
                        cell_info.network_code = 1;
                        cell_info.rat = CN_CELL_RAT_WCDMA;
                        cell_info.current_ac = 2;
                        cell_info.current_cell_id = 2;
                        cell_info.service_status = CN_NW_LIMITED_SERVICE;
                    }
                    break;

                    case STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_NO_COVERAGE:
                    {
                        cell_info.country_code = 1;
                        cell_info.network_code = 1;
                        cell_info.rat = CN_CELL_RAT_WCDMA;
                        cell_info.current_ac = 2;
                        cell_info.current_cell_id = 2;
                        cell_info.service_status = CN_NW_NO_COVERAGE;
                    }
                    break;
                }
                // Not resetting here, as this will be done in the event list handler

                *msg_pp = create_cn_message(CN_EVENT_CELL_INFO, CN_SUCCESS, 0, sizeof(cell_info), &cell_info);
                if (!msg_pp) {
                    goto error;
                }
            }
            break;

            case CN_EVENT_MODEM_REGISTRATION_STATUS: {
                cn_registration_info_t reg_info;
                memset(&reg_info, 0, sizeof(reg_info));
                if (sim_stub_response_type == STE_SIM_STUB_SEND_CN_REG_INFO_MANUAL_MODE) {
                    reg_info.rat = CN_RAT_TYPE_GSM;
                    reg_info.search_mode = CN_NETWORK_SEARCH_MODE_MANUAL;
                } else {
                    reg_info.rat = CN_RAT_TYPE_HSDPA;
                    reg_info.search_mode =CN_NETWORK_SEARCH_MODE_AUTOMATIC;
                }
                // Not resetting here, as this will b e done in  the event list handler

                *msg_pp = create_cn_message(CN_EVENT_MODEM_REGISTRATION_STATUS, CN_SUCCESS, 0, sizeof(reg_info), &reg_info);
                if (!msg_pp) {
                    goto error;
                }
            }
            break;

            case CN_REQUEST_TIMING_ADVANCE:
            {
                cn_timing_advance_info_t ta = {CN_GSS_TIMING_ADVANCE_VALID, 42};
                *msg_pp = create_cn_message(CN_RESPONSE_TIMING_ADVANCE, CN_SUCCESS, data.tag, sizeof(ta), &ta);
                break;
            }
            case CN_REQUEST_USSD:
            {
                cn_response_ussd_t rsp_p;
                rsp_p.session_id = 1;
                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                    {
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
                      rsp_p.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_NONE;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE: /* Call Control Rejected */
                    {
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      rsp_p.ss_error.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_NONE;

                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_REQUEST_CC_REJECTED, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_CC_TEMP_FAILURE: /* Modem not ready */
                    {
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      rsp_p.ss_error.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_NONE;

                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_REQUEST_MODEM_NOT_READY, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_ModMM_FAILURE: /* MM error code */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_GSM_MM_ERROR;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_MM;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_ModCC_FAILURE: /* CC error code */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_GSM;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);

                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_SS_BUSY_FAILURE: /* CN internal error code : SS busy */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_BUSY;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_SS_NOT_AVAILABLE_FAILURE: /* CN internal error code : SS not available */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;

                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_USSD_BUSY_FAILURE: /* CN internal error code : Ussd Busy */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_USSD_BUSY;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_FAILURE: /* CN internal error code : General Error */
                    {
                      rsp_p.ss_error.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_MM_ERROR;
                      rsp_p.ss_error.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      *msg_pp = create_cn_message(CN_RESPONSE_USSD, CN_SUCCESS, data.tag, sizeof(rsp_p), &rsp_p);
                    }
                    break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS; /* Reset to default */
                break;
            }
            case CN_REQUEST_NMR_INFO:
            {
                cn_nmr_info_t nmr;
                int i;
                //Either GERAN or UTRAN
            if (toggle_nmr == 1) {
                toggle_nmr = 0;
                nmr.rat = CN_NMR_RAT_TYPE_GERAN;

                for(i = 0; i < 16; i++) {
                nmr.nmr_info.geran_info.measurement_results[i] = 0;
                }

                nmr.nmr_info.geran_info.measurement_results[0] = 52;
                nmr.nmr_info.geran_info.measurement_results[1] = 52;

                nmr.nmr_info.geran_info.num_of_channels = 10;
                nmr.nmr_info.geran_info.ARFCN_list[0] = 561;
                nmr.nmr_info.geran_info.ARFCN_list[1] = 565;
                nmr.nmr_info.geran_info.ARFCN_list[2] = 568;
                nmr.nmr_info.geran_info.ARFCN_list[3] = 569;
                nmr.nmr_info.geran_info.ARFCN_list[4] = 573;
                nmr.nmr_info.geran_info.ARFCN_list[5] = 575;
                nmr.nmr_info.geran_info.ARFCN_list[6] = 577;
                nmr.nmr_info.geran_info.ARFCN_list[7] = 581;
                nmr.nmr_info.geran_info.ARFCN_list[8] = 582;
                nmr.nmr_info.geran_info.ARFCN_list[9] = 585;
            }else {
                toggle_nmr = 1;
                nmr.rat = CN_NMR_RAT_TYPE_UTRAN;
                nmr.nmr_info.utran_info.mm_report_len = 8;
                nmr.nmr_info.utran_info.measurements_report[0] = 0x56;
                nmr.nmr_info.utran_info.measurements_report[1] = 0x78;
                nmr.nmr_info.utran_info.measurements_report[2] = 0x44;
                nmr.nmr_info.utran_info.measurements_report[3] = 0x45;
                nmr.nmr_info.utran_info.measurements_report[4] = 0x46;
                nmr.nmr_info.utran_info.measurements_report[5] = 0x47;
                nmr.nmr_info.utran_info.measurements_report[6] = 0x48;
                nmr.nmr_info.utran_info.measurements_report[7] = 0x98;
            }

                *msg_pp = create_cn_message(CN_RESPONSE_NMR_INFO, CN_SUCCESS, data.tag, sizeof(nmr), &nmr);
            }
            break;
            case CN_EVENT_USSD:
            {
                cn_ussd_info_t ussd_info;

                ussd_info.type = CN_USSD_TYPE_USSD_SESSION_TERMINATED;

                ussd_info.received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_END;
                ussd_info.length = 4;
                ussd_info.dcs = 0;
                ussd_info.session_id = 1;
                ussd_info.me_initiated = 0;

                memmove(ussd_info.ussd_string, (const char *)"USSD", 4);

                *msg_pp = create_cn_message(CN_EVENT_USSD, CN_SUCCESS, data.tag, sizeof(ussd_info), &ussd_info);
                break;
            }
            case CN_REQUEST_SS:
            {
                cn_ss_response_t ss;
                //                ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
                //                ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                //                ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_GSM;

                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS: /* Success Response */
                    {
                      ss.error_code.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_NONE;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
                      ss.additional_result.length = 1;
                      ss.additional_result.additional_result_data[0] = 0x12;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE: /* Call Control Rejected */
                    {
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.error_code.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_NONE;
                      ss.additional_result.length = 0;

                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_REQUEST_CC_REJECTED, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_CC_TEMP_FAILURE: /* Modem not ready */
                    {
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.error_code.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_NONE;
                      ss.additional_result.length = 0;

                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_REQUEST_MODEM_NOT_READY, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_ModMM_FAILURE: /* MM error code */
                    {
                      ss.error_code.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_GSM_MM_ERROR;
                      
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_MM;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_ModCC_FAILURE: /* CC error code */
                    {
                      ss.error_code.cn_ss_error_value.cause_value = CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_GSM;
                      ss.additional_result.length = 0;
                      ss.additional_result.additional_result_data[0] = 0x12;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_SS_BUSY_FAILURE: /* CN internal error code : SS busy */
                    {
                      ss.error_code.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_BUSY;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.additional_result.length = 0;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_SS_NOT_AVAILABLE_FAILURE: /* CN internal error code : SS not available */
                    {
                      ss.error_code.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.additional_result.length = 0;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_USSD_BUSY_FAILURE: /* CN internal error code : Ussd Busy */
                    {
                      ss.error_code.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_USSD_BUSY;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.additional_result.length = 0;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                    case STE_SIM_STUB_RSP_TYPE_FAILURE: /* CN internal error code : General Error */
                    {
                      ss.error_code.cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_MM_ERROR;
                      ss.error_code.cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
                      ss.additional_result.length = 0;
                      *msg_pp = create_cn_message(CN_RESPONSE_SS, CN_SUCCESS, data.tag, sizeof(ss), &ss);
                    }
                    break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS; /* reset response type to 0 */
                break;
            }

            case CN_REQUEST_CURRENT_CALL_LIST:
            {
                cn_call_list_t call_list;
                cn_call_context_t call_context;

                call_context.call_state = call_status;
                call_context.call_id = 1;

                call_list.nr_of_calls = 1;
                if ( call_status == CN_CALL_STATE_INCOMING ) {
                    call_context.is_MT = 1;
                    strcpy( call_context.number, "12345" );
                }
                call_list.call_context[0] = call_context;
                *msg_pp = create_cn_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, data.tag,
                        sizeof(call_list), &call_list);
                result = CN_SUCCESS;
                break;
            }

            case CN_REQUEST_HANGUP:
            {
                switch( sim_stub_response_type ) {
                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_HANGUP_FAILURE:
                        *msg_pp = create_cn_message(CN_RESPONSE_HANGUP, CN_FAILURE, data.tag, 0, NULL);
                        break;
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                    default:
                        call_status = CN_CALL_STATE_IDLE;
                        *msg_pp = create_cn_message(CN_RESPONSE_HANGUP, CN_SUCCESS, data.tag, 0, NULL);
                        result = CN_SUCCESS;
                        break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
            }

            case CN_REQUEST_DTMF_SEND:
            {
                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS: /* Success */
                        *msg_pp = create_cn_message(CN_RESPONSE_DTMF_SEND, CN_SUCCESS, data.tag, 0, NULL);
                        break;
                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_FAILURE:
                        *msg_pp = create_cn_message(CN_RESPONSE_DTMF_SEND, CN_REQUEST_NOT_ALLOWED, data.tag, 0, NULL);
                        break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
                break;
            }

            case CN_REQUEST_HOLD_CALL:
            {
                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                        call_status = CN_CALL_STATE_HOLDING;
                        *msg_pp = create_cn_message(CN_RESPONSE_HOLD_CALL, CN_SUCCESS, data.tag, 0, NULL);
                        result = CN_SUCCESS;
                        break;
                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_HOLD_FAILURE:
                        *msg_pp = create_cn_message(CN_RESPONSE_HOLD_CALL, CN_FAILURE, data.tag, 0, NULL);
                        break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
            }
            case CN_REQUEST_CELL_INFO:
            {
                cn_cell_info_t cell_info;

                cell_info.rat = CN_NMR_RAT_TYPE_UTRAN;
                cell_info.service_status = CN_NW_SERVICE;
                cell_info.current_ac = 0x003D;
                cell_info.current_cell_id = 0x0085AB54;

                (void)strncpy(cell_info.mcc_mnc, (const char *)"24001", CN_MAX_STRING_SIZE);

                switch( sim_stub_response_type ) {
                    default:
                    case STE_SIM_STUB_RSP_TYPE_SUCCESS:
                    *msg_pp = create_cn_message(CN_RESPONSE_CELL_INFO, CN_SUCCESS, data.tag, sizeof(cell_info), &cell_info);
                    break;
                    case STE_SIM_STUB_RSP_TYPE_CN_RESP_FAILURE:
                    *msg_pp = create_cn_message(CN_RESPONSE_CELL_INFO, CN_FAILURE, data.tag, sizeof(cell_info), &cell_info);
                    break;
                }
                sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
            }
            break;

            default: {
                result = CN_FAILURE;
                break;
            }
        }
    } else {
        goto error;
    }

    *size_p = 0;
    return result;

error:
    printf("sim_sub : cn: %s error\n", __func__);
    return CN_FAILURE;
}

cn_error_code_t cn_request_ussd(cn_context_t *context_p, cn_ussd_info_t *cn_ussd_info_p, cn_client_tag_t client_tag)
{
    UNUSED(cn_ussd_info_t*, cn_ussd_info_p);
    return send_cn_request(CN_REQUEST_USSD, context_p, client_tag);
}

cn_error_code_t cn_request_registration_state_normal(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    return send_cn_request(CN_REQUEST_REGISTRATION_STATE_NORMAL, context_p, client_tag);
}

cn_error_code_t cn_request_net_query_mode(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    return send_cn_request(CN_REQUEST_NET_QUERY_MODE, context_p, client_tag);
}

cn_error_code_t cn_request_dial(cn_context_t *context_p, cn_dial_t *dial_p, cn_client_tag_t client_tag)
{
    cn_uint8_t send_ussd_evt = 0;

    UNUSED(cn_dial_t *, dial_p);

    if ( STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_FAILURE == sim_stub_response_type ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return CN_FAILURE;
    }
    if ( sim_stub_response_type == STE_SIM_STUB_SEND_USDD_AFTER_CC ) {
         send_ussd_evt = 1;
    }

    if (send_cn_request(CN_REQUEST_DIAL, context_p, client_tag) == CN_SUCCESS) {
        if ( send_ussd_evt == 0 ) {
            call_status = CN_CALL_STATE_ACTIVE;
            return send_cn_request(CN_EVENT_CALL_STATE_CHANGED, context_p, 0);
        }
        return send_cn_event(CN_EVENT_USSD, CN_CONTEXT, 0);
    }
    return CN_FAILURE;
}

cn_error_code_t cn_request_current_call_list(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    if ( STE_SIM_STUB_RSP_TYPE_CN_REQ_CALL_LIST_FAILURE == sim_stub_response_type ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return CN_FAILURE;
    }
    return send_cn_request(CN_REQUEST_CURRENT_CALL_LIST, context_p, client_tag);
}

cn_error_code_t cn_request_hangup(cn_context_t *context_p, cn_call_state_filter_t filter, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    UNUSED(cn_call_state_filter_t, filter);
    UNUSED(cn_uint8_t, call_id);

    if ( STE_SIM_STUB_RSP_TYPE_CN_REQ_HANGUP_FAILURE == sim_stub_response_type ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return CN_FAILURE;
    }
    if(send_cn_request(CN_REQUEST_HANGUP, context_p, client_tag) == CN_SUCCESS) {
        call_status = CN_CALL_STATE_IDLE;
        return send_cn_request(CN_EVENT_CALL_STATE_CHANGED, context_p, 0);
    }
    return CN_FAILURE;
}

cn_error_code_t cn_request_hold_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    UNUSED( cn_uint8_t, active_call_id);

    if ( STE_SIM_STUB_RSP_TYPE_CN_REQ_HOLD_FAILURE == sim_stub_response_type ) {
        sim_stub_response_type = STE_SIM_STUB_RSP_TYPE_SUCCESS;
        return CN_FAILURE;
    }
    if( send_cn_request(CN_REQUEST_HOLD_CALL, context_p, client_tag) == CN_SUCCESS ) {
          call_status = CN_CALL_STATE_HOLDING;
          return send_cn_request(CN_EVENT_CALL_STATE_CHANGED, context_p, 0);
    }
    return CN_FAILURE;
}

cn_error_code_t cn_request_dtmf_send(cn_context_t *context_p,
                                     char *dtmf_string_p,
                                     cn_uint16_t length,
                                     cn_dtmf_string_type_t string_type,
                                     cn_uint16_t dtmf_duration_time,
                                     cn_uint16_t dtmf_pause_time,
                                     cn_client_tag_t client_tag)
{
    UNUSED(char*,                 dtmf_string_p);
    UNUSED(cn_uint16_t,           length);
    UNUSED(cn_dtmf_string_type_t, string_type);
    UNUSED(cn_uint16_t,           dtmf_duration_time);
    UNUSED(cn_uint16_t,           dtmf_pause_time);

    return send_cn_request(CN_REQUEST_DTMF_SEND, context_p, client_tag);
}

cn_error_code_t cn_get_timing_advance_value(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    return send_cn_request(CN_REQUEST_TIMING_ADVANCE, context_p, client_tag);
}

cn_error_code_t cn_request_nmr_info(cn_context_t *context_p, cn_nmr_rat_type_t rat, cn_nmr_utran_type_t utran, cn_client_tag_t client_tag)
{
    UNUSED(cn_nmr_rat_type_t,   rat);
    UNUSED(cn_nmr_utran_type_t, utran);
    return send_cn_request(CN_REQUEST_NMR_INFO, context_p, client_tag);
}

cn_error_code_t cn_request_ss(cn_context_t *context_p, cn_ss_command_t ss_command, cn_client_tag_t client_tag)
{
    UNUSED(cn_ss_command_t, ss_command);
    return send_cn_request(CN_REQUEST_SS, context_p, client_tag);
}

cn_error_code_t cn_request_cell_info(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    return send_cn_request(CN_REQUEST_CELL_INFO, context_p, client_tag);
}


//################################ END CN MOCK


int ste_mal_card_status(ste_mal_card_status_data_t *data)
{
  data->num_apps = 1;
  data->card_type = STE_UICC_CARD_TYPE_UICC;
  data->card_status = STE_SIM_CARD_STATUS_READY;
  return 0;
}

// Return realistic values for common case: active umts app
// pin enabled verified for pin and pin enabled only for pin2

static uint8_t app_status_aid[MAX_AID_LEN] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static char *app_status_label = "Simulation";

int ste_mal_app_status(int app_index, ste_mal_app_status_data_t *data)
{
  if (app_index != 0) return -1;

  data->app_type = STE_SIM_APP_TYPE_USIM;
  data->app_status = STE_SIM_APP_STATUS_ACTIVE;
  data->pin_mode = STE_SIM_PIN_MODE_NORMAL;
  data->activated_app = 1;
  data->aid = app_status_aid;
  data->aid_len = sizeof app_status_aid;
  data->label = app_status_label;
  data->label_len = strlen(app_status_label);
  data->pin_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
  data->pin2_state = STE_SIM_PIN_STATUS_ENABLED;
  return 0;
}

int ste_modem_application_select(ste_modem_t *m, uintptr_t client_tag)
{
    UNUSED(ste_modem_t *,m);
    UNUSED(uintptr_t,client_tag);
    return 0;
}

int ste_modem_channel_send(ste_modem_t * m,
                           uintptr_t client_tag,
                           uint16_t session_id,
                           size_t apdu_len,
                           uint8_t *apdu,
                           uint8_t *path,
                           uint8_t path_length)
{
    sim_stub_msg_t         *msg;
    ste_apdu_t             *p_apdu;
    sim_stub_sim_channel_send_response_t    *p;

    UNUSED(ste_modem_t *,m);
    UNUSED(uint8_t *,path);
    UNUSED(uint8_t,path_length);

    printf("sim_stub : ste_modem_channel_send test. \n");

    p = malloc( sizeof *p );
    if (!p)
    {
        printf("sim_stub : ste_modem_channel_send, create ctx\n");
        return -1;
    }

    p_apdu = ste_apdu_new(apdu, apdu_len);
    if (!p_apdu)
    {
        printf("sim_stub : ste_modem_channel_send, create APDU failed. \n");
        free(p);
        return -1;
    }

    p->apdu = p_apdu;
    p->session_id = session_id;

    msg = sim_stub_msg_new(SIM_STUB_MSG_SIM_CHANNEL_SEND, client_tag);
    msg->payload = p;
    msg->length = sizeof(*p);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_channel_open(ste_modem_t * m, uintptr_t client_tag, size_t aid_len, uint8_t *aid)
{
    static uint16_t         session_id = 0x100;
    sim_stub_msg_t         *msg;
    sim_stub_sim_channel_open_response_t    *p;

    UNUSED(ste_modem_t *,m);
    UNUSED(size_t ,aid_len);
    UNUSED(uint8_t *,aid);

    printf("sim_stub : ste_modem_channel_open test. \n");

    if (uicc_fail_tests_flag) return -1;

    p = malloc( sizeof *p );
    if (!p)
    {
        printf("sim_stub : ste_modem_channel_open, create ctx\n");
        return -1;
    }

    p->session_id = session_id++;
    msg = sim_stub_msg_new(SIM_STUB_MSG_SIM_CHANNEL_OPEN, client_tag);

    msg->payload = p;
    msg->length = sizeof(*p);
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_modem_channel_close(ste_modem_t * m, uintptr_t client_tag, uint16_t session_id)
{
    sim_stub_msg_t         *msg;


    UNUSED(ste_modem_t *,m);
    UNUSED(uint16_t ,session_id);

    printf("sim_stub : ste_modem_channel_close test. \n");

    if (uicc_fail_tests_flag) return -1;

    msg = sim_stub_msg_new(SIM_STUB_MSG_SIM_CHANNEL_CLOSE, client_tag);
    msg->payload = NULL;
    msg->length = 0;
    ste_msgq_add(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

void ste_modem_set_debug_level(ste_modem_t * m, uintptr_t client_tag, int level)
{
    UNUSED(ste_modem_t *,m);
    UNUSED(uintptr_t ,client_tag);
    UNUSED(int ,level);
}

void ste_modem_reset_backend_state(ste_modem_t * m, uintptr_t client_tag)
{
    UNUSED(ste_modem_t *,m);
    UNUSED(uintptr_t ,client_tag);
}

int ste_catd_modem_adapt_terminal_support_table_update(ste_modem_t * m,
                                             uintptr_t client_tag,
                                             int offset,
                                             const uint8_t* data,
                                             size_t data_size)
{
    sim_stub_msg_t *msg;
    UNUSED(ste_modem_t *,m);
    UNUSED(int, offset);
    UNUSED(const uint8_t*, data);
    UNUSED(size_t, data_size);

    msg = sim_stub_msg_new(SIM_STUB_MSG_ADAPT_DL, client_tag);
    msg->payload = NULL;
    msg->length = 0;
    ste_msgq_push(gStubData.mq, (ste_msg_t *) msg);

    return 0;
}

int ste_catd_modem_poll_req(ste_modem_t * m, uintptr_t client_tag)
{
    sim_stub_msg_t *msg;
    sim_stub_cat_cat_response_t   * cat_rsp_p;
    ste_apdu_t                    * cc_apdu_p;
    unsigned char           APDU_CC_RSP[] =
    {
        0x90, 0x00 // Status words
    };

    UNUSED(ste_modem_t *,m);

    cat_rsp_p = malloc(sizeof(sim_stub_cat_cat_response_t));
    if (!cat_rsp_p)
    {
        return -1;
    }

    memset(cat_rsp_p, 0, sizeof(sim_stub_cat_cat_response_t));
    cat_rsp_p->status = 0;

    // Check if we want to test on status word != 0x9000.
    if (modem_response_status_word.sw1 != 0x90) {
        APDU_CC_RSP[0] = modem_response_status_word.sw1;
        APDU_CC_RSP[1] = modem_response_status_word.sw2;
    }

    cc_apdu_p = ste_apdu_new(APDU_CC_RSP, sizeof(APDU_CC_RSP));
    if (!cc_apdu_p)
    {
        printf("sim_stub : ste_catd_modem_tr, create APDU failed. \n");
        free(cat_rsp_p);
        return -1;
    }
    cat_rsp_p->apdu = cc_apdu_p;

    msg = sim_stub_msg_new(SIM_STUB_MSG_POLL_REQ, client_tag);
    msg->payload = cat_rsp_p;
    msg->length = sizeof(sim_stub_cat_cat_response_t);
    ste_msgq_push(gStubData.mq, (ste_msg_t *) msg);

    return 0;

}

void ste_modem_uicc_status_info(ste_modem_t * m, ste_uicc_status_t *status)
{
    UNUSED(ste_modem_t *, m);
    *status = STE_UICC_STATUS_READY;
    return;
}

void ste_modem_set_app_type_to_sim()
{
    current_app_type = STE_SIM_APP_TYPE_SIM;
}

void ste_modem_set_pin_enabled()
{
    ste_sim_stub_pin_enabled = 1;

}

util_void_t util_continue_as_non_privileged_user()
{

}

util_void_t util_set_restricted_umask()
{

}

int calculate_log_type_index(util_log_type_t log_type)
{
    unsigned int target = 0;
    unsigned int index = 0;

    if (0 == log_type || UTIL_LOG_TYPE_MAX_VALUE < log_type) {
        goto exit;
    }

    while (target < log_type) {
        target = (int)pow(2, index);
        index++;
    }

exit:
    return index;
}

util_void_t util_print_log(util_log_module_t module, util_log_type_t module_log_level, util_log_type_t log_type,
    const char *func_name_p, const char *format_p, ...)
{

    va_list arg_list;
    char *new_format_p = NULL;
    char *log_type_p   = NULL;
    char *LOG_TAG      = NULL; /* no _p suffix can be used here */

    /* check if the log type is permitted according to log level */
    if ((log_type & module_log_level) == 0) {
        goto exit;
    }

    new_format_p = (char *) calloc(1, MAX_SIZE_OF_FORMAT_STRING);
    if (!new_format_p) {
        goto exit;
    }

    va_start(arg_list, format_p);

    /* prepare format string with all information */
    log_type_p = log_type_string[calculate_log_type_index(log_type)];

    /* print to standard output */
    snprintf(new_format_p, MAX_SIZE_OF_FORMAT_STRING, "%s, %s: %s\n",
             log_type_p, func_name_p, format_p);
    vprintf(new_format_p, arg_list);

exit:
    if (new_format_p) {
        free(new_format_p);
    }
}

util_log_type_t util_read_log_file(util_log_module_t module)
{
    UNUSED(util_log_module_t, module);
    return UTIL_LOG_TYPE_VERBOSE;
}
