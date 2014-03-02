/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_mal_modem.c
 * Description     : Sim modem model realisation.
 *                   Implements the interface modem.h for mal-uicc.h
 *
 * Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
 *
 */

/* Setting this flag prevents mal calls to write to sim,
   and success return code */
//#define DONT_WRITE_TO_SIM


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sim_mal_defs.h"

#include "simd.h" // For event stream, logging functions etc.
#include "uiccd.h" // Signals to uiccd
#include "catd_modem.h" // File implements this interface
#include "catd.h"

#include "mal_uicc.h" // File uses this to access MAL
#include "mal_uicc_apdu.h"
#include "mal_utils.h" // File which defines error type
#include "sim_file_path.h"

#include "sim_trace_util.h"
#include "shm_netlnk.h"

static int ste_mtbl_i_modem_card_info_th();

static int lookup_application_index(uint16_t aid_len,
                                    uint8_t *aid,
                                    uint8_t *app_index,
                                    uint8_t *app_type);

#define FILE_SIM_VOLTAGE "/sys/devices/platform/sim-detect.0/voltage"

pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex used to thread-safe sim_mal (since MAL is not thread-safe)

struct ste_modem_s {
  int                     mal_fd;     // -1 indicates modem is not connected
  int                     shmnetlnk_fd;     // -1 indicates modem is not connected
  int                     modem_reset_status; // -1 indicates modem reset in not indicated
};

#define MAX_APPS                    8
#define MAX_AID_SIZE                16
#define MAX_CHANNELS                20
#define CHANNEL_ID_FREE             0xFFu
#define CHANNEL_ID_UPDATING         0xFEu
#define CHANNEL_ID_UPDATING_ISIM    0xFDu
#define INVALID_CHANNEL_SESSION     0xFFFFu

//STRINGIZE is a standard macro for converting macro parameter into a string constant.
//The STRINGIZE_VALUE_OF will evaluate down to the final definition of a macro.
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#define DETECT_PATH STRINGIZE_VALUE_OF(SET_DETECT_PATH)

// Constants to be used as tag values in ste_cmd_tag_s
typedef enum {
    STE_CMD_FILEINFO_INFO_TAG,
    STE_CMD_FILEINFO_FORMAT_TAG,
    STE_CMD_TPDL_TAG,
    STE_CMD_ENVELOPE_COMMAND_TAG,
    STE_CMD_TERMINAL_RESPONSE_TAG,
    STE_CMD_MODEM_ENABLE_TAG,
    STE_CMD_MODEM_DISABLE_TAG,
    STE_CMD_REFRESH_TAG,
    STE_CMD_APPLICATION_APDU_TAG,
    STE_CMD_PIN_INFO_PIN_PUK,
    STE_CMD_PIN_INFO_PIN_ONLY,
    STE_CMD_PIN_INFO_PUK_ONLY,
    STE_CMD_APPL_LIST_VANILLA_TAG,
    STE_CMD_APPL_LIST_SELECT_TAG,
    STE_CMD_APPL_ACTIVATE_STARTUP,
    STE_CMD_APPL_DEACTIVATE_STARTUP,
    STE_CMD_APDU_SAP,
    STE_CMD_APDU_NORMAL_OPEN,
    STE_CMD_APDU_NORMAL_OPEN_ACTIVATED,
    STE_CMD_APDU_NORMAL_SEND,
    STE_CMD_APDU_NORMAL_CLOSE,
    STE_CMD_APDU_ERROR_CLOSE,
    STE_CMD_REFRESH_IND_PERMISSION,
    STE_CMD_REFRESH_IND_NOW,
    STE_CMD_ADAPT_TAG,
    STE_CMD_READ_BINARY_VANILLA_TAG,
    STE_CMD_ISIM_HOST_ACTIVATE,
    STE_CMD_ISIM_DEACTIVATE
} ste_cmd_tag_type_t;

// Client tag used to distinguish file format from file info since they are
// mapped to the same MAL call.
struct ste_cmd_tag_s {
  int tag;
  void *ct;
};

// Declarations for Adapt
#define ADAPT_EF_TERMINAL_SIM_PATH_LEN 4
#define ADAPT_EF_TERMINAL_USIM_PATH_LEN 6
#define ADAPT_EF_TERMINAL_FILE_SIZE 8

const uint16_t adapt_ef_terminal_file_id = 0x6fd2;

const uint8_t adapt_ef_terminal_sim_path[ADAPT_EF_TERMINAL_SIM_PATH_LEN] = {
    0x3f, 0x00, 0x7f, 0x66 };

const uint8_t adapt_ef_terminal_usim_path[ADAPT_EF_TERMINAL_USIM_PATH_LEN] = {
    0x3f, 0x00, 0x7f, 0xff, 0x7f, 0x66 };

// Fixed APDU templates for channel open/close
static const uint8_t  manage_channel_open[] = {
    0x00,   // CLA
    0x70,   // INS=Manage Channel
    0x00,   // P1: 0x00 = Open Channel
    0x00,   // P2: unused
            // Lc is unused
            // No command data
    0x01    // Le is an response length (just expecting a single byte)
};
// This contains all the bytes for the activate application APDU,
// except for the AID string itself
static const uint8_t select_app[] = {
    0x00,   // CLA  - to be modified
    0xA4,   // INS=Select
    0x04,   // P1=Select by AID
    0x00,   // was 0x0C,   // P2=Return Nothing
    0x00,   // Lc of data
    0x00,   // Le of response (allow any length of response)
};
static const uint8_t  manage_channel_close[] = {
    0x00,   // CLA
    0x70,   // INS=Manage Channel
    0x80,   // P1: 0x80 = Close Channel
    0x00,   // P2: channel ID (set in code)
            // Lc is unused
            // No command data
            // Le is unused
};

// Client tag used in the manage channel / activate application sequence.
struct ste_channel_app_tag_s {
    void     *request_ct;           //< The original application context
    uint16_t  session_id;           //< Obtained session ID
};

// Client tag used in ISIM activation/deactivation sequence.
struct ste_channel_isim_app_tag_s {
    void     *request_ct;           //< The original application context
    uint16_t  session_id;           //< Obtained session ID
    uint8_t   app_index;            //<-Application index in the list
};

/*
 * Data type used to store the interval set (needs to be returned in the response)
 * and the "original" client tag.
 */
typedef struct  {
    uint16_t interval;      // Interval in seconds
    void*    client_tag;    // The client tag specified by the client.
} poll_interval_tag_t;

/*
 * Data type used to store the "original" client tag.
 */
typedef struct  {
    void*    client_tag;    // The client tag specified by the client.
} poll_req_tag_t;

// -----------------------------------------------------------------------------
// Thread data

struct ste_mtbl_thread_data_s {
    int                     mal_fd;     // -1 indicates modem is not connected
};
typedef struct ste_mtbl_thread_data_s ste_mtbl_thread_data_t;

// Static data as Mal does not have a user_data argument. /Mats
// This is ONLY Used in
//      ste_catd_mtbl_mal_callback
//      ste_catd_mtbl_es_parser_func
static ste_modem_t *static_data = 0;

typedef struct {
  uint8_t       channel_id;
  size_t        aid_len;
  uint8_t       aid[MAX_AID_SIZE];
} channel_info_t;

// State values to be used while in the startup phase (STATE_TPDL)
#define CAT_TPDL_SUBSTATE_INIT       0 // Initial value
#define CAT_TPDL_SUBSTATE_TPDL_DONE  1 // When REG_OK is received
#define CAT_TPDL_SUBSTATE_ADAPT_DONE 2 // When file update cb received with OK
#define CAT_TPDL_SUBSTATE_FAIL       3 // If any step failed.

// Fields with X in comment need mutex when set
static struct {
    pthread_mutex_t   mtx;                    // Thread-safe card struct
    uint8_t           got_card_status;        // 0 if initial card state not got, 1 when got
    uint8_t           got_cat;                // 1 when tpdl ok, 2 when adapt ok, 3 if fail
    uint8_t           status;                 // X
    uint8_t           type;                   // X
    uint8_t           cat_ind_type;           // X card_type in the cat_ind
    uint8_t           startup_complete;
    uint8_t           number_of_apps;         // X Actual number of apps found on card
    uint8_t           i;                      // X Index to element that is active.
    uint8_t           reject_cause; // The reject cause
    uint8_t           refresh_client_id;      // Active client, used in refresh.
    uint8_t           clf_support;  // UICC CLF IF support
    channel_info_t    channels[MAX_CHANNELS]; // X
} card = {
    PTHREAD_MUTEX_INITIALIZER,
    0,
    CAT_TPDL_SUBSTATE_INIT,
    MAL_UICC_STATUS_UNKNOWN,
    MAL_UICC_CARD_TYPE_UNKNOWN,
    MAL_UICC_CARD_TYPE_UNKNOWN,
    0,
    0,
    MAX_APPS,
    0,
    0,
    0,
    {
        { 0, 0, { 0 } },                // Channel 0 is reserved and in use
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },

        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },

        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },

        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
        { CHANNEL_ID_FREE, 0, { 0 } },
    }
};

static void reset_card_state()
{
  uint8_t i;

  pthread_mutex_lock(&card.mtx);
  card.got_card_status = 0;
  card.type = 0;
  card.cat_ind_type = 0;
  card.startup_complete = 0;
  card.number_of_apps = 0;
  card.i = MAX_APPS;
  card.clf_support =0;
  memset(card.channels, 0, sizeof(card.channels));

  for (i = 1; i < MAX_CHANNELS; i++) {
    card.channels[i].channel_id = CHANNEL_ID_FREE;
  }

  pthread_mutex_unlock(&card.mtx);
}

/*
 * Struct records activated type of app, app id and the pin id that
 * protects it during the various stages (state) at startup.
 * The state is used to straighten out the race condition between our
 * activation and the modem built-in activation.
 */

// TODO: Suggest making int pin_id[2];
// For ICC, fill in the values from the appropriate CHV request
// For UICC, hard-code to 0x01 and 0x81

// We need to know when we have passed verification, regardless of how
// that was done.
#define ACTIVE_STATE_INACTIVE      0 // At startup
#define ACTIVE_STATE_PRE_PASSED    1 // If startup_completed got pre-mature
#define ACTIVE_STATE_NOT_PASSED    2 // At activate rsp
#define ACTIVE_STATE_PASSED        3 // At startup completed

#define PIN_UNKNOWN 0
#define PIN_DISABLED 1
#define PIN_ENABLED 2
#define PIN_ENABLED_PUK 3
#define PIN_ENABLED_PERM_BLOCKED 4

// Fields with X in comment need mutex when set
typedef struct {
  pthread_mutex_t mtx; // Thread-safe apps tbl
  uint8_t state;       // X This is active state tracking. NOT mal type
  uint8_t app_type;    // X MAL type
  uint8_t app_id;      // MAL
  uint8_t pin_id;      // MAL
  uint8_t pin2_id;
  uint8_t client_id;
  // All members below added to support sim_state_full
  uint8_t app_state;   // X MAL app state at appl list, and then appl_ind
  uint8_t aid[MAX_AID_SIZE]; // X MAL after appl list
  uint8_t aid_len;     // X
  char   *label;       // X
  uint8_t label_len;   // X Label length as found in ef_dir not including term 0.
  uint8_t pin_level;   // X disabled, enabled, enabled_puk, enabled_perm_blocked. This is the data got from a pin_info call
  uint8_t pin_ind;     // X Set to last pin ind got for this app, but also if PIN retries are exhaused it is set to PUK needed, and if those are exhaused then it is set to permanently blocked.
  uint8_t pin2_level;  // X disabled, enabled, enabled_puk, enabled_perm_blocked
  uint8_t pin2_ind;    // X Set to last pin2 ind got for this app
} app_data_t;

/**
 * Application table that stores data about all apps found on card.
 *
 * It might be that the modem starts emitting indications for an application
 * before the table is initialized. Those indications will end up in the
 * MAX_APPS slot, and so apps[MAX_APPS] is a temporary storage for such
 * unknown application. When the application is known, that material can be
 * retrieved and merged to its right place in the array.
 *
 * The table is modified in reader thread (runs mal call backs),
 * but data may also be read from uiccd server thread (functions of
 * catd_modem.h interface).
 *
 * Rules: When a mal cb function writes data, it should lock mutex.
 * When card_modem function reads data, it should lock mutex.
 */
static app_data_t apps[MAX_APPS+1];

// When in current need of pin verify, this stores app id and pin id.
// When unlocked, set to zero. Can be either PIN or PIN2
// TODO: Make thread safe; may be accessed by both rdr and main threads
#define PIN_STATE_UNKNOWN    0 // Before we know pin verify needed
#define PIN_STATE_PINLOCKED  1 // PIN_IND(PIN_VERIFY_NEEDED) received
#define PIN_STATE_PUKLOCKED  2 // PIN_IND(PIN_UNBLOCK_NEEDED) received
#define PIN_STATE_PERMLOCKED 3 // PIN_IND(PIN_PERMANENTLY_BLOCKED) received
#define PIN_STATE_PRE_VERIFIED 4 // PIN_IND(PIN_VERIFIED) got pre-mature
#define PIN_STATE_NOT_VERIFIED 5 // At PIN_RESP cb

// Re-initializes apps table
static void init_apps_table() {
  int i;

  for (i = 0; i < MAX_APPS+1; i++) {
    pthread_mutex_lock(&apps[i].mtx);
    apps[i].state = ACTIVE_STATE_INACTIVE;
    apps[i].app_type = MAL_UICC_APPL_TYPE_UNKNOWN;
    apps[i].app_id = 0;
    apps[i].pin_id = 0;
    apps[i].pin2_id = 0;
    apps[i].client_id = 0;
    apps[i].app_state = MAL_UICC_STATUS_APPL_NOT_ACTIVE;
    apps[i].aid_len = 0;
    free(apps[i].label);
    apps[i].label = NULL;
    apps[i].label_len = 0;
    apps[i].pin_level = PIN_UNKNOWN;
    apps[i].pin_ind = 0;
    apps[i].pin2_level = PIN_UNKNOWN;
    apps[i].pin2_ind = 0;
    pthread_mutex_unlock(&apps[i].mtx);
  }
}

/*
 * Structure used to store the context of a pin verify needed. Members
 * are set when ind is got, and obsolete when verification is done.
 * Should be cleared when verify was ok + pin ind for verified sent out.
 */
static struct {
  pthread_mutex_t mtx;
  uint8_t state;
  uint8_t pin_id;
  uint8_t app_id;
} pin_lock = { PTHREAD_MUTEX_INITIALIZER, PIN_STATE_UNKNOWN, 0, 0 };

#define MAL_TRY_TIMES 3 // Number of trials
#define MAL_TRY(E) { \
  int mal_try_e=0; \
  do { \
    if ((E) == 0) break; \
    mal_try_e++; \
    usleep(mal_try_e*100); \
  } while(mal_try_e < MAL_TRY_TIMES); \
}

static int get_active_pin_id(sim_uicc_pin_id_t pin_id);

/** Helper functions */
static void convert_mal_card_type(uint8_t mal_card_type, ste_uicc_card_type_t *uicc_card_type) {
  switch (mal_card_type) {
  case MAL_UICC_CARD_TYPE_ICC:
    *uicc_card_type = STE_UICC_CARD_TYPE_ICC;
    break;
  case MAL_UICC_CARD_TYPE_UICC:
    *uicc_card_type = STE_UICC_CARD_TYPE_UICC;
    break;
  case MAL_UICC_CARD_TYPE_USB:
    *uicc_card_type = STE_UICC_CARD_TYPE_USB;
    break;
  case MAL_UICC_CARD_TYPE_UNKNOWN:
  default:
    *uicc_card_type = STE_UICC_CARD_TYPE_UNKNOWN;
    break;
  }
}

static void convert_mal_card_status(uint8_t mal_card_status, ste_sim_card_status_t *uicc_card_status) {
  switch (mal_card_status) {
  case MAL_UICC_STATUS_CARD_READY:
    *uicc_card_status = STE_SIM_CARD_STATUS_READY;
    break;
  case MAL_UICC_STATUS_CARD_NOT_READY:
    *uicc_card_status = STE_SIM_CARD_STATUS_NOT_READY;
    break;
  case MAL_UICC_STATUS_CARD_NOT_PRESENT:
    *uicc_card_status = STE_SIM_CARD_STATUS_MISSING;
    break;
  case MAL_UICC_STATUS_CARD_DISCONNECTED:
    *uicc_card_status = STE_SIM_CARD_STATUS_DISCONNECTED;
    break;
  case MAL_UICC_STATUS_CARD_REJECTED:
    *uicc_card_status = STE_SIM_CARD_STATUS_INVALID;
    break;
  default:
    *uicc_card_status = STE_SIM_CARD_STATUS_UNKNOWN;
  }
}

static void convert_mal_app_type(uint8_t mal_appl_type, ste_sim_app_type_t *sim_app_type) {
  switch (mal_appl_type) {
  case MAL_UICC_APPL_TYPE_ICC_SIM:
    *sim_app_type = STE_SIM_APP_TYPE_SIM;
    break;
  case MAL_UICC_APPL_TYPE_UICC_USIM:
    *sim_app_type = STE_SIM_APP_TYPE_USIM;
    break;
  case MAL_UICC_APPL_TYPE_UICC_ISIM:
    *sim_app_type = STE_SIM_APP_TYPE_ISIM;
    break;
  case MAL_UICC_APPL_TYPE_UNKNOWN:
  default:
    *sim_app_type = STE_SIM_APP_TYPE_UNKNOWN;
    break;
  }
}
static void convert_mal_status_code(int mal_error_code,
                                    uint8_t status_code,
                                    uint8_t status_code_details,
                                    sim_uicc_status_code_t *uicc_status_code,
                                    sim_uicc_status_code_fail_details_t *uicc_status_code_fail_details)
{
  switch (mal_error_code)
  {
    case MAL_SUCCESS: // Go ahead to the next switch
    break;
    case MAL_NOT_SUPPORTED:
      *uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED;
      return; // The uicc_status_code and uicc_status_code_fail_details are not relevant
    break;
    case MAL_FAIL:
    default:
      *uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
      catd_log_f(SIM_LOGGING_E, "uicc: convert_mal_status_code: Got invalid error_code: %d", mal_error_code);
      return; // The uicc_status_code and uicc_status_code_fail_details are not relevant
    break;
  }

  switch (status_code)
  {
    case MAL_UICC_STATUS_OK:
      *uicc_status_code = SIM_UICC_STATUS_CODE_OK;
    break;
    case MAL_UICC_STATUS_FAIL:
      *uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
    break;
    case MAL_UICC_STATUS_UNKNOWN:
      *uicc_status_code = SIM_UICC_STATUS_CODE_UNKNOWN;
    break;
    case MAL_UICC_STATUS_NOT_READY:
      *uicc_status_code = SIM_UICC_STATUS_CODE_NOT_READY;
    break;
    case MAL_UICC_STATUS_SHUTTING_DOWN:
      *uicc_status_code = SIM_UICC_STATUS_CODE_SHUTTING_DOWN;
    break;
    case MAL_UICC_STATUS_CARD_READY:
      *uicc_status_code = SIM_UICC_STATUS_CODE_CARD_READY;
    break;
    case MAL_UICC_STATUS_CARD_NOT_READY:
      *uicc_status_code = SIM_UICC_STATUS_CODE_CARD_NOT_READY;
    break;
    case MAL_UICC_STATUS_CARD_DISCONNECTED:
      *uicc_status_code = SIM_UICC_STATUS_CODE_CARD_DISCONNECTED;
    break;
    case MAL_UICC_STATUS_CARD_NOT_PRESENT:
      *uicc_status_code = SIM_UICC_STATUS_CODE_CARD_NOT_PRESENT;
    break;
    case MAL_UICC_STATUS_CARD_REJECTED:
      *uicc_status_code = SIM_UICC_STATUS_CODE_CARD_REJECTED;
    break;
    case MAL_UICC_STATUS_PIN_ENABLED:
      *uicc_status_code = SIM_UICC_STATUS_CODE_PIN_ENABLED;
    break;
    case MAL_UICC_STATUS_PIN_DISABLED:
      *uicc_status_code = SIM_UICC_STATUS_CODE_PIN_DISABLED;
    break;
    case MAL_UICC_STATUS_APPL_ACTIVE:
      *uicc_status_code = SIM_UICC_STATUS_CODE_APPL_ACTIVE;
    break;
    case SIM_UICC_STATUS_CODE_APPL_NOT_ACTIVE:
      *uicc_status_code = SIM_UICC_STATUS_CODE_APPL_NOT_ACTIVE;
    break;
    default:
      catd_log_f(SIM_LOGGING_E, "uicc: convert_mal_status_code: Got unknown status code: %d", status_code);
      *uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
    break;
  }

  switch (status_code_details)
  {
    case MAL_UICC_NO_DETAILS:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    break;
    case MAL_UICC_INVALID_PARAMETERS:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS;
    break;
    case MAL_UICC_FILE_NOT_FOUND:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND;
    break;
    case MAL_UICC_SECURITY_CONDITIONS_NOT_SATISFIED:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_SECURITY_CONDITIONS_NOT_SATISFIED;
    break;
    case MAL_UICC_CARD_ERROR:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_CARD_ERROR;
    break;
    case MAL_UICC_SERVICE_NOT_SUPPORTED:
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED;
    break;
    case MAL_UICC_SESSION_EXPIRED:
    case MAL_UICC_APPL_CONFLICT:
    default:
      catd_log_f(SIM_LOGGING_E, "uicc: convert_mal_status_code: Got unknown status code details: %d", status_code_details);
      *uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
    break;
  }

  return;
}

extern void sim_maluicc_request_trace ( uint32_t uicc_MsgID, void* p_data, void *client_tag );
static int mal_uicc_request_wrapper(int32_t message_id, void *data, void *client_tag)
{
    int result = 0;
    int temp = 0;

    result = pthread_mutex_lock(&request_mutex);

    if (result == EDEADLK) {
       // This should not happen
       // If it should happen, check that the message queue's are not bypassed in uiccd and catd on callbacks from sim_mal.
       catd_log_s(SIM_LOGGING_E, "mal_uicc_request_wrapper : Thread tried to lock already owned mutex (EDEADLK). Bailing", 0);
       abort();
    } else if (result != 0) {
       catd_log_s(SIM_LOGGING_E, "mal_uicc_request_wrapper : Failed to lock the request mutex", 0);
       return -1;
    }

    sim_maluicc_request_trace(message_id, data, client_tag);
    result = mal_uicc_request(message_id, data, client_tag);

    if ( result != 0 ) {
        catd_log_f(SIM_LOGGING_E, "mal_uicc_request(%d) returned %d", message_id, result);
    }

    // result should be valid for the result of the request, so that clean-up
    // code can execute correctly.
    temp = pthread_mutex_unlock(&request_mutex);

    if (temp != 0) {
      catd_log_f(SIM_LOGGING_E, "mal_uicc_request_wrapper : Failed to unlock mutex");
    }

    return result;
}

enum uicc_control_req_type {
    UICC_CONTROL_REQ_TYPE_CARD_DEACTIVATE,
    UICC_CONTROL_REQ_TYPE_CARD_ACTIVATE,
    UICC_CONTROL_REQ_TYPE_COLD_RESET,
    UICC_CONTROL_REQ_TYPE_WARM_RESET,
    UICC_CONTROL_REQ_TYPE_NONE
};
//holds the last action for UICC_APDU_CONTROL request,
//it used in handle_mal_uicc_apdu_resp_cb handler
static enum uicc_control_req_type uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_NONE;

// Breakdown of call back cases.

/**
 * @brief Save result of application list to the apps table
 * @param app_len number of apps modem found on card
 * @param obj contains various appl info from modem and ef_dir file
 */

static void save_appl_do(app_data_t* app, uicc_sb_appl_data_object_t* obj)
{
  int j;

  if (obj->appl_do_len >= 5 &&
      obj->appl_do[0] == 0x61 &&
      obj->appl_do[2] == 0x4F)
    {
      // Template with AID tag found
      uint8_t aid_len = obj->appl_do[3];
      uint8_t *aid = (uint8_t *)&obj->appl_do[4];

      app->aid_len = aid_len;
      // Start copying out the aid, making sure not to go above MAX_AID_LEN
      for (j = 0; j<aid_len && j<MAX_AID_LEN; j++)
        app->aid[j] = aid[j];

      // The label is optional. Check if remaining appl template has
      // space enough, and tag is at right place.
      if (obj->appl_do[1] > aid_len+2 &&
          obj->appl_do[aid_len+4] == 0x50) {
        uint8_t label_len = obj->appl_do[4+aid_len+1];
        char *label = (char *)&obj->appl_do[4+aid_len+2];
        app->label_len = label_len;
        app->label = malloc(label_len+1);
        if (!app->label) return;
        for (j = 0; j<label_len; j++)
          app->label[j] = label[j];
        // Label is not zero-terminated string according to spec, but it is done
        // for convenience so it can be printf-ed
        app->label[label_len] = 0;
      } else {
        // No label found, so malloc an empty string
        app->label = malloc(1);
        app->label_len = 0;
        if (!app->label) return;
        app->label[0] = 0;
      }
      catd_log_f(SIM_LOGGING_D,"AppID=%d, Name=%s\n", app->app_id, app->label);
      sim_trace_print_data(app->aid, app->aid_len, catd_log_f);
    } else {
    // No apps template found, or no aid and no label
    // Set aid to none
    app->aid_len = 0;
    app->label_len = 0;
    app->label = malloc(1);
    if (!app->label) return;
    app->label[0] = 0;
  }
}

static void save_app(app_data_t* app, uicc_sb_appl_data_object_t* obj) {
  catd_log_f(SIM_LOGGING_D, "save_app: Enter");
  // Basic data that the MAL returns of the app

  app->app_state = obj->appl_status;
  app->app_type = obj->application_type;
  app->app_id = obj->appl_id;
  // appl_do member is a uint8 tlv taken from ef_dir. It contains
  // the aid and the label string.
  save_appl_do(app, obj);
  // Initialize remainder of app
  app->pin_id = 0;
  app->pin2_id = 0;
  app->client_id = 0;
  app->pin_level = PIN_UNKNOWN;
  app->pin_ind = 0; // No ind got yet
  app->pin2_level = PIN_UNKNOWN;
  app->pin2_ind = 0;
  catd_log_f(SIM_LOGGING_D, "save_app: Exit");
}

static uint8_t add_channel_id_to_class ( uint8_t class, uint8_t channel )
{
    uint8_t        result = 0;

    if ( channel <= 3 ) {
        // simple case, channel is stored in the lowest 2 bits.
        result = ( class & ~ 0x03u ) | channel;
    } else
    if ( channel <= 19 ) {
        // channel is stored in lowest 4 bits, with some rearrangement.
        // The two bits of secure messaging are compressed to a single bit.
        // So an original SM of the form 'x1' will lose information.
        // The channel ID is offset by 4.
        uint8_t sm = ( class & 0x08u ) << 2;
        result |= ( class & 0x80u );    // original class bits copied
        result |= 0x40u;                // extended channel flag
        result |= sm;                   // moved secure messaging flag
        result |= ( channel - 4 );      // the biased channel ID
    } else {
        catd_log_f(SIM_LOGGING_E, "add_channel_id_to_class: bad channel=%u",channel);
    }
    return result;
}

// Given a filled in apps table, this function sets card.i to
// the index of apps to the one to activate.
static void select_app_to_activate()
{
    int j;
    card.i = MAX_APPS; // Abandon previous selection, if any.

    catd_log_f(SIM_LOGGING_I, "select_app_to_activate: card type = %d", card.type);

    //for ICC card, the default application should be sim telecom application
    if (card.type == MAL_UICC_CARD_TYPE_ICC) {
        for (j = 0; j < card.number_of_apps; j++) {
            // Search for usim app
            if (apps[j].app_type == MAL_UICC_APPL_TYPE_ICC_SIM) {
                card.i = j; // This index is the app id shown to uiccd
                break;
            }
        }
    } else {
        for (j = 0; j < card.number_of_apps; j++) {
            // Search for usim app
            if (apps[j].app_type == MAL_UICC_APPL_TYPE_UICC_USIM) {
                card.i = j; // This index is the app id shown to uiccd
                break;
            }
        }
        if (card.i == MAX_APPS) {
            // No usim app found, so look for gsm app
            for (j = 0; j < card.number_of_apps; j++) {
                // Search for gsm app
                if (apps[j].app_type == MAL_UICC_APPL_TYPE_ICC_SIM) {
                    card.i = j;
                    break;
                }
            }
        }
    }

    if (card.i == MAX_APPS) {
        // No app found
        card.i = 0;
        apps[card.i].app_type = MAL_UICC_APPL_TYPE_ICC_SIM;
        apps[card.i].app_id = MAL_UICC_APPL_ID_UNKNOWN;
    }
    catd_log_f(SIM_LOGGING_D, "select_app_to_activate: Choose appid = %d apptype = %d", apps[card.i].app_id, apps[card.i].app_type);
}

void write_sim_detect(uint8_t value)
{
    if (strlen(DETECT_PATH) > 0) {
        int fd;
        int error;

        fd = open(DETECT_PATH, O_WRONLY | O_NONBLOCK);
        error = errno;
        if (fd != -1) {
            if  (write(fd, &value, sizeof(uint8_t)) == -1) {
                error = errno;
                catd_log_f(SIM_LOGGING_E, "%s: Could not write to file: %s",
                        __func__, strerror(errno));
            }
            close(fd);
        } else {
            catd_log_f(SIM_LOGGING_E, "%s: Could not open %s: %s", __func__,
                    DETECT_PATH, strerror(error));
        }
    }
}

static void ste_modem_reset_pin_state(ste_modem_t * UNUSED(m), uintptr_t UNUSED(client_tag))
{
    pthread_mutex_lock(&pin_lock.mtx);
    pin_lock.state = PIN_STATE_UNKNOWN;
    pin_lock.pin_id = 0;
    pin_lock.app_id = 0;
    pthread_mutex_unlock(&pin_lock.mtx);
}

static void sim_mal_card_fallback(uintptr_t client_tag)
{
    int j;

    ste_modem_reset_pin_state(NULL, 0);

    //reset the current app
    card.i = MAX_APPS;
    if (card.type == MAL_UICC_CARD_TYPE_ICC) {
        for (j = 0; j < card.number_of_apps; j++) {
            if (apps[j].app_type == MAL_UICC_APPL_TYPE_ICC_SIM) {
                card.i = j; // This index is the app id shown to uiccd
                break;
            }
        }
    }

    if (card.i == MAX_APPS) {
        // No app found
        card.i = 0;
        apps[card.i].app_type = MAL_UICC_APPL_TYPE_ICC_SIM;
        apps[card.i].app_id = MAL_UICC_APPL_ID_UNKNOWN;
    }
    catd_log_f(SIM_LOGGING_D, "sim_mal_card_fallback: app_id = %d", apps[card.i].app_id);
    catd_sig_modem_reset(client_tag);
    uiccd_sig_card_fallback(client_tag, apps[card.i].app_type, apps[card.i].app_id, card.type);
}

static void update_voltage(uint8_t voltage_class)
{
    FILE *fp = NULL;
    size_t written = 0;
    char *micro_voltage_p = NULL;
    size_t bytes = 0;
    fp = fopen(FILE_SIM_VOLTAGE, "w");

    if (!fp) {
        return;
    }

    if (card.status == MAL_UICC_CARD_READY ||
        card.status == MAL_UICC_STATUS_CARD_READY) {
        switch (voltage_class) {
            case MAL_UICC_VOLTAGE_CLASS_A:
                micro_voltage_p = "5000000";
            break;
            case MAL_UICC_VOLTAGE_CLASS_B:
                micro_voltage_p = "3000000";
            break;
            case MAL_UICC_VOLTAGE_CLASS_C:
                micro_voltage_p = "1800000";
            break;
            case MAL_UICC_VOLTAGE_CLASS_NOT_AVAILABLE:
            default:
                catd_log_f(SIM_LOGGING_I, "uicc : Voltage class not available, using default!");
                micro_voltage_p = "0"; // The voltage is unknown
            break;
        }
    } else {
        micro_voltage_p = "-1"; // Card invalid or not present
    }

    bytes = strlen(micro_voltage_p) * sizeof(char);
    written = fwrite(micro_voltage_p, bytes, 1, fp);

    if (written != 1) {
        catd_log_f(SIM_LOGGING_E, "uicc : Could not write preferred SIM voltage to the file system! (%d)", written);
    }

    if (fp) {
        fclose(fp);
    }
}

// Right now it is unknown how to recreate the exact state of each app
// if this fn is called after startup. Active/not active can be seen, but
// verification process is not clear.
// One way do to it would be to merge this call with the previous one.
// Also, unknown if there is a use case for calling this fn multiple times.
static void handle_appl_list_cb(struct ste_cmd_tag_s *ct,
                                uint32_t mal_status, uint8_t app_len,
                                uicc_sb_appl_data_object_t *obj[])
{
  int tag = ct->tag;
  void *client_tag = ct->ct;
  int i;
  sim_uicc_status_code_t code = SIM_UICC_STATUS_CODE_OK;

  free(ct);

  catd_log_f(SIM_LOGGING_I, "handle_appl_list_cb: card.type = %d, app_len = %d, mal_status = %d", card.type, app_len, mal_status);

  if (card.type == MAL_UICC_CARD_TYPE_ICC &&
      ((app_len == 0 || mal_status == MAL_UICC_STATUS_FAIL) ||
       (app_len == 1 && obj[0]->application_type == MAL_UICC_APPL_TYPE_UNKNOWN))) {
      // Activate unknown application
      card.i = 0;
      card.number_of_apps = 0;
      app_len = 0;
      apps[card.i].app_type = MAL_UICC_APPL_TYPE_ICC_SIM;
      apps[card.i].app_id = MAL_UICC_APPL_ID_UNKNOWN;
  } else if (mal_status != MAL_UICC_STATUS_FAIL && tag == STE_CMD_APPL_LIST_SELECT_TAG) {
      catd_log_f(SIM_LOGGING_D, "save_appl_list: Exit");

      for (i=0; i<app_len; i++) {
          pthread_mutex_lock(&apps[i].mtx);
          save_app(&apps[i], obj[i]);
          pthread_mutex_unlock(&apps[i].mtx);
      }

      card.number_of_apps = app_len;
      select_app_to_activate();     // Sets card.i
  } else {
      code = SIM_UICC_STATUS_CODE_FAIL;
  }

  // uiccd gets to know already number of apps found on sim card
  uiccd_sig_app_list((uintptr_t)client_tag, code, app_len);
}

static void activate_icc_pin_id(uicc_sb_chv_t *chv) {
  if (!chv) return;

  switch (chv->chv_qualifier) {
  case 1: apps[card.i].pin_id = chv->pin_id; break;
  case 2: apps[card.i].pin2_id = chv->pin_id; break; // PIN2
    break;
  default:
    // unknown
    break;
  }
}

static void activate_uicc_pin_id(uint8_t *UNUSED(fci), uint16_t UNUSED(length)) {
  // Right now, we only support single verification capable UICC and this
  // says application PIN is always referenced as 1. Otherwise, we need to
  // go through the fci in search of key references
  apps[card.i].pin_id = 1;
  apps[card.i].pin2_id = 0x81;
}

// Getting the pin ids in effect for the application
static void activate_cb_pin_id(uint8_t card_type, uicc_appln_host_activate_resp_t *sb) {
  switch (card_type) {
  case MAL_UICC_CARD_TYPE_ICC:
    activate_icc_pin_id(sb->uicc_sb_chv[0]);
    activate_icc_pin_id(sb->uicc_sb_chv[1]);
    break;
  case MAL_UICC_CARD_TYPE_UICC:
    // use uicc_sb_fci
    activate_uicc_pin_id(sb->uicc_sb_fci.fci, sb->uicc_sb_fci.fci_length);
    break;
  case MAL_UICC_CARD_TYPE_UNKNOWN:
  default:
    break;
  }
}

// This fn determines if the startup_completed msg shall be sent to uiccd
// right after activate rsp msg.
static void activate_cb_update_active_state(void *client_tag, mal_uicc_appln_resp_t *UNUSED(resp)) {
  switch (apps[card.i].state) {
  case ACTIVE_STATE_INACTIVE:
    if (apps[MAX_APPS].state ==  ACTIVE_STATE_PRE_PASSED) {
      // startup completed got before card.i was set.
      apps[card.i].state = ACTIVE_STATE_PRE_PASSED;
      apps[MAX_APPS].state =  ACTIVE_STATE_INACTIVE;
      // Intentional fall through to next case
    } else {
      apps[card.i].state = ACTIVE_STATE_NOT_PASSED;
      break;
    }
  case ACTIVE_STATE_PRE_PASSED: {
    ste_sim_app_type_t app_type;
    // startup completed already got due to modem activation. So we now signal
    // the suppressed indication. There will hence be no pin verify needed
    apps[card.i].state = ACTIVE_STATE_PASSED;
    convert_mal_app_type(apps[card.i].app_type, &app_type);
    catd_log_f(SIM_LOGGING_D, "Sending cached MAL_UICC_STARTUP_COMPLETE");
    uiccd_sig_uicc_status_payload_ind((uintptr_t)client_tag,
                                      STE_UICC_STATUS_READY,
                                      app_type, STE_UICC_CARD_TYPE_UNKNOWN);
    uiccd_sig_sim_status((uintptr_t)client_tag, STE_REASON_STARTUP_DONE);
    break;
  }
  case ACTIVE_STATE_NOT_PASSED:
    // this can only happen if we get more responses to activate, which we will
    // not and hence if we end up here there is error in our code.
  case ACTIVE_STATE_PASSED:
    // if an activate happened after startup completed
  default:
    break;
  }
}

static void handle_appl_host_activate_cb(void *client_tag, mal_uicc_appln_resp_t *resp)
{
  int tag;
  void *ct;
  struct ste_cmd_tag_s *my_ct = (struct ste_cmd_tag_s *)client_tag;
  tag = my_ct->tag;
  ct = my_ct->ct;
  free(my_ct);

  catd_log_f(SIM_LOGGING_D, "handle_appl_host_activate_cb. Status = %d", resp->status);

  if (tag == STE_CMD_APPL_ACTIVATE_STARTUP)
  {
    if (resp->status != MAL_UICC_STATUS_OK) {
      catd_log_f(SIM_LOGGING_E, "handle_appl_host_activate_cb. Activation failure with status=0x%x", resp->status);
      uiccd_sig_app_activate((uintptr_t)ct, STE_SIM_STATUS_FAIL, 0, STE_SIM_APP_TYPE_UNKNOWN);
      card.i = MAX_APPS;
      catd_sig_app_ready((uintptr_t)ct, 1);
      return;
    }
    // Recover state from indications received before card.i was set

    uiccd_sig_app_activate((uintptr_t)ct, STE_SIM_STATUS_OK, apps[card.i].app_id, apps[card.i].app_type);

    apps[card.i].client_id = resp->sub_block.uicc_appln_host_activate.uicc_sb_client.client_id;

    // Pick out pin_id for CHV1
    activate_cb_pin_id(resp->card_type, &resp->sub_block.uicc_appln_host_activate);
    activate_cb_update_active_state(ct, resp);
    catd_sig_app_ready((uintptr_t)ct, 0);

  } else if (tag == STE_CMD_ISIM_HOST_ACTIVATE) {
     struct ste_channel_isim_app_tag_s    *ct2 = (struct ste_channel_isim_app_tag_s *)ct;
     void                                 *request_tag = ct2->request_ct;
     uint16_t                              session_id = ct2->session_id;
     uint8_t                               i = ct2->app_index;
     sim_uicc_status_word_t                status_word = {0x00, 0x00};

    if (resp->status != MAL_UICC_STATUS_OK) {
        catd_log_f(SIM_LOGGING_E, "handle_appl_host_activate_cb. ISIM activation failure with status=0x%x", resp->status);

        // Reset the local channel data
        card.channels[session_id].channel_id = CHANNEL_ID_FREE;
        card.channels[session_id].aid_len    = 0;
        memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );

        uiccd_sig_sim_channel_open_response((uintptr_t)request_tag,
                                     SIM_UICC_STATUS_CODE_FAIL,
                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                     status_word,
                                     session_id);
    } else {
        apps[i].client_id   = resp->sub_block.uicc_appln_host_activate.uicc_sb_client.client_id;
        status_word.sw1     = 0x90;
        status_word.sw2     = 0x00;
        uiccd_sig_sim_channel_open_response((uintptr_t)request_tag,
                                         SIM_UICC_STATUS_CODE_OK,
                                         SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                         status_word,
                                         session_id);
    }
    free(ct2);

  }
}

static void handle_appl_host_deactivate_cb(void *client_tag, mal_uicc_appln_resp_t *resp)
{
    int tag;
    void *ct;
    struct ste_cmd_tag_s *my_ct = (struct ste_cmd_tag_s *)client_tag;
    tag = my_ct->tag;
    ct = my_ct->ct;
    free(my_ct);

    catd_log_f(SIM_LOGGING_D, "handle_appl_host_deactivate_cb. Status = %d", resp->status);

    if (tag == STE_CMD_APPL_DEACTIVATE_STARTUP)
    {
        if (resp->status != MAL_UICC_STATUS_OK) {
            catd_log_f(SIM_LOGGING_E, "handle_appl_host_deactivate_cb. DeActivation failure with status = 0x%x", resp->status);
            return;
        }
        apps[card.i].client_id = 0;
        apps[card.i].app_state = MAL_UICC_STATUS_APPL_NOT_ACTIVE;
        card.i = MAX_APPS;

    } else if (tag == STE_CMD_ISIM_DEACTIVATE) {
        struct ste_channel_isim_app_tag_s    *ct2 = (struct ste_channel_isim_app_tag_s *)ct;
        void                                 *request_tag = ct2->request_ct;
        uint16_t                              session_id = ct2->session_id;
        uint8_t                               i = ct2->app_index;
        sim_uicc_status_word_t                status_word = {0x00, 0x00};

        if (resp->status != MAL_UICC_STATUS_OK) {
            //TODO: When this path is tested, we need to check if the corresponding internal channel
            //is closed by the modem even when the response is a failure.
            catd_log_f(SIM_LOGGING_E, "handle_appl_host_deactivate_cb.ISIM DeActivation failure with status = 0x%x", resp->status);
            uiccd_sig_sim_channel_close_response((uintptr_t)request_tag,
                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                    status_word);
        } else {
            apps[i].client_id = 0;

            // Reset the local channel data
            card.channels[session_id].channel_id = CHANNEL_ID_FREE;
            card.channels[session_id].aid_len    = 0;
            memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );

            status_word.sw1     = 0x90;
            status_word.sw2     = 0x00;
            uiccd_sig_sim_channel_close_response((uintptr_t)request_tag,
                                                    SIM_UICC_STATUS_CODE_OK,
                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                    status_word);
        }
        free(ct2);
    }
}

static void handle_appl_shutdown_init_cb(void *client_tag, mal_uicc_appln_resp_t *resp)
{
    catd_log_f(SIM_LOGGING_I, "handle_appl_shutdown_init_cb: mal_status = %d",resp->status);

    if (resp->status != MAL_UICC_STATUS_OK) {
        catd_log_f(SIM_LOGGING_E, "handle_appl_shutdown_init_cb. Shutdown failure with status = 0x%x", resp->status);
        return;
    }

    // uiccd gets to know already number of apps found on sim card
    uiccd_sig_app_shutdown_init((uintptr_t)client_tag, STE_SIM_STATUS_OK);
}

static void handle_mal_uicc_ind_cb(int UNUSED(eventID), void *data, int UNUSED(mal_error), void *client_tag) {
  uicc_ind_t *ind = (uicc_ind_t *)data;

  if (!ind) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_ind_cb: service_type=0x%x", ind->service_type);
  switch (ind->service_type) {
  case MAL_UICC_START_UP_COMPLETE:
    catd_log_f(SIM_LOGGING_D, "MAL_UICC_START_UP_COMPLETE");
    uiccd_sig_uicc_status_ind((uintptr_t)client_tag, STE_UICC_STATUS_STARTUP_COMPLETED);
    card.startup_complete = 1; // By BT SAP
    switch (apps[card.i].state) {
    case ACTIVE_STATE_INACTIVE:
      // The modem has activated, and STARTUP_COMPLETE ind received before
      // our rsp to ACTIVATE. This can happen either before or after card.i
      // has been set.
      catd_log_f(SIM_LOGGING_V, "START_UP_COMPLETE: ACTIVE_STATE_INACTIVE");
      apps[card.i].state = ACTIVE_STATE_PRE_PASSED;
      break;
    case ACTIVE_STATE_NOT_PASSED: {
      catd_log_f(SIM_LOGGING_V, "START_UP_COMPLETE: ACTIVE_STATE_NOT_PASSED");
      ste_sim_app_type_t app_type;
      // Normal case, get it after activate rsp
      apps[card.i].state = ACTIVE_STATE_PASSED;
      switch (apps[card.i].app_type) {
      case MAL_UICC_APPL_TYPE_ICC_SIM: app_type = STE_SIM_APP_TYPE_SIM; break;
      case MAL_UICC_APPL_TYPE_UICC_USIM: app_type = STE_SIM_APP_TYPE_USIM; break;
      case MAL_UICC_APPL_TYPE_UICC_ISIM: app_type = STE_SIM_APP_TYPE_ISIM; break;
      case MAL_UICC_APPL_TYPE_UNKNOWN: default: app_type = STE_SIM_APP_TYPE_UNKNOWN; break;
      }
      uiccd_sig_uicc_status_payload_ind((uintptr_t)client_tag,
                                        STE_UICC_STATUS_READY,
                                        app_type, STE_UICC_CARD_TYPE_UNKNOWN);
      uiccd_sig_sim_status((uintptr_t)client_tag, STE_REASON_STARTUP_DONE);
      break;
    }
    case ACTIVE_STATE_PRE_PASSED:
      catd_log_f(SIM_LOGGING_V, "START_UP_COMPLETE: ACTIVE_STATE_PRE_PASSED");
      break;
    case ACTIVE_STATE_PASSED:
      // Getting this once again, silently ignore.
      catd_log_f(SIM_LOGGING_V, "START_UP_COMPLETE: PASSED (redundant)");
      break;
    default:
      catd_log_f(SIM_LOGGING_V, "START_UP_COMPLETE: Default");
      break;
    }
    break;
  case MAL_UICC_SHUTTING_DOWN:
    card.startup_complete = 0;

    catd_log_f(SIM_LOGGING_I, "MAL_UICC_SHUTTING_DOWN");
    uiccd_sig_uicc_status_ind((uintptr_t)client_tag, STE_UICC_STATUS_CLOSED);
    break;
  default:
    break;
  }
}

static void handle_mal_uicc_resp_cb(int eventID, void *data, int mal_error, void *client_tag) {
  mal_uicc_resp_sim_status_t *resp = (mal_uicc_resp_sim_status_t *)data;
  uint32_t status;

  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __func__);
      return;
  }

  status = STE_STATUS(mal_error, resp->status, resp->details);

  switch (resp->server_status) {
  case MAL_UICC_STATUS_NOT_READY:
    catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_resp_cb 0x%x STATUS_NOT_READY", eventID);
    uiccd_sig_server_status_response((uintptr_t) client_tag, status, STE_UICC_NOT_READY);
    break;
  case MAL_UICC_STATUS_START_UP_COMPLETED:
    catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_resp_cb 0x%x STATUS_NOT_READY STATUS_START_UP_COMPLETED", eventID);
    uiccd_sig_server_status_response((uintptr_t) client_tag, status, STE_UICC_READY);
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_resp_cb 0x%x unknown status", eventID);
  }
}

static void close_logical_channel_on_error ( uint16_t session_id )
{
    uicc_apdu_req_t         req = { 0, { { { 0, 0, 0 } } } };
    struct ste_cmd_tag_s   *ct = NULL;
    uint8_t                 cmd[sizeof(manage_channel_close)];

    ct = malloc(sizeof *ct);
    if (!ct) {
        catd_log_f(SIM_LOGGING_E, "close_logical_channel_on_error - alloc failure");
        return;
    }

    ct->ct = NULL;
    ct->tag = STE_CMD_APDU_ERROR_CLOSE;

    memcpy( cmd, manage_channel_close, sizeof(cmd) );
    cmd[3] = card.channels[session_id].channel_id;       // set the channel to be closed in P2 of the APDU

    req.service_type = MAL_UICC_APDU_SEND;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = cmd;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = sizeof(cmd);
    if ( mal_uicc_request_wrapper(MAL_UICC_APDU_REQ, &req, ct) != 0 ) {
        catd_log_f(SIM_LOGGING_E, "close_logical_channel_on_error - request failure");
    }

    // Reset the local channel data
    card.channels[session_id].channel_id = CHANNEL_ID_FREE;
    card.channels[session_id].aid_len    = 0;
    memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );
}

/* Callback when MAL_UICC_APDU_RESP is sent back from modem  */
static void handle_mal_uicc_apdu_resp_cb(int eventID, void *data, int mal_error, void *client_tag)
{
    uicc_apdu_resp_t *resp = (uicc_apdu_resp_t *)data;
    sim_uicc_status_word_t status_word = {0, 0};
    sim_uicc_status_code_t uicc_status_code;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
    int tag;
    void *ct;
    struct ste_cmd_tag_s *my_ct = (struct ste_cmd_tag_s *)client_tag;
    tag = my_ct->tag;
    ct = my_ct->ct;
    free(my_ct);

    if (MAL_UICC_APDU_RESP != eventID) {
        catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_resp_cb :: unknown response");
        return;
    }

    if (!resp) {
        catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
        return;
    }

    convert_mal_status_code(mal_error,
                            resp->status,
                            resp->details,
                            &uicc_status_code,
                            &uicc_status_code_fail_details);

    catd_log_f(SIM_LOGGING_D, "%s, %#.4x, status: %#.4x", __FUNCTION__, eventID, resp->status);

    if (tag == STE_CMD_APDU_SAP)
    {
        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb: STE_CMD_APDU_SAP");

        if (MAL_UICC_APDU_CONTROL == resp->service_type) {
                switch (uicc_ctrl_req_type) {
                    case UICC_CONTROL_REQ_TYPE_COLD_RESET:
                        uiccd_sig_sap_session_ctrlcard_coldreset_response((uintptr_t) ct, resp->status);
                        break;
                    case UICC_CONTROL_REQ_TYPE_WARM_RESET:
                        uiccd_sig_sap_session_ctrlcard_warmreset_response((uintptr_t) ct, resp->status);
                        break;
                    case UICC_CONTROL_REQ_TYPE_CARD_DEACTIVATE:
                        uiccd_sig_sap_session_ctrlcard_deactivate_response((uintptr_t) ct, resp->status);
                        break;
                    case UICC_CONTROL_REQ_TYPE_CARD_ACTIVATE:
                        uiccd_sig_sap_session_ctrlcard_activate_response((uintptr_t) ct, resp->status);
                        break;
                    default:
                        catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_resp_cb :: MAL_UICC_APDU_CONTROL unknown resp");
                        break;
                };
                uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_NONE;
        } else if (MAL_UICC_ATR_GET == resp->service_type) {
            if (MAL_UICC_STATUS_OK == resp->status) {
                uiccd_sig_sap_session_atr_get_response((uintptr_t) ct,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word,
                                                        resp->sub_block.uicc_apdu_atr_get_resp.uicc_sb_apdu.apdu,
                                                        resp->sub_block.uicc_apdu_atr_get_resp.uicc_sb_apdu.apdu_length);
            } else {
                  uiccd_sig_sap_session_atr_get_response((uintptr_t) ct,
                                                          uicc_status_code,
                                                          uicc_status_code_fail_details,
                                                          status_word,
                                                          NULL, 0);
            }
        } else if (MAL_UICC_APDU_SEND == resp->service_type) {
            if (MAL_UICC_STATUS_OK == resp->status) {
                uiccd_sig_sap_session_apdu_data_response((uintptr_t) ct,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word,
                                                        resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu,
                                                        resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu_length);
            } else {
                 uiccd_sig_sap_session_apdu_data_response((uintptr_t) ct,
                                                         uicc_status_code,
                                                         uicc_status_code_fail_details,
                                                         status_word,
                                                         NULL, 0);
            }
        } else {
            catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_resp_cb :: unknown sap req/rsp service type");
        }
    }
    else if (tag == STE_CMD_APDU_NORMAL_OPEN)
    {
        int             request_ok = 0;
        const uint8_t*  apdu_data = NULL;
        size_t          apdu_len = 0;
        struct ste_channel_app_tag_s    *ct2 = (struct ste_channel_app_tag_s*)ct;
        void           *request_tag = ct2->request_ct;
        uint16_t        session_id = ct2->session_id;
        struct ste_cmd_tag_s *ct1   = NULL;

        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb: STE_CMD_APDU_NORMAL_OPEN");

        if ((resp->status == MAL_UICC_STATUS_OK)&&(resp->n_sb == 1)) {
            const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu;
            size_t         len    = (size_t)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu_length;
            apdu_data       = r_apdu;
            apdu_len        = len-2;
            status_word.sw1 = r_apdu[len-2];
            status_word.sw2 = r_apdu[len-1];
            if ((status_word.sw1 == 0x90 && status_word.sw2 == 0x00) ||
                (status_word.sw1 == 0x91) ||
                (status_word.sw1 == 0x62 && status_word.sw2 == 0x00) ||
                (status_word.sw1 == 0x62 && status_word.sw2 == 0x85)) {

                uicc_apdu_req_t   req = { 0, { { { 0, 0, 0 } } } };
                ct1 = malloc(sizeof *ct1);
                if (ct1 != NULL) {
                    uint8_t         activate[MAX_AID_SIZE+sizeof(select_app)];
                    channel_info_t *ci = &card.channels[session_id];

                    ct1->ct = ct2;                                  // re-using secondary context ptr
                    ct1->tag = STE_CMD_APDU_NORMAL_OPEN_ACTIVATED;

                    ci->channel_id = r_apdu[0];

                    // Prepare the Application Activation APDU
                    memcpy(&activate[0],select_app,sizeof(select_app));     // CLA, INS, P2, P2
                    activate[4] = (uint8_t)ci->aid_len;                     // Length of AID
                    memcpy(&activate[5],ci->aid,ci->aid_len);               // AID
                    activate[5+ci->aid_len] = 0;                            // Result length
                    activate[0] = add_channel_id_to_class(activate[0],ci->channel_id);

                    req.service_type = MAL_UICC_APDU_SEND;
                    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
                    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = activate;
                    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = 6+ci->aid_len;
                    if ( mal_uicc_request_wrapper(MAL_UICC_APDU_REQ, &req, ct1) == 0 ) {
                        request_ok = 1;
                    } else {
                        // Application activation failed immediately, so close the channel
                        close_logical_channel_on_error(session_id);
                    }
                }
            }
        }
        if ( !request_ok ) {
            /* Something went wrong with manage channel, so send negative ack to application */

            // Reset the local channel data
            card.channels[session_id].channel_id = CHANNEL_ID_FREE;
            card.channels[session_id].aid_len    = 0;
            memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );

            uiccd_sig_sim_channel_open_response((uintptr_t)request_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word,
                                                INVALID_CHANNEL_SESSION);

            /* and free all the memory in use to manage this transaction */
            free(ct2);
            free(ct1);
        }
    }
    else if (tag == STE_CMD_APDU_NORMAL_OPEN_ACTIVATED)
    {
        int             request_ok = 0;
        const uint8_t*  apdu_data = NULL;
        size_t          apdu_len = 0;
        struct ste_channel_app_tag_s    *ct2 = (struct ste_channel_app_tag_s*)ct;
        void           *request_tag = ct2->request_ct;
        uint16_t        session_id = ct2->session_id;

        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb: STE_CMD_APDU_NORMAL_OPEN_ACTIVATED");

        if ((resp->status == MAL_UICC_STATUS_OK)&&(resp->n_sb == 1)) {
            const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu;
            size_t         len    = (size_t)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu_length;
            apdu_data       = r_apdu;
            apdu_len        = len-2;
            status_word.sw1 = r_apdu[len-2];
            status_word.sw2 = r_apdu[len-1];

            if ((status_word.sw1 == 0x90 && status_word.sw2 == 0x00) ||
                (status_word.sw1 == 0x91) ||
                (status_word.sw1 == 0x62 && status_word.sw2 == 0x00) ||
                (status_word.sw1 == 0x62 && status_word.sw2 == 0x83) ||
                (status_word.sw1 == 0x62 && status_word.sw2 == 0x85)) {

                uiccd_sig_sim_channel_open_response((uintptr_t)request_tag,
                                                    uicc_status_code,
                                                    uicc_status_code_fail_details,
                                                    status_word,
                                                    session_id);
                request_ok = 1;
            } else {
                uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
            }
        }

        if ( !request_ok ) {
            // Application activation failed, so close the channel and report
            close_logical_channel_on_error(session_id);
            uiccd_sig_sim_channel_open_response((uintptr_t)request_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word,
                                                INVALID_CHANNEL_SESSION);
        }

        free(ct2);
    }
    else if (tag == STE_CMD_APDU_NORMAL_SEND)
    {
        const uint8_t* apdu_data = NULL;
        size_t         apdu_len = 0;

        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb: STE_CMD_APDU_NORMAL_SEND");

        if ((resp->status == MAL_UICC_STATUS_OK)&&(resp->n_sb == 1)) {
            const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu;
            size_t         len    = (size_t)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu_length;
            apdu_data       = r_apdu;
            apdu_len        = len;              /* Send the whole response, including SW1,SW2 */
            status_word.sw1 = r_apdu[len-2];
            status_word.sw2 = r_apdu[len-1];
        }

        uiccd_sig_sim_channel_send_response((uintptr_t)ct,
                                            uicc_status_code,
                                            uicc_status_code_fail_details,
                                            status_word,
                                            apdu_data,
                                            apdu_len);
    }
    else if (tag == STE_CMD_APDU_NORMAL_CLOSE)
    {
        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb: STE_CMD_APDU_NORMAL_CLOSE");

        if ((resp->status == MAL_UICC_STATUS_OK)&&(resp->n_sb == 1)) {
            // Close channel only contains the status words, there is no data.
            const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu;
            status_word.sw1 = r_apdu[0];
            status_word.sw2 = r_apdu[1];
        }

        uiccd_sig_sim_channel_close_response((uintptr_t)ct,
                                             uicc_status_code,
                                             uicc_status_code_fail_details,
                                             status_word);
    }
    else if (tag == STE_CMD_APDU_ERROR_CLOSE)
    {
        // There is nothing to do in this case, the channel was closed because
        // of an error, and that error has already been reported to the caller.
        // If this fails here, all we can do is log it.
        catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_resp_cb STE_CMD_APDU_ERROR_CLOSE");
        if ((resp->status == MAL_UICC_STATUS_OK)&&(resp->n_sb == 1)) {
            // Close channel only contains the status words, there is no data.
            const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_apdu_send_resp.uicc_sb_apdu.apdu;
            status_word.sw1 = r_apdu[0];
            status_word.sw2 = r_apdu[1];
            if ( !(status_word.sw1 == 0x90 && status_word.sw2 == 0x00) ) {
                catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb - error close failed %x %x",
                    status_word.sw1, status_word.sw2);
            }
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_apdu_resp_cb 0x%x unknown tag", tag);
    }
}

/* Callback when smartcard is reseted */
static void handle_mal_uicc_apdu_reset_ind_cb(int eventID, void *data, int UNUSED(mal_error), void *client_tag)
{
    uicc_apdu_reset_ind_t *resp = (uicc_apdu_reset_ind_t *)data;

    if (MAL_UICC_APDU_RESET_IND != eventID) {
        catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_reset_ind_cb :: unknown response");
        return;
    }

    if (!resp) {
        catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
        return;
    }

    catd_log_f(SIM_LOGGING_D, "%s, %#.4x, service: %#.4x", __FUNCTION__, eventID, resp->service_type);

    if (MAL_UICC_READY == resp->service_type) {
        uiccd_sig_sap_session_ctrlcard_reset_ind((uintptr_t) client_tag, 0);
    } else if (MAL_UICC_ERROR == resp->service_type) {
        uiccd_sig_sap_session_ctrlcard_reset_ind((uintptr_t) client_tag, 1);
    } else {
        catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_apdu_reset_ind_cb :: unknown service type");
    }
}

static void handle_mal_uicc_card_reader_ind_cb(int UNUSED(eventID), void *data, int UNUSED(mal_error), void *client_tag) {
    uicc_ind_t *ind = (uicc_ind_t *)data;

    if (!ind) {
        catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
        return;
    }

    catd_log_f(SIM_LOGGING_I, "%s: service_type=0x%x", __PRETTY_FUNCTION__, ind->service_type);
    switch (ind->service_type) {
    case MAL_UICC_CARD_REMOVED:
        if (card.status != MAL_UICC_CARD_REMOVED) {
            card.status = MAL_UICC_CARD_REMOVED;
            uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_REMOVED_CARD);
            uiccd_sig_sim_status((uintptr_t) client_tag, STE_REASON_NO_CARD);
        }
        break;
    default:
        catd_log_f(SIM_LOGGING_E, "%s: unknown service type", __PRETTY_FUNCTION__);
        break;
    }
}

static void handle_mal_uicc_card_ind_cb(int UNUSED(eventID), void *data, int UNUSED(mal_error), void *client_tag) {
  uicc_card_ind_t *ind = (uicc_card_ind_t *)data;
  ste_uicc_card_type_t card_type;
  uint8_t voltage_class = MAL_UICC_VOLTAGE_CLASS_NOT_AVAILABLE;

  if (!ind) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  card.got_card_status = 1;
  catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_card_ind_cb: service_type=0x%x", ind->service_type);
  switch (ind->service_type) {
  case MAL_UICC_CARD_READY:
    card.status = MAL_UICC_STATUS_CARD_READY;
    catd_log_f(SIM_LOGGING_I, "STE_SIM_CARD_STATUS_READY");
    card.type = ind->card_type;
    card.clf_support = ind->sub_block_t.uicc_sb_card_info.clf_support;
    convert_mal_card_type(ind->card_type, &card_type);
    uiccd_sig_uicc_status_payload_ind((uintptr_t) client_tag, STE_UICC_STATUS_INIT, STE_SIM_APP_TYPE_UNKNOWN, card_type);
    voltage_class = ind->sub_block_t.uicc_sb_card_info.voltage_class;
#ifdef SET_DETECT_PATH
    write_sim_detect('1');
#endif
    break;
  case MAL_UICC_CARD_NOT_PRESENT:
    if (card.status != MAL_UICC_STATUS_CARD_NOT_PRESENT) {
      card.status = MAL_UICC_STATUS_CARD_NOT_PRESENT;
      catd_log_f(SIM_LOGGING_I, "STE_UICC_STATUS_NO_CARD");
      uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_NO_CARD);
      uiccd_sig_sim_status((uintptr_t) client_tag, STE_REASON_NO_CARD);
    }
    card.clf_support = 0;
    break;
  case MAL_UICC_CARD_REMOVED:
    /*
      If remove cause is due to presence detection failure, then no need to send this indication
      to upper layers as Card recovery is in progress. PIN caching shall be performed on receiving
      Cat ready Indication Later.
    */
    if (ind->sub_block_t.uicc_sb_card_remove_cause.remove_cause == MAL_UICC_CARD_REMOVE_CAUSE_PRESENCE_DETECTION ) {
        catd_log_f(SIM_LOGGING_I, "STE_UICC_STATUS_CARD_REMOVED_PRESENCE_DETECTION");
        return;
    }

    // If card indication is received with card removed with no prior card_reader_ind
    // then there are a chance that the card will recover
    if (card.status != MAL_UICC_CARD_REMOVED && card.status != MAL_UICC_STATUS_CARD_NOT_PRESENT) {
      card.status = MAL_UICC_STATUS_CARD_NOT_PRESENT;
      catd_log_f(SIM_LOGGING_I, "STE_UICC_STATUS_NO_CARD");
      uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_NO_CARD);
      uiccd_sig_sim_status((uintptr_t) client_tag, STE_REASON_NO_CARD);
    }
    card.clf_support = 0;
#ifdef SET_DETECT_PATH
    write_sim_detect('0');
#endif
    break;
  case MAL_UICC_CARD_DISCONNECTED:
    if (card.status != MAL_UICC_STATUS_CARD_DISCONNECTED) {
      card.status = MAL_UICC_STATUS_CARD_DISCONNECTED;
      catd_log_f(SIM_LOGGING_I, "STE_SIM_CARD_STATUS_DISCONNECTED");
      uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_DISCONNECTED_CARD);
      uiccd_sig_sim_status((uintptr_t) client_tag, STE_REASON_DISCONNECTED_CARD);
    }
    break;
  case MAL_UICC_CARD_REJECTED:
    if (card.status != MAL_UICC_STATUS_CARD_REJECTED) {
      ste_uicc_status_t uicc_status;
      ste_reason_t reason;

      card.status = MAL_UICC_STATUS_CARD_REJECTED;

      switch (ind->sub_block_t.uicc_sb_card_reject_cause.reject_cause) {
          case MAL_UICC_CARD_REJECT_CAUSE_NOT_AVAILABLE:
              uicc_status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
              reason = STE_REASON_REJECTED_CARD_INVALID;
          break;
          case MAL_UICC_CARD_REJECT_CAUSE_SIMLOCK:
              uicc_status = STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK;
              reason = STE_REASON_REJECTED_CARD_SIM_LOCK;
          break;
          case MAL_UICC_CARD_REJECT_CAUSE_SW_6F00:
              uicc_status = STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00;
              reason = STE_REASON_REJECTED_CARD_CONSECUTIVE_6F00;
          break;
          default:
              uicc_status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
              reason = STE_REASON_REJECTED_CARD_INVALID;
              catd_log_f(SIM_LOGGING_I, "SIM Card rejected due to unknown reason!");
          break;
      }

      card.reject_cause = ind->sub_block_t.uicc_sb_card_reject_cause.reject_cause;
      card.clf_support = 0;
      uiccd_sig_uicc_status_ind((uintptr_t) client_tag, uicc_status);
      uiccd_sig_sim_status((uintptr_t) client_tag, reason);
    }
    break;
  default:
//  catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_card_ind_cb 0x%x service_type=0x%x card_type=0x%x", eventID, ind->service_type, ind->card_type);
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_card_ind_cb: unknown service type");
  }

  update_voltage(voltage_class);
}

static void handle_mal_uicc_card_resp_cb(int eventID, void *data, int UNUSED(mal_error), void *client_tag) {
  mal_uicc_card_resp_t *resp = (mal_uicc_card_resp_t *)data;

  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  catd_log_f(SIM_LOGGING_I, "%s: service type: %d", __func__, resp->service_type);

  switch (resp->service_type) {
  case MAL_UICC_CARD_INFO_GET:
      update_voltage(resp->sub_block_t.uicc_sb_card_info_get.voltage_class);
      card.clf_support = resp->sub_block_t.uicc_sb_card_info_get.clf_support;
      break;
  case MAL_UICC_CARD_STATUS_GET: {
    ste_sim_card_status_t card_status;
    ste_uicc_card_type_t card_type = STE_UICC_CARD_TYPE_UNKNOWN;
    if (!card.got_card_status) {
      card.status = resp->sub_block_t.uicc_sb_card_status_get.card_status;
      catd_log_f(SIM_LOGGING_I, "CARD_STATUS_GET recovers missed CARD_IND!");

      // Use card status to recover a missed card_ind. In uiccd init state, the rsp should be received but do
      // nothing if it arrives there.
      switch (resp->sub_block_t.uicc_sb_card_status_get.card_status) {
      case MAL_UICC_STATUS_CARD_NOT_READY:
        // Continue wait for the indication
        break;
      case MAL_UICC_STATUS_CARD_READY:
        catd_log_f(SIM_LOGGING_I, "STE_SIM_CARD_STATUS_READY");
        card.got_card_status = 1;
        card.type = resp->card_type;
        convert_mal_card_type(resp->card_type, &card_type);

        // Request the card info so the voltage class can be updated
        if (ste_mtbl_i_modem_card_info_th() != 0) {
            catd_log_f(SIM_LOGGING_E, "ste_mtbl_i_modem_card_info_th: Failed", 0);
        }

        uiccd_sig_uicc_status_payload_ind((uintptr_t) client_tag, STE_UICC_STATUS_INIT,
                                          STE_SIM_APP_TYPE_UNKNOWN, card_type);
        break;
      case MAL_UICC_STATUS_CARD_NOT_PRESENT:
        card.got_card_status = 1;
        catd_log_f(SIM_LOGGING_I, "STE_SIM_CARD_STATUS_MISSING");
        uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_NO_CARD);
        uiccd_sig_sim_status((uintptr_t) client_tag, STE_REASON_NO_CARD);
        break;
      case MAL_UICC_STATUS_CARD_DISCONNECTED:
      {
          catd_log_f(SIM_LOGGING_I, "STE_UICC_STATUS_DISCONNECTED_CARD");
          uiccd_sig_uicc_status_ind((uintptr_t) client_tag, STE_UICC_STATUS_DISCONNECTED_CARD);
          uiccd_sig_sim_status((uintptr_t) client_tag, STE_UICC_STATUS_DISCONNECTED_CARD);
      }
      break;
      case MAL_UICC_STATUS_CARD_REJECTED:
      {
          ste_uicc_status_t uicc_status;
          ste_reason_t reason;
          card.got_card_status = 1;

          catd_log_f(SIM_LOGGING_I, "SIM Card rejected (Invalid card)");
          uicc_status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
          reason = STE_REASON_REJECTED_CARD_INVALID;

          uiccd_sig_uicc_status_ind((uintptr_t) client_tag, uicc_status);
          uiccd_sig_sim_status((uintptr_t) client_tag, reason);
     }
     break;
      default:
        break;
     }
    } else {
      // Normal query
      switch (resp->sub_block_t.uicc_sb_card_status_get.card_status) {
      case MAL_UICC_STATUS_CARD_NOT_READY:
        card_status = STE_SIM_CARD_STATUS_NOT_READY;
        break;
      case MAL_UICC_STATUS_CARD_READY:
        card_status = STE_SIM_CARD_STATUS_READY;
        break;
      case MAL_UICC_STATUS_CARD_NOT_PRESENT:
        card_status = STE_SIM_CARD_STATUS_MISSING;
        break;
      case MAL_UICC_STATUS_CARD_DISCONNECTED:
      case MAL_UICC_STATUS_CARD_REJECTED:
        card_status = STE_SIM_CARD_STATUS_INVALID;
        break;
      default:
        card_status = STE_SIM_CARD_STATUS_UNKNOWN;
        break;
      }
      uiccd_sig_server_card_status((uintptr_t) client_tag, resp->status,
                                   card_status);
    }
  }
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_card_resp_cb 0x%x service_type=0x%x card_type=0x%x", eventID, resp->service_type, resp->card_type);
    break;
  }
}


static void handle_mal_uicc_application_resp_cb(int32_t UNUSED(eventID), void *data, int mal_error, void *client_tag) {
  mal_uicc_appln_resp_t *resp = (mal_uicc_appln_resp_t *)data;
  uint32_t status;

  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  status = STE_STATUS(mal_error, resp->status, resp->details);

  switch (resp->service_type) {
  case MAL_UICC_APPL_LIST:
    catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_application_resp_cb : %d apps found on card", resp->n_sb);
    handle_appl_list_cb(client_tag, status, resp->n_sb,
                        resp->sub_block.uicc_appln_list.uicc_sb_appl_data_object);
    break;
  case MAL_UICC_APPL_HOST_ACTIVATE:
    handle_appl_host_activate_cb(client_tag, resp);
    break;

  case MAL_UICC_APPL_HOST_DEACTIVATE:
    handle_appl_host_deactivate_cb(client_tag, resp);
    break;

  case MAL_UICC_APPL_SHUT_DOWN_INITIATED:
    handle_appl_shutdown_init_cb(client_tag, resp);
    break;

  default:
    break;
  }
}

static void handle_mal_uicc_application_ind_cb(int UNUSED(eventID), void *data, int UNUSED(mal_error), void *client_tag) {
  uicc_application_ind_t *ind = (uicc_application_ind_t *)data;

  if (!ind) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_application_ind_cb : ServiceType=0x%x", ind->service_type);
  switch (ind->service_type) {
  case MAL_UICC_APPL_ACTIVATED:
    apps[card.i].app_state = MAL_UICC_STATUS_APPL_ACTIVE;
    // Most normal and wanted case!
    if (apps[card.i].app_id == ind->appl_id &&
        apps[card.i].app_type == ind->sub_block_t.appl_activated.sb_appln.application_type) {
        if (card.startup_complete && apps[card.i].state != ACTIVE_STATE_PASSED) {
            uiccd_sig_uicc_status_payload_ind((uintptr_t)client_tag,
                                              STE_UICC_STATUS_READY,
                                              apps[card.i].app_type, STE_UICC_CARD_TYPE_UNKNOWN);
            uiccd_sig_sim_status((uintptr_t)client_tag, STE_REASON_STARTUP_DONE);
        }
        break;
    }
    if (apps[card.i].app_id == 0) {
      // This may happen if modem activates app and no pin ind yet
      apps[card.i].app_id = ind->appl_id;
      apps[card.i].app_type = ind->sub_block_t.appl_activated.sb_appln.application_type;
      catd_log_f(SIM_LOGGING_I, "MAL_UICC_APPL_ACTIVATED. Set new app id=%d type=%d", apps[card.i].app_id , apps[card.i].app_type);
      break;
    }
    if (apps[card.i].app_id == ind->appl_id && apps[card.i].app_type == 0) {
      // app type may be null if we got only app_id so far from a pin ind
      apps[card.i].app_type = ind->sub_block_t.appl_activated.sb_appln.application_type;
      catd_log_f(SIM_LOGGING_I, "MAL_UICC_APPL_ACTIVATED. App id=%d set type=%d", apps[card.i].app_id , apps[card.i].app_type);
      break;
    }
    if (apps[card.i].app_id != ind->appl_id ||
        apps[card.i].app_type != ind->sub_block_t.appl_activated.sb_appln.application_type)
      {
        catd_log_f(SIM_LOGGING_I, "MAL_UICC_APPL_ACTIVATED. Multiple app id=%d type=%d?", apps[card.i].app_id , apps[card.i].app_type);
        break;
      }
    break;
  case MAL_UICC_APPL_TERMINATED:
    apps[card.i].app_state = MAL_UICC_STATUS_APPL_NOT_ACTIVE;
  case MAL_UICC_APPL_RECOVERED:
  default:
    break;
  }
}

/*
 * Call back function handling get file information. Note: Using client
 * tag in appl_cmd_resp_cb, this was identified. The client tag has already
 * been cleaned up.
 */
static void handle_file_info_cb(uicc_appl_cmd_resp_t *resp,
                                void *client_tag,
                                sim_uicc_status_code_t uicc_status_code,
                                sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                sim_uicc_status_word_t status_word)
{
  uint8_t *fci = NULL;
  uint32_t fci_length = 0;
  ste_sim_status_t status = STE_SIM_STATUS_FAIL;

  if (resp->status == MAL_UICC_STATUS_OK) {
    status = STE_SIM_STATUS_OK;
    fci = resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci;
    fci_length = resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci_length;
  }
  uiccd_sig_get_file_information_response((uintptr_t)client_tag,
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          status_word,
                                          fci,
                                          (int)fci_length);
}

/*
 * Given the file info byte of usim fcp, this returns symbolic name.
 */
static sim_file_structure_t decode_file_type_usim(uint8_t b) {
  // A strange case?
  if ((b & 0xBF) == 0x39) return SIM_FILE_STRUCTURE_BER_TLV;
  if ((b & 0x38) == 0x38) return SIM_FILE_STRUCTURE_DF; // DF or ADF
  switch (b&0x07) {
  case 0:
    return SIM_FILE_STRUCTURE_NO_INFO_GIVEN;
  case 0x01:
    return SIM_FILE_STRUCTURE_TRANSPARENT;
  case 0x02:
    return SIM_FILE_STRUCTURE_LINEAR_FIXED;
  case 0x06:
    return SIM_FILE_STRUCTURE_CYCLIC;
  default:
    return SIM_FILE_STRUCTURE_UNKNOWN;
  }
}

/*
 * Handle file format for usim app. The function converts info from the fcp
 * into sim app independent format so only one sig function is needed for
 * uiccd.
 */
static void handle_file_format_usim_cb(uicc_appl_cmd_resp_t *resp,
                                       void *client_tag,
                                       sim_uicc_status_code_t uicc_status_code,
                                       sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                       sim_uicc_status_word_t status_word)
{
  uint8_t *fcp = NULL;
  ste_sim_status_t status = STE_SIM_STATUS_FAIL;
  sim_file_structure_t file_type = SIM_FILE_STRUCTURE_UNKNOWN;
  uint8_t num_records = 0;
  uint16_t record_len = 0;
  uint32_t file_size = 0;

  if (resp->status == MAL_UICC_STATUS_OK &&
      resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci != NULL &&
      resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci_length > 0) {
    status = STE_SIM_STATUS_OK;
    fcp = resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci;
    int j, i = 2;
    // Decode fci for info we want (called fcp in std)
    while (i < fcp[1]) {
      switch (fcp[i]) {
      case 0x82: // File descriptor
        file_type = decode_file_type_usim(fcp[i+2]);
        if (file_type == SIM_FILE_STRUCTURE_CYCLIC ||
            file_type == SIM_FILE_STRUCTURE_LINEAR_FIXED) {
          record_len = ((uint16_t)fcp[i+4])*256+fcp[i+5];
          num_records = (uint8_t)fcp[i+6];
        }
        break;
      case 0x80: // File size
        for(j = 0; j<fcp[i+1]; j++)
          file_size = file_size*256+fcp[i+2+j];
        break;
      default:
        break;
      }
      i = i+2+fcp[i+1];
    }
  }
  uiccd_sig_sim_file_get_format_response(
      (uintptr_t)client_tag,
      uicc_status_code,
      uicc_status_code_fail_details,
      status_word,
      file_type,
      (int)file_size,
      (int)record_len,
      (int)num_records);
}

/*
 * Handle file format for gsm app. The function converts info from the fcp
 * into sim app independent format so only one sig function is needed for
 * uiccd.
 */
static void handle_file_format_gsm_cb(uicc_appl_cmd_resp_t *resp,
                                      void *client_tag,
                                      sim_uicc_status_code_t uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                      sim_uicc_status_word_t status_word)
{
  uint8_t *fcp = NULL;
  ste_sim_status_t status = STE_SIM_STATUS_FAIL;
  sim_file_structure_t file_type = SIM_FILE_STRUCTURE_UNKNOWN;
  uint32_t file_size = 0;
  uint8_t num_records = 0;
  uint16_t record_len = 0;

  if (resp->status == MAL_UICC_STATUS_OK &&
      resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci != NULL &&
      resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci_length > 0) {
    status = STE_SIM_STATUS_OK;
    fcp = resp->sub_block.appl_file_info_resp.uicc_sb_fci.fci;
    // Decode fci for GSM.
    // Map to sim_file_structure_t since that is simpler to work with than
    // sim std type.
    file_size=((uint32_t)fcp[2]*256+fcp[3]);
    switch (fcp[6]) {
    case 0x01: file_type=SIM_FILE_STRUCTURE_MF; break;
    case 0x02: file_type=SIM_FILE_STRUCTURE_DF; break;
    case 0x04:
      switch(fcp[13]) {
      case 0x00: file_type=SIM_FILE_STRUCTURE_TRANSPARENT; break;
      case 0x01:
        file_type=SIM_FILE_STRUCTURE_LINEAR_FIXED;
        record_len=(uint16_t)fcp[14];
        num_records=file_size/record_len;
        break;
      case 0x03:
        file_type=SIM_FILE_STRUCTURE_CYCLIC;
        record_len=(uint16_t)fcp[14];
        num_records=file_size/record_len;
        break;
      default: file_type=SIM_FILE_STRUCTURE_UNKNOWN; break;
      };
      break;
    default:
      file_type=SIM_FILE_STRUCTURE_UNKNOWN; break;
    }
  }
  uiccd_sig_sim_file_get_format_response((uintptr_t)client_tag,
                                         uicc_status_code,
                                         uicc_status_code_fail_details,
                                         status_word,
                                         (int)file_type,
                                         (int)file_size,
                                         (int)record_len,
                                         (int)num_records);
}

static void handle_read_record_cb(uicc_appl_cmd_resp_t *resp,
                                  void *client_tag,
                                  sim_uicc_status_code_t uicc_status_code,
                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                  sim_uicc_status_word_t status_word)
{
  uint32_t read_length = 0;
  uint8_t *read_data = NULL;
  ste_sim_status_t status = STE_SIM_STATUS_FAIL;

  if (resp->status == MAL_UICC_STATUS_OK) {
    status = MAL_UICC_STATUS_OK;
    read_length = resp->sub_block.appl_file_read_resp.data.data_length;
    read_data = resp->sub_block.appl_file_read_resp.data.data;
  }
  uiccd_sig_read_sim_file_record_response((uintptr_t)client_tag,
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          status_word,
                                          read_data,
                                          read_length);
}

static void handle_read_binary_cb(uicc_appl_cmd_resp_t *resp,
                                  void *client_tag,
                                  sim_uicc_status_code_t uicc_status_code,
                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                  sim_uicc_status_word_t status_word)
{
  uint32_t read_length = 0;
  uint8_t *read_data = NULL;
  ste_sim_status_t status = STE_SIM_STATUS_FAIL;

  if (resp->status == MAL_UICC_STATUS_OK) {
    status = STE_SIM_STATUS_OK;
    read_length = resp->sub_block.appl_file_read_resp.data.data_length;
    read_data = resp->sub_block.appl_file_read_resp.data.data;
  }
  uiccd_sig_read_sim_file_binary_response((uintptr_t)client_tag,
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          status_word,
                                          read_data,
                                          read_length);
}

static void handle_update_binary_cb(uicc_appl_cmd_resp_t *UNUSED(resp),
                                   struct ste_cmd_tag_s *my_client_tag,
                                   sim_uicc_status_code_t uicc_status_code,
                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                   sim_uicc_status_word_t status_word) {
    int tag = my_client_tag->tag;
    void *client_tag = my_client_tag->ct;
    free(my_client_tag);

    if (tag == STE_CMD_ADAPT_TAG) {
        // After adapt terminal support table file update attempt, cat server
        // ready is sent (provided that tpdl worded) regardless of status.
        // This means that the client has one shot at update.
        boolean adapt_file_updated = FALSE;
        if (uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            adapt_file_updated = TRUE;
        }
        else {
            adapt_file_updated = FALSE;
            catd_log_f(SIM_LOGGING_V, "handle_update_binary_cb: Status=%d, details=%d, sw1=0x%x sw2=0x%x", uicc_status_code, uicc_status_code_fail_details, status_word.sw1, status_word.sw2);
        }
        catd_sig_adapt_terminal_support_table_update_status(
            (uintptr_t)client_tag,
            adapt_file_updated, status_word);
        if (card.got_cat == CAT_TPDL_SUBSTATE_TPDL_DONE) {
            card.got_cat = CAT_TPDL_SUBSTATE_ADAPT_DONE;
        }
        catd_sig_cat_server_ready((uintptr_t)client_tag, 0);
    }
    else {
        uiccd_sig_update_sim_file_binary_response((uintptr_t)client_tag,
                                                  uicc_status_code,
                                                  uicc_status_code_fail_details,
                                                  status_word, NULL, 0);
    }
}

static void handle_appl_apdu_send_cb(uicc_appl_cmd_resp_t *resp,
                                     void *ct,
                                     sim_uicc_status_code_t uicc_status_code,
                                     sim_uicc_status_code_fail_details_t uicc_status_code_fail_details)
{
  const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.appl_apdu_send_resp.uicc_sb_apdu.apdu;
  size_t         len    = (size_t)resp->sub_block.appl_apdu_send_resp.uicc_sb_apdu.apdu_length;
  const uint8_t* apdu_data = NULL;
  size_t         apdu_len = 0;
  sim_uicc_status_word_t status_word;
  struct ste_cmd_tag_s *my_ct = ct;
  int            tag;
  uintptr_t      client_tag;

  catd_log_f(SIM_LOGGING_I, "handle_appl_apdu_send_cb: Enter");

  if (ct == NULL) {
      catd_log_f(SIM_LOGGING_E, "handle_appl_apdu_send_cb: context is NULL - exiting");
      return;
  }
  tag = my_ct->tag;
  client_tag = (uintptr_t)my_ct->ct;
  free(my_ct);

  assert(r_apdu);
  assert(len >= 2); // See explanation for >2 below.

  catd_log_f(SIM_LOGGING_I, "handle_appl_apdu_send_cb: MAL_UICC_APPL_APDU_SEND R-APDUlen=%d", len);

  /*
   * The MAL sends us an R-APDU that includes the apdu plus
   * two trailing bytes, that are the status words (SW1 & SW2).
   */

  if (resp->status == MAL_UICC_STATUS_OK){
     if (len >= 2) {
        apdu_data = r_apdu;
        apdu_len = len-2;
     }
  }

  if (len >= 2) {
    status_word.sw1 = r_apdu[len-2];
    status_word.sw2 = r_apdu[len-1];
  }
  else{
    status_word.sw1 = 0;
    status_word.sw2 = 0;
  }

  switch ( tag ) {
  case STE_CMD_APPLICATION_APDU_TAG:
  {
      uiccd_sig_appl_apdu_send_response(client_tag,
                                        uicc_status_code,
                                        uicc_status_code_fail_details,
                                        status_word,
                                        apdu_data,
                                        apdu_len);
  }
  break;
  case STE_CMD_APDU_NORMAL_SEND:
  {
      // In this case, send the status words at the end of the R-APDU as well
      uiccd_sig_sim_channel_send_response(client_tag,
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          status_word,
                                          r_apdu,
                                          len);
  }
  break;
  }
}

static void handle_mal_uicc_appl_cmd_resp_cb(int UNUSED(eventID), void *data, int mal_error, void *client_tag) {
  uicc_appl_cmd_resp_t *resp = (uicc_appl_cmd_resp_t *)data;
  sim_uicc_status_word_t status_word;
  sim_uicc_status_code_t uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;

  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  convert_mal_status_code(mal_error,
                          resp->status,
                          resp->details,
                          &uicc_status_code,
                          &uicc_status_code_fail_details);

  switch (resp->service_type) {
  case MAL_UICC_APPL_READ_TRANSPARENT:
    status_word.sw1 = resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw2;

    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_appl_cmd_resp_cb: MAL_UICC_APPL_READ_TRANSPARENT: status=0x%x sw1=0x%x sw2=0x%x",
                 resp->status,
                 resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw1,
                 resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw2);

    handle_read_binary_cb(resp,
                          client_tag,
                          uicc_status_code,
                          uicc_status_code_fail_details,
                          status_word);
    break;
  case MAL_UICC_APPL_READ_LINEAR_FIXED:
    status_word.sw1 = resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw2;

    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_appl_cmd_resp_cb: MAL_UICC_APPL_READ_LINEAR_FIXED: status=0x%x sw1=0x%x sw2=0x%x",
                 resp->status,
                 resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw1,
                 resp->sub_block.appl_file_read_resp.uicc_sb_status_word.sw2);

    handle_read_record_cb(resp,
                          (struct ste_req_cb_hook_s *)client_tag,
                          uicc_status_code,
                          uicc_status_code_fail_details,
                          status_word);
    break;
  case MAL_UICC_APPL_UPDATE_TRANSPARENT:
    status_word.sw1 = resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw2;

    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_appl_cmd_resp_cb: MAL_UICC_APPL_UPDATE_TRANSPARENT: status=0x%x sw1=0x%x sw2=0x%x",
                 resp->status,
                 resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw1,
                 resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw2);

    handle_update_binary_cb(resp,
                           client_tag,
                           uicc_status_code,
                           uicc_status_code_fail_details,
                           status_word);
    break;
  case MAL_UICC_APPL_UPDATE_LINEAR_FIXED: {
    status_word.sw1 = resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw2;

    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_appl_cmd_resp_cb: MAL_UICC_APPL_UPDATE_LINEAR_FIXED: status=0x%x sw1=0x%x sw2=0x%x",
                 resp->status,
                 resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw1,
                 resp->sub_block.appl_file_update_resp.uicc_sb_status_word.sw2);

    if (resp->status == MAL_UICC_STATUS_OK) {
      uiccd_sig_update_sim_file_record_response((uintptr_t)client_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word, NULL, 0);
    }
    else
      uiccd_sig_update_sim_file_record_response((uintptr_t)client_tag,
                                                uicc_status_code,
                                                uicc_status_code_fail_details,
                                                status_word, NULL, 0);
    break;
  }
  case MAL_UICC_APPL_FILE_INFO: {
    int tag;
    void *ct;
    struct ste_cmd_tag_s *my_ct = (struct ste_cmd_tag_s *)client_tag;

    tag = my_ct->tag;
    ct = my_ct->ct;
    free(my_ct);

    status_word.sw1 = resp->sub_block.appl_file_info_resp.uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block.appl_file_info_resp.uicc_sb_status_word.sw2;

    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_appl_cmd_resp_cb: MAL_UICC_APPL_FILE_INFO: status=0x%x sw1=0x%x sw2=0x%x",
                 resp->status,
                 resp->sub_block.appl_file_info_resp.uicc_sb_status_word.sw1,
                 resp->sub_block.appl_file_info_resp.uicc_sb_status_word.sw2);

    if (tag == STE_CMD_FILEINFO_INFO_TAG) {
        handle_file_info_cb(resp, ct, uicc_status_code, uicc_status_code_fail_details, status_word);
    }
    else if (apps[card.i].app_type == MAL_UICC_APPL_TYPE_ICC_SIM) {
        handle_file_format_gsm_cb(resp, ct, uicc_status_code, uicc_status_code_fail_details, status_word);
    }
    else if (apps[card.i].app_type == MAL_UICC_APPL_TYPE_UICC_USIM) {
        handle_file_format_usim_cb(resp, ct, uicc_status_code, uicc_status_code_fail_details, status_word);
    }
    else {
        uiccd_sig_sim_file_get_format_response((uintptr_t)ct,
                                               SIM_UICC_STATUS_CODE_FAIL,
                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                               status_word,
                                               SIM_FILE_STRUCTURE_UNKNOWN,
                                               0,
                                               0,
                                               0);
    }
    break;
  }
  case MAL_UICC_APPL_APDU_SEND: {
    catd_log_f(SIM_LOGGING_D, "handle_mal_uicc_appl_cmd_resp_cb : APDU_SEND, status=0x%x", resp->status);

    handle_appl_apdu_send_cb(resp,
                             client_tag,
                             uicc_status_code,
                             uicc_status_code_fail_details);
    break;
  }
  default:
    break;
  }
}

static void handle_mal_uicc_pin_ind_cb(int UNUSED(eventID), void *data, int UNUSED(mal_error), void *client_tag) {

  uicc_pin_ind_t *ind = (uicc_pin_ind_t *)data;
  ste_uicc_status_t uicc_status = STE_UICC_STATUS_UNKNOWN;
  ste_reason_t sim_status = STE_REASON_UNKNOWN;

  if (!ind) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_ind_cb: service_type=%d", ind->service_type);
  pin_lock.app_id = ind->appl_id;
  pin_lock.pin_id = ind->pin_id;

  if (ind->pin_id == apps[card.i].pin_id)
    apps[card.i].pin_ind = ind->service_type;
  else if (ind->pin_id == apps[card.i].pin2_id)
    apps[card.i].pin2_ind = ind->service_type;

  switch(ind->service_type) {
  case MAL_UICC_PIN_VERIFY_NEEDED:
    pin_lock.state = PIN_STATE_PINLOCKED;
    if (ind->pin_id == apps[card.i].pin_id) {
      uicc_status = STE_UICC_STATUS_PIN_NEEDED;
      sim_status = STE_REASON_PIN_NEEDED;
    } else if (ind->pin_id == apps[card.i].pin2_id) {
      uicc_status = STE_UICC_STATUS_PIN2_NEEDED;
      sim_status = STE_REASON_PIN2_NEEDED;
    }  else
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_pin_ind_cb: Unknown pin id=%d for verify", ind->pin_id);
    break;
  case MAL_UICC_PIN_UNBLOCK_NEEDED:
    pin_lock.state = PIN_STATE_PUKLOCKED;
    if (ind->pin_id == apps[card.i].pin_id) {
      uicc_status = STE_UICC_STATUS_PUK_NEEDED;
      sim_status = STE_REASON_PUK_NEEDED;
    } else if (ind->pin_id == apps[card.i].pin2_id) {
      uicc_status = STE_UICC_STATUS_PUK2_NEEDED;
      sim_status = STE_REASON_PUK2_NEEDED;
    } else
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_pin_ind_cb: Unknown pin id=%d for unblock", ind->pin_id);
    break;
  case MAL_UICC_PIN_PERMANENTLY_BLOCKED:
    pin_lock.state = PIN_STATE_PERMLOCKED;
    if(ind->pin_id == apps[card.i].pin_id) {
      uicc_status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
    }
    sim_status = STE_REASON_PERMANENTLY_BLOCKED;
    break;
  case MAL_UICC_PIN_VERIFIED:
    catd_log_f(SIM_LOGGING_D, "MAL_UICC_PIN_VERIFIED app id=%d pin id=%d", ind->appl_id, ind->pin_id);
    // Silence these while in startup
    if (apps[card.i].state == ACTIVE_STATE_PASSED) {
      switch (pin_lock.state) {
      case PIN_STATE_NOT_VERIFIED:
        catd_log_f(SIM_LOGGING_I, "MAL_UICC_PIN_VERIFIED: PIN_STATE_NOT_VERIFIED");
        // PIN_RESP ok has been received
        pin_lock.state = PIN_STATE_UNKNOWN;
        // Fall through
      case PIN_STATE_UNKNOWN:
        catd_log_f(SIM_LOGGING_I, "MAL_UICC_PIN_VERIFIED: PIN_STATE_UNKNOWN");
        if (ind->pin_id == apps[card.i].pin_id) {
          uicc_status = STE_UICC_STATUS_PIN_VERIFIED;
          sim_status = STE_REASON_PIN_VERIFIED;
        } else if (ind->pin_id == apps[card.i].pin2_id) {
          uicc_status = STE_UICC_STATUS_PIN2_VERIFIED;
          sim_status = STE_REASON_PIN2_VERIFIED;
        }
        pin_lock.app_id = 0;
        pin_lock.pin_id = 0;
        break;
      case PIN_STATE_PINLOCKED:
      case PIN_STATE_PUKLOCKED:
        catd_log_f(SIM_LOGGING_I, "MAL_UICC_PIN_VERIFIED: PIN_STATE_LOCKED");
        // This ind got before ok rsp
        pin_lock.state = PIN_STATE_PRE_VERIFIED;
        catd_log_f(SIM_LOGGING_I, "Cached early pin verified");
        break;
      default:
        catd_log_f(SIM_LOGGING_I, "MAL_UICC_PIN_VERIFIED: default");
        break;
      }
    }
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_pin_ind_cb: unhandled service type=0x%x", ind->service_type);
    return;
  }
  // Just signal known status codes
  if (uicc_status != STE_UICC_STATUS_UNKNOWN)
    uiccd_sig_uicc_status_ind((uintptr_t)client_tag, uicc_status);
  if (sim_status != STE_REASON_UNKNOWN)
    uiccd_sig_sim_status((uintptr_t)client_tag, sim_status);
}

static void update_pin_state(sim_uicc_status_word_t *status_word)
{
    if ((card.type == MAL_UICC_CARD_TYPE_ICC && (status_word->sw1 == 0x98 && status_word->sw2 == 0x40)) ||
        (card.type == MAL_UICC_CARD_TYPE_UICC && ((status_word->sw1 == 0x63 && status_word->sw2 == 0xc0) ||
                                                  (status_word->sw1 == 0x69 && status_word->sw2 == 0x83)))) {
        if (pin_lock.state == PIN_STATE_PUKLOCKED) {
            /* Card has been permanently blocked */
            pin_lock.state = PIN_STATE_PERMLOCKED;
            catd_log_f(SIM_LOGGING_I,"uicc : Incorrect PUK, card is now permanently blocked!");
        } else {
            pin_lock.state = PIN_STATE_PUKLOCKED;
            catd_log_f(SIM_LOGGING_I,"uicc : Incorrect PIN, card is now PUK locked!");
        }
    } else {
        catd_log_f(SIM_LOGGING_I,"uicc : Incorrect PIN, PIN2, PUK or PUK2 code: Try again.");
    }

    return;
}

static void handle_mal_uicc_pin_resp_cb(int UNUSED(eventID), void *data, int mal_error, void *client_tag) {
  uicc_pin_resp_t *resp = (uicc_pin_resp_t *)data;
  sim_uicc_status_word_t status_word;
  sim_uicc_status_code_t uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  struct ste_cmd_tag_s *temp_ct = NULL; /* Temporary structure used for PIN verify and change */
  void *ct;
  int tag;

  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  convert_mal_status_code(mal_error,
                          resp->status,
                          resp->details,
                          &uicc_status_code,
                          &uicc_status_code_fail_details);

  catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb");
  switch  (resp->service_type) {
  case MAL_UICC_PIN_VERIFY:
    /* Extract the internal client tag */
    temp_ct = client_tag;
    ct = temp_ct->ct;
    tag = temp_ct->tag;

    status_word.sw1 = resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw2;

    if (resp->status == MAL_UICC_STATUS_OK) {
      // active pin_levels should not be affected
      catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb: PIN_VERIFY OK",
                 resp->status);
      uiccd_sig_pin_verify_response((uintptr_t)ct, uicc_status_code, uicc_status_code_fail_details, status_word);
      if (apps[card.i].state == ACTIVE_STATE_PASSED) {
        switch (pin_lock.state) {
        case PIN_STATE_PINLOCKED:
        case PIN_STATE_PUKLOCKED:
          catd_log_f(SIM_LOGGING_I, "PIN_RESP: PIN_LOCKED");
          pin_lock.state = PIN_STATE_NOT_VERIFIED;
          break;
        case PIN_STATE_PRE_VERIFIED:
          catd_log_f(SIM_LOGGING_I, "PIN_RESP: PIN_STATE_PRE_VERIFIED");
          // Signal cached premature ind
          if (pin_lock.pin_id == apps[card.i].pin_id) {
            uiccd_sig_uicc_status_ind((uintptr_t)ct,
                                      STE_UICC_STATUS_PIN_VERIFIED);
            uiccd_sig_sim_status((uintptr_t)ct, STE_REASON_PIN_VERIFIED);
          } else if (pin_lock.pin_id == apps[card.i].pin2_id) {
            uiccd_sig_uicc_status_ind((uintptr_t)ct, STE_UICC_STATUS_PIN2_VERIFIED);
            uiccd_sig_sim_status((uintptr_t)ct, STE_REASON_PIN2_VERIFIED);
          } else
            // Verify was for a pin id that did not need verification, break out
            // silently
            break;

          pin_lock.state = PIN_STATE_UNKNOWN;
          pin_lock.app_id = 0;
          pin_lock.pin_id = 0;
          break;
        default:
          catd_log_f(SIM_LOGGING_I, "PIN_RESP: default");
          // Strange to get something else
          break;
        }
      } else {
        pin_lock.state = PIN_STATE_UNKNOWN;
        pin_lock.app_id = 0;
        pin_lock.pin_id = 0;
      }

      break;
    } else {
      status_word.sw1 = resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw1;
      status_word.sw2 = resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw2;

      catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb: PIN_VERIFY WRONG: n_sb=%d sw1=0x%x sw2=0x%x",
                 resp->n_sb,
                 resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw1,
                 resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw2);

      update_pin_state(&status_word);

      uiccd_sig_pin_verify_response((uintptr_t)ct, uicc_status_code, uicc_status_code_fail_details, status_word);
    }
    break;

  case MAL_UICC_PIN_UNBLOCK:
    status_word.sw1 = resp->sub_block_t.uicc_pin_unblock_resp.pin_uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block_t.uicc_pin_unblock_resp.pin_uicc_sb_status_word.sw2;

    if (resp->status == MAL_UICC_STATUS_OK) {
      catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb: PUK_VERIFY OK",
                 resp->status);
      uiccd_sig_pin_unblock_response((uintptr_t)client_tag, uicc_status_code, uicc_status_code_fail_details, status_word);
      // It might happen that someone does a pin unblock even not necessary
      if (apps[card.i].pin_level == PIN_ENABLED_PUK &&
          pin_lock.pin_id == apps[card.i].pin_id)
        apps[card.i].pin_level = PIN_ENABLED;
      else if (apps[card.i].pin2_level == PIN_ENABLED_PUK &&
               pin_lock.pin_id == apps[card.i].pin2_id)
        apps[card.i].pin2_level = PIN_ENABLED;
      // else no action?
      if (apps[card.i].state == ACTIVE_STATE_PASSED) {
        switch (pin_lock.state) {
        case PIN_STATE_PINLOCKED:
        case PIN_STATE_PUKLOCKED:
          catd_log_f(SIM_LOGGING_I, "PUK_RESP: PUK_LOCKED");
          pin_lock.state = PIN_STATE_NOT_VERIFIED;
          break;
        case PIN_STATE_PRE_VERIFIED:
          catd_log_f(SIM_LOGGING_I, "PUK_RESP: PIN_STATE_PRE_VERIFIED");
          // Signal cached premature ind
          if (pin_lock.pin_id == apps[card.i].pin_id) {
            uiccd_sig_uicc_status_ind((uintptr_t)client_tag, STE_UICC_STATUS_PIN_VERIFIED);
            uiccd_sig_sim_status((uintptr_t)client_tag, STE_REASON_PIN_VERIFIED);
          } else if (pin_lock.pin_id == apps[card.i].pin2_id) {
            uiccd_sig_uicc_status_ind((uintptr_t)client_tag, STE_UICC_STATUS_PIN2_VERIFIED);
            uiccd_sig_sim_status((uintptr_t)client_tag, STE_REASON_PIN2_VERIFIED);
          } else
            // Unblock was for pin id not needing verification, break out
            // silently
            break;
          pin_lock.state = PIN_STATE_UNKNOWN;
          pin_lock.app_id = 0;
          pin_lock.pin_id = 0;
          break;
        default:
          catd_log_f(SIM_LOGGING_I, "PUK_RESP: default");
          // Strange to get something else
          break;
        }
      } else {
        pin_lock.state = PIN_STATE_UNKNOWN;
        pin_lock.app_id = 0;
        pin_lock.pin_id = 0;
      }
      break;
    } else if (resp->status == MAL_UICC_STATUS_FAIL) {

      catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb: PUK_VERIFY WRONG: n_sb=%d sw1=0x%x sw2=0x%x",
                 resp->n_sb,
                 resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw1,
                 resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw2);

      update_pin_state(&status_word);

      uiccd_sig_pin_unblock_response((uintptr_t)client_tag, uicc_status_code, uicc_status_code_fail_details, status_word);
      break;
    }
    catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_pin_resp_cb: MAL_UICC_PIN_UNBLOCK unknown status=0x%x", resp->status);
    uiccd_sig_pin_unblock_response((uintptr_t)client_tag, uicc_status_code, uicc_status_code_fail_details, status_word);
    break;
  case MAL_UICC_PIN_DISABLE:
    status_word.sw1 = resp->sub_block_t.uicc_pin_disable_resp.pin_uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block_t.uicc_pin_disable_resp.pin_uicc_sb_status_word.sw2;

    // Only PIN1 disable is allowed
    if (resp->status == MAL_UICC_STATUS_OK) {
      apps[card.i].pin_level = PIN_DISABLED;
    } else if (resp->status == MAL_UICC_STATUS_FAIL) {
      update_pin_state(&status_word);
    }

    uiccd_sig_pin_disable_response((uintptr_t)client_tag, uicc_status_code, uicc_status_code_fail_details, status_word);
    break;
  case MAL_UICC_PIN_ENABLE:
    status_word.sw1 = resp->sub_block_t.uicc_pin_enable_resp.pin_uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block_t.uicc_pin_enable_resp.pin_uicc_sb_status_word.sw2;

    // Currently, the api only supports enabling pin, not pin2
    if (resp->status == MAL_UICC_STATUS_OK) {
      apps[card.i].pin_level = PIN_ENABLED;
    } else if (resp->status == MAL_UICC_STATUS_FAIL) {
      update_pin_state(&status_word);
    }

    uiccd_sig_pin_enable_response((uintptr_t)client_tag, uicc_status_code, uicc_status_code_fail_details, status_word);
    break;
  case MAL_UICC_PIN_CHANGE:
    /* Extract the internal client tag */
    temp_ct = client_tag;
    ct = temp_ct->ct;
    tag = temp_ct->tag;

    status_word.sw1 = resp->sub_block_t.uicc_pin_change_resp.pin_uicc_sb_status_word.sw1;
    status_word.sw2 = resp->sub_block_t.uicc_pin_change_resp.pin_uicc_sb_status_word.sw2;

    if (resp->status == MAL_UICC_STATUS_FAIL) {
        update_pin_state(&status_word);
    }

    uiccd_sig_pin_change_response((uintptr_t)ct, uicc_status_code, uicc_status_code_fail_details, status_word);
    break;
  case MAL_UICC_PIN_INFO:{
    uicc_sb_pin_info_t *info = &resp->sub_block_t.uicc_pin_info_resp.pin_uicc_sb_pin_info;
    ste_sim_pin_status_t pin_status;
    ste_sim_status_t     sim_status;
    int                 attempts = 0, attempts2 = 0;
    temp_ct = client_tag;
    ct = temp_ct->ct;
    tag = temp_ct->tag;

    if (resp->status == MAL_UICC_STATUS_OK) {
      if (tag == STE_CMD_PIN_INFO_PIN_ONLY) {
        attempts = info->pin_att;
      } else if (tag == STE_CMD_PIN_INFO_PUK_ONLY){
        attempts = info->puk_att;
      } else { // Both
        attempts = info->pin_att;
        attempts2 = info->puk_att;
      }

      switch (info->pin_status) {
          case MAL_UICC_STATUS_PIN_ENABLED:
              pin_status = STE_SIM_PIN_STATUS_ENABLED;
          break;
          case MAL_UICC_STATUS_PIN_DISABLED:
              pin_status = STE_SIM_PIN_STATUS_DISABLED;
          break;
          default:
              pin_status = STE_SIM_PIN_STATUS_UNKNOWN;
          break;
      }
      uiccd_sig_pin_info_response((uintptr_t)ct, uicc_status_code, uicc_status_code_fail_details, pin_status, attempts, attempts2);
    } else {
      switch (resp->status) {
          case MAL_UICC_STATUS_FAIL:
              sim_status = STE_SIM_STATUS_FAIL;
          break;
          case MAL_UICC_STATUS_UNKNOWN:
              sim_status = STE_SIM_STATUS_UNKNOWN;
          break;
          default:
              sim_status = STE_SIM_STATUS_UNKNOWN;
          break;
      }

      // The request was unsuccessful and no information was provided in the response.
      uiccd_sig_pin_info_response((uintptr_t)ct, uicc_status_code, uicc_status_code_fail_details, STE_SIM_PIN_STATUS_UNKNOWN, 0, 0);
    }
  }
  break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_pin_resp_cb: unknown service type 0x%x", resp->service_type);
    break;
  }

  free(temp_ct);
}

static void handle_mal_uicc_cat_ind_cb(int32_t UNUSED(eventID), void *data, void *client_tag) {
  uicc_cat_ind_t *resp = (uicc_cat_ind_t *)data;


  if (!resp) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  switch  (resp->service_type) {
  case MAL_UICC_READY:
      // check if this is a reset/fallback request or normal startup
      // for now, we only check for fallback, but in the future
      // if recovery support is needed, then the if() check should be changed
      catd_log_f(SIM_LOGGING_I, "handle_mal_uicc_cat_ind_cb: MAL_UICC_READY. card.cat_ind_type = %d, resp->card_type = %d", card.cat_ind_type, resp->card_type);
      card.type = resp->card_type;
      if (card.cat_ind_type != 0 && resp->card_type != card.cat_ind_type) {
          sim_mal_card_fallback((uintptr_t)client_tag);
      } else {
          catd_sig_modem_ready((uintptr_t)client_tag);
      }
      card.cat_ind_type = resp->card_type;
      break;
  case MAL_UICC_CAT_NOT_SUPPORTED:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_ind_cb: MAL_UICC_CAT_NOT_SUPPORTED: card type=%d",
        resp->card_type);
    catd_sig_modem_cat_not_supported((uintptr_t)client_tag);
    break;
  case MAL_UICC_CAT_FETCHED_CMD:
  {
    const uint8_t* r_apdu = (const uint8_t*)resp->sub_block.uicc_cat_fetched_cmd.uicc_sb_apdu.apdu;
    size_t         len    = (size_t)resp->sub_block.uicc_cat_fetched_cmd.uicc_sb_apdu.apdu_length;
    ste_apdu_t*    apdu   = NULL;

    assert(r_apdu);
    assert(len > 2); // See explanation for >2 below.

    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_ind_cb: MAL_UICC_CAT_FETCHED_CMD R-APDUlen=%d", len);

    /*
     * The MAL (or rather the modem below in this case) sends us a R-APDU that includes two trailing bytes,
     * which we have to remove. The last two bytes are actually the status words (SW1 & SW2).
     */
    apdu = ste_apdu_new(r_apdu, len - 2);
    catd_sig_apdu(apdu, (uintptr_t)client_tag);
    ste_apdu_delete(apdu);
    break;
  }
  case MAL_UICC_CAT_REG_OK:
      // catd_sig_cat_server_ready((uintptr_t)client_tag, 0);
      catd_sig_modem_ind_reg_ok((uintptr_t)client_tag);
      card.got_cat = CAT_TPDL_SUBSTATE_TPDL_DONE;
      card.cat_ind_type = resp->card_type;
      break;
  case MAL_UICC_CAT_REG_FAILED:
      card.got_cat = CAT_TPDL_SUBSTATE_FAIL;
      catd_sig_modem_not_ready((uintptr_t)client_tag);
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_ind_cb: unimplemented service_type=0x%x",
        resp->service_type);
  }
}

static void handle_mal_uicc_cat_resp_cb(int UNUSED(eventID), void *data, void *client_tag) {
  uicc_cat_resp_t *resp = (uicc_cat_resp_t *)data;
  ste_apdu_t* apdu = NULL;
  struct ste_cmd_tag_s *my_ct = client_tag;

  if (resp == 0 || my_ct == NULL ) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: NULL response");
      free(my_ct);
      return;
  }

  switch (resp->service_type) {
  case MAL_UICC_CAT_TERMINAL_PROFILE:
      if ( my_ct->tag != STE_CMD_TPDL_TAG ) {
          catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad TP context=%d", my_ct->tag );
      } else {
          catd_sig_tpdl_status((uintptr_t)my_ct->ct, resp->status);
          break;
      }
  case MAL_UICC_CAT_POLLING_SET:
  {
      poll_interval_tag_t* my_ct = (poll_interval_tag_t*)client_tag;
      int interval;

      // Fetch actual poll interval used and original client tag
      interval = my_ct->interval * 10; // modem uses seconds, client expects value in tenths of seconds.
      catd_sig_modem_set_poll_interval_response(resp->status, interval, (uintptr_t)my_ct->client_tag);
      break;
  }

  case MAL_UICC_CAT_ENABLE:
    if ( my_ct->tag != STE_CMD_MODEM_ENABLE_TAG ) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad ENABLE context=%d", my_ct->tag );
    } else {
      catd_sig_enable_status((uintptr_t)my_ct->ct, resp->status);
    }
    break;
  case MAL_UICC_CAT_DISABLE:
    if ( my_ct->tag != STE_CMD_MODEM_DISABLE_TAG ) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad DISABLE context=%d", my_ct->tag );
    } else {
      catd_sig_disable_status((uintptr_t)my_ct->ct, resp->status);
    }
    break;
  case MAL_UICC_CAT_TERMINAL_RESPONSE:
    if ( my_ct->tag != STE_CMD_TERMINAL_RESPONSE_TAG ) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad TR context=%d", my_ct->tag );
    } else {
      apdu = ste_apdu_new((const uint8_t*)resp->sub_block.uicc_cat_terminal_response_resp.uicc_sb_apdu.apdu,
                          (size_t)resp->sub_block.uicc_cat_terminal_response_resp.uicc_sb_apdu.apdu_length);
      catd_sig_modem_cat_response((uintptr_t)my_ct->ct, apdu, resp->status);
      // Apdu is copied within the sig function; hence free this original.
      ste_apdu_delete(apdu);
    }
    break;
  case MAL_UICC_CAT_ENVELOPE:
    if ( my_ct->tag != STE_CMD_ENVELOPE_COMMAND_TAG ) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad EC context=%d", my_ct->tag );
    } else {
      apdu = ste_apdu_new((const uint8_t*)resp->sub_block.uicc_cat_envelope_resp.uicc_sb_apdu.apdu,
                         (size_t)resp->sub_block.uicc_cat_envelope_resp.uicc_sb_apdu.apdu_length);
      catd_sig_modem_cat_response((uintptr_t)my_ct->ct, apdu, resp->status);
      // Apdu is copied within the sig function; hence free this original.
      ste_apdu_delete(apdu);
    }
    break;
  case MAL_UICC_CAT_REFRESH:
    if ( my_ct->tag != STE_CMD_REFRESH_TAG) {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: bad REFRESH context=%d", my_ct->tag );
    } else {
      catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: REFRESH - status=%d, n=%d, type=%d",
              resp->status,
              resp->sub_block.uicc_cat_refresh_resp.nb_refresh_result_instances,
              resp->sub_block.uicc_cat_refresh_resp.uicc_sb_refresh.type);

      catd_sig_modem_refresh_response((uintptr_t)my_ct->ct, resp->status);
    }
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_cat_resp_cb: unimplemented service_type=0x%x",
        resp->service_type);
    break;
  }
  free(my_ct);
}

static void handle_mal_uicc_refresh_ind_cb(int UNUSED(eventID), void *data, void *client_tag)
{
  uicc_refresh_ind_t         *ind = (uicc_refresh_ind_t *)data;
  int                         i;
  struct ste_cmd_tag_s       *my_ct = NULL;
  ste_cat_refresh_ind_type_t  ind_type = STE_CAT_REFRESH_IND_END;
  ste_apdu_refresh_type_t     refresh_type = STE_APDU_REFRESH_RESERVED;


  if (!ind) {
      catd_log_f(SIM_LOGGING_E, "%s: null data received", __PRETTY_FUNCTION__);
      return;
  }

  switch ( ind->type ) {
  case MAL_UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE:
      refresh_type = STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE;
      break;
  case MAL_UICC_REFRESH_NAA_FILE_CHANGE:
      refresh_type = STE_APDU_REFRESH_FILE_CHANGE;
      break;
  case MAL_UICC_REFRESH_NAA_INIT_FILE_CHANGE:
      refresh_type = STE_APDU_REFRESH_INIT_AND_FILE_CHANGE;
      break;
  case MAL_UICC_REFRESH_NAA_INIT:
      refresh_type = STE_APDU_REFRESH_INIT;
      break;
  case MAL_UICC_REFRESH_UICC_RESET:
      refresh_type = STE_APDU_REFRESH_UICC_RESET;
      break;
  case MAL_UICC_REFRESH_NAA_APPLICATION_RESET:
      refresh_type = STE_APDU_REFRESH_3G_APP_RESET;
      break;
  case MAL_UICC_REFRESH_NAA_SESSION_RESET:
      refresh_type = STE_APDU_REFRESH_3G_SESSION_RESET;
      break;
  default:
      break;
  }

  switch ( ind->service_type ) {
  case MAL_UICC_REFRESH_PERMISSION: {
    // refresh permission also has the list of changed files, but no indication
    // of the number of files present (except perhaps NULL pointers)
    uicc_refresh_req_t    req;
    my_ct = malloc(sizeof(*my_ct));

    if (!my_ct) {
        catd_log_f(SIM_LOGGING_E, "%s: failed to create client tag", __func__);
        return;
    }

    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->type = %x\n", ind->type);
    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->appl_id = %x\n", ind->appl_id);
    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->n_sb = %x\n", ind->n_sb);
    ind_type = STE_CAT_REFRESH_IND_PERMINSSION;

    req.service_type = MAL_UICC_REFRESH_STATUS;
    req.status       = MAL_UICC_REFRESH_OK;
    req.client_id    = apps[card.i].client_id;
    req.sub_block_t.uicc_refresh_status.is_refresh_result_present = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.refresh_status = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.additional_info_length = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.additional_info = 0;
    my_ct->tag = STE_CMD_REFRESH_IND_PERMISSION;

    /* The apps structure is reset on refresh reset. The client_id must be stored so that the refresh request
     * can send the REFRESH_DONE with the correct client_id */
    card.refresh_client_id = apps[card.i].client_id;

    i = mal_uicc_request_wrapper(MAL_UICC_REFRESH_REQ, &req, my_ct);
    if ( i != 0 ) {
        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: MAL_UICC_REFRESH_PERMISSION failed to say OK");
    }
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: MAL_UICC_REFRESH_PERMISSION");
    }
    break;
  case MAL_UICC_REFRESH_STARTING: {
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: MAL_UICC_REFRESH_STARTING");
    ind_type = STE_CAT_REFRESH_IND_START;
    }
    break;
  case MAL_UICC_REFRESH_NOW: {
    // refresh permission also has the list of changed files, but no indication
    // of the number of files present (except perhaps NULL pointers)
    uicc_refresh_req_t    req;

    my_ct = malloc(sizeof(*my_ct));

    if (!my_ct) {
        catd_log_f(SIM_LOGGING_E, "%s: failed to create client tag", __func__);
        return;
    }

    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->type = %x\n", ind->type);
    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->appl_id = %x\n", ind->appl_id);
    catd_log_f(SIM_LOGGING_E, "uicc_refresh_ind->n_sb = %x\n", ind->n_sb);
    ind_type = STE_CAT_REFRESH_IND_NOW;

    req.service_type = MAL_UICC_REFRESH_STATUS;
    req.status       = MAL_UICC_REFRESH_DONE;

    /* Since the apps structure is cleared at this time, this cannot be used for
     * retrieving the client_id. This is the reason why the old client id is stored into
     * a temporary parameter. The apps structure will be populated again when the refresh
     * has been completed */
    req.client_id    = card.refresh_client_id;
    req.sub_block_t.uicc_refresh_status.is_refresh_result_present = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.refresh_status = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.additional_info_length = 0;
    req.sub_block_t.uicc_refresh_status.uicc_sb_refresh_result.additional_info = 0;
    my_ct->tag = STE_CMD_REFRESH_IND_NOW;

    i = mal_uicc_request_wrapper(MAL_UICC_REFRESH_REQ, &req, my_ct);
    if ( i != 0 ) {
        catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: MAL_UICC_REFRESH_NOW failed to say DONE");
    }
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: MAL_UICC_REFRESH_NOW");
    }
    break;
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_refresh_ind_cb: unimplemented service_type=0x%x",
        ind->service_type);
  }

  catd_sig_modem_refresh_ind((uintptr_t)client_tag, refresh_type, ind_type, ind->appl_id);

}

static void handle_mal_uicc_refresh_resp_cb(int UNUSED(eventID), void *data, void *client_tag)
{
  uicc_refresh_resp_t *resp = (uicc_refresh_resp_t *)data;
  struct ste_cmd_tag_s *my_ct = client_tag;

  if (!resp || !my_ct ) {
      catd_log_f(SIM_LOGGING_E, "%s: NULL client tag or NULL response", __func__);
      return;
  }

  if (my_ct->tag != STE_CMD_REFRESH_IND_PERMISSION && my_ct->tag != STE_CMD_REFRESH_IND_NOW) {
      catd_log_f(SIM_LOGGING_E, "%s: wrong client tag", __func__);
      return;
  }

  catd_log_f(SIM_LOGGING_E, "%s: type=%x, status=%d, details=%d", __func__,
          resp->service_type, resp->status, resp->details);

  free(my_ct);
}

static void handle_mal_uicc_connector_resp_cb(int UNUSED(eventID), void *data, int mal_error, void *client_tag) {
  uicc_connector_resp_t *resp = (uicc_connector_resp_t *) data;
  sim_uicc_status_code_t uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;

  if (resp == 0) {
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_connector_resp_cb: NULL response");
    return;
  }

  convert_mal_status_code(mal_error,
                          resp->status,
                          resp->details,
                          &uicc_status_code,
                          &uicc_status_code_fail_details);

  switch (resp->service_type) {
  case MAL_UICC_CONNECT:
    // card is connected, reset states to default values

    init_apps_table();

    pthread_mutex_lock(&card.mtx);

    pin_lock.state = PIN_STATE_UNKNOWN;
    pin_lock.pin_id = 0;
    pin_lock.app_id = 0;

    card.number_of_apps = 0;   // Actual number of apps found
    card.i = MAX_APPS;  // Index to element that is active.
    card.type = MAL_UICC_CARD_TYPE_UNKNOWN;
    card.cat_ind_type = MAL_UICC_CARD_TYPE_UNKNOWN;
    card.status = MAL_UICC_STATUS_UNKNOWN;
    card.got_card_status = 0;
    {
        int i;
        for ( i = 0 ; i < MAX_CHANNELS ; i++ ) {
            card.channels[i].channel_id = CHANNEL_ID_FREE;
            card.channels[i].aid_len    = 0;
            memset( card.channels[i].aid, 0, sizeof(card.channels[i].aid) );
        }
    }
    pthread_mutex_unlock(&card.mtx);

    uiccd_sig_sim_connect_response((uintptr_t) client_tag, uicc_status_code, uicc_status_code_fail_details);
    break;
  case MAL_UICC_DISCONNECT:
    uiccd_sig_sim_disconnect_response((uintptr_t) client_tag, uicc_status_code, uicc_status_code_fail_details);
    break;
  case MAL_UICC_RECONNECT:
  default:
    catd_log_f(SIM_LOGGING_E, "handle_mal_uicc_connector_resp_cb: Unsupported service_type=0x%x",
               resp->service_type);
  }
}

// -----------------------------------------------------------------------------
// UICC Mal callback.
extern void sim_maluicc_indication_response_trace(int32_t message_id, void* data, int mal_error, void* client_tag);
static void ste_catd_mtbl_mal_callback(int32_t eventID, void *data, mal_error_type mal_error, void *client_tag)
{
  if (!data) {
    catd_log_f(SIM_LOGGING_E, "%s: null data received, eventID 0x%x", __PRETTY_FUNCTION__, eventID);
  }

  sim_maluicc_indication_response_trace((int)eventID, data, mal_error, client_tag);

  switch (eventID) {
  case MAL_UICC_IND:
    // handle_mal_uicc_ind_cb to be rewritten so that this lock is no longer needed.
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_ind_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_RESP:
    handle_mal_uicc_resp_cb(eventID, data, mal_error, client_tag);
    break;
  case MAL_UICC_CARD_READER_IND:
    pthread_mutex_lock(&card.mtx);
    handle_mal_uicc_card_reader_ind_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&card.mtx);
    break;
  case MAL_UICC_CARD_IND:
    pthread_mutex_lock(&card.mtx);
    handle_mal_uicc_card_ind_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&card.mtx);
    break;
  case MAL_UICC_CARD_RESP:
    pthread_mutex_lock(&card.mtx);
    handle_mal_uicc_card_resp_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&card.mtx);
    break;
  case MAL_UICC_APPLICATION_RESP:
    pthread_mutex_lock(&card.mtx);
    handle_mal_uicc_application_resp_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&card.mtx);
    break;
  case MAL_UICC_APPLICATION_IND:
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_application_ind_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_APPL_CMD_RESP:
    handle_mal_uicc_appl_cmd_resp_cb(eventID, data, mal_error, client_tag);
    break;
  case MAL_UICC_PIN_IND:
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_pin_ind_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_PIN_RESP:
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_pin_resp_cb(eventID, data, mal_error, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_CAT_IND:
    handle_mal_uicc_cat_ind_cb(eventID, data, client_tag);
    break;
  case MAL_UICC_APPL_CMD_IND:
    catd_log_f(SIM_LOGGING_I,"MAL_UICC_APPL_CMD_IND unimplemented!");
    break;
  case MAL_UICC_CAT_RESP:
    handle_mal_uicc_cat_resp_cb(eventID, data, client_tag);
    break;
  case MAL_UICC_REFRESH_IND:
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_refresh_ind_cb(eventID, data, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_REFRESH_RESP:
    pthread_mutex_lock(&apps[card.i].mtx);
    handle_mal_uicc_refresh_resp_cb(eventID, data, client_tag);
    pthread_mutex_unlock(&apps[card.i].mtx);
    break;
  case MAL_UICC_CONNECTOR_RESP:
    handle_mal_uicc_connector_resp_cb(eventID, data, mal_error, client_tag);
    break;
  case MAL_UICC_REQ:
  case MAL_UICC_APPLICATION_REQ:
  case MAL_UICC_CARD_REQ:
  case MAL_UICC_PIN_REQ:
  case MAL_UICC_APPL_CMD_REQ:
  case MAL_UICC_CAT_REQ:
  case MAL_UICC_REFRESH_REQ:
      catd_log_f(SIM_LOGGING_I, "sim_mal_modem.c:ste_catd_mtbl_mal_callback: REQ as RESP/IND=0x%x", eventID);
    break;
  case MAL_UICC_APDU_RESP:
    handle_mal_uicc_apdu_resp_cb(eventID, data, mal_error, client_tag);
    break;
  case MAL_UICC_APDU_RESET_IND:
    handle_mal_uicc_apdu_reset_ind_cb(eventID, data, mal_error, client_tag);
    break;
  default:
      catd_log_f(SIM_LOGGING_E, "sim_mal_modem.c:ste_catd_mtbl_mal_callback: unknown event=0x%x", eventID);
    }
}
// -----------------------------------------------------------------------------
// Event Stream callback

static                  ssize_t
ste_netlnk_parser_func(char *UNUSED(buf), char *UNUSED(buf_max), void *ud)
{
    ste_modem_t            *m;
    int netlnk_msg = -1;
    m = (ste_modem_t *) ud;
    static_data = m;
    catd_log_f(SIM_LOGGING_D, "sim_mal_modem : Enter ste_netlnk_parser_func");
    // handler does not return a status code anymore */
    netlnk_socket_recv(m->shmnetlnk_fd, &netlnk_msg);
    if (netlnk_msg == MODEM_RESET_IND) {
        catd_log_f(SIM_LOGGING_D, "sim_mal_modem : Received modem silent reset indication!");
        m->modem_reset_status = 1;
        return 2;
    }
    catd_log_f(SIM_LOGGING_D, "sim_mal_modem : Exit ste_netlnk_parser_func");
    return 1;
}
// -----------------------------------------------------------------------------
// Event Stream callback

static ssize_t ste_catd_mtbl_es_parser_func(char *buf, char *UNUSED(buf_max), void *ud)
{
    /*
     * Always null for non-buf ES!
     * If this ever changes, then this code structure must be modified to work as a buffered ES.
     */
    assert(!buf_max);

    /*
     * For non-buf ES, buf points to an int (the socket associated with this ES).
     *
     * However, when the ES is shut down, this parser function will be called with the pointer
     * set to NULL to indicate shutdown (at which point we do nothing here).
     */
    if (buf) { // Normal operation
        static_data = (ste_modem_t *)ud; // User data was set up to point at a modem structure when ES was created
        mal_uicc_response_handler();
    }

    return 1;
}

// -----------------------------------------------------------------------------
// Actions

static int ste_mtbl_i_fini_th(ste_modem_t * m, void *UNUSED(client_tag))
{
    mal_uicc_deinit();
    catd_rem_es(m->mal_fd);
    m->mal_fd = -1;
    m->shmnetlnk_fd = -1;
    m->modem_reset_status = -1;
    return 0;
}


static int ste_mtbl_i_enable_th(ste_modem_t *UNUSED(m),
                                uintptr_t client_tag)
{
    uicc_cat_req_t req;
    int i;
    struct ste_cmd_tag_s *my_ct = NULL;

    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }

    my_ct->tag = STE_CMD_MODEM_ENABLE_TAG;
    my_ct->ct = (void*)client_tag;

    memset(&req, 0, sizeof(req));
    req.service_type = MAL_UICC_CAT_ENABLE;

    i = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if (i != 0) {
        free(my_ct);
        return -1;
    }
    return i;
}

static int ste_mtbl_i_disable_th(ste_modem_t *UNUSED(m),
                                 uintptr_t client_tag)
{
    uicc_cat_req_t req;
    int i;
    struct ste_cmd_tag_s *my_ct = NULL;

    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }

    my_ct->tag = STE_CMD_MODEM_DISABLE_TAG;
    my_ct->ct = (void*)client_tag;

    memset(&req, 0, sizeof(req));
    req.service_type = MAL_UICC_CAT_DISABLE;

    i = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if (i != 0) {
        free(my_ct);
        return -1;
    }
    return i;
}

static int ste_mtbl_i_ec_th(ste_modem_t *UNUSED(m), uintptr_t client_tag, ste_apdu_t *apdu)
{
    uicc_cat_req_t req;
    int i;
    struct ste_cmd_tag_s *my_ct = NULL;

    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }

    my_ct->tag = STE_CMD_ENVELOPE_COMMAND_TAG;
    my_ct->ct = (void*)client_tag;

    memset(&req, 0, sizeof(req));
    req.service_type = MAL_UICC_CAT_ENVELOPE;
    req.sub_block_t.uicc_cat_envelope.uicc_sb_envelope.envelope_length = (uint16_t)ste_apdu_get_raw_length(apdu);
    req.sub_block_t.uicc_cat_envelope.uicc_sb_envelope.envelope = (uint8_t *)ste_apdu_get_raw(apdu);

    i = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if (i != 0) {
        free(my_ct);
        return -1;
    }
    return i;
}

static int ste_mtbl_i_set_poll_intervall_th(ste_modem_t* UNUSED(m), void* client_tag, int interval)
{
    poll_interval_tag_t* my_ct;
    uicc_cat_req_t req;
    int rc;

    // Input parameter in tenths of seconds, modem uses seconds.
    interval = (interval + 5) / 10; // convert to seconds with rounding...
    if (interval < 0 || interval > (int)UINT16_MAX) {
        return -1;
    }

    // We need to store the actual poll interval we set since we need to send it back.
    // Since we use the client_tag to store this we also need to store the original client tag.
    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }
    my_ct->interval   = (uint16_t)interval; // Safe to cast since we checked range above
    my_ct->client_tag = client_tag;         // Store callers client tag


    req.service_type = MAL_UICC_CAT_POLLING_SET;
    req.sub_block_t.uicc_cat_polling_set.uicc_sb_polling_set.poll_int = my_ct->interval;

    rc = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if(rc) {
        free(my_ct);
        rc = -1;
    }

    return rc;
}

static int ste_mtbl_i_poll_req_th(ste_modem_t* UNUSED(m), void* client_tag)
{
    poll_req_tag_t* my_ct;
    uicc_cat_req_t req;
    int rc;

    // Since we use the client_tag to store this we also need to store the original client tag.
    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }
    my_ct->client_tag = client_tag;         // Store callers client tag


    req.service_type = MAL_UICC_CAT_POLL;

    rc = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if(rc) {
        free(my_ct);
        rc = -1;
    }

    return rc;
}



static int ste_mtbl_i_tr_th(ste_modem_t *UNUSED(m), uintptr_t client_tag, ste_apdu_t *apdu)
{
    uicc_cat_req_t req;
    int i;
    struct ste_cmd_tag_s *my_ct = NULL;

    my_ct = malloc(sizeof(*my_ct));
    if (!my_ct) {
        return -1;
    }

    my_ct->tag = STE_CMD_TERMINAL_RESPONSE_TAG;
    my_ct->ct = (void*)client_tag;

    memset(&req, 0, sizeof(req));
    req.service_type = MAL_UICC_CAT_TERMINAL_RESPONSE;
    req.sub_block_t.uicc_cat_terminal_response.uicc_sb_terminal_response.tr_length = (uint16_t)ste_apdu_get_raw_length(apdu);
    req.sub_block_t.uicc_cat_terminal_response.uicc_sb_terminal_response.tr = (uint8_t *)ste_apdu_get_raw(apdu);

    i = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
    if (i != 0) {
        free(my_ct);
        return -1;
    }
    return i;
}

static int ste_mtbl_i_pin_verify_th(ste_modem_t * UNUSED(m),
                                    void *client_tag,
                                    uint8_t UNUSED(app_id),
                                    sim_uicc_pin_id_t pin_id,
                                    const char* pin, size_t len)
{
  int i = 0;
  uicc_pin_req_t req;
  struct ste_cmd_tag_s *my_ct = NULL;

  my_ct = malloc(sizeof(*my_ct));
  if (!my_ct) {
      return -1;
  }

  my_ct->tag = pin_id;
  my_ct->ct = (void*)client_tag;

  catd_log_f(SIM_LOGGING_I, "ste_mtbl_i_pin_verify: apps[card.i].app_id=%d apps[card.i].pin_id=%d pin_lock.app_id=%d pin_lock.pin_id=%d", apps[card.i].app_id, apps[card.i].pin_id, pin_lock.app_id, pin_lock.pin_id);

  if (pin_id == SIM_UICC_PIN_ID_UNKNOWN) {
    if (pin_lock.state == PIN_STATE_PINLOCKED) {
      // We're in the context of pin verify
      req.appl_id = pin_lock.app_id;
      req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.pin_id = pin_lock.pin_id;
    } else {
      catd_log_f(SIM_LOGGING_E, "Invalid PIN ID provided for the current state!");
      i = -1;
    }
  } else {
    catd_log_f(SIM_LOGGING_I, "NOTE: Out of context PIN verify");

    // Out of context; so verify against application PIN
    req.appl_id = apps[card.i].app_id;
    if ( pin_id == SIM_UICC_PIN_ID_PIN1 ) {
        req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin_id;
    } else {
        req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin2_id;
    }
  }

  if (!i) {
    req.service_type = MAL_UICC_PIN_VERIFY;
    req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.length = (uint8_t)len;
    req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.pin_qualifier = 0x00;

    strncpy(req.sub_block_t.uicc_pin_verify_req.pin_uicc_sb_pin.pin_code,
            pin, 8);
    i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, (void *)my_ct);
  } else {
    free(my_ct);
  }

  return i;
}

static int ste_mtbl_i_pin_disable_th(ste_modem_t * UNUSED(m),
                                     void *client_tag,
                                      uint8_t UNUSED(app_id), uint8_t UNUSED(pin_id),
                                     const char* pin, size_t len)
{
  int i;
  uicc_pin_req_t req;

  req.service_type = MAL_UICC_PIN_DISABLE;
  req.appl_id = apps[card.i].app_id;
  req.sub_block_t.uicc_pin_disable_req.pin_uicc_sb_pin.length = (uint8_t)len;
  req.sub_block_t.uicc_pin_disable_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin_id;
  req.sub_block_t.uicc_pin_disable_req.pin_uicc_sb_pin.pin_qualifier = 0x00;
  strncpy(req.sub_block_t.uicc_pin_disable_req.pin_uicc_sb_pin.pin_code,
          pin, 8);
  i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_pin_enable_th(ste_modem_t * UNUSED(m),
                                    void *client_tag,
                                    uint8_t UNUSED(app_id), uint8_t UNUSED(pin_id),
                                    const char* pin, size_t len)
{
  int i;
  uicc_pin_req_t req;

  req.service_type = MAL_UICC_PIN_ENABLE;
  req.appl_id = apps[card.i].app_id;
  req.sub_block_t.uicc_pin_enable_req.pin_uicc_sb_pin.length = (uint8_t)len;
  req.sub_block_t.uicc_pin_enable_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin_id;
  req.sub_block_t.uicc_pin_enable_req.pin_uicc_sb_pin.pin_qualifier = 0x00;
  strncpy(req.sub_block_t.uicc_pin_enable_req.pin_uicc_sb_pin.pin_code,
          pin, 8);
  i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_pin_info_th(ste_modem_t * UNUSED(m),
                                  void *client_tag,
                                  uint8_t UNUSED(app_id), uint8_t pin_id)
{
  int i;
  uicc_pin_req_t req;

  req.service_type = MAL_UICC_PIN_INFO;
  req.appl_id = apps[card.i].app_id;
  req.sub_block_t.uicc_pin_info_req.pin_uicc_sb_ref.pin_id = pin_id;

  catd_log_f(SIM_LOGGING_D, "ste_mtbl_i_pin_info_th: appid=%d, pinid=%d\n", apps[card.i].app_id, pin_id);
  i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, client_tag);
  return i;
}



static int ste_mtbl_i_pin_change_th(ste_modem_t * UNUSED(m),
                                    void *client_tag,
                                    int UNUSED(app_id),
                                    sim_uicc_pin_id_t pin_id,
                                    const char *old_pin,
                                    unsigned old_pin_len,
                                    const char *new_pin,
                                    unsigned new_pin_len) {

  int i;
  int mal_pin_id;
  uicc_pin_req_t req;
  struct ste_cmd_tag_s *my_ct = NULL;

  my_ct = malloc(sizeof(*my_ct));
  if (!my_ct) {
      return -1;
  }

  my_ct->tag = pin_id;
  my_ct->ct = (void*)client_tag;

  req.service_type = MAL_UICC_PIN_CHANGE;
  req.appl_id = apps[card.i].app_id;

  mal_pin_id = get_active_pin_id(pin_id);
  if (mal_pin_id == -1)
    return -1;

  // Prepare for old pin
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_old.length = old_pin_len;
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_old.pin_id = mal_pin_id;
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_old.pin_qualifier= MAL_UICC_PIN_OLD;
  strncpy(req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_old.pin_code,
          old_pin, 8);

  // Prepare for new pin
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_new.length = new_pin_len;
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_new.pin_id = mal_pin_id;
  req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_new.pin_qualifier= MAL_UICC_PIN_NEW;
  strncpy(req.sub_block_t.uicc_pin_change_req.pin_uicc_sb_pin_new.pin_code,
          new_pin, 8);

  i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, (void *) my_ct);
  return i;
}


static int ste_mtbl_i_pin_unblock_th(ste_modem_t * UNUSED(m),
                                    void *client_tag,
                                    uint8_t UNUSED(app_id),
                                    sim_uicc_pin_id_t pin_id,
                                    const char *pin,
                                    unsigned pin_len,
                                    const char *puk,
                                    unsigned puk_len)
{
  int i = 0;
  uicc_pin_req_t req;

  if (pin_id == SIM_UICC_PIN_ID_UNKNOWN) {
    if (pin_lock.state == PIN_STATE_PUKLOCKED) {
      // Verify in context of verify needed
      req.appl_id = pin_lock.app_id;
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.pin_id = pin_lock.pin_id;
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_puk.pin_id = pin_lock.pin_id;
    }  else {
      catd_log_f(SIM_LOGGING_E, "Incorrect PIN ID for PIN unblock!");
      i = -1;
    }
  } else {
    // Verify out of context
    req.appl_id = apps[card.i].app_id;

    if (pin_id == SIM_UICC_PIN_ID_PIN1) {
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin_id;
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_puk.pin_id = apps[card.i].pin_id;
    } else {
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.pin_id = apps[card.i].pin2_id;
      req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_puk.pin_id = apps[card.i].pin2_id;
    }
  }

  if (!i) {
    /* For SB UICC_SB_PIN */
    req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.length = (uint8_t)pin_len;
    req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.pin_qualifier = 0x00;
    strncpy(req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_pin.pin_code,
            pin, 8);

    /* For SB UICC_SB_PUK */
    req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_puk.puk_length =  (uint8_t)puk_len;
    strncpy(req.sub_block_t.uicc_pin_unblock_req.pin_uicc_sb_puk.puk_code,
            puk, 8);

    catd_log_f(SIM_LOGGING_I, "PUK_UNLOCK: app_id=%d pin_id=%d pin len=%d, pin=%s, puk len=%d puk=%s\n", apps[card.i].app_id, apps[card.i].pin_id, pin_len, pin, puk_len, puk);
    req.service_type = MAL_UICC_PIN_UNBLOCK;
    i = mal_uicc_request_wrapper(MAL_UICC_PIN_REQ, &req, client_tag);
  }

  return i;
}



static int ste_mtbl_i_sim_file_read_record_th(ste_modem_t * UNUSED(m),
                                              void *client_tag,
                                              uint8_t UNUSED(app_id),
                                              uint16_t file_id,
                                              uint8_t *file_path,
                                              uint8_t file_path_len,
                                              uint8_t rec_id,
                                              uint8_t length)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;

  req.service_type = MAL_UICC_APPL_READ_LINEAR_FIXED;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;
  req.sub_block_t.sb_linearfixed.ef = file_id;
  req.sub_block_t.sb_linearfixed.record = rec_id;
  req.sub_block_t.sb_linearfixed.offset = 0;
  req.sub_block_t.sb_linearfixed.amount = length;
  req.sub_block_t.sb_linearfixed.sfi = 0;
  req.sub_block_t.sb_linearfixed.path = file_path;
  req.sub_block_t.sb_linearfixed.path_length = file_path_len;
  req.sub_block_t.sb_linearfixed.sb_resp_info.response_info = MAL_UICC_RESP_INFO_STATUS_WORD;
  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_sim_file_read_binary_th(ste_modem_t * UNUSED(m),
                                              void *client_tag,
                                              uint8_t UNUSED(app_id),
                                              uint16_t file_id,
                                              uint8_t *file_path,
                                              uint8_t file_path_len,
                                              uint16_t offset,
                                              uint16_t length)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;
  req.service_type = MAL_UICC_APPL_READ_TRANSPARENT;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;
  req.sub_block_t.ru_transp.ef = file_id;
  req.sub_block_t.ru_transp.offset = offset;
  req.sub_block_t.ru_transp.amount = length;
  req.sub_block_t.ru_transp.sfi = 0;
  req.sub_block_t.ru_transp.path_length = file_path_len;
  req.sub_block_t.ru_transp.path = file_path;
  req.sub_block_t.ru_transp.sb_resp_info.response_info = MAL_UICC_RESP_INFO_STATUS_WORD;
  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_sim_file_update_record_th(ste_modem_t * UNUSED(m),
                                                void *client_tag,
                                                uint8_t UNUSED(app_id),
                                                uint16_t file_id,
                                                uint8_t *file_path,
                                                uint8_t file_path_len,
                                                uint8_t rec_id,
                                                uint16_t length,
                                                uint8_t *data)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;
  req.service_type = MAL_UICC_APPL_UPDATE_LINEAR_FIXED;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;
  req.sub_block_t.update_linearfixed.read_linearfixed.ef = file_id;
  req.sub_block_t.update_linearfixed.read_linearfixed.record = rec_id;
  req.sub_block_t.update_linearfixed.read_linearfixed.offset = 0;
  req.sub_block_t.update_linearfixed.read_linearfixed.amount = 0;
  req.sub_block_t.update_linearfixed.read_linearfixed.sfi = 0;
  req.sub_block_t.update_linearfixed.read_linearfixed.path = file_path;
  req.sub_block_t.update_linearfixed.read_linearfixed.path_length = file_path_len;
  req.sub_block_t.update_linearfixed.data.data_length = length;
  req.sub_block_t.update_linearfixed.data.data = data;
  req.sub_block_t.update_linearfixed.sb_resp_info.response_info = MAL_UICC_RESP_INFO_STATUS_WORD;
#ifdef DONT_WRITE_TO_SIM
  catd_log_f(SIM_LOGGING_I, "Writing to SIM disabled, returning success response code\n");
  i = 0; // Fake successful write
#else
  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, client_tag);
#endif
  return i;
}

static int ste_mtbl_i_sim_file_update_binary_th(ste_modem_t * UNUSED(m),
                                                void *client_tag,
                                                uint8_t UNUSED(app_id),
                                                uint16_t file_id,
                                                uint8_t *file_path,
                                                uint8_t file_path_len,
                                                uint16_t offset,
                                                uint16_t length,
                                                uint8_t *data)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;
  req.service_type = MAL_UICC_APPL_UPDATE_TRANSPARENT;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;
  req.sub_block_t.update_transp.read_transp.ef = file_id;
  req.sub_block_t.update_transp.read_transp.offset = offset;
  req.sub_block_t.update_transp.read_transp.amount = 0; // Not used
  req.sub_block_t.update_transp.read_transp.sfi = 0;
  req.sub_block_t.update_transp.read_transp.path_length = file_path_len;
  req.sub_block_t.update_transp.read_transp.path = file_path;
  req.sub_block_t.update_transp.data.data_length = length;
  req.sub_block_t.update_transp.data.data = data;
  req.sub_block_t.update_transp.sb_resp_info.response_info = MAL_UICC_RESP_INFO_STATUS_WORD;
#ifdef DONT_WRITE_TO_SIM
  catd_log_f(SIM_LOGGING_I, "Writing to SIM disabled, returning success response code\n");
  i = 0;
#else
  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, client_tag);
#endif
  return i;

}

static int ste_mtbl_i_get_file_information_th(ste_modem_t * UNUSED(m),
                                              void *client_tag,
                                              uint8_t UNUSED(app_id),
                                              uint16_t file_id,
                                              uint8_t *file_path,
                                              uint8_t path_len)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;

  req.service_type = MAL_UICC_APPL_FILE_INFO;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;
  req.sub_block_t.uicc_appl_file_info.ef = file_id;
  req.sub_block_t.uicc_appl_file_info.sfi = 0;
  req.sub_block_t.uicc_appl_file_info.path_length = path_len;
  req.sub_block_t.uicc_appl_file_info.path = file_path;
  req.sub_block_t.uicc_appl_file_info.sb_resp_info.response_info = MAL_UICC_RESP_INFO_STATUS_WORD;

  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, client_tag);

  if ( i != 0 ) {
      return -1;
  }

  return 0;
}

static int ste_mtbl_i_appl_apdu_send_th(ste_modem_t * UNUSED(m),
                                        void *client_tag,
                                        uint8_t UNUSED(app_id),
                                        uint8_t  cmd_force,
                                        uint16_t apdu_len,
                                        uint8_t *apdu,
                                        uint16_t file_id,
                                        uint8_t *file_path,
                                        uint8_t path_len)
{
  uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };
  int i;
  struct ste_cmd_tag_s *my_ct = NULL;

  catd_log_f(SIM_LOGGING_I, "ste_mtbl_i_appl_apdu_send_th: Enter");

  my_ct = malloc(sizeof(*my_ct));
  if (my_ct == NULL) {
      return -1;
  }

  req.service_type = MAL_UICC_APPL_APDU_SEND;
  req.appl_id = apps[card.i].app_id;
  req.session_id = MAL_UICC_SESSION_ID_NOT_USED;

  req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.cmd_force = cmd_force;
  req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.apdu_length = apdu_len;
  req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.apdu = apdu;

  req.sub_block_t.appl_apdu_send_req.uicc_sb_client.client_id = apps[card.i].client_id;

  req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.ef = file_id;
  req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.sfi = 0;
  req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path_length = path_len;
  req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path = file_path;

  my_ct->tag = STE_CMD_APPLICATION_APDU_TAG;
  my_ct->ct = (void*)client_tag;

  i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, my_ct);
  if (i != 0) {
      free(my_ct);
      return -1;
  }

  catd_log_f(SIM_LOGGING_I, "ste_mtbl_i_appl_apdu_send_th: return i = %d", i);

  return i;
}

static int ste_mtbl_i_refresh_th(ste_modem_t *UNUSED(m),
                                 uintptr_t client_tag,
                                 ste_apdu_refresh_type_t type,
                                 ste_apdu_application_id_t * app_id,
                                 ste_apdu_file_list_t * file_list)
{
    uicc_cat_req_t req;
    uint8_t        refresh_type;
    int i;
    int rv;
    struct ste_cmd_tag_s *my_ct = NULL;

    my_ct = malloc(sizeof(*my_ct));
    if (my_ct == NULL) {
        return -1;
    }

    switch ( type ) {
    case STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE:
        refresh_type = MAL_UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE;
        break;
    case STE_APDU_REFRESH_FILE_CHANGE:
        refresh_type = MAL_UICC_REFRESH_NAA_FILE_CHANGE;
        break;
    case STE_APDU_REFRESH_INIT_AND_FILE_CHANGE:
        refresh_type = MAL_UICC_REFRESH_NAA_INIT_FILE_CHANGE;
        break;
    case STE_APDU_REFRESH_INIT:
        refresh_type = MAL_UICC_REFRESH_NAA_INIT;
        break;
    case STE_APDU_REFRESH_UICC_RESET:
        refresh_type = MAL_UICC_REFRESH_UICC_RESET;
        break;
    case STE_APDU_REFRESH_3G_APP_RESET:
        refresh_type = MAL_UICC_REFRESH_NAA_APPLICATION_RESET;
        break;
    case STE_APDU_REFRESH_3G_SESSION_RESET:
        refresh_type = MAL_UICC_REFRESH_NAA_SESSION_RESET;
        break;
    default:
        free(my_ct);
        return -1;
    }

    my_ct->tag = STE_CMD_REFRESH_TAG;
    my_ct->ct = (void*)client_tag;

    memset(&req, 0, sizeof(req));
    req.service_type = MAL_UICC_CAT_REFRESH;
    req.sub_block_t.uicc_cat_refresh.uicc_sb_refresh.type = type;

    //check the AID part
    if (app_id)
    {
        uint8_t * update_data;

        req.sub_block_t.uicc_cat_refresh.is_aid_present = 1;
        req.sub_block_t.uicc_cat_refresh.uicc_sb_aid.str_len = app_id->len;
        update_data = (uint8_t *)calloc(1, ((uint8_t)app_id->len) * (sizeof(uint8_t)));
        if (!update_data)
        {
            free(my_ct);
            return -1;
        }
        req.sub_block_t.uicc_cat_refresh.uicc_sb_aid.str = update_data;
        memcpy(update_data, app_id->app, app_id->len);
    }
    //check the file list part
    if (file_list)
    {
        ste_apdu_file_name_t            *files_p;
        uint8_t                         *appln_path = NULL;
        uint8_t                         *file_id_p = NULL;
        uint8_t                          path_len = 0;
        uint16_t                         ef_id = 0;

        files_p = file_list->files_p;

        req.sub_block_t.uicc_cat_refresh.nb_appl_path = file_list->nr_of_files;
        for (i = 0; i < file_list->nr_of_files; i++)
        {
            req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i] = \
                                    (uicc_sb_appl_path_t *)calloc(1, sizeof(uicc_sb_appl_path_t));
            file_id_p = files_p->file_path + files_p->path_len - 2; //point to the start of file id
            path_len = files_p->path_len - 2; //except the file id
            ef_id = *file_id_p;
            ef_id = ef_id * 256 + *(file_id_p + 1);

            req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]->ef = ef_id; //the last 2 bytes in the full file path
            req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]->sfi = 0;
            req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]->path_length = path_len;
            appln_path = (uint8_t *)calloc(1, path_len * (sizeof(uint8_t)));
            req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]->path = appln_path;
            memcpy(appln_path, files_p->file_path, path_len);

            files_p++;
        }
    }

    rv = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);

    if (req.sub_block_t.uicc_cat_refresh.is_aid_present)
    {
        free(req.sub_block_t.uicc_cat_refresh.uicc_sb_aid.str);
    }

    for (i = 0; i < req.sub_block_t.uicc_cat_refresh.nb_appl_path; i++)
    {
        free(req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]->path);
        free(req.sub_block_t.uicc_cat_refresh.uicc_sb_appl_path[i]);
    }

    if (rv != 0) {
        free(my_ct);
        return -1;
    }
    return rv;
}

static int ste_mtbl_i_tpdl_th(ste_modem_t * UNUSED(m), void *client_tag, uint8_t * tp, size_t tp_size)
{
  uicc_cat_req_t req;
  int i;
  struct ste_cmd_tag_s *my_ct;

  my_ct = malloc(sizeof(*my_ct));
  if (!my_ct) return -1;
  my_ct->tag = STE_CMD_TPDL_TAG;
  my_ct->ct = client_tag;

  memset(&req, 0, sizeof(req));
  req.service_type = MAL_UICC_CAT_TERMINAL_PROFILE;
  req.sub_block_t.uicc_cat_terminal_profile.uicc_sb_terminal_profile.tp_length = tp_size;
  req.sub_block_t.uicc_cat_terminal_profile.uicc_sb_terminal_profile.tp = tp;

  i = mal_uicc_request_wrapper(MAL_UICC_CAT_REQ, &req, my_ct);
  if (i != 0) {
    free(my_ct);
    return -1;
  }
  return 0;
}

static int ste_mtbl_i_application_list_th(ste_modem_t *UNUSED(m), void *client_tag) {
  int i;
  mal_uicc_appln_req_t req;

  req.service_type = MAL_UICC_APPL_LIST;
  req.activate_last_appln = 0;
  i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_modem_sim_connect_th(ste_modem_t *UNUSED(m), void *client_tag) {
  int i;
  uicc_connector_req_t req;
  req.service_type = MAL_UICC_CONNECT;
  i = mal_uicc_request_wrapper(MAL_UICC_CONNECTOR_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_modem_sim_disconnect_th(ste_modem_t *UNUSED(m), void *client_tag) {
    int i;
    uicc_connector_req_t req;
    req.service_type = MAL_UICC_DISCONNECT;
    i = mal_uicc_request_wrapper(MAL_UICC_CONNECTOR_REQ, &req, client_tag);
    return i;
}

static int ste_mtbl_i_modem_card_status_th(ste_modem_t *UNUSED(m), void *client_tag) {
  int i;

  uicc_card_req_t req;
  req.service_type = MAL_UICC_CARD_STATUS_GET;
  i = mal_uicc_request_wrapper(MAL_UICC_CARD_REQ, &req, client_tag);
  return i;
}

static int ste_mtbl_i_modem_card_info_th() {
  int i;

  uicc_card_req_t req;
  req.service_type = MAL_UICC_CARD_INFO_GET;
  i = mal_uicc_request_wrapper(MAL_UICC_CARD_REQ, &req, NULL);
  return i;
}

static int ste_mtbl_i_application_host_activate_th(ste_modem_t *UNUSED(m),
                                                   void *client_tag,
                                                   uint8_t app_type,
                                                   uint8_t app_id) {
  int i;
  struct ste_cmd_tag_s    *ct;
  mal_uicc_appln_req_t req;

  req.service_type = MAL_UICC_APPL_HOST_ACTIVATE;
  req.activate_last_appln = 0;
  req.sub_block.appln_host_activate.sb_appln.application_type = app_type;
  req.sub_block.appln_host_activate.sb_appln.appl_id = app_id;
  req.sub_block.appln_host_activate.sb_appl_info.strat_up_type = 0;
  req.sub_block.appln_host_activate.uicc_sb_aid.str_len = 0;
  req.sub_block.appln_host_activate.uicc_sb_aid.str = NULL;

  ct = malloc(sizeof *ct);
  if (!ct) return -1;
  ct->ct = (void *)client_tag;
  ct->tag = STE_CMD_APPL_ACTIVATE_STARTUP;

  i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &req, ct);
  return i;
}

static int ste_mtbl_i_application_host_deactivate_th(ste_modem_t *UNUSED(m),
                                                     void *client_tag,
                                                     uint8_t app_type,
                                                     uint8_t app_id,
                                                     uint8_t client_id)
{
    int i;
    struct ste_cmd_tag_s    *ct;
    mal_uicc_appln_req_t req;

    req.service_type = MAL_UICC_APPL_HOST_DEACTIVATE;
    req.sub_block.uicc_appl_host_deactivate.sb_appln.application_type = app_type;
    req.sub_block.uicc_appl_host_deactivate.sb_appln.appl_id = app_id;
    req.sub_block.uicc_appl_host_deactivate.uicc_sb_client.client_id = client_id;

    ct = malloc(sizeof *ct);
    if (!ct) return -1;
    ct->ct = (void *)client_tag;
    ct->tag = STE_CMD_APPL_DEACTIVATE_STARTUP;

    i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &req, ct);
    return i;
}

static int ste_mtbl_i_app_shutdown_initiate_th(ste_modem_t *UNUSED(m),
                                                     void *client_tag,
                                                     uint8_t app_type,
                                                     uint8_t app_id,
                                                     uint8_t client_id)
{
    int i;
    mal_uicc_appln_req_t req;

    req.service_type = MAL_UICC_APPL_SHUT_DOWN_INITIATED;
    req.sub_block.uicc_appl_shut_down_initiated.sb_appln.application_type = app_type;
    req.sub_block.uicc_appl_shut_down_initiated.sb_appln.appl_id = app_id;
    req.sub_block.uicc_appl_shut_down_initiated.uicc_sb_client.client_id = client_id;

    i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &req, client_tag);
    return i;
}

static int ste_mtbl_i_init_th(ste_modem_t * m, void *UNUSED(client_tag))
{
  int                     ret;
    ste_es_t               *es = 0;
    ste_es_parser_closure_t pc;

    ret = mal_uicc_init(&(m->mal_fd), &(m->shmnetlnk_fd));
    if (ret != MAL_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "sim_mal_modem : FAILED to initialise mal uicc", 0);
        return -1;
    }

    ret = mal_uicc_register_callback(ste_catd_mtbl_mal_callback);
    if (ret != MAL_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "sim_mal_modem : failed to register callback%d", ret);
        return -1;
    }
    ret = mal_uicc_config();
    if (ret != MAL_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "sim_mal_modem : failed to config uicc%d", ret);
        return -1;
    }

    pc.func = ste_catd_mtbl_es_parser_func;
    pc.user_data = m;
    es = ste_es_new_nonbuf(m->mal_fd, &pc);
    ret = catd_add_es(es);
    if (ret != 0) {
        catd_log_f(SIM_LOGGING_E, "sim_mal_modem : internal failure%d", ret);
        return -1;
    }
    pc.func = ste_netlnk_parser_func;
    pc.user_data = m;
    es = ste_es_new_nonbuf(m->shmnetlnk_fd, &pc);
    ret = catd_add_es(es);
    if (ret != 0) {
        catd_log_f(SIM_LOGGING_E, "sim_mal_modem : internal failure%d", ret);
        return -1;
    }
    catd_log_f(SIM_LOGGING_I, "sim_mal_modem : Successfully initialized mal uicc", 0);
    return 0;
}

static int get_active_pin_id(sim_uicc_pin_id_t pin_id) {
    uint8_t mal_pin_id;

    switch (pin_id)
    {
        case SIM_UICC_PIN_ID_PIN1:
            mal_pin_id = apps[card.i].pin_id;
        break;
        case SIM_UICC_PIN_ID_PIN2:
            mal_pin_id = apps[card.i].pin2_id;
        break;
        default:
            mal_pin_id = -1;
        break;
    }

    return mal_pin_id;
}

static int hex2byt(const char *hex, uint8_t *byt, uint8_t *bytlen) {
  int i,r;
  int hexlen = hex ? strlen(hex)/2 : 0;
  *bytlen = 0;
  for (i=0; i<hexlen; i++) {
    if (i >= SIM_MAX_PATH_LENGTH)
      return -1;
    r = sscanf(hex+i*2, "%2hhx", byt+i);
    if (r == 0 || r == EOF)
      return -1;
  }
  *bytlen = (uint8_t)hexlen;
  return 0;
}

/* initiate APDU CONTROL service request */
static int ste_mtbl_i_sap_session_ctrlcard_th(ste_modem_t * UNUSED(m),
                                    void *client_tag, uint8_t sbl_action)
{
    int i;
    uicc_apdu_req_t req;
    struct ste_cmd_tag_s    *ct;

    ct = malloc(sizeof *ct);
    if (!ct) return -1;
    ct->ct = (void *)client_tag;
    ct->tag = STE_CMD_APDU_SAP;

    req.service_type = MAL_UICC_APDU_CONTROL;
    req.sub_block_t.uicc_apdu_control_req.uicc_sb_apdu_actions.action = sbl_action;
    req.sub_block_t.uicc_apdu_control_req.uicc_sb_apdu_actions.protocol = MAL_UICC_PROTOCOL_T0; // SAP requirement
    i = mal_uicc_request(MAL_UICC_APDU_REQ, &req, ct);
    return i;
}

/* initiate APDU ATR_GET service request */
static int ste_mtbl_i_sap_session_atr_get_th(ste_modem_t * UNUSED(m),
                                    void *client_tag)
{
    int i;
    uicc_apdu_req_t req;
    struct ste_cmd_tag_s    *ct;

    ct = malloc(sizeof *ct);
    if (!ct) return -1;
    ct->ct = (void *)client_tag;
    ct->tag = STE_CMD_APDU_SAP;

    req.service_type = MAL_UICC_ATR_GET;
    i = mal_uicc_request(MAL_UICC_APDU_REQ, &req, ct);
    return i;
}

/* initiate APDU SEND service request */
static int ste_mtbl_i_sap_session_apdu_data_th(ste_modem_t * UNUSED(m),
                                    void *client_tag, uint8_t *data, int length)
{
    int i;
    uicc_apdu_req_t req;
    struct ste_cmd_tag_s    *ct;

    ct = malloc(sizeof *ct);
    if (!ct) return -1;
    ct->ct = (void *)client_tag;
    ct->tag = STE_CMD_APDU_SAP;

    req.service_type = MAL_UICC_APDU_SEND;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = length;
    req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = data;
    i = mal_uicc_request(MAL_UICC_APDU_REQ, &req, ct);
    return i;
}

// -----------------------------------------------------------------------------
// Exposed interface. Types used in this interface are modem generic, and
// converted to modem-specific in the corresponding ste_mtbl_i* fns.

ste_modem_t            *ste_catd_modem_new()
{
    ste_modem_t            *m = calloc(1, sizeof(*m));
    if (m) {
        m->mal_fd = -1;
        m->shmnetlnk_fd = -1;
        m->modem_reset_status = -1;
    }
    return m;
}


void ste_catd_modem_delete(ste_modem_t * m, uintptr_t client_tag)
{
    if (m->mal_fd != -1)
        ste_catd_modem_disconnect(m, client_tag);
    free(m);
}



int ste_catd_modem_connect(ste_modem_t * m, uintptr_t client_tag)
{
    int                     i;
    if (m->mal_fd != -1)
        return -1;
    i = ste_mtbl_i_init_th(m, (void *)client_tag);
    if (i != 0) return -1;
    return 0;
}



int ste_catd_modem_disconnect(ste_modem_t * m, uintptr_t client_tag)
{
    int                     i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_fini_th(m, (void *)client_tag);
    if (i != 0) return -1;
    return 0;
}





int ste_catd_modem_ec(ste_modem_t * m, uintptr_t client_tag, ste_apdu_t * apdu)
{
    int                     i;
    if (m->mal_fd == -1) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_ec fails due to m->mal_fd = -1.");
        return -1;
    }
    i = ste_mtbl_i_ec_th(m, client_tag, apdu);
    if (i != 0) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_ec fails since ste_mtbl_i_ec_th fails..");
        return -1;
    }
    return 0;
}

int ste_catd_modem_enable(ste_modem_t * m, uintptr_t client_tag)
{
    int                     i;
    if (m->mal_fd == -1) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_enable fails due to m->mal_fd = -1.");
        return -1;
    }
    i = ste_mtbl_i_enable_th(m, client_tag);
    if (i != 0) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_enable fails since ste_mtbl_i_enable_th fails..");
        return -1;
    }
    return 0;
}

int ste_catd_modem_disable(ste_modem_t * m, uintptr_t client_tag)
{
    int                     i;
    if (m->mal_fd == -1) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_disable fails due to m->mal_fd = -1.");
        return -1;
    }
    i = ste_mtbl_i_disable_th(m, client_tag);
    if (i != 0) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_disable fails since ste_mtbl_i_disable_th fails..");
        return -1;
    }
    return 0;
}



int ste_catd_modem_tpdl(ste_modem_t * m, uintptr_t client_tag, uint8_t * tp, size_t tp_size)
{
    int                     i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_tpdl_th(m, (void *)client_tag, tp, tp_size);
    if (i != 0) return -1;
    return 0;
}




int ste_catd_modem_tr(ste_modem_t * m, uintptr_t client_tag, ste_apdu_t * apdu)
{
    int                     i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_tr_th(m, client_tag, apdu);
    if (i != 0) return -1;
    return 0;
}

int ste_catd_modem_set_poll_intervall(ste_modem_t * m, uintptr_t client_tag, int interval)
{
    int rc = -1;
    if (m->mal_fd != -1) {
        rc = ste_mtbl_i_set_poll_intervall_th(m, (void*)client_tag, interval);
    }
    return rc;
}

int ste_catd_modem_poll_req(ste_modem_t * m, uintptr_t client_tag)
{
    int rc = -1;
    if (m->mal_fd != -1) {
        rc = ste_mtbl_i_poll_req_th(m, (void*)client_tag);
    }
    return rc;
}

int ste_catd_modem_refresh_request(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   ste_apdu_refresh_type_t type,
                                   ste_apdu_application_id_t * app_id,
                                   ste_apdu_file_list_t * file_list)
{
    int                     i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_refresh_th(m, client_tag, type, app_id, file_list);
    if (i != 0) return -1;
    return 0;
}

int ste_catd_modem_mirror(ste_modem_t * UNUSED(m), uintptr_t UNUSED(client_tag),
        int UNUSED(fd), ste_apdu_t *UNUSED(apdu))
{
    return 0;
}


// Modified for MAL API. App_id and pin_id from, for example, a pin ind are
// supposed to be given back for verification.
int ste_modem_pin_verify(ste_modem_t * m, uintptr_t client_tag, int app_id, sim_uicc_pin_id_t pin_id,
                         const char* pin, unsigned len)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_pin_verify_th(m, (void *)client_tag, app_id, pin_id, pin, len);
    if (i == 0)
      return 0;
    return -1;
}


int ste_modem_pin_disable(ste_modem_t * m, uintptr_t client_tag, int app_id, int pin_id,
                         const char* pin, unsigned len)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_pin_disable_th(m, (void *)client_tag, app_id, pin_id, pin, len);

    if (i != 0) return -1;
    return 0;
}


int ste_modem_pin_enable(ste_modem_t * m, uintptr_t client_tag, int app_id, int pin_id,
                         const char* pin, unsigned len)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_pin_enable_th(m, (void *)client_tag, app_id, pin_id, pin, len);
    if (i != 0) return -1;
    return 0;
}


int ste_modem_pin_info(ste_modem_t * m, uintptr_t client_tag, int app_id, sim_uicc_pin_puk_id_t pin)
{
    int i;
    uint8_t pin_id;
    struct ste_cmd_tag_s *ct;

    catd_log_f(SIM_LOGGING_D, "ste_modem_pin_info: pin=%d\n", pin);
    if (m->mal_fd == -1)
      return -1;
    ct = malloc(sizeof *ct);
    if (!ct)
      return -1;
    if (pin == SIM_UICC_PIN_PIN1 || pin == SIM_UICC_PIN_PUK1)
      pin_id = apps[card.i].pin_id;
    else
      pin_id = apps[card.i].pin2_id;
    ct->ct=(void *)client_tag;
    if (pin == SIM_UICC_PIN_PIN1 || pin == SIM_UICC_PIN_PIN2)
      ct->tag=STE_CMD_PIN_INFO_PIN_ONLY;
    else
      ct->tag=STE_CMD_PIN_INFO_PUK_ONLY;
    i = ste_mtbl_i_pin_info_th(m, (void *)ct, app_id, pin_id);
    if (i != 0) {
      free(ct);
      return -1;
    }
    return 0;
}



int ste_modem_pin_change(ste_modem_t * m,
                         uintptr_t client_tag,
                         int app_id,
                         sim_uicc_pin_id_t pin_id,
                         const char *old_pin,
                         unsigned old_pin_len,
                         const char *new_pin,
                         unsigned new_pin_len)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_pin_change_th(m, (void *)client_tag, app_id, pin_id,
                                 old_pin, old_pin_len,
                                 new_pin, new_pin_len);
    if (i != 0) return -1;
    return 0;
}


int ste_modem_pin_unblock(ste_modem_t * m,
                          uintptr_t client_tag,
                          int app_id,
                          sim_uicc_pin_id_t pin_id,
                          const char *pin, unsigned pin_len,
                          const char *puk, unsigned puk_len)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_pin_unblock_th(m, (void *)client_tag, app_id, pin_id, pin, pin_len, puk, puk_len);
    if (i != 0) return -1;
    return 0;
}

typedef struct {
  uint16_t user_file_id;
  const char *res;
} file_path_user_data_t;

/*
 * This fn finds the path to a file id. Once found it signals stop, and then
 * stores file path into user data
 */
static int file_path_fn(void *user_data, uint16_t file_id,
                        const char* UNUSED(file_name), const char* file_path) {
  if (((file_path_user_data_t *)user_data)->user_file_id != file_id)
    return 0;
  ((file_path_user_data_t *)user_data)->res = file_path;
  return 1;
}

int ste_modem_file_read_record(ste_modem_t * m,
                               uintptr_t client_tag,
                               int app_id,
                               int file_id,
                               int rec_id,
                               int length,
                               const char *file_path)
{
  int i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;

  catd_log_f(SIM_LOGGING_I, "ste_modem_file_read_record: Enter");
  if (m->mal_fd == -1) return -1;
    if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;
    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type))
      file_path = data.res;
    else
      return -1;
  }
  if (file_path)
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, file path=%s", file_id, file_path);
  else
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, no file path", file_id);
  i = hex2byt(file_path, mal_file_path, &mal_file_path_len);
  if (i != 0) return -1;
  i = ste_mtbl_i_sim_file_read_record_th(m, (void *)client_tag,
                                         (uint8_t)app_id,
                                         (uint16_t)file_id,
                                         mal_file_path,
                                         mal_file_path_len,
                                         (uint8_t)rec_id,
                                         (uint8_t)length);
  if (i != 0) {
    catd_log_f(SIM_LOGGING_E, "ste_modem_file_read_record: Exit failed");
    return -1;
  }
    catd_log_f(SIM_LOGGING_D, "ste_modem_file_read_record: Exit success");
  return 0;
}

int ste_modem_file_read_binary(ste_modem_t * m,
                               uintptr_t client_tag,
                               int app_id,
                               int file_id,
                               int offset,
                               int length,
                               const char *file_path)
{
  int i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;

  catd_log_f(SIM_LOGGING_I, "ste_modem_file_read_binary: Enter");
  if (m->mal_fd == -1) return -1;
  if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;
    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type))
      file_path = data.res;
    else
      return -1;
  }
  if (file_path)
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, file path=%s", file_id, file_path);
  else
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, no file path", file_id);
  i = hex2byt(file_path, mal_file_path, &mal_file_path_len);
  if (i != 0) return -1;
  i = ste_mtbl_i_sim_file_read_binary_th(m, (void *)client_tag,
                                         (uint8_t)app_id,
                                         (uint16_t)file_id,
                                         mal_file_path,
                                         mal_file_path_len,
                                         (uint16_t)offset,
                                         (uint16_t)length);
  if (i != 0) {
    catd_log_f(SIM_LOGGING_E, "ste_modem_file_read_binary: Exit failed");
    return -1;
  }
  catd_log_f(SIM_LOGGING_D, "ste_modem_file_read_binary: Exit success");
  return 0;
}

int ste_modem_file_update_record(ste_modem_t * m, uintptr_t client_tag,
                                 int app_id, int file_id, int rec_id,
                                 int length, const char *file_path,
                                 uint8_t *data)
{
  int i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;

  catd_log_f(SIM_LOGGING_I, "ste_modem_file_update_record: Enter");
  if (m->mal_fd == -1) return -1;
  if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;
    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type))
      file_path = data.res;
    else
      return -1;
  }
  if (file_path)
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, file path=%s", file_id, file_path);
  else
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, no file path", file_id);
  i = hex2byt(file_path, mal_file_path, &mal_file_path_len);
  if (i != 0) return -1;
  i = ste_mtbl_i_sim_file_update_record_th(m, (void *)client_tag,
                                           (uint8_t)app_id,
                                           (uint16_t)file_id,
                                           mal_file_path, mal_file_path_len,
                                           (uint8_t)rec_id,
                                           (uint8_t)length,
                                           data);
  if (i != 0) {
    catd_log_f(SIM_LOGGING_E, "ste_modem_file_update_record: Exit failed");
    return -1;
  }
  catd_log_f(SIM_LOGGING_D, "ste_modem_file_update_record: Enter success");
  return 0;
}

int ste_modem_file_update_binary(ste_modem_t * m, uintptr_t client_tag,
                                 int app_id, int file_id, int offset,
                                 int length, const char *file_path,
                                 uint8_t *data)
{
  int i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;
  struct ste_cmd_tag_s *cmd_tag;

  catd_log_f(SIM_LOGGING_I, "ste_modem_file_update_binary: Enter");
  if (m->mal_fd == -1) return -1;
  if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;
    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type))
      file_path = data.res;
    else
      return -1;
  }
  if (file_path)
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, file path=%s", file_id, file_path);
  else
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, no file path", file_id);

  i = hex2byt(file_path, mal_file_path, &mal_file_path_len);
  if (i != 0) return -1;
  cmd_tag = malloc(sizeof(*cmd_tag));
  if (!cmd_tag) return -1;
  cmd_tag->tag = STE_CMD_READ_BINARY_VANILLA_TAG;
  cmd_tag->ct = (void *)client_tag;
  MAL_TRY(i = ste_mtbl_i_sim_file_update_binary_th(m, (void *)cmd_tag,
                                           (uint8_t)app_id,
                                           (uint16_t)file_id,
                                           mal_file_path, mal_file_path_len,
                                           (uint16_t)offset,
                                           (uint16_t)length,
                                           data));
  if (i != 0) {
    free(cmd_tag);
    catd_log_f(SIM_LOGGING_E, "ste_modem_file_update_binary: Exit failed");
    return -1;
  }
    catd_log_f(SIM_LOGGING_D, "ste_modem_file_update_binary: Exit success");
  return 0;
}

int ste_modem_get_file_information(ste_modem_t * m,
                                   uintptr_t client_tag,
                                   int app_id,
                                   int file_id,
                                   const char *file_path,
                                   sim_uicc_get_file_info_type_t type)
{
  int                     i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;
  struct ste_cmd_tag_s *my_ct;

  catd_log_f(SIM_LOGGING_I, "ste_modem_get_file_information: Enter");

  if (m->mal_fd == -1) {
    return -1;
  }

  if ( (!file_path || !*file_path) && file_id != 0 ) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;

    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type)) {
      file_path = data.res;
    }
  }

  if ( type == SIM_UICC_GET_FILE_INFO_TYPE_DF ) {
      file_id = MAL_UICC_EF_ID_NOT_PRESENT;
  }

  if (file_path) {
    catd_log_f(SIM_LOGGING_D, "Type=%d, File id=%4hx, file path=%s", type, file_id, file_path);
  } else {
    catd_log_f(SIM_LOGGING_D, "Type=%d, File id=%4hx, no file path", type, file_id);
  }

  if (file_path) {
      i = hex2byt(file_path, mal_file_path, &mal_file_path_len);

      if (i != 0) {
          return -1;
      }
  }

  my_ct = malloc(sizeof(*my_ct));

  if (!my_ct) {
      return -1;
  }

  my_ct->tag = STE_CMD_FILEINFO_INFO_TAG;
  my_ct->ct = (void *)client_tag;

  MAL_TRY(i = ste_mtbl_i_get_file_information_th(m,
                                                 (void *)my_ct,
                                                 (uint8_t)app_id,
                                                 (uint16_t)file_id,
                                                 file_path != NULL ? mal_file_path : NULL,
                                                 file_path != NULL ? mal_file_path_len : 0));

  if (i != 0) {
    free(my_ct);
    catd_log_f(SIM_LOGGING_E, "ste_modem_get_file_information: Exit failed");
    return -1;
  }

  catd_log_f(SIM_LOGGING_D, "ste_modem_get_file_information: Exit success");

  return 0;
}

int ste_modem_sim_file_get_format(ste_modem_t * m,
                                  uintptr_t client_tag,
                                  int app_id,
                                  int file_id,
                                  const char * file_path)
{
  int                     i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;
  struct ste_cmd_tag_s *my_ct;

  catd_log_f(SIM_LOGGING_I, "ste_modem_sim_file_get_format: Enter");

  if (m->mal_fd == -1) {
    return -1;
  }

  if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;

    catd_log_f(SIM_LOGGING_I, "Looking up path for 0x%x:", data.user_file_id);

    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type)) {
      file_path = data.res;
    }
  }

  if (file_path) {
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, file path=%s", file_id, file_path);
  } else {
    catd_log_f(SIM_LOGGING_D, "File id=%4hx, no file path", file_id);
  }

  if (file_path) {
      i = hex2byt(file_path, mal_file_path, &mal_file_path_len);

      if (i != 0) {
          return -1;
      }
  }

  my_ct = malloc(sizeof(*my_ct));

  if (!my_ct) {
      return -1;
  }

  my_ct->tag = STE_CMD_FILEINFO_FORMAT_TAG;
  my_ct->ct = (void *)client_tag;

  MAL_TRY(i = ste_mtbl_i_get_file_information_th(m, (void *)my_ct,
                                                 (uint8_t)app_id,
                                                 (uint16_t)file_id,
                                                 file_path != NULL ? mal_file_path : NULL,
                                                 file_path != NULL ? mal_file_path_len : 0));

  if (i != 0) {
    free(my_ct);
    catd_log_f(SIM_LOGGING_E, "ste_modem_sim_file_get_format: Exit failed");
    return -1;
  }

  catd_log_f(SIM_LOGGING_D, "ste_modem_sim_file_get_format: Exit success");
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
  int i;
  uint8_t mal_file_path[SIM_MAX_PATH_LENGTH];
  uint8_t mal_file_path_len;

  catd_log_f(SIM_LOGGING_I, "ste_modem_appl_apdu_send: Enter");
  if (m->mal_fd == -1) return -1;
  if (!file_path || !*file_path) {
    file_path_user_data_t data;
    data.user_file_id = (uint16_t)file_id;
    data.res = NULL;
    if (sim_file_some_path(file_path_fn, &data, apps[card.i].app_type))
      file_path = data.res;
    else
      return -1;
  }
  if (file_path)
    catd_log_f(SIM_LOGGING_I, "File id=%4hx, file path=%s", file_id, file_path);
  else
    catd_log_f(SIM_LOGGING_I, "File id=%4hx, no file path", file_id);
  i = hex2byt(file_path, mal_file_path, &mal_file_path_len);
  if (i != 0) return -1;
  i = ste_mtbl_i_appl_apdu_send_th(m, (void *)client_tag,
                                      (uint8_t)app_id,
                                      (uint8_t)cmd_force,
                                      (uint16_t)apdu_len,
                                      apdu,
                                      (uint16_t)file_id,
                                      mal_file_path,
                                      mal_file_path_len);

  if (i != 0) {
    catd_log_f(SIM_LOGGING_I, "ste_modem_appl_apdu_send: Exit failed");
    return -1;
  }
  catd_log_f(SIM_LOGGING_I, "ste_modem_appl_apdu_send: Exit success");
  return 0;
}



int ste_modem_sim_connect(ste_modem_t * m, uintptr_t client_tag) {
    int i;

    catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Enter", 0);
    if (m->mal_fd == -1) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Failed", 0);
        return -1;
    }
    i = ste_mtbl_i_modem_sim_connect_th(m, (void *)client_tag);
    if (i != MAL_SUCCESS) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Failed", 0);
      return -1;
    }
    catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Success", 0);
    return 0;
}

int ste_modem_sim_disconnect(ste_modem_t * m, uintptr_t client_tag) {
    int i;

    catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Enter", 0);
    if (m->mal_fd == -1) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Failed", 0);
        return -1;
    }
    i = ste_mtbl_i_modem_sim_disconnect_th(m, (void *)client_tag);
    if (i != MAL_SUCCESS) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Failed", 0);
      return -1;
    }
    catd_log_f(SIM_LOGGING_E, "ste_modem_sim_connect: Success", 0);
    return 0;
}

// Query modem card status
int ste_modem_card_status(ste_modem_t * m, uintptr_t client_tag) {
    int i;

    catd_log_f(SIM_LOGGING_I, "ste_modem_card_status: Enter", 0);
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_modem_card_status_th(m, (void *)client_tag);
    if (i != 0) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_card_status: Failed", 0);
      return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_modem_card_status: Success", 0);
    return 0;
}

// Retrieve application list from uicc card, and choose one to activate
int ste_modem_application_list_and_select(ste_modem_t *m, uintptr_t client_tag) {
  int i;
  struct ste_cmd_tag_s *ct;

  catd_log_f(SIM_LOGGING_I, "ste_modem_application_list: Enter");
  if (m->mal_fd == -1)
    return -1;
  ct = malloc(sizeof *ct);
  if (!ct) return -1;
  ct->ct = (void *)client_tag;
  ct->tag = STE_CMD_APPL_LIST_SELECT_TAG;
  i = ste_mtbl_i_application_list_th(m, (void *)ct);
  if (i != 0) {
    catd_log_f(SIM_LOGGING_E, "ste_modem_application_list_and_select: Failed");
    free(ct);
    return -1;
  }
  catd_log_f(SIM_LOGGING_D, "ste_modem_application_list_and_select: Success");
  return 0;
}
// Retrieve application list from uicc card; vanilla version
int ste_modem_application_list(ste_modem_t *m, uintptr_t client_tag) {
  int i;
  struct ste_cmd_tag_s *ct;

  catd_log_f(SIM_LOGGING_I, "ste_modem_application_list: Enter");
  if (m->mal_fd == -1)
    return -1;
  ct = malloc(sizeof *ct);
  if (!ct) return -1;
  ct->ct = (void *)client_tag;
  ct->tag = STE_CMD_APPL_LIST_VANILLA_TAG;
  i = ste_mtbl_i_application_list_th(m, (void *)ct);
  if (i != 0) {
    free(ct);
    catd_log_f(SIM_LOGGING_E, "ste_modem_application_list: Failed");
    return -1;
  }
  catd_log_f(SIM_LOGGING_D, "ste_modem_application_list: Success");
  return 0;
}

// After a call to application list, this function activates the selected one

int ste_modem_activate_selected_app(ste_modem_t *m, uintptr_t client_tag) {
  int i;

  catd_log_f(SIM_LOGGING_I, "ste_modem_application_select: Enter");

  if (m->mal_fd == -1 || card.i == MAX_APPS) {
    return -1;
  }

  i = ste_mtbl_i_application_host_activate_th(m, (void *)client_tag, apps[card.i].app_type, apps[card.i].app_id);

  if (i != 0) {
    catd_log_f(SIM_LOGGING_E, "ste_modem_application_host_activate: Failed");
    return -1;
  }
  catd_log_f(SIM_LOGGING_D, "ste_modem_application_host_activate: Success");
  return 0;
}

int ste_modem_deactivate_selected_app(ste_modem_t *m, uintptr_t client_tag)
{
    int i;

    catd_log_f(SIM_LOGGING_I, "ste_modem_deactivate_selected_app: Enter");

    if (m->mal_fd == -1 || card.i == MAX_APPS) {
        return -1;
    }

    i = ste_mtbl_i_application_host_deactivate_th(m, (void *)client_tag, apps[card.i].app_type, apps[card.i].app_id, apps[card.i].client_id);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "ste_mtbl_i_application_host_deactivate_th: Failed");
        return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_mtbl_i_application_host_deactivate_th: Success");

    return 0;
}

int ste_modem_app_initiate_shutdown(ste_modem_t *m, uintptr_t client_tag)
{
    int i;

    catd_log_f(SIM_LOGGING_I, "ste_modem_app_initiate_shutdown: Enter");

    if (m->mal_fd == -1 || card.i == MAX_APPS) {
        return -1;
    }

    i = ste_mtbl_i_app_shutdown_initiate_th(m, (void *)client_tag, apps[card.i].app_type, apps[card.i].app_id, apps[card.i].client_id);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "ste_mtbl_i_app_shutdown_initiate_th: Failed");
        return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_mtbl_i_app_shutdown_initiate_th: Success");

    return 0;
}


int ste_modem_sap_session_ctrl_activate_card(ste_modem_t * m, uintptr_t client_tag)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_ctrlcard_th(m, (void *)client_tag, MAL_UICC_CONTROL_CARD_ACTIVATE);
    uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_CARD_ACTIVATE;
    if (i != 0) return -1;
    return 0;
}

int ste_modem_sap_session_ctrl_deactivate_card(ste_modem_t * m, uintptr_t client_tag)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_ctrlcard_th(m, (void *)client_tag, MAL_UICC_CONTROL_CARD_DEACTIVATE);
    uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_CARD_DEACTIVATE;
    if (i != 0) return -1;
    return 0;
}

int ste_modem_sap_session_ctrl_coldreset_card(ste_modem_t * m, uintptr_t client_tag)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_ctrlcard_th(m, (void *)client_tag, MAL_UICC_CONTROL_COLD_RESET);
    uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_COLD_RESET;
    if (i != 0) return -1;
    return 0;
}

int ste_modem_sap_session_ctrl_warmreset_card(ste_modem_t * m, uintptr_t client_tag)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_ctrlcard_th(m, (void *)client_tag, MAL_UICC_CONTROL_WARM_RESET);
    uicc_ctrl_req_type = UICC_CONTROL_REQ_TYPE_WARM_RESET;
    if (i != 0) return -1;
    return 0;
}

int ste_modem_sap_session_atr_get(ste_modem_t * m, uintptr_t client_tag)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_atr_get_th(m, (void *)client_tag);
    if (i != 0) return -1;
    return 0;
}

int ste_modem_sap_session_apdu_data(ste_modem_t * m, uintptr_t client_tag, uint8_t *data, size_t length)
{
    int i;
    if (m->mal_fd == -1)
        return -1;
    i = ste_mtbl_i_sap_session_apdu_data_th(m, (void *)client_tag, data, length);
    if (i != 0) return -1;
    return 0;
}

static int ste_mal_card_status_with_mtx(ste_mal_card_status_data_t *data)
{
  data->num_apps = card.number_of_apps;
  convert_mal_card_type(card.type, &data->card_type);
  convert_mal_card_status(card.status, &data->card_status);
  return 0;
}

int ste_mal_card_status(ste_mal_card_status_data_t *data)
{
  int rv;

  pthread_mutex_lock(&card.mtx);
  rv = ste_mal_card_status_with_mtx(data);
  pthread_mutex_unlock(&card.mtx);
  return rv;
}



// This function looks into the apps table and assembles the info found there
// into the data argument
static int ste_mal_app_status_with_mtx(int app_index, ste_mal_app_status_data_t *data)
{
  if (app_index >= card.number_of_apps) {
    return -1;
  }

  convert_mal_app_type(apps[app_index].app_type, &data->app_type);
  if (apps[app_index].app_state == MAL_UICC_STATUS_APPL_NOT_ACTIVE)
    data->app_status = STE_SIM_APP_STATUS_NOT_ACTIVE;
  else if (apps[app_index].pin_ind == MAL_UICC_PIN_VERIFY_NEEDED)
    data->app_status = STE_SIM_APP_STATUS_ACTIVE_PIN;
  else if (apps[app_index].pin_ind == MAL_UICC_PIN_UNBLOCK_NEEDED)
    data->app_status = STE_SIM_APP_STATUS_ACTIVE_PUK;
  else
    data->app_status = STE_SIM_APP_STATUS_ACTIVE;
  data->pin_mode = STE_SIM_PIN_MODE_NORMAL;
  data->activated_app = (app_index == card.i ? 1 : 0);
  data->aid = apps[app_index].aid;
  data->aid_len = apps[app_index].aid_len;
  data->label = apps[app_index].label;
  data->label_len = apps[app_index].label_len; // Does NOT include term 0!
  // pin level is data got from pin_info on pin. Based on that,
  // use pin_inds, and perhaps pin resp data to determine the pin_state
  switch (apps[app_index].pin_level) {
  case PIN_DISABLED:
    data->pin_state = STE_SIM_PIN_STATUS_DISABLED;
    break;
  case PIN_ENABLED:
    if (apps[app_index].pin_ind == MAL_UICC_PIN_VERIFIED)
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
    else
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_NOT_VERIFIED;
    break;
  case PIN_ENABLED_PUK:
    if (apps[app_index].pin_ind == MAL_UICC_PIN_VERIFIED)
      // This case most probably should not happen, since pin_level should reset
      // to PIN_ENABLED whenever unblock succeeded, and hence covered on
      // previous case
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
    else
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_BLOCKED;
    break;
  case PIN_ENABLED_PERM_BLOCKED:
    // Nothing can ever be done here
    data->pin_state = STE_SIM_PIN_STATUS_ENABLED_PERM_BLOCKED;
    break;
  case PIN_UNKNOWN:
  default:
    // Try to recover from pin inds alone but no update of pin_level here!
    switch (apps[app_index].pin_ind) {
    case MAL_UICC_PIN_VERIFIED:
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
      break;
    case MAL_UICC_PIN_PERMANENTLY_BLOCKED:
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_PERM_BLOCKED;
      break;
    case MAL_UICC_PIN_UNBLOCK_NEEDED:
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_BLOCKED;
      break;
    case MAL_UICC_PIN_VERIFY_NEEDED:
      data->pin_state = STE_SIM_PIN_STATUS_ENABLED_NOT_VERIFIED;
      break;
    default:
      // If appl was activated without modem asked for pin, then this
      // means pin is disabled
      if (apps[app_index].app_state == MAL_UICC_STATUS_APPL_ACTIVE)
        data->pin_state = STE_SIM_PIN_STATUS_DISABLED;
      else
        data->pin_state = STE_SIM_PIN_STATUS_UNKNOWN;
    }
    break;
  }
  switch (apps[app_index].pin2_level) {
  case PIN_DISABLED:
    data->pin2_state = STE_SIM_PIN_STATUS_DISABLED;
    break;
  case PIN_ENABLED:
    if (apps[app_index].pin2_ind == MAL_UICC_PIN_VERIFIED)
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
    else
     data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_NOT_VERIFIED;
    break;
  case PIN_ENABLED_PUK:
    if (apps[app_index].pin2_ind == MAL_UICC_PIN_VERIFIED)
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
    else
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_BLOCKED;
    break;
  case PIN_ENABLED_PERM_BLOCKED:
    data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_PERM_BLOCKED;
    break;
  case PIN_UNKNOWN:
  default:
    switch (apps[app_index].pin2_ind) {
    case MAL_UICC_PIN_VERIFIED:
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
      break;
    case MAL_UICC_PIN_PERMANENTLY_BLOCKED:
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_PERM_BLOCKED;
      break;
    case MAL_UICC_PIN_UNBLOCK_NEEDED:
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_BLOCKED;
      break;
    case MAL_UICC_PIN_VERIFY_NEEDED:
      data->pin2_state = STE_SIM_PIN_STATUS_ENABLED_VERIFIED;
      break;
    default:
      data->pin2_state = STE_SIM_PIN_STATUS_UNKNOWN;
    }
    break;
  }
  return 0;
}

int ste_mal_app_status(int app_index, ste_mal_app_status_data_t *data) {
  int rv;

  if (app_index < 0 || app_index >= MAX_APPS)
    return -1;
  pthread_mutex_lock(&apps[app_index].mtx);
  rv = ste_mal_app_status_with_mtx(app_index, data);
  pthread_mutex_unlock(&apps[app_index].mtx);
  return rv;
}

#define MAX_PATH_LENGTH 16

static int ste_mtbl_i_sim_modem_channel_send_th(ste_modem_t *UNUSED(m),
                                                void        *client_tag,
                                                uint16_t     session_id,
                                                uint16_t     apdu_len,
                                                uint8_t     *apdu,
                                                uint8_t     *path,
                                                uint8_t      path_length)
{
    int                     i = -1;
    int                     status = -1;
    struct ste_cmd_tag_s   *ct = NULL;
    uint8_t                *req_apdu = NULL;
    uint8_t                 app_index = 0;
    uint8_t                 app_type = 0;

    if ( session_id >= MAX_CHANNELS ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_send: Bad Session ID(%u)",session_id);
        return -1;
    }
    if ( card.channels[session_id].channel_id >= MAX_CHANNELS ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_send: Session has no channel(%u)",session_id);
        return -1;
    }

    ct = malloc(sizeof *ct);
    if (!ct) return -1;
    req_apdu = malloc(apdu_len);
    if (!req_apdu) {
        free(ct);
        return -1;
    }

    ct->ct = (void *)client_tag;
    ct->tag = STE_CMD_APDU_NORMAL_SEND;

    memcpy( req_apdu, apdu, apdu_len );

    if ((card.channels[session_id].channel_id == 0) || (card.channels[session_id].channel_id == CHANNEL_ID_UPDATING_ISIM)) {

        if (!path) {
            catd_log_f(SIM_LOGGING_E, "ste_modem_channel_send: NULL file Path");
            goto Exit;
        }
        // Send selected APDU's can be sent to the telecom application
        // using the application APDU interface.
        uicc_appl_cmd_req_t req = { 0, 0, 0, { { { 0, 0, 0, 0 } } } };

        req.service_type = MAL_UICC_APPL_APDU_SEND;

        if(card.channels[session_id].channel_id == CHANNEL_ID_UPDATING_ISIM){

        //The understanding in case of UICC_APPL_CMD_REQ(UICC_APPL_APDU_SEND) is
        //that the modem takes care of setting the correct class byte based on the appl id sent.
        //TODO: Test this ISIM path to confirm this understanding.

            status = lookup_application_index(card.channels[session_id].aid_len,
                                                card.channels[session_id].aid,
                                                &app_index,
                                                &app_type);
            if ((status == -1)||(app_index > MAX_APPS)) {
                catd_log_f(SIM_LOGGING_E, "%s: Invalid application ID (AID) for the channel", __func__);
                goto Exit;
            }
            req.appl_id = apps[app_index].app_id;
        } else {
            req.appl_id = apps[card.i].app_id;
        }

        req.session_id = MAL_UICC_SESSION_ID_NOT_USED;

        // Use the supplied APDU for the request.
        req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.cmd_force = 0;
        req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.apdu_length = apdu_len;
        req.sub_block_t.appl_apdu_send_req.uicc_sb_apdu.apdu = apdu;

        req.sub_block_t.appl_apdu_send_req.uicc_sb_client.client_id = 0;

        // Use the previously stored path for this request.
        req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.ef = (uint16_t )(path[path_length-2]<<8 | path[path_length-1]);
        req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.sfi = 0;
        path_length = path_length - 2;
        req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path_length = path_length;

        req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path = malloc(path_length * sizeof(uint8_t));

        if (!req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path) {
            catd_log_f(SIM_LOGGING_E, "%s: Could not allocate memory for channel_send!", __func__);
            i = -1;
        } else {
            catd_log_f(SIM_LOGGING_D, "%s: path = %x length = %d", __func__, path, path_length);
            memmove(req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path, path, path_length);

            i = mal_uicc_request_wrapper(MAL_UICC_APPL_CMD_REQ, &req, ct);

            free(req.sub_block_t.appl_apdu_send_req.uicc_sb_appl_path.path);
        }
    }
    else
    {
        // Send any APDU to non-telecom applications using the
        // limited APDU interface.
        uicc_apdu_req_t         req = { 0, { { { 0, 0, 0 } } } };
        req_apdu[0] = add_channel_id_to_class( apdu[0], card.channels[session_id].channel_id );

        req.service_type = MAL_UICC_APDU_SEND;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = req_apdu;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = apdu_len;

        i = mal_uicc_request_wrapper(MAL_UICC_APDU_REQ, &req, ct);
    }

Exit:
    if ( i != 0 ) free(ct);

    free( req_apdu );

    return i;
}

int ste_modem_channel_send(ste_modem_t * m,
                           uintptr_t client_tag,
                           uint16_t session_id,
                           size_t apdu_len,
                           uint8_t *apdu,
                           uint8_t *path,
                           uint8_t path_length)
{
    int i;

    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_send: Enter");

    pthread_mutex_lock(&card.mtx);

    i = ste_mtbl_i_sim_modem_channel_send_th(m,
                                             (void *)client_tag,
                                             session_id,
                                             (uint16_t)apdu_len,
                                             apdu,
                                             path,
                                             path_length);

    pthread_mutex_unlock(&card.mtx);
    if (i != 0) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_channel_send: Exit failed");
      return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_send: Exit success");
    return 0;
}

static int lookup_application_index(uint16_t aid_len,
                                    uint8_t *aid,
                                    uint8_t *app_index,
                                    uint8_t *app_type)
{
    int i;
    uint8_t aid_nfc_Rid[5] = {0xA0,0x00,0x00,0x00,0x18}; /* ETSI 102.220 */
    uint16_t app_code = 0;

    if (!memcmp(aid, aid_nfc_Rid, 5)) {
        /* get sixth and seventh byte of AID */
        app_code = ((uint16_t)aid[5] << 8) + (uint16_t)aid[6];
        catd_log_f(SIM_LOGGING_D, "lookup_application_index: NFC AID with app_code = %d",app_code);
        *app_index = 0xFF;
        return 0;
    }

    for (i = 0; i < MAX_APPS + 1; i++) {
        if (apps[i].aid_len == aid_len &&
            !memcmp(apps[i].aid, aid, aid_len)) {
            *app_index = i;
            *app_type  = apps[i].app_type;
            return 0;
        }
    }

    return -1; /* Application ID not found */
}

/*
 * Some information on channel open.
 * The limited APDU interface only allows MANAGE CHANNEL to be sent on channel 0.
 * Following the opening of the channel, the next step is to activate the application.
 * This is done in the response callback.
 */

static int ste_mtbl_i_sim_modem_channel_open_th(ste_modem_t *UNUSED(m),
                                                void        *client_tag,
                                                uint16_t     aid_len,
                                                uint8_t     *aid)
{
    uicc_apdu_req_t                      req = { 0, { { { 0, 0, 0 } } } };
    int                                  i = -1;
    int                                  status = -1;
    struct ste_cmd_tag_s                *ct = NULL;
    struct ste_channel_app_tag_s        *ct2 = NULL;
    struct ste_channel_isim_app_tag_s   *ct2_isim = NULL;
    uint8_t                              cmd[sizeof(manage_channel_open)];
    uint8_t                              app_index = 0;
    uint8_t                              app_type = 0;
    uint16_t                             session_id;
    mal_uicc_appln_req_t                 req_param;

    if ( aid_len > MAX_AID_SIZE ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_open: AID too long(%u)",aid_len);
        return -1;
    }
    if ( aid_len == 0 || aid == NULL ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_open: AID Invalid (len = %u)",aid_len);
        return -1;
    }
    for ( session_id = 0 ; session_id < MAX_CHANNELS ; session_id++ ) {
        if ( card.channels[session_id].channel_id == CHANNEL_ID_FREE ) break;
    }
    if ( session_id == MAX_CHANNELS ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_open: No Free channels");
        return -1;
    }

    catd_log_f(SIM_LOGGING_I, "%s: sim_modem_channel_open_th, card.clf_support = %d", __func__, card.clf_support);

    status = lookup_application_index(aid_len,
                                  aid,
                                  &app_index,
                                  &app_type);

    /*If MAL_UICC_CLF_IF_SUPPORTED, we go ahead even if the AID does not point to a valid index*/
    if (card.clf_support != MAL_UICC_CLF_IF_SUPPORTED) {
        if (status == -1) {
            catd_log_f(SIM_LOGGING_E, "%s: Invalid application ID (AID) for open channel", __func__);
            return -1;
        }
        catd_log_f(SIM_LOGGING_I, "%s: Found app_index in the aid list, app_index = %d, app_type = &d", __func__, app_index, app_type);
    }

    if (MAL_UICC_APPL_TYPE_UICC_ISIM == app_type) {

        ct2_isim = malloc(sizeof *ct2_isim);
        if (!ct2_isim) return -1;
        ct = malloc(sizeof *ct);
        if (!ct) {
            free(ct2_isim);
            return -1;
        }

        ct2_isim->request_ct = (void *)client_tag;
        ct2_isim->session_id = session_id;
        ct2_isim->app_index  = app_index;

        card.channels[session_id].channel_id = CHANNEL_ID_UPDATING_ISIM;
        card.channels[session_id].aid_len    = aid_len;
        memcpy( card.channels[session_id].aid, aid, aid_len );

        ct->ct = ct2_isim;
        ct->tag = STE_CMD_ISIM_HOST_ACTIVATE;

        req_param.service_type = MAL_UICC_APPL_HOST_ACTIVATE;
        req_param.activate_last_appln = 0;
        req_param.sub_block.appln_host_activate.sb_appln.application_type = apps[app_index].app_type;
        req_param.sub_block.appln_host_activate.sb_appln.appl_id = apps[app_index].app_id;
        req_param.sub_block.appln_host_activate.sb_appl_info.strat_up_type = 0;
        req_param.sub_block.appln_host_activate.uicc_sb_aid.str_len = 0;
        req_param.sub_block.appln_host_activate.uicc_sb_aid.str = NULL;

        i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &req_param, ct);

    } else {
        ct2 = malloc(sizeof *ct2);
        if (!ct2) return -1;
        ct = malloc(sizeof *ct);
        if (!ct) {
        free(ct2);
        return -1;
        }

        ct2->request_ct = (void *)client_tag;
        ct2->session_id = session_id;

        card.channels[session_id].channel_id = CHANNEL_ID_UPDATING;
        card.channels[session_id].aid_len    = aid_len;
        memcpy( card.channels[session_id].aid, aid, aid_len );

        ct->ct = ct2;

        ct->tag = STE_CMD_APDU_NORMAL_OPEN;

        memcpy( cmd, manage_channel_open, sizeof(cmd) );

        req.service_type = MAL_UICC_APDU_SEND;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = cmd;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = sizeof(cmd);

        i = mal_uicc_request_wrapper(MAL_UICC_APDU_REQ, &req, ct);
    }

    if ( i != 0 ) {
        free(ct);
        if (!ct2) free(ct2);
        if (!ct2_isim) free(ct2_isim);
        // Reset the local channel data
        card.channels[session_id].channel_id = CHANNEL_ID_FREE;
        card.channels[session_id].aid_len    = 0;
        memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );
    }

    return i;
}

int ste_modem_channel_open(ste_modem_t * m, uintptr_t client_tag, size_t aid_len, uint8_t *aid)
{
    int i = -1;

    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_open: Enter");

    pthread_mutex_lock(&card.mtx);
    i = ste_mtbl_i_sim_modem_channel_open_th(m,
                                             (void *)client_tag,
                                             (uint16_t)aid_len,
                                             aid);
    pthread_mutex_unlock(&card.mtx);
    if (i != 0) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_channel_open: Exit failed");
      return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_open: Exit success");
    return 0;
}

static int ste_mtbl_i_sim_modem_channel_close_th(ste_modem_t *UNUSED(m),
                                                 void        *client_tag,
                                                 uint16_t     session_id)
{
    uicc_apdu_req_t         req = { 0, { { { 0, 0, 0 } } } };
    int                     i = -1;
    int                     status = -1;
    struct ste_cmd_tag_s   *ct = NULL;
    struct ste_channel_isim_app_tag_s   *ct2_isim = NULL;
    uint8_t                 cmd[sizeof(manage_channel_close)];
    uint8_t                 app_index = 0;
    uint8_t                 app_type = 0;
    mal_uicc_appln_req_t    request;
    channel_info_t         *ci = NULL;

    if ( session_id >= MAX_CHANNELS ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_close: Bad Session ID(%u)",session_id);
        return -1;
    }
    if ( card.channels[session_id].channel_id >= MAX_CHANNELS ) {
        catd_log_f(SIM_LOGGING_E, "ste_modem_channel_close: Session has no channel(%u)",session_id);
        return -1;
    }

    ci = &card.channels[session_id];
    status = lookup_application_index(ci->aid_len,ci->aid,&app_index,&app_type);

    if (MAL_UICC_APPL_TYPE_UICC_ISIM == app_type) {

        ct2_isim = malloc(sizeof *ct2_isim);
        if (!ct2_isim) return -1;
        ct = malloc(sizeof *ct);
        if (!ct) {
            free(ct2_isim);
            return -1;
        }

        ct2_isim->request_ct = (void *)client_tag;
        ct2_isim->session_id = session_id;
        ct2_isim->app_index  = app_index;

        ct->ct = ct2_isim;

        ct->tag = STE_CMD_ISIM_DEACTIVATE;

        request.service_type = MAL_UICC_APPL_HOST_DEACTIVATE;
        request.sub_block.uicc_appl_host_deactivate.sb_appln.application_type = apps[app_index].app_type;
        request.sub_block.uicc_appl_host_deactivate.sb_appln.appl_id = apps[app_index].app_id;
        request.sub_block.uicc_appl_host_deactivate.uicc_sb_client.client_id = apps[app_index].client_id;

        i = mal_uicc_request_wrapper(MAL_UICC_APPLICATION_REQ, &request, client_tag);

        if ( i != 0 ) {
            free(ct);
            free(ct2_isim);
        }
    } else {

        ct = malloc(sizeof *ct);
        if (!ct) return -1;

        ct->ct = (void *)client_tag;

        ct->tag = STE_CMD_APDU_NORMAL_CLOSE;

        memcpy( cmd, manage_channel_close, sizeof(cmd) );
        cmd[3] = card.channels[session_id].channel_id;       // set the channel to be closed in P2 of the APDU

        req.service_type = MAL_UICC_APDU_SEND;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.cmd_force = MAL_UICC_APDU_CMD_FORCE_NOT_USED;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu = cmd;
        req.sub_block_t.uicc_apdu_send_req.uicc_sb_apdu.apdu_length = sizeof(cmd);

        i = mal_uicc_request_wrapper(MAL_UICC_APDU_REQ, &req, ct);
        // Reset the local channel data
        card.channels[session_id].channel_id = CHANNEL_ID_FREE;
        card.channels[session_id].aid_len    = 0;
        memset( card.channels[session_id].aid, 0, sizeof(card.channels[session_id].aid) );

        if ( i != 0 ) free(ct);
    }

    return i;
}

int ste_modem_channel_close(ste_modem_t * m, uintptr_t client_tag, uint16_t session_id)
{
    int i = -1;

    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_close: Enter");

    pthread_mutex_lock(&card.mtx);
    i = ste_mtbl_i_sim_modem_channel_close_th(m,
                                             (void *)client_tag,
                                             session_id);
    pthread_mutex_unlock(&card.mtx);
    if (i != 0) {
      catd_log_f(SIM_LOGGING_E, "ste_modem_channel_close: Exit failed");
      return -1;
    }
    catd_log_f(SIM_LOGGING_D, "ste_modem_channel_close: Exit success");

    return 0;
}

void ste_modem_set_debug_level(ste_modem_t *UNUSED(m), uintptr_t UNUSED(client_tag), int level)
{
    sim_trace_debug_level = (sim_logging_t)level;
}

void ste_modem_reset_backend_state(ste_modem_t * UNUSED(m), uintptr_t UNUSED(client_tag))
{
    init_apps_table();
    reset_card_state();
    ste_modem_reset_pin_state(NULL, 0);
}

int ste_catd_modem_adapt_terminal_support_table_update(ste_modem_t *m,
                                             uintptr_t client_tag,
                                             int offset,
                                             const uint8_t* data,
                                             size_t data_size) {
    const uint8_t *path;
    uint8_t path_len;
    int i = 0;
    int tag = STE_CMD_ADAPT_TAG;
    struct ste_cmd_tag_s *cmd_tag;

    if (!data) {
        return -1;
    }

    switch (apps[card.i].app_type) {
    case MAL_UICC_APPL_TYPE_ICC_SIM:
        path = adapt_ef_terminal_sim_path;
        path_len = ADAPT_EF_TERMINAL_SIM_PATH_LEN;
        catd_log_f(SIM_LOGGING_V, "ste_catd_modem_adapt_terminal_support_table_update: App type is SIM");
        break;
    case MAL_UICC_APPL_TYPE_UICC_USIM:
        path = adapt_ef_terminal_usim_path;
        path_len = ADAPT_EF_TERMINAL_USIM_PATH_LEN;
        catd_log_f(SIM_LOGGING_V, "ste_catd_modem_adapt_terminal_support_table_update: App type is USIM");
        break;
    default:
        // Handle this error case already in the caller's response function
        //catd_sig_adapt_terminal_support_table_update_status(client_tag, 1);

        catd_log_f(SIM_LOGGING_E, "ste_catd_modem_adapt_terminal_support_table_update: App type is unknown");
        return 0;
    }
    cmd_tag = malloc(sizeof(*cmd_tag));
    if (!cmd_tag) {
        goto error;
    }
    cmd_tag->tag = tag;
    cmd_tag->ct = (void *)client_tag;
    MAL_TRY(i = ste_mtbl_i_sim_file_update_binary_th(m, cmd_tag,
                apps[card.i].app_id,
                adapt_ef_terminal_file_id,
                (uint8_t *)path,
                path_len,
                (uint16_t)offset,
                (uint16_t)data_size,
                (uint8_t *)data));
    if (i == 0) {
        catd_log_f(SIM_LOGGING_D, "ste_catd_modem_adapt_terminal_support_table_update: Exit success");
        return 0;
    }
error:
    if (cmd_tag)
        free(cmd_tag);
    catd_log_f(SIM_LOGGING_E, "ste_catd_modem_adapt_terminal_support_table_update: Exit failed");
    return -1;
}

void ste_modem_uicc_status_info(ste_modem_t * UNUSED(m), ste_uicc_status_t *status)
{
    pthread_mutex_lock(&card.mtx);

    if (card.status == MAL_UICC_STATUS_CARD_READY) {
        if (pin_lock.state == PIN_STATE_PINLOCKED) {
            if (pin_lock.pin_id == apps[card.i].pin_id) {
                *status = STE_UICC_STATUS_PIN_NEEDED;
            } else if (pin_lock.pin_id == apps[card.i].pin2_id) {
                *status = STE_UICC_STATUS_PIN2_NEEDED;
            }
        } else if (pin_lock.state == PIN_STATE_PUKLOCKED) {
            if (pin_lock.pin_id == apps[card.i].pin_id) {
                *status = STE_UICC_STATUS_PUK_NEEDED;
            } else if (pin_lock.pin_id == apps[card.i].pin2_id) {
                *status = STE_UICC_STATUS_PUK2_NEEDED;
            }
        } else {
            *status =  STE_UICC_STATUS_READY;
        }
    } else if (pin_lock.state == PIN_STATE_PERMLOCKED) {
        *status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
    } else if (card.status == MAL_UICC_STATUS_CARD_REJECTED) {
        switch (card.reject_cause) {
            case MAL_UICC_CARD_REJECT_CAUSE_SIMLOCK:
                *status = STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK;
            break;
            case MAL_UICC_CARD_REJECT_CAUSE_SW_6F00:
                *status = STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00;
            break;
            case MAL_UICC_CARD_REJECT_CAUSE_NOT_AVAILABLE:
            default:
                *status = STE_UICC_STATUS_REJECTED_CARD_INVALID;
            break;
        }
    } else if (card.status == MAL_UICC_CARD_REMOVED || card.status == MAL_UICC_STATUS_CARD_NOT_PRESENT) {
        *status = STE_UICC_STATUS_NO_CARD;
    } else if (card.status == MAL_UICC_STATUS_CARD_DISCONNECTED) {
        *status = STE_UICC_STATUS_DISCONNECTED_CARD;
    } else {
        *status = STE_UICC_STATUS_UNKNOWN;
    }

    pthread_mutex_unlock(&card.mtx);
}
